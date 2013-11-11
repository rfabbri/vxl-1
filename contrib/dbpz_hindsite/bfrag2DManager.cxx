/*************************************************************************
 *    NAME: Daniel Spinosa
 *    FILE: bfrag2DManager.cpp
 *    DATE: 
 *************************************************************************/

#include "bfrag2DManager.h"
#include <vcl_cstdio.h>


bfrag2DManager::bfrag2DManager()
{
    numPieces = 0;    
}

bfrag2DManager::~bfrag2DManager(){
    clearTheFrags();
    clearAssemblies();
    clearTheViews();
}

void bfrag2DManager::displayCurve( Curve<double,double> *theCurve ){

    thebfrag2DTableau->addCurve( theCurve );

}

int bfrag2DManager::getSelected( vcl_vector<int> &cols, vcl_vector<int> &rows ){
    vcl_vector<int> whoCares;
    return theGrid->get_selected_positions( &cols, &rows, &whoCares );
}

void bfrag2DManager::newGrid( int w, int h, int numPairs, int numPuzzles ){
    //vcl_cout << "creating new grid w/ numpairs: " << numPairs << vcl_endl;
    //vcl_cout << "2d manager creating a new grid: " << w << "x" << h << vcl_endl;

    //the views will all leak if not deleted now
    clearTheViews();

    theShell->remove(theGrid);
    gridBfrag2DTableaux.clear();

    vgui_bfrag2D_tableau_sptr newBfragTab;
    vgui_viewer2D_tableau_sptr newViewerTab;

    theGrid = vgui_grid_tableau_new(w,h);
    //so that the user can't modify the views in the grid and the grid itself:
    theGrid->set_uses_paging_events(false);
    theGrid->set_grid_size_changeable(false);

    int i = 0;
    for( int yy = 0; yy < w; yy++ ){
        for( int xx = 0; xx < h; xx++ ) {
            //newBfragTab = new vgui_bfrag2D_tableau();
            newBfragTab = vgui_bfrag2D_tableau_new();
            newBfragTab->setIndex(i++);
            newBfragTab->setNumPairs( numPairs );
            newBfragTab->setNumPuzzles( numPuzzles );
            gridBfrag2DTableaux.push_back(newBfragTab);

            newViewerTab = vgui_viewer2D_tableau_new(newBfragTab);
            viewerTableaux.push_back( newViewerTab );

            theGrid->add_at(newViewerTab,xx,yy);
        }
    }

    theShell->add(theGrid);

}

void bfrag2DManager::addPointAt( float x, float y, int gridIndex, const vgui_style_sptr& style ){
    gridBfrag2DTableaux[gridIndex]->add_point( x, y );
}

void bfrag2DManager::addCurveAt( Curve<double,double> *theCurve, int gridIndex, const vgui_style_sptr& style ){

    //vcl_cout << "adding curve at pos: " << gridIndex << vcl_endl;
    gridBfrag2DTableaux[gridIndex]->addCurve( theCurve, style );

}

void bfrag2DManager::displayBfragAt( int which, int gridIndex, XForm3x3 *theXForm ){
    assert( static_cast<unsigned>(which) < theFrags.size() );
    theViews.push_back( gridBfrag2DTableaux[gridIndex]->addbfrag2D( theFrags[which], theXForm ) );

}

vcl_vector<Curve<double,double> > bfrag2DManager::getTopContoursForPuzzleSolving()
{
    //vcl_cout << "bfrag2DManager: creating Curve<double,double> of top contours for use in PuzzleSolving" << vcl_endl;
    vcl_vector<Curve<double,double> > topContours;

    //make sure we have at least 2 curves loaded
    if( theFrags.size() < 2 ){
        vgui_dialog warn("Error");
        //ok_dl.set_modal(true);
        warn.message("Cannot begin pairwise matching with less than 2 fragments loaded");
        warn.set_cancel_button( 0 );
        warn.ask();
        return topContours;
    }



    Curve<double,double> newCurve;

    //convert from our polyline to a Curve<double,double> and put it in the vector
    for( unsigned i = 0; i < theFrags.size(); i++ ){
        newCurve = theFrags[i]->getTopContourAsCurve();    
        topContours.push_back( newCurve );
    } 

    //vcl_cout << "bfrag2DManager returning Curve list of " << topContours.size() << " curves." << vcl_endl;
    return topContours;
}

