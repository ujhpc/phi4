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


template<typename T, typename indexer_t, typename accessor_t>
class Field {



public:
  Field(accessor_t accessor):field_(accessor) {};
  
  T operator[](unsigned int i) {return field_[i];};
  
  T operator[](const unsigned int  *coord)  {
    return field_[indexer_t::site(coord)];
  };
  

  
 private:
  accessor_t field_;
  

  
};







#endif
