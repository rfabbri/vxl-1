
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x1.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>

#include <brip/brip_vil_float_ops.h>

#include "breg3d_util.h"


bool breg3d_util::read_cameras(const char *filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
  std::ifstream file_inp(filename);
  if (!file_inp.good()) {
    std::cerr << "error opening file "<< filename <<std::endl;
    return false;
  }
  unsigned ncameras;

  file_inp >> ncameras;
  for (unsigned i=0; i < ncameras; i++) {
    vnl_double_3x3 K,R;
    vnl_double_3x1 T;
    file_inp >> K;
    file_inp >> R;
    file_inp >> T;
    Ks.push_back(K);
    Rs.push_back(R);
    Ts.push_back(T);
  }
  file_inp.close();

  return true;

}


bool breg3d_util::write_cameras(const char *filename, std::vector<vnl_double_3x3> &Ks, std::vector<vnl_double_3x3> &Rs, std::vector<vnl_double_3x1> &Ts)
{
std::ofstream file_out(filename);
  if (!file_out.good()) {
    std::cerr << "error opening file "<< filename <<std::endl;
    return false;
  }
  unsigned ncameras = Ks.size();

  file_out << ncameras << std::endl << std::endl;
  for (unsigned i=0; i < ncameras; i++) {

    file_out << Ks[i] << std::endl;
    file_out << Rs[i] << std::endl;
    file_out << Ts[i] << std::endl;
    file_out << std::endl;
  }
  file_out.close();

  return true;

}


vil_image_view<float> breg3d_util::load_image(std::string img_fname)
{
  vil_image_resource_sptr img_res = vil_load_image_resource(img_fname.c_str());
  vil_image_view<float> frame_view = brip_vil_float_ops::convert_to_float(img_res);
  float img_norm_val = 1.0f / 255.0f;
  vil_math_scale_values(frame_view,img_norm_val);

  return frame_view;
}


void breg3d_util::save_image(vil_image_view<float> img, std::string img_fname)
{
  float img_norm_val = 255.0f;
  vil_math_scale_values(img,img_norm_val);
  vil_image_view<vxl_byte> frame_byte = brip_vil_float_ops::convert_to_byte(img);
  vil_save(frame_byte,img_fname.c_str());

  return;
}

