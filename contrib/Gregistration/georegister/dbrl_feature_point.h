#ifndef dbrl_feature_point_h_
#define dbrl_feature_point_h_
//:
// \file
// \author Vishal Jain
// \date   Aug 2005

#include "dbrl_feature.h"
#include "dbrl_feature_sptr.h"

#include <vcl_iosfwd.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector.h>
#include <vsl/vsl_binary_io.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//

class dbrl_feature_point  : public dbrl_feature
{
 public:

  //:
  dbrl_feature_point();
  dbrl_feature_point( vnl_vector<double> const& loc );
  dbrl_feature_point( double x,double y );
  ~dbrl_feature_point( );
  //virtual dbrl_feature_sptr transform(dbrl_transformation_sptr & tform);

  virtual double distance(const dbrl_feature_sptr & pt);

  //: read a feature
  virtual
  bool read( vcl_istream& is);

  //: write a feature
  virtual
  void write( vcl_ostream& os ) const;

  vnl_vector<double>  location() const{return location_;}

  void set_location(vnl_vector<double> loc){location_=loc;}

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
  virtual vcl_string is_a() const {return "dbrl_feature_point";}

  virtual dbrl_feature * clone() const;
 protected:
  //:
  // Create an uninitialized feature with enough space to store a dim
  // dimensional feature.
  //
  //dbrl_feature_point( unsigned dim=0 );


 protected:
  //vnl_vector<double> location_;
};

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_feature_point* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_feature_point* &p);

#endif // dbrl_feature_point_h_
