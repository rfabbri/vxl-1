/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_bfrag2D_tableau     : public vgui_easy2D_tableau
 *    Asgn: 
 *    Date: 
 *
 *        A simple subclass of the easy2D tableau used to display the
 *    vgui_soview2D_bfrag2D views of bfrag2Ds and to interact with them.
 *    Overrode functions necessary to acheive
 *
 *
 ***************************************************************************/
#ifndef _VGUI_BFRAG2D_TABLEAU_H__
#define _VGUI_BFRAG2D_TABLEAU_H__

#include <vgui/vgui_dialog.h>
#include <vgui/vgui_easy2D_tableau.h>
#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_tableau_sptr.h>

#include <bgui/bgui_vsol2D_tableau.h>

#include "vgui_soview2D_bfrag2D.h"
#include "vgui_soview2D_bfrag2D_assembly.h"
#include "bfrag2D_assembly.h"
#include "bfrag2D.h"
#include "Curve.h"

//for translation
#include "xform.h"

//*
//#include <vxl_config.h> // for vxl_byte
//#include <vil/vil_rotate.h>
//*
//#include "vil/vil_rotate.h"
//#include "vil/vil_resample_bilin.h"


#include "vgui_bfrag2D_tableau_sptr.h"

class vgui_bfrag2D_tableau : public bgui_vsol2D_tableau {
public:
    
    //TEST
    /*
    virtual bool mouse_down(int x,int y, vgui_button button, vgui_modifier modifier ){
        vcl_cout << myIndex << "mouse down at: (" << x << "," << y << ")" << vcl_endl;
        return true;
    }
    */
    //TEST

    vgui_bfrag2D_tableau(const char* n="unnamed");

    vgui_bfrag2D_tableau(vgui_image_tableau_sptr const& it,
                         const char* n="unnamed");

    vgui_bfrag2D_tableau(vgui_tableau_sptr const& t,
                         const char* n="unnamed");

    ~vgui_bfrag2D_tableau();

    void generalInit();
    void setIndex( int i ){ myIndex = i; };
    void setNumPairs( int n ){ numPairs = n; };
    void setNumPuzzles( int n ){ numPuzzles = n; };

    virtual vcl_string type_name() const;


    vgui_soview2D_bfrag2D *addbfrag2D( bfrag2D *theFrag, XForm3x3 *theXform = &XForm3x3() );
    //set views to 0 if you don't wish to keep reference to all created views
    vgui_soview2D_bfrag2D_assembly *addbfrag2D_assembly( bfrag2D_assembly *theAssembly, vcl_vector<vgui_soview2D_bfrag2D*> *views );

    //overrode this (prevously of vgui_easy2D_tab to force alpha blending ON
    vgui_soview2D_image *add_image( float x, float y, vil_image_view_base const& img );

    void addCurve( Curve<double,double>* theCurve, const vgui_style_sptr& style = NULL );

    //overrode to handle clicking events
    virtual bool handle( const vgui_event & e );

    //overrode to use MY polygon w/ correct draw_select() stuff
    bgui_vsol_soview2D_new_polygon *add_new_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline, const vgui_style_sptr& style = NULL);

    
    void add_popup(vgui_menu& menu);
    //PAIR
    void reorderPair( int toWhere );
    void deleteMePair();
    void deleteSelectedPairs();
    //FRAG
    void deleteMeFrag();
    void deleteSelectedFrags();
    //PUZZLE
    void reorderPuzzle( int toWhere );
    void deleteMePuzzle();
    void deleteSelectedPuzzles();

private:

    //b/c dialog boxes are not always modal, so we'll invent our own little hack...
    bool modal;

    int myIndex, numPairs, numPuzzles;
    

};


//this stuff is needed to establish inheritance between tableau  smart pointers
//cloned from bgui_vsol2D_tableau
struct vgui_bfrag2D_tableau_new : public vgui_bfrag2D_tableau_sptr
{
  typedef vgui_bfrag2D_tableau_sptr base;

  vgui_bfrag2D_tableau_new(const char* n="unnamed") :
    base(new vgui_bfrag2D_tableau(n)) { }

  vgui_bfrag2D_tableau_new(vgui_image_tableau_sptr const& it,
                                 const char* n="unnamed") :
    base(new vgui_bfrag2D_tableau(it,n)) { }

  vgui_bfrag2D_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed") :
    base(new vgui_bfrag2D_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};



#endif


