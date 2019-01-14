#include <string>
#include <vector>
#include<cstdio>
#include <vbl/vbl_bounding_box.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>
#include <vul/vul_sequence_filename_map.h>
#include <vul/vul_file_iterator.h>
#include <vector>
#include <vnl/vnl_matrix.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <dbvrl/dbvrl_minimizer.h>
#include <dbvrl/dbvrl_world_roi.h>
#include <dbvrl/dbvrl_transform_2d.h>
#include <dbvrl/dbvrl_transform_2d_sptr.h>
#include <dbvrl/dbvrl_region.h>
#include <dbvrl/dbvrl_region_sptr.h>
#include <vimt/vimt_transform_2d.h>
#include <vimt/vimt_resample_bilin.h>
#include <vbl/vbl_bounding_box.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_track.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbirl/dbirl_h_computation.h>
#include <cstdlib>

enum dbirl_homography_type{
  dbirl_affine,
  dbirl_similarity };

//---------------------------------------------
static void filenames_from_directory(std::string const& dirname,
                                     std::vector<std::string>& filenames)
{  std::string s(dirname);
  s += "/*.*";
  for (vul_file_iterator fit = s;fit; ++fit) {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    filenames.push_back(fit());
  }
}

//-------------------------------------------
static bool read_tracks(
  std::string const& track_file_name, 
  std::vector< dbinfo_track_geometry_sptr >& tracks )
{
  tracks.clear();
  vsl_b_ifstream is( track_file_name );
  dbinfo_track_storage_sptr track_storage = new dbinfo_track_storage();
  track_storage->b_read( is );
  std::vector< dbinfo_track_sptr > track_list = track_storage->tracks();
  for( int i = 0; i < static_cast<int>(track_list.size()); i++ )
    tracks.push_back( track_list[i]->track_geometry() );
  return true;
}

static bool register_images(std::string const& track_file_name,
                     std::string const& image_indir,
                     std::string const& image_outdir,
                     dbirl_homography_type homography_type,
                     int ground_frame, 
                     std::string homography_file_name ){
  int bimg_ni;
  int bimg_nj;

  int offset_i;
  int offset_j;

  vbl_bounding_box<double,2> box;

  // Read the track file.
  std::cerr << "\ndbirl_app: reading track file: ";
  std::vector< dbinfo_track_geometry_sptr > tracks;
  read_tracks(track_file_name, tracks );
  std::cerr << tracks.size() << " tracks.\n";

  // Compute the homographies.
  std::cerr << "dbirl_app: computing homographies.\n";
  std::vector< vgl_h_matrix_2d<double> > homographies;
  dbirl_h_computation hc;
  if( homography_type == dbirl_similarity )
    hc.compute_similarity( tracks, homographies, ground_frame );
  else
    hc.compute_affine( tracks, homographies, ground_frame );
  if( homography_file_name != "none" ){
    std::ofstream ofp( homography_file_name.c_str() );
    for( int i = 0; i < static_cast<int>(homographies.size()); i++ ){
      ofp << "Frame No " <<i<<"\n";
      ofp << homographies[i];
    }
  }

  // Register the images.
  std::cerr << "dbirl_app: registering images.\n";

  unsigned nframes = homographies.size();
  if(!nframes)
    {
      std::cout << "no transforms to use in registration\n";
      return false;
    }
  std::vector<std::string> in_filenames;
  filenames_from_directory(image_indir, in_filenames);
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
            return false;
        }
      unsigned ni =  imgr->ni(), nj =  imgr->nj(); 
      infile_counter = 0;//return to first frame

  std::vector<vimt_transform_2d > xforms;  
  for(unsigned i=0;i<nframes;i++)
    {
      vimt_transform_2d p;
      p.set_affine(homographies[i].get_matrix().extract(2,3));
      xforms.push_back(p);
      box.update(p(0,0).x(),p(0,0).y());
      box.update(p(0,nj).x(),p(0,nj).y());
      box.update(p(ni,0).x(),p(ni,0).y());
      box.update(p(ni,nj).x(),p(ni,nj).y());
    }
  
  bimg_ni=(int)std::ceil(box.max()[0]-box.min()[0]);
  bimg_nj=(int)std::ceil(box.max()[1]-box.min()[1]);

  offset_i=(int)std::ceil(0-box.min()[0]);
  offset_j=(int)std::ceil(0-box.min()[1]);

  std::string outfile = image_outdir + "/reg";

  for(unsigned frame = 0;frame<nframes; ++frame)
    {
      no_valid_image = true;
      while(no_valid_image)
        {
          imgr = 
            vil_load_image_resource(in_filenames[infile_counter++].c_str());
          no_valid_image = !imgr||imgr->ni()==0||imgr->nj()==0;
          if(infile_counter>=n_infiles)
            {
              std::cout << "Number of homographies and input images do not match\n";
              return false;
            }
        }
      vil_image_view<float> curr_view =
        brip_vil_float_ops::convert_to_float(*imgr);
  
        vimt_transform_2d ftxform=xforms[frame].inverse();
        vimt_image_2d_of<float> sample_im;

        vgl_point_2d<double> p(-offset_i,-offset_j);
        vgl_vector_2d<double> u(1,0);
        vgl_vector_2d<double> v(0,1);

        vimt_image_2d_of<float> curr_img(curr_view,ftxform);
        vimt_resample_bilin(curr_img,sample_im,p,u,v,bimg_ni,bimg_nj);

        vil_image_resource_sptr outresc = 
          vil_new_image_resource_of_view(brip_vil_float_ops::convert_to_byte(sample_im.image()));
        std::string outname = vul_sprintf("%s%05d.%s", outfile.c_str(),
                                             frame,
                                             "tif");
        vil_save(brip_vil_float_ops::convert_to_byte(outresc), 
                                                     outname.c_str());
    }
    return true;
}

//-------------------------------------------
int main(int argc,char * argv[])
{
    if(!(argc==6 || argc==7))
    {
        std::cout<<"Usage : dbirl_app track_file image_in_dir image_out_dir homography_type ground_frame homography_file(optional)\n";
        return -1;
    }
    else
    {
        std::string track_file_name(argv[1]);
        std::string image_indir(argv[2]);
        std::string image_outdir(argv[3]);
        std::string homography_type(argv[4]);
        int ground_frame = std::atoi(argv[5]);
        std::string homography_file_name("none");
        if( argc == 7 ) homography_file_name = std::string( argv[6] );
        dbirl_homography_type type;
        if( homography_type == "affine" )
          type = dbirl_affine;
        else if( homography_type == "similarity" )
          type = dbirl_similarity;
        else{
          std::cout << "Invalid homography type: use 'affine' or 'similarity'\n";
          return -1;
        }

        if(!register_images(track_file_name, image_indir, image_outdir, type, ground_frame, homography_file_name ))
          {  
            std::cout << "Registration failed \n";
            return -1;
          }
        return 0;
    }
}
