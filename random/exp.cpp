#include<cstdlib>

#include<iostream>
#include<iomanip>
#include<cmath>

const int n_max=32;
const int n_coeffs=8;
float expi_[n_max];
float coeffs_[n_coeffs];


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

void comp(float x) {
  float e=std::exp(x);
  float my_e=my_expf(x);
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
  
  comp(-0.3f);
  comp(-0.5f);
  comp(-0.7f);
  comp(-0.9f);
  comp(-1.0f);

  comp(-1.5f);
  
  comp(-11.3244f);

  float sum=0.0f;
  for(int i=0;i<100000000;i++) {
    float x=-1.0f*drand48();
    //    sum+=std::exp(x);
    sum+=my_expf(x);
  }

  std::cout<<sum<<std::endl;

}

