//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 06/28/09
//
//

#include "vox_match_shock_graphs_one_to_many.h"
#include "vox_match_shock_graphs_params.h"
#include <dborl/algo/dborl_index_parser.h>
#include <dborl/dborl_index.h>
#include <dborl/dborl_index_node.h>
#include <dbskr/pro/dbskr_shock_match_process.h>
#include <dborl/algo/dborl_utilities.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

//: Default ctor
vox_match_shock_graphs_one_to_many::vox_match_shock_graphs_one_to_many
(vox_match_shock_graphs_params_sptr params):
    params_(params),
    ind_(),
    output_file_(),
    query_shock_()
{

    // Nothing else to do her
}

//: Default dtor
vox_match_shock_graphs_one_to_many::~vox_match_shock_graphs_one_to_many()
{
    // Clear out costs
    final_costs_.clear();

}

//: this method actually performs the algorithm
bool vox_match_shock_graphs_one_to_many::process()
{
    
    bool status(false);
    
    // Kick of three steps of processing

    // Reading of index file is also performed in initialize process
    status = initialize();
    if ( status )
    {
        // Move on to matching
        status = perform_matching();
        if ( status )
        {
            // Finally write out results
            status = write_out();
        }
    }
     
    return status;
}

//: this method actually initializes arguments for other helper methods
bool vox_match_shock_graphs_one_to_many::initialize()
{
    // Keep status flag
    bool status(true);

    // Parse index file
    //creating a parser for the index file
    dborl_index_parser parser;
    parser.clear();
    ind_ = dborl_index_parser::parse(params_->index_filename_(),parser);

    if ( !ind_ )
    {
        vcl_cerr<<"Paring index file failed!!"<<vcl_endl;
        status = false;
    }
    vcl_cout<<vcl_endl;

    // Determine where in the input shock is coming from
    // either input object dir, or the associated file path
    // Use associated file
    if ( vul_file::exists(params_->query_assoc_dir_()))
    {
        // associated filename
        vcl_string assoc_filename;

        // Iterate over all files in directory
        vul_file_iterator fn(params_->query_assoc_dir_()+"/*");
        for ( ; fn ; ++fn) 
        {
            //To deal with hidden files need to check if directories
            if (!vul_file::is_directory(fn.filename()))
            {
                assoc_filename=fn.filename();
            }
        }

        query_shock_ = params_->query_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        query_shock_ = params_->query_object_dir_() + "/" +
            params_->query_object_name_() + ".esf";

    }
    
    // Determine where to write match files to
    // either output folder or input object dir
    if (params_->save_to_object_folder_())
    { 
        output_file_ = params_->output_shock_match_folder_() + "/";
    }
    else 
    {
        output_file_ = params_->query_object_dir_() + "/";
    }
        
    if (!vul_file::exists(output_file_)) 
    {
        vul_file::make_directory(output_file_);
        
    }
    output_file_ = output_file_ + params_->query_object_name_()+
        params_->output_match_extension_();
       
    // Do some error checking
    status = vul_file::exists(query_shock_);
    if (! status ) 
    {
        vcl_cerr << "Cannot find shock (.esf) file: " << 
            query_shock_ << vcl_endl;
    }

    return status;
}

