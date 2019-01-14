// This is dbsk2d/pro/dbsk2d_save_esf_process.cxx

//:
// \file

#include "dbsk2d_save_esf_process.h"
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <iostream>
#include <fstream>

dbsk2d_save_esf_process::dbsk2d_save_esf_process() : bpro1_process()
{
  if( !parameters()->add(  "Output file <filename...>" , 
                           "-esfoutput" ,
                           bpro1_filepath("","*.esf") 
                         ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}

//: Clone the process
bpro1_process*
dbsk2d_save_esf_process::clone() const
{
  return new dbsk2d_save_esf_process(*this);
}

std::vector< std::string > dbsk2d_save_esf_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

std::vector< std::string > dbsk2d_save_esf_process::get_output_type()
{
  std::vector< std::string > to_return;
  to_return.clear();
  return to_return;
}

bool dbsk2d_save_esf_process::execute()
{
  bpro1_filepath output_path;

  parameters()->get_value( "-esfoutput" , output_path);
  std::string output_file = output_path.path;

  return save_extrinsic_shock_graph(output_file);
}

bool dbsk2d_save_esf_process::save_extrinsic_shock_graph (std::string filename)
{
  // get input storage class
  dbsk2d_shock_storage_sptr input_shock;
  input_shock.vertical_cast(input_data_[0][0]);

  //the esf file I/O class
  dbsk2d_xshock_graph_fileio file_io;

  return file_io.save_xshock_graph(input_shock->get_shock_graph(), filename);
}
