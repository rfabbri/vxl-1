#ifndef dbrl_feature_point_tangent_curvature_groupings_h_
#define dbrl_feature_point_tangent_curvature_groupings_h_
//:
// \file
// \author Vishal Jain
// \date   Aug 2005

#include "dbrl_feature_point_tangent_curvature.h"
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class dbrl_feature_point_tangent_curvature_groupings: public dbrl_feature_point_tangent_curvature
{
 public:

  //:
  dbrl_feature_point_tangent_curvature_groupings( vnl_vector<double> const loc,double const dir,vcl_vector<unsigned> ns);
  dbrl_feature_point_tangent_curvature_groupings( double x,double y,double dir,vcl_vector<unsigned> ns);
  dbrl_feature_point_tangent_curvature_groupings();
  ~dbrl_feature_point_tangent_curvature_groupings();


       vcl_vector<unsigned> feature_neighbor_map_;
  void set_location(vnl_vector<double> loc){location_=loc;}

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
  virtual vcl_string is_a() const {return "dbrl_feature_point_tangent_curvature_groupings";}

  virtual dbrl_feature * clone() const;



 protected:

};

#endif // dbrl_feature_point_tangent_curvature_h_
