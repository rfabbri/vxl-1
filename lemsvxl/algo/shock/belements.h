#ifndef  _BELEMENTS_H
#define  _BELEMENTS_H

#include <vcl_iostream.h>
#include <vcl_sstream.h>

#include <vcl_list.h>
#include <vcl_vector.h>

#include "BaseGUIGeometry.h"
#include "ishock-common.h"
//#include "dyn-val.h"

typedef enum 
{
  BOGUS_BND,
  BPOINT, BLINE, BARC,
  BCONTOUR,
  BCONTACTPOINT  //only for midElement() computation, not a real type 
} BOUNDARY_ELEMENT_TYPE;

// All the boundary element classes
class BElement;
class BPoint;
class BLine;
class BArc;

//The basic shockelement class boundary elements need to know about
class SIElement;
class SINode;
class SISource;
class SILink;
class SIContact;

typedef enum{
  // left and right are determined from the view of the A3 node 
  // looking at the boundary
  RIGHT_BELEMENT = 0,
  CENTER_BELEMENT = 1,
  LEFT_BELEMENT = 2,
  BOGUS_TYPE = 3
} PROJ_BELEMENT_TYPE;

//some useful type definitions
typedef vcl_list<BElement* > BElementList;
typedef BElementList::iterator BElementListIterator;

/*****************************************************************
  BOUNDARY COORDINATE
  By Nhon Trinh
  Date: Jul 21, 2004
*/  
  typedef struct
  {
    /* Definition of origin of a belement:
        BPoint: the point itself
        Bline: the _start point
        BArc:  the _start point
       Definition of distance:
        BPoint: distance = 0;
        BLine: from the origin
        BArc: arc length from the origin
    */
    BElement* belm;
    double distance;
    double btangent;
  }BCoordinate;

  // struct to hold info of projection of A3 points on the boundary
  typedef struct 
  {
    SINode* A3_node;
    PROJ_BELEMENT_TYPE type;
    double distance;  // distance from the origin of the boundary element
    double btangent;  // tangent of the boundary at this coordinate 
  } A3_node_proj; 

  typedef struct 
  { 
    SINode* A3_node;
    BElementList belm_list; // ordered CCW from the view of the node
    BCoordinate bcoors[3]; // 0: right, 1: center, 2: left
    double node_tangent; // tangent of the shock at the node
    double arc_angle;
  }A3node_bnd_link;

  typedef struct{
    Point pt;
    double tangent;
  }point_tangent;

typedef vcl_list<SIElement* >SIElementList;
typedef vcl_list<A3_node_proj*> A3_node_proj_list;

class BElement : public BaseGUIGeometry
{
protected:
  int  _id;
  bool _bGUIElm;        //==TRUE: GUIElement, ==FALSE: others
  BOUNDARY_ELEMENT_TYPE _type;
  int _edge_id; //hack for storing curve ids

public:

  SIElementList shockList;    //vcl_list of shocks formed by this boundary
  // added by Nhon Jul 29 2004. contains info of A3 node projection on the boundary
  A3_node_proj_list A3_node_list;
  //
  typedef SIElementList::iterator shockListIterator;
  typedef SIElementList::reverse_iterator RshockListIterator;

  BElement ();
  virtual ~BElement (){}
  int id() { return _id; }
  int edge_id() {return _edge_id;}
  void set_edge_id(int edge_id) {_edge_id = edge_id;}
  bool isGUIElm() { return _bGUIElm; }
  void setGUIElm(bool bguielm) { _bGUIElm = bguielm; }
  //For GUIElm, return itself, else, return the twinElm.
  virtual BElement* GUIElm() {
    if (_bGUIElm)
      return this;
    else
      return NULL;
  }

  BOUNDARY_ELEMENT_TYPE type() { return _type; }

  //for compatibility
  virtual Point start()=NULL;
  virtual Point end()=NULL;

  virtual BPoint* s_pt() { return NULL; }
  virtual BPoint* e_pt() { return NULL; }

