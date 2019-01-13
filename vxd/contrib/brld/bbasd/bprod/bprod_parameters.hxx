// This is breye/bprod/bprod_parameters.hxx
#ifndef bprod_parameters_txx_
#define bprod_parameters_txx_
//:
// \file
// \brief Templated code for bprod parameters.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 7/2/2004


#include "bprod_parameters.h"
#include <utility>
#include <iostream>


//: Set the current value to \p val
template<class T>
bool
bprod_param_type<T>::set_value( const T& val )
{
  if( has_bounds_ && (val < min_value_ || max_value_ < val) )
    return false;

  value_ = val;
  return true;
}


//: Create a string representation of the value
template<class T>
std::string
bprod_param_type<T>::create_string(const T& val) const
{
  std::stringstream stm;
  stm << val;
  return stm.str();
}


//: Parse a string representation of the value
template<class T>
T
bprod_param_type<T>::parse_string(const std::string& input) const
{
  T val;
  std::istringstream strm(input);
  strm >> val;
  return val;
}

//===========================================================================================

//: Use this macro to instantiate bprod_parameters for each parameter type
#define BPROD_PARAMETERS_INSTANTIATE(T) \
template class bprod_param_type< T >;

#endif // bprod_parameters_txx_
