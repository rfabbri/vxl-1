/**************************************************************/
/*    Name: Daniel Spinosa
//    File: pairMatchMenu.h
//    Asgn: 
//    Date: 
 *
 *   This file is just for menu interaction, it is usually a pass
 *  through class, in that you click something and most of the work
 *  is done by the pairMatchManager 
***************************************************************/

#ifndef pairMatchMenu_H__
#define pairMatchMenu_H__

#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include "pairMatchManager.h"


class pairMatchMenu {

public:

    pairMatchMenu(){};
    virtual ~pairMatchMenu(){};

    void add_to_menu( vgui_menu & menu );


    //FILE
    static void loadCurves();
    static void quit(){ pairMatchManager::instance()->quit(); };

    //CURVE
    static void downsampleCurves();
    static void matchEm(){ pairMatchManager::instance()->matchEm(); };
    static void reverseA(){ pairMatchManager::instance()->reverseA(); };
    static void reverseB(){ pairMatchManager::instance()->reverseB(); };

    //DRIVER program, run many tests (hacky)
    static void runTests(){ pairMatchManager::instance()->runTests(); };

    //DRAW
    static void drawMatchLines();
    static void showCorners();
    static void showCurvePoint(){ pairMatchManager::instance()->showCurvePoint(); };
    
protected:

private:
  
};

#endif
