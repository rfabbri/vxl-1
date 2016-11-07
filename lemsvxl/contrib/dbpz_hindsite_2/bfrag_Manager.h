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
#include "bfrag_curve.h"
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
    void showPuzzleCost(int index );
    void showPuzzleCosts();
    void updatePuzzleCost(int index );
    void updatePuzzleCosts();
    
    //display
    void displayCurrentPairs();
    void displayNPairs(int howMany, int start);
    void displayAllPuzzleSolutions();
    void displayNPuzzleSolutions();
    void displayAllContours();
    void displayNContours( int howMany, int start );
    void displayPairsWithCurrentSettings(){ displayNPairs( gridWH_*gridWH_, _firstDisplayedPiece+1 ); };

    //settings
    void displayAllTopImages( bool show );
    void displayAllBotImages( bool show );
    void displayAllBotCurves( bool show );

    // Can's functions for the visualizer
    void get_image_filenames(vcl_string input);
    void load_xml(vcl_string filename, int chosen, vcl_string filename_images);
    void check_and_update_status_file(vcl_string foldername, int num_pieces_added);
    void check_and_update_assemblies_file(vcl_string foldername, int file_number);
    void save_xml(vcl_string filename);

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
    vcl_vector<searchState> states_;
    vcl_vector< vcl_pair< vgl_point_2d<int>, int > > offsets_;

    vcl_string PZ2fn;

    //for drawing the matched pairs from the searchState returned
    vcl_vector<vcl_pair<bfrag_curve*, bfrag_curve*> > matchedPairs_;
    searchState pairMatchResults_, _originalPairMatchResults;
    int gridWH_; //stores the width & height (its made square) of the current grid

    //drawing puzzle solutions states
    void displaySolutions( vcl_vector< searchState > *state);
    int _firstDisplayedPiece;
    int _firstDisplayedPuzzle;

    int _curIteration;
    vcl_vector< searchState > _allStateVector;
    vcl_vector< vil_image_view<vxl_byte> > image_to_show;

    vcl_vector< vcl_string > images_to_use_fnames;
    vcl_vector< vil_image_resource_sptr > images_to_use_resource;
    vcl_vector< vcl_pair<vil_image_view<vxl_byte>, int> > images_to_show;
    // the second element is the ID for the state
//    vcl_pair< vcl_vector< vil_image_view<vxl_byte>,  int> images_to_show;
    bool SHOW_IMAGES;
    bool SHOW_CONTOURS;
    vcl_vector<vcl_string> FRAG_PATHS;
};

#endif
