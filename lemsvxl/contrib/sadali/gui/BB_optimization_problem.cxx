#define MIN(a,b)  ((a<b) ? a : b)
#include "BB_optimization_problem.h"
#include <vil/vil_math.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_double_3.h>

#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_double_4x3.h>
#include <brip/brip_vil_float_ops.h>
#include <vnl/vnl_inverse.h>
#include <vil/vil_copy.h>
#include <vnl/algo/vnl_svd.h>
#include <gui/planar_curve_reconst_problem.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>

 void BB_optimization_problem::f(vnl_vector<double> const& x, vnl_vector<double>& fx)
    { double param_t = x[0];
      double param_alpha = x[1];
     
      
      //vcl_cout<<"i: "<<i<<"  newt:  "<<newt<<"\t";
      //vcl_cout<<newalpha<<"\n";
      vcl_vector<vnl_double_4> WorldPoints;
      fx.fill(0.0);

      vnl_double_4x4 UpdatedBBMatrix;
      updateBBMat_LM(UpdatedBBMatrix,param_t,param_alpha);
      
      computeWorldPoints_LM(WorldPoints, UpdatedBBMatrix);  
    
      compute_err_LM(WorldPoints,fx, UpdatedBBMatrix);
      WorldPoints.clear();
      
    

      
    }
 void 
     BB_optimization_problem::set_img(const vil_image_view<float> &leftimg, const vil_image_view<float> &rightimg)
 {
     vil_copy_deep(leftimg, left_img) ;
      vil_copy_deep(rightimg, right_img) ;
     mapleft.set_size(left_img.ni(), left_img.nj());
     mapright.set_size(right_img.ni(), right_img.nj());
 }
 
 void
    BB_optimization_problem::set_left_Proj(vnl_double_3x4 leftMat)
 {
    LeftProjMat = leftMat;
 }
 void 
     BB_optimization_problem::set_right_Proj(vnl_double_3x4 leftMat)
 {
    RightProjMat = leftMat;
    K_ = LeftProjMat.extract(3,3);
    t_[0] = RightProjMat[0][3]-LeftProjMat[0][3];
    t_[1] = RightProjMat[1][3]-LeftProjMat[1][3];
    t_[2] = RightProjMat[2][3]-LeftProjMat[2][3];

 }
