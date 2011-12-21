
/*
 *
 * Quick and dirty phi^4 simulation (2D)
 *
 *   exp (\sum_x (\sum_i (phi(x)-phi(x+i) )^2  -m phi(x)^2 -phi(x)^4 )
 *
 */

#include<iostream>
using namespace std;

#include<stdlib.h>
#include<stdio.h>
#include<cmath>

const double EPSILON =  1.0;
const int  meas   =     25;
const int write_out   = 1;
const int D=2;
#define N_X 128
#define N_Y 128

double m;
double m_2;
double g;
double i_Lambda=1.0;;

  
	  double a;	  



  double  phi[N_X][N_Y]={{0.0}};

  int x_up[N_X],x_dn[N_X];
  int y_up[N_Y],y_dn[N_Y];


long int accepted=0;

void make_sweep();

int
main(int argc,char *argv[]) {

  int n_term;
  int n_prod;
  int seed;


  double phi2=0.0;
  double better_phi2=0.0;
      
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

  a=4.0+m_2;  
  m_2/=2.0;
 
  
  srand48(seed);

  /*
   *
   * termalisation loop
   *
   */

  for(ix=0;ix<N_X;ix++)    {
    x_up[ix]=(ix+1)%N_X;
    x_dn[ix]=(ix+N_X-1)%N_X;
#if 0
    fprintf(stderr,"%3d %3d %3d\n",x_dn[ix],ix,x_up[ix]);
#endif
  }

  for(iy=0;iy<N_Y;iy++)    {
    y_up[iy]=(iy+1)%N_Y;
    y_dn[iy]=(iy+N_Y-1)%N_Y;
#if 0
    fprintf(stderr,"%3d %3d %3d\n",y_dn[iy],iy,y_up[iy]);
#endif
  }

  for(ix=0;ix<N_X;ix++)
    if(ix!=x_up[x_dn[ix]])
      exit(1);

  for(iy=0;iy<N_Y;iy++)
    if(iy!=y_up[y_dn[iy]])
      exit(1);


#if 1
  for(ix=0;ix<N_X;ix++) {
    for(iy=0;iy<N_Y;iy++) {
      phi[ix][iy]=2*drand48()-1.0;
    }
  }
#endif


  for(sweep=0;sweep<n_term;sweep++)    {
    make_sweep();
      
  }


  double amag=0.0;
  double mag=0.0;
  double xi=0.0;

  for(sweep=0;sweep<n_prod;sweep++)    {
      
    make_sweep();
    
    int n_meas=0;
    if( (sweep+1)%meas==0 ) {

      double loc_mag=0.0;
      for(ix=0;ix<N_X;ix++) {
	for(iy=0;iy<N_Y;iy++) {
#if 0

	  double b;

	  double xx;
	  
	  xx=phi[x_up[ix]][iy]+
	    phi[x_dn[ix]][iy]+
	    phi[ix][y_up[iy]]+
	    phi[ix][y_dn[iy]];
		

	  //	  better_phi2 +=(a + 2*b*(2 + b*xx*xx))/(2*(a + 4*b)*(a+4*b));
	  better_phi2 += (a +xx*xx)/(a*a);
#endif	

	    
	  loc_mag+=phi[ix][iy];
	  phi2+=phi[ix][iy]*phi[ix][iy];
	}

      }
      
      amag += fabs(loc_mag);
      mag  +=loc_mag;
      xi   += loc_mag*loc_mag;
      ++n_meas;
    
      
    
      if((n_meas)%write_out == 0)  {
	   
	fprintf(stdout,"%.12g %.12g %.16g %.16g %.16g\n",phi2/(N_X*N_Y*n_meas),better_phi2/(N_X*N_Y*n_meas),
		mag/(n_meas),xi/(n_meas),
		amag/n_meas);
	phi2=0.0;
	better_phi2=0.0;
	mag=0.0;
	amag=0.0;
	xi=0.0;
	n_meas=0;
      }

    
    if((sweep+1)%(100*meas) == 0)
      fflush(stdout);


    }
  }

  fprintf(stderr,"acceptance %f\n",((double) accepted)/(N_X*N_Y*n_prod));

}


  void
make_sweep() {

    int i;
    for(i=0;i<N_X*N_Y;i++) {
      int ix,iy;

      double  old_action=0.0;
      double  new_action=0.0;
      double  delta_action;
      double  phi_tmp;
      double  phi2_tmp;
      double  corona;

      ix=lrand48()%N_X;
      iy=lrand48()%N_Y;


      corona=phi[x_up[ix]][iy]+phi[x_dn[ix]][iy]+
	phi[ix][y_up[iy]]+phi[ix][y_dn[iy]];

      double big_corona= -i_Lambda*(
				    (
				     phi[x_up[x_up[ ix]] ][iy]+
				     phi[x_dn[x_dn[ ix]] ][iy]+
				     phi[ix] [y_up[ y_up[iy] ] ]+
				     phi[ix] [y_dn[ y_dn[iy] ] ]
				     )
				    -8.0*(
					  phi[x_up[ix] ][iy]+
					  phi[x_dn[ix] ][iy]+
					  phi[ix][y_up[iy] ]+
					  phi[ix][y_dn[iy] ]
					  )+
				    2.0*(
					 phi[x_up[ix] ][y_up[iy] ]+
					 phi[x_dn[ix] ][y_up[iy] ]+
					 phi[x_up[ix] ][y_dn[iy] ]+
					 phi[x_dn[ix] ][y_dn[iy] ]
					 )
				    );
	    

      phi_tmp=phi[ix][iy];
	      
      old_action=(corona+big_corona)*phi_tmp;

      phi2_tmp=phi_tmp*phi_tmp;

      old_action -= (D+m_2+10.0*i_Lambda)*phi2_tmp;
      old_action -= g*phi2_tmp*phi2_tmp;
      
	      
      phi_tmp=phi[ix][iy]+EPSILON*(drand48() + drand48() -1.0);

      new_action=(corona+big_corona)*phi_tmp;

      phi2_tmp=phi_tmp*phi_tmp;

      new_action -= (D+m_2+10.0*i_Lambda)*phi2_tmp;
      new_action -= g*phi2_tmp*phi2_tmp;


      delta_action=new_action-old_action;
	      
      if(delta_action< 0.0)
	if(exp(delta_action) < drand48())
	  goto next;
      
      
      phi[ix][iy]=phi_tmp;
      accepted++;

    next:;
    }


  }

