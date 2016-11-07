/**************************************************************/
/*    Name: Daniel Spinosa
//    File: bfrag_Menu.h
//    Asgn: 
//    Date: 
 *
 *   This file is just for menu interaction, it is usually a pass
 *  through class, in that you click something and most of the work
 *  is done by the bfrag_Manager (who passes off bfrag2D stuff to the
 *  bfrag2D_Manager).
***************************************************************/

#ifndef bfrag_Menu_h_
#define bfrag_Menu_h_

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include "bfrag_Manager.h"


class bfrag_Menu {

public:

    bfrag_Menu();
    virtual ~bfrag_Menu();

    void add_to_menu( vgui_menu & menu );
    
    //file
    static void load();
    static void addPuzzle();
    static void addbfrag2D();
    static void savePZ2as();
    static void savePZ2();
    static void quit();
    static void removeSelectedFragment();
    static void generatePuzzle();

    static void execLiveWire();

    //pair matching
    static void pairMatch();
    static void resetPairs();
    static void removeSelectedPairs();

    //puzzle solving
    static void matchingIteration();
    static void completeMatching();
    static void removeSeletedPuzzles();
    

    //display
    static void displayNPairs();
    static void displayCurrentPairs();
    static void displayAllPuzzleSolutions();
    static void displayAllContours();
    static void displayNContours();
    static void displayNextPair();//deprecated
    static void displayContour();
    static void displayAssembly();

    //settings
    static void topImagesShow();
    static void topImagesHide();
    static void botImagesShow();
    static void botImagesHide();
    static void botCurvesShow();
    static void botCurvesHide();
    static void pairmatchOptions();
    static void solvingOptions();


    //help
    static void about();
    static void help_contents();

protected:

private:
    //keeps track of last pair displayed, so user can see the next N pairs
    int whereToStart_;

};

#endif
