//:
// \file
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 05/08/08
//
//        An algorithm to run edge detection and linking algorithms on images, 
//        input object is a single image, output is an edge map or a 
//        boundary fragment map, with an option to associate to the image or 
//        to write to the object folder
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include <iostream>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <set>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>
#include <dbdet/pro/dbdet_generic_linker_process.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_save_cem_process.h>
#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

	std::vector<bpro1_storage_sptr> edge_det_results;
    std::cout<<"************* load in Edges *********"<<std::endl;
    dbdet_load_edg_process load_edg_pro;

    //load the input image
    std::string input_edg = argv[1];

    bpro1_filepath input(input_edg,".edg");
    load_edg_pro.parameters()->set_value("-edginput",input);
    
    // Before we start the process lets clean input output
    load_edg_pro.clear_input();
    load_edg_pro.clear_output();
    
    bool status = load_edg_pro.execute();
    // Grab output from load process for edges
    // if process did not fail
    if ( status )
    {
        edge_det_results = load_edg_pro.get_output();
    }

    load_edg_pro.finish();
    
    //Clean up after ourselves
    load_edg_pro.clear_input();
    load_edg_pro.clear_output();





    //******************** Edge Linking *********************************
    // Perform sel linking if we are not doing contour tracing

    // Set up storage for sel results
    std::vector<bpro1_storage_sptr> el_results;
	std::cout<<"************ Symbolic Edge Linking     ************"<<std::endl;
	dbdet_sel_process sel_pro;
//	set_process_parameters_of_bpro1(*params, sel_pro, params->tag_edge_linking_);
	                                  
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
	    el_results = sel_pro.get_output();
	}

	//Clean up after ourselves
	sel_pro.clear_input();
	sel_pro.clear_output();

	if (el_results.size() != 1) 
	{
	    std::cerr << "Process output does not contain a sel data structure"
	             << std::endl;
	    return 1;
	}

    //******************** Save Contours  *********************************
    // Change to the dbdet version by Yuliang
    std::cout<<"************ Saving Contours  ************"<<std::endl;

    std::string output_file;

    output_file = argv[2];

    bool write_status(false);
    std::cout << "output: " << output_file << std::endl;
    
    bpro1_filepath output(output_file, ".cem");

    // In this everything else, is .cem, .cemv , .cfg, etc
    dbdet_save_cem_process save_cem_pro;
    save_cem_pro.parameters()->set_value("-cem_filename",output);

    // Before we start the process lets clean input output
    save_cem_pro.clear_input();
    save_cem_pro.clear_output();

    // Kick of process
    save_cem_pro.add_input(el_results[0]);
    write_status = save_cem_pro.execute();
    save_cem_pro.finish();

    //Clean up after ourselves
    save_cem_pro.clear_input();
    save_cem_pro.clear_output();


    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;

    // Just to be safe lets flush everything
    std::cerr.flush();
    std::cout.flush();

    //Success we made it this far
    return 0;
}

