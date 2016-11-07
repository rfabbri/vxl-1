#ifndef PUZZLESOLVING_H
#define PUZZLESOLVING_H

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>

#include "Curve.h"
#include "Point.h"
#include "DPMatch.h"
#include "FineDPMatch.h"
#include "xform.h"
#include "utils.h"

#include "global.h"

typedef vcl_vector<vcl_pair<int,int> > intMap;
typedef vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map_with_cost;
typedef vcl_vector<vcl_pair<double, int> > indexedMeasures; 


class map_cost_less {
 public:
  bool operator() (vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map1, 
       vcl_pair<double,vcl_vector<vcl_pair<int,int> > > map2) { 
      return map1.first<map2.first; 
  }
};

class cost_ind_less {
 public:
  bool operator() (vcl_pair<double,int> c1, vcl_pair<double,int> c2){ 
      return c1.first<c2.first; 
  }
};
class cost_ind_more {
 public:
  bool operator() (vcl_pair<double,int> c1, vcl_pair<double,int> c2){ 
      return c1.first>c2.first; 
  }
};


//essentially a struct, but i need to define operator(s) on it
class pairwiseMatch{
public:
    // DATA
    double               cost;         //old DOULBE from cost vector (the actual cost)
    int                  myIndex;      //old  INT   from cost vector (which numMatch am i)
    intMap               pointMap;     //old _maps
    XForm3x3             xForm;        //old _xForms
    vcl_pair<int,int>    whichCurves;  //old _pairs
    // DATA

    pairwiseMatch(){};
    void print(){ vcl_cout << "match [" << myIndex << "]->cost: [" << cost << "]   curves(" << whichCurves.first << "," << whichCurves.second << ") " << vcl_endl; /*xForm->print();*/ };

    //i use this operator explicity in sort b/c otherwise it seems sort will compare
    //the memory location of the pointers, and not use the pointers' operators
    bool operator() (pairwiseMatch *a, pairwiseMatch *b){
        return a->cost < b->cost;
    }
};


class pairwiseMatchSort {
public:
    bool operator() (pairwiseMatch a, pairwiseMatch b){
        return a.cost < b.cost;
    }
};

class searchState {

public:  
    vcl_vector<Curve<double,double> > _cList;

    //pointer style
    //vcl_vector< pairwiseMatch* > _matches;
    //non pointer style
    vcl_vector< pairwiseMatch > _matches;

    //------------------MATCH-----------------------
    //map indexes     vector<pair<int,int>>   -- says which point maps to which point
    //  vector  -- corresponds to _pairs vector [i.e. this[0] and _pairs[0]]
    //vcl_vector<vcl_vector<vcl_pair<int,int> > > _maps;
    //  vector -- transform corresponds to the pairs, convention: either smaller gets transform or the second one is Xformed or ...
    //vcl_vector<XForm3x3> _xForms;
    //vcl_vector<vcl_pair<int,int> > _pairs;
    //------------------MATCH-----------------------


    int _numMatch;
    int _num_new;
    Curve<double,double> _merged;
    vcl_vector<Curve<double,double> > _constr;

    vcl_string *_filename; //Added to store filename

public:

    /* //Pointer style
    void sanityCheckMatches();
    void sanityCheckMatchesByIndex();
    void sortPairwiseMatches();
    void printPairwiseMatches();
    void printPairwiseMatchesByIndex();
    */
    //non ponter style
    /*
    void sanityCheckMatchesNPS();
    void sanityCheckMatchesByIndexNPS();
    void printPairwiseMatchesNPS();
    void printPairwiseMatchesByIndexNPS();
    void printPairwiseMatchesSortedNPS();
    */
    void sortPairwiseMatches();


    vcl_vector< Curve<double,double> > _ecList;
    bool active;
    /*
    //it seems like node and xnode aren't used anywhere else, and since i dont have
    //the define for a Group,just gunna leave these commented out
    // - spinner -
    Group *node;
    Group *xnode;
    */

    //vcl_vector<vcl_pair<double,int> > cost;

    //List of the indices of the pieces added to the puzzle.
    vcl_vector<int> process;

    int nProcess;
    double tCost;
    double sCost;
    vcl_vector<vcl_pair<vcl_vector<int>,PuzPoint<double> > > open_junc;
    vcl_vector<vcl_pair<vcl_vector<int>,PuzPoint<double> > > closed_junc;
    vcl_vector<vcl_pair<int,int> > old_edges;
    vcl_vector<vcl_pair<int,int> > new_edges;
    vcl_pair<int,int> new_edge;

