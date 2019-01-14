// This is ozge/face_gui/pro/dbru_save_facedb_process.cxx

//:
// \file

#include <ozge/face_gui/pro/dbru_save_facedb_process.h>
#include <iostream>
#include <bpro1/bpro1_parameters.h>
#include <ozge/face_gui/pro/dbru_facedb_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
//: Constructor
dbru_save_facedb_process::dbru_save_facedb_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","*") ) )
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
dbru_save_facedb_process::~dbru_save_facedb_process()
{
}


//: Clone the process
bpro1_process*
dbru_save_facedb_process::clone() const
{
  return new dbru_save_facedb_process(*this);
}


//: Return the name of the process
std::string dbru_save_facedb_process::name()
{
  return "Save facedb";
}


//: Call the parent function and reset num_frames_
void
dbru_save_facedb_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
std::vector< std::string >
dbru_save_facedb_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back("dbru_facedb_storage");
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
std::vector< std::string >
dbru_save_facedb_process::get_output_type()
{
  return std::vector< std::string >();
}


//: Returns the number of input frames to this process
int
dbru_save_facedb_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbru_save_facedb_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbru_save_facedb_process::execute()
{
  if ( input_data_.size() != 1 ){
    std::cout << "In save_facedb_process::execute() - "
             << "no facedb storage to save \n";
    return false;
  }
  dbru_facedb_storage_sptr facedb_storage;
  facedb_storage.vertical_cast(input_data_[0][0]);
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
  std::string path = file.path;
  vsl_b_ofstream os(path);
  facedb_storage->b_write(os);
  return true;
}


//: Finish
bool
dbru_save_facedb_process::finish()
{
  return true;
}