double
BB_optimization_problem::compute_err_LM(const vcl_vector<vnl_double_4> &WorldPoints, vnl_vector<double> &residual_vector,
                                        const vnl_double_4x4 &BB)
{
if (WorldPlane_point_samp)
{
      double error = 0.0;
      
    int pixels_inside = 0;
    int pixels_outside  = 0;
    int WPsize = WorldPoints.size();
  
    vcl_cout<<WPsize;
    mapleft.fill(0.0);
     mapright.fill(0.0);
    for (int i = 0; i<WPsize; i++)
    {
        vnl_double_3  LeftProjPoint = LeftProjMat*WorldPoints[i];
        vnl_double_3  RightProjPoint = RightProjMat*WorldPoints[i];
        vgl_homg_point_2d<double> point(LeftProjPoint[0]/LeftProjPoint[2],LeftProjPoint[1]/LeftProjPoint[2]);
        vgl_homg_point_2d<double> right_point(RightProjPoint[0]/RightProjPoint[2],RightProjPoint[1]/RightProjPoint[2]);
        double diff;
        bool out_of_bounds = false;
        


        int xcoordleft = (int)point.x();
        int ycoordleft = (int)point.y();
        int xcoordright = (int)right_point.x();
        int ycoordright = (int)right_point.y();

        if (
         ((xcoordleft<0)||(ycoordleft<0)||(xcoordleft>=left_img.ni())||(ycoordleft>=left_img.nj()))
         ||
         ((xcoordright<0)||(ycoordright<0)||(xcoordright>=right_img.ni())||(ycoordright>=right_img.nj()))
         )
         {
            diff = 3.5;
            out_of_bounds =true;
            
           
         }
            else
        if  (((left_mask(xcoordleft, ycoordleft)==vxl_byte(0)) || (right_mask(xcoordright, ycoordright)==vxl_byte(0)) )&&use_mask)
        {
            diff =1.2;
            pixels_outside++;
            mapleft( xcoordleft,ycoordleft) = 1.0;
            mapright(xcoordright,ycoordright) = 1.0;
        }
       
        else
        {
            double leftval  = brip_vil_float_ops::bilinear_interpolation(left_img,point.x(),point.y());
           
               
        double rightval  = brip_vil_float_ops::bilinear_interpolation(right_img,right_point.x(),right_point.y());
        
            diff = leftval-rightval;
            pixels_inside++;
            mapleft( xcoordleft,ycoordleft) = 1.0;
            mapright(xcoordright,ycoordright) = 1.0;
   
        }

        
        error+= (diff*diff);
      
        residual_vector[i] = diff;
        
    

          
  }

 double m, n;

 vil_math_sum(m,mapleft, 0);
 vil_math_sum(n,mapright, 0);

double normalize = MIN(m,n);
 for (int i = 0; i<residual_vector.size();i++)
 {
     residual_vector[i]=residual_vector[i]/normalize;
 }
 



 return error;
}
else
{
mapleft.fill(0.0);
mapright.fill(0.0);

 double error_left = 0.0;
 double error_right = 0.0;
 double x1,y1;
 double diff;
 double m=0,n=0.0;
 vnl_double_4 test_point(1.0,0.0,0.0,1.0);
 vnl_double_4 test_BB_point =BB*test_point;
 compute_plane_params_LM(BB);
 vnl_svd<double> svd_decomp(LeftProjMat);
 vnl_double_4x3 psuedoinverse = svd_decomp.pinverse(3);
 vnl_double_4  CameraCenter = svd_decomp.nullvector();
 double denom = dot_product(Plane_params,CameraCenter);
 int inside_pixel = 0;
 int pixel_samps = 0;
 for (int i = 0; i<left_img.ni(); i++)
     for (int j = 0; j<left_img.nj(); j++)
     {

         diff = 0.0;   
         if  ((left_mask(i, j)!=vxl_byte(0)))
         {
             vnl_vector<double> param_vec(3);

             x1 = (double)i; 
             y1 = (double)j;
             vnl_double_3 orig_point(x1,y1,1.0);
             vgl_homg_point_2d<double> orig_pt(x1,y1,1.0);
#if 0
             vnl_double_4 back_proj_point = psuedoinverse * orig_point;
             double num = dot_product(Plane_params, back_proj_point);
             double lambda = -num/denom;
             vnl_double_4 threeD_point = back_proj_point+lambda*CameraCenter;

             param_vec[0] = threeD_point[1]/threeD_point[3];
             param_vec[1] = threeD_point[2]/threeD_point[3];
             param_vec[2] = 1.0;
             //   vcl_cout<<"Point by projection"<<newpoint->x()<<"\t"<<newpoint->y()<<"\t"<<newpoint->z()<<"\n";
             //  vcl_cout<<"Point by homography"<<result_point[0]<<"\t"<<result_point[1]<<"\t"<<result_point[2]<<"\n";
             planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BB,orig_point,LeftProjMat);
             vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));
             LM_instance->minimize_without_gradient(param_vec);
      //       LM_instance->diagnose_outcome(vcl_cout);
             vnl_double_4 newpt(test_BB_point[0],  param_vec[0]/param_vec[2], param_vec[1]/param_vec[2],1.0);
             vnl_double_3 proj_point= RightProjMat*newpt;
             vgl_homg_point_2d<double> right_point(proj_point[0]/proj_point[2],proj_point[1]/proj_point[2]);
        
             delete reconstructor;
             delete LM_instance;
#endif
             vgl_homg_point_2d<double> right_point= Left2Right(orig_pt);
             if  ((( right_point.x()/right_point.w())<0.0)||(( right_point.y()/right_point.w())<0.0)||
                 (( right_point.x()/right_point.w())>right_img.ni())||(( right_point.y()/right_point.w())>right_img.nj()))
                 continue;
             mapright(int( right_point.x()/right_point.w()),int( right_point.y()/right_point.w())) = 1.0;
             if (right_mask(int( right_point.x()/right_point.w()),int( right_point.y()/right_point.w()) )==vxl_byte(0))
                 
             {
                
                diff = 1.2;
                residual_vector[pixel_samps] = diff;
                pixel_samps++;
                inside_pixel++;

             }
                 else
                 {
                     inside_pixel++;

                     double leftval  = left_img(i,j);
                     double rightval  = brip_vil_float_ops::bilinear_interpolation(right_img,
                         right_point.x()/right_point.w(),right_point.y()/right_point.w());
                     diff = leftval-rightval;
                     residual_vector[pixel_samps] = diff;

                     pixel_samps++;

                 }

         }
         error_left+= diff*diff;
     }
     vil_math_sum(m,mapright,0);
     error_left/=(double)num_of_residuals;
     inside_pixel = 0;
     vnl_svd<double> svd_decomp_R(RightProjMat);
     psuedoinverse = svd_decomp_R.pinverse(3);
     CameraCenter = svd_decomp_R.nullvector();
     denom = dot_product(Plane_params,CameraCenter);
    
     for (int i = 0; i<right_img.ni(); i++)
         for (int j = 0; j<right_img.nj(); j++)
         {
             diff = 0.0;

             if  ((right_mask(i, j)!=vxl_byte(0)))
             {
                  vnl_vector<double> param_vec(3);

                 x1 = (double)i; 
                 y1 = (double)j;
                 vnl_double_3 orig_point(x1,y1,1.0);
                 vgl_homg_point_2d<double> orig_pt(x1,y1,1.0);
#if 0
                 vnl_double_4 back_proj_point = psuedoinverse * orig_point;
                 double num = dot_product(Plane_params, back_proj_point);
                 double lambda = -num/denom;
                 vnl_double_4 threeD_point =  back_proj_point+lambda*CameraCenter;

                 param_vec[0] = threeD_point[1]/threeD_point[3];
                 param_vec[1] = threeD_point[2]/threeD_point[3];
                 param_vec[2] = 1.0;
               planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BB, orig_point, RightProjMat);
                 vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));
                 LM_instance->minimize_without_gradient(param_vec);
        //         LM_instance->diagnose_outcome(vcl_cout);
                 vnl_double_4 newpt(test_BB_point[0], param_vec[0]/param_vec[2], param_vec[1]/param_vec[2],1.0);
                 vnl_double_3 proj_point= LeftProjMat*newpt;
                 vgl_homg_point_2d<double> left_point(proj_point[0]/proj_point[2],proj_point[1]/proj_point[2]);
               
                
                 delete reconstructor;
                 delete LM_instance;
