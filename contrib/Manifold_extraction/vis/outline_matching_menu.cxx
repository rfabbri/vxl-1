#include "outline_matching_menu.h"

#include <bvis1/bvis1_manager.h>
#include <bvis1/menu/bvis1_command.h>

#include <dbgui/dbgui_utils.h>

#include <dbcvr/vis/dbcvr_curvematch_displayer.h>
// #include <dbcvr/pro/dbcvr_curvematch_process.h>

#include <vidpro1/vidpro1_process_manager.h>

#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_add_menu.h>
#include <bvis1/menu/bvis1_process_macro_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>

#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>
#include <bvis1/tool/bvis1_correspond_tools.h>



outline_matching_menu::outline_matching_menu()
{
}


vgui_menu
outline_matching_menu::setup_menu()
{
    vgui_menu MainMenu;

    /////////////// File Menu ///////////////
    bvis1_file_menu file_menu;

    //AMIR:added these to make loading and saving more intuitive
    vgui_menu load_menu;
    vgui_menu save_menu;

    //these have to be customized to each application which is why they cannot
    //be defined in bvis1_file_menu

 
    FILE_MENU_ADD_PROCESS_NAME( ".CON", "Load .CON File", load_menu);
   
    FILE_MENU_ADD_PROCESS_NAME( ".CON", "Save .CON File", save_menu);

    file_menu.add("Load..", load_menu);
    file_menu.add("Save..", save_menu);
    file_menu.separator();

    file_menu.add_to_menu( MainMenu );

    /////////////// Video Menu ///////////////
    bvis1_video_menu video_menu;
    video_menu.add_to_menu( MainMenu );

    /////////////// Repository Menu ////////////////////
    bvis1_repository_menu repository_menu;

    repository_menu.add_to_menu( MainMenu );

    /////////////// Add Menu ////////////////////
    bvis1_add_menu add_menu;

    add_menu.add_to_menu( MainMenu );

    /////////////// Processes Menu ///////////////
    vgui_menu processes_menu;
    MENU_ADD_PROCESS_NAME( "Curve Matching", processes_menu );
    MENU_ADD_PROCESS_NAME( "Lie contour matching", processes_menu );
    MENU_ADD_PROCESS_NAME( "shape articulation", processes_menu);
    MENU_ADD_PROCESS_NAME( "lie contour geodesic shapes", processes_menu);
    MENU_ADD_PROCESS_NAME( "lie contour mean", processes_menu);
    MENU_ADD_PROCESS_NAME( "lie contour discrete mean", processes_menu);
    MENU_ADD_PROCESS_NAME("lie contour geodesics",processes_menu);
    MENU_ADD_PROCESS_NAME("lie contour principal geodesics",processes_menu);
    MENU_ADD_PROCESS_NAME("lie contour shape classifier",processes_menu);
    MENU_ADD_PROCESS_NAME("lie spoke mean",processes_menu);
    MENU_ADD_PROCESS_NAME("lie spoke geodesics",processes_menu);
    MENU_ADD_PROCESS_NAME("lie spoke principal geodesics",processes_menu);
    MENU_ADD_PROCESS_NAME("lie spoke shape classifier",processes_menu);
    MENU_ADD_PROCESS_NAME("Flip spokes",processes_menu);

    MainMenu.add( "Processes" , processes_menu );


    /////////////// Process Macro Menu ///////////////
    bvis1_process_macro_menu process_macro_menu;
    process_macro_menu.add_to_menu (MainMenu);


    /////////////// Tableau Menu ///////////////
    bvis1_tableau_menu tableau_menu;
    tableau_menu.add_to_menu( MainMenu );

    /////////////// View Menu ////////////////
    bvis1_view_menu view_menu;
    view_menu.add_to_menu(MainMenu);

    return MainMenu;
}

