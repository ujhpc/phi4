#include<cmath>
#include<iostream>
#include<mkl.h>


main() {


  float x=0.0f;

  float y=std::log(x);

  if(-1.0< y) 
    std::cout<<"greater "<<std::endl;
  else
    std::cout<<"smaller "<<std::endl;


  float v[4];
  float w[4];
  float sum=0.0f;
  for(int i=0;i<10000000;i++) {  
#if 0
   #pragma unroll
    for(int j=0;j<4;j++) {
      v[j]=drand48();
    }
      vsLn(4,v,w);
   #pragma unroll
      for(int j=0;j<4;j++) {
	sum+=w[j];
    }

#else
      for(int j=0;j<4;j++) {
	sum+=std::log(drand48());
    } 

#endif
  }

  std::cout<<sum<<std::endl;



}
