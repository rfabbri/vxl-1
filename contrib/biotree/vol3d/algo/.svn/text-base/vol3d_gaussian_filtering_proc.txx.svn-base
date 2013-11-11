#ifndef vol3d_gaussian_filtering_proc_txx
#define vol3d_gaussian_filtering_proc_txx

#include "vol3d_gaussian_filtering_proc.h"
#include <vol3d/algo/vol3d_radius_detection.h>
#include <imgr/imgr_scan_resource_sptr.h>
#include <imgr/imgr_scan_resource_io.h>
#include <xmvg/algo/xmvg_dot_product_2d.h>
#include <xmvg/xmvg_gaussian_filter_descriptor.h>

#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vnl/vnl_math.h>
#include <vil/vil_pixel_format.h>
#include <vil3d/algo/vil3d_histogram.h>
#include <vil3d/vil3d_save.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_otsu_threshold.h>

#include <vxl_config.h>
#include <vcl_map.h>
#include <vcl_limits.h>

template <class T>
vol3d_gaussian_filtering_proc<T>:: vol3d_gaussian_filtering_proc(
  vil3d_image_resource_sptr img_res_sptr,
  vcl_vector<vol3d_gaussian_kernel_3d> &kernels)
  : img_res_sptr_(img_res_sptr),  kernels_(kernels)
{
  
  biob_worldpt_roster_sptr roster = new biob_grid_worldpt_roster(img_res_sptr->ni(), 
      img_res_sptr->nj(), img_res_sptr->nk(), 0, 0, 0);
  field_ = new biob_worldpt_field<xmvg_filter_response<double> > (roster);
  
  unsigned num_pts = field_->roster()->num_points();
  view_3d_ = (img_res_sptr_->get_view());

}

template <class T>
double vol3d_gaussian_filtering_proc<T>:: convolve(int x, int y, int z, 
                                                vol3d_gaussian_kernel_3d kernel)
{
  double val=0;
  int w = kernel.width();


  for (int k=z-w; k<=z+w; k++) {
    for (int j=y-w; j<=y+w; j++) {
      for (int i=x-w; i<=x+w; i++) {  
        vil3d_image_view<T> view = *view_3d_;
        T voxel_val = 0;
        if ((i > 0) && (j>0) && (k>0) && 
          (i < static_cast<int> (view.ni())) && 
          (j < static_cast<int> (view.nj())) && 
          (k < static_cast<int> (view.nk()))) 
          voxel_val = view(i,j,k);
        
        if (voxel_val > 0) {
          val += voxel_val * kernel.get(i-x, j-y, k-z);
        }
      }
    }
  }
  return val;
}
template <class T>
vbl_array_3d<double> 
vol3d_gaussian_filtering_proc<T>::convert_to_vbl_array(vil3d_image_view_base_sptr view_sptr,
                                                    int min_x, int min_y, int min_z,
                                                    int max_x, int max_y, int max_z,
                                                    double& min_val, double &max_val)
{
  int x = max_x - min_x;
  int y = max_y - min_y;
  int z = max_z - min_z;
  vbl_array_3d<double> volume(x, y, z);
  min_val = vcl_numeric_limits<float >::max();
  max_val = vcl_numeric_limits<float >::min();

  for (int k=0; k<z; k++) {
    for (int j=0; j<y; j++) {
      for (int i=0; i<x; i++) {  
        vil3d_image_view<T> view = *view_sptr;
        if ((i > 0) && (j>0) && (k>0) && 
            (i < static_cast<int> (view.ni())) && 
            (j < static_cast<int> (view.nj())) && 
            (k < static_cast<int> (view.nk())))  {
          T voxel_val = 0;
          voxel_val = view(i+min_x, j+min_y, k+min_z);
          double val = voxel_val;
          if (min_val>val)
            min_val = val;
          if (max_val<val)
            max_val = val;
          volume[i][j][k] = val;
        } else {
          volume[i][j][k] = 0.0;
        }
        
      }
    }
  }
  return volume;
}

template <class T>
vbl_array_3d<double> 
vol3d_gaussian_filtering_proc<T>::radius_detection(int min_x, int min_y, int min_z,
                                                int max_x, int max_y, int max_z,
                                                double r_min, double r_max)
{
  int x = max_x - min_x;
  int y = max_y - min_y;
  int z = max_z - min_z;

  double min_val = vcl_numeric_limits<float >::max();
  double max_val = vcl_numeric_limits<float >::min();

  vbl_array_3d<double> vol_array = convert_to_vbl_array(view_3d_,
                                                    min_x, min_y, min_z,
                                                    max_x, max_y, max_z,
                                                    min_val, max_val);

  double avg = (min_val + max_val)/2;

  vcl_vector<double> count;
  vil3d_image_view<T> img_3d = img_res_sptr_->get_view();
  vil3d_histogram(img_3d,count,min_val,max_val,max_val - min_val + 1);

  bsta_histogram<double> b_hist(min_val,max_val,count);
  bsta_otsu_threshold<double> b_thresh(b_hist);      
  unsigned int marcate = b_thresh.bin_threshold();
  double std = vcl_sqrt(b_hist.variance(0,marcate));

  vbl_array_3d<double> volume = convert_to_vbl_array(view_3d_, min_x, 
    min_y, min_z, max_x, max_y, max_z, min_val, max_val);
  vol3d_radius_detection rd(r_min, r_max); 
  vbl_array_3d<double> radius = rd.radius(volume, /*2000./3.*/ std/3, float(0.3));// mean, sigma); 

#if 1
  vil3d_image_view<unsigned char> radius_view(x, y, z);
  vcl_ofstream of("C:\\test_images\\filters\\newcast35um_reconstructed\\results\\slice_rad.txt");
  of << x << " " << y << " " << z << vcl_endl;
  for (int k=0; k<z; k++) {
    for (int i=0; i<y; i++) {
      for (int j=0; j<x; j++) {
        of << radius[j][i][k] << " ";
        unsigned char &c = (radius_view(j,i,k));
        c = radius[j][i][k];
      }
     of << vcl_endl;
   }
  of << vcl_endl;
  }

  vil3d_save(radius_view, "C:\\test_images\\filters\\newcast35um_reconstructed\\results\\radius.gipl");
#endif 

  return radius;
}

