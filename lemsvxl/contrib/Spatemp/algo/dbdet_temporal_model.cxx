#include <Spatemp/algo/dbdet_temporal_model.h>
#include <Spatemp/algo/dbdet_temporal_utils.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <dbnl/algo/dbnl_eno.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_transpose.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#define anglediff 0.34
#define smax 2
#define kdiffmax 0.15
dbdet_temporal_curvature_velocity_model::dbdet_temporal_curvature_velocity_model(vnl_vector_fixed<double,2> m, vnl_matrix_fixed<double,2,2> c)
{
    mean_=m;
    covar_=c;
}


dbdet_temporal_curvature_velocity_model::dbdet_temporal_curvature_velocity_model(dbdet_curvelet * c1, dbdet_curvelet *c2,int t):covar_(1)
{
  
    covar_(0,0)=0.6;    covar_(0,1)=0.0;
    covar_(1,0)=0.0;    covar_(1,1)=0.2;

    double theta1=c1->ref_edgel->tangent;
    double x1=c1->ref_edgel->pt.x();
    double y1=c1->ref_edgel->pt.y();

    double theta2=c2->ref_edgel->tangent;
    double x2=c2->ref_edgel->pt.x();
    double y2=c2->ref_edgel->pt.y();
    if(vcl_fabs(theta2-theta1)<anglediff)
    {
        dbdet_curve_model * m=c1->curve_model->intersect(c2->curve_model);
        if(m->bundle_is_valid())
        {
            //: computing the mean velcoity
            mean_[0]=(x2-x1)/t;
            mean_[1]=(y2-y1)/t;

            //: adding a covariance of 0.6 along the edge and 0.3 across the edge
            vnl_matrix_fixed<double,2,2> rmat(1);
            rmat(0,0)=vcl_cos(theta2);
            rmat(0,1)=-vcl_sin(theta2);
            rmat(1,0)=vcl_sin(theta2);
            rmat(1,1)=vcl_cos(theta2);

            vnl_matrix_fixed<double,2,2> rmat_tpose(1);
            rmat_tpose(0,0)=vcl_cos(theta2);
            rmat_tpose(0,1)=vcl_sin(theta2);
            rmat_tpose(1,0)=-vcl_sin(theta2);
            rmat_tpose(1,1)=vcl_cos(theta2);

            covar_=rmat_tpose*covar_*rmat;
            isvalid_=true;
            num_obs_=1;
        }
        else 
            isvalid_=false;
    }
    else
        isvalid_=false;
}



bool 
dbdet_temporal_curvature_velocity_model::is_model_intersect(dbdet_temporal_model * m, double dist_epsilon_square)
{
    if(dbdet_temporal_curvature_velocity_model *cvmodel=dynamic_cast<dbdet_temporal_curvature_velocity_model*>(m))
    {
        vnl_vector_fixed<double,2> V0m=cvmodel->mean_;
        vnl_matrix_fixed<double,2,2> c0m=cvmodel->covar_;

        double d1=sqr_mahalanobis_dist(V0m);
        
        if(d1>dist_epsilon_square)
           return false;

    }
    return true;
}


double 
dbdet_temporal_curvature_velocity_model::sqr_mahalanobis_dist(vnl_vector_fixed<double,2> V0)
{
        vnl_matrix_fixed<double,2,2> inv_covar_=vnl_inverse(covar_);
        vnl_vector_fixed<double,2> b=V0-mean_;
        double d=b[0]*b[0]*inv_covar_(0,0)+b[0]*b[1]*inv_covar_(0,1)+b[1]*b[0]*inv_covar_(1,0)+b[1]*b[1]*inv_covar_(1,1);

        return d;
}

void
dbdet_temporal_curvature_velocity_model::print_model()
{
vcl_cout<<" V0x= "<<mean_[0]<<" V0y= "<<mean_[1];//<<" k = "<<kmean <<" ksig= "<<ksig<<"";
}

dbdet_temporal_curvature_velocity_model * intersect(dbdet_temporal_curvature_velocity_model & m1,dbdet_temporal_curvature_velocity_model & m2)
{
    unsigned nobs1=m1.num_obs();
    vnl_vector_fixed<double,2> V1=m1.meanV();
    vnl_matrix_fixed<double,2,2> C1=m1.covarV();
    
    unsigned nobs2=m2.num_obs();
    vnl_vector_fixed<double,2> V2=m2.meanV();
    vnl_matrix_fixed<double,2,2> C2=m2.covarV();

    double alpha=double(nobs1)/double(nobs1+nobs2);
    //: to be revised TODO
    vnl_vector_fixed<double,2> mean_new=(alpha*V1+(1-alpha)*V2);
    vnl_matrix_fixed<double,2,2> C_new=(C1*alpha+C2*(1-alpha));

    dbdet_temporal_curvature_velocity_model * m = new dbdet_temporal_curvature_velocity_model(mean_new,C_new);

    return m;

}
 
dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(dbdet_curvelet * c0, dbdet_curvelet *ct, int t)
{

    double beta=0;
    if(compute_intersection(c0,ct,beta))
        isvalid_=true;
    else
        isvalid_=false;

    ref_curvelet=c0;
    //: storing the normal displacemeent . should be didived by t  to get velcoity.
    betas[t]=beta;///t;
    betas[0]=0;///t;

    b0=beta;
    
    if(t>0) 
    {
        t_pos_infinity=beta/vcl_fabs(beta)*100000;
        t_neg_infinity=-t_pos_infinity;
    }
    else
    {
        t_neg_infinity=beta/vcl_fabs(beta)*100000;
        t_pos_infinity=-t_neg_infinity;
    }
   
}
dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(double beta,double beta_t,dbdet_curvelet * refc)
{
    float vmax=3;


    float dbt=0.1;
    b0=beta;
    bt=beta_t;
    ref_curvelet=refc;
    isvalid_=false;
    iscomputed_=false;
    if(vcl_fabs(b0)<vmax)
    {
    float v_wmax=vcl_sqrt(vmax*vmax-b0*b0);
    if(dbdet_CC_curve_model_3d * ccmodel=dynamic_cast<dbdet_CC_curve_model_3d *> (ref_curvelet->curve_model))
    {    
        k=ccmodel->k;
        dk=ccmodel->dk;
    }
    tangent=ref_curvelet->ref_edgel->tangent;

    //: case when k nears zero
    if(vcl_fabs(k)<0.05)
    {
        w1=w2=0;
        dw=v_wmax;
        isvalid_=true;
        compute_one_paramter_V();
    }
    else
    {
    float btmax=0;
    float btmin=0;
    float wmax=0;
    float wmin=0;
    if(bt<0 && k<0)
    {
        btmax=vcl_min<float>(0,bt+dbt);
        btmin=bt-dbt;
        isvalid_=true;
    }
    else if (bt>0 && k>0)
    {
        btmin=vcl_max<float>(0,bt-dbt);
        btmax=bt+dbt;
        isvalid_=true;
    }
    else if (bt>0 && k<0)
    {
        btmax=0;
        btmin=vcl_min<float>(0,bt-dbt);
        if(btmin<0)
            isvalid_=true;
    }   
    else if (bt<0 && k>0)
    {
        btmin=0;
        btmax=vcl_max<float>(0,bt+dbt);
        if(btmax>0)
            isvalid_=true;
    }
    
    if(isvalid_)
    {
        wmax=vcl_max(vcl_min<float>(vcl_sqrt(btmax/k),v_wmax),vcl_max<float>(vcl_sqrt(btmin/k),-v_wmax));
        wmin=vcl_min(vcl_min<float>(vcl_sqrt(btmax/k),v_wmax),vcl_max<float>(vcl_sqrt(btmin/k),-v_wmax));

        w1=(wmax+wmin)/2;
        w2=-w1;
        dw=(wmax-wmin)/2;

        compute_one_paramter_V();
    }
    }
    }
}

dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(float beta, int t)
{
b0=beta;
betas[t]=beta;///t;
    betas[0]=0;///t;

    
    if(t>0) 
    {
        t_pos_infinity=beta/vcl_fabs(beta)*100000;
        t_neg_infinity=-t_pos_infinity;
    }
    else
    {
        t_neg_infinity=beta/vcl_fabs(beta)*100000;
        t_pos_infinity=-t_neg_infinity;
    }
}
dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(dbdet_curvelet * refc, vcl_map<int,double> betamap,vcl_map<int,dbdet_curvelet*> cmap)
{
    ref_curvelet=refc;
    betas=betamap;
    //cs=cmap;
    isvalid_=true;
    //if( compute_betas() && compute_bs())
    //    compute_w();

}
dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model( vcl_map<int,double> betamap)
{
    betas=betamap;
    compute_betas();
    isvalid_=true;

}

dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(const dbdet_temporal_normal_velocity_model & m, dbdet_temporal_normal_velocity_model & nbr):bt(m.bt),b0(m.b0),k(m.k),bs(0.0),t_pos_infinity(m.t_pos_infinity),t_neg_infinity(m.t_neg_infinity)
{
    
    ref_curvelet=m.ref_curvelet;
    compute_beta_s(nbr);
}

dbdet_temporal_normal_velocity_model::dbdet_temporal_normal_velocity_model(const dbdet_temporal_normal_velocity_model & m):bt(m.bt),b0(m.b0),k(m.k),bs(0.0),t_pos_infinity(m.t_pos_infinity),t_neg_infinity(m.t_neg_infinity)
{
    }
bool 
dbdet_temporal_normal_velocity_model::is_model_intersect(dbdet_temporal_model * m, double dist_epsilon_square)
{
    if(dbdet_temporal_normal_velocity_model *cvmodel=dynamic_cast<dbdet_temporal_normal_velocity_model*>(m))
    {
        if(cvmodel->ref_curvelet==ref_curvelet)
        {
            vcl_map<int,double>::iterator b_ref_iter;
            vcl_map<int,double>::iterator b_iter;
            bool flag=true;
            for(b_iter=cvmodel->betas.begin();b_iter!=cvmodel->betas.end();b_iter++)
            {
                if(b_iter->first!=0)
                {
                    double min_last=t_neg_infinity;
                    double max_last=t_pos_infinity;
                    for(b_ref_iter=betas.begin();b_ref_iter!=betas.end();b_ref_iter++)
                    {
                        if(b_ref_iter->first>b_iter->first)
                            max_last=b_ref_iter->second;
                        else
                            min_last=b_ref_iter->second;
                    }
                    if(min_last>max_last)
                    {
                        if(!(b_iter->second<min_last && b_iter->second>max_last))
                        {
                            flag=false;
                        }

                    }
                    else
                    {
                        if(!(b_iter->second>min_last && b_iter->second<max_last))
                        {
                            flag=false;
                        }
                    }
                }

            }


            return flag;
        }
        return false;
    }
    return false;
}

