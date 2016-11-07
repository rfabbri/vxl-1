// This is brl/vidpro/process/vidpro_save_image_process.cxx

//:
// \file

#include <vidpro/process/vidpro_save_image_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>



//: Constructor
vidpro_save_image_process::vidpro_save_image_process() : bpro_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
vidpro_save_image_process::~vidpro_save_image_process()
{
}


//: Clone the process
bpro_process*
vidpro_save_image_process::clone() const
{
  return new vidpro_save_image_process(*this);
}


//: Return the name of the process
vcl_string vidpro_save_image_process::name()
{
  return "Save Image";
}


//: Call the parent function and reset num_frames_
void
vidpro_save_image_process::clear_output()
{
  bpro_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
vidpro_save_image_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "image" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
vidpro_save_image_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
vidpro_save_image_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
vidpro_save_image_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
vidpro_save_image_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro_save_image_process::execute() - "
             << "not exactly one input images \n";
    return false;
  }
  clear_output();

  // get image from the storage class
  vidpro_image_storage_sptr image_stg;
  image_stg.vertical_cast(input_data_[0][0]);


  vil_image_resource_sptr image_sptr = image_stg->get_image();
  
  // vil_save expects a byte image so we must first make a byte
  // image out of whatever we have
  vil_image_view<vxl_byte> save_image;
  
  switch(image_sptr->pixel_format())
  {
  case VIL_PIXEL_FORMAT_BYTE: 
    {
      save_image = image_sptr->get_view();
      break;
    }
  case VIL_PIXEL_FORMAT_UINT_16:
    {
      vil_image_view<vxl_uint_16> uint_image = image_sptr->get_view();
      vil_image_view<double> double_image;
      vil_convert_stretch_range_limited(uint_image, double_image,
                                        vxl_uint_16(0),
                                        vxl_uint_16(65535), 
                                        0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
   
  case VIL_PIXEL_FORMAT_BOOL: 
    {
      vil_image_view<bool> bool_image = image_sptr->get_view();
      vil_convert_cast(bool_image, save_image);
      vil_math_scale_values(save_image,255.0);
      break;
    }
    
  case VIL_PIXEL_FORMAT_FLOAT:
    {
      vil_image_view<float> float_image = image_sptr->get_view();
      vil_image_view<double> double_image;
      vil_convert_stretch_range_limited(float_image, double_image,
                                        0.0f, 1.0f, 0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
  
  case VIL_PIXEL_FORMAT_DOUBLE:
    {
      vil_image_view<double> double_image = image_sptr->get_view();
      vil_convert_stretch_range_limited(double_image, double_image,
                                        0.0, 1.0, 0.0, 255.999);
      vil_convert_cast(double_image, save_image);
      break;
    }
  
  default:
    {
      // stretch and convert to a byte image for display
      save_image = *vil_convert_stretch_range(vxl_byte(), image_sptr->get_view());
    }
  }

  bpro_filepath file;
  parameters()->get_value( "-filename" , file );
  vil_save(save_image,file.path.c_str());
 

  return true;

}


//: Finish
bool
vidpro_save_image_process::finish()
{
  return true;
}


