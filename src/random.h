#ifndef __RANDOM_H__
#define __RANDOM_H__

#include<cstdlib>

class rand48_array {
 static const int pitch = 32;
  
 public:
  
 rand48_array(int n):n_generators_(n), 
    seeds_(new unsigned short[pitch*n_generators_]) {

  };
  
  void gen_seeds(long int);
  void set_seeds(const unsigned  short *seed);
  void get_seeds(unsigned  short *seed) const;



  double rand(int i) {
    return erand48(seeds_+pitch*i);
  };

  static rand48_array *generator()  {return generator_;}
  static void init(int n, long int seed) {
    generator_= new rand48_array(n);
    generator_->gen_seeds(seed);
  }
 private:
  int n_generators_;


  unsigned short *seeds_;

  static rand48_array *generator_;

};


#endif


