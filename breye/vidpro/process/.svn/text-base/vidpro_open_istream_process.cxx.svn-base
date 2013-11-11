// This is brl/vidpro/process/vidpro_open_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_open_istream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>

//#include <vidpro/vidpro_vidl_gui_param_dialog.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#include <vul/vul_file.h>

#ifdef HAS_DSHOW
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_dshow_live_istream.h>
#endif

#ifdef HAS_VIDEODEV
#include <vidl_v4l_istream.h>
#endif

#ifdef HAS_FFMPEG
#include <vidl/vidl_ffmpeg_istream.h>
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>
#endif

#ifdef HAS_DC1394
#include <vidl/vidl_dc1394_istream.h>
#endif



//: Constructor
vidpro_open_istream_process::vidpro_open_istream_process() : bpro_process()
{
    this->add_parameters();
}


//: Destructor
vidpro_open_istream_process::~vidpro_open_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_open_istream_process::clone() const
{
    return new vidpro_open_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_open_istream_process::name()
{
    return "Open Video Istream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_open_istream_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;

    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_open_istream_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    // output type
    to_return.push_back( "istream" );

    return to_return;
}


//: Run the process on the current frame
bool
vidpro_open_istream_process::execute()
{
    //get stream type
    unsigned istream_type=0;
    parameters()->get_value( "-istream_type" , istream_type );

    //get file name of directory
    bpro_filepath istream_path;
    parameters()->get_value( "-istream_filename" , istream_path );

   
    vidpro_istream_storage_sptr vis_storage = this->open_istream(istream_type, istream_path.path);

    if(!vis_storage)
    {
        return false;
        vcl_cerr<<"Error: Opening the input stream\n";
    }
   
    
    clear_output();

    this->set_global(0,0);
       
    output_data_[0].push_back(vis_storage);
    return true;

    vcl_cerr<<"Error in Open Istream process: Not able to open the istream\n";
    return false;
}


//: Finish
bool vidpro_open_istream_process::finish() 
{
    return true;
}


//: Returns the number of input frames to this process
int
vidpro_open_istream_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro_open_istream_process::output_frames()
{
    return 1;
}


void
vidpro_open_istream_process::add_parameters()
{
   
    choices_.push_back( "Image List" ); choice_codes_.push_back(IMAGE_LIST);
#ifdef HAS_FFMPEG
    choices_.push_back( "FFMPEG" ); choice_codes_.push_back(FFMPEG)
#endif
#ifdef HAS_DSHOW
    choices_.push_back( "DSHOW from File" ); choice_codes_.push_back(DSHOW_FILE);
//    choices_.push_back( "DSHOW Live" ); choice_codes_.push_back(DSHOW_LIVE)
#endif
#ifdef HAS_VIDEODEV
    choices_.push_back( "VIDEODEV" );choice_codes_.push_back(VIDEODEV);
#endif


    if( !parameters()->add( "Select an Input Stream Type" , "-istream_type" , choices_, 0 )||
        !parameters()->add( "Filename" , "-istream_filename" ,bpro_filepath("","*") ) )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }


}

vidpro_istream_storage_sptr
vidpro_open_istream_process::open_istream(int type, vcl_string const& filename)
{
    switch(choice_codes_[type]){
    case IMAGE_LIST:{
          vcl_string directory = vul_file::dirname(filename); // Strips away "/filename.ext" of the path
          return image_list_istream(directory.c_str());}
       
#ifdef HAS_FFMPEG
    case FFMPEG:
        return ffmpeg_istream(filename);
#endif
#ifdef HAS_DSHOW
    case DSHOW_FILE:
         return dshow_file_istream(filename);
             
#endif
#ifdef HAS_VIDEODEV
        case VIDEODEV
            return vd4l_istream(filename);
#endif

    default:
        break;
    }
     return NULL;

}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open an image list istream
//-----------------------------------------------------------------------------
vidpro_istream_storage_sptr
vidpro_open_istream_process::image_list_istream(vcl_string const& glob)
{

   
    vcl_string s(glob.c_str());
    s += "/*";
    vidl_image_list_istream* vis = new vidl_image_list_istream(s);
    if (!vis|| !vis->is_open()) {
        vcl_cerr <<"Failed to open the input stream\n";
        delete vis;
        return NULL;
    }

    // create the storage data structure
    vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();

    vis_storage->set_istream(vis);
    vis_storage->set_path(s);
    vis_storage->set_type("image_list");


    return vis_storage;
}




//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a FFMPEG istream
//-----------------------------------------------------------------------------
vidpro_istream_storage_sptr
vidpro_open_istream_process::ffmpeg_istream(vcl_string const& filename)
{
#ifdef HAS_FFMPEG

    vidl_ffmpeg_istream* vis = new vidl_ffmpeg_istream(image_filename);
    if (!vis || !vis->is_open()) {
        vcl_cerr<<"Failed to open the input stream\n";
        delete vis;
        return NULL;
    }
    // create the storage data structure
    vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();

    vis_storage->set_istream(vis);
    vis_storage->set_path(filename);
    vis_storage->set_type("ffmpeg");

    return vis_storage;
#else // HAS_FFMPEG
    vcl_cerr <<"FFMPEG support not compiled in\n";
    return NULL;
#endif // HAS_FFMPEG
}

//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a DSHOW_FILE istream
//-----------------------------------------------------------------------------
vidpro_istream_storage_sptr
vidpro_open_istream_process::dshow_file_istream(vcl_string const& filename)
{
#ifdef HAS_DSHOW

    vidl_dshow_file_istream *vis = new vidl_dshow_file_istream(filename);
    if (!vis || !vis->is_open()) {
        vcl_cerr<<"Failed to open the input stream\n";
        delete vis;
        return NULL;
    }
    // create the storage data structure
    vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();

    vis_storage->set_istream(vis);
    vis_storage->set_path(filename);
    vis_storage->set_type("dshow");

    return vis_storage;
    ;

#else // HAS_DSHOW
    vcl_cerr <<"DSHOW support not compiled in\n";
    return NULL;
#endif // HAS_DSHOW

}
