//This is dbsk2d/pro/dbsk2d_load_bnd_process.cxx

//:
// \file


#include "dbsk2d_load_bnd_process.h"

#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

#include <dbsk2d/dbsk2d_file_io.h>

//: Constructor
dbsk2d_load_bnd_process::dbsk2d_load_bnd_process() : 
  bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Input file <filename...>" , "-bndinput" , bpro1_filepath("","*.bnd") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbsk2d_load_bnd_process::clone() const
{
  return new dbsk2d_load_bnd_process(*this);
}

vcl_vector< vcl_string > dbsk2d_load_bnd_process::get_input_type() 
{
  vcl_vector< vcl_string > to_return;
  return to_return;
}

vcl_vector< vcl_string > dbsk2d_load_bnd_process::get_output_type() 
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool dbsk2d_load_bnd_process::execute()
{
  bpro1_filepath input;
  parameters()->get_value( "-bndinput" , input);
  vcl_string input_file_path = input.path;

  int num_of_files = 0;

  output_data_.clear();

  // make sure that input_file_path is sane
  if (input_file_path == "") { return false; }

  // test if fname is a directory
  if (vul_file::is_directory(input_file_path))
  {
    vul_file_iterator fn=input_file_path+"/*.bnd";
    for ( ; fn; ++fn) 
    {
      vcl_string input_file = fn();
  
      vidpro1_vsol2D_storage_sptr new_bnd = loadBND(input_file);
      output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_bnd));
      num_of_files++;
    }

    //this is the number of frames to be outputted
    num_frames_ = num_of_files;
  }
  else {
    vcl_string input_file = input_file_path;

    vidpro1_vsol2D_storage_sptr new_bnd = loadBND(input_file);
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,new_bnd));
    num_frames_ = 1;
  }

  return true;
}

//: \todo finish the add-arc portion (vsol_2d doesn't have a good arc class)
vidpro1_vsol2D_storage_sptr dbsk2d_load_bnd_process::loadBND (vcl_string filename)
{
  // new vector to store the contours
  vcl_vector< vsol_spatial_object_2d_sptr > geoms;

  dbsk2d_file_io::load_bnd_v3_0(filename, geoms);
  vcl_cout << "Loaded: " << filename.c_str() << ".\n";
    
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(geoms, filename);
  //vcl_cout << geoms.size() << " vsol2D objects loaded.\n";

  return output_vsol;
}
