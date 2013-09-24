#pragma once

#include <iostream>
#include <cstdlib>

#include "typedefs.h"

static inline IVec taus_step(IVec z, IVec S1, IVec S2, IVec S3, IVec M) {
  IVec b = (((z << S1) ^ z) >> S2);
  return (((z & M) << S3) ^ b);
}

static inline IVec LCG_step(IVec z, IVec A, IVec C) { return (A * z + C); }

class taus_array {

 public:
  static const int pitch = 16;

  taus_array(int n) : n_generators_(n) {
    seeds_ =
        (IVec*)_mm_malloc(pitch * n_generators_ * sizeof(IVec), sizeof(IVec));
    gen_seeds(121245);
  }

  ~taus_array() { _mm_free(seeds_); }

  inline IVec* seed(int i) { return &seeds_[i * pitch]; }

  inline IVec irand(int i) {
    IVec* s = seed(i);
    IVec seed0 = taus_step(s[0], 13, 19, 12, 4294967294u);
    IVec seed1 = taus_step(s[1], 2, 25, 4, 4294967288u);
    IVec seed2 = taus_step(s[2], 3, 11, 17, 4294967280u);
    IVec seed3 = LCG_step(s[3], 1664525u, 1013904223u);

    s[0] = seed0;
    s[1] = seed1;
    s[2] = seed2;
    s[3] = seed3;
    return seed0 ^ seed1 ^ seed2 ^ seed3;
  }

  inline FVec rand_sym(int i) {
    return FVec(Float(2.328306436538696e-10)) * (FVec)irand(i);
  }

  inline FVec rand(int i) { return rand_sym(i) + FVec(Float(0.5)); }

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
  IVec* seeds_;

  static taus_array* generator_;
};
