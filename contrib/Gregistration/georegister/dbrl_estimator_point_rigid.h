#ifndef _dbrl_estmator_point_rigid_h_
#define _dbrl_estmator_point_rigid_h_

#include <rgrl/rgrl_feature_sptr.h>

class dbrl_estimator_point_rigid
    {   
    public:
    dbrl_estimator_point_rigid(){};
    virtual ~dbrl_estimator_point_rigid(){};

    virtual
    dbrl_transformation_sptr estimate( vcl_vector<rgrl_feature_sptr> f1,
                                       vcl_vector<rgrl_feature_sptr> f2,
                                       dbrl_correspondence M) ;

     //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbrl_estimator_point_rigid";}

  virtual dbrl_estimator * clone() const;
    };
//: Binary save dbrl_estimator_point_rigid* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_point_rigid* p);

//: Binary load dbrl_estimator_point_rigid* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_point_rigid* &p);

#endif
