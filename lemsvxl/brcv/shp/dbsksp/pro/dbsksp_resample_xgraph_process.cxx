// This is dbsksp/pro/dbsksp_resample_xgraph_process.cxx

//:
// \file

#include "dbsksp_resample_xgraph_process.h"

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <dbsksp/algo/dbsksp_fit_xgraph.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_resample_xgraph_process::
dbsksp_resample_xgraph_process()
{
  if( !parameters()->add("Sampling distance ds =" , "-sample-ds" , 1.0f) ||
    !parameters()->add("Distance RMS error threshold (pixels): " , "-distance-rms-error-threshold" , 0.5f)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_resample_xgraph_process::
~dbsksp_resample_xgraph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_resample_xgraph_process::
clone() const
{
  return new dbsksp_resample_xgraph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_resample_xgraph_process::
name()
{ 
  return "Resample an xgraph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_resample_xgraph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_resample_xgraph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_resample_xgraph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_resample_xgraph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_resample_xgraph_process::
execute()
{
  float distance_rms_error_threshold = 0.5;
  this->parameters()->get_value("-distance-rms-error-threshold", distance_rms_error_threshold);

  float sample_ds = 1.0f;
  this->parameters()->get_value("-sample-ds", sample_ds);


  // input shock
  dbsksp_xgraph_storage_sptr input_xgraph;
  input_xgraph.vertical_cast(input_data_[0][0]);
  
  // Call the fitting function
  dbsksp_fit_xgraph fit_xgraph(distance_rms_error_threshold);
  dbsksp_xshock_graph_sptr resampled_xgraph = 0;
  vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr > map_new_node_to_old_node;
  fit_xgraph.fit_to(input_xgraph->xgraph(), resampled_xgraph, map_new_node_to_old_node, sample_ds);

  // Save computed xshock to output storage
  if (resampled_xgraph)
  {
    // storage for output graph
    dbsksp_xgraph_storage_sptr output_xgraph = dbsksp_xgraph_storage_new();
    output_data_[0].push_back(output_xgraph);
    output_xgraph->set_xgraph(resampled_xgraph);
    return true;
  }
  else
  {
    return false;
  }
  return true;
}

bool dbsksp_resample_xgraph_process::
finish()
{
  return true;
}





