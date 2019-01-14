#include <cali/cali_calibrator.h>
#include <cali/cali_simulated_scan_resource.h>
#include <cali/cali_vnl_least_squares_function.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstddef>
#include <cstdlib>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <cali/cali_artf_corresponder.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_smart_ptr.h>

cali_calibrator::cali_calibrator(cali_param par) :par_(par),
                                                  scan_interval_(par.INTERVAL),
                                                  scan_log_file_(par.LOGFILE), 
                                                  conics_bin_file_base_(par.CONICS_BIN_FILE_BASE), 
                                                  artf_imgs_num_(par.END - par.START + 1)
{ 
  // create the scan information
  create_scan(scan_log_file_);

  // create the resources
  //resources_.resize(scan_.n_views());
  for (unsigned i=0; i<scan_.n_views(); i+=scan_interval_) {
    resources_.push_back(vil_new_image_resource((unsigned) (2.0*scan_.kk().principal_point().x()), 
          (unsigned) (2.0*scan_.kk().principal_point().y()),  1,  VIL_PIXEL_FORMAT_BYTE));
  }
}

void cali_calibrator::calibrate_user_interaction(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,std::vector<double> x_coord_tol,std::vector<double> y_coord_tol,std::vector<double> z_coord_tol,std::vector<double> rad_tol)
 {

  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan_, resources_,par_);
  cali_artf_corresponder corr;
  vnl_vector<double> x1;
  // in stage 1,the 7 artifact parameters along with the principal point are varied ,thus making the total number of 
  // parameters as 9
  par_.NUM_OF_PARAMETERS = 9;
  //keeping the artifact parameters active
  int i;
  for (i = 0;i<7;i++) 
  {
          par_.SETPARAM[i] = 1;
  }

  //freezing the x-scale & the y-scale
  par_.SETPARAM[i++] = 0;
  par_.SETPARAM[i++] = 0;

  //activating the principal point x & y coordinates
  par_.SETPARAM[i++] = 1;
  par_.SETPARAM[i++] = 1;

  //freezing the parameters corresponding to turn table translation & rotation as well as the x,y,z & the rad tolerances
  //for the CMM coordinates...the max number of parameters that can be varied is 70 

  for (;i<70;i++)
  {
          par_.SETPARAM[i] = 0;
  }

  x1.set_size(par_.NUM_OF_PARAMETERS);
  cali_vnl_least_squares_function lsqr_fn1(par_,
                  resources_,
                  scan_,
                  conics_bin_file_base_,
                  scan_interval_,
                  par_.END - par_.START );

  lsqr_fn1.set_lsqr_fn_params_with_tolerance(x1,
                  artf_rot,
                  artf_tr,
                  scan_,
                  x_coord_tol,
                  y_coord_tol,
                  z_coord_tol,
                  rad_tol);



  std::cout << "X values before--> " << std::endl;
  for (unsigned k=0; k<x1.size(); k++) {
          std::cout << k << "=" << x1[k] << std::endl;
  }


  vnl_levenberg_marquardt lm1(lsqr_fn1);
  std::cerr << "x tolerance : " << lm1.get_x_tolerance() << "\n";
  lm1.set_x_tolerance(lm1.get_x_tolerance()*1e4);
  std::cerr << "set new x tolerance : " << lm1.get_x_tolerance() << "\n";
  std::cerr << " : " << lm1.get_x_tolerance() << "\n";
  /*
     std::cerr << "f tolerance : " << lm1.get_f_tolerance() << "\n";
     lm1.set_f_tolerance(lm1.get_f_tolerance()*1e1);
     std::cerr << "set new f tolerance : " << lm1.get_f_tolerance() << "\n";
     std::cerr << "max_function_evals : " << lm1.get_max_function_evals() << "\n";
     */

  /*
     lm1.set_max_function_evals(5000);
     std::cerr << "set new max_function_evals : " << lm1.get_max_function_evals() << "\n";
     */



  // lm.set_x_tolerance(lm.get_x_tolerance());
  lm1.set_trace(true);
  lm1.minimize(x1);
  lm1.diagnose_outcome(std::cout);

  std::string txt_file1 = par_.CONVERGEDVALUES_STAGE1;
  std::ofstream fstream1(txt_file1.c_str());
  std::cout << " minimization ended" << std::endl;
  std::cout << "X values after--> " << std::endl;

  for (unsigned k=0; k<x1.size(); k++) {
          std::cout << k << "=" << x1[k] << std::endl;
          fstream1 << x1[k] << "\n";
  }

  std::cerr << "Error=" << lm1.get_end_error() << std::endl;
  fstream1 << "Error=" << lm1.get_end_error() << std::endl;
  std::cerr << "num_iterations_" << lm1.get_num_iterations()<< std::endl;
  fstream1 << "num_iterations_" << lm1.get_num_iterations()<< std::endl;
  fstream1.close();

  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_tr;
  double x_scale, y_scale;
  vgl_point_2d<double> princp_p;

  lsqr_fn1.lsqr_fn_params_with_tolerance(x1, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, princp_p,
                  x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

  vpgl_calibration_matrix<double> kk1(scan_.kk());
  kk1.set_x_scale(x_scale);
  kk1.set_y_scale(y_scale);


  kk1.set_principal_point(princp_p); 
  scan_.set_calibration_matrix(kk1);

  par_.NUM_OF_PARAMETERS = 18;

  //keeping the artifact parameters active

  for (i = 0;i<18;i++) 
  {
          par_.SETPARAM[i] = 1;
  }

  // freezing the rest all parameters
  for (;i<70;i++)
  {
          par_.SETPARAM[i] = 0;
  }

  char cont = 'y';

  while(cont=='y'){
          for(int i = 0; i < par_.BALL_NUMBER; i++){
                  std::cerr << "Use Ball " << i+1 << ": " << par_.SETMASK[i] << "\n";
          }
          char change;
          std::cerr << "Change ?  y/n";
          std::cin >> change;
          if(change == 'y'){
                  int on_off = 0;
                  int ncorr = 0;
                  for(int i = 0; i < par_.BALL_NUMBER; i++){
                          std::cerr << "Use Ball " << i+1 << ": 1/0 ?";
                          std::cin >> on_off;
                          par_.SETMASK[i] = on_off;
                          if(on_off > 0) ncorr++;
                  }
                  par_.NUM_OF_CORRESP = ncorr;
          }

          // in stage 2,the 18 parameters are varied which correspond to artifact rotation,translation, the turn table rotation,
          // turn table translation, the principal point, the x-scale & the y-scale 
          vnl_vector<double> x2;


          x2.set_size(par_.NUM_OF_PARAMETERS);
          cali_vnl_least_squares_function lsqr_fn2(par_,resources_, scan_, conics_bin_file_base_, scan_interval_, par_.END - par_.START ); 
          lsqr_fn2.set_lsqr_fn_params_with_tolerance(x2, artf_rot, artf_tr, scan_,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);


          std::cerr << "X values before--> " << std::endl;
          for (unsigned k=0; k<x2.size(); k++) {
                  std::cerr << k << "=" << x2[k] << std::endl;
          }


          vnl_levenberg_marquardt lm2(lsqr_fn2);

          std::cerr << "x tolerance : " << lm2.get_x_tolerance() << "\n";
          lm2.set_x_tolerance(lm2.get_x_tolerance()*1e3);
          std::cerr << "set new x tolerance : " << lm2.get_x_tolerance() << "\n";

          // lm.set_x_tolerance(lm.get_x_tolerance());
          lm2.set_trace(true);
          lm2.minimize(x2);
          lm2.diagnose_outcome(std::cout);

          std::string txt_file2 = par_.CONVERGEDVALUES_STAGE2;
          std::ofstream fstream2(txt_file2.c_str());
          std::cout << " minimization ended" << std::endl;
          std::cerr << "X values after--> " << std::endl;

          for (unsigned k=0; k<x2.size(); k++) {
                  std::cerr << k << "=" << x2[k] << std::endl;
                  fstream2 << x2[k] << "\n";
          }


          std::cerr << "Error=" << lm2.get_end_error() << std::endl;
          fstream2 << "Error=" << lm2.get_end_error() << std::endl;
          std::cerr << "num_iterations_" << lm2.get_num_iterations()<< std::endl;
          fstream2 << "num_iterations_" << lm2.get_num_iterations()<< std::endl;

          fstream2.close();
          lsqr_fn2.lsqr_fn_params_with_tolerance(x2, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, princp_p,
                          x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);


          vpgl_calibration_matrix<double> kk2(scan_.kk());
          kk2.set_x_scale(x_scale);
          kk2.set_y_scale(y_scale);

          kk2.set_principal_point(princp_p);

          scan_.set_calibration_matrix(kk2);


          xscan_orbit_base_sptr orbit_base = (scan_.orbit());
          xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
          xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
                          orbit.t(),
                          turn_table_rot,
                          turn_table_tr);
          scan_.set_orbit(new_orbit);


          std::cerr << "Continue ?  y/n";
          std::cin >> cont;
  }



  std::ofstream ofst((par_.OUTPUT_SCANFILE).c_str());
  ofst<<scan_<<std::endl;




}

