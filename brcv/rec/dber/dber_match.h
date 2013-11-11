//---------------------------------------------------------------------
// This is brcv/rec/dber/dber_match.h
//:
// \file
// \brief Edgel based matching algorithm
//        Given two sets of edgels, find a correspondence between edgels
//        based on edgel similarity
//
// \author
//  O.C. Ozcanli - October 23, 2006
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dber_match_h
#define _dber_match_h

#include <vcl_vector.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

#include <mbl/mbl_thin_plate_spline_2d.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

class dber_match 
{
 public:

  dber_match();
  virtual ~dber_match(){};
    
  vcl_vector<vsol_line_2d_sptr>& get_lines1() { return lines1_; }
  vcl_vector<vsol_line_2d_sptr>& get_lines2() { return lines2_; }

  void set_lines1(const vcl_vector<vsol_line_2d_sptr>& l);// { lines1_ = l; }
  void set_lines2(const vcl_vector<vsol_line_2d_sptr>& l);// { lines2_ = l; }

  vcl_vector<unsigned>& get_assignment() { return assign_; }

  void clear_assignment();
  void clear_lines2();
  void clear_lines1();

  void set_poly1(vsol_polygon_2d_sptr p) { poly1_ = p; }
  void set_poly2(vsol_polygon_2d_sptr p) { poly2_ = p; }

  //: match the current edgel-image pairs, uses mutual information based edgel similaity and hungarian matching
  double match(vcl_vector<vil_image_view<vxl_byte> >&set1, vcl_vector<vil_image_view<vxl_byte> >&set2, float smoothing_sigma, float width_radius, float radius);

  //: match the current edgel-image pairs, use greedy matching algorithm
  //  if cost is greater than the threshold, those matches are eliminated
  //  without being considered
  double match_greedy(double threshold);

  //: set sigma
  void set_sigma(double sigma) { sigma_square_ = sigma*sigma; }

  //: set scale factor
  void set_scale_factor(double scale) { scale_factor_ = scale; }

  //: set radius
  void set_radius(int radius) { radius_ = radius; }

  double find_global_mi(dbinfo_observation_sptr obs1, dbinfo_observation_sptr obs2);
  vil_image_resource_sptr get_correspondence_image(void) { return cor_im_; }

  bool find_tps(bool pure_affine);

 protected:

  //: correspondence image for debugging purposes
  vil_image_resource_sptr cor_im_;
  
  vsol_polygon_2d_sptr poly1_, poly2_;

  mbl_thin_plate_spline_2d tps_;
  vgl_norm_trans_2d<double> trans1_;
  vgl_norm_trans_2d<double> trans2_;

  vcl_vector<vsol_line_2d_sptr> lines1_, lines2_;
  
  // vector that holds the assigment of edgels
  vcl_vector<unsigned> assign_;
 // int num_cost_elems_;
  double sigma_square_;

  double scale_factor_;
  // neighborhood for each edgel
  int radius_;

  double maxval_;
};


#endif // _dber_match_h
