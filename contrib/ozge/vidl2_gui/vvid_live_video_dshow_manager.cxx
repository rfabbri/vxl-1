// This is brl/vvid/vvid_live_video_dshow_manager.cxx
#include "vvid_live_video_dshow_manager.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vcl_sstream.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>
#include <vul/vul_file.h>
//#include <vil1/vil1_memory_image_of.h>
#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <bgui/bgui_vtol2D_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vtol/vtol_face_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <sdet/sdet_detector_params.h>
#include <sdet/sdet_harris_detector_params.h>
#include <vpro/vpro_harris_corner_process.h>
#include <vpro/vpro_camera_calibration_process.h>

//#include <vvid/cmu_1394_camera_params.h>
#include <vpro/vpro_video_process.h>
#include <vpro/vpro_edge_process.h>
#include <vpro/vpro_region_process.h>
#include <vpro/vpro_capture_process.h>
#include <bgui/bgui_histogram_tableau.h>

#include <vpro/vpro_frame_diff_process.h>

//static live_video_manager instance
vvid_live_video_dshow_manager *vvid_live_video_dshow_manager::instance_ = 0;


vvid_live_video_dshow_manager *vvid_live_video_dshow_manager::instance()
{
  if (!instance_)
  {
    instance_ = new vvid_live_video_dshow_manager();
    instance_->init();
  }
  return vvid_live_video_dshow_manager::instance_;
}

//-----------------------------------------------------------
// constructors/destructor
//
vvid_live_video_dshow_manager::
vvid_live_video_dshow_manager() :
  //cp_(cmu_1394_camera_params()),
  //cp_(vidl_dshow_istream_params()),
  histogram_(true),
  width_(960),
  height_(480),
  min_msec_per_frame_(1000.0/30.0),
  win_(0),
  video_process_(0),
  init_successful_(false)
{}

