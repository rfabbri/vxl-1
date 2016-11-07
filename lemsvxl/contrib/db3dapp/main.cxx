#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>

#include "MeshEditorMenu.h"
#include "SceneHandler.h"
#include "SceneEventHandler.h"

#include <dbmsh/vis/dbmsh_vis_shape.h>

#include <Inventor/nodes/SoSelection.h>


void click(void* userdata, SoPath* path)
{
  SceneEventHandler::instance()->click( path );
}

void unclick(void* userdata, SoPath* path)
{
  SceneEventHandler::instance()->unclick( path );
}


int main(int argc, char** argv)
{
  // initialize vgui
  vgui::init(argc, argv);

  // initialize bgui_3d
  bgui3d_init();

  // Set up the menu
  vgui_menu mainMenu;
  MeshEditorMenu menu;
  vgui_menu menu_holder = menu.add_to_menu( mainMenu );

  dbmsh_vis_shape::initClasses();
  // create the scene graph root
  SoSelection* root = new SoSelection;
  root->ref();
  SceneHandler::instance()->setRoot( root );
  root->addSelectionCallback( click );
  root->addDeselectionCallback( unclick );
  
  // wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d(root);

  SoCamera* camera = tab3d->camera();
  
  SceneHandler::instance()->setCamera( camera );

  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  int err =  vgui::run( shell, 1024, 768, menu_holder, "dbmsh" );
  

  return err;
  

}

