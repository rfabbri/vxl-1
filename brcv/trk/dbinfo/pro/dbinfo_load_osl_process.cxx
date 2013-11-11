// This is brl/brcv/trk/dbinfo/pro/dbinfo_load_osl_process.cxx

//:
// \file

#include <dbinfo/pro/dbinfo_load_osl_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_parameters.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbinfo/pro/dbinfo_osl_storage.h>


//: Constructor
dbinfo_load_osl_process::dbinfo_load_osl_process() : bpro1_process(), osl_storage_(0)
{
  if( !parameters()->add( "Osl file <filename...>" , "-osl_filename", bpro1_filepath("","*") ))
    {
      vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


//: Destructor
dbinfo_load_osl_process::~dbinfo_load_osl_process()
{
}


//: Clone the process
bpro1_process*
dbinfo_load_osl_process::clone() const
{
  return new dbinfo_load_osl_process(*this);
}


//: Return the name of the process
vcl_string dbinfo_load_osl_process::name()
{
  return "Load OSL";
}


//: Call the parent function and reset num_frames_
void
dbinfo_load_osl_process::clear_output(int resize)
{
 
  bpro1_process::clear_output(resize);  
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbinfo_load_osl_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbinfo_load_osl_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  //no output type required
  to_return.clear();
  return to_return;
}


//: Returns the number of input frames to this process
int
dbinfo_load_osl_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
dbinfo_load_osl_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbinfo_load_osl_process::execute()
{
  // read the osls from the storage file  
  bpro1_filepath osl_path;
  parameters()->get_value( "-osl_filename" , osl_path );
  vcl_string path = osl_path.path;
  vsl_b_ifstream is(path);
  osl_storage_ = new dbinfo_osl_storage();
  osl_storage_->b_read(is);
  osl_storage_->set_frame(-2);//for global storage
  this->finish();//Not called by the execute menu macro.
  return true;
}

//: Finish
bool
dbinfo_load_osl_process::finish() 
{
  if(!osl_storage_)
    return false;
 vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
   if(!res)
     return false;
   res->initialize_global();
   //more official way
   osl_storage_->mark_global();
   //Old way of enforcing global
   if(!res->store_data(osl_storage_))
    {
      vcl_cout << "In dbinfo_osl_display_tool::display_osl() "
               << "store to repository failed\n";
      return false;
    }
  return true;
}



