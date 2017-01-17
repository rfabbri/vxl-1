// GUI menu for dbsksp_gui
#include "elbow_segmentation_gui_menu.h"
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <dbgui/dbgui_utils.h>

#include <bvis1/menu/bvis1_command.h>
#include <bvis1/menu/bvis1_file_menu.h>
#include <bvis1/menu/bvis1_video_menu.h>
#include <bvis1/menu/bvis1_process_macro_menu.h>
#include <bvis1/menu/bvis1_tableau_menu.h>
#include <bvis1/menu/bvis1_view_menu.h>
#include <bvis1/menu/bvis1_repository_menu.h>
#include <bvis1/menu/bvis1_add_menu.h>

#include <bvis1/bvis1_tool.h>
#include <bvis1/tool/bvis1_vsol2D_tools.h>
#include <bvis1/tool/bvis1_soview2D_tools.h>


elbow_segmentation_gui_menu::elbow_segmentation_gui_menu()
{
}

static void render_ps()
{
	dbgui_utils::render_to_ps(bvis1_manager::instance());
}

vgui_menu
elbow_segmentation_gui_menu::setup_menu()
{
	vgui_menu MainMenu;

	/////////////// File Menu ///////////////
	bvis1_file_menu file_menu;


	//AMIR:added these to make loading and saving more intuitive
	vgui_menu load_menu;
	vgui_menu save_menu;

	//these have to be customized to each application which is why they cannot
	//be defined in bvis1_file_menu

	FILE_MENU_ADD_PROCESS_NAME("Load 3D volume", "Load 3D volume", load_menu);
	FILE_MENU_ADD_PROCESS_NAME("Load 3D edges", "Load 3D edges", load_menu);
	//FILE_MENU_ADD_PROCESS_NAME( "Image", "Load Image", load_menu);

	FILE_MENU_ADD_PROCESS_NAME("Save 3D edges", "Save 3D edges", save_menu);


	file_menu.add("Load..", load_menu);
	file_menu.add("Save..", save_menu);
	//file_menu.add("Save to PostScript", render_ps);
	file_menu.separator();
	file_menu.add_to_menu( MainMenu );

	vgui_menu tools_menu;
	MENU_ADD_PROCESS_NAME( "Run 3D edge detector", tools_menu );
	MENU_ADD_PROCESS_NAME( "Run 3D edge detector (threaded)", tools_menu );
	MENU_ADD_PROCESS_NAME( "Compute largest edge cluster", tools_menu );
	tools_menu.separator();
	MENU_ADD_PROCESS_NAME( "Run slice-by-slice levelset segmentation", tools_menu );
	tools_menu.separator();
	MENU_ADD_PROCESS_NAME( "3D reconstruction from edges (Poisson)", tools_menu );
	MENU_ADD_PROCESS_NAME( "Mesh post-processing for visualization", tools_menu );

	MainMenu.add("Processes", tools_menu);

	vgui_menu vis_menu;
	MENU_ADD_PROCESS_NAME( "Show 2D slices", vis_menu );
	MENU_ADD_PROCESS_NAME( "Show 3D mesh", vis_menu );
	MainMenu.add("Visualization", vis_menu);


	return MainMenu;
}


