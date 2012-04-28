#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include"random.h"

#define Dim 2
#define FLOAT double

typedef Indexer<Dim> Ind;

typedef ScalarFieldAccessor<double,Dim> SFA;
typedef  Field<FLOAT &, Ind, SFA> ScalarField;
typedef  cell_partition<Dim,octal_cell<2>,Ind> Partition;
//typedef  single_partition<Ind> Partition;

inline double RAND(int i) {return rand48_array::generator()->rand(i);}
//inline double RAND(int i) {return drand48();}

#endif