#endif
              
                 vgl_homg_point_2d<double> left_point = Left2Right.preimage(orig_pt);
                 double leftval = right_img(i,j);
                 if  ((( left_point.x()/left_point.w() )<0.0)||(( left_point.y()/left_point.w())<0.0)||
                     (( left_point.x()/left_point.w() )>left_img.ni())||(( left_point.y()/left_point.w())>left_img.nj()))
                     continue;  
                 mapleft(int( left_point.x()/left_point.w()),int( left_point.y()/left_point.w())) = 1.0;

                 if (left_mask(int( left_point.x()/left_point.w()),int( left_point.y()/left_point.w()) )==vxl_byte(0))
                 {
              
                  diff = 1.2;
                  residual_vector[pixel_samps] = diff;
                  pixel_samps++;
                  inside_pixel++;
                 }
                 else
                 {

                     inside_pixel++;
                     double rightval = brip_vil_float_ops::bilinear_interpolation(left_img,
                         left_point.x()/left_point.w(),left_point.y()/left_point.w());
                     diff = leftval-rightval;
                     residual_vector[pixel_samps] = diff;
                     pixel_samps++;
                     
                 }

             }
             error_right+= diff*diff;
         }
         vil_math_sum(n,mapleft,0);
         error_right/=(double)num_of_residuals;
         for (int i = 0; i<num_of_residuals;i++)
         {
             residual_vector[i]/=num_of_residuals;
         }
         
         return (error_left+error_right); 
}
} 


 void
     BB_optimization_problem::computeWorldPoints_LM(vcl_vector<vnl_double_4>  &WP, vnl_double_4x4 BB)
 {
     int  numx = (int)vcl_floor(1.0/sampx);
     int  numy = (int)vcl_floor(1.0/sampy);
     vnl_double_4 newpoint_World;
     vnl_double_4 newp;
       //*****Depends on front plane x=0 or x=1******/////
     newp[0] = 1.0;
       //*****Depends on front plane x=0 or x=1******/////
     newp[3] = 1.0;
  
     for (int i= 0; i<numx; i++ )
         for (int j=0 ; j<numy; j++)
         {
             newp[1] = i*sampx;
             newp[2] = j*sampy;
             newpoint_World = BB*newp;
             WP.push_back(newpoint_World);

         }

    


 }
 void BB_optimization_problem::updateBBMat_LM(vnl_double_4x4 &UpdatedBBMatrix, const double coeff, const double newalpha)
 {


          vnl_double_4 pivot_point;
     
         
     vnl_double_4 Normal;

 //    vcl_cout<<trans_vec;
   //  temp= RSt;
//     vnl_double_4 Normal(-1.0,0.0,0.0,1.0);
     vnl_double_3x1 tempt, Normaltemp;
     tempt[0][0] = -t_[0];
     tempt[1][0] = -t_[1];
     tempt[2][0] = -t_[2];

     Normaltemp = vnl_inverse(K_)*(tempt);

     Normal[0] = Normaltemp[0][0];
     Normal[1] = Normaltemp[1][0];
     Normal[2] = Normaltemp[2][0];

   
    
     double coef= coeff;
     vcl_cout<<"coef:  "<<coef<<"\n";
     vcl_cout<<"angle: "<<newalpha<<"\n";
     vnl_double_4x4 Temp;
     Temp.set_identity();

vnl_double_4x4 T;
T.set_identity();

Temp= InitialBB;
if (opt_translate)
{

if (translate_from_KT)
{
Temp[0][3] = trans_vec[0]+coef*Normal[0];
Temp[1][3] = trans_vec[1]+coef*Normal[1];
Temp[2][3] = trans_vec[2]+coef*Normal[2];
}
else
{  //*****Depends on front plane x=0 or x=1******/////
    vnl_double_4 cornerpointone(1.0,0.0,0.0,1.0);
    vnl_double_4 cornerpointtwo(1.0,1.0,0.0,1.0);
    vnl_double_4 cornerpointthr(1.0,0.0,1.0,1.0);
    vnl_double_4 cornerpointfou(1.0,1.0,1.0,1.0);
  //*****Depends on front plane x=0 or x=1******/////
    cornerpointone = InitialBB*cornerpointone;
    cornerpointtwo = InitialBB*cornerpointtwo;
    cornerpointthr = InitialBB*cornerpointthr;
    cornerpointfou = InitialBB*cornerpointfou;
    vcl_vector<vnl_double_4> corners(4);
    corners[0] = cornerpointone;
    corners[1] = cornerpointtwo;
    corners[2] = cornerpointthr;
    corners[3] = cornerpointfou;
    compute_Normal_LM(corners, Normal);
     double dist = compute_lengthBB_LM(InitialBB);
    vcl_cout<<"\nNormal2:   "<<Normal<<"\n";
    Temp[0][3] = trans_vec[0]+coef*Normal[0]*dist;
    Temp[1][3] = trans_vec[1]+coef*Normal[1]*dist;
    Temp[2][3] = trans_vec[2]+coef*Normal[2]*dist;

}
}
//vcl_cout<<"Temp is  "<<Temp;
 //*****Depends on front plane x=0 or x=1******/////
vnl_double_4 pivot(1.0,0.5,0.5, 1.0) ;
pivot_point = Temp*pivot;
//pivot_point = pivot;
vnl_double_4 rot_axis(1.0,1.0,0.0, 0.0) ;
rot_axis = Temp* rot_axis;
 //*****Depends on front plane x=0 or x=1******/////
vnl_double_4x4 R = rot_mat(pivot_point,rot_axis, newalpha);
//vcl_cout<<" R is " <<R;
if (opt_rotation)

UpdatedBBMatrix = R*Temp;
else
UpdatedBBMatrix = Temp;
//vcl_cout<<"New Mat:\n"<<UpdatedBBMatrix;


 }


 vnl_double_4x4
     BB_optimization_problem::rotX_mat(const double rad) {
         // TODO: fill this in
         vnl_double_4x4 result;
         result.set_identity();
         
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if (((i==2)&&(j==2))||((i==1)&&(j==1)))
                     result[i][j] = cos(rad);
                 else 
                     if ((i==2)&&(j==1))
                         result[i][j] = sin(rad);

                     else if ((i==1)&&(j==2))
                         result[i][j] = -1.0*sin(rad);
                     else 
                         if (i==j)
                             result[i][j] = 1.0;
                         else 
                             result[i][j] =0.0;



             }


             // TODO: this return value is bogus
             return result;
     };

 // Returns a rotation matrix effecting rotation around the Y axis by
 // specified radians
