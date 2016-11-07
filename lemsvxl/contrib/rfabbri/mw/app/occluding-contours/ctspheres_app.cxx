#include "ctspheres_app.h"
#include "ctspheres_io.h"

#include <vcl_iostream.h>

#include <dbdif/dbdif_rig.h>
#include <mw/algo/mw_match_old.h>
#include <dbdif/dbdif_analytic.h>
#include <mw/mw_dist.h>
#include <dbdif/algo/dbdif_data.h>

#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vgl/algo/vgl_homg_operators_3d.h>

#include <vnl/vnl_math.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vpgl/vpgl_fundamental_matrix.h>
#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/pro/dvpgl_camera_storage_sptr.h>


static void
match_contours(
  const vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con,
  const vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con_dt,
  const vcl_vector<unsigned> &match,
  dbdif_rig &rig,
  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con_match,
  vcl_vector<vcl_vector<bool> > &valid);

static void 
define_perturbation(
    double theta, double phi, 
    bool read_params_from_file, 
    bool write_info, 
    const dbdif_rig &rig, 
    mw_vector_3d &my_c_t);

static void 
define_perturbation(
    double theta, double phi, 
    bool read_params_from_file, 
    bool write_info, 
    const dbdif_rig &rig, 
    mw_vector_3d &my_c_t, double mag);


static void
mw_sphere_apparent_contour_do_a_camera_pair(
      bool read_params_from_file, bool write_info,
      dbdif_rig &rig, bool perturb, const mw_vector_3d &my_c_t, const vcl_string &suffix,
      vcl_vector<vcl_vector<mw_vector_3d> > &crv_3d
      );