    searchState(){};
    searchState(vcl_vector<Curve<double,double> > contours);
    searchState(vcl_vector<Curve<double,double> > contours, vcl_vector<Curve<double,double> > econs);
    ~searchState(){};
  
    bool operator==(searchState state2);
    void merge(Curve<double,double>* cv);
    void updateCost();
    void structure();
    void comboMatch();
    void addMatch(vcl_vector<vcl_pair<int,int> > map, double cst, int c1, int c2);
    void purge(); 
 
    
    Curve<double,double>* piece(int index){return &_cList[index];} //keeper
    Curve<double,double>* merged(){return &_merged;}
    Curve<double,double>* constr(int index){return &_constr[index];}
    //int p1(int index){return _pairs[index].first;}
    //int p2(int index){return _pairs[index].second;}
    //vcl_vector<vcl_pair<int,int> > map(int index){return _maps[index];}
    //XForm3x3 xForm(int index){return _xForms[index];}
    int numMatch(){return _numMatch;}
    int numPiece(){return _cList.size();}
    int numCon(){return _constr.size();}


};


class PuzzleSolving {

public:
    vcl_vector<vcl_vector<int> > _pairMatchesByPiece;

    //my new, do nothing constructor
    PuzzleSolving(){ _nPieces = 0; };

    PuzzleSolving(vcl_string fname){
        //file name here seems kinda pointless as it is overwritten when loadFiles(fn) is called
        //and even in that case, i don't think imma be using any of jonahs loading from a file code...
        _filename=fname;
        //setGlobals();
        _nPieces = 0;

    }
  
    ~PuzzleSolving() {};

    //--spinner
    void setContours( vcl_vector<Curve<double,double> >  &contoursIN );
    void printContours();
    void preProcessCurves();
    //--spinner


    searchState pairMatch();
    //void dispTopPair(searchState init, int index);
    vcl_vector<searchState> search(vcl_vector<searchState> states);

    //Curve information functions:
    void characterizeCurves();
    vcl_vector<double> curveInfo(int i);

    //Functions added for inspection of pairwise matches by piece
    void dispTopPairOfPiece(searchState this_state, int pieceNumber, int count);
    int catPairMatchSize(int pieceNumber);
    void categorizePairMatches(searchState this_state);


    //Functions added for inspective of pairwise matches by pair
    void categorizeMatchesByPair(searchState this_state);
    void dispByPair(searchState this_state, int firstIndex, int secondIndex, int count);
    int pairMatchSize(int firstIndex, int secondIndex);

    //Returns the index corresponding to the given parameters
    int returnIndex(searchState this_state, int firstIndex, int secondIndex, int count);

    //Functions added for ordering matches in order to improve piece addition to the puzzle
    void categorizeByNonRepeatingPair(searchState this_state);

    //Toggles the timing functionality
    //void timingToggle();
    //void timingDisplay();

    //void setGlobals();
    int loadFiles (const vcl_string& batchFileName); 
    vcl_vector<searchState> iterateSearch(searchState this_state, int f);
    
    /*
    void imToggle();
    void lnToggle();
    */

    // Below are the methods to handle openGL.
    /*  //which are now commented out b/c im  using my own GUI
    void drawLines(Group* state_node, Curve<double,double>* C, StandardColor color);
    void drawState(searchState* state, int window);
    searchState drawLayout(Group* state_node, searchState state, vcl_vector<int> tree);
    void initGL();
    void deleteAll();
    void setCanvas(GLWindow* canv){_canvas=canv;}
    XForm3x3 autoScale(Group *node, double X, double Y, double size);

    void drawAverageTangent(PuzPoint<double> drawPoint1, double angle, StandardColor col, Group * line_node);
    void drawOnePoint(PuzPoint<double> drawPoint, StandardColor col, int size, Group * line_node);
    void drawOnePoint(double x, double y, StandardColor col, int size, Group * line_node);
    void drawTwoCurves(Curve<double, double> *c1, Curve<double, double> *c2, StandardColor col1,
                            StandardColor col2);
    void drawTwoCurves(Curve<double, double> *c1, Curve<double, double> *c2, StandardColor col1,
                            StandardColor col2, Group * line_node);
    void drawTwoCurvesWithCorners(Curve<double, double> *c1, Curve<double, double> *c2, StandardColor col1,
                            StandardColor col2);
    void drawintMap(Curve<double, double> *c1, Curve<double, double> *c2, intMap map, StandardColor col, Group * line_node);
    void markCorners(Curve<double, double> *c1, StandardColor col, Group * line_node);
    */

