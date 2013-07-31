#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifndef DIM
#define DIM 2
#endif
#ifndef FLOAT
#define FLOAT double
#endif

typedef FLOAT Float;

#include "simd.h"

#if SIMD <= 1
typedef Float FVec;
typedef simd<FLOAT, 4>::mask_scalar IVec;
typedef FVec SFVec;
typedef IVec SIVec;
#define SSIMD 1
#else
// some integer ops like shift are not available on AVX1
#if !__AVX2__&& SIMD > 4
#define SSIMD 4
#else
#define SSIMD SIMD
#endif

typedef simd<FLOAT, SIMD> FVec;
typedef simd<FVec::mask_scalar, SIMD> IVec;
typedef simd<FLOAT, SSIMD> SFVec;
typedef simd<FVec::mask_scalar, SSIMD> SIVec;
#endif

#include "random.h"
#include "indexer.h"
#include "field.h"
#include "partition.h"

typedef Indexer<DIM> Ind;
#if SCALAR
typedef ScalarField<ScalarFieldArray<Float>, Ind> Field;
#else
typedef VectorField<VectorFieldArray<Float, NCOMP>, Ind> Field;
#endif

#define CELL_PARTITION 1
#if CELL_PARTITION
typedef cell_partition<DIM, octal_cell<DIM>, Ind> Partition;
#else
typedef single_partition<Ind> Partition;
#endif

#if USE_RAND48
inline Float RAND(int i) { return rand48_array::generator()->rand(i); }
inline Float RAND_SYM(int i) {
  return rand48_array::generator()->rand(i) - (Float)0.5;
}
typedef rand48_array rand_array_t;
#else
inline FVec RAND(int i) { return taus_array::generator()->rand(i); }
inline FVec RAND_SYM(int i) { return taus_array::generator()->rand_sym(i); }
typedef taus_array rand_array_t;
#endif

#define REPn(n) REP##n
#define REP(n, f, ...) REPn(n)(f, __VA_ARGS__)
#define REP1(f, ...) f(0, __VA_ARGS__)
#define REP2(f, ...) f(0, __VA_ARGS__), f(1, __VA_ARGS__)
#define REP4(f, ...) \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__)
#define REP8(f, ...)                                                          \
  f(0, __VA_ARGS__), f(1, __VA_ARGS__), f(2, __VA_ARGS__), f(3, __VA_ARGS__), \
      f(4, __VA_ARGS__), f(5, __VA_ARGS__), f(6, __VA_ARGS__),                \
      f(7, __VA_ARGS__)

#endif
