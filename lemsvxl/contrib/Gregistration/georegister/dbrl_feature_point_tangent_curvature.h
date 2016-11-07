#ifndef dbrl_feature_point_tangent_curvature_h_
#define dbrl_feature_point_tangent_curvature_h_
//:
// \file
// \author Vishal Jain
// \date   Aug 2005

#include "dbrl_feature.h"
#include <vcl_iosfwd.h>
#include <vnl/vnl_vector.h>
#include <dbgl/algo/dbgl_circ_arc.h>

//: Represent a point in space.
//
// This type of feature can be used to represent simple point-to-point
// correspondences.
//
class dbrl_feature_point_tangent_curvature: public dbrl_feature
{
 public:

  //:
  dbrl_feature_point_tangent_curvature( vnl_vector<double> const loc,double const dir);
  dbrl_feature_point_tangent_curvature( double x,double y,double dir);
  dbrl_feature_point_tangent_curvature();
  ~dbrl_feature_point_tangent_curvature();

  bool compute_arc(double arclength);

  double  dir() const;
  double  k() const{return k_;}
  void set_k(double k){k_=k;is_k_set_=true;}
  void set_location(vnl_vector<double> loc){location_=loc;}

  //dbgl_circ_arc get_arc(){return circarc_;}
  vnl_vector<double>  location_homg() const;
  void set_k_flag(){is_k_set_=true;}
  bool k_flag(){return is_k_set_;}

  vnl_vector<double> const& location() const{return location_;}



  virtual 
     double distance(const dbrl_feature_sptr & pt);

  //: read a feature
  virtual
      bool read( vcl_istream& is);

  //: write a feature
  virtual
      void write( vcl_ostream& os ) const;

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
  virtual vcl_string is_a() const {return "dbrl_feature_point_tangent_curvature";}

  virtual dbrl_feature * clone() const;


 protected:



 protected:
  //vnl_vector<double> location_;
  double dir_;
  double k_;
 // dbgl_circ_arc circarc_;
  bool is_k_set_;
};

#endif // dbrl_feature_point_tangent_curvature_h_
