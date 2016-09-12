#ifndef bioproc_filtering_proc_txx_
#define bioproc_filtering_proc_txx_

#include "bioproc_filtering_proc.h"
#include <imgr/imgr_scan_resource_sptr.h>
#include <imgr/imgr_scan_resource_io.h>
#include <xmvg/algo/xmvg_dot_product_2d.h>
#include <xmvg/algo/xmvg_ramp_compensation.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vgl/xio/vgl_xio_box_3d.h>
#include <vnl/vnl_math.h>

template<class T, class F> 
void bioproc_filtering_proc<T,F>::check_sample_roster(
     const vgl_box_3d<double> & box, double resolution,
     biob_worldpt_roster_sptr sample_roster){
  if (!sample_roster){
    biob_worldpt_roster_sptr roster = new biob_grid_worldpt_roster(box, resolution);
    field_ = new biob_worldpt_field<xmvg_filter_response<T> > (roster);
   }
  else {
    field_ = new biob_worldpt_field<xmvg_filter_response<T> > (sample_roster);
  }
}

template<class T, class F> 
bioproc_filtering_proc<T,F>:: bioproc_filtering_proc(xscan_scan const& scan, 
    vgl_box_3d<double> &box,   double resolution,
    vgl_box_3d<double> &outer_box,
    xmvg_composite_filter_3d<T, F> const & filter_3d,
    vcl_string bin_scan_file,
    biob_worldpt_roster_sptr sample_roster) 
    : scan_(scan),   box_(box), outer_box_(outer_box), filter_3d_(filter_3d)
{
  check_sample_roster(box_, resolution, sample_roster);
  
  //unused variable
  //unsigned num_pts = field_->roster()->num_points();

  // set up splat resource
  set_up_splr();

  // if there is a binary file name for imgr_bounded_image_view, read it directly
  // from the binary file
  if (bin_scan_file.size() > 0) {
    vsl_b_ifstream is(bin_scan_file.data());
    v3d_.b_read(is);
  } else
    // set up image resource
    imgr();
}

template<class T, class F>
bioproc_filtering_proc<T, F>:: bioproc_filtering_proc(
                           imgr_scan_images_resource const & scan_images,
                           vgl_box_3d<double> &box,
                           double resolution,
                           vgl_box_3d<double> &outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file,
                           biob_worldpt_roster_sptr sample_roster)
: scan_(scan_images.get_scan()), box_(box), outer_box_(outer_box), filter_3d_(filter_3d)
{

  check_sample_roster(box, resolution, sample_roster);
  /*unsigned num_pts = field_->roster()->num_points();unused variable*/
  
  set_up_splr();

  if (bin_scan_file.size() > 0) {
    vsl_b_ifstream is(bin_scan_file.data());
    v3d_.b_read(is);
  } else {
    // get bounded view
    view_3d_ = scan_images.get_bounded_view(outer_box_);
    v3d_ = view_3d_;
  }
}



template<class T, class F>
bioproc_filtering_proc<T,F>:: ~bioproc_filtering_proc()
{
  if(splr_!=0)
    delete splr_;
}

template<class T, class F>
void bioproc_filtering_proc<T,F>::imgr()
{
  imgr_scan_resource_sptr resc = imgr_scan_resource_io::read_resource(scan_);

  if(!resc)
    return;

  view_3d_ = resc->get_bounded_view(outer_box_);
  v3d_ = view_3d_;
}

template<class T, class F>
void bioproc_filtering_proc<T,F> :: set_up_splr()
{
  splr_ = new bioproc_dummy_splat_resource<T, F>(scan_, filter_3d_); 
}

template<class T, class F>
void bioproc_filtering_proc<T, F> :: execute(const vcl_vector<biob_worldpt_index> * which_samples)
{
  unsigned nviews = scan_.scan_size();
  
  unsigned npts = which_samples ? which_samples->size() : field_->roster()->num_points();

  unsigned num_atomic_filters = filter_3d_.size();
  
  vcl_vector<xmvg_filter_response<T> > response_field_(npts);
  
  for(unsigned int i = 0; i< npts; i++){
    // get point
  
    xmvg_filter_response<T> response(num_atomic_filters, T(0));

    for(orbit_index t = 0; t < nviews; t++){
      //: get view after cast to desired type
      dbil_bounded_image_view<unsigned short> dv = v3d_.view_2d(t);

      biob_worldpt_index pti = which_samples ?  ((*which_samples)[i]) : biob_worldpt_index(i);
      
      //: get filter_2d
      xmvg_composite_filter_2d<T> uncomp_filter = splr_->splat(t, field_->roster()->point(pti));

      /////// Second attempt at compensating for the 1/r effect -- JLM
      xmvg_composite_filter_2d<T> filter;
      xmvg_ramp_compensation<T>(uncomp_filter, filter);
      ////////

      filter.eliminate_residue_effect();

#if 0
      /////////////////////////////////////////////////////////////////////////////////
      // Ram-Lak kernel in spatial domain
      // B is the cut-off frequency
      // k is the size of the kernel on one side
      double B = 0.5;
      int k = 7;
      vcl_vector<double> kernel(2*k+1);
      int index = 0;
      for(int i=-k; i<=k; i++)
      {
        if(i == 0)
          kernel[index] = vcl_pow(B,2.0);
        else if(i % 2 == 0)
          kernel[index] = 0.0;
        else
          kernel[index] = -4*vcl_pow(B,2.0) / (vcl_pow((vnl_math::pi  * i), 2.0));
        index++;
      }
      /////////////////////////////////////////////////////////////////////////////////
      filter.convolve_rows_with_kernel(kernel);
#endif
      //: apply the 2d filter to image

      response += xmvg_dot_product_2d(filter, dv);
    }

    response_field_[i] = response;

    vcl_cout << "finish compute response at voxel " << i << '\n';
  }
  field_->set_values(response_field_);

}

template<class T, class F>
void x_write(vcl_ostream& os, bioproc_filtering_proc<T,F> &proc)
{
  vsl_basic_xml_element element("bioproc_filtering_proc");
  element.x_write_open(os);
  
  //: composite filter 3d
  xmvg_composite_filter_3d<T, F> f = proc.composite_filter();
  x_write(os, f);
  
  // write the roster and values
  x_write(os, proc.worldpt_field());
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef BIOPROC_FILTERING_PROC_INSTANTIATE
#define BIOPROC_FILTERING_PROC_INSTANTIATE(T, F) \
template class bioproc_filtering_proc<T, F>;\
template void x_write(vcl_ostream&, bioproc_filtering_proc<T, F> &)

#endif
