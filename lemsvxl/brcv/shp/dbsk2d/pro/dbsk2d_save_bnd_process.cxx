// This is dbsk2d/pro/dbsk2d_save_bnd_process.cxx

//:
// \file

#include "dbsk2d_save_bnd_process.h"

#include <dbsk2d/dbsk2d_file_io.h>

dbsk2d_save_bnd_process::dbsk2d_save_bnd_process() : bpro1_process()
{
  if( !parameters()->add(  "Output file <filename...>" , 
                           "-bndoutput" ,
                           bpro1_filepath("","*.bnd") 
                         ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}

//: Clone the process
bpro1_process*
dbsk2d_save_bnd_process::clone() const
{
  return new dbsk2d_save_bnd_process(*this);
}

std::vector< std::string > dbsk2d_save_bnd_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

std::vector< std::string > dbsk2d_save_bnd_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.clear();
  return to_return;
}

bool dbsk2d_save_bnd_process::execute()
{
  bpro1_filepath output_path;

  parameters()->get_value( "-bndoutput" , output_path);
  std::string output_file = output_path.path;

  return saveBND(output_file);
}

bool dbsk2d_save_bnd_process::saveBND (std::string filename)
{
  // get input storage class
  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);
  std::vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
  if (dbsk2d_file_io::save_bnd_v3_0(filename, vsol_list))
  {
    std::cout << "File " << filename << " saved.\n";
    return true;
  }
  else
    return false;
}
