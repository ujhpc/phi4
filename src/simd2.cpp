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
#if 0
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
extern "C" int int_test2(int a, int b) {
  ivec_t c(a + 1, a + 2, a + 3, a + 4);
  ivec_t d(a - 1, a - 2, a - 3, a - 4);
  return (c ^ d)[2];
}
extern "C" float gath(float* ptr, int a, int b) {
  ivec_t c(a + 1, a + 2, a + 3, a + 4);
  ivec_t d(a - 1, a - 2, a - 3, a - 4);
  fvec_t f(ptr, c);
  fvec_t g(ptr, d);
  return (f + g)[2];
}
#endif

fvec_t::vector_t gath(float* ptr, ivec_t::vector_t index) {
  ivec_t i(index);
  fvec_t f(ptr, i);
  return f.v;
}
