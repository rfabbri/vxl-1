// This is /contrib/biotree/vol3d/vol3d_gaussian_kernel.h

#ifndef vol3d_gaussian_kernel_3d_h_
#define vol3d_gaussian_kernel_3d_h_

//: 
// \file    vol3d_gaussian_kernel_3d.h
// \brief   Creates a 3D gaussian kernel at the given direction, dimensions and voxel size
//          the actual size of the kernel is (2*w+1)x(2*w+1)x(2*w+1), and the indexing is 
//          valid in [-w, w] in each direction
// \author  Gamze D. Tunali
// \date    2006-05-01
// 
#include <vcl_string.h>
#include <vgl/vgl_box_3d.h> 
#include <vgl/vgl_point_3d.h> 
#include <vnl/vnl_quaternion.h>
#include <vnl/xio/vnl_xio_vector_fixed.h>

class vol3d_gaussian_kernel_3d{
public:
  vol3d_gaussian_kernel_3d(int w, double voxel_size, double sigma_r, 
    double sigma_z, vgl_vector_3d<double> dir);

  //: returns the filter value at the given kernel position (x,y,z).
  // The i, j, k is within range [-w, w]
  double get(int i, int j, int k);

  // returns the sum of the kernel values
  double sum();

  //: returns the dimension of the 3D voxel data, it is the same for all 3 axis
  int dim() { return (kernel_width_*2+1); }

  //: voxel indices are determined by width (w) as [-w, w]
  int width() { return kernel_width_; }

  //: size of the voxel at reconstruction, used only while computing the kernel values
  double voxel_size() { return voxel_size_; }

  double sigma_r() { return sigma_r_; }

  double sigma_z() { return sigma_z_; }

  //: direction of the filter
  vgl_vector_3d<double> dir() { return dir_;}

  void print(vcl_ostream& os);

   //: returns the rotation quaternion from (0, 0, 1) to the filter direction
  vnl_quaternion<double> get_rotation();

protected:
  int kernel_width_;
  double voxel_size_;
  double sigma_r_;
  double sigma_z_;
  vgl_vector_3d<double> dir_;
  vcl_vector<double> field_;

private:
  //: filter value at a given 3D point
  double gaussian(double x, double y, double z);
 
};

void x_write(vcl_ostream& os, vol3d_gaussian_kernel_3d kernel, vcl_string name="");

#endif
