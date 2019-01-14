//This is brcv/seg/dbdet/pro/dbdet_save_cem_process.cxx

#include <iostream>
#include <cassert>
#include <fstream>
#include <cmath>

#include "dbdet_save_cem_process.h"

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>

#include <dbdet/algo/dbdet_cem_file_io.h>

dbdet_save_cem_process::dbdet_save_cem_process() : bpro1_process()
{
  if( !parameters()->add( "Output file <filename...>" , "-cem_filename" , bpro1_filepath("","*.cem")))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}

std::string dbdet_save_cem_process::name() 
{
  return "Save .CEM File";
}

std::vector< std::string > dbdet_save_cem_process::get_input_type() 
{
  std::vector< std::string > to_return;
  to_return.push_back( "sel");
  return to_return;
}

std::vector< std::string > dbdet_save_cem_process::get_output_type() 
{
  std::vector< std::string > to_return;
  return to_return;
}

//: Clone the process
bpro1_process*
dbdet_save_cem_process::clone() const
{
  return new dbdet_save_cem_process(*this);
}


bool dbdet_save_cem_process::execute()
{  
  bpro1_filepath cem_filename;
  
  //get the input storage class
  dbdet_sel_storage_sptr input_sel;
  input_sel.vertical_cast(input_data_[0][0]);

  parameters()->get_value( "-cem_filename", cem_filename);

  //save the contour fragment graph to the file
  bool retval = dbdet_save_cem(cem_filename.path, input_sel->EM(), input_sel->CFG());

  if (!retval) {
    std::cerr << "Error while saving file: " << cem_filename.path << std::endl;
    return false;
  }
  return true;
}
