#include <cstdio>
#include "simd2.h"

#if __AVX2__
#define SIMD 8
#else
#define SIMD 4
#endif

typedef simd<float, SIMD> fvec_t;
typedef simd<int, SIMD> ivec_t;

// Some code generation tests
extern "C" int float_test(float a, float b) {
  ivec_t c = fvec_t(a) < fvec_t(b);
  c >>= 2;
  return c[2];
}
extern "C" int int_test(int a, int b) {
  ivec_t c = ivec_t(a) ^ ivec_t(b);
  c = max(c, ivec_t(1));
  c >>= 2;
  return c[2];
}
