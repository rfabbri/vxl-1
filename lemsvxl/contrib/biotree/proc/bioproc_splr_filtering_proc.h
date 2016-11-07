#ifndef BIOPROC_SPLR_FILTERING_PROC_H_
#define BIOPROC_SPLR_FILTERING_PROC_H_
//: 
// \file  bioproc_splr_filtering_proc.h
// \brief  the control process of filtering the images
// \author    Kongbin Kang, modified by P. Klein
// \date        2005-03-18, modified 2005_06_27
//
#include <biob/biob_worldpt_field.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <imgr/imgr_scan_images_resource.h>
#include <splr/splr_basic_pizza_slice_splat_collection.h>
template< class T, class F>
class  bioproc_splr_filtering_proc
{
  public:

    //: constructor, NOTE: this one will be deprecated
    bioproc_splr_filtering_proc(const xscan_scan & scan,
                           vgl_box_3d<double> box,
                           double resolution,
                           vgl_box_3d<double> outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file="",
                           unsigned start_index=0, unsigned period=1);
    bioproc_splr_filtering_proc(imgr_scan_images_resource const & scan_image,
                           vgl_box_3d<double> box,
                           double resolution,
                           vgl_box_3d<double>  outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file="",
                           unsigned start_index=0, unsigned period=1);

    //: compute responses--if file ptr provided, write computed responses
    void execute(vcl_ofstream * outputfile = 0);

    biob_worldpt_field<xmvg_filter_response<T> > & response_field();

    imgr_image_view_3d_base_sptr get_image_resource() { return view_3d_;}
    
    xmvg_composite_filter_3d<T, F> composite_filter() { return filter_3d_; }

    const biob_worldpt_field<vnl_quaternion<double> > & rotation_field();

  private:
//: scan plan
    xscan_scan scan_;
  
//: active region of interests
    vgl_box_3d<double> box_;

 //: bounds of splats
    vgl_box_3d<double> outer_box_;

    //: composite filter 3d
    xmvg_composite_filter_3d<T, F> filter_3d_;

    

//:nominal spacing between sample points
  double resolution_;



    //: splat resource

    splr_basic_pizza_slice_splat_collection<T, F> splr_;

    
  
   

    //: stores the roster and the response values 
    biob_worldpt_field<xmvg_filter_response<T> > response_field_;
    
    //: bounded 3D view
    imgr_image_view_3d_base_sptr view_3d_;

    //: helper function to set up image resource
    void imgr();

    //: helper function to get image resource from binary file
    void set_up_imgr_from_bin(vcl_string bin_scan_file);
};

template< class T, class F>
void x_write(vcl_ostream &os, bioproc_splr_filtering_proc<T, F> &proc);

#endif