vvid_live_video_dshow_manager::~vvid_live_video_dshow_manager()
{}
//----------------------------------------------------------
// determine the number of active cameras and install the reduced
// resolution views accordingly.
//
void vvid_live_video_dshow_manager::init()
{
  //Determine the number of active cameras
  // for now we assume use a pre-defined _N_views

  // Get the list of available cameras
  vcl_vector<vcl_string> device_names = vidl_dshow::get_capture_device_names();
  num_cameras_ = device_names.size();
  vcl_cout << "Number of Cameras Detected: " << num_cameras_ << vcl_endl;
  
  //for (unsigned int i = 0; i<num_cameras_; i++) {
  //  vidl_dshow_istream_params::print_parameter_help(device_names[i]);
  //}

  if (num_cameras_ <= 0) {
    vcl_cerr << "Exiting - no cameras detected\n";
    vcl_exit(0);
  }

  //vcl_vector<vidl_dshow_istream_params> params;

  /*num_cameras_ = 0;
  vcl_vector<bool> chosen(device_names.size(), false);
  for (unsigned i = 0; i<device_names.size(); i++) {

    vgui_dialog cam_dlg("Select Camera to view");
   //vcl_vector<bool> chosen(num_cameras_, false);
    bool dummy;
    vcl_string str = "Use camera " + device_names[i];
    cam_dlg.checkbox(str.c_str(), dummy);
    if (!cam_dlg.ask())
      return;

    if (dummy) {
      num_cameras_++;
      vcl_cout << "camera " << device_names[i] << " was chosen\n";
    } else
      vcl_cout << "camera " << device_names[i] << " was not chosen\n";
    chosen[i] = dummy;
  }*/

  num_cameras_ = 1;
  unsigned int choice = 0;
  vgui_dialog cam_dlg("Select Camera");
  cam_dlg.choice("Select camera:", device_names, choice);
  
  if (!cam_dlg.ask())
    return;

  vcl_cout << "camera " << device_names[choice] << " is chosen\n";
  vcl_cout << "num of cameras: " << num_cameras_ << "\n";

  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(2*num_cameras_,2);
  grid->set_grid_size_changeable(true);
  sample_ = 1;
  init_successful_ = true;
  edges_ = true;
  vvid_live_video_dshow_tableau_sptr vtab;
  bgui_vtol2D_tableau_sptr vt2D;
  vgui_image_tableau_sptr vi2D;

  for (unsigned cam=0, i = 0; cam<num_cameras_; ++cam, i++)
  {
    //if (!chosen[i]) i++;
    //vcl_cout << "i: " << i << " setting camera params: " << device_names[i] << "\n";
    vcl_cout << " setting camera params of " << device_names[choice] << "\n";
    vidl_dshow_istream_params cp = vidl_dshow_istream_params();
    cp.set_device_name(device_names[choice]);
    set_camera_params(cp, sample_);
    //vcl_cout << "Camera " << cam << " is chosen device: " << cp.device_name() << "\n";
    vidl_dshow_live_istream<vidl_dshow_istream_params> *ds_istream = 
        new vidl_dshow_live_istream<vidl_dshow_istream_params>(cp);

    if (!ds_istream || !ds_istream->is_open()) {
      vcl_cout << "Failed to open the input stream for camera: " << cp.device_name() << "\n";
      vcl_exit(1);
    }

    vtab = vvid_live_video_dshow_tableau_new(cam, sample_, ds_istream);
    vtabs_.push_back(vtab);
    //init_successful_ = init_successful_&&vtab->attach_live_video();
    if (!init_successful_)
    {
      vcl_cout << "In vvid_live_video_dshow_manager::init() -"
               << " bad initialization - camera #"<< cam << vcl_endl;
      return;
    }
    vt2D =  bgui_vtol2D_tableau_new(vtab);
    vt2Ds_.push_back(vt2D);

    // make a 2D viewer tableau and add it to the grid
    vgui_viewer2D_tableau_sptr v2d = vgui_viewer2D_tableau_new(vt2D);
    grid->add_at(v2d, 2*cam, 0);

    // make a histogram
    bgui_histogram_tableau_new htab;
    htabs_.push_back(htab);
    vgui_viewer2D_tableau_new viewer(htab);
    grid->add_at(viewer, 2*cam, 1);

    // make an image tableau
    vi2D = bgui_image_tableau_new();
    //vt2D = bgui_vtol2D_tableau_new(vi2D);
    //vi2Ds_.push_back(vt2D);
    vi2Ds_.push_back(vi2D);
    //v2d = vgui_viewer2D_tableau_new(vt2D);
    v2d = vgui_viewer2D_tableau_new(vi2D);
    grid->add_at(v2d, 2*cam+1, 0);

    cam_calibs_.push_back(0);
  }

  // get the camera paramaters from the last camera (assume are are the same)
  //cp_ = vtab->get_camera_params();

  // put the grid in a shell
  vgui_shell_tableau_sptr shell = vgui_shell_tableau_new(grid);
  this->add_child(shell);
  video_process_  = (vpro_video_process*)0;
}

//: make an event handler
// Note that we have to get an adaptor and set the tableau to receive events
bool vvid_live_video_dshow_manager::handle(const vgui_event &e)
{
  //nothing special here yet
  //just pass the event back to the base class
  return vgui_wrapper_tableau::handle(e);
}
//---------------------------------------------------------
//:
//  Cameras have different possible resolutions, frame rates and color
//  sampling choices. This method sets up a vgui choice menu item
//  based on the valid video configurations supported by the camera.
//  (currently the rgb_ flag doesn't do anything but ultimately will
//   control color/vs/mono acquisition if the camera supports it.)
//  The pix_sample_interval determines the resolution of the display
//
void vvid_live_video_dshow_manager::set_camera_params(vidl_dshow_istream_params& cp, int& pix_sample_interval)
{
  vidl_dshow_istream_params::print_parameter_help(cp.device_name());

  //static int pix_sample_interval = 1;
  pix_sample_interval = 1;

  //vcl_vector<vcl_string> choices;
  //vcl_string no_choice="CurrentConfiguration";
  //choices.push_back(no_choice);

  //vcl_vector<vcl_string> device_names = vidl_dshow::get_capture_device_names();
  //for (unsigned int i = 0; i<device_names.size(); i++)
    //choices.push_back(device_names[i]);

/*
  vcl_vector<vcl_string> valid_descrs = vtab->get_capability_descriptions();
  for (vcl_vector<vcl_string>::iterator cit = valid_descrs.begin();
       cit != valid_descrs.end(); cit++)
       choices.push_back(*cit);
       */
  
  //static int choice=0;
  static double max_frame_rate=30.0;
  //Set up the dialog.
  vgui_dialog cam_dlg("Camera Parameters");
  //cam_dlg.message(vtab->current_capability_desc().c_str());
  //cam_dlg.choice("Choose Camera", choices, choice);
  /*cam_dlg.checkbox("Auto Exposure ", cp_.auto_exposure_);
  cam_dlg.checkbox("Auto Gain ", cp_.auto_gain_);
  cam_dlg.field("Shutter Speed", cp_.shutter_);
  cam_dlg.field("brightness",cp_.brightness_);
  cam_dlg.field("sharpness",cp_.sharpness_);
  cam_dlg.field("exposure",cp_.exposure_);
  cam_dlg.field("gain",cp_.gain_);
  */
  //cam_dlg.field("Display Sample Interval", pix_sample_interval);
  /*cam_dlg.field("Maximum Frame Rate", max_frame_rate);
  cam_dlg.checkbox("image capture(acquisition) ", cp_.capture_);
  cam_dlg.checkbox("RGB(monochrome) ", cp_.rgb_);
  cam_dlg.checkbox("Auto White Balance",cp_.autowhitebalance_);
  cam_dlg.field("White Balance U",cp_.whitebalanceU_);
  cam_dlg.field("White Balance V",cp_.whitebalanceV_);
  cam_dlg.checkbox(" One push WhiteBalance",cp_.onepushWBbalance_);
*/

  int output_mode = 2;  // TODO: check this
  cam_dlg.field("output mode",output_mode);

  if (!cam_dlg.ask())
    return;

  //cp.set_device_name(choices[choice]);
  cp.set_output_format(output_mode);

  min_msec_per_frame_ = 1000.0/max_frame_rate;
}

