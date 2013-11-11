//:
// \file
// \author Eli Fine
// \date 09/29/2008
//
//        Archaeology contour extraction on VOX.
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// To generate the initial XML file:
// dborl_contour_tracer -print-def-xml
//
// To run with specified parameters in input_defaults.xml.
// dbsk3dappvox -x input_defaults.xml
//

/*
#include <dbdet/tracer/dbdet_contour_tracer.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_string.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vil/algo/vil_threshold.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_file_io.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_generator.h>

#include <vcl_iostream.h>
#include <vul/vul_file.h>

#include "dborl_contour_tracer_params.h"
#include "dborl_contour_tracer_params_sptr.h"

#include <dbdet/pro/dbdet_contour_tracer_process.h>
*/

#include "dborl_archaeology_algo_params_sptr.h"
#include "dborl_archaeology_algo_params.h"
#include <vcl_string.h>
#include <vcl_vector.h>
#include <dborl/dborl_index_sptr.h>
#include <dborl/dborl_index_node_sptr.h>
#include <vil/vil_load.h>
#include <structure/dbseg_seg_object_base.h>
#include <pro/dbseg_jseg_process.h>
#include <pro/dbseg_seg_process.h>
#include <pro/dbseg_seg_save_process.h>
#include <pro/dbseg_seg_save_contour_process.h>
#include <vil/vil_image_view_base.h>
#include <vul/vul_file.h>




int main (int argc, char *argv[]) 
{
  // constructs with the default values
  dborl_archaeology_algo_params_sptr params = new dborl_archaeology_algo_params ("dborl_archaeology_algo");  
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  // 1) load the input shock3d file 1 (a pair of .cms and .fs files)
  vcl_string input_ctracer = params->input_dir_() + "/" + params->input_name_() + params->input_extension_();
  if (!vul_file::exists(input_ctracer)) {
    vcl_cout << "Cannot find image files : " << input_ctracer << "\n";
    return 0;
  }




    //load the image
    vil_image_view_base_sptr original_image = vil_load((params->input_dir_() + "/" +params->input_name_() + params->input_extension_()).c_str());

    //segment the image
    vil_image_view_base_sptr segmented_image = new vil_image_view<vxl_byte>(dbseg_jseg_process::static_execute(original_image, params->TQUAN.get_default(), params->mthresh.get_default(), params->scale.get_default()));

    //create the segmentation structure
    dbseg_seg_object_base* segmentation_structure = dbseg_seg_process::static_execute(segmented_image, original_image, true);

    //save the structure
    vul_file::make_directory_path(params->output_dir_.value_str() + "\\segmentation_structures");
    bpro1_filepath struct_destination = bpro1_filepath(params->output_dir_.value_str() + "\\segmentation_structures\\" + params->input_name_.value_str() + ".seg");
    dbseg_seg_save_process::static_execute(segmentation_structure, struct_destination);

    //extract the contour
    vul_file::make_directory_path(params->output_dir_.value_str() + "\\contours");
    bpro1_filepath contour_destination = bpro1_filepath(params->output_dir_.value_str() + "\\contours\\" + params->input_name_.value_str() + ".con");
    dbseg_seg_save_contour_process::static_execute(segmentation_structure, contour_destination);

  return 0;
}









