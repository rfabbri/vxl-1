#include <cali/cali_artf_projector.h>
#include <cali/cali_cylinder_artifact.h>
#include <vcl_cmath.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_sphere_3d.h>
#include <vil/vil_memory_image.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_det.h>
#include <vil/vil_new.h>


cali_artf_projector::cali_artf_projector(cali_artf_model* artifact)
:artifact_(artifact){
    //vcl_string txt_file = "C:\\test_images\\gen_calib\\centers.txt";
   // vcl_string txt_file = cali_filepaths::CENTERS;
    vcl_string txt_file = CENTERS;
    fstream.open(txt_file.c_str());
}

cali_artf_projector::cali_artf_projector(cali_artf_model* artifact,cali_param par)
:artifact_(artifact),par_(par){
    //vcl_string txt_file = "C:\\test_images\\gen_calib\\centers.txt";
   // vcl_string txt_file = cali_filepaths::CENTERS;
    vcl_string txt_file = par_.CENTERS;
   // fstream.open(txt_file.c_str());
    fstream.open(txt_file.c_str(),vcl_ofstream::out | vcl_ofstream::app);

    /*this->fstream.open (par_.DIFF.c_str(), 
    vcl_ofstream::out | vcl_ofstream::app);*/
}

//: generates backprojected rays throughout the bounded image and finds the intersection 
// with the artifact. If there is an intersection, fills that pixel with a value based on 
// the artifact density at  that point
vil_image_resource_sptr
cali_artf_projector::build_projection_img(xmvg_perspective_camera<double> camera, 
                                         dbil_bounded_image_view<unsigned char> & view) {
  unsigned int dim_y = view.jb0() + view.nj();
  unsigned int dim_x = view.ib0() + view.ni();

  // the resulting vil_image_view with the filled values
  vil_image_resource_sptr res= vil_new_image_resource(view.nib(), view.njb(), 1, VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<unsigned char> img = *(res->get_copy_view());
  img.fill((unsigned char)255);
  
  for (unsigned int j=view.jb0(); j<dim_y; j++){
    for (unsigned int i=view.ib0(); i<dim_x; i++){
      // compute the pixel value at this point
      vgl_homg_line_3d_2_points<double> line = camera.xmvg_backproject(vgl_homg_point_2d<double>(double(i), double(j), 1.0));

      // get the length of the intersection, i.e. the length of the line inside sphere
      double length = artifact_->ray_artifact_intersect(line);
      //unsigned char& pix = view.gpix(i, j);
     // unsigned char& pix = img(i, j);
      if (length > 0)   // intersects
        // compute the pixel intensity
        img(i,j) = (unsigned char)(255.0 * exp(-1.0*length*artifact_->density()));
      else 
        img(i,j) = 255; // white, no intersection
      
        
    }
  }
  res->put_view(img);

  return res;
}

//: creates an image showing only the centers of the balls as small rectangles
vil_image_resource_sptr
cali_artf_projector::build_centers_projection_img(xmvg_perspective_camera<double> camera, 
                                         dbil_bounded_image_view<unsigned char> &view) 
{
  unsigned int dim_y = view.jb0() + view.nj();
  unsigned int dim_x = view.ib0() + view.ni();
  fstream << "------------------------------" << "\n";
  // the resulting vil_image_view with the filled values
  //vil_image_view<unsigned char> img(view.nib(), view.njb());
  vil_image_resource_sptr res= vil_new_image_resource(view.nib(), view.njb(), 1, VIL_PIXEL_FORMAT_BYTE);
  vil_image_view<unsigned char> img = *(res->get_copy_view());
  vcl_vector<vsol_conic_2d >  centers = build_ball_projections(camera);
  for (unsigned int i=0; i < centers.size(); i++){
     
      //unsigned char& pix = view.gpix(i, j);
    unsigned int x = (int) centers[i].centre().x();
    unsigned int y = (int) centers[i].centre().y();
    if ((x > 0) && (x < dim_x) && (y > 0) && (y < dim_y)) {
    fstream << i << " X=" << x << " Y=" << y << "\n";

    // create a box of black area
    for (unsigned int a = x-10; a <= x+10; a++)
      for (unsigned int b = y-10; b <= y+10; b++){
        unsigned char& pix = img(a, b);
        pix = 0; // 
      }
    }
  }
  res->put_view(img);
  return res;
}

//: creates a vector of conics, as the projections of the given spheres on the 
// image plane with the given paramaters
vcl_vector<vsol_conic_2d> 
cali_artf_projector::build_ball_projections(xmvg_perspective_camera<double> camera)
{
  vcl_vector<vgl_point_3d<double> > balls = artifact_->ball_centers();
  vgl_homg_point_2d<double> center_proj;
  vcl_vector<vsol_conic_2d > result_set;
  vcl_vector<double>radius = artifact_->ball_radii();
//  vcl_vector<double>radius(cali_cylinder_artifact::BALL_NUMBER);
 // vcl_vector<double>radius;
 // double radius;
  double a, b;
 
  


 unsigned int k = 0;
  for (unsigned int i=0; i<balls.size(); i++) {
     
     vgl_homg_point_3d<double> center(balls[i].x(), balls[i].y(), balls[i].z(), 1.0);
   //  vcl_cout<<" ball centres: "<<" x "<<balls[i].x()<<" y "<<balls[i].y()<<" z "<<balls[i].z()<<vcl_endl<<vcl_endl;

     center_proj = camera.project(center);
     vgl_point_3d<double> point(center_proj.x()/center_proj.w(), 
       center_proj.y()/center_proj.w(), 0);
     /*
     if ((i==0) || (i == cali_cylinder_artifact::BALL_NUMBER-1)) {
       radius = cali_cylinder_artifact::ball_radius_big;
     } else {
       radius = cali_cylinder_artifact::ball_radius_small;
     }

     
     */
  /*vgl_point_2d<double>cent = project_sphere_image(camera, center, radius[i], a, b); 
  vsol_conic_2d ellipse(vsol_point_2d (cent.x(), cent.y()), a, b, 0.0);
  fstream << ++k <<" x " <<cent.x() <<" y " <<cent.y() <<" a "<<a<<" b "<<b<<vcl_endl;*/
     project_sphere(camera, center, radius[i], a, b);

     vsol_conic_2d ellipse(vsol_point_2d (point.x(), point.y()), a, b, 0.0);
fstream << ++k <<" x " <<point.x() <<" y " <<point.y() <<" a "<<a<<" b "<<b<<vcl_endl;

     //    vcl_cout<<" x " <<point.x() <<" y " <<point.y() <<" a "<<a<<" b "<<b<<vcl_endl<<vcl_endl;

     result_set.push_back(ellipse);
  }
  return result_set;
}

//: computes the width, height (a,b) of the conic, when a sphere is
// projected onto the image plane with a given camera and also returns the center of the conic
// from the contour projection




vgl_point_2d<double>
cali_artf_projector::project_sphere_image(xmvg_perspective_camera<double> camera, 
                                    vgl_homg_point_3d<double> center,
                                    double radius,
                                    double &a, double &b) 
{
  vnl_matrix_fixed<double, 3, 4> P = camera.get_matrix();
  vnl_matrix_fixed<double, 4, 3> P_transp = P.transpose();
  vnl_matrix_fixed<double, 4, 4> S;
  S.set_identity();
  S.put(0,3,-1*center.x());
  S.put(1,3,-1*center.y());
  S.put(2,3,-1*center.z());
  S.put(3,0,-1*center.x());
  S.put(3,1,-1*center.y());
  S.put(3,2,-1*center.z());

  double a10 = center.x()*center.x() + 
       center.y()*center.y() + 
       center.z()*center.z() - 
       radius*radius;

  S.put(3,3,a10);
  double det = vnl_det(S);
  if (det == 0) {
    vcl_cout << "Determinant is 0" << vcl_endl;
  }

  vnl_matrix<double> Sinv = vnl_matrix_inverse<double>(S);
  vnl_double_3x3 C(P*Sinv*P_transp);
  vnl_matrix<double> Cinv = vnl_matrix_inverse<double>(C);
      
  //get C11
  vnl_matrix_fixed<double, 2, 2> C11;
  C11.put(0,0, Cinv.get(0,0));
  C11.put(0,1, Cinv.get(0,1));
  C11.put(1,0, Cinv.get(1,0));
  C11.put(1,1, Cinv.get(1,1));

  vnl_matrix_fixed<double, 2, 1> C12;
  C12.put(0,0, Cinv.get(0,2));
  C12.put(1,0, Cinv.get(1,2));
     
  vnl_symmetric_eigensystem<double> eign_solver(C11);
  vnl_matrix<double> R;
  vnl_vector<double> D;
  vnl_symmetric_eigensystem_compute(C11, R, D);
                                       
  vnl_matrix<double> C11inv = vnl_matrix_inverse<double>(C11);
  vnl_matrix<double> T = C11inv*C12;
  T *= -1;

  vnl_matrix_fixed<double, 3, 3> E;
  E.set_identity();
  E.put(0,0,R.get(0,0));
  E.put(0,1,R.get(0,1));
  E.put(1,0,R.get(1,0));
  E.put(1,1,R.get(1,1));
  E.put(0,2,T(0,0));
  E.put(1,2,T(1,0));
  vnl_matrix<double> E_transp = E.transpose();
  vnl_matrix_fixed<double, 3, 3> result = E_transp*Cinv*E;
  
  double x = -1/(result(0,0)/result(2,2));
  if (x < 0)
    a = 0;
  else
    a = sqrt(x);

  x = -1/(result(1,1)/result(2,2));
  if (x < 0)
    b = 0;
  else
    b = sqrt(x);

  double a11 = Cinv.get(0,0);
  double a12 = Cinv.get(1,0)/2;
  double a13 = Cinv.get(2,0)/2;
  double a22 = Cinv.get(1,1);
  double a23 = Cinv.get(1,2)/2;
  //unused variable
  //double a33 = Cinv.get(2,2);

  double cx = 2*(a12*a23 - a22*a13)/(a11*a22 - a12*a12);
  double cy = 2*(a13*a12 - a23*a11)/(a11*a22 - a12*a12);
  vgl_point_2d <double>ct(cx,cy);
  return ct;

}



void
cali_artf_projector::project_sphere(xmvg_perspective_camera<double> camera, 
                                    vgl_homg_point_3d<double> center,
                                    double radius,
                                    double &a, double &b) 
{
  vnl_matrix_fixed<double, 3, 4> P = camera.get_matrix();
  vnl_matrix_fixed<double, 4, 3> P_transp = P.transpose();
  vnl_matrix_fixed<double, 4, 4> S;
  S.set_identity();
  S.put(0,3,-1*center.x());
  S.put(1,3,-1*center.y());
  S.put(2,3,-1*center.z());
  S.put(3,0,-1*center.x());
  S.put(3,1,-1*center.y());
  S.put(3,2,-1*center.z());

  double a10 = center.x()*center.x() + 
       center.y()*center.y() + 
       center.z()*center.z() - 
       radius*radius;

  S.put(3,3,a10);
  double det = vnl_det(S);
  if (det == 0) {
    vcl_cout << "Determinant is 0" << vcl_endl;
  }

  vnl_matrix<double> Sinv = vnl_matrix_inverse<double>(S);
  vnl_double_3x3 C(P*Sinv*P_transp);
  vnl_matrix<double> Cinv = vnl_matrix_inverse<double>(C);
      
  //get C11
  vnl_matrix_fixed<double, 2, 2> C11;
  C11.put(0,0, Cinv.get(0,0));
  C11.put(0,1, Cinv.get(0,1));
  C11.put(1,0, Cinv.get(1,0));
  C11.put(1,1, Cinv.get(1,1));

  vnl_matrix_fixed<double, 2, 1> C12;
  C12.put(0,0, Cinv.get(0,2));
  C12.put(1,0, Cinv.get(1,2));
     
  vnl_symmetric_eigensystem<double> eign_solver(C11);
  vnl_matrix<double> R;
  vnl_vector<double> D;
  vnl_symmetric_eigensystem_compute(C11, R, D);
                                       
  vnl_matrix<double> C11inv = vnl_matrix_inverse<double>(C11);
  vnl_matrix<double> T = C11inv*C12;
  T *= -1;

  vnl_matrix_fixed<double, 3, 3> E;
  E.set_identity();
  E.put(0,0,R.get(0,0));
  E.put(0,1,R.get(0,1));
  E.put(1,0,R.get(1,0));
  E.put(1,1,R.get(1,1));
  E.put(0,2,T(0,0));
  E.put(1,2,T(1,0));
  vnl_matrix<double> E_transp = E.transpose();
  vnl_matrix_fixed<double, 3, 3> result = E_transp*Cinv*E;
  
  double x = -1/(result(0,0)/result(2,2));
  if (x < 0)
    a = 0;
  else
    a = sqrt(x);

  x = -1/(result(1,1)/result(2,2));
  if (x < 0)
    b = 0;
  else
    b = sqrt(x);

}

