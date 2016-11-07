//This is contrib/shock2d/gui/shock2d_gui_menu.h
#ifndef shock2d_gui_menu_h_
#define shock2d_gui_menu_h_

//:
// \file
// \brief Shock2d GUI Menu
// \author Amir Tamrakar
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>


//: menu class for Shock2d GUI
class shock2d_gui_menu {

public:
  shock2d_gui_menu();
  virtual ~shock2d_gui_menu(){}

  vgui_menu setup_menu();

};

#endif //shock2d_gui_menu_h_
