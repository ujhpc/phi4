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


template<typename T, typename I, typename accessor_t>
class Field {



public:

  typedef I indexer_t;

  Field(accessor_t accessor):field_(accessor) {};
  
  T operator[](int i) {return field_[i];};
  
  T operator[](const unsigned int  *coord)  {
    return field_[indexer_t::site(coord)];
  };
  
  T get(int i) {return field_[i];};
  void  set(const T t, int i) {field_[i]=t;};

  
  
 private:
  accessor_t field_;
  

  
};

template<typename T> 
class ScalarFieldArray {


public:
  typedef std::vector<T> storage_t;

  ScalarFieldArray(int n):field_(n) {};

  class ScalarFieldAccesor {
  
    T get(int i) const {return (*pfield_)[i];}
    T get(int i,int) const {return get(i);}
    T operator()(int i) const  {return get(i);}
    T operator()(int i,int) const  {return get(i);}
    
    void set(T t, int i) { (*pfield_)[i]=t;}
    void set(T t, int i, int ) { set(t,i);}
    

  private:
    ScalarFieldAccesor(storage_t *p):pfield_(p);		       
    storage_t *pfield_;
  };


  typedef  ScalarFieldAccesor accessor_t;

  accessor_t accessor() const {
    return ScalarAccesor(field_);
  }


private:
  storage_t field_;
  
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
    

    void set(T t, int i, int j) { (*pfield_)[i*N_+j]=t;}
    void set(vector_t v, int i) { 
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
