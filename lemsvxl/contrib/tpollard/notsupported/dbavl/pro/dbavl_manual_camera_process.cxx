#include "dbavl_manual_camera_process.h"

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_curve_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vul/vul_awk.h>



//-------------------------------------------
dbavl_manual_camera_process::dbavl_manual_camera_process() : bpro1_process()
{
  if( !parameters()->add( 
    "World points file" , "-dbavlwpf" , bpro1_filepath("","*") ) ||
    !parameters()->add( 
    "Camera file" , "-dbavlcf" , bpro1_filepath("","*") ) ){
    vcl_cerr << "ERROR: Adding parameters in " __FILE__<< vcl_endl;
  }
}


//---------------------------------------------
dbavl_manual_camera_process::~dbavl_manual_camera_process()
{
}


//--------------------------------------------
bpro1_process* 
dbavl_manual_camera_process::clone() const
{
  return new dbavl_manual_camera_process(*this);
}


//------------------------------------------
vcl_string
dbavl_manual_camera_process::name()
{
  return "Manual Camera Computation";
}


//-----------------------------------------------
vcl_vector< vcl_string > dbavl_manual_camera_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//----------------------------------------------------
vcl_vector< vcl_string > dbavl_manual_camera_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Run the process on the current frame
bool
dbavl_manual_camera_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_harris_corner_process::execute() - not exactly one"
          << " input image \n";
    return false;
  }
  clear_output();

  // Get the world points.
  bpro1_filepath wp_path;
  parameters()->get_value( "-dbavlwpf" , wp_path );
  vcl_string world_point_file = wp_path.path;

  vcl_vector< vgl_point_3d<double> > all_world_points;
  vcl_ifstream wp_stream( world_point_file.c_str() );
  vul_awk awk( wp_stream );
  while( awk ){
    vgl_point_3d<double> new_point( atof(awk[0]), atof(awk[1]), atof(awk[2]) );
    all_world_points.push_back( new_point );
    ++awk;
  }

  // Get the image points and lines.
  vidpro1_vsol2D_storage_sptr image_points_storage;
  image_points_storage.vertical_cast(input_data_[0][0]);
  vcl_vector< vsol_spatial_object_2d_sptr > image_points_vsol = 
    image_points_storage->all_data();
  // Do nothing if not enough points supplied.
  if( image_points_vsol.size() < all_world_points.size() ){
    vcl_cerr << "ERROR: not enough image constraints provided.\n";
    return true;
  }

  vcl_vector< vgl_point_2d<double> > image_points;
  vcl_vector< vgl_point_3d<double> > world_points;
  vcl_vector< vgl_line_segment_2d<double> > up_lines;
  int c;
  for( c = 0; c < all_world_points.size(); c++ ){
    if( image_points_vsol[c]->cast_to_point() == NULL ){
      vcl_cerr << "ERROR: expected a vsol_point.\n";
      continue;
    }
    vgl_point_2d<double> new_image_point( 
      image_points_vsol[c]->cast_to_point()->x(),
      image_points_vsol[c]->cast_to_point()->y() );
    if( new_image_point.x() < 0 || new_image_point.y() < 0 )
      continue;
    image_points.push_back( new_image_point );
    world_points.push_back( all_world_points[c] );
  }
  for( c; c < image_points_vsol.size(); c++ ){
    if( image_points_vsol[c]->cast_to_curve() == NULL ) {
      vcl_cerr << "\nERROR: expected a vsol_line.";
      continue;
    }
    vsol_line_2d* new_line_vsol = 
      image_points_vsol[c]->cast_to_curve()->cast_to_line();
    vgl_line_segment_2d<double> new_line( 
      vgl_point_2d<double>( new_line_vsol->p0()->x(), new_line_vsol->p0()->y() ),
      vgl_point_2d<double>( new_line_vsol->p1()->x(), new_line_vsol->p1()->y() ) );
    up_lines.push_back( new_line );
  }

  vcl_cerr << "\nThis frame's constraints:\n Image-world point pairs:\n";
  for( int i = 0; i < image_points.size(); i++ )
    vcl_cerr << "  " << image_points[i] << world_points[i] << '\n';
  vcl_cerr << " Up lines:\n";
  for( int i = 0; i < up_lines.size(); i++ )
    vcl_cerr << "  " << up_lines[i] << '\n';

  // Compute the camera and store it.
  vpgl_proj_camera<double> camera;
  dbavl_manual_camera mc;
  if( !mc.compute( world_points, image_points, up_lines, camera ) ){
    computed_cameras.push_back( vpgl_proj_camera<double>() );
    return true;
  }
  computed_cameras.push_back( camera );

  // Reproject the world points for verification.
  vcl_vector< vsol_spatial_object_2d_sptr > reproj_world_points;
  for( int i = 0; i < all_world_points.size(); i++ ){
    vgl_homg_point_2d<double> ip = camera.project( 
      vgl_homg_point_3d<double>( all_world_points[i] ) );
    vsol_spatial_object_2d_sptr new_point_obj = 
      new vsol_point_2d( ip.x()/ip.w(), ip.y()/ip.w() );
    reproj_world_points.push_back( new_point_obj );
  }
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects( reproj_world_points, "reprojected world points");
  output_data_[0].push_back(output_vsol);

  return true;
}


//------------------------------------------
bool
dbavl_manual_camera_process::finish()
{
  bpro1_filepath camera_path;
  parameters()->get_value( "-dbavlcf" , camera_path );
  vcl_string camera_file = camera_path.path;

  vcl_ofstream camera_stream( camera_file.c_str() );
  for( int i = 0; i < computed_cameras.size(); i++ )
    camera_stream << "Frame " << i << '\n' << computed_cameras[i].get_matrix() << '\n';
  return true;
}


//-------------------------------------------------
int
dbavl_manual_camera_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbavl_manual_camera_process::output_frames()
{
  return 1;
}



