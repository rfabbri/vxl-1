#ifndef simple_scene_cxx_
#define simple_scene_cxx_

//:
// \file
// \brief A basic 3d renderer
// \author Thomas Pollard
// \date 04/17/06

#include <vnl/vnl_det.h>
#include <vgl/vgl_distance.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vpgl\algo\vpgl_list.h>

#include "simple_scene.h"



//--------------------------------------------
bool 
simple_scene::render( 
  vcl_vector< vpgl_proj_camera<double> > cameras,
  vcl_vector< vgl_vector_3d<double> > lights,
  vcl_string image_directory,
  vcl_string camera_file,
  vcl_string light_file )
{
  // Check that the directory is good and create file names.
  vcl_vector< vcl_string > image_files;
  for( int i = 0; i < static_cast<int>(cameras.size()); i++ ){
    vcl_stringstream new_file_name;
    new_file_name << image_directory << "\\";
    if( i < 10 ) new_file_name << "000" << i;
    else if( i < 100 ) new_file_name << "00" << i;
    else if( i < 1000 ) new_file_name << "0" << i;
    else new_file_name << i;
    new_file_name << ".tif";
    image_files.push_back( new_file_name.str() );
  }
  vcl_ofstream camera_stream( camera_file.c_str() );
  vcl_ofstream light_stream( light_file.c_str() );

  // Now render an image for each camera.
  for( int f = 0; f < static_cast<int>(cameras.size()); f++ ){
    vcl_cerr << "\nRendering Image " << f << " with camera:\n" <<
      cameras[f].get_matrix() << '\n';
    
    vil_image_view<vxl_byte> new_image( image_width_, image_height_ ); // maybe backwards
    
    // Ray trace each pixel.
    for( int i = 0; i < image_width_; i++ ){
      for( int j = 0; j < image_height_; j++ ){

        // Get the ray.
        vgl_homg_line_3d_2_points<double> line = 
          cameras[f].backproject( vgl_homg_point_2d<double>(i,j) );
        
        vgl_point_3d<double> p(
          cameras[f].camera_center() );;
        vgl_vector_3d<double> v( 
          line.point_infinite().x(),
          line.point_infinite().y(),
          line.point_infinite().z() );
        if( dot_product( v, vgl_vector_3d<double>( -p.x(), -p.y(), -p.z() ) ) < 0 )
          v = -v;

        // Find the cone color on this ray.
        //new_image(i,j) = cone_color( p, v, lights[f] );
        vgl_point_3d<double> contact_point, dummy_point;
        vgl_vector_3d<double> contact_normal, dummy_normal;
        int contact_color = cube_color( p, v, contact_point, contact_normal );
        int occlusion = cube_color( 
          contact_point, lights[f], dummy_point, dummy_normal );
        if( occlusion != 0 )
          new_image(i,j) = 0;
        else
          new_image(i,j) = (int)floor( contact_color*
            abs( dot_product( contact_normal, lights[f] ) ) );
      }
    }

    vil_save( new_image, image_files[f].c_str() );
    camera_stream << "FRAME " << f << '\n' << cameras[f].get_matrix() << "\n";
    light_stream << lights[f].x() << ' ' << lights[f].y() 
      << ' ' << lights[f].z() << "\n";
  }
  return true;
};


//----------------------------------------------
bool 
simple_scene::render_360(
  int num_views,
  vcl_string image_directory,
  vcl_string camera_file,
  vcl_string light_file,
  bool random_views )
{
  vgl_point_3d<double> cam_start( 0, 100, 50 );

  vpgl_calibration_matrix<double> new_K;
  new_K.set_focal_length( 100 );
  new_K.set_x_scale( image_width_/(2.0*scene_rad_) );
  new_K.set_y_scale( image_height_/(2.0*scene_rad_) );
  new_K.set_principal_point( 
    vgl_point_2d<double>( image_width_/2.0, image_height_/2.0 ) );

  vcl_vector< vgl_vector_3d<double> > lights;
  vcl_vector< vpgl_proj_camera<double> > cameras;

  for( int f = 0; f < num_views; f++ ){

    // Get the camera center.
    double camera_angle;
    if( random_views )
      camera_angle = 2*3.14159*rand()/(double)RAND_MAX;
    else
      camera_angle = 2*3.14159*f/num_views;
    double rx = cos( camera_angle ), ry = sin( camera_angle );
    double scale = 1.0;
    if( random_views ) scale = rand()/(double)RAND_MAX;
    vgl_point_3d<double> camera_center( scale*(cam_start.x()*rx-cam_start.y()*ry), 
      scale*(cam_start.x()*ry+cam_start.y()*rx), cam_start.z() );

    // Set up the camera.
    vpgl_perspective_camera<double> new_camera;
    new_camera.set_calibration( new_K );
    new_camera.set_camera_center( camera_center );
    new_camera.look_at( vgl_homg_point_3d<double>(0,0,0) );
    cameras.push_back( new_camera );

    // Set up the lights.
    vgl_vector_3d<double> light( -1+2*rand()/(double)RAND_MAX,
      -1+2*rand()/(double)RAND_MAX, .5+.5*rand()/(double)RAND_MAX );
    normalize( light );
    lights.push_back( (.5+.5*rand()/(double)RAND_MAX)*light );
  }

  return render( cameras, lights, image_directory, camera_file, light_file );
};


