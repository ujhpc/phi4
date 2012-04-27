#include<cmath>

#include"gtest.h"

#include"random.h"

class rand48_arrayTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    r_array001=new rand48_array(1);
    r_array001->gen_seeds(1213);

    r_array002=new rand48_array(2);
    r_array002->gen_seeds(1213);
  }

  rand48_array *r_array001;
  rand48_array *r_array002;
  rand48_array *r_array004;
  rand48_array *r_array128;

};


void 
test_gen(rand48_array *gen,int k,int n) {
  double tol=5.0*sqrt((1.0/12.0)/n);

  double mean=0;

  for(int i=0;i<n;++i) {
    double value=gen->rand(k);
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


    double value1=r_array001->rand(0);
    double value2=r_array002->rand(0);
    double value3=r_array002->rand(1);

    ASSERT_DOUBLE_EQ(value1,value2)<<i;
    ASSERT_NE(value2,value3)<<i;
    

  }

}


int 
main(int argc,char *argv[]) {
::testing::InitGoogleTest(&argc,argv);

  return RUN_ALL_TESTS();
  
}
