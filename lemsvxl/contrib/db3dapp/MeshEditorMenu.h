/**************************************************************/
/*  Name: Alexander Bowman akb
//  File: MeshEditorMenu.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MeshEditorMenu_h_
#define MeshEditorMenu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>

#include "MenuHandler.h"

class MeshEditorMenu {

public:

  MeshEditorMenu();
  virtual ~MeshEditorMenu();
  vgui_menu add_to_menu(vgui_menu & menu);

  // FILE MENU
  static void fileNew()     { MenuHandler::instance()->fileNew(); }
  static void fileOpen()    { MenuHandler::instance()->fileOpen(); }
  static void fileSave()    { MenuHandler::instance()->fileSave(); }
  static void fileQuit()    { MenuHandler::instance()->fileQuit(); }
  
  static void addNodeByPoint()     { MenuHandler::instance()->addNodeByPoint(); } 
  static void addNodeByDragger()     { MenuHandler::instance()->addNodeByDragger(); } 
  //static void addFace()     { MenuHandler::instance()->addFace(); } 

  static void actionInterrogation() { MenuHandler::instance()->actionInterrogation(); } 
  static void actionAddition()    { MenuHandler::instance()->actionAddition(); } 
  static void actionDeletion()    { MenuHandler::instance()->actionDeletion(); } 
  static void actionAlteration()   { MenuHandler::instance()->actionAlteration(); }

  static void optionsDisplay() { MenuHandler::instance()->optionsDisplay(); }
  static void optionsColor()   { MenuHandler::instance()->optionsColor(); }
  static void options_view_show_hide()    { MenuHandler::instance()->options_view_show_hide(); }

};

#endif
