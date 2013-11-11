/*************************************************************************
 *    NAME: Daniel Spinosa
 *    FILE: pairMatchMenu.cpp
 *    DATE: 
 *************************************************************************/
#include "pairMatchMenu.h"
#include <vgui/vgui.h>



void
pairMatchMenu::add_to_menu( vgui_menu & menu )
{

    //FILE
    vgui_menu fileItems;
        fileItems.add( "&Load Curves...", loadCurves );
        fileItems.separator();
        fileItems.add( "&Quit", quit );
    menu.add( "&File", fileItems);

    vgui_menu curveStuff;
        curveStuff.add( "&Downsample Curves...", downsampleCurves );
        curveStuff.add( "Reverse &A", reverseA );
        curveStuff.add( "Reverse &B", reverseB );
        curveStuff.separator();
        curveStuff.add( "&Match Them", matchEm );
        curveStuff.add( "&Run Tests", runTests );
    menu.add( "&Curve Stuff", curveStuff );

    vgui_menu drawing;
        drawing.add( "&Draw Match Lines", drawMatchLines );
        drawing.add( "&Show Point On Curve", showCurvePoint );
        drawing.add( "Show &Corners", showCorners );
    menu.add( "&Drawing", drawing );

}

void pairMatchMenu::showCorners(){
    pairMatchManager::instance()->showCorners();
}

void pairMatchMenu::drawMatchLines(){
    pairMatchManager::instance()->drawMatchLines();
}

void pairMatchMenu::loadCurves(){

    vcl_string fn1, fn2;
    int sampling = 1;

    vgui_dialog loadDialog("Load");
    loadDialog.file( "Curve A: ", vcl_string("*"), fn1 );
    loadDialog.file( "Curve B: ", vcl_string("*"), fn2 );
    loadDialog.field( "Sampling: ", sampling );

    if( loadDialog.ask() ){
        pairMatchManager::instance()->loadCurves( fn1.c_str(), fn2.c_str(), sampling );
        pairMatchManager::instance()->displayCurves();
    }

    
}

void pairMatchMenu::downsampleCurves(){
    int rate = 1;

    vgui_dialog diag( "Downsample Curves" );
    diag.field( "Sample how often: ", rate );
    if( diag.ask() && rate > 1 && rate < 100 ){
        pairMatchManager::instance()->downsampleCurves( rate );
     }

}
