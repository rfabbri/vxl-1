#include <world_model/poly_hybrid_menu.h>
#include <world_model/poly_hybrid_manager.h>

#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

int main(int argc, char** argv)
{
  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl 
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // Set up the app_menu
  vgui_menu main_menu;
  poly_hybrid_menu app_menu;
  vgui_menu menu_holder = app_menu.add_to_menu (main_menu);
  
  vgui_grid_tableau_sptr grid = vgui_grid_tableau_new (3,1); 
  poly_hybrid_manager* mgr = poly_hybrid_manager::instance();
  mgr->setup_scene( grid );

  // Put the grid into a shell tableau at the top the hierarchy
  vgui_shell_tableau_new shell(grid);

  // Create a window, add the tableau and show it on screen.
  int result =  vgui::run(shell, 1000, 600, menu_holder, "poly" );
  
  delete poly_hybrid_manager::instance();

}
