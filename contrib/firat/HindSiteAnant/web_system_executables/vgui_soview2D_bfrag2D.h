/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_soview2D_bfrag2D.h        : public vgui_soview2D
 *    Asgn: 
 *    Date: 
 *
 *        This class maintaings the functionality of drawing a bfrag2D
 *    in any form (with-or-w/o: top image, bot image, top curve, bot curve)
 *
 *    also include "clicked within this bfrag2D" functionality?
 *
 *
 ***************************************************************************/

#ifndef __VGUI_SOVIEW_BFRAG2D_H__
#define __VGUI_SOVIEW_BFRAG2D_H__

#include <vgui/vgui_soview2D.h>

#include "bfrag2D.h"

//#include <bgui/bgui_vsol_soview2D.h>
#include "bgui_vsol_soview2D_new.h"


class vgui_soview2D_bfrag2D : public vgui_soview2D {


public:

    vgui_soview2D_bfrag2D(){ topCurve = 0;
                             botCurve = 0; };
    vgui_soview2D_bfrag2D( bgui_vsol_soview2D_new_polygon * tc, 
                           bgui_vsol_soview2D_new_polygon * bc,
                           vgui_soview2D_image * ti,
                           vgui_soview2D_image * bi,
                           bfrag2D* f);
    ~vgui_soview2D_bfrag2D(){};

    virtual void draw() const;
    virtual void draw_select() const;

    virtual float distance_squared(float x, float y) const;
    virtual void get_centroid(float* x, float* y) const;
    virtual void translate(float x, float y);
    
    void popupInfo();

    //i guess these should be private w/ accessor stuff, but really, who needs that
    bool drawTopCurve;
    bool drawTopImage;
    bool drawBotCurve;
    bool drawBotImage;

private:


    bfrag2D *theFrag;

    bgui_vsol_soview2D_new_polygon *topCurve, *botCurve;
    vgui_soview2D_image *topImage, *botImage;
    
};


#endif
