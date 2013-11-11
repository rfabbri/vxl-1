//: LEMS 3D Mesh & Shock Processor using SoWin
// \file
// \brief LEMS 3D Mesh & Shock Processor in Coin3D (OpenInventor).
//
// \author 
//    Ming-Ching Chang (mcchang@lems.brown.edu)
//    Part of the code is a migration of Frederic Leymarie's (leymarie@lems.brown.edu)
//    work on 3D Shocks. 
//
// \date 07/26/2004
//
// \verbatim
//  Modifications:
//    MingChing Chang,  Feb to April 2004, Initial version.
//
// \endverbatim.
 
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_timer.h>

//Note the order here
#include <dbmsh3d/vis/dbmsh3d_click_getinfo.h>
#include <dbmsh3d/cmdproc/dbmsh3d_cmdproc.h>
#include <dbmsh3dr/vis/dbmsh3dr_cmdproc.h>

#include <dbgdt3d/vis/dbgdt3d_vis_backpt.h>
#include <dbgdt3d/cmdproc/dbgdt3d_cmdproc.h>

#include <dbsk3d/algo/dbsk3d_fs_fileio.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_fileio.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>
#include <dbsk3d/pro/dbsk3d_process.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_click_getinfo.h>
#include <dbsk3d/cmdproc/dbsk3d_cmdproc.h>
#include <dbsk3dr/vis/dbsk3dr_cmdproc.h>

#include <Inventor/SoDB.h>
#include <Inventor/Win/SoWin.h>  //<=== Platform: MS Windows
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/events/SoKeyboardEvent.h>

SoSelection* _root; //use SoSelection to enable click-selection.

extern dbsk3d_pro* _spv_;

void select_callback (void* data, SoPath* path)
{
  if (dbsk3d_output_object_info (path) == false)
    dbmsh3d_output_object_info (path);
  _root->touch(); //to redraw (add selection bounding box)
}

void deselect_callback (void* data, SoPath* path)
{
  _root->touch(); //to redraw (remove selection bounding box)
}

void click_to_delete (void* data, SoPath* path)
{
  vcl_string name = path->getTail()->getTypeId().getName().getString();
  //Remove the visualization of this hitObject.
  SoShape* hitObject = (SoShape*) path->getTail();
  path->truncate (path->getLength() - 1);
  SoSeparator* sep = (SoSeparator*) path->getTail();
  sep->removeChild (hitObject);
}

void click_to_delete_shocks (void* data, SoPath* path)
{
  SoShape* hitObject = (SoShape*) path->getTail();
  //vcl_string name = hitObject->getTypeId().getName().getString();

  if (hitObject->getTypeId() == ms_sheet_SoIndexedFaceSet::getClassTypeId()) {
    //Click to delete a ms_sheet MS.
    ms_sheet_SoIndexedFaceSet* MS_vis = (ms_sheet_SoIndexedFaceSet*) hitObject;
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MS_vis->element();

    if (MS->have_icurve_chain() == false)
      vul_printf (vcl_cout, "\tWarning: Deleting MS %d with icurve_chain!!\n", MS->id());

    //Mark all FF of this MS invalid and delete them.
    //Need to handle the shared_F of MS.
    vcl_set<dbmsh3d_face*> FF_to_trim;
    MS->get_F_set (FF_to_trim, true);

    //Remove the ms_sheet.
    _spv_->ms_hypg()->remove_S_complete_hypg (MS);

    perform_trim_xform (_spv_->fs_mesh(), FF_to_trim);
    /*vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim.begin();
    for (; it != FF_to_trim.end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
      FF_prune_pass_Gs (FF);
    }
    _spv_->fs_mesh()->del_invalid_FFs_complete();*/

    //Remove the visualization of this hitObject.
    path->truncate (path->getLength() - 1);
    SoSeparator* sep = (SoSeparator*) path->getTail();
    sep->removeChild (hitObject);
  }
  else if (hitObject->getTypeId() == fs_face_SoFaceSet::getClassTypeId()) {
    //Click to delete a fs_face FF.
    fs_face_SoFaceSet* FF_vis = (fs_face_SoFaceSet*) hitObject;
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FF_vis->element();

    vul_printf (vcl_cout, "\tDelete FF %d and isolated FE's and FV's.\n", FF->id());
    FF_prune_pass_Gs (FF);
    _spv_->fs_mesh()->remove_F_complete (FF);

    //Remove the visualization of this hitObject.
    path->truncate (path->getLength() - 1);
    SoSeparator* sep = (SoSeparator*) path->getTail();
    sep->removeChild (hitObject);
  }
}

