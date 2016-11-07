#include "bioproj_mem_io.h"
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_cstdlib.h>
#include <vnl/vnl_math.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <bscs/bscs.h>
#include <vcsl/vcsl_axis.h>
#include <vcsl/vcsl_axis_sptr.h>
#include <vcsl/vcsl_micron.h>


// Look for a set of filenames that match the glob spec in filename
// The globbing format expects only '#' to represent numbers.
// Do not use "*" or "?"
// All "#" should be in one contiguous group.
static void parse_globbed_filenames(const vcl_string & input,
                                    vcl_vector<vcl_string> &filenames)
{
  filenames.clear();
  vcl_string filename = input;

  // Avoid confusing globbing functions
  if (filename.find("*") != filename.npos) return;
  if (filename.find("?") != filename.npos) return;

  // Check that all the #s are in a single group.
  vcl_size_t start = filename.find_first_of("#");
  if (start == filename.npos) return;
  vcl_size_t end = filename.find_first_not_of("#", start);
  if (filename.find_first_of("#",end) != filename.npos) return;
  if (end == filename.npos) end = filename.length();
  for (vcl_size_t i=start, j=start; i!=end; ++i, j+=12)
    filename.replace(j,1,"[0123456789]");


  // Search for the files
  for (vul_file_iterator fit(filename); fit; ++fit)
    filenames.push_back(fit());


  if (filenames.empty()) return;

  start = (start + filenames.front().size()) - input.size();
  end = (end + filenames.front().size()) - input.size();

  // Put them all in numeric order.
  vcl_sort(filenames.begin(), filenames.end());

  // Now discard non-contiguously numbered files.
  long count = vcl_atol(filenames.front().substr(start, end-start).c_str());
  vcl_vector<vcl_string>::iterator it=filenames.begin()+1;
  while (it != filenames.end())
  {
    if (vcl_atol(it->substr(start, end-start).c_str()) != ++count)
      break;
    ++it;
  }
  filenames.erase(it, filenames.end());
}


bioproj_mem_io::bioproj_mem_io(vcl_string log_fname, vcl_string scan_fname, vcl_string box_fname, vcl_string fname_pattern,
                       float i_zero,float voxel_size, double sigma_r, double sigma_z)
                       : i_zero_(i_zero),voxel_size_(voxel_size), sigma_r_(sigma_r), sigma_z_(sigma_z) 
{

  imgr_skyscan_log log(log_fname);
  imgr_skyscan_log_header hdr = log.header();
  sensor_pix_size_ =  hdr.cam_pixel_size_;
  source_origin_dist_ = hdr.object_to_source_dist_;
  source_sensor_dist_ = hdr.camera_to_source_dist_;
  sensor_dim_[0] = hdr.number_of_columns_;
  sensor_dim_[1] = hdr.number_of_rows_;
  rot_step_angle_ = hdr.rotation_step_;

  vcl_cerr << "image_pix_size_ =   " << hdr.image_pixel_size_  << "\n"; 
  vcl_cerr << "sensor_pix_size_ =   " << sensor_pix_size_   << "\n"; 
  vcl_cerr << "source_origin_dist_ =" << source_origin_dist_  << "\n";
  vcl_cerr << "source_sensor_dist_ =" << source_sensor_dist_  << "\n";
  vcl_cerr << "sensor_dim_[0] =     " << sensor_dim_[0]  << "\n";
  vcl_cerr << "sensor_dim_[1] =     " << sensor_dim_[1]  << "\n";
  vcl_cerr << "rot_step_angle_ =    " << rot_step_angle_ << "\n";

  
  sigma_r_extent_ = static_cast<int>((sensor_dim_[0] > sensor_dim_[1] 
                          ?  sensor_dim_[1]
                          : sensor_dim_[0])/(2.*sigma_r_)) - 10;
  sigma_z_extent_ = static_cast<int>(vcl_max(1.,sigma_r_extent_/10.));
  vcl_cerr << "r_extent is " << sigma_r_extent_ << "\n";
  vcl_cerr << "z_extent is " << sigma_z_extent_ << "\n";

  sigma_r_ *= hdr.image_pixel_size_;
  sigma_z_ *= hdr.image_pixel_size_;
  vcl_cerr << "sigma_r_ =    " << sigma_r_ << "\n";
  vcl_cerr << "sigma_z_ =    " << sigma_z_ << "\n";

  // read the scan
  vcl_ifstream scan_file(scan_fname.c_str());
  scan_file >> scan_;
  scan_file.close();

  // read the box
  vcl_ifstream box_file(box_fname.c_str());
  box_.read(box_file);
  box_file.close();

  vcl_cerr << "voxel_size_ =    " << voxel_size_ << "\n";

  //box is in biotree standard coordinate system, determine grid spacings
  bscs coord_system;
  //voxel size is specified in microns
  vcsl_micron_sptr microns = vcsl_micron::instance();

  //1/box units per std) / (voxel size/voxel units per std) 
  double scalefactor =  microns->units_per_standard_unit() / 
                        (voxel_size_*coord_system.axis(0)->unit()->units_per_standard_unit());

  grid_w_ = static_cast<int>(vnl_math_rnd(box_.width()*scalefactor));
  grid_h_ = static_cast<int>(vnl_math_rnd(box_.height()*scalefactor));
  grid_d_ = static_cast<int>(vnl_math_rnd(box_.depth()*scalefactor));

  vcl_cerr << "grid_w_ =    " << grid_w_ << "\n";
  vcl_cerr << "grid_h_ =    " << grid_h_ << "\n";
  vcl_cerr << "grid_d_ =    " << grid_d_ << "\n";

  nviews_ = scan_.n_views();

  //filenames_ = log.get_imagenames();
  parse_globbed_filenames(fname_pattern,filenames_); 

  // allocate the responses data structure and set to zero
  grid_.set_size(grid_w_, grid_h_, grid_d_);
  grid_.fill(0.0);
}

