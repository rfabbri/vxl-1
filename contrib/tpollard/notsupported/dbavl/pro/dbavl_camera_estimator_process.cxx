#include "dbavl_camera_estimator_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vsol/vsol_point_2d.h>


//-------------------------------------------
dbavl_camera_estimator_process::dbavl_camera_estimator_process() : bpro1_process()
{
  if( !parameters()->add( 
    "Search Radius" , "-dbavlsr" , 0 ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  } 
  //parameters()->get_value( "-dbavlsr" , tracker.params.search_rad );
}


//---------------------------------------------
dbavl_camera_estimator_process::~dbavl_camera_estimator_process()
{
}


//--------------------------------------------
bpro1_process* 
dbavl_camera_estimator_process::clone() const
{
  return new dbavl_camera_estimator_process(*this);
}


//------------------------------------------
vcl_string
dbavl_camera_estimator_process::name()
{
  return "Aerial Camera Estimator";
}


//-----------------------------------------------
vcl_vector< vcl_string >
dbavl_camera_estimator_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//----------------------------------------------------
vcl_vector< vcl_string > 
dbavl_camera_estimator_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Run the process on the current frame
bool
dbavl_camera_estimator_process::execute()
{
  return true;
}


//------------------------------------------
bool
dbavl_camera_estimator_process::finish()
{
  return true;
}


//-------------------------------------------------
int
dbavl_camera_estimator_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbavl_camera_estimator_process::output_frames()
{
  return 1;
}



