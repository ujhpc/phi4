#include<cstdio>
#include<cstdlib>


#define FLOAT double
#define __TARGET__
#include "tausworthe.h"

main() {

  unsigned z[8]={6544,9988877,9888,1234,98765,98766,87654,776666};
  
  double sum1=0.0;
  double sum2=0.0;

  for(int i=0;i<100000000;i++) {
    //    sum+=drand48();
    sum1+=rnd(z);
    sum2+=rnd(z+4);
    
  }

  fprintf(stdout,"%12.8f\n",sum1+sum2);
}
