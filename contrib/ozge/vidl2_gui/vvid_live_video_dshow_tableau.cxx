// This is brl/vvid/vvid_live_video_dshow_tableau.cxx
#include "vvid_live_video_dshow_tableau.h"
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_soview2D.h>
//#include <vvid/vvid_live_video_dshow_manager.h>
#include <ozge/vidl_gui/vvid_live_video_dshow_manager.h>

vvid_live_video_dshow_tableau::vvid_live_video_dshow_tableau()
  : live_(false), pixel_sample_interval_(1), istream_(0), catch_mouse_(true)
{
  gesture_left = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
}


vvid_live_video_dshow_tableau::vvid_live_video_dshow_tableau(int node,
                                                 int pixel_sample_interval,
                                                 vidl_istream *istream)
{
  node_ = node;
  live_ = false;
  pixel_sample_interval_ = pixel_sample_interval;
  istream_ = istream;
  catch_mouse_ = true;
  gesture_left = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f);
}

vvid_live_video_dshow_tableau::~vvid_live_video_dshow_tableau()
{
  node_ = 0;
  live_ = false;
  pixel_sample_interval_ = 1;
  catch_mouse_ = false;
  if (!istream_)
    delete istream_;
}

vcl_string vvid_live_video_dshow_tableau::type_name() const
{
  return "xcv_image_tableau";//this name is hard coded in the vgui_viewer_tab
                             //bad arrangement but shouldn't cause problems
                             //if we don't use two specalized image tableaux at
                             //the same time
}

//: Handle all events for this tableau.
bool vvid_live_video_dshow_tableau::handle(vgui_event const &e)
{
  if (catch_mouse_ && gesture_left(e)) 
  {
    float ix, iy;
    vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
    //vgl_homg_point_2d<double> pt(double(ix), double(iy));
    img_pts_.push_back(vgl_homg_point_2d<double> (double(ix), double(iy)));
    vcl_cout << "pushed: " << int(vcl_floor(ix+0.5)) << " " << int(vcl_floor(iy+0.5)) << vcl_endl;
    vvid_live_video_dshow_manager::instance()->post_overlay_redraw();
  }

  if (catch_mouse_ && e.modifier == vgui_SHIFT && !img_pts_.empty()) {
    vvid_live_video_dshow_manager::instance()->post_overlay_redraw();
  }

  if (catch_mouse_ && e.type == vgui_DRAW_OVERLAY) {
    for (unsigned i = 0; i<img_pts_.size(); i++) {
      vgui_soview2D_point pt(img_pts_[i].x(), img_pts_[i].y());
      pt.set_style(style_);
      pt.draw();
    }
  }

  return base::handle(e); //currently just let the parent do the work
}

/* NOT SUPPORTED
void vvid_live_video_dshow_tableau::set_camera_params(const vidl_dshow_istream_params& cp)
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_dshow_tableau::set_camera_params() - warning, "
               << "no camera present, but param values were reassigned\n";
      return;
    }
    
  bool live =live_;
  if (live)
    this->stop_live_video();
  cam_->.set_params(cp);
  cam_.update_settings();

  if (live)
    this->start_live_video();
}*/
/*
bool vvid_live_video_dshow_tableau::attach_live_video()
{
  if (!cam_.get_camera_present())
    {
      vcl_cout << "In vvid_live_video_dshow_tableau::attach_live_video() - "
               << "no camera present\n";
      return false;
    }
  cam_.m_cameraInitialized = false;
  cam_.init(node_);
  vcl_cout << "The Camera Attributes: \n" << cam_ << " \n";
  return true;
}*/

bool vvid_live_video_dshow_tableau::start_live_video()
{
  if (!istream_)
    {
      vcl_cout << "In vvid_live_video_dshow_tableau::startlive_video() - "
               << "no camera present\n";
      live_ = false;
      return false;
    }
    if(!istream_->is_open())
    {
      vcl_cout << "In vvid_live_video_dshow_tableau::startlive_video() - "
               << "failed to start camera\n";
      live_ = false;
      return false;
    }
    
  live_ = true;

//  if (rgb_)
//    {
//    get_camera_rgb_image(rgb_frame_, pixel_sample_interval_);
//    this->set_image(rgb_frame_);
//    }
//  else
//    {
      //vcl_cout << "get image\n";
      get_camera_image(rgb_frame_, mono_frame_, pixel_sample_interval_);
      //vcl_cout << "got image\n";
      this->set_image(mono_frame_);
 //   }
  return true;
}

void vvid_live_video_dshow_tableau::update_frame()
{
  if(!live_)
    return;
  //if (rgb_)
  //  get_camera_rgb_image(rgb_frame_, pixel_sample_interval_);
  //else
  // update rgb image as well
    get_camera_image(rgb_frame_, mono_frame_, pixel_sample_interval_);
  this->reread_image();
  this->post_redraw();
}

void vvid_live_video_dshow_tableau::stop_live_video()
{
  if (!istream_)
    {
      vcl_cout << "In vvid_live_video_dshow_tableau::stop_live_video() - "
               << "no camera present\n";
      return;
    }
  live_=false;
}

void vvid_live_video_dshow_tableau::
get_camera_rgb_image(vil_image_view<vxl_byte>& im,
                     int pix_sample_interval)
{
  vil_image_view<vxl_byte> img;
  
  vcl_cout << "frame number: " << istream_->frame_number() << vcl_endl;
  vidl_frame_sptr frame = istream_->read_frame();
  
  if(frame && vidl_convert_to_view(*frame,img))
    //if (flip_ud_) {
    //static vil_image_view<vxl_byte> img_ud;
      
    //  img_ud = vil_flip_ud(img);
      //img_rgb = img_ud;
    //  im = img_ud;
    //} else 
      //img_rgb = img;
      im = img;

  // TODO: pixel sample interval conversion for now
  //im = vil1_from_vil_image_view(img_rgb);
}

bool vvid_live_video_dshow_tableau::
get_current_rgb_image(int pix_sample_interval,
                      vil_image_view<vxl_byte>& im)
{
  //get_camera_rgb_image(im,pix_sample_interval);
  im = &rgb_frame_;
  return true; 
}

//: return rgb image in vil image and mono image in vil1 image
void 
vvid_live_video_dshow_tableau::get_camera_image(vil_image_view<vxl_byte>& im_rgb,
                                                vil1_memory_image_of<unsigned char>& im,
                                                int pix_sample_interval)
{
  static vil_image_view<vxl_byte> img;
  static vil_image_view<unsigned char> img_mono;
  vidl_frame_sptr frame = istream_->read_frame();
  if(frame && vidl_convert_to_view(*frame,img))
    //if (flip_ud_) {
    //  static vil_image_view<vxl_byte> img_ud;
    //  img_ud = vil_flip_ud(img);
    //  im_rgb = img_ud;
    //  if (img_ud.nplanes() == 3)
    //    vil_convert_planes_to_grey(img_ud, img_mono);
    //  else
    //    img_mono = img_ud;
    //} else {
      im_rgb = img;
      if (img.nplanes() == 3)
        vil_convert_planes_to_grey(img, img_mono);
      else
        img_mono = img;
    //}

  // TODO: pixel sample interval conversion for now
  im = vil1_from_vil_image_view(img_mono);
}

bool vvid_live_video_dshow_tableau::
get_current_image(int pix_sample_interval,
                       vil1_memory_image_of<unsigned char>& im )
{
  im = mono_frame_;
  return true;
}
