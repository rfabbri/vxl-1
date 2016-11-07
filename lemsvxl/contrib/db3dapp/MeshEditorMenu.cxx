/*************************************************************************
 *    NAME: Alexander K. Bowman
 *    USER: akb
 *    FILE: MeshEditorMenu.cxx
 *    DATE: 
 *************************************************************************/
#include "MeshEditorMenu.h"
#include <vgui/vgui.h> 

/*************************************************************************
 * Function Name: MeshEditorMenu::MeshEditorMenu
 * Parameters: 
 * Effects: 
 *************************************************************************/

MeshEditorMenu::MeshEditorMenu()
{

}

/*************************************************************************
 * Function Name: MeshEditorMenu::~MeshEditorMenu
 * Parameters: 
 * Effects: 
 *************************************************************************/

MeshEditorMenu::~MeshEditorMenu()
{
 
}

/*************************************************************************
 * Function Name: MeshEditorMenu::add_to_menu
 * Parameters:  vgui_menu & menu 
 * Returns: void
 * Effects: 
 *************************************************************************/
vgui_menu
MeshEditorMenu::add_to_menu(vgui_menu& topBar )
{
  
  
  vgui_menu fileMenu;
  fileMenu.add( "New...", fileNew );
  fileMenu.add( "Open...", fileOpen );
  fileMenu.add( "Save...", fileSave );
  fileMenu.separator();
  fileMenu.add( "Quit...", fileQuit );
  topBar.add( "File", fileMenu );

  vgui_menu addMenu;
  vgui_menu addNodeMenu;
    addNodeMenu.add( "Point", addNodeByPoint );
    addNodeMenu.add( "Dragger", addNodeByDragger );
  addMenu.add( "Node by", addNodeMenu );
  //addMenu.add( "Link...", addLink );
  //addMenu.add( "Face...", addFace );
  topBar.add( "Add", addMenu );

  vgui_menu optionsMenu;
  optionsMenu.add( "Display Options...", optionsDisplay );
  optionsMenu.add( "Color Options...", optionsColor );
  optionsMenu.add ("View show/hide...", options_view_show_hide);
  topBar.add( "Options", optionsMenu );

  vgui_menu actionMenu;
  actionMenu.add( "Interrogate", actionInterrogation );
  actionMenu.add( "Add", actionAddition );
  actionMenu.add( "Delete", actionDeletion );
  actionMenu.add( "Alteration", actionAlteration );
  topBar.add( "Action", actionMenu );
  return topBar;

  
}

