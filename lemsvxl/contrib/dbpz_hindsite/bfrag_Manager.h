/***********************************************************************
 *    Name: Daniel Spinosa
 *    File: bfrag_Manager.h
 *    Asgn: 
 *    Date: 
 *
 *     Manages most of the program, user intraction.  The intermediary
 *   "bfrag_Menu" actually gets the clicks, but it pretty much just runs
 *   routines in here.  This handles the hard work and passes off bfrag2D
 *   related grunt work to the bfrag2D_Manager.
 *
 ************************************************************************/

#ifndef bfrag_Manager_h_
#define bfrag_Manager_h_

#include <time.h>

#include <vgui/vgui.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_dialog.h>
#include <vcl_algorithm.h>
#include <vgui/vgui_style.h>

#include "bfrag2DManager.h"
#include "utils.h"
#include "PuzzleSolving.h"
#include "global.h"
#include "Curve.h"
#include "xform.h"


class bfrag_Manager {

public:
    bfrag_Manager();
    virtual ~bfrag_Manager();
    static bfrag_Manager* instance();
    
    //file operations
    void quit();
    int loadPuzzle( vcl_string fn );
    int addPuzzle( vcl_string fn );
    int savePZ2as( vcl_string fn );
    void savePZ2();
    void generatePuzzle();

    //FRAG
    int addbfrag2D( vcl_string topConFn, vcl_string botConFn, 
                    vcl_string topImageFn, vcl_string botImageFn, vcl_string name );
    void displayContour();
    void displayAssembly();//deprecated
    void removeSelectedFragments();
    void removeFragment( int which );

    //PAIR
    void pairMatch();
    void reorderPair( int from, int to );
    void removeSelectedPairs();
    void removePair( unsigned which );
    void resetPairs();

    //PUZZLE
    void matchingIteration();
    void completeMatching();
    void removeSelectedPuzzles();
    void removePuzzle( int which);
    void reorderPuzzle( int from, int to );
    
    //display
    void displayCurrentPairs();
    void displayNPairs(int howMany, int start);
    void displayAllPuzzleSolutions();
    void displayAllContours();
    void displayNContours( int howMany, int start );
    void displayPairsWithCurrentSettings(){ displayNPairs( gridWH_*gridWH_, _firstDisplayedPiece+1 ); };

    //settings
    void displayAllTopImages( bool show );
    void displayAllBotImages( bool show );
    void displayAllBotCurves( bool show );

    //accessor
    vcl_string getLoadedName(){ return PZ2fn; };
    void setShell( vgui_shell_tableau_sptr s);
    vgui_shell_tableau_sptr shell(){ return theShell; };

private:
    static bfrag_Manager *instance_;

    bool loadedPuzzle;
    int numPuzzlePieces;
    
    vgui_shell_tableau_sptr theShell;
    vgui_viewer2D_tableau_sptr theViewer;
    vgui_grid_tableau_sptr theGrid;

    //to manage (store, etc) all of the bfrag2D's
    bfrag2DManager *theBfrag2DManager;

    //jonahs puzzle solver
    PuzzleSolving *_puzzleSolver;

    vcl_string PZ2fn;

    //for drawing the matched pairs from the searchState returned
    vcl_vector<vcl_pair<Curve<double,double>*,Curve<double,double>*> > matchedPairs_;
    searchState pairMatchResults_, _originalPairMatchResults;
    int gridWH_; //stores the width & height (its made square) of the current grid

    //drawing puzzle solutions states
    void displaySolutions( vcl_vector< searchState > *state);
    int _firstDisplayedPiece;
    int _firstDisplayedPuzzle;

    int _curIteration;
    vcl_vector< searchState > _allStateVector;

};

#endif
