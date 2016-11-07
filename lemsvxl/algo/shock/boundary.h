#ifndef  _BOUNDARY_H
#define  _BOUNDARY_H

#include <vcl_iostream.h>
#include <vcl_sstream.h>
//using namespace std;

#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_map.h>

#include "ishock-common.h"
#include "belements.h"

typedef enum {
  BOGUS_BOUNDARY_TYPE,
  NORMAL_BOUNDARY=1,
  BUCKETING_BOUNDARY=2,
  DYNVAL_BOUNDARY=3,
} BOUNDARY_TYPE;

typedef enum {
  BOGUS_BOUNDARY_LIMIT,
  NO_LIMIT=1,
  BIG_RECTANGLE=2,
  BIG_CIRCLE=3,
} BOUNDARY_LIMIT;

class IShockView;
class IShock;
class Boundary;

#include "CEDT.h"

//iterator type to use for going through the boundary elements
typedef vcl_vector<BElement* > BElmVectorType;
typedef vcl_map<int, BElement* > BElmListType;
typedef vcl_map<int, BElement* >::iterator BElmListIterator;
typedef vcl_map<int, BElement* >::reverse_iterator RBElmListIterator;
typedef vcl_pair<int, BElement*> ID_BElm_pair;
typedef vcl_list<BContour*> BContourList;
typedef vcl_list<BContour*>::iterator BContourListIterator;

class Boundary 
{
friend class IShock;
protected:
  BOUNDARY_TYPE  _type;
  BOUNDARY_LIMIT _BoundaryLimit;
public:
  IShock* shock;              //shock object created by the current boundary object
  CEDT*   _CEDT;              //structure for CEDT on current boundary

  vcl_map<int, BElement*> BElmList;        //vcl_list of all the boundary elements
  vcl_map<int, BElement*> taintedBElmList;  //some  operation has affected these elements
                            //so shocks need to be recomputed from them
  vcl_map<int, BElement*> update_list;      //to store all boundary elements
                            //that need to be redrawn

  BContourList contourList;

  Boundary (BOUNDARY_LIMIT newbndlimit=NO_LIMIT);
  virtual ~Boundary ();

  BOUNDARY_TYPE type() { return _type; }
  BOUNDARY_LIMIT GetBoundaryLimit() { return _BoundaryLimit; }
  void SetBoundaryLimit (BOUNDARY_LIMIT newbndlimit) { _BoundaryLimit = newbndlimit; }

public:
  int  _nextAvailableID;      //the next available id
  bool  _bIsPreprocesvcl_singNeeded;
  bool  _bIsBPointsOnly;
  bool  _bIsBLinesOnly;

  virtual void _Initialize(BOUNDARY_LIMIT newbndlimit=NO_LIMIT);  //initialize the boundary

  //add and remove BElements
  void addBElement (BElement* elm);
  void addBElement_InitSource (BElement* elm);
  void delBElement (BElement* elm);
  //BElmListIterator delBElement (BElmListIterator it);

  void delBElementList ();    //delete all the BElements in the vcl_list
  //delete all the shocks caused by a boundary element
  void delBElementShockList (BElement* belm);

  //returns the next available ID 
  int nextAvailableID(){_nextAvailableID++; return _nextAvailableID;}

public:
  virtual void Clear();

  void clear_contours();
  void weedOutShortcontours();
  void clearBoundaryLimitElements ();
  void addBigCircleToLimitOutsideShocks();
  void addBigRecvcl_tangleToLimitOutsideShocks();

  //returns # of BElements in the BElmList
  int nBElement() { return BElmList.size(); }

  //Add a GUIPoint, return the BPoint object
  virtual BPoint* addGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent=-100.0, double conf=0.0);
  virtual BPoint* addNonGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent=-100.0);

  //Add a GUILine, return the BLine object
  virtual BElement* addGUILine (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey);
  virtual BElement* addGUILineBetween (BPoint* spt, BPoint* ept);
  virtual BElement* addGUILineBetween (BPoint* spt, Point end_pt, BPoint** newEndPt);
  BElement* addGUILine (Point start_pt, Point end_pt);
  BElement* addGUILineBetween (Point start_pt, BPoint* ept, BPoint** newStartPt);
  
  virtual void addPolyLineBetween (BPoint* spt, BPoint* ept, vcl_vector<Point> pts, int id);

  //Add a GUIArc, return the BArc object
  virtual BElement* addGUIArc (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey,
                COORD_TYPE centerx, COORD_TYPE centery, double r, ARC_NUD nud, ARC_NUS nus);
  virtual BElement* addGUIArcBetween (BPoint* spt, BPoint* ept, Point center, 
                  double r, ARC_NUD nud, ARC_NUS nus);
  virtual BElement* addGUIArcBetween (BPoint* bspt, Point ept, Point center, 
                  double r, ARC_NUD nud, ARC_NUS nus);
  BElement* addGUIArcBetween (Point spt, BPoint* bept, Point center, 
                  double r, ARC_NUD nud, ARC_NUS nus);

  void addGUIBiArcBetween (BPoint* arc_start_pt, double SAngle, 
                    BPoint* arc_end_pt, double EAngle);

  BElementList getAllNeighboringBElementsFromAGUIElement (BElement* GUIElm);

  virtual bool delGUIElement (BElement* elm);

  //functions to modify the GUIElements
  void moveGUIElement (BElement* elm, COORD_TYPE dx, COORD_TYPE dy);
  void mergeDuplicatePoints (BPoint* pt1, BPoint* pt2);

  //this function is used to chop a boundary element from its endpoint
  //to the midpoint given which should be on the boundary
  BPoint* chopBElement(BElement* belm, BPoint* endPt, Point midPt, bool chopCompletely=false);

  //for redrawing a particular BElement
  void updateBoundaryDrawing (BElement* belm);

  //PREPROCESSING
  BPoint* doesBPointExist (BPoint* bpoint);
  void PreProcessBPoint (BPoint* bp);
  void PreProcessGUIElement (BElement* belm);
  void PreProcessBoundary (void);
  void PreProcessBoundaryForEdgeInput(double position_accuracy, double angle_accuracy, 
    double operator_width, double operator_length);

  //FILEIO
  bool  _bLoadConAsLine;
  void LoadFile (const char* filename);
    void LoadBND (const char* filename);
    void LoadTXT (const char* filename);
    void LoadCON_Point (const char* filename);
    void LoadCON_Line (const char* filename);
    void LoadEDG (const char* filename);
    void LoadCEM (const char* filename);
  void SaveBND (const char* filename);
  void SaveCON (const char* filename);
  void SaveCEM (const char* filename);

  void setRangeOfInfluence (BElement* elm);
  void show_all();

  //Debug Functions
  void DebugPrintBoundaryList();
  void DebugPrintTaintedBoundaryList();
  void MessageOutBoundarySummaries (int wndid);
  void DebugPrintBElementInfoFromID(int id);

  //Extrinsic computations: REQ FOR TRANSITION TO VXL
  void UpdateBoundary();
};

class Boundary_CEDT : public Boundary
{
public:
  Boundary_CEDT(){}
  virtual ~Boundary_CEDT(){}
};


#endif
