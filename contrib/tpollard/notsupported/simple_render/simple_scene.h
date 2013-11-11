#ifndef simple_scene_h_
#define simple_scene_h_

//:
// \file
// \brief A basic 3d renderer
// \author Thomas Pollard
// \date 04/17/06

#include <vcl_string.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_proj_camera.h>

class simple_scene_face;


class simple_scene
{
public:

  simple_scene() :
    image_height_( 400 ),
    image_width_( 400 ),
    background_color_( 255 ),
    directional_lighting_( true ),
    scene_rad_( 2.0 ){}

  // Render the scene with all of the provided cameras.
  bool render( 
    vcl_vector< vpgl_proj_camera<double> > cameras,
    vcl_vector< vgl_vector_3d<double> > lights,
    vcl_string image_directory,
    vcl_string camera_file,
    vcl_string light_file );

  // Render views of the scene in a circle around the world origin with the camera
  // pointed at the origin.
  bool render_360(
    int num_views,
    vcl_string image_directory,
    vcl_string camera_file,
    vcl_string light_file,
    bool random_views = false );

  // Rendering parameters:
  int image_height_;
  int image_width_;
  int background_color_;
  bool directional_lighting_;
  double scene_rad_;


private:

  // Get the color of a cone model along the ray p+tv.
  int cone_color(
    const vgl_point_3d<double>& p,
    const vgl_vector_3d<double>& v,
    const vgl_vector_3d<double>& light );

  //
  int cube_color(
    const vgl_point_3d<double>& p,
    const vgl_vector_3d<double>& v,
    vgl_point_3d<double>& contact_point,
    vgl_vector_3d<double>& contact_normal );


};



#endif // simple_scene_h_
