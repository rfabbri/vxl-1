#include <vul/vul_arg.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_bundle_adjust.h>
#include <vnl/vnl_crs_index.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_double_3.h>

void get_cams(const std::vector<std::string> > &cam_fnames, std::vector<vpgl_perspective_camera<double> > *cams)
{
  assert(cams->size() == 0);
  for (unsigned i=0; i < cam_fnames.size(); ++i) {
    vpgl_perspective_camera<double> cam;
    bmcsd_util::read_cam_anytype(cams_fnames[i], cam_type, &cam);
    cams->push_back(cam);
  }
}

void get_corrs(unsigned ncams, std::vector<std::vector<bool> > *mask)
{ 
  unsigned npts=; // XXX

  std::vector< std::vector<vgl_point_2d<double> > > imgpts_percam(ncams);

  imgpts[0].push_back(vgl_point_2d<double> (x,y,z));


  std::vector<vgl_point_3d<double> > &ini_world = *pworld;
  std::vector<vgl_point_2d<double> > image_points_linearlist;

  for (unsigned c = 0; c < ncams; ++c)
    for (unsigned pw=0; pw < npts; ++i)
      image_points_linearlist.push_back(imgpts_percam[c][pw]);

  // make the mask (using all the points)
  *mask = std::vector<std::vector<bool> > (cameras.size(), std::vector<bool>(npts,true) );
}


void initialize_world_by_triangulation(
    imgpts, 
    const std::vector<vpgl_perspective_camera<double> > &cams,
    std::vector<vgl_point_3d<double> > *world)
{
  // assume all points show up in all cams
  npts = imgspts[0].size();
  *world = std::vector<vgl_point_3d<double> >(npts,vgl_point_3d<double>(0.0, 0.0, 0.0));
}

void write_cams(std::vector<vpgl_perspective_camera<double> > &cams)
{
  vcl_vector<vcl_string> cam_fname_noexts; 
  for (unsigned i=0; i < cams.size(); ++i) {
    cam_fname_noexts.push_back(std::string(i) + "-badj");
    std::cout << "outputting " << cam_fname_noexts.back() << std::endl;
  }
  vul_file::make_directory("badj"); 
  std::cout << "Temp fname TODO: " << vul_temp_filename() << std::endl;
  
  bmcsd_util::write_cams("./badj", cam_fname_noexts, bmcsd_util::BMCS_3X4, cams);
}

int main(int argc, char** argv)
{

  vul_arg<std::vector<std::string> > a_cams("-cams", "load camera files (space-separated)");
  vul_arg_parse(argc,argv);
  
  std::vector<vpgl_perspective_camera<double> > ini_cams;
  get_cams(a_cams.value_, &ini_cams);

  get_corrs(&corr);

  initialize_world_by_triangulation(&ini_world);
 

  // make the initial world points (TODO: initial triang)
  std::vector<vgl_point_3d<double> > ini_world(world.size(),vgl_point_3d<double>(0.0, 0.0, 0.0));

  // write a VRML version of the scene for debugging
  vpgl_bundle_adjust::write_vrml("badj_ini.wrl",ini_cams,ini_world);


  // optimization with fixed calibration
  {
    std::vector<vpgl_perspective_camera<double> > unknown_cameras(ini_cams);
    std::vector<vgl_point_3d<double> > unknown_world(ini_world);

    // -------------------------------------------------

    vpgl_bundle_adjust ba;
    // ba.set_use_gradient(false);
    bool converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    std::cout << "Converged? " << converge << std::endl;
    vpgl_bundle_adjust::write_vrml("badj_fixed_k.wrl",unknown_cameras,unknown_world);
    write_cams(unknown_cameras);
  }


  // TODO: with focal length badj

  /*
  // test optimization with shared calibration and unknown focal length
  {
    std::vector<vpgl_perspective_camera<double> > unknown_cameras(ini_cams);
    std::vector<vgl_point_3d<double> > unknown_world(ini_world);

    vpgl_bundle_adjust ba;
    ba.set_self_calibrate(true);
    ba.set_max_iterations(10000);
    bool converge = ba.optimize(unknown_cameras, unknown_world, subset_image_points, mask);
    std::cout << "Converged? " << converge << std::endl;

    vpgl_bundle_adjust::write_vrml("test_bundle_est_f.wrl",unknown_cameras,unknown_world);
  }
  */

  return 0;
}