void vvid_live_video_dshow_manager::set_detection_params()
{
  if (vtabs_.size() != num_cameras_)
  {
    vcl_cout << "in vvid_live_video_dshow_manager::set_camera_params() -"
             << " no live video tableau\n";
    return;
  }
  //cache the live video state to restore
  bool live = vtabs_.back()->get_video_live();
  if (live)
    this->stop_live_video();
  static bool agr = false;
  static sdet_detector_params dp;
  static float max_gap = 0;
  vgui_dialog det_dialog("Video Edges");
  det_dialog.field("Gaussian sigma", dp.smooth);
  det_dialog.field("Noise Threshold", dp.noise_multiplier);
  det_dialog.checkbox("Automatic Threshold", dp.automatic_threshold);
  det_dialog.checkbox("Agressive Closure", agr);
  det_dialog.checkbox("Compute Junctions", dp.junctionp);
  det_dialog.checkbox("Edges vs Regions", edges_);
  if (!det_dialog.ask())
    return;
  if (agr)
    dp.aggressive_junction_closure=0;
  else
    dp.aggressive_junction_closure=-1;
  dp.maxGap = max_gap;

  if (edges_)
    video_process_  = new vpro_edge_process(dp);
  else
    video_process_  = new vpro_region_process(dp);
  sample_ = 2;
  if (live)
    this->start_live_video();
}

void vvid_live_video_dshow_manager::no_op()
{
  video_process_ = 0;
}

void vvid_live_video_dshow_manager::difference_frames()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
  }
  static vpro_frame_diff_params fdp;
  vgui_dialog frame_diff_dialog("Frame_Diff Params");
  frame_diff_dialog.field("Display Scale Range", fdp.range_);
  if (!frame_diff_dialog.ask())
    return;
  video_process_ = new vpro_frame_diff_process(fdp);
}

void vvid_live_video_dshow_manager::camera_calibration()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
  }
  vgui_dialog cal_dialog("Camera Calibration Process Params");
  vcl_string file_name = "";
  vcl_string ext = ".ply";
  cal_dialog.file("Calibration model file: ", ext, file_name);
  unsigned int num_pts = 6;
  cal_dialog.field("Number of points: ", num_pts);
  if (!cal_dialog.ask())
    return;
  if (num_pts < 6) {
    vcl_cout << "Minimum number of points is 6\n";
    return;
  }

  // cakibrate the first camera!!!!!!! for now
  vpro_camera_calibration_process* pro = new vpro_camera_calibration_process(file_name, num_pts);
  vcl_vector< vgl_homg_point_2d<double> > pts = vtabs_[0]->get_image_pts();
  //vtabs_[i]->get_image_pts(pts);
  vcl_cout << "pushed points size: " << pts.size() << " exiting if 0\n";
  if (pts.size() < 6) {
    vcl_cout << "In vvid_live_video_dshow_manager::camera_calibration() - points are not entered properly\n";
    return;
  }

  pro->set_img_points(pts);
  pro->execute();
  cam_calibs_[0] = pro->get_camera();
  video_process_ = pro;
}
void vvid_live_video_dshow_manager::clear_calibration()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    cam_calibs_[i] = 0;  // TODO: camera calibration process variables are not cleared!! 
    vt2Ds_[i]->clear_all();
    vt2Ds_[i]->post_redraw();
  }
}