void cali_calibrator::calibrate_in_two_stages(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,
    std::vector<double> x_coord_tol,std::vector<double> y_coord_tol,
    std::vector<double> z_coord_tol,std::vector<double> rad_tol) 
{

  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan_, resources_,par_);

  cali_artf_corresponder corr;


  // cali_vnl_least_squares_function lsqr_fn(par_,resources_, scan_, conics_bin_file_base_, scan_interval_, par_.END - par_.START );
  vnl_vector<double> x1;

  // in stage 1,the 7 artifact parameters along with the principal point are varied ,thus making the total number of 
  // parameters as 9

  par_.NUM_OF_PARAMETERS = 9;

  //keeping the artifact parameters active
  int i;
  for (i = 0;i<7;i++) 
  {
    par_.SETPARAM[i] = 1;
  }

  //freezing the x-scale & the y-scale
  par_.SETPARAM[i++] = 0;
  par_.SETPARAM[i++] = 0;

  //activating the principal point x & y coordinates
  par_.SETPARAM[i++] = 1;
  par_.SETPARAM[i++] = 1;

  //freezing the parameters corresponding to turn table translation & rotation as well as the x,y,z & the rad tolerances
  //for the CMM coordinates...the max number of parameters that can be varied is 70 

  for (;i<70;i++)
  {
    par_.SETPARAM[i] = 0;
  }

  x1.set_size(par_.NUM_OF_PARAMETERS);
  cali_vnl_least_squares_function lsqr_fn1(par_,
      resources_,
      scan_,
      conics_bin_file_base_,
      scan_interval_,
      par_.END - par_.START );

  lsqr_fn1.set_lsqr_fn_params_with_tolerance(x1,
      artf_rot,
      artf_tr,
      scan_,
      x_coord_tol,
      y_coord_tol,
      z_coord_tol,
      rad_tol);

  std::string txt_file1 = par_.CONVERGEDVALUES_STAGE1;
  std::ofstream fstream1(txt_file1.c_str());


  std::cout << "X values before--> " << std::endl;
  for (unsigned k=0; k<x1.size(); k++) {
    std::cout << k << "=" << x1[k] << std::endl;
  }


  vnl_levenberg_marquardt lm1(lsqr_fn1);
  std::cerr << "x tolerance : " << lm1.get_x_tolerance() << "\n";
  lm1.set_x_tolerance(lm1.get_x_tolerance()*1e4);
  std::cerr << "set new x tolerance : " << lm1.get_x_tolerance() << "\n";
  std::cerr << "setting max function evals to 100\n";
