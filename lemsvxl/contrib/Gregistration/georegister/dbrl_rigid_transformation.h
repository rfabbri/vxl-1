#ifndef _dbrl_rigid_transformation_h
#define _dbrl_rigid_transformation_h

#include "dbrl_transformation.h"
#include <vnl/vnl_matrix.h>
#include <vsl/vsl_binary_io.h>

class dbrl_rigid_transformation:public dbrl_transformation
    {
    public:
        dbrl_rigid_transformation(){};
        dbrl_rigid_transformation(vnl_vector<double> t,vnl_matrix<double> r,double scale):T_(t),R_(r),scale_(scale){};
        ~dbrl_rigid_transformation(){};

        virtual bool transform();
        virtual void print_transformation(vcl_ostream &os){}

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
    virtual vcl_string is_a() const {return "dbrl_rigid_transformation";}

    virtual  dbrl_transformation * clone() const;
    protected:
        vnl_vector<double> T_;
        vnl_matrix<double> R_;
        double scale_;

        vnl_vector<double> map_location(vnl_vector<double> frompt);
        vnl_vector<double> map_dir(vnl_vector<double> &fromdir);




    };

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_rigid_transformation* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_rigid_transformation* &p);

#endif
