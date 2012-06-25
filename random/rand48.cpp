#include<cmath>
#include<cstdio>
#include<cstdlib>



long unsigned int mask=0x0000ffffffffffff;
long unsigned int a = 0x5DEECE66D;
long unsigned int c = 0xB;
double inv=3.5527136788005009294e-15;

double 
rand48(long unsigned &seed) {
  seed= mask & (a*seed+c);
  return inv*seed;
}

main() {


  
  fprintf(stdout,"%lu\n",sizeof(unsigned long int));

  long unsigned int seed=987765544;
  for(int i=0;i<100;++i) {
    seed= mask & (a*seed+c);
    fprintf(stdout,"%12.8f\n",inv*seed);

  }

}
