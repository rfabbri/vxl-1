// This is dborl/algo/vox_generate_shape/vox_generate_shape_main.cxx

//:
// \file
// \author Yuliang Guo	
// \date   Sep 4, 2014
// \brief An algorithm to generate shapes based on trained model
// \verbatim
//    Modifications
// \endverbatim


#include "vox_generate_shape_params.h"
#include "vox_generate_shape_params_sptr.h"
#include "dbsks_shape_generator.h"

#include <dborl/dborl_eval_det.h>
#include <dborl/algo/dborl_xio_image_description.h>

#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_det_desc_xgraph.h>
#include <dbsks/algo/dbsks_vox_utils.h>

#include <vsol/vsol_box_2d.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_convert.h>
#include <vil/vil_load.h>
#include <vcl_cstdlib.h>

// =============================================================================
//: VOX executable to detect objects, represented by xshock_graph, in images
int main(int argc, char *argv[]) 
{

  vul_timer timer;
  timer.mark();

  //1. Parse the input xml file ////////////////////////////////////////////////
  vcl_cout
    << "\n---------------------------------------------------------------------"
    << "\nLoad parameter file"
    << "\n--------------------------------------------------------------------\n";

  // construct parameters with the default values;
  vox_generate_shape_params_sptr params = new vox_generate_shape_params("vox_generate_shape");

  // parse the command line arguments
  if (!params->parse_command_line_args(argc, argv))
  {
    return EXIT_FAILURE;
  }

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
  {
    vcl_cout << "problems in writing params file to: " 
      << params->print_params_file() << vcl_endl;
  }

  // exit if there is nothing else to do
  if (params->exit_with_no_processing() || params->print_params_only())
  {
    return EXIT_SUCCESS;
  }

  // always call this method to actually parse the input parameter file whose 
  // name is extracted from the command line
  if (!params->parse_input_xml())
  {
    return EXIT_FAILURE;
  }

/*

  //>> Write input.xml file to output folder
  // filename format: input+applelogos_another.xml
  params->print_input_xml(params->get_output_folder() + "/" + "input+" + 
    params->input_object_name_() + ".xml");

*/  
  
  //////////////////////////////////////////////////////////////////////////////
  //2. Generate Shapes based on trained geometric model

  	// List of detections to return
    vcl_vector<dbsks_det_desc_xgraph_sptr > shape_list;

    //> Set up params
    dbsks_shape_generator m;

    // xgraph protype + model
    m.xgraph_file              = params->get_xgraph_file();
    m.xgraph_geom_file         = params->get_xgraph_geom_file();
    m.xgraph_geom_param_file   = params->get_xgraph_geom_param_file();
	m.num_samples = params->num_samples();


    //> Run detection
    bool success = m.execute();

    if (!success)
    {
      vcl_cout << "\nERROR: Shape Sampling failed. No results created." << vcl_endl;
      return EXIT_FAILURE;
    }

    //> Collect detection results
    shape_list = m.output_shape_list;

 //> Save each detection: xgraph + screenshot
  vcl_cout << "\n>> Save each detected xgraph to output folder: " 
    << params->get_output_folder() << "...";

  // Original image
//  vil_image_view<vxl_byte > source_image = *vil_convert_cast(vxl_byte(), image_resource->get_view());

  // black background image
	unsigned ni=704;
	unsigned nj=480;
	unsigned nplanes=3;
  vil_image_view<vxl_byte > source_image(ni,nj,nplanes);

  vcl_string det_group_id = params->input_model_category_();

    if (!vul_file::is_directory(params->get_output_folder()))
    {
      vul_file::make_directory(params->get_output_folder());
    }

  // Write all detections to a folder within the output folder
  dbsks_save_detections_to_folder(shape_list, params->input_model_category_(), 
    params->input_model_category_(), 
    det_group_id, 
    source_image,
    params->get_output_folder(),
    det_group_id);
  vcl_cout << "done.\n";

  return EXIT_SUCCESS;
}

