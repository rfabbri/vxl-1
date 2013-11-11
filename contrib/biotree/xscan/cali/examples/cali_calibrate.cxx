#include <vcl_cstdio.h>
#include <vnl/vnl_math.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <cali/cali_artf_corresponder.h>
#include <cali/cali_cylinder_artifact.h>
#include <cali/cali_calibrator.h>
#include <xscan/xscan_uniform_orbit.h>

void set_tolerances(   vcl_vector<double>& x_coord_tol,
    vcl_vector<double>& y_coord_tol,
    vcl_vector<double>& z_coord_tol,
    vcl_vector<double>& rad_tol);

int main(int argc, char* argv[]) 
{
  if(argc < 2){
    vcl_cerr << "usage: " << argv[0] << " <parameter file>\n";
    exit(1);
  }

  cali_param par(argv[1]);
  cali_calibrator calib(par);

  // create the artifact rotation, translation for the initial position
  /* vnl_quaternion<double>artf_rot(-0.000553801,0.00202034,0.895235,-0.445825);
     vgl_point_3d<double> artf_tr(-0.0347696,-0.178137,-5.88102);
  */


//  vnl_quaternion<double> artf_rot(0.0,0.0,3*M_PI/4. - 1.1); 
//  vnl_quaternion<double> artf_rot(0,0,0.587606214074162,0.809147042991217 );
//  vgl_point_3d<double>  artf_tr(0,0,-6.8);
  vnl_quaternion<double> artf_rot(par.PARAM[0],par.PARAM[1],par.PARAM[2]);
  vgl_point_3d<double>  artf_tr(par.PARAM[4],par.PARAM[5],par.PARAM[6]);


  /*
  vnl_quaternion<double> artf_rot(  -0.00231673, -0.000147629, 0.584657, 0.840427);
  vgl_point_3d<double>  artf_tr(-0.0869239,0.173656, -7.46849);
  */

  vcl_cout << artf_rot << "\n";
  vcl_cout << "rotation axis: " << artf_rot.axis() << "\n";
  vcl_cout << "rotation angle: " << artf_rot.angle()*180./M_PI << "\n";

  vcl_vector<double>x_coord_tol;
  vcl_vector<double>y_coord_tol;
  vcl_vector<double>z_coord_tol;
  vcl_vector<double>rad_tol;

  set_tolerances(x_coord_tol, y_coord_tol, z_coord_tol, rad_tol);

//  calib.calibrate_with_tolerance(artf_rot,artf_tr,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol); 

  calib.calibrate_in_two_stages(artf_rot,artf_tr,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

//    calib.calibrate_user_interaction(artf_rot,artf_tr,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);



  return 0;

}

void set_tolerances(   vcl_vector<double>& x_coord_tol,
    vcl_vector<double>& y_coord_tol,
    vcl_vector<double>& z_coord_tol,
    vcl_vector<double>& rad_tol)
{

  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(-0.0947263);
  x_coord_tol.push_back(0.0404068);
  x_coord_tol.push_back(0.050888);
  x_coord_tol.push_back(0.296285);
  x_coord_tol.push_back(0.286996);
  x_coord_tol.push_back(0.109258);
  x_coord_tol.push_back(0.125204);
  x_coord_tol.push_back(-0.103531);
  x_coord_tol.push_back(-0.0743285);
  x_coord_tol.push_back(0);
  x_coord_tol.push_back(0);


  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0.00245647);
  y_coord_tol.push_back(0.0188508);
  y_coord_tol.push_back(-0.183153);
  y_coord_tol.push_back(-0.376836);
  y_coord_tol.push_back(-0.182137);
  y_coord_tol.push_back(-0.164995);
  y_coord_tol.push_back(0.128703);
  y_coord_tol.push_back(0.243105);
  y_coord_tol.push_back(-0.00153744);
  y_coord_tol.push_back(0);
  y_coord_tol.push_back(0);


  for (int i = 0;i<13;i++)
  {
    z_coord_tol.push_back(0);
    rad_tol.push_back(0);
  }
}
