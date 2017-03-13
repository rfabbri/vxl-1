//:
// \file
// \author Caio SOUZA
// \date 03/13/2017
//
//        An algorithm to run geometric contour breaker
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include "dborl_geometric_contour_breaker_params.h"
#include "dborl_geometric_contour_breaker_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <dbdet/algo/dbdet_load_edg.h>
#include <dbdet/algo/dbdet_cem_file_io.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/sel/dbdet_curve_fragment_graph.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>

#include <dbdet/pro/dbdet_contour_breaker_geometric_process.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    dborl_geometric_contour_breaker_params_sptr params = 
        new dborl_geometric_contour_breaker_params("dborl_geometric_contour_breaker");  

    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web 
    // interface
    if (!params->print_params_xml(params->print_params_file()))
        vcl_cerr << "problems in writing params file to: " << 
            params->print_params_file() << vcl_endl;

    if (params->exit_with_no_processing() || params->print_params_only())
        return 0;

    //: always call this method to actually parse the input parameter file 
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
        return 1;

    //load the input image
    vcl_string img_file = params->input_object_dir_() + "/" 
        + params->input_object_name_() + params->input_extension_();

    if (!vul_file::exists(img_file)) 
    {
        vcl_cerr << "Cannot find image: " << img_file << vcl_endl;
        return 1;
    }

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(img_file.c_str());
    vidpro1_image_storage_sptr input_img = new vidpro1_image_storage();
    input_img->set_image(img_sptr);
    if (!img_sptr) 
    {
        vcl_cerr << "Cannot load image: " << img_file << vcl_endl;
        return 1;
    }

    //------------------------------------------
    vcl_string edg_file = params->input_object_dir_() + "/" 
        + params->input_object_name_() + ".edg";

    if (!vul_file::exists(edg_file)) 
    {
        vcl_cerr << "Cannot find edgemap: " << edg_file << vcl_endl;
        return 1;
    }

    // Grab edgemap
    dbdet_edgemap_sptr edgemap_sptr;
    dbdet_load_edg(edg_file, true, 1.0, edgemap_sptr);
    dbdet_edgemap_storage_sptr input_edg = new dbdet_edgemap_storage();
    input_edg->set_edgemap(edgemap_sptr);
    if (!edgemap_sptr) 
    {
        vcl_cerr << "Cannot load edgemap: " << edg_file << vcl_endl;
        return 1;
    }

   //--------------------------------------------
    vcl_string cem_file = params->input_object_dir_() + "/" 
        + params->input_object_name_() + ".cem";
    if (!vul_file::exists(cem_file)) 
    {
        vcl_cerr << "Cannot find cfrags: " << cem_file << vcl_endl;
        return 1;
    }

    // Grab curve fragments
    dbdet_sel_storage_sptr input_sel = new dbdet_sel_storage();
    dbdet_curve_fragment_graph& CFG = input_sel->CFG();
    dbdet_edgemap_sptr frags_edgemap_sptr = dbdet_load_cem(cem_file, CFG);
    input_sel->set_EM(frags_edgemap_sptr);
    if (!frags_edgemap_sptr) 
    {
        vcl_cerr << "Cannot load cfrags: " << cem_file << vcl_endl;
        return 1;
    }

    vcl_cout<<"************ Contour Breaker Geometric ************"<<vcl_endl;
    dbdet_contour_breaker_geometric_process cbg_pro;
    set_process_parameters_of_bpro1(*params, 
                                    cbg_pro, 
                                    params->algo_abbreviation_);  

    // Before we start the process lets clean input output
    cbg_pro.clear_input();
    cbg_pro.clear_output();

    // Add inputs (the order matters)
    cbg_pro.add_input(input_img);
    cbg_pro.add_input(input_edg);
    cbg_pro.add_input(input_sel);
    bool el_status = cbg_pro.execute();
    cbg_pro.finish();

    // Grab output from geometric contour breaker
    // if process did not fail

    // Set up storage for cbg
    vcl_vector<bpro1_storage_sptr> cbg_results;
    if ( el_status )
    {
        cbg_results = cbg_pro.get_output();
    }

    //Clean up after ourselves
    cbg_pro.clear_input();


    if (cbg_results.size() != 1) 
    {
        vcl_cerr << "Process output does not contain a sel data structure"
                 << vcl_endl;
        return 1;
    }

    //******************** Saving Section  *******************************
    // From this point forward, saving of edges, curvlets, contours
 
    //get the input storage class
    dbdet_sel_storage_sptr sel;
    sel.vertical_cast(cbg_results[0]);

    vcl_string cem_file_out = params->input_object_dir_() + "/" 
        + params->input_object_name_() + "_break_geom.cem";
    //save the contour fragment graph to the file
    bool retval = dbdet_save_cem(cem_file_out, sel->EM(), sel->CFG());

    if (!retval) {
      vcl_cerr << "Error while saving file: " << cem_file_out << vcl_endl;
      return 1;
    }
    //--------------------------------------------------------------------
    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    // Just to be safe lets flush everything
    vcl_cerr.flush();
    vcl_cout.flush();

    //Success we made it this far
    return 0;
}

