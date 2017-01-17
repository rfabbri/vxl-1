//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 10/25/07
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//


#include "dbskr_visualize_patch_detections.h"

#include <vcl_iostream.h>
#include <vul/vul_arg.h>
#include <bxml/bxml_document.h>
#include <bxml/bxml_read.h>

int main(int argc, char *argv[]) {

  // Arguments
  vul_arg_info_list arg_list;
 
  vul_arg<vcl_string> query_name(arg_list,"-q","name of the query","");
  vul_arg<vcl_string> query_path(arg_list,"-qp","path of the query","");
  vul_arg<vcl_string> param_file(arg_list,"-x","the name of the parameter file","");
  vul_arg<vcl_string> ext(arg_list,"-ext","the extension of the model and query image files","");
  vul_arg<vcl_string> html_img_ext(arg_list,"-htmlext","the extension of the image used in html files","");
  vul_arg<int> html_img_w(arg_list,"-htmlw","the width in pixel of the image used in html files",10);
  vul_arg<bool> create_only_html(arg_list, "-onlyhtml", "create only the html", false);
  vul_arg<bool> show_match_images(arg_list, "-withmatches", "show match images in the html", false);
  vul_arg<bool> print_usage_only(arg_list,"-usage", "print usage info and exit",false);
  vul_arg<bool> print_help(arg_list,"-help", "print usage info and exit",false);
 
  vul_arg_include(arg_list);
  vul_arg_parse(argc, argv);

  if (print_help() || print_usage_only()) {
    vul_arg_display_usage_and_exit();
    return 0;
  }
 
  bxml_document param_doc = bxml_read(param_file());
  if (!param_doc.root_element())
    return 0;
  
  if (param_doc.root_element()->type() != bxml_data::ELEMENT) {
    vcl_cout << "params root is not ELEMENT\n";
    return 0;
  }

  dbskr_detect_patch_params params;

  if (!params.parse_from_data(param_doc.root_element()))
    return 0;

  dbskr_visualize_patch_detections algo(params, query_path(), query_name(), ext(), html_img_ext());
  algo.html_image_width_ = html_img_w();
  algo.create_only_html_ = create_only_html();
  algo.show_match_images_ = show_match_images();
 
  if (!algo.initialize()) {
    vcl_cout << "Initialization failed\n";
    return -1;
  } else 
    vcl_cout << "initialized..";
  
  algo.process();
  vcl_cout << " processed..";
  algo.finalize();
  vcl_cout << " finalized..\n";

  return 0;
}

