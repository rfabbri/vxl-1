#ifndef _dbrl_rpm_affine_
#define _dbrl_rpm_affine_
#include "dbrl_robust_point_matching.h"
#include <iostream>
#include "dbrl_match_set_sptr.h"
#include "dbrl_estimator_point_affine.h"
#include "dbrl_correspondence.h"
#include "dbrl_affine_transformation.h"

class dbrl_rpm_affine_params:public dbrl_robust_point_matching_params
    {
    public:
        dbrl_rpm_affine_params();
        dbrl_rpm_affine_params(double initlambda,double Mconvg);
        dbrl_rpm_affine_params(double initlambda,double Mconvg,double Tstart,
                                 double Tend,double outlierval,double annealrate);
        ~dbrl_rpm_affine_params(){};

        double initlambda(){return initlambda_;};
        double Mconvg(){return Mconvg_;};
       
    protected:
        double initlambda_;
        //; convergence error for M 
        double Mconvg_;


    };
class dbrl_rpm_affine: public dbrl_robust_point_matching
    {
    public:
        dbrl_rpm_affine(dbrl_rpm_affine_params & params,
            std::vector<dbrl_feature_sptr>  f1,
            std::vector<dbrl_feature_sptr>  f2);
        virtual ~dbrl_rpm_affine(){};

        virtual dbrl_match_set_sptr rpm(std::string name="Euclidean");

        bool rpm_at(double T,dbrl_correspondence & M,dbrl_estimator_point_affine * affine_est,
            dbrl_transformation_sptr &tform,std::vector<dbrl_feature_sptr> &f1x,
            std::vector<dbrl_feature_sptr> & f2x,
            double l,std::string name="Euclidean");

        double distance(std::vector<dbrl_feature_sptr> f1,
                std::vector<dbrl_feature_sptr> f2, dbrl_correspondence & M);

        double energy(){return energy_;}
    protected:
        dbrl_rpm_affine_params params_;
        double energy_;

    };

#endif
