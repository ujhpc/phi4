#ifndef __SWEEP_H__
#define __SWEEP_H__

#include "partition.h"
#include "typedefs.h"

const int N_HIT=8;

template<typename F> struct parameters {
  F g;
  F m_2;
  F i_Lambda;
};


template<typename F>
long int make_sweep(F &field, const parameters<Float> &pars, int block_sweeps);


#endif
