// This is brcv/rec/dber/pro/dber_save_instances_process.cxx

//:
// \file

#include <dber/pro/dber_save_instances_process.h>
#include <vcl_iostream.h>
#include <bpro1/bpro1_parameters.h>
#include <dber/pro/dber_instance_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <bvis1/bvis1_manager.h>

#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>


//: Constructor
dber_save_instances_process::dber_save_instances_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dber_save_instances_process::~dber_save_instances_process()
{
}


//: Clone the process
bpro1_process*
dber_save_instances_process::clone() const
{
  return new dber_save_instances_process(*this);
}


//: Return the name of the process
vcl_string dber_save_instances_process::name()
{
  return "Save Video Instances";
}


//: Call the parent function and reset num_frames_
void
dber_save_instances_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dber_save_instances_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dber_instance_storage");
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dber_save_instances_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dber_save_instances_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dber_save_instances_process::output_frames()
{
  return 1;
}


//: Run the process on the current frame
bool
dber_save_instances_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dber_save_instances_process::execute() - "
             << "no instance storage to save \n";
    return false;
  }
  dber_instance_storage_sptr ins_storage;
  ins_storage.vertical_cast(input_data_[0][0]);
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
  vcl_string path = file.path;
  
  vsl_b_ofstream os(path);
  vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames = ins_storage->get_frames();
  unsigned numframes=frames.size();
  vsl_b_write(os,numframes);
  for(unsigned i=0;i<numframes;i++)
    vsl_b_write(os,frames[i]);
  os.close();
  
  return true;
}

//: Finish
bool
dber_save_instances_process::finish()
{
  return true;
}


