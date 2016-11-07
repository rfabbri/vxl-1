// This is dbsksp/pro/dbsksp_trace_shock_boundary_process.cxx

//:
// \file

#include "dbsksp_trace_shock_boundary_process.h"
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>

//: Constructor
dbsksp_trace_shock_boundary_process::
dbsksp_trace_shock_boundary_process()
{
  // two types of shock graph - intrinsic and extrinsic
  vcl_vector<vcl_string > shock_graph_types;
  shock_graph_types.push_back("dbsksp_shock_graph");
  shock_graph_types.push_back("dbsksp_xshock_graph");

  if( !parameters()->add("Type of input shock graph: " , "-shock_graph_type", 
    shock_graph_types, 0) ||
    !parameters()->add("Approximate sampling resolution (in pixels) (only for xshock):", "-approx_ds", double(5.0))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }

}


//: Destructor
dbsksp_trace_shock_boundary_process::
~dbsksp_trace_shock_boundary_process()
{
}


//: Clone the process
bpro1_process* dbsksp_trace_shock_boundary_process::
clone() const
{
  return new dbsksp_trace_shock_boundary_process(*this);
}

//: Returns the name of this process
vcl_string dbsksp_trace_shock_boundary_process::
name()
{ 
  return "Trace shock boundary"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_trace_shock_boundary_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_trace_shock_boundary_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_trace_shock_boundary_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsksp_trace_shock_boundary_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_trace_shock_boundary_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsksp_trace_shock_boundary_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // i) Parse the parameters
  
  unsigned int shock_graph_type = 0;
  this->parameters()->get_value( "-shock_graph_type" , shock_graph_type);

  double approx_ds;
  this->parameters()->get_value("-approx_ds", approx_ds);

  // ii) Get the storage

  // get the shock graph from the storage classes
  dbsksp_shock_storage_sptr shock_storage;
  shock_storage.vertical_cast(input_data_[0][0]);

  // create the output storage class
  vidpro1_vsol2D_storage_sptr vsol_storage = vidpro1_vsol2D_storage_new();
  output_data_[0].push_back(vsol_storage);
  

  if (shock_graph_type == 0)
  {
    dbsksp_shock_graph_sptr shock_graph = shock_storage->shock_graph();
    vcl_vector<vsol_spatial_object_2d_sptr > boundary(shock_graph->trace_boundary());
    vsol_storage->add_objects(boundary, "boundary");
  }
  else if (shock_graph_type == 1)
  {
    dbsksp_xshock_graph_sptr xgraph = shock_storage->xshock_graph();
    
    
    // trace the boundary
    vcl_vector<vgl_point_2d<double > > bnd_pts;
    dbsksp_trace_xgraph_boundary_as_polygon(xgraph, bnd_pts, approx_ds);

    // convert to vsol polygon
    vcl_vector<vsol_point_2d_sptr > vertices(bnd_pts.size(), 0);
    for (unsigned i =0; i < bnd_pts.size(); ++i)
    {
      vertices[i] = new vsol_point_2d(bnd_pts[i]);
    }
    vsol_storage->add_object(new vsol_polygon_2d(vertices), "boundary");
  }
   
  return true;
}

bool dbsksp_trace_shock_boundary_process::
finish()
{
  return true;
}





