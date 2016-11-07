//:
// \file
// \brief 
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date 07/12/09
//
//

#include "vox_visualize_patch_matches.h"
#include "vox_visualize_patch_matches_params.h"
#include <dborl/algo/dborl_utilities.h>

#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>


//: Default ctor
vox_visualize_patch_matches::vox_visualize_patch_matches
(vox_visualize_patch_matches_params_sptr params):
    params_(params),
    match_(),
    output_html_()
{

    // Create match object with nothing for now
    match_ = new dbskr_shock_patch_match();

}

//: Default dtor
vox_visualize_patch_matches::~vox_visualize_patch_matches()
{
    //Does nothing
}

//: this method actually performs the algorithm
bool vox_visualize_patch_matches::process()
{
    
    bool status(false);
    
    // Kick of steps of processing

    // All paths to query and model shock patch storage is formed
    status = initialize();
    if ( status )
    {
        // Move on to matching
        status = visualize();
    }
     
    return status;
}

//: this method actually initializes arguments for other helper methods
bool vox_visualize_patch_matches::initialize()
{

    // Lets grab the parameters
    vcl_string model_file = params_->model_object_dir_()+"/"+
        params_->model_object_name_()+
        "-patch_strg.bin";

    vcl_string query_file =  params_->query_object_dir_()+"/"+
        params_->query_object_name_()+
        params_->input_shock_patch_extension_()+"/"+
        params_->query_object_name_()+
        "-patch_strg.bin";

    //Determine which input object we are going to use
    //Either from the input_object_dir or the associated file
    //The associated file always takes precendence
    vcl_string match_file;

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

        match_file = params_->query_assoc_dir_() + "/" + assoc_filename;

    }
    else
    {
        // Use the database entries
        match_file = params_->query_object_dir_()+"/"+
        params_->model_object_name_() + "_"+
        params_->query_object_name_() + "_match.bin";

    }
   
    // Lets create directory of where output html files will go
    if (params_->save_to_object_folder_())
    { 
        output_html_ = params_->output_visualize_folder_()+"/";
    }
    else 
    {
        output_html_ = params_->query_object_dir_()+"/";
    }

    if (!vul_file::exists(output_html_)) 
    {
        vul_file::make_directory(output_html_);
        
    }
 
    output_html_ += params_->model_object_name_() + "_"+
        params_->query_object_name_() + "_match_table.html";

    // Do some error checking
    bool model_status = vul_file::exists(model_file);
    if (!model_status ) 
    {
        vcl_cerr << "Cannot find model shock storage (.bin) file: " << 
            model_file << vcl_endl;

        return model_status;
    }

    bool query_status = vul_file::exists(query_file);
    if ( !query_status ) 
    {
        vcl_cerr << "Cannot find query shock storage (.bin) file: " << 
            query_file << vcl_endl;

        return query_status;
    }

    bool match_status = vul_file::exists(match_file);
    if ( !match_status ) 
    {
        vcl_cerr << "Cannot find match storage (.bin) file: " << 
            match_file << vcl_endl;

        return match_status;
    }

    // ------------------ Load the query patches ------------------------------
    // Read in binary files
    dbskr_shock_patch_storage_sptr query_st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifsq(query_file.c_str());
    query_st->b_read(ifsq);
    ifsq.close();

    // Throw an error for zero patches
    if (!query_st->size()) 
    {
        vcl_cerr << "zero patches in query st!!!: " << query_file << vcl_endl;
        return false;
    }

    // Finish Loading all patches
    if (!query_st->load_patch_shocks(query_file, "patch_strg.bin"))
    {
     
        vcl_cerr<<"Error loading query shock patches"<<vcl_endl;
        return false;

    }

    // ------------------ Load the model patches ------------------------------
    // Read in binary files
    dbskr_shock_patch_storage_sptr model_st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifsm(model_file.c_str());
    model_st->b_read(ifsm);
    ifsm.close();

    // Throw an error for zero patches
    if (!model_st->size()) 
    {
        vcl_cerr << "zero patches in model st!!!: " << model_file << vcl_endl;
        return false;
    }

    // Finish Loading all patches
    if (!model_st->load_patch_shocks(model_file, "patch_strg.bin"))
    {
     
        vcl_cerr<<"Error loading model shock patches"<<vcl_endl;
        return false;

    }
    
    //: create match structure by reading in existing match    
    vsl_b_ifstream ifs(match_file.c_str());
    match_->b_read(ifs);
    ifs.close();

    //: prepare id maps for this match
    vcl_map<int, dbskr_shock_patch_sptr> model_map;
    for (unsigned ii = 0; ii < model_st->size(); ii++)
    { 
        model_map[model_st->get_patch(ii)->id()] = model_st->get_patch(ii);
    }
    match_->set_id_map1(model_map);

    // Create maps of ids to patches
    vcl_map<int, dbskr_shock_patch_sptr> query_map;
    for (unsigned ii = 0; ii < query_st->size(); ii++)
    { 
        query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
    }
    match_->set_id_map2(query_map);
    
    // Prepare the output folder for processing
    vcl_string images_dir = params_->image_write_folder_()+"/";
    if (!vul_file::exists(images_dir))
    {
        vul_file::make_directory(images_dir);
    }

    vcl_string model_image = params_->model_object_dir_()+"/"+
        params_->model_object_name_()+
        params_->input_image_extension_();

    vcl_string query_image = params_->query_object_dir_()+"/"+
        params_->query_object_name_()+
        params_->input_image_extension_();

    vil_rgb<int> color(1, 0, 0);

    // Load model images
    if ( params_->write_model_images_only_())
    {

        vil_image_resource_sptr img_m = 
            vil_load_image_resource(model_image.c_str());

        model_st->create_ps_images(img_m, 
                                   images_dir + params_->model_object_name_(), 
                                   false, color);
    }
  
    // Load query images
    vil_image_resource_sptr img_q = 
        vil_load_image_resource(query_image.c_str());

    query_st->create_ps_images(img_q, 
                               images_dir + params_->query_object_name_(), 
                               false, color); 
    
    if ( params_->show_html_matches_() )
    {
        //: create the match images
        vcl_string model_shock = params_->model_object_dir_() + "/" +
            params_->model_object_name_() + ".esf";
        vcl_string query_shock = params_->query_object_dir_() + "/" +
            params_->query_object_name_() + ".esf";

        dbsk2d_xshock_graph_fileio loader;
        dbsk2d_shock_graph_sptr model_sg = 
            loader.load_xshock_graph(model_shock);
        dbsk2d_shock_graph_sptr query_sg = 
            loader.load_xshock_graph(query_shock);
    
        match_->create_match_ps_images(images_dir, 
                                       params_->model_object_name_(), 
                                       model_sg, 
                                       params_->query_object_name_(), 
                                       query_sg); 

    }

}

//: This function is a wrapper around the dbskr/pro augmented
//  with one to many matching
bool vox_visualize_patch_matches::visualize()
{

    //******************** Visualize Shock Matches ****************************
    vcl_cout<<"************  Visualize Shock Matches  *************"<<vcl_endl;


    //: create the html file with the match matrix between model and 
    //  query patches
    vcl_string table_title = params_->model_object_name_() + "_" 
                          + params_->query_object_name_() + "_match_table";
 
    vcl_string input_dir = "images/";

    match_->create_html_rank_order_table(input_dir, 
                                         params_->model_object_name_(), 
                                         params_->query_object_name_(), 
                                         output_html_, 
                                         table_title, 
                                         params_->html_image_extension_(), 
                                         params_->show_html_matches_());


    return true;

 
}

