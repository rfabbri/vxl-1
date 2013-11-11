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
  //vcl_cout << "centroid: " << centroid << vcl_endl;
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
  vcl_cout << camera_center_vec.x() << " " << camera_center_vec.y() << " " << camera_center_vec.z() << "\n";

  vpgl_perspective_camera<double> persp_cam;

  //generate_persp_camera(focal_length,principal_point, x_scale, y_scale, camera_center, persp_cam);
  vpgl_calibration_matrix<double> K(focal_length, principal_point, x_scale, y_scale);
  persp_cam.set_calibration(K);
  vgl_point_3d<double> camera_center_point2(camera_center_vec.x(), camera_center_vec.y(), camera_center_vec.z());
  vcl_cout << camera_center_point2.x() << " " << camera_center_point2.y() << " " << camera_center_point2.z() << "\n";
  persp_cam.set_camera_center(camera_center_point2);

  persp_cam.look_at(vgl_homg_point_3d<double>(centroid));
  return persp_cam;
}


//: also prepares the command file
bool prepare_cameras_for_eth80(vcl_string output_prefix)
{
  if (output_prefix.compare("") == 0) {
    vcl_cout << "output_prefix not specified\n";
    return false;
  }

  vcl_vector<vcl_pair<int, int> > angles(26, vcl_pair<int, int>(0,0));
  angles[0] = vcl_pair<int, int>(0,0);
  angles[1] = vcl_pair<int, int>(22,0);
  angles[2] = vcl_pair<int, int>(22,90);
  angles[3] = vcl_pair<int, int>(22,180);
  angles[4] = vcl_pair<int, int>(22,270);
  angles[5] = vcl_pair<int, int>(35,45);
  angles[6] = vcl_pair<int, int>(35,135);
  angles[7] = vcl_pair<int, int>(35,225);
  angles[8] = vcl_pair<int, int>(35,315);
  angles[9] = vcl_pair<int, int>(45,0);
  angles[10] = vcl_pair<int, int>(45,90);
  angles[11] = vcl_pair<int, int>(45,180);
  angles[12] = vcl_pair<int, int>(45,270);
  angles[13] = vcl_pair<int, int>(66,27);
  angles[14] = vcl_pair<int, int>(66,63);
  angles[15] = vcl_pair<int, int>(66,117);
  angles[16] = vcl_pair<int, int>(66,153);
  angles[17] = vcl_pair<int, int>(66,207);
  angles[18] = vcl_pair<int, int>(66,243);
  angles[19] = vcl_pair<int, int>(66,297);
  angles[20] = vcl_pair<int, int>(66,333);
  angles[21] = vcl_pair<int, int>(68,0);
  angles[22] = vcl_pair<int, int>(68,90);
  angles[23] = vcl_pair<int, int>(68,180);
  angles[24] = vcl_pair<int, int>(68,270);
  angles[25] = vcl_pair<int, int>(30,100);

  for (unsigned i = 0; i < angles.size(); i++) {
    vcl_stringstream ss;
    ss.width(3);
    ss.fill('0');
    ss << angles[i].first;

    vcl_stringstream ss2;
    ss2.width(3);
    ss2.fill('0');
    ss2 << angles[i].second;
    
    double angle1 = (double)angles[i].first*vnl_math::pi/180.0;
    double angle2 = (double)angles[i].second*vnl_math::pi/180.0;

    vpgl_perspective_camera<double> persp_cam = generate_camera(angle1, angle2);

    vcl_string camera_file = output_prefix + "-" + ss.str() + "-" + ss2.str() + ".txt";
    //vcl_cout << "cam file: " << camera_file << vcl_endl;
    
    vcl_ofstream ofs(camera_file.c_str());
    if (!ofs.is_open()) {
      vcl_cerr << "Failed to open file " << camera_file << vcl_endl;
      return false;
    }

    vnl_matrix_fixed<double,3,4> P = persp_cam.get_matrix();
    ofs << P;
    ofs.close();

    vcl_string camera_file_krt = output_prefix + "-" + ss.str() + "-" + ss2.str() + "-krt.txt";
    vcl_ofstream ofs_krt(camera_file_krt.c_str());
    if (!ofs_krt.is_open()) {
      vcl_cerr << "Failed to open file " << camera_file_krt << vcl_endl;
      return false;
    }

    vpgl_calibration_matrix<double> K = persp_cam.get_calibration();
    ofs_krt << K.get_matrix() << vcl_endl;
  
    vgl_rotation_3d<double> R = persp_cam.get_rotation();
    ofs_krt << R.as_matrix() << vcl_endl;

    vgl_point_3d<double> C = persp_cam.get_camera_center();
    ofs_krt << C.x() << " " << C.y() << " " << C.z() << vcl_endl;
    ofs_krt.close();
  }

  return true;
}

