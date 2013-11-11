#ifndef imgrv_manager_h_
#define imgrv_manager_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Manager for displaying imgr_view_3d views
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy March 09, 2005    Initial version.
// \endverbatim
//---------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgui/vgui_deck_tableau_sptr.h>
#include <vgui/vgui_range_map_params_sptr.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <vil/vil_image_resource.h>

class vgui_window;

class imgrv_manager : public vgui_wrapper_tableau
{
 public:
  imgrv_manager();
  ~imgrv_manager();
  static imgrv_manager *instance();
  void quit();
  void load_view_3d();
  void init();
  void set_range_params();
  void cine_mode();
  void stop_cine_mode();
  //: access to the window
  vgui_window* get_window(){return win_;}
  void set_window(vgui_window* win){win_=win;}

 protected:
  void add_image(vil_image_resource_sptr& image);

 private:
  bool stop_cine_;
  static imgrv_manager *instance_;
  vgui_window* win_;
  vgui_range_map_params_sptr range_params_;
  vgui_deck_tableau_sptr dtab_;
};

#endif // imgrv_manager_h_
