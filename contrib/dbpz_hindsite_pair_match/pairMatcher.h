#ifndef __PAIR_MATCHER_H__
#define __PAIR_MATCHER_H__

#include <vgui/vgui_easy2D_tableau.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <vgui/vgui_dialog.h>

#include <vcl_iostream.h>

#include "my_bsol_intrinsic_curve_2d.h"

#include "vsol/vsol_polygon_2d.h"
#include "vsol/vsol_polygon_2d_sptr.h"

#include "curve_dpmatch_2d.h"

#ifndef min
#define min(a,b) (a<b?a:b)
#endif
#ifndef max
#define max(a,b) (a>b?a:b)
#endif

class pairMatcher {
public:
    pairMatcher( bgui_vsol2D_tableau_sptr invsol2D );
    virtual ~pairMatcher(){};

    void loadCurves(const char*, const char*, int skipRate = 1);
    void resampleCurves( int skipRate );
    
    //the start point chosen is found on normal direction curve,
    //the curve is then reverse around the start point
    int runMatch(int startA = 0, int endA = -1, bool reverseA = false, int startB = 0, int endB = -1, bool reverseB = false, int matchStartA = 1 );
    void reverseA(){ curveA->reverse(); };
    void reverseB(){ curveB->reverse(); };

    //drawing
    void drawCurves();
    void showCurvePoint( int curveNum, int pointNum );
    void drawMatchLines();
    void showCorners();
    

private:
    bgui_vsol2D_tableau_sptr thevsol2D;

    my_bsol_intrinsic_curve_2d *curveA, *curveB;
    vsol_polygon_2d_sptr curveApoly, curveBpoly;

    int loadCon( vcl_string fn, my_bsol_intrinsic_curve_2d* &storage, vsol_polygon_2d_sptr &storagePoly, int start = 0, int end = -1, bool reverse = false ); //end = -1 means all the way
    void fatalError( vcl_string msg );

    curve_dpmatch_2d *curveMatcher;

};



#endif //__PAIR_MATCHER_H__
