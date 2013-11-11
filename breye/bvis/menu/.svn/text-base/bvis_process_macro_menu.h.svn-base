//This is bvis/menu/bvis_process_macro_menu.h

#ifndef bvis_process_macro_menu_header
#define bvis_process_macro_menu_header

//:
// \file
// \brief The bvis_process_macro_menu 
// \author Amir Tamrakar
// \date Nov 4, 2003
//
// \verbatim
//  Modifications
//    Amir Tamrakar April 21, 2004
//    -Made it a subclass of vgui_menu to make it more flexible
//     There is no reason not to subclass it from vgui_menu
//    -This also removes the need to have a vgui_menu (items) defined inside this class
// \endverbatim

#include <bvis/bvis_video_manager.h>
#include <bvis/bvis_mview_manager.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_text_tableau_sptr.h>
template <class T>
class bvis_process_macro_menu: public vgui_menu 
{

public:

  bvis_process_macro_menu();
  virtual ~bvis_process_macro_menu();

  void add_to_menu( vgui_menu & menu );
 
  static void start_recording();
  static void stop_recording();

  static void clear_all();
  static void delete_last();

  static void view_process_list();

protected:

private:

};

#endif
