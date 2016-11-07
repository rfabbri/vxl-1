// This is dbsks/pro/dbsks_save_dp_results_process.cxx

//:
// \file

#include "dbsks_save_dp_results_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <dbsks/dbsks_dp_match.h>

#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

//
//#include <vcl_iostream.h>
//#include <vcl_fstream.h>

//: Constructor
dbsks_save_dp_results_process::
dbsks_save_dp_results_process()
{
  if ( !parameters()->add("Save dp results to file: " , "-dp_file", 
    bpro1_filepath("","*")) ||
    !parameters()->add("Add frame number as prefix: " , "-add_frame_number", false) ||
    !parameters()->add("Save DP results to a text file: " , "-save_dp_to_txt", false) ||
    !parameters()->add("Clear dp memory in storage after saving: " , 
      "-clear_after_save", false)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_save_dp_results_process::
~dbsks_save_dp_results_process()
{
}


//: Clone the process
bpro1_process* dbsks_save_dp_results_process::
clone() const
{
  return new dbsks_save_dp_results_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_save_dp_results_process::
name()
{ 
  return "Save DP results"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_save_dp_results_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_save_dp_results_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_save_dp_results_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_save_dp_results_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_save_dp_results_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_save_dp_results_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------

  bpro1_filepath dp_file;
  parameters()->get_value( "-dp_file", dp_file);

  bool add_frame_number = false;
  parameters()->get_value( "-add_frame_number", add_frame_number);

  bool save_dp_to_txt = false;
  parameters()->get_value( "-save_dp_to_txt", save_dp_to_txt);

  bool clear_after_save = false;
  parameters()->get_value( "-clear_after_save", clear_after_save);

  // STORAGE CLASSES ----------------------------------------------------------
  
  // get the user-input shapematch storage
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(input_data_[0][0]);


  //// PROCESS DATA -------------------------------------------------------------

  vcl_string file_path = dp_file.path;

  vcl_string new_file_path;
  if (add_frame_number == true)
  {
    vcl_string base_name = 
      vul_file::strip_directory(vul_file::strip_extension(file_path));
    vcl_string file_dir = vul_file::dirname(file_path);
    vcl_string file_ext = vul_file::extension(file_path);

    int frame_number = shapematch_storage->frame();
    
    // construct a new filename
    vcl_string new_base_name = vul_sprintf("%03d", frame_number) + "_" +
      base_name;
    new_file_path = file_dir + "\\" + new_base_name + file_ext;
  }
  else
  {
    new_file_path = file_path;
  }

  dbsks_dp_match_sptr dp_engine = shapematch_storage->dp_engine();
  ////////////////////////////////////////////////////////////////////////////
  dp_engine->save_dp_optim_results(new_file_path);
  ////////////////////////////////////////////////////////////////////////////

  // Save a text file of the arc_grid params
  if (save_dp_to_txt)
  {
    vcl_string dp_file = vul_file::strip_extension(new_file_path) + ".txt";
    
    vcl_ofstream os(dp_file.c_str());
    shapematch_storage->dp_engine()->print_summary(os);
    os.close();
  }

  // Clear data inside the cost matrices if requested
  if (clear_after_save)
  {
    shapematch_storage->dp_engine()->clear_all_temp_files();

    // erase all possible memory !!!!
    shapematch_storage->set_dp_engine(new dbsks_dp_match());
  }

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_save_dp_results_process::
finish()
{
  return true;
}



