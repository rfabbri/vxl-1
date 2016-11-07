/*************************************************************************
 *    NAME: Daniel Spinosa
 *    FILE: bfrag_Manager.cpp
 *    DATE: 
 *************************************************************************/

#include "bfrag_Manager.h"

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
void bfrag_Manager::pairMatch(){
    //vcl_cout << "--------------bfrag_Manager:: beginning the pairwise matching-----------------" << vcl_endl;
    
    //have the bfrag2DManager create a Curve<double,double> from our contour representation
    //for the top curves and set _Contours in PuzzleSolving to be that
    vcl_vector<Curve<double,double> > bfragContours = theBfrag2DManager->getTopContoursForPuzzleSolving();
    if( bfragContours.size() == 0 ){
        vcl_cout << "bfrag_Manager: ABORTING pairwise matching, not enuf pieces loaded" << vcl_endl;
        return;
    }
    //vcl_cout << "bfrag_Manager received curves from bfrag2DManager of size: " << bfragContours.size() << vcl_endl;
    _puzzleSolver->setContours( bfragContours );
    //_puzzleSolver->printContours();
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
        vcl_cout << "Pair Match running time: " << hr << " hours, " << mn << " min, " << sec << " sec, " << mili << " miliseconds." << vcl_endl;
        //vcl_cout << "____  " << (int)floor(diff/1000.0) << " seconds ____" << vcl_endl;
    //puzzleSolving->pairMatch()
    
    //OLD SORT: costs
    //pairMatchSortedCost_ = pairMatchResults_.cost;
    //vcl_sort(pairMatchSortedCost_.begin(),pairMatchSortedCost_.end(),cost_ind_less());
    //NEW SORT: pairwiseMatches
    pairMatchResults_.sortPairwiseMatches();
    //pairMatchResults_.printPairwiseMatchesSortedNPS();

    displayCurrentPairs();

    //add pair matches to allStateVector
    _allStateVector.push_back( pairMatchResults_ );

    vgui_dialog done_dl("Done!");
    done_dl.message( "Pairwise Matching Complete!" );
    done_dl.message( "Click \"Puzzle Solving | Solve Puzzle\" to finish solving the puzzle" );
    done_dl.set_cancel_button( 0 );
    done_dl.ask();

}

