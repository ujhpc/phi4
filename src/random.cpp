#include<cstdlib>
#include<cstdio>

#include "typedefs.h"

const unsigned int two_16=1<<16;

void rand48_array::gen_seeds(long seed) {
  srand48(seed);
  for(int i=0;i<pitch*n_generators_;++i) {
    seeds_[i]=lrand48()%two_16;
  }
}
void rand48_array::set_seeds(const unsigned  short *seed) {
  for(int i=0;i<pitch*n_generators_;++i) {
    seeds_[i]=seed[i];
  }
}

void rand48_array::get_seeds(unsigned  short *seed) const {
  for(int i=0;i<pitch*n_generators_;++i) {
    seed[i]=seeds_[i];
  }

}

int rand48_array::fwrite_state(FILE *fout) {
  return std::fwrite(seeds_,sizeof(short),n_generators_,fout);
}

rand48_array *rand48_array::generator_;

void taus_array::gen_seeds(long seed) {
  srand48(seed);
  for(int i=0;i<4*n_generators_;++i) {   
    unsigned r;
    while( (r=lrand48()) < 128) {};
    seeds_[i]=r;
  }
}

void taus_array::set_seeds(const unsigned   *seed) {
  for(int i=0;i<4*n_generators_;++i) {
    seeds_[i]=seed[i];
  }
}

void taus_array::get_seeds(unsigned   *seed) const {
  for(int i=0;i<4*n_generators_;++i) {
    seed[i]=seeds_[i];
  }
}

taus_array *taus_array::generator_;
