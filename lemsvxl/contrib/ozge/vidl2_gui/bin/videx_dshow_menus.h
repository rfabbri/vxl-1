#ifndef videx_dshow_menus_h_
#define videx_dshow_menus_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief the setup for menus for videx
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//   J.L. Mundy
//   Ozge Can Ozcanli adapted the code to use Direct Show libraries
//   May 4, 2006
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------
class videx_dshow_menus
{
 public:
  static void quit_callback();
//  static void set_camera_params_callback();
  static void set_detection_params_callback();
  static void no_op_callback();
  static void start_live_video_callback();
  static void stop_live_video_callback();
  static void toggle_histogram_callback();
  static void toggle_mouse_callback();
  static void difference_frames_callback();
  static void clear_calibration_callback();
  static void camera_calibration_callback();
  static void compute_harris_corners_callback();
//  static void capture_sequence_callback();
//  static void init_capture_callback();
//  static void stop_capture_callback();
  static void reset_camera_link_callback();
  static vgui_menu get_menu();
 private:
  videx_dshow_menus() {}
};
#endif // videx_dshow_menus_h_
