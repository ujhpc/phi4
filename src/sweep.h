#ifndef __SWEEP_H__
#define __SWEEP_H__

#include"partition.h"

const double EPSILON =  1.0;
const int N_HIT=5;


template<typename F> struct parameters {
  F g;
  F m_2;
  F i_Lambda;
};


template<typename F, typename P  > 
long int 
  make_sweep(F &field, const parameters<double> &pars, const P &part = 
	     single_partition<typename F::indexer_t>() ) ;


#endif