double
dbdet_temporal_normal_velocity_model::avg_beta()
{
    vcl_map<int,double>::iterator iter=betas.begin();
    double avg=0.0;
    for(;iter!=betas.end();iter++)
    {
        avg+=iter->second/iter->first;
    }
    return avg/betas.size();
}
bool
dbdet_temporal_normal_velocity_model::compute_betas()
{
    if(betas.size()<=2)
        return false;
    //betas[0]=0;
    vnl_vector<double> be(betas.size());

    vnl_matrix<double> A(betas.size(),3);
        vcl_map<int,double>::iterator  iter;
    int cnt;
    for(iter=betas.begin(), cnt=0;iter!=betas.end();iter++,cnt++)
    {
        A(cnt,0)=(double)iter->first;
        
        A(cnt,1)=(double)iter->first*(double)iter->first/2.0;
        A(cnt,2)=-iter->second;
    }

    

    vnl_svd<double> M(A);
    vnl_vector<double> solution=M.nullvector();

    if(solution[2]!=0)
    {

        b0=solution[0]/solution[2];
        bt=solution[1]/solution[2];
        return true;
    }

    
    //vcl_cout<<"solution of "<<A << " is " << M.nullvector();
    //vnl_vector<double> x(betas.size());
    //int cnt=0;
    //unsigned pivot=1000;
    //for(iter=betas.begin();iter!=betas.end();iter++)
    //{
    //    be.put(cnt,iter->second);
    //    x.put(cnt,iter->first);
    //    cnt++;
    //}
    //dbnl_eno_1d  eno_curve;
    //if(eno_curve.interpolate(&be,&x))
    //{
    //    pivot=eno_curve.interval_index(0);

    //    dbnl_eno_interp c=eno_curve[pivot];
    //    b=c.derivative(0);
    //    bt=c.second_derivative(0);
    //    return true;
    //}
    return false;
}
bool
dbdet_temporal_normal_velocity_model::compute_beta_s(dbdet_temporal_normal_velocity_model & nbr)
{
    double bn=nbr.b0;
    double xn=nbr.ref_curvelet->ref_edgel->pt.x();
    double yn=nbr.ref_curvelet->ref_edgel->pt.y();

    double x0=ref_curvelet->ref_edgel->pt.x();
    double y0=ref_curvelet->ref_edgel->pt.y();

    double s =vcl_sqrt((xn-x0)*(xn-x0)+(yn-y0)*(yn-y0));
    if(s>=0.3){
        if((xn-x0)*vcl_cos(ref_curvelet->ref_edgel->tangent)+(yn-y0)*vcl_sin(ref_curvelet->ref_edgel->tangent)>0)
            bs=(bn-b0)/s;
        else
            bs=(b0-bn)/s;
        return true;
    }
    else
        return false;

}



