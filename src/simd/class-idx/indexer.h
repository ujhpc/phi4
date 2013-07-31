#pragma once

#include <iostream>
#include "iterator.h"

template<int Dim>
class Indexer {
  typedef int coordinate_t;

public:
  typedef LatticeIterator<Dim> iterator;
  enum {D=Dim};

  static coordinate_t site(const coordinate_t *coord) {
    int s=0;

    for(int i=0;i<Dim;i++)
      s += vol(i)*coord[i];

    return s;
  }

  static coordinate_t site_safe(const coordinate_t *coord) {
    int c[D];
    for(int i=0;i<D;++i) {
      c[i]=coord[i];
      if(c[i]>=dim(i))
        c[i]-=dim(i);
      if(c[i]<0)
        c[i]+=dim(i);
    }
    return site(c);
  }

  static coordinate_t index(coordinate_t site, coordinate_t dir) {
    return dir*vols_[Dim]+site;
  }

  static coordinate_t index(const coordinate_t *coord, coordinate_t dir) {
    return dir*vols_[Dim]+site(coord);
  }

  static void init(const coordinate_t *dim) {
    vols_[0]=1;

    for(int i=0;i<Dim;i++) {
      dims_[i]=dim[i];
      vols_[i+1]=dims_[i]*vols_[i];
    }

    n_links_=vols_[Dim]*Dim;
  }

  static int dim(int i)  {return dims_[i];}
  static int dim()  {return Dim;}
  static const int *dims()  {return &dims_[0];}
  static int vol(int i)  {return vols_[i];}
  static int n_sites()  {return vol(Dim);}
  static int n_links()  {return n_links_;}

  static LatticeIterator<Dim> begin() {return LatticeIterator<Dim>(dims_);}
  static LatticeIterator<Dim> end() {return LatticeIterator<Dim>();}

  static coordinate_t up(coordinate_t site, coordinate_t dir) {
    coordinate_t mask = vols_[dir+1]-1;
    return (site & ~mask) | ((site + vols_[dir]) & mask);
  }

  static coordinate_t dn(coordinate_t site, coordinate_t dir) {
    coordinate_t mask = vols_[dir+1]-1;
    return (site & ~mask) | ((site - vols_[dir]) & mask);
  }

  static void init_arrays() {}

  static void clean() {}

private:
  static coordinate_t n_links_;

  static coordinate_t dims_[Dim];
  static coordinate_t vols_[Dim+1];
};

template<int Dim>  typename Indexer<Dim>::coordinate_t Indexer<Dim>::n_links_;

template<int Dim> typename Indexer<Dim>::coordinate_t Indexer<Dim>::dims_[Dim];
template<int Dim> typename Indexer<Dim>::coordinate_t Indexer<Dim>::vols_[Dim+1];
