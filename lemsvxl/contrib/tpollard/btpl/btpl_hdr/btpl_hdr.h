#include <iostream>

#include <string>
#include <vector>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>

// This is an implementation of "Recovering High Dynamic Range Radiance Maps from
// Photographs" by Debevec and Malik.

class btpl_hdr{

public:

  btpl_hdr();

  // Estimate the log inverse camera response function g.
  void estimate_g(
    const std::vector< vil_image_view<vxl_byte> >& imgs,
    const std::vector<float>& shutter_speeds );
  void load_g( 
    std::string file_name );
  void save_g(
    std::string file_name );

  // Convert a set of images of different shutter speeds to HDR once
  // function g is computed.
  void compute_hdr(
    const std::vector< vil_image_view<vxl_byte> >& imgs,
    const std::vector<float>& shutter_speeds,
    vil_image_view<float>& hdr_img );

  void compute_variance_image(
    const std::vector< vil_image_view<vxl_byte> >& imgs,
    const std::vector<float>& shutter_speeds,
    vil_image_view<float>& var_img,
    float &var);

  void compute_variance(
    const std::vector< vil_image_view<vxl_byte> >& imgs,
    const std::vector<float>& shutter_speeds,
    float &var);

  void compute_variance1(
    const std::vector< vil_image_view<vxl_byte> >& imgs,
    const std::vector<float>& shutter_speeds,
    vil_image_view<float>& var_img,
    float &var);

  std::string inspection_file;
  int z_min_, z_max_;

protected:

  vnl_vector<float> g_;


  float zweight( 
    int z );

};
