// This is dbskr/pro/dbskr_detect_shock_patches_process.cxx

//:
// \file

#include <dbskr/pro/dbskr_detect_shock_patches_process.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <dborl/dborl_image_description.h>
#include <vul/vul_file.h>
#include <bbas/bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>

dbskr_detect_shock_patches_process::dbskr_detect_shock_patches_process()
:bpro1_process()
{

    //Create string for really long description
    vcl_string descrip = "constraint that at least k model patches ";
    descrip += "have top matches less than threshold";

    if ( !parameters()->add( "Query Bin file:" , 
                             "-query_filepath" , 
                             bpro1_filepath("", "*.bin")) ||

         !parameters()->add( "Match Sim Bin file:" , 
                             "-match_sim_filepath" , 
                             bpro1_filepath("", "*.bin")) ||

         !parameters()->add( "threshold for detections", 
                             "-thres", (double) 0.5f ) ||
         !parameters()->
         add( "use top N matches of the image to each model patch",
              "-top_N",(unsigned int) 3 ) ||

         !parameters()->
         add("at least k model patches", "-con_k",(unsigned int) 2)       
         
   
        )
    {
        
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;

    }

}

//: Clone the process
bpro1_process*
dbskr_detect_shock_patches_process::clone() const
{
  return new dbskr_detect_shock_patches_process(*this);
}

vcl_vector< vcl_string > dbskr_detect_shock_patches_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

vcl_vector< vcl_string > dbskr_detect_shock_patches_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    // ouptut the storage class
    to_return.push_back( "vsol2D" );
    return to_return;
}

bool dbskr_detect_shock_patches_process::execute()
{

    // Read in parameters
    double threshold;
    parameters()->get_value("-thres",threshold);

    unsigned int top_N, con_k;
    parameters()->get_value("-top_N",top_N);
    parameters()->get_value("-con_k",con_k);

    bpro1_filepath model_filepath;
    parameters()->get_value("-match_sim_filepath", model_filepath);
    vcl_string match_file = model_filepath.path;

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
        
    // ------------------ Perform Detection ------------------------------
    vcl_cout<<"Running Detection on " << vul_file::strip_directory(match_file)
            << " at a threshold of " << threshold<<" "<<vcl_endl;

    //: create match structure by reading in existing match
    dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
    vsl_b_ifstream ifsm(match_file.c_str());
    match->b_read(ifsm);
    ifsm.close();

    vcl_map<int, dbskr_shock_patch_sptr> query_map;
    for (unsigned ii = 0; ii < query_st->size(); ii++)
    { 
        query_map[query_st->get_patch(ii)->id()] = query_st->get_patch(ii);
    }
    match->set_id_map2(query_map);
    
    vsol_box_2d_sptr box;
    if (!match->detect_instance(box,top_N,con_k,threshold)) 
    {
        vcl_cerr << "Detection Error in procssing" << "!!!!\n";
        return false;
    }

    // Bouding box could be empty 
    if ( box )
    {   
        // Add to vidpro storage this new bounding box
        vsol_polygon_2d_sptr box_poly = bsol_algs::poly_from_box(box);

        vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
        output_vsol->add_object(box_poly->cast_to_spatial_object());
        output_data_[0].push_back(output_vsol);
        vcl_cout<<"Bounding box detected at this threshold"<<vcl_endl;
    }
    else
    {

        vcl_cout<<"No bounding box detected at this threshold"<<vcl_endl;

    }

    vcl_cout<<vcl_endl;
    return true;
}
