#ifndef _SIELEMENT_H
#define _SIELEMENT_H

/////////////////////////////////////////////////////
// INTRINSIC SHOCK
/////////////////////////////////////////////////////

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vnl/vnl_math.h>

#include "BaseGUIGeometry.h"
#include "ishock-common.h"
#include "boundary.h"
#include "dyn-val.h"

typedef enum {
  BOGUS_JUNCTION_TYPE,
  SINK_JUNCTION,
  DEGENERATE_JUNCTION,
  LEFT_REGULAR_JUNCTION,
  RIGHT_REGULAR_JUNCTION,
  TO_INFINITY,          //special!!
} JUNCTION_TYPE;

typedef enum {
  BOGUS_COLINEAR_CONTACT,
  ISOLATED_CONTACT,
  LEFT_COLINEAR_CONTACT,
  RIGHT_COLINEAR_CONTACT,
} COLINEAR_CONTACT;

typedef enum {
  BOGUS_TAU_DIRECTION,
  TAU_INCREASING,
  TAU_DECREASING,
}TAU_DIRECTION_TYPE;

typedef enum{
  NOT_TRAVERSED=1,
  TRAVERSED_FORWARD=2,
  TRAVERSED_BACKWARD=3,
  TRAVERSED_BOTHWAYS=4
}GRAPH_TRAVERSAL_STATE;

//The shock element classes
class SIElement;
  class SINode;
    class A3Source;
    class SISource;
    class SIJunct;
    class SISink;
  class SILink;
    class SIPointPoint;
    class SIPointLine;
    class SILineLine;
    class SIPointLineContact;
    class SIPointArc;
    class SILineArc;
    class SIArcArc;
    class SIPointArcContact;

//some useful type definitions
typedef vcl_list<SILink* >SILinksList;
typedef vcl_list<SINode* >SINodesList;

//this needs to be converted to a == operator
//int isSameSource(SISource* A, SISource* B);

class SIElement : public BaseGUIGeometry
{
public:
  typedef enum { 
    BOGUS_SHOCK_LABEL,           
    REGULAR='R', SEMI_DEGENERATE = 'S', DEGENERATE = 'D', CONTACT = 'C' 
  } SHOCKLABEL;

  typedef enum {
    BOGUS_SHOCK_TYPE,
    A3SOURCE, SOURCE, SINK, JUNCT, POINTPOINT, POINTLINE, POINTARC, LINELINE, 
    LINEARC, ARCARC, POINTLINECONTACT, POINTARCCONTACT, 
    THIRDORDER, ARCTHIRDORDER
  } SHOCK_TYPE;

  typedef enum {
    BOGUS_GRAPH_TYPE,
    NODE, LINK
  } GRAPH_TYPE;

  typedef enum {
    BOGUS_COMP=0,
    NO_COMP, LINE_COMP, PP_ONLY, P_PT, PT_PT
  } COMPLETION_TYPE;

protected:
  int  _id;      //unique id
  int  _order;    //order (i.e., First, second, Third and Fourth
  bool  _bActive;  //bool, =1:active, =0:dead
  bool  _bPropagated;//different from _bActive because
              //a contact shock can still be active after it propagates
  bool  _bValid;    //this flag is set to false after dynamic validation in the 
              //constructors of the shocks
  bool  _bHidden;  //I need it for simple pruning operations
              //for contour grouping 
  GRAPH_TRAVERSAL_STATE  _traverseState; //state of this element in the traversal

  SHOCKLABEL  _label;  //REGULAR, SEMI_DEGENERATE, or CONTACT,...
  SHOCK_TYPE  _type;  //SPOINT, SLINE, SPARABOLA,..
  GRAPH_TYPE  _graph_type; //NODE or LINK

  RADIUS_TYPE  _startTime; //formation time (distance)
  RADIUS_TYPE  _simTime;   //for ordering in propagation
  Point    _origin;    //origin of the intrinsic coordinate system

  //SYMMETRY TRANSFORM 
  double  _dPnCost;  //vcl_cost of pruning this shock
  double  _dOC;      //change in the original contour length by the removal of the current shock
  double  _dNC;      //length of the new contour added

  //In fact, _dPnScore = |_dOC + _dNC(parents) - _dNC| + _dPnScore (parents)

  COMPLETION_TYPE _ctype; //type of completion for gap transform

public:
  //for ESF operations
  bool  bIO;    //bool Inside=1, Outside=0
  bool  bIOVisited;
  int  IOLabel;

  int  id() { return _id; }
  void  setId (int id) { _id = id; }
  int  order() { return _order; }
  bool  isActive() { return _bActive; }
  bool  isPropagated() { return _bPropagated; }
  bool  isValid(){ return _bValid;}
  bool  isHidden(){return _bHidden;}
  bool  isNotHidden(){return !_bHidden;}
 
  void  setActive(bool bactive) { _bActive = bactive; }
  void  setPropagated(bool bprop) { _bPropagated = bprop; }
  void  setHiddenFlag(bool bhide){_bHidden=bhide;}
  void  hide(){_bHidden = true;}
  void  unhide(){_bHidden = false;}

  GRAPH_TRAVERSAL_STATE getTraversedState(){return _traverseState;}
  void setTraversedState(GRAPH_TRAVERSAL_STATE state){ _traverseState = state;}

  //useful functions
  bool isAContact(){return _label==CONTACT;}
  bool isANode(){return _graph_type==NODE;}
  bool isALink(){return _graph_type==LINK;}
  bool isASource(){return _type == SOURCE;}
  bool isASink(){return _type ==SINK;}
  bool isAJunction(){return _type==JUNCT;}
  bool isAnA3Source(){return _type==A3SOURCE;}

  SHOCKLABEL label() { return _label; }
  SHOCK_TYPE type() { return _type; }
  GRAPH_TYPE graph_type() { return _graph_type; }
  COMPLETION_TYPE comp_type() { return _ctype;}
  void setComp_type(COMPLETION_TYPE ctype){_ctype=ctype;}
  
  RADIUS_TYPE startTime() { return _startTime; }
  RADIUS_TYPE simTime() { return _simTime; }
  void setStartTime (RADIUS_TYPE stime)
  {
    _startTime = stime;
  }

  Point  origin() { return _origin; }
  void  setOrigin(Point origin) { _origin = origin; }
  
  SIElement (int newid, int neworder, 
          SHOCKLABEL newlabel, RADIUS_TYPE stime);
  virtual ~SIElement() {}

  virtual void getInfo (vcl_ostream& ostrm) {}

  // FOR SYMMETRY TRANSFORMS
  double  dPnCost() { return _dPnCost; }
  void    set_dPnCost (double pnCost) { _dPnCost = pnCost; }
  double  dOC() { return _dOC;}
  double  dNC() { return _dNC;}
  void    set_dNC(double newdNC){_dNC = newdNC;}

  virtual void computeSalience (void) {}

  // DYNAMICS DEFINITIONS
  virtual double r (TAU_TYPE tau=0);
  // rp and rpp are computed with respect to tau, not arclength. 
  // This is not the same as the rp in Giblin and Kimia's paper (2D intrinsic reconstruction...)
  virtual double rp (TAU_TYPE tau=0); 
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
};

