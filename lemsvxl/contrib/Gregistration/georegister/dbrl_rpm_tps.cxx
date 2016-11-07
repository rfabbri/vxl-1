#include "dbrl_rpm_tps.h"
#include "dbrl_feature_point.h"
#include "dbrl_feature_point_tangent.h"
#include <vul/vul_timer.h>
#include <vnl/vnl_transpose.h>

dbrl_rpm_tps_params::dbrl_rpm_tps_params()
    {

    }
dbrl_rpm_tps_params::dbrl_rpm_tps_params(double initlambda1,double initlambda2,double Mconvg):dbrl_robust_point_matching_params()
    {
    initlambda1_=initlambda1;
    initlambda2_=initlambda2;
    Mconvg_=Mconvg;
    }
dbrl_rpm_tps_params::dbrl_rpm_tps_params(double initlambda1,double initlambda2,double Mconvg,double Tstart,
                                         double Tend,double outlierval,double annealrate):dbrl_robust_point_matching_params( Tstart, Tend,
                                         outlierval,annealrate)
    {
    initlambda1_=initlambda1;
    initlambda2_=initlambda2;
    Mconvg_=Mconvg;
    }


dbrl_rpm_tps::dbrl_rpm_tps(dbrl_rpm_tps_params & params,
                           vcl_vector<dbrl_feature_sptr>  f1,
                           vcl_vector<dbrl_feature_sptr>  f2)
    {
    params_=params;
    f1_=f1;
    f2_=f2;

    }

dbrl_match_set_sptr dbrl_rpm_tps::rpm(vcl_string name)
    {

    dbrl_estimator_point_thin_plate_spline * tps_est= new dbrl_estimator_point_thin_plate_spline();
    dbrl_transformation_sptr tform;


    //: initialize M 
    dbrl_correspondence M(f1_.size(),f2_.size());
    M.setinitialoutlier(params_.outlier());

    //: intializing annealing parameters
    double T=params_.initialT();
    double lambda1init=params_.initlambda1();
    double lambda2init=params_.initlambda2();
    lambda1=lambda1init*T;
    lambda2=lambda2init*T;

    vcl_vector<dbrl_feature_sptr> f1x(f1_);
    vcl_vector<dbrl_feature_sptr> f2x(f2_);
    //unused double residual=0.0;
    //: annealing (outer loop)
    while(T>params_.finalT())
        {
        rpm_at(T,M,tps_est,tform,f1x,f2x,lambda1,lambda2);
        //vcl_cout<<"\nTemperature is "<<T;//<<" and residual is "<<residual;
        //: update parameters
        T*=params_.annealrate();
        lambda1=lambda1init*params_.annealrate();
        lambda2=lambda2init*params_.annealrate();
        }

    //: compute actual transformation
    vcl_vector<dbrl_feature_sptr> f1xtemp(f1_);
    vcl_vector<dbrl_feature_sptr> f2xtemp(f2_);
    normalize_point_set(M.M(),f1xtemp);
    tps_est->set_lambda1(10.0);
    tps_est->set_lambda2(0.01);
    tform=tps_est->estimate(f1xtemp,f2xtemp,M);
    dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());

    tpstform->print_transformation(vcl_cout);

    ////tpstform->print_transformation(vcl_cout);
    tpstform->set_from_features(f2xtemp);
    tpstform->transform();
    f2xtemp.clear();
    f2xtemp=tpstform->get_to_features();

    //: compute distance between f1xtemp and f2xtemp for energy 
    


    energy_=tpstform->compute_energy();
    energy_+=distance(f1xtemp,f2xtemp);
    dbrl_match_set_sptr matchset=new dbrl_match_set(M,tform,tps_est);
    matchset->set_original_features(f1_,f2_);
    matchset->set_mapped_features(f1_,f2xtemp);

    return matchset;
    }

bool dbrl_rpm_tps::rpm_at(double T,dbrl_correspondence & M,
                          dbrl_estimator_point_thin_plate_spline * tps_est,
                          dbrl_transformation_sptr &tform,
                          vcl_vector<dbrl_feature_sptr> &f1x,
                          vcl_vector<dbrl_feature_sptr> & f2x,
                          double l1,double l2,vcl_string name)
    {

    vul_timer t;
    double errdist=1e6;
    int cnt=0;
    //: loop for M convergence
    while(errdist>params_.Mconvg() & cnt<3)
        {
        tps_est->set_lambda1(l1); 
        tps_est->set_lambda2(l2); 
        vnl_matrix<double> m_tmp=compute_correspondence_weights(T,f1x,f2x);
        M.updateM(m_tmp);
        errdist=M.errdist();

        vcl_vector<dbrl_feature_sptr> f1xform(f1x);
        normalize_point_set(M.M(),f1xform);
        tform=tps_est->estimate(f1xform,f2x,M);
        dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
        //tpstform->print_transformation(vcl_cout);
        tpstform->set_from_features(f2x);
        tpstform->transform();

        f2x.clear();
        f2x=tpstform->get_to_features();
        cnt++;
        }

    return true;
    }

 void dbrl_rpm_tps::normalize_point_set(vnl_matrix<double> & M, vcl_vector<dbrl_feature_sptr> & f2)
    {

    vnl_matrix<double> pts(f2.size(),2,0.0);
    for(unsigned i=0;i<f2.size();i++)
        {
            if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
            {
                pts(i,0)=pt->location()[0];
                pts(i,1)=pt->location()[1];
            }
            else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
            {
                pts(i,0)=pt->location()[0];
                pts(i,1)=pt->location()[1];
            }
        }
    vnl_transpose Mtrans(M);
    vnl_matrix<double> Mt=Mtrans.asMatrix();
    vnl_matrix<double> vpts=Mtrans*pts;
    f2.clear();
    for(int i=0;i<static_cast<int>(Mt.rows());i++)
        {
        if(Mt.get_row(i).sum()==0.0)
            vpts.set_row(i,-100);
        else
            {
            vpts.scale_row(i,1/Mt.get_row(i).sum());
            }
        f2.push_back(new dbrl_feature_point(vpts.get_row(i)));
        }
    }
    
 double dbrl_rpm_tps::distance(vcl_vector<dbrl_feature_sptr> f1,
                vcl_vector<dbrl_feature_sptr> f2)
{
    double dist=0.0;
    for(unsigned i=0;i<f1.size();i++)
    {
       if(dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(f1[i].ptr()))
           if(dbrl_feature_point* pt2=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
            {
                if(pt1->location()[0]!=-100 )
                    dist+=(pt1->location()[0]-pt2->location()[0])*(pt1->location()[0]-pt2->location()[0])+
                    (pt1->location()[1]-pt2->location()[1])*(pt1->location()[1]-pt2->location()[1]);
            }
        else if(dbrl_feature_point_tangent* pt1=dynamic_cast<dbrl_feature_point_tangent*>(f1[i].ptr()))
            if(dbrl_feature_point_tangent* pt2=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
            {
            if(pt1->location()[0]!=-100 )
                {
                dist+=(pt1->location()[0]-pt2->location()[0])*(pt1->location()[0]-pt2->location()[0])+
                        (pt1->location()[1]-pt2->location()[1])*(pt1->location()[1]-pt2->location()[1]);
                }
            }
     }
    return dist;
}
