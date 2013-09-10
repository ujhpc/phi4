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

#define __SIMD_H__REP
#include "simd2.h"

#if SIMD <= 1
typedef Float FVec;
typedef simd_integral_of<Float>::type IVec;
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
typedef FVec::itype IVec;
typedef simd<FLOAT, SSIMD> SFVec;
typedef SFVec::itype SIVec;
#endif

#endif

// COMMON HEADERS /////////////////////////////////////////////////////////////

#ifdef SIMD
#include "random_simd.h"
#else
#include "random.h"
#endif
#ifdef FAST_INDEXER
#include "fast_indexer.h"
#else
#include "indexer.h"
#endif
#include "field.h"
#include "partition.h"

// FIELD TYPE /////////////////////////////////////////////////////////////////

typedef Indexer<DIM> Ind;
#if NCOMP <= 1
typedef ScalarField<ScalarFieldArray<Float>, Ind> Field;
#else
typedef VectorField<VectorFieldArray<Float, NCOMP>, Ind> Field;
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
