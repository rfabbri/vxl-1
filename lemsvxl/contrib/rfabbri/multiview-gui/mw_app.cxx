//:
//\file
//\brief multiview interactive application
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date Sat Apr 16 22:49:07 EDT 2005
//

#include <bvis1/bvis1_manager.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include "mw_util.h"
#include "mw_rig.h"
#include "mw_app.h"

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>
#include <iostream>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>


#define PI vnl_math::pi

void 
mw_misc()
{
}

void
test_point_reconstruct()
{
   vgl_homg_point_2d<double> p1,p2,p3;
 
   std::string fname1("curr/p1010049.jpg");
   std::string fname2("curr/p1010053.jpg");
   std::string fname3("curr/p1010069.jpg");
   // Reading cameras
   vpgl_perspective_camera <double> Pr1,Pr2,Pr3;
 
   if (!read_cam(fname1,fname2,&Pr1,&Pr2)) {
      std::cerr << "epip_interactive: error reading cam\n";
      return;
   }
 
   if (!read_cam(fname3,&Pr3)) {
      std::cerr << "epip_interactive: error reading cam\n";
      return;
   }
 
   vpgl_fundamental_matrix<double> f13(Pr1,Pr3);
   vpgl_fundamental_matrix<double> f23(Pr2,Pr3);
 
 
   // origin of board
   //  p1.set(1395,211);
   //  p2.set(1811,319);
   // 1st corner to right of orig.
   p1.set(1482,217);
   p2.set(1895,307);
 
   p3 = mw_epipolar_point_transfer(p1,p2,f13,f23);
   std::cout << "P3: " << p3.x()/p3.w() << "  " << p3.y()/p3.w() <<  std::endl;
 
 
   // Now do by least squares and compare:
 
   vnl_matrix_fixed<double,4,4> Rc;
   vnl_matrix_fixed<double,3,3> Rct;
   vnl_matrix_fixed<double,3,3> Rcs;
 
   Pr1.get_rotation_matrix().get(&Rc);
 
   std::cout << "Rotation:\n" << Rc;
 
   // transpose and reduce to 3x3:
   for (unsigned i=0; i<3; ++i) for (unsigned j=0; j<3; ++j) {
      Rcs(i,j) = Rc(i,j);
      Rct(i,j) = Rc(j,i);
   }
   std::cout << "Rotation transposed 3x3 :\n" << Rct;
 
 
   //
   // Camera 1 : write things in 3D common coordinate system
   //
 
   vgl_point_3d<double> c1_pt = Pr1.get_camera_center();
 
   mw_vector_3d 
      c1(c1_pt.x(),c1_pt.y(),c1_pt.z()),
      //: world coordinates
      e11, e12, e13, gama1, F1,
      //: camera coordinates
      e11_cam(1,0,0), e12_cam(0,1,0), e13_cam(0,0,1);
 
 
   e11 = Rct*e11_cam;
   std::cout << "E11:\n" << e11  << std::endl;
   e12 = Rct*e12_cam;
   std::cout << "E12:\n" << e12  << std::endl;
   e13 = F1 = Rct*e13_cam;
   std::cout << "E13:\n" << e13  << std::endl;
 
   double x_scale, y_scale, u, v;
 
   x_scale = Pr1.get_calibration().x_scale();
   y_scale = Pr1.get_calibration().y_scale();
 
   u = p1.x();
   v = p1.y();
 
   vgl_point_2d<double> uv0 = Pr1.get_calibration().principal_point();
 
   gama1 = ((u - uv0.x())/x_scale)*e11 + ((v - uv0.y())/y_scale)*e12  + e13;
 
   // =========
   //
   //
   
   vgl_homg_point_3d<double> paux1(gama1(0),gama1(1),gama1(2));
   vgl_homg_point_3d<double> paux2(gama1(0)+c1(0),gama1(1)+c1(1),gama1(2)+c1(2));
   vgl_homg_point_3d<double> w_origin(0,0,0);
   vgl_homg_line_3d_2_points<double> ray1(paux1,paux2);
 
 
   double d=vgl_homg_operators_3d<double>::perp_dist_squared(ray1,w_origin);
   
   // test distance of (0,0,0) to line
   std::cout << "Distance: " << d << std::endl;
 
   //
   // Camera 2 : translate things to 3D common coordinate system
   //
   vnl_matrix_fixed<double,3,3> Rct2;
   vnl_matrix_fixed<double,4,4> Rc2;
 
   Pr2.get_rotation_matrix().get(&Rc2);
 
   std::cout << "Rotation2:\n" << Rc2;
 
   // transpose and reduce to 3x3:
   for (unsigned i=0; i<3; ++i) for (unsigned j=0; j<3; ++j)
      Rct2(i,j) = Rc2(j,i);
   std::cout << "Rotation transposed 3x3  2 :\n" << Rct2;
 
   vgl_point_3d<double> c2_pt = Pr2.get_camera_center();
 
   mw_vector_3d 
      c2(c2_pt.x(),c2_pt.y(),c2_pt.z()),
      //: world coordinates
      e21, e22, e23, gama2, F2,
      //: camera coordinates
      e21_cam(1,0,0), e22_cam(0,1,0), e23_cam(0,0,1);
 
   e21 = Rct2*e21_cam;
   std::cout << "E11:\n" << e21  << std::endl;
   e22 = Rct2*e22_cam;
   std::cout << "E12:\n" << e22  << std::endl;
   e23 = F2 = Rct2*e23_cam;
   std::cout << "E13:\n" << e23  << std::endl;
 
   x_scale = Pr2.get_calibration().x_scale();
   y_scale = Pr2.get_calibration().y_scale();
   u = p2.x();
   v = p2.y();
 
 
   uv0 = Pr2.get_calibration().principal_point();
 
   gama2 = ((u - uv0.x())/x_scale)*e21 + ((v - uv0.y())/y_scale)*e22  + e23;
 
   // =========
   //
   //
   
   vgl_homg_point_3d<double> paux12(gama2(0),gama2(1),gama2(2));
   vgl_homg_point_3d<double> paux22(gama2(0)+c2(0),gama2(1)+c2(1),gama2(2)+c2(2));
   vgl_homg_line_3d_2_points<double> ray2(paux12,paux22);
 
   d=vgl_homg_operators_3d<double>::perp_dist_squared(ray2,w_origin);
   
   // test distance of (0,0,0) to line
   std::cout << "Distance 2: " << d << std::endl;
 
   std::cout << "====================================" << std::endl;
 
   std::cout << "Gama1:\n" << gama1 << "\nGama2: \n" <<  gama2 << std::endl
            << "c1:\n " << c1 << "\nc2:\n" << c2 << std::endl;
 
   // Least squares reconstr.
   vnl_matrix_fixed<double,3,2> A;
 
   for (unsigned i=0; i<3; ++i)
      A(i,0) = gama1(i);
 
   for (unsigned i=0; i<3; ++i)
      A(i,1) = -gama2(i);
 
   vnl_svd<double> svd(A);
   vnl_vector<double> lambda = svd.solve(c2-c1);
 
   std::cout << "Lambda:\n" << lambda << std::endl;
   std::cout << "Norm: " << svd.norm() << std::endl
      << "Mynorm2:" << (A*lambda +c1 - c2).two_norm() << std::endl;
 
 
   mw_vector_3d Cpt_v = c1 + lambda(0)*gama1;
   vgl_homg_point_3d<double> Cpt(Cpt_v(0), Cpt_v(1), Cpt_v(2));
   std::cout << "Reconstructed point: " << Cpt << std::endl;
 
   // Project to 3rd:
   vgl_homg_point_2d<double> p3_lsqr; 
 
   p3_lsqr = Pr3.project(Cpt);
   std::cout << "Tranferred origin to 3rd view: \n" 
      "Least squares: " << p3_lsqr.x()/p3_lsqr.w() << " " << p3_lsqr.y()/p3_lsqr.w() << std::endl
   << "Epip Transfer: " << p3.x()/p3.w() << "  " << p3.y()/p3.w() <<  std::endl;
 
   //=========== Tangents
 
 
   mw_vector_3d t1,
      T(0,1,0); //:< test 
   mw_vector_3d t1_cam;
   mw_vector_2d t1_img;
 
   t1 = T - dot_product(T, F1)*gama1;
   std::cout << "t1: " << t1 << std::endl;
 
   t1_cam = Rcs * t1; // 3rd component now 0
 
   std::cout << "t1_cam: " << t1_cam << std::endl;
   std::cout << "x,y scales: " << x_scale << " " << y_scale << std::endl;
 

   t1_img[0] = t1_cam[0]*x_scale;
   t1_img[1] = t1_cam[1]*y_scale;
   t1_img.normalize();
 
   std::cout << "t1_img: " << t1_img << std::endl;
 
   std::cout << "p1: " << p1 << std::endl;
 
   std::cout << "p1 + 88t1: \n" << p1.x()/p1.w() + 88*t1_img[0] << " " << p1.y()/p1.w() + 88*t1_img[1] << std::endl;
 
   std::cout << "================= Project point test: =======================" << std::endl;
 
   vgl_homg_point_3d<double> orig(0,0,0), other(53.3,0,0);
   vgl_homg_point_2d<double> aux;
 
   aux = Pr1.project(orig);
   std::cout << "Origin in retina 1: " << aux.x()/aux.w() << "  " <<   aux.y()/aux.w() << std::endl;
 
   aux = Pr1.project(other);
   std::cout << "Other in retina 1: " << aux.x()/aux.w() << "  " <<   aux.y()/aux.w() << std::endl;
 
 
   std::cout << "\n\n\n";
   std::cout << "================= Tangent reconstruction: =======================" << std::endl;
 
   // Camera 1:
   mw_vector_3d t1_cam_bkwd;
   mw_vector_3d t1_world_bkwd;
 
   t1_cam_bkwd[0] = t1_img[0]/x_scale;   //:< take off streching;
   t1_cam_bkwd[1] = t1_img[1]/y_scale;
   t1_cam_bkwd[2] = 0;
   t1_cam_bkwd.normalize();
 
   t1_world_bkwd = Rct*t1_cam_bkwd;
 
   // Camera 2:
   mw_vector_3d t2_cam_bkwd;
   mw_vector_3d t2_world_bkwd;
   mw_vector_3d T_rec;
 
   t2_cam_bkwd[0] = (1982 - p2.x())/x_scale;   //:< determined by the eye
   t2_cam_bkwd[1] = (295 - p2.y())/y_scale;
   t2_cam_bkwd[2] = 0;
   t2_cam_bkwd.normalize();
 
   t2_world_bkwd = Rct2*t2_cam_bkwd;
 
   std::cout << "Test t1 dot F1 zero: " << dot_product(t1_world_bkwd,F1) << std::endl << std::endl;
   std::cout << "Test t1 dot F2 zero: " << dot_product(t2_world_bkwd,F2) << std::endl << std::endl;
 
   T_rec = vnl_cross_3d( vnl_cross_3d(t1_world_bkwd,gama1), vnl_cross_3d(t2_world_bkwd,gama2) );
 
   std::cout << "T reconstructed: \n" << T_rec << std::endl;
}