//: This function is a wrapper around the dbskr/pro augmented
//  with one to many matching
bool vox_match_shock_graphs_one_to_many::perform_matching()
{
    // Loop over index file and compare query shock against all 
    // shapes within index file
    dborl_index_node_sptr root    = ind_->root_->cast_to_index_node();
    unsigned int number_of_shapes = root->paths().size();

    // Indicate the matching state
    vcl_cout<<"Matching "<< vul_file::strip_directory(query_shock_) <<" to "<< 
        number_of_shapes << " shapes. "<<vcl_endl;
    vcl_cout<<vcl_endl;

    // Resize costs to the number of shapes
    final_costs_.resize(number_of_shapes);

    // Set up process
    dbskr_shock_match_process matchsg_pro;
    set_process_parameters_of_bpro1(*params_, 
                                    matchsg_pro, 
                                    "edit_data");  
        
    // Set process parameters
    matchsg_pro.parameters()->set_value("-load1", true);
    matchsg_pro.parameters()->set_value("-load2", true );   
    matchsg_pro.parameters()->set_value("-prune_tree",params_->prune_tree_());
    matchsg_pro.parameters()->set_value("-prune_thres",params_->prune_thres_());
    matchsg_pro.parameters()->set_value("-write_out",false);
    matchsg_pro.parameters()->set_value("-mirror_match",params_->perform_mirror_matching_());

    // Set first shock graph to 
    bpro1_filepath query_filepath(query_shock_,".esf");
    matchsg_pro.parameters()->set_value("-esf1" , query_filepath );

    // Create storage for output of one to many matching 
    bool matchsg_status(false);
    dbskr_shock_match_storage_sptr output_match;
    vcl_vector<bpro1_storage_sptr> shock_matching_results;
    vcl_string model_shock;

    // Determine type of normalization
    bool write_match_norm = params_->write_matching_norm_();

    // Loop over children of index file
    unsigned int i(0);
    for ( ; i < number_of_shapes ; ++i)
    {
        // load the model shock
        model_shock = root->paths()[i] + "/" + root->names()[i] + ".esf";

        if (!vul_file::exists(model_shock)) 
        {
            vcl_cerr << "Cannot find shock (.esf) files: " << 
                model_shock << vcl_endl;
            return false;
        }

        // Indicate what we are matching
        vcl_cout<<"************ "
                <<"Matching "<< vul_file::strip_directory(query_shock_)
                <<" to "<< vul_file::strip_directory(model_shock) 
                <<" ************"<<vcl_endl;
       

        // Set input shock graphs to compare
        bpro1_filepath model_filepath(model_shock,".esf");
        matchsg_pro.parameters()->set_value("-esf2" , model_filepath );

        // Lets clean out input and outputs before we start
        matchsg_pro.clear_input();
        matchsg_pro.clear_output();

        // Kick of process
        matchsg_status = matchsg_pro.execute();
        matchsg_pro.finish();

        if (matchsg_status)
        {
            shock_matching_results = matchsg_pro.get_output();
        }

        //clean up after ourselves
        matchsg_pro.clear_input();
        matchsg_pro.clear_output();

        if ( shock_matching_results.size() != 1 )
        {
            vcl_cerr << "Shock Graph matching failed! "<<vcl_endl;
            return false;
        }

        // Grab the underlying shock matching structure
        output_match = dbskr_shock_match_storage_new();
        output_match.vertical_cast(shock_matching_results[0]);

        // Fill up the final cost vector with either unormalized or not
        // depending on the cost
        if ( write_match_norm )
        {
            final_costs_[i] = output_match->get_sm_cor()->final_norm_cost();
        }
        else
        {
            final_costs_[i] = output_match->get_sm_cor()->final_cost();
        }
                
        // Update status results
        params_->percent_completed = ((float)(i+1)/number_of_shapes)*100.0f;
        params_->print_status_xml();

        vcl_cout<<vcl_endl;
    }

    return true;
}

//: This method writes out the msghm files
bool vox_match_shock_graphs_one_to_many::write_out()
{
    // Basically loop over all shapes and write out the cost
    dborl_index_node_sptr root = ind_->root_->cast_to_index_node();
    unsigned int number_of_shapes = root->paths().size();

    // Input extension 
    unsigned int i(0);

    // Open up file
    vcl_ofstream tf(output_file_.c_str());

    if (!tf) {
        vcl_cerr << "Unable to open mshgm file " << 
            output_file_ << " for write " << vcl_endl;
        return false;
    }
    else
    {
        for( ; i<number_of_shapes; i++)
        {
            tf << root->names()[i] << vcl_endl;
            tf << final_costs_[i] << vcl_endl;
        }
        tf.close();
        
        return true;
    }
    
}
