#ifndef vis_manager_h_
#define vis_manager_h_
//:
// \file
// \brief  Manager for displaying file_format result
// \author Kongbin Kang and H. Can Aras
//

#include <vcl_vector.h>
#include <vgui/vgui_deck_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vil/vil_image_resource.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_cstdio.h>
#include <vpgl/algo/vpgl_project.h>
#include <xscan/xscan_scan.h>

class vgui_window;

class vis_manager : public vgui_wrapper_tableau
{
public:
  typedef vgui_wrapper_tableau base;
  vis_manager();
  ~vis_manager();
  static vis_manager *instance();
  void quit();
  void load_view_3d();
  void load_cali_bnd_views();
  void init();
  void set_range_params();
  void step_mode();
  void stop_step_mode();
  void cine_mode();
  void stop_cine_mode();
  //: 3D to 2D box projection
  //  The user should provide two files using the pop-up window to be used in this function.
  //  The file with BX3 extension should be created using "write" function of vgl_box_3d.
  //  The file with SCN extension should be created using << operator of xscan_scan
  void box_projection();

  //: save scan from rsq file
  void save_scan();

  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

protected:
  void add_image(vil_image_resource_sptr& image);
  //: Handle all events for this tableau.
  bool handle(vgui_event const &e);
  void get_pixel_info(const int x, const int y,vgui_event const &e, char* msg);

private:
  //: needed for creating fake event during cine mode
  int wx_, wy_;
  //: minimum and maximum intensity
  double min_int_;
  double max_int_;

  // isq or rsq file?
  int file_type_;

  //: a deck of image tableaux
  vcl_vector<vgui_image_tableau_sptr> img_tabs_;
  //: a deck of boxes
  vcl_vector< vgl_box_2d<double> > img_box_;

  //: scan
  xscan_scan scan_;

  bool stop_cine_;
  static vis_manager *instance_;
  vgui_window* win_;
  vgui_range_map_params_sptr range_params_;
  vgui_deck_tableau_sptr dtab_;
};

#endif // vis_manager_h_
