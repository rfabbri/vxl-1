//This is bvis/menu/bvis_tableau_menu.h

#ifndef bvis_tableau_menu_header
#define bvis_tableau_menu_header

//:
// \file
// \brief The bvis_add_menu 
// \author Mark Johnson (mrj)
// \date Fri Aug  8 10:33:39 EDT 2003
//
// This menu is designed to house menu actions that are 
// designed to affect tableaux
// All tableau specific menu are to be added to this menu
//
// \verbatim
//  Modifications
//    Amir Tamrakar April 21, 2004
//    -Made it a subclass of vgui_menu to make it more flexible
//     There is no reason not to subclass it from vgui_menu
//    -This also removes the need to have a vgui_menu (items) defined inside this class
//   Eduardo Almeida March 24, 2007
//    - bvis_manager.h to bvis_video_manager.h
// \endverbatim

#include <bvis/bvis_video_manager.h>
#include <vgui/vgui_menu.h>
template <class T>
class bvis_tableau_menu  : public vgui_menu
{

public:

  bvis_tableau_menu() {}
  virtual ~bvis_tableau_menu() {}

  void add_to_menu( vgui_menu & menu );

  static void get_info_on_active_tableau();
  
  static void cache_tableaux();

  static void regenerate_tableaux();

  static void make_empty_storage();

  
protected:

private:

};

#endif
