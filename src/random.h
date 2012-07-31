#ifndef __RANDOM_H__
#define __RANDOM_H__


#include <cstdlib>
#include <cstdio>

#include<limits>
#include<iostream>

#include "typedefs.h"

template<typename S, int SEEDS_PER_GEN, int ALIGNMENT_IN_BYTES>  
class random_array_state {

 
    public:
  random_array_state(int n_gen):
    n_gen_(n_gen), 
    SEEDS_BLOCK_SIZE(ALIGNMENT_IN_BYTES*((sizeof(S)*SEEDS_PER_GEN+ALIGNMENT_IN_BYTES-1)/ALIGNMENT_IN_BYTES)),
    SEEDS_BLOCK(SEEDS_BLOCK_SIZE/sizeof(S)),
    seeds_per_gen(SEEDS_PER_GEN)
  {
    posix_memalign((void **)&seeds_,ALIGNMENT_IN_BYTES,SEEDS_BLOCK_SIZE*n_gen_);
  }

  int n_gen() const {return n_gen_;}

  void set_seeds(const S *seeds) {
    for(int i=0;i<n_gen_;++i) {
      for(int j=0;j<SEEDS_PER_GEN;++j)
	seeds_[i*SEEDS_BLOCK_SIZE/sizeof(S)+j]=seeds[i*SEEDS_PER_GEN+j];
    }
  }


  void get_seeds(S *seeds) const {
    for(int i=0;i<n_gen_;++i) {
      for(int j=0;j<SEEDS_PER_GEN;++j)
	seeds[i*SEEDS_PER_GEN+j]=seeds_[i*SEEDS_BLOCK_SIZE/sizeof(S)+j];
    }
  }


  void gen_seeds(long seed) {
    srand48(seed);
    for(int i=0;i<n_gen_;++i) {
      for(int j=0;j<SEEDS_PER_GEN;++j)
	seeds_[i*SEEDS_BLOCK_SIZE/sizeof(S)+j]=
	  lrand48()%(std::numeric_limits<S>::max());
    }
  }

  int fwrite_state(FILE *fout) {
    S *seeds=new S[SEEDS_PER_GEN*n_gen_];
    get_seeds(seeds);
    int bytes;
    const int n_params=3;
    int params[n_params]={n_gen_,SEEDS_PER_GEN,sizeof(S)};
    bytes=std::fwrite(params,sizeof(int),n_params,fout);
    bytes+=std::fwrite(seeds,sizeof(S),n_gen_*SEEDS_PER_GEN,fout);
    delete [] seeds;
    return bytes;
  };
    
  int fread_state(FILE *fin) {
    const int n_params=3;
    int params[n_params];
    int bytes=std::fread(params,sizeof(int),n_params,fin);
    if(params[0]!=n_gen_) {
      std::cerr<<"number of generators differ : program "<<n_gen_<<" read  "<<params[0]<<std::endl;
      exit(7);
    }
    if(params[1]!=SEEDS_PER_GEN) {
      std::cerr<<"SEES_PER_GEN differ : program "<<SEEDS_PER_GEN<<" read  "<<params[1]<<std::endl;
      exit(7);
    }
    if(params[2]!=sizeof(S)) {
      std::cerr<<"sizeof seeds differ : program "<<sizeof(S)<<" read  "<<params[2]<<std::endl;
      exit(7);
    }
    S *seeds=new S[SEEDS_PER_GEN*n_gen_];
    bytes+=std::fread(seeds,sizeof(S),n_gen_*SEEDS_PER_GEN,fin);
    set_seeds(seeds);
    delete [] seeds;
    return bytes;
  }
    


protected:
  const int SEEDS_BLOCK_SIZE;
  const int SEEDS_BLOCK;
  const int seeds_per_gen;
  int n_gen_;
  S   *seeds_;
 

};



class rand48_array: public random_array_state<unsigned short, 3 , 256 > {
  
 public:
  
 rand48_array(int n):
   random_array_state<unsigned short,3,256>(n) {};
  

  Float rand(int i) {
    return erand48(seeds_+SEEDS_BLOCK*i);
  };

  long int  irand(int i) {
    return nrand48(seeds_+SEEDS_BLOCK*i);
  };

  static rand48_array *generator()  {return generator_;}
  static void init(int n, long int seed) {
    generator_= new rand48_array(n);
    generator_->gen_seeds(seed);
  }


 private:

  static rand48_array *generator_;

};




class taus_array: public random_array_state<unsigned int , 4 , 256 > {

 public:


  taus_array(int n):random_array_state<unsigned int , 4 , 256 >(n) {
    gen_seeds(121245);
  }

  inline  unsigned irand(int i) {
    taus_step(seeds_[i*SEEDS_BLOCK],13,19,12,4294967294u);
    taus_step(seeds_[i*SEEDS_BLOCK+1], 2,25, 4,4294967288u);
    taus_step(seeds_[i*SEEDS_BLOCK+2], 3,11,17,4294967280u);
    LCG_step(seeds_[i*SEEDS_BLOCK+3],1664525u,1013904223u);
    return (seeds_[i*SEEDS_BLOCK]^
	    seeds_[i*SEEDS_BLOCK+1]^
	    seeds_[i*SEEDS_BLOCK+2]^
	    seeds_[i*SEEDS_BLOCK+3]
	    );
  }

  inline  Float rand(int i) {
    taus_step(seeds_[i*SEEDS_BLOCK],13,19,12,4294967294u);
    taus_step(seeds_[i*SEEDS_BLOCK+1], 2,25, 4,4294967288u);
    taus_step(seeds_[i*SEEDS_BLOCK+2], 3,11,17,4294967280u);
    LCG_step(seeds_[i*SEEDS_BLOCK+3],1664525u,1013904223u);
    return 2.328306436538696e-10*(Float)(seeds_[i*SEEDS_BLOCK]^
					 seeds_[i*SEEDS_BLOCK+1]^
					 seeds_[i*SEEDS_BLOCK+2]^
					 seeds_[i*SEEDS_BLOCK+3]
					 );
  }

  static taus_array *generator()  {return generator_;}
  static void init(int n, long int seed) {
    generator_= new taus_array(n);
    generator_->gen_seeds(seed);
  }
 


 private:

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


