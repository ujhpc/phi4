#include <cstdlib>
#include <cmath>
#ifdef _OPENMP
  #include <omp.h>
#endif

#include "typedefs.h"
#include "sweep.h"

const Float l2_e=1.442695040888963;


template<typename F,typename P>
class Updater {
public:  
  typedef typename F::indexer_t indexer_t;
  Updater( F &f,const P& partition, const parameters<Float> &pars):
    field(f),
    partition_(partition),
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

    Float  old_action=(Float)0.0;
    Float  new_action=(Float)0.0;
    Float  delta_action;
    Float  phi_tmp;
    Float  phi2_tmp;


    Float small_corona=(Float)0.0;

    for(int mu=0;mu<indexer_t::D;mu++) {
      small_corona+=field[indexer_t::up(i,mu)]+field[indexer_t::dn(i,mu)];
    }

      

    Float big_corona_01=(Float)0.0;
    Float big_corona_02=(Float)0.0;
    Float big_corona_11=(Float)0.0;

	    
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

      
    Float big_corona=-pars_.i_Lambda*(big_corona_02
				      -(Float)4.0*indexer_t::D*big_corona_01
				      +(Float)2.0*big_corona_11);

    Float corona=small_corona+big_corona;

    int accepted=0;
#pragma unroll
    for(int h=0;h<N_HIT;h++) {
      phi_tmp=field[i];
	      
      old_action=corona*phi_tmp;


      phi2_tmp=phi_tmp*phi_tmp;


      old_action -= (quadratic_coef+gr*phi2_tmp)*phi2_tmp;
      
	      
      phi_tmp += (Float)EPSILON*(RAND(tid)  -(Float)0.5);

      new_action=corona*phi_tmp;

      phi2_tmp=phi_tmp*phi_tmp;
     
      new_action -= phi2_tmp*(quadratic_coef+gr*phi2_tmp);

      delta_action=new_action-old_action;
	      
      if(delta_action< (Float)0.0)
	if(std::exp(delta_action) < RAND(tid) )
	  goto next;
      
      
      field[i]=phi_tmp;
      accepted++;

    next:;
    }
    return accepted;
  }

  
  F &field;
  const P &partition_;
  const parameters<Float> pars_;

  const Float quadratic_coef_1;
  const Float quadratic_coef_2;
  const Float quadratic_coef;
  
  Float gr;

};

template<typename F, typename P  > 
long int 
make_sweep(F &field, const parameters<Float> &pars, const P &partition ) {

  long int accepted=0;

  Updater<F,Partition> update(field,partition,pars);
#pragma omp parallel default(none) shared(partition,update,accepted)
  for(int p=0;p<partition.n_partitions();++p) {

    /* this loop can parallelised */
#pragma omp for reduction(+:accepted)
    for(int s=0;s<partition.partition_size();++s) {
      int i=partition.partition(p,s);

      accepted+=update(i);

    }

  }
  return accepted;
}

template
long int make_sweep< Field<Float &, Ind, SFA>, Partition >( Field<Float &, Ind, SFA>&,const parameters<Float> &, 
const  Partition  &partition 
);
