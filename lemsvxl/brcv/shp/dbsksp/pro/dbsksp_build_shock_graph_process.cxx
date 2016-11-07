// This is dbsksp/pro/dbsksp_build_shock_graph_process.cxx

//:
// \file

#include "dbsksp_build_shock_graph_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/algo/dbsksp_convert.h>

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>



// ----------------------------------------------------------------------------
//: Constructor
dbsksp_build_shock_graph_process::
dbsksp_build_shock_graph_process()
{
  this->build_type_descriptions_.push_back("from a polyline");
  this->build_type_descriptions_.push_back("one simple A_1^3 node");

  // two types of shock graph - intrinsic and extrinsic
  this->shock_graph_types_.push_back("dbsksp_shock_graph");
  this->shock_graph_types_.push_back("dbsksp_xshock_graph");

  if( !parameters()->add("Build type: " , "-build_type" , 
    this->build_type_descriptions_, 0) ||
    !parameters()->add("Type of output shock graph: " , "-shock_graph_type", 
    this->shock_graph_types_, 0) 
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_build_shock_graph_process::
~dbsksp_build_shock_graph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_build_shock_graph_process::
clone() const
{
  return new dbsksp_build_shock_graph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_build_shock_graph_process::
name()
{ 
  return "Build Shock Graph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_build_shock_graph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  unsigned int build_type = 0;
  parameters()->get_value( "-build_type" , build_type);
  vcl_string mode = this->build_type_descriptions_[build_type];

  // type 1: create a shock graph from a polyline
  if (mode == "from a polyline")
  {
    to_return.push_back("vsol2D");
  }
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_build_shock_graph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_build_shock_graph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_build_shock_graph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_build_shock_graph_process::
execute()
{

  unsigned int build_type = 0;
  this->parameters()->get_value( "-build_type" , build_type);
  vcl_string mode = this->build_type_descriptions_[build_type];
  
  unsigned int shock_graph_choice = 0;
  this->parameters()->get_value( "-shock_graph_type" , shock_graph_choice);
  vcl_string shock_graph_type = this->shock_graph_types_[shock_graph_choice];

  // place holder for output graph
  dbsksp_shock_storage_sptr output_shock = dbsksp_shock_storage_new();
  output_data_[0].push_back(output_shock);

  // type 1: create a shock graph from a polyline
  if (mode == "from a polyline")
  {

    if ( input_data_.size() != 1 ){
      vcl_cerr << "In dbsksp_build_shock_graph_process::execute() - "
             << "not exactly one input frame" << vcl_endl;
      return false;
    }

    // get contour from the storage classes
    vidpro1_vsol2D_storage_sptr vsol_storage;
    vsol_storage.vertical_cast(input_data_[0][0]);

    vcl_vector<vsol_spatial_object_2d_sptr > all_data = vsol_storage->all_data();

    // search for the first polyline
    vsol_polyline_2d_sptr polyline;
    for (unsigned int i=0; i<all_data.size(); ++i)
    {
      if (all_data[i]->cast_to_curve() && all_data[i]->cast_to_curve()->cast_to_polyline())
      {
        polyline = all_data[i]->cast_to_curve()->cast_to_polyline();
        break;
      }   
    }

    // Quit if there is no polyline present
    if (!polyline)
    {
      vcl_cout << "There is no polyline in the vsol storage class.\n";
      return false;
    }

    if (shock_graph_type == "dbsksp_shock_graph")
    {
      // Construct shock graph
      dbsksp_shock_graph_sptr shock_graph = new dbsksp_shock_graph();
      shock_graph->init(polyline);
      shock_graph->compute_all_dependent_params();
      output_shock->set_shock_graph(shock_graph);
    }
    else if (shock_graph_type == "dbsksp_xshock_graph")
    {
      // Construct extrinsic shock graph
      dbsksp_shock_graph_sptr shock_graph = new dbsksp_shock_graph();
      shock_graph->init(polyline);
      shock_graph->compute_all_dependent_params();
      dbsksp_xshock_graph_sptr xshock_graph = dbsksp_convert_to_xshock_graph(shock_graph);
      output_shock->set_xshock_graph(xshock_graph);
    }
    else
    {
      vcl_cerr << "ERROR: unknown shock graph type.\n";
    }

  }
  else if (mode == "one simple A_1^3 node")
  {
    if (shock_graph_type == "dbsksp_shock_graph")
    {
      // Construct shock graph
      dbsksp_shock_graph_sptr shock_graph = new dbsksp_shock_graph();
      shock_graph->init_simple_a13_graph();
      shock_graph->compute_all_dependent_params();
      output_shock->set_shock_graph(shock_graph);
    }
    else if (shock_graph_type == "dbsksp_xshock_graph")
    {
      // Construct shock graph
      dbsksp_shock_graph_sptr shock_graph = new dbsksp_shock_graph();
      shock_graph->init_simple_a13_graph();
      shock_graph->compute_all_dependent_params();
      dbsksp_xshock_graph_sptr xshock_graph = dbsksp_convert_to_xshock_graph(shock_graph);
      output_shock->set_xshock_graph(xshock_graph);
    }
    else
    {
      vcl_cerr << "ERROR: unknown shock graph type.\n";
    }
  }

  
  
   
  return true;
}

bool dbsksp_build_shock_graph_process::
finish()
{
  return true;
}





