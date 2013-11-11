#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_matrix.h>
#include <vbl/vbl_bounding_box.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_resample_bilin.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/algo/vpgl_list.h>
#include <vcl_cstdlib.h>

// Given cameras of a registered scene, use this app to get the cameras of the original
// scene, assuming it was registered with dbirl or dbvrl.


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


//-------------------------------------------
int main( int argc, char* argv[] )
{
  if( argc!=7 ) {
    vcl_cout<<"Usage : dbrcl_unregister_app input_cameras input_homogs unregistered_images output_cameras homog_spacing homog_start\n";
    return -1;
  }

  vcl_string input_cameras( argv[1] );
  vcl_string input_homogs( argv[2] );
  vcl_string unregistered_images( argv[3] );
  vcl_string output_cameras( argv[4] );
  int spacing = vcl_atoi( argv[5] );
  int offset = vcl_atoi( argv[6] );

  vcl_vector< vpgl_proj_camera<double> > camera_list, output_camera_list;
  vcl_vector< vgl_h_matrix_2d<double> > homog_list;
  if( !vpgl_read_list( camera_list, input_cameras ) )
    return -1;
  if( !vpgl_read_list( homog_list, input_homogs ) )
    return -1;

  
  // Find the offset of the original images.
  vcl_vector<vcl_string> in_filenames;
  filenames_from_directory( unregistered_images, in_filenames );
  unsigned n_infiles = in_filenames.size();
  unsigned infile_counter = 0;
      //read the first image
      bool no_valid_image = true;
      vil_image_resource_sptr imgr;
      while(no_valid_image)
        {
          imgr = 
            vil_load_image_resource(in_filenames[infile_counter++].c_str());
          no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
          if(infile_counter>=n_infiles)
            return -1;
        }
      unsigned ni =  imgr->ni(), nj =  imgr->nj(); 
      infile_counter = 0;//return to first frame

  vcl_vector<vimt_transform_2d > xforms;  
  vbl_bounding_box<double,2> box;

  for(unsigned i=0;i<homog_list.size();i++)
    {
      vimt_transform_2d p;
      p.set_affine(homog_list[i].get_matrix().extract(2,3));
      xforms.push_back(p);
      box.update(p(0,0).x(),p(0,0).y());
      box.update(p(0,nj).x(),p(0,nj).y());
      box.update(p(ni,0).x(),p(ni,0).y());
      box.update(p(ni,nj).x(),p(ni,nj).y());
    }
  
  int offset_i=(int)vcl_ceil(0-box.min()[0]);
  int offset_j=(int)vcl_ceil(0-box.min()[1]);

  //unused int bimg_ni=(int)vcl_ceil(box.max()[0]-box.min()[0]);
  //unused int bimg_nj=(int)vcl_ceil(box.max()[1]-box.min()[1]);

  vgl_h_matrix_2d<double> shift_homog;
  shift_homog.set_identity();
  shift_homog.set_translation( offset_i, offset_j );

  for( int i = 0; i < static_cast<int>(camera_list.size()); i++ ){
    if( offset+spacing*i >= static_cast<int>(homog_list.size()) ) 
      return -1;
    vpgl_proj_camera<double> old_camera = camera_list[i];
    vgl_h_matrix_2d<double> homog = shift_homog*homog_list[ offset+spacing*i ];
    vpgl_proj_camera<double> new_camera = premultiply( old_camera, homog.get_inverse() );
    output_camera_list.push_back( new_camera );
  }

  if( !vpgl_write_list( output_camera_list, output_cameras ) )
    return -1;

  return 0;
}

