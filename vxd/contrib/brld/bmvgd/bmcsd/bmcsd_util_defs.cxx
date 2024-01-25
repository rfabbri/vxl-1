#include "bmcsd_util.h"
#include <cstring>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <vul/vul_file.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vsol/vsol_point_2d.h>

int n8[8][2] = {
  { 0,  1}, // 0
  {-1,  1}, // 1
  {-1,  0}, // 2
  {-1, -1}, // 3
  { 0, -1}, // 4
  { 1, -1}, // 5
  { 1,  0}, // 6
  { 1,  1}  // 7
};

/* 4-neighborhood coordinates
    -1  0  1
     _______ 
 -1 |   2   |
  0 |1  *  3|
  1 |   0   |
     ------- 
*/
int n4[4][2] = {
   { 0,  1}, // 0
   {-1,  0}, // 1
   { 0, -1}, // 2
   { 1,  0}  // 3
};


static bool
bmcsd_read_offset(std::string noext,vnl_double_3x3 *m);

static bool
bmcsd_read_scaling(std::string noext,vnl_double_3x3 *m);

//:  Loads one sequence of points from a .con file into a vector of points
// \param[out] points:  reference to user-provided vector; it will be resized to
// contain vsol_point_2d smart pointers.
//
// \return true on success
//
//TODO: put this in a file loader library
bool
load_con_file(
      std::string filename, 
      std::vector<vsol_point_2d_sptr> &points, 
      bool *is_open)
{
  std::ifstream infp(filename.c_str(), std::ios::in);

  if (!infp) {
    std::cout << " Error opening file  " << filename << std::endl;
    return false;
  }

  //
  // Parse header
  //
  char lineBuffer[2000];
  infp.getline(lineBuffer,2000);
  if (std::strncmp(lineBuffer,"CONTOUR",7)) {
    std::cerr << "Invalid File " << filename << std::endl
             << "Should be CONTOUR " << lineBuffer << std::endl;
    return false;
  }

  char openFlag[2000];
  infp.getline(openFlag,2000);
  if (!std::strncmp(openFlag,"OPEN",4))
    *is_open = true;
  else if (!std::strncmp(openFlag,"CLOSE",5))
    *is_open = false;
  else{
    std::cerr << "Invalid File " << filename << std::endl
             << "Should be OPEN/CLOSE " << openFlag << std::endl;
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
con_filenames(std::string image_fname,std::vector<std::string> &con_fnames)
{

   std::string cmdline("/home/rfabbri/cprg/vxlprg/lemsvxlsrc/contrib/bmvgd/bmcsd/scripts/confiles ");
   cmdline.append(image_fname);
   if (system(cmdline.c_str()) == -1) {
      std::cerr << "error trying to exec helper script confiles" << std::endl;
      return false;
   }

   // output of script:
   std::string con_filenames="/tmp/lemsvxl_bmcsd_confiles";
   std::ifstream infp(con_filenames.c_str(), std::ios::in);
   if (!infp) {
     std::cout << " Error opening output of script, file  " << con_filenames << std::endl;
     return false;
   }

   std::string cfname;
   while (!infp.eof()) {
      char buf[1024];
      infp.getline(buf,1024);
      if (*buf != '\0')
         con_fnames.push_back(buf);
   }

#ifndef NDEBUG
   if (con_fnames.size()==0)
      std::cerr << "Warning: no contour files for image " << image_fname << std::endl;
#endif
   return true;
}


//: read .scale file and calibration matrix m
//
// Scaling file contains new:old 1D aspect ratio, where (new) is separated from
// (old) by a space, and both are integers.
//
// \return true if no bad errors; false if any bad errors. (this has nothing to
// do with the existence of a .scale file)
bool
bmcsd_read_scaling(std::string noext,vnl_double_3x3 *m)
{
   std::ifstream infp((noext+".scale").c_str(), std::ios::in);
   if (!infp) {
     std::cout << "No scaling file " << noext+".scale"<< std::endl;
   } else {
     // Scaling file contains new:old 1D aspect ratio
     char buf[1024];
     infp.getline(buf,1024);
     infp.close();
     std::istringstream strs(buf);

     double new_dim=-1, old_dim=-1;
     strs >> new_dim;
     strs >> old_dim;
     std::cout << "Scalings: " << new_dim << " " << old_dim<< std::endl;
     (*m) = (new_dim/old_dim) * (*m);
     (*m)(2,2) = 1;
   }

   return true;
}

//: read .offset file and updates principal point in calibration matrix m
// \return true if no bad errors; false if any bad errors. (this has nothing to
// do with the existence of a .origin file)
bool
bmcsd_read_offset(std::string noext,vnl_double_3x3 *m)
{
   std::ifstream infp((noext+".origin").c_str(), std::ios::in);
   if (!infp) {
     std::cout << "No offset file " << noext+".origin"<< std::endl;
   } else {
     char buf[1024];
     infp.getline(buf,1024);
     infp.close();
     std::istringstream strs(buf);

     double col_offset, row_offset;
     strs >> col_offset;
     strs >> row_offset;
     std::cout << "OFFSETS: " << col_offset << " " << row_offset << std::endl;
     (*m)(0,2) -= col_offset;
     (*m)(1,2) -= row_offset;
   }

   return true;
}

//: returns directory of the file name, as well as the prefix, which is the
//common prefix to all filenames of data relating to the file
bool
bmcsd_get_prefix(std::string img_name, std::string *dir, std::string *noext)
{
//   std::cout << "Parsing image name: " << img_name << std::endl;

   *dir = vul_file::dirname(img_name);
//   std::cout << "Directory: " << dir2 << std::endl;

   *noext = vul_file::strip_extension(img_name);
//   std::cout << "Filename w/o extension: " << noext << std::endl;

   return true;
}

//: given file prefix (with path) this function
// reads the extrinsic calibration from an ascii file into R and c
static bool
read_extrinsic(
      std::string prefix_path_name, 
      vgl_h_matrix_3d<double> *Rout,
      vgl_homg_point_3d<double> *center_out)
{
   std::cout << "Reading extrinsic params\n\n";
   prefix_path_name.append(".extrinsic");
   std::ifstream infp_ext(prefix_path_name.c_str(), std::ios::in);
   if (!infp_ext) {
     std::cerr << " Error opening file  " << prefix_path_name << std::endl;
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

//   std::cout << "Rotation\n";
//   std::cout << *Rout << std::endl << std::endl;

   infp_ext >> t_dummy;
   vgl_homg_point_3d<double> center(t_dummy[0],t_dummy[1],t_dummy[2]);

   *center_out = center;

//   std::cout << "Center:\n";
//   std::cout << t_dummy << std::endl;
   return true;
}

static bool
read_extrinsic(
      std::string prefix_path_name, 
      vgl_h_matrix_3d<double> *Rout,
      vgl_homg_point_3d<double> *center_out);

// reads intrinsic and extr. camera info from matlab calibration toolbox data
bool
read_cam(std::string img_name1, std::string img_name2, vpgl_perspective_camera <double> *P1out,
      vpgl_perspective_camera <double> *P2out)
{
   bool stat;

   std::string dir, noext;
   if (!bmcsd_get_prefix(img_name1,&dir,&noext)) {
      std::cerr << "Error in read_cam: couldn't get prefix\n";
      return false;
   }

   // Intrinsic params
   std::string cam_fname(dir+"/calib.intrinsic");
   std::ifstream infp(cam_fname.c_str(), std::ios::in);
   if (!infp) {
     std::cerr << " Error opening file  " << cam_fname << std::endl;
     return false;
   }

   vnl_double_3x3 m, m1, m2;
   m.read_ascii(infp);
   infp.close();
   m1 = m2 = m;

   if (!bmcsd_read_offset(noext,&m1))
     return false;

   vpgl_calibration_matrix<double> K1(m1);


   // Extrinsic params
   
   // File 1
   vgl_h_matrix_3d<double> R;
   vgl_homg_point_3d<double> center;
   stat = read_extrinsic(noext, &R, &center);
      if (!stat) {
         std::cerr << "error while reading extrinsic" << std::endl;
         return false;
      }

   vgl_rotation_3d<double> rrot(R);

   vpgl_perspective_camera<double> P1(K1, center, rrot);
   *P1out = P1;

   // File 2
   if (!bmcsd_get_prefix(img_name2,&dir,&noext)) {
      std::cerr << "Error in read_cam: couldn't get prefix file 2\n";
      return false;
   }

   if (!bmcsd_read_offset(noext,&m2))
     return false;

   vpgl_calibration_matrix<double> K2(m2);

   vgl_h_matrix_3d<double> R2;
   vgl_homg_point_3d<double> center2;
   stat = read_extrinsic(noext, &R2, &center2);
      if (!stat) {
         std::cerr << "error while reading extrinsic" << std::endl;
         return false;
      }

   vgl_rotation_3d<double> rot(R2);
   vpgl_perspective_camera<double> P2(K2, center2, rot);
   *P2out = P2;

   return true;
}

// reads intrinsic and extr. camera info from matlab calibration toolbox data
// TODO break read_cam into stream operator for vpgl_camera
bool
read_cam( std::string img_name1, 
      vpgl_perspective_camera <double> *P1out)
{
   bool stat;

   std::string dir, noext;
   if (!bmcsd_get_prefix(img_name1,&dir,&noext)) {
      std::cerr << "Error in read_cam: couln't get prefix\n";
      return false;
   }

   // Intrinsic params
   std::string cam_fname(dir+"/calib.intrinsic");
   std::ifstream infp(cam_fname.c_str(), std::ios::in);
   if (!infp) {
     std::cerr << " Error opening file  " << cam_fname << std::endl;
     return false;
   }

   vnl_double_3x3 m, m1;
   m.read_ascii(infp);
   infp.close();

   m1 = m;

   // Try to read offset, if exists
   if (!bmcsd_read_offset(noext,&m1))
     return false;

   // Try to read scaling, if exists
   if (!bmcsd_read_scaling(noext,&m1))
     return false;

   vpgl_calibration_matrix<double> K(m1);




   // Extrinsic params
   
   // File 1
   vgl_h_matrix_3d<double> R;
   vgl_homg_point_3d<double> center;
   stat = read_extrinsic(noext, &R, &center);
      if (!stat) {
         std::cerr << "error while reading extrinsic" << std::endl;
         return false;
      }
   std::cout << "Read rotation (before call vpgl_camera): " << std::endl << R << std::endl;
   vpgl_perspective_camera<double> P1(K, center, vgl_rotation_3d<double>(R));
   *P1out = P1;

   return true;
}

//: read camera from file containing linear row-wise list of camera values,
// then decompose it into K, R and C and generate a perspective_camera.
// The file has prefix *.projmatrix.
bool
read_3x4_matrix_into_cam( std::string img_name1, 
      vpgl_perspective_camera <double> *P1out)
{

   std::string dir, noext;
   if (!bmcsd_get_prefix(img_name1,&dir,&noext)) {
      std::cerr << "Error in read_3x4_matrix_into_cam\n";
      return false;
   }

   std::string cam_fname(noext+".projmatrix");
   std::ifstream infp(cam_fname.c_str(), std::ios::in);
   if (!infp) {
     std::cerr << " Error opening file  " << cam_fname << std::endl;
     return false;
   }

   vnl_matrix_fixed<double,3,4> camera_matrix;

   for (unsigned i=0; i<12; ++i) {
     double val;
     infp >> val;
     camera_matrix.data_block()[i] = val;
   }

   infp.close();

   bool stat =
     vpgl_perspective_decomposition( camera_matrix, *P1out);

   if (!stat) {
     std::cerr << "read_3x4_matrix_into_cam: could not decompose matrix" << std::endl;
     return false;
   }
   return true;
}

//: transfers a point to a 3rd image given p1,p1,F13 and F23. It will intersect
// the epipolar lines l13 and l23 on the 3rd image, returning p3.
// \todo move to vpgl/algo
vgl_homg_point_2d<double>
bmcsd_epipolar_point_transfer( 
      const vgl_homg_point_2d<double> &p1, 
      const vgl_homg_point_2d<double> &p2, 
      const vpgl_fundamental_matrix<double> &f13,
      const vpgl_fundamental_matrix<double> &f23)
{
   vgl_homg_point_2d<double> p3; 
   vgl_homg_line_2d<double> l13,l23;

   l13 = f13.l_epipolar_line(p1);
   l23 = f23.l_epipolar_line(p2);

   p3 = vgl_homg_operators_2d<double>::intersection(l13,l23);

   return p3;
}
/*
bool
reconstruct_pt_tangents(
      // Input:
      bsold_interp_curve_2d *curve1, 
      bsold_interp_curve_2d *curve2,
      const vnl_matrix <double> &s,  // nx2 w/ pairs of arc lengths of pts to reconstruct
      const vpgl_perspective_camera <double> &Pr1,
      const vpgl_perspective_camera <double> &Pr2,
      // Output: 
      std::vector<vsol_point_2d_sptr> *Gamas,
      std::vector<double> *error, // distance btw two rays in 3D
      std::vector<vgl_vector_3d<double>> *Ts
      )
      */

/* Doing bmcsd_rig ...
bool
reconstruct_pt_tangents(
      // Input:
      bsold_interp_curve_2d *curve1, 
      bsold_interp_curve_2d *curve2,
      double s1p, double s2p, double s1, double s2,
      const vpgl_perspective_camera <double> &Pr1,
      const vpgl_perspective_camera <double> &Pr2,
      // Output (caller's storage; we do not alloc em here): 
      double vnl_vector_fixed<double,3> *Gama_s,
      double vnl_vector_fixed<double,3> *Gama_sp,
      std::vector<vgl_vector_3d<double>> *T_s,
      std::vector<vgl_vector_3d<double>> *T_sp,
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
    
      std::cout << "Lambda:\n" << lambda << std::endl;
      // the error is:   (A*lambda +c1 - c2).two_norm()
    
      vnl_vector_fixed<double,3> Cpt_v = c1 + lambda(0)*gama1;
      vgl_homg_point_3d<double> Cpt(Cpt_v(0), Cpt_v(1), Cpt_v(2));
      std::cout << "Reconstructed point: " << Cpt << std::endl;

     //=========== Tangents
      std::cout << "\n\n\n";
      std::cout << "================= Tangent reconstruction: =======================" << std::endl;
    
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
    
      std::cout << "Test t1 dot F1 zero: " << dot_product(t1_world_bkwd,F1) << std::endl << std::endl;
      std::cout << "Test t2 dot F2 zero: " << dot_product(t2_world_bkwd,F2) << std::endl << std::endl;
    
      T_rec = vnl_cross_3d( vnl_cross_3d(t1_world_bkwd,gama1), vnl_cross_3d(t2_world_bkwd,gama2) );

   }

   return true;
}
*/


//: reads a vector of vsol points from a binary file, where each coordinate is
// stored row-by-row: x1,y1,x2,y2,x3,y3,x4,y4..
bool
myreadv(std::string fname, std::vector<vsol_point_2d_sptr> &pts)
{

  std::ifstream infp(fname.c_str(), std::ios::in | std::ios::binary);

  if (!infp) {
    std::cerr << "myreadv: error, unable to open file name" << std::endl;
    return false;
  }



  // determine size of file
  long fsize;

  std::filebuf *pbuf;

  pbuf=infp.rdbuf();

  // get file size using buffer's members
  fsize=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
  pbuf->pubseekpos (0,std::ios::in);

  fsize = fsize/(2*sizeof(double));

  pts.resize(fsize);
  // read it all
  for(unsigned long i=0; i<(unsigned long)fsize; ++i) {
    double x,y;
    infp.read(reinterpret_cast <char * > (&x), sizeof(double));
    infp.read(reinterpret_cast <char * > (&y), sizeof(double));

    pts[i] = new vsol_point_2d(x,y);
  }


  infp.close();
  return true;
}

//: reads a vector of vsol points from a binary file, where each coordinate is
// stored row-by-row: x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4..
bool
myreadv(std::string fname, std::vector<vgl_point_3d<double> > &pts)
{

  std::ifstream infp(fname.c_str(), std::ios::in | std::ios::binary);

  if (!infp) {
    std::cerr << "myreadv: error, unable to open file name" << std::endl;
    return false;
  }



  // determine size of file
  long fsize;

  std::filebuf *pbuf;

  pbuf=infp.rdbuf();

  // get file size using buffer's members
  fsize=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
  pbuf->pubseekpos (0,std::ios::in);

  fsize = fsize/(3*sizeof(double));

  pts.resize(fsize);
  // read it all
  for(unsigned long i=0; i<(unsigned long)fsize; ++i) {
    double x,y,z;
    infp.read(reinterpret_cast <char * > (&x), sizeof(double));
    infp.read(reinterpret_cast <char * > (&y), sizeof(double));
    infp.read(reinterpret_cast <char * > (&z), sizeof(double));

    pts[i] = vgl_point_3d<double> (x,y,z);
  }


  infp.close();
  return true;
}

bool
myreadv(std::string fname, std::vector<bmcsd_vector_3d> &pts)
{
  std::ifstream infp(fname.c_str(), std::ios::in | std::ios::binary);

  if (!infp) {
    std::cerr << "myreadv: error, unable to open file name" << std::endl;
    return false;
  }

  // determine size of file
  long fsize;

  std::filebuf *pbuf;

  pbuf=infp.rdbuf();

  // get file size using buffer's members
  fsize=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
  pbuf->pubseekpos (0,std::ios::in);

  fsize = fsize/(3*sizeof(double));

  pts.resize(fsize);
  // read it all
  for(unsigned long i=0; i<(unsigned long)fsize; ++i) {
    infp.read(reinterpret_cast <char * > (pts[i].data_block()), 3*sizeof(double));
  }

  infp.close();
  return true;
}

//: reads a 1D vector of doubles from a binary  file
bool
myread(std::string fname, std::vector<double> &pts)
{

  std::ifstream infp(fname.c_str(), std::ios::in | std::ios::binary);

  if (!infp) {
    std::cerr << "myread: error, unable to open file name" << std::endl;
    return false;
  }



  // determine size of file
  long fsize;

  std::filebuf *pbuf;

  pbuf=infp.rdbuf();

  // get file size using buffer's members
  fsize=pbuf->pubseekoff (0,std::ios::end,std::ios::in);
  pbuf->pubseekpos (0,std::ios::in);

  fsize = fsize/(sizeof(double));

  pts.resize(fsize);
  // read it all
  for(unsigned long i=0; i<(unsigned long)fsize; ++i) {
    double y;
    infp.read(reinterpret_cast <char * > (&y), sizeof(double));

    pts[i] = y;
  }


  infp.close();
  return true;
}


//: write a vector of vsol_point_2d to a binary file, to be loaded in e.g.
// matlab
bool
mywritev(std::string fname, const std::vector<vsol_point_2d_sptr> &pts)
{
  std::ofstream fp(fname.c_str(), std::ios::out | std::ios::binary);

  if (!fp) {
    std::cerr << "mywritev: error, unable to open file name" << std::endl;
    return false;
  }

  for (unsigned long i=0; i<pts.size(); ++i) {
     double aux=pts[i]->x();
     fp.write((char *)&(aux),sizeof(double));
     aux = pts[i]->y();
     fp.write((char *)&(aux),sizeof(double));
  }

  fp.close();
  return true;
}

bool
mywritev(std::string fname, const std::vector<bmcsd_vector_3d> &crv_3d)
{
  std::ofstream 
    fcrv_3d;

  fcrv_3d.open(fname.c_str(),std::ios::out | std::ios::binary);

  if (!fcrv_3d) {
    std::cerr << "mywritev: error, unable to open file name" << std::endl;
    return false;
  }

  for (unsigned k=0; k<crv_3d.size(); ++k) {
       fcrv_3d.write((char *)(crv_3d[k].data_block()),3*sizeof(double));
  }
  fcrv_3d.close();
  return true;
}

bool
mywritev(std::string fname, const std::vector<vgl_point_3d<double> > &crv_3d)
{
  std::ofstream 
    fcrv_3d;

  fcrv_3d.open(fname.c_str(),std::ios::out | std::ios::binary);

  if (!fcrv_3d) {
    std::cerr << "mywritev: error, unable to open file name" << std::endl;
    return false;
  }

  for (unsigned k=0; k<crv_3d.size(); ++k) {
    bmcsd_vector_3d v  = bmcsd_util::vgl_to_vnl(crv_3d[k]);

    fcrv_3d.write((char *)(v.data_block()),3*sizeof(double));
  }
  fcrv_3d.close();
  return true;
}

bool 
mywritev(std::string prefix, std::string ext, const std::vector<bdifd_1st_order_curve_3d> &crv_3d)
{
// The output files will be named like $prefix-3dcurve-$crv_id-{points|tangents}$ext

  std::string myprefix = prefix + std::string("-3dcurve-");

  for (unsigned c=0; c < crv_3d.size(); ++c) {
    unsigned np = crv_3d[c].size();
    if (np == 0) {
      std::cerr << "Warning: curve [" << c << "] is empty.\n";
      continue;
    }

    std::vector<bmcsd_vector_3d> tgts(np), pts(np);
    for (unsigned p=0; p < np; ++p) {
      pts[p] = crv_3d[c][p].Gama;
      tgts[p] = crv_3d[c][p].T;
    }

    std::ostringstream crv_id;
    crv_id << std::setw(std::ceil(std::log(crv_3d.size())/std::log(10.0))+1) << std::setfill('0');
    crv_id << c;
    
    std::string newprefix = myprefix + crv_id.str();

    bool retval = mywritev(newprefix + std::string("-points") + ext, pts);
    if (!retval)
      return retval;

    retval = mywritev(newprefix + std::string("-tangents") + ext, tgts);
    if(!retval)
      return retval;
  }
  return true;
}

bool 
myreadv(std::string prefix, std::string ext, std::vector<bdifd_1st_order_curve_3d> &crv_3d)
{
  std::string myprefix = prefix + std::string("-3dcurve-");

#ifndef NDEBUG
  std::cout << "myreadv: reading curves with prefix " << myprefix << std::endl;
#endif
  for (unsigned c=0; c < crv_3d.size(); ++c) {
    std::ostringstream crv_id;
    crv_id << std::setw(std::ceil(std::log(crv_3d.size())/std::log(10.0))+1) << std::setfill('0');
    crv_id << c;
    
    std::string newprefix = myprefix + crv_id.str();

    std::vector<bmcsd_vector_3d> tgts(crv_3d.size()), pts(crv_3d.size());

    std::string p_fname = newprefix + std::string("-points") + ext;
#ifndef NDEBUG
    std::cout << "myreadv: reading " << p_fname << std::endl;
#endif
    bool retval = myreadv(p_fname, pts);
    if (!retval) {
      std::cerr << "myreadv: error reading points" << p_fname << std::endl;
      return retval;
    }
    unsigned np = pts.size();

    std::string t_fname = newprefix + std::string("-tangents") + ext;
#ifndef NDEBUG
    std::cout << "myreadv: reading " << t_fname << std::endl;
#endif
    retval = myreadv(t_fname, tgts);
    if(!retval) {
      std::cerr << "myreadv: error reading tangents" << t_fname << std::endl;
      return retval;
    }

    assert(np == tgts.size());

    crv_3d[c].resize(np);
    for (unsigned p=0; p < np; ++p) {
      crv_3d[c][p].Gama = pts[p];
      crv_3d[c][p].T    = tgts[p];
    }
  }
  return true;
}

bool
mywrite(std::string fname, const std::vector<double> &v)
{
  std::ofstream 
    fp;

  fp.open(fname.c_str(),std::ios::out | std::ios::binary);

  if (!fp) {
    std::cerr << "myread: error, unable to open file name" << std::endl;
    return false;
  }

  for (unsigned k=0; k<v.size(); ++k) {
    double aux = v[k];
    fp.write((char *)(&aux),sizeof(double));
  }
  fp.close();
  return true;
}

bool
mywrite_ascii(std::string fname, const std::vector<double> &v)
{
  std::ofstream 
    fp;

  fp.open(fname.c_str(),std::ios::out);

  if (!fp) {
    std::cerr << "mywrite_ascii: error, unable to open file name" << std::endl;
    return false;
  }

  if (v.size()) {
    fp << v[0];
    for (unsigned k=1; k<v.size(); ++k) {
      fp << " " << v[k];
    }
    fp << std::endl;
  }
  fp.close();
  return true;
}


//:  This function returns the normal vector of a eulerspiral-based
//geno_curve_2d. This normal vector takes into account the sign of the
//curvature, so that at concave parts it points out, and at convex parts it
//points in.
//
// \param[in] i : the sample index (samples are indexed from 0 to c.size() inclusive)
//
//TODO move to bsold_geno_curve2d or a derived class based on approx.
//eulerspirals
//
void
get_normal_arc(const bsold_geno_curve_2d &c, unsigned i, double *normal_x, double *normal_y)
{
    vgl_vector_2d<double> tgt;

    double sign_k;

    assert(c.order() == 2);


    if (i == 0) {
      tgt = c[i].tangent_at(0);
      sign_k = c[i].curvature_at(0);
    } else if (i == c.size()) {
      tgt    = c[i-1].tangent_at(1);
      sign_k = c[i-1].curvature_at(1);
    } else {
      assert (i < c.size());
      vgl_vector_2d<double> t_i = c.interval(i)->tangent_at(0);
      vgl_vector_2d<double> t_i_min_1 = c.interval(i-1)->tangent_at(1);

      double tgt_x = t_i.x() + t_i_min_1.x();
      double tgt_y = t_i.y() + t_i_min_1.y();

      double norm_t = hypot(tgt_x,tgt_y);
      tgt_x /=  norm_t;
      tgt_y /=  norm_t;

      tgt.set(tgt_x,tgt_y);

      sign_k = (c[i].curvature_at(0) + c[i-1].curvature_at(1))*0.5;
    }


    double tolerance = std::numeric_limits<double>::epsilon()*1000;
    if (std::fabs(sign_k) <= tolerance)
      sign_k = 0;
    else
      sign_k = sign_k / std::fabs(sign_k);


    *normal_x = -sign_k * tgt.y();
    *normal_y =  sign_k * tgt.x();
}

//:  This function returns the normal vector of an arc-based //geno_curve_2d. This normal vector takes into account the sign of the
//curvature, so that at concave parts it points out, and at convex parts it
//points in.
//
// \param[in] i : the sample index (samples are indexed from 0 to c.size() inclusive)
//
//TODO move to bsold_geno_curve2d or a derived class based on approx.
//eulerspirals
//
void
get_normal(const bsold_geno_curve_2d &c, unsigned i, double *normal_x, double *normal_y)
{
    vgl_vector_2d<double> tgt;

    double sign_k;

    assert(c.order() == 3);

    if (i == 0) {
      tgt = c[i].tangent_at(0);
      sign_k = c[i].curvature_at(0);
    } else if (i == c.size()) {
      tgt = c[i-1].tangent_at(1);
      sign_k = c[i-1].curvature_at(1);
    } else {
      assert (i < c.size());
      tgt = c[i].tangent_at(0);
      sign_k = (c[i].curvature_at(0) + c[i-1].curvature_at(1))*0.5;
    }


    double tolerance = std::numeric_limits<double>::epsilon()*1000;
    if (std::fabs(sign_k) <= tolerance)
      sign_k = 0;
    else
      sign_k = sign_k / std::fabs(sign_k);


    *normal_x = -sign_k * tgt.y();
    *normal_y =  sign_k * tgt.x();
}

void bmcsd_util::
clip_to_img_bounds(
      const vil_image_view<vxl_uint_32> &img,
      std::vector<vsol_point_2d_sptr> *curve_ptr) 
{
  std::vector<vsol_point_2d_sptr> points_clip; 
  const std::vector<vsol_point_2d_sptr> &curve = *curve_ptr;

  points_clip.reserve(curve.size()); 

  for (unsigned i=0; i < curve.size(); ++i) {
   if ( img.in_range(
         (unsigned)(curve[i]->x()+0.5), 
         (unsigned)(curve[i]->y()+0.5 ) ) )
     points_clip.push_back(curve[i]);
  }
  *curve_ptr = points_clip;
}

void bmcsd_util::
clip_to_img_bounds(
    const vil_image_view<vxl_uint_32> &img,
    bdifd_1st_order_curve_2d *curve_ptr)
{
  const bdifd_1st_order_curve_2d &curve = *curve_ptr;

  bdifd_1st_order_curve_2d points_clip;

  points_clip.reserve(curve.size()); 

  for (unsigned i=0; i < curve.size(); ++i) {
   if ( img.in_range(
         (unsigned)(curve[i].gama[0]+0.5), 
         (unsigned)(curve[i].gama[1]+0.5 ) ) )
     points_clip.push_back(curve[i]);
  }
  *curve_ptr = points_clip;
}

void mw_util::
clip_to_img_bounds(
    const vil_image_view<vxl_uint_32> &img,
    dbdif_1st_order_curve_2d *curve_ptr,
    vcl_vector<unsigned> &orig_indices)
{
  const bdifd_1st_order_curve_2d &curve = *curve_ptr;

  bdifd_1st_order_curve_2d points_clip;

  points_clip.reserve(curve.size()); 

  for (unsigned i=0; i < curve.size(); ++i) {
   if ( img.in_range(
         (unsigned)(curve[i].gama[0]+0.5), 
         (unsigned)(curve[i].gama[1]+0.5 ) ) )
   {
     points_clip.push_back(curve[i]);
     orig_indices.push_back(i);
   }
  }
  *curve_ptr = points_clip;
}

bool bmcsd_util::
in_img_bounds(const bdifd_1st_order_curve_2d &curve, 
    const vil_image_view<vxl_uint_32> &img)
{
  for (unsigned i=0; i < curve.size(); ++i) {
   if ( !img.in_range(
         (unsigned)(curve[i].gama[0]+0.5), 
         (unsigned)(curve[i].gama[1]+0.5 ) ) )
     return false;
  }
  return true;
}

bool bmcsd_util::
in_img_bounds(const bdifd_1st_order_curve_2d &curve, 
    const vil_image_view<vxl_uint_32> &img, unsigned radius)
{
  for (unsigned i=0; i < curve.size(); ++i) {
    unsigned ix = (unsigned)(curve[i].gama[0]+0.5);
    unsigned iy = (unsigned)(curve[i].gama[1]+0.5 );
    if ( !img.in_range(ix, iy) )
      return false;

    if (ix < radius || iy < radius || 
        ix+radius > img.ni() || iy+radius > img.nj())
      return false;
  }
  return true;
}

bool bmcsd_util::
in_img_bounds( const std::vector<vsol_point_2d_sptr> &curve, 
    const vil_image_view<vxl_uint_32> &img)
{
  for (unsigned i=0; i < curve.size(); ++i) {
   if ( !img.in_range(
         (unsigned)(curve[i]->x()+0.5), 
         (unsigned)(curve[i]->y()+0.5 ) ) )
     return false;
  }
  return true;
}

bool bmcsd_util::
in_img_bounds(const vsol_polyline_2d &curve,
    const vil_image_view<vxl_uint_32> &img)
{
  for (unsigned i=0; i < curve.size(); ++i) {
    if ( !img.in_range(
          (unsigned)(curve.vertex(i)->x()+0.5), 
          (unsigned)(curve.vertex(i)->y()+0.5 ) ) )
      return false;
  }
  return true;
}

bool bmcsd_util::
in_img_bounds(const vsol_polyline_2d &curve,
    const vil_image_view<vxl_uint_32> &img, unsigned radius)
{
  for (unsigned i=0; i < curve.size(); ++i) {
    unsigned ix = (unsigned)(curve.vertex(i)->x()+0.5);
    unsigned iy = (unsigned)(curve.vertex(i)->y()+0.5 );
    if ( !img.in_range(ix, iy) )
      return false;

    if (ix < radius || iy < radius || 
        ix+radius > img.ni() || iy+radius > img.nj())
      return false;
  }
  return true;
}

void bmcsd_util::
get_vsol_point_vector(const vsol_polyline_2d &crv, std::vector<vsol_point_2d_sptr> *pts_ptr)
{
  std::vector<vsol_point_2d_sptr> &pts = *pts_ptr;

  pts.reserve(crv.size());
  for (unsigned k=0; k < crv.size(); ++k)
    pts.push_back(crv.vertex(k));
}

unsigned bmcsd_util::
find_nearest_pt(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, unsigned &mindist)
{
  unsigned min_idx,d;
  long int dx,dy;


  dx = static_cast<long>(crv->vertex(0)->x() - pt->x());
  dy = static_cast<long>(crv->vertex(0)->y() - pt->y());
  mindist = static_cast<unsigned>(dx*dx + dy*dy);
  min_idx = 0;

  for (unsigned i=1; i<crv->size(); ++i) {
    dx = static_cast<long>(crv->vertex(i)->x() - pt->x());
    dy = static_cast<long>(crv->vertex(i)->y() - pt->y());
    d = static_cast<unsigned>(dx*dx + dy*dy);
    if (mindist > d) {
      mindist = d;
      min_idx = i;
    }
  }

  return min_idx;
}

unsigned bmcsd_util::
find_nearest_pt_using_double(const vsol_point_2d_sptr &pt, const vsol_polyline_2d_sptr &crv, double &mindist)
{
  unsigned min_idx;
  double d,dx,dy;
  

  dx = crv->vertex(0)->x() - pt->x();
  dy = crv->vertex(0)->y() - pt->y();
  mindist = dx*dx + dy*dy;
  min_idx = 0;

  for (unsigned i=1; i<crv->size(); ++i) {
    dx = crv->vertex(i)->x() - pt->x();
    dy = crv->vertex(i)->y() - pt->y();
    d = dx*dx + dy*dy;
        
    if (mindist > d) {
      mindist = d;
      min_idx = i;
    }
  }

  return min_idx;
}


void bmcsd_util::
prune_curves(
    unsigned min_num_samples, 
    std::vector< vsol_polyline_2d_sptr > *pcurves,
    bbld_subsequence_set *ss
    )
{
  std::vector< vsol_polyline_2d_sptr > &old_curves = *pcurves;
  std::vector< vsol_polyline_2d_sptr > new_curves;

  new_curves.reserve(old_curves.size());
  ss->reserve(old_curves.size());
  for (unsigned i=0; i < old_curves.size(); ++i) {
    if (old_curves[i]->size() > min_num_samples) {
      new_curves.push_back(old_curves[i]);

      bbld_subsequence s(0, old_curves[i]->size());
      s.set_orig_id(i);
      ss->push_back(s);
    }
  }
  *pcurves = new_curves;
  std::vector< vsol_polyline_2d_sptr >(*pcurves).swap(*pcurves);
  ss->trim_memory();
}

void bmcsd_util::
prune_curves_by_length(
    double min_length, 
    std::vector< vsol_polyline_2d_sptr > *pcurves,
    bbld_subsequence_set *ss
    )
{
  std::vector< vsol_polyline_2d_sptr > &old_curves = *pcurves;
  std::vector< vsol_polyline_2d_sptr > new_curves;

  new_curves.reserve(old_curves.size());
  ss->reserve(old_curves.size());
  for (unsigned i=0; i < old_curves.size(); ++i) {
    if (old_curves[i]->length() > min_length) {
      new_curves.push_back(old_curves[i]);

      bbld_subsequence s(0, old_curves[i]->size());
      s.set_orig_id(i);
      ss->push_back(s);
    }
  }
  *pcurves = new_curves;
  std::vector< vsol_polyline_2d_sptr >(*pcurves).swap(*pcurves);
  ss->trim_memory();
}

void mw_util::
prune_curves_by_length_with_flags(
    double min_length, 
    std::vector< vsol_polyline_2d_sptr > *pcurves,
    dbbl_subsequence_set *ss, std::vector<std::vector<bool> > &flags
    )
{
  std::vector< vsol_polyline_2d_sptr > &old_curves = *pcurves;
  std::vector< vsol_polyline_2d_sptr > new_curves;

  std::vector<std::vector<bool> > new_flags;

  new_curves.reserve(old_curves.size());
  ss->reserve(old_curves.size());
  for (unsigned i=0; i < old_curves.size(); ++i) {
    if (old_curves[i]->length() > min_length) {
      new_curves.push_back(old_curves[i]);
      new_flags.push_back(flags[i]);

      bbld_subsequence s(0, old_curves[i]->size());
      s.set_orig_id(i);
      ss->push_back(s);
    }
  }
  *pcurves = new_curves;
  flags.clear();
  flags = new_flags;
  std::vector< vsol_polyline_2d_sptr >(*pcurves).swap(*pcurves);
  ss->trim_memory();
}

bool bmcsd_util::
read_cam_anytype(std::string fname, camera_file_type type, 
  vpgl_perspective_camera<double> *cam)
{
  switch (type) {
    case bmcsd_util::BMCS_INTRINSIC_EXTRINSIC:
      std::cout << "Reading camera: intrinsic/extrinsic, fname = " << fname << std::endl;
      if (! read_cam(fname, cam) )
        return false;
    break;

    case bmcsd_util::BMCS_3X4:
      std::cout << "Reading camera: 3x4 matrix, fname = " << fname << std::endl;
      if (! read_3x4_matrix_into_cam(fname, cam) )
         return false;
    break;
    default:
      std::cerr << "Invalid camera type\n";
      return false;
  }
  return true;
}

bool bmcsd_util::
write_cams(std::string dir, std::string prefix, camera_file_type type, 
    const std::vector<vpgl_perspective_camera<double> > &cam)
{
  if (cam.empty()) {
    std::cerr << "Warning: trying to write no cameras\n";
    return true; // no real errors encountered.
  }

  switch (type) {
    case bmcsd_util::BMCS_INTRINSIC_EXTRINSIC: {
      std::cout << "Writing camera: intrinsic/extrinsic, dir = " << dir << std::endl;

      { // Intrinsics
        std::string k_fname(dir+"/calib.intrinsic");
        std::ofstream fp;

        fp.open(k_fname.c_str(),std::ios::out);

        if (!fp) {
          std::cerr << "write_cam: error, unable to open file name " << k_fname << std::endl;
          return false;
        }
        fp << std::setprecision(20);
        fp << cam[0].get_calibration().get_matrix();
      }

       // Extrinsics
      for (unsigned v=0; v < cam.size(); ++v) {
        std::ostringstream v_str;
        v_str << std::setw(4) << std::setfill('0') << v;

        std::string e_fname(dir + std::string("/") + prefix + v_str.str() + std::string(".extrinsic"));
        std::cout << "Writing extrinsics file: " << e_fname << std::endl;
        std::ofstream fp;

        fp.open(e_fname.c_str(),std::ios::out);

        if (!fp) {
          std::cerr << "write_cam: error, unable to open file name " << e_fname << std::endl;
          return false;
        }
        fp << std::setprecision(20);
        fp << cam[v].get_rotation().as_matrix() << std::endl;
        fp << vgl_to_vnl(cam[v].get_camera_center()) << std::endl;

        fp.close();
      }
      return true;
    } break;

    case bmcsd_util::BMCS_3X4:
      std::cout << "Writing camera: 3x4 matrix, dir = " << dir << std::endl;
      std::cerr << "Error: Not supported\n";
      return false;
    break;
    default:
      std::cerr << "Invalid camera type\n";
      return false;
  }
  return true;
}

bool bmcsd_util::
write_cams(
      std::string dir, 
      std::vector<std::string> cam_fname_noexts, 
      camera_file_type type, 
      const std::vector<vpgl_perspective_camera<double> > &cam)
{
  if (cam.empty()) {
    std::cerr << "Warning: trying to write no cameras\n";
    return true; // no real errors encountered.
  }

  assert(cam_fname_noexts.size() == cam.size());

  switch (type) {
    case bmcsd_util::BMCS_INTRINSIC_EXTRINSIC: {
      std::cout << "Writing camera: intrinsic/extrinsic, dir = " << dir << std::endl;

      { // Intrinsics
        std::string k_fname(dir+"/calib.intrinsic");
        std::ofstream fp;

        fp.open(k_fname.c_str(),std::ios::out);

        if (!fp) {
          std::cerr << "write_cam: error, unable to open file name " << k_fname << std::endl;
          return false;
        }
        fp << std::setprecision(20);
        fp << cam[0].get_calibration().get_matrix();
      }

       // Extrinsics
      for (unsigned v=0; v < cam.size(); ++v) {
        std::string e_fname(dir + std::string("/") 
            + cam_fname_noexts[v] + std::string(".extrinsic"));
        std::cout << "Writing extrinsics file: " << e_fname << std::endl;
        std::ofstream fp;

        fp.open(e_fname.c_str(),std::ios::out);

        if (!fp) {
          std::cerr << "write_cam: error, unable to open file name " << e_fname << std::endl;
          return false;
        }
        fp << std::setprecision(20);
        fp << cam[v].get_rotation().as_matrix() << std::endl;
        fp << vgl_to_vnl(cam[v].get_camera_center()) << std::endl;

        fp.close();
      }
      return true;
    } break;

    case bmcsd_util::BMCS_3X4:
      std::cout << "Writing camera: 3x4 matrix, dir = " << dir << std::endl;

      for (unsigned v=0; v < cam.size(); ++v) {
        std::string e_fname(dir + std::string("/") 
            + cam_fname_noexts[v] + std::string(".projmatrix"));
        std::cout << "Writing cam file: " << e_fname << std::endl;
        std::ofstream fp;

        fp.open(e_fname.c_str(),std::ios::out);

        if (!fp) {
          std::cerr << "write_cam: error, unable to open file name " << e_fname << std::endl;
          return false;
        }
        fp << std::setprecision(20);
        fp << cam[v].get_matrix() << std::endl;
        fp.close();
      }
      return false;
    break;
    default:
      std::cerr << "Invalid camera type\n";
      return false;
  }
  return true;
}