//Imporvcl_tant!!
//For SINode: _origin is vstart
//For SILink: _origin is origin of the intrinsic coord

class SINode : public SIElement
{
protected:
  SILink* _cSLink;
  SILink* _cSLink2;

public:
  //vcl_list of boundary elements forming this node
  BElementList bndList;

  //vcl_list of parent shocks
  SILinksList PSElementList;
  
  SINode (int newid, int neworder, SHOCKLABEL newlabel, RADIUS_TYPE stime);
  virtual ~SINode ();

  int nBElement() { return bndList.size(); }
  int nPSElement() { return PSElementList.size();}

  void clear_pSLink(SILink* pslink);

  SILink* cSLink() { return _cSLink; }
  void set_cSLink(SILink* cslink) { _cSLink = cslink; }
  void clear_cSLink () { _cSLink = NULL; }

  SILink* cSLink2() { return _cSLink2; }
  void set_cSLink2(SILink* cslink2) { _cSLink2 = cslink2; }
  void clear_cSLink2 () { _cSLink2 = NULL; }

  //returns the number of edges starting at this node
  int outdeg(bool doNotincludehidden=true);
  //returns the number of edges ending at this node
  int indeg(bool doNotincludehidden=true); 
  //returns the number of edges adjacent to this node
  int degree(bool doNotincludehidden=true)
  { return outdeg(doNotincludehidden) + indeg(doNotincludehidden);}

  int countUnprunedParents (void);
  SILink* UnprunedParent (void);

  virtual void computeSalience (void) {}

  //VALIDATION
  virtual bool validate (Boundary *bnd);

  // return true if the node corresponds to curvature extrema on the boundary
  bool is_curvature_xtrema_node();

  // return the SILink connected to this node;
  SILinksList neighbor_links(bool include_hidden_links = false);
};

class SIA3Source : public  SINode 
{
public:
  SIA3Source (int newid, RADIUS_TYPE stime, Point Or,
          BElementList blist, SILinksList pselist);
   ~SIA3Source();

  virtual void computeSalience (void);

   // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
   virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SISource : public  SINode 
{
protected:
  bool _bValid;  //used to store shock sources that are not valid
            //For Dynamic validation Algorithms
public:

  void setbValid(bool bVal) { _bValid = bVal;}
  bool isValid() { return _bValid;}

  SISource (int newid, SHOCKLABEL newlabel, 
         RADIUS_TYPE stime, Point Or, BElement* belm1, BElement* belm2);
  ~SISource ();

  virtual BElement* getBElement1() 
  { 
    assert (bndList.size()==2);
    return bndList.front(); 
  }
   virtual BElement* getBElement2() 
  {
    assert (bndList.size()==2);
    return bndList.back(); 
  }

   virtual void getInfo (vcl_ostream& ostrm);
  virtual void computeSalience (void);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd, bool bPropagationCompleted=true);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SISink : public  SINode 
{
public:

  SISink (int newid, RADIUS_TYPE stime, Point Or,
        BElementList blist, SILinksList pselist );
  ~SISink ();

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  virtual void computeSalience (void);

   virtual void getInfo (vcl_ostream& ostrm);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SIJunct : public SINode
{
public:

  SIJunct (int newid, RADIUS_TYPE stime, Point Or,
        BElementList blist, SILinksList pselist);
  ~SIJunct ();

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  virtual void computeSalience (void);

  virtual void getInfo (vcl_ostream& ostrm);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};


class SILink : public SIElement
{
protected:
  int  _edgeID;        //ESF: commond ID of the shock edges after they are grouped
  BElement* _lBElement;  //ids of the left boundary elements
  BElement* _rBElement;  //ids of the right boundary elements

  SILink*  _lNeighbor;    //left neighboring shockwave
  SILink*  _rNeighbor;    //right neighboring shockwave

  SINode*  _pSNode;      //parent shockwave.
  SINode*  _cSNode;      //child shockwave.

  RADIUS_TYPE  _endTime;    //end time (distance)

  TAU_TYPE    _lsvector, _rsvector; //for debug

  TAU_TYPE    _LsTau;     //intrinsic parameter at the starting point using
                  //left boundary element's coordinate system
  TAU_TYPE    _LeTau;     //parameter at end point
  TAU_TYPE    _RsTau;     //intrinsic parameter at the starting point using
                  //right boundary element's coordinate system
  TAU_TYPE    _ReTau;     //parameter at end point
  TAU_TYPE    _EndVector; //EndVector of the current shock and also
                  //StartVector for the child shock computed from intersections.
  DIST_TYPE  _H;       //the H=2R, distance between 2 boundary elements

  TAU_TYPE _minLTau;
  TAU_TYPE _maxLTau;
  TAU_TYPE _minRTau;
  TAU_TYPE _maxRTau;

public:
  int  edgeID() { return _edgeID; }
  void  setEdgeID(int edgeid) { _edgeID = edgeid; }

  BElement* lBElement() { return _lBElement; }
  void set_lBElement(BElement* lbelm) { _lBElement = lbelm; }
  void clear_lBElement () { _lBElement = NULL; }

  BElement* rBElement() { return _rBElement; }
  void set_rBElement(BElement* rbelm) { _rBElement = rbelm; }
  void clear_rBElement () { _rBElement = NULL; }

  SILink* lNeighbor() { return _lNeighbor; }
  void set_lNeighbor(SILink* lneighbor) { _lNeighbor = lneighbor; }
  virtual void clear_lNeighbor () { _lNeighbor = NULL; }

  SILink* rNeighbor() { return _rNeighbor; }
  void set_rNeighbor(SILink* rneighbor) { _rNeighbor = rneighbor; }
  virtual void clear_rNeighbor () { _rNeighbor = NULL; }

  SINode* pSNode() { return _pSNode; }
  void set_pSNode(SINode* psnode) { _pSNode = psnode; }
  void clear_pSNode () { _pSNode = NULL; }

  SINode* cSNode() { return _cSNode; }
  void set_cSNode(SINode* csnode) { _cSNode = csnode; }
  void clear_cSNode () { _cSNode = NULL; }

  RADIUS_TYPE endTime() { return _endTime; }
  void setEndTime(RADIUS_TYPE time) {_endTime = time;}

  virtual void setSimTime (RADIUS_TYPE stime);

  TAU_TYPE LsTau() {
    return _LsTau;
  }
  void setLsTau(TAU_TYPE lstau) {
    _LsTau = lstau;
  }
  TAU_TYPE LeTau() {
    return _LeTau;
  }
  void setLeTau(TAU_TYPE letau) {
    _LeTau = letau;
  }
  TAU_TYPE RsTau() {
    return _RsTau;
  }
  void setRsTau(TAU_TYPE rstau) {
    _RsTau = rstau;
  }
  TAU_TYPE ReTau() {
    return _ReTau;
  }
  void setReTau(TAU_TYPE retau) {
    _ReTau = retau;
  }
  virtual TAU_TYPE EndVector() {
    return _EndVector;
  }
  void setEndVector(TAU_TYPE endvector) {
    _EndVector = endvector;
  }
  DIST_TYPE H() { return _H; }
  void setH(DIST_TYPE h) { _H = h; }

  virtual VECTOR_TYPE u(){return 0;}

  TAU_TYPE minLTau() {
    return _minLTau;
  }
  TAU_TYPE maxLTau() {
    return _maxLTau;
  }
  TAU_TYPE minRTau() {
    return _minRTau;
  }
  TAU_TYPE maxRTau() {
    return _maxRTau;
  }

  virtual bool isLSTauValid (TAU_TYPE lstau) { return false; } //base function
  virtual bool isRSTauValid (TAU_TYPE rstau) { return false; } //base function
  bool isTauValid_MinMax (TAU_TYPE ltau, TAU_TYPE rtau)
  {
    return AisLEq(_minLTau,ltau) && AisLEq(ltau,_maxLTau) &&
         AisLEq(_minRTau,rtau) && AisLEq(rtau,_maxRTau);
  }

  void correctLTau (TAU_TYPE ltau);
  void correctRTau (TAU_TYPE rtau);

  SILink (int newid, int neworder, SHOCKLABEL newlabel, RADIUS_TYPE stime,
           BElement* lbe, BElement* rbe, SINode* pse);
   ~SILink ();

  virtual TAU_TYPE sTau () { return _LsTau; }
  virtual TAU_TYPE eTau () { return _LeTau; }
  //returns whether the tau paramter is increvcl_asing or decrevcl_asing
  //used in conjunction with sTau() and eTau() functions
  //so check them for the appropriate convention
  virtual TAU_DIRECTION_TYPE tauDir(){return TAU_INCREASING;}
  
  virtual TAU_TYPE RTau(TAU_TYPE ltau) { return ltau; }
  virtual TAU_TYPE LTau(TAU_TYPE rtau) { return rtau; }

  //virtual TAU_TYPE getLTauFromTime (RADIUS_TYPE time) { return 0; }
  //virtual TAU_TYPE getRTauFromTime (RADIUS_TYPE time) { return 0; }

  virtual Point getPtFromLTau (TAU_TYPE ltau) { return _origin; }
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return _origin; }
  virtual Point getPtFromTau (TAU_TYPE tau)   { return _origin; }

  virtual Point getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual Point getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual Point getMidPt (void)   { return getPtFromLTau((_LsTau+_LeTau)/2); }

  virtual Point getLFootPt (double tau=0)   { return _origin; }
   virtual Point getRFootPt (double tau=0)   { return _origin; }

  virtual JUNCTION_TYPE getJunctionType (void);

  virtual void computeSalience (void) {}

  //VALIDATION
  virtual bool validate (Boundary *bnd, bool bPropagationCompleted=true);

  //PRUNING
  //bool isCornerShock ();
  SILink* GetChildLink (); //Return child link only if it is a PrunedJunction
  SILink* GetParentLink (); //Return parent link only if it is a PrunedJunction

  //Graph related
  bool isSemiInfinite(){return _cSNode==NULL;}
};  

class SIEdge : public SIElement
{
protected:

