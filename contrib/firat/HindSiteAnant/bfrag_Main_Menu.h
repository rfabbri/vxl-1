/**************************************************************
 *    Name: Daniel Spinosa
 *    File: bfrag_Main_Menu.h
 *    Asgn: 
 *    Date: 
 *
 *     This is a dumb class, it was just in the template i used
 *   to get going, but when the time comes, i may just get rid
 *   of this (if its possible/easy & better than having this)
 *
 ***************************************************************/

#ifndef bfrag_Main_Menu_
#define bfrag_Main_Menu_

#include <vgui/vgui_menu.h>
#include "bfrag_Manager.h"


class bfrag_Main_Menu {

public:
  bfrag_Main_Menu();
  virtual ~bfrag_Main_Menu(){}

  vgui_menu setup_menu();
  
private:


};

#endif
