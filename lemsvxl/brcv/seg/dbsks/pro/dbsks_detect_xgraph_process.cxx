// This is dbsks/pro/dbsks_detect_xgraph_process.cxx

//:
// \file

#include "dbsks_detect_xgraph_process.h"
#include <bpro1/bpro1_parameters.h>



//: Constructor
dbsks_detect_xgraph_process::
dbsks_detect_xgraph_process()
{
  if ( 
    !parameters()->add("Prefix of record-keeping files: ", "tmp_prefix", 
    std::string("D:/vision/data/symseg/temp/tmp") )
    )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbsks_detect_xgraph_process::
~dbsks_detect_xgraph_process()
{
}


//: Clone the process
bpro1_process* dbsks_detect_xgraph_process::
clone() const
{
  return new dbsks_detect_xgraph_process(*this);
}

//: Returns the name of this process
std::string dbsks_detect_xgraph_process::
name()
{ 
  return "Detect xgraph"; 
}

//: Provide a vector of required input types
std::vector< std::string > dbsks_detect_xgraph_process::
get_input_type()
{
  std::vector< std::string > to_return;
  return to_return;
}


//: Provide a vector of output types
std::vector< std::string > dbsks_detect_xgraph_process::
get_output_type()
{
  std::vector<std::string > to_return;
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_detect_xgraph_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_detect_xgraph_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_detect_xgraph_process::
execute()
{
  if ( input_data_.size() != 1 ){
    std::cerr << "In dbsks_detect_xgraph_process::execute() - "
             << "not exactly one input images" << std::endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
    
  // STORAGE CLASSES ----------------------------------------------------------

  // PROCESS DATA -------------------------------------------------------------

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_detect_xgraph_process::
finish()
{
  return true;
}
