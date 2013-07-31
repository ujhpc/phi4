#pragma once

template <int D> class LatticeIterator {
 public:

  LatticeIterator(const int* dims, const int* coords) {
    for (int i = 0; i < D; ++i) {
      dims_[i] = dims[i];
      coords_[i] = coords[i];
    }
    coords_[D] = 0;
  }

  LatticeIterator(const int* dims) {
    for (int i = 0; i < D; ++i) {
      dims_[i] = dims[i];
      coords_[i] = 0;
    }
    coords_[D] = 0;
  }

  LatticeIterator() {
    for (int i = 0; i < D; ++i) {
      coords_[i] = 0;
    }
    coords_[D] = 1;
  }
  ;

  bool operator==(const LatticeIterator& rhs) {
    for (int i = 0; i <= D; ++i) {
      if (coords_[i] != rhs.coords_[i])
        return false;
    }
    return true;
  }

  bool operator!=(const LatticeIterator& rhs) { return !operator==(rhs); }

  LatticeIterator& operator++() {

    inc();
    return *this;
  }

  LatticeIterator operator++(int) {
    LatticeIterator it(*this);
    inc();
    return it;
  }

  int* operator*() { return &coords_[0]; }

 protected:
  int dims_[D];
  int coords_[D + 1];

  void inc() {
    coords_[0]++;
    int i = 0;
    while (i < D && coords_[i] >= dims_[i]) {
      coords_[i] -= dims_[i];
      i++;
      coords_[i]++;
    }
  }
};
