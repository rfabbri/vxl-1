// This is /contrib/biotree/vol3d/vol3d_gaussian_kernel_3d.cxx
#include "vol3d_gaussian_kernel_3d.h"
#include <vnl/vnl_cross.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_vector_3d.h>

double Z_DIR_THRESH = 0.005;

vol3d_gaussian_kernel_3d::vol3d_gaussian_kernel_3d(int w, 
                                                 double voxel_size, 
                                                 double sigma_r,
                                                 double sigma_z,
                                                 vgl_vector_3d<double> dir)
: kernel_width_(w), voxel_size_(voxel_size), sigma_r_(sigma_r), 
sigma_z_(sigma_z),dir_(dir)
{
  field_.resize(dim()*dim()*dim());
  vnl_vector<double> new_pos(3);
  int num=0;

  vgl_vector_3d<double> z(0., 0., 1.);
  dir_ /= dir_.length();
  double angle = dot_product(dir_, z);

  vnl_quaternion<double> q = get_rotation();
  for (int k = (-1*kernel_width_); k <= kernel_width_; k++) {
    for (int j = (-1*kernel_width_); j <= kernel_width_; j++) {
      for (int i = (-1*kernel_width_); i <= kernel_width_; i++) {
        vnl_vector<double> v(3);
        v[0]=i*voxel_size_; v[1]=j*voxel_size_; v[2]=k*voxel_size_;

        // check if direction is very close to z ,if so do not rotate the direction
        if ((1.0 - vcl_fabs(angle)) < Z_DIR_THRESH) {
          new_pos = v;
        } else {
          new_pos = q.rotate(v);
        }
        
        // compute the gaussian at the new position after rotation
        double g = gaussian(new_pos[0], new_pos[1],  new_pos[2]);
        field_[num++]= g;
      }
    }
  }
}

double vol3d_gaussian_kernel_3d::gaussian(double x, double y, double z)
{
  assert ((sigma_r_ > 0) && (sigma_z_ > 0));

  double a = ((x*x) + (y*y))/(sigma_r_*sigma_r_);
  double b = -0.5*(a + ((z*z) / (sigma_z_*sigma_z_)));
  double res = -1.*(a - 2.0)*vcl_exp(b);
  return res;
}

vnl_quaternion<double> vol3d_gaussian_kernel_3d::get_rotation()
{
  vgl_vector_3d<double> z(0., 0., 1.);
  vnl_vector<double> axis(3);
  vnl_vector<double> f_vec(3);
  f_vec[0] = dir_.x();
  f_vec[1] = dir_.y();
  f_vec[2] = dir_.z();
  f_vec.normalize();

  vnl_vector<double> z_vec(3);
  z_vec[0] = z.x();
  z_vec[1] = z.y();
  z_vec[2] = z.z();
  z_vec.normalize();

  axis = vnl_cross_3d<double> (f_vec, z_vec);
  axis.normalize();
  double d = dot_product(f_vec, z_vec);
  double rot_angle = vcl_acos(d);

  vnl_quaternion<double> q(axis, rot_angle);
  return q;
}

double vol3d_gaussian_kernel_3d::get(int i, int j, int k) 
{ 
  i += kernel_width_;
  j += kernel_width_;
  k += kernel_width_;
  unsigned index =  dim()*dim()*k + dim()*j + i;
  if (index >= field_.size()) {
    vcl_cerr << "vol3d_gaussian_kernel_3d::get(i, j, k), index is out of bounds)" << vcl_endl; 
    return 0;
  }
  return field_[index];
}

double vol3d_gaussian_kernel_3d::sum()
{
  double sum = 0.0;
  for (int k = (-1*kernel_width_); k <= kernel_width_; k++) {
    for (int j = (-1*kernel_width_); j <= kernel_width_; j++) {
      for (int i = (-1*kernel_width_); i <= kernel_width_; i++) {
        sum += get(i, j, k);
      }
    }
  }
  return sum;
}

void vol3d_gaussian_kernel_3d::print(vcl_ostream& os) 
{
  for (int k = (-1*kernel_width_); k <= kernel_width_; k++) {
    for (int j = (-1*kernel_width_); j <= kernel_width_; j++) {
      for (int i = (-1*kernel_width_); i <= kernel_width_; i++) {
        os << get(i, j, k) << " ";
      }
      os << "\n";
    }
    os << "\n";
  }
}

void x_write(vcl_ostream& os, vol3d_gaussian_kernel_3d kernel, vcl_string name)
{
  vcl_string elm_name;

  if (name.length() == 0)
    elm_name = "vol3d_gaussian_kernel_3d";
  else
    elm_name = name;
  vsl_basic_xml_element element(elm_name);
  
  element.add_attribute("kernel_width" , kernel.width());
  element.add_attribute("voxel_size" , kernel.voxel_size());
  element.add_attribute("sigma_r" , kernel.sigma_r());
  element.add_attribute("sigma_z" , kernel.sigma_z());
  element.x_write_open(os);
  x_write(os, kernel.dir(), "kernel_direction");
  element.x_write_close(os);
}
