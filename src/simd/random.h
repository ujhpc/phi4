#pragma once

#include <iostream>
#include <cstdlib>

#include "typedefs.h"

class rand48_array {
  static const int pitch = 32;

 public:

  rand48_array(int n)
      : n_generators_(n), seeds_(new unsigned short[pitch * n_generators_]) {};

  void gen_seeds(long int);
  void set_seeds(const unsigned short* seed);
  void get_seeds(unsigned short* seed) const;

  Float rand(int i) { return erand48(seeds_ + pitch * i); }
  ;

  long int irand(int i) { return nrand48(seeds_ + pitch * i); }
  ;

  static rand48_array* generator() { return generator_; }
  static void init(int n, long int seed) {
    generator_ = new rand48_array(n);
    generator_->gen_seeds(seed);
  }

 private:
  int n_generators_;

  unsigned short* seeds_;

  static rand48_array* generator_;
};

static inline SIVec taus_step(SIVec z, SIVec S1, SIVec S2, SIVec S3, SIVec M) {
  SIVec b = (((z << S1) ^ z) >> S2);
  return (((z & M) << S3) ^ b);
}

static inline SIVec LCG_step(SIVec z, SIVec A, SIVec C) { return (A * z + C); }

class taus_array {

 public:
  static const int pitch = 16;

  taus_array(int n) : n_generators_(n) {
    seeds_ = (SIVec*)_mm_malloc(pitch * n_generators_ * sizeof(SIVec),
                                sizeof(SIVec));
    gen_seeds(121245);
  }

  ~taus_array() { _mm_free(seeds_); }

  inline SIVec* seed(int i) { return &seeds_[i * pitch]; }

  inline SIVec irand(int i) {
    SIVec* s = seed(i);
    SIVec seed0 = taus_step(s[0], 13, 19, 12, 4294967294u);
    SIVec seed1 = taus_step(s[1], 2, 25, 4, 4294967288u);
    SIVec seed2 = taus_step(s[2], 3, 11, 17, 4294967280u);
    SIVec seed3 = LCG_step(s[3], 1664525u, 1013904223u);

    s[0] = seed0;
    s[1] = seed1;
    s[2] = seed2;
    s[3] = seed3;
    return seed0 ^ seed1 ^ seed2 ^ seed3;
  }

  inline SFVec srand_sym(int i) {
    return SFVec((Float)2.328306436538696e-10) * (SFVec)irand(i);
  }

  inline FVec rand_sym(int i) {
#if SIMD == SSIMD
    return srand_sym(i);
#elif SIMD == 8 && SSIMD == 4
    SFVec v1 = srand_sym(i);
    SFVec v2 = srand_sym(i);
#warning Short fused SIMD Tausworthe implementation!
    return FVec(v1[0], v1[1], v1[2], v1[3], v2[0], v2[1], v2[2], v2[3]);
#else
#error Layout not supported!
#endif
  }

  inline FVec rand(int i) { return rand_sym(i) + FVec((Float)0.5); }

  void gen_seeds(long int);
  void set_seeds(const unsigned* seed);
  void get_seeds(unsigned* seed) const;
  static taus_array* generator() { return generator_; }
  static void init(int n, long int seed) {
    generator_ = new taus_array(n);
    generator_->gen_seeds(seed);
  }

 private:
  int n_generators_;
  SIVec* seeds_;

  static taus_array* generator_;
};
