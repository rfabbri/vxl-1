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

void get_cams(std::vector<vpgl_perspective_camera<double> > *cams)
{
}

void initialize_world_by_triangulation()
{
}

void get_corrs()
{ 
  unsigned npts=; // XXX
  std::vector<vgl_point_3d<double> > &ini_world = *pworld;

  world = std::vector<vgl_point_3d<double> >(npts,vgl_point_3d<double>(0.0, 0.0, 0.0));

  std::vector<vgl_point_2d<double> > image_points;


  for (unsigned c = 0; c < ncams; ++c)
    for (unsigned pw=0; pw < npts; ++i)
      image_points.push_back(imgpts[c][pw]);

  // make the mask (using all the points)
  std::vector<std::vector<bool> > mask(cameras.size(), std::vector<bool>(world.size(),true) );
}

void write_cams(std::vector<vpgl_perspective_camera<double> > &cams)
{
  vcl_vector<vcl_string> cam_fname_noexts; 
  for (unsigned i=0; i < cams.size(); ++i) {
    cam_fname_noexts.push_back(std::string(i) + "-badj");
    std::cout << "outputting " << cam_fname_noexts.back() << std::endl;
  }
  bmcsd_util::write_cams("./", cam_fname_noexts, bmcsd_util::BMCS_3X4, cams);
}

int main(int argc, char** argv)
{
  std::vector<vpgl_perspective_camera<double> > ini_cams;

  get_cams(&ini_cams);

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
