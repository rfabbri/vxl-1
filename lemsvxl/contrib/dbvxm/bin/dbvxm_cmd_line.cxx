// classify shock patch sets

#include "dbvxm_cmd_line.h"

#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vil/vil_plane.h>
#include <vil/vil_convert.h>
#include <brip/brip_vil_float_ops.h>
#include <vnl/vnl_math.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <bvgl/bvgl_changes.h>
#include <bvgl/bvgl_changes_sptr.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <bvgl/bvgl_change_obj.h>
#include <bil/algo/bil_roi_mask.h>
#include <bsol/bsol_algs.h>
#include <vsol/vsol_box_2d.h>
#include <bsta/bsta_joint_histogram.h>

#include <bvxm/rec/bvxm_param_estimation.h>
#include <bvxm/rec/bvxm_glitch.h>
#include <bvxm/rec/bvxm_fg_bg_pair_density.h>

#include <core/vil_pro/vil_io_image_view_base.h>
#include <bsta/algo/bsta_fit_weibull.h>
#include <bsta/bsta_histogram.h>

vpgl_perspective_camera<double> generate_camera(double angle1, double angle2) 
{
  const int IMAGE_U = 256;
  const int IMAGE_V = 256;
  const double x_scale = 900;
  const double y_scale = 900;
  const double focal_length = 1.;

  vgl_point_2d<double> principal_point(IMAGE_U/2., IMAGE_V/2.);

  vgl_point_3d<double> centroid(0,0,0);
  //std::cout << "centroid: " << centroid << std::endl;
  double camera_dist = 10;
  //vgl_point_3d<double> camera_center_vec(0, 0, camera_dist);
  vgl_vector_3d<double> camera_center_vec(0, 0, 1);  
  vgl_point_3d<double> camera_center_point(0,0,1);

  //: now rotate this center vector,
  // first around y-axis (in x-z plane) by angle1
  vgl_h_matrix_3d<double> T;
  T.set_identity();
  //: roll: about z-axis; pitch: about y-axis; yaw: about x-axis
  //  angle1 is around y-axis and angle2 is around z-axis
  //T.set_rotation_roll_pitch_yaw(angle2, 0.0, angle1);
                               //yaw   //pitch //roll
  T.set_rotation_roll_pitch_yaw(angle1,   0.0,    angle2);
  //vgl_homg_point_3d<double> camera_center_vec_h(camera_center_vec);
  vgl_homg_point_3d<double> camera_center_vec_h(camera_center_point);
  vgl_homg_point_3d<double> cam_cent = T(camera_center_vec_h);

  camera_center_point = vgl_point_3d<double>(cam_cent.x()/cam_cent.w(), cam_cent.y()/cam_cent.w(), cam_cent.z()/cam_cent.w());
  camera_center_vec = vgl_vector_3d<double>(camera_center_point.x(), camera_center_point.y(), camera_center_point.z());
  camera_center_vec = normalize(camera_center_vec);
  camera_center_vec = camera_dist*camera_center_vec;  
  std::cout << camera_center_vec.x() << " " << camera_center_vec.y() << " " << camera_center_vec.z() << "\n";

  vpgl_perspective_camera<double> persp_cam;

  //generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
  vpgl_calibration_matrix<double> K(focal_length, principal_point, x_scale, y_scale);
  persp_cam.set_calibration(K);
  vgl_point_3d<double> camera_center_point2(camera_center_vec.x(), camera_center_vec.y(), camera_center_vec.z());
  std::cout << camera_center_point2.x() << " " << camera_center_point2.y() << " " << camera_center_point2.z() << "\n";
  persp_cam.set_camera_center(camera_center_point2);

  persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  return persp_cam;
}


