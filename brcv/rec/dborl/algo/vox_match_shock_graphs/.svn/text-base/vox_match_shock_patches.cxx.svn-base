//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/11/09
//
//

#include "vox_match_shock_patches.h"
#include "vox_match_shock_graphs_params.h"
#include <dbskr/pro/dbskr_shock_patch_match_process.h>
#include <dbskr/pro/dbskr_shock_match_process.h>
#include <vul/vul_file.h>
#include <dborl/algo/dborl_utilities.h>

//: Default ctor
vox_match_shock_patches::vox_match_shock_patches
(vox_match_shock_graphs_params_sptr params):
    params_(params),
    model_file_(),
    query_file_(),
    output_match_folder_(),
    output_match_prefix_()
{

    // Nothing else to do her
}

//: Default dtor
vox_match_shock_patches::~vox_match_shock_patches()
{
    //Does nothing
}

//: this method actually performs the algorithm
bool vox_match_shock_patches::process()
{
    
    bool status(false);
    
    // Kick of steps of processing

    // All paths to query and model shock patch storage is formed
    status = initialize();
    if ( status )
    {
        // Move on to matching
        status = perform_matching();
    }
     
    return status;
}

//: this method actually initializes arguments for other helper methods
bool vox_match_shock_patches::initialize()
{

    // Lets grab the parameters
    model_file_ = params_->model_object_dir_()+"/"+
        params_->model_object_name_()+
        "-patch_strg.bin";

    query_file_ =  params_->query_object_dir_()+"/"+
        params_->query_object_name_()+
        params_->input_shock_patch_extension_()+"/"+
        params_->query_object_name_()+
        "-patch_strg.bin";
    
    // Lets create directory of where output shock patches will go
    if (params_->save_to_object_folder_())
    { 
        output_match_folder_ = params_->output_shock_patch_match_folder_();
    }
    else 
    {
        output_match_folder_ = params_->query_object_dir_();
    }

        
    if (!vul_file::exists(output_match_folder_)) 
    {
        vul_file::make_directory(output_match_folder_);
        
    }

    output_match_prefix_ = params_->model_object_name_() + "_"+
        params_->query_object_name_() + "_match";

    // Do some error checking
    bool model_status = vul_file::exists(model_file_);
    if (!model_status ) 
    {
        vcl_cerr << "Cannot find model shock storage (.bin) file: " << 
            model_file_ << vcl_endl;
    }

    bool query_status = vul_file::exists(query_file_);
    if ( !query_status ) 
    {
        vcl_cerr << "Cannot find query shock storage (.bin) file: " << 
            query_file_ << vcl_endl;
    }

    return query_status & model_status;
}

//: This function is a wrapper around the dbskr/pro augmented
//  with one to many matching
bool vox_match_shock_patches::perform_matching()
{

    //******************** Match Shock Graphs ****************************
    vcl_cout<<"************  Match Shock Patches  *************"<<vcl_endl;

    // Create file path objects
    bpro1_filepath shock_patch_model(model_file_,".bin");
    bpro1_filepath query_patch_model(query_file_,".bin");
    bpro1_filepath output_folder(output_match_folder_,"");

    dbskr_shock_patch_match_process matchsg_pro;
    set_process_parameters_of_bpro1(*params_, 
                                    matchsg_pro, 
                                    "edit_data");  
   
    // Set process parameters
    matchsg_pro.parameters()->set_value("-model_filepath", shock_patch_model);
    matchsg_pro.parameters()->set_value("-query_filepath", query_patch_model );
    matchsg_pro.parameters()->set_value("-output_folder" , output_folder );
    matchsg_pro.parameters()->set_value("-output_prefix" , 
                                        output_match_prefix_ );
    
    // Before we start the process lets clean input output
    matchsg_pro.clear_input();
    matchsg_pro.clear_output();

    // No input needed just call process
    bool matchsg_status = matchsg_pro.execute();
    matchsg_pro.finish();
       
    //Clean up after ourselves
    matchsg_pro.clear_input();
    matchsg_pro.clear_output();

    if (!matchsg_status) 
    {
        vcl_cerr << "Extracting of shock patches failed !"
                 << vcl_endl;
        return false;
    }
  

    return matchsg_status;
}

