//:
// \file
// \author Ricardo Fabbri
// \date 07/23/2010
//
//        An algorithm to run edge detection, curvelet detection
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include "dborl_edge_det_link_params.h"
#include "dborl_edge_det_link_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    dborl_edge_det_link_params_sptr params = 
        new dborl_edge_det_link_params("dborl_edge_det_link");  

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
    vcl_string input_img = params->input_object_dir_() + "/" 
        + params->input_object_name_() + params->input_extension_();

    if (!vul_file::exists(input_img)) 
    {
        vcl_cerr << "Cannot find image: " << input_img << vcl_endl;
        return 1;
    }

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(input_img.c_str());
    if (!img_sptr) 
    {
        vcl_cerr << "Cannot load image: " << input_img << vcl_endl;
        return 1;
    }

    // Create vid pro storage
    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(img_sptr);

    // Lets trace out contours ignore everything else except for saving
    
    //******************** Edge Detection ********************************
    // Perform third order edge detection if we are not tracing contours
    
    // Create output storage for edge detection
    vcl_vector<bpro1_storage_sptr> edge_det_results;
    
    {
    vcl_cout<<"************ Edge Detection   ************"<<vcl_endl;
    /*
    if (img_sptr->nplanes() != 3)
    {
    */
    
        dbdet_third_order_edge_detector_process pro_gray_edg;
        set_process_parameters_of_bpro1(*params, 
                                        pro_gray_edg, 
                                        params->tag_gray_edge_detection_);

        // Before we start the process lets clean input output
        pro_gray_edg.clear_input();
        pro_gray_edg.clear_output();

        // Start the process sequence
        pro_gray_edg.add_input(inp);
        bool to_g_status = pro_gray_edg.execute();
        pro_gray_edg.finish();

        // Grab output from gray scale third order edge detection
        // if process did not fail
        if ( to_g_status )
        {
            edge_det_results = pro_gray_edg.get_output();
        }

        //Clean up after ourselves
        pro_gray_edg.clear_input();
        pro_gray_edg.clear_output();

        /* XXX rfabbri
    } 
    // Perform color third order edge detection
    else 
    {

        dbdet_third_order_color_edge_detector_process pro_color_edg;
        set_process_parameters_of_bpro1(*params, 
                                        pro_color_edg, 
                                        params->
                                        tag_color_edge_detection_);  

        // Before we start the process lets clean input output
        pro_color_edg.clear_input();
        pro_color_edg.clear_output();

        pro_color_edg.add_input(inp);
        bool to_c_status = pro_color_edg.execute();
        pro_color_edg.finish();

        // Grab output from color third order edge detection
        // if process did not fail
        if ( to_c_status )
        {
            edge_det_results = pro_color_edg.get_output();
        }

        //Clean up after ourselves
        pro_color_edg.clear_input();
        pro_color_edg.clear_output();

    }
    */
    }

    if (edge_det_results.size() != 1 )
    {
        vcl_cerr<< "Process output does not contain an edge map"<<vcl_endl;
        return 1;
    
    }

    //******************** Edge Linking *********************************
    // Perform sel linking if we are not doing contour tracing

    // Set up storage for sel results
    vcl_vector<bpro1_storage_sptr> sel_results;

    {
    vcl_cout<<"************ Edge Linking     ************"<<vcl_endl;
    dbdet_sel_process sel_pro;
    set_process_parameters_of_bpro1(*params, 
                                    sel_pro, 
                                    params->tag_edge_linking_);  

    // Before we start the process lets clean input output
    sel_pro.clear_input();
    sel_pro.clear_output();

    // Use input from edge detection
    sel_pro.add_input(edge_det_results[0]);
    bool el_status = sel_pro.execute();
    sel_pro.finish();

    // Grab output from symbolic edge linking
    // if process did not fail
    if ( el_status )
    {
        sel_results = sel_pro.get_output();
    }

    //Clean up after ourselves
    sel_pro.clear_input();
    }

    if (sel_results.size() != 1) 
    {
        vcl_cerr << "Process output does not contain a sel data structure"
                 << vcl_endl;
        return 1;
    }

    //******************** Saving Section  *******************************
    // From this point forward, saving of edges, curvlets, contours

    vcl_cout<<"************ Saving Curvelets ************"<<vcl_endl;
    vcl_string output_cvlet_file;

    if (params->save_to_object_folder_())
    { 
        output_cvlet_file = params->output_edge_link_folder_() + "/";
    }
    else 
    {
        output_cvlet_file = params->input_object_dir_() + "/";
    }

    if (!vul_file::exists(output_cvlet_file)) 
    {
        vul_file::make_directory(output_cvlet_file);

    }

    output_cvlet_file = output_cvlet_file + 
        params->input_object_name_() + params->output_extension_();

    bpro1_filepath output_cvlet(output_cvlet_file,
                                params->output_extension_());

    dbdet_save_cvlet_map_process save_cvlet_pro;
    save_cvlet_pro.parameters()->set_value("-cvlet_output",
                                           output_cvlet);

    // Before we start the process lets clean input output
    save_cvlet_pro.clear_input();
    save_cvlet_pro.clear_output();

    // Kick of process
    save_cvlet_pro.add_input(sel_results[0]);
    bool write_cv_status = save_cvlet_pro.execute();
    save_cvlet_pro.finish();

    //Clean up after ourselves
    save_cvlet_pro.clear_input();
    save_cvlet_pro.clear_output();

    if ( !write_cv_status )
    {

        vcl_cerr << "Problems in saving .cvlet curvlet file: " 
                 << output_cvlet_file << vcl_endl;
        return 1;

    }

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

