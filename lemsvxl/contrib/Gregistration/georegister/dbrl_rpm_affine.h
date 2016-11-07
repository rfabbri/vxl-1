#ifndef _dbrl_rpm_affine_
#define _dbrl_rpm_affine_
#include "dbrl_robust_point_matching.h"
#include <vcl_iostream.h>
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
            vcl_vector<dbrl_feature_sptr>  f1,
            vcl_vector<dbrl_feature_sptr>  f2);
        virtual ~dbrl_rpm_affine(){};

        virtual dbrl_match_set_sptr rpm(vcl_string name="Euclidean");

        bool rpm_at(double T,dbrl_correspondence & M,dbrl_estimator_point_affine * affine_est,
            dbrl_transformation_sptr &tform,vcl_vector<dbrl_feature_sptr> &f1x,
            vcl_vector<dbrl_feature_sptr> & f2x,
            double l,vcl_string name="Euclidean");

        double distance(vcl_vector<dbrl_feature_sptr> f1,
                vcl_vector<dbrl_feature_sptr> f2, dbrl_correspondence & M);

        double energy(){return energy_;}
    protected:
        dbrl_rpm_affine_params params_;
        double energy_;

    };

#endif
