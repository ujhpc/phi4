#ifndef __RANDOM_H__
#define __RANDOM_H__

#include<cstdlib>

class rand48_array {
  
  
 public:
  
 rand48_array(int n):n_generators_(n), 
    seeds_(new unsigned short[3*n_generators_]) {

  };
  
  void gen_seeds(long int);
  void set_seeds(const unsigned  short *seed);
  void get_seeds(unsigned  short *seed) const;



  double rand(int i) {
    return erand48(seeds_+3*i);
  };

 private:
  int n_generators_;


  unsigned short *seeds_;


};


#endif

