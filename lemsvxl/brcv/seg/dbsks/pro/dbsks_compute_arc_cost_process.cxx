// This is dbsks/pro/dbsks_compute_arc_cost_process.cxx

//:
// \file

#include "dbsks_compute_arc_cost_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>

#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_dp_match_sptr.h>
#include <dbsks/dbsks_ocm_image_cost.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vnl/vnl_math.h>




//: Constructor
dbsks_compute_arc_cost_process::
dbsks_compute_arc_cost_process()
{

  if ( !parameters()->add("edge strength threshold: " , "edge_threshold", 63.0f) ||
    !parameters()->add("chamfer_sigma: " , "chamfer_sigma", 8.0f) ||
    !parameters()->add("orient_sigma: " , "angle_sigma", 0.5f) ||
    !parameters()->add("arc sampling rate, ds = " , "ds", 3.0f) ||
    
    !parameters()->add("Use image center?: " , "use_image_center", true ) ||
    !parameters()->add("center_x: " , "center_x", 256.0f ) ||
    !parameters()->add("center_y: " , "center_y", 256.0f ) ||
    
    !parameters()->add("step_x: " , "step_x", 8.0f ) ||
    !parameters()->add("step_y: " , "step_y", 8.0f ) ||
    
    !parameters()->add("Use image size?: " , "use_image_size", true ) ||
    !parameters()->add("half_num_x: " , "half_num_x", int(32) ) ||
    !parameters()->add("half_num_y: " , "half_num_y", int(32) ) ||
    
    !parameters()->add("num_theta: " , "num_theta", int(16) ) ||
    
    !parameters()->add("min_chord: " , "min_chord", 8.0f ) ||
    !parameters()->add("step_chord: " , "step_chord", 8.0f ) ||
    !parameters()->add("num_chord: " , "num_chord", int(20) ) ||
    
    !parameters()->add("step_height: " , "step_height", 4.0f ) ||
    !parameters()->add("half_num_height: " , "half_num_height", int(16) ) ||
    !parameters()->add("Initialize only?: " , "initialize_only", false )
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_compute_arc_cost_process::
~dbsks_compute_arc_cost_process()
{
}


//: Clone the process
bpro1_process* dbsks_compute_arc_cost_process::
clone() const
{
  return new dbsks_compute_arc_cost_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_compute_arc_cost_process::
name()
{ 
  return "Compute arc cost"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_compute_arc_cost_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_compute_arc_cost_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("dbsks_shapematch");
  to_return.push_back( "image" );
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_compute_arc_cost_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_compute_arc_cost_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_compute_arc_cost_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_compute_arc_cost_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  
  // USER PARAMS --------------------------------------------------------------
  
  //
  dbsks_circ_arc_grid_params params;
    
  float edge_threshold = 25.0f;
  parameters()->get_value( "edge_threshold", edge_threshold);


  float chamfer_sigma = 4.0f;
  parameters()->get_value( "chamfer_sigma", chamfer_sigma);

  float angle_sigma = 0.5f;
  parameters()->get_value( "angle_sigma", angle_sigma);

  float ds = 3.0f;
  parameters()->get_value( "ds", ds);

  bool use_image_center = false;
  parameters()->get_value( "use_image_center", use_image_center);

  parameters()->get_value( "center_x", params.center_x);
  parameters()->get_value( "center_y", params.center_y);
  
  parameters()->get_value( "step_x", params.step_x);
  parameters()->get_value( "step_y", params.step_y);

  bool use_image_size = false;
  parameters()->get_value( "use_image_size", use_image_size);

  parameters()->get_value( "half_num_x", params.half_num_x);
  parameters()->get_value( "half_num_y", params.half_num_y);
  parameters()->get_value( "num_theta", params.num_theta);
  parameters()->get_value( "min_chord", params.min_chord);
  parameters()->get_value( "step_chord", params.step_chord);
  parameters()->get_value( "num_chord", params.num_chord);
  parameters()->get_value( "step_height", params.step_height);
  parameters()->get_value( "half_num_height", params.half_num_height);
  
  assert(chamfer_sigma >0);
  assert(ds >0);

  bool initialize_only = false;
  parameters()->get_value( "initialize_only", initialize_only);


  

  // STORAGE CLASSES ----------------------------------------------------------
  
  // get the shock graph from storage classes
  // image
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][0]);


  // PROCESS DATA -------------------------------------------------------------

  

  
  // >> Compute the potential image
  // retrieve the edgemap
  vil_image_resource_sptr image_resource = image_storage->get_image();


  

  vil_image_view<float > image_view;
  if (image_resource->nplanes()==1)
  {
    image_view = *vil_convert_cast(float(), image_resource->get_view());
  }
  else
  {
    image_view = *vil_convert_cast(float(), 
    vil_convert_to_grey_using_rgb_weighting(image_resource->get_view()));
  }

  // adjust the grid center to the center of the image, if requested
  if (use_image_center)
  {
    params.center_x = float(image_resource->ni() / 2);
    params.center_y = float(image_resource->nj() / 2);
  }

  if (use_image_size)
  {
    params.half_num_x = vnl_math::rnd(image_resource->ni()/(2*params.step_x)) + 1;
    params.half_num_y = vnl_math::rnd(image_resource->nj()/(2*params.step_y)) + 1;

    // limite the grid size to 36 x 36
    if (params.half_num_x > 36)
      params.half_num_x = 36;

    if (params.half_num_y > 36)
      params.half_num_x = 36;
  }

  
  // Construct a circular arc grid
  dbsks_circ_arc_grid arc_grid;
  arc_grid.set_grid(params);
  
  // >> Set up the shape detector
  dbsks_dp_match_sptr dp_engine = new dbsks_dp_match();

  
  ///////////////////////////////////////////////////////////////////////////////
  
  // Initialize all necessary variables
  dp_engine->init_arc_image_cost(image_view, edge_threshold, arc_grid, ds, 
    chamfer_sigma, angle_sigma);

  // Determine tolerance near zero for chamfer matching
  // Choose: tol = 1/2 grid_step
  //float tol = (params.step_x + params.step_y) / 4;
  float tol = 0;

  dp_engine->oriented_chamfer()->set_tolerance_near_zero(tol);
  
  // Carry out the computation
  if (!initialize_only)
  {
    dp_engine->compute_arc_image_cost();
  }
  ///////////////////////////////////////////////////////////////////////////////

  // create the output storage class
  dbsks_shapematch_storage_sptr shapematch_storage = dbsks_shapematch_storage_new();
  output_data_[0].push_back(shapematch_storage);
  shapematch_storage->set_dp_engine(dp_engine);


  vidpro1_image_storage_sptr out_image_storage = vidpro1_image_storage_new();
  output_data_[0].push_back(out_image_storage);
  
  out_image_storage->set_image(
    vil_new_image_resource_of_view (dp_engine->oriented_chamfer()->compute_chamfer_cost()));

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_compute_arc_cost_process::
finish()
{
  return true;
}







