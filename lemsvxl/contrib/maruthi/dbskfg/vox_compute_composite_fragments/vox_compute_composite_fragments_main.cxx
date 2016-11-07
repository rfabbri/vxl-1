//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date October 25th, 2010
// \brief An algorithm to extract composite graph fragments
//  from 
//  dbskfg_extract_fragments_process.cxx 
//
// \verbatim
// \endverbatim

#include <vil/vil_load.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <vidpro1/process/vidpro1_load_con_process.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include "vox_compute_composite_fragments_params.h"
#include "vox_compute_composite_fragments_params_sptr.h"
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <con_graph/pro/dbskfg_form_containment_graph_region_process.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbsk2d/pro/dbsk2d_load_bnd_process.h>

int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_compute_composite_fragments_params_sptr params = 
        new vox_compute_composite_fragments_params(
            "vox_compute_composite_fragments");  
  
    // parse the command line arguments
    if (!params->parse_command_line_args(argc, argv))
        return 1;

    //: always print the params file if an executable to work with ORL web 
    // interface
    if (!params->print_params_xml(params->print_params_file()))
    {
        vcl_cerr << "problems in writing params file to: " 
                 << params->print_params_file() << vcl_endl;
    }

    // exit if there is nothing else to do
    if (params->exit_with_no_processing() || params->print_params_only())
    {
        return 0;
    }

    //: always call this method to actually parse the input parameter file 
    // whose name is extracted from the command line
    if (!params->parse_input_xml())
    {
        return 1;
    }

    //Determine which input object we are going to use
    //Either from the input_object_dir or the associated file
    //The associated file always takes precendence
    vcl_string input_vsol_fn;

    // Use associated file
    if ( vul_file::exists(params->input_assoc_dir_()))
    {
        // associated filename
        vcl_string assoc_filename;

        // Iterate over all files in directory
        vul_file_iterator fn(params->input_assoc_dir_()+"/*");
        for ( ; fn ; ++fn) 
        {
            //To deal with hidden files need to check if directories
            if (!vul_file::is_directory(fn.filename()))
            {
                assoc_filename=fn.filename();
            }
        }

        input_vsol_fn = params->input_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        input_vsol_fn = params->input_object_dir_() + "/" +
            params->input_object_name_() + params->input_contour_extension_();

    }

    if (!vul_file::exists(input_vsol_fn)) 
    {
        vcl_cerr << "Cannot find contour file: " << input_vsol_fn << vcl_endl;
        return 1;
    }

    vcl_string input_contour_extension = vul_file::extension(input_vsol_fn);

    // Create output storage
    vcl_vector<bpro1_storage_sptr> vsol_contour;

    //Call appropriate process to load file
    if ( input_contour_extension == ".cem" ||
         input_contour_extension == ".cemv" )
    {
        
        // Call vidpro load cem process
        vidpro1_load_cem_process load_pro_cem;

        bpro1_filepath input(input_vsol_fn,input_contour_extension);

        load_pro_cem.parameters()->set_value("-ceminput",input);

        // Before we start the process lets clean input output
        load_pro_cem.clear_input();
        load_pro_cem.clear_output();

        // Pass in input vsol string
        bool load_cem_status = load_pro_cem.execute();
        load_pro_cem.finish();

        // Grab output from symbolic edge linking
        if ( load_cem_status )
        {
            vsol_contour = load_pro_cem.get_output();
        }

        //Clean up after ourselves
        load_pro_cem.clear_input();
        load_pro_cem.clear_output();

    }
    else if ( input_contour_extension == ".con")
    {

        // Call vidpro load con process
        vidpro1_load_con_process load_pro_con;

        bpro1_filepath input(input_vsol_fn,input_contour_extension);

        load_pro_con.parameters()->set_value("-coninput",input);

        // Before we start the process lets clean input output
        load_pro_con.clear_input();
        load_pro_con.clear_output();

        // Pass in input vsol string
        bool load_con_status = load_pro_con.execute();
        load_pro_con.finish();

        // Grab output from symbolic edge linking
        if ( load_con_status )
        {
            vsol_contour = load_pro_con.get_output();
        }

        //Clean up after ourselves
        load_pro_con.clear_input();
        load_pro_con.clear_output();

    }
    else if ( input_contour_extension == ".bnd")
    {
        // Call dbsk2d shock load bnd process
        dbsk2d_load_bnd_process load_pro_bnd;

        bpro1_filepath input(input_vsol_fn,input_contour_extension);

        load_pro_bnd.parameters()->set_value("-bndinput",input);

        // Before we start the process lets clean input output
        load_pro_bnd.clear_input();
        load_pro_bnd.clear_output();

        // Pass in input vsol string
        bool load_bnd_status = load_pro_bnd.execute();
        load_pro_bnd.finish();

        // Grab output from symbolic edge linking
        if ( load_bnd_status )
        {
            vsol_contour = load_pro_bnd.get_output();
        }

        //Clean up after ourselves
        load_pro_bnd.clear_input();
        load_pro_bnd.clear_output();
    }
    else
    {

        vcl_cerr << "Unknown input type: " << 
            input_contour_extension << " Quit now" <<vcl_endl;
        return 1;

    }


    if ( vsol_contour.size() != 1)
    {
        vcl_cerr<<"Problem loading file, "<<input_vsol_fn<<
            "Could not get vsol data structure"<<vcl_endl;
        
        return 1;
    }


    // Grab the underlying contours
    vidpro1_vsol2D_storage_sptr vsol_contour_storage = 
        vidpro1_vsol2D_storage_new();
    vsol_contour_storage.vertical_cast(vsol_contour[0]);

    //Make sure the input image exists
    vcl_string input_image_fn = params->input_object_dir_() + "/" 
        + params->input_object_name_() + params->input_image_extension_();
    
    if (!vul_file::exists(input_image_fn)) 
    {
        vcl_cerr << "Cannot find image file: " << input_image_fn << vcl_endl;
        return 1;
    }

    // Grab image
    vil_image_resource_sptr img_sptr = 
        vil_load_image_resource(input_image_fn.c_str());
    if (!img_sptr) 
    {
        vcl_cerr << "Cannot load image: " << input_image_fn << vcl_endl;
        return 1;
    }

    // Create vid pro storage
    vidpro1_image_storage_sptr inp = new vidpro1_image_storage();
    inp->set_image(img_sptr);

    // Lets create directory of where output composite graph fragments will go
    vcl_string output_file;
    if (params->save_to_object_folder_())
    { 
        output_file = params->output_cgraph_fragments_folder_();
 
    }
    else 
    {
        output_file = params->input_object_dir_() + 
            "/" + params->input_object_name_() + "_cgraph_fragments";
    }
        
    if (!vul_file::exists(output_file)) 
    {
        vul_file::make_directory(output_file);
        
    }
    else
    {
        //Delete curent directory and start over
        vcl_string file_glob = "-r " + output_file;
        vul_file::delete_file_glob(file_glob);
        
        // Now remake directory
        vul_file::make_directory(output_file);

    }

    //******************** Load in Training data  ******************************
    vcl_cout<<"************  Loading Training Data  *************"<<vcl_endl;

    vcl_string training_directory = params->training_path_();

    dbskfg_transform_manager::Instance().
        read_in_training_data(training_directory);

    double beta0 = params->logistic_beta0_();
    double beta1 = params->logistic_beta1_();

    dbskfg_transform_manager::Instance().set_beta0_logit(beta0);
    dbskfg_transform_manager::Instance().set_beta1_logit(beta1);

  
    //******************** Extract Cgraph Fragments ****************************
    vcl_cout<<"************  Extract Cgraph Fragments  *************"<<vcl_endl;
    
    dbskfg_form_containment_graph_region_process cg_pro;
    set_process_parameters_of_bpro1(*params, 
                                    cg_pro, 
                                    params->tag_compute_composite_fragments_);  

    // Clear input output
    cg_pro.clear_input();
    cg_pro.clear_output();

    // Add image and vsol storage
    cg_pro.add_input(vsol_contour_storage);
    cg_pro.add_input(inp);
   

    // Lets set the parameters
    bpro1_filepath output_folder(output_file,"");

    // Set input image 
    cg_pro.parameters()->set_value("-output_folder" , output_folder );
    cg_pro.parameters()->set_value("-output_prefix" ,
                                   params->input_object_name_() );

 
    // No input needed just call process
    bool cg_status = cg_pro.execute();
    cg_pro.finish();
       
    //Clean up after ourselves
    cg_pro.clear_input();
    cg_pro.clear_output();

    if (!cg_status) 
    {
        vcl_cerr << "Extracting of composite graph fragments failed !"
                 << vcl_endl;
        return 1;
    }

    

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}

