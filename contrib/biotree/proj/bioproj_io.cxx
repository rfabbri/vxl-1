#include "bioproj_io.h"
#include <vcl_fstream.h>
#include <vnl/vnl_math.h>

bioproj_io::bioproj_io(vcl_string scan_fname, vcl_string box_fname,
                       double resolution, double sensor_pix_size, 
                       double source_origin_dist, double source_sensor_dist,
                       vnl_int_2 const sensor_dim, double rot_step_angle, vcl_string file_path,
                       double sigma_r, double sigma_z, int sigma_r_extent, int sigma_z_extent,
                       int num_comp_planes)
                       : resolution_(resolution), sensor_pix_size_(sensor_pix_size),
                       source_origin_dist_(source_origin_dist), source_sensor_dist_(source_sensor_dist),
                       sensor_dim_(sensor_dim), rot_step_angle_(rot_step_angle), file_path_(file_path), 
                       sigma_r_(sigma_r), sigma_z_(sigma_z), 
                       sigma_r_extent_(sigma_r_extent), sigma_z_extent_(sigma_z_extent),
                       num_comp_planes_(num_comp_planes)
{
  // read the scan
  vcl_ifstream scan_file(scan_fname.c_str());
  scan_file >> scan_;
  scan_file.close();

  // read the box
  vcl_ifstream box_file(box_fname.c_str());
  box_.read(box_file);
  box_file.close();

  nviews_ = scan_.n_views();
}
