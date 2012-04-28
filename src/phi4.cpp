#include<iostream>
using namespace std;

#include<stdlib.h>
#include<stdio.h>
#include<cmath>

#include"indexer.h"
#include"field.h"
#include"sweep.h"
#include"measurments.h"

const int  meas   =     25;
const int write_out   = 1;


#define N_X 128
#define N_Y 128
 
typedef Indexer<2> Ind;

typedef ScalarFieldAccessor<double,2> SFA;

int
main(int argc,char *argv[]) {

  std::vector<double> field_array(N_X*N_Y);
  SFA sfa(field_array);
  Field<double &, Ind, SFA> phi_field(field_array);


  int dim[2]={N_X,N_Y};
  Ind::init(dim);

  int n_term;
  int n_prod;
  int seed;


  double phi2=0.0;
      
  int sweep;

  int ix,iy;

  
  struct parameters<double> pars;

  pars.m_2=atof(argv[1]);

  pars.g=atof(argv[2]);
  pars.i_Lambda=atoi(argv[3]);

  n_term=atoi(argv[4]);
  n_prod=atoi(argv[5]);
  seed=atoi(argv[6]);

  fprintf(stderr,"%f %f %f\n",pars.m_2,pars.g,pars.i_Lambda);
  fprintf(stderr,"%d %d %d\n",n_term,n_prod,seed);
 
  
  srand48(seed);

  
  for(int i=0;i<N_X*N_Y;i++) {
      phi_field[i]=2*drand48()-1.0;
    }

  /*
   * termalisation loop
   */

  long int accepted=0;

  for(sweep=0;sweep<n_term;sweep++)    {
    accepted+=make_sweep(phi_field,pars);      
  }
  fprintf(stderr,"acceptance %f\n",((double) accepted)/(N_HIT*Ind::n_sites()*n_term));
  

  double amag= 0.0;
  double mag = 0.0;
  double xi  = 0.0;


  MagnetisationMeasurer magnetisation;

  for(sweep=0;sweep<n_prod;sweep++)    {
      
    accepted+=make_sweep(phi_field,pars);
    


    int n_meas=0;
    if( (sweep+1)%meas==0 ) {
#if 1
      magnetisation.measure(phi_field);
      
      if((magnetisation.n_meas())%write_out == 0)  {
	   
	fprintf(stdout,"%.12g  %.16g %.16g %.16g\n",
		magnetisation.phi2()/(N_X*N_Y),
		magnetisation.mag(),
		magnetisation.xi(),
	        magnetisation.amag()
		);
	
		magnetisation.reset();
      }
#else

      double loc_mag=0.0;
      for(int site=0;site<Ind::n_sites();++site) { 
	loc_mag+=phi_field[site];
	phi2+= phi_field[site]*phi_field[site];
      }
      
      amag += fabs(loc_mag);
      mag  += loc_mag;
      xi   += loc_mag*loc_mag;
      ++n_meas;
    
          
      if((n_meas)%write_out == 0)  {
	   
	fprintf(stdout,"%.12g  %.16g %.16g %.16g\n",phi2/(N_X*N_Y*n_meas),
		mag/(n_meas),xi/(n_meas),
		amag/n_meas);
	phi2=0.0;
	mag=0.0;
	amag=0.0;
	xi=0.0;
	n_meas=0;
      }
#endif
    
    if((sweep+1)%(100*meas) == 0)
      fflush(stdout);


    }
  }

  if(n_prod>0)
    fprintf(stderr,"acceptance %f\n",((double) accepted)/(N_HIT*Ind::n_sites()*n_prod));
  
  Ind::clean();
}


