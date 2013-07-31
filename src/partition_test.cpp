#include <gtest/gtest.h>

#include "indexer.h"
#include "partition.h"

typedef Indexer<2> Ind2;
typedef cell_partition<2, octal_cell<2>, Indexer<2> > partition_t;

class partitionTest : public ::testing::Test {

 protected:

  void SetUp() {
    int dims[2] = { 4, 4 };
    Indexer<2>::init(dims);
    partition = new partition_t;
    //    partition->gen_partitions();
  }

  void TearDown() {};

  partition_t* partition;
};

/*

  6 7 4 5
  2 3 0 1
  4 5 6 7
  0 1 2 3
*/

TEST_F(partitionTest, two_dim_4x4_test) {

  EXPECT_EQ(0, partition->partition(0, 0));
  EXPECT_EQ(10, partition->partition(0, 1));

  EXPECT_EQ(1, partition->partition(1, 0));
  EXPECT_EQ(11, partition->partition(1, 1));

  EXPECT_EQ(2, partition->partition(2, 0));
  EXPECT_EQ(8, partition->partition(2, 1));

  EXPECT_EQ(3, partition->partition(3, 0));
  EXPECT_EQ(9, partition->partition(3, 1));

  EXPECT_EQ(4, partition->partition(4, 0));
  EXPECT_EQ(14, partition->partition(4, 1));

  EXPECT_EQ(5, partition->partition(5, 0));
  EXPECT_EQ(15, partition->partition(5, 1));

  EXPECT_EQ(6, partition->partition(6, 0));
  EXPECT_EQ(12, partition->partition(6, 1));

  EXPECT_EQ(7, partition->partition(7, 0));
  EXPECT_EQ(13, partition->partition(7, 1));
}
