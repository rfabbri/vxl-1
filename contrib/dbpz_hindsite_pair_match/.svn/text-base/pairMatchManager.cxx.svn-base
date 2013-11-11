#include "pairMatchManager.h"

pairMatchManager::pairMatchManager(){
    thevsol2D = new bgui_vsol2D_tableau( "vsol tableau" );
    theViewer =  new vgui_viewer2D_tableau( thevsol2D );
    theShell = new vgui_shell_tableau( theViewer );
    
    thePairMatcher = new pairMatcher( thevsol2D );

}

pairMatchManager::~pairMatchManager(){
    delete thePairMatcher;
}

pairMatchManager* 
pairMatchManager::instance_ = 0;

pairMatchManager* 
pairMatchManager::instance()
{
  if (!instance_)
  {
    instance_ = new pairMatchManager();
  }
  return pairMatchManager::instance_;
}

void pairMatchManager::runTests(){
    assert(0);
}

void pairMatchManager::showCurvePoint(){
    vgui_dialog d("Show Point...");
    int curveNum = 1, pointNum = 0;
    d.field( "Which Curve? ", curveNum );
    d.field( "Which Point? ", pointNum );
    if( d.ask() ){
        thePairMatcher->showCurvePoint( curveNum, pointNum );
    }
}

void pairMatchManager::matchEm(){
    int startA = 0, endA = -1, startB = 0, endB = -1, matchStartA = 1;
    bool reverseA = false, reverseB = false;
    //get params
    vgui_dialog d("Parameters For Matching");
    d.field( "Curve A start point: ", startA );
    d.field( "Curve A end point (-1 for end): ", endA );
    d.field( "Match Start: ", matchStartA ); //first matched curve length to look at
    d.checkbox( "Reverse curve A? ", reverseA );
    d.field( "Curve B start point: ", startB );
    d.checkbox( "Reverse curve B? ", reverseB );

    //call runMatch
    if( d.ask()){
        thePairMatcher->runMatch( startA, endA, reverseA, startB, endB, reverseB, matchStartA );
    }
}

void pairMatchManager::loadCurves( const char* fn1, const char* fn2, int skipRate){
    thePairMatcher->loadCurves( fn1, fn2, skipRate );
}

