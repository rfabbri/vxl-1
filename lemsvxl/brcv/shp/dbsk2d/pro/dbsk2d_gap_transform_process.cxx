// This is brcv/shp/dbsk2d/pro/dbsk2d_gap_transform_process.cxx

//:
// \file

#include "dbsk2d_gap_transform_process.h"
#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbsk2d/algo/dbsk2d_shock_transforms.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>

dbsk2d_gap_transform_process::dbsk2d_gap_transform_process()
{
  if (!parameters()->add( "Threshold for contour cost" , "-cont_threshold" , (float)0.5f ) ||
      !parameters()->add( "Threshold for appearance cost" , "-app_threshold" , (float)0.5f ) ||
      !parameters()->add( "alpha contour" , "-alpha_cont" , (float)1.0f ) ||
      !parameters()->add( "alpha appearance" , "-alpha_app" , (float)1.0f ) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

dbsk2d_gap_transform_process::~dbsk2d_gap_transform_process()
{
}


//: Clone the process
bpro1_process* dbsk2d_gap_transform_process::clone() const
{
  return new dbsk2d_gap_transform_process(*this);
}


vcl_string dbsk2d_gap_transform_process::name()
{
  return "Perform Gap Transforms";
}

vcl_vector< vcl_string > dbsk2d_gap_transform_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbsk2d_gap_transform_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  to_return.push_back( "vsol2D" );
  return to_return;
}

int
dbsk2d_gap_transform_process::input_frames()
{
  return 1;
}

int
dbsk2d_gap_transform_process::output_frames()
{
  return 1;
}


bool
dbsk2d_gap_transform_process::execute()
{
  float cont_t = 0, app_t = 0;
  float alpha_cont = 0, alpha_app = 0;

  parameters()->get_value( "-cont_threshold" , cont_t);
  parameters()->get_value( "-app_threshold" , app_t);
  parameters()->get_value( "-alpha_cont" , alpha_cont);
  parameters()->get_value( "-alpha_app" , alpha_app);

  // get input storage class
  dbsk2d_shock_storage_sptr shock;
  shock.vertical_cast(input_data_[0][0]);

  //prune this shock graph and output a coarse shock graph 
  //corresponding to the remaining shock edges
  // coarse shock graph is being modified so it needs to be recreated, but intrinsic shock graph is never modified
  // dbsk2d_shock_graph_sptr sg = new dbsk2d_shock_graph(*(shock->get_shock_graph()));
  // dbsk2d_ishock_graph_sptr isg = new dbsk2d_ishock_graph(*(shock->get_ishock_graph()));
  dbsk2d_shock_transforms transformer(shock->get_ishock_graph(),shock->get_shock_graph());
  transformer.set_image(shock->get_image());

  
  vcl_vector< vsol_spatial_object_2d_sptr > contours;
  transformer.perform_all_gap_transforms(cont_t, app_t, alpha_cont, alpha_app, true);
  transformer.get_eulerspirals(contours);
  transformer.clear_eulerspirals();

  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(contours, "spirals");

  // create the output storage class
  // dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
  // output_shock->set_boundary(shock->get_boundary()); //this is not coshure (just very very temp)
  // output_shock->set_ishock_graph(isg);
  // output_shock->set_shock_graph(sg);
  // output_shock->set_image(shock->get_image());
  output_data_[0].push_back(shock);
  output_data_[0].push_back(output_vsol);

  return true;
}

bool
dbsk2d_gap_transform_process::finish()
{
  return true;
}


