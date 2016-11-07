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

#include "dborl_cmd_line.h"

int main(int argc, char *argv[]) {

  // Arguments
  vul_arg_info_list arg_list;
  //vul_arg<double> sigma(arg_list,"-sigma","sigma for Gaussian smoothing",1.0);
  //vul_arg<float> thresh1(arg_list,"-t1","Threshold for truncating motion",0.001f);
  //vul_arg<float> thresh2(arg_list,"-t2","Threshold for detecting motion",0.0001f);
  
   vul_arg<bool> prepare_xml_for_detection(arg_list,"-prepare-xml","run the routine to prepare an input xml file for the given threshold interval",false);
   vul_arg<bool> prepare_xml_for_detection2(arg_list,"-prepare-xml2","run the routine to prepare an input xml file for the given threshold interval",false);
  
   vul_arg<bool> prepare_plot_arg(arg_list, "-prepare-plot", "run prepare plot", false);
  
  vul_arg<vcl_string> image_folder(arg_list,"-images","the folder that contains images","");
  vul_arg<vcl_string> image_ext(arg_list,"-ext","all the images in the folder with this extension are used","");
  
  vul_arg<vcl_string> input_file(arg_list,"-input","the file of the routine","");
  vul_arg<vcl_string> input_file2(arg_list,"-input2","the file of the routine","");
  vul_arg<vcl_string> output_prefix(arg_list,"-output","the prefix for the output files for the routine","");

  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);

  vul_arg<float> min_thres(arg_list, "-min", "min threshold", 0.0f);
  vul_arg<float> max_thres(arg_list, "-max", "max threshold", 0.0f);
  vul_arg<float> inc_thres(arg_list, "-inc", "threshold increment", 0.0f);

  vul_arg<int> processor_cnt(arg_list, "-np", "number of processors", -1);

  vul_arg<vcl_string> legends_file(arg_list, "-legends", "path of the legends file", "");
  vul_arg<vcl_string> plot_type(arg_list, "-plot", "type of the plot, e.g. roc, prc, etc. all if to plot all", "");

  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  if (prepare_xml_for_detection()) {
    if (!prepare_param_files(input_file(), output_prefix(), min_thres(), inc_thres(), max_thres())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -prepare-xml: \n";
      vcl_cout << input_file.option() << " " << input_file.help() << vcl_endl;
      vcl_cout << output_prefix.option() << " " << output_prefix.help() << vcl_endl;
      //vcl_cout << "if -category <category name> option exists, then it appends the <category name> before the object name in the list\n";
      vcl_cout << min_thres.option() << " " << min_thres.help() << vcl_endl;
      vcl_cout << max_thres.option() << " " << max_thres.help() << vcl_endl;
      vcl_cout << inc_thres.option() << " " << inc_thres.help() << vcl_endl;
    }
  } else if (prepare_xml_for_detection2()) {
    if (!prepare_param_files2(input_file(), output_prefix(), min_thres(), inc_thres(), max_thres(), processor_cnt())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -prepare-xml2: \n";
      vcl_cout << input_file.option() << " " << input_file.help() << vcl_endl;
      vcl_cout << output_prefix.option() << " " << output_prefix.help() << vcl_endl;
      //vcl_cout << "if -category <category name> option exists, then it appends the <category name> before the object name in the list\n";
      vcl_cout << min_thres.option() << " " << min_thres.help() << vcl_endl;
      vcl_cout << max_thres.option() << " " << max_thres.help() << vcl_endl;
      vcl_cout << inc_thres.option() << " " << inc_thres.help() << vcl_endl;
      vcl_cout << processor_cnt.option() << " " << processor_cnt.help() << vcl_endl;
    }
  } else if (prepare_plot_arg()) {
    if (input_file2().compare("") != 0) {
      if (!prepare_plot2(input_file(), input_file2(), output_prefix(), legends_file(), plot_type(), min_thres(), inc_thres(), max_thres())) 
      {
        vcl_cout << "routine failed\n";
        vcl_cout << "usage with -prepare-plot: \n";
        vcl_cout << input_file.option() << " " << input_file.help() << vcl_endl;
        vcl_cout << input_file2.option() << " " << input_file2.help() << vcl_endl;
        vcl_cout << output_prefix.option() << " " << output_prefix.help() << vcl_endl;
        //vcl_cout << "if -category <category name> option exists, then it appends the <category name> before the object name in the list\n";
        vcl_cout << legends_file.option() << " " <<legends_file.help() << vcl_endl;
        vcl_cout << plot_type.option() << " " <<plot_type.help() << vcl_endl;
        vcl_cout << min_thres.option() << " " << min_thres.help() << vcl_endl;
        vcl_cout << max_thres.option() << " " << max_thres.help() << vcl_endl;
        vcl_cout << inc_thres.option() << " " << inc_thres.help() << vcl_endl;
      }
    } else if (!prepare_plot(input_file(), output_prefix(), legends_file(), plot_type(), min_thres(), inc_thres(), max_thres())) {
      vcl_cout << "routine failed\n";
      vcl_cout << "usage with -prepare-plot: \n";
      vcl_cout << input_file.option() << " " << input_file.help() << vcl_endl;
      vcl_cout << output_prefix.option() << " " << output_prefix.help() << vcl_endl;
      //vcl_cout << "if -category <category name> option exists, then it appends the <category name> before the object name in the list\n";
      vcl_cout << legends_file.option() << " " <<legends_file.help() << vcl_endl;
      vcl_cout << plot_type.option() << " " <<plot_type.help() << vcl_endl;
      vcl_cout << min_thres.option() << " " << min_thres.help() << vcl_endl;
      vcl_cout << max_thres.option() << " " << max_thres.help() << vcl_endl;
      vcl_cout << inc_thres.option() << " " << inc_thres.help() << vcl_endl;
    }
  }

  return 0;
}

