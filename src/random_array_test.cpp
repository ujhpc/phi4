#include"gtest.h"

#include"random.h"

class rand48_arrayTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    r_array001=new rand48_array(1);
    r_array001->gen_seeds(1213);
  }

  rand48_array *r_array001;
  rand48_array *r_array002;
  rand48_array *r_array004;
  rand48_array *r_array128;

};


TEST_F(rand48_arrayTest,one_generator) {
  
  for(int i=0;i<10000;++i) {
    r_array001->next(0);
    double value=r_array001->value(0);
    ASSERT_LT(value,1.0);
    ASSERT_GE(value,0.0);
  }
}


int 
main(int argc,char *argv[]) {
::testing::InitGoogleTest(&argc,argv);

  return RUN_ALL_TESTS();
  
}
