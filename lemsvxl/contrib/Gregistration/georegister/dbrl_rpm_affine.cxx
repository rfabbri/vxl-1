#include "dbrl_rpm_affine.h"
#include <vul/vul_timer.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_curvature.h>

dbrl_rpm_affine_params::dbrl_rpm_affine_params()
    {
        
    }
dbrl_rpm_affine_params::dbrl_rpm_affine_params(double initlambda,double Mconvg):dbrl_robust_point_matching_params()
    {
        initlambda_=initlambda;
        Mconvg_=Mconvg;
    }
dbrl_rpm_affine_params::dbrl_rpm_affine_params(double initlambda,double Mconvg,double Tstart,
                                 double Tend,double outlierval,double annealrate):dbrl_robust_point_matching_params( Tstart, Tend,
                                 outlierval,annealrate)
    {
        initlambda_=initlambda;
        Mconvg_=Mconvg;
    }


dbrl_rpm_affine::dbrl_rpm_affine(dbrl_rpm_affine_params & params,
                                 vcl_vector<dbrl_feature_sptr>  f1,
                                 vcl_vector<dbrl_feature_sptr>  f2)
    {
    params_=params;
    f1_=f1;
    f2_=f2;

    }
dbrl_match_set_sptr dbrl_rpm_affine::rpm(vcl_string name)
    {

    dbrl_estimator_point_affine * affine_est= new dbrl_estimator_point_affine();
    dbrl_transformation_sptr tform;

    vcl_vector<dbrl_feature_sptr> f1x(f1_);
    vcl_vector<dbrl_feature_sptr> f2x(f2_);
    
    //: initialize annealing parameters
    double T=params_.initialT();
    double lambdainit=params_.initlambda();
    double lambda=lambdainit*T;

    //: intialize correspondence Matrix M
    dbrl_correspondence M(f1_.size(),f2_.size());
    M.setinitialoutlier(params_.outlier());


    //: annealing (outer loop)
    while(T>params_.finalT())
        {
            lambda=lambdainit*T;
            rpm_at(T,M,affine_est,tform,f1x,f2x,lambda,name);
            T*=params_.annealrate();
        }
    affine_est->set_lambda(0.001);
    tform=affine_est->estimate(f1_,f2_,M);

    dbrl_affine_transformation * affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());

    //vcl_cout<<"\n Final xform \n";
    //affinetform->print_summary(vcl_cout);

    vcl_vector<dbrl_feature_sptr> f2xtemp(f2_);
    affinetform->set_from_features(f2xtemp);
    affinetform->transform();
    f2xtemp.clear();
    f2xtemp=affinetform->get_to_features();

    energy_=distance(f1_,f2xtemp,M);

    dbrl_match_set_sptr matchset=new dbrl_match_set(M,tform,affine_est);
    matchset->set_original_features(f1_,f2_);
    matchset->set_mapped_features(f1_,f2xtemp);

    return matchset;
    }

bool dbrl_rpm_affine::rpm_at(double T,dbrl_correspondence & M,
                             dbrl_estimator_point_affine * affine_est,
                             dbrl_transformation_sptr &tform,
                             vcl_vector<dbrl_feature_sptr> &f1x,
                             vcl_vector<dbrl_feature_sptr> &f2x,
                             double l,vcl_string name)
    {

    vul_timer t;
    double errdist=1e6;
    int cnt=0;
    //: loop for M convergence
    while(errdist>params_.Mconvg() & cnt<5)
        {

            if(name=="Euclidean")
                {
                vnl_matrix<double> m_tmp=compute_correspondence_weights(T,f1_,f2x);
                M.updateM(m_tmp);
                }
            else if(name=="Projected")
                {
                vnl_matrix<double> m_tmp=compute_correspondence_point_tangent_weights(T,f1_,f2x);
                M.updateM(m_tmp);
                }
            else if(name=="Circular")
            {
                vnl_matrix<double> m_tmp=compute_correspondence_point_tangent_curvature_weights(T,f1_,f2x);
                M.updateM(m_tmp);
            }
            else if(name=="NeighborEuclidean")
            {
                vnl_matrix<double> m_tmp=compute_correspondence_weights(T,f1_,f2x);
                M.updateM(m_tmp);
                vnl_matrix<double> m_tmp1=compute_neighborhood_weights(T,f1_,f2x,M.M());
                //vcl_cout<<"\n Input Matrix \n"<<m_tmp;
                M.updateM(m_tmp1,params_.annealrate());
            }
            else if(name=="NeighborProjected")
            {
                vnl_matrix<double> m_tmp=compute_correspondence_point_tangent_weights(T,f1_,f2x);
                M.updateM(m_tmp);
                vnl_matrix<double> m_tmp1=compute_neighborhood_weights(T,f1_,f2x,M.M());
                //vcl_cout<<"\n Input Matrix \n"<<m_tmp;
                M.updateM(m_tmp1,params_.annealrate());
            }
            //vcl_cout<<"\n Correspondence Matrix \n";
            //M.print_summary(vcl_cout);
            
            
            //: estimate the affine parameters
            affine_est->set_lambda(l);  
            tform=affine_est->estimate(f1_,f2x,M);
            //vcl_cout<<M.M();
            //vcl_cout<<"\n Affine transform \n";
            //tform->print_transformation(vcl_cout);
            dbrl_affine_transformation *affinetform=dynamic_cast<dbrl_affine_transformation *> (tform.ptr());
            affinetform->set_from_features(f2x);
            affinetform->transform();
            f2x.clear();
            f2x=affinetform->get_to_features();

            errdist=M.errdist();
            cnt++;
        }

    return true;
    }
double dbrl_rpm_affine::distance(vcl_vector<dbrl_feature_sptr> f1,
                vcl_vector<dbrl_feature_sptr> f2, dbrl_correspondence & M)
{
    double dist=0.0;
    for(unsigned i=0;i<f1.size();i++)
    {
        double x1;
        double y1;
        if(dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(f1[i].ptr()))
        {
            x1=pt1->location()[0];
            y1=pt1->location()[1];
        }
        if(dbrl_feature_point_tangent* pt1=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
        {
            x1=pt1->location()[0];
            y1=pt1->location()[1];
        }
                if(dbrl_feature_point_tangent_curvature* pt1=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f1[i].ptr()))
        {
            x1=pt1->location()[0];
            y1=pt1->location()[1];
        }

        for(unsigned j=0;j<f2.size();j++)
        {
            double x2;
            double y2;
            if(dbrl_feature_point* pt2=dynamic_cast<dbrl_feature_point*>(f2[j].ptr()))
            {
                x2=pt2->location()[0];
                y2=pt2->location()[1];
            }
            if(dbrl_feature_point_tangent* pt2=dynamic_cast<dbrl_feature_point_tangent*>(f2[j].ptr()))
            {
                x2=pt2->location()[0];
                y2=pt2->location()[1];
            }
                        if(dbrl_feature_point_tangent_curvature* pt2=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[j].ptr()))
            {
                x2=pt2->location()[0];
                y2=pt2->location()[1];
            }

            dist+=M(i,j)*((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        }
    }
    return dist;
}
