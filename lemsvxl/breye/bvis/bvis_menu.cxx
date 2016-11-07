/*************************************************************************
 *    NAME: Mark Johnson
 *    USER: mrj
 *    FILE: bvis_menu.cxx
 *    DATE: Thu Jul 17 14:25:19 2003
 *************************************************************************/
#include "bvis_menu.h"

/*************************************************************************
 * Function Name: bvis_menu::bvis_menu
 * Parameters: 
 * Effects: 
 *************************************************************************/

bvis_menu::bvis_menu() : 
	file_io_group_menu ("File I/O" ),
	image_adjustment_group_menu( "Image Adjustment" ), 
	edge_detection_group_menu( "Edge Detection" ), 
	shock_detection_group_menu( "Shock" ), 
	curve_group_menu( "Curve" ), 
	other_group_menu( "Other" ), 
	geom_group_menu( "Create" ), 
	shock_group_menu( "View" )
{
}

/*************************************************************************
 * Function Name: bvis_menu::setup_menu
 * Parameters: 
 * Returns: vgui_menu
 * Effects: 
 * Remarks: note the order!
 *************************************************************************/
vgui_menu bvis_menu::setup_menu()
{
	vgui_menu MainMenu;
	
	/////////////// File Menu ///////////////
	file_menu.add_to_menu( MainMenu );

	/////////////// Video Menu ///////////////
	video_menu.add_to_menu( MainMenu );
	
	/////////////// Processes Menu ///////////////
		file_io_group_menu.add_process (bvis_manager::manager()->process_manager()->get_process_by_name( "Load .CON Curve" ) );
		file_io_group_menu.add_process (bvis_manager::manager()->process_manager()->get_process_by_name( "Image I/O" ) );
	file_io_group_menu.add_to_menu( processes_menu );

		image_adjustment_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Brightness/Contrast" ) );
		image_adjustment_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Gaussian Blur" ) );
	image_adjustment_group_menu.add_to_menu( processes_menu );

		edge_detection_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Logical Linear" ) );
		edge_detection_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Susan" ) );
		edge_detection_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "ContourTracing" ) );
	edge_detection_group_menu.add_to_menu( processes_menu );

		shock_detection_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Shock" ) );
	shock_detection_group_menu.add_to_menu( processes_menu );

		curve_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Curve Matching" ) );
		curve_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Curve Averaging" ) );
	curve_group_menu.add_to_menu( processes_menu );

		other_group_menu.add_process(bvis_manager::manager()->process_manager()->get_process_by_name( "Harris Corners" ) );
	other_group_menu.add_to_menu( processes_menu );

	MainMenu.add( "Processes" , processes_menu );

	/////////////// Process Macro Menu ///////////////
	process_macro_menu.add_to_menu (MainMenu);

	/////////////// Batch Menu ///////////////
	batch_menu.add_to_menu( MainMenu );

	/////////////// Tools Menu ///////////////
		geom_group_menu.add_tool( bvis_tool_sptr( new bvis_line_tool ) );
		geom_group_menu.add_tool( bvis_tool_sptr( new bvis_point_tool ) );
		geom_group_menu.add_tool( bvis_tool_sptr( new bvis_polyline_tool ) );
		geom_group_menu.add_tool( bvis_tool_sptr( new bvis_polygon_tool ) );
		geom_group_menu.add_tool( bvis_tool_sptr( new bvis_translate_tool ) );
		shock_group_menu.add_to_menu( shock_tools );
		geom_group_menu.add_to_menu( geom_tools );
	tools.add( "Geometry" , geom_tools );

		shock_group_menu.add_tool( bvis_tool_sptr( new bvis_highlight_tool ) );
	tools.add( "Shock" , shock_tools );

	MainMenu.add( "Tools" , tools );

	/////////////// Tableau Menu ///////////////
	tableau_menu.add_to_menu( MainMenu );
	
	return MainMenu;
}