bool dbdet_temporal_normal_velocity_model::compute_one_paramter_V()
{  
    
    //if(vcl_fabs(k)<0.005 || bt/k<0)
    //{
    //    isvalid_=false;
    //    return false;
    //}

    //w1=vcl_sqrt(bt/k);
    //w2=-vcl_sqrt(bt/k);

    double uc=160;
    double vc=120;

    double s=320;

    double gx=(ref_curvelet->ref_edgel->pt.x()-uc)/s;
    double gy=(ref_curvelet->ref_edgel->pt.y()-vc)/s;
    
    double r=2*vcl_sqrt(gx*gx+gy*gy);
    double theta=ref_curvelet->ref_edgel->tangent;

    vnl_vector<float> g(3);
    g[0]=gx;g[1]=gy;g[2]=1;

    vnl_vector<float>  t(3);
    t[0]=vcl_cos(tangent);t[1]=vcl_sin(tangent);t[2]=0;

    vnl_vector<float>  n(3);
    n[0]=-vcl_sin(tangent);n[1]= vcl_cos(tangent);n[2]=0;

    vnl_vector<float> gt1=w1*t+b0*n;
    vnl_vector<float> gt2=w2*t+b0*n;
    
    tau1=vnl_cross_3d<float>(gt1,g);
    if(r<0.01)
        r=0.01;

    tau1(2)=tau1(2)/r;
    tau2=vnl_cross_3d<float>(gt2,g);
    tau2(2)=tau2(2)/r;
    dtau=vnl_cross_3d<float>(t,g);
    dtau(2)=dtau(2)/r;

    vnl_matrix<float> D1=outer_product(tau1,tau1);
    vnl_matrix<float> D2=outer_product(tau2,tau2);


    vnl_vector<float> lambdas(3);
    vnl_matrix<float> Vs(3,3);

    if(!vnl_symmetric_eigensystem_compute(D1,Vs,lambdas))
        return false;


    int tableofcoeffs[9][2]={{0,0},{0,1},{0,2},{1,0},{1,1},{1,2},{2,0},{2,1},{2,2}};


    vnl_matrix<float> dd(9,9);
    //dw=0.2;
    for(unsigned i=0;i<9;i++)
        for(unsigned j=0;j<9;j++)
        {
            int d11=tableofcoeffs[i][0];
            int d12=tableofcoeffs[i][1];
            int d21=tableofcoeffs[j][0];
            int d22=tableofcoeffs[j][1];

            dd(i,j)= tau1(d11)*tau1(d21)*dw*dtau(d12)*dtau(d22)+
                     tau1(d11)*tau1(d22)*dw*dtau(d12)*dtau(d21)+
                     tau1(d12)*tau1(d22)*dw*dtau(d11)*dtau(d21)+
                     tau1(d12)*tau1(d21)*dw*dtau(d11)*dtau(d22)+
                     3*dw*dw*dtau(d11)*dtau(d12)*dtau(d21)*dtau(d22);
        }
        
    vnl_matrix<float> Delta1(3,3,0);
    Delta1(2,2)=-1/lambdas(2);

    vnl_matrix<float> Delta2(3,3,0);
    Delta2(2,2)=-1/lambdas(2);
    
    vnl_matrix<float> U1(3,9,0.0);
    U1(0,0)=Vs(0,0);    U1(0,3)=Vs(1,0);     U1(0,6)=Vs(2,0);   
    U1(1,1)=Vs(0,0);    U1(1,4)=Vs(1,0);     U1(1,7)=Vs(2,0);
    U1(2,2)=Vs(0,0);    U1(2,5)=Vs(1,0);     U1(2,8)=Vs(2,0);

    vnl_matrix<float> U2(3,9,0.0);
    U2(0,0)=Vs(0,1);    U2(0,3)=Vs(1,1);     U2(0,6)=Vs(2,1);   
    U2(1,1)=Vs(0,1);    U2(1,4)=Vs(1,1);     U2(1,7)=Vs(2,1);
    U2(2,2)=Vs(0,1);    U2(2,5)=Vs(1,1);     U2(2,8)=Vs(2,1);

    v1_1v1_1=Vs*Delta1*Vs.transpose()*U1*dd*U1.transpose()*Vs*Delta1*Vs.transpose();
    v1_1v1_2=Vs*Delta1*Vs.transpose()*U1*dd*U2.transpose()*Vs*Delta2*Vs.transpose();
    v1_2v1_2=Vs*Delta2*Vs.transpose()*U2*dd*U2.transpose()*Vs*Delta2*Vs.transpose();

    v1_1=Vs.get_column(0);
    v1_2=Vs.get_column(1);

    //: getting the range on free paramter 
    double psi=vcl_atan2(dot_product<float>(dtau,v1_1),dot_product<float>(dtau,v1_2));

    if(b0>=0.2)
    {
        phi1_min=-vnl_math::pi-psi;
        phi1_max=0-psi;
    }
    else if(b0<-0.2)
    {
        phi1_min=0-psi;
        phi1_max=vnl_math::pi-psi;
    }
    else
    {
        phi1_min=0;
        phi1_max=2*vnl_math::pi;
    }
    //: for w2
    if(!vnl_symmetric_eigensystem_compute(D2,Vs,lambdas))
        return false;



    dd.fill(0);
    //vnl_matrix<float> dd(9,9);
    //dw=0.2;
    for(unsigned i=0;i<9;i++)
        for(unsigned j=0;j<9;j++)
        {
            int d11=tableofcoeffs[i][0];
            int d12=tableofcoeffs[i][1];
            int d21=tableofcoeffs[j][0];
            int d22=tableofcoeffs[j][1];

            dd(i,j)= tau2(d11)*tau2(d21)*dw*dtau(d12)*dtau(d22)+
                     tau2(d11)*tau2(d22)*dw*dtau(d12)*dtau(d21)+
                     tau2(d12)*tau2(d22)*dw*dtau(d11)*dtau(d21)+
                     tau2(d12)*tau2(d21)*dw*dtau(d11)*dtau(d22)+
                     3*dw*dw*dtau(d11)*dtau(d12)*dtau(d21)*dtau(d22);
        }
    Delta1.fill(0.0); 
    Delta1(2,2)=-1/lambdas(2);

    Delta2.fill(0.0); 
    Delta2(2,2)=-1/lambdas(2);
    
    U1.fill(0.0);
    U1(0,0)=Vs(0,0);    U1(0,3)=Vs(1,0);     U1(0,6)=Vs(2,0);   
    U1(1,1)=Vs(0,0);    U1(1,4)=Vs(1,0);     U1(1,7)=Vs(2,0);
    U1(2,2)=Vs(0,0);    U1(2,5)=Vs(1,0);     U1(2,8)=Vs(2,0);

    U2.fill(0.0);
    U2(0,0)=Vs(0,1);    U2(0,3)=Vs(1,1);     U2(0,6)=Vs(2,1);   
    U2(1,1)=Vs(0,1);    U2(1,4)=Vs(1,1);     U2(1,7)=Vs(2,1);
    U2(2,2)=Vs(0,1);    U2(2,5)=Vs(1,1);     U2(2,8)=Vs(2,1);

    v2_1v2_1=Vs*Delta1*Vs.transpose()*U1*dd*U1.transpose()*Vs*Delta1*Vs.transpose();
    v2_1v2_2=Vs*Delta1*Vs.transpose()*U1*dd*U2.transpose()*Vs*Delta2*Vs.transpose();
    v2_2v2_2=Vs*Delta2*Vs.transpose()*U2*dd*U2.transpose()*Vs*Delta2*Vs.transpose();

    v2_1=Vs.get_column(0);
    v2_2=Vs.get_column(1);
    psi=vcl_atan2(dot_product<float>(dtau,v2_1),dot_product<float>(dtau,v2_2));

    if(b0>=0.2)
    {
        phi2_min=-vnl_math::pi-psi;
        phi2_max=0-psi;
    }
    else if(b0<-0.2)
    {
        phi2_min=0-psi;
        phi2_max=vnl_math::pi-psi;
    }
    else
    {
        phi2_min=0;
        phi2_max=2*vnl_math::pi;
    }
    iscomputed_=true;
    return true;
    
}
void dbdet_temporal_normal_velocity_model::compute_2d_map()
{

    for(float phi1=phi1_min;phi1<=phi1_max;)
    {
        vnl_vector<float> Vmean=v1_1*vcl_cos(phi1)+v1_2*vcl_sin(phi1);


        double theta_mean, phi_mean,r ;
        dbdet_temporal_utils::cartesian2spherical(Vmean(0),Vmean(1),Vmean(2),theta_mean, phi_mean,r);
 
        //: converting to -1 to 1 range
        f1t.push_back(theta_mean/vnl_math::pi_over_2);
        f1p.push_back(phi_mean/vnl_math::pi);

        vnl_matrix<float> Vcov=vcl_cos(phi1)*vcl_cos(phi1)*v1_1v1_1
                                +vcl_sin(phi1)*vcl_sin(phi1)*v1_2v1_2
                                +2*vcl_cos(phi1)*vcl_sin(phi1)*v1_1v1_2;
        vnl_vector<float> lambdas(3);
        vnl_matrix<float> Vs(3,3);

        if(!vnl_symmetric_eigensystem_compute(Vcov,Vs,lambdas))
            return ;


        phi1=phi1+0.2;
    }
    for(float phi1=phi2_min;phi1<=phi2_max;)
    {
        vnl_vector<float> Vmean=v2_1*vcl_cos(phi1)+v2_2*vcl_sin(phi1);


        double theta_mean, phi_mean,r ;
        dbdet_temporal_utils::cartesian2spherical(Vmean(0),Vmean(1),Vmean(2),theta_mean, phi_mean,r);
 
        //: converting to -1 to 1 range
        f2t.push_back(theta_mean/vnl_math::pi_over_2);
        f2p.push_back(phi_mean/vnl_math::pi);

        vnl_matrix<float> Vcov=vcl_cos(phi1)*vcl_cos(phi1)*v2_1v2_1
                                +vcl_sin(phi1)*vcl_sin(phi1)*v2_2v2_2
                                +2*vcl_cos(phi1)*vcl_sin(phi1)*v2_1v2_2;
        vnl_vector<float> lambdas(3);
        vnl_matrix<float> Vs(3,3);

        if(!vnl_symmetric_eigensystem_compute(Vcov,Vs,lambdas))
            return ;


        phi1=phi1+0.2;
    }
    //    for(float phi1=0;phi1<=2*vnl_math::pi;)
    //{
    //    vnl_vector<float> Vmean=v2_1*vcl_cos(phi1)+v2_2*vcl_sin(phi1);

    //    double theta_mean=asin(vcl_fabs(Vmean(2)));
    //    double phi_mean=atan2(Vmean(1),Vmean(0));
    //    if(phi_mean<0)
    //        phi_mean+=vnl_math::pi;

    //    f2t.push_back(theta_mean/vnl_math::pi_over_2);
    //    f2p.push_back(phi_mean/vnl_math::pi);

    //    vnl_matrix<float> Vcov=vcl_cos(phi1)*vcl_cos(phi1)*v2_1v2_1
    //                            +vcl_sin(phi1)*vcl_sin(phi1)*v2_2v2_2
    //                            +2*vcl_cos(phi1)*vcl_sin(phi1)*v2_1v2_2;
    //    vnl_vector<float> lambdas(3);
    //    vnl_matrix<float> Vs(3,3);

    //    if(!vnl_symmetric_eigensystem_compute(Vcov,Vs,lambdas))
    //        return ;

    //    for(float theta=0;theta<=vnl_math::pi_over_2;)
    //    {
    //        for(float phi=0;phi<=vnl_math::pi;)
    //        {
    //            vnl_vector<float> Vtest(3);
    //            Vtest[0]=vcl_cos(theta)*vcl_cos(phi);
    //            Vtest[1]=vcl_cos(theta)*vcl_sin(phi);
    //            Vtest[2]=vcl_sin(theta);

    //            if(dot_product(Vtest-Vmean,Vs.get_column(2))*dot_product(Vtest-Vmean,Vs.get_column(2))/(2*lambdas(2)*lambdas(2))<2)
    //            {
    //                f2t.push_back(theta/vnl_math::pi_over_2);
    //                f2p.push_back(phi/vnl_math::pi);
    //            }
    //            phi=phi+0.1;        
    //        }
    //        theta=theta+0.1;
    //    }
    //    phi1=phi1+0.2;
    //}
}
dbdet_temporal_normal_velocity_model *
intersect(dbdet_temporal_normal_velocity_model * m1,dbdet_temporal_normal_velocity_model * m2)
{

    if(m1->is_model_intersect(m2,0.5))
    {

        vcl_map<int,double> bmap=m1->betas;
        vcl_map<int,double>::iterator iter=m2->betas.begin();
        for(;iter!=m2->betas.end();iter++)
            bmap[iter->first]=iter->second;


        dbdet_temporal_normal_velocity_model * m=new dbdet_temporal_normal_velocity_model(bmap);
        return m;
        
    }
    else
    {
        dbdet_temporal_normal_velocity_model * m=NULL;
        return m;
    }
}
void
dbdet_temporal_normal_velocity_model::print_model()
{
    vcl_map<int,double>::iterator iter=betas.begin();
    for(;iter!=betas.end();iter++)
        vcl_cout<<iter->first<<" "<<iter->second<<"\n";

    double tx=vcl_cos(ref_curvelet->ref_edgel->tangent);
    double ty=vcl_sin(ref_curvelet->ref_edgel->tangent);

    double nx=-ty;
    double ny=tx;

    double ytx1=w1*tx+b0*nx;
    double ytx2=w2*tx+b0*nx;

    double yty1=w1*ty+b0*ny;
    double yty2=w2*ty+b0*ny;

    vcl_cout<<ref_curvelet->ref_edgel->pt<<" "<<"["<<tx<<","<<ty<<"]"<<"["<<"["<<ytx1<<","<<yty1<<"]"<<"["<<ytx2<<","<<yty2<<"]";
    vcl_cout<<"beta ="<<b0<<" bs="<<bs<<" beta_t ="<<bt<<" K= "<<k<<" dk="<<dk<<" w1=" <<w1<<" w2 =" <<w2<<" dw= "<<dw<<"\n";
    
    //for(unsigned i=0;i<2;i++)
    //    vcl_cout<<"( "<<a[i]<<","<<b[i]<< ","<<c[i]<<") ";
    vcl_cout<<")\n";



}

