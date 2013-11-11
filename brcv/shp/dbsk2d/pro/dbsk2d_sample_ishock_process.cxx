// This is brcv/shp/dbsk2d/pro/dbsk2d_sample_ishock_process.cxx

//:
// \file

#include "dbsk2d_sample_ishock_process.h"
#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"

#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

//: Constructor
dbsk2d_sample_ishock_process::dbsk2d_sample_ishock_process()
{
  if (!parameters()->add( "Sampling resolution" , "-res" , (float)1.0 ) ||
      !parameters()->add( "inside?" , "-inside" , (bool) true ) ||
      !parameters()->add( "both sides" , "-both" , (bool) false ) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbsk2d_sample_ishock_process::~dbsk2d_sample_ishock_process()
{
}

//: Clone the process
bpro1_process*
dbsk2d_sample_ishock_process::clone() const
{
  return new dbsk2d_sample_ishock_process(*this);
}

vcl_string
dbsk2d_sample_ishock_process::name()
{
  return "Sample Shocks";
}

vcl_vector< vcl_string >
dbsk2d_sample_ishock_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string >
dbsk2d_sample_ishock_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

int dbsk2d_sample_ishock_process::input_frames()
{
  return 1;
}

int dbsk2d_sample_ishock_process::output_frames()
{
  return 1;
}

bool dbsk2d_sample_ishock_process::execute()
{
  float sampling_resoluton=0;

  parameters()->get_value( "-res" , sampling_resoluton);

  bool inside, both;
  parameters()->get_value( "-inside" , inside);
  parameters()->get_value( "-both" , both);

  // get input storage class
  dbsk2d_shock_storage_sptr input_shock;
  input_shock.vertical_cast(input_data_[0][0]);

  //sample the intrinsic shock graph into an extrinsic shock graph
  dbsk2d_sample_ishock ishock_sampler(input_shock->get_shock_graph());
  if (both)
    ishock_sampler.sample(sampling_resoluton, BOTHSIDE);
  else if (inside)
    ishock_sampler.sample(sampling_resoluton, INSIDE);
  else
    ishock_sampler.sample(sampling_resoluton, OUTSIDE);

  // create the output storage class
  dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
  output_shock->set_image(input_shock->get_image()); //this is not coshure (just very very temp)
  output_shock->set_boundary(input_shock->get_boundary()); //this is not coshure (just very very temp)
  output_shock->set_ishock_graph(NULL);
  output_shock->set_shock_graph(ishock_sampler.extrinsic_shock_graph());
  output_data_[0].push_back(output_shock);

  return true;
}

bool dbsk2d_sample_ishock_process::finish()
{
  return true;
}


