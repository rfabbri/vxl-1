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

#include "dborl_bnd_generic_params.h"
#include "dborl_bnd_generic_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbsol/dbsol_file_io.h>

int main(int argc, char *argv[]) {

  dborl_bnd_generic_params_sptr params = new dborl_bnd_generic_params("dborl_bnd_generic");  // constructs with the default values;
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

  dbdet_generic_linker_process pro;
  set_process_parameters_of_bpro1(*params, pro, params->algo_abbreviation_);
  //: set the output to vsol regardless of the input
  pro.parameters()->set_value("-output_vsol", true);

  //load the input edge map
  vcl_string input_edg;
  if (params->input_edge_from_object_folder_()) 
    input_edg = params->input_object_dir_() + "/" + params->input_object_name_() + params->input_extension_();
  else 
    input_edg = params->input_edge_folder_() + "/" + params->input_object_name_() + params->input_extension_();
 
  if (!vul_file::exists(input_edg)) {
    vcl_cout << "Cannot find edg file: " << input_edg << "\n";
    return 0;
  }

  dbdet_edgemap_sptr edge_map;
  if (!dbdet_load_edg(input_edg, true, 1.0, edge_map)) {
    vcl_cout << "Cannot find edg file: " << input_edg << "\n";
    return 0;
  }

  dbdet_edgemap_storage_sptr edgemap_sto = dbdet_edgemap_storage_new();
  edgemap_sto->set_edgemap(edge_map);

  pro.add_input(edgemap_sto);
  pro.execute();
  vcl_cout << " processed..";
  pro.finish();
  vcl_cout << " finalized..\n";

  //:get the output
  vcl_vector<bpro1_storage_sptr> out = pro.get_output();
  if (out.size() != 1) {
    vcl_cout << "Process output does not contain a boundary map\n";
    return 0;
  }

  //save this edge map onto a file
  if (params->save_bnds_()) {
    
    vidpro1_vsol2D_storage* output_vsol = dynamic_cast<vidpro1_vsol2D_storage*>(out[0].ptr());
    if (!output_vsol) {
      vcl_cout << "Process output cannot be cast to a vsol storage\n";
      return 0;
    }

    vcl_string output_file;
    if (params->save_to_object_folder_()) 
      output_file = params->input_object_dir_() + "/";
    else {
      output_file = params->output_bnd_folder_() + "/";
      if (!vul_file::exists(output_file)) 
        vul_file::make_directory(output_file);
    }
    
    output_file = output_file + params->input_object_name_() + params->output_extension_();

    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = output_vsol->all_data();
    if (!dbsol_save_cem(vsol_list, output_file)) {
      vcl_cout << "Problems in saving edge file: " << output_file << vcl_endl;
      return 0;
    }
  }

  return 1;
}

