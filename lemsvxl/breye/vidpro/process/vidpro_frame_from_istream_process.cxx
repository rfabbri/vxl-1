// This is brl/vidpro/process/vidpro_frame_from_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_frame_from_istream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vidl/vidl_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_convert.h>

//: Constructor
vidpro_frame_from_istream_process::vidpro_frame_from_istream_process() : bpro_process()
{
}


//: Destructor
vidpro_frame_from_istream_process::~vidpro_frame_from_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_frame_from_istream_process::clone() const
{
    return new vidpro_frame_from_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_frame_from_istream_process::name()
{
    return "Frame from Istream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_frame_from_istream_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;

    to_return.push_back( "istream" );

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_frame_from_istream_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    // output type
    to_return.push_back( "image" );

    return to_return;
}


//: Returns the number of input frames to this process
int
vidpro_frame_from_istream_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
vidpro_frame_from_istream_process::output_frames()
{
    return 1;
}


//: Run the process on the current frame
bool
vidpro_frame_from_istream_process::execute()
{
    // get istream 
    bpro_storage_sptr sto =input_data_[0][0];
    vidpro_istream_storage_sptr is_sto;
    is_sto.vertical_cast(sto);
    istream_= is_sto->get_istream();
    clear_output();

    
    if(!istream_)
        return false;

    if (!istream_->is_open()) {
        vcl_cout << "Failed to open the input stream\n";
        return false;
    }

    istream_->advance();
    vidl_frame_sptr frame = istream_->current_frame();

    vil_image_view<vxl_byte> image;   // other types?
    vidl_convert_to_view(*frame, image);
    vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(image);


    // create the storage data structure
    vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
    image_storage->set_image( image_sptr );


    output_data_[0].push_back(image_storage);
    return true;

}


//: Finish
bool vidpro_frame_from_istream_process::finish() 
{
    return true;
}




