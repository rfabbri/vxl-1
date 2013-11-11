// This is contrib/mleotta/pro/mleotta_save_keypoints_process.cxx

//:
// \file

#include "mleotta_save_keypoints_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbdet/pro/dbdet_keypoint_storage.h>
#include <vsl/vsl_vector_io.h>



//: Constructor
mleotta_save_keypoints_process::mleotta_save_keypoints_process() 
{
  if( !parameters()->add( "Binary file <filename...>" , "-keypoints_filename", bpro1_filepath("","*") )  )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
mleotta_save_keypoints_process::~mleotta_save_keypoints_process()
{
}


//: Clone the process
bpro1_process*
mleotta_save_keypoints_process::clone() const
{
  return new mleotta_save_keypoints_process(*this);
}


//: Return the name of the process
vcl_string mleotta_save_keypoints_process::name()
{
  return "Save Keypoints";
}


//: Call the parent function and reset num_frames_
void
mleotta_save_keypoints_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
mleotta_save_keypoints_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "keypoints" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
mleotta_save_keypoints_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();

  return to_return;
}


//: Returns the number of input frames to this process
int
mleotta_save_keypoints_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
mleotta_save_keypoints_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
mleotta_save_keypoints_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In mleotta_save_keypoints_process::execute() - "
             << "not exactly one keypoints frame \n";
    return false;
  }
  clear_output();
  
  dbdet_keypoint_storage_sptr keypoints;
  keypoints.vertical_cast(input_data_[0][0]);

  all_keypoints_.push_back(keypoints->keypoints());
  return true;  
}


//: Finish
bool
mleotta_save_keypoints_process::finish() 
{
    
  bpro1_filepath path;
  parameters()->get_value( "-keypoints_filename" , path );
  vcl_string filename = path.path;

  vsl_b_ofstream bfs(filename);
  for(unsigned int i=0; i<all_keypoints_.size();  ++i)
    vsl_b_write(bfs, all_keypoints_[i]);
  bfs.close();
  

  return true;
}


