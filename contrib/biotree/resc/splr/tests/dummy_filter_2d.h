#if !defined(DUMMY_FILTER_2D_H_)
#define DUMMY_FILTER_2D_H_

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

struct dummy_filter_2d {
  double x_;
  dummy_filter_2d() : x_(0.0) {}
  dummy_filter_2d(double x) : x_(x) {}
  bool operator==(const dummy_filter_2d & other) const{
   return x_ == other.x_;
  }
};

void vsl_b_write(vsl_b_ostream& os, const 
   dummy_filter_2d& filter);

void vsl_print_summary(vcl_ostream& os, const dummy_filter_2d & filter);

void vsl_b_read(vsl_b_istream& is,
   dummy_filter_2d& filter);

#endif
