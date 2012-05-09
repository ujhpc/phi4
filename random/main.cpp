#include<cstdio>
#include<cstdlib>


class taus_array {

public:

  taus_array(int n):n_generators_(n) {
    seeds_ = new unsigned[4*n_generators_];    
    gen_seeds(121245);
  }

inline  double rnd(int i) {
  taus_step(seeds_[i*4],13,19,12,4294967294u);
  taus_step(seeds_[i*4+1], 2,25, 4,4294967288u);
  taus_step(seeds_[i*4+2], 3,11,17,4294967280u);
  LCG_step(seeds_[i*4+3],1664525u,1013904223u);
  return 2.328306436538696e-10*(double)(seeds_[i*4]^
					seeds_[i*4+1]^
					seeds_[i*4+2]^
					seeds_[i*4+3]
					);
  }

  void gen_seeds(long int);
  void set_seeds(const unsigned *seed);
  void get_seeds(unsigned *seed) const;
  
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


};

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





inline  unsigned taus_step(unsigned &z, int S1,int S2,int S3,unsigned M) {
    unsigned b=(((z<<S1)^z)>>S2);
    return z=(((z &M)<<S3)^b);
  }
  
inline  unsigned LCG_step(unsigned &z,unsigned A,unsigned C) {
    return z=(A*z+C);
  }

inline  double rnd(unsigned *z) {
  return 2.328306436538696e-10*(double)(
				  taus_step(z[0],13,19,12,4294967294u)^
				  taus_step(z[1], 2,25, 4,4294967288u)^
				  taus_step(z[2], 3,11,17,4294967280u)^
				  LCG_step(z[3],1664525u,1013904223u) );
    }

inline  double rnd2(unsigned *z) {
  taus_step(z[0],13,19,12,4294967294u);
  taus_step(z[1], 2,25, 4,4294967288u);
  taus_step(z[2], 3,11,17,4294967280u);
  LCG_step(z[3],1664525u,1013904223u);
  return 2.328306436538696e-10*(double)( z[0]^z[1]^z[2]^z[3]);
  
}



main() {

  unsigned z[8]={6544u,9988877u,9888u,1234u,98765u,98766u,87654u,776666u};
  
  double sum1=0.0;
  double sum2=0.0;

  taus_array taus(2);
  taus.gen_seeds(19798987);

  for(int i=0;i<50000000;i++) {
    //    sum+=drand48();
    sum1+=taus.rnd(0);
    sum2+=taus.rnd(1);
    
  }

  fprintf(stdout,"%12.8f\n",sum1+sum2);
}
