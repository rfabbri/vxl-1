#include <vcl_iostream.h>

#include <dbul/dbul_dir_file.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbmsh3d/dbmsh3d_mesh_mc.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>

#include <bgui3d/bgui3d.h>
#include <bgui3d/bgui3d_file_io.h>
#include <bgui3d/bgui3d_examiner_tableau.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/nodes/SoSeparator.h>


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

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();

  dbmsh3dvis_init_vispt_OpenInventor_classes ();

  // #################################################
  // Load and display a mesh file
  dbmsh3d_mesh_mc M;

  if (argc == 1) {
    vcl_cout << vcl_endl << "Usage: meshvis [in_file] [out_file]" << vcl_endl;
    vcl_cout << vcl_endl << "  View .xyz .p3d .ply .ply2 .wrl .iv .off .m files." << vcl_endl;
    vcl_cout << vcl_endl << "  Save optional mesh file specified in [out_file]." << vcl_endl;
  }
  else {
    //Use the Coin3d library to import VRML (wrl) or OpenInventor IV file.
    //For other mesh file, read it into our mesh data structure.
    bool view_vrml_iv = false;
   vcl_string input_file = argv[1];

    vcl_string suffix = dbul_get_suffix (input_file);
    if (suffix == ".ply2")
      dbmsh3d_load_ply2 (&M, input_file.c_str());
    else if (suffix == ".ply")
      dbmsh3d_load_ply (&M, input_file.c_str());
    else if (suffix == ".off")
      dbmsh3d_load_off (&M, input_file.c_str());
    else if (suffix == ".m")
      dbmsh3d_load_m (&M, input_file.c_str());
    else if (suffix == ".wrl" || suffix == ".WRL") {
      root->addChild (bgui3d_import_file (input_file));
      view_vrml_iv = true;
    }
    else if (suffix == ".iv" || suffix == ".IV") {
      root->addChild (bgui3d_import_file (input_file));
      view_vrml_iv = true;
    }

    if (view_vrml_iv == false) {

      M.IFS_to_MHE();
      M.build_face_IFS ();

      SoSeparator* vis = new SoSeparator;
      //Draw the whole mesh to a single object
      root->addChild (draw_M (&M, true)); 

      if (argc == 3) { //Save output file.
        vcl_string out_file = argv[2];
        suffix = dbul_get_suffix (out_file);
        if (suffix == ".ply2")
          dbmsh3d_save_ply2 (&M, out_file.c_str());
        else if (suffix == ".ply")
          dbmsh3d_save_ply (&M, out_file.c_str());
        else if (suffix == ".xml")
          dbmsh3d_save_xml (&M, out_file.c_str());
      }
    }

  }

  // #################################################

  // wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run (shell, 400, 400);
}