  BContour* _lContour;    //ids of the left boundary elements
  BContour* _rContour;    //ids of the right boundary elements

  //this structure allows for a directed graph
  //but functions are also included for undirected graph functions

  SINode*  _pSNode;      //parent node.
  SINode*  _cSNode;      //child node.

  SILinksList _links;

  double  _endTime;    //end time (distance)

public:
  BContour* lContour() { return _lContour; }
  void set_lContour(BContour* lbcon) { _lContour = lbcon; }
  void clear_lContour () { _lContour = NULL; }

  BContour* rContour() { return _rContour; }
  void set_rContour(BContour* rbcon) { _rContour = rbcon; }
  void clear_rContour () { _rContour = NULL; }

  SINode* pSNode() { return _pSNode; }
  void set_pSNode(SINode* psnode) { _pSNode = psnode; }
  void clear_pSNode () { _pSNode = NULL; }

  SINode* cSNode() { return _cSNode; }
  void set_cSNode(SINode* csnode) { _cSNode = csnode; }
  void clear_cSNode () { _cSNode = NULL; }

  SILinksList* links(){return &_links;}

  SILink* startLink() {return _links.front();}
  SILink* lastLink() { return _links.back();}

  SIEdge (int newid, SHOCKLABEL newlabel,
           BContour* lbcon, BContour* rbcon, 
        SINode* psnode, SINode* csnode, SILinksList slinks);
   ~SIEdge ();

  virtual void computeSalience (void) {}
};  

class SIContact : public SILink
{
protected:
  VECTOR_TYPE  _n;  //angle of contact shock with X-axis (extrinsic info)
  int    _nu; //0: LeftPoint, 1:RightPoint
  COLINEAR_CONTACT _ColinearContactType;

public:
  SIContact* _neighbor;
  VECTOR_TYPE n() { return _n; }
  int  nu() { return _nu; }

  COLINEAR_CONTACT ColinearContactType() 
  { return _ColinearContactType; }

  void setColinearContactType (COLINEAR_CONTACT newtype) {
    _ColinearContactType = newtype;
  }

  SIContact (int newid, BElement* lbe, BElement* rbe) :
    SILink (newid, 1, CONTACT, 0, lbe, rbe, NULL) {
    _ColinearContactType = ISOLATED_CONTACT;
    _neighbor = NULL;
    _bHidden = true; //contacts are not real shocks
  }
   ~SIContact () 
  { 
    if (_ColinearContactType == LEFT_COLINEAR_CONTACT || 
       _ColinearContactType == RIGHT_COLINEAR_CONTACT ){
      //delete the link from the right collinear contact
      Neighbor()->setNeighbor(NULL);
      //it's no longer a collinear contact so change its property
      Neighbor()->setColinearContactType(ISOLATED_CONTACT);
    }
  }

  SIContact* Neighbor(){return _neighbor;}
  void setNeighbor(SIContact* nelm){_neighbor = nelm;}

  //overwite this function because clear neighbor is a special
  //function for contacts. 
  virtual void clear_rNeighbor (); 
  virtual void clear_lNeighbor (); 

