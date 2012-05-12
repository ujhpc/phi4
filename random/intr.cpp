#include<cstdio>

#include"emmintrin.h"

#include"taus.h"

void taus_step(unsigned &z, int S1,int S2,int S3,unsigned M) {
  unsigned b=(((z<<S1)^z)>>S2);
  z=(((z &M)<<S3)^b);
}
  
void LCG_step(unsigned &z,unsigned A,unsigned C) {
  z=(A*z+C);
}


inline void  taus_step_SSE2(__m128i &z, int S1,int S2,int S3,unsigned M) {
  const __m128i A = _mm_set_epi32(M,M,M,M);
  __m128i b=_mm_srli_epi32(_mm_xor_si128(_mm_slli_epi32(z,S1),z),S2);
  z=_mm_xor_si128(_mm_slli_epi32(_mm_and_si128(z, A),S3),b);
}




static inline __m128i muly(const __m128i &a, const __m128i &b)
{
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
    return _mm_mullo_epi32(a, b);
#else               // old CPU - use SSE 2
    __m128i tmp1 = _mm_mul_epu32(a,b); /* mul 2,0*/
    __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4)); /* mul 3,1 */
    return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
#endif
}

inline void LCG_step_SSE2 ( __m128i  &z, unsigned A,unsigned C) {
  const __m128i mm_A = _mm_set_epi32(A,A,A,A);
  const __m128i mm_C = _mm_set_epi32(C,C,C,C);
  z=muly(z,mm_A);
  z=_mm_add_epi32(z,mm_C);
}


inline  __m128i  irnd_SSE2( __m128i *z) {
  taus_step_SSE2(z[0],13,19,12,4294967294u);
  taus_step_SSE2(z[1], 2,25, 4,4294967288u);
  taus_step_SSE2(z[2], 3,11,17,4294967280u);
  LCG_step_SSE2(z[3],1664525u,1013904223u);
  //  return 2.328306436538696e-10*(double)
  return _mm_xor_si128(_mm_xor_si128(z[0],z[1]),
		       _mm_xor_si128(z[2],z[3]));
  
}

inline  __m128  rnd_sym_SSE2( __m128i z) {
  const __m128  inv = _mm_set1_ps(2.328306436538696e-10f);
			       
  return _mm_mul_ps(_mm_cvtepi32_ps(z),inv);
}


inline  __m128  rnd_SSE2( __m128i z) {
  const __m128  half = _mm_set1_ps(0.5f);
  return _mm_add_ps(rnd_sym_SSE2(z),half);
}

main() {


  unsigned int z=777;
  
  unsigned int seed[4]={z,z,z,z};
  
  __m128i mm_z = _mm_set_epi32(z,z,z,z);
  __m128i mm_seed[4];
  mm_seed[0] =_mm_set_epi32(z,z,z,z);
  mm_seed[1] =_mm_set_epi32(z,z,z,1232);
  mm_seed[2] =_mm_set_epi32(z,21,z,z);
  mm_seed[3] =_mm_set_epi32(z,z,9876,z);

  double sum=0.0f;
  for(int i=0;i<1000000;++i) 
    for(int j=0;j<100;j++) {
#if 0
      taus_step(z,13,19,12,4294967294u);
      taus_step_SSE2(mm_z,13,19,12,4294967294u);
#else
      //LCG_step(z,1664525u,1013904223u);
      //LCG_step_SSE2(mm_z,1664525u,1013904223u);
      __m128 x = rnd_SSE2(irnd_SSE2(mm_seed));
      float  *t=(float *)&x;
      sum+=(t[0]+t[1]+t[2]+t[3]);

      

#endif
    }
  
  fprintf(stdout,"sum = %12.8f\n",sum*0.25e-8);
  
  unsigned int *y=(unsigned int*)&mm_z;


  printf("%u\n----\n",z);
  printf("%u\n",y[0]);
  printf("%u\n",y[1]);
  printf("%u\n",y[2]);
  printf("%u\n",y[3]);




}
