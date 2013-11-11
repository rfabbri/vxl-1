// This is rec/dbskr/pro/dbskr_save_shock_patch_process.cxx

//:
// \file

#include "dbskr_save_shock_patch_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_patch_storage.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>

// -----------------------------------------------------------------------------
//: Constructor
dbskr_save_shock_patch_process::
dbskr_save_shock_patch_process() : bpro1_process()
{
  if( !parameters()->add( "Filename (-patch_strg.bin)" , "-filepath", bpro1_filepath("","*.bin") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}



// -----------------------------------------------------------------------------
//: Destructor
dbskr_save_shock_patch_process::
~dbskr_save_shock_patch_process()
{
}



// -----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbskr_save_shock_patch_process::
clone() const
{
  return new dbskr_save_shock_patch_process(*this);
}



// -----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbskr_save_shock_patch_process::
name()
{
  return "Save Shock Patch Storage";
}



// -----------------------------------------------------------------------------
//: Call the parent function and reset num_frames_
void dbskr_save_shock_patch_process::
clear_output()
{
  bpro1_process::clear_output();
}



// -----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbskr_save_shock_patch_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock_patch" );
  return to_return;
}



// -----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbskr_save_shock_patch_process::
get_output_type()
{
  return vcl_vector< vcl_string >();
}



// -----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbskr_save_shock_patch_process::
input_frames()
{
  return 1;
}



// -----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbskr_save_shock_patch_process::
output_frames()
{
  return 0;
}




// -----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbskr_save_shock_patch_process::execute()
{
  if ( input_data_.size() != 1 )
  {
    vcl_cout << "In dbskr_save_shock_patch_process::execute() - "
             << "not exactly one input frame.\n";
    return false;
  }
  clear_output();

  //>> Process parametrs
  bpro1_filepath filepath;
  parameters()->get_value( "-filepath" , filepath );
  
  //>> Retrieve shock patch storage
  dbskr_shock_patch_storage_sptr shock_patch_storage;
  shock_patch_storage.vertical_cast(input_data_[0][0]);


  //>> Save shock patch storage
  vsl_b_ofstream bfs(filepath.path.c_str());
  shock_patch_storage->b_write(bfs);
  bfs.close();

  vcl_cout << "\nFinished writing shock patch storage to file " << filepath.path << vcl_endl;
  return true;
}




// -----------------------------------------------------------------------------
//: Finish
bool dbskr_save_shock_patch_process::
finish()
{
  return true;
}


