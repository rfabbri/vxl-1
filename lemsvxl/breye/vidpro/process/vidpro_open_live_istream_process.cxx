// This is brl/vidpro/process/vidpro_open_live_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_open_live_istream_process.h>
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
#include <vidl/vidl_dshow_live_istream.h>
#include <vidl/vidl_dshow_istream_params.h>
#endif


#include <mbl/mbl_read_props.h>

//: Constructor
vidpro_open_live_istream_process::vidpro_open_live_istream_process() : bpro_process()
{
    this->add_parameters();
}


//: Destructor
vidpro_open_live_istream_process::~vidpro_open_live_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_open_live_istream_process::clone() const
{
    return new vidpro_open_live_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_open_live_istream_process::name()
{
    return "Open Video Istream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_open_live_istream_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;

    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_open_live_istream_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    // output type
    to_return.push_back( "istream" );

    return to_return;
}


//: Run the process on the current frame
bool
vidpro_open_live_istream_process::execute()
{
    //get stream type
    unsigned istream_type=0;
    parameters()->get_value( "-istream_type" , istream_type );

    //get file name of directory
    bpro_filepath config_file;
    parameters()->get_value( "-config_file" , config_file );

    //get file name of directory
    vcl_string device_name;
    parameters()->get_value( "-device_name" , device_name );

   
    vidpro_istream_storage_sptr vis_storage = this->open_live_istream(istream_type, device_name, config_file.path);

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
bool vidpro_open_live_istream_process::finish() 
{
    return true;
}


//: Returns the number of input frames to this process
int
vidpro_open_live_istream_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro_open_live_istream_process::output_frames()
{
    return 1;
}


void
vidpro_open_live_istream_process::add_parameters()
{
   
#ifdef HAS_DSHOW
   choices_.push_back( "DSHOW Live" ); choice_codes_.push_back(DSHOW_LIVE);
#endif

    if( !parameters()->add( "Select an Input Stream Type" , "-istream_type" , choices_, 0 )||
        !parameters()->add( "Device Name" , "-device_name" , vcl_string("")) ||
        !parameters()->add( "Configuration File" , "-config_file" ,bpro_filepath("","*") ) )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }


}

vidpro_istream_storage_sptr
vidpro_open_live_istream_process::open_live_istream(int type, 
                                                     vcl_string const& device_name, 
                                                     vcl_string const& config_file)
{
    switch(choice_codes_[type]){
#ifdef HAS_DSHOW
    case DSHOW_LIVE:
         return dshow_live_istream(device_name, config_file);
             
#endif

    default:
        break;
    }
     return NULL;

}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a DSHOW_FILE istream
//-----------------------------------------------------------------------------
vidpro_istream_storage_sptr
vidpro_open_live_istream_process::dshow_live_istream(vcl_string const& device_name,
                                                      vcl_string const& config_file)
{
#ifdef HAS_DSHOW

    vidl_dshow_live_istream<vidl_dshow_istream_params> *vis = new 
    vidl_dshow_live_istream<vidl_dshow_istream_params>(vidl_dshow_istream_params().set_device_name(device_name)
    .set_properties(mbl_read_props(vcl_ifstream(config_file.c_str())))
    );

    if (!vis || !vis->is_open()) {
        vcl_cerr<<"Failed to open the input stream\n";
        delete vis;
        return NULL;
    }
    // create the storage data structure
    vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();

    vis_storage->set_istream(vis); 

    return vis_storage;
    ;

#else // HAS_DSHOW
    vcl_cerr <<"DSHOW Live support not compiled in\n";
    return NULL;
#endif // HAS_DSHOW

}
