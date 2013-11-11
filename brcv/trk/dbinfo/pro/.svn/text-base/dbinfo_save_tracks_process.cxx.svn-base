// This is brl/vidpro1/process/dbinfo_save_tracks_process.cxx

//:
// \file

#include <dbinfo/pro/dbinfo_save_tracks_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_parameters.h>
#include <dbinfo/pro/dbinfo_track_storage.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
//: Constructor
dbinfo_save_tracks_process::dbinfo_save_tracks_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbinfo_save_tracks_process::~dbinfo_save_tracks_process()
{
}


//: Clone the process
bpro1_process*
dbinfo_save_tracks_process::clone() const
{
  return new dbinfo_save_tracks_process(*this);
}


//: Return the name of the process
vcl_string dbinfo_save_tracks_process::name()
{
  return "Save Tracks";
}


//: Call the parent function and reset num_frames_
void
dbinfo_save_tracks_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbinfo_save_tracks_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // storage type required
  to_return.push_back("dbinfo_track_storage");
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbinfo_save_tracks_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dbinfo_save_tracks_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbinfo_save_tracks_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dbinfo_save_tracks_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbbgm_save_tracks_process::execute() - "
             << "no track storage to save \n";
    return false;
  }
  dbinfo_track_storage_sptr track_storage;
  track_storage.vertical_cast(input_data_[0][0]);
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
  vcl_string path = file.path;
  vsl_b_ofstream os(path);
  track_storage->b_write(os);
  return true;
}


//: Finish
bool
dbinfo_save_tracks_process::finish()
{
  return true;
}


