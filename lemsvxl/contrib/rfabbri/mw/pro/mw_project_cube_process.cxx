//:
// \file

#include "mw_project_cube_process.h"

#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbdif/algo/dbdif_data.h>
#include <vpgl/vpgl_perspective_camera.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/io/dvpgl_io_cameras.h>



#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>

//: Constructor
mw_project_cube_process::mw_project_cube_process()
{
  if( 
      !parameters()->add( "     x min" , "-xmin"     , 30.0 ) ||
      !parameters()->add( "     x max" , "-xmax"     , 50.0 ) ||
      !parameters()->add( "     y min" , "-ymin"     , -50.0 ) ||
      !parameters()->add( "     y max" , "-ymax"     , -70.0 ) ||
      !parameters()->add( "     z min" , "-zmin"     , 30.0 ) ||
      !parameters()->add( "     z max" , "-zmax"     , 50.0 )
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
mw_project_cube_process::~mw_project_cube_process()
{
}


//: Clone the process
bpro1_process*
mw_project_cube_process::clone() const
{
  return new mw_project_cube_process(*this);
}


//: Return the name of this process
vcl_string
mw_project_cube_process::name()
{
  return "Project Cube";
}


//: Return the number of input frame for this process
int
mw_project_cube_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
mw_project_cube_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > mw_project_cube_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vpgl camera" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > mw_project_cube_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
mw_project_cube_process::execute()
{
  clear_output();


  // get camera matrix

  dvpgl_camera_storage_sptr cam_storage;

  cam_storage.vertical_cast(input_data_[0][0]);

  if(cam_storage->get_camera()->type_name() != "vpgl_perspective_camera") {
    vcl_cerr << "Error: process requires a perspective camera" << vcl_endl;
    return false;
  }
  const vpgl_perspective_camera<double> *pcam = 
    static_cast<const vpgl_perspective_camera<double> *>(cam_storage->get_camera());

  vcl_cout << "NAME: " << cam_storage->name() << vcl_endl;
  vcl_cout << "Camera: \n" << pcam->get_matrix();

  const vpgl_perspective_camera<double> &cam = *pcam;

  // get the parameters

  double xmin, xmax, ymin, ymax, zmin, zmax;
  parameters()->get_value( "-xmin", xmin);
  parameters()->get_value( "-xmax", xmax);
  parameters()->get_value( "-ymin", ymin);
  parameters()->get_value( "-ymax", ymax);
  parameters()->get_value( "-zmin", zmin);
  parameters()->get_value( "-zmax", zmax);

  vcl_vector<vsol_point_3d_sptr> vertices3d(8);
  vertices3d[0] = new vsol_point_3d(xmax, ymax, zmin);
  vertices3d[1] = new vsol_point_3d(xmin, ymax, zmax);
  vertices3d[2] = new vsol_point_3d(xmax, ymax, zmax);
  vertices3d[3] = new vsol_point_3d(xmin, ymin, zmax);
  vertices3d[4] = new vsol_point_3d(xmax, ymin, zmax);
  vertices3d[5] = new vsol_point_3d(xmin, ymax, zmin);
  vertices3d[6] = new vsol_point_3d(xmin, ymin, zmin);
  vertices3d[7] = new vsol_point_3d(xmax, ymin, zmin);

  vcl_vector<vsol_point_2d_sptr> pts(8);
  for (unsigned i=0; i < 8; ++i)
    pts[i] = new vsol_point_2d(vgl_point_2d<double>( cam.project(vertices3d[i]->get_p()) ));

  vcl_vector<vsol_line_2d_sptr> lines(12);
  lines[0]  = new vsol_line_2d(pts[1], pts[2]);
  lines[1]  = new vsol_line_2d(pts[2], pts[4]);
  lines[2]  = new vsol_line_2d(pts[4], pts[3]);
  lines[3]  = new vsol_line_2d(pts[3], pts[1]);
                                       
  lines[4]  = new vsol_line_2d(pts[0], pts[5]);
  lines[5]  = new vsol_line_2d(pts[0], pts[7]);
  lines[6]  = new vsol_line_2d(pts[7], pts[6]);
  lines[7]  = new vsol_line_2d(pts[6], pts[5]);
                                       
  lines[8]  = new vsol_line_2d(pts[4], pts[7]);
  lines[9]  = new vsol_line_2d(pts[2], pts[0]);
  lines[10] = new vsol_line_2d(pts[1], pts[5]);
  lines[11] = new vsol_line_2d(pts[3], pts[6]);

  vcl_vector< vsol_spatial_object_2d_sptr > segments;

  for (unsigned i=0; i<lines.size(); i++) {
    segments.push_back(lines[i]->cast_to_spatial_object());
  }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(segments, "segments");
  output_data_[0].push_back(output_vsol);

  return true;
}

bool
mw_project_cube_process::finish()
{
  return true;
}