void click_to_del_shocks_save_files ()
{
  if (_spv_->ms_hypg()->is_modified()) {
    //Re-compute fs_edge and fs_vertex types.
    _spv_->ms_hypg()->fs_mesh()->compute_all_FEs_FVs_type(); 

    if (dbsk3d_cmd_dels()==2 || dbsk3d_cmd_dels()==3) {
      //-dels 2,3: Re-compute a valid coarse-scale ms_hypg from the fs_mesh.
      _spv_->reset_ms_hypg ();
      bool r = _spv_->build_ms_hypg ();
      assert (r);
    }

    vcl_string cms_file;
    if (dbsk3d_cmd_cms_ofile())
      cms_file = dbul_get_dir_file (dbsk3d_cmd_cms_ofile());
    else
      cms_file = _spv_->dir_file() + "-dels";

    vul_printf (vcl_cout, "Writing CMS and FS files %s...\n", cms_file.c_str());
    save_to_cms (_spv_->ms_hypg(), (cms_file + ".cms").c_str());
    save_to_fs (_spv_->ms_hypg()->fs_mesh(), (cms_file + ".fs").c_str());

    if (dbmsh3d_cmd_ofile()) {
      _spv_->build_ms_graph_sa_from_cms ();
      save_to_sg (_spv_->sg_sa(), (cms_file + ".sg").c_str());
    }
  }
  else if (_spv_->fs_mesh()->is_modified()) {
    //Re-compute fs_edge and fs_vertex types.
    _spv_->fs_mesh()->compute_all_FEs_FVs_type(); 

    vcl_string fs_file;
    if (dbsk3d_cmd_fs_ofile())
      fs_file = dbul_get_dir_file (dbsk3d_cmd_fs_ofile());
    else
      fs_file = _spv_->dir_file() + "-dels";

    vul_printf (vcl_cout, "Writing FS file %s...\n", fs_file.c_str());
    save_to_fs (_spv_->fs_mesh(), (fs_file + ".fs").c_str());
  }
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

int main (int argc, char **argv)
{
  //parse command-line arugments. exit of argument is "-?".
  vul_arg_parse (argc, argv);
  int result = PRO_RESULT_SUCCESS;
  dbmsh3d_app_window_title = "LEMS 3D Mesh and Shock App Win32";

  //Initialize homemade Coin3d OpenInventor classes.
  //see http://doc.coin3d.org/Coin/classSoType.html#d2.
  SoDB::init();
  dbmsh3dvis_init_vispt_OpenInventor_classes();
  gdtvis_init_vispt_OpenInventor_classes();
  dbsk3dvis_init_vispt_OpenInventor_classes();

  HWND mainwin = SoWin::init(argc, argv, argv[0]);
  _root = new SoSelection;
  _root->ref();
  _root->policy = SoSelection::SHIFT;

  if (dbsk3d_cmd_dels()) //Enable click-to-delete shocks.
    _root->addSelectionCallback (click_to_delete_shocks);
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
  dbgdt3d_pro_vis* gpv = new dbgdt3d_pro_vis ();
  dbsk3d_pro_vis* spv0 = new dbsk3d_pro_vis;
  dbsk3d_pro_vis* spv1 = new dbsk3d_pro_vis;
  dbsk3dr_pro_vis* spvr = new dbsk3dr_pro_vis (spv0, spv1);

  mpv0->set_verbose (dbmsh3d_cmd_verbose());
  mpv1->set_verbose (dbmsh3d_cmd_verbose());
  mpv2->set_verbose (dbmsh3d_cmd_verbose());
  mpvr->set_verbose (dbmsh3d_cmd_verbose());
  gpv->set_verbose (dbmsh3d_cmd_verbose());
  spv0->set_verbose (dbmsh3d_cmd_verbose());
  spv1->set_verbose (dbmsh3d_cmd_verbose());
  spvr->set_verbose (dbmsh3d_cmd_verbose());

  _init_click_to_del_shock_ptr (spv0);
  
  //Run the command-line process execution.
  bool task = false;
  task |= dbmsh3d_check_cmdproc (mpv0, argc, argv);
  dbmsh3d_setup_provis_parameters_0 (mpv0);
  dbmsh3d_setup_provis_parameters_1 (mpv1);
  task |= dbmsh3dr_check_cmdproc (mpvr, argc, argv);
  dbmsh3dr_setup_provis_parameters (mpvr);
  task |= dbsk3d_check_cmdproc (spv0, argc, argv);  
  dbgdt3d_setup_provis_parameters (gpv);
  task |= dbgdt3d_check_cmdproc (gpv, argc, argv);
  dbsk3d_setup_provis_parameters (spv0);
  task |= dbsk3dr_check_cmdproc (spvr, argc, argv);
  dbsk3dr_setup_provis_parameters (spvr);
  
  //Print message if no command-line process executed.
  if (task == false) {
    vul_printf (vcl_cout, "ERROR in main():\tNo process specified!\n");
    vul_printf (vcl_cout, "\n\t Specify filename to view, or");
    vul_printf (vcl_cout, "\n\t use -h for more help.\n");
    dbmsh3d_cmd_gui() = 0;
    result = PRO_RESULT_NO_PROCESS;
  }
  else {  //Run the command-line task execution.
    _root->addChild (dbmsh3d_cmdproc_execute (mpv0));
    _root->addChild (dbmsh3dr_cmdproc_execute (mpvr));
    _root->addChild (dbmsh3d_cmdproc_execute_2 (mpv2));
    _root->addChild (dbgdt3d_cmdproc_execute (gpv));
    _root->addChild (dbsk3d_cmdproc_execute (spv0));
    _root->addChild (dbsk3dr_cmdproc_execute (spvr));
  }
  if (dbmsh3d_cmd_verbose()) {
    vcl_cerr << "\nTotal running time: " << total_timer.real() << " milliseconds. ";
    total_timer.print(vcl_cout);
    vcl_cout << vcl_endl;
  }
  /////////////////////////////////////////////////////////////////////

  if (dbmsh3d_cmd_gui()) {
     //Use SoWinExaminerViewer as our main viewer
    fprintf (stderr, "Starting the Coin3D SoWin GUI...\n");
    SoWinExaminerViewer* eviewer = new SoWinExaminerViewer (mainwin);
    ///eviewer->setTransparencyType(SoGLRenderAction::ADD);
    eviewer->setTransparencyType (SoGLRenderAction::SORTED_OBJECT_BLEND);
    eviewer->setSceneGraph (_root);
    eviewer->setTitle (dbmsh3d_app_window_title.c_str());
    eviewer->setBackgroundColor (color_from_code (spv0->bg_ccode_));
    eviewer->show();
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
    
    //Save interactive editing results on shocks to file.
    // load .fs or .cms to perform operation.
    // Specify -ocms -ofs for output files, or the output would be prefix-dels.        
    if (dbsk3d_cmd_dels()) //-dels 1: click to delete shocks. 
      click_to_del_shocks_save_files ();

    //Save visualization to iv or wrl file.
    dbmsh3d_save_vis_to_iv_wrl (_root);
    _root->unref();
  }
  
  delete mpv0, mpv1, mpv2;
  delete mpvr;
  delete spv0, spv1;
  delete spvr;
  return result;
}


