// This is dbsks/pro/dbsks_load_arc_cost_process.cxx

//:
// \file

#include "dbsks_load_arc_cost_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_sptr.h>
#include <dbsks/dbsks_ocm_image_cost.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>



//: Constructor
dbsks_load_arc_cost_process::
dbsks_load_arc_cost_process()
{
  if ( !parameters()->add("Load arc costs from file: " , "-arc_cost_file", 
    bpro1_filepath("","*") ) ||
    !parameters()->add("Add frame number as prefix: " , "-add_frame_number", false)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_load_arc_cost_process::
~dbsks_load_arc_cost_process()
{
}


//: Clone the process
bpro1_process* dbsks_load_arc_cost_process::
clone() const
{
  return new dbsks_load_arc_cost_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_load_arc_cost_process::
name()
{ 
  return "Load arc cost"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_load_arc_cost_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_load_arc_cost_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_load_arc_cost_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_load_arc_cost_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_load_arc_cost_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_load_arc_cost_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
  bpro1_filepath arc_cost_file;
  parameters()->get_value( "-arc_cost_file", arc_cost_file);

  bool add_frame_number = false;
  parameters()->get_value( "-add_frame_number", add_frame_number);

  // INPUT STORAGE CLASSES ----------------------------------------------------

  // get the user-input image storage
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);
  vil_image_resource_sptr image_resource = image_storage->get_image();
  

  // PROCESS DATA -------------------------------------------------------------


  // determine filename of the input file
  vcl_string file_path = arc_cost_file.path;
  vcl_string new_file_path;
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
  else
  {
    new_file_path = file_path;
  }


  // Create a new DP match to load the arc costs
  dbsks_dp_match_sptr dp_engine = new dbsks_dp_match();

  vcl_cout << "Loading arc costs from file: " << new_file_path << "\n";

 
  //////////////////////////////////////////////////
  dp_engine->load_circ_arc_costs(new_file_path);
  vcl_cout << "Loaded arc_grid: \n";
  dp_engine->arc_grid().print(vcl_cout);
  //////////////////////////////////////////////////



  // set the edge map and compute other Oriented Chamfer Distance related params
  // edgemap
  vil_image_view<float > edgemap;
  if (image_resource->nplanes()==1)
  {
    edgemap = *vil_convert_cast(float(), image_resource->get_view());
  }
  else
  {
    edgemap = *vil_convert_cast(float(), 
    vil_convert_to_grey_using_rgb_weighting(image_resource->get_view()));
  }

  
  dp_engine->init_arc_image_cost(edgemap, 
    dp_engine->oriented_chamfer()->edge_threshold_, 
    dp_engine->arc_grid(), 
    dp_engine->ds_shapelet_,
    dp_engine->oriented_chamfer()->sigma_distance_);

  // OUTPUT STORAGE CLASSES ----------------------------------------------------

  // create the output storage class
  dbsks_shapematch_storage_sptr shapematch_storage = dbsks_shapematch_storage_new();
  output_data_[0].push_back(shapematch_storage);
  shapematch_storage->set_dp_engine(dp_engine);

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_load_arc_cost_process::
finish()
{
  return true;
}



