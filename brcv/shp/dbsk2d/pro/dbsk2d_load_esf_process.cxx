//This is dbsk2d/pro/dbsk2d_load_esf_process.cxx

//:
// \file

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_cstring.h>
#include <vcl_string.h>

#include "dbsk2d_load_esf_process.h"
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

dbsk2d_load_esf_process::dbsk2d_load_esf_process() : 
  bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-esfinput" , bpro1_filepath("","*.esf") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbsk2d_load_esf_process::clone() const
{
  return new dbsk2d_load_esf_process(*this);
}

vcl_vector< vcl_string > dbsk2d_load_esf_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > dbsk2d_load_esf_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

bool dbsk2d_load_esf_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-esfinput" , input);
  vcl_string input_file_path = input.path;

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    vul_file_iterator fn=input_file_path+"/*.esf";
    for ( ; fn; ++fn) 
    {
      vcl_string input_file = fn();
  
      dbsk2d_shock_storage_sptr new_shock = load_extrinsic_shock_graph(input_file);
      output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_shock));
      num_of_files++;
    }

    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    vcl_string input_file = input_file_path;

    dbsk2d_shock_storage_sptr new_shock = load_extrinsic_shock_graph(input_file);
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_shock));
    num_frames_ = 1;
  }

  return true;
}

//: load the .esf file
dbsk2d_shock_storage_sptr 
dbsk2d_load_esf_process::load_extrinsic_shock_graph (vcl_string filename)
{
  //the esf file I/O class
  dbsk2d_xshock_graph_fileio file_io;

  // create the output storage class
  dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
  output_shock->set_shock_graph(file_io.load_xshock_graph(filename));

  return output_shock;
}
