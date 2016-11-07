//This is bvis/bvis_add_menu.h

#ifndef bvis_add_menu_h_
#define bvis_add_menu_h_

//:
// \file
// \brief The bvis_add_menu 
// \author Mark Johnson (mrj)
// \date Thu Jul 17 14:41:27 EDT 2003
//
// \verbatim
//  Modifications
//    Amir Tamrakar April 21, 2004
//    -Made it a subclass of vgui_menu to make it more flexible
//     There is no reason not to subclass it from vgui_menu
//    -This also removes the need to have a vgui_menu (items) defined inside this class
// \endverbatim


#include <vgui/vgui_menu.h>

class bvis_add_menu: public vgui_menu 
{

public:

  bvis_add_menu();
  virtual ~bvis_add_menu();

  void add_to_menu( vgui_menu & menu );

  static void add_new_frame();
  
protected:

private:

};

#endif
