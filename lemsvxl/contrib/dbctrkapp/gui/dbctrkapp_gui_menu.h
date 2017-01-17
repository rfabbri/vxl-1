//This is contrib/dbctrkapp/gui/dbctrkapp_gui_menu.h
#ifndef dbctrkapp_gui_menu_h_
#define dbctrkapp_gui_menu_h_

//:
// \file
// \brief dbctrkapp GUI Menu
// \author Amir Tamrakar
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>

//: menu class for dbctrkapp GUI
class dbctrkapp_gui_menu {

public:
  dbctrkapp_gui_menu();
  virtual ~dbctrkapp_gui_menu(){}

  vgui_menu setup_menu();

};

#endif //dbctrkapp_gui_menu_h_
