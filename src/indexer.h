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

    init_arrays();
  }

  static void clean() {
    delete[] up_;
    delete[] dn_;
  }

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

  static site_t index(site_t site, int dir) { return dir * vols_[Dim] + site; }

  static site_t index(const coordinate_t coord, int dir) {
    return dir * vols_[Dim] + site(coord);
  }

  static int dim(int i) { return dims_[i]; }
  static int dim() { return Dim; }
  static int vol(int i) { return vols_[i]; }
  static int n_sites() { return vol(Dim); }
  static int n_links() { return n_links_; }

  static LatticeIterator<Dim> begin() { return LatticeIterator<Dim>(dims_); }
  static LatticeIterator<Dim> end() { return LatticeIterator<Dim>(); }

  static site_t up(site_t site, int dir) { return up_[index(site, dir)]; }
  static site_t dn(site_t site, int dir) { return dn_[index(site, dir)]; }

 private:
  static void init_arrays() {
    up_ = new site_t[n_links_];
    dn_ = new site_t[n_links_];

    for (iterator it = begin(); it != end(); ++it) {
      for (int mu = 0; mu < Dim; ++mu) {
        int coord[Dim];

        for (int i = 0; i < Dim; ++i) {
          coord[i] = (*it)[i];
        }

        coord[mu] = (*it)[mu] + 1;
        if (coord[mu] >= dim(mu))
          coord[mu] -= dim(mu);

        up_[index(*it, mu)] = site(coord);

        coord[mu] = (*it)[mu] - 1;
        if (coord[mu] < 0)
          coord[mu] += dims_[mu];

        dn_[index(*it, mu)] = site(coord);
      }
    }
  }

  static site_t n_links_;

  static site_t dims_[Dim];
  static site_t vols_[Dim + 1];

  static site_t* up_;
  static site_t* dn_;
};

template <int Dim, typename Site> int Indexer<Dim, Site>::n_links_;
template <int Dim, typename Site> int Indexer<Dim, Site>::dims_[Dim];
template <int Dim, typename Site> int Indexer<Dim, Site>::vols_[Dim + 1];

template <int Dim, typename Site>
typename Indexer<Dim, Site>::site_t* Indexer<Dim, Site>::up_;
template <int Dim, typename Site>
typename Indexer<Dim, Site>::site_t* Indexer<Dim, Site>::dn_;
