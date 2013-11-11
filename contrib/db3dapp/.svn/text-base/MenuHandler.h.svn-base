/**************************************************************/
/*  Name: Alexander Bowman
//  File: MenuHandler.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MenuHandler_h_
#define MenuHandler_h_

#include <vgui/vgui_menu.h>

class MenuHandler {

public:
  MenuHandler();
  virtual ~MenuHandler();
  static MenuHandler* instance();
  
  // FILE MENU
  void fileNew();                           
  void fileOpen();                          
  void fileSave();                          
  void fileQuit();                          

  // ADD MENU
  void addNodeByPoint();
  void addNodeByDragger();
  void addLink();
  void addFace();

  // OPTIONS MENU
  void optionsDisplay();
  void optionsColor();
  void options_view_show_hide();

  // ACTIONS
  void actionInterrogation();
  void actionAddition(); 
  void actionDeletion();
  void actionAlteration();

  void defaults();

  float getNodeSize() { return _nodeSize; }

private:
  static MenuHandler* instance_;
  vcl_string ext;
  vcl_string _fileToOpen;
  bool _isFileOpen;

  float _nodeSize;
  float _lineWidth;
  float _pointSize;

  int _nodeColor;
  int _linkColor;
  int _faceColor;


};

#endif
