#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#ifndef DIM
  #define DIM 2
#endif
#ifndef FLOAT
  #define FLOAT double
#endif

typedef FLOAT Float;

#include "random.h"
#include "indexer.h"
#include "field.h"
#include "partition.h"

typedef Indexer<DIM> Ind;
#if NCOMP <= 1
  typedef ScalarField<ScalarFieldArray<Float>, Ind> Field;
#else
  typedef VectorField<VectorFieldArray<Float, NCOMP>, Ind> Field;
#endif

#if _OPENMP || SIMD > 1
  typedef cell_partition<DIM, octal_cell<DIM>, Ind> Partition;
#else
  typedef single_partition<Ind> Partition;
#endif

#if USE_RAND48
  inline Float RAND(int i) { return rand48_array::generator()->rand(i); }
  typedef rand48_array rand_array_t;
#else
  inline Float RAND(int i) { return taus_array::generator()->rand(i); }
  typedef taus_array rand_array_t;
#endif

#endif
