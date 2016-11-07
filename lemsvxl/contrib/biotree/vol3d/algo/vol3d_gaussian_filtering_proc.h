#ifndef vol3d_gaussian_filtering_proc_h_
#define vol3d_gaussian_filtering_proc_h_
//: 
// \file      vol3d_gaussian_filtering_proc.h
// \brief     the control process of gaussian filtering of the 3D images
// \author    Gamze Tunali
// \date      2006-05-1
//
#include <vcl_string.h>
#include <vbl/vbl_smart_ptr.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>

#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_worldpt_field.h>
#include <xmvg/xmvg_filter_response.h>
#include <vol3d/vol3d_gaussian_kernel_3d.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <imgr/imgr_scan_images_resource.h>
#include <vbl/vbl_array_3d.h>

template <class T>
class  vol3d_gaussian_filtering_proc{
  public:

    vol3d_gaussian_filtering_proc(vil3d_image_resource_sptr img_res_sptr,
                                  vcl_vector<vol3d_gaussian_kernel_3d> &kernels);

    ~vol3d_gaussian_filtering_proc() {};
  
    //: the control process
    void execute();

    void execute_with_rad_det(vil3d_image_resource_sptr radius_res_sptr = 0);

    biob_worldpt_field<xmvg_filter_response<double> > & worldpt_field() {
      return *field_;
    }

    vcl_vector<vol3d_gaussian_kernel_3d> kernels() {return kernels_; }

  private:

    double convolve(int x, int y, int z, vol3d_gaussian_kernel_3d kernel);

    vbl_array_3d<double> convert_to_vbl_array(vil3d_image_view_base_sptr view_sptr,
                                              int min_x, int min_y, int min_z,
                                              int max_x, int max_y, int max_z,
                                              double& min_val, double &max_val);

    vbl_array_3d<double> radius_detection(int min_x, int min_y, int min_z,
                                          int max_x, int max_y, int max_z,
                                          double r_min, double r_max);

    vil3d_image_resource_sptr img_res_sptr_;

    vcl_vector<vol3d_gaussian_kernel_3d> kernels_;

    vil3d_image_view_base_sptr view_3d_;

    //: stores the roster and the response values 
    vbl_smart_ptr<biob_worldpt_field<xmvg_filter_response<double> > > field_;

};

template <class T>
void x_write(vcl_ostream& os, vol3d_gaussian_filtering_proc<T> &proc);

#endif
