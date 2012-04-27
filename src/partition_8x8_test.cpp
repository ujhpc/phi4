#include<gtest/gtest.h>


#include"indexer.h"
#include"partition.h"

typedef Indexer<2> Ind2; 
typedef  cell_partition<2,octal_cell<2>, Indexer<2> > partition_t;

class partition8x8Test : public ::testing::Test {

  
protected:



  void SetUp() {
    int dims[2]={8,8};
    Indexer<2>::init(dims);
    partition=new partition_t;
    partition->gen_partitions();
  }
  
  void TearDown() {};

 partition_t *partition;

};

/*

  6 7 4 5 6 7 4 5
  2 3 0 1 2 3 0 1
  4 5 6 7 4 5 6 7
  0 1 2 3 0 1 2 3
  6 7 4 5 6 7 4 5
  2 3 0 1 2 3 0 1
  4 5 6 7 4 5 6 7
  0 1 2 3 0 1 2 3

*/

TEST_F(partition8x8Test,two_dim_8x8_test) {

  EXPECT_EQ(0 ,partition->partition(0,0));
  EXPECT_EQ(4 ,partition->partition(0,1));
  EXPECT_EQ(32,partition->partition(0,2));
  EXPECT_EQ(36,partition->partition(0,3));
  EXPECT_EQ(18,partition->partition(0,4));
  EXPECT_EQ(22,partition->partition(0,5));
  EXPECT_EQ(50,partition->partition(0,6));
  EXPECT_EQ(54,partition->partition(0,7));
  
  EXPECT_EQ(1 ,partition->partition(1,0));
  EXPECT_EQ(5 ,partition->partition(1,1));
  EXPECT_EQ(33,partition->partition(1,2));
  EXPECT_EQ(37,partition->partition(1,3));
  EXPECT_EQ(19,partition->partition(1,4));
  EXPECT_EQ(23,partition->partition(1,5));
  EXPECT_EQ(51,partition->partition(1,6));
  EXPECT_EQ(55,partition->partition(1,7));

   
}