//  lm1.set_max_function_evals(100);
  /*
  std::cerr << "f tolerance : " << lm1.get_f_tolerance() << "\n";
  lm1.set_f_tolerance(lm1.get_f_tolerance()*1e1);
  std::cerr << "set new f tolerance : " << lm1.get_f_tolerance() << "\n";
  std::cerr << "max_function_evals : " << lm1.get_max_function_evals() << "\n";
  */

  /*
  lm1.set_max_function_evals(5000);
  std::cerr << "set new max_function_evals : " << lm1.get_max_function_evals() << "\n";
  */



  // lm.set_x_tolerance(lm.get_x_tolerance());
  lm1.set_trace(true);
  lm1.minimize(x1);
  lm1.diagnose_outcome(std::cout);

  std::cout << " minimization ended" << std::endl;
  std::cout << "X values after--> " << std::endl;

  for (unsigned k=0; k<x1.size(); k++) {
    std::cout << k << "=" << x1[k] << std::endl;
//    fstream1 << x1[k] << "\n";
  }
  int count = 0;
  for (unsigned k=0; k<18; k++) {
    if(par_.SETPARAM[k]){
      fstream1 << x1[count++] << "\n";
    }
    else{
      fstream1 << par_.PARAM[k] << "\n";
    }
  }



  std::cout << "Error=" << lm1.get_end_error() << std::endl;
  fstream1 << "Error=" << lm1.get_end_error() << std::endl;
  std::cout << "num_iterations_" << lm1.get_num_iterations()<< std::endl;
  fstream1 << "num_iterations_" << lm1.get_num_iterations()<< std::endl;

  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_tr;
  double x_scale, y_scale;
  vgl_point_2d<double> princp_p;

  lsqr_fn1.lsqr_fn_params_with_tolerance(x1, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, princp_p,
      x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

  vpgl_calibration_matrix<double> kk1(scan_.kk());
  kk1.set_x_scale(x_scale);
  kk1.set_y_scale(y_scale);


  kk1.set_principal_point(princp_p);

  scan_.set_calibration_matrix(kk1);




  fstream1.close();

  // in stage 2,the 18 parameters are varied which correspond to artifact rotation,translation, the turn table rotation,
  // turn table translation, the principal point, the x-scale & the y-scale 
  vnl_vector<double> x2;

  par_.NUM_OF_PARAMETERS = 13;

  //keeping the artifact parameters active

  for (i = 0;i<18;i++) 
  {
    par_.SETPARAM[i] = 1;
  }

  //hold the turntable rotation constant
  par_.SETPARAM[11] = 0;
  par_.SETPARAM[12] = 0;
  par_.SETPARAM[13] = 0;
  par_.SETPARAM[14] = 0;
  // force the y-component of turntable translation to be zero
  par_.SETPARAM[16] = 0;

  // freezing the rest all parameters
  for (;i<70;i++)
  {
    par_.SETPARAM[i] = 0;
  }

  x2.set_size(par_.NUM_OF_PARAMETERS);
  cali_vnl_least_squares_function lsqr_fn2(par_,resources_, scan_, conics_bin_file_base_, scan_interval_, par_.END - par_.START );

  lsqr_fn2.set_lsqr_fn_params_with_tolerance(x2, artf_rot, artf_tr, scan_,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);



  std::string txt_file2 = par_.CONVERGEDVALUES_STAGE2;
  std::ofstream fstream2(txt_file2.c_str());


  std::cout << "X values before--> " << std::endl;
  for (unsigned k=0; k<x2.size(); k++) {
    std::cout << k << "=" << x2[k] << std::endl;
  }


  vnl_levenberg_marquardt lm2(lsqr_fn2);

  std::cerr << "x tolerance : " << lm2.get_x_tolerance() << "\n";
  lm2.set_x_tolerance(lm2.get_x_tolerance()*1e3);
  std::cerr << "set new x tolerance : " << lm2.get_x_tolerance() << "\n";

  // lm.set_x_tolerance(lm.get_x_tolerance());
  lm2.set_trace(true);
  lm2.minimize(x2);
  lm2.diagnose_outcome(std::cout);

  std::cout << " minimization ended" << std::endl;
  std::cout << "X values after--> " << std::endl;

  for (unsigned k=0; k<x2.size(); k++) {
    std::cout << k << "=" << x2[k] << std::endl;
  }
  count = 0;
  for (unsigned k=0; k<18; k++) {
    if(par_.SETPARAM[k]){
      fstream2 << x2[count++] << "\n";
    }
    else{
      fstream2 << par_.PARAM[k] << "\n";
    }
  }

  std::cout << "Error=" << lm2.get_end_error() << std::endl;
  fstream2 << "Error=" << lm2.get_end_error() << std::endl;
  std::cout << "num_iterations_" << lm2.get_num_iterations()<< std::endl;
  fstream2 << "num_iterations_" << lm2.get_num_iterations()<< std::endl;

  lsqr_fn2.lsqr_fn_params_with_tolerance(x2,
                                         artf_rot,
                                         artf_tr,
                                         turn_table_rot,
                                         turn_table_tr,
                                         x_scale,
                                         y_scale,
                                         princp_p, 
                                         x_coord_tol,
                                         y_coord_tol,
                                         z_coord_tol,
                                         rad_tol);


  vpgl_calibration_matrix<double> kk2(scan_.kk());
  kk2.set_x_scale(x_scale);
  kk2.set_y_scale(y_scale);

  kk2.set_principal_point(princp_p);

  scan_.set_calibration_matrix(kk2);


  xscan_orbit_base_sptr orbit_base = (scan_.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
      orbit.t(),
      turn_table_rot,
      turn_table_tr);
  scan_.set_orbit(new_orbit);

  // std::ofstream ofst("C:\\proc-files\\cali_gen_scan_file\\cali_scan.scn");
  std::ofstream ofst((par_.OUTPUT_SCANFILE).c_str());
  ofst<<scan_<<std::endl;


  fstream2.close();


}

