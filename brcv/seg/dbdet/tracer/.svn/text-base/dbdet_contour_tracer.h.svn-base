// This is dbdet_contour_tracer.h
#ifndef dbdet_contour_tracer_h
#define dbdet_contour_tracer_h
//:
//\file
//\brief Subpixel ENO contour tracer 
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 10/28/2005 09:36:41 AM EDT
//
//\verbatim
//  Modifications
//  Ozge C. Ozcanli April 26, 2006 -- Added the 4th step of Gaussian smoothing
//                                    CAUTION: only the largest contour is smoothed!!
//\endverbatim
//

#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_point_2d.h>



//:  Subpixel ENO contour tracer
// Detects boundaries of objects in binary images and traces them into 
// ordered sets of points.
//
// Overal algorithm:
//  1) Signed DT 
//  2) Image Curvature smoothing of the Signed DT surface
//  3) Contour tracing
//  4) Gaussian smoothing of the resulting contour
// 
// General idea at: Siddiqi, Kimia, and Shu GMIP 1997
//
// Known bugs: FIXME
//  - Nhon Trihn has a case where the tracer gives a wrong contour for an object
//  touching the lower image boundary.
//
class dbdet_contour_tracer : public vbl_ref_count {
public:

  dbdet_contour_tracer()
    : sigma_(0),
      curvature_smooth_nsteps_(1),
      curvature_smooth_beta_(0.3f),
      index_of_max_contour_(0)
    {}
  ~dbdet_contour_tracer() {};

  //: sigma for Gaussian smoothing of traced contours
  void set_sigma(float new_sigma) { sigma_ = new_sigma; }

  void set_curvature_smooth_nsteps(unsigned nsteps)
    { curvature_smooth_nsteps_ = nsteps;  }

  void set_curvature_smooth_beta(float beta)
    { curvature_smooth_beta_ = beta; }

  //: Contour tracing on a binary image
  bool trace(const vil_image_view<bool> &img);

  //: Contour tracing on a signed euclidean distance transform image (or a
  // embedding surface image in which the zero-levelset defines the contours to
  // be traced.
  bool trace_sedt_image(const vil_image_view<float >& s_edt);

  const vcl_vector<vcl_vector<vsol_point_2d_sptr> > & contours() const
    { return contours_; }

  //: returns largest traced contour
  const vcl_vector<vsol_point_2d_sptr>& largest_contour() const
    { return contours_[index_of_max_contour_]; }

  //: index of largest contour in the structure returned by contours()
  unsigned index_of_largest_contour() const
    { return index_of_max_contour_; }

  bool output_con_file(const vcl_string &filename, unsigned idx);

  bool output_con_file(const vcl_string &filename)
    { return output_con_file(filename, index_of_max_contour_); }

protected:
  // parameters:
  float sigma_;
  unsigned curvature_smooth_nsteps_;
  float curvature_smooth_beta_;

  //: Vector of detected contours
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > contours_;
  //: index of largest contour in the vector
  unsigned index_of_max_contour_;

  // internal curvature smoothing code (curvature_smoothing.cxx)
  void curvature_smoothing(double *image_array, double beta, int steps, int scale, 
                           int height, int width) const;

};


#endif // dbdet_contour_tracer_h
