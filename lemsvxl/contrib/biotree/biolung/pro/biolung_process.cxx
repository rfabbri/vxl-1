// This is contrib/biotree/biolung/pro/biolung_process.cxx

//:
// \file

#include <biotree/biolung/pro/biolung_process.h>
#include <iostream>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource_sptr.h>

#include <biotree/biolung/biolung_lungmodel.h>

//: Constructor
biolung_process::biolung_process() : bpro1_process()
{
  if( !parameters()->add( "Output Volumedata file <filename...>" , "-volume_filename", bpro1_filepath("","*")) ||
      !parameters()->add( "zdim" , "-zdim" , (int)525 ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
biolung_process::~biolung_process()
{
}

//: Clone the process
bpro1_process*
biolung_process::clone() const
{
  return new biolung_process(*this);
}

//: Return the name of the process
std::string biolung_process::name()
{
  return "3DLungModel";
}


//: Call the parent function
void
biolung_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
std::vector< std::string >
biolung_process::get_input_type()
{
  std::vector< std::string > to_return;
  // no input type required
  to_return.clear();
  
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
std::vector< std::string >
biolung_process::get_output_type()
{
  std::vector< std::string > to_return;
  // no input type required
  to_return.clear();
  
  return to_return;
}


//: Returns the number of input frames to this process
int
biolung_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
biolung_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
biolung_process::execute()
{
  bpro1_filepath volume_filename;
  parameters()->get_value( "-volume_filename" , volume_filename );

  int zdim=0;
  parameters()->get_value( "-zdim" , zdim );

  //compute the tree model and construct the volume data 
  std::cout << "Computing 3D Lung model...";
  biolung_lungmodel LungModel(zdim);
  std::cout << "done!" << std::endl;

  //write the volume data into file
  std::cout << "Saving volume data: " << volume_filename.path.c_str() << "...";
  LungModel.saveVolumeInFile(volume_filename.path);
  std::cout << "done!" << std::endl;

  output_data_.clear();
  return true;
}


//: Finish
bool
biolung_process::finish() 
{
  return true;
}



