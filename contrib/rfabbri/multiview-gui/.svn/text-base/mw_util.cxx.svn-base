#include "mw_util.h"

#include <vsol/vsol_point_2d.h>
#include <vcl_cstring.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

//:  Loads one sequence of points from a .con file into a vector of points
// \param[out] points:  reference to user-provided vector; it will be resized to
// contain vsol_point_2d smart pointers.
//
// \return true on success
//
//TODO: put this in a file loader library
bool
load_con_file(
      vcl_string filename, 
      vcl_vector<vsol_point_2d_sptr> &points, 
      bool *is_open)
{
  vcl_ifstream infp(filename.c_str(), vcl_ios::in);

  if (!infp) {
    vcl_cout << " Error opening file  " << filename << vcl_endl;
    return false;
  }

  //
  // Parse header
  //
  char lineBuffer[2000];
  infp.getline(lineBuffer,2000);
  if (vcl_strncmp(lineBuffer,"CONTOUR",7)) {
    vcl_cerr << "Invalid File " << filename << vcl_endl
             << "Should be CONTOUR " << lineBuffer << vcl_endl;
    return false;
  }

  char openFlag[2000];
  infp.getline(openFlag,2000);
  if (!vcl_strncmp(openFlag,"OPEN",4))
    *is_open = true;
  else if (!vcl_strncmp(openFlag,"CLOSE",5))
    *is_open = false;
  else{
    vcl_cerr << "Invalid File " << filename << vcl_endl
             << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    return false;
  }

  // Read points
  int i, numOfPoints;
  infp >> numOfPoints;
  points.reserve(numOfPoints);
  double x,y;
  for (i=0;i<numOfPoints;i++) {
    infp >> x >> y;
    vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
    points.push_back(newPt);
  }

  //close file
  infp.close();

  return true;
}


//: given image filename, return vector of filenames of the corresponding con
// files named after a convention that they share the same prefix as the image
// filename
// OBS: this only works on unix systems
bool
con_filenames(vcl_string image_fname,vcl_vector<vcl_string> &con_fnames)
{

   vcl_string cmdline("confiles ");
   cmdline.append(image_fname);
   if (system(cmdline.c_str()) == -1) {
      vcl_cerr << "error trying to exec helper script confiles" << vcl_endl;
      return false;
   }

   // output of script:
   vcl_string con_filenames="/tmp/lemsvxl_mw_confiles";
   vcl_ifstream infp(con_filenames.c_str(), vcl_ios::in);
   if (!infp) {
     vcl_cout << " Error opening output of script, file  " << con_filenames << vcl_endl;
     return false;
   }

   vcl_string cfname;
   while (!infp.eof()) {
      char buf[1024];
      infp.getline(buf,1024);
      if (*buf != '\0')
         con_fnames.push_back(buf);
   }

#ifndef NDEBUG
   if (con_fnames.size()==0)
      vcl_cerr << "Warning: no contour files for image " << image_fname << vcl_endl;
#endif
   return true;
}



//: returns directory of the file name, as well as the prefix, which is the
//common prefix to all filenames of data relating to the file
bool
mw_get_prefix(vcl_string img_name, vcl_string *dir, vcl_string *prefix)
{
   vcl_cout << "Parsing image name: " << img_name << vcl_endl;

   vcl_string cmd("mw_basename ");
   cmd += img_name;
   if (system(cmd.c_str()) == -1)
      return false;

   vcl_string path_fname="/tmp/lemsvxl_mw_bname";
   vcl_ifstream infp(path_fname.c_str(), vcl_ios::in);
   if (!infp) {
     vcl_cout << " Error opening file  " << path_fname << vcl_endl;
     return false;
   }

   char buf[1024];
   infp.getline(buf,1024);
   *dir = buf;
   vcl_cout << "Directory: " << *dir << vcl_endl;
   infp.getline(buf,1024);
   *prefix = buf;
   vcl_cout << "Prefix: " << *prefix << vcl_endl;

   return true;
}

