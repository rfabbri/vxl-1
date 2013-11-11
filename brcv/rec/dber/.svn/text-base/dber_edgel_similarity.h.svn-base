//---------------------------------------------------------------------
// This is brcv/rec/dber/dber_edgel_similarity.h
//:
// \file
// \brief Various edgel similarity measures
//
// \author
//  O.C. Ozcanli - October 23, 2006
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dber_edgel_similarity_h
#define _dber_edgel_similarity_h

#include <vcl_vector.h>
#include <vsol/vsol_line_2d_sptr.h>

#include <vil/vil_image_view.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>


class dber_edgel_similarity 
{
 public:

  dber_edgel_similarity() {};
  virtual ~dber_edgel_similarity(){};
  
  //: compute current matching cost of matching these two edgels
  //  small if spatially distant and orientatations are different
  //  if orthogonal this is zero, (no polarizations)
  static double compute_current_cost(vsol_line_2d_sptr e1, vsol_line_2d_sptr e2, double sigma_square, double & max);

  //: compute the normalized current norm between two edgel sets 
  static double current_norm(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square);

  //: measure support for lines1 by lines2 wrt current edgel similarity measure
  static double measure_support(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square);

  //: get image sets for each edgel sets and prepare gradient images, etc.
  void prepare_images(vcl_vector<vil_image_view<vxl_byte> >&set1, vcl_vector<vil_image_view<vxl_byte> >&set2, float smoothing_sigma);

  void update_histograms_from_all_planes(int x1, int y1, int x2, int y2, 
                                         bsta_histogram<float>& h1, 
                                         bsta_histogram<float>& h2, 
                                         bsta_joint_histogram<float>& jh);

  //: compute cost of matching these two edgels, 
  //  the cost is based on gradient mutual information in the neighborhood of the edgels
  double compute_cost(vsol_line_2d_sptr e1, vsol_line_2d_sptr e2, double width_radius, double radius);

protected:

  //: If a mutual information based similarity will be used,
  //  then prepare and save images in this class

  //: treat images coming from different frames separately
  vcl_vector< vil_image_view<vxl_byte> > im1_views_;
  vcl_vector< vil_image_view<vxl_byte> > im2_views_;
  //: gradients of each frame
  vcl_vector< vil_image_view<float> > I1xs_, I1ys_;
  vcl_vector< vil_image_view<float> > I2xs_, I2ys_;
  unsigned nplanes_;

};


#endif // _dber_edgel_similarity_h
