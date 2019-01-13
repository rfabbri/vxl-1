//---------------------------------------------------------------------
// This is bbasd/bnld/examples/example_compute_fresnel_integral.cxx
//:
// \file
// \brief
//
// \author Based on original code by 
//  Nhon Trinh
// \date 2/3/2005
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <bnld/bnld_fresnel.h>
#include <iostream>

int main( int argc, char* argv[] )
{
  double x = 0;
  std::cout << "A simple program to compute fresnel integral" << std::endl;
  std::cout << "Enter x = ";
  std::cin >> x;
  double xc, xs;
  bnld_fresnel_integral(x, &xc, &xs);
  std::cout << "x = " << x << " \t fresnel_cos(x) = " << xc 
      << " \t fresnel_sin(x) = " << xs << std::endl;
  return 0;
}

