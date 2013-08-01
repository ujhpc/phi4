#include <cmath>
#include <gtest/gtest.h>

#include "typedefs.h"

template <typename R> class rand_array_test : public ::testing::Test {
 protected:

  virtual void SetUp() {
    r_array001 = new R(1);
    r_array001->gen_seeds(1213);

    r_array002 = new R(2);
    r_array002->gen_seeds(1213);
  }

  virtual void TearDown() {
    delete r_array001;
    delete r_array002;
  }
  R* r_array001;
  R* r_array002;
};

TYPED_TEST_CASE_P(rand_array_test);

template <typename R> void test_gen(R* gen, int k, int n) {
  Float tol = 5.0 * sqrt((1.0 / 12.0) / n);

  Float mean = 0;

  for (int i = 0; i < n; ++i) {
    Float value = gen->rand(k);
    ASSERT_LT(value, 1.0);
    ASSERT_GE(value, 0.0);
    mean += value;
  }

  mean /= n;
  ASSERT_NEAR(mean, 0.5, tol);
}

TYPED_TEST_P(rand_array_test, one_generator) {

  test_gen(this->r_array001, 0, 10000);
}

TYPED_TEST_P(rand_array_test, two_generators) {

  test_gen(this->r_array002, 0, 10000);
  test_gen(this->r_array002, 1, 10000);
}

TYPED_TEST_P(rand_array_test, two_generators_eq) {

  const int seeds_per_gen = TypeParam::seeds_per_gen;
  typename TypeParam::seed_t seeds1[seeds_per_gen];
  typename TypeParam::seed_t seeds2[2 * seeds_per_gen];

  this->r_array001->get_seeds(seeds1);
  this->r_array002->get_seeds(seeds2);

  for (int i = 0; i < seeds_per_gen; ++i) {
    ASSERT_EQ(seeds1[i], seeds2[i]);
  }

  int n = 10000;
  for (int i = 0; i < n; ++i) {

    Float value1 = this->r_array001->rand(0);
    Float value2 = this->r_array002->rand(0);
    Float value3 = this->r_array002->rand(1);

    ASSERT_DOUBLE_EQ(value1, value2) << i;
    ASSERT_NE(value2, value3) << i;
  }
}

TYPED_TEST_P(rand_array_test, save_restore_n_test) {

  const int n = 128;

  TypeParam r_array(n);

  for (int i = 0; i < n; ++i) {
    test_gen(&r_array, i, i * 13 + 17);
  }

  FILE* fout = fopen("test.rng", "w");
  r_array.fwrite_state(fout);

  std::vector<Float> check1(n);
  for (int i = 0; i < n; ++i) {
    check1[i] = r_array.rand(i);
  }

  fclose(fout);

  for (int i = 0; i < n; ++i) {
    test_gen(&r_array, i, (n - i) * 7 + 213);
  }

  FILE* fin = fopen("test.rng", "r");
  r_array.fread_state(fin);
  fclose(fin);

  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(check1[i], r_array.rand(i));
  }
}

TYPED_TEST_P(rand_array_test, save_restore_n_two_rng_test) {

  const int n = 128;
  TypeParam r_array1(n);

  for (int i = 0; i < n; ++i) {
    test_gen(&r_array1, i, i * 17 + 13);
  }

  FILE* fout = fopen("test.rng", "w");
  r_array1.fwrite_state(fout);
  fclose(fout);

  TypeParam r_array2(n);
  FILE* fin = fopen("test.rng", "r");
  r_array2.fread_state(fin);
  fclose(fin);

  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(r_array1.rand(i), r_array2.rand(i));
  }
}

REGISTER_TYPED_TEST_CASE_P(rand_array_test,
                           one_generator,
                           two_generators,
                           two_generators_eq,
                           save_restore_n_test,
                           save_restore_n_two_rng_test);

typedef ::testing::Types<rand48_array, taus_array> Tested_Types;
INSTANTIATE_TYPED_TEST_CASE_P(My, rand_array_test, Tested_Types);

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
