// This is bwm_lidar_mesh.cxx
//   Ming-Ching Chang
//   Dec 13, 2007.

#ifndef bwm_lidar_mesh_h_
#define bwm_lidar_mesh_h_

//============================================================
//Meshing lidar image

#include <vcl_vector.h>
#include <vil/vil_image_view.h>


class lidar_pixel
{
public:
  int         label_;   //Building, Vegeration, Ground.
  double      height_;  //
  float       color_r_, color_g_, color_b_;
public:
  lidar_pixel (int label, double height, float r, float g, float b) {
    label_ = label;
    height_ = height;
    color_r_ = r;
    color_g_ = g;
    color_b_ = b;
  }
};

class lidar_range_data
{
public:
  vil_image_view<vxl_byte> img1;
  vil_image_view<vxl_byte> img2;
  vcl_vector<vcl_vector<lidar_pixel*> > data_;

public:
  lidar_range_data () {
  }


};

bool load_lidar_label_file (const char* file, lidar_range_data* LRD);


#endif //bwm_lidar_mesh_h_



