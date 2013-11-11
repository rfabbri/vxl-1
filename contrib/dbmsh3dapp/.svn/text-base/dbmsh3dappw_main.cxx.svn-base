//---------------------------------------------------------------------
//:
// \file   lemsvxlsrc/contrib/dbmsh3dapp/dbmsh3dappw_main.cxx
// \brief  Mesh Processing and Visualization Application using SoWin for Windows.
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

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>
#include <dbmsh3d/vis/dbmsh3d_click_getinfo.h>
#include <dbmsh3dr/vis/dbmsh3dr_cmdproc.h>

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

  //vcl_string name = path->getTail()->getTypeId().getName().getString();
  //if (strcmp("SoIndexedLineSet", name.c_str())) {
  //   SoIndexedLineSet* line = (SoIndexedLineSet*) path->getTail();
  //   path->truncate(path->getLength() - 1 );
  //   SoSeparator* group = (SoSeparator*)path->getTail();
  //   group->removeChild(line);
  //}
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
  ///gdtvis_init_vispt_OpenInventor_classes();

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
  
  //Print message if no command-line process executed.
  if (task == false) {
    vul_printf (vcl_cout, "\nERROR in main():\tNo task specified!\n");
    vul_printf (vcl_cout, "\n\t Specify filename to view, or");
    vul_printf (vcl_cout, "\n\t use -h for more help.\n");
    dbmsh3d_cmd_gui() = 0;
    result = PRO_RESULT_NO_PROCESS;
  }
  else { //Run the command-line task execution.
    _root->addChild (dbmsh3d_cmdproc_execute (mpv0));
    _root->addChild (dbmsh3dr_cmdproc_execute (mpvr));
    _root->addChild (dbmsh3d_cmdproc_execute_2 (mpv2));
  }
  if (dbmsh3d_cmd_verbose()) {
    vcl_cerr << "\nTotal running time: " << total_timer.real() << " milliseconds.\n";
    total_timer.print(vcl_cout);
  }
  /////////////////////////////////////////////////////////////////////

  if (dbmsh3d_cmd_gui()) {
    //Use SoWinExaminerViewer as our main viewer
    vul_printf (vcl_cout, "Starting the Coin3D SoWin GUI...\n");
    SoWinExaminerViewer* eviewer = new SoWinExaminerViewer (mainwin);
    ///eviewer->setTransparencyType(SoGLRenderAction::ADD);
    eviewer->setTransparencyType (SoGLRenderAction::SORTED_OBJECT_BLEND);
    eviewer->setSceneGraph (_root);
    eviewer->setTitle (dbmsh3d_app_window_title.c_str());
    eviewer->setBackgroundColor (color_from_code (mpv0->bg_ccode_));
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

  delete mpv0, mpv1, mpv2;
  delete mpvr;
  return result;
}