  virtual TAU_TYPE EndVector()
  {
    if (_ColinearContactType == ISOLATED_CONTACT){
      return _EndVector;
    }
    else if (_ColinearContactType == LEFT_COLINEAR_CONTACT){
      return _LsTau; //this is set during init
    }
    else if (_ColinearContactType == RIGHT_COLINEAR_CONTACT){
      return _RsTau; //this is set during init
    }
    else {
       assert(0);
      return 0; //should not ever happen
    }
  }

  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SIPointPoint : public  SILink 
{
private:
   VECTOR_TYPE _u;  //angle of unit vector
   VECTOR_TYPE _n;  //angle of tangent with X-axis
               //clockwise in C++ graphics
               //counterclockwise in Matlab graphics
public:
   virtual VECTOR_TYPE u() { return _u; }
  void setU(VECTOR_TYPE u) { _u = u; }
   VECTOR_TYPE n() { return _n; }
  void setN (VECTOR_TYPE n) { _n = n; }

  BPoint* lBPoint() { return (BPoint*)_lBElement; }
  BPoint* rBPoint() { return (BPoint*)_rBElement; }

  SIPointPoint (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SIPointPoint () {}

  DIST_TYPE d (TAU_TYPE ltau) { return r(ltau); }
  virtual TAU_TYPE RTau(TAU_TYPE ltau) { return 2*M_PI - ltau; }
  virtual TAU_TYPE LTau(TAU_TYPE rtau) { return 2*M_PI - rtau; }
  virtual bool isLSTauValid () {
    bool result = AisGEq(_LsTau,0) && AisLEq(_LsTau,M_PI_2);
    //assert (result);
    return result;
  }
  virtual bool isRSTauValid () {
    bool result = (AisGEq(_RsTau,3*M_PI_2) && AisGEq(_RsTau,M_PI_2));
    //assert (result);
    return result;
  }
  virtual TAU_TYPE computeMinLTau() { return _LsTau; }
  virtual TAU_TYPE computeMaxLTau() { return (TAU_TYPE) M_PI_2; }
  virtual TAU_TYPE computeMinRTau() { return (TAU_TYPE) M_PI_2*3; }
  virtual TAU_TYPE computeMaxRTau() { return _RsTau; }

  virtual TAU_TYPE getLTauFromTime (RADIUS_TYPE time) { return vcl_acos(_H/(2*time)); }
  virtual TAU_TYPE getRTauFromTime (RADIUS_TYPE time) { return 2*M_PI - getLTauFromTime(time); }
  virtual TAU_TYPE getTauFromTime (RADIUS_TYPE time) { return getLTauFromTime(time); }
  virtual TAU_TYPE getMidTau (RADIUS_TYPE curtime) { return (sTau()+getTauFromTime(curtime))/2; }

   virtual Point getStartPt (void) { return getPtFromLTau (_LsTau); }
   virtual Point getEndPt (void)   { return getPtFromLTau (_LeTau); }
  virtual Point getMidPt (void)   
  { 
    if (_simTime > MAX_RADIUS)
      return getPtFromTau ((sTau()+getTauFromTime(MAX_RADIUS))/2); 
    else
      return getPtFromTau (getMidTau(_simTime)); 
  }

   virtual Point getPtFromLTau (TAU_TYPE ltau);
   virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau (LTau(rtau)); }
   virtual Point getPtFromTau (TAU_TYPE tau)   { return getPtFromLTau(tau); }
   virtual Point getLFootPt (TAU_TYPE tau=0)   { return ((BPoint*)_lBElement)->pt(); }
   virtual Point getRFootPt (TAU_TYPE tau=0)   { return ((BPoint*)_rBElement)->pt(); }
   virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return _vectorPoint (getStartPt(), _n, MAX_RADIUS);
    else 
      return getEndPt ();
  }

   virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void);

   // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
}; 

class SIPointLineContact : public SIContact {
public:
  BPoint*  lBPoint() { assert(_nu==0); return (BPoint*)_lBElement; }
  BLine*  rBLine()  { assert(_nu==0); return (BLine*) _rBElement; }
  BLine*  lBLine()  { assert(_nu==1); return (BLine*) _lBElement; }
  BPoint*  rBPoint() { assert(_nu==1); return (BPoint*)_rBElement; }

  SIPointLineContact (int newid, BElement* lbe, BElement* rbe);
  ~SIPointLineContact () {}

  virtual Point getStartPt (void) { return _origin; }
  virtual Point getEndPt (void)   { return _vectorPoint (_origin, _n, _endTime); }
  virtual Point getMidPt (void)
    { return _vectorPoint (_origin, _n,  vnl_math_min (_endTime, MAX_RADIUS)/2); }
  virtual Point getEndPtWithinRange (void) 
    { return _vectorPoint (_origin, _n,  vnl_math_min (_endTime,MAX_RADIUS)); }

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void) {}

  // DYNAMICS DEFINITIONS
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
};

class SIPointArcContact : public SIContact {
private:
   int _nud;  //Same definition as linked BArc->_nud
          //To center: -1, linked BArc CCW
          //Away from center: 1, linked BArc CW
   DIST_TYPE _R;  //radius of the arc for easy reference

public:
  int nud() { return _nud; }
  DIST_TYPE R() { return _R; }

  BPoint*  lBPoint() { assert(_nu==0); return (BPoint*)_lBElement; }
  BArc*    rBArc()   { assert(_nu==0); return (BArc*)  _rBElement; }
  BArc*    lBArc()   { assert(_nu==1); return (BArc*)  _lBElement; }
  BPoint*  rBPoint() { assert(_nu==1); return (BPoint*)_rBElement; }

  SIPointArcContact (int newid, BElement* lbe, BElement* rbe);
  ~SIPointArcContact () {}

  virtual Point getStartPt (void) { return _origin; }
  virtual Point getEndPt (void)   { return _vectorPoint (_origin, _n, _endTime); }
  virtual Point getMidPt (void)
    { return _vectorPoint (_origin, _n,  vnl_math_min (_endTime,MAX_RADIUS)/2); }
  virtual Point getEndPtWithinRange (void) 
    { return _vectorPoint (_origin, _n,  vnl_math_min (_endTime,MAX_RADIUS)); }

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void) {}

  // DYNAMICS DEFINITIONS
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
};

class SIPointLine : public  SILink 
{
private:
  VECTOR_TYPE _n;  //angle of unit vcl_vector in the direction AB
  VECTOR_TYPE _u;  //angle of unit vcl_vector normal to n (i.e. - PI/2)
  int _nu;    // +1 if shock is in the direction of n, else -1.
  DIST_TYPE _delta;  //delta = distance from A to M (foot)
  DIST_TYPE _l;  //length of the line

public:
  VECTOR_TYPE n() { return _n; }
  virtual VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  DIST_TYPE delta() { return _delta; }
  DIST_TYPE l() { return _l; }

  BPoint*  lBPoint() { assert(_nu==1);  return (BPoint*)_lBElement; }
  BLine*  rBLine()  { assert(_nu==1);  return (BLine*) _rBElement; }
  BLine*  lBLine()  { assert(_nu==-1); return (BLine*) _lBElement; }
  BPoint*  rBPoint() { assert(_nu==-1); return (BPoint*)_rBElement; }

  SIPointLine (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
                LTAU_TYPE lsvector, LTAU_TYPE rsvector);
  ~SIPointLine () {}

