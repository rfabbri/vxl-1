//This is /contrib/biotree/proc/vis/vis_manager.h

#ifndef bioproc_vis_manager_h_
#define bioproc_vis_manager_h_
//: 
// \file    vis_manager.h
// \brief   manager for bioproc process
// \author  Kongbin Kang and H. Can Aras
// \date    2005-05-04
// 

#include <vcl_vector.h>
#include <vgui/vgui_deck_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vil/vil_image_resource.h>
#include <vgui/vgui_image_tableau_sptr.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <imgr/file_formats/imgr_isq_file_format.h>
#include <vcl_cstdio.h>

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
  void init();
  void add_box();
  void set_range_params();
  void step_mode();
  void stop_step_mode();
  void cine_mode();
  void stop_cine_mode();
  void save_box();
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
  //: minium and maxium intensity
  double min_int_;
  double max_int_;

  // isq or rsq file?
  int file_type_;

  //: a deck of image tableaus
  vcl_vector<vgui_image_tableau_sptr> img_tabs_;

  bool stop_cine_;
  static vis_manager *instance_;
  vgui_window* win_;
  vgui_range_map_params_sptr range_params_;
  vgui_deck_tableau_sptr dtab_;

  bgui3d_examiner_tableau_sptr etab_;

  imgr_isq_file_format *isq_;

  //: store corn point of drawing box
  float x1_, y1_, x2_, y2_;

  //: store picked corner points
  vcl_vector<vgl_point_3d<double> > corners_;

  vgl_box_3d<double> bounding_box_;

};

#endif // vis_manager_h_