//: reads 3D levelset image, trace 0-levelset at each slice, do matching
void 
match_and_reconstruct()
{

  vcl_vector<vil_image_view<float> > imgv;
  read_levelset(imgv);

  unsigned ni,nj;
  ni = imgv[0].ni();
  nj = imgv[0].nj();

  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con_dt;

  trace_contours(imgv, con, 2);
  trace_contours(imgv, con_dt, 3);
  assert (con.size() <= 13);
  assert (con_dt.size() <= 13);

  // perform matching

  vcl_vector<vsol_point_2d> m;
  vcl_vector<vsol_point_2d> m_dt;

  m.resize(con.size(),vsol_point_2d(0,0));
  m_dt.resize(con_dt.size(),vsol_point_2d(0,0));

  vcl_cout << "Centroids slice t\n";
  // compute centers of masses
  for (unsigned i=0; i<m.size(); ++i) {
    for (unsigned k=0; k<con[i].size(); ++k) {
      m[i].set_x(m[i].x() + con[i][k]->x());
      m[i].set_y(m[i].y() + con[i][k]->y());
    }

    m[i].set_x(m[i].x() / con[i].size());
    m[i].set_y(m[i].y() / con[i].size());
    printf("%g\t%g\n",m[i].x(), m[i].y());
  }

  vcl_cout << "\nCentroids slice t+dt\n";
  // compute centers of masses
  for (unsigned i=0; i<m_dt.size(); ++i) {
    for (unsigned k=0; k<con_dt[i].size(); ++k) {
      m_dt[i].set_x(m_dt[i].x() + con_dt[i][k]->x());
      m_dt[i].set_y(m_dt[i].y() + con_dt[i][k]->y());
    }

    m_dt[i].set_x(m_dt[i].x() / con_dt[i].size());
    m_dt[i].set_y(m_dt[i].y() / con_dt[i].size());
    printf("%g\t%g\n",m_dt[i].x(), m_dt[i].y());
  }



  // match centers of masses

  vcl_vector<unsigned> match;
  vcl_vector<double> cost;

  match.resize(m.size());

  
  cost.resize(m.size(),ni*ni + nj*nj);

  for (unsigned i=0; i<m.size(); ++i) {
    for (unsigned j=0; j<m_dt.size(); ++j) {
      double newcost;

      double dx = m[i].x() - m_dt[j].x();
      double dy = m[i].y() - m_dt[j].y();


      newcost = dx*dx + dy*dy;

      if (cost[i] > newcost) {
        cost[i]  = newcost;
        match[i] = j;
      }
    }
  }

  // detect erratic cases

  printf("Match\t|\tCost\n");
  for (unsigned i=0; i<m.size(); ++i) {
    printf("%d\t%g\n",match[i],cost[i]);
    for (unsigned j=0; j<m.size(); ++j) {
      if (i != j && match[i] == match[j])
        vcl_cerr << "Error: duplicate match!\n";
    }
  }


  // Perform intra-contour matching and reconstruction

  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_ctspheres(Kmatrix);
  vpgl_calibration_matrix<double> K(Kmatrix);
  vpgl_perspective_camera<double> *P, *P_dt;

  P    = dbdif_turntable::camera_ctspheres(2, K);
  P_dt = dbdif_turntable::camera_ctspheres(3, K);

  dbdif_rig rig(*P,*P_dt);

  delete P; 
  delete P_dt;

  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con_match;
  con_match.resize(con.size());

  for (unsigned i=0; i<con.size(); ++i) {
    con_match[i].resize(con[i].size());
    for (unsigned k=0; k<con[i].size(); ++k) {
      con_match[i][k] = new vsol_point_2d();

      vgl_homg_point_2d<double> p(con[i][k]->x(),con[i][k]->y(),1);

      vsol_polyline_2d_sptr polycrv = new vsol_polyline_2d(con_dt[match[i]]);

      vgl_homg_line_2d<double> l = rig.f12.l_epipolar_line(p);

      nearest_match_along_line (
          *(con[i][k]),
          l,
          polycrv,
          con_match[i][k].ptr()
          );
    }
  }

  vcl_ofstream 
    fcrv_3d;

  vcl_ostringstream imgnumber; //:< number of first or central image
  imgnumber << 2;

  vcl_string prefix("ct-spheres/dat/reconstr-600x-");
  vcl_string ext(".dat");

  for (unsigned i=0; i<con.size(); ++i) {
    vcl_vector<mw_vector_3d> crv_3d;
    vcl_vector<bool> valid;

    rig.reconstruct_3d_occluding_contour(
        &crv_3d,
        valid,
        con[i],
        con_match[i]);


    vcl_ostringstream contnumber;
    contnumber << i;


    vcl_string 
      fname=prefix + imgnumber.str() + vcl_string("-") + contnumber.str() + ext;

    fcrv_3d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

    vcl_cout << "writing: " << fname << vcl_endl;
    for (unsigned k=0; k<crv_3d.size(); ++k) {
      if (valid[k]) {
         fcrv_3d.write((char *)(crv_3d[k].data_block()),3*sizeof(double));
      }
    }

    crv_3d.clear();
    valid.clear();
    fcrv_3d.close();
  }

}

// Application to test *autocalibration* based on occluding contours of an
// analytic sphere. 
//
// \param[in] read_params_from_file: true if you want to ignore parameter values
// and read them from files.
void
mw_sphere_apparent_contour_app_orientation(
    double theta, double phi,
    bool read_params_from_file=false
    )
{
  // - display the resulting projection as a binary image (?)
  //    - create binary images (rasterization of contour)
  //    - create vsol objects
  //    - create storages for each
  //    - create 2 frames, store each result in a frame


  bool perturb=true;

  // Define cameras
  
  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_ctspheres(Kmatrix);
  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P, *P_dt;

  P    = dbdif_turntable::camera_ctspheres(0, K);
  P_dt = dbdif_turntable::camera_ctspheres(1, K);

  dbdif_rig rig1(*P,*P_dt);

  delete P; delete P_dt;


  P    = dbdif_turntable::camera_ctspheres(1, K);
  P_dt = dbdif_turntable::camera_ctspheres(2, K);

  dbdif_rig rig2(*P,*P_dt);

  delete P; delete P_dt;

  bool write_info=false;
  vcl_string suffix("-dummy"); //:< not used


  double hmin=vcl_numeric_limits<double>::infinity(), theta_min=0, phi_min=0;
  mw_vector_3d my_c_t; //:< perturbed vector

  unsigned long n=0;
  for (theta=0; theta<360; theta+=10)
    for (phi=0; phi<180; phi+=5) {

      vcl_cout << "Iteration number: " << ++n << 
          "  Theta: " << theta << "  Phi: " << phi  << "   (degrees)" << vcl_endl;

      define_perturbation(theta, phi, read_params_from_file, write_info, rig1, my_c_t);

      // MATCH + RECONSTRUCTION

      vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_01;
      mw_sphere_apparent_contour_do_a_camera_pair
        (read_params_from_file,  write_info, rig1, perturb, my_c_t, suffix, crv_3d_01);

      vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_12;
      mw_sphere_apparent_contour_do_a_camera_pair
        (read_params_from_file, write_info, rig2, perturb, my_c_t, suffix, crv_3d_12);


      assert(crv_3d_01.size() == crv_3d_12.size() && crv_3d_01.size() == 1);
      double h = mw_dist::hdist(crv_3d_01[0],crv_3d_12[0]);

      if (h!=0 && h < hmin) {
        hmin = h;
        theta_min = theta;
        phi_min = phi;
      }

      vcl_cout << "h : " << vcl_sqrt(h) << vcl_endl;
      vcl_cout << "hmin : " << vcl_sqrt(hmin) << vcl_endl;
      vcl_cout << 
        "Theta min: " << theta_min << "  Phi min: " << phi_min << vcl_endl;

    }

  vcl_cout << 
    "Theta min: " << theta_min << "  Phi min: " << phi_min << vcl_endl;
}

