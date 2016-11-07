/*************************************************************************
 *    NAME: Daniel Spinosa
 *    FILE: bfrag_Manager.cpp
 *    DATE: 
 *************************************************************************/

#include "bfrag_Manager.h"
#include <vcl_cstdio.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_print.h>
#include <vul/vul_file_iterator.h>
#include <bxml/bxml_read.h>

class intGreaterThan 
{
public:
  bool operator() (int a, int b) { return a>b; }
};

bfrag_Manager::bfrag_Manager()
{
  loadedPuzzle = false;
  numPuzzlePieces = 0;
  theBfrag2DManager = new bfrag2DManager();
  //thebfrag2DTableau = 0;
  theViewer = 0;
  theGrid = 0;
  gridWH_ = 1;
  _puzzleSolver = new PuzzleSolving();
  _firstDisplayedPiece = 1;
  _curIteration = 0;
/*  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag0/g0_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag1/g1_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag2/g2_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag3/g3_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag4/g4_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag5/g5_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag6/g6_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/tile-g/all_cons/frag7/g7_front.jpg");*/

/*  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag00/00t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag01/01t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag02/02t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag03/03t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag04/04t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag05/05t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag06/06t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag07/07t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag08/08t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag09/09t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag10/10t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag11/11t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag12/12t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag13/13t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag14/14t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag15/15t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag16/16t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag17/17t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag18/18t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag19/19t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag20/20t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag21/21t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag22/22t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag23/23t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag24/24t.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/marble/cons/frag25/25t.jpg");*/
/*
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag001/1b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag002/2b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag011/11b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag012/12b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag016/16b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag038/38b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag040/40b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag042/42b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag052/52b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag054/54b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag056/56b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag064/64b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag068/68b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag070/70b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag072/72b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag073/73b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag075/75b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag077/77b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag079/79b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag086/86b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag087/87b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag088/88b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag093/93b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag094/94b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag095/95b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag096/96b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag097/97b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag113/113b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag114/114b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag117/117b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag120/120b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag125/125b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag126/126b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag127/127b_front.jpg");*/
/*
  vcl_string folder = "D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/*";
  vcl_string folder2 = "D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/";
  vcl_vector<vcl_string> names;
  vul_file_iterator f(folder);
  ++f;
  ++f;
  while(f.filename() != NULL)
  {
    vcl_string frag_folder = f.filename();
    vcl_string temp = folder2;
    temp.append(frag_folder);
    temp.append("/");
    char digit1 = temp[temp.size()-4];
    char digit2 = temp[temp.size()-3];
    char digit3 = temp[temp.size()-2];

    if(digit1 != '0')
    {
      temp = temp + digit1;
      temp = temp + digit2;
    }
    else if(digit2 != '0')
      temp = temp + digit2;
    
    temp = temp + digit3;

    temp.append("b_front.jpg");
    names.push_back(temp);
    ++f;
  }

  names[133] = "D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag154-1/154-1b_front.jpg";
  names[162] = "D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag188-1/188-1b_front.jpg";

  for(unsigned i=0; i < names.size(); i++)
    images_to_use_fnames.push_back(names[i]);
*/
/*  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag001/1b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag002/2b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag003/3b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag004/4b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag005/5b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag006/6b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag007/7b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag008/8b_front.jpg");
  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag009/9b_front.jpg");*/

//  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag097/97b_front.jpg");
//  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-b/frag302/302b_front.jpg");

//  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-a/frag097/97a_front.jpg");
//  images_to_use_fnames.push_back("D:/MyDocs/projects/PuzzleSolving/can_datasets/highland-a/frag302/302a_front.jpg");
/*
  for(unsigned i=0; i < images_to_use_fnames.size(); i++)
    images_to_use_resource.push_back(vil_load_image_resource(images_to_use_fnames[i].c_str()));*/
}


/*************************************************************************
 * Function Name: bfrag_menu::~dbvis1_bfrag_menu
 * Parameters: 
 * Effects: 
 *************************************************************************/

bfrag_Manager::~bfrag_Manager()
{
    delete theBfrag2DManager;
    delete _puzzleSolver;
}

/*************************************************************************
 * Function Name: bfrag_menu::add_to_menu
 * Parameters:  vgui_menu & menu 
 * Returns: void
 * Effects: 
 *************************************************************************/

bfrag_Manager* 
bfrag_Manager::instance_ = 0;

bfrag_Manager* 
bfrag_Manager::instance()
{
  if (!instance_)
  {
    instance_ = new bfrag_Manager();
  }
  return bfrag_Manager::instance_;
}


void
bfrag_Manager::quit()
{

    if( loadedPuzzle ){
        vgui_dialog quit_dl("Do You Really Want To Quit?");
        //quit_dl.set_modal(true);
        quit_dl.message("You have a puzzle loaded!\nDo you really want to quit The Fragment Assembler?");
        quit_dl.set_ok_button( "Yes" );
        quit_dl.set_cancel_button( "No" );
    if (quit_dl.ask())
        vgui::quit();
    } else {
        vgui::quit();
    }
        
}

int bfrag_Manager::addPuzzle( vcl_string fn ) {
    //sore filename for later use
    PZ2fn = fn;

    if( fn.size() <= 0 ){
        //as if they hit cancel
        return -2;
    }

    //if a puzzle isn't already loaded, load this one as normal
    if( !loadedPuzzle ) {
        return loadPuzzle( fn );
    }

    int err = theBfrag2DManager->addPuzzle( fn );
    if( err >= 0 ) {
        //puzzle loaded successfully, err = #pieces total

        numPuzzlePieces = err;
        loadedPuzzle = true;

        //draw em!
        displayAllContours();

        return numPuzzlePieces;
    } else {

        //on error, the puzzle already loaded will remain
        return err;

    }
}

/********************************************************************************
 * loadPuzzle, given a file name attempts to load that puzzle using 
 * bfrag_File_Handling's loadPuzzle( vcl_Ssring ) function
 ********************************************************************************/

