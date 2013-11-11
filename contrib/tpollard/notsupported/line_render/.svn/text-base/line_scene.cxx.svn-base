#ifndef line_scene_cxx_
#define line_scene_cxx_

//:
// \file
// \brief A basic 2d renderer
// \author Thomas Pollard
// \date 06/28/06

#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include "line_scene.h"


//--------------------------------------------
void 
line_scene::add_line( 
  vgl_point_2d<float> start_point,
  vgl_point_2d<float> end_point,
  float color )
{
  start_points.push_back( start_point );
  end_points.push_back( end_point );
  colors.push_back( color );
};


//--------------------------------------------
bool 
line_scene::render_arc( 
  vcl_string image_file,
  int start_angle,
  int end_angle,
  int num_frames )
{
  vil_image_view<vxl_byte> new_image( 2+num_pixels_, num_frames );
  for( int i = 0; i < new_image.ni(); i++ )
    for( int j = 0; j < new_image.nj(); j++ )
      new_image(i,j) = (int)floor( background_color_*255 );

  // Render each view.
  for( int i = 0; i < num_frames; i++ ){
    vnl_vector<double> z_buffer( num_pixels_, -100000 );

    // Determine the angle for this view and encode it in the first two image pixels.
    float this_angle = start_angle + i*(end_angle-start_angle)/(num_frames-1.0);
    while( this_angle < 0 || this_angle > 360 ) 
      this_angle -= 360*this_angle/fabs(this_angle);

    int bit1 = (int)floor(this_angle/10);
    int bit2 = (int)floor(((this_angle/10)-bit1)*100);
    assert( fabs( bit1*10+bit2/10.0-this_angle ) < 1.0 );
    new_image( 0, i ) = bit1; new_image( 1, i ) = bit2;

    float a = num_pixels_*cos(3.1416*this_angle/180.0)/(float)view_diameter_;
    float b = -num_pixels_*sin(3.1416*this_angle/180.0)/(float)view_diameter_;
    float c = num_pixels_/2.0;

    // Render each line in the scene.
    for( int l = 0; l < start_points.size(); l++ ){
      float start_proj = a*start_points[l].x() + b*start_points[l].y() + c;
      float end_proj = a*end_points[l].x() + b*end_points[l].y() + c;
      float start_depth = start_points[l].x()*sin(3.1416*this_angle/180.0) +
        start_points[l].y()*cos(3.1416*this_angle/180.0);
      float end_depth = end_points[l].x()*sin(3.1416*this_angle/180.0) +
        end_points[l].y()*cos(3.1416*this_angle/180.0);
      if( start_proj > end_proj ){
        float temp_end_proj = end_proj; end_proj = start_proj; 
        start_proj = temp_end_proj;
        float temp_end_depth = end_depth; end_depth = start_depth; 
        start_depth = temp_end_depth;
      }
      start_proj = floor( start_proj );
      end_proj = ceil( end_proj );
      int num_line_pixels = static_cast<int>(end_proj-start_proj+1);
      for( int p = 0; p < num_line_pixels; p++ ){
        if( start_proj+p < 0 ) continue;
        if( start_proj+p >= num_pixels_ ) break;
        float this_depth = 
          start_depth*p/(num_line_pixels-1.0) + 
          end_depth*(1-p/(num_line_pixels-1.0));
        if( z_buffer( start_proj+p ) < this_depth ){
          z_buffer( start_proj+p ) = this_depth;
          new_image( 2+start_proj+p, i ) = static_cast<unsigned>(floor(255*colors[l]));
        }
      }
    }

  }

  // Now save the image to disk.
  vil_save( new_image, image_file.c_str() );
  return true;
};


//--------------------------------------------------
bool 
line_scene::render_2d_scene(
  vcl_string image_file )
{
  float image_rad = 200;
  vil_image_view<vxl_byte> new_image( 2*image_rad, 2*image_rad );
  for( int i = 0; i < new_image.ni(); i++ )
    for( int j = 0; j < new_image.nj(); j++ )
      new_image(i,j) = (int)floor( background_color_*255 );

  // Find a volume to hold all of the lines.
  float maxp = -10000;
  for( int i = 0; i < start_points.size(); i++ ){
    if( fabs(start_points[i].x()) > maxp ) maxp = fabs(start_points[i].x());
    if( fabs(start_points[i].y()) > maxp ) maxp = fabs(start_points[i].y());
    if( fabs(end_points[i].x()) > maxp ) maxp = fabs(end_points[i].x());
    if( fabs(end_points[i].y()) > maxp ) maxp = fabs(end_points[i].y());
  }
  float pmult = .9*image_rad/maxp;

  // Draw each line.
  for( int l = 0; l < start_points.size(); l++ ){
    vgl_point_2d<float> image_start(
      start_points[l].x()*pmult + image_rad, -start_points[l].y()*pmult + image_rad );
    vgl_point_2d<float> image_end(
      end_points[l].x()*pmult + image_rad, -end_points[l].y()*pmult + image_rad );
    for( float t = 0; t <= 1; t+=1.0/(3.0*image_rad) ){
      vgl_point_2d<float> this_pixel( t*image_start.x() + (1-t)*image_end.x(),
        t*image_start.y() + (1-t)*image_end.y() );
      new_image( (int)this_pixel.x(), (int)this_pixel.y() ) = (int)floor(255*colors[l]);
    }
  }

  // Now save the image to disk.
  vil_save( new_image, image_file.c_str() );
  return true;
};


#endif // simple_scene_cxx_