void vvid_live_video_dshow_manager::clear_image_pts()
{
  for (unsigned i = 0; i<num_cameras_;i++)
    vtabs_[i]->clear_image_pts();
}

void vvid_live_video_dshow_manager::compute_harris_corners()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
  }
  //static int track_window;
  static sdet_harris_detector_params hdp;
  vgui_dialog harris_dialog("harris");
  harris_dialog.field("sigma", hdp.sigma_);
  harris_dialog.field("thresh", hdp.thresh_);
  harris_dialog.field("N = 2n+1, (n)", hdp.n_);
  harris_dialog.field("Max No.Corners(percent)", hdp.percent_corners_);
  harris_dialog.field("scale_factor", hdp.scale_factor_);
  //harris_dialog.checkbox("Tracks", track_);
  //harris_dialog.field("Window", track_window);

  if (!harris_dialog.ask())
    return;

  video_process_ = new vpro_harris_corner_process(hdp);
  //if (track_)
  //{
  //  frame_trail_.clear();
  //  frame_trail_.set_window(track_window);
 // }
}

/*
void vvid_live_video_dshow_manager::capture_sequence()
{
  this->stop_live_video();
  vgui_dialog save_video_dlg("Save Video Sequence");
  static vcl_string video_filename = "";
  static vcl_string ext = "*.*";
  save_video_dlg.file("Video Filename:", ext, video_filename);
  if (!save_video_dlg.ask())
    return;
  sample_ = 1;
  video_process_ = new vpro_capture_process(video_filename);
}

void vvid_live_video_dshow_manager::init_capture()
{
  this->stop_live_video();
  vgui_dialog save_video_dlg("Init Capture");
  static vcl_string video_directory = vul_file::get_cwd();
  static vcl_string ext = "*.*";
  static bool auto_increment = true;
  static vcl_string dir_prefix = "video";
  static int dir_index = 0;
  save_video_dlg.file("Video Directory:", ext, video_directory);
  save_video_dlg.checkbox("Automatically Create Incremental Subdirectories", auto_increment);
  save_video_dlg.field("Directory Prefix", dir_prefix);
  save_video_dlg.field("Current Directory Index", dir_index);
  vcl_stringstream complete_path;
  complete_path << "Complete Path: " << video_directory << '/' << dir_prefix << dir_index;
  save_video_dlg.message(complete_path.str().c_str());

  if (!save_video_dlg.ask())
    return;

  // if not a directory, use the base directory
  if (!vul_file::is_directory(video_directory))
    video_directory = vul_file::dirname(video_directory);

  vcl_string video_filename = video_directory;
  if (auto_increment){
    vcl_stringstream auto_dir;
    auto_dir << '/' << dir_prefix << dir_index++;
    video_filename += auto_dir.str();
    vul_file::make_directory(video_filename);
  }

  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vcl_stringstream camera_subdir;
    camera_subdir << "/camera" << i << '/';
    vul_file::make_directory(video_filename+camera_subdir.str());
    vtabs_[i]->start_capture(video_filename+camera_subdir.str());
  }
}

void vvid_live_video_dshow_manager::stop_capture()
{
  this->stop_live_video();
  for (unsigned i=0; i<num_cameras_; ++i)
    vtabs_[i]->stop_capture();
}
*/
void vvid_live_video_dshow_manager::toggle_histogram()
{
  if (histogram_) {
    histogram_ = false;
    for (unsigned i=0; i<num_cameras_; ++i)
      htabs_[i]->clear();
  }
  else
    histogram_ = true;
}

//: toggle mouse events
void vvid_live_video_dshow_manager::toggle_mouse()
{ 
  catch_mouse_ = !catch_mouse_; 
  for (unsigned i=0; i<num_cameras_; ++i)
      vtabs_[i]->set_mouse(catch_mouse_);
}

