//:
// \file
// \brief 
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/16/07
#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

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
#include <brip/brip_vil_float_ops.h>

#include <vul/vul_timer.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>

#include "dbvxm_cmd_line.h"

int main(int argc, char *argv[]) {

  // Arguments
  vul_arg_info_list arg_list;
  
  vul_arg<bool> prepare_cameras_for_eth80_arg(arg_list,"-prepare-cameras","run the routine to prepare cameras for eth80");
  vul_arg<vcl_string> output_prefix(arg_list,"-output","the prefix for the output files for the routine","");
  
  vul_arg<bool> prepare_joint_hist_arg(arg_list,"-prepare-hist", "run the routine to prepare joint histogram for a given image");
  vul_arg<vcl_string> image_name(arg_list,"-image","the name with path for the input image file for the routine","");
  vul_arg<vcl_string> changes_file_name(arg_list,"-changes","the name with path for the binary changes file","");

  vul_arg<bool> collect_stats_and_fit_weibull_arg(arg_list,"-fit-weibull", "run the routine to collect stats from a given measurement img wrt to a prob map and fit weibull distribution");
  vul_arg<vcl_string> mask_img_name(arg_list,"-mask","the name with path for the binary mask file","");
  vul_arg<vcl_string> measurement_img_name(arg_list,"-meas-img","the name with path for the measurement image (e.g. glitch area difference)","");
  vul_arg<vcl_string> prob_map_name(arg_list,"-prob-map","the name with path for the prob map for the image (e.g. glitch probability)","");
  vul_arg<float> measurement_max(arg_list,"-meas-max","value max for the measurements (e.g. max glitch area difference)",25.0f);
  vul_arg<int> measurement_bin_cnt(arg_list,"-meas-bin","number of bins to be used to create histogram for measurement values in [0, meas-max]",25);
  vul_arg<int> neighborhood_size(arg_list,"-n","size of neighborhood for a pixel to be included within the mask for its stat to be collected",6);
  vul_arg<bool> not(arg_list,"-not","if present, the prob map is inverted before collecting stats",false);
  vul_arg<bool> fit(arg_list,"-fit","if present, a weibull distribution is fitted to the stats. (otherwise an exponential is fitted)",false);

  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);

  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  if (prepare_cameras_for_eth80_arg()) {
    if (!prepare_cameras_for_eth80(output_prefix())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -prepare-cameras: \n";
      vcl_cout << output_prefix.option() << " " << output_prefix.help() << vcl_endl;
    }
  } else if (prepare_joint_hist_arg()) {
    if (!prepare_joint_hist(image_name(), changes_file_name())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -prepare-hist: \n";
      vcl_cout << image_name.option() << " " << image_name.help() << vcl_endl;
      vcl_cout << changes_file_name.option() << " " << changes_file_name.help() << vcl_endl;
    }
  } else if (collect_stats_and_fit_weibull_arg()) {
    if (!collect_statistics_for_glitch_area(mask_img_name(), prob_map_name(), measurement_img_name(), measurement_max(), measurement_bin_cnt(), neighborhood_size(), not(), fit())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -fit-weibull: \n";
      vcl_cout << mask_img_name.option() << " " << mask_img_name.help() << vcl_endl;
      vcl_cout << prob_map_name.option() << " " << prob_map_name.help() << vcl_endl;
      vcl_cout << not.option() << " " << not.help() << vcl_endl;
      vcl_cout << measurement_img_name.option() << " " << measurement_img_name.help() << vcl_endl;
      vcl_cout << measurement_max.option() << " " << measurement_max.help() << vcl_endl;
      vcl_cout << measurement_bin_cnt.option() << " " << measurement_bin_cnt.help() << vcl_endl;
      vcl_cout << neighborhood_size.option() << " " << neighborhood_size.help() << vcl_endl;
    }
  }

  return 0;
}

