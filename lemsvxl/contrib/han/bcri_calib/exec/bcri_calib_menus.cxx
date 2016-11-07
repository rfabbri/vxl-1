#include "bcri_calib_menus.h"

#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include "bcri_calib.h"
//#include <bmvl/bmvv/bmvv_multiview_manager.h>
//Static munu callback functions


//void bcri_calib_menus::init_kalman_callback()
//{
//  brrc_windows_frame::instance()->init_kalman();
//}

//bmvv_menus definition
vgui_menu bcri_calib_menus::get_menu()
{
   vgui_menu menu_bar;
  
   
   

   vgui_menu file_menu;
   //vgui_menu probe_menu;
   //vgui_menu debug_menu;
   vgui_menu manual_menu;
   //vgui_menu epipole_menu;
   //vgui_menu vd_menu;
   //vgui_menu knn_menu;
   //vgui_menu pts_menu;
   //vgui_menu probe_IO;
   //vgui_menu feat_menu;
   //vgui_menu auto_menu;
   //vgui_menu automatic_menu;
   
   
   file_menu.add("load image", load_image, (vgui_key)'l', vgui_CTRL);
   file_menu.add("clear image",clear_image);
   file_menu.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);


   manual_menu.add("select point",select_correspond);
         manual_menu.add("select point all",select_correspond_all);
   manual_menu.add("solve",solve);
          manual_menu.add("solve_reverse",solve_reverse);

         manual_menu.add("change",change,(vgui_key)'c');

         manual_menu.add("solve all",solve_all);
   manual_menu.add("back projection",back);
   manual_menu.add("back projection reverse",back_reverse);
         manual_menu.separator();

         manual_menu.add("**select pts1 pts2",select_pts1_pts2);
   manual_menu.add("**solve robust",solve_robust);


   //menu_bar.add("*Automatic", automatic_menu);
   menu_bar.add("File i/o", file_menu);

   menu_bar.add("manual", manual_menu);

   return menu_bar;
}

void  bcri_calib_menus::clear_image()
{
   bcri_calib::instance()->clear_display();
}
//void  bcri_calib_menus::clear_display()
//{
//   bcri_calib::instance()->clear_display();
//}

void bcri_calib_menus::quit_callback()
{
  bcri_calib::instance()->quit();
}

void bcri_calib_menus::load_image()
{
  bcri_calib::instance()->load_image();
}


void bcri_calib_menus::select_correspond()
{
  bcri_calib::instance()->select_correspond();
}


void bcri_calib_menus::select_correspond_all()
{
  bcri_calib::instance()->select_correspond_all();
}

void bcri_calib_menus::select_pts1_pts2()
{
        bcri_calib::instance()->read_3d_real_points();
  bcri_calib::instance()->select_pts1_pts2();
}
void bcri_calib_menus::solve()
{
  bcri_calib::instance()->read_3d_real_points();
  bcri_calib::instance()->solve();
}

void bcri_calib_menus::solve_reverse()
{
  bcri_calib::instance()->read_3d_real_points_reverse();
  bcri_calib::instance()->solve_reverse();
}

void bcri_calib_menus::solve_robust()
{
  
  bcri_calib::instance()->solve_robust();
}

void bcri_calib_menus::change()
{bcri_calib::instance()->clear_display();
        bcri_calib::instance()->change();
  bcri_calib::instance()->read_3d_real_points();
  bcri_calib::instance()->solve_quiet();
        
        bcri_calib::instance()->back_project();
}

void bcri_calib_menus::solve_all()
{
  bcri_calib::instance()->read_3d_real_points();
  bcri_calib::instance()->solve_all();
}

void bcri_calib_menus::back()
{
  bcri_calib::instance()->back_project();
}
void bcri_calib_menus::back_reverse()
{
  bcri_calib::instance()->back_project_reverse();
}

