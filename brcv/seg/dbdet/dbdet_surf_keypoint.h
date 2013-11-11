// This is brcv/seg/dbdet/dbdet_surf_keypoint.h
#ifndef dbdet_surf_keypoint_h_
#define dbdet_surf_keypoint_h_
//:
// \file
// \brief A keypoint with scale, orientation, and descriptor
//   This keypoint is implemented as described in
//  \verbatim
//    "SURF: Speeded Up Robust Features"
//    Herbert Bay, Tinne Tuytelaars, Luc Van Gool
//    Proceedings of the ninth European Conference on Computer Vision, May 2006
//  \endverbatim
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/27/08
//
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbdet_keypoint.h"
#include "dbdet_keypoint_sptr.h"
#include <vcl_vector.h>
#include <vil/vil_image_resource_sptr.h>

//: A Surf keypoint as described 
class dbdet_surf_keypoint : public dbdet_keypoint
{
 public:
  //: Constructor
  dbdet_surf_keypoint() {}
  //: Constructor
  dbdet_surf_keypoint(double x, double y, double s=1.0, double o=0.0,
                      const vnl_vector<double>& d=vnl_vector<double>())
   : dbdet_keypoint(x,y), scale_(s), orientation_(o), descriptor_(d) {}
  //: Destructor
  virtual ~dbdet_surf_keypoint() {}
  
  virtual bool has_scale() const { return true; }
  virtual double scale() const { return scale_; }
  virtual bool set_scale(double s) { scale_ = s; return true; }
  
  virtual bool has_orientation() const { return true; }
  virtual double orientation() const { return orientation_; }
  virtual bool set_orientation(double o) { orientation_ = o; return true; }
  
  virtual unsigned int descriptor_size() const { return 64; }
  virtual const vnl_vector<double>& descriptor() const { return descriptor_; }
  virtual bool set_descriptor(const vnl_vector<double>& d) { descriptor_ = d; return true; }


  //=============== Binary I/O Methods ========================
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_surf_keypoint"; }

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
  bool sign_;
  vnl_vector<double> descriptor_;

  friend vcl_istream& operator >> (vcl_istream& is, dbdet_surf_keypoint& s);
};


vcl_istream& operator >> (vcl_istream& is, dbdet_surf_keypoint& s);


#endif // dbdet_surf_keypoint_h_
