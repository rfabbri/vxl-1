
#include <iostream>
#include <fstream>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_timer.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_cem_process.h>
#include <vidpro1/process/vidpro1_save_con_process.h>
#include <vidpro1/process/vidpro1_save_image_process.h>

#include <dbdet/pro/dbdet_third_order_edge_detector_process.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <dbdet/pro/dbdet_sel_process.h>

#include <dbdet/pro/dbdet_evaluation_process.h>
#include <dbdet/pro/dbdet_prune_fragments_Logistic_Regression.h>
#include <dbdet/pro/dbdet_sel_extract_contours_process.h>
#include <dbdet/pro/dbdet_contour_tracer_process.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <dbdet/pro/dbdet_load_cem_process.h>
#include <dbdet/pro/dbdet_load_edg_process.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_save_cvlet_map_process.h>
#include <dbdet/pro/dbdet_convert_edgemap_to_image_process.h>

//#include <pro/brld_edge_point_tangent_bg_model_process.h> 
#include <pro/brld_edge_point_tangent_fg_detect_process.h>
#include <pro/brld_edge_image_to_oriented_edgemap_process.h>
#include <pro/brld_oriented_edgemap_to_edge_image_process.h>
#include <pro/brld_edge_point_tangent_fg_detect_process.h>

#include "brld_edge_point_tangent_bg_model_process_2.h"