// Application to test *autocalibration* based on occluding contours of an
// analytic sphere. 
//
// \param[in] read_params_from_file: true if you want to ignore parameter values
// and read them from files.
void
mw_sphere_apparent_contour_app_magnitude(
    double theta, double phi,
    bool read_params_from_file=false
    )
{
  // - display the resulting projection as a binary image (?)
  //    - create binary images (rasterization of contour)
  //    - create vsol objects
  //    - create storages for each
  //    - create 2 frames, store each result in a frame


  bool perturb=true;

  // Define cameras
  
  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_ctspheres(Kmatrix);
  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P, *P_dt;

  P    = dbdif_turntable::camera_ctspheres(0, K);
  P_dt = dbdif_turntable::camera_ctspheres(1, K);

  dbdif_rig rig1(*P,*P_dt);

  delete P; delete P_dt;


  P    = dbdif_turntable::camera_ctspheres(1, K);
  P_dt = dbdif_turntable::camera_ctspheres(2, K);

  dbdif_rig rig2(*P,*P_dt);

  delete P; delete P_dt;

  bool write_info=false;
  vcl_string suffix("-dummy"); //:< not used


  double hmin=vcl_numeric_limits<double>::infinity(), mag_min=0, theta_min=0, phi_min=0;
  mw_vector_3d my_c_t; //:< perturbed vector

//  theta = 180;
//  phi = 89.75;

//  for (mag=1.05; mag<20; mag+=0.02) {
  unsigned long n=0;
  double mag;
  for (theta=0; theta<360; theta+=15)
    for (phi=180; phi>=0; phi-=10)
      for (mag=0.01; mag<4; mag+=0.15) {
        vcl_cout << "-------------" << vcl_endl << vcl_endl;
      vcl_cout << "Iteration number: " << ++n << "  mag(mm): " << mag << 
          "  Theta: " << theta << "  Phi: " << phi  << "   (degrees)" << vcl_endl;

      define_perturbation(theta, phi, read_params_from_file, write_info, rig1, my_c_t,mag);

      // MATCH + RECONSTRUCTION

      vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_01;
      mw_sphere_apparent_contour_do_a_camera_pair
        (read_params_from_file,  write_info, rig1, perturb, my_c_t, suffix, crv_3d_01);

      vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_12;
      mw_sphere_apparent_contour_do_a_camera_pair
        (read_params_from_file, write_info, rig2, perturb, my_c_t, suffix, crv_3d_12);


      assert(crv_3d_01.size() == crv_3d_12.size() && crv_3d_01.size() == 1);
      double h = mw_dist::hdist(crv_3d_01[0],crv_3d_12[0]);

      if (h!=0 && h < hmin) {
        hmin = h;
        mag_min = mag;
        theta_min = theta;
        phi_min = phi;
      }

      vcl_cout << "h : " << vcl_sqrt(h) << vcl_endl;
      vcl_cout << "hmin : " << vcl_sqrt(hmin) << vcl_endl;
      vcl_cout << 
        "mag min: " << mag_min << vcl_endl;
      vcl_cout << 
        "Theta min: " << theta_min << "  Phi min: " << phi_min << vcl_endl;

    }

     vcl_cout << 
        "FINAL mag min: " << mag_min << vcl_endl;
      vcl_cout << 
        "Theta min: " << theta_min << "  Phi min: " << phi_min << vcl_endl;
}


