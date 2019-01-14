// This is dbsksp/pro/dbsksp_save_xgraph_xml_process.cxx

//:
// \file

#include "dbsksp_save_xgraph_xml_process.h"
#include <bpro1/bpro1_parameters.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

//: Constructor
dbsksp_save_xgraph_xml_process::
dbsksp_save_xgraph_xml_process()
{
  if( !parameters()->add( "Output XML file <filename...>" , 
    "-xmlfile", bpro1_filepath("",".xml") )
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbsksp_save_xgraph_xml_process::
~dbsksp_save_xgraph_xml_process()
{
  
}


//: Clone the process
bpro1_process* dbsksp_save_xgraph_xml_process::
clone() const
{
  return new dbsksp_save_xgraph_xml_process(*this);
}

//: Returns the name of this process
std::string dbsksp_save_xgraph_xml_process::
name()
{ 
  return "Save xgraph XML"; 
}

//: Provide a vector of required input types
std::vector< std::string > dbsksp_save_xgraph_xml_process::
get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbsksp_save_xgraph_xml_process::
get_output_type()
{
  std::vector<std::string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_save_xgraph_xml_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsksp_save_xgraph_xml_process::
output_frames()
{
  return 0;
}

//: Execute this process
bool dbsksp_save_xgraph_xml_process::
execute()
{
  if ( this->input_data_.size() != 1 ){
    std::cerr << "ERROR: executing " __FILE__ "not exactly one input frame.\n";
    return false;
  }


  bpro1_filepath xmlfile;
  parameters()->get_value( "-xmlfile" , xmlfile );

  // retrieve storage class
  dbsksp_xgraph_storage_sptr shock_storage;
  shock_storage.vertical_cast(input_data_[0][0]);

  // save to file
  std::cout << "\nSaving xgraph to file: " << xmlfile.path << "...";
  if (shock_storage->xgraph() && x_write(xmlfile.path, shock_storage->xgraph()))
  {
    std::cout << "[ OK ].\n";
  }
  else
  {
    std::cout << "[ Failed ]\n";
  };
  
  this->clear_output();
  return true;
}

bool dbsksp_save_xgraph_xml_process::
finish()
{
  return true;
}





