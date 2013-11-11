#ifndef _dbrl_thin_plate_spline_transformation_h
#define _dbrl_thin_plate_spline_transformation_h

#include "dbrl_transformation.h"
#include <vnl/vnl_matrix.h>

class dbrl_thin_plate_spline_transformation:public dbrl_transformation
    {
    public:
        dbrl_thin_plate_spline_transformation(){};
        dbrl_thin_plate_spline_transformation(vnl_matrix<double> A,vnl_matrix<double> warp_coeffs, vnl_matrix<double> tps_kernel, vcl_vector<dbrl_feature_sptr> fs);
        ~dbrl_thin_plate_spline_transformation(){};

        virtual bool transform();
        
        double warp_frobenius_norm();
        double affine_frobenius_norm();
        virtual void print_transformation(vcl_ostream &os);

        vnl_matrix<double> get_warp_coeffs(){return warp_coeffs_;}
        vnl_matrix<double> get_affine(){return A_;}
        void build_K(vcl_vector<dbrl_feature_sptr> &x ) ;


        void set_lambda1(double lambda1){lambda1_=lambda1;}
        void set_lambda2(double lambda2){lambda2_=lambda2;}
        double compute_energy();

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
        virtual vcl_string is_a() const {return "dbrl_thin_plate_spline_transformation";}

    virtual dbrl_transformation * clone() const;
    protected:

        vnl_matrix<double> A_;
        vnl_matrix<double> tps_kernel_;
        vnl_matrix<double> warp_coeffs_;
        vnl_matrix<double> tps_warp_;
        vnl_vector<double> map_location(vnl_vector<double> &  frompt) ;
        vnl_vector<double> map_dir(vnl_vector<double> &fromdir) const;

        vnl_vector<double> map_location(vnl_vector<double> pt,int index);

        vcl_vector<dbrl_feature_sptr> fs_;
vnl_matrix<double> base_pts;
        double lambda1_;
        double lambda2_;
    };

//: Binary save dbrl_feature_point* to stream.
void vsl_b_write(vsl_b_ostream &os, const dbrl_thin_plate_spline_transformation* p);

//: Binary load dbrl_feature_point* from stream.
void vsl_b_read(vsl_b_istream &is, dbrl_thin_plate_spline_transformation* &p);

#endif