//--------------------------------------------------
int 
simple_scene::cone_color(
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& light )
{  
  double cone_rad = 3.0;
  double cone_top = 1.0;

  // Find the intersection of the ray with the cone.
  double a = v.x()*v.x()+v.y()*v.y()-cone_rad*cone_rad*v.z()*v.z();
  double b = 2*( p.x()*v.x() + p.y()*v.y()+cone_rad*cone_rad*v.z()*(cone_top-p.z()) );
  double c = p.x()*p.x() + p.y()*p.y() - 
    cone_rad*cone_rad*(p.z()-cone_top)*(p.z()-cone_top);
  double descrim = b*b-4*a*c;
  if( descrim < 0 )
     return background_color_;

  double t1 = ( -b-sqrt(descrim) )/(2.0*a);
  double t2 = ( -b+sqrt(descrim) )/(2.0*a);
  vgl_point_3d<double> q1 = p + t1*v;
  vgl_point_3d<double> q2 = p + t2*v;
  vgl_point_3d<double> q;
  if( ( q1.z() > cone_top || q1.z() < 0 ) && ( q2.z() > cone_top || q2.z() < 0 ) )
    return background_color_;
  if( ( q1.z() > cone_top || q1.z() < 0 ) ) q = q2;
  else if( ( q2.z() > cone_top || q2.z() < 0 ) ) q = q1;
  else{
    if( t2 > t1 ) q = q1;
    else q = q2;
  }

  assert( abs( q.x()*q.x()+q.y()*q.y() - 
    cone_rad*cone_rad*( cone_top-q.z() )*( cone_top-q.z() ) ) < .001 );

  if( q.z() > cone_top )
    return background_color_;
  if( q.z() < 0 )
    return background_color_;

  
  // Color the point accordingly.
  double color = 255*( .6+.4*sin( 2*q.x() )*sin( 2*q.y() ) );
  //double color = ( 1-(cone_top-q.z())/cone_top );
  if( color < 1 ) color = 1;

  // Return the lighted color.
  if( !directional_lighting_ )
    return (int)ceil( .9*color );

  vgl_vector_3d<double> n(
    q.x()/(sqrt( (1+cone_rad*cone_rad)*(q.x()*q.x()+q.y()*q.y()) ) ),
    q.y()/(sqrt( (1+cone_rad*cone_rad)*(q.x()*q.x()+q.y()*q.y()) ) ),
    cone_rad/sqrt( 1+cone_rad*cone_rad ) );
  if( dot_product( n, light ) < 0 )
    return 0;

  return (int)ceil( dot_product( n, light )*color );
};


//----------------------------------------------
int 
simple_scene::cube_color(
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& v,
  vgl_point_3d<double>& contact_point,
  vgl_vector_3d<double>& contact_normal )
{
  float cube_rad = 1.0;

  // Intersect the ray with all faces.
  vgl_point_3d<double> closest_point;
  vgl_vector_3d<double> closest_normal;
  float closest_distance = 1000000;
  int closest_color = 0;
  for( int f = 0; f < 6; f++ ){

    float t;
    if( f == 0 ) t = (-cube_rad-p.z())/v.z(); // Base plane
    if( f == 1 ) t = (cube_rad-p.z())/v.z(); // Cube top
    if( f == 2 ) t = (-cube_rad-p.x())/v.x(); // Sides
    if( f == 3 ) t = (cube_rad-p.x())/v.x(); 
    if( f == 4 ) t = (-cube_rad-p.y())/v.y(); 
    if( f == 5 ) t = (cube_rad-p.y())/v.y();
    if( t < 0 || abs(t) < .001 ) continue;

    vgl_point_3d<double> this_point = p+t*v;
    if( f != 0 && ( // Check bounds.
      fabs( this_point.x() ) > cube_rad+.0001 || 
      fabs( this_point.y() ) > cube_rad+.0001 ||
      fabs( this_point.z() ) > cube_rad+.0001 ) ) continue;

    float this_distance = vgl_distance( this_point, p );
    if( this_distance > closest_distance ) continue;
    closest_distance = this_distance;
    closest_point = this_point;
    if( f == 0 ) closest_normal.set( 0, 0, 1 );
    if( f == 1 ) closest_normal.set( 0, 0, 1 );
    if( f == 2 ) closest_normal.set( -1, 0, 0 );
    if( f == 3 ) closest_normal.set( 1, 0, 0 );
    if( f == 4 ) closest_normal.set( 0, -1, 0 );
    if( f == 5 ) closest_normal.set( 0, 1, 0 );

    if( f == 0 ) closest_color = 225;
    if( f == 1 ) closest_color = 175;
    if( f == 2 ) closest_color = 125;
    if( f == 3 ) closest_color = 100;
    if( f == 4 ) closest_color = 75;
    if( f == 5 ) closest_color = 50;
  }
  contact_point = closest_point;
  contact_normal = closest_normal;
  return closest_color;
};

#endif // simple_scene_cxx_
