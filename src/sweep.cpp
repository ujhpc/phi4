#include<cstdlib>
#include<cmath>
#ifdef _OPENMP
#include<omp.h>
#endif


#include"sweep.h"

#include"indexer.h"
#include"field.h"

#include"typedefs.h"
#include"random.h"


const double l2_e=1.442695040888963;


template<typename F>
class Updater {
public:  
  typedef typename F::indexer_t indexer_t;
  Updater( F &f,const parameters<double> &pars):
    field(f),
    pars_(pars),
    quadratic_coef_1(indexer_t::D+pars.m_2/2.0),
    quadratic_coef_2(indexer_t::D*(1.0+2.0*indexer_t::D)*pars.i_Lambda),
    quadratic_coef(quadratic_coef_1+quadratic_coef_2), gr(pars.g/24.0) 
  {};
  

  int operator()(int i) {
#ifdef _OPENMP
    const int tid=omp_get_thread_num();
#else
    const int tid=0;
#endif

    double  old_action=0.0;
    double  new_action=0.0;
    double  delta_action;
    double  phi_tmp;
    double  phi2_tmp;


    double small_corona=0.0;

    for(int mu=0;mu<indexer_t::D;mu++) {
      small_corona+=field[indexer_t::up(i,mu)]+field[indexer_t::dn(i,mu)];
    }

      

    double big_corona_01=0.0;
    double big_corona_02=0.0;
    double big_corona_11=0.0;

	    
    for(int mu=0;mu<indexer_t::D;mu++) {
      big_corona_02 +=field[indexer_t::up(indexer_t::up(i,mu),mu)];
      big_corona_02 +=field[indexer_t::dn(indexer_t::dn(i,mu),mu)];
      
      big_corona_01 += field[indexer_t::up(i,mu)];
      big_corona_01 += field[indexer_t::dn(i,mu)];
      
      for(int nu=0;nu<mu;nu++) {
	big_corona_11 += field[indexer_t::up(indexer_t::up(i,mu),nu)];
	big_corona_11 += field[indexer_t::dn(indexer_t::dn(i,mu),nu)];
	big_corona_11 += field[indexer_t::dn(indexer_t::up(i,mu),nu)];
	big_corona_11 += field[indexer_t::up(indexer_t::dn(i,mu),nu)];
      }
    }

      
    double big_corona=-pars_.i_Lambda*(big_corona_02
				      -4.0*indexer_t::D*big_corona_01
				      +2.0*big_corona_11);

    double corona=small_corona+big_corona;

    int accepted=0;
#pragma unroll
    for(int h=0;h<N_HIT;h++) {
      phi_tmp=field[i];
	      
      old_action=corona*phi_tmp;


      phi2_tmp=phi_tmp*phi_tmp;


      old_action -= (quadratic_coef+gr*phi2_tmp)*phi2_tmp;
      
	      
      phi_tmp += EPSILON*(RAND(tid)  -0.5);

      new_action=corona*phi_tmp;

      phi2_tmp=phi_tmp*phi_tmp;
     
      new_action -= phi2_tmp*(quadratic_coef+gr*phi2_tmp);

      delta_action=new_action-old_action;
	      
      if(delta_action< 0.0)
	if(exp(delta_action) < RAND(tid) )
	  goto next;
      
      
      field[i]=phi_tmp;
      accepted++;

    next:;
    }
    return accepted;
  }

  F &field;
  const parameters<double> pars_;

  const double quadratic_coef_1;
  const double quadratic_coef_2;
  const double quadratic_coef;
  
  double gr;

};

template<typename F, typename P  > 
long int 
make_sweep(F &field, const parameters<double> &pars, const P &partition ) {

  long int accepted=0;
  
  typedef typename F::indexer_t indexer_t;
  const double quadratic_coef_1 = indexer_t::D+pars.m_2/2.0;
  const double quadratic_coef_2 = indexer_t::D*(1.0+2.0*indexer_t::D)*pars.i_Lambda;  
  const double quadratic_coef   = quadratic_coef_1+quadratic_coef_2;
  
  double register gr=pars.g/24.0;

  for(int p=0;p<partition.n_partitions();++p) {

    /* this loop can parallelised */
    
    Updater<F> update(field,pars);

    #pragma omp parallel for
    for(int s=0;s<partition.partition_size();++s) {
      int i=partition.partition(p,s);
#if 1
      accepted+=update(i);
#else
      #ifdef _OPENMP
      const int tid=omp_get_thread_num();
      #else
      const int tid=0;
      #endif

      //std::cerr<<"thread "<<tid<<" iter "<<s<<" site "<<i<<std::endl;
      double  old_action=0.0;
      double  new_action=0.0;
      double  delta_action;
      double  phi_tmp;
      double  phi2_tmp;


      double small_corona=0.0;

      for(int mu=0;mu<indexer_t::D;mu++) {
	small_corona+=field[indexer_t::up(i,mu)]+field[indexer_t::dn(i,mu)];
      }

      

      double big_corona_01=0.0;
      double big_corona_02=0.0;
      double big_corona_11=0.0;

	    
      for(int mu=0;mu<indexer_t::D;mu++) {
	big_corona_02 +=field[indexer_t::up(indexer_t::up(i,mu),mu)];
	big_corona_02 +=field[indexer_t::dn(indexer_t::dn(i,mu),mu)];
      
	big_corona_01 += field[indexer_t::up(i,mu)];
	big_corona_01 += field[indexer_t::dn(i,mu)];
      
	for(int nu=0;nu<mu;nu++) {
	  big_corona_11 += field[indexer_t::up(indexer_t::up(i,mu),nu)];
	  big_corona_11 += field[indexer_t::dn(indexer_t::dn(i,mu),nu)];
	  big_corona_11 += field[indexer_t::dn(indexer_t::up(i,mu),nu)];
	  big_corona_11 += field[indexer_t::up(indexer_t::dn(i,mu),nu)];
	}
      }

      
      double big_corona=-pars.i_Lambda*(big_corona_02
					-4.0*indexer_t::D*big_corona_01
					+2.0*big_corona_11);

      double corona=small_corona+big_corona;

      //      register double phi=field[i]; 
#pragma unroll
      for(int h=0;h<N_HIT;h++) {
	phi_tmp=field[i];
	      
	old_action=corona*phi_tmp;


	phi2_tmp=phi_tmp*phi_tmp;


	old_action -= (quadratic_coef+gr*phi2_tmp)*phi2_tmp;
      
	      
	phi_tmp += EPSILON*(RAND(tid)  -0.5);

	new_action=corona*phi_tmp;

	phi2_tmp=phi_tmp*phi_tmp;
     
	new_action -= phi2_tmp*(quadratic_coef+gr*phi2_tmp);

	delta_action=new_action-old_action;
	      
	if(delta_action< 0.0)
	  if(exp(delta_action) < RAND(tid) )
	    goto next;
      
      
	field[i]=phi_tmp;
	accepted++;

      next:;
      }
      //      field[i]=phi;
#endif
    }

  }
  return accepted;
}

template
long int make_sweep< Field<double &, Ind, SFA>, Partition >( Field<double &, Ind, SFA>&,const parameters<double> &, 
const  Partition  &partition 
);
