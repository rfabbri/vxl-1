// This is dbsksp/pro/dbsksp_normalize_xgraph_process.cxx

//:
// \file

#include "dbsksp_normalize_xgraph_process.h"
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vsol/vsol_box_2d.h>
#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_normalize_xgraph_process::
dbsksp_normalize_xgraph_process()
{
  if( !parameters()->add("Normalized shape size (square root of shape area): " , "-norm_shape_size" , 100.0f)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_normalize_xgraph_process::
~dbsksp_normalize_xgraph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_normalize_xgraph_process::
clone() const
{
  return new dbsksp_normalize_xgraph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_normalize_xgraph_process::
name()
{ 
  return "Normalize an xgraph"; 
}




// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_normalize_xgraph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_normalize_xgraph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_normalize_xgraph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_normalize_xgraph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_normalize_xgraph_process::
execute()
{

  // Parse parameter values
  float norm_shape_size = 100.0f;
  this->parameters()->get_value("-norm_shape_size", norm_shape_size);

  vcl_cout << "\nNormalize xgraph around bounding box center.\n";
  
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

  // Current size
  double cur_size = vcl_sqrt(xgraph->area());
  vcl_cout << "Old shape size = " << cur_size << "\n";
  vcl_cout << "New shape size = " << norm_shape_size << "\n";
  
  // Create a duplicate xgraph and scale it around bounding box center
  dbsksp_xshock_graph_sptr norm_xgraph = new dbsksp_xshock_graph(*xgraph);
  norm_xgraph->scale_up(center_x, center_y, norm_shape_size / cur_size);

  // Save output
  if (norm_xgraph)
  {
    // create a storage and push this tree in
    dbsksp_xgraph_storage_sptr output_storage = dbsksp_xgraph_storage_new();
    output_data_[0].push_back(output_storage);
    output_storage->set_xgraph(norm_xgraph);
    vcl_cout << "XGraph normalization completed.\n";
    return true;
  }

  return false;
}


//------------------------------------------------------------------------------
//:
bool dbsksp_normalize_xgraph_process::
finish()
{
  return true;
}





