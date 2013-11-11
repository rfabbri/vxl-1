// This is dbskr/pro/dbskr_shock_patch_match_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_shock_patch_match_process.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>


dbskr_shock_patch_match_process::
dbskr_shock_patch_match_process() : bpro1_process()
{

    if ( !parameters()->add( "Model Bin file:" , "-model_filepath" , bpro1_filepath("", "*.bin")) ||
         !parameters()->add( "Query Bin file:" , "-query_filepath" , bpro1_filepath("", "*.bin")) ||
         !parameters()->add( "Output folder:"  , "-output_folder"  , bpro1_filepath("", "")) ||
         !parameters()->add( "Output Sim file prefix:" , "-output_prefix", vcl_string("")) ||
         !parameters()->add( "shock graph pruning threshold" , "-prune_thres", (float) 0.8f ) ||
         !parameters()->add( "normalize: using reconst bnd length (else total splice cost)" , "-norm_reconst", (bool) true ) ||
         !parameters()->add("Sampling ds to reconstruct the scurve", "-scurve_sample_ds", (float) 5.0f ) ||
         !parameters()->add("Interpolation ds to get densely interpolated versions of the scurves: meaningful if localized_edit option is ON","-scurve_interpolate_ds" , (float) 1.0f ) ||
         !parameters()->add("Improves elastic matching cost of scurves using the densely interpolated version", "-localized_edit", (bool) false ) ||
         !parameters()->add("use the elastic splice cost computation","-elastic_splice_cost", (bool) false) ||
         !parameters()->add("curve matching R that weighs bending over stretching","-curve_matching_R", (double) 6.0f) ||
         !parameters()->add("use circular completions at leaf branches during shock matching","-circular_ends", (bool) true) ||
         !parameters()->add("use combined edit cost during shock matching","-combined_edit", (bool) false) ||
         !parameters()->add("run coarse edit distance algorithm","-coarse_edit", (bool) false) 

        )
    {
        
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;

    }

}

//: Clone the process
bpro1_process*
dbskr_shock_patch_match_process::clone() const
{
  return new dbskr_shock_patch_match_process(*this);
}

vcl_vector< vcl_string > dbskr_shock_patch_match_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

vcl_vector< vcl_string > dbskr_shock_patch_match_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

