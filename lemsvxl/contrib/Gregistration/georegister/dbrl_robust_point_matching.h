#ifndef _dbrl_robust_point_matching_
#define _dbrl_robust_point_matching_
#include "dbrl_feature_sptr.h"
#include <vector>
#include <vnl/vnl_matrix.h>
#include "dbrl_match_set_sptr.h"
class dbrl_robust_point_matching
    {   
    public:
        dbrl_robust_point_matching();
        virtual ~dbrl_robust_point_matching(){}

        virtual dbrl_match_set_sptr rpm(std::string name="Euclidean")=0;
        vnl_matrix<double> compute_correspondence_weights(double T,
                                                          std::vector<dbrl_feature_sptr> f1,
                                                          std::vector<dbrl_feature_sptr> f2);

        vnl_matrix<double> compute_correspondence_point_tangent_weights(double T,
                                                          std::vector<dbrl_feature_sptr> f1,
                                                          std::vector<dbrl_feature_sptr> f2);
        vnl_matrix<double> compute_correspondence_point_tangent_curvature_weights(double T,
                                                                                   std::vector<dbrl_feature_sptr> f1,
                                                                                   std::vector<dbrl_feature_sptr> f2);

        vnl_matrix<double> compute_neighborhood_weights(double T,std::vector<dbrl_feature_sptr> f1,
            std::vector<dbrl_feature_sptr> f2,std::string type);


        vnl_matrix<double> compute_neighborhood_weights(double T,
                                                        std::vector<dbrl_feature_sptr> f1,
                                                        std::vector<dbrl_feature_sptr> f2, 
                                                        vnl_matrix<double> dist);
        double  projected_distance (double x1,double y1,double theta1,double x2,double y2,double theta2);


        
    protected:
        double Tinit_;
        double Tfinal_;
        double anneal_rate_;
        std::vector<dbrl_feature_sptr> f1_;
        std::vector<dbrl_feature_sptr> f2_;
        double distPointLineSegment (double ptx,double pty, 
                             double lstartx,double lstarty, 
                             double lendx,double lendy);

    };
class dbrl_robust_point_matching_params
    {
    public:
        dbrl_robust_point_matching_params();
        dbrl_robust_point_matching_params(double Tstart,double Tend,
                                 double outlierval,double annealrate);
        ~dbrl_robust_point_matching_params(){}

        double initialT(){return Tstart_;}
        double finalT(){return Tend_;}
    double annealrate(){return annealrate_;}
    double outlier(){return outlierval_;}
    protected:
        double Tstart_;
        double Tend_;
        double annealrate_;
        double outlierval_;

    };
#endif