void cali_calibrator::create_scan(std::string fname) {


  std::cout << "creating scan from " << fname << "\n";
  std::FILE *fp = std::fopen(fname.data(),"r");
  assert(fp != NULL);
  imgr_skyscan_log_header skyscan_log_header(fp);
  std::fclose(fp);

  imgr_skyscan_log skyscan_log(fname.data());
  scan_ = skyscan_log.get_scan();
  scan_.set_scan_size(par_.END - par_.START + 1);
  std::cout << scan_ << std::endl;


  /* std::ifstream ifile(fname.c_str());

     ifile >> scan_;
     std::cout << scan_ << std::endl;
     scan_.set_scan_size(par_.END);*/

}
void cali_calibrator::calibrate(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr) 
{

  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan_, resources_,par_);

  cali_artf_corresponder corr;

  cali_vnl_least_squares_function lsqr_fn(par_,resources_, scan_, conics_bin_file_base_, scan_interval_, artf_imgs_num_);
  vnl_vector<double> x;
  x.set_size(par_.NUM_OF_PARAMETERS);


  lsqr_fn.set_lsqr_fn_params(x, artf_rot, artf_tr, scan_);



  std::string txt_file = par_.CONVERGEDVALUES;
  std::ofstream fstream(txt_file.c_str());


  std::cout << "X values before--> " << std::endl;
  for (unsigned k=0; k<x.size(); k++) {
    std::cout << k << "=" << x[k] << std::endl;
  }


  vnl_levenberg_marquardt lm(lsqr_fn);

  lm.set_x_tolerance(lm.get_x_tolerance()*1e4);
  // lm.set_x_tolerance(lm.get_x_tolerance());
  lm.set_trace(true);
  lm.minimize(x);
  lm.diagnose_outcome(std::cout);

  std::cout << " minimization ended" << std::endl;
  std::cout << "X values after--> " << std::endl;

  for (unsigned k=0; k<x.size(); k++) {
    std::cout << k << "=" << x[k] << std::endl;
    fstream << x[k] << "\n";
  }

  std::cout << "Error=" << lm.get_end_error() << std::endl;
  fstream << "Error=" << lm.get_end_error() << std::endl;
  std::cout << "num_iterations_" << lm.get_num_iterations()<< std::endl;
  fstream << "num_iterations_" << lm.get_num_iterations()<< std::endl;

  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_tr;
  double x_scale, y_scale;
  vgl_point_2d<double> princp_p;

  lsqr_fn.lsqr_fn_params(x, artf_rot, artf_tr, turn_table_rot, turn_table_tr, x_scale, y_scale, princp_p);


  fstream.close();
}