int bfrag_Manager::loadPuzzle( vcl_string fn ) {

    //sore filename for later use
    PZ2fn = fn;

    if( fn.size() <= 0 ){
        //as if they hit cancel
        return -2;
    }

    //if a puzzle has already been successfully loaded
    if( loadedPuzzle && numPuzzlePieces != 0 ) {
        //make sure user wants to replace currently loaded puzzle
        vgui_dialog warn_dl("Warning!");
        //warn_dl.set_modal(true);
        char warningMessage[255];
        sprintf( warningMessage, "A Puzzle file with %d pieces is already open.  Are you sure you want to open this file and replace previous puzzle?", numPuzzlePieces);
        warn_dl.message( warningMessage );
        warn_dl.set_ok_button( "Yes, I'm Sure" );
        warn_dl.set_cancel_button( "No!" );
        if( !warn_dl.ask() ){
            //they clicked no, return to program
            return -2;
        }
    }

    int err = theBfrag2DManager->loadPuzzle( fn );

    //remove old pairwise matches
    pairMatchResults_ = searchState();
    _originalPairMatchResults = searchState();
    _allStateVector.clear();


    if( err >= 0 ) {
        //puzzle loaded successfully, err = #pieces loaded

        numPuzzlePieces = err;
        loadedPuzzle = true;

        //draw em!
        displayAllContours();

        return numPuzzlePieces;
    } else {

        //on error, the old puzzle will have been unloaded (no puzzle loaded!)
        numPuzzlePieces = 0;
        loadedPuzzle = false;
        return err;
    }


}

void bfrag_Manager::savePZ2(){
    if( PZ2fn.length() > 2 )
    theBfrag2DManager->savePZ2as( PZ2fn );
}

//save the current setup
int bfrag_Manager::savePZ2as( vcl_string fn ){

    if( fn.size() == 0 ){
        //as if they hit cancel
        return -2;
    }

    vgui_dialog note("Set Puzzle Note");
    vcl_string newline;
    note.field( "Add To Current Note:", newline );
    note.message( "(cancel to add nothing)" );
    if( note.ask() ){
        if(newline.length() > 0){
            theBfrag2DManager->addToNote( newline );
        }
    } //else do nothing

    return theBfrag2DManager->savePZ2as( fn );
    
}


//adds one piece
int bfrag_Manager::addbfrag2D( vcl_string topConFn, vcl_string botConFn, 
                               vcl_string topImageFn, vcl_string botImageFn, vcl_string name ){

    //make sure topConFn != null
    if( topConFn.size() <= 4 ){
        vgui_dialog warn_dl("Error!");
        //warn_dl.set_modal(true);
        warn_dl.message( "Error: You must specify a Top Curve!" );
        warn_dl.set_ok_button( "Try Again..." );
        warn_dl.set_cancel_button( "Cancel" );
        if( !warn_dl.ask() ){
            //they clicked no, return to program
            return -2;
        } else {
            //try again
            return -1;
        }
    }


    //vcl_cout << "MANAGER: adding a piece to the current piece list" << vcl_endl;

    int err = theBfrag2DManager->addPiece( topConFn, botConFn, topImageFn, botImageFn, name );
    if( err >= 0 ) {
        //puzzle loaded successfully, err = total #pieces loaded up to now

        numPuzzlePieces = err;
        loadedPuzzle = true;

        //redisplay!
        displayAllContours();

        return numPuzzlePieces;
    } else {
        return err;
    }

}


void bfrag_Manager::displayAssembly(){
    assert(0);

    //use checkbox

    vcl_vector<vcl_string> choices;
    vcl_vector< bool > whichBool;
    //bool temp = false;

    vgui_dialog contourChooser( "Choose Pieces To Add To Assembly" );
    //contourChooser.set_modal(true);
    contourChooser.message ( "Index:\"Name\" [hasTopCurve][hasTopImage][hasBotCurve][hasBotImage]" );

    theBfrag2DManager->getBfragNameList( choices );


    //for whatever reason i coldn't push back the new bool() in the same for loop as i used it
    //for the check box, so im setting up the vector here, beforehand
    for( unsigned i = 0; i < choices.size(); i++ ){
        whichBool.push_back( bool(false) );
    }

    for( unsigned i = 0; i < choices.size(); i++ ){

        //this (bool&) cast was necessary b/c the compiler included w/ MS .NET framwork baulks
        //without it
        contourChooser.checkbox( (choices[i]).c_str(), (bool &) whichBool[i] );

    }


    if( contourChooser.ask() ){

        /*
        for( int i = 0; i < whichBool.size(); i++ ){
            vcl_cout << "which bool is: " << whichBool[i] << vcl_endl;     
        }
        */

        theBfrag2DManager->clear_display();
        theBfrag2DManager->displayAssembly( whichBool );
        theBfrag2DManager->post_redraw();

    }
}