int main(int argc, char *argv[]) {
    // Let time how long this takes
    // Start timer
    vul_timer t;

	int num = 41;
	std::cout<<"************ Load in txt file containing processing edge map file names ************" <<std::endl;

	std::string txt_file = argv[1];
	std::string output_path = argv[2];

	std::vector<std::string> filenames; // save all the edge maps for the whole sequence

	std::ifstream myfile (txt_file.c_str());
	std::string line;
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			filenames.push_back(line);
			//std::cout << filenames.back() << std::endl;
		}
		myfile.close();
		myfile.close();
	}
  	else std::cout << "Unable to open file" <<std::endl;	


	std::cout<<"************ load in the first set of working on edge files **************** " << std::endl;


	// construct the bg model for the center frame
	std::vector<bpro1_storage_sptr> bg_results;
	brld_edge_point_tangent_bg_model_process_2 bg_pro;
	bg_pro.clear_input();
	bg_pro.clear_output();




	std::deque<vidpro1_image_storage_sptr> edge_images_working_on; // maybe just construct the edge image version
	for (int i=0; i< num; i++)
	{
		std::vector<bpro1_storage_sptr> edge_det_results;
		dbdet_load_edg_process load_edg_pro;

        //load the input image
        std::string input_edg = filenames[i];

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
		
		//dbdet_edgemap_storage_sptr input_edgemap; // not sure if its deleted after iteration
  		//input_edgemap.vertical_cast(edge_det_results[0]);
		//dbdet_edgemap_sptr EM = input_edgemap->get_edgemap();

		// convert the edge map to edge image which is used in build BG model as well as FG detection
		std::cout<<"convert edge map to edge image"<<std::endl;
		std::vector<bpro1_storage_sptr> edge_map2img_results;
		brld_oriented_edgemap_to_edge_image_process edge_map2img_pro;

		edge_map2img_pro.clear_input();
		edge_map2img_pro.clear_output();
		edge_map2img_pro.add_input(edge_det_results[0]);

		//std::cout<<"6"<<std::endl;
		bool edge_map2img_status = edge_map2img_pro.execute();
		edge_map2img_pro.finish();

		// if process did not fail
		if ( edge_map2img_status )
		{
		    edge_map2img_results = edge_map2img_pro.get_output();
		}

		//Clean up after ourselves
		edge_map2img_pro.clear_input();
		edge_map2img_pro.clear_output();

		vidpro1_image_storage_sptr edge_image;
		edge_image.vertical_cast(edge_map2img_results[0]);

		edge_images_working_on.push_back(edge_image); // where is pisical address of EM?

		bg_pro.add_input(edge_map2img_results[0]);

	}
	std::cout<<"construct BG model"<<std::endl;	
	bool bg_status = bg_pro.execute();
	bg_pro.finish();

	if(bg_status)
		bg_results = bg_pro.get_output();

	bg_pro.clear_input();
	bg_pro.clear_output();

	std::cout<<"refine FG edges"<<std::endl;
	//detect the foreground edges
	std::vector<bpro1_storage_sptr> fg_results;
    brld_edge_point_tangent_fg_detect_process fg_pro;
	fg_pro.clear_input();
	fg_pro.clear_output();

	fg_pro.add_input(edge_images_working_on[20]);
	fg_pro.add_input(bg_results[0]);

	//std::cout<<"7"<<std::endl;
	bool fg_status = fg_pro.execute();
	fg_pro.finish();
	//std::cout<<"8"<<std::endl;

	if(fg_status)
		fg_results = fg_pro.get_output();

	fg_pro.clear_input();
	fg_pro.clear_output();

	//convert edge image back to edge map
	std::cout<<"convert edge image to edge map"<<std::endl;
	std::vector<bpro1_storage_sptr> edge_img2map_results;
	brld_edge_image_to_oriented_edgemap_process edge_img2map_pro;

	edge_img2map_pro.clear_input();
	edge_img2map_pro.clear_output();
	edge_img2map_pro.add_input(fg_results[0]);

	//std::cout<<"6"<<std::endl;
	bool edge_img2map_status = edge_img2map_pro.execute();
	edge_img2map_pro.finish();

	// if process did not fail
	if ( edge_img2map_status )
	{
	    edge_img2map_results = edge_img2map_pro.get_output();
	}

	//Clean up after ourselves
	edge_img2map_pro.clear_input();
	edge_img2map_pro.clear_output();

	std::cout<<"************ Save FG Edges    ************"<<std::endl;

	std::string out_name = filenames[20].substr(filenames[20].size()-8, 8);
	std::cout << out_name << std::endl;
    dbdet_save_edg_process save_edg_pro;
	//std::string output_file = "/home/guoy/lemsvxl/bin_release/brcv/rec/dborl/algo/vox_multi_frame_fg_edge_refine/0021_fg.edg";
	std::string output_file = output_path + out_name;
    bpro1_filepath output(output_file,".edg");

    save_edg_pro.parameters()->set_value("-edgoutput",output);

    // Before we start the process lets clean input output
    save_edg_pro.clear_input();
    save_edg_pro.clear_output();

    save_edg_pro.add_input(edge_img2map_results[0]);
    bool status = save_edg_pro.execute();
    save_edg_pro.finish();

    //Clean up after ourselves
    save_edg_pro.clear_input();
    save_edg_pro.clear_output();


	std::cout << "******************** Shift the Center Frame *************************" << std::endl;
	for (int i = 21; i<filenames.size()-20; i++)
	{
		std::vector<bpro1_storage_sptr> edge_det_results;
		dbdet_load_edg_process load_edg_pro;

		// load in the last coming in edge map and remove the first one in the working on edge image set
        //load the input image
        std::string input_edg = filenames[i+20];

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
		//dbdet_edgemap_storage_sptr input_edgemap; // not sure if its deleted after iteration
  		//input_edgemap.vertical_cast(edge_det_results[0]);
		//dbdet_edgemap_sptr EM = input_edgemap->get_edgemap();

		// convert the edge map to edge image which is used in build BG model as well as FG detection
		std::cout<<"convert edge map to edge image"<<std::endl;
		std::vector<bpro1_storage_sptr> edge_map2img_results;
		brld_oriented_edgemap_to_edge_image_process edge_map2img_pro;

		edge_map2img_pro.clear_input();
		edge_map2img_pro.clear_output();
		edge_map2img_pro.add_input(edge_det_results[0]);

		//std::cout<<"6"<<std::endl;
		bool edge_map2img_status = edge_map2img_pro.execute();
		edge_map2img_pro.finish();

		// if process did not fail
		if ( edge_map2img_status )
		{
		    edge_map2img_results = edge_map2img_pro.get_output();
		}

		//Clean up after ourselves
		edge_map2img_pro.clear_input();
		edge_map2img_pro.clear_output();

		vidpro1_image_storage_sptr edge_image;
		edge_image.vertical_cast(edge_map2img_results[0]);

		std::cout<<"adjust the working on edge images set"<<std::endl;
		edge_images_working_on.pop_front();
		edge_images_working_on.push_back(edge_image); // where is pisical address of EM?
	

		std::cout<<"construct BG model"<<std::endl;
		// construct the bg model for the center frame
		std::vector<bpro1_storage_sptr> bg_results;
		brld_edge_point_tangent_bg_model_process_2 bg_pro;
		bg_pro.clear_input();
		bg_pro.clear_output();

		for (int j = 0 ; j< num; j++)
			bg_pro.add_input(edge_images_working_on[j]);

	
		bool bg_status = bg_pro.execute();
		bg_pro.finish();

		if(bg_status)
			bg_results = bg_pro.get_output();

		bg_pro.clear_input();
		bg_pro.clear_output();

		std::cout<<"refine FG edges"<<std::endl;
		//detect the foreground edges
		std::vector<bpro1_storage_sptr> fg_results;
		brld_edge_point_tangent_fg_detect_process fg_pro;
		fg_pro.clear_input();
		fg_pro.clear_output();

		fg_pro.add_input(edge_images_working_on[20]); // always push in the center of the working set
		fg_pro.add_input(bg_results[0]);

		//std::cout<<"7"<<std::endl;
		bool fg_status = fg_pro.execute();
		fg_pro.finish();
		//std::cout<<"8"<<std::endl;

		if(fg_status)
			fg_results = fg_pro.get_output();

		fg_pro.clear_input();
		fg_pro.clear_output();

		//convert edge image back to edge map
		std::cout<<"convert edge image to edge map"<<std::endl;
		std::vector<bpro1_storage_sptr> edge_img2map_results;
		brld_edge_image_to_oriented_edgemap_process edge_img2map_pro;

		edge_img2map_pro.clear_input();
		edge_img2map_pro.clear_output();
		edge_img2map_pro.add_input(fg_results[0]);

		//std::cout<<"6"<<std::endl;
		bool edge_img2map_status = edge_img2map_pro.execute();
		edge_img2map_pro.finish();

		// if process did not fail
		if ( edge_img2map_status )
		{
			edge_img2map_results = edge_img2map_pro.get_output();
		}

		//Clean up after ourselves
		edge_img2map_pro.clear_input();
		edge_img2map_pro.clear_output();

		std::cout<<"************ Save FG Edges    ************"<<std::endl;

		std::string out_name = filenames[i].substr(filenames[i].size()-8, 8);
		std::cout << out_name << std::endl;
		dbdet_save_edg_process save_edg_pro;
		//std::string output_file = "/home/guoy/lemsvxl/bin_release/brcv/rec/dborl/algo/vox_multi_frame_fg_edge_refine/0021_fg.edg";
		std::string output_file = output_path + out_name;
		bpro1_filepath output(output_file,".edg");

		save_edg_pro.parameters()->set_value("-edgoutput",output);

		// Before we start the process lets clean input output
		save_edg_pro.clear_input();
		save_edg_pro.clear_output();

		save_edg_pro.add_input(edge_img2map_results[0]);
		bool sv_status = save_edg_pro.execute();
		save_edg_pro.finish();

		//Clean up after ourselves
		save_edg_pro.clear_input();
		save_edg_pro.clear_output();
	}
	

	return 0;
}
