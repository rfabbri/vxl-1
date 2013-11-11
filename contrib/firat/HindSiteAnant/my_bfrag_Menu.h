/**************************************************************/
/*    Name: Daniel Spinosa
//    File: my_bfrag_Menu.h
//    Asgn: 
//    Date: 
 *
 *   This file is just for menu interaction, it is usually a pass
 *  through class, in that you click something and most of the work
 *  is done by the bfrag_Manager (who passes off bfrag2D stuff to the
 *  bfrag2D_Manager).
***************************************************************/

#ifndef my_bfrag_Menu_h_
#define my_bfrag_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include "bfrag_Manager.h"


class my_bfrag_Menu {

public:

    my_bfrag_Menu();
    virtual ~my_bfrag_Menu();

    void add_to_menu( vgui_menu & menu );
    
    //file
    static void load();
    static void save();
    static void quit();
    static void removeSelectedFragment();

    static void resetPairs();
    static void removeSelectedPairs();

    static void removeSeletedPuzzles();
    
    static void displayNPairs();
    static void displayCurrentPairs();
    static void displayAllPuzzleSolutions();
    static void displayNPuzzleSolutions();
    static void displayAllContours();
    static void displayNContours();
    static void displayNextPair();//deprecated
    static void displayContour();
    static void displayAssembly();

    //help
    static void about();
    static void help_contents();

protected:

private:
    //keeps track of last pair displayed, so user can see the next N pairs
    int whereToStart_;

};

#endif
