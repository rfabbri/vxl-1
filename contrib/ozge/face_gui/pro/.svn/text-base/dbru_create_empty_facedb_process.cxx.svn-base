// This is brl/vidpro1/process/dbru_create_empty_facedb_process.cxx

//:
// \file

#include <ozge/face_gui/pro/dbru_create_empty_facedb_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_parameters.h>
#include <ozge/face_gui/pro/dbru_facedb_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>
//: Constructor
dbru_create_empty_facedb_process::dbru_create_empty_facedb_process() : bpro1_process()
{
}


//: Destructor
dbru_create_empty_facedb_process::~dbru_create_empty_facedb_process()
{
}


//: Clone the process
bpro1_process*
dbru_create_empty_facedb_process::clone() const
{
  return new dbru_create_empty_facedb_process(*this);
}


//: Return the name of the process
vcl_string dbru_create_empty_facedb_process::name()
{
  return "Create Empty facedb";
}


//: Call the parent function and reset num_frames_
void
dbru_create_empty_facedb_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbru_create_empty_facedb_process::get_input_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbru_create_empty_facedb_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbru_facedb_storage");
  return to_return;
}


//: Returns the number of input frames to this process
int
dbru_create_empty_facedb_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
dbru_create_empty_facedb_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbru_create_empty_facedb_process::execute()
{
  facedb_storage_ = dbru_facedb_storage_new();
  facedb_storage_->set_frame(-2); // for global storage (not depending on the frame number
  this->finish();
  return true;
}


//: Finish
bool
dbru_create_empty_facedb_process::finish() 
{
  if(!facedb_storage_)
    return false;
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   facedb_storage_->mark_global();
   //Old way of enforcing global
   if(!res->store_data(facedb_storage_))
    {
      vcl_cout << "In dbinfo_facedb_display_tool::display_facedb() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}

