#pragma once

template <int Dim, typename IndexerType, typename Cell> class Block {
 public:
  Block(int* corner_a) {
    for (int i = 0; i < Dim; ++i)
      corner_[i] = corner_a[i];

    corner_offset_ = IndexerType::safe_site(corner_);
  }

  Block(int corner_offset) : corner_offset_(corner_offset) {};

  static void init(int* dims_a) {
    int grid_dims[Cell::D];
    block_size_ = 1;
    for (int i = 0; i < Dim; ++i) {
      dims_[i] = dims_a[i];
      block_size_ *= dims_[i];
      grid_dims[i] = dims_[i] / cell_.cell_dims[i];
    }
    n_partitions_ = Cell::n_start_points;
    partition_size_ = block_size_ / n_partitions_;
    partitions_ = new int[block_size_];
    for (int sp = 0; sp < Cell::n_start_points; ++sp) {
      int index = 0;
      for (int cp = 0; cp < Cell::n_points; ++cp) {
        LatticeIterator<IndexerType::D> grid(grid_dims);
        LatticeIterator<IndexerType::D> grid_end;
        while (grid != grid_end) {
          int coords[Cell::D];
          for (int i = 0; i < Cell::D; ++i) {
            coords[i] = cell_.cell_dims[i] * (*grid)[i];
            coords[i] += cell_.start_points[sp][i] + cell_.cell_points[cp][i];
            if (coords[i] >= dims_[i])
              coords[i] -= dims_[i];
          }
          int site = IndexerType::site_safe(coords);
          partitions_[sp * partition_size_ + index] = site;
          ++index;
          ++grid;
        }
      }
    }

    n_grids_ = 1 << Dim;
    grid_size_ = IndexerType::n_sites() / (n_grids_ * block_size_);

    grid_ = new int[grid_size_ * n_grids_];

    for (int i = 0; i < Dim; ++i)
      grid_dims[i] = IndexerType::dim(i) / (2 * dims_[i]);
    int start[Dim];
    for (int i = 0; i < Dim; ++i)
      start[i] = 2;

    LatticeIterator<Dim> starting_point(start);
    LatticeIterator<Dim> end;
    int gp = 0;
    while (starting_point != end) {
      int j = 0;
      LatticeIterator<Dim> grid(grid_dims);
      while (grid != end) {

        int coord[Dim];
        for (int i = 0; i < Dim; ++i) {
          coord[i] = ((*starting_point)[i] + (*grid)[i] * 2) * dims_[i];
        }
        grid_[gp * grid_size_ + j] = IndexerType::site_safe(coord);
        //	std::cerr<<"gp j "<<gp<<" "<<j<<"
        //"<<grid_[gp*grid_size_+j]<<std::endl;
        ++j;
        ++grid;
      }
      ++gp;
      // std::cerr<<"gp "<<gp<<std::endl;
      ++starting_point;
    }
  }

  static int n_partitions() { return n_partitions_; }
  static int partition_size() { return partition_size_; }

  // returns the j-th site of the i-th partition
  static int partition(int i, int j) {
    return partitions_[i * partition_size_ + j];
  }

  int corner_offset() const { return corner_offset_; }

  static int n_grids() { return n_grids_; }
  static int grid_size() { return grid_size_; }
  static int grid(int i, int j) { return grid_[i * grid_size_ + j]; }

 private:
  int corner_[Dim];
  int corner_offset_;
  static int block_size_;
  static int n_partitions_;
  static int partition_size_;
  static int dims_[Dim];
  static int* partitions_;
  static int n_grids_;
  static int grid_size_;
  static int* grid_;
  static Cell cell_;
};

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::block_size_;

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::n_partitions_;

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::partition_size_;

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::dims_[Dim];

template <int Dim, typename IndexerType, typename Cell>
int* Block<Dim, IndexerType, Cell>::partitions_;

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::n_grids_;

template <int Dim, typename IndexerType, typename Cell>
int Block<Dim, IndexerType, Cell>::grid_size_;

template <int Dim, typename IndexerType, typename Cell>
int* Block<Dim, IndexerType, Cell>::grid_;

template <int Dim, typename IndexerType, typename Cell>
Cell Block<Dim, IndexerType, Cell>::cell_;
