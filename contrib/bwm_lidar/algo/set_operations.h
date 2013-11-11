// This is lemsvxlsrc/contrib/bwm_lidar/algo/set_operations.h

//:
// \file
// \brief Standard Set Operations
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#ifndef set_operations_t_
#define set_operations_t_

#include "all_includes.h"

class set_operations {
public:
  set_operations();
  void add(int element);
  void add(int element, int set_element);
  void join(int element1, int element2);
  void print();
  int get_set_id(int element);
private:
  vcl_vector<vcl_vector<int> > sets;
};

#endif
