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

#ifndef _pairMatchMainMenu_H__
#define _pairMatchMainMenu_H__

#include <vgui/vgui_menu.h>
#include "pairMatchMenu.h"


class pairMatchMainMenu {

public:
    pairMatchMainMenu(){};
    virtual ~pairMatchMainMenu(){}

    vgui_menu setupMenu();
  
private:


};

#endif