// projects a parametric curve into a cam
void
example_project()
{
   // Sample 3D circle
   double t,theta, step=1;
   
   double sz=360/step;
   std::vector< vgl_homg_point_3d<double> > Ps;
   Ps.reserve((int) sz);
      
   for (theta=0; theta<360-step; theta+=step) {
      t = (theta/180.0)*PI;
      vgl_homg_point_3d<double> pt(4*cos(t),4*sin(t),2000);
      Ps.push_back(pt);       
   }

   // project then output
   vnl_double_3x3 m;
   m[0][0]=1000;m[0][1]=0;   m[0][2]=0;
   m[1][0]=0;   m[1][1]=1000;m[1][2]=0;
   m[2][0]=0;   m[2][1]=0;   m[2][2]=1;
 
   vpgl_calibration_matrix<double> K(m);

   vgl_h_matrix_3d<double> R;
   R.set_identity();
   vgl_homg_point_3d<double> center(0,0,0.0);
   vpgl_perspective_camera<double> P(K, center, R);

   for (int i=0; i<Ps.size(); ++i) {
      vgl_point_2d<double> gama = P.project(Ps[i]);
      std::cout << gama << std::endl;
   }


   std::cout << "Done!" << std::endl;
}

// Calls process on a filename, without dialog
void 
call_show_contours_process(char *fname)
{
   bpro1_process_sptr pro= MANAGER->process_manager()->get_process_by_name("Show Contours");

   std::set<bpro1_storage_sptr> modified;

   // set the parameter
   std::string sfname(fname);
   bpro1_filepath fp(sfname);
   std::string pname("-image_filename");
   pro->parameters()->set_value(pname,fp);
   std::cout << sfname << std::endl;

   std::vector<std::string> out_name(pro->output_names());
   out_name.push_back("image0");
   pro->set_output_names(out_name);

   //now run the process
   MANAGER->process_manager()->run_process_on_current_frame(pro, &modified);

   // update the display for any modified storage objects
   for ( std::set<bpro1_storage_sptr>::iterator itr = modified.begin();
         itr != modified.end(); ++itr ) {
     bvis1_manager::instance()->add_to_display(*itr);
   }

   bvis1_manager::instance()->display_current_frame();
}



