#pragma once

#include "partition.h"
#include "types.h"

#ifndef EPSILON
const Float EPSILON = 1.0;
#endif

#ifndef N_HIT
const int N_HIT = 8;
#endif

template <typename F> struct parameters {
  F g;
  F m_2;
  F i_Lambda;
};

#if defined(CACHE) && defined(SIMD)  //////////////////////////////////////////

template <typename F>
long int make_sweep(F& field, const parameters<Float>& pars, int block_sweeps);

#else  ////////////////////////////////////////////////////////////////////////

template <typename F, typename P>
long int make_sweep(F& field,
                    const parameters<Float>& pars,
                    const P& part = single_partition<typename F::indexer_t>());
#endif