void vvid_live_video_dshow_manager::display_topology()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
    vcl_vector<vtol_topology_object_sptr> const & seg = video_process_->get_output_topology();

    for (vcl_vector<vtol_topology_object_sptr>::const_iterator ti=seg.begin();
         ti != seg.end(); ti++)
    {
      if (edges_)
      {
        vtol_edge_2d_sptr e=(*ti)->cast_to_edge()->cast_to_edge_2d();
        if (e)
          vt2Ds_[i]->add_edge(e);
      }
      else
      {
        vtol_face_2d_sptr f=(*ti)->cast_to_face()->cast_to_face_2d();
        if (f)
          vt2Ds_[i]->add_face(f);
      }
    }
  }
}

void vvid_live_video_dshow_manager::display_image()
{
  if (!video_process_)
    return;

  for (unsigned i=0; i<num_cameras_; ++i)
  { 
    if (vi2Ds_[i])
      vi2Ds_[i]->set_image(video_process_->get_output_image());
  }
}
void vvid_live_video_dshow_manager::display_spatial_objects()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();   
    vcl_vector<vsol_spatial_object_2d_sptr> const & objs = video_process_->get_output_spatial_objects();
    vt2Ds_[i]->add_spatial_objects(objs);
  }
}

void vvid_live_video_dshow_manager::run_frames()
{
  if (!init_successful_)
    return;
  while (vtabs_.back()->get_video_live()) {
    vul_timer t;
    for (unsigned i=0; i<num_cameras_; ++i)
    {
      vtabs_[i]->update_frame();

      if (histogram_)
        htabs_[i]->update(vtabs_[i]->get_rgb_frame());

      if (video_process_)//always grey scale
      {
        vil1_memory_image_of<unsigned char> image;
        video_process_->clear_input();

        if (vtabs_[i]->get_current_image(sample_,image))
          video_process_->add_input_image(image);
        else return;
        if (video_process_->execute())
        {
          if (video_process_->get_output_type()==vpro_video_process::IMAGE)
            display_image();
          if (video_process_->get_output_type()==vpro_video_process::TOPOLOGY)
            display_topology();
          if (video_process_->get_output_type()==vpro_video_process::SPATIAL_OBJECT)
            display_spatial_objects();
        }
      }

      vt2Ds_[i]->post_redraw();
    }
    vgui::run_till_idle();
    // delay until the minimum time has passed for this frame
    while (t.real()<min_msec_per_frame_) ;
    float ft = float(t.real())/1000.f, rate=1e33f;
    if (ft!=0.f) rate = 1.0f/ft;
    vgui::out << "Tf = " << ft << " sec/frame  = " << rate << " frs/sec\n";
  }
}

void vvid_live_video_dshow_manager::start_live_video()
{
  if (!init_successful_ || vtabs_.size() != num_cameras_)
    return;

  for (unsigned i=0; i<num_cameras_; ++i)
    if (!vtabs_[i]->start_live_video())
    {
      vcl_cout << "In vvid_live_video_dshow_manager::start_live_video() -"
               << " start failed - camera #" << i << vcl_endl;
      return;
    }
  this->run_frames();
}

void vvid_live_video_dshow_manager::stop_live_video()
{
  if (!init_successful_)
    return;

  for (unsigned i=0; i<num_cameras_; ++i)
    if (vtabs_[i])
      vtabs_[i]->stop_live_video();
  if (video_process_)
    video_process_->finish();
}

void vvid_live_video_dshow_manager::quit()
{
  for (unsigned i=0; i<num_cameras_; ++i)
  {
    vt2Ds_[i]->clear_all();
  }
  this->stop_live_video();
  vcl_exit(1);
}

bool vvid_live_video_dshow_manager::
get_current_image(unsigned camera_index,
                  int pix_sample_interval,
                  vil1_memory_image_of<unsigned char>& im)
{
  if (!init_successful_||!vtabs_[camera_index])
  {
    vcl_cout << "In vvid_live_video_manger::get_current_imge(..) -"
             << " bad initialization\n";
    return false;
  }

  return vtabs_[camera_index]->get_current_image(pix_sample_interval, im);
}

void vvid_live_video_dshow_manager::
set_process_image(vil1_memory_image_of<unsigned char>& im)
{
  process_mono_ = im;
}
