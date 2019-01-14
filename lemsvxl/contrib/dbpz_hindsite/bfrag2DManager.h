/**************************************************************
 *    Name: Daniel Spinosa
 *    File: bfrag2DManager.h
 *    Asgn: 
 *    Date: 
 *
 *      Class handles higher level loading (of .pz2 files) and
 *  creation and managing of bfrag2D files.  See bfrag2D for 
 *  loading and storage of .con and .cem files as well as the 
 *  fragment associated image.
 *
 *  Also manages bfrag2D_assmembly's
 *
 ***************************************************************/

#ifndef __BFRAG2DMANAGER_H__
#define __BFRAG2DMANAGER_H__



#include <vector>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>

#include "bfrag2D.h"
#include "vgui_bfrag2D_tableau.h"
#include "Curve.h"
#include "xform.h"


class bfrag2DManager {

public:
    bfrag2DManager();
    virtual ~bfrag2DManager();

    int loadPuzzle( std::string );
    void generatePuzzle( std::string fcon, std::string bcon, std::string fimage, std::string bimage );
    int addPuzzle( std::string );
    int addPiece( std::string topConFn, std::string botConFn, 
                  std::string topImageFn, std::string botImageFn, std::string name );
    int savePZ2as( std::string fn );
    int removeBfrag( int where );
    void addToNote( std::string s ){ _noteStrings.push_back(std::string(s)); };

    void clear_display(){ thebfrag2DTableau->clear(); theViews.clear(); redrawTableaux(); };

    void setTableau( vgui_bfrag2D_tableau_sptr tab ){ thebfrag2DTableau = tab; };
    void setShell( vgui_shell_tableau_sptr tab ){ theShell = tab; };
    void setGrid( vgui_grid_tableau_sptr tab ){ theGrid = tab; };

    void newGrid( int w, int h, int numPairs, int numPuzzles = -1 );

    //returns which grid spots are selected
    int getSelected( std::vector<int> &cols, std::vector<int> &rows );
    
    void addCurveAt( Curve<double,double> *theCurve, int gridIndex, const vgui_style_sptr& style = NULL );
    void addPointAt( float x, float y, int gridIndex, const vgui_style_sptr& style = NULL );

    //void display(int which); //depreated
    void displayAssembly( std::vector< bool > whichFrags );
    void displayBfragAt( int which, int where, XForm3x3 *theXform = &XForm3x3() );
    void post_redraw(){ thebfrag2DTableau->post_redraw(); };
    void getBfragNameList( std::vector<std::string> &outList );

    void displayCurve( Curve<double,double> *theCurve );

    void displayAllTopImages( bool show );
    void displayAllBotImages( bool show );
    void displayAllBotCurves( bool show );

    void redrawTableaux();

    //return a std::vector<Curve<double,double> > of all the top curves for use in puzzleSolving
    std::vector<Curve<double,double> > getTopContoursForPuzzleSolving();

private:

    vgui_bfrag2D_tableau_sptr thebfrag2DTableau;
    vgui_shell_tableau_sptr theShell;
    vgui_grid_tableau_sptr theGrid;

    //extracts contents within " " or the next token if " is not found
    //return -1 upon unmatched " ", 0 on otherwise
    int extractFileName( std::ifstream &input, char* outFn );

    //to delete all the new'ed bfrags in theFrags vector
    void clearTheFrags();
    void clearAssemblies();
    void clearTheViews();
    int parseFail( std::string errmsg );

    //these all get deleted
    std::vector < bfrag2D* > theFrags;
    std::vector < vgui_soview2D_bfrag2D* > theViews;
    std::vector < bfrag2D_assembly* > theAssemblies;

    std::vector < vgui_bfrag2D_tableau_sptr > gridBfrag2DTableaux;
    std::vector< vgui_viewer2D_tableau_sptr > viewerTableaux;

    int numPieces;

    std::vector< std::string > _noteStrings;


};






#endif
