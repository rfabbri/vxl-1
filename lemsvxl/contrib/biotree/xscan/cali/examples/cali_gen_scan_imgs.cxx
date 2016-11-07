#include <xscan/xscan_dummy_scan.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_new.h>
#include <cali/cali_simulated_scan_resource.h>
#include <cali/cali_vnl_least_squares_function.h>
#include <cali/cali_param.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

int main(int argc, char* argv[]) {
  if(argc < 2){
    vcl_cerr << "usage " << argv[0] << " <parameter file>\n";
    exit(1);
  }
    vcl_string path = argv[1];
  cali_param par(path);
  vcl_string fname = par.LOGFILE;
  vcl_FILE *fp = vcl_fopen(fname.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  vcl_fclose(fp);
  
  imgr_skyscan_log skyscan_log(fname.data());
  xscan_scan scan;
  scan = skyscan_log.get_scan();
  vcl_cout << scan << vcl_endl;

scan.set_scan_size(par.END - par.START + 1);

  int nviews = scan.scan_size();
  // change the scan
  vpgl_calibration_matrix<double> kk(scan.kk());
  vcl_cout << "scan SIZE---->" << nviews << vcl_endl;
  vcl_vector<vil_image_resource_sptr> resources(nviews);
  for (int i=0; i<nviews; i++) {
       resources[i] = vil_new_image_resource(skyscan_log_header.number_of_columns_,skyscan_log_header.number_of_rows_,1,  VIL_PIXEL_FORMAT_BYTE);
      //  resources[i] = vil_new_image_resource(2000,1048,1,  VIL_PIXEL_FORMAT_BYTE);
  //        //scan.kk().principal_point().y()*2,  1,  VIL_PIXEL_FORMAT_BYTE);
    }



/*
  double ball_indent;
  double ball_big_indent;
  double ball_distance;
  */
  vnl_quaternion<double> artf_rot; 
  vgl_point_3d<double> artf_trans;
  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_trans;
  double x_scale;
  double y_scale;
  //unused variable double xy_scale;
  vgl_point_2d<double> princp_point;

  
  // read the x values from the text file
  vcl_string txt_file = par.CONVERGEDVALUES;
  vcl_cout << "reading in values from " <<  txt_file << "\n";
  vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);
   vnl_vector<double> x(par.SIZE_OF_X);

  double val;
  char val_string[256];
  int x_size=0;
  while (!fstream.eof()) {
    fstream.getline(val_string, 256);
    val = vcl_atof(val_string);
    x[x_size++] = val;
    vcl_cout << x_size << "=" << val << vcl_endl;
  }


  /*
  cali_vnl_least_squares_function::lsqr_fn_params(x, 
    ball_indent, ball_big_indent, 
    ball_distance, artf_rot,
    artf_trans,turn_table_rot,turn_table_trans,                     
    x_scale,y_scale,xy_scale,princp_point);
*/
 /* cali_vnl_least_squares_function::lsqr_fn_params(x, artf_rot,
    artf_trans,turn_table_rot,turn_table_trans,                     
    x_scale,y_scale,xy_scale,princp_point);*/
  cali_vnl_least_squares_function::gen_scan_lsqr_fn_params(x, artf_rot,
    artf_trans,turn_table_rot,turn_table_trans,                     
    x_scale,y_scale,princp_point);

  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);
  kk.set_principal_point(princp_point);
  scan.set_calibration_matrix(kk);

  //vnl_quaternion<double> turn_table_rot(-0.880907,0.160384,-0.157736,0.875526);
  turn_table_rot.normalize();
  //vnl_double_3 turn_table_trans(0.510952,5.18166,151.153);
  xscan_orbit_base_sptr orbit_base = (scan.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                        orbit.t(),
                        turn_table_rot,
                        turn_table_trans);
  scan.set_orbit(new_orbit);

  /*
  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan, resources, 
    ball_indent, ball_big_indent, ball_distance);

    */

  //vcl_vector<double>x_coord_tol(0,0,-0.0947263,0.0404068,0.050888,0.296285,0.286996,0.109258,0.125204,-0.103531,
     // -0.0743285,0,0);
  //vcl_vector<double>y_coord_tol(0,0,0.00245647,0.0188508,-0.183153,-0.376836,-0.182137,-0.164995,0.128703,0.243105,
     // -0.00153744,0,0);
  //vcl_vector<double>z_coord_tol(0,0,0,0,0,0,0,0,0,0,0,0,0);
  //vcl_vector<double>rad_tol(0,0,0,0,0,0,0,0,0,0,0,0,0);

  vcl_vector<double>x_coord_tol;

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

   vcl_vector<double>y_coord_tol;

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

   vcl_vector<double>z_coord_tol;
   vcl_vector<double>rad_tol;

   for (int i = 0;i<13;i++)
   {
       z_coord_tol.push_back(0);
       rad_tol.push_back(0);
   }


      

 // cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan,resources,par);
 cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan,resources,par,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);
   
  //vnl_vector<double> v(3);
  //v[0] = 0;
  //v[1] = 0;
  //v[2] = 1;
  //double theta = (vnl_math::pi/2);//90 degrees
  //double theta = -2.25503732174851;
  //vnl_quaternion<double> artf_rot(v, 0); // -1.35);
  //vnl_quaternion<double> artf_rot(0.00394938, -0.00698621, 0.330063, 1.44456);
  //artf_rot.normalize();
  //vgl_point_3d<double> artf_tr(0, 0, -14*cali_cylinder_artifact::height/16);
  //vgl_point_3d<double> artf_trans(-0.437369, -3.6068, -49.6446);  //17.6695971242095, 0.0545319300998807, -57.8451903241621);
  vcl_cout << "Rotation---------------------------->" << vcl_endl;
  vcl_cout << artf_rot << vcl_endl;
  vcl_cout << "Translation------------------------->" << vcl_endl;
  vcl_cout << artf_trans << vcl_endl;
//  vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C:\\test_images\\result_two_indent", "test_artifact", artf_rot, artf_trans,22);
// vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C:\\march_03\\artifact35um\\tolerance_study\\test_images", "test_images", artf_rot, artf_trans,par.INTERVAL);
//vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C:\\latest_amherst\\upside_down_artf_test_images", "test_images", artf_rot, artf_trans,par.INTERVAL);
// vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan("C:\\march_03\\artifact17um\\test_images", "test_images", artf_rot, artf_trans,par.INTERVAL);
 vcl_vector<vil_image_resource_sptr> images = scan_res.simulate_scan(".", "test_images", artf_rot, artf_trans,par.INTERVAL);
}

