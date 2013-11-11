// This is dbsksp/pro/dbsksp_simplify_xshock_graph_process.cxx

//:
// \file

#include "dbsksp_simplify_xshock_graph_process.h"

#include <dbsksp/algo/dbsksp_simplify.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_simplify_xshock_graph_process::
dbsksp_simplify_xshock_graph_process()
{
  if( !parameters()->add("In-place simplification (no new shock graph will be created)" , "-simplify_in_place", false) ||
    !parameters()->add("Max deviation [pixels] allowed as result of simplification (epsilon): " , "-epsilon", double(1.0)) ||
    !parameters()->add("Sampling rate [pixels] of the new boundary to compute deviation (sampling_ds)", "-sampling_ds", double (1.0))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_simplify_xshock_graph_process::
~dbsksp_simplify_xshock_graph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_simplify_xshock_graph_process::
clone() const
{
  return new dbsksp_simplify_xshock_graph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_simplify_xshock_graph_process::
name()
{ 
  return "Simplify xshock graph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_simplify_xshock_graph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_simplify_xshock_graph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  bool simplify_in_place = false;
  this->parameters()->get_value("-simplify_in_place", simplify_in_place);
  if (!simplify_in_place)
    to_return.push_back( "dbsksp_shock" );
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_simplify_xshock_graph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_simplify_xshock_graph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_simplify_xshock_graph_process::
execute()
{
  // parse process parameters
  bool simplify_in_place = false;
  this->parameters()->get_value("-simplify_in_place", simplify_in_place);
  
  double epsilon = 1.0;
  this->parameters()->get_value("-epsilon", epsilon);


  double sampling_ds = 1.0;
  this->parameters()->get_value("-sampling_ds", sampling_ds);

  // input shock
  dbsksp_shock_storage_sptr input_shock;
  input_shock.vertical_cast(input_data_[0][0]);

  // create a copy of the existing xshock graph
  dbsksp_xshock_graph_sptr xgraph;
  dbsksp_shock_storage_sptr output_shock; // place holder for output graph
  
  if (simplify_in_place)
  {
    xgraph = input_shock->xshock_graph();
    output_shock = input_shock;
  }
  else
  {
    xgraph = new dbsksp_xshock_graph(*input_shock->xshock_graph());
    output_shock = dbsksp_shock_storage_new();
    output_shock->set_xshock_graph(xgraph);
  }
  output_data_[0].push_back(output_shock);
  
  // trace out the boundary of the current shock graph
  double approx_ds = 1.0;
  vsol_polygon_2d_sptr boundary = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, approx_ds);
  
  // run simplification code
  if (!dbsksp_reduce_A12_Ainfty_nodes(xgraph, boundary, epsilon, sampling_ds))
  {
    return false;
  }
  
  return true;
}


// -----------------------------------------------------------------------------
//: 
bool dbsksp_simplify_xshock_graph_process::
finish()
{
  return true;
}





