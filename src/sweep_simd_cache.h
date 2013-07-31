#pragma once

#include "partition.h"
#include "typedefs.h"

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

template <typename F>
long int make_sweep(F& field, const parameters<Float>& pars, int block_sweeps);
