//---------------------------------------------------------------------
//:
// \file   lemsvxlsrc/contrib/dbmsh3dapp/dbmsh3dapp_main.cxx
// \brief  Mesh Processing and Visualization Application.
//
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date   Feb 12, 2007
// 
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_timer.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>
#include <dbmsh3dr/vis/dbmsh3dr_cmdproc.h>

#include <Inventor/nodes/SoSeparator.h>


int main (int argc, char** argv)
{
  //parse command-line arugments. exit of argument is "-?".
  vul_arg_parse (argc, argv);
  int result = PRO_RESULT_SUCCESS;
  dbmsh3d_app_window_title = "LEMS 3D Mesh App";

  //initialize bgui_3d and inherented OpenInventor classes
  bgui3d_init(); 
  dbmsh3dvis_init_vispt_OpenInventor_classes();
  ///gdtvis_init_vispt_OpenInventor_classes();

  /////////////////////////////////////////////////////////////////////
  vul_timer total_timer;
  dbmsh3d_pro_vis* mpv0 = new dbmsh3d_pro_vis;
  dbmsh3d_pro_vis* mpv1 = new dbmsh3d_pro_vis;
  dbmsh3d_pro_vis* mpv2 = new dbmsh3d_pro_vis;
  dbmsh3dr_pro_vis* mpvr = new dbmsh3dr_pro_vis (mpv0, mpv1);
  
  mpv0->set_verbose (dbmsh3d_cmd_verbose());
  mpv1->set_verbose (dbmsh3d_cmd_verbose());
  mpv2->set_verbose (dbmsh3d_cmd_verbose());
  mpvr->set_verbose (dbmsh3d_cmd_verbose());

  //Check command-line parameters.
  bool task = false;
  task |= dbmsh3d_check_cmdproc (mpv0, argc, argv);
  dbmsh3d_setup_provis_parameters_0 (mpv0);
  dbmsh3d_setup_provis_parameters_1 (mpv1);
  task |= dbmsh3dr_check_cmdproc (mpvr, argc, argv);
  dbmsh3dr_setup_provis_parameters (mpvr);
  
  SoSeparator *root = new SoSeparator;
  root->ref();

  //Print message if no command-line process executed.
  if (task == false) {
    vul_printf (vcl_cout, "\nERROR in main():\tNo process specified!\n");
    vul_printf (vcl_cout, "\n\t Specify filename to view, or");
    vul_printf (vcl_cout, "\n\t use -h for more help.\n");
    dbmsh3d_cmd_gui() = 0;
    result = PRO_RESULT_NO_PROCESS;
  }  
  else {
    //Run the command-line process execution.
    root->addChild (dbmsh3d_cmdproc_execute (mpv0));
    root->addChild (dbmsh3dr_cmdproc_execute (mpvr));
    root->addChild (dbmsh3d_cmdproc_execute_2 (mpv2));
  }
  if (dbmsh3d_cmd_verbose()) {
    vcl_cerr << "\nTotal running time: " << total_timer.real() << " milliseconds.\n";
    total_timer.print(vcl_cout);
  }
  /////////////////////////////////////////////////////////////////////
  
  if (dbmsh3d_cmd_gui()) {
    //Run the specified cmd-line process with visualization in GUI Window.  
    //force option "--mfc-use-gl" to use gl in initializing vgui.    
    vul_printf (vcl_cout, "Starting bgui3d window...\n");
    int my_argc = argc+1;
    char** my_argv = new char*[argc+1];
    for (int i=0; i<argc; i++)
      my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init (my_argc, my_argv);
    delete []my_argv;
    
    //Wrap the scene graph in an examiner tableau
    bgui3d_examiner_tableau_new tab3d (root);
    root->unref ();
    //Put a shell tableau at the top of our tableau tree.
    vgui_shell_tableau_new shell (tab3d);
    //Create a window, add the tableau and show it on screen.
    result = vgui::run (shell, 1024, 768);
  }

  delete mpv0, mpv1, mpv2;
  delete mpvr;
  return result;
}

