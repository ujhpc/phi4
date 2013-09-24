#include "typedefs.h"

void taus_array::gen_seeds(long seed) {
  srand48(seed);
  for (int i = 0; i < pitch * n_generators_; ++i) {
#if SIMD <= 1
    unsigned r;
    while ((r = lrand48()) < 128) {
    }
    seeds_[i] = r;
#else
    for (int s = 0; s < SIMD; ++s) {
      unsigned r;
      while ((r = lrand48()) < 128) {
      }
      seeds_[i][s] = r;
    }
#endif
  }
}

void taus_array::set_seeds(const unsigned* seed) {
  for (int i = 0; i < 4 * n_generators_; ++i) {
    seeds_[i] = seed[i];
  }
}

void taus_array::get_seeds(unsigned* seed) const {
  for (int i = 0; i < 4 * n_generators_; ++i) {
#if SIMD <= 1
    seed[i] = seeds_[i];
#else
    seed[i] = seeds_[i][0];
#endif
  }
}

taus_array* taus_array::generator_;
