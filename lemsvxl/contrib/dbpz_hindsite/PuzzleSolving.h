#ifndef PUZZLESOLVING_H
#define PUZZLESOLVING_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Curve.h"
#include "Point.h"
#include "DPMatch.h"
#include "FineDPMatch.h"
#include "xform.h"
#include "utils.h"

#include "global.h"

typedef std::vector<std::pair<int,int> > intMap;
typedef std::pair<double,std::vector<std::pair<int,int> > > map_with_cost;
typedef std::vector<std::pair<double, int> > indexedMeasures; 


class map_cost_less {
 public:
  bool operator() (std::pair<double,std::vector<std::pair<int,int> > > map1, 
       std::pair<double,std::vector<std::pair<int,int> > > map2) { 
      return map1.first<map2.first; 
  }
};

class cost_ind_less {
 public:
  bool operator() (std::pair<double,int> c1, std::pair<double,int> c2){ 
      return c1.first<c2.first; 
  }
};
class cost_ind_more {
 public:
  bool operator() (std::pair<double,int> c1, std::pair<double,int> c2){ 
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
    std::pair<int,int>    whichCurves;  //old _pairs
    // DATA

    pairwiseMatch(){};
    void print(){ std::cout << "match [" << myIndex << "]->cost: [" << cost << "]   curves(" << whichCurves.first << "," << whichCurves.second << ") " << std::endl; /*xForm->print();*/ };

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
    std::vector<Curve<double,double> > _cList;

    //pointer style
    //std::vector< pairwiseMatch* > _matches;
    //non pointer style
    std::vector< pairwiseMatch > _matches;

    //------------------MATCH-----------------------
    //map indexes     vector<pair<int,int>>   -- says which point maps to which point
    //  vector  -- corresponds to _pairs vector [i.e. this[0] and _pairs[0]]
    //std::vector<std::vector<std::pair<int,int> > > _maps;
    //  vector -- transform corresponds to the pairs, convention: either smaller gets transform or the second one is Xformed or ...
    //std::vector<XForm3x3> _xForms;
    //std::vector<std::pair<int,int> > _pairs;
    //------------------MATCH-----------------------


    int _numMatch;
    int _num_new;
    Curve<double,double> _merged;
    std::vector<Curve<double,double> > _constr;

    std::string *_filename; //Added to store filename

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


    std::vector< Curve<double,double> > _ecList;
    bool active;
    /*
    //it seems like node and xnode aren't used anywhere else, and since i dont have
    //the define for a Group,just gunna leave these commented out
    // - spinner -
    Group *node;
    Group *xnode;
    */

    //std::vector<std::pair<double,int> > cost;

    //List of the indices of the pieces added to the puzzle.
    std::vector<int> process;

    int nProcess;
    double tCost;
    double sCost;
    std::vector<std::pair<std::vector<int>,PuzPoint<double> > > open_junc;
    std::vector<std::pair<std::vector<int>,PuzPoint<double> > > closed_junc;
    std::vector<std::pair<int,int> > old_edges;
    std::vector<std::pair<int,int> > new_edges;
    std::pair<int,int> new_edge;

    searchState(){};
    searchState(std::vector<Curve<double,double> > contours);
    searchState(std::vector<Curve<double,double> > contours, std::vector<Curve<double,double> > econs);
    ~searchState(){};
  
    bool operator==(searchState state2);
    void merge(Curve<double,double>* cv);
    void updateCost();
    void structure();
    void comboMatch();
    void addMatch(std::vector<std::pair<int,int> > map, double cst, int c1, int c2);
    void purge(); 
 
    
    Curve<double,double>* piece(int index){return &_cList[index];} //keeper
    Curve<double,double>* merged(){return &_merged;}
    Curve<double,double>* constr(int index){return &_constr[index];}
    //int p1(int index){return _pairs[index].first;}
    //int p2(int index){return _pairs[index].second;}
    //std::vector<std::pair<int,int> > map(int index){return _maps[index];}
    //XForm3x3 xForm(int index){return _xForms[index];}
    int numMatch(){return _numMatch;}
    int numPiece(){return _cList.size();}
    int numCon(){return _constr.size();}


};


class PuzzleSolving {

public:
    std::vector<std::vector<int> > _pairMatchesByPiece;

    //my new, do nothing constructor
    PuzzleSolving(){ _nPieces = 0; };

    PuzzleSolving(std::string fname){
        //file name here seems kinda pointless as it is overwritten when loadFiles(fn) is called
        //and even in that case, i don't think imma be using any of jonahs loading from a file code...
        _filename=fname;
        //setGlobals();
        _nPieces = 0;

    }
  
