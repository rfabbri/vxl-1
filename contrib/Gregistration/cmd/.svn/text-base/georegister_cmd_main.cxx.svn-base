//:
// \file
// \author Anil Usumezbas (anil_usumezbas@brown.edu)
// \date Sep 14, 2011
// \brief An algorithm to extract foregrounds from video
//        This is a wrapper around:
//        brcv/seg/dbdet/dbdet_third_order_color_edge_detector_process
//        contrib/Gregistration/pro/dbrl_oriented_edgemap_to_edge_image_process
//        contrib/Gregistration/pro/dbrl_edge_point_tangent_bg_model_process
//        contrib/Gregistration/pro/dbrl_edge_point_tangent_fg_detect_process
//        contrib/Gregistration/pro/dbrl_edge_image_to_oriented_edgemap_process
//
// \verbatim
//    Modifications
//
// \endverbatim

#include "georegister_cmd_params.h"
#include "georegister_cmd_params_sptr.h"
#include <vul/vul_timer.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_file.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_load.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/process/vidpro1_save_image_process.h>
#include <vcl_fstream.h>
#include <dbdet/pro/dbdet_third_order_color_edge_detector_process.h>
#include <pro/dbrl_oriented_edgemap_to_edge_image_process.h>
#include <pro/dbrl_edge_image_to_oriented_edgemap_process.h>
#include <pro/dbrl_edge_point_tangent_bg_model_process.h>
#include <pro/dbrl_edge_point_tangent_fg_detect_process.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <dbdet/pro/dbdet_save_edg_process.h>
#include <vil/vil_new.h>

