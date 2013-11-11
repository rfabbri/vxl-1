#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer3D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>


#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_window.h>
#include <vgui/vgui_adaptor.h>

#include "my_bfrag_Main_Menu.h"
#include "bfrag_Manager.h"

#include "params.h"

// see global.h for parameter descriptions
void initialize_params()
{
  // web system's algorithm-I parameters only
  PSI = 1.0;
  NUM_TIMES = 30;
  FINE_RESAMPLE_DS = 1.0;
  COARSE_RESAMPLE_DS = 25.0;
//  TRANSITION_STEEPNESS = 8;
  VICINITY = 35;
  MIN_TAN_TURN = 0.5236;
  DIST_STEP = 0.5;
  NUM_FINE_OUT = 5;
  NUM_COARSE_OUT = 50;
  MIN_MAP_SIZE_CRSE = 2;
  OLAP_THRESH_HIGH = 25.0;
  TH_D = 0.2;
  TH_L = 0.2;   
  TH_A = 0.2;   
  TH_LA = 100.0; 
  MIN_D = 30.0;  
  LAM = 1;     
  MAX_FINE_COST = 100.0;
  FINE_SAMPLE_SKIP = 5;
  LOCAL_REG_ITER_PAIR = 10;
  LENGTH_THRESH_LOW = 25.0;
  EDGE_COEF = -5.0;
  MIN_MAP_SIZE_FINE = 25;
  //ANGLE_SIM_THRESH = 0.2;
  // web system's algorithm-II parameters only
  LINEUP_DIST_THRESH = 15.0;
  OLAP_THRESH_LOW = 10.0; 
  IDENTICAL_BONUS = 20;
  CULLING_RANGE = 100;
  NUM_TOP_STATES_SAVED = 25;
  NUM_TOP_STATES_PROCESSED = 5;
  LOCAL_REG_ITER_PUZ = 25;
  CLOSED_JUNCTION_BONUS = 25;
  LENGTH_THRESH_HIGH = 50.0;
  DIST_THRESH_LOW = 3.0;
  DIST_THRESH_HIGH = 10.0;
  DIAG_THRESH_HIGH = 5.0;
  NUM_STATES_ITER = 12;
  // common parameters to both algorithms
  SMOOTH_EDGE_BONUS = 10.0;
  LOCAL_REG_THRESH_HIGH = 10.0;
  LOCAL_REG_THRESH_LOW = 1.0;
  NUM_POINTS_AVERAGED = 15.0;
  MOVE_BACK = 25.0;
  DISTANCE_THRESH_1 = 10;
  DISTANCE_THRESH_2 = 15;
  DIAG_THRESH_LOW = 2.0;
  DIAG_COEF = -5.0;
  LENGTH_COEF = -1;
  DIST_COEF = 20;
}

int main(int argc, char** argv)
{

  initialize_params();

  vgui::init(argc, argv);

  my_bfrag_Main_Menu menubar;
  vgui_menu menu_holder = menubar.setup_menu();
  unsigned w = 1024, h = 768;
  vcl_string title = "HindSite - Visualizer";
  vgui_window* win = vgui::produce_window(w, h, menu_holder, title);

  win->set_statusbar(true);
  win->enable_vscrollbar(false);
  win->enable_hscrollbar(false);
  win->set_statusbar(false);

  bfrag_Manager::instance()->setShell( vgui_shell_tableau_new() );
  win->get_adaptor()->set_tableau( bfrag_Manager::instance()->shell() );

  win->show();

  return vgui::run();

}


