#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector.h>
#include <vil/vil_image_view.h>

// This is an implementation of "Recovering High Dynamic Range Radiance Maps from
// Photographs" by Debevec and Malik.

class btpl_hdr{

public:

  btpl_hdr();

  // Estimate the log inverse camera response function g.
  void estimate_g(
    const vcl_vector< vil_image_view<vxl_byte> >& imgs,
    const vcl_vector<float>& shutter_speeds );
  void load_g( 
    vcl_string file_name );
  void save_g(
    vcl_string file_name );

  // Convert a set of images of different shutter speeds to HDR once
  // function g is computed.
  void compute_hdr(
    const vcl_vector< vil_image_view<vxl_byte> >& imgs,
    const vcl_vector<float>& shutter_speeds,
    vil_image_view<float>& hdr_img );

  void compute_variance_image(
    const vcl_vector< vil_image_view<vxl_byte> >& imgs,
    const vcl_vector<float>& shutter_speeds,
    vil_image_view<float>& var_img,
    float &var);

  void compute_variance(
    const vcl_vector< vil_image_view<vxl_byte> >& imgs,
    const vcl_vector<float>& shutter_speeds,
    float &var);

  void compute_variance1(
    const vcl_vector< vil_image_view<vxl_byte> >& imgs,
    const vcl_vector<float>& shutter_speeds,
    vil_image_view<float>& var_img,
    float &var);

  vcl_string inspection_file;
  int z_min_, z_max_;

protected:

  vnl_vector<float> g_;


  float zweight( 
    int z );

};
