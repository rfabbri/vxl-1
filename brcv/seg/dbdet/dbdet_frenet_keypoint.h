// This is dbdet_frenet_keypoint.h
#ifndef dbdet_frenet_keypoint_h
#define dbdet_frenet_keypoint_h
//:
//\file
//\brief A keypoint with attached Frenet frame (tangent,normal,curvature,etc)
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 01/17/07 14:16:09 EST
//


#include "dbdet_keypoint.h"

//: A keypoint consisting of point position, and tangent, normal, curvature, etc at such point.
// Up to 3rd-order intrinsic geometry can be specified.
//
// If the keypoint has degenerate/singular local differential geometry, the flag valid() indicates
// this.
// 
class dbdet_frenet_keypoint : public dbdet_keypoint
{
 public:
  //: Constructor
  dbdet_frenet_keypoint() {}

  //: Constructor
  //\param[in] valid : true if point is generic/non-singular
  //
  dbdet_frenet_keypoint(
      double x, double y,
      double tx, double ty,
      double nx, double ny,
      double k, double kdot,
      bool valid = true
      )
   : dbdet_keypoint(x,y),
   tx_(tx),
   ty_(ty),
   nx_(nx),
   ny_(ny),
   k_(k),
   kdot_(kdot),
   valid_(valid)
  {}

  //: Destructor
  virtual ~dbdet_frenet_keypoint() {}
  
  virtual bool has_scale() const { return false; }
  virtual double scale() const { return 1.0; }
  virtual bool set_scale(double /*s*/) { return false; }
  
  virtual bool has_orientation() const { return true; }
  //: result between -PI and PI (inclusive)
  virtual double orientation() const { return vcl_atan2(ty_,tx_); }
  virtual bool set_orientation(double /*o*/) { return true; }
  
  //=============== Binary I/O Methods ========================
  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return "dbdet_frenet_keypoint"; }

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
  
public:
   double tx_;
   double ty_;
   double nx_;
   double ny_ ;
   double k_;
   double kdot_; 
   bool valid_;
};


#endif // dbdet_frenet_keypoint_h

