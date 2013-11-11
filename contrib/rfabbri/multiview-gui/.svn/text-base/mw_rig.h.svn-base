// This is mw_rig.h
#ifndef mw_rig_h
#define mw_rig_h
//:
//\file
//\brief This structure aids programming based on global coordinates as in notation of the paper
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 05/04/2005 06:26:54 PM EDT
//

#include "mw_util.h"

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vnl/vnl_cross.h>
#include <vpgl/vpgl_perspective_camera.h>


//: It contains 2 perspective cameras with same intrinsic params
// It caches many camera-related data to facilitate work in global
// non-homogeneous coordinates. In this philosophy we avoid converting back to image
// pixel coordinates whenever we can. Also, the cache variable have all
// compatible representations when possible, e.g. matrices and vectors
//
typedef vnl_vector_fixed<double,3> mw_vector_3d;
typedef vnl_vector_fixed<double,2> mw_vector_2d;
class mw_rig {
public: // Methods
   mw_rig(): unit_x(1,0,0), unit_y(0,1,0), unit_z(0,0,1) {}
   ~mw_rig() {}

   bool read(vcl_string img_name1, vcl_string img_name2);

   void get_gama1(double u, double v,mw_vector_3d *gama1);
   void get_gama2(double u, double v,mw_vector_3d *gama2);


   //: project given gama1
   void project_t1(const mw_vector_3d &T, const mw_vector_3d &gama1, mw_vector_3d *t1)
   { *t1 = T - dot_product(T,F1)*gama1; }

   double reconstruct_point_lsqr(
      const mw_vector_3d &gama1, const mw_vector_3d &gama2,
      mw_vector_3d *Cpt_v);

   inline void reconstruct_tangent(
      const mw_vector_3d &gama1, const mw_vector_3d &gama2,
      const mw_vector_3d &t1, const mw_vector_3d &t2,
      mw_vector_3d *Cpt_v);

   //: overload for vgl_vector_2d
   double mw_rig::
   reconstruct_tangent(
         const mw_vector_3d &gama1, const mw_vector_3d &gama2,
         const vgl_vector_2d<double> &t1_img, const vgl_vector_2d<double> &t2_img,
         mw_vector_3d *T_rec);

   void img1_to_world_vector(double img_x, double img_y, vnl_vector_fixed<double, 3> *t1_world) {
      mw_vector_3d t1_cam;

      t1_cam[0] = img_x/x_scale;   //:< take off streching;
      t1_cam[1] = img_y/y_scale;
      t1_cam[2] = 0;
      *t1_world= Rot1_t*t1_cam;
      t1_world->normalize();
   }

   void img2_to_world_vector(double img_x, double img_y, vnl_vector_fixed<double, 3> *t2_world) {
      mw_vector_3d t2_cam;

      t2_cam[0] = img_x/x_scale;   //:< take off streching;
      t2_cam[1] = img_y/y_scale;
      t2_cam[2] = 0;
      *t2_world= Rot2_t*t2_cam;
      t2_world->normalize();
   }

   void world_to_img1_vector(const vnl_vector_fixed<double, 3> &v_world, double *img_x, double *img_y) {
      mw_vector_3d v_cam1;

      v_cam1 = Rot1 * v_world;

      *img_x = v_cam1[0]*x_scale;
      *img_y = v_cam1[1]*y_scale;
      double len = hypot(*img_x,*img_y);
      *img_x /= len;
      *img_y /= len;
   }

   void world_to_img2_vector(const vnl_vector_fixed<double, 3> &v_world, double *img_x, double *img_y) {
      mw_vector_3d v_cam2;

      v_cam2 = Rot2 * v_world;

      *img_x = v_cam2[0]*x_scale;
      *img_y = v_cam2[1]*y_scale;
      double len = hypot(*img_x,*img_y);
      *img_x /= len;
      *img_y /= len;
   }


   // TODO:
   // project_tangent
   //
   // project_point returning image point in WORLD coordinates
public: // DATA

   vpgl_perspective_camera <double> Pr1;
   vpgl_perspective_camera <double> Pr2;

   vnl_matrix_fixed<double,3,3> Rot1, Rot2;
   vnl_matrix_fixed<double,3,3> Rot1_t, Rot2_t;
   double x_scale;
   double y_scale;

   mw_vector_3d 
      c1,c2,
      e11, e12, e13,
      e21, e22, e23,
      F1, F2; //:< these are currently equal to ei3

   // Utility:
   mw_vector_3d unit_x, unit_y, unit_z;

   // Marginal variables:
   struct results {
      //: stores [gama1 -gama2] after a call to reconstruct_point_lsqr
      vnl_matrix_fixed<double,3,2> A;
      //: stores 2 depths after a call to reconstruct_point_lsqr
      vnl_vector<double> lambda;
   } results;

};


//: input tangents in world coords and normalized
inline void mw_rig::
reconstruct_tangent(
      const mw_vector_3d &gama1,
      const mw_vector_3d &gama2,
      const mw_vector_3d &t1,
      const mw_vector_3d &t2,
      mw_vector_3d *T_rec)
{
   // assert dot_product(t1,F1) near zero

   assert (mw_util::near_zero(dot_product(t1,F1)) && mw_util::near_zero(dot_product(t2,F2)));
   *T_rec = vnl_cross_3d( vnl_cross_3d(t1,gama1), vnl_cross_3d(t2,gama2) );
}

#endif // mw_rig_h