bool
dbdet_temporal_normal_velocity_model::compute_intersection(dbdet_curvelet * refc, dbdet_curvelet * c, double & beta)
{


        double x1=refc->ref_edgel->pt.x();
        double y1=refc->ref_edgel->pt.y();
        double k1;
        double theta1;
        if(dbdet_CC_curve_model * ccmodel1=dynamic_cast<dbdet_CC_curve_model *> (refc->curve_model))
        {
            k1=ccmodel1->k;
            theta1=ccmodel1->tangent;
        }
        else if(dbdet_CC_curve_model_3d * ccmodel1=dynamic_cast<dbdet_CC_curve_model_3d *> (refc->curve_model))
        {
            k1=ccmodel1->k;
            theta1=ccmodel1->theta;
        }
        else
            return false;

        double n1x=-vcl_sin(theta1);
        double n1y=vcl_cos(theta1);

        double x2=c->ref_edgel->pt.x();
        double y2=c->ref_edgel->pt.y();
        double k2;
        double theta2;

        double s2max=c->spos;
        double s2min=c->sneg;
        if(dbdet_CC_curve_model * ccmodel2=dynamic_cast<dbdet_CC_curve_model *> (c->curve_model))
        {
            k2=ccmodel2->k;
            theta2=ccmodel2->theta;
        }
        else if(dbdet_CC_curve_model_3d * ccmodel2=dynamic_cast<dbdet_CC_curve_model_3d *> (c->curve_model))
        {
            k2=ccmodel2->k;
            theta2=ccmodel2->theta;
        }
        else
            return false;

        //: TODO get a reasonable value.
        if(vcl_fabs(k1-k2)>vcl_fabs(k1))
            return false;

        double theta_t=vcl_fmod(vcl_fabs(theta1-theta2),(2*vnl_math::pi));
        double lb=1;
        if(theta_t>lb)
            return false;

        double n2x=-vcl_sin(theta2);
        double n2y=vcl_cos(theta2);

        double t2x=vcl_cos(theta2);
        double t2y=vcl_sin(theta2);

        if(vcl_fabs(k2)>0.001 )
        {
            double s1= (-(n1y*t2x-n1x*t2y)+vcl_sqrt((n1y*t2x-n1x*t2y)*(n1y*t2x-n1x*t2y)+2*k2*(n1y*n2x-n1x*n2y)*(n1y*(x1-x2)-n1x*(y1-y2))))/(k2*(n1y*n2x-n1x*n2y));
            double s2= (-(n1y*t2x-n1x*t2y)-vcl_sqrt((n1y*t2x-n1x*t2y)*(n1y*t2x-n1x*t2y)+2*k2*(n1y*n2x-n1x*n2y)*(n1y*(x1-x2)-n1x*(y1-y2))))/(k2*(n1y*n2x-n1x*n2y));

            if(s1>-s2min && s1<s2max)
            {
                if(vcl_fabs(n1x)>0.5)
                    beta=(x2-x1 +t2x*s1+0.5*k2*n2x*s1*s1)/n1x;
                else    
                    beta=(y2-y1 +t2y*s1+0.5*k2*n2y*s1*s1)/n1y;
                return true;
            }
            else if(s2>-s2min && s2<s2max)
            {
                if(vcl_fabs(n1x)>0.5)
                    beta=(x2-x1 +t2x*s2+0.5*k2*n2x*s2*s2)/n1x;
                else    
                    beta=(y2-y1 +t2y*s2+0.5*k2*n2y*s2*s2)/n1y;
                return true;
            }
            else
                 return false;
        }
        else
        {
            double s=(n1y*(x1-x2)-n1x*(y1-y2))/(n1y*t2x-n1x*t2y);
            if(s>-s2min && 2<s2max)
            {
            if(vcl_fabs(n1x)>0.5)
                beta=(x2-x1+t2x*s)/n1x;
            else    
                beta=(y2-y1+t2y*s)/n1y;
            return true;
            }
            else
                return false;
        }



}


