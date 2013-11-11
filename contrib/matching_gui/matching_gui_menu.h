
//This is contrib/ozge/gui/matching_gui_menu.h
#ifndef matching_gui_menu_h_
#define matching_gui_menu_h_

//:
// \file
// \brief matching GUI Menu
// \author Amir Tamrakar
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>

//: menu class for matching GUI
class matching_gui_menu {

public:
  matching_gui_menu();
  virtual ~matching_gui_menu(){}

  vgui_menu setup_menu();

};

#endif //matching_gui_menu_h_
