//:
// \file
// \author Maruthi Narayanan ( mn@lems.brown.edu )
// \date June 17, 2009
// \brief An algorithm to extract shock patches from an extrinsinc shock graph
//        This is a wrapper around 
//  brcv/rec/dbskr/pro/dbskr_extract_subgraph_and_find_shock_patches_process
//
// \verbatim
// \endverbatim


#include "vox_extract_shock_patches_params.h"
#include "vox_extract_shock_patches_params_sptr.h"
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>
#include <dbskr/pro/dbskr_extract_subgraph_and_find_shock_patches_process.h>


int main(int argc, char *argv[]) 
{
    // Let time how long this takes
    // Start timer
    vul_timer t;

    // construct parameters with the default values;
    vox_extract_shock_patches_params_sptr params = 
        new vox_extract_shock_patches_params("dborl_extract_shock_patches");  
  
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
    vcl_string input_esf_fn;

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

        input_esf_fn = params->input_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        input_esf_fn = params->input_object_dir_() + "/" +
            params->input_object_name_() + params->input_shock_extension_();

    }
    
    if (!vul_file::exists(input_esf_fn)) 
    {
        vcl_cerr << "Cannot find esf file: " << input_esf_fn << vcl_endl;
        return 1;
    }

    //Make sure the input image exists
    vcl_string input_image_fn = params->input_object_dir_() + "/" 
        + params->input_object_name_() + params->input_image_extension_();
    
    if (!vul_file::exists(input_image_fn)) 
    {
        vcl_cerr << "Cannot find image file: " << input_image_fn << vcl_endl;
        return 1;
    }

    // Lets create directory of where output shock patches will go
    vcl_string output_file;
    if (params->save_to_object_folder_())
    { 
        output_file = params->output_shock_patch_folder_();
 
    }
    else 
    {
        output_file = params->input_object_dir_() + 
            "/" + params->input_object_name_() + "_shock_patches";
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

    //******************** Extract Shock Patches ****************************
    vcl_cout<<"************  Extract Shock Patches  *************"<<vcl_endl;
    
    dbskr_extract_subgraph_and_find_shock_patches_process esp_pro;
    set_process_parameters_of_bpro1(*params, 
                                    esp_pro, 
                                    params->tag_extract_shock_patches_);  
 
    // Lets set the parameters
    bpro1_filepath output_folder(output_file,"");
    bpro1_filepath image_filepath(input_image_fn,
                                  params->input_image_extension_());
    bpro1_filepath esf_filepath(input_esf_fn,
                                params->input_shock_extension_());

    // Set input image 
    esp_pro.parameters()->set_value("-image_filepath", image_filepath);
    esp_pro.parameters()->set_value("-esf_filepath"  , esf_filepath );
    esp_pro.parameters()->set_value("-output_folder" , output_folder );
    esp_pro.parameters()->set_value("-output_prefix" ,
                                    params->input_object_name_() );

    // Before we start the process lets clean input output
    esp_pro.clear_input();
    esp_pro.clear_output();

    // No input needed just call process
    bool esp_status = esp_pro.execute();
    esp_pro.finish();
       
    //Clean up after ourselves
    esp_pro.clear_input();
    esp_pro.clear_output();

    if (!esp_status) 
    {
        vcl_cerr << "Extracting of shock patches failed !"
                 << vcl_endl;
        return 1;
    }

    

    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;


    return 0;
}