  //number of shocks formed by this boundary
  int nSElement() { return shockList.size(); }

  //functions to add and remove links to shocks
  void addBnd2ShockLink (SIElement* elm) { shockList.push_back(elm); }
  void delBnd2ShockLink (SIElement* elm) { shockList.remove(elm); }

  virtual void getInfo (vcl_ostream& ostrm) {}

  //return the neighboring Boundary Element linked via this shock
  BElement* getNeighboringBElementFromSILink (SILink* silink);
  //return the neighboring Boundary Element linked via this shock
  BElement* getNeighboringBElementFromSISource (SISource* sisource);
  //ONLY CHECK FOR SILink and SISource, FOR ALL THE OTHERS, RETURN NULL.
  BElement* getNeighboringBElementFromSIElement (SIElement* sielm);

  BElementList getAllNeighboringBElements();

  //functions for streamlining CEDT functions
  virtual DIST_TYPE distFromPoint (Point pt){return 0;}
  virtual DIST_TYPE dirFromPoint (Point pt){return 0;}

  //functions for computing boundary tangents
  virtual double TangentAlongCurveAtPoint (Point pt)=0;
  virtual double TangentAgainstCurveAtPoint (Point pt)=0;

  //this is used to update connectivity of line and arcs when their
  //endpoints are changed
  virtual void reconnect(BPoint* oldPt, BPoint* newPt){}

  // get explicit tangent point from A3_node_info
  point_tangent* get_A3_proj_point_tangent(A3_node_proj* p);

  // find intersection between a line (specified by (origin) and (line_angle)) and a belement
  // return distance from the start() of the belement
  double intersect_distance(Point* origin, double line_angle);
};    

#define TANGENT_UNDEFINED -100 //For BPoint tangent

//points can be either free svcl_tanding, at endpoints of lines
//or at junctions of lines and arcs
class BPoint : public BElement 
{
protected:
  Point _pt;
  INPUT_TAN_TYPE _dir; //for point tangent pairs
  double _conf;
public:

  bool _bSomething;
  bool isSomething() { return _bSomething;}

  Point pt() {
    return _pt;
  }

  //vcl_list of pointers to elements this point is connected to 
  //this vcl_list is always maintained at a CCW order
  BElementList LinkedBElmList;

  BPoint (COORD_TYPE x, COORD_TYPE y, int id, bool bGUI=false, 
        INPUT_TAN_TYPE tangent=TANGENT_UNDEFINED, double conf=0.0);
  virtual ~BPoint ();

  virtual Point start() { return _pt; }
  virtual Point end() { return _pt; }

  //just for compatibility
  virtual BPoint* s_pt() { return this; }
  virtual BPoint* e_pt() { return this; }

  int nLinkedElms(){return LinkedBElmList.size();}
  BElement* BPoint::_prev (BElementList::iterator it);
  SIContact* GetReActivatingContactShock (BElement* lelm, BElement* relm);
  void connectTo(BElement* elm);
  void disconnectFrom(BElement* elm);
  void moveBPoint (COORD_TYPE dx, COORD_TYPE dy)  {_pt.x+=dx; _pt.y+=dy;}
  void mergeWith (BPoint* bpt);

  //these will return right and left elements 
  //only if it is a connected point
  BElement* getElmToTheRightOf(BElement* elm);
  BElement* getElmToTheLeftOf(BElement* elm);

  bool isFreePoint(){return nLinkedElms()==0;}
  bool isEndPoint(){return nLinkedElms()==2;}
  bool isJunctionPoint(){return nLinkedElms()>2;}

  bool hasATangent(){
    if (isFreePoint())
      return _dir!=TANGENT_UNDEFINED;
    else if (isEndPoint())
      return true;
    else
      return false;
  }

  //functions for computing boundary tangents
  virtual double TangentAlongCurveAtPoint (Point pt){return 0;}
  virtual double TangentAgainstCurveAtPoint (Point pt){return 0;}

