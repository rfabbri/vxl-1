// This is brcv/seg/dbdet/pro/test_sel_circle_process.cxx

//:
// \file

#include "dbdet_test_sel_circle_process.h"
#include <dbdif/dbdif_analytic.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

//#include <mw/algo/mw_test_sel_geometry.h>

//: Constructor
dbdet_test_sel_circle_process::dbdet_test_sel_circle_process()
{
  if( 
      !parameters()->add( "Initial radius" , "-rad_ini" , 3.0 ) ||
      !parameters()->add( "Final radius" , "-rad_end" , 100.0 ) ||
      !parameters()->add( "Edge linker: Radius of Neighborhood" , "-nrad" , 3 ) ||
      !parameters()->add( "Edge linker: Position uncertainty" , "-dx" , 0.1 ) ||
      !parameters()->add( "Edge linker: Orientation uncertainty(Deg)" , "-dt" , 5.0 ) ||
      !parameters()->add( "Perturb?" , "-perturb"     , false) ||
      !parameters()->add( "   perturb position" , "-dpos"     , 0.1) ||
      !parameters()->add( "   perturb tangents (deg)" , "-dtan"     , 5.0)
      ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbdet_test_sel_circle_process::~dbdet_test_sel_circle_process()
{
}


//: Clone the process
bpro1_process*
dbdet_test_sel_circle_process::clone() const
{
  return new dbdet_test_sel_circle_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_test_sel_circle_process::name()
{
  return "Test SEL Geometry - Circle";
}


//: Return the number of input frame for this process
int
dbdet_test_sel_circle_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_test_sel_circle_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_test_sel_circle_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_test_sel_circle_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  return to_return;
}

//: Execute the process
bool
dbdet_test_sel_circle_process::execute()
{

  double rad_ini=0;
  parameters()->get_value( "-rad_ini", rad_ini);
  double rad_end=0;
  parameters()->get_value( "-rad_end", rad_end);
  double dx=0;
  parameters()->get_value( "-dx", dx);
  double dt=0;
  parameters()->get_value( "-dt", dt);

  int nrad=0;
  parameters()->get_value( "-nrad", nrad);

  bool do_perturb=false;
  parameters()->get_value( "-perturb", do_perturb);
  double dpos=0, dtan=0;
  if (do_perturb) {
    parameters()->get_value( "-dpos", dpos);
    parameters()->get_value( "-dtan", dtan);
  }

//  mw_test_sel_geometry::test_circle( rad_ini, rad_end, nrad, dx, dt, do_perturb, dpos, dtan);

  vcl_cerr << "Function must be updated before attempting to test SEL\n";
  abort();

  return true;
}

bool
dbdet_test_sel_circle_process::finish()
{
  return true;
}