//: given file prefix (with path) this function
// reads the extrinsic calibration from an ascii file into R and t
static bool
read_extrinsic(
      vcl_string prefix_path_name, 
      vgl_h_matrix_3d<double> *Rout,
      vgl_homg_point_3d<double> *center_out)
{
   vcl_cout << "Reading extrinsic params\n\n";
   prefix_path_name.append(".extrinsic");
   vcl_ifstream infp_ext(prefix_path_name.c_str(), vcl_ios::in);
   if (!infp_ext) {
     vcl_cerr << " Error opening file  " << prefix_path_name << vcl_endl;
     return false;
   }

   vnl_matrix_fixed<double,3,3> mm;
   mm.read_ascii(infp_ext);

   vnl_vector_fixed<double,3> t_dummy;
   t_dummy[0] = 0;
   t_dummy[1] = 0;
   t_dummy[2] = 0;
   vgl_h_matrix_3d<double> R(mm,t_dummy);
   *Rout = R;

//   vcl_cout << "Rotation\n";
//   vcl_cout << *Rout << vcl_endl << vcl_endl;

   infp_ext >> t_dummy;
   vgl_homg_point_3d<double> center(t_dummy[0],t_dummy[1],t_dummy[2]);

   *center_out = center;

//   vcl_cout << "Center:\n";
//   vcl_cout << t_dummy << vcl_endl;
}

static bool
read_extrinsic(
      vcl_string prefix_path_name, 
      vgl_h_matrix_3d<double> *Rout,
      vgl_homg_point_3d<double> *center_out);

// reads intrinsic and extr. camera info from matlab calibration toolbox data
bool
read_cam(vcl_string img_name1, vcl_string img_name2, vpgl_perspective_camera <double> *P1out,
      vpgl_perspective_camera <double> *P2out)
{
   bool stat;

   vcl_string dir, pref;
   if (!mw_get_prefix(img_name1,&dir,&pref)) {
      vcl_cerr << "Error in read_cam\n";
      return false;
   }

   // Intrinsic params
   vcl_string cam_fname(dir+"/calib.intrinsic");
   vcl_ifstream infp(cam_fname.c_str(), vcl_ios::in);
   if (!infp) {
     vcl_cerr << " Error opening file  " << cam_fname << vcl_endl;
     return false;
   }

   vnl_double_3x3 m;
   m.read_ascii(infp);
   vpgl_calibration_matrix<double> K(m);


   // Extrinsic params
   
   // File 1
   vgl_h_matrix_3d<double> R;
   vgl_homg_point_3d<double> center;
   stat = read_extrinsic(pref, &R, &center);
      if (!stat) {
         vcl_cerr << "error while reading extrinsic" << vcl_endl;
         return false;
      }
   vpgl_perspective_camera<double> P1(K, center, R);
   *P1out = P1;

   // File 2
   if (!mw_get_prefix(img_name2,&dir,&pref)) {
      vcl_cerr << "Error in read_cam\n";
      return false;
   }

   vgl_h_matrix_3d<double> R2;
   vgl_homg_point_3d<double> center2;
   stat = read_extrinsic(pref, &R2, &center2);
      if (!stat) {
         vcl_cerr << "error while reading extrinsic" << vcl_endl;
         return false;
      }

   vpgl_perspective_camera<double> P2(K, center2, R2);
   *P2out = P2;

   return true;
}

// reads intrinsic and extr. camera info from matlab calibration toolbox data
bool
read_cam( vcl_string img_name1, 
      vpgl_perspective_camera <double> *P1out)
{
   bool stat;

   vcl_string dir, pref;
   if (!mw_get_prefix(img_name1,&dir,&pref)) {
      vcl_cerr << "Error in read_cam\n";
      return false;
   }

   // Intrinsic params
   vcl_string cam_fname(dir+"/calib.intrinsic");
   vcl_ifstream infp(cam_fname.c_str(), vcl_ios::in);
   if (!infp) {
     vcl_cerr << " Error opening file  " << cam_fname << vcl_endl;
     return false;
   }

   vnl_double_3x3 m;
   m.read_ascii(infp);
   vpgl_calibration_matrix<double> K(m);


   // Extrinsic params
   
   // File 1
   vgl_h_matrix_3d<double> R;
   vgl_homg_point_3d<double> center;
   stat = read_extrinsic(pref, &R, &center);
      if (!stat) {
         vcl_cerr << "error while reading extrinsic" << vcl_endl;
         return false;
      }
   vpgl_perspective_camera<double> P1(K, center, R);
   *P1out = P1;

   return true;
}


