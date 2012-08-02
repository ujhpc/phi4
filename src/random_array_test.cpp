#include<cmath>
#include<gtest/gtest.h>


#include"typedefs.h"

class rand48_arrayTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    r_array001=new rand48_array(1);
    r_array001->gen_seeds(1213);

    r_array002=new rand48_array(2);
    r_array002->gen_seeds(1213);
  }

  
  virtual void TearDown() {
    delete r_array001;
    delete r_array002;
    
  }
  rand48_array *r_array001;
  rand48_array *r_array002;
 

};


void 
test_gen(rand48_array *gen,int k,int n) {
  Float tol=5.0*sqrt((1.0/12.0)/n);

  Float mean=0;

  for(int i=0;i<n;++i) {
    Float value=gen->rand(k);
    ASSERT_LT(value,1.0);
    ASSERT_GE(value,0.0);
    mean+=value;

  }

  mean/=n;
  ASSERT_NEAR(mean,0.5,tol);

}

TEST_F(rand48_arrayTest,one_generator) {

  test_gen(r_array001,0,10000);
  
}


TEST_F(rand48_arrayTest,two_generators) {

  
 

  test_gen(r_array002,0,10000);
  test_gen(r_array002,1,10000);
  
 
}

TEST_F(rand48_arrayTest,two_generators_eq) {
 
  unsigned short seeds1[3];
  unsigned short seeds2[6];
  
  r_array001->get_seeds(seeds1);
  r_array002->get_seeds(seeds2);
  
  ASSERT_EQ(seeds1[0],seeds2[0]);
  ASSERT_EQ(seeds1[1],seeds2[1]);
  ASSERT_EQ(seeds1[2],seeds2[2]);



  ASSERT_DOUBLE_EQ(erand48(seeds1),erand48(seeds2));

  int n=10000;
  for(int i=0;i<n;++i) {


    Float value1=r_array001->rand(0);
    Float value2=r_array002->rand(0);
    Float value3=r_array002->rand(1);

    ASSERT_DOUBLE_EQ(value1,value2)<<i;
    ASSERT_NE(value2,value3)<<i;
    

  }

}

TEST_F(rand48_arrayTest,save_restore_test) {

  FILE *fout=fopen("test.rng","w");
  test_gen(r_array002,0,517);
  test_gen(r_array002,1,1256);
  
  r_array002->fwrite_state(fout);
  double bval0=r_array002->rand(0);
  double bval1=r_array002->rand(1);
  fclose(fout);
  
  test_gen(r_array002,0,717);
  test_gen(r_array002,1,926);

  FILE *fin=fopen("test.rng","r");
  r_array002->fread_state(fin);
  fclose(fin);
  
  double eval0=r_array002->rand(0);
  double eval1=r_array002->rand(1);


  ASSERT_DOUBLE_EQ(bval0,eval0);
  ASSERT_DOUBLE_EQ(bval1,eval1);
  
  
}


TEST_F(rand48_arrayTest,save_restore_n_test) {

  const int n=128;

  rand48_array r_array(n) ;
  

  for(int i =0;i<n;++i) {
    test_gen(&r_array,i,i*13+17);
  };
  
  FILE *fout=fopen("test.rng","w");
  r_array.fwrite_state(fout);

  std::vector<Float> check1(n);
  for(int i =0;i<n;++i) {
    check1[i]=r_array.rand(i);
  };

  fclose(fout);
  
  for(int i =0;i<n;++i) {
    test_gen(&r_array,i,(n-i)*7+213);
  };

  FILE *fin=fopen("test.rng","r");
  r_array.fread_state(fin);
  fclose(fin);
  
  
  for(int i =0;i<n;++i) {
    ASSERT_EQ(check1[i],r_array.rand(i));
  };
  
  
  
}

TEST_F(rand48_arrayTest,save_restore_two_rng_test) {

  FILE *fout=fopen("test.rng","w");
  test_gen(r_array002,0,517);
  test_gen(r_array002,1,1256);
  
  r_array002->fwrite_state(fout);
  double bval0=r_array002->rand(0);
  double bval1=r_array002->rand(1);
  fclose(fout);
  
  
  rand48_array array002(2);
  FILE *fin=fopen("test.rng","r");
  array002.fread_state(fin);
  fclose(fin);
  
  double eval0=array002.rand(0);
  double eval1=array002.rand(1);


  ASSERT_DOUBLE_EQ(bval0,eval0);
  ASSERT_DOUBLE_EQ(bval1,eval1);
  
  
}




int 
main(int argc,char *argv[]) {
::testing::InitGoogleTest(&argc,argv);

  return RUN_ALL_TESTS();
  
}