  DIST_TYPE d (TAU_TYPE ptau) { return r(ptau); }
  virtual LTAU_TYPE  RTau(LTAU_TYPE Ltau);
  virtual LTAU_TYPE  LTau(LTAU_TYPE Rtau);
  virtual bool isLSTauValid () {
    bool result;
    if (_nu==1)
      result = AisGEq(_LsTau,0) && AisLEq(_LsTau,M_PI);
    else
      result = LisGEq(_delta-_LsTau,0) && LisLEq(_delta-_LsTau,_l);
    //assert (result);
    return result;
  }
  virtual bool isRSTauValid () {
    bool result;
    if (_nu==1)
      result = LisGEq(_RsTau+_delta,0) && LisLEq(_RsTau+_delta,_l);
    else
      result = AisGEq(_RsTau,M_PI) && AisLEq(_RsTau,2*M_PI);
    //assert (result);
    return result;
  }
  virtual LTAU_TYPE  computeMinLTau() { return _LsTau; }
  virtual LTAU_TYPE  computeMaxLTau() { return _nu==1 ? LTau(computeMaxRTau()) : _delta; }
  virtual LTAU_TYPE  computeMinRTau() { return _nu==1 ? _RsTau : RTau(computeMaxLTau()); }
  virtual LTAU_TYPE  computeMaxRTau() { return _nu==1 ? _l-_delta : _RsTau; }

  //always use tau on the point side
  TAU_TYPE sTau() { return _nu==1 ? _LsTau : _RsTau; }
  TAU_TYPE eTau() { return _nu==1 ? _LeTau : _ReTau; }
  virtual TAU_DIRECTION_TYPE tauDir() {return _nu==1 ? TAU_INCREASING:TAU_DECREASING;}

  //for testing the solution of intersections
  bool isTauValid_MinMax (LTAU_TYPE letau, LTAU_TYPE retau);

  virtual TAU_TYPE getPointTauFromTime (RADIUS_TYPE time)
  { 
    TAU_TYPE ptau = vcl_acos (_H/time -1);
    return (_nu==1) ? ptau : 2*M_PI-ptau; 
  }
  virtual LTAU_TYPE getLTauFromTime (RADIUS_TYPE time)
  {
    TAU_TYPE ptau = getPointTauFromTime (time);
    return (_nu==1) ? ptau : LTau(ptau);
  }
  virtual LTAU_TYPE getRTauFromTime (RADIUS_TYPE time)
  {
    TAU_TYPE ptau = getPointTauFromTime (time);
    return (_nu==1) ? RTau(ptau) : ptau;
  }
  virtual Point getStartPt (void) { return getPtFromPointTau (sTau()); }
  virtual Point getEndPt (void)   { return getPtFromPointTau (eTau()); }
  virtual Point getMidPt (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromPointTau ((sTau()+getPointTauFromTime(MAX_RADIUS))/2);
    else
      return getPtFromPointTau ((sTau()+eTau())/2);
  }

  virtual Point getPtFromPointTau (TAU_TYPE ptau);
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromPointTau(tau); }
  virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromPointTau (getPointTauFromTime(MAX_RADIUS));
    else 
      return getEndPt ();
  }

  virtual Point getLFootPt (TAU_TYPE ptau);
  virtual Point getRFootPt (TAU_TYPE ptau);

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
}; 

class SIPointArc : public  SILink 
{
private:
  VECTOR_TYPE _u;  //angle of unit vcl_vector normal to n (i.e. - PI/2)
  int _nu;    //+1: LeftPoint, RightArc
          //-1: LeftArc, RightPoint
  int _s;     //sigma: +1:H>R  and -1:R>H
  int _nudl, _nudr; //nud of left and right arcs
  int _case;  //4 cases for PointArc

  double _a;
  double _b2; //b^2
  double _b;
  double _c;
  double _Asym;
  double _LAsym;
  double _RAsym;

  VECTOR_TYPE _Sl;
  VECTOR_TYPE _Sr;
  VECTOR_TYPE _El;
  VECTOR_TYPE _Er;

  VECTOR_TYPE _As;//barc->startAngle()
  VECTOR_TYPE _Ae;//barc->endAngle()

public:
  virtual VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  int s() { return _s; }
  int nudl() { return _nudl; }
  int nudr() { return _nudr; }

  double a(void) { return _a; }
  double b2() { return _b2; }
  double c() { return _c; }

  double Asym() { return _Asym; }

  BPoint*  lBPoint() { assert(_nu==1);  return (BPoint*)_lBElement; }
  BArc*    rBArc()   { assert(_nu==1);  return (BArc*)  _rBElement; }
  BArc*    lBArc()   { assert(_nu==-1); return (BArc*)  _lBElement; }
  BPoint*  rBPoint() { assert(_nu==-1); return (BPoint*)_rBElement; }  

  VECTOR_TYPE Sl() { return _Sl; }
  VECTOR_TYPE Sr() { return _Sr; }
  VECTOR_TYPE El() { return _El; }
  VECTOR_TYPE Er() { return _Er; }

  VECTOR_TYPE As() { return _As; }
  VECTOR_TYPE Ae() { return _Ae; }

  DIST_TYPE Rl() 
  { 
    assert(lBElement()); 
    if (_nu==1) //LeftPoint, RightArc
      return 0;
    else
      return lBArc()->R(); 
  }
  DIST_TYPE Rr()
  {
    assert (rBElement());
    if (_nu==1) //LeftPoint, RightArc
      return rBArc()->R();
    else
      return 0;
  }

  SIPointArc (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
           SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SIPointArc () {}

  DIST_TYPE d (TAU_TYPE ltau) { return dFromLTau(ltau); }
  virtual DIST_TYPE dFromLTau (TAU_TYPE Ltau);
  virtual DIST_TYPE dFromRTau (TAU_TYPE Rtau);

  virtual TAU_TYPE RTau(TAU_TYPE Ltau);
  virtual TAU_TYPE LTau(TAU_TYPE Rtau);

  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();

  virtual TAU_TYPE computeMinLTau ();
  virtual TAU_TYPE computeMaxLTau ();
  virtual TAU_TYPE computeMinRTau ();
  virtual TAU_TYPE computeMaxRTau ();

  virtual TAU_DIRECTION_TYPE tauDir()
  {
    if (_s==-1 && _nu==-1) return TAU_DECREASING;
    else return TAU_INCREASING;
  }

  virtual TAU_TYPE getLTauFromTime (RADIUS_TYPE time);
  virtual TAU_TYPE getRTauFromTime (RADIUS_TYPE time) { return RTau(getLTauFromTime(time)); }

  virtual Point getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual Point getEndPt (void) { return getPtFromLTau(_LeTau); }
  virtual Point getMidPt (void)
  {
    //The best solution is: estimate max_endTime as initial endTime!
    if (_s>0) {
      if (_endTime > MAX_RADIUS)
        return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
      else
        return getPtFromLTau ((_LsTau+_LeTau)/2);
    }
    else {
      return getPtFromLTau ((_LsTau+_LeTau)/2);
    }
  }

  virtual Point getPtFromLTau (TAU_TYPE ltau);
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau(LTau(rtau)); }
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromLTau(tau); }

  virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau (getLTauFromTime(MAX_RADIUS));
    else 
      return getEndPt ();
  }

  virtual Point getLFootPt (TAU_TYPE ltau);
  virtual Point getRFootPt (TAU_TYPE rtau);

  virtual void computeSalience (void);

  virtual void getInfo (vcl_ostream& ostrm);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau);

  //VALIDATION
  virtual bool validate (Boundary *bnd);

  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
}; 