//: transfers a point to a 3rd image given p1,p1,F13 and F23. It will intersect
// the epipolar lines l13 and l23 on the 3rd image, returning p3.
// \todo move to dvpgl/algo
vgl_homg_point_2d<double>
mw_epipolar_point_transfer( 
      const vgl_homg_point_2d<double> &p1, 
      const vgl_homg_point_2d<double> &p2, 
      const vpgl_fundamental_matrix<double> &f13,
      const vpgl_fundamental_matrix<double> &f23)
{
   vgl_homg_point_2d<double> p3; 
   vgl_homg_line_2d<double> l13,l23;

   l13 = f13.image2_epipolar_line(p1);
   l23 = f23.image2_epipolar_line(p2);

   p3 = vgl_homg_operators_2d<double>::intersection(l13,l23);

   return p3;
}
/*
bool
reconstruct_pt_tangents(
      // Input:
      dbsol_interp_curve_2d *curve1, 
      dbsol_interp_curve_2d *curve2,
      const vnl_matrix <double> &s,  // nx2 w/ pairs of arc lengths of pts to reconstruct
      const vpgl_perspective_camera <double> &Pr1,
      const vpgl_perspective_camera <double> &Pr2,
      // Output: 
      vcl_vector<vsol_point_2d_sptr> *Gamas,
      vcl_vector<double> *error, // distance btw two rays in 3D
      vcl_vector<vgl_vector_3d<double>> *Ts
      )
      */

