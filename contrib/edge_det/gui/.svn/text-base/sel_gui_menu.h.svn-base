
//This is contrib/shock2d/gui/sel_gui_menu.h
#ifndef sel_gui_menu_h_
#define sel_gui_menu_h_

//:
// \file
// \brief Symbolic edge linking GUI Menu
// \author Amir Tamrakar
// \date 02/14/05
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <dbgui/dbgui_utils.h>
#include <bvis1/bvis1_manager.h>

//: menu class for sel GUI
class sel_gui_menu {

public:
  sel_gui_menu();
  virtual ~sel_gui_menu(){}

  vgui_menu setup_menu();

};

//: \brief The command for printing to ps
class sel_print_command : public vgui_command
{
 public:
  sel_print_command(){}
  void execute()
  {
    dbgui_utils::render_to_ps(bvis1_manager::instance());
  }

};

#endif //sel_gui_menu_h_
