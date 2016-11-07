#include "vehicle_model_menus.h"

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vgui/vgui.h>
#include <vgui/vgui_key.h>
#include <vgui/vgui_menu.h>
#include "vehicle_model_vis_manager.h"


void vehicle_model_menus::quit_callback()
    {
    vcl_exit(1);
    }

void vehicle_model_menus::load_image_callback()
    {
    vehicle_model_vis_manager::instance()->load_image();
    }

void vehicle_model_menus::set_range_params_callback()
    {
    vehicle_model_vis_manager::instance()->set_range_params();
    }

void vehicle_model_menus::pick_model_callback()
    {
    vehicle_model_vis_manager::instance()->pick_model();
    }

void vehicle_model_menus::adjust_model_callback()
    {
    vehicle_model_vis_manager::instance()->adjust_model();
    }

void vehicle_model_menus::translate_model_callback()
    {
    vehicle_model_vis_manager::instance()->translate_model();
    }

void vehicle_model_menus::load_ref_model_callback()
    {
    vehicle_model_vis_manager::instance()->load_ref_model();
    }

void vehicle_model_menus::load_intrinsic_mean_model_callback()
    {
    vehicle_model_vis_manager::instance()->load_intrinsic_mean_model();
    }

void vehicle_model_menus::save_ref_model_callback()
    {
    vehicle_model_vis_manager::instance()->save_ref_model();
    }

void vehicle_model_menus::save_adjusted_model_callback()
    {
    vehicle_model_vis_manager::instance()->save_adjusted_model();
    }

vgui_menu vehicle_model_menus::get_menu()
    {
    vgui_menu menubar;
    vgui_menu menufile;
    vgui_menu menuview;
    vgui_menu menuedit;
    vgui_menu menuprocess;

    //file menu entries
    menufile.add( "Load", load_image_callback);
    menufile.add( "Quit", quit_callback,(vgui_key)'q', vgui_CTRL);
    menufile.add("Save reference model",save_ref_model_callback);
    menufile.add("Save adjusted model",save_adjusted_model_callback);
    menufile.add("Load reference model",load_ref_model_callback);
    menufile.add("Load intrinsic mean model",load_intrinsic_mean_model_callback);

    //view menu entries

    menuview.add( "Set Range Parameters", set_range_params_callback);

    //edit menu entries
    menuedit.add("pick the model",pick_model_callback);
    menuedit.add("adjust the model",adjust_model_callback);
menuedit.add("translate the model",translate_model_callback);

    //Top level menu layout
    menubar.add( "File", menufile);
    menubar.add( "View", menuview);
    menubar.add( "Edit", menuedit);
#if 0
    menubar.add( "Edit", menuedit);
    menubar.add( "Process", menuprocess);
    menubar.add( "Debug", menudebug);
#endif
    return menubar;
    }