void bfrag2DManager::getBfragNameList( vcl_vector<vcl_string> &outList ){
    
    char indexCHAR[128];
    vcl_string cur;

    for( vcl_vector< bfrag2D* >::iterator it = theFrags.begin();
          it != theFrags.end();
          ++it ) {

              sprintf( indexCHAR, "%i", (*it)->getIndex() );

              cur = vcl_string(vcl_string(indexCHAR) + ": \"" + (*it)->getName() + "\" ");
              if( (*it)->getTopCurveFn().size() != 0 ){
                  cur += " [X]";
              } else {
                  cur += " [_]";
              }
              if( (*it)->getTopImageFn().size() != 0 ){
                  cur += " [X]";
              } else {
                  cur += " [_]";
              }
              if( (*it)->getBotCurveFn().size() != 0 ){
                  cur += " [X]";
              } else {
                  cur += " [_]";
              }
              if( (*it)->getBotImageFn().size() != 0 ){
                  cur += " [X]";
              } else {
                  cur += " [_]";
              }

              
              outList.push_back(vcl_string(cur));
              

          }

}

/*
void bfrag2DManager::display( int which ){
    theViews.clear();
    if( which <= theFrags.size())
        theViews.push_back( thebfrag2DTableau->addbfrag2D( theFrags[which] ) );
}
*/

void bfrag2DManager::displayAssembly( vcl_vector< bool > whichFrags ){
    clearAssemblies();
    bool assemblyExists = false;
    //vcl_cout << "bfrag2DManager: creating NEW ASSEMBLY - whichFrags size: " << whichFrags.size() << vcl_endl;

    double d[16];

    bfrag2D_assembly *newAssembly = new bfrag2D_assembly();

    for( unsigned i = 0; i < whichFrags.size(); i++ ){
        //vcl_cout << "which frags is: " << whichFrags[i] << vcl_endl;
        if(whichFrags[i]){
            assemblyExists = true;

            //vcl_cout << "Adding piece [" << i << "] to assembly" << vcl_endl;

            //XXX HACK: d is not a matrix yet!
            newAssembly->addPiece( theFrags[i], d );                

        }
    }

    if(assemblyExists){
        theAssemblies.push_back( newAssembly );
        thebfrag2DTableau->addbfrag2D_assembly( newAssembly, &theViews );
    }


}

//creates a new PZ2 file using the currently loaded items
int bfrag2DManager::savePZ2as( vcl_string fn ){

    //vcl_cout << "saving puzzle as: " << fn << "." << vcl_endl;
    //vcl_cout << "-------------------------------------------------" << vcl_endl;


    //try to open file, w/o creating
    //vcl_ifstream check( fn.c_str(), ios::nocreate );
    vcl_ifstream check( fn.c_str(), vcl_ios_in );

    if( check.good() ){
        check.close();

        //file already existed, make sure they want this
        vgui_dialog ok_dl("Are You Sure?");
        //ok_dl.set_modal(true);
        ok_dl.message("File Already Exists, Overwrite?");
        ok_dl.set_ok_button( "Yes" );
        ok_dl.set_cancel_button( "No" );
        if( !ok_dl.ask()){
            //on "NO" return to file choose dialog
            return -1;
        } 
    }

    //they either want to overwrite, or its new, so here goes
    vcl_ofstream output( fn.c_str() );

    //make sure it opened for writing
    if( !output.good() ){
        vgui_dialog ok_dl("Are You Sure?");
        //ok_dl.set_modal(true);
        ok_dl.message("File could not be opened for writing.");
        ok_dl.set_ok_button( "Try Again..." );
        ok_dl.set_cancel_button( "Cancel" );
        if( ok_dl.ask() ){
            //on "Try Again" return to file choose dialog
            return -1;
        } else {
            //on cancel, get outta here
            return -2;
        }
    }

    //write header
    output << "PZ2 (Puzzle, version 2.0)\n\nFile Automatically Generated by: The Fragment Assembler\n\n(C) 2004 -- Lems @ Brown University\n\n";

    output << "<PZ2>\n\n";

    if( _noteStrings.size() > 0 ){
        output << "<Note> \n";
        for( unsigned i = 0; i < _noteStrings.size(); i++ ){
            output << _noteStrings[i] << "\n";
        }
        output << "</Note>\n\n";
    }

    for( vcl_vector< bfrag2D* >::iterator it = theFrags.begin();
         it != theFrags.end();
         ++it ) {
        output << "<" << (*it)->getIndex() << ">\n";
        output << "<Name> \"" << (*it)->getName() << "\"\n";
        output << "<TopCurve> \"" << (*it)->getTopCurveFn() << "\"\n";
        if ( ((*it)->getTopImageFn()).size() != 0 )
            output << "<TopImage> \"" << (*it)->getTopImageFn() << "\"\n";
        if ( ((*it)->getBotCurveFn()).size() != 0 )
            output << "<BotCurve> \"" << (*it)->getBotCurveFn() << "\"\n";
        if ( ((*it)->getBotImageFn()).size() != 0 )
            output << "<BotImage> \"" << (*it)->getBotImageFn() << "\"\n";
        output << "</" << (*it)->getIndex() << ">\n\n";
    }

    output << "</PZ2>";




    output.close();

    //on success
    return 0;


}

