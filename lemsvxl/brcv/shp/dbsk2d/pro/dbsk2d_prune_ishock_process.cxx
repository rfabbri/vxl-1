// This is brcv/shp/dbsk2d/pro/dbsk2d_prune_ishock_process.cxx

//:
// \file

#include "dbsk2d_prune_ishock_process.h"
#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"

#include <dbsk2d/algo/dbsk2d_prune_ishock.h>

dbsk2d_prune_ishock_process::dbsk2d_prune_ishock_process()
{
  if (!parameters()->add( "Prune Threshold" , "-threshold" , (float)1.0 ) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

dbsk2d_prune_ishock_process::~dbsk2d_prune_ishock_process()
{
}


//: Clone the process
bpro1_process* dbsk2d_prune_ishock_process::clone() const
{
  return new dbsk2d_prune_ishock_process(*this);
}


vcl_string dbsk2d_prune_ishock_process::name()
{
  return "Prune Shocks";
}

vcl_vector< vcl_string > dbsk2d_prune_ishock_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

vcl_vector< vcl_string > dbsk2d_prune_ishock_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

int
dbsk2d_prune_ishock_process::input_frames()
{
  return 1;
}

int
dbsk2d_prune_ishock_process::output_frames()
{
  return 0;
}


bool
dbsk2d_prune_ishock_process::execute()
{
  float prune_threshold=0;

  parameters()->get_value( "-threshold" , prune_threshold);

  // get input storage class
  dbsk2d_shock_storage_sptr shock;
  shock.vertical_cast(input_data_[0][0]);

  //prune this shock graph and output a coarse shock graph 
  //corresponding to the remaining shock edges
  dbsk2d_prune_ishock ishock_pruner(shock->get_ishock_graph(), shock->get_shock_graph());
  ishock_pruner.prune(prune_threshold);
  ishock_pruner.compile_coarse_shock_graph();

  return true;
}

bool
dbsk2d_prune_ishock_process::finish()
{
  return true;
}


