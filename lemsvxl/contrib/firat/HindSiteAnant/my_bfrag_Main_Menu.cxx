/**************************************************************/
/*    Name: Daniel Spinosa
//    File: bfrag_Main_Menu.cpp
//    Asgn: 
//    Date: 
***************************************************************/

#include "my_bfrag_Main_Menu.h"
#include "my_bfrag_Menu.h"


my_bfrag_Main_Menu::my_bfrag_Main_Menu()
{
}

vgui_menu my_bfrag_Main_Menu::setup_menu()
{
  vgui_menu MainMenu;

  /////////////// bfrag Menu //////////////
  my_bfrag_Menu menu;
  menu.add_to_menu( MainMenu );

  return MainMenu;
}
