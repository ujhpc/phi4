#pragma once

#ifndef DIM
#define DIM 2
#endif
#ifndef FLOAT
#define FLOAT double
#endif

typedef FLOAT Float;

// SIMD ///////////////////////////////////////////////////////////////////////

#ifdef SIMD

#ifndef OLD
#include "simd2.h"
#else
#include "simd.h"
#define itype mask
#endif

#if SIMD <= 1
typedef Float FVec;
typedef simd_integral_of<Float>::type IVec;
#else
// some integer ops like shift are not available on AVX1
#if !__AVX2__&& SIMD > 4
#error SIMD=8 requires at least AVX2
#endif
typedef simd<FLOAT, SIMD> FVec;
typedef FVec::itype IVec;
#endif

#endif

// COMMON HEADERS /////////////////////////////////////////////////////////////

#if defined(SIMD) || defined(FAST_INDEXER)
#include "fast_indexer.h"
typedef Indexer<DIM, IVec> Ind;
#else
#include "indexer.h"
typedef Indexer<DIM> Ind;
#endif

#include "field.h"
#include "partition.h"

// FIELD TYPE /////////////////////////////////////////////////////////////////

#ifdef SIMD
class Storage : public std::vector<Float> {
 public:
  Storage(int size) : std::vector<Float>(size) {}
  Float at(const int i) const { return (*this)[i]; }
  FVec at(const IVec& i) const { return FVec(this->data(), i); }
};
#else
typedef std::vector<Float> Storage;
#endif

#if NCOMP <= 1
typedef ScalarField<Float, Ind, Storage> Field;
#else
typedef VectorField<Float, NCOMP, Ind, Storage> Field;
#endif

// PARTITIONING ///////////////////////////////////////////////////////////////

#if _OPENMP || SIMD > 1
typedef cell_partition<DIM, octal_cell<DIM>, Ind> Partition;
#else
typedef single_partition<Ind> Partition;
#endif

// CACHE BLOCK ////////////////////////////////////////////////////////////////

#ifdef CACHE
#include "cache_block.h"
typedef Block<DIM, Ind, octal_cell<DIM> > BlockType;
#endif

// RANDOM GENERATOR ///////////////////////////////////////////////////////////

#ifdef SIMD
#include "random_simd.h"
#else
#include "random.h"
#endif

#if !defined(SIMD) && defined(USE_RAND48)

inline Float RAND(int i) { return rand48_array::generator()->rand(i); }
inline Float RAND_SYM(int i) {
  return rand48_array::generator()->rand(i) - Float(0.5);
}
typedef rand48_array rand_array_t;

#elif defined(SIMD)

inline FVec RAND(int i) { return taus_array::generator()->rand(i); }
inline FVec RAND_SYM(int i) { return taus_array::generator()->rand_sym(i); }
typedef taus_array rand_array_t;

#else

inline Float RAND(int i) { return taus_array::generator()->rand(i); }
inline Float RAND_SYM(int i) {
  return taus_array::generator()->rand(i) - Float(0.5);
}
typedef taus_array rand_array_t;

#endif
