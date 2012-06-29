#include<cstdlib>

#include<iostream>
#include<iomanip>
#include<cmath>
#include<fvec.h>

#include"emmintrin.h"

const int n_max=32;
const int n_coeffs=8;
float expi_[n_max];
float coeffs_[n_coeffs];


inline float my_exp_1(float x) {
 float sum=1.0f;
 float prod=1.0f;
  
#pragma unroll
  for(int i=0;i<n_coeffs;++i) {
    prod*=x;
    sum+=prod*coeffs_[i];
  }
  return sum;
}

inline float my_expf(float x) {
  int fl=ceil(x);
  //int fl=0;
  float y=x-fl;

  float sum=1.0f;
  float prod=1.0f;
  //  std::cerr<<"my_exp "<<fl<<" "<<y<<" "<<(int)fl<<std::endl;
#pragma unroll
  for(int i=0;i<n_coeffs;++i) {
    prod*=y;
    sum+=prod*coeffs_[i];
  }
  return sum*expi_[(int)fabs(fl)];
}


static inline float add_elements(__m128 const & x) {
__m128 s;
s = _mm_hadd_ps(x, x);
s = _mm_hadd_ps(s, s);
return _mm_cvtss_f32(s);
}

static const float coef[16] = { // table of 1/n!
1., 1./2., 1./6., 1./24., 1./120., 1./720., 1./5040.,
1./40320., 1./362880., 1./3628800., 1./39916800.,
1./4.790016E8, 1./6.22702E9, 1./8.71782E10,
1./1.30767E12, 1./2.09227E13};


float Exp(float x) { // Approximate exp(x) for small x
__declspec(align(16)) // align table by 16

float x2 = x * x; // x^2
float x4 = x2 * x2; // x^4
// Define vectors of four floats
F32vec4 xxn(x4, x2*x, x2, x); // x^1, x^2, x^3, x^4
F32vec4 xx4(x4); // x^4
F32vec4 s(0.f, 0.f, 0.f, 1.f); // initialize sum
for (int i = 0; i < 16; i += 4) { // Loop by 4
s += xxn * _mm_load_ps(coef+i); // s += x^n/n!
xxn *= xx4; // next four x^n
}
return add_elements(s); // add the four sums
}


float Exp8(float x) { // Approximate exp(x) for small x
__declspec(align(16)) // align table by 16

float x2 = x * x; // x^2
float x4 = x2 * x2; // x^4
// Define vectors of four floats
F32vec4 xxn(x4, x2*x, x2, x); // x^1, x^2, x^3, x^4
F32vec4 xx4(x4); // x^4
F32vec4 s(0.f, 0.f, 0.f, 1.f); // initialize sum

s += xxn * _mm_load_ps(coef); // s += x^n/n!
xxn *= xx4;
s += xxn * _mm_load_ps(coef+4); // s += x^n/n!
xxn *= xx4;

return add_elements(s); // add the four sums
}

void comp(float x) {
  float e=std::exp(x);
  // float my_e=my_expf(x);
 float my_e=Exp8(x);
  std::cout<<e<<" "<<my_e<<" "<<my_e-e<<" "<<my_e/e-1.0f<<std::endl;
}

main() {

  
  for(int i=0;i<n_max;i++) {
    expi_[i]=std::exp(-(float)i);
  }

  float factorial=1.0f;

  for(int i=0;i<n_coeffs;++i) {

    factorial=(i+1)*factorial;
    coeffs_[i]=1.0f/factorial;

    std::cout<<coeffs_[i]<<std::endl;
  }

  std::cout<<std::setprecision(16);
#if 0  
  comp(-0.3f);
  comp(-0.5f);
  comp(-0.7f);
  comp(-0.9f);
  //comp(-1.0f);

  //comp(-1.5f);
  
  //comp(-11.3244f);
#endif
#if 1
  float sum=0.0f;
  for(int i=0;i<100000000;i+=10) {
    #pragma unroll
    for(int j=0;j<10;j++) {
      float x=-1.0f*drand48();
      //sum+=std::exp(x);
      //sum+=Exp8(x);
      sum+=0.0001f;
    }

  }



  std::cout<<sum<<std::endl;
#endif

}

