// This is brl/vidpro/process/vidpro_open_dc1394_istream_process.cxx

//:
// \file

#include <vidpro/process/vidpro_open_dc1394_istream_process.h>
#include <vcl_iostream.h>

#include <bpro/bpro_parameters.h>

#include <vgui/vgui_error_dialog.h>
#include <vgui/vgui_dialog.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_flip.h>

#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>

#include <vul/vul_file.h>

#ifdef HAS_DC1394
#include <vidl/vidl_dc1394_istream.h>
#endif



//: Constructor
vidpro_open_dc1394_istream_process::vidpro_open_dc1394_istream_process() : bpro_process()
{
}


//: Destructor
vidpro_open_dc1394_istream_process::~vidpro_open_dc1394_istream_process()
{
}


//: Clone the process
bpro_process*
vidpro_open_dc1394_istream_process::clone() const
{
    return new vidpro_open_dc1394_istream_process(*this);
}


//: Return the name of the process
vcl_string vidpro_open_dc1394_istream_process::name()
{
    return "Open Video Istream";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > vidpro_open_dc1394_istream_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;

    // no input type required
    to_return.clear();

    return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > vidpro_open_dc1394_istream_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;

    // output type
    to_return.push_back( "istream" );

    return to_return;
}


//: Run the process on the current frame
bool
vidpro_open_dc1394_istream_process::execute()
{


   
    vidpro_istream_storage_sptr vis_storage = this-> dc1394_istream();

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
bool vidpro_open_dc1394_istream_process::finish() 
{
    return true;
}


//: Returns the number of input frames to this process
int
vidpro_open_dc1394_istream_process::input_frames()
{
    return 0;
}


//: Returns the number of output frames from this process
int
vidpro_open_dc1394_istream_process::output_frames()
{
    return 1;
}



//-----------------------------------------------------------------------------
//: Use vgui dialogs to open a dc1394 istream
//-----------------------------------------------------------------------------
vidpro_istream_storage_sptr
vidpro_open_dc1394_istream_process:: dc1394_istream()
{
#ifdef HAS_DC1394
  vgui_dialog dlg("Open dc1394 Input Stream");

  //: Probe cameras for valid options
  vidl_iidc1394_params::valid_options options;
  vidl_dc1394_istream::valid_params(options);

  if (options.cameras.empty()){
    vgui_error_dialog("No cameras found");
    return NULL;
  }

#ifndef NDEBUG
  vcl_cout << "Detected " << options.cameras.size() << " cameras\n";
  for (unsigned int i=0; i<options.cameras.size(); ++i){
    const vidl_iidc1394_params::valid_options::camera& cam = options.cameras[i];
    vcl_cout << "Camera "<<i<<": "<< cam.vendor << " : " << cam.model
             << " : node "<< cam.node <<" : port "<<cam.port<< '\n';
    for (unsigned int j=0; j<cam.modes.size(); ++j){
      const vidl_iidc1394_params::valid_options::valid_mode& m = cam.modes[j];
      vcl_cout << "\tmode "<<j<<" : "
               << vidl_iidc1394_params::video_mode_string(m.mode) << '\n';
      for (unsigned int k=0; k<m.frame_rates.size(); ++k){
        vcl_cout << "\t\tframe rate : "
                 << vidl_iidc1394_params::frame_rate_val(m.frame_rates[k]) << '\n';
      }
    }
  }
  vcl_cout << vcl_endl;
#endif

  vidl_iidc1394_params params;

  // Select the camera
  //-----------------------------------
  static unsigned int camera_id = 0;
  if (options.cameras.size() <= camera_id)
    camera_id = 0;

  if (options.cameras.size() > 1){
    vgui_dialog dlg("Select an IIDC 1394 camera");
    vcl_vector<vcl_string> camera_names;
    for (unsigned int i=0; i<options.cameras.size(); ++i){
      vcl_stringstream ss;
      ss << options.cameras[i].vendor << " "
         << options.cameras[i].model
         << " (node "<<options.cameras[i].node << ")";
      camera_names.push_back(ss.str());
    }
    dlg.choice("Camera",camera_names,camera_id);
    if (!dlg.ask())
      return NULL;
  }
  const vidl_iidc1394_params::valid_options::camera& cam = options.cameras[camera_id];
  params.node_ = cam.node;
  params.port_ = cam.port;

  // Select the mode
  //-----------------------------------
  if (cam.modes.empty())
  {
    vgui_error_dialog("No valid modes for this camera");
    return NULL;
  }
  static unsigned int mode_id = 0;
  bool use_1394b = cam.b_mode;
  if (cam.modes.size() > 1){
    vgui_dialog dlg("Select a capture mode");
    vcl_vector<vcl_string> mode_names;
    for (unsigned int i=0; i<cam.modes.size(); ++i){
      if(cam.modes[i].mode ==  cam.curr_mode)
        mode_id = i;
      mode_names.push_back(vidl_iidc1394_params::video_mode_string(cam.modes[i].mode));
    }
    dlg.choice("Mode",mode_names,mode_id);
    dlg.checkbox("1394b",use_1394b);
    if (!dlg.ask())
      return NULL;
  }
  const vidl_iidc1394_params::valid_options::valid_mode& m = cam.modes[mode_id];
  params.video_mode_ = m.mode;
  params.b_mode_ = use_1394b;
  params.speed_ = use_1394b ? vidl_iidc1394_params::ISO_SPEED_800 : vidl_iidc1394_params::ISO_SPEED_400;


  // Select the frame rate
  //-----------------------------------
  if (vidl_iidc1394_params::video_format_val(m.mode) < 6){
    if (m.frame_rates.empty())
    {
      vgui_error_dialog("No valid frame rates for this mode");
      return NULL;
    }
    static unsigned int fr_id = 0;
    if (m.frame_rates.size() > 1){
      vgui_dialog dlg("Select a frame rate");
      vcl_vector<vcl_string> rate_names;
      for (unsigned int i=0; i<m.frame_rates.size(); ++i){
        if(m.frame_rates[i] == cam.curr_frame_rate)
          fr_id = i;
        vcl_stringstream name;
        name << vidl_iidc1394_params::frame_rate_val(m.frame_rates[i]) << " fps";
        rate_names.push_back(name.str());
      }
      dlg.choice("Frame Rate",rate_names,fr_id);
      if (!dlg.ask())
        return NULL;
    }
    params.frame_rate_ = m.frame_rates[fr_id];
  }

  // Select the feature values
  //-------------------------------------
  if(!cam.features.empty()){
    params.features_ = cam.features;
    vgui_dialog dlg("Set feature values");
    for(unsigned int i=0; i<params.features_.size(); ++i){
      vidl_iidc1394_params::feature_options& f = params.features_[i];
      vcl_stringstream ss;
      ss << vidl_iidc1394_params::feature_string(f.id) << " [" << f.min << " - "<<f.max<<"]";
      dlg.field(ss.str().c_str(), f.value);
    }

    if (!dlg.ask())
      return NULL;
  }

  static unsigned int num_dma_buffers = 2;
  static bool drop_frames = false;
  {
    vgui_dialog dlg("Enter DMA Options");
    dlg.field("Number of DMA Buffers",num_dma_buffers);
    dlg.checkbox("Drop Frames",drop_frames);
    if (!dlg.ask())
      return NULL;
  }

  vidl_dc1394_istream* i_stream = new vidl_dc1394_istream();
  i_stream->open(num_dma_buffers, drop_frames, params);
  if (!i_stream || !i_stream->is_open()) {
    vgui_error_dialog("Failed to open the input stream");
    delete i_stream;
    return NULL;
  }

   // create the storage data structure
    vidpro_istream_storage_sptr vis_storage = vidpro_istream_storage_new();

    vis_storage->set_istream(i_stream);
    return vis_storage;



#else // HAS_DC1394
  vgui_error_dialog("dc1394 support not compiled in");
  return NULL;
#endif // HAS_DC1394
}