//: also prepares the command file
bool prepare_cameras_for_eth80(std::string output_prefix)
{
  if (output_prefix.compare("") == 0) {
    std::cout << "output_prefix not specified\n";
    return false;
  }

  std::vector<std::pair<int, int> > angles(26, std::pair<int, int>(0,0));
  angles[0] = std::pair<int, int>(0,0);
  angles[1] = std::pair<int, int>(22,0);
  angles[2] = std::pair<int, int>(22,90);
  angles[3] = std::pair<int, int>(22,180);
  angles[4] = std::pair<int, int>(22,270);
  angles[5] = std::pair<int, int>(35,45);
  angles[6] = std::pair<int, int>(35,135);
  angles[7] = std::pair<int, int>(35,225);
  angles[8] = std::pair<int, int>(35,315);
  angles[9] = std::pair<int, int>(45,0);
  angles[10] = std::pair<int, int>(45,90);
  angles[11] = std::pair<int, int>(45,180);
  angles[12] = std::pair<int, int>(45,270);
  angles[13] = std::pair<int, int>(66,27);
  angles[14] = std::pair<int, int>(66,63);
  angles[15] = std::pair<int, int>(66,117);
  angles[16] = std::pair<int, int>(66,153);
  angles[17] = std::pair<int, int>(66,207);
  angles[18] = std::pair<int, int>(66,243);
  angles[19] = std::pair<int, int>(66,297);
  angles[20] = std::pair<int, int>(66,333);
  angles[21] = std::pair<int, int>(68,0);
  angles[22] = std::pair<int, int>(68,90);
  angles[23] = std::pair<int, int>(68,180);
  angles[24] = std::pair<int, int>(68,270);
  angles[25] = std::pair<int, int>(30,100);

  for (unsigned i = 0; i < angles.size(); i++) {
    std::stringstream ss;
    ss.width(3);
    ss.fill('0');
    ss << angles[i].first;

    std::stringstream ss2;
    ss2.width(3);
    ss2.fill('0');
    ss2 << angles[i].second;
    
    double angle1 = (double)angles[i].first*vnl_math::pi/180.0;
    double angle2 = (double)angles[i].second*vnl_math::pi/180.0;

    vpgl_perspective_camera<double> persp_cam = generate_camera(angle1, angle2);

    std::string camera_file = output_prefix + "-" + ss.str() + "-" + ss2.str() + ".txt";
    //std::cout << "cam file: " << camera_file << std::endl;
    
    std::ofstream ofs(camera_file.c_str());
    if (!ofs.is_open()) {
      std::cerr << "Failed to open file " << camera_file << std::endl;
      return false;
    }

    vnl_matrix_fixed<double,3,4> P = persp_cam.get_matrix();
    ofs << P;
    ofs.close();

    std::string camera_file_krt = output_prefix + "-" + ss.str() + "-" + ss2.str() + "-krt.txt";
    std::ofstream ofs_krt(camera_file_krt.c_str());
    if (!ofs_krt.is_open()) {
      std::cerr << "Failed to open file " << camera_file_krt << std::endl;
      return false;
    }

    vpgl_calibration_matrix<double> K = persp_cam.get_calibration();
    ofs_krt << K.get_matrix() << std::endl;
  
    vgl_rotation_3d<double> R = persp_cam.get_rotation();
    ofs_krt << R.as_matrix() << std::endl;

    vgl_point_3d<double> C = persp_cam.get_camera_center();
    ofs_krt << C.x() << " " << C.y() << " " << C.z() << std::endl;
    ofs_krt.close();
  }

  return true;
}

bool prepare_joint_hist(std::string image_name, std::string changes_file)
{
  if (image_name.compare("") == 0) {
    std::cout << "image_name not specified\n";
    return false;
  }

  if (changes_file.compare("") == 0) {
    std::cout << "changes_file not specified\n";
    return false;
  }

  // try loading the image
  vil_image_resource_sptr res = vil_load_image_resource(image_name.c_str());
  if (res->nplanes() == 3) {
    std::cout << "input image is not grey scale!\n";
    return false;
  }

  //: load the changes
  vsl_b_ifstream is(changes_file.c_str());
  bvgl_changes_sptr c = new bvgl_changes();
  c->b_read(is);
  is.close();

  std::vector<std::pair<float, float> > pairs;
  if (!bvxm_param_estimation::create_fg_pairs(res, c, pairs, true, "./out.wrl")) {
    std::cout << "Problems in creation of foreground pairs\n";
    return false;
  }

  for (unsigned i = 0; i < pairs.size(); i++) {
    std::cout << "pair " << i << ": " << pairs[i].first << " " << pairs[i].second << "\n";
  }

  std::cout << "number of pairs: " << pairs.size() << std::endl;
  double init_sigma = bvxm_param_estimation::estimate_fg_pair_density_initial_sigma(pairs);
  std::cout << "initial sigma: " << init_sigma << std::endl;
  double est_sigma = bvxm_param_estimation::estimate_fg_pair_density_sigma_amoeba(pairs, init_sigma);
  std::cout << "estimated sigma: " << est_sigma << std::endl;

  unsigned ni = res->ni();
  unsigned nj = res->nj();
  vil_image_view<float> inp_img(ni, nj, 1);
  if (res->nplanes() != 1) {
    std::cout << "In dbvxm_param_estimation::create_fg_pairs() -- input view is not grey scale!\n";
    return false;
  }
  vil_image_view<vxl_byte> inp_img_view(res->get_view());
  vil_convert_stretch_range_limited(inp_img_view, inp_img, (vxl_byte)0, (vxl_byte)255, 0.0f, 1.0f);
  bvxm_fg_pair_density fd;
  fd.set_sigma(est_sigma);
  vil_image_view<float> map = fd.prob_density(inp_img);
  float minv, maxv;
  vil_math_value_range(map, minv, maxv);
  vil_image_view<vxl_byte> map_b(map.ni(), map.nj());
  vil_convert_stretch_range_limited(map, map_b, 0.0f, maxv);
  vil_save(map_b, "./fg_map_out.png");

  bvxm_fg_bg_pair_density fbd;
  fbd.set_sigma(est_sigma);
  map = fbd.prob_density(inp_img);
  vil_math_value_range(map, minv, maxv);
  vil_convert_stretch_range_limited(map, map_b, 0.0f, maxv);
  vil_save(map_b, "./fbg_map_out.png");

  return true;
}