dbdet_3D_velocity_model::dbdet_3D_velocity_model():theta_phi_hist(10,20)
{
theta_phi_hist.fill(0);

}
dbdet_3D_velocity_model::dbdet_3D_velocity_model(int tbins, int pbins, float theta_min, float theta_max, float phi_min, float phi_max):theta_phi_hist(tbins, pbins)
{
    theta_phi_hist.fill(0);
    theta_min_=theta_min;
    theta_max_=theta_max;
    phi_min_=phi_min;
    phi_max_=phi_max;
    tbins_=tbins;
    pbins_=pbins;
}

bool dbdet_3D_velocity_model::increment(float theta, float phi)
{
    if(theta<=theta_max_ && theta>=theta_min_)
    {
        if(phi<=phi_max_ && phi>=phi_min_)
        {
            int indexi=(theta-theta_min_)/(theta_max_-theta_min_) *tbins_;
            int indexj=(phi-phi_min_)/(phi_max_-phi_min_)*pbins_;

            theta_phi_hist(indexi, indexj)++;
            return true;
        }
    }
    return false;
}

void dbdet_3D_velocity_model::print()
{
    vcl_cout<<"\n"<<theta_phi_hist<<"\n";
}


dbdet_second_order_velocity_model::dbdet_second_order_velocity_model(float b,float k, float bt, float bs)
{
    b_=b;
    k_=k;
    bt_=bt;
    bs_=bs;
}
vnl_vector<float> 
dbdet_second_order_velocity_model::compute_V_by_rho(dbdet_edgel* e,float s,double alpha)
{
    float vmax=4.00;
    float tx=vcl_cos(e->tangent);
    float ty=vcl_sin(e->tangent);
    float x=(e->pt.x()-160)/s;
    float y=(e->pt.y()-120)/s;

    double w=-b_/vcl_tan(e->tangent);

    if(vcl_fabs(w)>vmax)
        w=vmax;
    float r=(2*w*bs_+bt_+w*w*k_);

    vnl_vector<float> Vbyrho(3);
    Vbyrho(0)=-r/(2*b_)*x+w/s*tx-b_/s*ty;
    Vbyrho(1)=-r/(2*b_)*y+w/s*ty+b_/s*tx;
    Vbyrho(2)=-r/(2*b_)*1+0;

    return Vbyrho;
}

