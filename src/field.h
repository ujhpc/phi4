#pragma once

#include <cstdio>
#include <vector>

template <typename T> class ScalarFieldArray {

 public:
  typedef std::vector<T> storage_t;
  typedef T scalar_t;

  ScalarFieldArray(int n) : field_(n) {};

  class ScalarFieldAccessor {
   public:
    T get(int i) const { return field_[i]; }
    T get(int i, int) const { return get(i); }
    T operator()(int i) const { return get(i); }
    T operator()(int i, int) const { return get(i); }

    void set(int i, T t) { field_[i] = t; }
    void set(int i, int, T t) { set(i, t); }

   private:
    ScalarFieldAccessor(storage_t& p) : field_(p) {};
    storage_t& field_;

    friend class ScalarFieldArray;
  };

  typedef ScalarFieldAccessor accessor_t;

  accessor_t accessor() { return ScalarFieldAccessor(field_); }

  int fwrite(FILE* fout) {
    return std::fwrite(&field_[0], sizeof(T), field_.size(), fout);
  }

  int fread(FILE* fin) {
    return std::fread(&field_[0], sizeof(T), field_.size(), fin);
  }

 private:
  storage_t field_;
};

template <typename A, typename Indexer> class ScalarField {
 public:
  typedef typename A::scalar_t scalar_t;
  typedef Indexer indexer_t;
  typedef typename A::accessor_t accessor_t;

  ScalarField(int n = indexer_t::n_sites())
      : field_(n), accessor_(field_.accessor()) {};

  scalar_t get(int i) const { return accessor_.get(i); }
  scalar_t get(int i, int) const { return get(i); }
  scalar_t operator()(int i) const { return get(i); }
  scalar_t operator()(int i, int) const { return get(i); }

  void set(int i, scalar_t t) { accessor_.set(i, t); }
  void set(int i, int, scalar_t t) { set(i, t); }

  accessor_t accessor() { return accessor_; }
  static const int n_components = 1;

  int fwrite(FILE* fout) { field_.fwrite(fout); }

  int fread(FILE* fin) { field_.fread(fin); }

 private:
  A field_;
  accessor_t accessor_;
};

template <typename T, int N_COMP> class VectorFieldArray {

 public:
  typedef std::vector<T> storage_t;
  typedef T scalar_t;
  typedef T* vector_t;

  VectorFieldArray(int n) : n_fields_(n), field_(n_fields_ * N_COMP) {};

  class VectorFieldAccesor {
   public:
    vector_t get(int i) const { return &pfield_[i * N_COMP]; }
    scalar_t get(int i, int j) const { return pfield_[i * N_COMP + j]; }
    vector_t operator()(int i) const { return get(i); }
    scalar_t operator()(int i, int j) const { return get(i, j); }

    void set(int i, int j, T t) { pfield_[i * N_COMP + j] = t; }
    void set(int i, vector_t v) {
      int offset = i * N_COMP;

      for (int j = 0; j < N_COMP; j++, offset++) {
        pfield_[offset] = v[j];
      }
    }

    void set(int i, scalar_t s) {
      int offset = i * N_COMP;

      for (int j = 0; j < N_COMP; j++, offset++) {
        pfield_[offset] = s;
      }
    }

   private:
    VectorFieldAccesor(storage_t& p) : pfield_(p) {};
    storage_t& pfield_;
    friend class VectorFieldArray;
  };

  typedef VectorFieldAccesor accessor_t;

  accessor_t accessor() { return VectorFieldAccesor(field_); }

  int fwrite(FILE* fout) {
    return std::fwrite(&field_[0], sizeof(T), field_.size(), fout);
  }

  int fread(FILE* fin) {
    return std::fread(&field_[0], sizeof(T), field_.size(), fin);
  }

  static const int n_components = N_COMP;

 private:
  int n_fields_;
  storage_t field_;
};

template <typename A, typename Indexer> class VectorField {
 public:
  typedef typename A::scalar_t scalar_t;
  typedef typename A::vector_t vector_t;
  typedef Indexer indexer_t;
  typedef typename A::accessor_t accessor_t;

  static const int n_components = A::n_components;

  VectorField(int n = indexer_t::n_sites())
      : field_(n), accessor_(field_.accessor()) {};

  vector_t get(int i) const { return accessor_.get(i); }
  scalar_t get(int i, int j) const { return accessor_.get(i, j); }
  vector_t operator()(int i) const { return get(i); }
  scalar_t operator()(int i, int j) const { return get(i, j); }

  void set(int i, vector_t t) { accessor_.set(i, t); }
  void set(int i, scalar_t t) { accessor_.set(i, t); }
  void set(int i, int j, scalar_t t) { accessor_.set(i, j, t); }

  accessor_t accessor() { return accessor_; }

  int fwrite(FILE* fout) { field_.fwrite(fout); }

  int fread(FILE* fin) { field_.fread(fin); }

 private:
  A field_;
  accessor_t accessor_;
};
