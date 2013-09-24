#pragma once

#include "iterator.h"

template <int Dim, typename Site = int, typename Direction = int>
class Indexer {
 public:
  enum {
    D = Dim
  };
  typedef Site coordinate_t[Dim];
  typedef Site site_t;
  typedef Direction direction_t;

  typedef LatticeIterator<Dim> iterator;

  static void init(const coordinate_t dim) {
    vols_[0] = 1;

    for (int i = 0; i < Dim; i++) {
      dims_[i] = dim[i];
      vols_[i + 1] = dims_[i] * vols_[i];
    }

    n_links_ = vols_[Dim] * Dim;
  }

  static void clean() {}

  static site_t site(const coordinate_t coord) {
    int s = 0;

    for (int i = 0; i < Dim; i++)
      s += vol(i) * coord[i];

    return s;
  }

  static site_t site_safe(const coordinate_t coord) {
    int c[Dim];
    for (int i = 0; i < Dim; ++i) {
      c[i] = coord[i];
      if (c[i] >= dim(i))
        c[i] -= dim(i);
      if (c[i] < 0)
        c[i] += dim(i);
    }
    return site(c);
  }

  static int dim(int i) { return dims_[i]; }
  static int dim() { return Dim; }
  static int vol(int i) { return vols_[i]; }
  static int n_sites() { return vol(Dim); }
  static int n_links() { return n_links_; }

  static LatticeIterator<Dim> begin() { return LatticeIterator<Dim>(dims_); }
  static LatticeIterator<Dim> end() { return LatticeIterator<Dim>(); }

  static site_t up(site_t site, direction_t dir) {
    site_t mask = vols_[dir + 1] - 1;
    return (site & ~mask) | ((site + vols_[dir]) & mask);
  }

  static site_t dn(site_t site, direction_t dir) {
    site_t mask = vols_[dir + 1] - 1;
    return (site & ~mask) | ((site - vols_[dir]) & mask);
  }

 private:
  static site_t n_links_;

  static site_t dims_[Dim];
  static site_t vols_[Dim + 1];
};

template <int Dim, typename Site, typename Direction>
typename Indexer<Dim, Site, Direction>::site_t
    Indexer<Dim, Site, Direction>::n_links_;

template <int Dim, typename Site, typename Direction>
typename Indexer<Dim, Site, Direction>::site_t
    Indexer<Dim, Site, Direction>::dims_[Dim];

template <int Dim, typename Site, typename Direction>
typename Indexer<Dim, Site, Direction>::site_t
    Indexer<Dim, Site, Direction>::vols_[Dim + 1];