bool prepare_joint_hist(vcl_string image_name, vcl_string changes_file)
{
  if (image_name.compare("") == 0) {
    vcl_cout << "image_name not specified\n";
    return false;
  }

  if (changes_file.compare("") == 0) {
    vcl_cout << "changes_file not specified\n";
    return false;
  }

  // try loading the image
  vil_image_resource_sptr res = vil_load_image_resource(image_name.c_str());
  if (res->nplanes() == 3) {
    vcl_cout << "input image is not grey scale!\n";
    return false;
  }

  //: load the changes
  vsl_b_ifstream is(changes_file.c_str());
  bvgl_changes_sptr c = new bvgl_changes();
  c->b_read(is);
  is.close();

  vcl_vector<vcl_pair<float, float> > pairs;
  if (!bvxm_param_estimation::create_fg_pairs(res, c, pairs, true, "./out.wrl")) {
    vcl_cout << "Problems in creation of foreground pairs\n";
    return false;
  }

  for (unsigned i = 0; i < pairs.size(); i++) {
    vcl_cout << "pair " << i << ": " << pairs[i].first << " " << pairs[i].second << "\n";
  }

  vcl_cout << "number of pairs: " << pairs.size() << vcl_endl;
  double init_sigma = bvxm_param_estimation::estimate_fg_pair_density_initial_sigma(pairs);
  vcl_cout << "initial sigma: " << init_sigma << vcl_endl;
  double est_sigma = bvxm_param_estimation::estimate_fg_pair_density_sigma_amoeba(pairs, init_sigma);
  vcl_cout << "estimated sigma: " << est_sigma << vcl_endl;

  unsigned ni = res->ni();
  unsigned nj = res->nj();
  vil_image_view<float> inp_img(ni, nj, 1);
  if (res->nplanes() != 1) {
    vcl_cout << "In dbvxm_param_estimation::create_fg_pairs() -- input view is not grey scale!\n";
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


bool collect_statistics_for_glitch_area(vcl_string mask_name, vcl_string prob_map_name, vcl_string glitch_img_name,
                                        float measurement_max, int measurement_bin_cnt, int neighborhood_size, bool not, bool fit_weibull)
{
  if (mask_name.compare("") == 0) {
    vcl_cout << "mask_name not specified\n";
    return false;
  }

  if (prob_map_name.compare("") == 0) {
    vcl_cout << "prob_map_name not specified\n";
    return false;
  }

  if (glitch_img_name.compare("") == 0) {
    vcl_cout << "glitch_img_name not specified\n";
    return false;
  }
  vcl_cout << "Running collect_statistics_for_glitch_area() with:\n";
  vcl_cout << " -mask: \t" << mask_name << vcl_endl;
  vcl_cout << " -prob-map: \t" << prob_map_name << vcl_endl;
  vcl_cout << " -meas-img: \t" << glitch_img_name << vcl_endl;
  vcl_cout << " -meas-max: \t" << measurement_max << vcl_endl;
  vcl_cout << " -meas-bin: \t" << measurement_bin_cnt << vcl_endl;
  vcl_cout << " -n: \t" << neighborhood_size << vcl_endl;

  //: load the image
  //vcl_string image_filename1 = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\density_mask0.bin";
  vcl_string image_filename1 = mask_name;
  vsl_b_ifstream is(image_filename1);
  vil_image_view_base_sptr mask_sptr;
  vsl_b_read(is, mask_sptr);
  is.close();
  vil_image_view<bool> mask(mask_sptr);

  //: load the image
  //vcl_string image_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_map_0.tiff";
  //vil_image_view<float> image = vil_load(image_filename.c_str());
  //vcl_string image_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_glitch_map_0.tiff";
  vcl_string image_filename = glitch_img_name;
  vil_image_view<float> image = vil_load(image_filename.c_str());

  //vcl_string map_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\brown_prob_map_conv_0.tiff";
  //vil_image_view<float> map = vil_load(map_filename.c_str());
  //vcl_string map_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\change_area_glitch_norm_prob_map_0.tiff";
  vcl_string map_filename = prob_map_name;
  vil_image_view<float> map = vil_load(map_filename.c_str());

  float min, max;
  vil_math_value_range(map, min, max);
  vcl_cout << " input prob map value range min: " << min << " max: " << max << vcl_endl;

  if (not) {
    vcl_cout << " -not: true --> will invert the input prob map\n";
     for (unsigned i = 0; i < map.ni(); i++) 
       for (unsigned j = 0; j < map.nj(); j++) { 
         if (map(i, j) < 0 || map(i, j) > 1.0f) {
           vcl_cout << "In collect_statistics_for_glitch_area() -- Problem with the input probability map, val: " << map(i,j) << " not in [0,1] range\n";
           return true;
         }
         map(i,j) = 1.0f-map(i,j);
       }
  }


  //vcl_string gt_filename = "C:\\projects\\roi_1\\bin_6\\bin_all_test_output\\test_img_0_gt.png";
  //vil_image_view<vxl_byte> gt_view = vil_load(gt_filename.c_str());
  //vil_image_view<bool> gt_foreground(gt_view.ni(), gt_view.nj(), 1);
  //gt_foreground.fill(false);
  //for (unsigned i = 0; i < gt_view.ni(); i++)
  //  for (unsigned j = 0; j < gt_view.nj(); j++)
  //    gt_foreground(i,j) = (gt_view(i,j) == 255 ? true : false);
 
  if ( image.ni() == 0 || image.nj() == 0 || map.ni() == 0 || map.nj() == 0) {
    vcl_cerr << "Failed to load image file" << image_filename << vcl_endl;
    return false;
  }
  

  //bsta_histogram<float> hist(25.0f,25);  //value range [0,25], 25 bins
  bsta_histogram<float> hist(measurement_max, measurement_bin_cnt);  //value range [0,25], 25 bins

  vcl_vector<vcl_pair<int, int> > neighborhood;
  vcl_vector<vcl_pair<int, int> > neighborhood_outer;
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

  vcl_vector<float> counts = hist.count_array();
  vcl_vector<float> vals = hist.value_array();

  float mean = hist.mean();
  float var = hist.variance();
  float stad_dev = (float)sqrt(var);
  
  vcl_ofstream of("collect_stats_and_fit_weibull_out.out");
  for (unsigned i = 0; i < vals.size(); i++) {
    vcl_cout << vals[i] << "\t";
    of << vals[i] << "\t";
  }
  vcl_cout << "\n";
  of << "\n";

  for (unsigned i = 0; i < counts.size(); i++) {
    vcl_cout << counts[i] << "\t";
    of << counts[i] << "\t";
  }
  vcl_cout << "\n";
  of << "\n";
  vcl_cout << "\n";
  of << "\n";
  
  
  //bsta_weibull<float> back_model_(mean, 1.0f);
  vcl_vector<float> wei_out(vals.size(), 0.0f);
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
    vcl_cout << "weibull fitted with residual: " << residual << vcl_endl; 
    
  }
  
  bsta_weibull<float> back_model_(lambda, k);
  vcl_cout << " weibull params: " << " lambda: " << lambda << " k: " << k << vcl_endl;
  
  
  for (unsigned i = 0; i < vals.size(); i++) {
    wei_out[i] = back_model_.prob_density(vals[i]);
    vcl_cout << wei_out[i] << "\t";
    of << wei_out[i] << "\t";
  }
  vcl_cout << "\n";
  of << "\n";
  
  of << "mean: " << mean << " var: " << var << " stad dev: " << stad_dev << vcl_endl; 
  of << "lambda: " << lambda << " k: " << k << vcl_endl;
  vcl_cout << "mean: " << mean << " var: " << var << " stad dev: " << stad_dev << vcl_endl; 
  
  of.close();
  return true;
}



