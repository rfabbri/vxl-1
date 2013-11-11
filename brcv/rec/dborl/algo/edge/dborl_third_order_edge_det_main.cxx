//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/08/08
//
//        An algorithm to run edge detection and linking algorithms on images, 
//        input object is a single image, output is an edge map or a boundary fragment map, with an option to associate to the image or to write to the object folder
//
//        This is a wrapper around brcv/seg/dbdet/dbdet_third_order_edge_detector_process
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include "dborl_third_order_edge_det_params.h"
#include "dborl_third_order_edge_det_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/algo/dbdet_load_edg.h>

int main(int argc, char *argv[]) {

  dborl_third_order_edge_det_params_sptr params = new dborl_third_order_edge_det_params("dborl_third_order_edge_det");  // constructs with the default values;
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  //: always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  //: always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  dbdet_third_order_edge_detector_process pro;
  set_process_parameters_of_bpro1(*params, pro, params->algo_abbreviation_);
  
  //load the input image
  vcl_string input_img = params->input_object_dir_() + "/" + params->input_object_name_() + params->input_extension_();
  if (!vul_file::exists(input_img)) {
    vcl_cout << "Cannot find image: " << input_img << "\n";
    return 0;
  }

  vil_image_resource_sptr img = vil_load_image_resource(input_img.c_str());
  if (!img) {
    vcl_cout << "Cannot load image: " << input_img << "\n";
    return 0;
  }

  vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
  inp->set_image(img);

  pro.add_input(inp);
  pro.execute();
  vcl_cout << " processed..";
  pro.finish();
  vcl_cout << " finalized..\n";

  //:get the output
  vcl_vector<bpro1_storage_sptr> out = pro.get_output();
  if (out.size() != 1) {
    vcl_cout << "Process output does not contain an edge_map\n";
    return 0;
  }

  //save this edge map onto a file
  if (params->save_edges_()) {
    dbdet_edgemap_storage* edge_map = dynamic_cast<dbdet_edgemap_storage*>(out[0].ptr());
    if (!edge_map) {
      vcl_cout << "Process output cannot be cast to an edge_map\n";
      return 0;
    }

    dbdet_edgemap_sptr edges = edge_map->get_edgemap();

    vcl_string output_file;
    if (params->save_to_object_folder_()) 
      output_file = params->input_object_dir_() + "/";
    else {
      output_file = params->output_edge_folder_() + "/";
      if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);
    }
    
    output_file = output_file + params->input_object_name_() + params->output_extension_();

    if (!dbdet_save_edg(output_file, edges)) {
      vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
      return 0;
    }
  }

  return 0;
}

