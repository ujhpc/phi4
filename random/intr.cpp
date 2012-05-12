#include<cstdio>

#include"emmintrin.h"

void taus_step(unsigned &z, int S1,int S2,int S3,unsigned M) {
  unsigned b=(((z<<S1)^z)>>S2);
  z=(((z &M)<<S3)^b);
}
  
void LCG_step(unsigned &z,unsigned A,unsigned C) {
  z=(A*z+C);
}


void taus_step_SSE2(__m128i &z, int S1,int S2,int S3,unsigned M) {
  const __m128i A = _mm_set_epi32(M,M,M,M);
  __m128i b=_mm_srli_epi32(_mm_xor_si128(_mm_slli_epi32(z,S1),z),S2);
  z=_mm_xor_si128(_mm_slli_epi32(_mm_and_si128(z, A),S3),b);
}



void LCG_step_SSE2 ( __m128i  &z, unsigned A,unsigned C) {
  const __m128i mm_A = _mm_set_epi32(A,A,A,A);
  const __m128i mm_C = _mm_set_epi32(C,C,C,C);
  //z=_mm_mullo_epi32(z,mm_A); //SSE4
  z=_mm_add_epi32(z,mm_C);
}


main() {


  unsigned int z=777;
  __m128i mm_z = _mm_set_epi32(z,z,z,z);
  


  for(int i=0;i<10000000;++i) 
    for(int j=0;j<100;j++) {
#if 0
      taus_step(z,13,19,12,4294967294u);
      taus_step_SSE2(mm_z,13,19,12,4294967294u);
#else
      LCG_step(z,1664525u,1013904223u);
       LCG_step_SSE2(mm_z,1664525u,1013904223u);
      
#endif
    }
  
  unsigned int *y=(unsigned int*)&mm_z;


  printf("%u\n----\n",z);
  printf("%u\n",y[0]);
  printf("%u\n",y[1]);
  printf("%u\n",y[2]);
  printf("%u\n",y[3]);




}
