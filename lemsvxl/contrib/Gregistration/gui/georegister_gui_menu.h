//This is contrib/georegister/gui/georegister_gui_menu.h
#ifndef georegister_gui_menu_h_
#define georegister_gui_menu_h_

//:
// \file
// \brief georegister GUI Menu
// \author vj
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>

//: menu class for georegister GUI
class georegister_gui_menu {

public:
  georegister_gui_menu();
  virtual ~georegister_gui_menu(){}

  vgui_menu setup_menu();

};

#endif //georegister_gui_menu_h_
