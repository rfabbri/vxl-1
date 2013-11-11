//This is bvis/menu/bvis_view_menu.h

#ifndef bvis_view_menu_header
#define bvis_view_menu_header

//:
// \file
// \brief The bvis_video_menu 
// \author Amir Tamrakar
// \date April 21, 2004
//
// This menu contains all the menus related to creating and managing multiple views of the data
//
// \verbatim
//  Modifications
//   Amir Tamrakar April 21, 2004
//    -Made it a subclass of vgui_menu to make it more flexible
//     There is no reason not to subclass it from vgui_menu
//    -This also removes the need to have a vgui_menu (items) defined inside this class
//   Eduardo Almeida March 24, 2007
//    - bvis_manager.h to bvis_video_manager.h
// \endverbatim

#include <bvis/bvis_video_manager.h>
#include <vgui/vgui_menu.h>
template <class T>
class bvis_view_menu : public vgui_menu
{

public:

  bvis_view_menu() {}
  virtual ~bvis_view_menu() {}

  void add_to_menu( vgui_menu & menu );

  static void add_view();
  static void remove_view();
  static void layer_per_view();
  
protected:

private:

};

#endif
