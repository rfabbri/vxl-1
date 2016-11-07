#ifndef line_scene_h_
#define line_scene_h_

//:
// \file
// \brief A basic 2d renderer.
// \author Thomas Pollard
// \date 06/28/06
//   This scene contains a set of lines in the x,y plane that will be rendered using the
// "render_arc" function.  This function renders the scene from "num_frames" views in an 
// arc around the origin at angles measured clockwise from the positive y-axis.
//   All of these 1d views will be compiled as rows in a single picture file.  The 
// first two pixels p1, p2 in each row determine the angle of that view by the forumla:
// angle = p1*10 + p2/10 which will give the angle in degrees to 1 decimal place.

#include <vcl_string.h>
#include <vgl/vgl_point_2d.h>


class line_scene
{
public:

  line_scene() :
    num_pixels_( 400 ),
    view_diameter_( 2 ),
    background_color_( 1.0 ){}

  // Add a face to the scene.
  void add_line( 
    vgl_point_2d<float> start_point,
    vgl_point_2d<float> end_point,
    float color );

  // Render the scene in an arc around the origin.
  bool render_arc( 
    vcl_string image_file,
    int starting_angle,
    int end_angle,
    int num_frames );

  // Render the whole 2d scene, for viewing purposes.
  bool render_2d_scene(
    vcl_string image_file );

  // Rendering parameters:
  int num_pixels_;
  float view_diameter_;
  float background_color_;


private:

  vcl_vector< vgl_point_2d<float> > start_points, end_points;
  vcl_vector< float > colors;

};




#endif // simple_scene_h_
