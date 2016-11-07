#ifndef dbrl_feature_point_tangent_h_
#define dbrl_feature_point_tangent_h_
//:
// \file
// \author Vishal Jain
// \date   Aug 2005

#include "dbrl_feature.h"
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class dbrl_feature_point_tangent: public dbrl_feature
{
 public:

  //:

  dbrl_feature_point_tangent( vnl_vector<double> const loc,double const dir);
  dbrl_feature_point_tangent( double x,double y,double dir);
  dbrl_feature_point_tangent();

  ~dbrl_feature_point_tangent();

  void set_location(vnl_vector<double> loc){location_=loc;}

  //virtual transform();

  virtual double distance(const dbrl_feature_sptr & pt);

  //: read a feature
  virtual
  bool read( vcl_istream& is);

  //: write a feature
  virtual
  void write( vcl_ostream& os ) const;

  vnl_vector<double>  location() const{return location_;}

  double  dir() const;

    vnl_vector<double>  location_homg() const;


    virtual void print_feature(vcl_ostream &os);

  // ==== Binary IO methods ======

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbrl_feature_point_tangent";}

  virtual dbrl_feature * clone() const;


 protected:

  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature.
  //



 protected:
 // vnl_vector<double> location_;
  double dir_;
};
//: Binary save dbrl_feature_point_tangent* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_feature_point_tangent* p);

//: Binary load dbrl_feature_point_tangent* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_feature_point_tangent* &p);

#endif // dbrl_feature_point_tangent_h_
