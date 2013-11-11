#include "pairMatcher.h"

pairMatcher::pairMatcher( bgui_vsol2D_tableau_sptr invsol2D ){
    vcl_cout << "pair Matcher Init" << vcl_endl;

    thevsol2D = invsol2D;

    curveMatcher = new curve_dpmatch_2d();
}


void pairMatcher::loadCurves( const char *fn1, const char *fn2, int skipRate ){

    //load up 2 curves
    curveA = new my_bsol_intrinsic_curve_2d();
    curveB = new my_bsol_intrinsic_curve_2d();
    curveApoly = new vsol_polygon_2d();
    curveBpoly = new vsol_polygon_2d();
    loadCon( fn1, curveA, curveApoly );
    loadCon( fn2, curveB, curveBpoly);
    
    if( skipRate > 1 ){
        resampleCurves( skipRate );
    }

    curveA->computeProperties();
    curveB->computeProperties();

}

void pairMatcher::resampleCurves( int skipRate ){

    curveA->coarseResample( skipRate );
    curveB->coarseResample( skipRate );

    curveA->computeProperties();
    curveB->computeProperties();

    drawCurves();

}

void pairMatcher::drawCurves(){

    thevsol2D->clear();

    thevsol2D->add_vsol_polygon_2d( new vsol_polygon_2d(*(curveA->getStorage())) );
    thevsol2D->add_vsol_polygon_2d( new vsol_polygon_2d(*(curveB->getStorage())) );

}


int pairMatcher::runMatch(int startA, int endA, bool reverseA, int startB, int endB, bool reverseB, int matchStartA ){
    vcl_cout << "pair Matcher Begin Running" << vcl_endl;

    my_bsol_intrinsic_curve_2d *Aorig, *Borig;
    Aorig = new my_bsol_intrinsic_curve_2d( *curveA );
    Borig = new my_bsol_intrinsic_curve_2d( *curveB );

    //set the curves (w/ whatver hackes are needed)
    if( startA )curveA->changeStart( startA );
    if( reverseA ) curveA->reverse();
    if( startB ) curveB->changeStart( startB );
    if( reverseB ) curveB->reverse();

    //they are intrinsic -> dont do this and your results are F'd up
    curveA->computeProperties();
    curveB->computeProperties();

    curveMatcher->setCurve1( curveA );
    curveMatcher->setCurve2( curveB );

    int sizeA = curveA->getStorage()->size();
    int sizeB = curveB->getStorage()->size();

    //end points
    if( endA == -1 || endA > sizeA ){
        endA = sizeA;
    }
    if( endB == -1 || endB > sizeB ){
        endB = sizeB;
    }

    //force end points to be at or before next corner
    vcl_vector<vsol_point_2d> curveAcornerPoints = curveA->findCorners( 20.0f, 0.5f, 0.5236f );
    vcl_vector<vsol_point_2d> curveBcornerPoints = curveB->findCorners( 20.0f, 0.5f, 0.5236f );
    vcl_vector<int> *curveAcornerIndices = &(curveA->corners_);
    vcl_vector<int> *curveBcornerIndices = &(curveB->corners_);
    //do some forcing!



    //match them
    int delta = 15, i = 0, Lbarbest;
    double leastCost;
    vcl_cout << "L  Lbar-cost NormCost" << vcl_endl;
    for( int L = matchStartA; L < endA ; L++ ){
        leastCost = 999999999999999;
        //vcl_cout << "mathching at L: " << L << vcl_endl;
        for( int Lbar = L - delta; Lbar < L + delta; Lbar++ ){
            if( Lbar > 0 && Lbar < endB ){
                curveMatcher->Match( 0, L, 0, Lbar );
                vcl_cout << Lbar << " " << curveMatcher->finalCost() << vcl_endl;
                if( curveMatcher->finalCost() < leastCost ){
                    leastCost = curveMatcher->finalCost();
                    Lbarbest = Lbar;
                }
            }
        }
        vcl_cout << L << "\t" << Lbarbest << "\t" << leastCost << "\t" << leastCost/min(L,Lbarbest) << vcl_endl;
    }
    
    //vcl_cout << "DP Table for most recent run" << vcl_endl;
    //curveMatcher->ListDPTable(endA, endB);


    //curveA = Aorig;
    //curveB = Borig;
    //delete curveMatcher;
    //curveMatcher = new curve_dpmatch_2d();
    //curveMatcher->setCurve1( curveA );
    //curveMatcher->setCurve2( curveB );

    return 0;
}