//add a piece to the current puzzle
int bfrag2DManager::addPiece( vcl_string topConFn, vcl_string botConFn, 
                              vcl_string topImageFn, vcl_string botImageFn, vcl_string name ){

    //we are assured by bfrag_Manager topConFn is not null
    assert( topConFn.size() > 4 );

    bfrag2D *newFrag = new bfrag2D( numPieces + 1, name );

    int errnum;

    if( (errnum = newFrag->loadContour( topConFn, 1 )) != 0 ){
        char errstr[256];
        vcl_sprintf(errstr, "Error (%i): Contour -%s- Could Not Be Loaded! Aborting.", errnum, topConFn.c_str());
        return parseFail( errstr );
    }
    if( (errnum = newFrag->loadImage( topImageFn, 1 )) != 0 ){
        char errstr[256];
        vcl_sprintf(errstr, "Error (%i): Image -%s- Could Not Be Loaded! Aborting.", errnum, topImageFn.c_str());
        return parseFail( errstr );
    }
    if( (errnum = newFrag->loadContour( botConFn, 0 )) != 0 ){
        char errstr[256];
        vcl_sprintf(errstr, "Error (%i): Contour -%s- Could Not Be Loaded! Aborting.", errnum, botConFn.c_str());
        return parseFail( errstr );
    }
    if( (errnum = newFrag->loadImage( botImageFn, 0 )) != 0 ){
        char errstr[256];
        vcl_sprintf(errstr, "Error (%i): Image -%s- Could Not Be Loaded! Aborting.", errnum, botImageFn.c_str());
        return parseFail( errstr );
    }

    theFrags.push_back( newFrag );

    //increment numPieces and make sure its teh same as the vector size
    if( ++numPieces == static_cast<int>(theFrags.size()) ){
        //vcl_cout << "---Completed Successfull Piece Addition of piece [" << numPieces << "]." << vcl_endl;
    } else {
        //vcl_cout << "NOT GOOD vectorsize: " << theFrags.size() << "!= numPieces: " << numPieces << vcl_endl;
    }

    return numPieces;

}

int bfrag2DManager::loadPuzzle( vcl_string fn ){
    //out with the old, in with the new...
    //bfrag2DManager::clearTheFrags deletes them too
    clearTheFrags();
    numPieces = 0;
    _noteStrings.clear();

    //now load a new one!
    return addPuzzle( fn );
}