void cali_calibrator::calibrate_with_tolerance(vnl_quaternion<double> artf_rot, 
    vgl_point_3d<double> artf_tr,
    std::vector<double> x_coord_tol,
    std::vector<double> y_coord_tol,
    std::vector<double> z_coord_tol,
    std::vector<double> rad_tol) 
{

  cali_simulated_scan_resource scan_res = cali_simulated_scan_resource(scan_, resources_,par_);

  cali_artf_corresponder corr;

  cali_vnl_least_squares_function lsqr_fn(par_,
      resources_,
      scan_,
      conics_bin_file_base_,
      scan_interval_,
      artf_imgs_num_);
  vnl_vector<double> x;


  x.set_size(par_.NUM_OF_PARAMETERS);


  lsqr_fn.set_lsqr_fn_params_with_tolerance(x, artf_rot, artf_tr, scan_,x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);



  std::string txt_file = par_.CONVERGEDVALUES;
  std::ofstream fstream(txt_file.c_str());


  std::cout << "X values before--> " << std::endl;
  for (unsigned k=0; k<x.size(); k++) {
    std::cout << k << "=" << x[k] << std::endl;
  }


  std::cout << "constructing vnl_levenberg_marquardt " << std::endl;
  vnl_levenberg_marquardt lm(lsqr_fn);

  lm.set_x_tolerance(lm.get_x_tolerance()*1e4);
  // lm.set_x_tolerance(lm.get_x_tolerance());
  lm.set_trace(true);
  std::cout << "calling minimize " << std::endl;
  lm.minimize(x);
  std::cout << "calling diagnose_outcome " << std::endl;
  lm.diagnose_outcome(std::cout);

  std::cout << " minimization ended" << std::endl;
  std::cout << "X values after--> " << std::endl;

  for (unsigned k=0; k<x.size(); k++) {
    std::cout << k << "=" << x[k] << std::endl;
    //  fstream << x[k] << "\n";
  }


  std::cout << "Error=" << lm.get_end_error() << std::endl;
  std::cout << "num_iterations_" << lm.get_num_iterations()<< std::endl;

  vnl_quaternion<double> turn_table_rot;
  vnl_double_3 turn_table_tr;
  double x_scale, y_scale;
  vgl_point_2d<double> princp_p;

  lsqr_fn.lsqr_fn_params_with_tolerance(x, 
      artf_rot, artf_tr, 
      turn_table_rot, turn_table_tr, 
      x_scale, y_scale, princp_p,
      x_coord_tol,y_coord_tol,z_coord_tol,rad_tol);

  fstream << artf_rot.x() << "\n";
  fstream << artf_rot.y() << "\n";
  fstream << artf_rot.z() << "\n";
  fstream << artf_rot.r() << "\n";
  fstream << artf_tr.x() << "\n";
  fstream << artf_tr.y() << "\n";
  fstream << artf_tr.z() << "\n";
  fstream << x_scale << "\n";
  fstream << y_scale << "\n";
  fstream << princp_p.x() << "\n";
  fstream << princp_p.y() << "\n";
  fstream << turn_table_rot.x() << "\n";
  fstream << turn_table_rot.y() << "\n";
  fstream << turn_table_rot.z() << "\n";
  fstream << turn_table_rot.r() << "\n";
  fstream << turn_table_tr[0] << "\n";
  fstream << turn_table_tr[1] << "\n";
  fstream << turn_table_tr[2] << "\n";
  fstream << "Error=" << lm.get_end_error() << std::endl;
  fstream << "num_iterations_" << lm.get_num_iterations()<< std::endl;



  vpgl_calibration_matrix<double> kk(scan_.kk());
  kk.set_x_scale(x_scale);
  kk.set_y_scale(y_scale);


  kk.set_principal_point(princp_p);

  scan_.set_calibration_matrix(kk);


  xscan_orbit_base_sptr orbit_base = (scan_.orbit());
  xscan_uniform_orbit orbit = static_cast<const xscan_uniform_orbit&>(*orbit_base);
  xscan_orbit_base_sptr new_orbit = new xscan_uniform_orbit(orbit.rot(),
      orbit.t(),
      turn_table_rot,
      turn_table_tr);
  scan_.set_orbit(new_orbit);

  // std::ofstream ofst("C:\\proc-files\\cali_gen_scan_file\\cali_scan.scn");
  std::ofstream ofst((par_.OUTPUT_SCANFILE).c_str());
  ofst<<scan_<<std::endl;


  fstream.close();
}


