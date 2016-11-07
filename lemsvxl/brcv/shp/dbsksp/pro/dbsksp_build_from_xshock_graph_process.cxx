// This is dbsksp/pro/dbsksp_build_from_xshock_graph_process.cxx

//:
// \file

#include "dbsksp_build_from_xshock_graph_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/algo/dbsksp_build_from_xshock_graph.h>

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>



// ----------------------------------------------------------------------------
//: Constructor
dbsksp_build_from_xshock_graph_process::
dbsksp_build_from_xshock_graph_process()
{
  this->build_type_descriptions_.push_back("build from extrinsic shock graph");

  if( !parameters()->add("Build type: " , "-build_type" , 
    this->build_type_descriptions_, 0) ||
    !parameters()->add("Shock  sampling ds = " , 
    "-scurve_sample_ds", double(1.0)) ||
    !parameters()->add("Shock interpolation sampling ds = " , 
    "-shock_interp_sample_ds", double(6.0) )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_build_from_xshock_graph_process::
~dbsksp_build_from_xshock_graph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_build_from_xshock_graph_process::
clone() const
{
  return new dbsksp_build_from_xshock_graph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_build_from_xshock_graph_process::
name()
{ 
  return "Build from extrinsic shock graph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_build_from_xshock_graph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("shock");
  

  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_build_from_xshock_graph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_build_from_xshock_graph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_build_from_xshock_graph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_build_from_xshock_graph_process::
execute()
{
  // Preliminary check for inputs 

  unsigned int build_type = 0;
  parameters()->get_value( "-build_type" , build_type);
  vcl_string mode = this->build_type_descriptions_[build_type];

  double scurve_sample_ds = 0;
  parameters()->get_value( "-scurve_sample_ds" , scurve_sample_ds);

  double shock_interp_sample_ds = 0;
  parameters()->get_value( "-shock_interp_sample_ds" , shock_interp_sample_ds);
  
  // type 1: create a shock graph from a polyline
  if (mode != "build from extrinsic shock graph")
  {
    vcl_cerr << "ERROR: invalid build type.\n";
    return false;
  }

  if ( input_data_.size() != 1 )
  {
    vcl_cerr << "In dbsksp_build_from_xshock_graph_process::execute() - "
            << "not exactly one input frame" << vcl_endl;
    return false;
  }

  // get contour from the storage classes
  dbsk2d_shock_storage_sptr xshock_storage;
  xshock_storage.vertical_cast(input_data_[0][0]);


  // Convert the extrinsic shock graph to a generative shock graph
  dbsksp_build_from_xshock_graph builder;
  builder.set_scurve_sample_ds(scurve_sample_ds);
  builder.set_shock_interp_sample_ds(shock_interp_sample_ds);
  

  dbsksp_shock_graph_sptr shock_graph = 
    //builder.build_from_xshock_graph(xshock_storage->get_shock_graph());
    builder.build_from_xshock_graph_using_skr_tree(xshock_storage->get_shock_graph());
    
  // create the output storage class
  dbsksp_shock_storage_sptr output_shock = dbsksp_shock_storage_new();
  output_data_[0].push_back(output_shock);
  output_shock->set_shock_graph(shock_graph);
  for (unsigned i=0; i < builder.shapelets.size(); ++i)
  {
    output_shock->add_shapelet(builder.shapelets[i]);
  }
   
  return true;
}

bool dbsksp_build_from_xshock_graph_process::
finish()
{
  return true;
}





