//This is contrib/fine/gui/seg_gui_menu.h
#ifndef seg_gui_menu_h_
#define seg_gui_menu_h_

//:
// \file
// \brief seg GUI Menu
// \author Amir Tamrakar
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>


//: menu class for seg GUI
class seg_gui_menu {

public:
  seg_gui_menu();
  virtual ~seg_gui_menu(){}

  vgui_menu setup_menu();

};

#endif //seg_gui_menu_h_


