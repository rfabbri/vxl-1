#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_grid_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>
#include <Inventor/nodes/SoSeparator.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_fileio.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>


int mesh_vis(dbmsh3d_mesh& fixed_mesh, dbmsh3d_mesh& moving_mesh)
{
  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl 
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);

  int my_argc = 2;
  char** my_argv = new char*[2];
  my_argv[0] = "mesh_vis";
  my_argv[1] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();

  // #################################################
  // Load and display meshes

  dbmsh3dvis_init_vispt_OpenInventor_classes ();

  // fixed mesh
  fixed_mesh.IFS_to_MHE();
  fixed_mesh.setup_IFS_mesh ();
  SoSeparator* fixed_vis = new SoSeparator;

  SbColor fixed_color = SbColor (1.0f, 0.1f, 0.1f);
  //// Draw each individual faces/edges/vertices as separate object to allow getInfo.
  //vis = draw_mesh_faces (&mesh, true, 0.0f, color);
  //root->addChild (vis);

  // Draw the whole mesh to a single object
  fixed_vis = draw_mesh(&fixed_mesh, true, 0.0f, fixed_color);
  root->addChild(fixed_vis);


  // moving mesh
  moving_mesh.IFS_to_MHE();
  moving_mesh.setup_IFS_mesh ();
  SoSeparator* moving_vis = new SoSeparator;

  SbColor moving_color = SbColor (0.1f, 1.0f, 0.1f);
  //// Draw each individual faces/edges/vertices as separate object to allow getInfo.
  //vis = draw_mesh_faces (&mesh, true, 0.0f, color);
  //root->addChild (vis);

  // Draw the whole mesh to a single object
  moving_vis = draw_mesh(&moving_mesh, true, 0.0f, moving_color);
  root->addChild(moving_vis);



  // #################################################

  // wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);
}

