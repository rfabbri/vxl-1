/**************************************************************/
/*    Name: Daniel Spinosa
//    File: bfrag_Main_Menu.cpp
//    Asgn: 
//    Date: 
***************************************************************/

#include "pairMatchMainMenu.h"

vgui_menu
pairMatchMainMenu::setupMenu()
{
  vgui_menu MainMenu;

  //add our menu
  pairMatchMenu menu;
  menu.add_to_menu( MainMenu );

  return MainMenu;
}
