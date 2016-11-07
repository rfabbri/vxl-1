#ifndef BB_optimization_problem__h
#define BB_optimization_problem__h
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4x4.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_least_squares_function.h>
#include <vcl_vector.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

class  BB_optimization_problem : public vnl_least_squares_function

{
public:
    BB_optimization_problem(bool translater,bool rotat, bool trans_by_KT, bool WP_pt_samp, double samplex,double sampley ,
        vnl_double_4x4 & BB, unsigned int num_residuals, const vnl_double_3x4& LeftProj,const vnl_double_3x4& RightProj)
        : vnl_least_squares_function( 2, num_residuals, no_gradient)
    {
        sampx = samplex;
        sampy = sampley;
        InitialBB = BB;
        num_of_residuals =num_residuals;
        opt_rotation = rotat;
        opt_translate = translater;
        trans_vec =BB.get_column(3);
        trans_vec[3] = 0.0;
        translate_from_KT = trans_by_KT;
        WorldPlane_point_samp= WP_pt_samp;
        set_left_Proj(LeftProj);
        set_right_Proj(RightProj);
        compute_plane_params_LM(BB);
        
      

    }
    ~BB_optimization_problem()
    {
    }
    virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);
    void set_left_Proj(vnl_double_3x4 ProjMat);
    void set_right_Proj(vnl_double_3x4 ProjMat);
    void set_img(const vil_image_view<float> &leftimg, const vil_image_view<float> &rightimg);
    
    void set_masks(const vil_image_view<vxl_byte> &leftmask, const vil_image_view<vxl_byte> &rightmask);
    
   

    void updateBBMat_LM(vnl_double_4x4 &UpdatedBBMatrix, const double newt, const double newalpha);
      
    void   computeWorldPoints_LM( vcl_vector < vnl_double_4 > &WorldPoints, vnl_double_4x4 UpdatedBBMatrix);  
    double compute_err_LM(const vcl_vector< vnl_double_4 > &WorldPoints, vnl_vector<double> &residuals,
        const vnl_double_4x4 &BB);
    void compute_plane_params_LM(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal);
     vnl_double_4x4 rot_mat(const vnl_double_4 &p, const  vnl_double_4 &v, const double a);
     vnl_double_4x4     rotZ_mat(const double radians);
     vnl_double_4x4     rotY_mat(const double radians);
     vnl_double_4x4     rotX_mat(const double radians);
     vnl_double_4x4 trans_mat(const vnl_double_4 &trans);
     void compute_plane_params_LM(const vnl_double_4x4 &BB);
     void
         compute_Normal_LM(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal);




protected:
    double 
        compute_lengthBB_LM(const vnl_double_4x4 &BBMat);



    vnl_double_3x4 LeftProjMat;
    vnl_double_3x4 RightProjMat;
    vnl_double_4x4 InitialBB;
    vnl_double_4 trans_vec;
    vnl_double_3x3 K_;
    vnl_double_3 t_;
    vnl_double_4 Plane_params;

    vil_image_view<float> left_img;
    vil_image_view<float> right_img;
    vil_image_view<vxl_byte> left_mask;
    vil_image_view<vxl_byte> right_mask;
    vil_image_view<float> mapleft;
    vil_image_view<float> mapright;


    double sampx;
    double sampy;
    double tinterval;
    double t ;
    bool use_mask;
    unsigned int num_of_residuals;

    bool opt_rotation;
    bool opt_translate;
    bool translate_from_KT;

    vgl_h_matrix_2d<double> Hleft;
    vgl_h_matrix_2d<double> Hright;
    vgl_h_matrix_2d<double> Left2Right;
    bool WorldPlane_point_samp;

   





}
;
#endif
