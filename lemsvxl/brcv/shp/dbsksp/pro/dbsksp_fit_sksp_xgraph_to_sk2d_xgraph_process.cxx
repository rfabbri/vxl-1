// This is dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.cxx

//:
// \file

#include "dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h"

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <dbsksp/algo/dbsksp_fit_xgraph.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process()
{
  if( !parameters()->add("Distance RMS error threshold (pixels): " , "-distance-rms-error-threshold" , 0.5f)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
~dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
clone() const
{
  return new dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
name()
{ 
  return "Fit sksp_xgraph to sk2d_xgraph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
execute()
{
  float distance_rms_error_threshold = 0.5;
  this->parameters()->get_value("-distance-rms-error-threshold", distance_rms_error_threshold);


  // input shock
  dbsk2d_shock_storage_sptr input_shock;
  input_shock.vertical_cast(input_data_[0][0]);
  
  // Call the fitting function
  dbsksp_fit_xgraph fit_xgraph(distance_rms_error_threshold);
  dbsksp_xshock_graph_sptr xshock_graph = fit_xgraph.fit_to(input_shock->get_shock_graph());

  // Save computed xshock to output storage
  if (xshock_graph)
  {
    // storage for output graph
    dbsksp_xgraph_storage_sptr output_shock = dbsksp_xgraph_storage_new();
    output_data_[0].push_back(output_shock);
    output_shock->set_xgraph(xshock_graph);
    return true;
  }
  else
  {
    return false;
  }
  return true;
}

bool dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process::
finish()
{
  return true;
}





