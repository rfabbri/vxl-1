// This is brcv/trk/dbmrf/pro/dbmrf_curve_projector_process.cxx

//:
// \file

#include "dbmrf_curve_projector_process.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include "dbmrf_curvel_3d_storage.h"
#include <bpro1/bpro1_parameters.h>

#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>


//: Constructor
dbmrf_curve_projector_process::dbmrf_curve_projector_process() : bpro1_process()
{
  if( !parameters()->add( "Extract(false)/Project(true)" , "-project", false ) ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


//: Destructor
dbmrf_curve_projector_process::~dbmrf_curve_projector_process()
{
}


//: Clone the process
bpro1_process* 
dbmrf_curve_projector_process::clone() const
{
  return new dbmrf_curve_projector_process(*this);
}


//: Return the name of the process
vcl_string
dbmrf_curve_projector_process::name()
{
  return "Extract 2D Curves";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbmrf_curve_projector_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "curvel_3d" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbmrf_curve_projector_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns the number of input frames to this process
int
dbmrf_curve_projector_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbmrf_curve_projector_process::output_frames()
{
  return 1;
}


//: Project a 3d curve
static vsol_digital_curve_2d_sptr
curvel_3d_project(const bmrf_curve_3d_sptr& curve_3d,
                  const vnl_double_3x4& camera)
{
  vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d();
  for ( bmrf_curve_3d::const_iterator itr = curve_3d->begin();
        itr != curve_3d->end();  ++itr )
  {
    vnl_double_4 pt4((*itr)->x(), (*itr)->y(), (*itr)->z(), 1.0);
    vnl_double_3 pt3 = camera * pt4;
    dc->add_vertex(new vsol_point_2d(pt3[0]/pt3[2], pt3[1]/pt3[2]));
  }
  return dc;
}


//: Extract the 2d curve at frame f used to create the 3d curve
static vsol_digital_curve_2d_sptr
curvel_3d_extract(const bmrf_curve_3d_sptr& curve_3d,
                  int frame)
{
  vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d();
  for ( bmrf_curve_3d::const_iterator itr = curve_3d->begin();
        itr != curve_3d->end();  ++itr )
  {
    vnl_double_2 pt;
    if((*itr)->pos_in_frame(frame, pt))
      dc->add_vertex(new vsol_point_2d(pt[0], pt[1]));
  }
  if(dc->size() == 0)
    return NULL;
  return dc;
}


//: Run the process on the current frame
bool
dbmrf_curve_projector_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << __FILE__ << " - not exactly one input frame" << vcl_endl;
    return false;
  }

  // get 3D curves from the storage class

  dbmrf_curvel_3d_storage_sptr frame_curves_3D;
  frame_curves_3D.vertical_cast(input_data_[0][0]);

  vcl_set<bmrf_curve_3d_sptr> curves;
  frame_curves_3D->get_curvel_3d( curves );
  int frame = frame_curves_3D->frame();
  vnl_double_3x4 camera = frame_curves_3D->camera();

  bool project=false;
  parameters()->get_value( "-project", project );

  vcl_vector< vsol_spatial_object_2d_sptr > vsol2d_curves;
  for ( vcl_set<bmrf_curve_3d_sptr>::iterator itr = curves.begin();
        itr != curves.end();  ++itr )
  {
    if(project){ // Project the curves into the image
      vsol2d_curves.push_back(curvel_3d_project(*itr, camera).ptr());
    }

    else{ // Extract the 2D curves used to make the 3D curves
      if(vsol_spatial_object_2d_sptr obj = curvel_3d_extract(*itr, frame).ptr())
        vsol2d_curves.push_back(obj);
    }
  }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_curves = vidpro1_vsol2D_storage_new();
  output_curves->add_objects(vsol2d_curves);
  output_data_[0].push_back(output_curves);

  return true;
}


//: Finish
bool
dbmrf_curve_projector_process::finish()
{
  return true;
}




