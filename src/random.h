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


class taus_array {

public:
  static const int pitch=16;

  taus_array(int n):n_generators_(n) {
    seeds_ = new unsigned[pitch*n_generators_];    
    gen_seeds(121245);
  }

inline  double rand(int i) {
  taus_step(seeds_[i*pitch],13,19,12,4294967294u);
  taus_step(seeds_[i*pitch+1], 2,25, 4,4294967288u);
  taus_step(seeds_[i*pitch+2], 3,11,17,4294967280u);
  LCG_step(seeds_[i*pitch+3],1664525u,1013904223u);
  return 2.328306436538696e-10*(double)(seeds_[i*pitch]^
					seeds_[i*pitch+1]^
					seeds_[i*pitch+2]^
					seeds_[i*pitch+3]
					);
  }

  void gen_seeds(long int);
  void set_seeds(const unsigned *seed);
  void get_seeds(unsigned *seed) const;
    static taus_array *generator()  {return generator_;}
    static void init(int n, long int seed) {
    generator_= new taus_array(n);
    generator_->gen_seeds(seed);
  }
private:
  int n_generators_;
  unsigned *seeds_;

void taus_step(unsigned &z, int S1,int S2,int S3,unsigned M) {
    unsigned b=(((z<<S1)^z)>>S2);
    z=(((z &M)<<S3)^b);
  }
  
void LCG_step(unsigned &z,unsigned A,unsigned C) {
    z=(A*z+C);
  }

  static taus_array *generator_;


};


#endif


