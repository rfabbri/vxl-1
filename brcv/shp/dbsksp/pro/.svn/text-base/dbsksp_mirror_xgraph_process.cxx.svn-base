// This is dbsksp/pro/dbsksp_mirror_xgraph_process.cxx

//:
// \file

#include "dbsksp_mirror_xgraph_process.h"

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vsol/vsol_box_2d.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_mirror_xgraph_process::
dbsksp_mirror_xgraph_process()
{
  if( !parameters()->add("Mirror around y-axis going thru bbox center?" , "-mirror_y_axis_bbox_center", true)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_mirror_xgraph_process::
~dbsksp_mirror_xgraph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_mirror_xgraph_process::
clone() const
{
  return new dbsksp_mirror_xgraph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_mirror_xgraph_process::
name()
{ 
  return "Mirror an xgraph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_mirror_xgraph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_mirror_xgraph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_mirror_xgraph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_mirror_xgraph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_mirror_xgraph_process::
execute()
{

  // Parse parameter values
  bool mirror_y_axis_centroid = true;
  this->parameters()->get_value("-mirror_y_axis_bbox_center", mirror_y_axis_centroid);

  if (!mirror_y_axis_centroid)
  {
    vcl_cout << "No mirroring. Done.\n";
    return false;
  }

  vcl_cout << "\nMirror xgraph around y-axis going through bbox center.\n";
  
  // input xgraph
  dbsksp_xgraph_storage_sptr input_storage;
  input_storage.vertical_cast(input_data_[0][0]);
  
  dbsksp_xshock_graph_sptr xgraph = input_storage->xgraph();
  if (!xgraph)
  {
    vcl_cout << "Input xgraph non-existing. Process failed.\n";
    return false;
  }

  // Computing bounding box center
  xgraph->update_bounding_box();
  vsol_box_2d_sptr bbox = xgraph->bounding_box();
  double center_x = (bbox->get_min_x() + bbox->get_max_x()) / 2;
  double center_y = (bbox->get_min_y() + bbox->get_max_y()) / 2;
  vcl_cout << "Bounding box center: (x,y) = (" << center_x << " , " << center_y << ")\n";
  
  
  // Create a duplicate xgraph and mirror it around y-axis
  dbsksp_xshock_graph_sptr mirror_xgraph = new dbsksp_xshock_graph(*xgraph);
  mirror_xgraph->mirror_around_y_axis(center_x);

  // Save output
  if (mirror_xgraph)
  {
    // create a storage and push this tree in
    dbsksp_xgraph_storage_sptr output_storage = dbsksp_xgraph_storage_new();
    output_data_[0].push_back(output_storage);
    output_storage->set_xgraph(mirror_xgraph);
    vcl_cout << "XGraph mirroring completed.\n";
    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
//:
bool dbsksp_mirror_xgraph_process::
finish()
{
  return true;
}