class SILineLine : public  SILink 
{
private:
  Point _Al, _Bl, _Ar, _Br;
  DIST_TYPE _lL;    // length of the line segment
  DIST_TYPE _lR;

  VECTOR_TYPE _nl;  //angle of unit vcl_vector along left line
  VECTOR_TYPE _ul;  //angle of normal (nl - Pi/2)
  VECTOR_TYPE _nr;  //angle of unit vcl_vector along right line
  VECTOR_TYPE _ur;  //angle of normal (nr - Pi/2)
  double _sigma; // nl dot nr

  ANGLE_TYPE _thetaL; //angle between nr and ul
  ANGLE_TYPE _thetaR;
  ANGLE_TYPE _phi;
  DIST_TYPE _deltaL; //distance from Origin to Al
  DIST_TYPE _deltaR; //distance from Origin to Ar

  double _N1L, _N1R; // the slope of the shock with respect to the line
  double _N2L, _N2R; //parameters for the intercepts

public:
  VECTOR_TYPE nl() { return _nl; }
  VECTOR_TYPE ul() { return _ul; }
  VECTOR_TYPE nr() { return _nr; }
  VECTOR_TYPE ur() { return _ur; }
  double sigma() { return _sigma; }

  ANGLE_TYPE thetaL() { return _thetaL; }
  ANGLE_TYPE thetaR() { return _thetaR; }
  bool   _phiSpecialCase;
  double phi() { return _phi; }
  DIST_TYPE deltaL() { return _deltaL; }
  DIST_TYPE deltaR() { return _deltaR; }

  Point Al() { return _Al; }
  Point Bl() { return _Bl; }
  Point Ar() { return _Ar; }
  Point Br() { return _Br; }

  double N1L() { return _N1L; }
  double N1R() { return _N1R; }
  double N2L() { return _N2L; }
  double N2R() { return _N2R; }
    
  DIST_TYPE lL() { return _lL; }
  DIST_TYPE lR() { return _lR; }

  BLine*  lBLine() { return (BLine*)_lBElement; }
  BLine*  rBLine() { return (BLine*)_rBElement; }

  SILineLine (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SILineLine () {}

  virtual DIST_TYPE d (TAU_TYPE Ltau);
  virtual TAU_TYPE RTau(TAU_TYPE Ltau);
  virtual TAU_TYPE LTau(TAU_TYPE Rtau);
  virtual bool isLSTauValid () {
    bool result = LisGEq(_LsTau,0) && LisLEq(_LsTau,_lL);
    //assert (result);
    return result;
  }
  virtual bool isRSTauValid () {
    bool result = LisGEq(_RsTau,0) && LisLEq(_RsTau,_lR);
    //assert (result);
    return result;
  }
  virtual TAU_TYPE computeMinLTau() { return  vnl_math_max (0.0, LTau(_lR)); }
  virtual TAU_TYPE computeMaxLTau() { return  vnl_math_min (_lL, LTau(0)); }
  virtual TAU_TYPE computeMinRTau() { return  vnl_math_max (0.0, RTau(_lL)); }
  virtual TAU_TYPE computeMaxRTau() { return  vnl_math_min (_lR, RTau(0)); }

  bool isTauValid_MinMax (TAU_TYPE letau, TAU_TYPE retau);

  virtual TAU_DIRECTION_TYPE tauDir() {return TAU_DECREASING;}

  virtual TAU_TYPE getLTauFromTime (RADIUS_TYPE time) { return _deltaL - time/vcl_tan(_phi); }
  virtual TAU_TYPE getRTauFromTime (RADIUS_TYPE time) { return time/vcl_tan(_phi) + _deltaR; }

  virtual Point getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual Point getEndPt (void) { return getPtFromLTau(_LeTau); }
  virtual Point getMidPt (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
    else
      return getPtFromLTau ((_LsTau+_LeTau)/2);
  }

  virtual Point getPtFromLTau (TAU_TYPE ltau);
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau(LTau(rtau)); }
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromLTau(tau); }
  virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau (getLTauFromTime(MAX_RADIUS));
    else
      return getEndPt ();
  }

  virtual Point getLFootPt (TAU_TYPE tau) { return _vectorPoint (_Al, _nl, tau); }
  virtual Point getRFootPt (TAU_TYPE tau) 
  { 
    double rtau = RTau(tau);
    return _vectorPoint (_Ar, _nr, rtau); 
  }

  virtual void getInfo (vcl_ostream& ostrm);
  virtual void computeSalience (void);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
}; 

class SILineArc : public SILink 
{
protected:
  VECTOR_TYPE _n;  //no
  VECTOR_TYPE _u;  //uo = no - M_PI_2
  int _nu;    // +1/-1 just like parabola
  DIST_TYPE _delta;  //delta = distance from A to M (foot)
  DIST_TYPE _l;  //length of the line
  DIST_TYPE _R;  //R of the arc
  Point _foot; //projection of center of arc to line.
  int _case;  //8 cases for LineArc
  int _s;     //sigma = dot(u0,uL)
               // +1: center is in the same side as the arc
               // -1: center is in diff side as the arc
  int _nud;   // -1: caused by CCW Arc, sTau>0 decrevcl_asing
               // +1: caused by CW Arc, sTau=0 increvcl_asing
               //so nud=BArc->nud
  double _c;  //Parabola intrinsic parameter

  VECTOR_TYPE _CCWAs;  //arc->CCWStartAngle()
  VECTOR_TYPE _CCWAe; //arc->CCWEndAngle()
  VECTOR_TYPE _As;    //arc->startAngle()
  VECTOR_TYPE _Ae;    //arc->endAngle()

public:
  VECTOR_TYPE n() { return _n; }
  virtual VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  DIST_TYPE delta() { return _delta; }
  DIST_TYPE l() { return _l; }
  DIST_TYPE R() { return _R; }
  int s() { return _s; }
  int nud() { return _nud; }
  int snud() { //originally _s*_nud
    return _s*_nud;
  }
  double c() { return _c; }

  double CCWAs() { return _CCWAs; }
  double CCWAe() { return _CCWAe; }

  VECTOR_TYPE As() { return _As; }
  VECTOR_TYPE Ae() { return _Ae; }

  BArc*    lBArc()  { assert(_nu==1);  return (BArc*)  _lBElement; }
  BLine*  rBLine() { assert(_nu==1);  return (BLine*) _rBElement; }
  BLine*  lBLine() { assert(_nu==-1); return (BLine*) _lBElement; }
  BArc*    rBArc()  { assert(_nu==-1); return (BArc*)  _rBElement; }

  SILineArc (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
                LTAU_TYPE lsvector, LTAU_TYPE rsvector);
  ~SILineArc () {}

  virtual DIST_TYPE d (TAU_TYPE ptau);
  virtual LTAU_TYPE RTau(LTAU_TYPE Ltau);
  virtual LTAU_TYPE LTau(LTAU_TYPE Rtau);

  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();

