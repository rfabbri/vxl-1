#ifndef bioproc_splr_filtering_proc_txx_
#define bioproc_splr_filtering_proc_txx_

#include <vbl/io/vbl_io_array_2d.txx>

#include "bioproc_splr_filtering_proc.h"
#include <imgr/imgr_scan_resource_sptr.h>
#include <imgr/imgr_scan_resource_io.h>
#include <imgr/imgr_bounded_image_view_3d.h>
#include <xmvg/algo/xmvg_dot_product_2d.h>
#include <splr/splr_basic_pizza_slice_splat_collection.txx>
#include <xscan/xscan_uniform_orbit.h>
#include <xmvg/io/xmvg_io_composite_filter_2d.h>
#include <vsl/vsl_basic_xml_element.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vnl/xio/vnl_xio_quaternion.txx>

template<class T, class F>
void bioproc_splr_filtering_proc<T, F>:: set_up_imgr_from_bin(vcl_string bin_scan_file){
    vsl_b_ifstream is(bin_scan_file.data());
    imgr_bounded_image_view_3d<unsigned short> * v3d = new imgr_bounded_image_view_3d<unsigned short>();
    v3d->b_read(is);
    view_3d_ = v3d;
}

template<class T, class F>
bioproc_splr_filtering_proc<T, F>:: bioproc_splr_filtering_proc(
                           const xscan_scan & scan, 
                           vgl_box_3d<double> box,
                           double resolution,
                           vgl_box_3d<double> outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file,
                           unsigned start_index, unsigned period)
: scan_(scan), box_(box), outer_box_(outer_box), filter_3d_(filter_3d), 
  resolution_(resolution), splr_(scan_, &filter_3d_, box, resolution_, start_index, period),
  response_field_(splr_.roster()) {
  if (bin_scan_file.size() > 0) {
    set_up_imgr_from_bin(bin_scan_file);
  } else {
    // set up image resource
    imgr();
  }
}

template<class T, class F>
bioproc_splr_filtering_proc<T, F>:: bioproc_splr_filtering_proc(
                           imgr_scan_images_resource const & scan_images,
                           vgl_box_3d<double> box,
                           double resolution,
                           vgl_box_3d<double> outer_box,
                           xmvg_composite_filter_3d<T, F> const& filter_3d,
                           vcl_string bin_scan_file,
                           unsigned start_index, unsigned period)
: scan_(scan_images.get_scan()), box_(box), outer_box_(outer_box), filter_3d_(filter_3d), 
  resolution_(resolution), splr_(scan_, &filter_3d_, box, resolution_, start_index, period),
  response_field_(splr_.roster()) {
  if (bin_scan_file.size() > 0) {
    set_up_imgr_from_bin(bin_scan_file);
  } else {
    // get bounded view
    view_3d_ = scan_images.get_bounded_view(outer_box_);
  }
}

template<class T, class F>
biob_worldpt_field<xmvg_filter_response<T> >  &
bioproc_splr_filtering_proc<T,F>::response_field() {
  return response_field_;
}

template<class T, class F>
const biob_worldpt_field<vnl_quaternion<double> >  &
bioproc_splr_filtering_proc<T,F>::rotation_field() {
  return splr_.rotation_field();
}

template<class T, class F>
void bioproc_splr_filtering_proc<T,F>::imgr(){
  imgr_scan_resource_sptr resc = imgr_scan_resource_io::read_resource(scan_);
  if(!resc){
    vcl_cerr << "******** (bioproc_splr_filtering_proc.txx) FAILED TO READ IMAGE RESOURCE ************\n";
  }
  else {
    view_3d_ = resc->get_bounded_view(outer_box_);
  }
}

template<class T, class F>
void bioproc_splr_filtering_proc<T, F> :: execute(vcl_ofstream * outputfile){
  /* Go through the representatives.  For each one, compute the splats
     (and compute responses for the corresponding representees)
  */
  unsigned counter = 0;
  biob_worldpt_index_enumerator_sptr iter = splr_.collection()->enumerator();//could provide a which_points here
  //but that would require implementing an unimplemented method of splr_subset_worldpt_roster
  while (iter->has_next()){
    //get next point index; if necessary, compute splat
    biob_worldpt_index pti = iter->next();
    if (++counter % 100 == 0){
      vcl_cout << "(bioproc_splr_filtering_proc) point " << counter << "\n";
    }
    xmvg_filter_response<T> response(filter_3d_.size(), T(0));
    for(orbit_index t = 0; t < scan_.scan_size(); t++){
      //: get view after cast to desired type
      imgr_bounded_image_view_3d<unsigned short> v3d = view_3d_;
      dbil_bounded_image_view<unsigned short> dv = v3d.view_2d(t);
      //fetch the splat (already incorporates ramp compensation)
      xmvg_composite_filter_2d<T> filter = splr_.collection()->splat(t, pti);
      filter.eliminate_residue_effect();
      // apply the 2d filter to image
      response += xmvg_dot_product_2d(filter, dv);
    }
    response_field_.values()[pti.index()] = response;
    if (outputfile){
      //write worldpt_index and response
      (*outputfile) << pti.index() << " " 
                    << response;//writes each of the filter responses in turn, then a newline
    }
  }
}

//: x_write should be called after execute() method is executed, because the 
// response values are undefined before that 
template<class T, class F>
void x_write(vcl_ostream &os, bioproc_splr_filtering_proc<T, F> &proc) {
  vsl_basic_xml_element element("bioproc_splr_filtering_proc");
  element.x_write_open(os);
  
  //: composite filter 3d
  //  xmvg_composite_filter_3d<T, F> f = proc.composite_filter();
  //  x_write(os, f);
  x_write(os, proc.composite_filter());
  x_write(os, proc.response_field());
  x_write(os, proc.rotation_field());
  element.x_write_close(os);
}

// Code for easy instantiation.
#undef BIOPROC_SPLR_FILTERING_PROC_INSTANTIATE
#define BIOPROC_SPLR_FILTERING_PROC_INSTANTIATE(T, F) \
template class bioproc_splr_filtering_proc<T, F>; \
template void x_write(vcl_ostream &, bioproc_splr_filtering_proc<T, F> &); \
SPLR_BASIC_PIZZA_SLICE_SPLAT_COLLECTION_INSTANTIATE(T, F ) \


#endif
