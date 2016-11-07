#ifndef _dbrl_estmator_point_affine_h_
#define _dbrl_estmator_point_affine_h_
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include "dbrl_feature_sptr.h"
#include "dbrl_correspondence.h"
#include "dbrl_transformation_sptr.h"
#include "dbrl_estimator.h"
#include <vsl/vsl_binary_io.h>

class dbrl_estimator_point_affine:public dbrl_estimator
    {   
    public:
    dbrl_estimator_point_affine(){lambda_=1.0;};
     ~dbrl_estimator_point_affine(){};

    virtual
    dbrl_transformation_sptr estimate( vcl_vector<dbrl_feature_sptr> f1,
                                       vcl_vector<dbrl_feature_sptr> f2,
                                       dbrl_correspondence & M) const;
    virtual
    double residual(const vcl_vector<dbrl_feature_sptr>& f1,
                    const vcl_vector<dbrl_feature_sptr>& f2,
                    const dbrl_correspondence &M,
                    const dbrl_transformation_sptr& tform) const;
    void set_lambda(double lambda){lambda_=lambda;}

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
  virtual vcl_string is_a() const {return "dbrl_estimator_point_affine";}

  virtual dbrl_estimator * clone() const;

    protected:
        double lambda_;

    };

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_estimator_point_affine* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_estimator_point_affine* &p);


#endif
