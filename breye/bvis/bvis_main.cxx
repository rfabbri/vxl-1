/*************************************************************************
 *    name: mark johnson
 *    user: mrj
 *    file: bvis_main.cxx
 *    date: tue jun 24 10:50:22 2003
 *************************************************************************/

#include <bvis/bvis_manager.h>

#include <vidpro/vidpro_process_manager.h>
#include <vidpro/vidpro_process_manager_sptr.h>
#include <vidpro/vidpro_repository.h>
#include <vidpro/vidpro_repository_sptr.h>
#include <vidpro/vidpro_image_storage.h>
#include <vidpro/vidpro_shock_storage.h>
#include <vidpro/vidpro_curve_storage.h>
#include <vidpro/vidpro_curvematch_storage.h>
#include <vidpro/vidpro_vsol_storage.h>
#include <bvis/bvis_manager_storage.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_macro.h>
#include <vgui/vgui_find.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <bvis/bvis_menu.h>

#include <vidpro/vidpro_parameters.h>

#include <vidpro/vidpro_video_process_sptr.h>
#include <vidpro/vidpro_con_load_process.h>
#include <vidpro/vidpro_imageIO_process.h>
#include <vidpro/vidpro_brightness_contrast_process.h>
#include <vidpro/vidpro_harris_corners_process.h>
#include <vidpro/vidpro_gaussian_blur_process.h>
#include <vidpro/vidpro_shock_process.h>
#include <vidpro/vidpro_curvematch_process.h>
#include <vidpro/vidpro_curveaverage_process.h>
#include <vidpro/vidpro_logical_linear_process.h>
#include <vidpro/vidpro_susan_process.h>
#include <vidpro/vidpro_curvematch_process.h>
#include <vidpro/vidpro_contourtracing_process.h>


int main( int argc , char** argv )
{
  vgui::init( argc , argv );
  
  vidpro_repository_sptr repository_sptr( new vidpro_repository() );
  repository_sptr->register_type(vidpro_image_storage_new());
  repository_sptr->register_type(vidpro_vsol_storage_new());
  repository_sptr->register_type(vidpro_shock_storage_new());
  repository_sptr->register_type(vidpro_curve_storage_new());
  repository_sptr->register_type(vidpro_curvematch_storage_new());
  repository_sptr->register_type(bvis_manager_storage_new());

  //initialize the process manager and register all the processes
  vidpro_process_manager_sptr process_manager( new vidpro_process_manager(argc , argv , repository_sptr) );

  process_manager->register_process( vidpro_video_process_sptr( new vidpro_imageIO_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_con_load_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_brightness_contrast_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_harris_corners_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_logical_linear_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_susan_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_gaussian_blur_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_shock_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_curvematch_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_curveaverage_process() ) );
  process_manager->register_process( vidpro_video_process_sptr( new vidpro_contourtracing_process() ) );


  //initialize the gui manager
  bvis_manager::initialize( argc , argv , process_manager , repository_sptr );
  bvis_menu menu;
  
  if( bvis_manager::manager()->commandline_mode() ) {
   vcl_cerr << "Command Line Mode" << vcl_endl;
  } else {

    vgui_menu menu_holder = menu.setup_menu();

    vgui_window * win = vgui::produce_window( bvis_manager::manager()->get_width() , bvis_manager::manager()->get_height() ,
                                              menu_holder , "Brown Eyes" );

    win->get_adaptor()->set_tableau( vgui_shell_tableau_new( vgui_viewer2D_tableau_new( bvis_manager::manager()->get_tableau() ) ) );
    win->set_statusbar( true );
    win->show();
    vgui::run();
  
  }
  return 0;
}


