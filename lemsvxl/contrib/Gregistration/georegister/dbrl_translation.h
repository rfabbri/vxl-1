#ifndef _dbrl_translation_h
#define _dbrl_translation_h

#include "dbrl_transformation.h"

class dbrl_translation:public dbrl_transformation
    {
    public:
    dbrl_translation(){}
    dbrl_translation(vnl_vector<double> t):T_(t){}
    ~dbrl_translation(){}

    virtual bool transform();
    virtual void print_transformation(vcl_ostream &os){}

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
        vnl_vector<double> T_;
                vnl_vector<double> map_location(vnl_vector<double> frompt) const;
        vnl_vector<double> map_dir(vnl_vector<double> &fromdir) const;


    vnl_vector<double> map_location(vnl_vector<double> frompt);
    vnl_vector<double> map_dir(vnl_vector<double> &fromdir);    

    };

//: Binary save dbrl_translation* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_translation* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_translation* &p);

#endif
