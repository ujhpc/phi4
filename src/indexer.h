#ifndef __INDEXER_H__
#define __INDEXER_H__


template<typename Dim>  
class Indexer {

  typedef unsigned int coordinate_t;
public:

  enum {D=Dim};

  static coordinate_t site(const coordinate_t *coord) {
    
  }



private:
  static coordinate_t *up_;
  static coordinate_t *dn_;
 

};


#endif
