// GUI main program for itrack

#include <vgui/vgui.h>
#include <vgui/vgui_adaptor.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/internals/vgui_accelerate.h>
#include "imatch_menu.h"
#include <bvis1/bvis1_macros.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_displayer_sptr.h>
#include <vidpro1/vidpro1_process_manager_sptr.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/vidpro1_repository.h>

// Storage type header files
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <dbinfo/pro/dbinfo_osl_storage.h>
// Displayer header files
#include <bvis1/displayer/bvis1_image_displayer.h>
#include <bvis1/displayer/bvis1_vsol2D_displayer.h>
#include <bvis1/displayer/bvis1_vtol_displayer.h>
#include <dbinfo/vis/dbinfo_osl_displayer.h>
#include <dbbgm/vis/dbbgm_distribution_image_displayer.h>
// Process header files
#include <vidpro1/process/vidpro1_load_image_process.h>
#include <vidpro1/process/vidpro1_load_video_process.h>
#include <dbinfo/pro/dbinfo_load_osl_process.h>
#include <dbinfo/pro/dbinfo_save_osl_process.h>
#include <vidpro1/process/vidpro1_save_video_process.h>
#include <dbinfo/pro/dbinfo_imatch_minfo_process.h>
int main(int argc, char** argv)
{
  vgui::init(argc, argv);
  
  // Register the displayer
  REG_DISPLAYER( bvis1_image_displayer );
  REG_DISPLAYER( bvis1_vsol2D_displayer );
  REG_DISPLAYER( bvis1_vtol_displayer );
  REG_DISPLAYER( dbinfo_osl_displayer);
  // Register the storage types
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( vidpro1_vsol2D_storage );
  REG_STORAGE( dbinfo_osl_storage );

  // Register all the processes
  REG_PROCESS( vidpro1_load_image_process );
  REG_PROCESS( vidpro1_load_video_process );
  REG_PROCESS( vidpro1_save_video_process );
  REG_PROCESS( dbinfo_load_osl_process );
  REG_PROCESS( dbinfo_save_osl_process );
  REG_PROCESS( dbinfo_imatch_minfo_process );

#if defined(VCL_WIN32)
vcl_cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();
_setmaxstdio(2048);
#endif
  
  imatch_menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 600, h = 512;
  vcl_string title = "imatch GUI";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);
#if 0
  bvis1_manager::instance()->add_new_frame();
#endif
  win->get_adaptor()->set_tableau( bvis1_manager::instance() );
  win->set_statusbar(true);
  win->show();
  return vgui::run();
}
