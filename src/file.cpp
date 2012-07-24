#include<iostream>
//#include<fstream>
#include <popt.h>



bool is_table_end(const struct poptOption & opt) {
    
  return ((NULL==opt.longName) && ('\0'==opt.shortName)
	  && opt.argInfo==0 &&
	  opt.arg==NULL  &&
	  opt.val==0 &&
	  NULL==opt.descrip &&
	  NULL==opt.argDescrip) ;


}

bool is_option(const struct poptOption & opt) {
    
  return (NULL!=opt.longName) || ('\0'!=opt.shortName);

}

void print_arg(int arg_info, void *value) {
  switch (arg_info) {
  case POPT_ARG_STRING :

    std::cerr<<*(char **)(value);
    break;
  case POPT_ARG_INT :
    std::cerr<<*(int *)value;
    break;
  case POPT_ARG_FLOAT :
    std::cerr<<*(float *)value;
    break;
  case POPT_ARG_DOUBLE :
    std::cerr<<*(double *)value;
    break;
  }

}

std::ostream  &print_option(std::ostream &out,const struct poptOption &opt) { 

  if(opt.longName!=NULL)
    out<<opt.longName<<"\t";
  else if(opt.shortName!=NULL)
    out<<opt.shortName<<"\t";
  else return out;
 
 if(NULL!=opt.arg) {
   print_arg(opt.argInfo,opt.arg);
 }

 if(NULL!=opt.descrip)
   out<<"\t"<<opt.descrip;
 
 return out;
}

std::ostream &print_parameters(std::ostream &out,struct poptOption *opts) {
  int i=0;
  while(!is_table_end(opts[i]) ) {
    if(is_option(opts[i]) ) {
      out<<"# ";
      print_option(out,opts[i]);
      out<<"\n";
    }
    i++;
  }
  return out;
}
