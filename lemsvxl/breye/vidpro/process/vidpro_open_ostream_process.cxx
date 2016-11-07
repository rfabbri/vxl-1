// This is brl/vidpro/process/vidpro_open_ostream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_open_ostream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>

//#include <vidpro/vidpro_vidl_gui_param_dialog.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_file_format.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#include <brdb/brdb_tuple.h>

#ifdef HAS_FFMPEG
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>
#endif





//: Constructor
vidpro_open_ostream_process::vidpro_open_ostream_process() : bpro_process()
{
    
    this->add_parameters();
}


//: Destructor
vidpro_open_ostream_process::~vidpro_open_ostream_process()
{
}


//: Clone the process
bpro_process*
vidpro_open_ostream_process::clone() const
{
    return new vidpro_open_ostream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_open_ostream_process::name()
{
    return "Open Video Ostream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_open_ostream_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;

    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_open_ostream_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    // output type
    to_return.clear();

    return to_return;
}


//: Run the process on the current frame
bool
vidpro_open_ostream_process::execute()
{
    //get stream type
    unsigned vos_type=0;
    parameters()->get_value( "-ostream_type" , vos_type );

    //get file name of directory
    bpro_filepath vos_directory;
    parameters()->get_value( "-ostream_directory" , vos_directory );

      unsigned vos_fmt=0;
    parameters()->get_value( "-ostream_fmt" , vos_fmt );

      vcl_string filename_fmt;
    parameters()->get_value( "-filename_fmt" , filename_fmt );

     unsigned start_index=0;
    parameters()->get_value( "-start_index" , start_index );

    
    vcl_string ostream_name;
    parameters()->get_value( "-ostream_name" , ostream_name );
   
    vidpro_ostream_storage_sptr vos_storage = this->open_ostream(vos_type,
                                                                  vos_directory.path,
                                                                  filename_fmt,
                                                                  fmt_choices_[vos_fmt],
                                                                  start_index);

    if(!vos_storage)
    {
        return false;
        vcl_cerr<<"Error: Opening the input stream\n";
    }
   
    //no input storage class for this process
    clear_output();


   
    vidl_ostream_sptr vos = vos_storage->get_ostream(); 
    vos_storage->set_name (ostream_name);
    bpro_storage_sptr sto = vos_storage;
    brdb_database_manager::instance()->
        add_tuple("global_data",new brdb_tuple(sto->name(),sto->type(),sto));


   
    vcl_cerr<<"Error in Open Istream process: Not able to open the istream\n";
    return false;
}


//: Finish
bool vidpro_open_ostream_process::finish() 
{
    return true;
}


//: Returns the number of input frames to this process
int
vidpro_open_ostream_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro_open_ostream_process::output_frames()
{
    return 0;
}


void
vidpro_open_ostream_process::add_parameters()
{

   
   // provide a list of choices for ostream types
    choices_.push_back( "Image List" ); choice_codes_.push_back(IMAGE_LIST);

// provide a list of choices for valid file formats
  if (fmt_choices_.empty()) {
    for (vil_file_format** p = vil_file_format::all(); *p; ++p)
      fmt_choices_.push_back((*p)->tag());
  }
  
  static int fmt_idx = 0;
  static unsigned int start_index = 0;
  static vcl_string name_format = "%05u";
       

    if( !parameters()->add( "Select an Onput Stream Type" , "-ostream_type" , choices_, 0 )||
        !parameters()->add( "Output Stream Name" , "-ostream_name" ,vcl_string(""))|| 
        !parameters()->add( "Output stream Directory" , "-ostream_directory" ,bpro_filepath("","*")) ||
        !parameters()->add( "Image File Format", "-ostream_fmt", fmt_choices_, fmt_idx) ||
        !parameters()->add ("Starting Index", "-start_index", start_index) ||
        !parameters()->add ("Filename Formate", "-filename_fmt", name_format))
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }


}

vidpro_ostream_storage_sptr
vidpro_open_ostream_process::open_ostream(int type, 
                                           vcl_string const& directory,
                                           vcl_string const &name_format,
                                           vcl_string const &imagefile_fmt,
                                           unsigned int start_index)
{
    switch(choice_codes_[type]){
    case IMAGE_LIST:
        return image_list_ostream(directory, name_format, imagefile_fmt,start_index);
       
    default:
        break;
    }
     return NULL;

}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open an image list ostream
//-----------------------------------------------------------------------------
vidpro_ostream_storage_sptr
vidpro_open_ostream_process::image_list_ostream(vcl_string const &directory,
                                                 vcl_string const &name_format,
                                                 vcl_string const &imagefile_fmt,
                                                 unsigned int start_index)
{

  vidl_image_list_ostream* vos = new vidl_image_list_ostream(directory,
                                            name_format,
                                            imagefile_fmt,
                                            start_index);

  if (!vos || !vos->is_open()) {
    vcl_cerr<<"Failed to create output image list stream\n";
    delete vos;
    return NULL;
  }

  // create the storage data structure
    vidpro_ostream_storage_sptr vos_storage = vidpro_ostream_storage_new();

    vos_storage->set_ostream(vos); 

    return vos_storage;
}





