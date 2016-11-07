// This is contrib/knee_cartilage/dbknee_coord_cmd/dbknee_compute_local_coord_cmd_main.cxx

//:
// \file
// \brief Compute coordinate system on a knee joint
// \date Jan 1, 2007
//
// \verbatim
//  Modifications:
// \endverbatim.
 

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vnl/vnl_file_matrix.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

//: Note the order here!!

#include <dbknee/dbknee_coord.h>

#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_process_vis.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>

#include <dbknee/vis/dbknee_vis_coord.h>



#include <Inventor/SoDB.h>
#include <Inventor/Win/SoWin.h>     //<=== Specify the platform to be MS Windows
#include <Inventor/Win/viewers/SoWinExaminerViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/events/SoKeyboardEvent.h>

bool  CMD_VALIDATE = false;

//=====================================================
//: General visualization parameters.
int CMD_SHOW_GUI = 1;
int CMD_VIS = 0;
int  CMD_SHOW_ID = 0;
float CMD_SHOW_TNB = 0.0f;
float CMD_CUBE_SIZE = -1.0f;    //0.01f
float CMD_VERTEX_SIZE = -1.0f;  //0.03f;
float CMD_TRANSP = 0.0f;
float CMD_MS_TRANSP = 0.0f;
int  CMD_COLORCODE = COLOR_SILVER;
int  CMD_COLORCODE2 = COLOR_GOLD;
bool CMD_SHAPE_HINTS = true;
int  CMD_LIGHT = 0;     //default lighting.
int  CMD_BG_COL = 0; //1
int  CMD_GSIZE = 1;
int  CMD_SHOW_IDV = 0;      //draw each pointgene individually
int  CMD_VIS_INF = 0;
int  CMD_BND = 1;       //option for drawing boundary mesh/points.
float CMD_PRUNE_BOX_RATIO = 0.1f;
float CMD_VIEW_DX = -1.0f;      // -1 as un-specified value.
float CMD_VIEW_DY = -1.0f;
float CMD_VIEW_DZ = -1.0f;

bool CMD_DEL = 0;

//: Output Files
char* CMD_OP3D = NULL;
char* CMD_OIV = NULL;
char* CMD_OIV_BIN = NULL;
char* CMD_OVRML = NULL;
char* CMD_OVRML2 = NULL;

////: ================== THE SYSTEM PARAMETERS ==================
bool VIEW_P3D = false;

void eventcb (void* userdata, SoEventCallback* node)
{}


//: ================== THE MAIN FUNCTION ==================
//: Run results
#define RUN_RESULT_SUCCESS  0
#define RUN_RESULT_NO_PROCESS  -1
#define RUN_RESULT_FILE_READ_ERROR  -2

//: Ming: To enable click-selection, we need to use SoSelection instead of SoSeparator.
//:       This makes everything selectable.
SoSelection* _root;

