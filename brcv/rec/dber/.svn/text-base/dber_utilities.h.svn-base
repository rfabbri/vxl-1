//---------------------------------------------------------------------
// This is brcv/rec/dber/dber_match.h
//:
// \file
// \brief Edgel based matching utilities
//
// \author
//  O.C. Ozcanli - October 23, 2006
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------
#ifndef _dber_utilities_h
#define _dber_utilities_h

#include <vcl_vector.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <bsta/bsta_histogram.h>
#include <bsta/bsta_joint_histogram.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

//: This class computes properties of edgel sets given as vsol_polyline vectors
class dber_utilities
{
 public:

  dber_utilities() {};
  //dber_match(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2);
  //dber_match(dbskr_scurve_sptr c1,dbskr_scurve_sptr c2, double R1, vcl_vector<double>& lambda, int num_cost_elems);
  virtual ~dber_utilities(){};

  //:  scale all edgels in lines2 so that widhts are equal
  static void scale_lines(vcl_vector<vsol_line_2d_sptr>& l, double scale_factor);
  static void translate_lines(vcl_vector<vsol_line_2d_sptr>& l, double x, double y);
  static vsol_box_2d_sptr get_box(vcl_vector<vsol_line_2d_sptr>& l);

  //: rotate the lines about the given center point
  static void rotate_lines(vcl_vector<vsol_line_2d_sptr>& l, vgl_h_matrix_2d<double> H, double cent_x, double cent_y);

  //: translate lines so that translation is optimal
  static void optimize_translation(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square);

  //: find center of gravity of edgels
  static vsol_point_2d_sptr center_of_gravity(vcl_vector<vsol_line_2d_sptr>& l1);

  //: find the dominant direction of a polygon by fitting a line via linear regression
  static vgl_line_2d<double> find_dominant_dir(vsol_polygon_2d_sptr poly, double x, double y);
};

//: A least squares cost function for registering edgel sets
// by minimizing current-matching norm
class dber_cost_func : public vnl_cost_function
{
public:
  //: Constructors/Destructor

  dber_cost_func(unsigned number_of_parameters, vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2);
  ~dber_cost_func(){}
  
  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  fx has been sized appropriately before the call.
  virtual double f(vnl_vector<double> const& x);

  void set_sigma_square(double sigma) { sigma_square_ = sigma; }

protected:

  //: number of transform parameters
  unsigned no_params_;

  vcl_vector<vsol_line_2d_sptr> lines1_;
  vcl_vector<vsol_line_2d_sptr> lines2_;

  double maxval_;
  double sigma_square_;
  
 private:
  //:not sensible
  dber_cost_func();
  //
};


#endif // _dber_utilities_h