vnl_double_4x4
     BB_optimization_problem::rotY_mat(const double radians) {
        vnl_double_4x4 rotY ;
        rotY.set_identity();
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             { if (((i==0)&&(j==0))||((i==2)&&(j==2)))
             rotY[i][j] = cos(radians);
             else
                 if ((i==0)&&(j==2))
                     rotY[i][j] = sin(radians);
                 else if ((i==2)&&(j==0))
                     rotY[i][j] = -1.0*sin(radians);

                 else
                     if (i==j)
                         rotY[i][j] = 1.0;
                     else
                         rotY[i][j] = 0.0;


             }

             // TODO: this return value is bogus
             return rotY;

     };

 // Returns a rotation matrix effecting rotation around the Z axis by
 // specified radians
vnl_double_4x4
     BB_optimization_problem::rotZ_mat(const double radians) {
        
        vnl_double_4x4 rotZ;
        rotZ.set_identity();
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             { if (((i==0)&&(j==0))||((i==1)&&(j==1)))
             rotZ[i][j] = cos(radians);
             else
                 if ((i==1)&&(j==0))
                     rotZ[i][j] = sin(radians);
                 else if ((i==0)&&(j==1))
                     rotZ[i][j] = -1.0*sin(radians);

                 else
                     if (i==j)
                         rotZ[i][j] = 1.0;
                     else
                         rotZ[i][j] = 0.0;


             }

            
             return rotZ;
     };

 // Returns a rotation matrix effecting a rotation around the given vector and
 // point, by the specified number of radians.
 vnl_double_4x4
     BB_optimization_problem::rot_mat(const vnl_double_4 &p, const  vnl_double_4 &v, const double a){


         double theta;
         double phi;



         theta = -1.00*atan2(v[0],v[2]);
         vnl_double_4 axaz(v[0], 0.0 , v[2], 0.0);
         double magn = axaz.two_norm();
         phi = atan2(v[1],magn);


         vnl_double_4x4 T= (trans_mat(-1.0*p));



        vnl_double_4x4 A = rotY_mat(theta);




         vnl_double_4x4  B  = rotX_mat(phi);


         vnl_double_4x4  C = rotZ_mat((double)a);


         vnl_double_4x4 Binv = rotX_mat(-phi);


         vnl_double_4x4  Ainv = rotY_mat(-theta);


         vnl_double_4x4  Tinv = (trans_mat(p));


         return Tinv*Ainv*Binv*C*B*A*T;



     };  


 vnl_double_4x4
     BB_optimization_problem::trans_mat(const vnl_double_4 &trans)
 {

     vnl_double_4x4 T;
     T.set_identity();
     T[0][3] = trans[0];
     T[1][3] = trans[1];
     T[2][3] = trans[2];
     return T;

 }

 void BB_optimization_problem::set_masks(const vil_image_view<vxl_byte> &leftmask, const vil_image_view<vxl_byte> &rightmask)
{
     vil_copy_deep(leftmask, left_mask );
     vil_copy_deep(rightmask, right_mask);

}

