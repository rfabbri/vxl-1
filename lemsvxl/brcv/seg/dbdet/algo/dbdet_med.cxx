#include "dbdet_med.h"

#include <iostream>
#include <fstream>
#include <cassert>
#include <algorithm>

//------------------------------------------------------------------------------
// Multiscale edge detector object
//------------------------------------------------------------------------------

//:
dbdet_med::dbdet_med(int nrad, double dtheta, double dpos):
  nrad_(nrad), dtheta_(dtheta), dpos_(dpos)
{
}

//:destructor
dbdet_med::~dbdet_med()
{
}

//: 
void dbdet_med::build_curvelets()
{
  std::cout << "Building curvelets ..." ;
  std::cout.flush();


  std::cout << "done!" << std::endl;
}



void dbdet_med::report_stats()
{
  std::cout << "======================================" << std::endl;
  std::cout << "Summary\n";
  std::cout << "======================================" << std::endl;

  std::cout << "======================================" << std::endl;
}