void
test_point_reconstruct_rig()
{
 
   std::string fname1("curr/p1010049.jpg");
   std::string fname2("curr/p1010053.jpg");

   mw_rig rig;
   rig.read(fname1,fname2);
   
   // origin of board
   //  p1.set(1395,211);
   //  p2.set(1811,319);
   // 1st corner to right of orig.
   vgl_homg_point_2d<double> p1,p2;

   p1.set(1482,217);
   p2.set(1895,307);

   mw_vector_3d gama1, gama2;

   rig.get_gama1(p1.x(),p1.y(),&gama1);
   rig.get_gama2(p2.x(),p2.y(),&gama2);

   
   // Least squares reconstr
   mw_vector_3d Cpt_v;
   rig.reconstruct_point_lsqr(gama1,gama2,&Cpt_v);

   std::cout << "Reconstructed point: " << Cpt_v << std::endl;


   // Forward tangent projection


   mw_vector_3d t1,
      T(0,1,0); //:< test 

   mw_vector_2d t1_img;

   rig.project_t1(T,gama1,&t1);
   
   double x,y;
   rig.world_to_img1_vector(t1, &t1_img[0],&t1_img[1]);

   std::cout << "t1_img: " << t1_img << std::endl;
 
   std::cout << "p1: " << p1 << std::endl;
 
   std::cout << "p1 + 88t1: \n" << p1.x()/p1.w() + 88*t1_img[0] << " " << p1.y()/p1.w() + 88*t1_img[1] << std::endl;
   
   /*


   // Tangent reconstruction
   vgl_vector_2d<double> 
      t2(1982 - p2.x(), 295 - p2.y());

   mw_vector_3d T_rec;

   rig.reconstruct_tangent(gama1,gama2,t1,t2, &T_rec);

   std::cout << "T reconstructed: \n" << T_rec << std::endl;
   */
}
