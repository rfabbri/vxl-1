// This is algo/bapl/bapl_keypoint.cxx
//:
// \file

#include "bapl_keypoint.h"
#include <iostream>

//: Print a summary of the keypoint data to a stream
std::ostream& operator<< (std::ostream& os, bapl_keypoint const & k)
{
  k.print_summary(os);
  return os;
}