cali_calibrator::cali_calibrator(std::string scan_log_file, 
    std::string conics_bin_file_base, 
    int artf_imgs_num,
    int interval=1)
:scan_interval_(interval),scan_log_file_(scan_log_file), conics_bin_file_base_(conics_bin_file_base), 
  artf_imgs_num_(artf_imgs_num) 
{ 
  // create the scan information
  create_scan(scan_log_file);

  // create the resources
  //resources_.resize(scan_.n_views());
  for (unsigned i=0; i<scan_.n_views(); i+=interval) {
    resources_.push_back(vil_new_image_resource((unsigned) (2.0*scan_.kk().principal_point().x()), 
          (unsigned) (2.0*scan_.kk().principal_point().y()),  1,  VIL_PIXEL_FORMAT_BYTE));
  }
}
cali_calibrator::cali_calibrator(std::string scan_log_file, 
    std::string conics_bin_file_base, 
    int artf_imgs_num,
    cali_param par,
    int interval=1)
:par_(par),scan_interval_(interval),scan_log_file_(scan_log_file), conics_bin_file_base_(conics_bin_file_base), 
  artf_imgs_num_(artf_imgs_num)
{ 
  // create the scan information
  create_scan(scan_log_file);

  // create the resources
  //resources_.resize(scan_.n_views());
  for (unsigned i=0; i<scan_.n_views(); i+=interval) {
    resources_.push_back(vil_new_image_resource((unsigned) (2.0*scan_.kk().principal_point().x()), 
          (unsigned) (2.0*scan_.kk().principal_point().y()),  1,  VIL_PIXEL_FORMAT_BYTE));
  }
}



