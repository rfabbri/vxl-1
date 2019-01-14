// This is dbsksp/pro/dbsksp_load_xgraph_xml_process.cxx

//:
// \file

#include "dbsksp_load_xgraph_xml_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/pro/dbsksp_xgraph_storage_sptr.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

//: Constructor
dbsksp_load_xgraph_xml_process::
dbsksp_load_xgraph_xml_process()
{
  if( !parameters()->add( "Input XML file <filename...>" , "-xmlfile", 
    bpro1_filepath("",".xml")) )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbsksp_load_xgraph_xml_process::
~dbsksp_load_xgraph_xml_process()
{
  
}


//: Clone the process
bpro1_process* dbsksp_load_xgraph_xml_process::
clone() const
{
  return new dbsksp_load_xgraph_xml_process(*this);
}

//: Returns the name of this process
std::string dbsksp_load_xgraph_xml_process::
name()
{ 
  return "Load xgraph XML"; 
}

//: Provide a vector of required input types
std::vector< std::string > dbsksp_load_xgraph_xml_process::
get_input_type()
{
  std::vector< std::string > to_return;
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbsksp_load_xgraph_xml_process::
get_output_type()
{
  std::vector<std::string > to_return;
  to_return.push_back("dbsksp_xgraph");
  return to_return;
}

//: Return the number of input frames for this process
int dbsksp_load_xgraph_xml_process::
input_frames()
{
  return 0;
}


//: Return the number of output frames for this process
int dbsksp_load_xgraph_xml_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsksp_load_xgraph_xml_process::
execute()
{
  // 1. parse process parameters
  
  // filename
  bpro1_filepath xmlfile;
  parameters()->get_value( "-xmlfile" , xmlfile );
    
  

  // 3. process
  dbsksp_xshock_graph_sptr xg = 0;
  if ( x_read(xmlfile.path, xg) )
  {
    std::cout << "Loading xshock_graph XML file completed.\n";
    dbsksp_xgraph_storage_sptr shock_storage = dbsksp_xgraph_storage_new();
    shock_storage->set_xgraph(xg);
    this->output_data_[0].push_back(shock_storage);
  }
  else
  {
    std::cerr << "Loading xshock_graph XML file failed.\n";
    xg = 0;
  }
  
  return true;
}

bool dbsksp_load_xgraph_xml_process::
finish()
{
  return true;
}





