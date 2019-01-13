// This is breye1/bpro1/bpro1_parameters.hxx
#ifndef bpro1_parameters_txx_
#define bpro1_parameters_txx_
//:
// \file
// \brief Templated code for bpro1 parameters.
// \author Based on original code by Matt Leotta
// \date 7/2/2004


#include "bpro1_parameters.h"
#include <utility>
#include <iostream>


//: Set the current value to \p val
template<class T>
bool
bpro1_param_type<T>::set_value( const T& val )
{
  if( has_bounds_ && (val < min_value_ || max_value_ < val) )
    return false;

  value_ = val;
  return true;
}


//: Create a string representation of the value
template<class T>
std::string
bpro1_param_type<T>::create_string(const T& val) const
{
  std::stringstream stm;
  stm << val;
  return stm.str();
}


//: Parse a string representation of the value
template<class T>
T
bpro1_param_type<T>::parse_string(const std::string& input) const
{
  T val;
  std::istringstream strm(input);
  strm >> val;
  return val;
}

//===========================================================================================

//: Use this macro to instantiate bpro1_parameters for each parameter type
#define VIDPRO_PARAMETERS_INSTANTIATE(T) \
template class bpro1_param_type< T >;

#endif // bpro1_parameters_txx_
