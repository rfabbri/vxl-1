// This is dbsks/pro/dbsks_detect_shape_process.cxx

//:
// \file

#include "dbsks_detect_shape_process.h"
#include <vil/vil_image_resource.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_utils.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <dbsks/xio/dbsks_xio_stats.h>


#include <vil/vil_convert.h>
#include <vnl/vnl_math.h>

#include <vul/vul_sprintf.h>
#include <vul/vul_get_timestamp.h>





//: Constructor
dbsks_detect_shape_process::
dbsks_detect_shape_process()
{
  vcl_vector<vcl_string > opt_mode;
  opt_mode.push_back("Complete one pass with model_height"); // 0
  opt_mode.push_back("do nothing - for now"); // 1
  opt_mode.push_back("Initialize only"); // 2

  if ( !parameters()->add("Optimization mode: " , "opt_mode" , opt_mode, 0) ||
    !parameters()->add("Model height (height of model bbox): " , "model_height", 350.0f) ||
    !parameters()->add("sigma_deform: " , "sigma_deform", 0.25f) ||
    
    !parameters()->add("Use image center? (256,256) otherwise" , "use_image_center", true) ||
    !parameters()->add("shapelet grid center_x: " , "center_x", 256.0f ) ||
    !parameters()->add("shapelet grid center_y: " , "center_y", 256.0f ) ||

    !parameters()->add("Use arc_grid step_x and step_y?", 
      "use_arc_grid_steps", true) ||
    !parameters()->add("shapelet grid step_x: " , "step_x", 8.0f ) ||
    !parameters()->add("shapelet_grid step_y: " , "step_y", 8.0f ) ||

    !parameters()->add("Use image size? (32, 32) otherwise" , "use_image_size", true) ||
    !parameters()->add("shapelet_grid half_num_x: " , "half_num_x", int(32) ) ||
    !parameters()->add("shapelet_grid half_num_y: " , "half_num_y", int(32) ) ||

    !parameters()->add("Use arc_grid num_theta? 16 otherwise" , 
    "use_arc_grid_num_theta", true) ||
    //!parameters()->add("shapelet_grid num_psi: " , "num_psi", int(16) ) ||

    !parameters()->add("shapelet_grid half_num_phiA: " , "half_num_phiA", int(2) ) ||
    !parameters()->add("shapelet_grid half_num_phiB: " , "half_num_phiB", int(2) ) ||
    // !parameters()->add("Force phiA and phiB centered at pi/2: " , 
    //                                    "force_phi_centered_at_pi_over_2", false ) ||
    
    // !parameters()->add("Half range of phiA and phiB: " , 
    //                              "half_range_phi", float(vnl_math::pi_over_2/2) ) ||
    // !parameters()->add("shapelet_grid max_m: " , "max_m", 0.3f ) ||
    !parameters()->add("shapelet_grid half_num_m: " , "half_num_m", int(1) ) ||

    // !parameters()->add("shapelet_grid max_log2_len: " , "max_log2_len", 0.3f ) ||
    !parameters()->add("shapelet_grid half_num_len: " , "half_num_len", int(1) ) ||

    // !parameters()->add("shapelet_grid max_log2_rA: " , "max_log2_rA", 0.3f ) ||
    !parameters()->add("shapelet_grid half_num_rA: " , "half_num_rA", int(1) ) ||

    !parameters()->add("Shock graph statistics file: ", "stats_file", 
    vcl_string("D:/vision/data/symseg/ETHZShapeClasses/giraffes_stats.xml") ) || 
    
    !parameters()->add("Prefix of record-keeping files: ", "tmp_prefix", 
    vcl_string("D:/vision/data/symseg/temp/tmp") ) || 

    !parameters()->add("Temporary data folder: ", "temp_data_folder", 
    vcl_string("D:/vision/data/symseg/temp/") )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_detect_shape_process::
~dbsks_detect_shape_process()
{
}


//: Clone the process
bpro1_process* dbsks_detect_shape_process::
clone() const
{
  return new dbsks_detect_shape_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_detect_shape_process::
name()
{ 
  return "Detect Shape"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_detect_shape_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  to_return.push_back( "dbsksp_shock" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_detect_shape_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  to_return.push_back("vsol2D");

  return to_return;
}

//: Return the number of input frames for this process
int dbsks_detect_shape_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_detect_shape_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_detect_shape_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_detect_shape_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
    
  unsigned int opt_mode = 100;
  parameters()->get_value( "opt_mode" , opt_mode);

  float sigma_deform = 0.25f;
  parameters()->get_value( "sigma_deform", sigma_deform);

  float model_height = 350;
  parameters()->get_value( "model_height", model_height);

  bool use_image_center = false;
  parameters()->get_value( "use_image_center", use_image_center);

  dbsks_shapelet_grid_params params;
  // hack because the menu is too long
  params.center_x = 256.0f;
  params.center_y = 256.0f;
  parameters()->get_value( "center_x", params.center_x);
  parameters()->get_value( "center_y", params.center_y);

  bool use_arc_grid_steps = false;
  parameters()->get_value( "use_arc_grid_steps", use_arc_grid_steps);
  parameters()->get_value( "step_x", params.step_x);
  parameters()->get_value( "step_y", params.step_y);

  bool use_image_size = false;
  parameters()->get_value( "use_image_size", use_image_size);
  // hack because the menu is too long
  params.half_num_x = 32;
  params.half_num_y = 32;
  parameters()->get_value( "half_num_x", params.half_num_x);
  parameters()->get_value( "half_num_y", params.half_num_y);

  bool use_arc_grid_num_theta = false;
  parameters()->get_value( "use_arc_grid_num_theta", use_arc_grid_num_theta);

  // hack because the menu is too long
  params.num_psi = 16;
  // parameters()->get_value( "num_psi", params.num_psi);

  // hack because menu is too long
  params.force_phi_centered_at_pi_over_2 = false;
  // parameters()->get_value( "force_phi_centered_at_pi_over_2", 
//    params.force_phi_centered_at_pi_over_2);


  params.half_range_phiA = float(vnl_math::pi/4);
  parameters()->get_value( "half_num_phiA", params.half_num_phiA);

  params.half_range_phiB = float(vnl_math::pi/4);
  parameters()->get_value( "half_num_phiB", params.half_num_phiB);

  
  // parameters()->get_value( "half_range_phi", params.half_range_phiA);
  params.half_range_phiB = params.half_range_phiA;
  
  // parameters()->get_value( "max_m", params.max_m);
  parameters()->get_value( "half_num_m", params.half_num_m);

  // parameters()->get_value( "max_log2_len", params.max_log2_len);
  parameters()->get_value( "half_num_len", params.half_num_len);

  // parameters()->get_value( "max_log2_rA", params.max_log2_rA);
  parameters()->get_value( "half_num_rA", params.half_num_rA);

  assert(sigma_deform >0);

  // Prefix of record-keeping files
  vcl_string stats_file;
  parameters()->get_value("stats_file", stats_file);

  // Prefix of record-keeping files
  vcl_string tmp_prefix;
  parameters()->get_value("tmp_prefix", tmp_prefix);

  // folder to save temporary files
  vcl_string temp_data_folder;
  parameters()->get_value("temp_data_folder", temp_data_folder);
  


  // STORAGE CLASSES ----------------------------------------------------------
  
  // DP shape matcher
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(input_data_[0][0]);

  // shock graph
  dbsksp_shock_storage_sptr sksp_storage;
  sksp_storage.vertical_cast(input_data_[0][1]);

  // image storage - just to get the size of the image
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][2]);

  // PROCESS DATA -------------------------------------------------------------

  // assuming the arc_grid and their associated costs have been loaded to DP
  dbsks_circ_arc_grid arc_grid = shapematch_storage->dp_engine()->arc_grid();

  // Retrieve the image from storage
  vil_image_resource_sptr image_resource = image_storage->get_image();

  // >> Take care of the options relating to the grid params (use_arc_grid_center, etc...)
  if (use_image_center)
  {
    params.center_x = float(image_resource->ni() / 2);
    params.center_y = float(image_resource->nj() / 2);
  }

  if (use_arc_grid_steps)
  {
    params.step_x = float(arc_grid.step_x_);
    params.step_y = float(arc_grid.step_y_);
  }

  if (use_image_size)
  {
    params.half_num_x = vnl_math::rnd(image_resource->ni()/(2*params.step_x)) + 1;
    params.half_num_y = vnl_math::rnd(image_resource->nj()/(2*params.step_y)) + 1;

    // limite the grid size to 36 x 36
    if (params.half_num_x > 36)
      params.half_num_x = 36;

    if (params.half_num_y > 36)
      params.half_num_x = 36;
  }

  if (use_arc_grid_num_theta)
  {
    params.num_psi = arc_grid.num_theta_;
  }


  // (try to) generate a random prefix
  int secs, msecs;
  vul_get_timestamp(secs, msecs);
  tmp_prefix += vul_sprintf("%p%d", image_resource.as_pointer(), msecs);

  
  
  // Add parameters for running at multiple scales
  // add option in load_arc_cost to add frame number in front of the file

  if (opt_mode == 0) // Run a complete pass
  {
    // >> Set up the shape detector
    dbsks_dp_match_sptr dp_engine = shapematch_storage->dp_engine();
    
    dp_engine->set_graph_params(sksp_storage->shock_graph(), 
      image_resource->ni(), image_resource->nj());

    //: Set up parameters to run DP with the given working graph
    dp_engine->set_dp_params(params, sigma_deform, tmp_prefix, temp_data_folder);

    // Load the statistics
    // ATTENTION: the statistic file need to be associated with the shock graph in
    // the storage class
    // \TODO put the statistics inside the storage class
    dbsks_shock_graph_stats stats;
    if (!x_read(stats_file, sksp_storage->shock_graph(), stats))
    {
      vcl_cout << "ERROR: could not read shock graph statistics file.\n";
      //return false;
    }

    vcl_cout << "Set shock graph statistics.\n";
    dp_engine->set_shock_graph_stats(stats);
    


    vcl_cout << "Now run DP on the selected image. \n";


    // set tolerance near zero for the edges (1/2 grid step);
    //float tol = (params.step_x + params.step_y) / 4;
    float tol = 0;
    dp_engine->oriented_chamfer()->set_tolerance_near_zero(tol);

    ///////////////////////////////////////////////////////////////////////////////
    //dp_engine->run_optim_for_one_model_height(model_height);
    
    dp_engine->run_optim_for_one_model_size(model_height);


    // DEBUG /////////
    ////dp_engine->init_dp_vars_using_stats(model_height);
    //double current_cost = 
    //  dp_engine->compute_graph_cost(sksp_storage->shock_graph(), true, true);

    //vcl_cout << "Cost of current graph = " << current_cost << "\n";
    ///////////////////////////////////////////////////////////////////////////////

    vcl_cout << "Done.\n";

    //// display real cost of the fragments
    //dp_engine->display_real_cost(dp_engine->graph_opt_i_state_, vcl_cout);


    vcl_cout << "Create new output shock storage.\n";

    // create the output storage class
    dbsksp_shock_storage_sptr out_sksp_storage = dbsksp_shock_storage_new();
    output_data_[0].push_back(out_sksp_storage);

    // Result shock graph
    vcl_cout << "Create new shock graph.\n";
    dbsksp_shock_graph_sptr new_shock_graph = 
      new dbsksp_shock_graph(*dp_engine->graph());
    new_shock_graph->compute_all_dependent_params();

    // Save to storage class
    out_sksp_storage->set_shock_graph(new_shock_graph);
    
    vcl_cout << "Export optimal shapelets to new shock storage class.\n";
    if (!dp_engine->shapelet_list.empty())
    {
      for (unsigned i =0; i<dp_engine->shapelet_list.size(); ++i)
      {
        out_sksp_storage->add_shapelet(dp_engine->shapelet_list[i]);
      }
    }


    vcl_cout << "Export boundary of sub-optimal solutions to a vsol2D storage.\n";
    
    // create the output storage class
    vidpro1_vsol2D_storage_sptr vsol2D_storage = vidpro1_vsol2D_storage_new();
    output_data_[0].push_back(vsol2D_storage);

    for (unsigned i =0; i < dp_engine->list_graph_opt_i_state_.size(); ++i)
    {
      vcl_vector<vsol_spatial_object_2d_sptr > vsol_list = 
        dp_engine->trace_graph_boundary(dp_engine->list_graph_opt_i_state_[i]);
      vsol2D_storage->add_objects(vsol_list);
    }
    vcl_cout << "All done.\n";

  }
  else if (opt_mode == 1) // Refine and run once
  {
    
  }
  else if (opt_mode == 2) // Initialize only
  {    
    // >> Set up the shape detector
    dbsks_dp_match_sptr dp_engine = shapematch_storage->dp_engine(); 
    
    // Set up parameters to construct a working graph
    dp_engine->set_graph_params(sksp_storage->shock_graph(), 
      image_resource->ni(), image_resource->nj());

    //: Set up parameters to run DP with the given working graph
    dp_engine->set_dp_params(params, sigma_deform, tmp_prefix, temp_data_folder);

    ///////////////////////////////////////////////////////////////////////////////
    dp_engine->init_dp_vars(model_height);
    ///////////////////////////////////////////////////////////////////////////////

    // create the output storage class
    dbsksp_shock_storage_sptr out_sksp_storage = dbsksp_shock_storage_new();
    output_data_[0].push_back(out_sksp_storage);

    // Result shock graph
    dbsksp_shock_graph_sptr new_shock_graph = 
      dbsks_construct_graph(dp_engine, dp_engine->graph_opt_i_state_);
    new_shock_graph->compute_all_dependent_params();

    // Save to storage class
    out_sksp_storage->set_shock_graph(new_shock_graph);

  }
    
  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_detect_shape_process::
finish()
{
  return true;
}