// Application to test reconstruction based on occluding contours of an
// analytic sphere.
//
// \param[in] read_params_from_file: true if you want to ignore parameter values
// and read them from files.
void
mw_sphere_apparent_contour_app1(
    double theta, double phi,
    bool read_params_from_file=false,
    bool write_info=true
    )
{

  // - display the resulting projection as a binary image (?)
  //    - create binary images (rasterization of contour)
  //    - create vsol objects
  //    - create storages for each
  //    - create 2 frames, store each result in a frame


  bool perturb=true;

  // Define cameras
  
  vnl_double_3x3 Kmatrix;
  dbdif_turntable::internal_calib_ctspheres(Kmatrix);
  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P, *P_dt;

  P    = dbdif_turntable::camera_ctspheres(0, K);
  P_dt = dbdif_turntable::camera_ctspheres(1, K);

  dbdif_rig rig1(*P,*P_dt);

  delete P; delete P_dt;


  P    = dbdif_turntable::camera_ctspheres(1, K);
  P_dt = dbdif_turntable::camera_ctspheres(2, K);

  dbdif_rig rig2(*P,*P_dt);

  delete P; delete P_dt;


  mw_vector_3d my_c_t; //:< perturbed vector

  define_perturbation(theta, phi, read_params_from_file, write_info, rig1, my_c_t);

  // MATCH + RECONSTRUCTION

  vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_01;

  vcl_string suffix("-cam01"); //:< cameras 0 and 1
  mw_sphere_apparent_contour_do_a_camera_pair
    (read_params_from_file,  write_info, rig1, perturb, my_c_t, suffix,crv_3d_01);

  vcl_vector<vcl_vector<mw_vector_3d> > crv_3d_12;
  suffix = "-cam12"; //:< cameras 1 and 2 
  mw_sphere_apparent_contour_do_a_camera_pair
    (read_params_from_file, write_info, rig2, perturb, my_c_t, suffix, crv_3d_12);
}


