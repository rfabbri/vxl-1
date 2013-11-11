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
   
  do_first_pass = true;
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
  to_return.push_back( "vpgl camera" );
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

  // Get the world points.
  bpro1_filepath wp_path;
  parameters()->get_value( "-dbcclwpf" , wp_path );
  vcl_string world_point_file = wp_path.path;

  vcl_vector< vgl_point_3d<double> > all_world_points;
  vcl_ifstream wp_stream( world_point_file.c_str() );
  vul_awk awk( wp_stream );
  while( awk ){
    vgl_point_3d<double> new_point( atof(awk[0]), atof(awk[1]), atof(awk[2]) );
    all_world_points.push_back( new_point );
    ++awk;
  }

  vpgl_proj_camera<double> this_camera;

  // If this is the first time through, pick a default camera.
  if( do_first_pass ){//image_points_vsol.size() < all_world_points.size() ){
    double min_x, max_x, min_y, max_y, min_z, max_z;
    max_x = min_x = all_world_points[0].x();
    max_y = min_y = all_world_points[0].y();
    max_z = min_z = all_world_points[0].z();
    for( int i = 1; i < all_world_points.size(); i++ ){
      if( all_world_points[i].x() < min_x ) min_x = all_world_points[i].x();
      if( all_world_points[i].x() > max_x ) max_x = all_world_points[i].x();
      if( all_world_points[i].y() < min_y ) min_y = all_world_points[i].y();
      if( all_world_points[i].y() > max_y ) max_y = all_world_points[i].y();
      if( all_world_points[i].z() < min_z ) min_z = all_world_points[i].z();
      if( all_world_points[i].z() > max_z ) max_z = all_world_points[i].z();
    }
    double dx = max_x-min_x, dy = max_y-min_y, dz = max_z-min_z;
    vpgl_perspective_camera<double> default_camera;
    default_camera.set_camera_center( vgl_point_3d<double>( max_x+dx, max_y+dy, max_z+dz/2.0 ) );
    default_camera.look_at( vgl_homg_point_3d<double>( min_x, min_y, min_z ) );
    vpgl_calibration_matrix<double> K;
    K.set_principal_point( vgl_point_2d<double>( 400, 400 ) );
    double scale = 1000;///sqrt( dx*dx+dy*dy+dz*dz );
    K.set_x_scale( scale ); K.set_y_scale( scale );
    default_camera.set_calibration( K );
    this_camera = default_camera;
  }

  // Otherwise compute a new camera.
  else{
    vidpro1_vsol2D_storage_sptr image_points_storage;
    image_points_storage.vertical_cast(input_data_[0][0]);
    vcl_vector< vsol_spatial_object_2d_sptr > image_points_vsol = 
    image_points_storage->all_data();

    dvpgl_camera_storage_sptr old_camera_storage;
    old_camera_storage.vertical_cast( input_data_[0][1] );
    this_camera = *(old_camera_storage->get_camera());

    vcl_vector< vgl_point_2d<double> > all_image_points;
    for( int i = 0; i < all_world_points.size(); i++ ){
      all_image_points.push_back( vgl_point_2d<double>( 
        image_points_vsol[i]->cast_to_point()->x(),
        image_points_vsol[i]->cast_to_point()->y() ) );
    }

    // Check for changed points.
    vcl_vector< vgl_point_3d<double> > set_world_points;
    vcl_vector< vgl_point_2d<double> > set_image_points;
    for( int p = 0; p < all_world_points.size(); p++ ){
      vgl_homg_point_2d<double> old_ip_homg = this_camera.project( 
        vgl_homg_point_3d<double>( all_world_points[p] ) );
      vgl_point_2d<double> old_ip( 
        old_ip_homg.x()/old_ip_homg.w(), old_ip_homg.y()/old_ip_homg.w() );
      if( vgl_distance( old_ip, all_image_points[p] ) > .001 ){
        set_world_points.push_back( all_world_points[p] );
        set_image_points.push_back( all_image_points[p] );
      }
    }

    // Get marked "up" lines in the image.
    vcl_vector< vgl_line_segment_2d<double> > up_lines;
    for( int i = all_world_points.size(); i < image_points_vsol.size(); i++ ){
      if( image_points_vsol[i]->cast_to_curve() == NULL ) {
        vcl_cerr << "\nERROR: expected a vsol_line.";
        continue;
      }
      vsol_line_2d* new_line_vsol = 
        image_points_vsol[i]->cast_to_curve()->cast_to_line();
      vgl_line_segment_2d<double> new_line( 
        vgl_point_2d<double>( new_line_vsol->p0()->x(), new_line_vsol->p0()->y() ),
        vgl_point_2d<double>( new_line_vsol->p1()->x(), new_line_vsol->p1()->y() ) );
      up_lines.push_back( new_line );
    }

    // If enough points have been set, compute a new camera.
    if( set_world_points.size() > 5 ){
      dbccl_manual_camera mc;
      mc.compute( set_world_points, set_image_points, up_lines, this_camera );
    }
  }

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
  if( do_first_pass ){
    do_first_pass = false;
    return true;
  }

  bpro1_filepath camera_path;
  parameters()->get_value( "-dbcclcf" , camera_path );
  vcl_string camera_file = camera_path.path;

  vcl_ofstream camera_stream( camera_file.c_str() );
  for( int i = 0; i < input_data_.size(); i++ ){
    dvpgl_camera_storage_sptr cam_storage;
    cam_storage.vertical_cast( input_data_[i][3] );
    camera_stream << "Frame " << i << '\n' << cam_storage->get_camera()->get_matrix() << '\n';
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



