// This is brcv/seg/dbdet/dbdet_arc_patch.h
#ifndef dbdet_arc_patch_h_
#define dbdet_arc_patch_h_

//:
// \file
// \brief An image patch whose central axis is a circular arc (instead of a segment)
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 22, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_line_segment_2d.h>
#include <dbgl/algo/dbgl_circ_arc.h>







// =============================================================================
// dbdet_arc_patch
// =============================================================================



//: A detected keypoint (base class)
class dbdet_arc_patch : public vbl_ref_count
{
public:
  //: Constructor
  dbdet_arc_patch() {}

  // Constructor 2
  dbdet_arc_patch(const dbgl_circ_arc& arc, double width): axis_(arc), width_(width){}
  
  //: Destructor
  virtual ~dbdet_arc_patch() {}

  // DATA ACCESS ---------------------------------------------------------------

  //: central axis of the patch
  dbgl_circ_arc axis() const {return this->axis_; }

  //: width of the patch
  double width() const { return this->width_; }

  //: Compute coordinate and orientation of a grid of points
  bool compute_grid(double dx, double dy, vnl_matrix<double >& x,
    vnl_matrix<double >& y, vnl_matrix<double >& vx, vnl_matrix<double >& vy) const;

  //: Approximate center line of this patch
  vgl_line_segment_2d<double > center_line() const;

  //: Print an ascii summary to the stream
  vcl_ostream& print_summary(vcl_ostream &os) const {return os;}

protected:
  dbgl_circ_arc axis_;
  double width_;
};


//: Compute histogram of orientated gradient (HOG) in an arc patch, taking the 
// central axis of the patch as the reference direction
vnl_vector<double > dbdet_compute_HOG(const vil_image_view<double >& Gx, 
                                     const vil_image_view<double >& Gy,
                                     const dbdet_arc_patch& patch,
                                     int num_bins = 9);


//: Compute mean gradient magnitude in a patch
double dbdet_compute_mean_gradient_magnitude(const vil_image_view<double >& Gx, 
                                     const vil_image_view<double >& Gy,
                                     const dbdet_arc_patch& patch,
                                     double dx = 1,
                                     double dy = 1);



#endif // dbdet_arc_patch_h_
