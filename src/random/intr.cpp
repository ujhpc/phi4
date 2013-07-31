#include <cstdio>

#include "../random.h"

#include "taus_sse.h"

void taus_step(unsigned& z, int S1, int S2, int S3, unsigned M) {
  unsigned b = (((z << S1) ^ z) >> S2);
  z = (((z & M) << S3) ^ b);
}

void LCG_step(unsigned& z, unsigned A, unsigned C) { z = (A * z + C); }

main() {

  unsigned int z = 777;

  unsigned int seed[4] = { z, z, z, z };

  __m128i mm_z = _mm_set_epi32(z, z, z, z);
  __m128i mm_seed[4];
  mm_seed[0] = _mm_set_epi32(z, z, z, z);
  mm_seed[1] = _mm_set_epi32(z, z, z, 1232);
  mm_seed[2] = _mm_set_epi32(z, 21, z, z);
  mm_seed[3] = _mm_set_epi32(z, z, 9876, z);

  taus_sse_array rnd(2);
  rnd.gen_seeds(1377);

  double sum = 0.0f;
  for (int i = 0; i < 1000000; ++i)
    for (int j = 0; j < 100; j++) {
#if 0
      taus_step(z,13,19,12,4294967294u);
      taus_step_SSE2(mm_z,13,19,12,4294967294u);
#else
      // LCG_step(z,1664525u,1013904223u);
      // LCG_step_SSE2(mm_z,1664525u,1013904223u);
      __m128 x = rnd.mm_rand(1);
      float* t = (float*)&x;
      sum += (t[0] + t[1] + t[2] + t[3]);
// float a=rnd.rand(0);
// sum+=a;

#endif
    }

  fprintf(stdout, "sum = %12.8f\n", sum * 0.25e-8);

  unsigned int* y = (unsigned int*)&mm_z;

  printf("%u\n----\n", z);
  printf("%u\n", y[0]);
  printf("%u\n", y[1]);
  printf("%u\n", y[2]);
  printf("%u\n", y[3]);
}
