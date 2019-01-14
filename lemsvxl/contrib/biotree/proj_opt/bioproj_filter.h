#ifndef bioproj_filter_h_
#define bioproj_filter_h_


#include <iostream>

class bioproj_filter{
public:
  bioproj_filter() {};
  virtual ~bioproj_filter(){ delete [] filter_; };

  double *filter_;
  int half_kernel_size_;
  int full_kernel_size_;
};

#endif
