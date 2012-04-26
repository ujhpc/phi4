#ifndef __SWEEP_H__
#define __SWEEP_H__

const double EPSILON =  1.0;
const int N_HIT=5;


template<typename F> struct parameters {
  F g;
  F m_2;
  F i_Lambda;
};


template<typename F> 
long int 
make_sweep(F &field, const parameters<double> &pars) ;


#endif
