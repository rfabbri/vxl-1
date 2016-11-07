// This is dbsks/pro/dbsks_save_arc_cost_process.cxx

//:
// \file

#include "dbsks_save_arc_cost_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>
#include <dbsks/dbsks_dp_match.h>

#include <vul/vul_file.h>
#include <vul/vul_sprintf.h>

//
//#include <vcl_iostream.h>
//#include <vcl_fstream.h>

//: Constructor
dbsks_save_arc_cost_process::
dbsks_save_arc_cost_process()
{
  if ( !parameters()->add("Save arc costs to file: " , "-arc_cost_file", 
    bpro1_filepath("","*")) ||
    !parameters()->add("Add frame number as prefix: " , "-add_frame_number", false) ||
    !parameters()->add("Save arc grid description to a text file: " , "-save_grid_to_txt", false) ||
    !parameters()->add("Clear arc cost in storage after saving: " , 
      "-clear_after_save", false)
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_save_arc_cost_process::
~dbsks_save_arc_cost_process()
{
}


//: Clone the process
bpro1_process* dbsks_save_arc_cost_process::
clone() const
{
  return new dbsks_save_arc_cost_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_save_arc_cost_process::
name()
{ 
  return "Save arc cost"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_save_arc_cost_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_save_arc_cost_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.clear();
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_save_arc_cost_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_save_arc_cost_process::
output_frames()
{
  return 1;
}

//: Execute this process
bool dbsks_save_arc_cost_process::
execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cerr << "In dbsks_save_arc_cost_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
  bpro1_filepath arc_cost_file;
  parameters()->get_value( "-arc_cost_file", arc_cost_file);

  bool add_frame_number = false;
  parameters()->get_value( "-add_frame_number", add_frame_number);

  bool save_grid_to_txt = false;
  parameters()->get_value( "-save_grid_to_txt", save_grid_to_txt);

  bool clear_after_save = false;
  parameters()->get_value( "-clear_after_save", clear_after_save);

  // STORAGE CLASSES ----------------------------------------------------------
  
  // get the user-input shapematch storage
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(input_data_[0][0]);


  // PROCESS DATA -------------------------------------------------------------

  vcl_string file_path = arc_cost_file.path;
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

    
  shapematch_storage->dp_engine()->save_circ_arc_costs(new_file_path);
  

  // Save a text file of the arc_grid params
  if (save_grid_to_txt)
  {
    vcl_string arc_grid_file = vul_file::strip_extension(new_file_path) + ".txt";
    vcl_ofstream os(arc_grid_file.c_str());
    shapematch_storage->dp_engine()->arc_grid().print(os);
    os.close();
  }

  // Clear data inside the cost matrices if requested
  if (clear_after_save)
  {
    shapematch_storage->dp_engine()->clear_arc_cost_data();
  }

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_save_arc_cost_process::
finish()
{
  return true;
}



