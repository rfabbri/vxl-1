// This is contrib/biotree/biocts/pro/biocts_process.cxx

//:
// \file

#include <biotree/biocts/pro/biocts_process.h>
#include <vcl_iostream.h>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include <biotree/biocts/biocts_ctsim.h>
#include <biotree/biocts/biocts_Volume3D.h>


//: Constructor
biocts_process::biocts_process() : bpro1_process(), num_frames_(0)
{
  if( !parameters()->add( "Volumedata file <filename...>" , "-volume_filename", bpro1_filepath("","*")) ||
      !parameters()->add( "# of views" , "-numviews" , (int)1 ) ||
      !parameters()->add( "Lambda" ,   "-lambda" ,   (int)0 ) ||
      !parameters()->add( "Noise" ,   "-noise" ,   (int)0 ) ||
      !parameters()->add( "Gain" ,   "-gain" ,   (int)40 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
biocts_process::~biocts_process()
{
}

//: Clone the process
bpro1_process*
biocts_process::clone() const
{
  return new biocts_process(*this);
}


//: Return the name of the process
vcl_string biocts_process::name()
{
  return "CTsim";
}


//: Call the parent function and reset num_frames_
void
biocts_process::clear_output()
{
  bpro1_process::clear_output();
  num_frames_ = 0;
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
biocts_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // no input type required
  to_return.clear();
  
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
biocts_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );

  return to_return;
}


//: Returns the number of input frames to this process
int
biocts_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
biocts_process::output_frames()
{
  return num_frames_;
}


//: Run the process on the current frame
bool
biocts_process::execute()
{
  bpro1_filepath volume_filename;
  parameters()->get_value( "-volume_filename" , volume_filename );

  int num_views=0, lambda=0, noise=0, gain=0;
  parameters()->get_value( "-numviews" , num_views );
  parameters()->get_value( "-lambda" , lambda );
  parameters()->get_value( "-noise" , noise );
  parameters()->get_value( "-gain" , gain );

  //read the volume data from file
  vcl_cout << "Loading volume data: " << volume_filename.path.c_str() << "..." << vcl_endl;
  biocts_Volume3D vol(volume_filename.path);

  vcl_cout << "Volume data loaded!" << vcl_endl;

  //compute CT here
  biocts_ctsim CTvolume(lambda, noise, gain, num_views, &vol);

  vcl_cout << "CT simulation done!" << vcl_endl;

  output_data_.clear();
  for (int i=0; i<num_views; i++){
    vil_image_resource_sptr loaded_image = vil_new_image_resource_of_view( CTvolume.screens[i]);
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
    image_storage->set_image( loaded_image );
    output_data_.push_back(vcl_vector< bpro1_storage_sptr > (1,image_storage));
  }
  num_frames_ = num_views;

  return true;
}


//: Finish
bool
biocts_process::finish() 
{
  return true;
}



