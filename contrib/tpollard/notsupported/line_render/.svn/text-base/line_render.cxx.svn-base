#include <vcl_string.h>

#include "line_scene.h"

void create_2_line_scene( line_scene* scene );
void create_1_line_scene( line_scene* scene );
void create_1_box_scene( line_scene* scene );
void create_2_box_scene( line_scene* scene );

//-------------------------------------------
int main( int argc, char* argv[] )
{
  line_scene scene;
  create_2_box_scene( &scene );
  vcl_string image_file1( "c:\\rotation.tif" );
  vcl_string image_file2( "c:\\fullview.tif" );
  scene.render_2d_scene( image_file2 );
  scene.render_arc( image_file1, 0, 360, 30 );
  return 0;
}


//------------------------------------------
void 
create_2_line_scene( 
  line_scene* scene )
{
  scene->num_pixels_ = 400;
  scene->view_diameter_ = 10;

  scene->add_line( vgl_point_2d<float>( -2, 2 ), vgl_point_2d<float>( 2, 2 ), .2 );
  scene->add_line( vgl_point_2d<float>( -2, -2 ), vgl_point_2d<float>( 2, -2 ), .8 );
};


//------------------------------------------
void 
create_1_line_scene( 
  line_scene* scene )
{
  scene->num_pixels_ = 400;
  scene->view_diameter_ = 10;

  scene->add_line( vgl_point_2d<float>( 2, 2 ), vgl_point_2d<float>( 2, -2 ), .1 );
};


//------------------------------------------
void 
create_1_box_scene( 
  line_scene* scene )
{
  scene->num_pixels_ = 400;
  scene->view_diameter_ = 10;

  scene->add_line( vgl_point_2d<float>( 2, 2 ), vgl_point_2d<float>( 2, -2 ), .1 );
  scene->add_line( vgl_point_2d<float>( 2, 2 ), vgl_point_2d<float>( -2, 2 ), .3 );
  scene->add_line( vgl_point_2d<float>( -2, -2 ), vgl_point_2d<float>( 2, -2 ), .5 );
  scene->add_line( vgl_point_2d<float>( -2, -2 ), vgl_point_2d<float>( -2, 2 ), .7 );
};


//------------------------------------------
void 
create_2_box_scene( 
  line_scene* scene )
{
  scene->num_pixels_ = 400;
  scene->view_diameter_ = 10;

  scene->add_line( vgl_point_2d<float>( 1, 1 ), vgl_point_2d<float>( 2, 1 ), .1 );
  scene->add_line( vgl_point_2d<float>( 2, 1 ), vgl_point_2d<float>( 2, -1 ), .2 );
  scene->add_line( vgl_point_2d<float>( 2, -1 ), vgl_point_2d<float>( 1, -1 ), .3 );
  scene->add_line( vgl_point_2d<float>( 1, -1 ), vgl_point_2d<float>( 1, 1 ), .4 );

  scene->add_line( vgl_point_2d<float>( -1, 1 ), vgl_point_2d<float>( -2, 1 ), .5 );
  scene->add_line( vgl_point_2d<float>( -2, 1 ), vgl_point_2d<float>( -2, -1 ), .6 );
  scene->add_line( vgl_point_2d<float>( -2, -1 ), vgl_point_2d<float>( -1, -1 ), .7 );
  scene->add_line( vgl_point_2d<float>( -1, -1 ), vgl_point_2d<float>( -1, 1 ), .8 );
};
