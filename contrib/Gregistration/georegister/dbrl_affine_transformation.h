#ifndef _dbrl_affine_transformation_h
#define _dbrl_affine_transformation_h

#include "dbrl_transformation.h"
#include <vnl/vnl_matrix.h>

class dbrl_affine_transformation:public dbrl_transformation
    {
    public:
        dbrl_affine_transformation();
        dbrl_affine_transformation(vnl_matrix<double> A):A_(A){};
        ~dbrl_affine_transformation(){};

        virtual bool transform();
        double frobenius_norm();

        virtual void print_transformation(vcl_ostream &os);

          // ==== Binary IO methods ======

        double energy(); 
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbrl_affine_transformation";}



  virtual dbrl_transformation * clone() const;
    protected:
        vnl_matrix<double> A_;
        vnl_vector<double> map_location(vnl_vector<double> frompt) const;
        vnl_vector<double> map_dir(vnl_vector<double> &fromdir) const;



    };

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_affine_transformation* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_affine_transformation* &p);

#endif
