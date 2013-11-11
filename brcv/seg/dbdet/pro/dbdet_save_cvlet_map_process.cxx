//This is brcv/seg/dbdet/pro/dbdet_save_cvlet_map_process.cxx

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_fstream.h>
#include <vcl_cmath.h>

#include "dbdet_save_cvlet_map_process.h"

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

#include <dbdet/algo/dbdet_cvlet_map_io.h>

dbdet_save_cvlet_map_process::dbdet_save_cvlet_map_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-cvlet_output" , bpro1_filepath("","*.cvlet")))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

vcl_string dbdet_save_cvlet_map_process::name() 
{
  return "Save .CVLET File";
}

vcl_vector< vcl_string > dbdet_save_cvlet_map_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "sel");
  return to_return;
}

vcl_vector< vcl_string > dbdet_save_cvlet_map_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_save_cvlet_map_process::clone() const
{
  return new dbdet_save_cvlet_map_process(*this);
}


bool dbdet_save_cvlet_map_process::execute()
{  
  bpro1_filepath output;
  
  //get the input storage class
  dbdet_sel_storage_sptr input_sel;
  input_sel.vertical_cast(input_data_[0][0]);

  dbdet_curvelet_map& cvlet_map = input_sel->CM();

  parameters()->get_value( "-cvlet_output" , output );
  vcl_string output_file = output.path;

  //save this curvelet map onto the file
  bool retval = dbdet_save_cvlet_map(output_file, cvlet_map);

  return retval;
}