    //Functions used by the edge continuity tests
    int visualEdgeCheck(Curve<double, double>* c1, Curve<double, double>* c2, intMap map);

    //Function added to allow for visual debugging functionality
    /* //are now commented out b/c ill write my own debugging stuff
       //we especially can't have this gl stuff...
    vcl_vector<map_with_cost> visualTestDP(Curve<double,double> c1, Curve<double,double> c2);
    map_with_cost visualFineScaleMatch(Curve<double,double> c1i, Curve<double,double> c2, intMap fmap);
    void drawTwoCurvesWithTextOut(Curve<double, double> *c1, Curve<double, double> *c2, StandardColor col1,
                            StandardColor col2, vcl_string out);
    void drawStateWithTextOut(searchState *state, vcl_string out);
    */

    void runPair(int piece1, int piece2);

  
    //this wasnt in here,why? -spinner-
    int initialCheck(Curve<double, double>* c1, Curve<double, double>* c2, intMap map);

    
private:
    vcl_vector<Curve<double,double> >  _Contours;
    vcl_vector<Curve<double,double> >  _EContours;
    vcl_vector<vcl_string> _filenames;
    vcl_vector<vcl_string> _imagenames;
    vcl_vector<vcl_string> _masknames;
    /*
    //seems like _lines isn't used anywhere, and i don't have PolylineGeom, luckily it
    //seems i dont' need it --> commenting this out
    // - spinner -
    vcl_vector<PolylineGeom*> _lines;
    */
    //Group* _linesGroup;
    vcl_string _filename;
    //GLWindow* _canvas;
    int _nPieces;


    vcl_vector< Curve<double,double>* > __contours__;
 

};

// ---------------puzzle solving functions-------------------

// -------------end puzzle solving functions-----------------

// Global Functions

vcl_vector<vcl_pair<double,vcl_vector<vcl_pair<int,int> > > > 
  testDP(Curve<double,double> c1, Curve<double,double> c2);

vcl_pair<double,vcl_vector<vcl_pair<int,int> > > 
  fineScaleMatch(Curve<double,double> c1i, Curve<double,double> c2, 
     vcl_vector<vcl_pair<int,int> > fmap);

vcl_vector<vcl_pair<int,int> >
  localReg(Curve<double,double>* cv, Curve<double,double>* mg, 
     double* dis,double* len, double* dgn, int num_iter);

vcl_pair<double,vcl_vector<vcl_pair<int,int> > >
  fineCost(Curve<double,double> c1, Curve<double,double> c2, 
     vcl_vector<vcl_pair<int,int> > map, int type);

int edgeCheck(Curve<double, double> *c1, Curve<double, double> *c2, intMap map);
int checkEdges(Curve<double, double> *c1, Curve<double, double> *c2);

bool distanceCheck(Curve<double, double> *c1, Curve<double, double> *c2, int checkIndex1, int checkIndex2);

//Returns true if angles are close to one another
bool anglesAreClose(double angle1, double angle2);



bool inPolygon(double x, double y, Curve<double,double> *c);
vcl_pair<double,int> ptDist(double x, double y, Curve<double,double> *c);
double detectOverlap(Curve<double,double> *c1, Curve<double,double> *c2);
XForm3x3 regContour(Curve<double,double> *c1, Curve<double,double> *c2, vcl_vector<vcl_pair<int,int> > map, bool flip=0, XForm3x3 *out = 0);
double flat(Curve<double,double> *c);
//void imSmooth(RGBAImage *img, int window, vcl_vector<double> *ex, vcl_vector<double> *ey, vcl_vector<double> *lum);
double const_test(XForm3x3 T, Vect3 P1, Vect3 P2, Vect3 P3, Vect3 P4);
vcl_pair<double,double> center(Curve<double,double> *c);
void printAlert(vcl_string out);
void printDividingLine();
double pointToPointDistance(PuzPoint<double> p1, PuzPoint<double> p2);
double findAverageTangent(Curve<double, double> *c1, int index, bool moveForward, int numPtsAveraged);
bool checkLocalOverlap(Curve<double, double>* c1, Curve<double, double> *c2, bool forward1, 
                int numPointsToCheck, int start1);

#endif  /*  PUZZLESOLVING_H */
