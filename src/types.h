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

#ifndef LEGACY
#include "simd.h"
#else  // legacy code support
#define __SIMD_H__REP
#include "legacy/simd.h"
#define itype mask
#define __simd_rep REP
#endif

#if SIMD <= 1

typedef Float FVec;
typedef simd_integral_of<Float>::type IVec;

#ifdef LEGACY  // legacy code support
typedef FVec SFVec;
typedef IVec SIVec;
#define SSIMD 1
#endif

#else

// some integer ops like shift are not available on AVX1
#if !__AVX2__&& SIMD > 4
#ifndef LEGACY
#error SIMD=8 requires at least AVX2
#else
#define SSIMD 4
#endif
#elif defined LEGACY
#define SSIMD SIMD
#endif
typedef simd<FLOAT, SIMD> FVec;
typedef FVec::itype IVec;
#ifdef LEGACY  // legacy code support
typedef simd<FLOAT, SSIMD> SFVec;
typedef SFVec::itype SIVec;
#endif

#endif

#endif

// COMMON HEADERS /////////////////////////////////////////////////////////////

#if (defined SIMD && !defined LEGACY) || defined FAST_INDEXER
#include "fast_indexer.h"
#ifndef LEGACY
typedef Indexer<DIM, IVec> Ind;
#else  // legacy code supprt
typedef Indexer<DIM, int> Ind;
#endif
#else
#include "indexer.h"
typedef ::Indexer<DIM> Ind;
#endif

#include "field.h"
#include "partition.h"

// FIELD TYPE /////////////////////////////////////////////////////////////////

#if defined SIMD && !defined LEGACY
class Storage {
 public:
  Storage(int size) { ptr = new Float[size]; }
  ~Storage() { delete[] ptr; }
  Float at(const int i) const { return ptr[i]; }
  void assign(const int i, const Float v) { ptr[i] = v; }
  FVec at(const IVec& i) const { return FVec(ptr, i); }

 private:
  Float* ptr;
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
#ifndef LEGACY
#include "random_simd.h"
#else  // legacy code support
#include "legacy/random_simd.h"
#endif
#else
#include "random.h"
#endif

#if !defined SIMD&& defined USE_RAND48

inline Float RAND(int i) { return rand48_array::generator()->rand(i); }
inline Float RAND_SYM(int i) {
  return rand48_array::generator()->rand(i) - Float(0.5);
}
typedef rand48_array rand_array_t;

#elif defined SIMD

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
