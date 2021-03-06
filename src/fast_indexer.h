#pragma once

#include "iterator.h"

template <int Dim, typename Site = int> class Indexer {
 public:
  enum {
    D = Dim
  };
  typedef Site coordinate_t[Dim];
  typedef Site site_t;

  typedef LatticeIterator<Dim> iterator;

  static void init(const int dim[Dim]) {
    vols_[0] = 1;
    for (int i = 0; i < Dim; i++) {
      dims_[i] = dim[i];
      vols_[i + 1] = dims_[i] * vols_[i];
    }
    n_links_ = vols_[Dim] * Dim;
  }

  static void clean() {}

  static site_t site(const coordinate_t coord) {
    site_t s = 0;
    for (int i = 0; i < Dim; i++)
      s += vols_[i] * coord[i];
    return s;
  }

  static int scalar_site(const int coord[Dim]) {
    int s = 0;
    for (int i = 0; i < Dim; i++)
      s += vols_[i] * coord[i];
    return s;
  }

  static int site_safe(const int coord[Dim]) {
    int s[Dim];
    for (int i = 0; i < Dim; ++i) {
      int dim = dims_[i];
      s[i] = coord[i];
      if (s[i] >= dim)
        s[i] -= dim;
      else if (s[i] < 0)
        s[i] += dim;
    }
    return scalar_site(s);
  }

  static int dim(int i) { return dims_[i]; }
  static int dim() { return Dim; }
  static int vol(int i) { return vols_[i]; }
  static int n_sites() { return vol(Dim); }
  static int n_links() { return n_links_; }

  static LatticeIterator<Dim> begin() { return LatticeIterator<Dim>(dims_); }
  static LatticeIterator<Dim> end() { return LatticeIterator<Dim>(); }

  static site_t up(site_t site, int dir) {
    site_t mask = vols_[dir + 1] - 1;
    return (site & ~mask) | ((site + vols_[dir]) & mask);
  }

  static site_t dn(site_t site, int dir) {
    site_t mask = vols_[dir + 1] - 1;
    return (site & ~mask) | ((site - vols_[dir]) & mask);
  }

 private:
  static int n_links_;
  static int dims_[Dim];
  static int vols_[Dim + 1];
};

template <int Dim, typename Site> int Indexer<Dim, Site>::n_links_;
template <int Dim, typename Site> int Indexer<Dim, Site>::dims_[Dim];
template <int Dim, typename Site> int Indexer<Dim, Site>::vols_[Dim + 1];
