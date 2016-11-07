// This is brcv/seg/dbdet/dbdet_lowe_keypoint.h
#ifndef dbdet_lowe_keypoint_h_
#define dbdet_lowe_keypoint_h_
//:
// \file
// \brief A keypoint with scale, orientation, and descriptor
//   This keypoint is implemented as described in
//  \verbatim
//   "Distinctive Image Features from Scale-Invariant Keypoints"
//    David G. Lowe,
//    International Journal of Computer Vision, 60, 2 (2004), pp. 91-110.
//  \endverbatim
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/22/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbdet_keypoint.h"
#include <bil/algo/bil_scale_image.h>


//: A keypoint as described by David Lowe
class dbdet_lowe_keypoint : public dbdet_keypoint
{
 public:
  //: Constructor
  dbdet_lowe_keypoint() {}
  //: Constructor
  dbdet_lowe_keypoint(double x, double y, double s=1.0, double o=0.0,
                      const vnl_vector<double>& d=vnl_vector<double>())
   : dbdet_keypoint(x,y), scale_(s), orientation_(o), descriptor_(d) {}
  //: Destructor
  virtual ~dbdet_lowe_keypoint() {}
  
  virtual bool has_scale() const { return true; }
  virtual double scale() const { return scale_; }
  virtual bool set_scale(double s) { scale_ = s; return true; }
  
  virtual bool has_orientation() const { return true; }
  virtual double orientation() const { return orientation_; }
  virtual bool set_orientation(double o) { orientation_ = o; return true; }
  
  virtual unsigned int descriptor_size() const { return 128; }
  virtual const vnl_vector<double>& descriptor() const { return descriptor_; }
  virtual bool set_descriptor(const vnl_vector<double>& d) { descriptor_ = d; return true; }

  //: normalize the descriptor
  void normalize_descriptor(double max_comp = 0.2);

  //: compute the descriptor
  void compute_descriptor(const bil_scale_image<float>& scale_grad_dir,
                          const bil_scale_image<float>& scale_grad_mag);

  //=============== Binary I/O Methods ========================
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_lowe_keypoint"; }

  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual dbdet_keypoint* clone() const;

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  virtual void print_summary(vcl_ostream &os) const;
  
 private:
  double scale_;
  double orientation_;
  vnl_vector<double> descriptor_;

};




#endif // dbdet_lowe_keypoint_h_