    ~PuzzleSolving() {};

    //--spinner
    void setContours( std::vector<Curve<double,double> >  &contoursIN );
    void printContours();
    void preProcessCurves();
    //--spinner


    searchState pairMatch();
    //void dispTopPair(searchState init, int index);
    std::vector<searchState> search(std::vector<searchState> states);

    //Curve information functions:
    void characterizeCurves();
    std::vector<double> curveInfo(int i);

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
    int loadFiles (const std::string& batchFileName); 
    std::vector<searchState> iterateSearch(searchState this_state, int f);
    
    /*
    void imToggle();
    void lnToggle();
    */

    // Below are the methods to handle openGL.
    /*  //which are now commented out b/c im  using my own GUI
    void drawLines(Group* state_node, Curve<double,double>* C, StandardColor color);
    void drawState(searchState* state, int window);
    searchState drawLayout(Group* state_node, searchState state, std::vector<int> tree);
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
    std::vector<map_with_cost> visualTestDP(Curve<double,double> c1, Curve<double,double> c2);
    map_with_cost visualFineScaleMatch(Curve<double,double> c1i, Curve<double,double> c2, intMap fmap);
    void drawTwoCurvesWithTextOut(Curve<double, double> *c1, Curve<double, double> *c2, StandardColor col1,
                            StandardColor col2, std::string out);
    void drawStateWithTextOut(searchState *state, std::string out);
    */

    void runPair(int piece1, int piece2);

  
    //this wasnt in here,why? -spinner-
    int initialCheck(Curve<double, double>* c1, Curve<double, double>* c2, intMap map);

    
private:
    std::vector<Curve<double,double> >  _Contours;
    std::vector<Curve<double,double> >  _EContours;
    std::vector<std::string> _filenames;
    std::vector<std::string> _imagenames;
    std::vector<std::string> _masknames;
    /*
    //seems like _lines isn't used anywhere, and i don't have PolylineGeom, luckily it
    //seems i dont' need it --> commenting this out
    // - spinner -
    std::vector<PolylineGeom*> _lines;
    */
    //Group* _linesGroup;
    std::string _filename;
    //GLWindow* _canvas;
    int _nPieces;


    std::vector< Curve<double,double>* > __contours__;
 

};

// ---------------puzzle solving functions-------------------

// -------------end puzzle solving functions-----------------

// Global Functions

std::vector<std::pair<double,std::vector<std::pair<int,int> > > > 
  testDP(Curve<double,double> c1, Curve<double,double> c2);

std::pair<double,std::vector<std::pair<int,int> > > 
  fineScaleMatch(Curve<double,double> c1i, Curve<double,double> c2, 
     std::vector<std::pair<int,int> > fmap);

std::vector<std::pair<int,int> >
  localReg(Curve<double,double>* cv, Curve<double,double>* mg, 
     double* dis,double* len, double* dgn, int num_iter);

std::pair<double,std::vector<std::pair<int,int> > >
  fineCost(Curve<double,double> c1, Curve<double,double> c2, 
     std::vector<std::pair<int,int> > map, int type);

int edgeCheck(Curve<double, double> *c1, Curve<double, double> *c2, intMap map);
int checkEdges(Curve<double, double> *c1, Curve<double, double> *c2);

bool distanceCheck(Curve<double, double> *c1, Curve<double, double> *c2, int checkIndex1, int checkIndex2);

//Returns true if angles are close to one another
bool anglesAreClose(double angle1, double angle2);



bool inPolygon(double x, double y, Curve<double,double> *c);
std::pair<double,int> ptDist(double x, double y, Curve<double,double> *c);
double detectOverlap(Curve<double,double> *c1, Curve<double,double> *c2);
XForm3x3 regContour(Curve<double,double> *c1, Curve<double,double> *c2, std::vector<std::pair<int,int> > map, bool flip=0, XForm3x3 *out = 0);
double flat(Curve<double,double> *c);
//void imSmooth(RGBAImage *img, int window, std::vector<double> *ex, std::vector<double> *ey, std::vector<double> *lum);
double const_test(XForm3x3 T, Vect3 P1, Vect3 P2, Vect3 P3, Vect3 P4);
std::pair<double,double> center(Curve<double,double> *c);
void printAlert(std::string out);
void printDividingLine();
double pointToPointDistance(PuzPoint<double> p1, PuzPoint<double> p2);
double findAverageTangent(Curve<double, double> *c1, int index, bool moveForward, int numPtsAveraged);
bool checkLocalOverlap(Curve<double, double>* c1, Curve<double, double> *c2, bool forward1, 
                int numPointsToCheck, int start1);

#endif  /*  PUZZLESOLVING_H */
