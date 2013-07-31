#pragma once

#include<time.h>
#include<stdint.h>

class LinuxTimer {
public:

  static bool supported(int id,struct timespec *res) {
     if(clock_getres(id,res)!=0)
       return true;
     return false;
  }
  LinuxTimer(int id):id_(id),supported_(false){
    if(clock_getres(id_,&res_)==0)
      supported_=true;      
  };

  operator  bool() const {
    return supported_;
  } 

  bool bad() const {
    return !supported_;
  }

  void start() {
    clock_gettime(id_,&start_);
  }

  void stop() {
    clock_gettime(id_,&stop_);
  }

  uint64_t ellapsed_time() const {
    return 1000000000ll*stop_.tv_sec+stop_.tv_nsec-
      1000000000ll*start_.tv_sec-start_.tv_nsec;      
  }

 
  void print_res() const { 
    std::cout<<"resolution of timer "<<id_<<" is  "<<res_.tv_sec<<"s and "<<res_.tv_nsec<<"ns\n";
};


private:
  int id_;
  bool supported_;
  struct timespec res_;
  struct timespec start_;
  struct timespec stop_;
  
};
