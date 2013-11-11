#ifndef _dbrl_rpm_rigid_
#define _dbrl_rpm_rigid_

class dbrl_rpm_rigid: public dbrl_robust_point_matching
    {
    public:
        dbrl_rpm_rigid(){};
        ~dbrl_rpm_rigid(){};

        virtual dbrl_match_set_sptr rpm();



    protected:
        double theta_;
        vnl_vector<double> mu1_;
        vnl_vector<double> mu2_;

        vnl_matrix<double> computerotationmatrix(double theta);
    };
#endif
