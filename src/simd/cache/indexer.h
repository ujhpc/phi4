#ifndef __INDEXER_H__
#define __INDEXER_H__

#include<iostream>


#include"iterator.h"

template<int Dim>
class Indexer {

  typedef int coordinate_t;

public:
  typedef LatticeIterator<Dim> iterator;
  enum {D=Dim};

  /** calculates the site number from coordinates
      does assumes that all the coordinates are in range
  */
  static coordinate_t site(const coordinate_t *coord) {
    int s=coord[0];
    for(int i=1;i<Dim;i++)
      s += vol(i)*coord[i];
    return s;
  }

  /** calculates the site number from coordinates
      checks if the coordinates are in range. If not
      wraps them around. 
  */
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
    return dir*vols_[Dim]+site(coord);;
  }

  static void init(const coordinate_t *dim) {
    vols_[0]=1;
    for(int i=0;i<Dim;i++) {
      dims_[i]=dim[i];
      vols_[i+1]=dims_[i]*vols_[i];
    }
    n_links_=vols_[Dim]*Dim;
    init_arrays();
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
    return up_[index(site,dir)];
  }

  static coordinate_t dn(coordinate_t site, coordinate_t dir) {
    return dn_[index(site,dir)];
  }

  static void init_arrays() {
    up_  = new coordinate_t[n_links_];
    dn_  = new coordinate_t[n_links_];

    for(iterator it=begin();it!=end();++it) {
      for(int mu=0;mu<Dim;++mu) {
	int coord[Dim];
	for(int i=0;i<Dim;++i) {
	  coord[i]=(*it)[i];
	}
	coord[mu]=(*it)[mu]+1;
	if(coord[mu]>= dim(mu))
	  coord[mu] -= dim(mu);
	up_[index( *it,mu)] = site(coord);
	coord[mu]=(*it)[mu]-1;
	if(coord[mu]<0)
	  coord[mu]+=dims_[mu];
	dn_[index( *it,mu)] = site(coord);
      }
    }
  }



  static void clean() {
    delete [] up_;
    delete [] dn_;
  }
 
private:
  static coordinate_t n_links_;
  static coordinate_t dims_[Dim];
  static coordinate_t vols_[Dim+1];
  static coordinate_t *up_;
  static coordinate_t *dn_;
};


template<int Dim> typename Indexer<Dim>::coordinate_t Indexer<Dim>::n_links_;
template<int Dim> typename Indexer<Dim>::coordinate_t Indexer<Dim>::dims_[Dim];
template<int Dim> typename Indexer<Dim>::coordinate_t Indexer<Dim>::vols_[Dim+1];
template<int Dim> typename Indexer<Dim>::coordinate_t *Indexer<Dim>::up_;
template<int Dim> typename Indexer<Dim>::coordinate_t *Indexer<Dim>::dn_;


#endif
