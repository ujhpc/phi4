#ifndef __LOGING_H__
#define __LOGING_H__



#include<iostream>

class Log {


  enum Levels  {TRACE, DEBUG, INFO, WARN,FATAL};
public:
 Log(std::ostream &out):out_(out),level_(INFO),current_entry_level_(INFO),info(INFO) {
  }


  template<typename T> Log &operator<<(const T &val) {
    out_<<val;
    return *this;
  }




private:

  struct entry_point {
    entry_point(Levels level):level_(level) {};
    template<typename T>  Log &operator<<(const T &val) { 
      current_entry_level_=level_;
      out_<<val;
      return *;
    }

  private:
    const Levels level_;
  };

    
  Levels level_;
  Levels current_entry_level_;
  std::ostream &out_;
 public:
  entry_point info;

};


#endif
