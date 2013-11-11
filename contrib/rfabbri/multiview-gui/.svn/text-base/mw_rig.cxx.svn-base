#include "mw_rig.h"

#include <vnl/algo/vnl_svd.h>


bool mw_rig::
read(vcl_string img_name1, vcl_string img_name2)
{
   if (!read_cam(img_name1,img_name2,&Pr1,&Pr2)) {
      vcl_cerr << "mw_rig: error reading cam\n";
      return false;
   }

   x_scale = Pr1.get_calibration().x_scale();
   y_scale = Pr1.get_calibration().y_scale();


   // Cache Rotations
   vnl_matrix_fixed<double,4,4> Raux1, Raux2;

   Pr1.get_rotation_matrix().get(&Raux1);
   Pr2.get_rotation_matrix().get(&Raux2);


   for (unsigned i=0; i<3; ++i) for (unsigned j=0; j<3; ++j) {
      Rot1(i,j) = Raux1(i,j);
      Rot1_t(i,j) = Raux1(j,i);
      Rot2(i,j) = Raux2(i,j);
      Rot2_t(i,j) = Raux2(j,i);
   }
   
   // Cache principal point as vnl vector
   vgl_point_3d<double> aux_pt;
  
   aux_pt = Pr1.get_camera_center();
   c1[0] = aux_pt.x(); c1[1] = aux_pt.y(); c1[2] = aux_pt.z(); 

   aux_pt = Pr2.get_camera_center();
   c2[0] = aux_pt.x(); c2[1] = aux_pt.y(); c2[2] = aux_pt.z(); 

   e11 =      Rot1_t * unit_x;
   e12 =      Rot1_t * unit_y;
   e13 = F1 = Rot1_t * unit_z;

   e21 =      Rot2_t * unit_x;
   e22 =      Rot2_t * unit_y;
   e23 = F2 = Rot2_t * unit_z;
}

void mw_rig::
get_gama1(double u, double v,mw_vector_3d *gama1)
{
   vgl_point_2d<double> uv0 = Pr1.get_calibration().principal_point();
   
   *gama1 = ((u - uv0.x())/x_scale)*e11 + ((v - uv0.y())/y_scale)*e12  + e13;
}

void mw_rig::
get_gama2(double u, double v,mw_vector_3d *gama2)
{
   vgl_point_2d<double> uv0 = Pr2.get_calibration().principal_point();
   
   *gama2 = ((u - uv0.x())/x_scale)*e21 + ((v - uv0.y())/y_scale)*e22  + e23;
}

//: returns error norm, ie, minimum distance btw the backprojection rays
double mw_rig::
reconstruct_point_lsqr(
      const mw_vector_3d &gama1,
      const mw_vector_3d &gama2,
      mw_vector_3d *Cpt_v)
{

   // global: vnl_matrix_fixed<double,3,2> A;
 
   for (unsigned i=0; i<3; ++i) {
      results.A(i,0) = gama1(i);
      results.A(i,1) = -gama2(i);
   }
 
   vnl_svd<double> svd(results.A);
   results.lambda = svd.solve(c2-c1);
 
   *Cpt_v = c1 + results.lambda(0)*gama1;

   return (results.A*results.lambda +c1 - c2).two_norm();
}

//: input tangents in world coords and normalized
double mw_rig::
reconstruct_tangent(
      const mw_vector_3d &gama1,
      const mw_vector_3d &gama2,
      const vgl_vector_2d<double> &t1_img,
      const vgl_vector_2d<double> &t2_img,
      mw_vector_3d *T_rec)
{
   mw_vector_3d t1_world;
   img1_to_world_vector(t1_img.x(),t1_img.y(), &t1_world);

   mw_vector_3d t2_world;
   img2_to_world_vector(t2_img.x(),t2_img.y(), &t2_world);

   reconstruct_tangent(gama1,gama2,t1_world,t2_world,T_rec);
}

