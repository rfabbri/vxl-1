#ifndef _dbrl_cubic_transformation_h
#define _dbrl_cubic_transformation_h

#include "dbrl_transformation.h"
#include <vnl/vnl_matrix.h>
#include <dt/dbrl_clough_tocher_patch.h>

class dbrl_cubic_transformation:public dbrl_transformation
    {
    public:
        dbrl_cubic_transformation();
        dbrl_cubic_transformation(vcl_vector<dbrl_clough_tocher_patch> patchesx,
            vcl_vector<dbrl_clough_tocher_patch> patchesy);
        ~dbrl_cubic_transformation(){}

        virtual bool transform();

        virtual void print_transformation(vcl_ostream &os);

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
        virtual vcl_string is_a() const {return "dbrl_cubic_transformation";}

        virtual dbrl_transformation * clone() const;
    protected:
        vcl_vector<dbrl_clough_tocher_patch> patchesx_;
        vcl_vector<dbrl_clough_tocher_patch> patchesy_;

        vnl_vector<double> map_location(vnl_vector<double> frompt) const;
        vnl_vector<double> map_dir(vnl_vector<double> &fromdir) const;


    };

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_cubic_transformation* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_cubic_transformation* &p);

#endif
