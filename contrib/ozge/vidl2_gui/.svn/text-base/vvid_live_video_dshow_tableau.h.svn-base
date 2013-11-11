// This is brl/vvid/vvid_live_video_dshow_tableau.h
#ifndef vvid_live_video_dshow_tableau_h_
#define vvid_live_video_dshow_tableau_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A special image tableau that has a live camera as an image generator.
// \author
//   J.L. Mundy
//   04/16/2006 - O. C. Ozcanli added Direct Show support to the manager
//                Note: Direct Show is Windows based and opens all video streams 
//                      that Windows Media Player can open in your local machine
//                      (Necessary codec bundles should be installed apriori 
//                       to be able to open some formats)
//
//
// \verbatim
//  Modifications:
//   J.L. Mundy January 09, 2002    Initial version.
// \endverbatim
//--------------------------------------------------------------------------------
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgui/vgui_image_tableau.h>
//#include <vvid/vvid_live_video_dshow_tableau_sptr.h>
#include <ozge/vidl_gui/vvid_live_video_dshow_tableau_sptr.h>


//#include <vidl/vidl_istream.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
//#include <vidl/vidl_image_list_istream.h>
//#include <vidl/vidl_image_list_ostream.h>

#include <vil/vil_flip.h>
#include <vidl/vidl_dshow_file_istream.h>
#include <vidl/vidl_dshow_live_istream.h>
#include <vidl/vidl_dshow.h>
#include <vidl/vidl_dshow_istream_params.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_convert.h>

#include <vgui/vgui_event_condition.h>
#include <vgui/vgui_style_sptr.h>
#include <vgl/vgl_homg_point_2d.h>

class vvid_live_video_dshow_tableau : public vgui_image_tableau
{
 public:
  typedef vgui_image_tableau base;
  vvid_live_video_dshow_tableau();

  vvid_live_video_dshow_tableau(int node, int pixel_sample_interval,
                                vidl_istream *istream);
  ~vvid_live_video_dshow_tableau();
  vcl_string type_name() const;

  //:video camera properties
  //void set_camera_params(const vidl_dshow_istream_params& cp);
  //vidl_dshow_istream_params get_camera_params() { return cp_; }
  //bool video_capabilities(int format, int mode, int frame_rate) { return cam_.m_videoFlags[format][mode][frame_rate]; }

  // ? int get_current() { return cam_.get_current(); }
  // ? void set_current(int current) { cam_.set_current(current); }
  
  //vcl_string current_capability_desc() { return cam_.current_capability_desc(); }
  //vcl_vector<vcl_string> get_capability_descriptions() { return cam_.get_capability_descriptions(); }

  //:live video processing
  //bool attach_live_video();
  bool start_live_video();
  void update_frame();
  void stop_live_video();

  //:live capture methods
  //void start_capture(vcl_string const & video_file_name) { cam_.start_capture(video_file_name); }
  //bool stop_capture() { return cam_.stop_capture(); }
  //: collection state, i.e. is live and capturing frames
  bool get_video_live() const { return live_; }
  //bool get_video_rgb() const { return rgb_; }
  //bool get_video_flip_ud() const { return flip_ud_; }

  //: pixel sample interval for display
  void set_pixel_sample_interval(int pix_sample_itvl) { pixel_sample_interval_=pix_sample_itvl; }
  
  //: return rgb image in vil image and mono image in vil1 image
  void get_camera_image(vil_image_view<vxl_byte>& im_rgb,
                        vil1_memory_image_of<unsigned char>& im,
                        int pix_sample_interval = 1);

  void get_camera_rgb_image(vil_image_view<vxl_byte>& im,
                        int pix_sample_interval = 1);

  vcl_vector< vgl_homg_point_2d<double> >& get_image_pts() { return img_pts_; } 
  void get_image_pts(vcl_vector< vgl_homg_point_2d<double> >& pts) { pts = img_pts_; } 
  void clear_image_pts() { img_pts_.clear(); }

  // return current cached images with no resampling
  vil_image_view<vxl_byte> get_rgb_frame(){ return rgb_frame_; }
  vil1_memory_image_of<unsigned char> get_frame(){ return mono_frame_; }

  //: Access to current cached camera frames
  // Both styles of image access are supported

  //: New pointer to the image
  //vil1_memory_image_of<unsigned char> get_current_image(int pix_sample_interval);
  //vil_image_view<vxl_byte> get_current_rgb_image(int pix_sample_interval);

  //: User supplies the pointer to the image
  bool get_current_image(int pix_sample_interval,
                         vil1_memory_image_of<unsigned char>& im);

  bool get_current_rgb_image(int pix_sample_interval,
                             vil_image_view<vxl_byte>& im);

  //: toggle mouse events
  void set_mouse(bool mouse) { catch_mouse_ = mouse; }

 protected:
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);

 private:
  //status flags
  bool live_;//video is live
  //the live camera
  int node_;//the camera id
  vidl_istream *istream_;
  int pixel_sample_interval_;//default pixel sample spacing
  vil_image_view<vxl_byte> rgb_frame_;
  vil1_memory_image_of< unsigned char > mono_frame_;

  bool catch_mouse_;
  vgui_event_condition gesture_left;
  vgui_event_condition gesture_draw;
  vgui_style_sptr style_;

  vcl_vector< vgl_homg_point_2d<double> > img_pts_;
};

struct vvid_live_video_dshow_tableau_new : public vvid_live_video_dshow_tableau_sptr
{
 public:
  vvid_live_video_dshow_tableau_new() :
    vvid_live_video_dshow_tableau_sptr(new vvid_live_video_dshow_tableau()) {}

  vvid_live_video_dshow_tableau_new(int node, int pixel_sample_interval,
                                    vidl_istream * cp) :
    vvid_live_video_dshow_tableau_sptr(new vvid_live_video_dshow_tableau(node,
                                                                         pixel_sample_interval,
                                                                         cp))
  {}

  operator vgui_image_tableau_sptr () const { vgui_image_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // vvid_live_video_dshow_tableau_h_
