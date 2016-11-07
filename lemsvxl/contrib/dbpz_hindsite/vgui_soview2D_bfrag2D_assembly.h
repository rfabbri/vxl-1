/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_soview2D_bfrag2D_assembly.h        : public vgui_soview2D
 *    Asgn: 
 *    Date: 
 *
 *        This class represents a view of one assembly of bfrags.  It contains
 *    a vector of pointers to the appropriate soview's and maintains a view
 *    matrix for each soview so as to enable correct draw position of view.
 *
 *    It will do these things.
 *
 ***************************************************************************/

#ifndef __VGUI_SOVIEW_BFRAG2D_ASSEMBLY_H__
#define __VGUI_SOVIEW_BFRAG2D_ASSEMBLY_H__

#include "vgui_soview2D_bfrag2D.h"

//#include <bgui/bgui_vsol_soview2D.h>
#include "bgui_vsol_soview2D_new.h"

class vgui_soview2D_bfrag2D_assembly : public vgui_soview2D {

public:
    
    vgui_soview2D_bfrag2D_assembly(){};
    vgui_soview2D_bfrag2D_assembly( vgui_soview2D_bfrag2D* in, double *inMatrix );
    ~vgui_soview2D_bfrag2D_assembly(){};

    virtual void draw() const;
    virtual void draw_select() const;

    virtual float distance_squared(float x, float y) const{ return 0; };
    virtual void get_centroid(float* x, float* y) const{};
    virtual void translate(float x, float y){};


    void addPositionedPiece( vgui_soview2D_bfrag2D* in,
                             double *inMatrix );

    vcl_vector< vgui_soview2D_bfrag2D* > getViews(){ return theViews; };
    vcl_vector< double* > getMatrices(){ return theMatrices; };

private:

    vcl_vector< vgui_soview2D_bfrag2D* > theViews;
    //also gunna need a parallel vector for the matricies!
    vcl_vector< double* > theMatrices;

};

#endif
