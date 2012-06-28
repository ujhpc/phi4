#ifndef __FIELD_H__
#define __FIELD_H__

#include<vector>

template<typename T, unsigned int Dim, 
	 typename storage_t = std::vector<T>, 
  typename return_t = T& > 
class ScalarFieldAccessor {  
  
public:
  ScalarFieldAccessor(storage_t &storage):storage_(storage){};
  
  return_t operator[](unsigned int i) {return storage_[i];}
private:
  storage_t &storage_;
};


template<typename T, unsigned int Dim, 
	 typename storage_t = std::vector<T>, 
	 typename return_t = T* > 
class VectorFieldAccessor {  
  
public:
  VectorFieldAccessor(storage_t &storage):storage_(storage){};
  
  return_t operator[](unsigned int i) {return &storage_[i];}
private:
  storage_t &storage_;
};


template<typename T> struct strip_ref {
  typedef T bare_t;

};

template<typename T> struct strip_ref<T  &> {
  typedef T bare_t;
};

template<typename T, typename I, typename accessor_t>
class Field {



public:

  typedef I indexer_t;

  Field(accessor_t accessor):field_(accessor) {};
  
#if 0  
   T operator[](int i) {return field_[i];};


  T operator[](const unsigned int  *coord)  {
    return field_[indexer_t::site(coord)];
  };
#endif

  T get(int i) {return field_[i];};
  void  set(int i, typename strip_ref<T>::bare_t t) {field_[i]=t;};

    
 private:
  accessor_t field_;
  
};

template<typename T> 
class ScalarFieldArray {


public:
  typedef std::vector<T> storage_t;
  typedef T scalar_t;
  
  ScalarFieldArray(int n):field_(n) {};
  
  class ScalarFieldAccessor {
  public:
    T get(int i) const {return field_[i];}
    T get(int i,int) const {return get(i);}
    T operator()(int i) const  {return get(i);}
    T operator()(int i,int) const  {return get(i);}
    
    void set(int i, T t) { field_[i]=t;}
    void set(int i,int,T t) { set(t,i);}
    

  private:
    ScalarFieldAccessor(storage_t &p):field_(p) {};		       
    storage_t &field_;
    
    friend  class ScalarFieldArray;
  };


  typedef  ScalarFieldAccessor accessor_t;

  accessor_t accessor()  {
    return ScalarFieldAccessor(field_);
  }


private:
  storage_t field_;
  
};


template<typename A, typename Indexer>
class ScalarField {
 public:
  typedef  typename A::scalar_t scalar_t;
  typedef  Indexer indexer_t;
  typedef typename A::accessor_t accessor_t;

 ScalarField(int n = indexer_t::n_sites()):field_(n), 
    accessor_(field_.accessor()) {
  };

  scalar_t get(int i) const {return accessor_.get(i);}
  scalar_t get(int i,int) const {return get(i);}
  scalar_t operator()(int i) const  {return get(i);}
  scalar_t operator()(int i,int) const  {return get(i);}
    
  void set(int i,     scalar_t t) { accessor_.set(i,t);}
  void set(int i,int, scalar_t t) { set(t,i);}
  
  accessor_t accessor() {return accessor_;}
  
 private:
  A field_;
  accessor_t accessor_;
};


template<typename T > 
class VectorFieldArray {

public:
  typedef std::vector<T> storage_t;
  typedef T* vector_t;
  
  VectorFieldArray(int n, int N):n_fields_(n),N_(N),
				 field_(n_fields_*N_) {};

  class VectorFieldAccesor {
  
    vector_t get(int i) const {return &pfield_[i*N_];}
    T get(int i,int j) const {return get(i*N_+j);}
    vector_t operator()(int i) const  {return get(i);}
    T operator()(int i,int) const  {return get(i);}
    

    void set(int i, int j,T t) { (*pfield_)[i*N_+j]=t;}
    void set(int i, vector_t v) { 
      int offset =i*N_;
      
      for(int j=0;j<N_;j++;offset++) {
	(*pfield_)[offset]=v[j];
      }
    }


  private:
    VectorFieldAccesor(storage_t *p):pfield_(p);		       
    storage_t *pfield_;
  };


  typedef  VectorFieldAccesor accessor_t;

  accessor_t accessor() const {
    return VectorFieldAccesor(field_);
  }


private:
  int n_fields_;
  int N_;
  storage_t field_;
  
};


#endif
