#ifndef Poly_Coin3d_Menu_h_
#define Poly_Coin3d_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "poly_coin3d_manager.h" 

class poly_coin3d_menu 
{
public:
  poly_coin3d_menu () {
  }
  virtual ~poly_coin3d_menu() {
  }
  vgui_menu add_to_menu (vgui_menu& menu);

  //: FILE MENU
  static void create_poly()   { poly_coin3d_manager::instance()->create_poly(); }
  static void proj_poly()  { poly_coin3d_manager::instance()->proj_poly(); }
  static void extrude_poly()  { poly_coin3d_manager::instance()->extrude_poly(); }
  static void draw_line()  { poly_coin3d_manager::instance()->draw_line(); }
  static void deselect_all(){poly_coin3d_manager::instance()->deselect_all();}
  static void print_selected_vertex(){poly_coin3d_manager::instance()->print_selected_vertex();}
  static void clear_poly()  { poly_coin3d_manager::instance()->clear_poly(); }
  static void clear_all()  { poly_coin3d_manager::instance()->clear_all(); }
  static void exit()  { poly_coin3d_manager::instance()->exit(); }
  static void extrude_coin3d()  { poly_coin3d_manager::instance()->extrude_coin3d(); }
  static void divide_coin3d()  { poly_coin3d_manager::instance()->divide_coin3d(); }
};

#endif
