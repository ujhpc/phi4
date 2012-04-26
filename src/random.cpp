#include<cstdlib>

#include"random.h"

const unsigned int two_16=1<<16;

void rand48_array::gen_seeds(long seed) {
  srand48(seed);
  for(int i=0;i<3*n_generators_;++i) {
    seeds_[i]=lrand48()%two_16;
  }
}
void rand48_array::set_seeds(const unsigned  short *seed) {
  for(int i=0;i<3*n_generators_;++i) {
    seeds_[i]=seed[i];
  }
}

void rand48_array::get_seeds(unsigned  short *seed) const {
  for(int i=0;i<3*n_generators_;++i) {
    seed[i]=seeds_[i];
  }

}
