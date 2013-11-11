#ifndef bcri_calib_menus_h_
#define bcri_calib_menus_h_
//-----------------------------------------------------------------------------
//:
// \file
// \brief This file is adopted from Joe's work.
//  This is essentially a namespace for static methods;
//  there is no public constructor
// \author
//  han
//
//-----------------------------------------------------------------------------

#include <vvid/vvid_file_manager.h>
#include <vvid/bin/vidfpl_menus.h>


#include <vgui/vgui_menu.h>
class bcri_calib_menus
{
public:
  //static void load_status();
  //static void save_status();
  //static void create_line();
  //static void show_epipole();
  static void load_image();
  static void clear_image();  
  //static void kalman_go_callback();
  
  static void quit_callback();
  static void back();
  static void back_reverse();
  

  static void select_correspond();
        static void select_correspond_all();
        static void select_pts1_pts2();
  static void solve();
          static void solve_reverse();
        static void solve_robust();
        static void change();

        static void solve_all();
  static vgui_menu get_menu();
   
   
private:
   bcri_calib_menus() {}
};

#endif // bcri_calib_menus_h_


