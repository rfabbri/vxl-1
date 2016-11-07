//:
// \file
// \author Yuliang Guo
// \date 09/25/13
//
//        An pipeline compute shoack graphs from ground truth silhouette
//      
// \verbatim
//   Modifications
//  
// \endverbatim

//
//

#include "dborl_compute_xgraphs_from_silhou_params.h"
#include "dborl_compute_xgraphs_from_silhou_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <vul/vul_file_iterator.h>
#include <bbas/bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vil/vil_load.h>
#include <vcl_cstdlib.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
//#include <vidpro1/process/vidpro1_save_cem_process.h>
//#include <vidpro1/process/vidpro1_save_con_process.h>

//#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
//#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
//#include <dbdet/pro/dbdet_sel_process.h>
//#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
//#include <dbdet/pro/dbdet_prune_curves_process.h>
//#include <dbdet/pro/dbdet_save_edg_process.h>
//#include <dbdet/pro/dbdet_load_edg_process.h>
//#include <dbdet/pro/dbdet_edgemap_storage.h>
//#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
//#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h>
//#include <dbdet/pro/dbdet_save_cem_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
//#include <dbsk2d/pro/dbsk2d_gap_transform_process.h>
#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>                  
#include <dbsk2d/pro/dbsk2d_load_esf_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>

#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

// Convert esf file to an xgraph
bool convert_esf_to_xgraph(const vcl_string& esf_file, float tol,  
                           dbsksp_xshock_graph_sptr& xgraph);

