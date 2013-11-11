#include <splr/tests/dummy_filter_2d.h>

void vsl_b_write(vsl_b_ostream& os, const 
   dummy_filter_2d& filter){
  vsl_b_write(os, filter.x_);
}

void vsl_b_read(vsl_b_istream& is,
   dummy_filter_2d& filter){
  vsl_b_read(is, filter.x_);
}

void vsl_print_summary(vcl_ostream & os, const dummy_filter_2d & filter){
}