int main(int argc, char *argv[])
{

    //Start timer
    vul_timer t;

    //Construct parameters with the default values
    georegister_cmd_params_sptr params = new georegister_cmd_params("georegister_cmd");

    //Parse the command line arguments
    if(!params->parse_command_line_args(argc, argv))
	return 1;

    //Print the parameters file
    if(!params->print_params_xml(params->print_params_file()))
	vcl_cerr << "ERROR: Problems in writing params file to: " << params->print_params_file() << vcl_endl;

    //Exit if there's nothing else to do
    if(params->exit_with_no_processing() || params->print_params_only())
	return 0;

    //Parse the input parameter file whose name is extracted from the command line
    if(!params->parse_input_xml())
	return 1;

    //Locate the text file that contains the filenames of every image, one filename per line
    vcl_string list_file_location = params->input_object_dir_() + "/input_sequence.txt";

    vcl_ifstream list_file(list_file_location.c_str());
    
    if(!list_file.is_open())
    {
	vcl_cerr << "ERROR: Cannot open \"input_sequence.txt\"!" << vcl_endl;
	return 1;
    }

    vcl_vector<vcl_string> filenames;

    //Parse the input sequence text file for filenames to be processed
    while(list_file)
    {
	vcl_string current_filename;
	list_file >> current_filename;
        if(!current_filename.empty())
            filenames.push_back(current_filename);
    }

    unsigned num_frames = filenames.size();

    vcl_vector<bpro1_storage_sptr> edge_images;

    vcl_cout << "---------------------------" << vcl_endl;
    vcl_cout << "COMPUTING EDGE IMAGES" << vcl_endl;
    vcl_cout << "---------------------------" << vcl_endl;

    //Loop over all images and compute edge images for each of them
    for(unsigned i=0; i<num_frames; ++i)
    {
        vcl_cout << "Frame No: " << i << vcl_endl;

	//Load the input image
	vcl_string input_img_dir = params->input_object_dir_() + "/" + filenames[i];

	if(!vul_file::exists(input_img_dir))
	{
	    vcl_cerr << "ERROR: Cannot find image: " << input_img_dir << vcl_endl;
	    return 1;
	}

	//Grab the input image
	vil_image_resource_sptr input_img_rsc = vil_load_image_resource(input_img_dir.c_str());
    
	if(!input_img_rsc)
	{
	    vcl_cerr << "ERROR: Cannot load image: " << input_img_dir << vcl_endl;
	    return 1;
	}

	//Create vidpro storage for the input image
	vidpro1_image_storage_sptr input_img = new vidpro1_image_storage();
	input_img->set_image(input_img_rsc);

	//Compute third order color edges on the input image

        dbdet_third_order_color_edge_detector_process edge_det_pro;
        set_process_parameters_of_bpro1(*params, edge_det_pro, params->tag_third_order_color_);

        edge_det_pro.clear_input();
        edge_det_pro.clear_output();

        edge_det_pro.add_input(input_img);
        bool edge_det_status = edge_det_pro.execute();
        edge_det_pro.finish();

        vcl_vector<bpro1_storage_sptr> edge_det_results;

        if(edge_det_status)
            edge_det_results = edge_det_pro.get_output();

        edge_det_pro.clear_input();
        edge_det_pro.clear_output();

        if(edge_det_results.size() != 1)
        {
            vcl_cerr << "ERROR: Third order color edge detection failed!" << vcl_endl;
            return 1;
        }

        //Convert the edge maps into edge images
        
        dbrl_oriented_edgemap_to_edge_image_process edge_image_pro;
        
        edge_image_pro.clear_input();
        edge_image_pro.clear_output();

        edge_image_pro.add_input(edge_det_results[0]);
        bool edge_image_status = edge_image_pro.execute();
        edge_image_pro.finish();

        vcl_vector<bpro1_storage_sptr> edge_image_results;
        
        if(edge_image_status)
            edge_image_results = edge_image_pro.get_output();

        edge_image_pro.clear_input();
        edge_image_pro.clear_output();

        if(edge_image_results.size() != 1)
        {
            vcl_cerr << "ERROR: Edgemap to edge image conversion failed!" << vcl_endl;
            return 1;
        }

        //vidpro1_image_storage_sptr current_edge_image_results;
        //current_edge_image_results.vertical_cast(edge_image_results[0]);

        edge_images.push_back(edge_image_results[0]);        
    }
 
    //Number of frames for which foreground will be computed
    //i.e. frames that are not in the padding area
    unsigned num_fg_frames = num_frames - 40;

    vcl_cout << "---------------------------" << vcl_endl;
    vcl_cout << "EXTRACTING FOREGROUND" << vcl_endl;
    vcl_cout << "---------------------------" << vcl_endl;

    for(unsigned i=0; i<num_fg_frames; ++i)
    {
        vcl_cout << "Frame No: " << i+20 << vcl_endl;
        //Sequence of frame numbers -relative to the current fg frame- that will be used as training
        int seq[8] = {-10, 10, -20, 20, -5, 5, -17, 17};


        //Model the background

        dbrl_edge_point_tangent_bg_model_process bg_model_pro;
        set_process_parameters_of_bpro1(*params, bg_model_pro, params->tag_bg_modeling_);

        bg_model_pro.clear_input();
        bg_model_pro.clear_output();

        bool bg_model_status;

        dbbgm_distribution_image_storage_sptr background;

        for(unsigned j=0; j<8; ++j)
        {
            vidpro1_image_storage_sptr current_edg_image;
            current_edg_image.vertical_cast(edge_images[i+20+seq[j]]);

            bg_model_pro.add_input(current_edg_image);
            bg_model_pro.add_input(background);
            bg_model_status = bg_model_pro.execute();
            bg_model_pro.finish();
        }

        vcl_vector<bpro1_storage_sptr> bg_model_results;

        if(bg_model_status)
            bg_model_results = bg_model_pro.get_output();

        bg_model_pro.clear_input();
        bg_model_pro.clear_output();

        if(bg_model_results.size() != 1)
        {
            vcl_cerr << "ERROR: Background modeling failed!" << vcl_endl;
            return 1;
        }

        //Extract the foreground

        dbrl_edge_point_tangent_fg_detect_process fg_detect_pro;
        set_process_parameters_of_bpro1(*params, fg_detect_pro, params->tag_fg_extraction_);

        fg_detect_pro.clear_input();
        fg_detect_pro.clear_output();

        //We need to make a copy of the edge image because this process modifies the input edge image

        vidpro1_image_storage_sptr edge_image_conv;
        edge_image_conv.vertical_cast(edge_images[i+20]);

        vil_image_resource_sptr input_edg_rsc = edge_image_conv->get_image();
        vil_image_view<float> input_edg_view = input_edg_rsc->get_view(0,input_edg_rsc->ni(),0,input_edg_rsc->nj());

        vil_image_view<float> input_edg_view_copy;
        input_edg_view_copy.deep_copy(input_edg_view);

        vil_image_resource_sptr input_edg_rsc_copy = vil_new_image_resource_of_view(input_edg_view_copy);

        vidpro1_image_storage_sptr input_edg_image = new vidpro1_image_storage();
        input_edg_image->set_image(input_edg_rsc_copy);

        //vcl_cout << input_edg_image << vcl_endl;
        //vcl_cout << edge_images[i+20] << vcl_endl;
        //bpro1_storage input_edg_data = *(edge_images[i+20]);
        //vidpro1_image_storage v = static_cast<vidpro1_image_storage>(input_edg_data);

        //vidpro1_image_storage_sptr input_edg_image = new vidpro1_image_storage(v);
        //input_edg_image.vertical_cast(input_edg_image_ptr);

        dbbgm_distribution_image_storage_sptr input_bg;
        input_bg.vertical_cast(bg_model_results[0]);

        fg_detect_pro.add_input(input_edg_image);
        fg_detect_pro.add_input(input_bg);
        bool fg_detect_status = fg_detect_pro.execute();
        fg_detect_pro.finish();

        vcl_vector<bpro1_storage_sptr> fg_detect_results;

        if(fg_detect_status)
            fg_detect_results = fg_detect_pro.get_output();

        fg_detect_pro.clear_input();
        fg_detect_pro.clear_output();

        if(fg_detect_results.size() != 2)
        {
            vcl_cerr << "ERROR: Foreground extraction failed!" << vcl_endl;
            return 1;
        }

        //Convert the edge image into oriented edge map

        dbrl_edge_image_to_oriented_edgemap_process edge_map_pro;
        
        edge_map_pro.clear_input();
        edge_map_pro.clear_output();

        edge_map_pro.add_input(fg_detect_results[0]);
        bool edge_map_status = edge_map_pro.execute();
        edge_map_pro.finish();

        vcl_vector<bpro1_storage_sptr> edge_map_results;
        
        if(edge_map_status)
            edge_map_results = edge_map_pro.get_output();

        edge_map_pro.clear_input();
        edge_map_pro.clear_output();

        if(edge_map_results.size() != 1)
        {
            vcl_cerr << "ERROR: Edge image to edge map conversion failed!" << vcl_endl;
            return 1;
        }


        if(vul_file::make_directory_path("./output"))
        {
            
            //Save the foregrounds in the form of edge images
            vidpro1_save_image_process save_image_pro;
            bpro1_parameters_sptr save_image_params = new bpro1_parameters();
            save_image_params->add("Filename", "-filename", bpro1_filepath("./output/" + filenames[i+20],"*"));
            save_image_pro.set_parameters(save_image_params);
        
            save_image_pro.clear_input();
            save_image_pro.clear_output();

            //save_image_pro.add_input(fg_detect_results[0]);
            save_image_pro.add_input(fg_detect_results[0]);
            bool save_image_status = save_image_pro.execute();
            save_image_pro.finish();

            save_image_pro.clear_input();
            save_image_pro.clear_output();

            vcl_string current_basename = vul_file::strip_extension(filenames[i+20]);

            //Save the foreground in the form of edge maps
            vcl_cout << "I am saving edge maps" << vcl_endl;
            dbdet_save_edg_process save_edg_pro;
            bpro1_parameters_sptr save_edg_params = new bpro1_parameters();
            save_edg_params->add("Output file <filename...>" , "-edgoutput" , bpro1_filepath("./output/" + current_basename + ".edg","*.edg"));
            save_edg_pro.set_parameters(save_edg_params);

            save_edg_pro.clear_input();
            save_edg_pro.clear_output();

            save_edg_pro.add_input(edge_map_results[0]);
            bool save_edg_status = save_edg_pro.execute();
            save_edg_pro.finish();

            save_edg_pro.clear_input();
            save_edg_pro.clear_output();        

        }
        

    }
            
    return 0;
}
