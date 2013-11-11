// This is dbsks/pro/dbsks_trace_opt_graph_bnd_process.cxx

//:
// \file

#include "dbsks_trace_opt_graph_bnd_process.h"
#include <vil/vil_image_resource.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>

#include <dbsks/dbsks_dp_match.h>

//#include <vil/vil_convert.h>
//#include <vnl/vnl_math.h>




//: Constructor
dbsks_trace_opt_graph_bnd_process::
dbsks_trace_opt_graph_bnd_process()
{

  if ( !parameters()->add("Scale index: " , "-scale_index", unsigned(0)) ||
    !parameters()->add("Out file to: " , "-out_file", bpro1_filepath("","*") )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_trace_opt_graph_bnd_process::
~dbsks_trace_opt_graph_bnd_process()
{
}


//: Clone the process
bpro1_process* dbsks_trace_opt_graph_bnd_process::
clone() const
{
  return new dbsks_trace_opt_graph_bnd_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_trace_opt_graph_bnd_process::
name()
{ 
  return "Trace opt graph boundary"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_trace_opt_graph_bnd_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_trace_opt_graph_bnd_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("vsol2D");
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_trace_opt_graph_bnd_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_trace_opt_graph_bnd_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_trace_opt_graph_bnd_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_trace_opt_graph_bnd_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
    
  unsigned scale_index = 0;
  parameters()->get_value( "-scale_index", scale_index);

  // Prefix of files to save temporary data
  bpro1_filepath out_file;
  parameters()->get_value("-out_file", out_file);
  

  // STORAGE CLASSES ----------------------------------------------------------
  
  // DP shape matcher
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(input_data_[0][0]);
  dbsks_dp_match_sptr dp_engine = shapematch_storage->dp_engine();

  // image storage - just to get the size of the image
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][1]);

  // PROCESS DATA -------------------------------------------------------------


  vcl_vector<vsol_spatial_object_2d_sptr > vsol_list;
  ///////////////////////////////////////////////////////////////////////////////
  dp_engine->trace_opt_graph_bnd(vsol_list);
  ///////////////////////////////////////////////////////////////////////////////

  // create the output storage class
  vidpro1_vsol2D_storage_sptr vsol2D_storage = vidpro1_vsol2D_storage_new();
  output_data_[0].push_back(vsol2D_storage);

  vsol2D_storage->add_objects(vsol_list, "opt_graph");
  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_trace_opt_graph_bnd_process::
finish()
{
  return true;
}
