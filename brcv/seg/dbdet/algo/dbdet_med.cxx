#include "dbdet_med.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

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
  vcl_cout << "Building curvelets ..." ;
  vcl_cout.flush();


  vcl_cout << "done!" << vcl_endl;
}



void dbdet_med::report_stats()
{
  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "Summary\n";
  vcl_cout << "======================================" << vcl_endl;

  vcl_cout << "======================================" << vcl_endl;
}