int main(int argc, char *argv[]) {

    // Let time how long this takes
    // Start timer
    vul_timer t;

    dborl_compute_xgraphs_from_silhou_params_sptr params = 
        new dborl_compute_xgraphs_from_silhou_params("dborl_compute_xgraphs_from_silhou");  

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
    
    // Create storage
    vcl_vector<bpro1_storage_sptr> ct_results;
    if ( params->trace_contours_())
    {
        vcl_cout<<"************ Contour Tracing  ************"<<vcl_endl;

        dbdet_contour_tracer_process ct_pro;
        set_process_parameters_of_bpro1(*params, 
                                        ct_pro, 
                                        params->tag_contour_tracing_);
        
        // Before we start the process lets clean input output
        ct_pro.clear_input();
        ct_pro.clear_output();

        // Start the process sequence
        ct_pro.add_input(inp);
        bool ct_status = ct_pro.execute();
        ct_pro.finish();

        // Grab output from gray scale third order edge detection
        // if process did not fail
        if ( ct_status )
        {
            ct_results = ct_pro.get_output();
        }

        //Clean up after ourselves
        ct_pro.clear_input();
        ct_pro.clear_output();

        if (ct_results.size() != 1 )
        {
            vcl_cerr<< "Contour tracing failed"<<vcl_endl;
            return 1;
        }

    }

// Grab the underlying contours
    vidpro1_vsol2D_storage_sptr vsol_contour_storage = 
        vidpro1_vsol2D_storage_new();
    vsol_contour_storage.vertical_cast(ct_results[0]);

    if ( params->add_bbox_())
    {
        vcl_cout<<"************  Compute  Bbox  *************"<<vcl_endl;

        // Grab the underlying contours
        vidpro1_vsol2D_storage_sptr vsol_contour_storage;
        vsol_contour_storage.vertical_cast(ct_results[0]);

        // create new bounding box
        vsol_box_2d_sptr bbox = new vsol_box_2d();

        // Determine largest bounding box 
        vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = 
            vsol_contour_storage->all_data();
    
        for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
        {
            bbox->grow_minmax_bounds(vsol_list[b]->get_bounding_box());
        }

        // Enlarge bounding box from size
        // Calculate xcenter, ycenter
        double xcenter = bbox->width()/2.0;
        double ycenter = bbox->height()/2.0;

        // Translate to center and scale
        double xmin_scaled = ((bbox->get_min_x()-xcenter)*2)+xcenter;
        double ymin_scaled = ((bbox->get_min_y()-ycenter)*2)+ycenter;
        double xmax_scaled = ((bbox->get_max_x()-xcenter)*2)+xcenter;
        double ymax_scaled = ((bbox->get_max_y()-ycenter)*2)+ycenter;
        
        bbox->add_point(xmin_scaled,ymin_scaled);
        bbox->add_point(xmax_scaled,ymax_scaled);

        vcl_cout << "bbox (minx, miny) (maxx, maxy) (width, height): " 
                 << "("   << bbox->get_min_x() << ", " << bbox->get_min_y() 
                 << ") (" << bbox->get_max_x() << ", " << bbox->get_max_y() 
                 << ") (" 
                 << bbox->width() << ", " 
                 << bbox->height() << ")"<<vcl_endl;
 
        // Add to vidpro storage this new bounding box
        vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(bbox);
        vsol_contour_storage->add_object(box_poly->cast_to_spatial_object());

    }

    
    //******************** IShock Computation *******************************
    vcl_cout<<"************ Computing Shock *************"<<vcl_endl;

    dbsk2d_compute_ishock_process shock_pro;
    set_process_parameters_of_bpro1(*params, 
                                    shock_pro, 
                                    params->tag_compute_ishock_);  
    
    // Before we start the process lets clean input output
    shock_pro.clear_input();
    shock_pro.clear_output();

    // Create empty image stroage
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

    // Use input from edge detection
    shock_pro.add_input(image_storage);
    shock_pro.add_input(vsol_contour_storage);
    bool ishock_status = shock_pro.execute();
    shock_pro.finish();

    // Grab output from symbolic edge linking
    vcl_vector<bpro1_storage_sptr> shock_results;

    // If ishock status is bad we will keep iterating with noise till we get 
    // a valid shock computation otherwise call it quits
    if (!ishock_status)
    {
        // Add noise to parameter set
        shock_pro.parameters()->set_value("-b_noise",true);

        // Clean up before we start running
        shock_pro.clear_input();
        shock_pro.clear_output();

        unsigned int i(0);
        unsigned int num_iterations = params->num_iter_();

        for ( ; i < num_iterations; ++i)
        {
            vcl_cout<<vcl_endl;
            vcl_cout<<"************ Retry Compute Shock,iter: "
                    <<i+1<<" *************"<<vcl_endl;
          
            // Add inputs
            shock_pro.add_input(image_storage);
            shock_pro.add_input(vsol_contour_storage);

            // Kick off process again
            ishock_status = shock_pro.execute();
            shock_pro.finish();

            if ( ishock_status )
            {
                // We have produced valid shocks lets quit
                break;
                
            }

            // Clean up after ourselves
            shock_pro.clear_input();
            shock_pro.clear_output();

        }
    }
        
    if ( ishock_status )
    {
        shock_results = shock_pro.get_output();

        // Clean up after ourselves
        shock_pro.clear_input();
        shock_pro.clear_output();

    }

   
    if (shock_results.size() != 1) 
    {
        vcl_cerr << "Shock computation failed after "<<params->num_iter_()
                 <<" iterations"
                 << vcl_endl;
        return 1;
    }

    //********************   Prune Ishock    *******************************

    // Perform Prune Ishock on Intrinsinc Shock Graph
    vcl_vector<bpro1_storage_sptr> shock_ps_results;
	if ( params->prune_ishock_())
    {

        vcl_cout<<"************  Prune Ishock  *************"<<vcl_endl;

        dbsk2d_prune_ishock_process ps_pro;
        set_process_parameters_of_bpro1(*params, 
                                        ps_pro, 
                                        params->tag_prune_ishock_);


        // Before we start the process lets clean input output
        ps_pro.clear_input();
        ps_pro.clear_output();

        // Grab the underlying contours
//        dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
//        output_shock.vertical_cast(shock_results[0]);
        
//        output_shock->set_image(img_sptr);
        
        // Use input from ishock computation
        ps_pro.add_input(shock_results[0]);
        bool ps_status = ps_pro.execute();
        ps_pro.finish();
        // Grab output from symbolic edge linking
        if ( ps_status )
        {
            shock_ps_results = ps_pro.get_output();
        }

        //Clean up after ourselves
        ps_pro.clear_input();
        ps_pro.clear_output();

        // It has two outputs
        if (shock_ps_results.size() != 1) 
        {
            vcl_cerr << "Shock after prune shock is Invalid!"
                     << vcl_endl;
            return 1;
        }

    }


/*
    //********************   Gap Transform    *******************************

    // Perform Gap Transform on Intrinsinc Shock Graph
    vcl_vector<bpro1_storage_sptr> shock_gt_results;

    if ( params->gap_transform_())
    {

        vcl_cout<<"************  Gap Transform  *************"<<vcl_endl;

        dbsk2d_gap_transform_process gt_pro;
        set_process_parameters_of_bpro1(*params, 
                                        gt_pro, 
                                        params->tag_gap_transform_);


        // Before we start the process lets clean input output
        gt_pro.clear_input();
        gt_pro.clear_output();

        // Grab the underlying contours
        dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
        output_shock.vertical_cast(shock_results[0]);

        // Open up image for gap transform
 
        //load the input image
        vcl_string input_img = params->input_object_dir_() + "/" 
            + params->input_object_name_() + params->input_image_extension_();

        if (!vul_file::exists(input_img)) 
        {
            vcl_cerr << "Cannot find image for gap transform: " 
                     << input_img << vcl_endl;
            return 1;
        }

        // Grab image
        vil_image_resource_sptr img_sptr = 
            vil_load_image_resource(input_img.c_str());
        
        if (!img_sptr) 
        {
            vcl_cerr << "Cannot load image for gap transform: " << 
                input_img << vcl_endl;
            return 1;
        }
        
        output_shock->set_image(img_sptr);
        
        // Use input from ishock computation
        gt_pro.add_input(output_shock);
        bool gt_status = gt_pro.execute();
        gt_pro.finish();

        // Grab output from symbolic edge linking
        if ( gt_status )
        {
            shock_gt_results = gt_pro.get_output();
        }

        //Clean up after ourselves
        gt_pro.clear_input();
        gt_pro.clear_output();

        // It has two outputs
        if (shock_gt_results.size() != 2) 
        {
            vcl_cerr << "Shock after gap transform is Invalid!"
                     << vcl_endl;
            return 1;
        }

    }

*/

    //******************** Sample Shocks  ********************************
    vcl_cout<<"************  Sampling Shock *************"<<vcl_endl;
    
    dbsk2d_sample_ishock_process sample_sg_pro;
    set_process_parameters_of_bpro1(*params, 
                                    sample_sg_pro, 
                                    params->tag_sample_shock_);


    // Before we start the process lets clean input output
    sample_sg_pro.clear_input();
    sample_sg_pro.clear_output();

    // Use input from either ishock computation or gap_transform
    if ( params->prune_ishock_())
    {
        sample_sg_pro.add_input(shock_ps_results[0]);
    }
    else
    {
        sample_sg_pro.add_input(shock_results[0]);
    }

    // Kick of process
    bool sample_status = sample_sg_pro.execute();
    sample_sg_pro.finish();

    // Grab output from sampling
    vcl_vector<bpro1_storage_sptr> sample_shock_results;
    if ( sample_status )
    {
        sample_shock_results   = sample_sg_pro.get_output();
    }

    //Clean up after ourselves
    sample_sg_pro.clear_input();
    sample_sg_pro.clear_output();

    if (sample_shock_results.size() != 1) 
    {
        vcl_cerr << "Sampling of Intrinsinc Shock Computation Failed"
                 << vcl_endl;
        return 1;
    }

    
    //******************** Save Shocks   ********************************

	if ( params->save_esf_())
    {
		vcl_cout<<"************   Saving  Shock *************"<<vcl_endl;    

		dbsk2d_save_esf_process save_sg_pro;
		
		vcl_string output_file;
		if (params->save_to_object_folder_())
		{ 
		    output_file = params->output_shock_folder_() + "/";
		}
		else 
		{
		    output_file = params->input_object_dir_() + "/";
		}
		    
		if (!vul_file::exists(output_file)) 
		{
		    vul_file::make_directory(output_file);
		    
		}
		
		output_file = output_file + params->input_object_name_()+
		    params->output_extension_();
		   
		bpro1_filepath output(output_file,params->output_extension_());

		save_sg_pro.parameters()->set_value("-esfoutput",output);

		// Before we start lets clean input output
		save_sg_pro.clear_input();
		save_sg_pro.clear_output();

		save_sg_pro.add_input(sample_shock_results[0]);
		bool status = save_sg_pro.execute();
		save_sg_pro.finish();

		//Clean up after ourselves
		save_sg_pro.clear_input();
		save_sg_pro.clear_output();

		if ( !status )
		{
		    vcl_cerr << "Problems in saving extrinsinc shock file: " 
		             << output_file << vcl_endl;
		    return 1;

		}
	}


    //******************** Convert esf to xgraph   ********************************
    vcl_cout<<"************  Convert esf to xgraph  *************\n";
    float tol = params->convert_sk2d_to_sksp_tol_();
	dbsksp_xshock_graph_sptr xgraph = 0;

  	// retrieve output
  	dbsk2d_shock_storage_sptr sk2d_storage = 0;
  	sk2d_storage.vertical_cast(sample_shock_results[0]);

  //2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph

	// set parameters for storage
	dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;
	fit_process.parameters()->set_value("-distance-rms-error-threshold", tol);
	fit_process.clear_input();
	fit_process.clear_output();
	fit_process.add_input(sk2d_storage.ptr());
	bool success = fit_process.execute();	

	if (!success)
	{
		vcl_cout << "ERROR: conversion failed.\n";

		// Update status results
		params->percent_completed = 0.0f;
		params->exit_code = 1;
		params->exit_message = "esf file conversion failed !";
		params->print_status_xml();

		return EXIT_FAILURE;
	}

	// retrieve output
	dbsksp_xgraph_storage_sptr sksp_storage;
	sksp_storage.vertical_cast(fit_process.get_output()[0]);  

	//3) Save output
	xgraph = sksp_storage->xgraph();

	// save xgraph to folder
/*
	vcl_string xgraph_folder = params->save_to_object_folder_() ? 
	params->object_dir_() : params->output_folder_();

	// create folder if it doesn't exist yet
	if (!vul_file::is_directory(xgraph_folder))
	{
		vul_file::make_directory(xgraph_folder);
	}
*/

    vcl_string xgraph_file;
    if (params->save_to_object_folder_())
    { 
        xgraph_file = params->output_shock_folder_() + "/";
    }
    else 
    {
        xgraph_file = params->input_object_dir_() + "/";
    }
        
    if (!vul_file::exists(xgraph_file)) 
    {
        vul_file::make_directory(xgraph_file);
        
    }
    
    xgraph_file = xgraph_file + params->input_object_name_()+ params->xgraph_extension_();

//	vcl_string xgraph_file = xgraph_folder + "/" + params->input_object_name_() + params->xgraph_extension_();
	vcl_cout << "\nSaving xgraph to: " << xgraph_file << "\n";
	success = x_write(xgraph_file, xgraph);

	if (!success)
	{
		vcl_cout << "\nERROR: Saving xgraph file failed.\n";
		// Update status results
		params->percent_completed = 0.0f;
		params->exit_code = 1;
		params->exit_message = "Saving -xgraph.xml file failed!";
		params->print_status_xml();

		return EXIT_FAILURE;
	}


    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    return 0;
}

