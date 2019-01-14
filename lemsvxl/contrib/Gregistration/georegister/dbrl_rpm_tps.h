#ifndef _dbrl_rpm_tps_
#define _dbrl_rpm_tps_

#include "dbrl_robust_point_matching.h"
#include <iostream>
#include "dbrl_match_set_sptr.h"
#include "dbrl_estimator_thin_plate_spline.h"
#include "dbrl_correspondence.h"
#include "dbrl_thin_plate_spline_transformation.h"

class dbrl_rpm_tps_params:public dbrl_robust_point_matching_params
    {
    public:
        dbrl_rpm_tps_params();
        dbrl_rpm_tps_params(double initlambda1,double initlambda2,double Mconvg);
        dbrl_rpm_tps_params(double initlambda1,double initlambda2,double Mconvg,double Tstart,
                                 double Tend,double outlierval,double annealrate);
        ~dbrl_rpm_tps_params(){};

        double initlambda1(){return initlambda1_;};
        double initlambda2(){return initlambda2_;};
        double Mconvg(){return Mconvg_;};
    protected:
        double initlambda1_;
        double initlambda2_;
        //; convergence error for M 
        double Mconvg_;


    };
class dbrl_rpm_tps: public dbrl_robust_point_matching
    {
    public:
        dbrl_rpm_tps(){};
        virtual ~dbrl_rpm_tps(){};
        dbrl_rpm_tps(dbrl_rpm_tps_params & params,
                                 std::vector<dbrl_feature_sptr>  f1,
                                 std::vector<dbrl_feature_sptr>  f2);
        virtual dbrl_match_set_sptr rpm(std::string name="Euclidean");
        void normalize_point_set(vnl_matrix<double> & M, std::vector<dbrl_feature_sptr> & f);
        bool rpm_at(double T,dbrl_correspondence & M,dbrl_estimator_point_thin_plate_spline * tps_est,
                    dbrl_transformation_sptr &tform,std::vector<dbrl_feature_sptr> &f1x,std::vector<dbrl_feature_sptr> & f2x,
                    double l1,double l2,std::string name="Euclidean");

        double energy(){return energy_;}

        double distance(std::vector<dbrl_feature_sptr> f1,
                std::vector<dbrl_feature_sptr> f2);
    protected:
        dbrl_rpm_tps_params params_;

        double lambda1;
        double lambda2;
        double energy_;
       
    };
#endif