bool dbskr_shock_patch_match_process::execute()
{

    // Grab all params
    // Grab tree edit params
    dbskr_tree_edit_params edit_params;    
    parameters()->get_value("-elastic_splice_cost"  , 
                            edit_params.elastic_splice_cost_); 
    parameters()->get_value("-scurve_sample_ds"     , 
                            edit_params.scurve_sample_ds_);
    parameters()->get_value("-scurve_interpolate_ds", 
                            edit_params.scurve_interpolate_ds_); 
    parameters()->get_value("-localized_edit"       , 
                            edit_params.localized_edit_ ); 
    parameters()->get_value("-curve_matching_R"     , 
                            edit_params.curve_matching_R_ ); 
    parameters()->get_value("-circular_ends"        , 
                            edit_params.circular_ends_); 
    parameters()->get_value("-combined_edit"        , 
                            edit_params.combined_edit_); 
    parameters()->get_value("-coarse_edit"          , 
                            edit_params.coarse_edit_); 

    float shock_prune_thres;
    parameters()->get_value( "-prune_thres", shock_prune_thres);

    bool norm_reconst;
    parameters()->get_value( "-norm_reconst", norm_reconst);
  
    bpro1_filepath output_folder_filepath;
    parameters()->get_value("-output_folder", output_folder_filepath);

    vcl_string output_prefix;
    parameters()->get_value("-output_prefix", output_prefix);
 
    vcl_string output_folder = output_folder_filepath.path;
    vcl_string output_name   = output_folder + "/" + output_prefix + ".bin";

    // ------------------ Load the model prototype ----------------------------
    vcl_string storage_end = "patch_strg.bin";

    // Grap file from parameters list
    bpro1_filepath model_filepath;
    parameters()->get_value( "-model_filepath" , model_filepath);
    vcl_string model_file = model_filepath.path;

    // Read in binary files
    dbskr_shock_patch_storage_sptr proto_st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifs(model_file.c_str());
    proto_st->b_read(ifs);
    ifs.close();

    // Throw an error for zero patches
    if (!proto_st->size()) {
        vcl_cerr << "zero patches in proto st!!!: " << model_file << vcl_endl;
        return false;
    }

    // Finish Loading all patches
    if (!proto_st->load_patch_shocks_and_create_trees
        (model_file, storage_end, 
         edit_params.elastic_splice_cost_, 
         edit_params.circular_ends_,
         edit_params.combined_edit_, 
         edit_params.scurve_sample_ds_, 
         edit_params.scurve_interpolate_ds_))
    {
     
        vcl_cerr<<"Error loading shock patches"<<vcl_endl;
        return false;

    }

    // ------------------ Load the query patches ------------------------------
    bpro1_filepath query_filepath;
    parameters()->get_value( "-query_filepath" , query_filepath);
    vcl_string query_file = query_filepath.path;

    // Read in binary files
    dbskr_shock_patch_storage_sptr query_st = dbskr_shock_patch_storage_new();
    vsl_b_ifstream ifsq(query_file.c_str());
    query_st->b_read(ifsq);
    ifsq.close();

    // Throw an error for zero patches
    if (!query_st->size()) {
        vcl_cerr << "zero patches in query st!!!: " << query_file << vcl_endl;
        return false;
    }

    // Finish Loading all patches
    if (!query_st->load_patch_shocks_and_create_trees
        (query_file, storage_end, 
         edit_params.elastic_splice_cost_, 
         edit_params.circular_ends_,
         edit_params.combined_edit_, 
         edit_params.scurve_sample_ds_, 
         edit_params.scurve_interpolate_ds_))
    {
     
        vcl_cerr<<"Error loading query shock patches"<<vcl_endl;
        return false;

    }

    // ------------------ Perform matching ------------------------------
    vcl_cout<<"Matching " << proto_st->size()<< " model patches to " 
            << query_st->size()<<" query patches"<<vcl_endl;

    //: create match structure
    dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
    patch_cor_map_type& map = match->get_map();
  
    //: set match parameters
    match->edit_params_ = edit_params;
    match->shock_pruning_threshold_ = shock_prune_thres;

    // : Do actual matching now
    vcl_vector<dbskr_shock_patch_sptr>& pv1 = proto_st->get_patches();
    vcl_vector<dbskr_shock_patch_sptr>& pv2 = query_st->get_patches();
  
    for (unsigned i = 0; i < pv1.size(); i++) 
    {

        if ( edit_params.coarse_edit_ )
        {
            find_patch_correspondences_coarse_edit(pv1[i], pv2, map, 
                                                   match->edit_params_);
        }
        else
        {
            find_patch_correspondences(pv1[i], pv2, map, match->edit_params_);
        }
        pv1[i]->kill_tree();
    }
    

    //: prepare id maps for this match
    vcl_map<int, dbskr_shock_patch_sptr> model_map;
    for (unsigned ii = 0; ii < proto_st->size(); ii++)
    {
        model_map[proto_st->get_patch(ii)->id()] = proto_st->get_patch(ii);
    }
    match->set_id_map1(model_map);

    vcl_map<int, dbskr_shock_patch_sptr> query_map;
    for (unsigned ii = 0; ii < query_st->size(); ii++)
    { 
        query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
    }
    match->set_id_map2(query_map);

    // Already normalize by total splice cost, change to length
    if (norm_reconst)
    {
        match->compute_length_norm_costs_of_cors();
    }
    else
    {
        match->compute_splice_norm_costs_of_cors();
    }
      
    // Write out results to ouput folder
    vsl_b_ofstream bfs(output_name.c_str());
    match->b_write(bfs);
    bfs.close();
    
    return true;
}