double dbdet_second_order_velocity_model::alphas(double alpha)
{
    double r=alpha*(alpha*k_+bs_)+bt_;
    return b_*k_+r/(2*b_)+(alpha*k_+bs_)*alpha/b_;
}
double dbdet_second_order_velocity_model::alphat(double alpha)
{
    double r=alpha*(alpha*k_+bs_)+bt_;
    return r/b_+(alpha*k_+bs_)*b_;
}
void dbdet_second_order_velocity_model::print_model()
{
    vcl_cout<<"(b="<<b_<<",k="<<k_<<",bt="<<bt_<<",bs_="<<bs_<<")";
}
dbdet_spherical_histogram_sptr
dbdet_second_order_velocity_model::computeV(dbdet_edgel* e,float s, float vmax)
{
    vmax=4.00;
    float tx=vcl_cos(e->tangent);
    float ty=vcl_sin(e->tangent);
    vbl_array_2d<float> votes(21,21);
    votes.fill(0);
    dbdet_spherical_histogram_sptr hist_sphere=new dbdet_spherical_histogram() ;
    float theta_min=-vnl_math::pi_over_2;
    float phi_min=-vnl_math::pi;

    float x=(e->pt.x()-160)/s;
    float y=(e->pt.y()-120)/s;

    float ut[3],un[3];
    ut[0]=-ty;ut[1]=tx;ut[2]=x*ty-y*tx;
    un[0]=-tx;un[1]=-ty;un[2]=x*tx+y*ty;

    if( vcl_fabs(b_)>vmax)
        return hist_sphere;

    if(vcl_fabs(b_)<0.3)
    {
        vnl_vector<float> ut_v(ut,3);
        vnl_matrix<float> D1=outer_product(ut_v,ut_v);

        vnl_vector<float> lambdas(3);
        vnl_matrix<float> Vs(3,3);

        if(!vnl_symmetric_eigensystem_compute(D1,Vs,lambdas))
            return hist_sphere;

        vnl_vector<float> v1= Vs.get_column(0);
        vnl_vector<float> v2= Vs.get_column(1);
    
        for(float psi=0;psi<2*vnl_math::pi;)
        {

            float a1=v1(1)*vcl_cos(psi)+v2(1)*vcl_sin(psi);
            float a2=v1(0)*vcl_cos(psi)+v2(0)*vcl_sin(psi);
            float a3=v1(2)*vcl_cos(psi)+v2(2)*vcl_sin(psi);

            float phi=vcl_atan2(a1,a2);
            float theta=vcl_atan(a3/vcl_sqrt(a1*a1+a2*a2));
            psi+=0.2;

            int index_j=vcl_floor(20*(theta-theta_min)/vnl_math::pi);
            int index_i=vcl_floor(20*(phi-phi_min)/(2*vnl_math::pi));
            hist_sphere->update(theta,phi);
            //votes(index_i,index_j)=1;
        }


    }
    else
    {
        float wmax=vcl_sqrt(vmax*vmax-b_*b_);

        float dbs2=0.1*0.1;
        float dk2=0.05*0.05;
        float dbt2=0.1*0.1;

        float d=vcl_sqrt(x*x+y*y);
        //: TODO also need to consider the case b=0;
        for(float w=-wmax;w<=wmax;)
        {
            float r=(2*w*bs_+bt_+w*w*k_);
            float m1=(2*b_/s*(w*ty+b_*tx)-r*y);
            float m2=(2*b_/s*(w*tx-b_*ty)-r*x);
            //: FIX the computation of dr.
            float dr=vcl_sqrt(4*w*w*dbs2+dbt2+w*w*w*w*dk2);
            float mag=vcl_sqrt(m1*m1+m2*m2);

            float theta=vcl_atan(r/mag);
            float phi=vcl_atan2(b_*m1,b_*m2);

            float vx=vcl_cos(theta)*vcl_cos(phi);
            float vy=vcl_cos(theta)*vcl_sin(phi);
            float vz=vcl_sin(theta);


            if((vx*ut[0]+vy*ut[1]+vz*ut[2])/b_>0)
            {
                float theta_r=vcl_cos(theta)/mag*(vcl_cos(theta)+vcl_sin(theta)*(y*vcl_sin(phi)+x*vcl_cos(phi)));
                float phi_r=(x*vcl_sin(phi)-y*vcl_cos(phi))/mag;

                float sig_theta=vcl_min(vcl_fabs(theta_r*dr)/(2*d),(float)vnl_math::pi_over_2);
                float sig_phi=vcl_min(vcl_fabs(phi_r*dr),(float)vnl_math::pi);

                int index_j=vcl_floor(20*(theta-theta_min)/vnl_math::pi);
                int index_i=vcl_floor(20*(phi-phi_min)/(2*vnl_math::pi));
                hist_sphere->update(theta,phi);

                if(votes(index_i,index_j)==0)
                {    
                    votes(index_i,index_j)++;

                    //for(float sigt=theta-sig_theta;sigt<=theta+sig_theta;)
                    //{
                    //    if(sigt>theta_min && sigt<(theta_min+vnl_math::pi))
                    //    {

                    //        int index_j=vcl_floor(20*(sigt-theta_min)/vnl_math::pi);

                    //        for(float sigp=phi-sig_phi;sigp<=phi+sig_phi;)
                    //        {
                    //            //: mod here is not correct
                    //            if(sigp>phi_min && sigp<(phi_min+2*vnl_math::pi))
                    //            {
                    //                int index_i=vcl_floor(20*(vcl_fmod(sigp,(float)vnl_math::pi)-phi_min)/(2*vnl_math::pi));
                    //                float vx1=vcl_cos(sigt)*vcl_cos(sigp);
                    //                float vy1=vcl_cos(sigt)*vcl_sin(sigp);
                    //                float vz1=vcl_sin(sigt);

                    //                if((vx1*ut[0]+vy1*ut[1]+vz1*ut[2])/b_>0)
                    //                {
                    //                    float r=-(vx1*un[0]+vy1*un[1]+vz1*un[2])/(vx1*ut[0]+vy1*ut[1]+vz1*ut[2]);

                    //                    float w=r*b_;
                    //                    if(vcl_fabs(w)<wmax && votes(index_i,index_j)==0)
                    //                    {                            
                    //                        //hist_sphere->update(sigt,sigp);
                    //                        //votes(index_i,index_j)++;
                    //                    }
                    //                }
                    //            }
                    //            sigp+=0.3;
                    //        }
                    //    }
                    //    sigt+=0.15;
                    //}
                }
            }
            w+=0.05;
        }
    }
    for(unsigned j=0;j<votes.cols();j++)
    {
        float sum=0;
        for(unsigned i=0;i<votes.rows();i++)
            sum+=votes(i,j);
        if(sum>0)
        {
            for(unsigned i=0;i<votes.rows();i++)
                votes(i,j)/=sum;
        }
    }
    //vcl_cout<<"\n";
    //vcl_cout<<votes;
    
    //hist_sphere.print();
    return hist_sphere;
}
