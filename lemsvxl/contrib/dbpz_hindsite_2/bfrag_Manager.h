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
#include <algorithm>
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
    int loadPuzzle( std::string fn );
    int addPuzzle( std::string fn );
    int savePZ2as( std::string fn );
    void savePZ2();
    void generatePuzzle();

    //FRAG
    int addbfrag2D( std::string topConFn, std::string botConFn, 
                    std::string topImageFn, std::string botImageFn, std::string name );
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
    void get_image_filenames(std::string input);
    void load_xml(std::string filename, int chosen, std::string filename_images);
    void check_and_update_status_file(std::string foldername, int num_pieces_added);
    void check_and_update_assemblies_file(std::string foldername, int file_number);
    void save_xml(std::string filename);

    //accessor
    std::string getLoadedName(){ return PZ2fn; };
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
    std::vector<searchState> states_;
    std::vector< std::pair< vgl_point_2d<int>, int > > offsets_;

    std::string PZ2fn;

    //for drawing the matched pairs from the searchState returned
    std::vector<std::pair<bfrag_curve*, bfrag_curve*> > matchedPairs_;
    searchState pairMatchResults_, _originalPairMatchResults;
    int gridWH_; //stores the width & height (its made square) of the current grid

    //drawing puzzle solutions states
    void displaySolutions( std::vector< searchState > *state);
    int _firstDisplayedPiece;
    int _firstDisplayedPuzzle;

    int _curIteration;
    std::vector< searchState > _allStateVector;
    std::vector< vil_image_view<vxl_byte> > image_to_show;

    std::vector< std::string > images_to_use_fnames;
    std::vector< vil_image_resource_sptr > images_to_use_resource;
    std::vector< std::pair<vil_image_view<vxl_byte>, int> > images_to_show;
    // the second element is the ID for the state
//    std::pair< std::vector< vil_image_view<vxl_byte>,  int> images_to_show;
    bool SHOW_IMAGES;
    bool SHOW_CONTOURS;
    std::vector<std::string> FRAG_PATHS;
};

#endif
