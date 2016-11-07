// This is rec/dbskr/pro/dbskr_load_shock_patch_process.cxx

//:
// \file

#include "dbskr_load_shock_patch_process.h"
#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>


// -----------------------------------------------------------------------------
//: Constructor
dbskr_load_shock_patch_process::dbskr_load_shock_patch_process() : bpro1_process()
{
  if( !parameters()->add( "Shock patch storage file <*-strg.bin>" , "-filepath", 
    bpro1_filepath("","-strg.bin") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}



// -----------------------------------------------------------------------------
//: Destructor
dbskr_load_shock_patch_process::
~dbskr_load_shock_patch_process()
{
}


// -----------------------------------------------------------------------------
//: Clone the process
bpro1_process*
dbskr_load_shock_patch_process::clone() const
{
  return new dbskr_load_shock_patch_process(*this);
}


// -----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbskr_load_shock_patch_process::name()
{
  return "Load Shock Patch Storage";
}



// -----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbskr_load_shock_patch_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();
  return to_return;
}

// -----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbskr_load_shock_patch_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "shock_patch" );

  return to_return;
}



// -----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbskr_load_shock_patch_process::
input_frames()
{
  return 1;
}


// -----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbskr_load_shock_patch_process::
output_frames()
{
  return 1;
}




// -----------------------------------------------------------------------------
//: Run the process on the current frame
bool
dbskr_load_shock_patch_process::execute()
{
  //>> Process parameters
  bpro1_filepath filepath;
  parameters()->get_value( "-filepath" , filepath );

  //no input storage class for this process
  clear_output();

  //>> Load shock patch storage from file
  dbskr_shock_patch_storage_sptr shock_patch_storage = dbskr_shock_patch_storage_new();
  vsl_b_ifstream ifs(filepath.path.c_str());
  shock_patch_storage->b_read(ifs);
  ifs.close();

  //>> Save to output storage
  output_data_[0].push_back(shock_patch_storage);
  return true;
}


// -----------------------------------------------------------------------------
//: Finish
bool dbskr_load_shock_patch_process::
finish() 
{
  return true;
}