//------------------------------------------------------------------------------
// Convert esf file to an xgraph
bool convert_esf_to_xgraph(const vcl_string& esf_file, float tol,  
                           dbsksp_xshock_graph_sptr& xgraph)
{
  // sanitize output container
  xgraph = 0;

  //1) Load esf file

  dbsk2d_load_esf_process load_process;
  load_process.parameters()->set_value("-esfinput" , bpro1_filepath(esf_file));
  load_process.clear_input();
  load_process.clear_output();

  bool success = load_process.execute();
  if (!success)
    return false;

  // retrieve output
  dbsk2d_shock_storage_sptr sk2d_storage = 0;
  sk2d_storage.vertical_cast(load_process.get_output()[0]);


  //2) Convert dbsk2d_shock_graph to dbsksp_xshock_graph

  // set parameters for storage
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process fit_process;
  fit_process.parameters()->set_value("-distance-rms-error-threshold", tol);
  fit_process.clear_input();
  fit_process.clear_output();
  fit_process.add_input(sk2d_storage.ptr());
  success = fit_process.execute();

  if (!success)
    return false;

  // retrieve output
  dbsksp_xgraph_storage_sptr sksp_storage;
  sksp_storage.vertical_cast(fit_process.get_output()[0]);  

  //3) Save output
  xgraph = sksp_storage->xgraph();
  return true;  
}