void bfrag_Manager::displayContour(){
    if( numPuzzlePieces == 0 ){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: No pieces to display.  Load a puzzle first." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    }

    vcl_vector<vcl_string> choices;
    int which;

    vgui_dialog contourChooser( "Choose A Contour" );
    //contourChooser.set_modal(true);
    contourChooser.message ( "Index:\"Name\" [hasTopCurve][hasTopImage][hasBotCurve][hasBotImage]" );

    theBfrag2DManager->getBfragNameList( choices );

    contourChooser.choice( "Choose piece to view", choices, which );

    if( contourChooser.ask() ){
        //clear display
        //theBfrag2DManager->clear_display();
        //display the new one
        //theBfrag2DManager->display(which);
        theBfrag2DManager->newGrid( 1, 1, -1 );
        theBfrag2DManager->displayBfragAt(which,0);
        _firstDisplayedPiece = which;
        theBfrag2DManager->post_redraw();
    }

}


void bfrag_Manager::setShell( vgui_shell_tableau_sptr s){ 

    theShell = s; 

    vgui_bfrag2D_tableau_sptr tab = vgui_bfrag2D_tableau_new();
    theViewer = vgui_viewer2D_tableau_new(tab);
    theGrid = vgui_grid_tableau_new(1,1);
    theGrid->add_at(theViewer, 0,0);

    theBfrag2DManager->setTableau( tab );
    theBfrag2DManager->setShell( theShell );
    theBfrag2DManager->setGrid( theGrid );

    theShell->add(theGrid);

    //vcl_cout << "-----MANAGER: shell, viewer, bfrag2D_tableau are all setup-----" << vcl_endl;
}

void bfrag_Manager::displayAllTopImages( bool show ){
    theBfrag2DManager->displayAllTopImages( show );
}

void bfrag_Manager::displayAllBotImages( bool show ){
    theBfrag2DManager->displayAllBotImages( show );
}

void bfrag_Manager::displayAllBotCurves( bool show ){
    theBfrag2DManager->displayAllBotCurves( show );

}

//lets give this project a soul
void bfrag_Manager::pairMatch()
{
  //have the bfrag2DManager create a Curve<double,double> from our contour representation
  //for the top curves and set _Contours in PuzzleSolving to be that
  vcl_vector<bfrag_curve> bfragContours = theBfrag2DManager->getTopContoursForPuzzleSolving();
  if( bfragContours.size() == 0 )
  {
    vcl_cout << "bfrag_Manager: ABORTING pairwise matching, not enuf pieces loaded" << vcl_endl;
    return;
  }
    
  _puzzleSolver->setContours( bfragContours );
  _puzzleSolver->preProcessCurves();

  //puzzleSolving->pairMatch()
  clock_t start, end, diff;
  start = clock();
  pairMatchResults_ = _puzzleSolver->pairMatch();
  _originalPairMatchResults = pairMatchResults_;
  end = clock();
  diff = end - start;
  double mili = diff % 1000;
  int sec = (int)floor((diff/1000.0)) % 60;
  int mn = (int)floor((diff/60000.0)) % 60;
  int hr = (int)floor((diff/360000.0));
//  vcl_cout << "Pair Match running time: " << hr << " hours, " << mn << " min, " << sec << " sec, " << mili << " miliseconds." << vcl_endl;
  vcl_cout << "Pair Match Running Time: " << double(diff) / CLOCKS_PER_SEC << "seconds" << vcl_endl;
    
  //OLD SORT: costs
  //pairMatchSortedCost_ = pairMatchResults_.cost;
  //vcl_sort(pairMatchSortedCost_.begin(),pairMatchSortedCost_.end(),cost_ind_less());
  //NEW SORT: pairwiseMatches
  pairMatchResults_.sortPairwiseMatches();

  displayCurrentPairs();

  // added by CAN
  pairMatchResults_ = _originalPairMatchResults;
  pairMatchResults_.sortPairwiseMatches();

  //add pair matches to allStateVector
  _allStateVector.push_back( pairMatchResults_ );

  vgui_dialog done_dl("Done!");
  done_dl.message( "Pairwise Matching Complete!" );
  done_dl.message( "Click \"Puzzle Solving | Solve Puzzle\" to finish solving the puzzle" );
  done_dl.set_cancel_button( 0 );
  done_dl.ask();
}

void bfrag_Manager::matchingIteration()
{
  if( pairMatchResults_.matches_ez_list_.size() == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Pairwise Matching first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  _curIteration++;
  if( _curIteration == numPuzzlePieces )
  {
    vgui_dialog done_dl("Done!");
    done_dl.message( "Puzzle Solving Complete!" );
    done_dl.set_cancel_button( 0 );
    done_dl.ask();
    return;
  }
  vcl_cout << "Matching Iteration: " << _curIteration << vcl_endl;
  
  clock_t start, end, diff;
  start = clock();
  _allStateVector = _puzzleSolver->search( _allStateVector );
  end = clock();
  diff = double(end) - start;
  double mili = diff % 1000;
  int sec = (int)floor((diff/1000.0)) % 60;
  int mn = (int)floor((diff/60000.0)) % 60;
  int hr = (int)floor((diff/360000.0));
//  vcl_cout << "Iteration running time of: " << hr << " hours, " << mn << " min, " 
//    << sec << " sec, " << mili << " miliseconds." << vcl_endl;
  vcl_cout << "Puzzle Solving Iteration Time: " << double(diff) / CLOCKS_PER_SEC << "seconds" << vcl_endl;
  
  displayAllPuzzleSolutions();
}

void bfrag_Manager::completeMatching()
{
  if( pairMatchResults_.matches_ez_list_.size() == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Pairwise Matching first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  while( _curIteration < numPuzzlePieces )
        matchingIteration();
}

void bfrag_Manager::displayAllPuzzleSolutions()
{ 
  if( pairMatchResults_.matches_ez_list_.size() == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Pairwise Matching first, then Puzzle Solving." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
    } 
  else if( _curIteration == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Puzzle Solving first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  
  _firstDisplayedPuzzle = 0;
  displaySolutions( &_allStateVector ); 
};

void bfrag_Manager::displayNPuzzleSolutions()
{ 
  if( pairMatchResults_.matches_ez_list_.size() == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Pairwise Matching first, then Puzzle Solving." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
    } 
  else if( _curIteration == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: Run Puzzle Solving first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  
  _firstDisplayedPuzzle = 0;
  displaySolutions( &_allStateVector ); 
};

void bfrag_Manager::displaySolutions(vcl_vector<searchState> *theStates)
{
  if( theStates->size() == 0 )
  {
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: No solutions to display.  Run Puzzle Solving first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }

  // sort the states according to cost
  vcl_cout << "Sorting the states according to cost" << vcl_endl;
  vcl_sort( theStates->begin(), theStates->end(), search_state_sort() );

  searchState state;
  gridWH_ = static_cast<int>(ceil(vcl_sqrt( (double) theStates->size() )));
  theBfrag2DManager->newGrid(gridWH_,gridWH_, -1, theStates->size() );

  for( unsigned w = 0; w < theStates->size(); w++ )
  {
    state = (*theStates)[w];
    int state_id = state.state_id_;
    state.load_state_curves_list();
    vcl_cout << w << ": tCost= " << state.tCost << vcl_endl;
    //why do they do this!?
    if( state.nProcess == 0)
    {
      vgui_dialog warn_dl("Error!");
      warn_dl.message( "Error: No solutions to display.  Run Puzzle Solving first." );
      warn_dl.set_ok_button( "Ok" );
      warn_dl.set_cancel_button( 0 );
      warn_dl.ask();
      theBfrag2DManager->redrawTableaux();
      break;
    }
    //FIRST
    //this draws an outline of the entire solution, but...
    //*** not an outline so much -- its the current contour to match new pieces too
    if(state.is_constr() == false)
      state.structure();

    for(unsigned i = 0; i < state.numCon(); i++)
    {
      if(SHOW_IMAGES && SHOW_CONTOURS)
        theBfrag2DManager->addCurveAt(state.constr(i), w, offsets_[state_id].first.x(), offsets_[state_id].first.y(), 
                                      vgui_style::new_style(0.3f,0.3f,0.3f,6,6));
      else if(SHOW_CONTOURS)
        theBfrag2DManager->addCurveAt(state.constr(i), w, 0, 0, vgui_style::new_style(0.3f,0.3f,0.3f,6,6));
    }

    // shows the cost
    double cost = state.tCost;
    theBfrag2DManager->show_cost(cost, w);

    //SECOND
    //this draws all the individual pieces
    vgui_style_sptr theStyle = vgui_style::new_style(0,1,0,1,1);
    for( int i = 0; i < state.nProcess; i++) 
    {
      int p = state.process[i];
      if( i == state.nProcess-1 )
        theStyle = vgui_style::new_style( 1,1,0, 1.5,1.5);
      if(SHOW_IMAGES && SHOW_CONTOURS)
        theBfrag2DManager->addCurveAt(state.piece(p), w, offsets_[state_id].first.x(), offsets_[state_id].first.y(), theStyle );
      else if(SHOW_CONTOURS)
        theBfrag2DManager->addCurveAt(state.piece(p), w, 0, 0, theStyle );
    } 
    if(SHOW_IMAGES)
      theBfrag2DManager->show_image( images_to_show[state_id].first, w );

    //THIRD
    //points where there has been identified an "open junction"
    //an "open junction" is a potential N-TUPLE JUNCTION point that was NOT completed
    //good for debugging only probably -- confusing and unnecessary for other purposes

    /*
    for(int i = 0; i < state.open_junc.size(); i++) {
    theBfrag2DManager->addPointAt( state.open_junc[i].second.x(), state.open_junc[i].second.y(), w, vgui_style::new_style(1,1,0,7,7) );
    }
    //see above, but this is for CLOSED not OPEN junctions
    for( int i = 0; i < state.closed_junc.size(); i++){
    theBfrag2DManager->addPointAt( state.closed_junc[i].second.x(), state.closed_junc[i].second.y(), w, vgui_style::new_style(1,1,0,7,7) );
    }
    */
    /*
    //Draw lines from the middle of the pices to one another
    //NOT DONE! the next section is my code and marks two center points
    //what follows it is the old way of doing the rest
    //i have no need for this as of now and its not a priority
    //--------------my code----------------
    double px1,py1,px2,py2;

    vcl_pair<double,double> cen1 = center(state.piece(state.new_edge.first));
    px1 = cen1.first;
    py1 = cen1.second;

    vcl_pair<double,double> cen2 = center(state.piece(state.new_edge.second));
    px2 = cen2.first;
    py2 = cen2.second; 


    theBfrag2DManager->addPointAt( px1, py1, w );
    theBfrag2DManager->addPointAt( px2, py2, w );
    //--------------my code----------------



    LineGeom* gline = new LineGeom(px1,py1,px2,py2);
    gline->loadStyle(Color(BLACK) & LineWidth(3));
    line_node->addChild(gline);
    int cnt;
    for(cnt=0;cnt<state->new_edges.size();cnt++) {
    cen1=center(state->piece(state->new_edges[cnt].first));
    px1 = cen1.first;
    py1 = cen1.second;  

    cen2=center(state->piece(state->new_edges[cnt].second));
    px2 = cen2.first;
    py2 = cen2.second;

    gline = new LineGeom(px1,py1,px2,py2);
    gline->loadStyle(Color(LIGHT_GRAY) & LineWidth(3));
    line_node->addChild(gline);
    }

    for(cnt=0;cnt<state->old_edges.size();cnt++) {
    cen1=center(state->piece(state->old_edges[cnt].first));
    px1 = cen1.first;
    py1 = cen1.second;

    cen2=center(state->piece(state->old_edges[cnt].second));
    px2 = cen2.first;
    py2 = cen2.second; 

    gline = new LineGeom(px1,py1,px2,py2);
    gline->loadStyle(Color(LIGHT_GRAY) & LineWidth(1));
    line_node->addChild(gline);
    }  

    state_node->multXForm(autoScale(line_node,x,y,size));

    if(!CONTOUR_FLAG)
    state_node->removeChild(line_node);

    state->node=state_node;
    */
  }
  /*
  vil_save(images_[0], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_0.jpg");
  vil_save(images_[1], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_1.jpg");
  vil_save(images_[2], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_2.jpg");
  vil_save(images_[3], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_3.jpg");
  vil_save(images_[4], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_4.jpg");
  vil_save(images_[5], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_5.jpg");
  vil_save(images_[6], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_6.jpg");
  vil_save(images_[7], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_7.jpg");
  vil_save(images_[8], "D:/MyDocs/projects/PuzzleSolving/Temp/dt_piece_8.jpg");*/

  theBfrag2DManager->post_redraw();
}


void bfrag_Manager::displayCurrentPairs(){
  displayNPairs( pairMatchResults_.matches_ez_list_.size(), 1 );
}

void bfrag_Manager::resetPairs(){
  if( pairMatchResults_.matches_ez_list_.size() > 0 ){
    pairMatchResults_ = _originalPairMatchResults;
    pairMatchResults_.sortPairwiseMatches();   

    displayCurrentPairs();
  }
}

void bfrag_Manager::removePair( unsigned which ){
  if( which < pairMatchResults_.matches_ez_list_.size() )
  {
    pairMatchResults_.matches_ez_list_.erase( pairMatchResults_.matches_ez_list_.begin() + which + _firstDisplayedPiece );
    //now redisplay em
    displayCurrentPairs();
  }
}

void bfrag_Manager::removeSelectedPairs(){

  vcl_vector<int> selectedRows, selectedCols;
#if 0
  //this variable is not used in the code.  PLEASE FIX!  -MM
  int howMany = theBfrag2DManager->getSelected(selectedCols, selectedRows);
#endif

  assert( selectedRows.size() == selectedCols.size() );

  /*
  for( int i = 0; i < selectedRows.size(); i++ ){
  vcl_cout << "[" << selectedCols[i] << "," << selectedRows[i] << "] -- " << (selectedRows[i]*gridWH_)+selectedCols[i] << vcl_endl;
  }
  */

  //this kinda sux, but i have to remove the last to the first . . .
  vcl_vector<int> toDelete;
  for( unsigned i = 0; i < selectedRows.size(); i++ ){
    toDelete.push_back( ((selectedRows[i]*gridWH_)+selectedCols[i]) );
  }
  //now sort that so the largest comes first
  vcl_sort(toDelete.begin(), toDelete.end(), intGreaterThan());
  for( unsigned i = 0; i < toDelete.size(); i++ ){
    //make sure then one were deleting actually exists...
    int junkint = pairMatchResults_.matches_ez_list_.size();
    if( (toDelete[i] < junkint) || (junkint < 0) ){
      //vcl_cout << "now deleting: " << toDelete[i] << vcl_endl;
      pairMatchResults_.matches_ez_list_.erase( pairMatchResults_.matches_ez_list_.begin()+toDelete[i] + _firstDisplayedPiece );

    }
  }

  if( pairMatchResults_.matches_ez_list_.size() > 0 ){
    //now redisplay em
    displayCurrentPairs();
  } else {
    void displayAllContours();
  }

}

void bfrag_Manager::displayNPairs(int howMany, int start){
  if(pairMatchResults_.matches_ez_list_.size() == 0){
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: No pairs to display.  Run Pair Matching first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  int pairjunkint = static_cast<int>(pairMatchResults_.matches_ez_list_.size());
  if( (start > pairjunkint) || (pairjunkint<0) ){
    vgui_dialog error_dl("Display Error");
    char infomsg[2048];
    sprintf( infomsg, "Error: You chose to start at %i but there are only %i pairs to display.", start, pairMatchResults_.matches_ez_list_.size());
    error_dl.message( infomsg );
    error_dl.set_cancel_button(0);
    error_dl.ask();
    return;
  }

  _cList.clear();
  unsigned size = _Contours.size();
  for(unsigned i=0; i<size; i++)
    _cList.push_back(_Contours[i]);

  _firstDisplayedPiece = start-1;

  gridWH_ = static_cast<int>(ceil(vcl_sqrt((double) howMany)));
  theBfrag2DManager->newGrid(gridWH_,gridWH_, pairMatchResults_.matches_ez_list_.size());

  int p1, p2;
  bfrag_curve *c1, *c2;
  assert (start >= 0);
  int pairmatch_int = static_cast<int>(pairMatchResults_.matches_ez_list_.size());

  //    FILE *fp = vcl_fopen("D:/MyDocs/projects/PuzzleSolving/Temp/matches.txt", "w");

  for( int i = start-1; i < pairmatch_int && i < (start-1)+howMany; i++ )
  {
    int index = pairMatchResults_.matches_ez_list_[i].first;
    p1 = _matches[index].whichCurves.first;
    p2 = _matches[index].whichCurves.second;
    c1 = pairMatchResults_.piece(p1);
    c2 = pairMatchResults_.piece(p2);

    XForm3x3 *pairXForm = new XForm3x3;
    regContour(c1,c2, _matches[index].pointMap, 0, pairXForm );
    matchedPairs_.push_back(vcl_pair<bfrag_curve *, bfrag_curve *>(c1,c2));
    // add these two pieces in the same tableau on the grid
    // draw the registered boundaries of the two pieces
    theBfrag2DManager->addCurveAt(c1,i-(start-1));
    theBfrag2DManager->addCurveAt(c2,i-(start-1));

    vcl_cout << i << ": " << pairMatchResults_.matches_ez_list_[i].second << " " 
      << _matches[index].whichCurves.first << " and " 
      << _matches[index].whichCurves.second << vcl_endl;

    //TEST
    //TURN OFF THE UPPER CONTOUR OR DONT USE THOSE CURVES AT ALL!!!!??
    // this is where we try to add the image, soon!
    //add the image for pice 2, which is not translated
    //CAN        theBfrag2DManager->displayBfragAt( p2, i-(start-1) );
    //now transform the image for p1 and add it to the display
    // CAN: there seems to be something wrong here, deal with it later
    //CAN        theBfrag2DManager->displayBfragAt( p1, i-(start-1), pairXForm );

    //TEST

    delete pairXForm;
  }
  //    vcl_fclose(fp);
}

void bfrag_Manager::reorderPair( int from, int to ){
  //vcl_cout << "manager reordering from: " << from << " to: " << to << vcl_endl;

  //vcl_cout << "COSTS BEFORE:" << vcl_endl;
  //pairMatchResults_.printPairwiseMatchesSortedNPS();
  unsigned tounsigned = static_cast<unsigned>(to);
  if( tounsigned == 0 )
  {
    pairMatchResults_.matches_ez_list_[from].second = pairMatchResults_.matches_ez_list_[to].second - COST_ADJUSTMENT;
  } 
  else if( tounsigned == pairMatchResults_.matches_ez_list_.size()-1 )
  {
    pairMatchResults_.matches_ez_list_[from].second = pairMatchResults_.matches_ez_list_[to].second + COST_ADJUSTMENT;
  } 
  else if( to > from )
  {
    double diff = pairMatchResults_.matches_ez_list_[to+1].second - pairMatchResults_.matches_ez_list_[to].second;
    pairMatchResults_.matches_ez_list_[from].second = pairMatchResults_.matches_ez_list_[to+1].second - diff/2.0;
  } 
  else 
  {
    double diff = pairMatchResults_.matches_ez_list_[to].second - pairMatchResults_.matches_ez_list_[to-1].second;
    //new cost is right in between the target and the one before it
    //which puts this just before target, exactly what we want
    //vcl_cout << "ORIG COST: " << pairMatchResults_._matches[from].cost << vcl_endl;
    //vcl_cout << "BETTER COST: [" << pairMatchResults_._matches[to-1].cost << "] WORSE COST: [" << pairMatchResults_._matches[to].cost << "]" << vcl_endl;
    //vcl_cout << "DIFF/2.0 (adjustment): " << diff/2.0 << vcl_endl;
    //vcl_cout << "NEW SCORE: " << pairMatchResults_._matches[to].cost - diff/2.0 << vcl_endl;

    pairMatchResults_.matches_ez_list_[from].second = pairMatchResults_.matches_ez_list_[to].second - diff/2.0;
  }

  //re-sort em
  pairMatchResults_.sortPairwiseMatches();

  //redisplay
  displayPairsWithCurrentSettings();

}

void bfrag_Manager::displayAllContours(){
  displayNContours( numPuzzlePieces, 1 );    
}

void bfrag_Manager::displayNContours( int howMany, int start ){
  if(!loadedPuzzle){
    vgui_dialog warn_dl("Error!");
    warn_dl.message( "Error: No contours to display.  Load puzzle or contours first." );
    warn_dl.set_ok_button( "Ok" );
    warn_dl.set_cancel_button( 0 );
    warn_dl.ask();
    return;
  }
  if( numPuzzlePieces == 0 ){
    gridWH_ = 1;
    theBfrag2DManager->newGrid(gridWH_,gridWH_, -9);
    theBfrag2DManager->redrawTableaux();
    return;
  }
  if( start > numPuzzlePieces ){
    vgui_dialog error_dl("Display Error");
    char infomsg[2048];
    sprintf( infomsg, "Error: You chose to start at %i but there are only %i contours to display.", start, numPuzzlePieces);
    error_dl.message( infomsg );
    error_dl.set_cancel_button(0);
    error_dl.ask();
    return;
  }

  gridWH_ = static_cast<int>(ceil(vcl_sqrt((double) howMany)));
  theBfrag2DManager->newGrid(gridWH_,gridWH_, -9); //-9 single FRAG popup menu

  for( int i = start-1; i < numPuzzlePieces && i < (start-1)+howMany; i++ )
    theBfrag2DManager->displayBfragAt( i, i-(start-1) );
}

void bfrag_Manager::removeFragment( int which ){
  if( !(theBfrag2DManager->removeBfrag(which)) ){
    numPuzzlePieces--;
  }
  displayAllContours();
}

void bfrag_Manager::removeSelectedFragments(){
  vcl_vector<int> selectedRows, selectedCols;
#if 0
  //this variable is not used in the code.  PLEASE FIX!  -MM
  int howMany = theBfrag2DManager->getSelected(selectedCols, selectedRows);
#endif

  //this kinda sux, but i have to remove the last to the first . . .
  vcl_vector<int> toDelete;
  for( unsigned i = 0; i < selectedRows.size(); i++ ){
    toDelete.push_back( ((selectedRows[i]*gridWH_)+selectedCols[i]) );
  }
  //now sort that so the largest comes first
  vcl_sort(toDelete.begin(), toDelete.end(), intGreaterThan());
  for( unsigned i = 0; i < toDelete.size(); i++ ){
    //make sure then one were deleting actually exists...
    if( !(theBfrag2DManager->removeBfrag(toDelete[i])) ){
      numPuzzlePieces--;
    }

  }

  //now redisplay em
  displayAllContours();

}

void bfrag_Manager::removeSelectedPuzzles(){
  vcl_vector<int> selectedRows, selectedCols;
#if 0
  //this variable is not used in the code.  PLEASE FIX!  -MM
  int howMany = theBfrag2DManager->getSelected(selectedCols, selectedRows);
#endif

  //this kinda sux, but i have to remove the last to the first . . .
  vcl_vector<int> toDelete;
  for( unsigned i = 0; i < selectedRows.size(); i++ ){
    toDelete.push_back( ((selectedRows[i]*gridWH_)+selectedCols[i]) );
  }

  //now sort that so the largest comes first
  vcl_sort(toDelete.begin(), toDelete.end(), intGreaterThan());

  for( unsigned i = 0; i < toDelete.size(); i++ )
  {
    //make sure then one were deleting actually exists...
    if( static_cast<unsigned>(toDelete[i]) < _allStateVector.size() )
      _allStateVector.erase( _allStateVector.begin() + toDelete[i] + _firstDisplayedPuzzle );
  }

  //now redisplay em, if they're all deleted, display the pairs
  if( _allStateVector.size() > 0 ){
    displayAllPuzzleSolutions();
  } else {
    //add pair matches to allStateVector
    _allStateVector.push_back( pairMatchResults_ );
    displayCurrentPairs();
  }

}

void bfrag_Manager::removePuzzle( int which )
{
  if( static_cast<unsigned>(which) < _allStateVector.size() )
    _allStateVector.erase( _allStateVector.begin() + which + _firstDisplayedPuzzle );

  //now redisplay em, if they're all deleted, display the pairs
  if( _allStateVector.size() > 0 )
    displayAllPuzzleSolutions(); 
  else
  {
    //add pair matches to allStateVector
    _allStateVector.push_back( pairMatchResults_ );
    displayCurrentPairs();
  }
}

void bfrag_Manager::reorderPuzzle( int from, int to )
{
  if(from > to)
    _allStateVector[from + _firstDisplayedPuzzle].tCost = _allStateVector[to + _firstDisplayedPuzzle].tCost - 0.001;
  else
    _allStateVector[from + _firstDisplayedPuzzle].tCost = _allStateVector[to + _firstDisplayedPuzzle].tCost + 0.001;

  searchState removed = searchState(_allStateVector[from]);
  _allStateVector.erase( _allStateVector.begin() + from + _firstDisplayedPuzzle );
  if( to < from )
    _allStateVector.insert( _allStateVector.begin() + to + _firstDisplayedPuzzle, removed );
  else if( static_cast<unsigned>(to) == _allStateVector.size() )
    _allStateVector.push_back( removed );
  else
    _allStateVector.insert( _allStateVector.begin() + to+1 + _firstDisplayedPuzzle, removed );

  displayAllPuzzleSolutions();
}

void bfrag_Manager::showPuzzleCost(int index)
{
  vgui_dialog cost_box("Solution Cost is...");
  cost_box.set_cancel_button(false);
  char temp[1000];
  vcl_sprintf(temp, "%f", _allStateVector[index + _firstDisplayedPuzzle].tCost);
  cost_box.message(temp);
  cost_box.ask();
}

void bfrag_Manager::showPuzzleCosts()
{
  vgui_dialog cost_box("Solution Costs are...");
  cost_box.set_cancel_button(false);
  for(unsigned i=0; i < _allStateVector.size(); i++)
  {
    vcl_stringstream cost_message;
    cost_message << "top-" << i+1 << ": " << _allStateVector[i + _firstDisplayedPuzzle].tCost << vcl_endl;
    cost_box.message(cost_message.str().c_str());
  }
  cost_box.ask();
}

void bfrag_Manager::updatePuzzleCost(int index)
{
  vgui_dialog cost_box("Update Solution Cost...");
  cost_box.message("All partial solution costs are as follows:\n");
  for(unsigned i=0; i < _allStateVector.size(); i++)
  {
    vcl_stringstream cost_message;
    cost_message << "top-" << i+1 << ": " << _allStateVector[i + _firstDisplayedPuzzle].tCost << vcl_endl;
    cost_box.message(cost_message.str().c_str());
  }

  double new_cost = _allStateVector[index + _firstDisplayedPuzzle].tCost;
  cost_box.field("Change cost to: ", new_cost);
  if(cost_box.ask())
  {
    _allStateVector[index + _firstDisplayedPuzzle].tCost = new_cost;
    this->displayAllPuzzleSolutions();
  }
}

void bfrag_Manager::updatePuzzleCosts()
{
  vgui_dialog cost_box("Update Solution Costs...");
  vcl_vector<double> new_costs;
  new_costs.resize(_allStateVector.size());

  for(unsigned i=0; i < _allStateVector.size(); i++)
    new_costs[i] = _allStateVector[i + _firstDisplayedPuzzle].tCost;

  for(unsigned i=0; i < _allStateVector.size(); i++)
  {
    vcl_stringstream cost_message;
    cost_message << "top-" << i+1 << " current cost: " << _allStateVector[i + _firstDisplayedPuzzle].tCost << ", change to";
    cost_box.field(cost_message.str().c_str(), new_costs[i]);
  }

  if(cost_box.ask())
  {
    for(unsigned i=0; i < _allStateVector.size(); i++)
      _allStateVector[i + _firstDisplayedPuzzle].tCost = new_costs[i];

    this->displayAllPuzzleSolutions();
  }
}

void bfrag_Manager::generatePuzzle(){
  vgui_dialog gen( "Generator..." );
  gen.message( "Automatic Puzzle File Generation" );
  vcl_string frontContourDir, backContourDir, frontImageDir, backImageDir, blank;
  gen.file( "Front Contour Directory ", blank, frontContourDir );
  gen.file( "Back Contour Directory ", blank, backContourDir );
  gen.file( "Front Image Directory ", blank, frontImageDir );
  gen.file( "Back Image Directory ", blank, backImageDir );
  gen.message( "Front Contour Directory is required" );

  if( gen.ask() ){
    if( frontContourDir.length() < 2 ){
      vgui_dialog err( "Error" );
      err.message( "You must specify a front contour directory." );
      if( err.ask() ){
        generatePuzzle();
      } else {
        return;
      }
    }

    //processing is done by the manager...
    theBfrag2DManager->generatePuzzle( frontContourDir, backContourDir, frontImageDir, backImageDir );
  }
}


void bfrag_Manager::get_image_filenames(vcl_string input)
{
  // open input file
  vcl_ifstream in(input.c_str());
  // create xml reader
  bxml_stream_read reader;
  bxml_data_sptr data;
  unsigned int depth;
  // read parameters
  while(data = reader.next_element(in, depth))
  {
    bxml_element* elem = static_cast<bxml_element*>(data.ptr());
    vcl_string att_name;
    if(elem->name() == "data")
    {
      unsigned id_no = 0;
      while(1)
      {
        char id_name[10000];
        char path_name[10000];
        sprintf(id_name, "id%d", id_no);
        sprintf(path_name, "path%d", id_no);
        id_no++;
        vcl_string no;
        vcl_string path;
        elem->get_attribute(id_name, no);
        if(no != "")
        {
          elem->get_attribute(path_name, path);
          FRAG_PATHS.push_back(path);
        }
        else
          break;
      }
    }
  }

  for(unsigned i=0; i<FRAG_PATHS.size(); i++)
  {
    vcl_string folder = FRAG_PATHS[i];
    folder.append("/");
    // brackets are for putting variables out of scope
    vcl_string ftype = folder;
    ftype.append("*_front.jpg");
    vul_file_iterator f(ftype);
    if(f.filename() != NULL) // there is top imageben de optum
    {
      vcl_string fname = folder;
      fname.append(f.filename());
      FRAG_PATHS[i] = fname;
    }
  }

  images_to_use_fnames.clear();
  for(unsigned i=0; i<FRAG_PATHS.size(); i++)
    images_to_use_fnames.push_back(FRAG_PATHS[i]);

  for(unsigned i=0; i < images_to_use_fnames.size(); i++)
    images_to_use_resource.push_back(vil_load_image_resource(images_to_use_fnames[i].c_str()));
}

void bfrag_Manager::load_xml(vcl_string filename, int chosen, vcl_string filename_images)
{
  if(chosen == 0)
  {
    SHOW_IMAGES = false;
    SHOW_CONTOURS = true;
  }
  else if(chosen == 1)
  {
    SHOW_IMAGES = true;
    SHOW_CONTOURS = false;
  }
  else if(chosen == 2)
  {
    SHOW_IMAGES = true;
    SHOW_CONTOURS = true;
  }
  _puzzleSolver = new PuzzleSolving();
  int pos = filename.find_last_of("\\");
  filename.erase(pos, filename.size());
  _puzzleSolver->read_experiment_search_state_and_puzzle_solving_objects(filename, states_, false);
  vcl_cout << "SEARCH STATES AND PUZZLE SOLVING OBJECT LOADED..." << vcl_endl;
  // load pairs and partial solutions
  _originalPairMatchResults = states_[0];
  pairMatchResults_ = states_[0];
  _allStateVector.clear();
  _allStateVector = states_;
  _curIteration = 1;

  loadedPuzzle = true;
  numPuzzlePieces = _Contours.size();
  theBfrag2DManager->clearTheFrags();
  for(unsigned i=0; i<_Contours.size(); i++)
  {
    bfrag2D *c = new bfrag2D();
    c->topContourCurve = _Contours[i];
    theBfrag2DManager->theFrags.push_back(c);
  }
  if(SHOW_IMAGES)
  {
    get_image_filenames(filename_images);
    // This is needed for associating images with states
    for(unsigned i=0; i < _allStateVector.size(); i++)
      _allStateVector[i].state_id_ = i;
    // create the images to show and associate them with the states
    for(unsigned i=0; i<images_to_show.size(); i++)
      images_to_show[i].first.clear();
    images_to_show.clear();
    offsets_.clear();
    images_to_show.resize(states_.size());

    searchState state;

    for( unsigned w = 0; w < states_.size(); w++ )
    {
      images_to_show[w].second = w;
      vcl_cout << "Processing State " << w << vcl_endl;

      state = states_[w];
      vbl_array_2d<unsigned> num_contrib;

      state.load_state_curves_list();
      if(state.is_constr() == false)
        state.structure();

      int offset_x = 10000000000;
      int offset_y = 10000000000;
      int imsize_w = 0;
      int imsize_h = 0;

      for(unsigned i=0; i<state._constr.size(); i++)
      {
        double w = state.constr(i)->box_.width();
        double h = state.constr(i)->box_.height();
        double minx = state.constr(i)->box_.min_x();
        double miny = state.constr(i)->box_.min_y();
        if(minx < offset_x)
          offset_x = vcl_floor(minx);
        if(miny < offset_y)
          offset_y = vcl_floor(miny);
        if(imsize_w < w)
          imsize_w = w;
        if(imsize_h < h)
          imsize_h = h;
      }
      offset_x = -offset_x;
      offset_y = -offset_y;

      vbl_array_2d<int> frag_id(imsize_w, imsize_h);
      frag_id.fill(-1);
      images_to_show[w].first.set_size(imsize_w, imsize_h, 3);
      images_to_show[w].first.fill(0);
      num_contrib.resize(imsize_w, imsize_h);
      num_contrib.fill(0);

      for( int i = 0; i < state.nProcess; i++) 
      {
        int curve_index = state.process[i];
        vil_image_view<vxl_byte> im = vil_convert_cast(vxl_byte(), images_to_use_resource[curve_index]->get_view());

        vcl_vector<vgl_point_2d<double> > curve = _Contours[curve_index].level3_;
        vnl_matrix_fixed<double,3,3> trans = state.transform_list_[curve_index];

        vgl_polygon<double> polygon(curve);
        vgl_polygon_scan_iterator<double> psi(polygon, false);
        for (psi.reset(); psi.next(); ) 
        {
          int y = psi.scany();
          for (int x = psi.startx(); x <= psi.endx(); ++x) 
          {
            float xx = x;
            float yy = y;
            int xxx = static_cast<int>(vcl_floor(xx+0.5f));
            int yyy = static_cast<int>(vcl_floor(yy+0.5f));

            double new_x = xxx * trans(0,0) + yyy * trans(0,1) + trans(0,2);
            double new_y = xxx * trans(1,0) + yyy * trans(1,1) + trans(1,2);

            int new_x_pos = static_cast<int> (vcl_floor(new_x + 0.5)) + offset_x;
            int new_y_pos = static_cast<int> (vcl_floor(new_y + 0.5)) + offset_y;

            if(new_x_pos < 0 || new_y_pos < 0 || 
              new_x_pos >= images_to_show[w].first.ni() || new_y_pos >= images_to_show[w].first.nj())
              continue;

            if(xxx < 0 || yyy < 0 || xxx >= im.ni() || yyy >= im.nj())
              continue;

            int red, green, blue;
            int num_contrib_here = num_contrib(new_x_pos, new_y_pos);
            if(num_contrib_here > 0 && frag_id(new_x_pos, new_y_pos) == curve_index)
            {
              red   = (im(xxx, yyy, 0) + (images_to_show[w].first)(new_x_pos, new_y_pos, 0) * num_contrib_here) / (num_contrib_here+1);
              green = (im(xxx, yyy, 1) + (images_to_show[w].first)(new_x_pos, new_y_pos, 1) * num_contrib_here) / (num_contrib_here+1);
              blue  = (im(xxx, yyy, 2) + (images_to_show[w].first)(new_x_pos, new_y_pos, 2) * num_contrib_here) / (num_contrib_here+1);
            }
            else if(num_contrib_here > 0 && frag_id(new_x_pos, new_y_pos) != curve_index)
            {
              red   = 255;
              green = 0;
              blue  = 0;
            }
            else
            {
              red   = im(xxx, yyy, 0);
              green = im(xxx, yyy, 1);
              blue  = im(xxx, yyy, 2);
              frag_id(new_x_pos, new_y_pos) = curve_index;
            }
            (images_to_show[w].first)(new_x_pos, new_y_pos, 0) = red;
            (images_to_show[w].first)(new_x_pos, new_y_pos, 1) = green;
            (images_to_show[w].first)(new_x_pos, new_y_pos, 2) = blue;
            num_contrib(new_x_pos, new_y_pos) += 1;
          }
        }
        int p = state.process[i];
      }
      num_contrib.clear();
      vcl_pair<vgl_point_2d<int>, int> temp_offset;
      temp_offset.first = vgl_point_2d<int> (offset_x, offset_y);
      temp_offset.second = w;
      offsets_.push_back(temp_offset);

      // fill the gaps arising from rotation
      vil_image_view<vxl_byte> temp_image;
      temp_image.deep_copy(images_to_show[w].first);
      unsigned image_w = images_to_show[w].first.ni();
      unsigned image_h = images_to_show[w].first.nj();
      for(unsigned j=1; j<image_h-1; j++)
      {
        for(unsigned i=1; i<image_w-1; i++)
        {
          if((images_to_show[w].first)(i,j) == 0)
          {
            int r = (int)(((images_to_show[w].first)(i-1,j,0) +  (images_to_show[w].first)(i+1,j,0) + 
              (images_to_show[w].first)(i,j-1,0) + (images_to_show[w].first)(i,j+1,0))/4.0);
            int g = (int)(((images_to_show[w].first)(i-1,j,1) +  (images_to_show[w].first)(i+1,j,1) + 
              (images_to_show[w].first)(i,j-1,1) + (images_to_show[w].first)(i,j+1,1))/4.0);
            int b = (int)(((images_to_show[w].first)(i-1,j,2) +  (images_to_show[w].first)(i+1,j,2) + 
              (images_to_show[w].first)(i,j-1,2) + (images_to_show[w].first)(i,j+1,2))/4.0);

            temp_image(i,j,0) = r; temp_image(i,j,1) = g; temp_image(i,j,2) = b;
          }
        }
      }
      images_to_show[w].first.deep_copy(temp_image);
    }
  }
}

void bfrag_Manager::check_and_update_status_file(vcl_string foldername, int num_pieces_added)
{
  vcl_string filename = foldername;
  filename.append("/status.txt");
  FILE *fp = vcl_fopen(filename.c_str(), "r");
  if(fp == NULL) // no status file present, so no update necessary
    return;
  else
  {
    vcl_fclose(fp);
    fp = vcl_fopen(filename.c_str(), "w");
    vcl_fprintf(fp, "Number_of_Iterations_Finished: %d\n", num_pieces_added-1);
    vcl_fprintf(fp, "Number_of_Pieces_Added_to_the_Puzzle: %d\n", num_pieces_added);
    vcl_fclose(fp);
  }
}

void bfrag_Manager::check_and_update_assemblies_file(std::string foldername, int file_number)
{
  vcl_string filename = foldername;
  // convert integer to string
  char buffer[32];
  itoa(file_number, buffer, 10);
  filename += "/";
  filename += buffer;
  filename += "frag_assembly.xml";
  FILE *fp = vcl_fopen(filename.c_str(), "r");
  if(fp == NULL) // no assembly file present, so no update necessary
    return;
  else
  {
    vcl_fclose(fp);
    this->_puzzleSolver->write_frag_assemblies_in_xml(filename, this->states_);
  }
}

void bfrag_Manager::save_xml(vcl_string filename)
{
  if(_puzzleSolver == NULL)
    vcl_cout << "PUZZLE SOLVING OBJECT IS EMPTY, NO OUTPUT WRITTEN..." << vcl_endl;
  else
  {
    // replace \ with / in the filename
    while(1)
    {
      int pos = filename.find_last_of("\\");
      if(pos == vcl_string::npos)
        break;
      filename[pos] = '/';
    }
    int pos = filename.find_last_of("/");
    filename.erase(pos, filename.size());
    states_ = _allStateVector;
    _puzzleSolver->write_experiment_search_state_and_puzzle_solving_objects(filename, states_, false);
    check_and_update_status_file(filename, states_[0].nProcess);
    // If "N" pieces were added, the assembly xml filename starts with "N-2"
    check_and_update_assemblies_file(filename, states_[0].nProcess-2);
    vcl_cout << "SEARCH STATES AND PUZZLE SOLVING OBJECT WRITTEN TO DISK..." << vcl_endl;
  }
}
