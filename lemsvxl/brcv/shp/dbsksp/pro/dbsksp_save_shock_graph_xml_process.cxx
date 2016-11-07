// This is dbsksp/pro/dbsksp_save_shock_graph_xml_process.cxx

//:
// \file

#include "dbsksp_save_shock_graph_xml_process.h"
#include <bpro1/bpro1_parameters.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>

#include <dbsksp/xio/dbsksp_xio_shock_graph.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

//: Constructor
dbsksp_save_shock_graph_xml_process::
dbsksp_save_shock_graph_xml_process()
{
  // two types of shock graph - intrinsic and extrinsic
  this->shock_graph_types_.push_back("dbsksp_shock_graph");
  this->shock_graph_types_.push_back("dbsksp_xshock_graph");

  if( !parameters()->add( "Output XML file <filename...>" , 
    "-xmlfile", bpro1_filepath("",".xml") ) ||
    !parameters()->add("Type of output shock graph: " , "-shock_graph_type", 
    this->shock_graph_types_, 0) 
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsksp_save_shock_graph_xml_process::
~dbsksp_save_shock_graph_xml_process()
{
  
}


//: Clone the process
bpro1_process* dbsksp_save_shock_graph_xml_process::
clone() const
{
  return new dbsksp_save_shock_graph_xml_process(*this);
}

//: Returns the name of this process
vcl_string dbsksp_save_shock_graph_xml_process::
name()
{ 
  return "Save shock graph XML"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_save_shock_graph_xml_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;

  unsigned int shock_graph_choice = 0;
  this->parameters()->get_value( "-shock_graph_type" , shock_graph_choice);
  vcl_string shock_graph_type = this->shock_graph_types_[shock_graph_choice];

  if (shock_graph_type == "dbsksp_shock_graph")
  {
    to_return.push_back( "dbsksp_shock" );
  }
  else if (shock_graph_type == "dbsksp_xshock_graph")
  {
    to_return.push_back("dbsksp_xgraph");
  }
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_save_shock_graph_xml_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_save_shock_graph_xml_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsksp_save_shock_graph_xml_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbsksp_save_shock_graph_xml_process::
execute()
{
  if ( this->input_data_.size() != 1 ){
    vcl_cerr << "ERROR: executing " __FILE__ "not exactly one input frame.\n";
    return false;
  }


  // parse process parameters
  unsigned int shock_graph_choice = 0;
  this->parameters()->get_value( "-shock_graph_type" , shock_graph_choice);
  vcl_string shock_graph_type = this->shock_graph_types_[shock_graph_choice];

  bpro1_filepath xmlfile;
  parameters()->get_value( "-xmlfile" , xmlfile );

  
  // process input data
  if (shock_graph_type == "dbsksp_shock_graph")
  {
    // retrieve storage class
    dbsksp_shock_storage_sptr shock_storage;
    shock_storage.vertical_cast(input_data_[0][0]);

    if (shock_storage->shock_graph() && x_write(xmlfile.path, shock_storage->shock_graph()))
    {
      vcl_cout << "Saving shock graph XML file succeeded.\n";
    }
    else
    {
      vcl_cout << "Saving shock graph XML file failed.\n";
    }
  }
  else if (shock_graph_type == "dbsksp_xshock_graph")
  {
    // retrieve storage class
    dbsksp_xgraph_storage_sptr shock_storage;
    shock_storage.vertical_cast(input_data_[0][0]);

    if (shock_storage->xgraph() && x_write(xmlfile.path, shock_storage->xgraph()))
    {
      vcl_cout << "Saving xshock graph XML file succeeded.\n";
    }
    else
    {
      vcl_cout << "Saving xshock graph XML file failed.\n";
    };
  }
  this->clear_output();
  return true;
}

bool dbsksp_save_shock_graph_xml_process::
finish()
{
  return true;
}





