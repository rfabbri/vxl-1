#ifndef _dbrl_rpm_tps_
#define _dbrl_rpm_tps_

#include "dbrl_robust_point_matching.h"
#include <vcl_iostream.h>
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
                                 vcl_vector<dbrl_feature_sptr>  f1,
                                 vcl_vector<dbrl_feature_sptr>  f2);
        virtual dbrl_match_set_sptr rpm(vcl_string name="Euclidean");
        void normalize_point_set(vnl_matrix<double> & M, vcl_vector<dbrl_feature_sptr> & f);
        bool rpm_at(double T,dbrl_correspondence & M,dbrl_estimator_point_thin_plate_spline * tps_est,
                    dbrl_transformation_sptr &tform,vcl_vector<dbrl_feature_sptr> &f1x,vcl_vector<dbrl_feature_sptr> & f2x,
                    double l1,double l2,vcl_string name="Euclidean");

        double energy(){return energy_;}

        double distance(vcl_vector<dbrl_feature_sptr> f1,
                vcl_vector<dbrl_feature_sptr> f2);
    protected:
        dbrl_rpm_tps_params params_;

        double lambda1;
        double lambda2;
        double energy_;
       
    };
#endif
