#ifndef __PARTITION_H__
#define __PARTITION_H__

#include"iterator.h"

template<int D> class octal_cell;

template<> class octal_cell<2> {
public:
  enum {D=2};
  enum {n_start_points=8};
  enum {n_points=2};

  octal_cell() {
    int sp[n_start_points][D]={{0,0},{1,0},{2,0},{3,0},
			       {0,1},{1,1},{2,1},{3,1}};

    int cp[n_points][D]={{0,0},{2,2}};
    
    for(int p=0;p<n_start_points;++p) 
      for(int i=0;i<D;++i)
	start_points[p][i]=sp[p][i];

    
    for(int p=0;p<n_points;++p) 
      for(int i=0;i<D;++i)
	cell_points[p][i]=cp[p][i];
    
    cell_dims[0]=4;
    cell_dims[1]=4;
  }

protected:
  int cell_dims[D];
  int start_points[n_start_points][D];
  int cell_points[n_points][D];
};




template<int D, typename Cell,typename indexer_t> 
class cell_partition : public Cell {
public:
  cell_partition():partitions_(new int[indexer_t::n_sites()]) {
    for(int i=0;i<D;++i)
      grid_dims_[i]=indexer_t::dim(i)/Cell::cell_dims[i];

    partition_size_=indexer_t::n_sites()/Cell::n_start_points;

  }

  void   gen_partitions() {
    

    for(int sp=0;sp<Cell::n_start_points;++sp) {
      int index=0;
      
      for(int cp=0;cp<Cell::n_points;++cp) {
	LatticeIterator<D> grid(grid_dims_);
	LatticeIterator<D> grid_end;


	while(grid!=grid_end) {
	  int coords[Cell::D];
	  for(int i=0;i<Cell::D;++i) {
	    coords[i] = Cell::cell_dims[i]*(*grid)[i];
	    coords[i]+= Cell::start_points[sp][i]+Cell::cell_points[cp][i];
	  }
	  int site=indexer_t::site_safe(coords);
	  partitions_[sp*partition_size_ + index]=site;
	  ++index;
	  ++grid;
	}
      }
    }
  }

  int partition(int i, int j) {
    return partitions_[i*partition_size_ +j];
  }

private:
  int partition_size_;
  int grid_dims_[D];
  int *partitions_;
      
};

#endif

