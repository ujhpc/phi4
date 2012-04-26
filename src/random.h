#ifndef __RANDOM_H__
#define __RANDOM_H__



class rand48_array {
  
  
 public:
  
 rand48_array(int i):n_generators_(i) {};
  
  void gen_seeds(long int);
  void set_seeds(const unsigned  short *seed);
  void get_seeds(unsigned  short *seed) const;

  void next(int i);
  double value(int i) ;
  
 private:
  int n_generators_;
  


};


#endif