  virtual LTAU_TYPE computeMinLTau ();
  virtual LTAU_TYPE computeMaxLTau ();
  virtual LTAU_TYPE computeMinRTau ();
  virtual LTAU_TYPE computeMaxRTau ();

  //for testing the solution of intersections
  bool isTauValid_MinMax (LTAU_TYPE letau, LTAU_TYPE retau);
  bool isTauValid_LA (LTAU_TYPE ltau, LTAU_TYPE rtau);

  //always use tau on the point side
  virtual TAU_TYPE sTau() { return _nu==1 ? _LsTau : _RsTau; }
  virtual TAU_TYPE eTau() { return _nu==1 ? _LeTau : _ReTau; }
  virtual TAU_DIRECTION_TYPE tauDir()
  {
    if (_nu==1){
      if(_nud==1) return TAU_INCREASING;
      else      return TAU_DECREASING;
    }
    else {
      if(_nud==1) return TAU_DECREASING;
      else      return TAU_INCREASING;
    }
  }

  virtual TAU_TYPE getPointTauFromTime (RADIUS_TYPE time);

  virtual Point getPtFromPointTau (TAU_TYPE ptau);
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromPointTau(tau); }

  virtual Point getStartPt (void) { return getPtFromTau (sTau()); }
  virtual Point getEndPt (void)   { return getPtFromTau (eTau()); }
  virtual Point getMidPt (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromPointTau ((sTau()+getPointTauFromTime(MAX_RADIUS))/2);
    else
      return getPtFromPointTau ((sTau()+eTau())/2);
  }

  virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromPointTau (getPointTauFromTime(MAX_RADIUS));
    else 
      return getEndPt ();
  }

  virtual Point getLFootPt (TAU_TYPE ptau);
  virtual Point getRFootPt (TAU_TYPE ptau);

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SIArcArc : public SILink {
private:
  VECTOR_TYPE _u;  //angle of unit vcl_vector from left Arc to Right arc
  int _nu;    //+1: LeftSmallerR
          //-1: RightSmallerR
  int _s;     //sigma: +1:H>(R1+R2), -1:H<|R1-R2| and
               //ELSE +2: if hyperbola and -2 if ellipse

  int _case;  //12 cases for ArcArc
  int _MU;
  int _hmu, _emu;  //Inner hyperbola and ellipse
  int _nudl, _nudr; //nud of left and right arcs
  DIST_TYPE _Rl, _Rr; //radii of the arcs for easy access

  double _a;
  double _b2; //b^2
  double _b;
  double _c;

  double _Asym, _LAsym, _RAsym;

  VECTOR_TYPE _Sl;
  VECTOR_TYPE _El;
  VECTOR_TYPE _Sr;
  VECTOR_TYPE _Er;

  VECTOR_TYPE _AsL;//Lbarc->startAngle()
  VECTOR_TYPE _AeL;//Lbarc->endAngle()
  VECTOR_TYPE _AsR;//Rbarc->startAngle()
  VECTOR_TYPE _AeR;//Rbarc->endAngle()

public:
  virtual VECTOR_TYPE u() { return _u; }
  //See notes: u for drawing, only different in s=-1&nu=1 case.
  VECTOR_TYPE ud() { return (_s==-1&&_nu==1) ? (_u+M_PI) : _u; }
  int nu() { return _nu; }
  int s() { return _s; }
  int MU() { return _MU; }
  int hmu() { return _hmu; }
  int emu() { return _emu; }
  int nudl() { return _nudl; }
  int nudr() { return _nudr; }
  DIST_TYPE Rl() { return _Rl; }
  DIST_TYPE Rr() { return _Rr; }
  int Case() { return _case; }

  double a() { return _a; }
  double b2() { return _b2; }
  double c() { return _c; }

  VECTOR_TYPE Sl() { return _Sl; }
  VECTOR_TYPE Sr() { return _Sr; }
  VECTOR_TYPE El() { return _El; }
  VECTOR_TYPE Er() { return _Er; }

  VECTOR_TYPE AsL() { return _AsL; }
  VECTOR_TYPE AeL() { return _AeL; }
  VECTOR_TYPE AsR() { return _AsR; }
  VECTOR_TYPE AeR() { return _AeR; }

  DIST_TYPE d (TAU_TYPE ltau) //distance from the left origin
  {
    if (_s>0) return _b2/(_c*vcl_cos(ltau) - _a);
    else      return _b2/(_a - _c*vcl_cos(ltau));
  }

  BArc*  lBArc() { return (BArc*) _lBElement; }
  BArc*  rBArc() { return (BArc*) _rBElement; }

  SIArcArc (int newid, RADIUS_TYPE stime, 
        BElement* lbe, BElement* rbe,
        SINode* pse, 
        TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SIArcArc () {}

  //double d (double ltau) { return dFromLTau(ltau); }
  virtual DIST_TYPE dFromLTau (TAU_TYPE Ltau);
  virtual DIST_TYPE dFromRTau (TAU_TYPE Rtau);
  virtual TAU_TYPE RTau(TAU_TYPE Ltau);
  virtual TAU_TYPE LTau(TAU_TYPE Rtau);

  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();

  virtual TAU_TYPE computeMinLTau ();
  virtual TAU_TYPE computeMaxLTau ();
  virtual TAU_TYPE computeMinRTau ();
  virtual TAU_TYPE computeMaxRTau ();

  virtual TAU_DIRECTION_TYPE tauDir()
  {
    //always uses the left tau
    switch (_case){
      case 1: return TAU_INCREASING;
      case 2: return TAU_INCREASING;
      case 3: return TAU_INCREASING;
      case 4: return TAU_DECREASING;
      case 5: return TAU_INCREASING;
      case 6: return TAU_DECREASING;
      case 7: return TAU_INCREASING;
      case 8: return TAU_DECREASING;
      case 9: return TAU_INCREASING;
      case 10:return TAU_DECREASING;
      case 11:return TAU_DECREASING;
      case 12:return TAU_INCREASING;
      default: assert (0); return TAU_INCREASING;
    }
  }

  //for testing the solution of intersections
  bool isTauValid_AA (TAU_TYPE letau, TAU_TYPE retau);

  virtual TAU_TYPE getLTauFromTime (RADIUS_TYPE time);
  virtual TAU_TYPE getRTauFromTime (RADIUS_TYPE time) { return RTau(getLTauFromTime(time)); }
      
  virtual Point getStartPt (void) {
    return _pSNode->origin();
    /////return getPtFromLTau(_LsTau); 
  }
  virtual Point getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual Point getMidPt (void)
  {
    //The best solution is: estimate max_endTime as initial endTime!
    if (_s>0) {
      if (_endTime > MAX_RADIUS)
        return getPtFromLTau ((_LsTau+getLTauFromTime(MAX_RADIUS))/2);
      else
        return getPtFromLTau ((_LsTau+_LeTau)/2);
    }
    else {
      assert (_LeTau>=0);
      return getPtFromLTau ((_LsTau+_LeTau)/2);
    }
  }
  virtual Point getPtFromLTau (TAU_TYPE ltau);
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau (LTau(rtau)); }
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromLTau(tau); }

  virtual Point getEndPtWithinRange (void)
  {
    if (_endTime > MAX_RADIUS)
      return getPtFromLTau (getLTauFromTime(MAX_RADIUS));
    else 
      return getEndPt ();
  }

  virtual Point getLFootPt (TAU_TYPE ltau);
  virtual Point getRFootPt (TAU_TYPE rtau);

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void);

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau);


  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