//load a .pz2 version 1.0 file (different spec from .puz files)
int bfrag2DManager::addPuzzle( vcl_string fn ){

    //vcl_cout << "adding puzzle: " << fn << "." << vcl_endl;
    //vcl_cout << "-------------------------------------------------" << vcl_endl;

    //open file, make sure it opened
    vcl_ifstream input( fn.c_str(), vcl_ios_in );

    if( !input.good() ){
        //could not open file

        vcl_cout << vcl_endl << "FILE DNE or BAD PERMISSIONS:" << fn.c_str() << vcl_endl;

        vgui_dialog ok_dl("Sorry");
        //ok_dl.set_modal(true);
        ok_dl.message("File Does Not Exist or you do not have sufficient permissions");
        ok_dl.set_ok_button( "Try Again..." );
        if( ok_dl.ask()){
            //on "ok" or "try again" user is returned to open dialogue
            return -1;
        } else {
            //on cancel they will be returned to program
            return -2;
        }
    }


    char token[512], topCurveFn[512], topImageFn[512], 
                     botCurveFn[512], botImageFn[512], pieceName[1024];
    
    bfrag2D* curFrag = 0;
    int errnum;
    

    //this works fine so long as there is whitespace between everything
    input >> token;

    //vcl_cout << "cur token: " << token << vcl_endl;

    while( strcmp( token, "<PZ2>" ) && !input.eof() ){ 
        input >> token; 
        //vcl_cout << "waiting for PZ2, cur token: " << token << vcl_endl;
    }

    //vcl_cout << "found PZ2 tag: " << token << vcl_endl;


    while( !input.eof() ){
        input >> token;

        //vcl_cout << "Processing token: " << token << vcl_endl;
        if( strcmp( token, "<Note>" ) == 0 ){
            //parse note
            //input >> token;
            //_noteString += token;

            vcl_string curString = vcl_string();
            
            //vcl_cout << "NOTE[";

            while( 1 ){
                if( input.eof() ){
                    return parseFail( "Error: <Note> tag not terminated" );
                }

                input >> token;

                //vcl_cout << token;

                if( !strcmp( token, "</Note>" ) ) break;

                if( input.peek() == 10 ){       //newline
                    //vcl_cout << vcl_endl;

                    curString += vcl_string(token);
                    _noteStrings.push_back( curString );
                    curString = vcl_string();
                } else if( input.peek() == 32 ){ //space                    
                    curString += token;
                    curString += " ";
                }                 
            }
            //vcl_cout << "]END NOTE" << vcl_endl;

            _noteStrings.push_back( curString );
            
        } else if( strcmp( token, "<Name>") == 0 ) {
            if( !curFrag) return parseFail( "No Current Piece: Expected <N> to begin new bfrag" );
            if( extractFileName( input, pieceName ) == -1)
                return parseFail( "Error: Unbalanced \" " );
            curFrag->setName( vcl_string(pieceName) );
        } else if( strcmp( token, "<TopCurve>") == 0 ) {
            if( !curFrag ) return parseFail( "No Current Piece: Expected <N> to begin new bfrag" );
            if( extractFileName( input, topCurveFn ) == -1)
                return parseFail( "Error: Unbalanced \" " );
            //vcl_cout << "Top Curve for frag " << curFrag->getIndex() << " is: " << topCurveFn << vcl_endl;
            if( (errnum = curFrag->loadContour( topCurveFn, 1 )) != 0 ){
                char errstr[256];
                sprintf(errstr, "Error (%i): Contour -%s- Could Not Be Loaded! Aborting.", errnum, topCurveFn);
                return parseFail( errstr );
            }

                
        } else if( strcmp( token, "<TopImage>") == 0 ) {
            if( !curFrag ) return parseFail( "No Current Piece: Expected <N> to begin new bfrag" );
            if( extractFileName( input, topImageFn ) == -1)
                return parseFail( "Error: Unbalanced \" " );
            //vcl_cout << "Top Image for frag " << curFrag->getIndex() << " is: " << topImageFn << vcl_endl;
            if( (errnum = curFrag->loadImage( topImageFn, 1 )) != 0 ){
                char errstr[256];
                sprintf(errstr, "Error (%i): Image -%s- Could Not Be Loaded! Aborting.", errnum, topImageFn);
                return parseFail( errstr );
            }


        } else if( strcmp( token, "<BotCurve>") == 0 ) {
            if( !curFrag ) return parseFail( "No Current Piece: Expected <N> to begin new bfrag" );
            if( extractFileName( input, botCurveFn ) == -1)
                return parseFail( "Error: Unbalanced \" " );
            //vcl_cout << "Bot Curve for frag " << curFrag->getIndex() << " is: " << botCurveFn << vcl_endl;
            if( (errnum = curFrag->loadContour( botCurveFn, 0 )) != 0 ){
                char errstr[256];
                sprintf(errstr, "Error (%i): Contour -%s- Could Not Be Loaded! Aborting.", errnum, botCurveFn);
                return parseFail( errstr );
            }


        } else if( strcmp( token, "<BotImage>") == 0 ) {
            if( !curFrag ) return parseFail( "No Current Piece: Expected <N> to begin new bfrag" );
            if( extractFileName( input, botImageFn ) == -1)
                return parseFail( "Error: Unbalanced \" " );
            //vcl_cout << "Bot Image for frag " << curFrag->getIndex() << " is: " << botImageFn << vcl_endl;
            if( (errnum = curFrag->loadImage( botImageFn, 0 )) != 0 ){
                char errstr[256];
                sprintf(errstr, "Error (%i): Image -%s- Could Not Be Loaded! Aborting.", errnum, botImageFn);
                return parseFail( errstr );
            }


        } else if( strcmp( token, "</PZ2>" ) == 0 ) {
            //we should leave the while loop
            if( curFrag ){
                return parseFail( "Reached puzzle closing </PZ2> tag but there is still a fragment open. Aborting" );
            }
            //vcl_cout << "REACHED END OF PZ2" << vcl_endl;
            break;
        

        } else if( (strncmp(token, "<", 1) == 0) &&
                   (strncmp(token + strlen(token) - 1, ">", 1) == 0) ){
            // if we've reached this point, we expect a number inside of
            // the brackets.  all other cases are taken care of, if its NOT
            // a number, FAIL
            
            if( strncmp(token + 1, "/", 1) == 0 ){
                //we hit a CLOSE PIECE
                if( curFrag == 0 ){
                    return parseFail( "Reached fragment close tag </N> but there is no open fragment. Aborting." );
                }
                
                char indexASCII[128];
                strncpy( indexASCII, token+2, strlen(token) - 3 );
                indexASCII[strlen(token)-3] = 0;
                if( atoi(indexASCII) != curFrag->getIndex() ){
                    return parseFail( "Error: Frag Close tag index does not match Frag Open tag index.  -Aborting-" );
                }

                theFrags.push_back( curFrag );
                curFrag = 0;

            } else {  //NEW FRAG

                //make sure were expecting a new piece
                if( curFrag ) {
                    return parseFail( "Error: Frag Open tag found, last Frag not closed.  -Aborting-");
                }

                //if its a BEGIN make new curFrag, clear all FN variables
                numPieces++;
                
                //STORE THE INDEX!!!!
                char indexASCII[128];
                strncpy( indexASCII, token + 1, strlen(token) - 2 );
                indexASCII[strlen(token)-2] = 0;
                //vcl_cout << "GRABBED " << indexASCII << " AS NEW INDEX " << vcl_endl;


                curFrag = new bfrag2D( atoi(indexASCII) );
                //vcl_cout << "NEW FRAG: index: " << curFrag->getIndex() << vcl_endl;


                *topCurveFn = 0;
                *topImageFn = 0;
                *botCurveFn = 0;
                *botImageFn = 0;
            } 
           

        } else {
            //BAD TOKEN
            char errstr[256];
            sprintf(errstr, "Error: invalid token: \"%s\"  -Aborting-", token);
            return parseFail( errstr );
        }
    }

    //make sure we loaded SOMETHING
    if( numPieces == 0 ){
        return parseFail( "File did not contain any PZ2 instructions" );
    }

    if( numPieces == static_cast<int>(theFrags.size()) ){
        //vcl_cout << "---Completed Successfull PZ2 loading of (" << numPieces << ") pieces." << vcl_endl;
    } else {
        //vcl_cout << "NOT GOOD vectorsize: " << theFrags.size() << "!= numPieces: " << numPieces << vcl_endl;
    }

    //give user the note
    if( _noteStrings.size() > 0 ){
        vgui_dialog ok_dl("Puzzle Note");
        //vcl_cout << "Note: ";
        for( unsigned i = 0; i < _noteStrings.size(); i++ ){
            //vcl_cout << _noteStrings[i] << vcl_endl;
            ok_dl.message( _noteStrings[i].c_str() );
        }
        ok_dl.set_cancel_button( 0 );
        ok_dl.ask();
    }

    return numPieces;

}

