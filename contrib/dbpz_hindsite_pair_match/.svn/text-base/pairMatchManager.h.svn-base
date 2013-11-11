/***********************************************************************
 *    Name: Daniel Spinosa
 *    File: pairMatchManager.h
 *    Asgn: 
 *    Date: 
 *
 *     Manages most of the program, user intraction.
 ************************************************************************/

#ifndef pairMatchManager_H__
#define pairMatchManager_H__

#define LEVEL_II 33


#include <vgui/vgui.h>
#include <vgui/vgui_menu.h>

#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include "pairMatcher.h"

class pairMatchManager {

public:
    pairMatchManager();
    virtual ~pairMatchManager();
    static pairMatchManager* instance();

    vgui_shell_tableau_sptr getTheShell(){ return theShell; };

    //FILE
    void quit(){ vgui::quit(); };
    void loadCurves( const char* fn1, const char* fn2, int skipRate = 1 );

    //CURVE
    void downsampleCurves( int rate ){ thePairMatcher->resampleCurves( rate );
                                       thePairMatcher->drawCurves(); };
    void matchEm();
    void reverseA(){ thePairMatcher->reverseA(); };
    void reverseB(){ thePairMatcher->reverseB(); };

    //TESTS
    void runTests();
    
    //DRAW
    void showCurvePoint();
    void drawMatchLines(){ thePairMatcher->drawMatchLines(); };
    void displayCurves(){ thePairMatcher->drawCurves(); };
    void showCorners(){ thePairMatcher->showCorners(); };

private:
    static pairMatchManager *instance_;

    vgui_shell_tableau_sptr theShell;
    vgui_viewer2D_tableau_sptr theViewer;
    bgui_vsol2D_tableau_sptr thevsol2D;

    pairMatcher *thePairMatcher;

};

#endif
