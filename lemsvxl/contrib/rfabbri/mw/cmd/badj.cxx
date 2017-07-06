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

void get_corrs(
    unsigned ncams, 
    std::vector<vgl_point_2d<double> > *image_points_linearlist, 
    std::vector< std::vector<vgl_point_2d<double> > > *pimgpts_percam,
    std::vector<std::vector<bool> > *mask)
{
  unsigned npts=10;
  std::vector< std::vector<vgl_point_2d<double> > > imgpts_percam = *pimgpts_percam;

  imgpts[0].resize(npts);
  imgpts[0][0].set(891.0, 460.0);
  imgpts[0][1].set(827.0, 550.0);
  imgpts[0][2].set(826.0, 732.0);
  imgpts[0][3].set(961.0, 734.0);
  imgpts[0][4].set(961.0, 550.0);
  imgpts[0][5].set(1026.0, 463.0);
  imgpts[0][6].set(1059.0, 463.0);
  imgpts[0][7].set(1043, 629);
  imgpts[0][8].set(1063, 700);
  imgpts[0][9].set(1025, 700);

  imgpts[1].resize(npts);
  imgpts[1][0].set(1011.5, 548.5);
  imgpts[1][1].set(961.5, 635.2);
  imgpts[1][2].set(958.5, 812.0);
  imgpts[1][3].set(1048.5, 816.2);
  imgpts[1][4].set(1051, 637);
  imgpts[1][5].set(1100, 552);
  imgpts[1][6].set(1125, 552);
  imgpts[1][7].set(1111, 716);
  imgpts[1][8].set(1126.5, 785);
  imgpts[1][9].set(1101, 784);

  imgpts[2].resize(npts);
  imgpts[2][0].set(923, 81);
  imgpts[2][1].set(854.5, 178.8);
  imgpts[2][2].set(856, 373);
  imgpts[2][3].set(1000, 372);
  imgpts[2][4].set(999, 178);
  imgpts[2][5].set(1070, 84);
  imgpts[2][6].set(1102, 82);
  imgpts[2][7].set(1087, 262);
  imgpts[2][8].set(1108, 333);
  imgpts[2][9].set(1067, 334);

  imgpts[3].resize(npts);
  imgpts[3][0].set(1069, 379);
  imgpts[3][1].set(1018, 472.6);
  imgpts[3][2].set(1018.5, 661);
  imgpts[3][3].set(1115, 662);
  imgpts[3][4].set(1114, 472);
  imgpts[3][5].set(1165, 380);
  imgpts[3][6].set(1191, 380);
  imgpts[3][7].set(1179, 603);
  imgpts[3][8].set(1194, 624);
  imgpts[3][9].set(1168, 624);


  for (unsigned c = 0; c < ncams; ++c)
    for (unsigned pw=0; pw < npts; ++i)
      image_points_linearlist->push_back(imgpts_percam[c][pw]);

  // make the mask (using all the points)
  *mask = std::vector<std::vector<bool> > (ncams, std::vector<bool>(npts,true) );
}


void initialize_world_by_triangulation(
    const std::vector<std::vector<vgl_point_2d<double> > > &imgpts,
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

  std::vector<vgl_point_2d<double> > image_points_linearlist; 
  std::vector< std::vector<vgl_point_2d<double> > > imgpts_percam;
  get_corrs(ini_cams.size(), &image_points_linearlist, &imgpts_percam, &mask);

  initialize_world_by_triangulation(imgpts_percam, ini_cams, &ini_world);
 
  // write a VRML version of the scene for debugging
  vpgl_bundle_adjust::write_vrml("badj_ini.wrl",ini_cams, ini_world);

  // optimization with fixed calibration
  {
    std::vector<vpgl_perspective_camera<double> > unknown_cameras(ini_cams);
    std::vector<vgl_point_3d<double> > unknown_world(ini_world);

    // -------------------------------------------------

    vpgl_bundle_adjust ba;
    // ba.set_use_gradient(false);
    bool converge = ba.optimize(unknown_cameras, unknown_world, image_points, mask);
    std::cout << "Converged? " << converge << std::endl;
    vpgl_bundle_adjust::write_vrml("badj_fixed_k.wrl", unknown_cameras, unknown_world);
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