/*****************************************************************
 *
 * Given an open and valid ifstream this function examines teh next
 * token. If it begins with a " character the functions continues through
 * the ifstream until if finds the trailing " character and returns what
 * was within the " " 's in outFn.  IF EOF is reached -1 is returned
 *****************************************************************/
int bfrag2DManager::extractFileName( vcl_ifstream &input, char* outFn ){
    char token[512];
    input >> token;
    int len;

    assert( input.good() );

    //look for beginning "
    if( strncmp( token, "\"", 1 ) == 0 ){
        //vcl_cout << "extracting File Name within \" \" \'s" << vcl_endl;

        //vcl_cout << "token currently is [" << token << "]" << vcl_endl;
        while( strncmp( token + strlen(token) - 1, "\"", 1 ) != 0 ){
            if( input.eof() ) return -1;

            len = strlen(token);
            strncpy(token+len, " ", 1);
            input >> (token + len+1);
            //vcl_cout << "token currently is [" << token << "]" << vcl_endl;
        }

        //copy after the first " into outFn
        strncpy( outFn, token+1, strlen(token) );
        //remove the trailing " and replace it w/ null terminating character
        *(outFn+strlen(outFn)-1) = 0;

    } else {
        strcpy( outFn, token );
    }


    return 0;

}

int bfrag2DManager::parseFail( vcl_string errmsg ){

    vcl_cout << "PARSER FAILURE: " << errmsg << vcl_endl;

    vgui_dialog ok_dl("Parser Failure");
    //ok_dl.set_modal(true);
    ok_dl.message( errmsg.c_str() );
    ok_dl.set_ok_button( "Open New..." );
    if( ok_dl.ask()){
        //on "ok" or "try again" user is returned to open dialogue
        return -1;
    } else {
        //on cancel they will be returned to program
        return -2;
    }

}


