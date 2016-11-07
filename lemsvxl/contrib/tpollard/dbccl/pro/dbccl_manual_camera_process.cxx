#include "dbccl_manual_camera_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>

#include <vgl/vgl_distance.h>
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vul/vul_awk.h>


#include <vpgl/vpgl_perspective_camera.h>



//-------------------------------------------
dbccl_manual_camera_process::dbccl_manual_camera_process() : bpro1_process()
{
  if( !parameters()->add( 
    "World points file" , "-dbcclwpf" , bpro1_filepath("","*") ) ||
    !parameters()->add( 
    "Camera file" , "-dbcclcf" , bpro1_filepath("","*") ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//---------------------------------------------
dbccl_manual_camera_process::~dbccl_manual_camera_process()
{
}


//--------------------------------------------
bpro1_process* 
dbccl_manual_camera_process::clone() const
{
  return new dbccl_manual_camera_process(*this);
}


//------------------------------------------
vcl_string
dbccl_manual_camera_process::name()
{
  return "Manual Camera Computation";
}


//-----------------------------------------------
vcl_vector< vcl_string > dbccl_manual_camera_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//----------------------------------------------------
vcl_vector< vcl_string > dbccl_manual_camera_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vpgl camera" );
  return to_return;
}


//: Run the process on the current frame
bool
dbccl_manual_camera_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbccl_manual_camera_process::execute() - not exactly one"
          << " input image \n";
    return false;
  }

  // Get the world points from file.
  vcl_vector< vgl_point_3d<double> > all_world_points;
  bpro1_filepath wp_path;
  parameters()->get_value( "-dbcclwpf" , wp_path );
  vcl_string world_point_file = wp_path.path;
  vcl_ifstream wp_stream( world_point_file.c_str() );
  vul_awk awk( wp_stream );
  while( awk ){
    vgl_point_3d<double> new_point( atof(awk[0]), atof(awk[1]), atof(awk[2]) );
    all_world_points.push_back( new_point );
    ++awk;
  }

  // Get the image points from storage.
  vidpro1_vsol2D_storage_sptr image_points_storage;
  vcl_vector< vgl_point_2d<double> > all_image_points;
  vcl_vector< vgl_line_segment_2d<double> > up_lines;
  image_points_storage.vertical_cast(input_data_[0][0]);
  vcl_vector< vsol_spatial_object_2d_sptr > image_points_vsol = 
    image_points_storage->all_data();
  for( int i = 0; i < all_world_points.size(); i++ ){
    all_image_points.push_back( vgl_point_2d<double>( 
    image_points_vsol[i]->cast_to_point()->x(),
    image_points_vsol[i]->cast_to_point()->y() ) );
  }

  // Remove all unused world points.
  vcl_vector< vgl_point_2d<double> > used_image_points;
  vcl_vector< vgl_point_3d<double> > used_world_points;
  for( int i = 0; i < all_world_points.size(); i++ ){
    if( all_image_points[i].x() < 0 || all_image_points[i].y() < 0 ) continue;
    used_image_points.push_back( all_image_points[i] );
    used_world_points.push_back( all_world_points[i] );
  }

  // Compute the camera from point correspondences.
  vpgl_proj_camera<double> this_camera;
  dbccl_manual_camera mc;
  mc.compute( used_world_points, used_image_points, up_lines, this_camera );

  // Now project the world points using the computed camera.
  vcl_vector< vsol_spatial_object_2d_sptr > proj_world_points;
  for( int p = 0; p < all_world_points.size(); p++ ){
    vgl_homg_point_2d<double> new_ip = this_camera.project( 
      vgl_homg_point_3d<double>( all_world_points[p] ) );
    vsol_spatial_object_2d_sptr new_point_obj = 
      new vsol_point_2d( new_ip.x()/new_ip.w(), new_ip.y()/new_ip.w() );
    proj_world_points.push_back( new_point_obj );
  }
  
  clear_output();
  vidpro1_vsol2D_storage_sptr proj_wps_vsol = vidpro1_vsol2D_storage_new();
  proj_wps_vsol->add_objects( proj_world_points, "projected world points" );
  output_data_[0].push_back(proj_wps_vsol);

  dvpgl_camera_storage_sptr cam_storage = dvpgl_camera_storage_new();
  cam_storage->set_camera( new vpgl_proj_camera<double>( this_camera ) );
  output_data_[0].push_back( cam_storage );
  return true;
};


//------------------------------------------
bool
dbccl_manual_camera_process::finish()
{
  bpro1_filepath camera_path;
  parameters()->get_value( "-dbcclcf" , camera_path );
  vcl_string camera_file = camera_path.path;

  vcl_ofstream camera_stream( camera_file.c_str() );
  for( int i = 0; i < input_data_.size(); i++ ){
    dvpgl_camera_storage_sptr cam_storage;
    if( input_data_[i].size() == 3 ){
      cam_storage.vertical_cast( input_data_[i][2] );
      camera_stream << "Frame " << i << '\n' << cam_storage->get_camera()->get_matrix() << '\n';
    }
    else
      camera_stream << "Frame " << i << '\n' << "BAD MATRIX" << '\n';
  }
  return true;
}


//-------------------------------------------------
int
dbccl_manual_camera_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbccl_manual_camera_process::output_frames()
{
  return 1;
}



