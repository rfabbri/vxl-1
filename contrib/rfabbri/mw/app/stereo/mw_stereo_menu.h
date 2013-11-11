#ifndef mw_stereo_menu_h_
#define mw_stereo_menu_h_

//:
// \file
// \brief Trimmed down Multiview GUI Menu
// \author Ricardo Fabbri
// \date Sat Apr 16 22:12:25 EDT 2005
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>


//: menu class for app GUI
class mw_stereo_menu {

public:
  mw_stereo_menu();
  virtual ~mw_stereo_menu(){}

  vgui_menu setup_menu();

};

#endif //mw_stereo_menu_h_