void
BB_optimization_problem::compute_Normal_LM(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal)
{
    assert(corners.size()==4);


    vnl_double_3 non_homgcorner1, non_homgcorner2, non_homgcorner3;
    non_homgcorner1 = corners[0].extract(3);
    non_homgcorner2 = corners[1].extract(3);
    non_homgcorner3 = corners[2].extract(3);
    vnl_double_3 v1 = non_homgcorner2-non_homgcorner1;
    vnl_double_3 v2 = non_homgcorner3-non_homgcorner1;
    vnl_double_3 N1 = cross_3d(v2,v1);
    double d = dot_product(non_homgcorner1,N1);
    Normal[0] = N1[0]/-d; 
    Normal[1] = N1[1]/-d;
    Normal[2] = N1[2]/-d;
    Normal[3] = 0.0;
    Normal.normalize();



}
void
BB_optimization_problem::compute_plane_params_LM(const vnl_double_4x4 &BB)

{
    //*****Depends on front plane x=0 or x=1******/////
    vnl_double_4 cornerpointone(1.0,0.0,0.0,1.0);
    vnl_double_4 cornerpointtwo(1.0,1.0,0.0,1.0);
    vnl_double_4 cornerpointthr(1.0,0.0,1.0,1.0);
    vnl_double_4 cornerpointfou(1.0,1.0,1.0,1.0);
    //*****Depends on front plane x=0 or x=1******/////
    cornerpointone = BB*cornerpointone;
    cornerpointtwo = BB*cornerpointtwo;
    cornerpointthr = BB*cornerpointthr;
    cornerpointfou = BB*cornerpointfou;

    //// Calculating Homographies

    vgl_h_matrix_2d_compute_4point hcl;
    vcl_vector <vgl_homg_point_2d <double> > point_set1, point_set2;
    vnl_double_3 projcponeL = LeftProjMat*cornerpointone;
    vnl_double_3 projcptwoL = LeftProjMat*cornerpointtwo;
    vnl_double_3 projcpthrL = LeftProjMat*cornerpointthr;
    vnl_double_3 projcpfouL = LeftProjMat*cornerpointfou;
    point_set1.push_back(vgl_homg_point_2d<double>(projcponeL[0]/projcponeL[2],projcponeL[1]/projcponeL[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcptwoL[0]/projcptwoL[2],projcptwoL[1]/projcptwoL[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcpthrL[0]/projcpthrL[2],projcpthrL[1]/projcpthrL[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcpfouL[0]/projcpfouL[2],projcpfouL[1]/projcpfouL[2]));


    point_set2.push_back(vgl_homg_point_2d<double>(cornerpointone[1],cornerpointone[2]));
    point_set2.push_back(vgl_homg_point_2d<double>(cornerpointtwo[1],cornerpointtwo[2]));
    point_set2.push_back(vgl_homg_point_2d<double>(cornerpointthr[1],cornerpointthr[2]));
    point_set2.push_back(vgl_homg_point_2d<double>(cornerpointfou[1],cornerpointfou[2]));

    Hleft = hcl.compute(point_set1, point_set2);
    point_set1.clear();

    vnl_double_3 projcponeR = RightProjMat*cornerpointone;
    vnl_double_3 projcptwoR = RightProjMat*cornerpointtwo;
    vnl_double_3 projcpthrR = RightProjMat*cornerpointthr;
    vnl_double_3 projcpfouR = RightProjMat*cornerpointfou;
    point_set1.push_back(vgl_homg_point_2d<double>(projcponeR[0]/projcponeR[2],projcponeR[1]/projcponeR[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcptwoR[0]/projcptwoR[2],projcptwoR[1]/projcptwoR[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcpthrR[0]/projcpthrR[2],projcpthrR[1]/projcpthrR[2]));
    point_set1.push_back(vgl_homg_point_2d<double>(projcpfouR[0]/projcpfouR[2],projcpfouR[1]/projcpfouR[2]));

    // H represents the homography that
    // transforms points from  plane1 into plane2.
    Hright = hcl.compute(point_set1, point_set2);
  //  vcl_cout<<Hleft<<"\n"<<Hright<<"\n";
    Left2Right = Hleft*Hright.get_inverse();










    vcl_vector<vnl_double_4> corners(4);
    corners[0] = cornerpointone;
    corners[1] = cornerpointtwo;
    corners[2] = cornerpointthr;
    corners[3] = cornerpointfou;
    vnl_double_3 non_homgcorner1, non_homgcorner2, non_homgcorner3;
    non_homgcorner1 = corners[0].extract(3);
    non_homgcorner2 = corners[1].extract(3);
    non_homgcorner3 = corners[2].extract(3);
    vnl_double_3 v1 = non_homgcorner2-non_homgcorner1;
    vnl_double_3 v2 = non_homgcorner3-non_homgcorner1;
    vnl_double_3 N1 = cross_3d(v2,v1);
    double d = dot_product(non_homgcorner1,N1);
    Plane_params[0] = N1[0]/-d; 
    Plane_params[1] = N1[1]/-d;
    Plane_params[2] = N1[2]/-d;
    Plane_params[3] = 1.0;
    
}

double 
BB_optimization_problem::compute_lengthBB_LM(const vnl_double_4x4 &BBMat)
{
    vnl_double_4 orig(0.0,0.0,0.0,1.0);
    vnl_double_4 axisx(1.0,0.0,0.0,1.0);
    orig= BBMat*orig;
    axisx = BBMat*axisx;
    vnl_double_4 diffx = axisx-orig;
    return diffx.two_norm();

}

