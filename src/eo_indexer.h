#pragma once

#include <cstdlib>

#include "iterator.h"

template <int Dim> class EOIndexer {

 public:

  typedef LatticeIterator<Dim> iterator;

  enum {
    D = Dim
  };

  EOIndexer(const int* dims) {
    init(dims);
    init_arrays();
  }

  EOIndexer(int lx, int ly) {
    int dims[Dim];
    if (Dim != 2)
      throw("picked a wrong constructor");

    dims[0] = lx;
    dims[1] = ly;

    init(dims);
    init_arrays();
  }

  EOIndexer(int lx, int ly, int lz) {
    int dims[Dim];
    if (Dim != 3)
      throw("picked a wrong constructor");

    dims[0] = lx;
    dims[1] = ly;
    dims[2] = lz;

    init(dims);
    init_arrays();
  }

  EOIndexer(int lx, int ly, int lz, int lt) {
    int dims[Dim];
    if (Dim != 4)
      throw("picked a wrong constructor");

    dims[0] = lx;
    dims[1] = ly;
    dims[2] = lz;
    dims[3] = lt;

    init(dims);
    init_arrays();
  }

  int dim(int i) const { return dims_[i]; }
  int dim() const { return Dim; }
  const int* dims() const { return &dims_[0]; }
  int vol(int i) const { return vols_[i]; }
  int n_sizeh() const { return n_sizeh_; }
  int n_sites() const { return vol(Dim); }
  int n_links() const { return n_links_; }

  inline int site(const int siteh, const int par) const {
    return (siteh + par * n_sizeh_);
  }

  int site_cartesian(const int* coords) {
    int s = coords[0];

    for (int i = 1; i < Dim; ++i) {
      s += coords[i] * vols_[i];
    }
    return s;
  }

  int site(const int* coords) {
    int s = coords[0];
    int par = s;
    for (int i = 1; i < Dim; ++i) {
      s += coords[i] * vols_[i];
      par += coords[i];
    }

    return site(s >> 0x1, par & 0x1);
  }

  int site_eo(const int* coords) {
    int s = coords[0];
    for (int i = 1; i < Dim; ++i) {
      s += coords[i] * vols_[i];
    }
    return s >> 0x1;
  }

  void de_site(int siteh, int eo, int coords[Dim]) {

    int site = 2 * siteh;
    int sum = 0;

    for (int i = Dim - 1; i > 0; i--) {
      coords[i] = site / vol(i);
      site = site % vol(i);
      sum += coords[i];
    }

    coords[0] = site;
    sum += coords[0];

    bool par = 0x1 & sum;
    if (par != eo)
      coords[0]++;
  }

  void de_site(int site_a, int coords[Dim]) {

    int eo;
    int site;
    if (site_a > n_sizeh()) {
      eo = 1;
      site = (site_a - n_sizeh());
    } else {
      eo = 0;
      site = site_a;
    }

    de_site(site, eo, coords);
  }

  int index(const int site, const int mu) const {
    return (site + mu * vols_[Dim]);
  }

  int index(const int* coords, int mu) { return index(site(coords), mu); }

  int index_eo(const int siteh, const int par, const int mu) const {
    return index(site(siteh, par), mu);
  }

  template <int par> int index_eo(const int siteh, const int mu) {
    return index(site(siteh, par), mu);
  }

  LatticeIterator<Dim> begin() { return LatticeIterator<Dim>(dims_); }
  LatticeIterator<Dim> end() { return LatticeIterator<Dim>(); }

  int site_up(const int site, const int mu) const {
    return site_up_[index(site, mu)];
  }

  int site_dn(const int site, const int mu) const {
    return site_dn_[index(site, mu)];
  }

  int site_up_eo(const int site, const int par, const int mu) const {
    return site_up_eo_[index_eo(site, par, mu)];
  }

  int site_dn_eo(const int site, const int par, const int mu) const {
    return site_dn_eo_[index_eo(site, par, mu)];
  }

  int site_mv(const int site, const int mu) const {
    int dir = mu % Dim;
    if (mu < Dim)
      return site_up(site, dir);
    else
      return site_dn(site, dir);
  }

  ~EOIndexer() {
    delete[] site_up_;
    delete[] site_dn_;
    delete[] site_up_eo_;
    delete[] site_dn_eo_;
  }

 private:

  void init(const int* dims) {
    vols_[0] = 1;
    for (int i = 0; i < Dim; ++i) {
      dims_[i] = dims[i];
      vols_[i + 1] = vols_[i] * dims_[i];
    }
    n_sizeh_ = vols_[Dim] / 2;
    n_links_ = vols_[Dim] * Dim;
  }

  void init_arrays() {
    site_up_ = new int[n_links_];
    site_dn_ = new int[n_links_];
    site_up_eo_ = new int[n_links_];
    site_dn_eo_ = new int[n_links_];

    for (iterator it = begin(); it != end(); ++it) {

      for (int mu = 0; mu < Dim; ++mu) {

        int coord[Dim];
        for (int i = 0; i < Dim; ++i)
          coord[i] = (*it)[i];

        coord[mu] = (*it)[mu] + 1;
        if (coord[mu] >= dims_[mu])
          coord[mu] -= dims_[mu];

        site_up_[index(*it, mu)] = site(coord);
        site_up_eo_[index(*it, mu)] = site_eo(coord);

        coord[mu] = (*it)[mu] - 1;
        if (coord[mu] < 0)
          coord[mu] += dims_[mu];

        site_dn_[index(*it, mu)] = site(coord);
        site_dn_eo_[index(*it, mu)] = site_eo(coord);
      }
    }
  }

  int* site_up_;
  int* site_dn_;
  int* site_up_eo_;
  int* site_dn_eo_;

  int dims_[Dim];
  int vols_[Dim + 1];
  int n_sizeh_;
  int n_links_;
};
