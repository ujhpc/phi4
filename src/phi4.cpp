#include <iostream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <popt.h>




template<typename T> int poptType();
template<> int poptType<float>()  { return POPT_ARG_FLOAT; }
template<> int poptType<double>() { return POPT_ARG_DOUBLE; }

template<typename T> struct numericType {
  static const char name[8];
};

template<> const char  numericType<float>::name[8]="float"; 
template<> const char  numericType<double>::name[8]="double"; 

#ifdef _OPENMP
  #include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"
#include "measurments.h"
#include "file.h"

const int  meas_freq   =     25;
const int write_out   = 1;


int
main(int argc,const char *argv[]) {

  int dim[DIM];

  int n_term = 0;
  int n_prod = 0;
  int seed   = 7675643;
      
  int sweep;

  int ix,iy;
  int n_x = 128;
  int n_y = 128;
#if DIM >= 3
  int n_z = 128;
#endif
#ifdef _OPENMP
  int threads = 0;
#endif
  char *tag;

  struct parameters<Float> pars = {
    .g        = (Float)0.0,
    .m_2      = (Float)0.25,
    .i_Lambda = (Float)2.0
  };

  poptContext optCon;
  int rc;
  struct poptOption options[] = {
#ifdef _OPENMP
    { "threads", 0, POPT_ARG_INT, &threads, 0,
      "set number of OpenMP threads" },
#endif
    { "n-x", 0, POPT_ARG_INT, &n_x, 0,
      "x size" },
    { "n-y", 0, POPT_ARG_INT, &n_y, 0,
      "y size" },
#if DIM >= 3
    { "n-z", 0, POPT_ARG_INT, &n_z, 0,
      "z size" },
#endif
    { "term", 't', POPT_ARG_INT, &n_term, 0,
      "number of termalisation sweeps" },
    { "sweep", 'n', POPT_ARG_INT, &n_prod, 0,
      "number of measurments sweeps" },
    { "seed", 's', POPT_ARG_INT, &seed, 0,
      "rng seed" },
    { "mass-squared", 'm', poptType<Float>(), &pars.m_2, 0,
      "m^2 value" },
    { NULL, 'g', poptType<Float>(), &pars.g, 0,
      "g value" },
    { "i-Lambda", 'L', poptType<Float>(), &pars.i_Lambda, 0,
      "inverse Lambda value" },
    { "tag", 0, POPT_ARG_STRING, &tag, 0, "tag"},
    POPT_AUTOHELP
    POPT_TABLEEND
  };

  
  optCon = poptGetContext(NULL, argc, argv, options, 0);

  while((rc = poptGetNextOpt(optCon)) > 0);
  if(rc < -1) {
    fprintf(stderr, "%s: %s\n",
      poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
      poptStrerror(rc));
    exit(-1);
  }

  print_parameters(std::cerr,options);
  std::cerr<<"# n-components "<<Field::n_components<<"\n";
  std::cerr<<"# Float "<<numericType<Float>::name<<"\n";
 
  dim[0]=n_x;
  dim[1]=n_y;
#if DIM >= 3
  dim[2]=n_z;
#endif


  
  std::string mag_file_name("mag.");
  mag_file_name+=std::string(tag);

  Ind::init(dim);

  Field  phi_field(Ind::n_sites());
   
  srand48(seed);

#ifdef _OPENMP
  if(threads > 0) {
    omp_set_num_threads(threads);
  }
  int n_threads=omp_get_max_threads();

  std::cerr<<"# OpenMP Yes  "<<n_threads<<" threads\n";


#else
  int n_threads=1;
  std::cerr<<"# OpenMP No\n";
#endif
  
  rand_array_t::init(n_threads,seed);
  
  for(int i=0;i<Ind::n_sites();i++) {
    phi_field.set(i, (Float)(2.0*drand48()-1.0) );
  }

  /*
   * termalisation loop 
   */


  

  long int accepted=0;

  Partition partition;

  for(sweep=0;sweep<n_term;sweep++)    {
    accepted+=make_sweep(phi_field,pars,partition);      
  }

  if(n_term>0)
    fprintf(stderr,"acceptance %f\n",((Float) accepted)/(N_HIT*Ind::n_sites()*n_term));
  
  

  MagnetisationMeasurer<Field::n_components> magnetisation;
  accepted=0;
  
  /*
   * main  simulation loop
   */

  FILE *fmag;
  if(
     NULL==(fmag=fopen(mag_file_name.c_str(),"w")) 
     ) {
    std::cerr<<"cannot open file `"<<mag_file_name<<"' for  writing"<<std::endl;
    exit(2);
  }
  int n_meas=0;
  for(sweep=0;sweep<n_prod;sweep++)    {
      
    accepted+=make_sweep(phi_field,pars,partition);
    
    
    if( (sweep+1)%meas_freq==0 ) {

      magnetisation.measure(phi_field);
      
      if((magnetisation.n_meas())%write_out == 0)  {
	   
	fprintf(fmag,"%.12g  %.16g %.16g %.16g\n",
		magnetisation.phi2()/Ind::n_sites(),
		magnetisation.mag(),
		magnetisation.xi(),
	        magnetisation.amag()
		);
		magnetisation.reset();
      }
      n_meas++;      
    }

    if(n_meas>0 && n_meas%100==0 )
      fflush(fmag);
    
  }

  
  if(n_prod>0)
    fprintf(stderr,"acceptance %f\n",
	    ((Float) accepted)/(N_HIT*Ind::n_sites()*n_prod));
  
  
  Ind::clean();
}


