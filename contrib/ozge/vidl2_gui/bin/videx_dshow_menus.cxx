#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
//#include <vvid/vvid_live_video_dshow_manager.h>
#include <ozge/vidl_gui/vvid_live_video_dshow_manager.h>
//#include <vvid/bin/videx_dshow_menus.h>
#include <ozge/vidl_gui/bin/videx_dshow_menus.h>

//Static munu callback functions

void videx_dshow_menus::quit_callback()
{
  vvid_live_video_dshow_manager::instance()->quit();
}

/*
void videx_dshow_menus::set_camera_params_callback()
{
  vvid_live_video_dshow_manager::instance()->set_camera_params();
}
*/
void videx_dshow_menus::set_detection_params_callback()
{
  vvid_live_video_dshow_manager::instance()->set_detection_params();
}

void videx_dshow_menus::no_op_callback()
{
  vvid_live_video_dshow_manager::instance()->no_op();
}

void videx_dshow_menus::clear_calibration_callback() 
{
  vvid_live_video_dshow_manager::instance()->clear_calibration();
}
void videx_dshow_menus::camera_calibration_callback()
{
  vvid_live_video_dshow_manager::instance()->camera_calibration();
}

void videx_dshow_menus::start_live_video_callback()
{
  vvid_live_video_dshow_manager::instance()->start_live_video();
}

void videx_dshow_menus::stop_live_video_callback()
{
  vvid_live_video_dshow_manager::instance()->stop_live_video();
}

void videx_dshow_menus::toggle_histogram_callback()
{
  vvid_live_video_dshow_manager::instance()->toggle_histogram();
}
void videx_dshow_menus::toggle_mouse_callback()
{
  vvid_live_video_dshow_manager::instance()->toggle_mouse();
}
void videx_dshow_menus::difference_frames_callback()
{
  vvid_live_video_dshow_manager::instance()->difference_frames();
}
void videx_dshow_menus::compute_harris_corners_callback()
{
  vvid_live_video_dshow_manager::instance()->compute_harris_corners();
}
/*
void videx_dshow_menus::capture_sequence_callback()
{
  vvid_live_video_dshow_manager::instance()->capture_sequence();
}

void videx_dshow_menus::init_capture_callback()
{
  vvid_live_video_dshow_manager::instance()->init_capture();
}

void videx_dshow_menus::stop_capture_callback()
{
  vvid_live_video_dshow_manager::instance()->stop_capture();
}
*/

//videx_menus definition
vgui_menu videx_dshow_menus::get_menu()
{
  vgui_menu menubar;
  vgui_menu menufile;
  vgui_menu menuview;
  vgui_menu menuedit;
  //file menu entries
  menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
//  menufile.add( "Capture(old)", capture_sequence_callback);
//  menufile.add( "Init Capture", init_capture_callback, (vgui_key)'i');
//  menufile.add( "Stop Capture", stop_capture_callback, (vgui_key)'e');

  //view menu entries
  menuview.add( "Start Live Video", start_live_video_callback, (vgui_key)'s');
  menuview.add( "Stop Live Video", stop_live_video_callback, (vgui_key)'s', vgui_CTRL);
  menuview.add( "Toggle Histogram", toggle_histogram_callback, (vgui_key)'h');
  menuview.add( "Toggle Catch Mouse", toggle_mouse_callback, (vgui_key)'m');

  //edit menu entries
//  menuedit.add("Camera Settings", set_camera_params_callback, (vgui_key)'p', vgui_CTRL);
  menuedit.add("Edge Detection Settings", set_detection_params_callback);
  menuedit.add("Frame Difference Settings", difference_frames_callback);
  menuedit.add("Compute Harris Corners", compute_harris_corners_callback);
  menuedit.add("Calibrate Cameras", camera_calibration_callback);
  menuedit.add("Clear Cameras", clear_calibration_callback);
  menuedit.add("No Live Operation", no_op_callback);

  //Top level menu layout
  menubar.add( "File", menufile);
  menubar.add( "View", menuview);
  menubar.add( "Edit", menuedit);
  return menubar;
}