void bfrag_Manager::matchingIteration(){
    if( pairMatchResults_._matches.size() == 0 ){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: Run Pairwise Matching first." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    }
    _curIteration++;
    if( _curIteration == numPuzzlePieces ){
        vgui_dialog done_dl("Done!");
        done_dl.message( "Puzzle Solving Complete!" );
        done_dl.set_cancel_button( 0 );
        done_dl.ask();
        return;
    }
    vcl_cout << "Matching Iteration: " << _curIteration << vcl_endl;

    //vcl_cout << "_allStateVector size BEFORE search: " << _allStateVector.size() << vcl_endl;
    //puzzleSolving->search( _allStateVector );
        clock_t start, end, diff;
        start = clock();
        _allStateVector = _puzzleSolver->search( _allStateVector );
        end = clock();
        diff = end - start;
        double mili = diff % 1000;
        int sec = (int)floor((diff/1000.0)) % 60;
        int mn = (int)floor((diff/60000.0)) % 60;
        int hr = (int)floor((diff/360000.0));
        vcl_cout << "Iteration running time of: " << hr << " hours, " << mn << " min, " << sec << " sec, " << mili << " miliseconds." << vcl_endl;
        //vcl_cout << "____  " << (int)floor(diff/1000.0) << " seconds ____" << vcl_endl;
    //puzzleSolving->search( _allStateVector );
    //vcl_cout << "_allStateVector size AFTER search: " << _allStateVector.size() << vcl_endl;


    displayAllPuzzleSolutions();
    /*
    for( int i = 0; i < _allStateVector.size(); i++ ){
        vcl_cout << "sanity check on AllStateVector[" << i << "]: ";
        _allStateVector[i].sanityCheckMatchesByIndexNPS();
    }
    */

    
}

void bfrag_Manager::completeMatching(){
    if( pairMatchResults_._matches.size() == 0 ){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: Run Pairwise Matching first." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    }
    while( _curIteration < numPuzzlePieces ){
        matchingIteration();
    }
}

void bfrag_Manager::displayAllPuzzleSolutions(){ 
    if( pairMatchResults_._matches.size() == 0 ){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: Run Pairwise Matching first, then Puzzle Solving." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    } else if( _curIteration == 0 ){
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

void bfrag_Manager::displaySolutions(vcl_vector<searchState> *theStates){
    if( theStates->size() == 0 ){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: No solutions to display.  Run Puzzle Solving first." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    }

    searchState state;
    gridWH_ = static_cast<int>(ceil(vcl_sqrt( (double) theStates->size() )));
    theBfrag2DManager->newGrid(gridWH_,gridWH_, -1, theStates->size() );

    for( unsigned w = 0; w < theStates->size(); w++ ){
    //for( int w = 0; w < 1; w++ ){
        state = (*theStates)[w];

        //why do they do this!?
        if( state.nProcess == 0){
            //vcl_cout << "************ state nProcess == 0 ************" << vcl_endl;
            theBfrag2DManager->redrawTableaux();
            break;
        }


        
        //FIRST
        //this draws an outline of the entire solution, but...
        //*** not an outline so much -- its the current contour to match new pieces too
        for(int i = 0; i < state.numCon(); i++) {
            //drawLines(line_node,state->constr(i),color);
            theBfrag2DManager->addCurveAt(state.constr(i), w, vgui_style::new_style(0.3f,0.3f,0.3f,6,6) );
        }
        

        
        //SECOND
        //this draws all the individual pieces
        vgui_style_sptr theStyle = vgui_style::new_style(0,1,0,1,1);
        for( int i = 0; i < state.nProcess; i++) {
            int p = state.process[i];

            if( i == state.nProcess-1 ){
                //the last added piece will be differnt
                theStyle = vgui_style::new_style( 1,1,0, 1.5,1.5); 
            }
            theBfrag2DManager->addCurveAt( state.piece(p), w, theStyle );
            
            
        }

        
        
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

    theBfrag2DManager->post_redraw();

}


void bfrag_Manager::displayCurrentPairs(){
    displayNPairs( pairMatchResults_._matches.size(), 1 );
}

void bfrag_Manager::resetPairs(){
    if( pairMatchResults_._matches.size() > 0 ){
        pairMatchResults_ = _originalPairMatchResults;
        pairMatchResults_.sortPairwiseMatches();   

        displayCurrentPairs();
    }
}

void bfrag_Manager::removePair( unsigned which ){
    if( which < pairMatchResults_._matches.size() ){
        pairMatchResults_._matches.erase( pairMatchResults_._matches.begin() + which + _firstDisplayedPiece );

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
            int junkint = pairMatchResults_._matches.size();
        if( (toDelete[i] < junkint) || (junkint < 0) ){
            //vcl_cout << "now deleting: " << toDelete[i] << vcl_endl;
            pairMatchResults_._matches.erase( pairMatchResults_._matches.begin()+toDelete[i] + _firstDisplayedPiece );

        }
    }

    if( pairMatchResults_._matches.size() > 0 ){
        //now redisplay em
        displayCurrentPairs();
    } else {
        void displayAllContours();
    }
    
}

void bfrag_Manager::displayNPairs(int howMany, int start){
    if(pairMatchResults_._matches.size() == 0){
        vgui_dialog warn_dl("Error!");
        warn_dl.message( "Error: No pairs to display.  Run Pair Matching first." );
        warn_dl.set_ok_button( "Ok" );
        warn_dl.set_cancel_button( 0 );
        warn_dl.ask();
        return;
    }
         int pairjunkint = static_cast<int>(pairMatchResults_._matches.size());
    if( (start > pairjunkint) || (pairjunkint<0) ){
        vgui_dialog error_dl("Display Error");
        char infomsg[2048];
        sprintf( infomsg, "Error: You chose to start at %i but there are only %i pairs to display.", start, pairMatchResults_._matches.size());
        error_dl.message( infomsg );
        error_dl.set_cancel_button(0);
        error_dl.ask();
        return;
    }

    _firstDisplayedPiece = start-1;

    gridWH_ = static_cast<int>(ceil(vcl_sqrt((double) howMany)));
    theBfrag2DManager->newGrid(gridWH_,gridWH_, pairMatchResults_._matches.size());

    int p1, p2;
    Curve<double,double> *c1, *c2;
    assert (start >= 0);
    int pairmatch_int = static_cast<int>(pairMatchResults_._matches.size());
    for( int i = start-1; i < pairmatch_int && i < (start-1)+howMany; i++ ){


        p1 = pairMatchResults_._matches[i].whichCurves.first;
        p2 = pairMatchResults_._matches[i].whichCurves.second;
        c1 = pairMatchResults_.piece(p1);
        c2 = pairMatchResults_.piece(p2);
        
        XForm3x3 *pairXForm = new XForm3x3;
        regContour(c1,c2, pairMatchResults_._matches[i].pointMap, 0, pairXForm );
        //vcl_cout << "xform for pair [" << i << "] is: " << vcl_endl;
        //pairXForm.print();

        matchedPairs_.push_back(vcl_pair<Curve<double,double>*,Curve<double,double>*>(c1,c2));

        //add these two pieces in the same tableau on the grid
        theBfrag2DManager->addCurveAt(c1,i-(start-1));
        theBfrag2DManager->addCurveAt(c2,i-(start-1));

        //TEST
        //TURN OFF THE UPPER CONTOUR OR DONT USE THOSE CURVES AT ALL!!!!??
        //add the image for pice 2, which is not translated
        theBfrag2DManager->displayBfragAt( p2, i-(start-1) );
        //now transform the image for p1 and add it to the display
        theBfrag2DManager->displayBfragAt( p1, i-(start-1), pairXForm );
        
        //TEST

        delete pairXForm;
    }

    

}

void bfrag_Manager::reorderPair( int from, int to ){
    //vcl_cout << "manager reordering from: " << from << " to: " << to << vcl_endl;

    //vcl_cout << "COSTS BEFORE:" << vcl_endl;
    //pairMatchResults_.printPairwiseMatchesSortedNPS();
    unsigned tounsigned = static_cast<unsigned>(to);
    if( tounsigned == 0 ){
        pairMatchResults_._matches[from].cost = pairMatchResults_._matches[to].cost - COST_ADJUSTMENT;
    } else if( tounsigned == pairMatchResults_._matches.size()-1 ){
        pairMatchResults_._matches[from].cost = pairMatchResults_._matches[to].cost + COST_ADJUSTMENT;
    } else if( to > from ){
        double diff = pairMatchResults_._matches[to+1].cost - pairMatchResults_._matches[to].cost;
        pairMatchResults_._matches[from].cost = pairMatchResults_._matches[to+1].cost - diff/2.0;
    } else {
        double diff = pairMatchResults_._matches[to].cost - pairMatchResults_._matches[to-1].cost;
        //new cost is right in between the target and the one before it
        //which puts this just before target, exactly what we want
        //vcl_cout << "ORIG COST: " << pairMatchResults_._matches[from].cost << vcl_endl;
        //vcl_cout << "BETTER COST: [" << pairMatchResults_._matches[to-1].cost << "] WORSE COST: [" << pairMatchResults_._matches[to].cost << "]" << vcl_endl;
        //vcl_cout << "DIFF/2.0 (adjustment): " << diff/2.0 << vcl_endl;
        //vcl_cout << "NEW SCORE: " << pairMatchResults_._matches[to].cost - diff/2.0 << vcl_endl;

        pairMatchResults_._matches[from].cost = pairMatchResults_._matches[to].cost - diff/2.0;
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

    for( int i = start-1; i < numPuzzlePieces && i < (start-1)+howMany; i++ ){   
        theBfrag2DManager->displayBfragAt( i, i-(start-1) );
    }
    
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

    for( unsigned i = 0; i < toDelete.size(); i++ ){
        //make sure then one were deleting actually exists...
        if( static_cast<unsigned>(toDelete[i]) < _allStateVector.size() ){
            _allStateVector.erase( _allStateVector.begin() + toDelete[i] + _firstDisplayedPuzzle );
        }
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

void bfrag_Manager::removePuzzle( int which ){
    if( static_cast<unsigned>(which) < _allStateVector.size() ){
        _allStateVector.erase( _allStateVector.begin() + which + _firstDisplayedPuzzle );
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

void bfrag_Manager::reorderPuzzle( int from, int to ){
    //vcl_cout << "moving puzzle " << from << " to: " << to << vcl_endl;

    searchState removed = searchState(_allStateVector[from]);
    _allStateVector.erase( _allStateVector.begin() + from + _firstDisplayedPuzzle );
    if( to < from ){
        _allStateVector.insert( _allStateVector.begin() + to + _firstDisplayedPuzzle, removed );
    } else if( static_cast<unsigned>(to) == _allStateVector.size() ){
        _allStateVector.push_back( removed );
    } else {
        _allStateVector.insert( _allStateVector.begin() + to+1 + _firstDisplayedPuzzle, removed );
    }
    displayAllPuzzleSolutions();



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

