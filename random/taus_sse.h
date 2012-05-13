#ifndef __TAUS_SSE_H__
#define __TAUS_SSE_H__

#include"emmintrin.h"








static inline __m128i muly(const __m128i &a, const __m128i &b) {
#ifdef __SSE4_1__  // modern CPU - use SSE 4.1
  return _mm_mullo_epi32(a, b);
#else               // old CPU - use SSE 2
  __m128i tmp1 = _mm_mul_epu32(a,b); /* mul 2,0*/
  __m128i tmp2 = _mm_mul_epu32( _mm_srli_si128(a,4), _mm_srli_si128(b,4)); /* mul 3,1 */
  return _mm_unpacklo_epi32(_mm_shuffle_epi32(tmp1, _MM_SHUFFLE (0,0,2,0)), _mm_shuffle_epi32(tmp2, _MM_SHUFFLE (0,0,2,0))); /* shuffle results to [63..0] and pack */
#endif
}


class taus_sse_array {
 public:


inline static  __m128i  
taus_step_SSE2(const __m128i &z, int S1,int S2,int S3,unsigned M)   {
  const __m128i A = _mm_set_epi32(M,M,M,M);
  __m128i b=_mm_srli_epi32(_mm_xor_si128(_mm_slli_epi32(z,S1),z),S2);
  return _mm_xor_si128(_mm_slli_epi32(_mm_and_si128(z, A),S3),b);
}


inline static __m128i 
LCG_step_SSE2 ( const __m128i  &z, unsigned A,unsigned C)  {
  const __m128i mm_A = _mm_set_epi32(A,A,A,A);
  const __m128i mm_C = _mm_set_epi32(C,C,C,C);
  return _mm_add_epi32(muly(z,mm_A),mm_C);
}


static inline  __m128i  
irnd_SSE2( __m128i *z) {
  z[0]=taus_step_SSE2(z[0],13,19,12,4294967294u);
  z[1]=taus_step_SSE2(z[1], 2,25, 4,4294967288u);
  z[2]=taus_step_SSE2(z[2], 3,11,17,4294967280u);
  z[3]=LCG_step_SSE2(z[3],1664525u,1013904223u);
  return _mm_xor_si128(_mm_xor_si128(z[0],z[1]),
		       _mm_xor_si128(z[2],z[3]));
  
}


static inline  __m128  
to_sym_float(__m128i z)  {
  const __m128  inv = _mm_set1_ps(2.328306436538696e-10f);
			       
  return _mm_mul_ps(_mm_cvtepi32_ps(z),inv);
}


static inline  __m128  to_float(__m128i z)  {
  const __m128  half = _mm_set1_ps(0.5f);
  return _mm_add_ps(to_sym_float(z),half);
}


 taus_sse_array(int n):n_generators_(n),
  seeds_(new __m128i[n_generators_]) {};

inline  __m128i  mm_irand(int i) const {
  return  irnd_SSE2(seeds_ + 4*i);
}

inline  __m128  mm_rand_sym(int i) const {
  return to_sym_float(mm_irand(i));
}

inline  __m128  mm_rand(int i) const {
  return to_float(mm_irand(i));
}

 inline  unsigned int  irand(int i)  {
  return 0;
}

 inline  float  rand_sym(int i)  {
  return 0.0f;
  
}

 inline  float  rand(int i)  {
  return 0.0f;
}


 private:
 int n_generators_;
 __m128i *seeds_;


};


#endif
