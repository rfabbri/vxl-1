/*************************************************************************
 *    NAME: Daniel Spinosa
 *    FILE: bfrag_Menu.cpp
 *    DATE: 
 *************************************************************************/
#include "bfrag_Menu.h"
#include <vgui/vgui.h>


bfrag_Menu::bfrag_Menu()
{
    whereToStart_ = 1;
    
}

/*************************************************************************
 * Function Name: bfrag_Menu::~dbvis1_bfrag_Menu
 * Parameters: 
 * Effects: 
 *************************************************************************/

bfrag_Menu::~bfrag_Menu()
{
}

/*************************************************************************
 * Function Name: bfrag_Menu::add_to_menu
 * Parameters:  vgui_menu & menu 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
bfrag_Menu::add_to_menu( vgui_menu & menu )
{
    vgui_menu fileItems;
        fileItems.add( "&Load New Puzzle...", load, (vgui_key)'L', vgui_CTRL );
        //fileItems.add( "&Generate Puzzle...", generatePuzzle, (vgui_key)'G', vgui_CTRL );
        fileItems.add( "Add Puzzle", addPuzzle, (vgui_key)'P', vgui_CTRL );       
        fileItems.separator();
        fileItems.add( "&Save Puzzle", savePZ2, (vgui_key)'S', vgui_CTRL );
        fileItems.add( "Save Puzzle &As...", savePZ2as, (vgui_key)'A', vgui_CTRL );
        //fileItems.separator();
        //fileItems.add( "&PairMatching Options...", pairmatchOptions, (vgui_key)'P', vgui_CTRL );
        //fileItems.add( "Puzzle&Solving Options...", solvingOptions, (vgui_key)'S', vgui_CTRL );
        fileItems.separator();
        fileItems.add( "&Quit", quit, (vgui_key)'Q', vgui_CTRL );
    menu.add( "&File" , fileItems );

    //FRAGS
    vgui_menu pieces;
        pieces.add( "&Trace Contours...", execLiveWire, (vgui_key)'L', vgui_CTRL );
        pieces.separator();
        pieces.add( "&Add A Piece...", addbfrag2D, (vgui_key)'A', vgui_CTRL ); 
        pieces.add( "&Remove selected Pieces", removeSelectedFragment, (vgui_key)'R', vgui_CTRL );
        pieces.separator(); 
        pieces.add( "S&how All Pieces", displayAllContours, (vgui_key)'H', vgui_CTRL);
        pieces.add( "Show &One Piece", displayContour, (vgui_key)'O', vgui_CTRL );
        pieces.add( "Show &N Pieces", displayNContours, (vgui_key)'N', vgui_CTRL);
        pieces.separator(); 
        vgui_menu topImageSettings;
            topImageSettings.add( "Display All", topImagesShow );
            topImageSettings.add( "Display None", topImagesHide );
        pieces.add( "&Top Images", topImageSettings );//Tells name of submenu
        vgui_menu botImageSettings;
            botImageSettings.add( "Display All", botImagesShow );
            botImageSettings.add( "Display None", botImagesHide );
        pieces.add( "&Bottom Images", botImageSettings );
        vgui_menu botCurveSettings;
            botCurveSettings.add( "Display All", botCurvesShow );
            botCurveSettings.add( "Display None", botCurvesHide );
        pieces.add( "Bottom &Curves", botCurveSettings );
    menu.add( "&Fragments", pieces );//Tells name of menu
        
        
    //PAIRS
    vgui_menu matching;
        matching.add( "&Run", pairMatch, (vgui_key)'R', vgui_CTRL );
        matching.separator();
        matching.add( "Re&move Selected Pairs", removeSelectedPairs, (vgui_key)'M', vgui_CTRL );
        matching.add( "Re&set Pairs", resetPairs, (vgui_key)'S', vgui_CTRL );
        matching.separator();
        matching.add( "&Show All Pairs", displayCurrentPairs, (vgui_key)'D', vgui_CTRL );       
        matching.add( "&Show N Pairs...", displayNPairs, (vgui_key)'S', vgui_CTRL );
    menu.add( "&Pairwise Matching", matching );

    //PUZZLE
    vgui_menu solving;
        solving.add( "&Complete Puzzle", completeMatching, (vgui_key)'S', vgui_CTRL );
        solving.add( "&Match Next Piece", matchingIteration, (vgui_key)'M', vgui_CTRL );
        solving.separator();
        solving.add( "&Remove Selected Solutions", removeSeletedPuzzles, (vgui_key)'R', vgui_CTRL );
        solving.separator();
        solving.add( "Display Current &Puzzle Completions", displayAllPuzzleSolutions, (vgui_key)'P', vgui_CTRL );
    menu.add( "&Puzzle Completion", solving );      

    vgui_menu help;
        help.add( "&Contents", help_contents, (vgui_key)'C', vgui_CTRL );
        help.separator();
        help.add( "&About", about, (vgui_key)'A', vgui_CTRL );
    menu.add( "&Help", help );

}

void bfrag_Menu::execLiveWire()
{
  // MING: need a standard cross-platform solution here!!!
#if 0
    
    STARTUPINFO          si = { sizeof(si) };
    PROCESS_INFORMATION  pi;
    char                 szExe[] = "ContourTracing.exe";

    if(CreateProcess(0, szExe, 0, 0, FALSE, 0, 0, 0, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        vgui_dialog begin("ERROR");
        begin.message( "Failed to load Contour Tracing Application");
        begin.ask();
    }
#endif
}

void bfrag_Menu::pairmatchOptions(){
}

void bfrag_Menu::solvingOptions(){
}

void bfrag_Menu::generatePuzzle(){
    bfrag_Manager::instance()->generatePuzzle();
}

void bfrag_Menu::removeSeletedPuzzles(){
    bfrag_Manager::instance()->removeSelectedPuzzles();
}

void bfrag_Menu::removeSelectedFragment(){
    bfrag_Manager::instance()->removeSelectedFragments();
}

void bfrag_Menu::addPuzzle(){
    vgui_dialog params("Add Puzzle...");
    //params.set_modal(true);
    vcl_string ext = "*.pz2";
    vcl_string fileToOpen;
    params.file("Puzzle File To Add (*.pz2)", ext, fileToOpen);
    
    if (!params.ask())
        return;

    //bring up this open box until they open it or cancel
    while (bfrag_Manager::instance()->addPuzzle( fileToOpen ) == -1){

        //if i just keep asking and not making new boxes
        //vxl crashes.  who knows...
        vgui_dialog params("Add Puzzle...");
        //params.set_modal(true);
        vcl_string ext = "*.pz2";
        params.file("Puzzle File To Add (*.pz2)", ext, fileToOpen);
    
        if (!params.ask())
            return;
        
    };
}

void bfrag_Menu::displayAllContours(){
    bfrag_Manager::instance()->displayAllContours();
}

void bfrag_Menu::displayNContours(){
        vgui_dialog which_dl("Display N Contours");
    int chosen, whereToStart = 1;
    vcl_vector<vcl_string> numPairs;
    numPairs.push_back("1");
    numPairs.push_back("4");
    numPairs.push_back("9");
    numPairs.push_back("16");
    numPairs.push_back("25");
    numPairs.push_back("36");
    numPairs.push_back("49");
    which_dl.choice("Number of Contours To Display: ", numPairs, chosen );
    which_dl.field("Starting with Contour: ", whereToStart );
    which_dl.message("NOTE: first contour is 1");
    
    if( which_dl.ask() ){
        switch(chosen){
        case 0:
            chosen = 1;
            break;
        case 1:
            chosen = 4;
            break;
        case 2:
            chosen = 9;
            break;
        case 3:
            chosen = 16;
            break;
        case 4:
            chosen = 25;
            break;
        case 5:
            chosen = 36;
            break;
        case 6:
            chosen = 49;
            break;
        }

        bfrag_Manager::instance()->displayNContours(chosen, whereToStart);
    }
}

void bfrag_Menu::completeMatching(){
    bfrag_Manager::instance()->completeMatching();
}

void bfrag_Menu::displayAllPuzzleSolutions(){
    bfrag_Manager::instance()->displayAllPuzzleSolutions();
}

void bfrag_Menu::matchingIteration(){
    bfrag_Manager::instance()->matchingIteration();
}

void bfrag_Menu::displayNPairs(){
    vgui_dialog which_dl("Display N Pairs");
    int chosen, whereToStart = 1;
    vcl_vector<vcl_string> numPairs;
    numPairs.push_back("1");
    numPairs.push_back("4");
    numPairs.push_back("9");
    numPairs.push_back("16");
    numPairs.push_back("25");
    numPairs.push_back("36");
    numPairs.push_back("49");
    which_dl.choice("Number of Pairs To Display: ", numPairs, chosen );
    which_dl.field("Starting with Pair: ", whereToStart );
    which_dl.message("NOTE: first pair is 1");
    
    if( which_dl.ask() ){
        switch(chosen){
        case 0:
            chosen = 1;
            break;
        case 1:
            chosen = 4;
            break;
        case 2:
            chosen = 9;
            break;
        case 3:
            chosen = 16;
            break;
        case 4:
            chosen = 25;
            break;
        case 5:
            chosen = 36;
            break;
        case 6:
            chosen = 49;
            break;
        }

        bfrag_Manager::instance()->displayNPairs(chosen, whereToStart);
    }
}

void bfrag_Menu::resetPairs(){
    bfrag_Manager::instance()->resetPairs();
}

void bfrag_Menu::removeSelectedPairs(){
    bfrag_Manager::instance()->removeSelectedPairs();
}

void bfrag_Menu::displayCurrentPairs(){
    bfrag_Manager::instance()->displayCurrentPairs();
}


void bfrag_Menu::displayNextPair(){
    assert(0);
    //bfrag_Manager::instance()->displayNextPair();
}

void bfrag_Menu::pairMatch(){
    bfrag_Manager::instance()->pairMatch();    
}

void bfrag_Menu::help_contents(){
    
    vgui_dialog about_dl("HindSite Help System");

    about_dl.message( "See the \"HindSite_Manual.doc\" file included with this program" );
    
    about_dl.set_cancel_button(0);
    about_dl.ask();
   
}

void bfrag_Menu::about(){

    vgui_dialog about_dl("About HindSite 1.0 -- 2004");

    about_dl.message( "HindSiste - The Fragment Assember version 1.0\n(C) 2004 LEMS @ Brown University" );
    about_dl.message( "Based on masters work by Jonah McBride");
    about_dl.message( "Author: Daniel Spinosa\nEmail: spinosa@gmail.com" );

    about_dl.set_cancel_button(0);
    about_dl.ask();
}

void bfrag_Menu::topImagesShow(){
    //vcl_cout << "top Images Show all" << vcl_endl;
    bfrag_Manager::instance()->displayAllTopImages(true);
}
void bfrag_Menu::topImagesHide(){
    //vcl_cout << "top Images Hide all" << vcl_endl;
    bfrag_Manager::instance()->displayAllTopImages(false);
}
void bfrag_Menu::botImagesShow(){
    //vcl_cout << "bot Images Show all" << vcl_endl;
    bfrag_Manager::instance()->displayAllBotImages(true);
}
void bfrag_Menu::botImagesHide(){
    //vcl_cout << "bot Images Hide all" << vcl_endl;
    bfrag_Manager::instance()->displayAllBotImages(false);
}
void bfrag_Menu::botCurvesShow(){
    //vcl_cout << "bot Curevs Show all" << vcl_endl;
    bfrag_Manager::instance()->displayAllBotCurves(true);
}
void bfrag_Menu::botCurvesHide(){
    //vcl_cout << "bot Curevs Hide all" << vcl_endl;
    bfrag_Manager::instance()->displayAllBotCurves(false);
}

void bfrag_Menu::savePZ2(){
    bfrag_Manager::instance()->savePZ2();
}

void bfrag_Menu::savePZ2as(){

    vcl_string saveFn = bfrag_Manager::instance()->getLoadedName();

    vgui_dialog params("Save As...");
    //params.set_modal(true);
    static vcl_string ext("*.pz2");
    params.file("PZ2 file name for saving (*.pz2) . . .", ext, saveFn);

    //on cancel leave
    if( !params.ask()) return;

    //otherwise make sure they specified a name and do the saving
    while(bfrag_Manager::instance()->savePZ2as( saveFn ) == -1){

        vgui_dialog params("Save...");
        //params.set_modal(true);
        params.file("PZ2 file name for saving (*.pz2) . . .", ext, saveFn);


        if( !params.ask()) return;
    }



}

void bfrag_Menu::addbfrag2D(){

    vcl_string topConFn, botConFn, topImageFn, botImageFn, name = "newPiece";

    vgui_dialog params("Open...");
    //params.set_modal(true);
    params.field( "Piece Name: ", name );
    params.message( "You must select at least a top curve.");

    static vcl_string ext("*.con, *.cem");
    static vcl_string no_ext;
    params.file("Top Contour (*.con, *.cem) . . .", ext, topConFn);
    params.file("Top Image . . .", no_ext, topImageFn);
    params.file("Bottom Contour (*.con, *.cem) . . ", ext, botConFn);
    params.file("Bottom Image . . .", no_ext, botImageFn);
    

    //on cancel just leave
    if (!params.ask()) return;

    //otherwise process this stuff...
    while (bfrag_Manager::instance()->addbfrag2D( topConFn, botConFn, topImageFn, botImageFn, name ) == -1){
    
        vgui_dialog params("Open...");
        //params.set_modal(true);
        params.message( "You must select at least a top curve.");
        params.file("Top Contour To Add (*.con, *.cem)", ext, topConFn);
        params.file("Top Image To Add . . .", no_ext, topImageFn);
        params.file("Bottom Contour To Add (*.con, *.cem)", ext, botConFn);
        params.file("Bottom Image To Add . . .", no_ext, botImageFn);
        

        //on cancel just leave
        if (!params.ask()) return;

    }
    


}


void bfrag_Menu::displayContour(){

    bfrag_Manager::instance()->displayContour();

}

void bfrag_Menu::displayAssembly(){

    bfrag_Manager::instance()->displayAssembly();

}


void bfrag_Menu::load(){

    vgui_dialog params("Open...");
    //params.set_modal(true);
    vcl_string ext = "*.pz2";
    vcl_string fileToOpen;
    params.file("Puzzle File To Load (*.pz2)", ext, fileToOpen);//inbuilt file browser condition. inbuilt in any dialogue box.
    if (!params.ask())
        return;
    //bring up this open box until they open it or cancel
    while (bfrag_Manager::instance()->loadPuzzle( fileToOpen ) == -1){

        //if i just keep asking and not making new boxes
        //vxl crashes.  who knows...
        vgui_dialog params("Open2...");
        //params.set_modal(true);
        vcl_string ext = "*.pz2";
        params.file("Puzzle File To Load1 (*.pz2)", ext, fileToOpen);
    
        if (!params.ask())
            return;
        
    };


}

void
bfrag_Menu::quit()
{
    bfrag_Manager::instance()->quit();
}
