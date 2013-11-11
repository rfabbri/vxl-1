// This is dbsksp/pro/dbsksp_convert_shock_graph_to_xshock_graph_process.cxx

//:
// \file

#include "dbsksp_convert_shock_graph_to_xshock_graph_process.h"
#include <dbsksp/algo/dbsksp_convert.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <bpro1/bpro1_parameters.h>

// ----------------------------------------------------------------------------
//: Constructor
dbsksp_convert_shock_graph_to_xshock_graph_process::
dbsksp_convert_shock_graph_to_xshock_graph_process()
{
  if( false
    //!parameters()->add("Build type: " , "-build_type" , this->build_type_descriptions_, 0)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_convert_shock_graph_to_xshock_graph_process::
~dbsksp_convert_shock_graph_to_xshock_graph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_convert_shock_graph_to_xshock_graph_process::
clone() const
{
  return new dbsksp_convert_shock_graph_to_xshock_graph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_convert_shock_graph_to_xshock_graph_process::
name()
{ 
  return "Convert shock graph to xshock graph"; 
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_convert_shock_graph_to_xshock_graph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_convert_shock_graph_to_xshock_graph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_xgraph" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_convert_shock_graph_to_xshock_graph_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_convert_shock_graph_to_xshock_graph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_convert_shock_graph_to_xshock_graph_process::
execute()
{
  // input shock
  dbsksp_shock_storage_sptr input_shock;
  input_shock.vertical_cast(input_data_[0][0]);

  // place holder for output graph
  dbsksp_xgraph_storage_sptr output_shock = dbsksp_xgraph_storage_new();
  output_data_[0].push_back(output_shock);


  // Conversion
  
  dbsksp_xshock_graph_sptr xshock_graph = dbsksp_convert_to_xshock_graph(input_shock->shock_graph());
  if (xshock_graph)
  {
    output_shock->set_xgraph(xshock_graph);
    vcl_cout << "\nConverting dbsksp_shock_graph to dbsksp_xshock_graph ... [ OK ]\n";
    return true;
  }
  else
  {
    vcl_cout << "\nConverting dbsksp_shock_graph to dbsksp_xshock_graph ... [ Failed ]\n";
    return false;
  }
}

bool dbsksp_convert_shock_graph_to_xshock_graph_process::
finish()
{
  return true;
}





