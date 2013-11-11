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
#include <vgui/vgui_text_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_adaptor.h>


#include "bfrag2D.h"
#include "vgui_bfrag2D_tableau.h"
//#include "Curve.h"
#include "bfrag_curve.h"
#include "xform.h"


class bfrag2DManager {

public:
    bfrag2DManager();
    virtual ~bfrag2DManager();

    int loadPuzzle( vcl_string );
    void generatePuzzle( vcl_string fcon, vcl_string bcon, vcl_string fimage, vcl_string bimage );
    int addPuzzle( vcl_string );
    int addPiece( vcl_string topConFn, vcl_string botConFn, 
                  vcl_string topImageFn, vcl_string botImageFn, vcl_string name );
    int savePZ2as( vcl_string fn );
    int removeBfrag( int where );
    void addToNote( vcl_string s ){ _noteStrings.push_back(vcl_string(s)); };

    void clear_display(){ thebfrag2DTableau->clear(); theViews.clear(); redrawTableaux(); };

    void setTableau( vgui_bfrag2D_tableau_sptr tab ){ thebfrag2DTableau = tab; };
    void setShell( vgui_shell_tableau_sptr tab ){ theShell = tab; };
    void setGrid( vgui_grid_tableau_sptr tab ){ theGrid = tab; };

    void newGrid( int w, int h, int numPairs, int numPuzzles = -1 );

    //returns which grid spots are selected
    int getSelected( vcl_vector<int> &cols, vcl_vector<int> &rows );
    
    void addCurveAt(bfrag_curve *theCurve, int gridIndex, int offset_x=0, int offset_y=0, const vgui_style_sptr& style=NULL);
    void addPointAt(float x, float y, int gridIndex, const vgui_style_sptr& style = NULL );

    //void display(int which); //depreated
    void displayAssembly( vcl_vector< bool > whichFrags );
    void displayBfragAt( int which, int where, XForm3x3 *theXform = &XForm3x3() );
    void post_redraw(){ thebfrag2DTableau->post_redraw(); };
    void getBfragNameList( vcl_vector<vcl_string> &outList );

    void displayCurve( bfrag_curve *theCurve );

    void displayAllTopImages( bool show );
    void displayAllBotImages( bool show );
    void displayAllBotCurves( bool show );

    void redrawTableaux();

    void show_cost(double cost, int index);
    void center_image(vgl_box_2d<double> &box, int index);
    void show_image(vil_image_view<vxl_byte> &image, int index);

    //return a vcl_vector<Curve<double,double> > of all the top curves for use in puzzleSolving
    vcl_vector<bfrag_curve> getTopContoursForPuzzleSolving();

//private:

    vgui_bfrag2D_tableau_sptr thebfrag2DTableau;
    vgui_shell_tableau_sptr theShell;
    vgui_grid_tableau_sptr theGrid;

    //extracts contents within " " or the next token if " is not found
    //return -1 upon unmatched " ", 0 on otherwise
    int extractFileName( vcl_ifstream &input, char* outFn );

    //to delete all the new'ed bfrags in theFrags vector
    void clearTheFrags();
    void clearAssemblies();
    void clearTheViews();
    int parseFail( vcl_string errmsg );

    //these all get deleted
    vcl_vector < bfrag2D* > theFrags;
    vcl_vector < vgui_soview2D_bfrag2D* > theViews;
    vcl_vector < bfrag2D_assembly* > theAssemblies;

    vcl_vector < vgui_bfrag2D_tableau_sptr > gridBfrag2DTableaux;
    vcl_vector< vgui_viewer2D_tableau_sptr > viewerTableaux;

    int numPieces;

    vcl_vector< vcl_string > _noteStrings;


};






#endif
