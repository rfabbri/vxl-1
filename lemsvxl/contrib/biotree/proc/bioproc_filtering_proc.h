#ifndef FILTERING_PROC_H_
#define FILTERING_PROC_H_
//: 
// \file  bioproc_filtering_proc.h
// \brief  the control process of filtering the images
// \author    Kongbin Kang
// \date        2005-03-18
//
#include <vcl_string.h>
#include <vbl/vbl_smart_ptr.h>
#include <biob/biob_grid_worldpt_roster.h>
#include <biob/biob_worldpt_field.h>
#include <xscan/xscan_scan.h>
#include <xmvg/xmvg_composite_filter_3d.h>
#include <xmvg/xmvg_filter_response.h>
#include <imgr/imgr_image_view_3d_base_sptr.h>
#include <imgr/imgr_scan_images_resource.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include "bioproc_dummy_splat_resource.h"

template< class T, class F>
class  bioproc_filtering_proc
{
  public:

    //: constructor, NOTE: this one will be deprecated
    bioproc_filtering_proc(xscan_scan const & scan,
                           vgl_box_3d<double> & box,
                           double resolution,
                           vgl_box_3d<double> &outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file="",
                           biob_worldpt_roster_sptr sample_roster = 0);

    bioproc_filtering_proc(imgr_scan_images_resource const & scan_image,
                           vgl_box_3d<double> &box,
                           double resoultion,
                           vgl_box_3d<double> &outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file="",
                           biob_worldpt_roster_sptr sample_roster = 0);

    ~bioproc_filtering_proc();
  
    //: the control process
    void execute(const vcl_vector<biob_worldpt_index> * which_samples = 0);

    biob_worldpt_field<xmvg_filter_response<T> > & worldpt_field() {
      return *field_;
    }

    imgr_image_view_3d_base_sptr get_image_resource() { return view_3d_;}

    bioproc_dummy_splat_resource<T,F>* splr(){return splr_;}

    xmvg_composite_filter_3d<T, F> composite_filter() { return filter_3d_; }

  private:

    //: if no sample roster was provided, use a grid
    void check_sample_roster(const vgl_box_3d<double> & box,
      double resolution, biob_worldpt_roster_sptr sample_roster);

    

    //: scan plan
    xscan_scan scan_;

//: active region of interests
    vgl_box_3d<double> box_;

//: the big region of interest
    vgl_box_3d<double> outer_box_;

//: composite fulter 3d
    xmvg_composite_filter_3d<T, F> filter_3d_;

    //: splat resource
    bioproc_dummy_splat_resource<T, F>* splr_;

    
    

    //: stores the roster and the response values 
    vbl_smart_ptr<biob_worldpt_field<xmvg_filter_response<T> > > field_;

    //: bounded 3D view
    imgr_image_view_3d_base_sptr view_3d_;
    imgr_bounded_image_view_3d<unsigned short> v3d_;
    //: helper function to set up image resource
    void imgr();

    //: helper function to set up splat resource
    void set_up_splr();

};

template< class T, class F>
void x_write(vcl_ostream& os, bioproc_filtering_proc<T, F> &proc);

#endif