class SIThirdOrder : public  SILink {
protected:
  VECTOR_TYPE _nl;  //angle of unit vcl_vector along left line
  VECTOR_TYPE _ul;  //nl - Pi/2
  DIST_TYPE  _lL;  // length of the line segment
  DIST_TYPE  _lR;
  Point _Al, _Bl, _Ar, _Br;

public:
  VECTOR_TYPE nl() { return _nl; }
  VECTOR_TYPE ul() { return _ul; }
  DIST_TYPE lL() { return _lL; }
  DIST_TYPE lR() { return _lR; }
  Point Al() { return _Al; }
  Point Bl() { return _Bl; }
  Point Ar() { return _Ar; }
  Point Br() { return _Br; }
  DIST_TYPE d() { return _startTime; }

  SIThirdOrder (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SIThirdOrder () {}

  TAU_TYPE RTau (TAU_TYPE ltau) { return (_LsTau-_LeTau) + _LeTau+_RsTau - ltau; } //length = LsTau-LeTau
  TAU_TYPE LTau (TAU_TYPE rtau) { return (_ReTau-_RsTau) + _LeTau+_RsTau - rtau; } //length = ReTau-RsTau

  virtual Point getStartPt (void) { return getPtFromLTau (_LsTau); }
  virtual Point getEndPt (void) { return getPtFromLTau (_LeTau); }
  virtual Point getMidPt (void)
  {
    return getPtFromLTau ((_LsTau+_LeTau)/2);
  }

  virtual Point getEndPtWithinRange (void)
  {
    return getEndPt ();
  }

  virtual Point getPtFromLTau (TAU_TYPE ltau);
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau (LTau(rtau)); }
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromLTau(tau); }

  virtual TAU_DIRECTION_TYPE tauDir(){ return TAU_DECREASING;}

  virtual Point getLFootPt (TAU_TYPE tau) { return _vectorPoint (_Al, _nl, tau); }
  virtual Point getRFootPt (TAU_TYPE tau) { return _vectorPoint (_Ar, _nl+M_PI, RTau(tau)); }

  virtual void getInfo (vcl_ostream& ostrm);
  virtual void computeSalience (void) {_dPnCost=ISHOCK_DIST_HUGE;}

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
}; 

class SIArcThirdOrder : public  SILink
{
protected:
  VECTOR_TYPE _ul, _ur;  //start angle for LsTau and RsTau
  int _nu;    //1:LPoint 2:RPoint 3:ArcArc
  int _nudl, _nudr; //nud of left and right arcs
  DIST_TYPE _Rl, _Rr;
  int _case;

  //for debug only
  TAU_TYPE _lsvector, _rsvector;

public:
  VECTOR_TYPE ul() { return _ul; }
  VECTOR_TYPE ur() { return _ur; }
  int nu() { return _nu; }
  int nudl() { return _nudl; }
  int nudr() { return _nudr; }
  DIST_TYPE Rl() { return _Rl; }
  DIST_TYPE Rr() { return _Rr; }
  DIST_TYPE d() { return _Rl; }
  
  BPoint*  lBPoint() { assert(_nu==1); return (BPoint*) _lBElement; }
  BArc*    rBArc()   { assert(_nu==3); return (BArc*)   _rBElement; }
  BArc*    lBArc()   { assert(_nu==3); return (BArc*)   _lBElement; }
  BPoint*  rBPoint() { assert(_nu==2); return (BPoint*) _rBElement; }

  SIArcThirdOrder (int newid, RADIUS_TYPE stime, 
                BElement* lbe, BElement* rbe,
                SINode* pse,
           TAU_TYPE lsvector, TAU_TYPE rsvector);
  ~SIArcThirdOrder () {}

  virtual TAU_TYPE RTau(TAU_TYPE ltau) { return _LsTau+_RsTau - ltau; }
  virtual TAU_TYPE LTau(TAU_TYPE rtau) { return _LsTau+_RsTau - rtau; }
  //double  maxLTau();
  //double  maxRTau();
  //double  minLTau();
  //double  minRTau();

  virtual Point getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual Point getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual Point getMidPt (void)
  {
    return getPtFromLTau ((_LsTau+_LeTau)/2);
  }

  virtual Point getEndPtWithinRange (void)
  {
    return getEndPt ();
  }

  virtual Point getPtFromLTau (TAU_TYPE ltau);
  virtual Point getPtFromRTau (TAU_TYPE rtau) { return getPtFromLTau (LTau(rtau));}
  virtual Point getPtFromTau (TAU_TYPE tau) { return getPtFromLTau(tau); }

  virtual TAU_DIRECTION_TYPE tauDir(){ return TAU_DECREASING;}

  virtual Point getLFootPt (TAU_TYPE ltau);
  virtual Point getRFootPt (TAU_TYPE rtau);

  virtual void getInfo (vcl_ostream& ostrm);

  virtual void computeSalience (void) { _dPnCost = ISHOCK_DIST_HUGE; }

  // DYNAMICS DEFINITIONS
  virtual double r  (TAU_TYPE tau=0); 
  virtual double rp (TAU_TYPE tau=0);
  virtual double rpp(TAU_TYPE tau=0);
  virtual double tangent (TAU_TYPE tau=0);
  virtual double g  (TAU_TYPE tau=0);
  virtual double k  (TAU_TYPE tau=0);
  virtual double v  (TAU_TYPE tau=0);
  virtual double a  (TAU_TYPE tau=0);

  //VALIDATION
  virtual bool validate (Boundary *bnd);
  
  //for fast drawing purposes :REQ. FOR TRANSITION TO VXL
  virtual void compute_extrinsic_locus();
};

#include <vgl/vgl_polygon.h>

//Visual Fragment class
class VisFrag: public BaseGUIGeometry
{
public:
  typedef vcl_vector<Point > Polygon;
protected:
  Polygon _frag_poly;
  SIEdge* _shock_edge;
  double _avg_intensity;
  double _std_dev;

  int _id;
public:
  VisFrag(int new_id, SIEdge* new_edge):BaseGUIGeometry(), 
   _shock_edge(new_edge), _avg_intensity(0.0), _id(new_id)
  {}
  ~VisFrag(){}

  double average_intensity(){return _avg_intensity;}
  void set_average_intensity(double avg_i){_avg_intensity = avg_i;}
  double std_dev(){return _std_dev;}
  void set_std_dev(double stddev){_std_dev = stddev;}
  Polygon* poly(){return &_frag_poly;}
  SIEdge* shock_edge(){return _shock_edge;}
  int id(){ return _id;}

  void push_back(Point newpt){_frag_poly.push_back(newpt);}

  virtual void getInfo (vcl_ostream& ostrm);
};

#endif
