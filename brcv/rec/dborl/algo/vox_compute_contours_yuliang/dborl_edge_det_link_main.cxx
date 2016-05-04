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

#include "dborl_edge_det_link_params.h"
#include "dborl_edge_det_link_params_sptr.h"

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>
#include <vcl_set.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_generic_color_edge_detector_process.h>
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
    vcl_string color_thresh_;

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

	vcl_cout << input_img << vcl_endl;

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

    // if we do not use existing cem file to use
    if ( !params->use_existing_cem_())
    {
    //******************** Edge Detection ********************************
    // Perform third order edge detection if we are not tracing contours
    
    // Create output storage for edge detection
    vcl_vector<bpro1_storage_sptr> edge_det_results;
  
    //******************** Use Existing Edges ****************************
    if ( !params->trace_contours_())
    {
        if ( params->use_existing_edges_())
        {
            vcl_cout<<"************* Using Existing Edges *********"<<vcl_endl;
            dbdet_load_edg_process load_edg_pro;

            //load the input image
            vcl_string input_edg = params->input_object_dir_() + "/" 
                + params->input_object_name_() + params->edge_extension_();

            bpro1_filepath input(input_edg,params->edge_extension_());
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


        }


    }
    
    if ( !params->trace_contours_() && !params->use_existing_edges_())
    {
        vcl_cout<<"************ Edge Detection   ************"<<vcl_endl;
        

        // Perform color third order edge detection
        if (img_sptr->nplanes() == 3 && params->edge_detect_method_() == "gTO")
        {
 
            dbdet_third_order_color_edge_detector_process pro_color_edg;
            set_process_parameters_of_bpro1(*params, 
                                            pro_color_edg, 
                                            params->
                                            tag_color_edge_detection_);  

            vcl_vector<bpro1_param*> pars  = pro_color_edg.parameters()->get_param_list();
			for (unsigned i = 0; i < pars.size(); i++) 
			{
		        	if(pars[i]->name()=="-thresh")
				color_thresh_ = pars[i]->value_str();	
			}
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
		else if (img_sptr->nplanes() == 3 && params->edge_detect_method_() == "generic")
        {
			dbdet_generic_color_edge_detector_process pro_generic_edg;
            set_process_parameters_of_bpro1(*params, 
                                            pro_generic_edg, 
                                            params->
                                            tag_generic_edge_detection_);  

            vcl_vector<bpro1_param*> pars  = pro_generic_edg.parameters()->get_param_list();
			for (unsigned i = 0; i < pars.size(); i++) 
			{
		        	if(pars[i]->name()=="-thresh")
				color_thresh_ = pars[i]->value_str();	
			}
            // Before we start the process lets clean input output
            pro_generic_edg.clear_input();
            pro_generic_edg.clear_output();

            pro_generic_edg.add_input(inp);
            bool to_c_status = pro_generic_edg.execute();
            pro_generic_edg.finish();

            // Grab output from color generic edge detection
            // if process did not fail
            if ( to_c_status )
            {
                edge_det_results = pro_generic_edg.get_output();
            }

            //Clean up after ourselves
            pro_generic_edg.clear_input();
            pro_generic_edg.clear_output();			
		}
        else //(img_sptr->nplanes() != 3)
        {
        
        
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

 
        }

  
        if (edge_det_results.size() != 1 )
        {
            vcl_cerr<< "Process output does not contain an edge map"<<vcl_endl;
            return 1;
        
        }

    }

    //******************** Edge Linking *********************************
    // Perform sel linking if we are not doing contour tracing

    // Set up storage for sel results
    vcl_vector<bpro1_storage_sptr> el_results;
    if(params->edge_linking_method_() == "sel")
    {
	    if ( !params->trace_contours_())
	    {
		vcl_cout<<"************ Symbolic Edge Linking     ************"<<vcl_endl;
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
		    el_results = sel_pro.get_output();
		}

		//Clean up after ourselves
		sel_pro.clear_input();
		sel_pro.clear_output();

		if (el_results.size() != 1) 
		{
		    vcl_cerr << "Process output does not contain a sel data structure"
		             << vcl_endl;
		    return 1;
		}

	    }
    }
    else if(params->edge_linking_method_() == "gen")
    {
	    if ( !params->trace_contours_())
	    {
		vcl_cout<<"************ Generic Edge Linking     ************"<<vcl_endl;
		dbdet_generic_linker_process gen_pro;
		set_process_parameters_of_bpro1(*params, 
		                                gen_pro, 
		                                params->tag_gen_linking_);  
	    
		// Before we start the process lets clean input output
		gen_pro.clear_input();
		gen_pro.clear_output();

		// Use input from edge detection
		gen_pro.add_input(edge_det_results[0]);
		bool el_status = gen_pro.execute();
		gen_pro.finish();

		// Grab output from symbolic edge linking
		// if process did not fail
		if ( el_status )
		{
		    el_results = gen_pro.get_output();
		}

		//Clean up after ourselves
		gen_pro.clear_input();
		gen_pro.clear_output();

		if (el_results.size() != 1) 
		{
		    vcl_cerr << "Process output does not contain a gen data structure"
		             << vcl_endl;
		    return 1;
		}

	    }
    }
    else
    {
    	vcl_cout<<"ERROR: NO Edge Linking Method is selected!"<< vcl_endl;
    }

    //******************** Extract Contours *********************************
    // Since contour tracer already produces vsol dont bother unless
    // we peformed edge detection and linking

    // create storage for ec results
    vcl_vector<bpro1_storage_sptr> ec_results;

    if ( !params->trace_contours_())
    {
        vcl_cout<<"************ Extract Contours ************"<<vcl_endl;

        // Converts a sel object to a vsol object
        dbdet_sel_extract_contours_process ec_pro;
        set_process_parameters_of_bpro1(*params, 
                                        ec_pro, 
                                        params->tag_extract_contours_);  

        // Before we start the process lets clean input output
        ec_pro.clear_input();
        ec_pro.clear_output();
    
        // Use input from symbolic edge linking
        ec_pro.add_input(el_results[0]);
        bool ec_status = ec_pro.execute();
        ec_pro.finish();

        // Grab output from extraction of contours
        // if process does not fail
        if (ec_status)
        {
            ec_results = ec_pro.get_output();
        }

        //Clean up after ourselves
        ec_pro.clear_input();
        ec_pro.clear_output();

        //:get the output from edge detection   
        if (ec_results.size() != 1) 
        {
            vcl_cerr << "Process output does not contain a vsol structure"
                     << vcl_endl;

            return 1;
        }
    }

    //******************** Prune Contours *********************************
    // Prune contours based on contrast
    // Grab contours that remain after pruning
    vcl_vector<bpro1_storage_sptr> pc_results;
        
    if ( params->prune_contours_())
    {
        vcl_cout<<"************ Prune Contours   ************"<<vcl_endl;

        dbdet_prune_curves_process pc_pro;
        set_process_parameters_of_bpro1(*params, 
                                        pc_pro, 
                                        params->tag_prune_contours_);  
    
        // Before we start the process lets clean input output
        pc_pro.clear_input();
        pc_pro.clear_output();

        // Use input from vsol object
        if ( params->trace_contours_())
        {
            pc_pro.add_input(ct_results[0]);
        }
        else
        {
            pc_pro.add_input(ec_results[0]);
        }

        pc_pro.add_input(inp);
        bool pc_status = pc_pro.execute();
        pc_pro.finish();

        //:get the output from pruning curve fragments
        // if process does not fail
        if ( pc_status )
        {
            pc_results = pc_pro.get_output();
        }

        //Clean up after ourselves
        pc_pro.clear_input();
        pc_pro.clear_output();

        if (pc_results.size() != 1) 
        {
            vcl_cerr << "Process output does not contain a \
                         set of remaining contours after pruning"<<vcl_endl;
            return 1;
        }

    }

    //******************** Prune Contours using parameters from logistic regression*********************************
    // Prune contours based on contrast
    // Grab contours that remain after pruning
    //vcl_vector<bpro1_storage_sptr> pc_results;
        
    if ( params->prune_contours_logistic_())
    {
        vcl_cout<<"************ Prune Contours Logistic Regression  ************"<<vcl_endl;

        dbdet_prune_fragments_Logistic_Regression pc_pro;
        set_process_parameters_of_bpro1(*params, 
                                        pc_pro, 
                                        params->tag_prune_contours_logistic_);  
    
        // Before we start the process lets clean input output
        pc_pro.clear_input();
        pc_pro.clear_output();

        pc_pro.add_input(el_results[0]);

        pc_pro.add_input(inp);
        bool pc_status = pc_pro.execute();
        pc_pro.finish();

        //:get the output from pruning curve fragments
        // if process does not fail
        if ( pc_status )
        {
            pc_results = pc_pro.get_output();
        }

        //Clean up after ourselves
        pc_pro.clear_input();
        pc_pro.clear_output();

        if (pc_results.size() != 1) 
        {
            vcl_cerr << "Process output does not contain a \
                         set of remaining contours after pruning"<<vcl_endl;
            return 1;
        }

    }
    //******************** Saving Section  *******************************
    // From this point forward, saving of edges, curvlets, contours

    //******************** Saving Edges  *********************************
    // save this edge map into a file

    // Though this would be dumb by a user to save edges when they 
    // have a contour, lets be prudent and make sure this is off
    if ( !params->trace_contours_())
    {
        if (params->save_edges_()) 
        {
            vcl_cout<<"************ Saving  Edges    ************"<<vcl_endl;

            dbdet_save_edg_process save_edg_pro;

            vcl_string output_file;
            if (params->save_to_object_folder_())
            { 
                output_file = params->output_edge_link_folder_() + "/";
            }
            else 
            {

                output_file = params->input_object_dir_() + "/";

            }
        
            if (!vul_file::exists(output_file)) 
            {
                vul_file::make_directory(output_file);

            }
    
            output_file = output_file + 
                params->input_object_name_();

            bool status(false);

            if ( params->edge_extension_() == ".edg" ||  params->edge_extension_() == ".edg.gz" )
            {                
                output_file = output_file + params->edge_extension_();
                bpro1_filepath output(output_file,params->edge_extension_());

                save_edg_pro.parameters()->set_value("-edgoutput",output);

                // Before we start the process lets clean input output
                save_edg_pro.clear_input();
                save_edg_pro.clear_output();

                save_edg_pro.add_input(edge_det_results[0]);
                status = save_edg_pro.execute();
                save_edg_pro.finish();

                //Clean up after ourselves
                save_edg_pro.clear_input();
                save_edg_pro.clear_output();

            }
            else
            {
                vcl_cerr<<"Unknown edge typ to save with"<<vcl_endl;
                
                //Set status to bad
                status = false;
            }
            if ( !status )
            {
        
                vcl_cerr << "Problems in saving edge file: " 
                         << output_file + params->edge_extension_()
                         << vcl_endl;
                return 1;
     
            }
        }
    }

   
    //******************** Save Curvelets  *********************************
    // Though this would be dumb by a user to save curvlet when they 
    // have a contour, lets be prudent and make sure this is off
    if ( !params->trace_contours_())
    {

        if ( params->save_curvelets_() )
        {

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
                params->input_object_name_() +
                ".cvlet";
    
            bpro1_filepath output_cvlet(output_cvlet_file,
                                        ".cvlet");
    
            dbdet_save_cvlet_map_process save_cvlet_pro;
            save_cvlet_pro.parameters()->set_value("-cvlet_output",
                                                   output_cvlet);

            // Before we start the process lets clean input output
            save_cvlet_pro.clear_input();
            save_cvlet_pro.clear_output();

            // Kick of process
            save_cvlet_pro.add_input(el_results[0]);
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

        }
    }


	//********************** save edge indicators **************************
	// Save edge indicator in files to tell which edge particapate in curvelets as well as in unambiguous frags + hypothesis tree
    if ( !params->trace_contours_())
    {
        if (params->save_edges_inds_() && params->edge_linking_method_() == "sel") 
        {

			dbdet_sel_storage_sptr input_sel_0;
			input_sel_0.vertical_cast(el_results[0]);
			vcl_set<int> frags_hyp_ind = input_sel_0->CFG().participate_edge_id;
			dbdet_curvelet_map& cvlet_map = input_sel_0->CM();

            vcl_string output_ind_file;
            if (params->save_to_object_folder_())
            { 
                output_ind_file = params->output_edge_link_folder_() + "/";
            }
            else 
            {

                output_ind_file = params->input_object_dir_() + "/";

            }
        
            if (!vul_file::exists(output_ind_file)) 
            {
                vul_file::make_directory(output_ind_file);

            }
            vcl_cout<<"************ Saving cvlet edge ids ************"<<vcl_endl;

            vcl_string output_ind_file_0 = output_ind_file + params->input_object_name_() + "_cvlet_ind.txt";

			vcl_ofstream outfp_0(output_ind_file_0.c_str(), vcl_ios::out);

			if (!outfp_0){
				vcl_cout << " Error opening file  " << output_ind_file_0.c_str() << vcl_endl;
				return false;
			}
			outfp_0.precision(5);

			for (unsigned i=0; i<cvlet_map.EM_->edgels.size(); i++){
				//get the list curvelets anchored at this edge
				if(cvlet_map.curvelets(i).size()>0)
				{
					outfp_0 << i << vcl_endl;
				}
			}

            vcl_cout<<"************ Saving frags + hypothesis edge ids ************"<<vcl_endl;
    
            vcl_string output_ind_file_1 = output_ind_file + params->input_object_name_() + "_frags_hyp_ind.txt";

			vcl_ofstream outfp_1(output_ind_file_1.c_str(), vcl_ios::out);

			if (!outfp_1){
				vcl_cout << " Error opening file  " << output_ind_file_1.c_str() << vcl_endl;
				return false;
			}
			outfp_1.precision(5);

			vcl_set<int>::iterator it = frags_hyp_ind.begin();
			for (; it!=frags_hyp_ind.end(); it++)
				outfp_1<< (*it) << vcl_endl;


		}

	}

    //******************** Save Contours  *********************************
    // Change to the dbdet version by Yuliang
    vcl_cout<<"************ Saving Contours  ************"<<vcl_endl;

    vcl_string output_file;
    if (params->save_to_object_folder_())
    { 
        output_file = params->output_edge_link_folder_() + "/";
    }
    else 
    {
        output_file = params->input_object_dir_() + "/";
    }
        
    if (!vul_file::exists(output_file)) 
    {
        vul_file::make_directory(output_file);
        
    }
    
    output_file = output_file + params->input_object_name_();

    bool write_status(false);
    
    if ( params->trace_contours_() )
    {

        if ( params->output_extension_() == ".con" )
        {
            bpro1_filepath output(output_file,params->output_extension_());

            vidpro1_save_con_process save_con_pro;
            save_con_pro.parameters()->set_value("-conoutput",output);

            // Before we start the process lets clean input output
            save_con_pro.clear_input();
            save_con_pro.clear_output();

            // Kick of process
            save_con_pro.add_input(ct_results[0]);
            write_status = save_con_pro.execute();
            save_con_pro.finish();

            //Clean up after ourselves
            save_con_pro.clear_input();
            save_con_pro.clear_output();
        }
        else if ( params->output_extension_() == ".cem" ||
                  params->output_extension_() == ".cemv" || 
                  params->output_extension_() == ".cemv.gz")
        {
            output_file = output_file + params->output_extension_();
            bpro1_filepath output(output_file,params->output_extension_());

            // In this everything else, is .cem, .cemv , .cfg, etc
            dbdet_save_cem_process save_cem_pro;
            save_cem_pro.parameters()->set_value("-cemoutput",output);

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

        }
        else
        {

            // Unknown file extension encountered
            vcl_cerr<<"Unknow file extension " << params->output_extension_() 
              << " to save with OR ..."<<vcl_endl;
            vcl_cerr<<"Bad combination of inputs parameters  ..." <<vcl_endl;
            vcl_cerr<<"For example, saving output of sel with .con ext"
                    <<vcl_endl;

            // set write status to bad
            write_status=false;
 
        }
 
    }
    else if (params->output_extension_() == ".cem" ||
                params->output_extension_() == ".cemv" ||
                params->output_extension_() == ".cemv.gz")
    {

        output_file = output_file /*+ "_c" + color_thresh_*/ + params->output_extension_();
        bpro1_filepath output(output_file,params->output_extension_());

        // In this everything else, is .cem, .cemv , .cfg, etc
        dbdet_save_cem_process save_cem_pro;
        save_cem_pro.parameters()->set_value("-cem_filename",output);

        // Before we start the process lets clean input output
        save_cem_pro.clear_input();
        save_cem_pro.clear_output();

   
        if ( params->prune_contours_() )
        {
            save_cem_pro.add_input(pc_results[0]);
        }
		else if (params->prune_contours_logistic_())
		{
			save_cem_pro.add_input(pc_results[0]);
		}
        else
        {
            save_cem_pro.add_input(el_results[0]);

        }

        write_status = save_cem_pro.execute();
        save_cem_pro.finish();

        //Clean up after ourselves
        save_cem_pro.clear_input();
        save_cem_pro.clear_output();
    }
    else if (params->output_extension_() == ".cvlet")
    {
      vcl_cout << "Just outputting .cvlet, no contours.\n";
    }
    else 
    {
        // Unknown file extension encountered
        vcl_cerr<<"Unknow file extension " << params->output_extension_() 
          << " to save with OR ..."<<vcl_endl;
        vcl_cerr<<"Bad combination of inputs parameters  ..." <<vcl_endl;
        vcl_cerr<<"For example, saving output of sel with .con ext"<<vcl_endl;

        // set write status to bad
        write_status=false;
    }

    if ( !write_status )
    {
        
        vcl_cerr << "Problems in saving dbdet contour file: " 
                 << output_file << vcl_endl;
        return 1;
     
    }


    }//end of the condition that no existing cem 


   //****************************** Load the cem file generated **************************************
    vcl_cout<<"************ Load CEM file ************"<<vcl_endl;
    vcl_string cem_file;

    if (params->save_to_object_folder_())
    { 
        cem_file = params->output_edge_link_folder_() + "/";
    }
    else 
    {
        cem_file = params->input_object_dir_() + "/";
    }

    if (!vul_file::exists(cem_file)) 
    {
        vul_file::make_directory(cem_file);

    }
    
    //params->output_image_extension_() == ".bmp";
    cem_file = cem_file + params->input_object_name_() + ".cem";

    bpro1_filepath cem_path(cem_file, ".cem");

    vcl_vector<bpro1_storage_sptr> load_cem_results;
    dbdet_load_cem_process load_cem_pro;
    load_cem_pro.parameters()->set_value("-cem_filename",cem_path);
    // Before we start the process lets clean input output
    load_cem_pro.clear_input();
    load_cem_pro.clear_output();

    // Kick of process
    bool load_cem_status = load_cem_pro.execute();
    load_cem_pro.finish();

	if ( load_cem_status )
	{
	    load_cem_results = load_cem_pro.get_output();
	}

    //Clean up after ourselves
    load_cem_pro.clear_input();
    load_cem_pro.clear_output();

    if ( !load_cem_status )
    {

        vcl_cerr << "Problems in loading cem file" 
                 << cem_file << vcl_endl;
        return 1;

    }


    //******************** Convert to Binary Map *********************************

    vcl_vector<bpro1_storage_sptr> cb_results;
        
    if ( params->convert_edgemap_to_image_())
    {
	vcl_cout<<"************ Convert to Binary Map   ************"<<vcl_endl;

	dbdet_convert_edgemap_to_image_process cb_pro;
	set_process_parameters_of_bpro1(*params, 
		                        cb_pro, 
		                        params->tag_convert_edgemap_to_image_);  

	// Before we start the process lets clean input output
	cb_pro.clear_input();
	cb_pro.clear_output();

	// Create vid pro storage
	//vidpro1_image_storage_sptr inp_1 = new vidpro1_image_storage();
	//inp_1->set_image(img_sptr);
	// Use input from prune contours

	cb_pro.add_input(load_cem_results[1]);

	cb_pro.add_input(inp);

	bool cb_status = cb_pro.execute();
	cb_pro.finish();

	//:get the output from pruning curve fragments
	// if process does not fail
	if ( cb_status )
	{
	    cb_results = cb_pro.get_output();
	}

	//Clean up after ourselves
	cb_pro.clear_input();
	cb_pro.clear_output();

	if (cb_results.size() != 1) 
	{
	    vcl_cerr << "Process output does not contain a \
		         binary map"<<vcl_endl;
	    return 1;
	}

	//******************** Save binary map*********************************

	vcl_cout<<"************ Saving Binary Map ************"<<vcl_endl;
	vcl_string output_binary_file;

	if (params->save_to_object_folder_())
	{ 
	output_binary_file = params->output_edge_link_folder_() + "/";
	}
	else 
	{
	output_binary_file = params->input_object_dir_() + "/";
	}

	if (!vul_file::exists(output_binary_file)) 
	{
	vul_file::make_directory(output_binary_file);

	}

	params->output_image_extension_() = ".png";
	output_binary_file = output_binary_file + params->input_object_name_() + params->output_image_extension_();

	bpro1_filepath output_binary(output_binary_file,
		                params->output_image_extension_());

	vidpro1_save_image_process save_image_pro;
	save_image_pro.parameters()->set_value("-filename",
		                           output_binary);

	// Before we start the process lets clean input output
	save_image_pro.clear_input();
	save_image_pro.clear_output();

	// Kick of process
	save_image_pro.add_input(cb_results[0]);
	bool save_image_status = save_image_pro.execute();
	save_image_pro.finish();

	//Clean up after ourselves
	save_image_pro.clear_input();
	save_image_pro.clear_output();

	if ( !save_image_status )
	{

	vcl_cerr << "Problems in saving .bmp binary contour map: " 
		 << output_binary_file << vcl_endl;
	return 1;

	}
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