template <class T>
void vol3d_gaussian_filtering_proc<T>:: execute()
{

  vcl_vector<xmvg_filter_response<double> > response_field_;
  
  int w = kernels_[0].width();
  unsigned p=0;
  for (unsigned k=0; k<img_res_sptr_->nk(); k++) {
    for (unsigned j=0; j<img_res_sptr_->nj(); j++) {
      for (unsigned i=0; i<img_res_sptr_->ni(); i++) {
        xmvg_filter_response<double> response(kernels_.size(), 0.);
        for (unsigned t = 0; t<kernels_.size(); t++) {
          double filter_val = convolve(i, j, k, kernels_[t]);
          response[t] = filter_val;
        }
        response_field_.push_back(response);
        vcl_cout << p++ << '\n';
     }
   }   
  }

  field_->set_values(response_field_);

}

template <class T>
void vol3d_gaussian_filtering_proc<T>::execute_with_rad_det(vil3d_image_resource_sptr radius_res_sptr)
{

  vcl_vector<xmvg_filter_response<double> > response_field_;
  
  int w = kernels_[0].width();
  unsigned p=0;
  double filter_val;

  int xdim = img_res_sptr_->ni();
  int ydim = img_res_sptr_->nj();
  int zdim = img_res_sptr_->nk();

  double min, max;
  vbl_array_3d<double> radius;
  if (radius_res_sptr == 0) {
    radius = radius_detection(0, 0, 0, xdim, ydim, zdim, 2.0, 8.0);
  } else {
    vil3d_image_view_base_sptr radius_view = radius_res_sptr->get_view();
    radius = convert_to_vbl_array(radius_view, 0, 0, 0, 
      radius_view->ni(), radius_view->nj(), radius_view->nk(), min, max);
  }
  
  assert ((radius.get_row1_count() == xdim) && 
          (radius.get_row2_count() == ydim) &&
          (radius.get_row3_count() == zdim));
  
  // create a vector of maps for already created kernels
  vcl_vector<vcl_map<double, vol3d_gaussian_kernel_3d*> > kernel_hash(kernels_.size());
  
  for (int k=0; k<img_res_sptr_->nk(); k++) {
    for (int j=0; j<img_res_sptr_->nj(); j++) {
      for (int i=0; i<img_res_sptr_->ni(); i++) {
        double r=radius[i][j][k];
        xmvg_filter_response<double> response(kernels_.size(), 0.);
        for (unsigned int t = 0; t<kernels_.size(); t++) {
          if (r != 0) {
            vol3d_gaussian_kernel_3d *kernel;
            vcl_map<double, vol3d_gaussian_kernel_3d*>::iterator iter = kernel_hash[t].find(r);
            if (iter == kernel_hash[t].end()){
              // create a new kernel based on the radius, only direction is used from
              // the previously created kernel
              double sigma_r = r/1.414;
              kernel = new vol3d_gaussian_kernel_3d(static_cast<int>(4.*sigma_r), 
                                                    kernels_[t].voxel_size(), sigma_r, 
                                                    sigma_r, kernels_[t].dir());
              kernel_hash[t][r]=kernel;
            } else {
              kernel = (*iter).second;
            }
            
            filter_val = convolve(i, j, k, *kernel);
          } else {  // r is zero
            filter_val = 0; 
          } 
          response[t] = filter_val;
        }
        response_field_.push_back(response);
        vcl_cout << p++ << '\n';
     }
   }   
  }

  field_->set_values(response_field_);

}

template <class T>
void x_write(vcl_ostream& os, vol3d_gaussian_filtering_proc<T> &proc)
{
  vsl_basic_xml_element element("vol3d_gaussian_filtering_proc");
  element.x_write_open(os);
  
  // write the kernel values
  vcl_vector<vol3d_gaussian_kernel_3d> kernels = proc.kernels();
  for (unsigned i=0; i < kernels.size(); i++) {
    xmvg_gaussian_filter_descriptor nnfd(kernels[i].sigma_r(), kernels[i].sigma_r()*2, 
      vgl_point_3d<double> (0, 0, 0), kernels[i].dir());
    x_write(os, nnfd); 
  }

  // write the roster and values
  x_write(os, proc.worldpt_field());
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef VOL3D_GAUSSIAN_FILTERING_PROC_INSTANTIATE
#define VOL3D_GAUSSIAN_FILTERING_PROC_INSTANTIATE(T) \
template class vol3d_gaussian_filtering_proc<T>; \
template void x_write(vcl_ostream &, vol3d_gaussian_filtering_proc<T> &) 

#endif
