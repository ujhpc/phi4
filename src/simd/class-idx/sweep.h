#ifndef __SWEEP_H__
#define __SWEEP_H__

#include "partition.h"
#include "typedefs.h"

// const Float EPSILON =  1.0;
#ifndef N_HIT
const int N_HIT = 8;
#endif

template <typename F> struct parameters {
  F g;
  F m_2;
  F i_Lambda;
};

template <typename F, typename P>
long int make_sweep(F& field,
                    const parameters<Float>& pars,
                    const P& part = single_partition<typename F::indexer_t>());

#endif
