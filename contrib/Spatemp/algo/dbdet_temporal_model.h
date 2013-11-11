#ifndef dbdet_temporal_model_h
#define dbdet_temporal_model_h

#include <vcl_vector.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>

#include <dbdet/sel/dbdet_curvelet.h>
#include <Spatemp/algo/dbdet_spherical_histogram.h>
#include <Spatemp/algo/dbdet_spherical_histogram_sptr.h>
class dbdet_temporal_model
{
public:
    dbdet_temporal_model(){}
    ~dbdet_temporal_model(){}

    bool isvalid(){return isvalid_;}

   // virtual bool is_model_intersect(dbdet_temporal_model * m, double epsilon)=0;

    virtual void print_model()=0;

    unsigned num_obs(){return num_obs_;}

    void set_num_obs(unsigned nobs){num_obs_=nobs;}
protected:
    bool isvalid_;
    unsigned num_obs_;
};
class dbdet_temporal_curvature_velocity_model:public dbdet_temporal_model
{
public:
    dbdet_temporal_curvature_velocity_model(){}
    dbdet_temporal_curvature_velocity_model(dbdet_curvelet * c1, dbdet_curvelet *c2, int t);
    dbdet_temporal_curvature_velocity_model(vnl_vector_fixed<double,2> m, vnl_matrix_fixed<double,2,2> c);
    ~dbdet_temporal_curvature_velocity_model();
    //void estimate_velocity(vgl_point_2d<double> p1, vgl_point_2d<double> p2);
    bool iscompatible(dbdet_curvelet * c);
    bool is_model_intersect(dbdet_temporal_model * m,  double dist_epsilon_square);
   // bool is_model_intersect_motion(dbdet_temporal_model * m, double dist_epsilon_square);
    void print_model();
    vnl_vector_fixed<double,2> meanV(){ return mean_;}
    vnl_matrix_fixed<double,2,2> covarV(){ return covar_;}
    double sqr_mahalanobis_dist(vnl_vector_fixed<double,2> V0);
private:

    double dx_;
    double ds_;
    double kmean;
    double ksig;
    double s_;

    vnl_vector_fixed<double,2> mean_;
    vnl_matrix_fixed<double,2,2> covar_;
    
    
};

//: this model is based on papadoublou::faugeras::IJCV:1993
class dbdet_temporal_normal_velocity_model:public dbdet_temporal_model
{
public:
    dbdet_temporal_normal_velocity_model(){}
    dbdet_temporal_normal_velocity_model(const dbdet_temporal_normal_velocity_model & m);
    dbdet_temporal_normal_velocity_model(double beta,double beta_t,dbdet_curvelet * ref_curvelet);
    dbdet_temporal_normal_velocity_model(const dbdet_temporal_normal_velocity_model & m, dbdet_temporal_normal_velocity_model & nbr);
    dbdet_temporal_normal_velocity_model(float beta, int t);
    dbdet_temporal_normal_velocity_model(dbdet_curvelet * c1, dbdet_curvelet *c2, int t);
    dbdet_temporal_normal_velocity_model( vcl_map<int,double> betamap);

    dbdet_temporal_normal_velocity_model(dbdet_curvelet * refc, vcl_map<int,double> betamap,vcl_map<int,dbdet_curvelet*> cmap);
    ~dbdet_temporal_normal_velocity_model(){}
    bool iscompatible(dbdet_curvelet * c);
    bool is_model_intersect(dbdet_temporal_model * m,  double dist_epsilon_square);
    bool compute_bs();
    double avg_beta();
    bool compute_betas();
    bool compute_beta_s(dbdet_temporal_normal_velocity_model & nbr);
    void print_model();
    dbdet_curvelet* ref_curvelet;
    vcl_map<int,double> betas;
    //vcl_map<int,dbdet_curvelet*> cs;
    void compute_2d_map();
    bool compute_intersection(dbdet_curvelet * refc, dbdet_curvelet * c, double & beta);


    bool compute_one_paramter_V();

    double bt;
    float b0;
    double k;
    double bs;
    float w1;
    float w2;
    double t_pos_infinity;
    double t_neg_infinity;
    
    double dk;
    double dw;
    vnl_vector<float> tau1;
    vnl_vector<float> tau2;
    vnl_vector<float> dtau;

    vnl_vector<float> v1_1;
    vnl_vector<float> v1_2;
    vnl_matrix<float> v1_1v1_1;
    vnl_matrix<float> v1_1v1_2;
    vnl_matrix<float> v1_2v1_2;

    vnl_vector<float> v2_1;
    vnl_vector<float> v2_2;
    vnl_matrix<float> v2_1v2_1;
    vnl_matrix<float> v2_1v2_2;
    vnl_matrix<float> v2_2v2_2;

    double ba_ratio1;
    double ba_ratio2;
    double a[2];
    double b[2];
    double c[2];


    double tangent;

    bool iscomputed_;
    vcl_vector<float> f1t;
    vcl_vector<float> f2t;

    vcl_vector<float> f1p;
    vcl_vector<float> f2p;

    float phi1_min;
    float phi1_max;
    float phi2_min;
    float phi2_max;
private:


};
//: this model is based on papadoublou::faugeras::IJCV:1993
class dbdet_second_order_velocity_model:public dbdet_temporal_model
{
public:
    dbdet_second_order_velocity_model(){}
    ~dbdet_second_order_velocity_model(){}
    dbdet_second_order_velocity_model(float b,float k, float bt, float bs);
    vnl_vector<float>  compute_V_by_rho(dbdet_edgel* e,float s,double alpha);

    void print_model();

    dbdet_spherical_histogram_sptr computeV(dbdet_edgel* e,float s=320, float vmax=3.0);

    double alphas(double alpha);
    double alphat(double alpha);
//private:
    float b_;
    float k_;
    float bt_;
    float bs_;
    dbdet_curvelet * refc_;
};

class dbdet_3D_velocity_model
{

public:
    dbdet_3D_velocity_model();
    dbdet_3D_velocity_model(int tbins, int pbins, float theta_min, float theta_max, float phi_min, float phi_max);
    ~dbdet_3D_velocity_model(){}

    bool increment(float theta, float phi);

    float theta_min_;
    float theta_max_;
    float phi_min_;
    float phi_max_;

    int tbins_;
    int pbins_;

    void print();
    vbl_array_2d<int> theta_phi_hist;
};
dbdet_temporal_curvature_velocity_model * intersect(dbdet_temporal_curvature_velocity_model & m1,dbdet_temporal_curvature_velocity_model & m2);
dbdet_temporal_normal_velocity_model * intersect(dbdet_temporal_normal_velocity_model * m1,dbdet_temporal_normal_velocity_model * m2);

#endif
