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

#include "dborl_dataset_procs.h"

int main(int argc, char *argv[]) {

  // Arguments
  vul_arg_info_list arg_list;
 
  vul_arg<bool> prepare_ethz_boundaries_arg(arg_list,"-prepare-ethz-boundaries","run the routine to prepare the ethz boundaries",false);
  vul_arg<vcl_string> index_file(arg_list,"-ind","the index file","");
  vul_arg<vcl_string> input_folder(arg_list,"-input","the input folder","");
  vul_arg<vcl_string> output_file(arg_list,"-out","the output file name","");
  vul_arg<vcl_string> input_xml(arg_list, "-inp-xml", "the input xml file to read all parameters", "");
  vul_arg<vcl_string> output_xml(arg_list, "-out-xml", "the name of the output xml file to write new model name and path", "");
  vul_arg<vcl_string> model_name(arg_list, "-model", "the name of the model to replace in the input xml file and write to output xml file", "");
  vul_arg<vcl_string> image_folder(arg_list,"-images","the folder that contains images","");
  vul_arg<vcl_string> image_ext(arg_list,"-ext","all the images in the folder with this extension are used","");
  vul_arg<vcl_string> dataset_list(arg_list,"-dataset-list","the file that contains the list of the elements in the dataset","");
  vul_arg<vcl_string> category_info_file(arg_list,"-category-info","the file that contains the info about the categories like names and prefixes","");
  vul_arg<vcl_string> output_folder(arg_list,"-output","the output folder of the routine","");
  vul_arg<vcl_string> bnd_param_xml(arg_list, "-bnd-xml", "the parameter file for boundary extraction", "");
  vul_arg<vcl_string> sh_param_xml(arg_list, "-shock-xml", "the parameter file for shock extraction", "");
  vul_arg<bool> check_existence(arg_list, "-check", "if supplied checks the existence of esf file and does not recompute if it exists\n", false);

  vul_arg<bool> write_def_params_arg(arg_list, "-print-def-xml", "write the default params into specified files", false);
  
  vul_arg<bool> prepare_match_html_arg(arg_list,"-prepare-match-html","run the routine to prepare the match html",false);
  vul_arg<vcl_string> model_image(arg_list, "-model-image", "model-image", "");
  vul_arg<vcl_string> query_image(arg_list, "-query-image", "query-image", "");
  vul_arg<vcl_string> model_st(arg_list, "-model-st", "model storage", "");
  vul_arg<vcl_string> query_st(arg_list, "-query-st", "query storage", "");
  vul_arg<vcl_string> match_file(arg_list, "-match-file", "match file", "");
  vul_arg<vcl_string> detection_param_xml(arg_list, "-detection-params", "detection params xml file", "");
  //vul_arg<float> sim_threshold(arg_list, "-sim-thres", "similarity threshold", 0.5f);

  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);
  vul_arg<vcl_string> category(arg_list, "-category", "category name", "");
  vul_arg<vcl_string> width_str(arg_list, "-width", "image width", "");

  vul_arg<bool> batch_convert_arg(arg_list, "-bc", "batch convert ps images into ppms", false);
  vul_arg<bool> detect_instance_arg(arg_list, "-detect", "detect instances in the query image", false);
  vul_arg<bool> save_patch_images_arg(arg_list, "-save-images", "save patch storage images", false);

  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }

  if (prepare_ethz_boundaries_arg()) {
    if (!prepare_ethz_boundaries(index_file(), image_ext(), bnd_param_xml(), sh_param_xml(), check_existence())) {
      vcl_cout << "usage with -prepare-ethz-boundaries: \n";
      vcl_cout << index_file.option() << " " << index_file.help() << vcl_endl;
      vcl_cout << image_ext.option() << " " << image_ext.help() << vcl_endl;
      vcl_cout << bnd_param_xml.option() << " " << bnd_param_xml.help() << vcl_endl;
      vcl_cout << sh_param_xml.option() << " " << sh_param_xml.help() << vcl_endl;
      vcl_cout << check_existence.option() << " " << check_existence.help() << vcl_endl;
    }
  } else if (prepare_match_html_arg()) {
    if (!prepare_match_html(model_image(), model_st(), 
      query_image(), query_st(), match_file(), detection_param_xml(), output_file(), width_str())) {
      vcl_cout << "usage with -prepare-match-html: \n";
      vcl_cout << model_image.option() << " " << model_image.help() << vcl_endl;
      vcl_cout <<model_st.option() << " " << model_st.help() << vcl_endl;
      vcl_cout <<query_image.option() << " " << query_image.help() << vcl_endl;
      vcl_cout <<query_st.option() << " " << query_st.help() << vcl_endl;
      vcl_cout <<match_file.option() << " " << match_file.help() << vcl_endl;
      vcl_cout <<detection_param_xml.option() << " " << detection_param_xml.help() << vcl_endl;
      vcl_cout <<output_file.option() << " " << output_file.help() << vcl_endl;
      vcl_cout <<width_str.option() << " " << width_str.help() << vcl_endl;
      //vcl_cout <<sim_threshold.option() << " " << sim_threshold.help() << vcl_endl;
    }
  } else if (batch_convert_arg()) {
    if (!batch_convert(image_folder())) {
      vcl_cout << "usage with -bc\n";
      vcl_cout << image_folder.option() << " " << image_folder.help() << vcl_endl;
    }
  } else if (detect_instance_arg()) {
    if (!detect_instance(model_image(), model_st(), 
      query_image(), query_st(), match_file(), detection_param_xml(), output_file(), width_str())) {
      vcl_cout << "usage with -prepare-match-html: \n";
      vcl_cout << model_image.option() << " " << model_image.help() << vcl_endl;
      vcl_cout <<model_st.option() << " " << model_st.help() << vcl_endl;
      vcl_cout <<query_image.option() << " " << query_image.help() << vcl_endl;
      vcl_cout <<query_st.option() << " " << query_st.help() << vcl_endl;
      vcl_cout <<match_file.option() << " " << match_file.help() << vcl_endl;
      vcl_cout <<detection_param_xml.option() << " " << detection_param_xml.help() << vcl_endl;
      vcl_cout <<output_file.option() << " " << output_file.help() << vcl_endl;
      vcl_cout <<width_str.option() << " " << width_str.help() << vcl_endl;
      //vcl_cout <<sim_threshold.option() << " " << sim_threshold.help() << vcl_endl;
    }
  } else if (write_def_params_arg()) {
    if (!write_def_params(bnd_param_xml(), sh_param_xml())) {
      vcl_cout << "usage with -print-def-xml: \n";
      vcl_cout << bnd_param_xml.option() << " " << bnd_param_xml.help() << vcl_endl;
      vcl_cout << sh_param_xml.option() << " " << sh_param_xml.help() << vcl_endl;
    }
  } else if (save_patch_images_arg()) {
    if (!save_patch_images(model_image(), model_st(), output_file())) {
      vcl_cout << "usage with -save-images: \n";
      vcl_cout << model_image.option() << " " << model_image.help() << " works for any image, not necessarily model " << vcl_endl;
      vcl_cout <<model_st.option() << " " << model_st.help() << " works for any storage, not necessarily model " << vcl_endl;
      vcl_cout <<output_file.option() << " " << output_file.help() << vcl_endl;
    }
  }

  return 0;
}

