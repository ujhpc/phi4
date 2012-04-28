#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#define Dim 2
#define FLOAT double

typedef Indexer<Dim> Ind;

typedef ScalarFieldAccessor<double,Dim> SFA;
typedef  Field<FLOAT &, Ind, SFA> ScalarField;
typedef  cell_partition<Dim,octal_cell<2>,Ind> Partition;
//typedef  single_partition<Ind> Partition;

#endif