  bool isFormingColinearContact (Point src, SIContact** cc1, SIContact** cc2);

  DIST_TYPE distPoint (Point pt) {
    return _distPointPoint (pt, _pt);
  }
  DIST_TYPE distSqPoint (Point pt) {
    return _distSqPointPoint (pt, _pt);
  }

  //functions for streamlining CEDT functions
  virtual DIST_TYPE distFromPoint (Point pt){return distPoint(pt);}
  virtual VECTOR_TYPE dirFromPoint (Point pt){return _vPointPoint(_pt, pt);}

  INPUT_TAN_TYPE tangent() { return _dir;}
  void set_tangent(INPUT_TAN_TYPE tangent){_dir = tangent;}

  double conf() {return _conf;}
  void set_conf(double conf){_conf = conf;}

  virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class BLine : public BElement
{
protected:
  VECTOR_TYPE _u; //unit direction of this line
  VECTOR_TYPE _n; //normal direction of this line, =u+M_PI_2
  DIST_TYPE  _l; //length of this line
  Point _start, _end;
  BPoint *startPt, *endPt; //two end point of line
  
public:
  BLine  *_twinLine; //the other twin line: for shock computation

  virtual Point start() { return _start; }
  virtual Point end() { return _end; }
  
  virtual BPoint* s_pt() { return startPt; }
  virtual BPoint* e_pt() {
    return endPt;
  }
  virtual BLine*  twinLine() { return _twinLine; }

  virtual BElement* GUIElm() {
    if (_bGUIElm)
      return this;
    else
      return _twinLine;
  }

  //Constructor for GUI
  //BLine () {}
  BLine (BPoint* startpt, BPoint* endpt, int id=-1, bool bGUI=false);
  virtual ~BLine ()
  {
  }

  DIST_TYPE L () { return _l; }
  VECTOR_TYPE U () { return _u; }
  VECTOR_TYPE N () { return _n; }

  //return t if valid
  //else return 0
  double _isPointValidInT (Point pt) {
    double t = _getT (pt, _start, _end);
    if (t>0 && t<1)
      return t;
    else
      return 0;
  }
  double _isPointValidInTFuzzy (Point pt, double FUZZYEPSILON) {
    double t = _getT (pt, _start, _end);
    if (t>FUZZYEPSILON && t<1-FUZZYEPSILON)
      return t;
    else
      return 0;
  }
  bool _isPointValidInPlane (Point pt) {
    //SLOW:
    VECTOR_TYPE vSP = _vPointPoint (_start, pt);
    double d = _angle_vector_dot (_n, vSP);
    if (d>0)
      return true;
    else
      return false;
  }
  //double isPointValidInTPlane (Point pt);
  double isPointValidInTPlaneFuzzy (Point pt);

  DIST_TYPE distPoint (Point pt) {
    return _distPointLine (pt, _start, _end);
  }
  DIST_TYPE validDistPoint (Point pt);
  DIST_TYPE validDistSqPoint (Point pt);
  DIST_TYPE initDistSqPointGUI (Point pt); //For local shock only.

  //functions for streamlining CEDT functions
  virtual DIST_TYPE distFromPoint (Point pt) {
    return validDistPoint(pt);
  }
  //assume that it is in the valid region

  virtual VECTOR_TYPE dirFromPoint (Point pt) {return _n;}
  //functions for computing boundary tangents
  virtual double TangentAlongCurveAtPoint (Point pt) {return _u;}
  virtual double TangentAgainstCurveAtPoint (Point pt) { return angle02Pi(_u+M_PI); }

  virtual void reconnect(BPoint* oldPt, BPoint* newPt);

  virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class BArc : public BElement 
{
protected:
  Point  _start, _end;
  Point  _center;
  double _R;
  ARC_NUD _nud; //ARC_NUD_CW:+1 , ARC_NUD_CCW:-1
  BPoint *startPt, *endPt; //two end point of arc

public:
  bool      _bAcross;
  VECTOR_TYPE _StartVector;
  VECTOR_TYPE _EndVector;
  VECTOR_TYPE _InTangent;
  VECTOR_TYPE _OutTangent;
  VECTOR_TYPE _CCWStartVector;
  VECTOR_TYPE _CCWEndVector;

  BArc   *_twinArc;  //the other twin arc: for shock computation
  virtual BPoint* s_pt() { return startPt; }
  virtual BPoint* e_pt() { return endPt; }
  virtual BArc*  twinArc() { return _twinArc; }

  virtual BElement* GUIElm() {
    if (_bGUIElm)
      return this;
    else
      return _twinArc;
  }

  virtual Point start() { return _start; }
  virtual Point end() { return _end; }
  Point center() { return _center; }
  double R() { return _R; }
  ARC_NUD nud() { return _nud; }

  //length of the arc
  double L() { return _R*CCW(_CCWStartVector,_CCWEndVector); }

  Point  CCWStartPoint() { return (_nud==1) ? _end : _start; }
  Point  CCWEndPoint() { return (_nud==1) ? _start : _end; }

  //tangent going into the arc at the startpt, etc.
  VECTOR_TYPE InTangentAtStartPt() {
    return _InTangent;
  }
  VECTOR_TYPE OutTangentAtStartPt() {
    return angle02Pi (_InTangent + M_PI);
  }
  VECTOR_TYPE InTangentAtEndPt() {
    return angle02Pi (_OutTangent + M_PI);
  }
  VECTOR_TYPE OutTangentAtEndPt() {
    return _OutTangent;
  }

  //functions for computing boundary tangents
  virtual double TangentAlongCurveAtPoint (Point pt);
  virtual double TangentAgainstCurveAtPoint (Point pt);

  //Constructor for GUI
  //BArc () {}
  BArc (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
      Point center, double r, ARC_NUD nud);
  virtual ~BArc () {}

  bool isVectorLessThanStart (VECTOR_TYPE v);
  bool isVectorGreaterThanEnd (VECTOR_TYPE v);
  bool isVectorValid (VECTOR_TYPE v);
  bool isVectorFuzzilyValid (VECTOR_TYPE v, double epsilon);

  //bool isPointValidInRAngle (Point pt, VECTOR_TYPE& a);
  bool isPointValidInRAngleFuzzy (Point pt, VECTOR_TYPE& a);

  DIST_TYPE distPoint (Point pt) {  //geometry distance
    DIST_TYPE d = _distPointPoint(_center, pt);
    return vcl_fabs (d-_R);
  }
  //first check validation, then return geometry distance. return ISHOCK_DIST_HUGE if not valid
  DIST_TYPE validDistPoint (Point pt);
  DIST_TYPE validDistSqPoint (Point pt);
  DIST_TYPE initDistSqPointGUI (Point pt); //For local shock only.

  //functions for streamlining CEDT functions
  virtual DIST_TYPE distFromPoint (Point pt) {return validDistPoint(pt);}
  virtual DIST_TYPE dirFromPoint (Point pt)
  {
    if (_nud==ARC_NUD_CW)
      return _vPointPoint(_center, pt);
    else
      return _vPointPoint(pt, _center);
  }

  virtual void reconnect(BPoint* oldPt, BPoint* newPt);

  virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

//#############################################################
//     CONTOUR
//#############################################################

class BContour : public BElement
{
public:
  BElementList elms;

  BContour(){}
  BContour(int id);

  virtual ~BContour(){}

  //for compatibility NEED TO BE UPDATED
  virtual Point start(){return Point(0,0);}
  virtual Point end(){ return Point(0,0);}

  //functions for computing boundary tangents
  virtual double TangentAlongCurveAtPoint (Point pt){return 0;}
  virtual double TangentAgainstCurveAtPoint (Point pt){return 0;}

  virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus(){}
};
    
#endif
