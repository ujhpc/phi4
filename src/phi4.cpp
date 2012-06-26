#include <iostream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <popt.h>

template<typename T> int poptType();
template<> int poptType<float>()  { return POPT_ARG_FLOAT; }
template<> int poptType<double>() { return POPT_ARG_DOUBLE; }

#ifdef _OPENMP
  #include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"
#include "measurments.h"

const int  meas   =     25;
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
      "set x size" },
    { "n-y", 0, POPT_ARG_INT, &n_y, 0,
      "set y size" },
#if DIM >= 3
    { "n-z", 0, POPT_ARG_INT, &n_z, 0,
      "set z size" },
#endif
    { "term", 't', POPT_ARG_INT, &n_term, 0,
      "n-term" },
    { "sweep", 'n', POPT_ARG_INT, &n_prod, 0,
      "n-sweep" },
    { "seed", 's', POPT_ARG_INT, &seed, 0,
      "seed" },
    { NULL, 'm', poptType<Float>(), &pars.m_2, 0,
      "set m^2 size" },
    { NULL, 'g', poptType<Float>(), &pars.g, 0,
      "set g size" },
    { "i-Lambda", 'L', poptType<Float>(), &pars.i_Lambda, 0,
      "set inverse Lambda size" },
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

  dim[0]=n_x;
  dim[1]=n_y;
#if DIM >= 3
  dim[2]=n_z;
#endif

  Ind::init(dim);
  std::cerr<<"n_sites "<<Ind::n_sites()<<std::endl;
  std::vector<Float> field_array(Ind::n_sites());
  SFA sfa(field_array);
  Field<Float &, Ind, SFA> phi_field(field_array);
  

  fprintf(stderr,"%f %f %f\n",pars.m_2,pars.g,pars.i_Lambda);
  fprintf(stderr,"%d %d %d\n",n_term,n_prod,seed);
 
  
  srand48(seed);

#ifdef _OPENMP
  if(threads > 0) {
    omp_set_num_threads(threads);
  }
  int n_threads=omp_get_max_threads();
#else
  int n_threads=1;
#endif
  std::cerr<<"n threads = "<<n_threads<<std::endl;
  rand_array_t::init(n_threads,seed);
  
  for(int i=0;i<Ind::n_sites();i++) {
      phi_field[i]=2*drand48()-1.0;
    }

  /*
   * termalisation loop 
   */

  long int accepted=0;

  Partition partition;

  for(sweep=0;sweep<n_term;sweep++)    {
    accepted+=make_sweep(phi_field,pars,partition);      
  }
  fprintf(stderr,"acceptance %f\n",((Float) accepted)/(N_HIT*Ind::n_sites()*n_term));
  
  

  MagnetisationMeasurer magnetisation;
  accepted=0;
  for(sweep=0;sweep<n_prod;sweep++)    {
      
    accepted+=make_sweep(phi_field,pars,partition);
    
    int n_meas=0;
    if( (sweep+1)%meas==0 ) {

      magnetisation.measure(phi_field);
      
      if((magnetisation.n_meas())%write_out == 0)  {
	   
	fprintf(stdout,"%.12g  %.16g %.16g %.16g\n",
		magnetisation.phi2()/Ind::n_sites(),
		magnetisation.mag(),
		magnetisation.xi(),
	        magnetisation.amag()
		);
		magnetisation.reset();
      }
    
    if((sweep+1)%(100*meas) == 0)
      fflush(stdout);

    }
  }

  if(n_prod>0)
    fprintf(stderr,"acceptance %f\n",
	    ((Float) accepted)/(N_HIT*Ind::n_sites()*n_prod));
  
  Ind::clean();
}


