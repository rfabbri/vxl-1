#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/algo/vpgl_list.h>
#include <dbrcl/dbrcl_compute_constrained_cameras.h>


//---------------------------------------------
static void filenames_from_directory(vcl_string const& dirname,
                                     vcl_vector<vcl_string>& filenames)
{  vcl_string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }
}


//---------------------------------
void mark_points(
  const vil_image_resource_sptr image_in,
  vcl_string image_out,
  vcl_vector< vgl_point_2d<double> > points )
{
  vil_image_view<vxl_byte> I = 
    vil_convert_to_grey_using_average( vil_convert_cast(vxl_byte(), image_in->get_view()) );
  for( int i = 0; i < static_cast<int>(points.size()); i++ ){
    int x = static_cast<int>(floor( points[i].x() )), y = static_cast<int>(floor( points[i].y() ));
    if( x < 0 ) x = 0;
    if( y < 0 ) y = 0;
    if( x >= static_cast<int>(I.ni()) ) x = I.ni()-1;
    if( y >= static_cast<int>(I.nj()) ) y = I.nj()-1;
    //vcl_cerr << points[i].x() << " " << points[i].y() << '\n';//I( points[i].x(), points[i].y() ) << '\n';
    I( x, y ) = 0;
  }
  vil_save( I, image_out.c_str() );
}


//-------------------------------------------
int main( int argc, char* argv[] )
{
  if( argc!=5 ){
    vcl_cout << "Usage : dbrcl_reproject_app constraint_file camera_file image_in_dir image_out_dir\n";
    return -1;
  }

  vcl_string constraint_file( argv[1] );
  vcl_string camera_file( argv[2] );
  vcl_string image_in_dir( argv[3] );
  vcl_string image_out_dir( argv[4] );

  // Get cameras.
  vcl_vector< vpgl_proj_camera<double> > camera_list;
  if( !vpgl_read_list( camera_list, camera_file ) )
    return -1;

  // Get the world points to project.
  dbrcl_compute_constrained_cameras ccc;
  ccc.verbose = false;
  if( !ccc.read_constraint_file( constraint_file ) )
    return -1;
  vcl_vector< vgl_point_3d<double> > world_points;
  ccc.get_world_points( world_points );

  // Load the image files.
  vcl_vector<vcl_string> images_in;
  filenames_from_directory( image_in_dir, images_in );
  vcl_vector< vil_image_resource_sptr > images;
  for( int i = 0; i < static_cast<int>(images_in.size()); i++ ){
    if( images_in[i].find("Thumbs.db" ) < 10000 ) continue; // Windows BS
    images.push_back( vil_load_image_resource( images_in[i].c_str() ) );
  }

  // Mark the images.
  for( int i = 0; i < static_cast<int>(camera_list.size()); i++ ){
    vpgl_proj_camera<double> this_camera = camera_list[i];
    vcl_vector< vgl_point_2d<double> > image_points;
    for( int j = 0; j < static_cast<int>(world_points.size()); j++ )
      image_points.push_back( this_camera.project( 
        vgl_homg_point_3d<double>( world_points[j] ) ) );

    vil_image_resource_sptr this_image = 
      vil_load_image_resource( images_in[i].c_str() );
    vcl_stringstream new_image_name;
    if( i < 10 )
      new_image_name << image_out_dir << "\\000" << i << ".png";
    else if( i < 100 )
      new_image_name << image_out_dir << "\\00" << i << ".png";
    else if( i < 1000 )
      new_image_name << image_out_dir << "\\0" << i << ".png";
    else
      new_image_name << image_out_dir << "\\" << i << ".png";
    mark_points( this_image, new_image_name.str(), image_points ); 
  }


  return 0;
}