//: param[out] crv_3d : reconstructed 3D curve
void
mw_sphere_apparent_contour_do_a_camera_pair(
      bool /*read_params_from_file*/, bool write_info,
      dbdif_rig &rig, bool perturb, const mw_vector_3d &my_c_t, const vcl_string &suffix,
      vcl_vector<vcl_vector<mw_vector_3d> > &crv_3d
      )
{
  vcl_string ext(".dat");

  // find sphere_occluding_contour for each view
  mw_vector_3d s0;
  s0(0) = 0;
  s0(1) = 0;
  s0(2) = 60;
  const double radius=5;
  double Gamma_radius, Gamma_radius2;

  vcl_vector<mw_vector_3d> Gamma;
  mw_vector_3d Gamma_center;
  dbdif_analytic::sphere_occluding_contour(
      radius, /* radius */
      s0, /* sphere center */
      rig.cam[0].c, /* camera center */
      Gamma, Gamma_center, Gamma_radius);


  vcl_vector<vsol_point_2d_sptr> xi; //:< image coordinates
  xi.resize(Gamma.size());
  for (unsigned i=0; i<Gamma.size(); ++i) {
    // - get image coordinates
    mw_vector_2d p_aux;
    p_aux = rig.cam[0].project_to_image(Gamma[i]);
    xi[i] = new vsol_point_2d(p_aux[0], p_aux[1]);
  }


  vcl_vector<mw_vector_3d> Gamma2;
  mw_vector_3d Gamma_center2;
  dbdif_analytic::sphere_occluding_contour(
      radius, /* radius */
      s0, /* sphere center */
      rig.cam[1].c, /* camera center */
      Gamma2, Gamma_center2, Gamma_radius2);

  vcl_vector<vsol_point_2d_sptr> xi2; //:< image coordinates
  xi2.resize(Gamma.size());
  for (unsigned i=0; i<Gamma2.size(); ++i) {
    // - get image coordinates
    mw_vector_2d p_aux;
    p_aux = rig.cam[1].project_to_image(Gamma2[i]);
    xi2[i] = new vsol_point_2d(p_aux[0], p_aux[1]);
  }

  if (write_info) {
    mywritev(vcl_string("ct-spheres/dat/xi") + suffix + ext,xi);
    mywritev(vcl_string("ct-spheres/dat/xi2") + suffix + ext,xi2);
    mywritev(vcl_string("ct-spheres/dat/Gamma-occl") + suffix + ext,Gamma);
    mywritev(vcl_string("ct-spheres/dat/Gamma2-occl") + suffix + ext,Gamma2);


    // Write contour generator #1's center, binormal (plane normal), and radius
    {
        vcl_ofstream fGamma_p;
        vcl_string fname = vcl_string("ct-spheres/dat/Gamma-occl-params-1") + suffix + ext;

        fGamma_p.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
        if (!fGamma_p) {
          vcl_cerr << "myreadv: error, unable to open file" << vcl_endl; exit(1);
        }
        fGamma_p.write((char *)(Gamma_center.data_block()),3*sizeof(double));
        mw_vector_3d nn = (rig.cam[0].c - s0).normalize();
        fGamma_p.write((char *)(nn.data_block()),3*sizeof(double));
        fGamma_p.write((char *)(&Gamma_radius),sizeof(double));
        fGamma_p.close();
    }
  }

  // ======= MATCH =========

  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con;
  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con_dt;
  vcl_vector<unsigned>contour_match_idx;
  contour_match_idx.push_back(0);

  con.push_back(xi);
  con_dt.push_back(xi2);


  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  con_match;
  vcl_vector<vcl_vector<bool> > valid;


  match_contours(con, con_dt, contour_match_idx, rig, con_match, valid);


  // ==================== RECONSTRUCT and OUTPUT ================
  vcl_ofstream 
    fcrv_3d,
    fcon, fcon_match, fvalid;
  vcl_string prefix("ct-spheres/dat/analytic-reconstr-");

  // XXX  pass as argument
  vcl_ostringstream imgnumber; //:< number of first or central image
  imgnumber << 2;

  crv_3d.resize(1);

  for (unsigned i=0; i<con.size(); ++i) {
    crv_3d[0].clear();

    if (!perturb) {
      rig.reconstruct_3d_occluding_contour(
          &(crv_3d[0]),
          valid[i],
          con[i],
          con_match[i]);
    } else {
      rig.reconstruct_3d_occluding_contour(
          &(crv_3d[0]),
          valid[i],
          con[i],
          con_match[i],
          my_c_t
          );
    }



    if (write_info) {
      vcl_ostringstream contnumber;
      contnumber << i;

      vcl_string 
        fname=prefix + imgnumber.str() + vcl_string("-") + contnumber.str() + suffix + ext;

      fcrv_3d.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

      vcl_cout << "writing: " << fname << vcl_endl;
      for (unsigned k=0; k<crv_3d[0].size(); ++k) {
        if (valid[i][k]) {
           fcrv_3d.write((char *)(crv_3d[0][k].data_block()),3*sizeof(double));
        } else {
  //        vcl_cout << "Not valid: " << i << k << vcl_endl;
        }
      }

      fcrv_3d.close();
    }
  }

  vcl_vector<mw_vector_3d> crv_valid;

  for (unsigned k=0; k<crv_3d[0].size(); ++k) {
    if (valid[0][k]) {
      crv_valid.push_back(crv_3d[0][k]);
    }
  }

  crv_3d[0].clear();
  crv_3d[0] = crv_valid;


  if (write_info) { // Only output the first contour match for debugging:

    vcl_ostringstream contnumber;
    contnumber << 0;

    vcl_string 
    fname = prefix + imgnumber.str() + vcl_string("-") + contnumber.str()
       + vcl_string("-con") + suffix + ext;

    mywritev(fname,con[0]);

    fname = prefix + imgnumber.str() + vcl_string("-") + contnumber.str()
       + vcl_string("-conmatch") + suffix + ext;

    mywritev(fname,con_match[0]);

    fname = prefix + imgnumber.str() + vcl_string("-") + contnumber.str()
       + vcl_string("-valid") + suffix + ext;

    fvalid.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);

    for (unsigned k=0; k<valid[0].size(); ++k) {
      double aux = (valid[0][k])?1.0:0.0;
      fvalid.write((char *)( &aux ),sizeof(double));
    }

    fvalid.close();
  }
}


