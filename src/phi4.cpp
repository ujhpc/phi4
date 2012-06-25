#include <iostream>
using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>
namespace po = boost::program_options;

#ifdef _OPENMP
  #include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"
#include "measurments.h"

const int  meas   =     25;
const int write_out   = 1;


int
main(int argc,char *argv[]) {

  int dim[DIM];

  int n_term;
  int n_prod;
  int seed;
      
  int sweep;

  int ix,iy;
  int n_x;
  int n_y;
#if DIM >= 3
  int n_z;
#endif
  
  struct parameters<Float> pars;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("threads", po::value<int>(), "set number of threads")
    ("n-x", po::value<int>(&n_x)->default_value(128), "set x size")
    ("n-y", po::value<int>(&n_y)->default_value(128), "set y size")
#if DIM >= 3
    ("n-z", po::value<int>(&n_z)->default_value(128), "set y size")
#endif
    ("term,t", po::value<int>(&n_term)->default_value(0), "n-term")
    ("sweep,n", po::value<int>(&n_prod)->default_value(0), "n-sweep")
    ("seed,s", po::value<int>(&seed)->default_value(7675643), "seed")
    ("m", po::value<FLOAT>(&pars.m_2)->default_value(0.25), "set m^2 size")
    ("g", po::value<FLOAT>(&pars.g)->default_value(0.0), "set g size")
    ("i-Lambda,L", po::value<FLOAT>(&pars.i_Lambda)->default_value(2.0), "set inverse Lambda size")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

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
  if(vm.count("threads")) {
    omp_set_num_threads(vm["threads"].as<int>());
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


