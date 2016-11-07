//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 10/25/10
//
//

#include "vox_match_bag_of_fragments.h"
#include "vox_match_composite_fragments_params.h"
#include <dbskfg/pro/dbskfg_match_bag_of_fragments_process.h>
#include <vul/vul_file.h>
#include <dborl/algo/dborl_utilities.h>

//: Default ctor
vox_match_bag_of_fragments::vox_match_bag_of_fragments
(vox_match_composite_fragments_params_sptr params):
    params_(params),
    model_file_(),
    query_file_(),
    output_match_folder_(),
    output_match_prefix_()
{

    // Nothing else to do her
}

//: Default dtor
vox_match_bag_of_fragments::~vox_match_bag_of_fragments()
{
    //Does nothing
}

//: this method actually performs the algorithm
bool vox_match_bag_of_fragments::process()
{
    
    bool status(false);
    
    // Kick of steps of processing

    // All paths to query and model cgraph storage is formed
    status = initialize();
    if ( status )
    {
        // Move on to matching
        status = perform_matching();
    }
     
    return status;
}

//: this method actually initializes arguments for other helper methods
bool vox_match_bag_of_fragments::initialize()
{

    // Lets grab the parameters
    model_file_ =  params_->model_object_dir_()+"/"+
        params_->model_object_name_()+
        params_->input_composite_fragments_extension_()+"/";

    query_file_ =  params_->query_object_dir_()+"/"+
        params_->query_object_name_()+
        params_->input_composite_fragments_extension_()+"/";
    
    // Lets create directory of where output cgraph matching should go
    if (params_->save_to_object_folder_())
    { 
        output_match_folder_ = params_->output_cgraph_match_folder_();
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
        vcl_cerr << "Cannot find model cgraph storage (.txt) file: " << 
            model_file_ << vcl_endl;
    }

    bool query_status = vul_file::exists(query_file_);
    if ( !query_status ) 
    {
        vcl_cerr << "Cannot find query cgraph storage (.txt) file: " << 
            query_file_ << vcl_endl;
    }

    return query_status & model_status;
}

//: This function is a wrapper around the dbskr/pro augmented
//  with one to many matching
bool vox_match_bag_of_fragments::perform_matching()
{

    //******************** Match Cgraph Fragmets ****************************
    vcl_cout<<"***********  Match Perceptual Fragments  ************"<<vcl_endl;

    // Create file path objects
    bpro1_filepath model(model_file_,".txt");
    bpro1_filepath query(query_file_,".txt");
    bpro1_filepath output_folder(output_match_folder_,"");

    dbskfg_match_bag_of_fragments_process matchsg_pro;
    set_process_parameters_of_bpro1(*params_, 
                                    matchsg_pro, 
                                    "edit_data");  
   
    // Set process parameters
    matchsg_pro.parameters()->set_value("-model_folder", model);
    matchsg_pro.parameters()->set_value("-query_folder", query );
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
        vcl_cerr << "Matching of Composite Fragments Failed"
                 << vcl_endl;
        return false;
    }
  

    return matchsg_status;
}