int main (int argc, char **argv)
{
  // ==========================================================================
  // 1) Set up the command line variables
  // ==========================================================================
  
  // input files
  vul_arg<vcl_string > meshfile("-meshfile", "Mesh file", "");
  vul_arg<vcl_string > csfile("-csfile", "Coordinate system file", "");

  vul_arg<vcl_string > outmesh("-outmesh", "Ouput mesh file", "");
  
 
  vul_arg<int > show_gui("-show_gui", "Visualize results", 1);
  vul_arg_parse(argc, argv);
  
  // ================== SOME APPLICATION RELATED VARIABLES ==================
  vcl_string ApplicationTitle = "Knee Cartilage 3D Visualizer";

  //: ================== VARIABLES ==================
  
  VIS_COLOR_CODE bg_colorcode = COLOR_BLACK;
  VIS_COLOR_CODE gene_colorcode = COLOR_WHITE;
  
  // ==========================================================================
  // 2) Initializes SoWin library (and implicitly also the Coin
  //   library). Returns a top-level / shell window to use.
  
  // Ming: To register my own type into the system.
  //       See http://doc.coin3d.org/Coin/classSoType.html#d2 for help.
  // ==========================================================================

  SoDB::init();

  //Initialize homemade Coin3d OpenInventor classes.
  dbmsh3dvis_init_vispt_OpenInventor_classes();
  dbsk3dvis_init_vispt_OpenInventor_classes();
  
  HWND mainwin = SoWin::init(argc, argv, argv[0]);

  //: Ming: To enable click-selection, we need to use SoSelection instead of SoSeparator.
  //:       This makes everything selectable.
  _root = new SoSelection;
  _root->policy = SoSelection::SHIFT;

  _root->ref();

  // Nhon: increase line drawing width
  SoDrawStyle* style = new SoDrawStyle();
  style->lineWidth.setValue(2.0f);
  _root->addChild(style);


  // ---------- Setting General Processing Parameters -------------------------
  
  SoSeparator* vis;

  
  // ==========================================================================
  // 3) PROCESS TASKS
  // ==========================================================================
  
  // --------------------------------------------------------------------------
  // i. Load the mehes and draw them out
  // --------------------------------------------------------------------------

  // determine file name
  if (meshfile() == "")
  {
    vcl_cerr << "One mesh files is required."
      << "Use '-meshfile' option for input mesh file.\n";
    return RUN_RESULT_NO_PROCESS;
  }

  vcl_cout << "Mesh file = " << meshfile() << vcl_endl;
  dbmsh3d_mesh mesh;
  dbmsh3d_load(&mesh, meshfile().c_str());

  // Add to visualize
  vis = draw_M (&mesh, false, 0.0f, COLOR_SILVER);
  _root->addChild (vis);

  // --------------------------------------------------------------------------
  // ii. Load the coordinate systems and draw them out
  // --------------------------------------------------------------------------

  if (csfile() == "")
  {
    vcl_cerr << "One cartesian coordinate system file is required."
      << "Use '-csfile' option for input coordinate system file.\n";
    return RUN_RESULT_NO_PROCESS;
  }

  dbknee_cylinder_based_coord cs;
  dbknee_read_cs_file(csfile(), cs);
  vis = dbknee_draw_3_axes(cs);
  _root->addChild(vis);



  // --------------------------------------------------------------------------
  // iii. Convert all meshes to local coordinates
  // --------------------------------------------------------------------------

  cs.wc_to_local(&mesh);


  // Add to visualize
  vis = draw_M (&mesh, false, 0.0f, COLOR_RED);
  _root->addChild (vis);


  dbknee_cylinder_based_coord std_cs;
  std_cs.init_default();
  vis = dbknee_draw_3_axes(std_cs);
  _root->addChild(vis);
    

  // ==========================================================================
  // 4) SAVE RESULTS
  // ==========================================================================

  dbmsh3d_save_ply(&mesh, outmesh().c_str(), true);


  // ==========================================================================
  // 5) VIEW RESULTS. 
  // ==========================================================================
  
  if (show_gui() == 0) {
    vcl_cout << "Show gui = " << show_gui() << vcl_endl;
    //: Don't show the GUI Window.
    return RUN_RESULT_SUCCESS;
  }

  // --------------------------------------------------------------------------
  // i. Use SoWinExaminerViewer as our main viewer
  // --------------------------------------------------------------------------
  ApplicationTitle += " -- ";
  

  fprintf (stderr, "\nmain(): starting the Coin3D GUI...\n");
  
  SoWinExaminerViewer* eviewer = new SoWinExaminerViewer (mainwin);
  ///eviewer->setTransparencyType(SoGLRenderAction::ADD);
  eviewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
  eviewer->setSceneGraph(_root);
  eviewer->setTitle(ApplicationTitle.c_str());
  eviewer->setBackgroundColor (color_from_code (COLOR_BLACK));
  eviewer->show();

  // --------------------------------------------------------------------------
  // ii. Handle HOTKEY & Event Callbacks.
  // --------------------------------------------------------------------------

  SoEventCallback * cb = new SoEventCallback;
  //Here, pass the main SoWinExaminerViewer* object as parameter.
  cb->addEventCallback(SoKeyboardEvent::getClassTypeId(), eventcb, eviewer);
  _root->insertChild(cb, 0);

  // --------------------------------------------------------------------------
  // iii. Initialize the camera viewpoint
  // --------------------------------------------------------------------------
  
  //SoCamera *camera = eviewer->getCamera();
  //camera->orientation.setValue( SbRotation(SbVec3f(1, 0, 0), (M_PI/3.0f)) *
  //                              SbRotation(SbVec3f(0, 0, 1), (M_PI/4.0f)) );

  
  // --------------------------------------------------------------------------
  // iv. Show the XYZ Axis Cross.
  // --------------------------------------------------------------------------
  eviewer->setFeedbackVisibility(TRUE);
  
  

  // --------------------------------------------------------------------------
  // v. Pop up the main window. Loop until exit.
  // --------------------------------------------------------------------------
  SoWin::show(mainwin);
  SoWin::mainLoop();
    
  // ==========================================================================
  // 6) Clean up resources. Exit.
  // ==========================================================================
  _root->unref();
  return RUN_RESULT_SUCCESS;

}


