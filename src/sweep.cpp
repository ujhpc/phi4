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
  {
  
    set_epsilon(1.0);
    
  };
  
  void set_epsilon(Float epsilon) {
    epsilon_=epsilon/std::sqrt(F::n_components);
    
  }

  int operator()(int i) {
#ifdef _OPENMP
    const int tid=omp_get_thread_num();
#else
    const int tid=0;
#endif

    Float  old_action=(Float)0.0;
    Float  new_action=(Float)0.0;
    Float  delta_action;


    Float phi2_tmp=(Float)0.0;
    Float corona[F::n_components];

    for(int k=0;k<F::n_components;++k) {

      Float small_corona=(Float)0.0;
      Float big_corona=(Float)0.0;
    
      
      for(int mu=0;mu<indexer_t::D;mu++) {
	small_corona+=field.get(indexer_t::up(i,mu),k)
	  +field.get(indexer_t::dn(i,mu),k);
      }


      Float big_corona_01=(Float)0.0;
      Float big_corona_02=(Float)0.0;
      Float big_corona_11=(Float)0.0;

   	    
      for(int mu=0;mu<indexer_t::D;mu++) {
	big_corona_02 +=field.get(indexer_t::up(indexer_t::up(i,mu),mu),k);
	big_corona_02 +=field.get(indexer_t::dn(indexer_t::dn(i,mu),mu),k);
      
	big_corona_01 += field.get(indexer_t::up(i,mu),k);
	big_corona_01 += field.get(indexer_t::dn(i,mu),k);
      
	for(int nu=0;nu<mu;nu++) {
	  big_corona_11 += field.get(indexer_t::up(indexer_t::up(i,mu),nu),k);
	  big_corona_11 += field.get(indexer_t::dn(indexer_t::dn(i,mu),nu),k);
	  big_corona_11 += field.get(indexer_t::dn(indexer_t::up(i,mu),nu),k);
	  big_corona_11 += field.get(indexer_t::up(indexer_t::dn(i,mu),nu),k);
	}
      }

      
      big_corona= -pars_.i_Lambda*(big_corona_02
		 		  -(Float)4.0*indexer_t::D*big_corona_01
		 		  +(Float)2.0*big_corona_11);
      corona[k]=small_corona+big_corona;

      Float phi_tmp=field.get(i,k);
      old_action+=corona[k]*phi_tmp;
				      
      phi2_tmp+= phi_tmp*phi_tmp;
    }

    
    old_action -= (quadratic_coef+gr*phi2_tmp)*phi2_tmp;
    
    int accepted=0;

#pragma unroll
    for(int h=0;h<N_HIT;h++) {
      new_action=(Float)0.0;
      Float phi_tmp[F::n_components];
      phi2_tmp=(Float)0.0;

      for(int k=0;k<F::n_components;++k) {
	phi_tmp[k]=field.get(i,k);
	
	phi_tmp[k] += epsilon_*(RAND(tid)  -(Float)0.5);

	new_action+=corona[k]*phi_tmp[k];

	phi2_tmp+=phi_tmp[k]*phi_tmp[k];
     

      }
      new_action -= phi2_tmp*(quadratic_coef+gr*phi2_tmp);

      delta_action=new_action-old_action;
	      
      if(delta_action< (Float)0.0)
	if(std::exp(delta_action) < RAND(tid) )
	  goto next;
      
      
      for(int k=0;k<F::n_components;++k) {
	field.set(i,k,phi_tmp[k]);
      }
       
      old_action=new_action;
      accepted++;
          
    next:;
    } //MULTI_HIT LOOP


    return accepted;
  }

  Float epsilon_;
  F &field;
  const P &partition_;
  const parameters<Float> pars_;

  const Float quadratic_coef_1;
  const Float quadratic_coef_2;
  const Float quadratic_coef;
  
  Float gr;

};

#ifdef STRIPS
template<typename F, typename P  > 
long int 
make_sweep(F &field, const parameters<Float> &pars, const P &partition ) {

  long int accepted=0;
  
  Updater<F,Partition> update(field,partition,pars);
#if   _OPENMP
  const int num_threads=omp_get_max_threads();
#else
  const int num_threads=1;
#endif

  const int n_sites=  F::indexer_t::n_sites();
  const int chunk =n_sites/(2*num_threads);

#pragma omp parallel default(none) shared(update,accepted)   
  {
#pragma omp for  reduction(+:accepted) 
    for(int i=0;i<num_threads;++i){ 
      for(int s=2*i*chunk;s<(2*i+1)*chunk;++s) 
	accepted+=update(s);
    
    }
#pragma omp for  reduction(+:accepted) 
    for(int i=0;i<num_threads;++i){ 
      for(int s=(2*i+1)*chunk;s<(2*i+2)*chunk;++s) 
	accepted+=update(s);
    
    }
  }
  return accepted;
 
}

#else
template<typename F, typename P  >
long int
make_sweep(F &field, const parameters<Float> &pars, const P &partition ) {

  long int accepted=0;

  Updater<F,Partition> update(field,partition,pars);
#pragma omp parallel default(none) shared(partition,update,accepted)
  for(int p=0;p<partition.n_partitions();++p) {


    /* this loop can be parallelised */
#pragma omp for reduction(+:accepted)
    for(int s=0;s<partition.partition_size();++s) {
      int i=partition.partition(p,s);

      accepted+=update(i);

    }

  }
  return accepted;
}
#endif


template
long int make_sweep< Field, Partition >( Field &,
const parameters<Float> &,
const  Partition  &partition
);
