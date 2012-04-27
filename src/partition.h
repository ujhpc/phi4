#ifndef __PARTITION_H__
#define __PARTITION_H__


template<int D> class octal_cell;

template<> class octal_cell<2> {
  enum {D=2};
  enum {n_start_points=D};
  enum {n_points=2};
protected:
  int cell_dim[D];
  start_points[n_start_points][D];
  points[n_points][D];
  
};


template<int D, typename Cell> 
class cell_partition : public Cell {
  
  cell_partition(int dims) {
  }

  void   gen_partitions() {
    

  }


  int partition(int i, int j); {
    return partitions_(i*partition_size_ +j);
  }
private:
  int partition_size_;
  int *partitions_;
    
};

#endif