bool collect_statistics_for_glitch_area(std::string mask_name, std::string prob_map_name, std::string glitch_img_name,
                                        float measurement_max, int measurement_bin_cnt, int neighborhood_size, bool not, bool fit_weibull)
{
  if (mask_name.compare("") == 0) {
    std::cout << "mask_name not specified\n";
    return false;
  }

  if (prob_map_name.compare("") == 0) {
    std::cout << "prob_map_name not specified\n";
    return false;
  }

  if (glitch_img_name.compare("") == 0) {
    std::cout << "glitch_img_name not specified\n";
    return false;
  }
  std::cout << "Running collect_statistics_for_glitch_area() with:\n";
  std::cout << " -mask: \t" << mask_name << std::endl;
  std::cout << " -prob-map: \t" << prob_map_name << std::endl;
  std::cout << " -meas-img: \t" << glitch_img_name << std::endl;
  std::cout << " -meas-max: \t" << measurement_max << std::endl;
  std::cout << " -meas-bin: \t" << measurement_bin_cnt << std::endl;
  std::cout << " -n: \t" << neighborhood_size << std::endl;

  //: load the image
  //std::string image_filename1 = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\density_mask0.bin";
  std::string image_filename1 = mask_name;
  vsl_b_ifstream is(image_filename1);
  vil_image_view_base_sptr mask_sptr;
  vsl_b_read(is, mask_sptr);
  is.close();
  vil_image_view<bool> mask(mask_sptr);

  //: load the image
  //std::string image_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_map_0.tiff";
  //vil_image_view<float> image = vil_load(image_filename.c_str());
  //std::string image_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_glitch_map_0.tiff";
  std::string image_filename = glitch_img_name;
  vil_image_view<float> image = vil_load(image_filename.c_str());

  //std::string map_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\brown_prob_map_conv_0.tiff";
  //vil_image_view<float> map = vil_load(map_filename.c_str());
  //std::string map_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_glitch_norm_prob_map_0.tiff";
  std::string map_filename = prob_map_name;
  vil_image_view<float> map = vil_load(map_filename.c_str());

  float min, max;
  vil_math_value_range(map, min, max);
  std::cout << " input prob map value range min: " << min << " max: " << max << std::endl;

  if (not) {
    std::cout << " -not: true --> will invert the input prob map\n";
     for (unsigned i = 0; i < map.ni(); i++) 
       for (unsigned j = 0; j < map.nj(); j++) { 
         if (map(i, j) < 0 || map(i, j) > 1.0f) {
           std::cout << "In collect_statistics_for_glitch_area() -- Problem with the input probability map, val: " << map(i,j) << " not in [0,1] range\n";
           return true;
         }
         map(i,j) = 1.0f-map(i,j);
       }
  }


  //std::string gt_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\test_img_0_gt.png";
  //vil_image_view<vxl_byte> gt_view = vil_load(gt_filename.c_str());
  //vil_image_view<bool> gt_foreground(gt_view.ni(), gt_view.nj(), 1);
  //gt_foreground.fill(false);
  //for (unsigned i = 0; i < gt_view.ni(); i++)
  //  for (unsigned j = 0; j < gt_view.nj(); j++)
  //    gt_foreground(i,j) = (gt_view(i,j) == 255 ? true : false);
 
  if ( image.ni() == 0 || image.nj() == 0 || map.ni() == 0 || map.nj() == 0) {
    std::cerr << "Failed to load image file" << image_filename << std::endl;
    return false;
  }
  

  //bsta_histogram<float> hist(25.0f,25);  //value range [0,25], 25 bins
  bsta_histogram<float> hist(measurement_max, measurement_bin_cnt);  //value range [0,25], 25 bins

  std::vector<std::pair<int, int> > neighborhood;
  std::vector<std::pair<int, int> > neighborhood_outer;
  bvxm_glitch::square_glitch(neighborhood_size, neighborhood, neighborhood_outer);
  
  unsigned si = 3; unsigned sj = 3;   // the whole image
  unsigned w = image.ni()-3; unsigned h = image.nj()-3;
  for (unsigned i = si; i < si+w; i++) 
    for (unsigned j = sj; j < sj+h; j++) {
      if (mask(i,j)) {
        
        //: collect the stats only if the whole neighborhood of the pixel is inside the mask
        bool all_inside = true;
        for (unsigned t = 0; t < neighborhood_outer.size(); t++)
          if (!mask(i+neighborhood_outer[t].first, j+neighborhood_outer[t].second)) {
            all_inside = false;
            break;
          }
        if (!all_inside)
          continue;

        //hist.upcount(image(i,j), 1.0f);
        //hist.upcount(image(i,j), (1.0f-map(i,j)));  // finding the foreground weighted areas
        //if (gt_foreground(i,j))
        //  hist.upcount(image(i,j), 1.0f);  // finding the foreground weighted areas
        //if (!gt_foreground(i,j))
        //  hist.upcount(image(i,j), 1.0f);  // finding the background weighted areas
        
        //hist.upcount(image(i,j), (1-map(i,j)));  // finding the background weighted areas
        float m = map(i,j);
        float iv = image(i,j);
        if (map(i,j) > 0)
          hist.upcount(image(i,j), map(i,j));  // finding the background weighted areas
      }
    }

  std::vector<float> counts = hist.count_array();
  std::vector<float> vals = hist.value_array();

  float mean = hist.mean();
  float var = hist.variance();
  float stad_dev = (float)sqrt(var);
  
  std::ofstream of("collect_stats_and_fit_weibull_out.out");
  for (unsigned i = 0; i < vals.size(); i++) {
    std::cout << vals[i] << "\t";
    of << vals[i] << "\t";
  }
  std::cout << "\n";
  of << "\n";

  for (unsigned i = 0; i < counts.size(); i++) {
    std::cout << counts[i] << "\t";
    of << counts[i] << "\t";
  }
  std::cout << "\n";
  of << "\n";
  std::cout << "\n";
  of << "\n";
  
  
  //bsta_weibull<float> back_model_(mean, 1.0f);
  std::vector<float> wei_out(vals.size(), 0.0f);
  float lambda = mean;
  float k = 1.0f;

  if (fit_weibull) {
    bsta_weibull_cost_function wcf(mean, stad_dev);
    bsta_fit_weibull<float> fw(&wcf);
    k = 0.0f;
    fw.init(k);
    fw.solve(k);
    float residual = fw.residual();
    lambda = fw.lambda(k);
    std::cout << "weibull fitted with residual: " << residual << std::endl; 
    
  }
  
  bsta_weibull<float> back_model_(lambda, k);
  std::cout << " weibull params: " << " lambda: " << lambda << " k: " << k << std::endl;
  
  
  for (unsigned i = 0; i < vals.size(); i++) {
    wei_out[i] = back_model_.prob_density(vals[i]);
    std::cout << wei_out[i] << "\t";
    of << wei_out[i] << "\t";
  }
  std::cout << "\n";
  of << "\n";
  
  of << "mean: " << mean << " var: " << var << " stad dev: " << stad_dev << std::endl; 
  of << "lambda: " << lambda << " k: " << k << std::endl;
  std::cout << "mean: " << mean << " var: " << var << " stad dev: " << stad_dev << std::endl; 
  
  of.close();
  return true;
}



