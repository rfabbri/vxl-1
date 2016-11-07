// This is brl/vidpro/process/vidpro_load_dshow_file_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_load_dshow_file_istream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>
#include <vidl/vidl_dshow_file_istream.h>


#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_ostream.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>



//: Constructor
vidpro_load_dshow_file_istream_process::vidpro_load_dshow_file_istream_process() : bpro_process()
{
 if( parameters()->add( "Video stream file <filename...>" , "-istream_filename" , bpro_filepath("","*") ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
vidpro_load_dshow_file_istream_process::~vidpro_load_dshow_file_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_load_dshow_file_istream_process::clone() const
{
  return new vidpro_load_dshow_file_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_load_dshow_file_istream_process::name()
{
  return "Load Movie";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_load_dshow_file_istream_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_load_dshow_file_istream_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  // output type
  to_return.push_back( "image" );

  return to_return;
}


//: Run the process on the current frame
bool
vidpro_load_dshow_file_istream_process::execute()
{
  bpro_filepath istream_path;
  
  parameters()->get_value( "-istream_filename" , istream_path );
  
  //needs to be adapted to different types of istream
  vidl_dshow_file_istream *vis= new vidl_dshow_file_istream(istream_path.path);
   
  //no input storage class for this process
  clear_output();

  // create the storage data structure
  vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();
 
  vis_storage->set_istream(vis); 
  vis_storage->set_name("istream0");
  bpro_storage_sptr sto = vis_storage;
  brdb_database_manager::instance()->
      add_tuple("global_data",new brdb_tuple(sto->name(),sto->type(),sto));
  

    if(!vis)
      vcl_cout << "Failed to open the input stream\n";

    if (!vis->is_open()) {
        vcl_cout << "Failed to open the input stream\n";
    
    }

    vis->advance();

    vidl_frame_sptr video_frame = vis->current_frame();
    vil_image_view<vxl_byte> image;   // do i need other types?
    vidl_convert_to_view(*video_frame, image);

    image= vil_flip_ud(image);
    vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(image);

    // create the storage data structure
    vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
    image_storage->set_image( image_sptr );
    image_storage->set_name("unprocessed");

    output_data_[0].push_back(image_storage);

  return true;
}


//: Finish
bool vidpro_load_dshow_file_istream_process::finish() 
{
  return true;
}


//: Returns the number of input frames to this process
int
vidpro_load_dshow_file_istream_process::input_frames()
{
  return 0;
}


//: Returns the number of output frames from this process
int
vidpro_load_dshow_file_istream_process::output_frames()
{
  return 1;
}