/* XXX Doing mw_rig 
bool
reconstruct_pt_tangents(
      // Input:
      dbsol_interp_curve_2d *curve1, 
      dbsol_interp_curve_2d *curve2,
      double s1p, double s2p, double s1, double s2,
      const vpgl_perspective_camera <double> &Pr1,
      const vpgl_perspective_camera <double> &Pr2,
      // Output (caller's storage; we do not alloc em here): 
      double vnl_vector_fixed<double,3> *Gama_s,
      double vnl_vector_fixed<double,3> *Gama_sp,
      vcl_vector<vgl_vector_3d<double>> *T_s,
      vcl_vector<vgl_vector_3d<double>> *T_sp,
      double *error_s1, // distance btw two rays in 3D
      double *error_s2, // distance btw two rays in 3D
      )
{
   vnl_matrix_fixed<double,4,4> Rc1;
   vnl_matrix_fixed<double,3,3> Rct1, Rct2;

   Pr1.get_rotation_matrix().get(&Rc1);
   Pr2.get_rotation_matrix().get(&Rc2);

   // transpose and reduce to 3x3:
   for (unsigned ii=0; ii<3; ++ii) for (unsigned j=0; j<3; ++j) {
      Rct1(ii,j) = Rc1(j,ii);
      Rct2(ii,j) = Rc2(j,ii);
   }
   
   // Write things in 3D world coordinate system
   vgl_point_3d<double> c1_pt = Pr1.get_camera_center();
   
   vnl_vector_fixed<double,3> 
      c1(c1_pt.x(),c1_pt.y(),c1_pt.z()),
      e11, e12, e13, gama1, F1,   //:< world coordinates
      e11_cam(1,0,0), e12_cam(0,1,0), e13_cam(0,0,1); //:< camera coordinates

   e11 = Rct*e11_cam;
   e12 = Rct*e12_cam;
   e13 = F1 = Rct*e13_cam;
   
   double x_scale, y_scale, u, v;
   
   x_scale = Pr1.get_calibration().x_scale();
   y_scale = Pr1.get_calibration().y_scale();



   unsigned i;
   for (i=0; i < s.rows(); ++i) {

      
      vsol_point_2d p1 = curve1.point_at(s(i,1));
      vsol_point_2d p2 = curve1.point_at(s(i,2));

      u = p1.x();
      v = p1.y();
      
      vgl_point_2d<double> uv0 = Pr1.get_calibration().principal_point();
      
      gama1 = ((u - uv0.x())/x_scale)*e11 + ((v - uv0.y())/y_scale)*e12  + e13;

      // Camera 2
      vgl_point_3d<double> c2_pt = Pr2.get_camera_center();
      
      vnl_vector_fixed<double,3> 
         c2(c2_pt.x(),c2_pt.y(),c2_pt.z()), //:< world coordinates
         e21, e22, e23, gama2, F2, //:< camera coordinates
         e21_cam(1,0,0), e22_cam(0,1,0), e23_cam(0,0,1);
      
      e21 = Rct2*e21_cam;
      e22 = Rct2*e22_cam;
      e23 = F2 = Rct2*e23_cam;
      
      x_scale = Pr2.get_calibration().x_scale();
      y_scale = Pr2.get_calibration().y_scale();
      u = p2.x();
      v = p2.y();
      
      uv0 = Pr2.get_calibration().principal_point();
      
      gama2 = ((u - uv0.x())/x_scale)*e21 + ((v - uv0.y())/y_scale)*e22  + e23;

      // At this pt we have, for each camera:  gama, F, bases of camera system
      // written in world coordinates, and 

      // Least squares pt reconstr.
      vnl_matrix_fixed<double,3,2> A;
    
      for (unsigned i=0; i<3; ++i)
         A(i,0) = gama1(i);
    
      for (unsigned i=0; i<3; ++i)
         A(i,1) = -gama2(i);
    
      vnl_svd<double> svd(A);
      vnl_vector<double> lambda = svd.solve(c2-c1);
    
      vcl_cout << "Lambda:\n" << lambda << vcl_endl;
      // the error is:   (A*lambda +c1 - c2).two_norm()
    
      vnl_vector_fixed<double,3> Cpt_v = c1 + lambda(0)*gama1;
      vgl_homg_point_3d<double> Cpt(Cpt_v(0), Cpt_v(1), Cpt_v(2));
      vcl_cout << "Reconstructed point: " << Cpt << vcl_endl;

     //=========== Tangents
      vcl_cout << "\n\n\n";
      vcl_cout << "================= Tangent reconstruction: =======================" << vcl_endl;
    
      // Camera 1:
      vnl_vector_fixed<double,3> t1_cam_bkwd;
      vnl_vector_fixed<double,3> t1_world_bkwd;
    
      t1_cam_bkwd[0] = t1_img[0]/x_scale;   //:< take off streching;
      t1_cam_bkwd[1] = t1_img[1]/y_scale;
      t1_cam_bkwd[2] = 0;
      t1_cam_bkwd.normalize();
    
      t1_world_bkwd = Rct*t1_cam_bkwd;
    
      // Camera 2:
      vnl_vector_fixed<double,3> t2_cam_bkwd;
      vnl_vector_fixed<double,3> t2_world_bkwd;
      vnl_vector_fixed<double,3> T_rec;
    
      t1_cam_bkwd[0] = t2_img[0]/x_scale;   //:< take off streching;
      t1_cam_bkwd[1] = t2_img[1]/y_scale;
      t2_cam_bkwd[2] = 0;
      t2_cam_bkwd.normalize();
    
      t2_world_bkwd = Rct2*t2_cam_bkwd;
    
      vcl_cout << "Test t1 dot F1 zero: " << dot_product(t1_world_bkwd,F1) << vcl_endl << vcl_endl;
      vcl_cout << "Test t2 dot F2 zero: " << dot_product(t2_world_bkwd,F2) << vcl_endl << vcl_endl;
    
      T_rec = vnl_cross_3d( vnl_cross_3d(t1_world_bkwd,gama1), vnl_cross_3d(t2_world_bkwd,gama2) );

   }

   return true;
}
*/
