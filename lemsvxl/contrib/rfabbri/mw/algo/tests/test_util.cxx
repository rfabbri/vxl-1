#include <testlib/testlib_test.h>

#include <mw/algo/mw_algo_util.h>
#include <dbdif/algo/dbdif_data.h>
#include <vgl/vgl_distance.h>


static void  test_reprojection_errors();
static void  test_boost_iostreams();

MAIN( test_util )
{
  START ("algo utilities");

  test_reprojection_errors();
  test_boost_iostreams();

  SUMMARY();
}



void  test_reprojection_errors()
{

  vnl_double_3x3 Kmatrix;

//  unsigned nrows = 400;
  unsigned ncols = 500;
  unsigned  crop_origin_x = 450;
  unsigned  crop_origin_y = 1750;
  dbdif_turntable::internal_calib_olympus(Kmatrix, ncols, crop_origin_x, crop_origin_y);

  vpgl_calibration_matrix<double> K(Kmatrix);

  vpgl_perspective_camera<double> *P;


  unsigned nviews = 3;

  vcl_vector<unsigned> angles;
  angles.push_back(30);
  angles.push_back(50);
  angles.push_back(70);

  vcl_vector<dbdif_camera> cam;
  vcl_vector<vpgl_perspective_camera<double> *> pcam;

  cam.resize(nviews);

  pcam.resize(nviews);
  for (unsigned i=0; i < nviews; ++i) {
    P = dbdif_turntable::camera_olympus(angles[i], K);
    cam[i].set_p(*P);
    pcam[i] = P;
  }

  vcl_vector<vcl_vector<dbdif_3rd_order_point_3d> > crv3d;
//  dbdif_data::space_curves_digicam_turntable_medium_sized( crv3d );
  dbdif_data::space_curves_olympus_turntable( crv3d );

  vcl_vector<vcl_vector<dbdif_3rd_order_point_2d> > crv2d;
  dbdif_data::project_into_cams(crv3d, cam, crv2d);
  vcl_cout << "Number of samples INCLUDING epitangencies: " << crv2d[0].size() << vcl_endl;
//  dbdif_data::project_into_cams_without_epitangency(crv3d, cam, crv2d, vnl_math::pi/6.0);
//  vcl_cout << "Number of samples after removal of epipolar tangency: " << crv2d[0].size() << vcl_endl;

  vcl_vector<unsigned> pt_id; //: can be used if you want to select only a subset of the points

  for (unsigned n=0; n < crv2d[0].size(); n+=1)
    pt_id.push_back(n);


  vcl_vector<vgl_point_3d<double> > vgl_pts;

  for (unsigned i=0; i < pt_id.size(); ++i) {
    vgl_pts.push_back(dbdif_data::get_point_crv3d(crv3d,pt_id[i]));
  }

  // -------------------------------------------------------------------------------

  vcl_vector<dbdif_camera> ecam(nviews);
  for (unsigned iv=0; iv < nviews; ++iv)
    ecam[iv].set_p(*(pcam[iv]));

  double dpos_total=0;
  double dtheta_total=0;
//  double dk_total=0;
//  double dkdot_total=0;
  unsigned n_total=0;
  vcl_vector<dbdif_3rd_order_point_2d> pts;
  pts.resize(nviews);
  double dnormal_plus_total=0;
  double dtangential_plus_total=0;
  double dnormal_minus_total=0;
  double dtangential_minus_total=0;

  for (unsigned ip=0; ip < pt_id.size(); ++ip) {

    for (unsigned iv=0; iv < nviews; ++iv) {
      pts[iv] = crv2d[iv][pt_id[ip]];
    }
    
    double dpos, dtheta, dk, dkdot;
    double dnormal_plus;
    double dnormal_minus;
    double dtangential_plus;
    double dtangential_minus;
    unsigned n;

    mw_algo_util::dg_reprojection_error( 
        pts, ecam, dpos, 
        dtheta, 
        dnormal_plus,
        dnormal_minus,
        dtangential_plus,
        dtangential_minus,
        dk, dkdot, n); 

    dpos_total   += dpos;
    dtheta_total += dtheta;
    dnormal_plus_total += dnormal_plus;
    dtangential_plus_total += dtangential_plus;
    dnormal_minus_total += dnormal_minus;
    dtangential_minus_total += dtangential_minus;

    n_total+=n;
  }
  dpos_total = vcl_sqrt(dpos_total);
  dtheta_total = dtheta_total/(double)n_total;
  dtangential_plus_total = vcl_sqrt(dtangential_plus_total);
  dnormal_plus_total = vcl_sqrt(dnormal_plus_total);
  dtangential_minus_total = vcl_sqrt(dtangential_minus_total);
  dnormal_minus_total = vcl_sqrt(dnormal_minus_total);

  vcl_cout << "                   Pos reproj error using DG: " << dpos_total << vcl_endl;
  vcl_cout << "            Theta reproj error using DG(rad): " << dtheta_total << vcl_endl;
  vcl_cout << "      Normal distance (max,min) reproj error: " << dnormal_plus_total << ", " << dnormal_minus_total << vcl_endl;
  vcl_cout << "  Tangential distance (max,min) reproj error: " << dtangential_plus_total << ", " << dtangential_minus_total << vcl_endl;

  TEST_NEAR("Positional reproj error using DG: ",dpos_total,0,1e-6);
  vcl_cout << "#vaid points: " << n_total << vcl_endl;

  // -------------------------------------------------------------------------------


  mw_algo_util::move_world_to_1st_cam(pcam,vgl_pts);




  { // sanity test

    // projection should be good

    double d_total=0;
    for (unsigned ip=0; ip < vgl_pts.size(); ++ip) {

      for (unsigned iv=0; iv < nviews; ++iv) {
        vgl_point_2d<double>p_proj = pcam[iv]->project(vgl_pts[ip]);

        vgl_point_2d<double> p_orig(crv2d[iv][pt_id[ip]].gama[0],crv2d[iv][pt_id[ip]].gama[1]);
        double d=vgl_distance(p_proj,p_orig);
        assert (d < 1e-5);
        d_total += d*d;
      }
    }
    d_total = vcl_sqrt(d_total);
    vcl_cout << "Total reprojection error after change of coords: " << d_total << vcl_endl;
    TEST("Total reprojection error after change of coords",d_total < 1e-5,true);
  }
}

#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>

void test_boost_iostreams()
{
    using namespace std;

    ifstream file("hello.gz", ios_base::in | ios_base::binary);
    boost::iostreams::filtering_istreambuf in;
    in.push(boost::iostreams::gzip_decompressor());
    in.push(file);
    boost::iostreams::copy(in, cout);
}
