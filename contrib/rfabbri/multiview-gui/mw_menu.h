#ifndef mw_menu_h_
#define mw_menu_h_

//:
// \file
// \brief Multiview GUI Menu
// \author Ricardo Fabbri
// \date Sat Apr 16 22:12:25 EDT 2005
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>


//: menu class for app GUI
class mw_menu {

public:
  mw_menu();
  virtual ~mw_menu(){}

  vgui_menu setup_menu();

};

#endif //mw_menu_h_