void
match_contours(
  const vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con,
  const vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con_dt,
  const vcl_vector<unsigned> &match,
  dbdif_rig &rig, //:< input
  vcl_vector<vcl_vector<vsol_point_2d_sptr> >  &con_match,
  vcl_vector<vcl_vector<bool> > &valid
    )
{

  con_match.resize(con.size());
  valid.resize(con.size());

  assert(con[0].size() > 3);

  bool normal_matching=false;

  double maxdist;
  if (true) {  // DEPENDS ON DATASET MAX SAMPLE SPACING
    double dx = con[0][2]->x() - con[0][1]->x();
    double dy = con[0][2]->y() - con[0][1]->y();

    maxdist = hypot(dx,dy)/1.9;
  } else
    maxdist = 1.4;

  for (unsigned i=0; i<con.size(); ++i) {
    con_match[i].resize(con[i].size());
    valid[i].resize(con[i].size(),true);
    for (unsigned k=0; k<con[i].size(); ++k) {
      con_match[i][k] = new vsol_point_2d();

      vgl_homg_point_2d<double> p(con[i][k]->x(),con[i][k]->y(),1);
      vsol_polyline_2d_sptr polycrv = new vsol_polyline_2d(con_dt[match[i]]);

      vgl_homg_line_2d<double> l;

      if (normal_matching) {
        // Normal matching, along plane with normal gamma_s
        l = mw_normal_correspondence_line(con[i],k);
      } else {
        /*epipolar_matching*/
        l = rig.f12.l_epipolar_line(p);
      }

      bool has_intersection =
      nearest_match_along_line (
          *(con[i][k]),
          l,
          polycrv,
          con_match[i][k].ptr(),
          maxdist
          );

      if (!has_intersection)
        valid[i][k] = false;
    }
  }
}

void 
define_perturbation(
    double theta, double phi, 
    bool read_params_from_file, 
    bool write_info, 
    const dbdif_rig &rig, 
    mw_vector_3d &my_c_t, double magnitude)
{
    if (read_params_from_file) {
      vcl_ifstream fconf("ct-spheres/config/perturb.txt",vcl_ios::in);
      if (!fconf)
        vcl_cerr << "Could not open file" << vcl_endl;

      fconf >> theta;
      fconf >> phi;
      fconf >> magnitude;

      fconf.close();
    }
    // override magnitude (for now)

    // convert to radians
    theta *= rad_to_degree_ratio;
    phi   *= rad_to_degree_ratio;

    my_c_t[0] = magnitude*sin(phi)*cos(theta);
    my_c_t[1] = magnitude*sin(phi)*sin(theta);
    my_c_t[2] = magnitude*cos(phi);

    if (write_info) {
      // write cam info
      vcl_ofstream fcam;
      vcl_string fname("ct-spheres/dat/analytic-reconstr-tmp-C-perturb.dat");

      fcam.open(fname.c_str(),vcl_ios::out | vcl_ios::binary);
      if (!fcam) {
        vcl_cerr << "myreadv: error, unable to open file name" << vcl_endl; exit(1);
      }
      fcam.write((char *)(rig.cam[0].c.data_block()),3*sizeof(double));
      fcam.write((char *)(rig.cam[1].c.data_block()),3*sizeof(double));
      fcam.write((char *)(my_c_t.data_block()),3*sizeof(double));

      fcam.close();
    }
}


// without magnitude parameter
void 
define_perturbation(
    double theta, double phi, 
    bool read_params_from_file, 
    bool write_info, 
    const dbdif_rig &rig, 
    mw_vector_3d &my_c_t)
{

    double magnitude;
    magnitude = (rig.cam[0].c - rig.cam[1].c).two_norm();

  define_perturbation(
      theta, phi, read_params_from_file, write_info, rig, my_c_t, magnitude);
}



