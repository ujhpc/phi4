#pragma once

#include <vector>

template <typename T, typename I, typename Storage = std::vector<T> >
class ScalarField {
 public:
  static const int n_components = 1;
  typedef Storage storage_t;
  typedef T scalar_t;
  typedef I indexer_t;
  typedef typename indexer_t::site_t site_t;

  ScalarField(int size = indexer_t::n_sites()) : storage_(size), size_(size) {}

  int size() const { return size_; }

  T get(int i, int) const { return storage_.at(i); }
  T operator()(int i, int) const { return storage_.at(i); }

  template <class V> V get(const site_t& i, int) const {
    return storage_.at(i);
  }
  template <class V> V operator()(const site_t& i, int c) const {
    return get(i, c);
  }

  void set(int i, T t) { storage_.assign(i, t); }
  void set(int i, int, T t) { storage_.assign(i, t); }

 private:
  storage_t storage_;
  int size_;
};

template <typename T, int Comp, typename I, typename Storage = std::vector<T> >
class VectorField {
 public:
  static const int n_components = Comp;
  typedef Storage storage_t;
  typedef T scalar_t;
  typedef T* vector_t;
  typedef I indexer_t;
  typedef typename indexer_t::site_t site_t;

  VectorField(int size = indexer_t::n_sites())
      : storage_(size * Comp), size_(size) {}

  int size() const { return size_; }

  scalar_t get(int i, int c) const { return storage_.at(i * Comp + c); }
  scalar_t operator()(int i, int c) const { return get(i, c); }

  template <typename V> V get(const site_t& i, int c) const {
    return storage_.at(i * site_t(Comp) + site_t(c));
  }
  template <typename V> V operator()(const site_t& i, int c) const {
    return get(i, c);
  }

  void set(int i, int c, T t) { storage_.assign(i * Comp + c, t); }
  void set(int i, scalar_t s) {
    i *= Comp;
    for (int c = 0; c < Comp; ++c, ++i) {
      storage_.assign(i, s);
    }
  }

 private:
  storage_t storage_;
  int size_;
};
