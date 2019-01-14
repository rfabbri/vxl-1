// This is brcv/rec/dbasn/dbasn_params.cxx
//:
// \file

#include <dbasn/dbasn_params.h>


//: Output stream operator for printing the parameter values
std::ostream& operator<<(std::ostream& os, const dbasn_params& params)
{
  os << "\n  dbasn_params: ";
  os << "T0: " << params.T0_ << ", ";
  os << "Tf: " << params.Tf_ << ", ";
  os << "Tr: " << params.Tr_ << ", ";
  os << "I0: " << params.I0_ << ", ";
  os << "I1: " << params.I1_ << ", ";
  os << "Is: " << params.Is_ << std::endl << "    ";
  os << "wN: " << params.wN_ << ", ";
  os << "wL: " << params.wL_ << std::endl;
  return os;
}