//delete em AND clear the vector
void bfrag2DManager::clearTheFrags(){
    
    for( unsigned i = 0; i  < theFrags.size(); i++ ){
        delete theFrags[i];
    }

    theFrags.clear();   
}

void bfrag2DManager::clearTheViews(){
    for( unsigned i = 0; i  < theViews.size(); i++ ){
        delete theViews[i];
    }

    theViews.clear();   
}

void bfrag2DManager::displayAllTopImages( bool show ){

    //vcl_cout << "top image settings chaning..." << vcl_endl;
    for(unsigned i = 0; i < theViews.size(); i++){
        //vcl_cout << "for view [" <<  i << "] changing draw top image to: " << show << vcl_endl;
        theViews[i]->drawTopImage = show;
    }
    post_redraw();
    redrawTableaux();
}

void bfrag2DManager::displayAllBotImages( bool show ){

    for(unsigned i = 0; i < theViews.size(); i++){
        theViews[i]->drawBotImage = show;
    }
    post_redraw();
    redrawTableaux();
}

void bfrag2DManager::displayAllBotCurves( bool show ){

    for(unsigned i = 0; i < theViews.size(); i++){
        theViews[i]->drawBotCurve = show;
    }
    post_redraw();
    redrawTableaux();
}


void bfrag2DManager::clearAssemblies(){
    for( unsigned i = 0; i < theAssemblies.size(); i++ ){
        delete theAssemblies[i];
    }
    theAssemblies.clear();
}

void bfrag2DManager::redrawTableaux(){

    for( unsigned i = 0; i < gridBfrag2DTableaux.size(); i++ ){
        gridBfrag2DTableaux[i]->post_redraw();
    }

}

int bfrag2DManager::removeBfrag( int where ){
    if( static_cast<unsigned>(where) < theFrags.size() ){
        theFrags.erase(theFrags.begin()+where);
        numPieces--;
        return 0;
    }
    return -1;
}

void bfrag2DManager::generatePuzzle( vcl_string fconDir, vcl_string bconDir, vcl_string fimageDir, vcl_string bimageDir )
{

//Ming: need a cross platform solution
#if 0
    assert(0); //finding files in directory aint workin for me yet

    //for every "-f.con" && "-f.cem" file in fconDir

    WIN32_FIND_DATA lpffd;

    vcl_string strTmp;
    HANDLE hFind;
    int rc, bRet = true;
 
    //strPath = strPath + "\\*.*";
 
    rc = false;
    hFind = FindFirstFile(fconDir.c_str(), &lpffd);

    if (hFind == INVALID_HANDLE_VALUE) {
        printf ("Invalid file handle. Error is %u\n", GetLastError());
    }

    vcl_cout << "fist file is: " << lpffd.cFileName << vcl_endl;

    while( FindNextFile(hFind, &lpffd) != 0){
        vcl_cout << "next file: " << lpffd.cFileName << vcl_endl;
    }

    FindClose(hFind);

    /*
    int addPiece( vcl_string topConFn, vcl_string botConFn, 
                  vcl_string topImageFn, vcl_string botImageFn, vcl_string name );
                  */
#endif
}



