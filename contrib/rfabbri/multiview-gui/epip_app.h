// This is epip_app_h
#ifndef epip_h
#define epip_h
//:
//\file
//\brief Tool to show corresponding epipolar lines
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 04/21/2005 04:53:53 PM EDT
//



#include <bvis1/bvis1_tool.h>

#include <vgui/vgui_easy2D_tableau_sptr.h> 
#include <vgui/vgui_viewer2D_tableau_sptr.h> 
#include <vgui/vgui_tableau_sptr.h> 
#include <vgui/vgui_tableau.h> 
#include <vgui/vgui_soview2D.h> 
#include <bpro1/bpro1_storage_sptr.h>
#include <vgui/vgui_event_condition.h>
#include <bgui/bgui_vsol2D_tableau_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>


void epip_interactive();
void point_transfer_app();


#define MANAGER bvis1_manager::instance()

//------------------------------------------------------------
// A tableau that displays the mouse position
// when left mouse button is pressed.
class epip_drawer_tableau : public vgui_tableau
{
public:
  epip_drawer_tableau();
  ~epip_drawer_tableau() { }

  void set_l_tableau(vgui_easy2D_tableau_sptr t) { tab_l_ = t; }
  void set_r_tableau(vgui_easy2D_tableau_sptr t) { tab_r_ = t; }
  void set_r_view(vgui_viewer2D_tableau_sptr m) {view_l_ = m;}
  void set_l_view(vgui_viewer2D_tableau_sptr m) {view_r_ = m;}
  void set_l_camera(vpgl_perspective_camera<double> *p) {p_l_ = p;}
  void set_r_camera(vpgl_perspective_camera<double> *p) {p_r_ = p;}
  void set_f_matrix(vpgl_fundamental_matrix<double> *m) {fm_ = m;}

  bool handle(const vgui_event &e);

protected:
  vgui_event_condition gesture0;
  vgui_soview2D_point *pt_;
  vgui_soview2D_infinite_line *soview_r_;

  vgui_easy2D_tableau_sptr tab_l_;
  vgui_easy2D_tableau_sptr tab_r_;
  vgui_viewer2D_tableau_sptr view_l_;
  vgui_viewer2D_tableau_sptr view_r_;

  vpgl_perspective_camera<double> *p_l_;
  vpgl_perspective_camera<double> *p_r_;
  vpgl_fundamental_matrix<double> *fm_;

  bool activated_;
  bool first_time_;
};

//-------------------------------------------------------------
// Make a smart-pointer constructor for our tableau.
struct epip_drawer_tableau_new : public vgui_tableau_sptr
{
  epip_drawer_tableau_new() : vgui_tableau_sptr(new epip_drawer_tableau()) { }
};

#endif // epip_app_h
