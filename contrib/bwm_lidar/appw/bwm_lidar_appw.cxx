//---------------------------------------------------------------------
//:
// \file   bwm_lidar_appw.cxx
// \brief  Brown World Model Lidar Meshing Application (for Windows).
//
// \author Ming-Ching Chang (mcchang@lems.brown.edu)
// \date   Dec 13, 2007
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

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_click_getinfo.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>

#include <bwm_lidar/algo/bwm_lidar_mesh.h>
#include <bwm_lidar/pro/bwm_lidar_cmdpara.h>
#include <bwm_lidar/vis/bwm_lidar_cmdproc.h>
#include <bwm_lidar/vis/bwm_lidar_vis.h>

#include <Inventor/SoDB.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/events/SoKeyboardEvent.h>
#include <Inventor/Win/SoWin.h>
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>

SoSelection* _root; //use SoSelection to enable click-selection.

void select_callback (void* data, SoPath* path)
{
  dbmsh3d_output_object_info (path);
  _root->touch(); //to redraw (add selection bounding box)
}

void deselect_callback (void* data, SoPath* path)
{
  _root->touch(); //to redraw (remove selection bounding box)
}

//: Handle human interaction: click and delete a drawing object.
void click_to_delete (void* data, SoPath* path)
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();

  SoShape* object = (SoShape*) path->getTail();
  path->truncate(path->getLength() - 1 );
  SoSeparator* group = (SoSeparator*) path->getTail();
  group->removeChild (object);
}

void event_callback (void* data, SoEventCallback* node)
{
  const SoEvent * event = node->getEvent();
  SoWinExaminerViewer * eviewer = (SoWinExaminerViewer*) data;
  SbColor col, new_col;

  //1)Key "<" for decrease the background intensity
  if (SO_KEY_PRESS_EVENT(event, PERIOD)) {
    col = eviewer->getBackgroundColor ();
    //background brightness += 10;
    float v = 10.0/256;
    new_col = SbColor (col[0]+v,col[1]+v,col[2]+v);

    if (col[0]+v>1)
      new_col = SbColor (1, 1, 1);
    node->setHandled();
  }
  //2)Key ">" for increase the background intensity
  else if (SO_KEY_PRESS_EVENT(event, COMMA)) {
    col = eviewer->getBackgroundColor ();
    //background brightness -= 10;
    float v = 10.0/256;
    new_col = SbColor (col[0]-v,col[1]-v,col[2]-v);

    if (col[0]-v<0)
      new_col = SbColor (0, 0, 0);
    node->setHandled();
  }

  //A)Change backgound intensity.
  if (new_col != col) {
    eviewer->setBackgroundColor (new_col);
  }
}

int main (int argc, char** argv)
{
  //parse command-line arugments. exit of argument is "-?".
  vul_arg_parse (argc, argv);
  int result = PRO_RESULT_SUCCESS;
  dbmsh3d_app_window_title = "LEMS 3D Mesh App SoWin";

  //Initialize homemade Coin3d OpenInventor classes.
  //see http://doc.coin3d.org/Coin/classSoType.html#d2.
  SoDB::init();
  dbmsh3dvis_init_vispt_OpenInventor_classes();

  HWND mainwin = SoWin::init(argc, argv, argv[0]);
  _root = new SoSelection;
  _root->ref();
  _root->policy = SoSelection::SHIFT;
  
  if (dbmsh3d_cmd_del()) //Enable click-to-delete.
    _root->addSelectionCallback (click_to_delete);
  else {
    _root->addSelectionCallback (select_callback, NULL);
    _root->addDeselectionCallback (deselect_callback, NULL);
  }

  /////////////////////////////////////////////////////////////////////
  vul_timer total_timer;
  
  //Check command-line parameters.
  bool task = false;
  task |= bwm_lidar_check_cmdproc (argc, argv);
  
  //Print message if no command-line process executed.
  if (task == false) {
    vul_printf (vcl_cout, "\nERROR in main():\tNo task specified!\n");
    vul_printf (vcl_cout, "\n\t Specify filename to view, or");
    vul_printf (vcl_cout, "\n\t use -h for more help.\n");
    dbmsh3d_cmd_gui() = 0;
    result = PRO_RESULT_NO_PROCESS;
  }
  else { //Run the command-line task execution.
    _root->addChild (bwm_lidar_cmdproc_execute ());
  }
  vcl_cerr << "\nTotal running time: " << total_timer.real() << " milliseconds.\n";
  total_timer.print(vcl_cout);
  /////////////////////////////////////////////////////////////////////

  if (dbmsh3d_cmd_gui()) {
    //Use SoWinExaminerViewer as our main viewer
    vul_printf (vcl_cout, "Starting the Coin3D SoWin GUI...\n");
    SoWinExaminerViewer* eviewer = new SoWinExaminerViewer (mainwin);
    ///eviewer->setTransparencyType(SoGLRenderAction::ADD);
    eviewer->setTransparencyType (SoGLRenderAction::SORTED_OBJECT_BLEND);
    eviewer->setSceneGraph (_root);
    eviewer->setTitle (dbmsh3d_app_window_title.c_str());
    eviewer->setBackgroundColor (color_from_code (COLOR_WHITE)); ///mpv0->bg_ccode_));
    eviewer->show ();
    //Handle HOTKEY & Event Callbacks.
    SoEventCallback* cb = new SoEventCallback;
    //Here, pass the main SoWinExaminerViewer* object as parameter.
    cb->addEventCallback (SoKeyboardEvent::getClassTypeId(), event_callback, eviewer);
    _root->insertChild (cb, 0);
    //Show the XYZ Reference Axis.
    eviewer->setFeedbackVisibility (true);
    //Pop up the main window. Loop until exit.
    SoWin::show (mainwin);
    SoWin::mainLoop ();
   
    //Save visualization to iv or wrl file.
    dbmsh3d_save_vis_to_iv_wrl (_root);
    _root->unref();
  }

  return result;
}

