// This is dbsks/pro/dbsks_load_dp_results_process.cxx

//:
// \file

#include "dbsks_load_dp_results_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_utils.h>

#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

//: Constructor
dbsks_load_dp_results_process::
dbsks_load_dp_results_process()
{
  if ( !parameters()->add("Load dp results from file: " , "-dp_file", 
    bpro1_filepath("","*")) ||
    !parameters()->add("Add frame number as prefix: " , "-add_frame_number", false) ||
    !parameters()->add("Create new shapematch_storage?: " , "-new_shapematch_storage", false)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_load_dp_results_process::
~dbsks_load_dp_results_process()
{
}


//: Clone the process
bpro1_process* dbsks_load_dp_results_process::
clone() const
{
  return new dbsks_load_dp_results_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_load_dp_results_process::
name()
{ 
  return "Load DP results"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_load_dp_results_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("image");

  bool new_shapematch_storage = false;
  parameters()->get_value( "-new_shapematch_storage", new_shapematch_storage);
  if (!new_shapematch_storage)
    to_return.push_back( "dbsks_shapematch" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_load_dp_results_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  to_return.push_back("dbsksp_shock");
  
  bool new_shapematch_storage = false;
  parameters()->get_value( "-new_shapematch_storage", new_shapematch_storage);
  if (new_shapematch_storage)
    to_return.push_back( "dbsks_shapematch" );

  return to_return;
}

//: Return the number of input frames for this process
int dbsks_load_dp_results_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_load_dp_results_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_load_dp_results_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_load_dp_results_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------

  bpro1_filepath dp_file;
  parameters()->get_value( "-dp_file", dp_file);

  bool add_frame_number = false;
  parameters()->get_value( "-add_frame_number", add_frame_number);


  bool new_shapematch_storage = false;
  parameters()->get_value( "-new_shapematch_storage", new_shapematch_storage);

  // STORAGE CLASSES ----------------------------------------------------------

  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);
  
  dbsks_dp_match_sptr dp_engine = 0;

  if (!new_shapematch_storage)
  {
    // get the user-input shapematch storage
    dbsks_shapematch_storage_sptr shapematch_storage;
    shapematch_storage.vertical_cast(input_data_[0][1]);
    dp_engine = shapematch_storage->dp_engine();
  }
  else
  {
    dp_engine = new dbsks_dp_match();
  }

 

  //// PROCESS DATA -------------------------------------------------------------

  vcl_string file_path = dp_file.path;
  
  vcl_string new_file_path = file_path;
  if (add_frame_number == true)
  {
    vcl_string base_name = 
      vul_file::strip_directory(vul_file::strip_extension(file_path));
    vcl_string file_dir = vul_file::dirname(file_path);
    vcl_string file_ext = vul_file::extension(file_path);

    int frame_number = image_storage->frame();
    
    // construct a new filename
    vcl_string new_base_name = vul_sprintf("%03d", frame_number) + "_" +
      base_name;
    
    new_file_path = file_dir + "\\" + new_base_name + file_ext;
  }


  ////////////////////////////////////////////////////////////////////////////
  dp_engine->load_dp_optim_results(new_file_path);
  ////////////////////////////////////////////////////////////////////////////

  // Generate the optimal graph and output
  dp_engine->construct_graph(dp_engine->graph_opt_i_state_, dp_engine->shapelet_list);

  // Output a shock graph with optimal states // just the fragments for now

  // create the output storage class
  dbsksp_shock_storage_sptr out_shock_storage = dbsksp_shock_storage_new();
  output_data_[0].push_back(out_shock_storage);

  //// optimized shock graph (not yet)
  //dbsksp_shock_graph_sptr new_shock_graph = 
  //  new dbsksp_shock_graph(*dp_engine->graph());

  // optimized shock graph (not yet)
  dbsksp_shock_graph_sptr new_shock_graph = dbsks_construct_graph(dp_engine, 
    dp_engine->graph_opt_i_state_);
    

  new_shock_graph->compute_all_dependent_params();

  // Save to storage class the shock graph and the fragments
  out_shock_storage->set_shock_graph(new_shock_graph);
  if (!dp_engine->shapelet_list.empty())
  {
    for (unsigned i =0; i<dp_engine->shapelet_list.size(); ++i)
    {
      out_shock_storage->add_shapelet(dp_engine->shapelet_list[i]);
    }
  }


  // shapematch storage
  if (new_shapematch_storage)
  {
    dbsks_shapematch_storage_sptr shapematch_storage = dbsks_shapematch_storage_new();
    output_data_[0].push_back(shapematch_storage);
    shapematch_storage->set_dp_engine(dp_engine);
  }






  return true;
}




// ----------------------------------------------------------------------------
bool dbsks_load_dp_results_process::
finish()
{
  return true;
}