void pairMatcher::showCorners(){
    vcl_cout << "drawing corners" << vcl_endl;

    curveA->computeProperties();
    curveB->computeProperties();

    vcl_vector< vsol_point_2d > cornersA = curveA->findCorners( 30, 2.0f, float(3.14*30)/180.0);
    vcl_vector< vsol_point_2d > cornersB = curveB->findCorners( 30, 2.0f, float(3.14*30)/180.0);
    
    for( int i = 0; i < cornersA.size(); i++ ){
        thevsol2D->add_circle( cornersA[i].x(), cornersA[i].y(), 5 );
    }

    for( int i = 0; i < cornersB.size(); i++ ){
        thevsol2D->add_circle( cornersB[i].x(), cornersB[i].y(), 5 );
    }

    thevsol2D->post_redraw();


}


void pairMatcher::drawMatchLines(){

    vcl_cout << "drawing match lines (size: " << (curveMatcher->finalMap())->size() << vcl_endl;

    vcl_vector< vcl_pair<int,int> > *matchPairs = curveMatcher->finalMap();

    for( int i = 0; i < matchPairs->size(); i++ ){
        thevsol2D->add_line( curveA->x(((*matchPairs)[i]).first), curveA->y(((*matchPairs)[i]).first), 
                             curveB->x(((*matchPairs)[i]).second), curveB->y(((*matchPairs)[i]).second));
        vcl_cout << "adding line (" << curveA->x(((*matchPairs)[i]).first) << "," << curveA->y(((*matchPairs)[i]).first) << ") to (" << curveB->x(((*matchPairs)[i]).second) << "," << curveB->y(((*matchPairs)[i]).second) << ")" << vcl_endl;
    }

    thevsol2D->post_redraw();
}


void pairMatcher::fatalError( vcl_string msg ){
    vcl_cout << "FATAL: " << msg << vcl_endl << "-QUITING-" << vcl_endl;
    int x;
    vcl_cin >> x;
    exit(-1);
}


int pairMatcher::loadCon( vcl_string fn, my_bsol_intrinsic_curve_2d* &storage, vsol_polygon_2d_sptr &storagePoly, int start, int end, bool reverse ){
    vcl_cout << "Loading: " << fn << vcl_endl;
    
    double x, y;
    int nPoints;
    char buffer[2048];
    vcl_vector< vsol_point_2d_sptr > points, newpoints, temppoints;

    //1)If file open fails, return.
    vcl_ifstream fp(fn.c_str(), vcl_ios::in);
    if (!fp) {
      vcl_cout<<" : Unable to Open "<< fn << vcl_endl;
      return -1;
    }

    //2)Read in file header.
    fp.getline(buffer,2000); //CONTOUR
    char openFlag[200];
    fp.getline(openFlag,200); //OPEN/CLOSE


    if (!strncmp(openFlag,"OPEN",4)){
        vcl_cout << "WARNING: CURVE IS MARKED __OPEN__ " << vcl_endl;
    } else if (!strncmp(openFlag,"CLOSE",5)){
        //perfect
    }else{
        vcl_cout << "Invalid File " << fn << vcl_endl;
        vcl_cout << "Should be OPEN/CLOSE: " << openFlag << vcl_endl;
        return -1;
    }
  

    fp >> nPoints;
    //vcl_cout << "Number of Points from Contour:" << nPoints << vcl_endl;

    int i;
    for ( i=0; i < nPoints; i++) {
        fp >> x >> y;
        vsol_point_2d_sptr newPt = new vsol_point_2d (x,y);
        points.push_back(newPt);
    }
    int size = i;
    //vcl_cout << "size should be: " << i << vcl_endl;

    if( reverse ){
        for( int i = size-1; i >= 0; i-- ){
            temppoints.push_back( points[i] );
        }
        points.clear();
        for( int i = 0; i < size; i++ ){
            points.push_back( temppoints[i] );
        }
    }


    //reorder points
    if( end == -1 ) end = size-1;
    for( int i = start; i != end; i++){
        if( i == size ) i = 0;
        newpoints.push_back( points[i] );
    }
        

    //put it into storage = bsol_intrinsic_curve_2d_sptr
    storage = new my_bsol_intrinsic_curve_2d(newpoints);
    storage->computeProperties();
    vcl_cout << "Size of my_intrinsic: " << storage->size() << vcl_endl;

    //can also use vsol_polygon_2d in same fashion...
    storagePoly = new vsol_polygon_2d(newpoints);

    fp.close();


    return 0; // on success

}

void pairMatcher::showCurvePoint( int curveNum, int pointNum ){
    vcl_vector<vsol_point_2d_sptr> *theStorage;

    if( curveNum == 1 ) theStorage = curveA->getStorage();
        else theStorage = curveB->getStorage();

    if( pointNum < theStorage->size() ){
        thevsol2D->add_circle( (*theStorage)[pointNum]->x(), (*theStorage)[pointNum]->y(), 10 );
        thevsol2D->post_redraw();
    }

}

