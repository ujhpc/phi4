
/*
 *
 * Quick and dirty phi^4 simulation (2D)
 *
 *   exp (\sum_x (\sum_i 1/2(phi(x)-phi(x+i) )^2  -m 1/2phi(x)^2 -lambda/4! phi(x)^4 )
 *
 */



#include<iostream>
using namespace std;

#include<stdlib.h>
#include<stdio.h>
#include<cmath>


#include"indexer.h"
#include"field.h"


const double EPSILON =  1.0;
const int  meas   =     25;
const int write_out   = 1;


const int N_HIT=5;

#define N_X 128
#define N_Y 128



double m;
double m_2;
double g;
double i_Lambda=1.0;

  
//double a;	  

long int accepted=0;


//typedef double *field_t;
typedef Indexer<2> Ind;


typedef ScalarFieldAccessor<double,2> SFA;

std::vector<double> field_array(N_X*N_Y);
SFA sfa(field_array);
Field<double &, Ind, SFA> phi_field(field_array);


void make_sweep(Field<double &, Ind, SFA> &field);


int
main(int argc,char *argv[]) {




  int dim[2]={N_X,N_Y};
  Ind::init(dim);

  int n_term;
  int n_prod;
  int seed;


  double phi2=0.0;
      
  int sweep;

  int ix,iy;


  m=atof(argv[1]);
  m_2=m;
  //  m=((m>0)?1:-1)*m*m/2.0;
  g=atof(argv[2])/24.0;
  i_Lambda=atoi(argv[3]);
  n_term=atoi(argv[4]);
  n_prod=atoi(argv[5]);
  seed=atoi(argv[6]);

  fprintf(stderr,"%f %f\n",m,g);
  fprintf(stderr,"%d %d %d\n",n_term,n_prod,seed);

  // a=4.0+m_2;  
  m_2/=2.0;
 
  
  srand48(seed);

  
  for(int i=0;i<N_X*N_Y;i++) {
      phi_field[i]=2*drand48()-1.0;
    }

  /*
   *
   * termalisation loop
   *
   */


  for(sweep=0;sweep<n_term;sweep++)    {
    make_sweep(phi_field);      
  }


  double amag= 0.0;
  double mag = 0.0;
  double xi  = 0.0;

  for(sweep=0;sweep<n_prod;sweep++)    {
      
    make_sweep(phi_field);
    
    int n_meas=0;
    if( (sweep+1)%meas==0 ) {

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

    
    if((sweep+1)%(100*meas) == 0)
      fflush(stdout);


    }
  }

  fprintf(stderr,"acceptance %f\n",((double) accepted)/(Ind::n_sites()*n_prod));
 Ind::clean();
}

void
make_sweep(Field<double &, Ind, SFA> &field) {

  for(int i=0;i<Ind::n_sites();i++) {
    

    double  old_action=0.0;
    double  new_action=0.0;
    double  delta_action;
    double  phi_tmp;
    double  phi2_tmp;


    double small_corona=0.0;
    const double quadratic_coef_1=Ind::D+m_2;
    const double quadratic_coef_2=Ind::D*(1.0+2.0*Ind::D)*i_Lambda;  
    const double quadratic_coef=  quadratic_coef_1+quadratic_coef_2;

    for(int mu=0;mu<Ind::D;mu++) {
      small_corona+=field[Ind::up(i,mu)]+field[Ind::dn(i,mu)];
    }

      

    double big_corona_01=0.0;
    double big_corona_02=0.0;
    double big_corona_11=0.0;

	    
    for(int mu=0;mu<Ind::D;mu++) {
      big_corona_02 +=field[Ind::up(Ind::up(i,mu),mu)];
      big_corona_02 +=field[Ind::dn(Ind::dn(i,mu),mu)];
      
      big_corona_01 += field[Ind::up(i,mu)];
      big_corona_01 += field[Ind::dn(i,mu)];
      
      for(int nu=0;nu<mu;nu++) {
	big_corona_11 += field[Ind::up(Ind::up(i,mu),nu)];
	big_corona_11 += field[Ind::dn(Ind::dn(i,mu),nu)];
	big_corona_11 += field[Ind::dn(Ind::up(i,mu),nu)];
	big_corona_11 += field[Ind::up(Ind::dn(i,mu),nu)];
      }
    }

      
    double big_corona=-i_Lambda*(big_corona_02
				 -4.0*Ind::D*big_corona_01
				 +2.0*big_corona_11);

    double corona=small_corona+big_corona;

    for(int h=0;h<N_HIT;h++) {
      phi_tmp=field[i];
	      
      old_action=corona*phi_tmp;


      phi2_tmp=phi_tmp*phi_tmp;


      old_action -= (quadratic_coef+g*phi2_tmp)*phi2_tmp;
      //      old_action -= g*phi2_tmp*phi2_tmp;
      
	      
      phi_tmp += EPSILON*(drand48()  -0.5);

      new_action=corona*phi_tmp;

      phi2_tmp=phi_tmp*phi_tmp;

     
      new_action -= phi2_tmp*(quadratic_coef+g*phi2_tmp);
      //new_action -= phi2_tmp;



      delta_action=new_action-old_action;
	      
      if(delta_action< 0.0)
       if(exp(delta_action) < drand48() )
	//if(0.5 < drand48() )
	  goto next;
      
      
      field[i]=phi_tmp;
      accepted++;

    next:;
    }
  }


   
}

