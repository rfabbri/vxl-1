#ifndef _ISHOCK_H
#define _ISHOCK_H

/////////////////////////////////////////////////////
// INTRINSIC SHOCK
/////////////////////////////////////////////////////

#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_math.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_map.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_distance.h>

#include "ishock-common.h"
#include "boundary.h"
#include "sielements.h"
#include "delaunay.h"
#include "quad.h"

typedef enum {
  BOGUS_ALGO_TYPE,
  DEFAULT_ALGO = 1,
  LAGRANGIAN=2,
  DYN_VAL=3,
  DYN_VAL_NEW=4,
  PROPAGATION=5,
} SHOCK_ALGO_TYPE;

typedef enum {
  BOGUS_INIT,
  DEFAULT_INIT = 1,
  LAGRANGIAN_INIT = 2,          //Regular O(n^3) initialization.
  DT_INIT_POINTS = 3,          //Initialize new sources usingDT (points only).
  BRUTE_FORCE_ADD_PATCH_INIT = 4,  //Initialize new sources after adding from everything else.
  REGULAR_ADD_PATCH_INIT = 5,    //Initialize new sources after adding from local neighborhood.
  DELETE_PATCH_INIT = 6,        //Initialize new sources after delete from local neighborhood.
  BUCKETING_INIT = 7,          //Initialize new sources usingBucketing.
  BUCKETING_ADD_INIT = 8,        //Initialize new sources after adding usingBucketing.
} SHOCK_INIT_TYPE;

//PROPAGATION AND INTERSECTIONS TYPES
typedef enum {
  BOGUS_PROPAGATION_TYPE=-1,
  NO_PROPAGATION=0,
  PROPAGATION_DONE,
  PROPAGATION_TO_INFINITY,

  INVALID_CANDIDATE_SOURCE,
  A3_FORMATION,
  REGULAR_JUNCT,
  DEGENERATE_JUNCT,
  SINK_FORMATION,

  NEW_SHOCK_FROM_A3,
  NEW_BRANCHES_FROM_SOURCE,
  NEW_SHOCK_FROM_JUNCT,
  INVALID_JUNCT_REMOVED,

  LEFT_INTERSECTION,
  RIGHT_INTERSECTION,
  BOTH_INTERSECTION,
  THIRD_ORDER_FORMATION,
  ARC_THIRD_ORDER_FORMATION,

  PROPAGATION_ERROR_DETECTED,
} PROPAGATION_TYPE;

typedef enum {
  BOGUS_PRUNE_TYPE,
  PRUNE_REGULAR = 0,
  PRUNE_PRESERVE_CORNER = 1,
} PRUNE_ALGORITHM;

// SHOCK INITILIZATION
#define ISHOCK_INITSHOCKS_INVALID_DISTANCE  MAX_RADIUS+4
#define ISHOCK_INITSHOCKS_BYPASS          -1
#define ISHOCK_INITSHOCKS_INIT_CONTACT      -2

typedef enum {
  BOGUS_INTERSECTION_CONDITION,
  THETA_0      = 1,
  RHO_COS_THETA  = 2,
  THETA_PI      = 3,
  NORMAL      = 4,
} INTERSECTION_CONDITION;

////////////////////////////////////////////////
// MISC Shock detection definition...
typedef enum {
   BOGUS_DIRECTION,
   LEFT, RIGHT,
  BOTH_DIRECTION
} DIRECTION;


//#define GUI_SHOW 1
//#define GUI_HIDE 0
#define GUI_RANGE_OF_INFLUENCE 2

//class used for storing the results of intrinsic intersection
//computation between two shock paths
class IntrinsicIntersection
{
public:
  RADIUS_TYPE    R;       //radius or time at intersection
  TAU_TYPE      LSLtau;  //Left shock Left tau
  TAU_TYPE      LSRtau;
  TAU_TYPE      RSLtau;
  TAU_TYPE      RSRtau;

  TAU_TYPE      newLtau; //parameter for child shock
  TAU_TYPE      newRtau; //parameter for child shock

  double angle;   //just for point line contacts at a junction

  IntrinsicIntersection()
  {
    R=ISHOCK_DIST_HUGE; LSLtau=0; LSRtau=0; RSLtau=0; RSRtau=0;
    newLtau=0; newRtau=0;
    angle=ISHOCK_DIST_HUGE;
  }

  ~IntrinsicIntersection() {}
};

// 11/25/2002: Amir
// I decided that since the problem is arising because of the fact that
// in the ordering process active shocks are going before inactive ones
// we should also use the active, inactive property in the shock vcl_list

//typedefs for SIElmList
typedef vcl_pair<RADIUS_TYPE, int> R_IDpair;//Radius=simTime and ID vcl_pair
typedef vcl_pair<bool, R_IDpair> A_R_IDpair; //active/inactive and R_ID vcl_pair
typedef vcl_pair<A_R_IDpair, SIElement*> SIElmPair;
typedef vcl_map<A_R_IDpair, SIElement*>::iterator SIElmListIterator;
typedef vcl_map<A_R_IDpair, SIElement*>::reverse_iterator R_SIElmIterator;

typedef vcl_pair<int, SIElement*> ID_SIElm_pair;

// typedefs for saliency vcl_list
typedef vcl_pair<double, int> Sal_IDpair;
typedef vcl_pair<Sal_IDpair, SIElement*> Sal_SIElmPair;
typedef vcl_map<Sal_IDpair, SIElement*>::iterator GDListIterator;

typedef vcl_map<int, SIEdge*> SIEdgeList;
typedef vcl_map<int, SIEdge*>::iterator SIEdgeListIterator;

typedef vcl_list<SINode*> SINodeList;
typedef vcl_list<SINode*>::iterator SINodeListIterator;

typedef vcl_list<VisFrag*> VisFragList;
typedef vcl_list<VisFrag*>::iterator VisFragListIterator;

class IShock : public vbl_ref_count
{
public:
  bool _bIsRealIShock;
  IShock (Boundary* bnd, SHOCK_ALGO_TYPE newAlgoType = LAGRANGIAN, bool isReal = true);
  virtual ~IShock ();
  SIElement* _nextShockToPropagate;

protected:
  int _nextAvailableID;  //keeps the next available id, always increases
  int nextAvailableID() { _nextAvailableID++; return _nextAvailableID; }
  long _nSourceElement;

  SHOCK_INIT_TYPE _ShockInitType;  //Shock Initialization Type
  SHOCK_ALGO_TYPE _ShockAlgoType;  //Shock Propagation Algorithm
  bool _bComputeShockLocally;    //Shock Locality

  RADIUS_TYPE     dCurrentTime;

public:
  Boundary* _boundary;  //Pointer to associated boundary
  Boundary* boundary() { return _boundary; }

  vcl_map<int, SIElement*> update_list; //vcl_list of elements that need to be redrawn

  // vcl_map data structure to contain all the shock elements ordered by time
  vcl_map<A_R_IDpair, SIElement*> SIElmList;

  //vcl_map data structure to keep track of the saliencies for Symmetry transforms
  vcl_map<Sal_IDpair, SIElement*> GDList;

  //This is the structure that maintains the pruned graph structure.
  //it is a vcl_list of all the shockgraph edges. The node information is
  //contained within it. The nodes themselves are the same as the original
  //nodes for now. But new ones need to be created with the proper
  //adjacency information in order for it to be a complete graph structure!

  SIEdgeList SIEdgesList;

  //this is the structure for storing the visual fragments
  //currently as polygons
  VisFragList VisFragsList;

  SHOCK_INIT_TYPE getShockInitType() {
    return _ShockInitType;
  }
  void setShockInitType (SHOCK_INIT_TYPE sit) {
    _ShockInitType = sit;
  }
  SHOCK_INIT_TYPE getShockInitTypeAdding(SHOCK_INIT_TYPE inittype) {
    switch (inittype) {
    case LAGRANGIAN_INIT:  return BRUTE_FORCE_ADD_PATCH_INIT;
    case DT_INIT_POINTS:    return BRUTE_FORCE_ADD_PATCH_INIT;
    case BUCKETING_INIT:    return BUCKETING_ADD_INIT;
    default: return BOGUS_INIT; //should not happen
    }
  }

  SHOCK_ALGO_TYPE GetShockAlgorithm() { return _ShockAlgoType; }
  void SetShockAlgorithm (SHOCK_ALGO_TYPE newalgo) { _ShockAlgoType = newalgo; }

  bool isComputeShockLocally() {
    return _bComputeShockLocally;
  }
  void setComputeShockLocally (bool blocal) {
    _bComputeShockLocally = blocal;
  }

  long nSourceElement() {
    return _nSourceElement;
  }
  long nSElement() {return SIElmList.size();}

  void Clear();

  //**** save the shock data in a file 
  void write_shock_file (const char* filename); //.ish file

  //**** SHOCK COMPUTATION FUNCTIONS ****
  //LAGRANGIAN 
  virtual bool DetectShocks (SHOCK_INIT_TYPE InitOption=DEFAULT_INIT, SHOCK_ALGO_TYPE Algorithm=DEFAULT_ALGO);
  long PatchShocksLocally (SHOCK_INIT_TYPE option=DEFAULT_INIT);
  virtual void InitializeShocks (SHOCK_INIT_TYPE InitOption=DEFAULT_INIT);

  void MergeFromIShock (IShock* ishock);

  void TestSTLProfiling (BElmListType* ListA, BElmListType* ListB, BElmListType* ListC);
  int _nDTVoronoiPoints;
  void InitializeShocksDTPoints (); //O(n*logn)

  void init_source_from_delaunay_edge (edge_ref e);
  void TraverseDelaunay(edge_ref e);
  void quad_do_enum (edge_ref e, unsigned mark);

  virtual bool PropagateShocks ();

  void PropagateShockJump (); //Fast Step
  PROPAGATION_TYPE  PropagateNextShock ();  

  //**** SHOCK VALIDATION ****
  bool ValidateBoundaryShockStructure (void);
    bool validateBElement (BElement* belm);
    bool validateSElement (SIElement* selm, bool bPropagated=true);
    bool ValidateShockList (void);
  bool ValidateShockInitilization (void);
  bool validateBElement (int id);
  bool validateSElement (int id);

  //temporary
  SISource* sourceExists(BElement* belm1, BElement* belm2);

  //public version of delASIElement //temp???
  void deleteAShock(SIElement* shock) {delASIElement(shock);}

public:
  SIElement::SHOCKLABEL determineShockLabel (BElement* elm1, BElement* elm2);

   void delTheSIElementList ();

   void addASIElement (SIElement* elm);
  void delASIElement (SIElement* elm);
  void updateASIElementSimTime(SILink* elm, RADIUS_TYPE R);
  virtual void moveASILinkToInfinity (SILink* elm);
  void moveASILinkToAJunction (IntrinsicIntersection intersection, SILink* elm, DIRECTION dir);
  virtual void moveSILinksToALeftJunction (SILink* lslink, SILink* current, IntrinsicIntersection inter);
  virtual void moveSILinksToARightJunction (SILink* current, SILink* rslink, IntrinsicIntersection inter);
  void deactivateASIElement (SIElement* elm);
  void reactivateASIElement (SIElement* elm);
  
  // For shock transform addition operator
  void initializeSourcesFromNewElement_BruteForce (BElement* newelm);
  void initializeSourcesFromNewElement (BElement* newelm);

  SIContact* contactExists(BElement* belm1, BElement* belm2);
  
  SIContact* initializeAContactShock(BElement* lbe, BElement* rbe, int option);
  SISource* initializeASource(BElement* elm1, BElement* elm2, Point start, RADIUS_TYPE time);

  bool doesBElementInvalidateSource (BElement* curBElm, BElement* elm1, BElement* elm2, 
                          Point midPoint, double midDistSq);
  bool doesBElementInvalidateSource2 (BElement* curBElm, BElement* elm1, BElement* elm2, 
                          Point midPoint, double midDistSq);
  //Validate Candidate Source usingall neighboring elements of the two BElms.
  virtual bool ValidateCandidateSource_TwoBElmNeighbors (BElement* belm1, BElement* belm2, 
                                       Point midPoint, double midDist);

  void checkAndIntersectColinearContacts (SISource* newSource, BElement* belm1, BElement* belm2);
    void intersectSIContactWithPP (SIContact* cc, SIPointPoint* spp);
    void intersectPPWithSIContact (SIPointPoint* spp, SIContact* cc);
    void intersectSIContactWithPL (SIContact* cc, SIPointLine* spl);
    void intersectPLWithSIContact (SIPointLine* spl, SIContact* cc);
    void intersectSIContactWithPA (SIContact* cc, SIPointArc* spa);
    void intersectPAWithSIContact (SIPointArc* spa, SIContact* cc);

  void InitializeAJunction(SILink* current);
  void InitializeASIJunct(SILink* current);
  void InitializeASink(SILink* current);

  void InitializeShockBranchesFromASource (SISource* source);
  void PointPointBranches (SISource* source, BPoint* bp1, BPoint* bp2);
  void PointLineBranches  (SISource* source, BPoint* bp1, BLine*  bl2);
  void PointArcBranches   (SISource* source, BPoint* bp1, BArc*   ba2);
  void LineArcBranches    (SISource* source, BLine*  bl1, BArc*   ba2);
  void ArcArcBranches     (SISource* source, BArc*   ba1, BArc*   ba2);
  
  bool InitializeShockFromASIJunct(SIJunct* current);
  void InitializeShockFromAnA3Source(SIA3Source* current);
    SILink* InitializeShockFromAJunction(SINode* parent,
        BElement* lbelm, BElement* rbelm, SILink* lshock, SILink* rshock);

  PROPAGATION_TYPE LookForIntersectionsWithNeighbors (SILink* current);
  IntrinsicIntersection ComputeIntersection (SILink* shL, SILink* shR);

public:

  //**** NEW DYNAMIC VALIDATION ****
  virtual bool validateCandidateSource (SISource* source);

  //**** ESF file generation FUNCTIONS ****

  double DELTA_SAMPLE; //parameter for setting sampling dist between two samples
    
  void MarkInsideOutside (void);
    void groupIOLabel (SIElement* dest, SIElement* cur);
    void setIOLabel (int srclabel, int newlabel);
    void setIO (int srclabel, int inout);

  void write_esf_file (const char* filename, int option);   //.esf file

  //**** SYMMETRY TRANSFORMS
  void Prune(double threshold=1.0);

  //to keep track of gradient descent steps
  long GD_step_count;

  bool GD_SymTrans_step(double threshold);
  void GD_SymTrans(long step=-1);
  void GD_SymTransToThreshold(double thresh=-1);
  void computeSymTransSalience (SIElement* shock);
  double computeGapTransSalience (SIElement* shock);
  void recomputeSaliencies();

  int delAllBElementsOfParentsToA3(SILink* current);
  int deleteContourSegmentBetween(BElement* SBElm, BElement* EBElm);

  int zeroLengthShockTransform(SILink* current);
  int splice(SIElement* current, int option=0);
  int A4_A2Transform(SISource* src, SISink* vcl_sink, SILink* first_link, SILink* last_link);
  int experimental_splice(SIElement* current);
  int remove_gap(SIElement* current);
  void PointPointRegulatoryDelete(BPoint*spt, BPoint*ept, Point start_pt);

  double PointPointCompletion (BPoint* spt, BPoint* ept, double R, Point cen, bool complete=true);
  double PointPointTangentCompletion (BPoint* spt, BPoint* ept, bool complete=true);
  double PointTangentPointTangentCompletion (BPoint* spt, BPoint* ept, bool complete=true);

  //************ Graph operations on the shock graph

  // Access operations
  int number_of_nodes(); // returns the number of nodes in IShock. 
  int number_of_edges(); // returns the number of edges in IShock.

  int outdeg(SINode* v, bool doNotincludehidden=true); // returns the number of edges adjacent to node v (| adj_edges(v)|).
  int indeg(SINode* v, bool doNotincludehidden=true); //returns the number of edges ending at v (| in_edges(v)|)
  int degree(SINode* v, bool doNotincludehidden=true);  //returns outdeg(v) + indeg(v).
  SINode* source(SILink* e); //returns the source node of edge e. 
  SINode* dest(SILink* e); //returns the dest node of edge e. 
  SINode* opposite(SINode* v, SILink* e); // returns dest(e) if v = source(e) and source(e) otherwise. 
 
  SINodesList all_nodes(); // returns the vcl_list V of all nodes of the graph
  SILinksList all_edges(); // returns the vcl_list E of all edges of the graph
  SILinksList adj_edges(SINode* v); // returns adj_edges(v). 
  SILinksList out_edges(SINode* v); // returns adj_edges(v) if IShock is directed and the empty vcl_list otherwise. 
  SILinksList in_edges(SINode* v); // returns in_edges(v) if IShock is directed and the empty vcl_list otherwise. 
  SINodesList adj_nodes(SINode* v); // returns the vcl_list of all nodes adjacent to v. 

  // graph operations assuming that this is a directed graph

  SILink* first_adj_edge(SINode* v); // returns the first edge in the adjacency vcl_list of v (nil if this vcl_list is empty). 
  SILink* last_adj_edge(SINode* v); // returns the last edge in the adjacency vcl_list of v (nil if this vcl_list is empty). 
  SILink* adj_succ(SILink* e); // returns the successor of edge e in the adjacency vcl_list of node source(e) (nil if it does not exist). 
  SILink* adj_pred(SILink* e); // returns the predecessor of edge e in the adjacency vcl_list of node source(e) (nil if it does not exist). 
  SILink* cyclic_adj_succ(SILink* e); // returns the cyclic successor of edge e in the adjacency vcl_list of node source(e). 
  SILink* cyclic_adj_pred(SILink* e); // returns the cyclic predecessor of edge e in the adjacency vcl_list of node source(e). 
  SILink* first_in_edge(SINode* v); // returns the first edge of in_edges(v) (nil if this vcl_list is empty). 
  SILink* last_in_edge(SINode* v); // returns the last edge of in_edges(v) (nil if this vcl_list is empty). 
  SILink* in_succ(SILink* e); // returns the successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  SILink* in_pred(SILink* e); // returns the predecessor of edge e in in_edges(target(e)) (nil if it does not exist). 
  SILink* cyclic_in_succ(SILink* e); // returns the cyclic successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  SILink* cyclic_in_pred(SILink* e); // returns the cyclic predecessor of edge e in in_edges(target(e)) (nil if it does not exist). 

  // Operations for undirected graphs 
  
  SILink* adj_succ(SILink* e, SINode* v); // returns the successor of edge e in the adjacency vcl_list of v.
  SILink* adj_pred(SILink* e, SINode* v);// returns the predecessor of edge e in the adjacency vcl_list of v.
  SILink* cyclic_adj_succ(SILink* e, SINode* v, bool include_hidden_SILinks = false); // finished.// returns the cyclic successor of edge e in the adjacency vcl_list of v. 
  SILink* cyclic_adj_pred(SILink* e, SINode* v, bool include_hidden_SILinks = false); // returns the cyclic predecessor of edge e in the adjacency vcl_list of v.

  BElementList node_projection_cone(SILink* e, SINode *v); // unfinished// return the belements in the "back" of v, forward direction is the edge e.

  int countUnprunedParentsOfANode(SINode* node);
  SILink* UnprunedParentOfAJunction(SIJunct* junct);
  SILink* UnprunedParentOfANode(SINode* node);

  SINode* traceToNextNode(SINode* cur_node, SILink** Llink);
  SINode* traceToNextNode(SILink* Flink, SILink** Llink);
  SINode* traceBackToPreviousNode( SILink* link);

  BElement* getRightBElementFromEdge(SILink* curEdge, SINode* curNode); 
  BElement* getLeftBElementFromEdge(SILink* curEdge, SINode* curNode); // finished 

  void IdentifyEndPoints();
  void ClearTraversedStates();

  BContourList CCWEulerTourAroundALoop(SILink* selm);
  BElementList CCWFollowABranch(SILink* e);
  BElementList CCWTraceContourBetween(SINode* sn, SILink* se, SINode* dn, SILink* de ); // finished
  void CCWEulerTourToGroupIntoContours();
  void FormShockGroups(bool DTbased=false);
  void ClearShockGroups();
  void FormDirectedShockGroups(SILink* first_link);
  void FormUnDirectedShockGroups(SINode* first_node, SILink* first_link);

  void FormVisualFragments();
  void FormDTFragments();
  void ClearVisualFragments();
  
  //************ GREEN GRAPH TESTS
  void FindAllContourFragments();
  void FormLoopGraph();
  void LabelGreenGraph();

  
  // **************** PROJECTIONS of ISHOCK on BOUNDARY *********
  vcl_list < A3node_bnd_link* > find_BCoor_of_cvt_xtrema_nodes(); 
  void update_boundary_with_A3_node_info(vcl_list < A3node_bnd_link* > &A3_bcoor_list);
  

  //**** DEBUG ROUTINES ***
  void DebugPrintShockList (bool bPrintAll=false);
  void DebugPrintOnePropagation (int id, PROPAGATION_TYPE action);
  void MessageOutDetectionResults (int wndid);

  void DebugPrintGDList();
  void DebugPrintShockInfoFromID(int id);

  void updateShockDrawing (SIElement* sielm);

  //Extrinsic computations: REQ FOR TRANSITION TO VXL
  void UpdateShocks();

  friend class Boundary;
    
};

#include "boundary-bucketing.h"

class IShock_Bucketing : public IShock
{
public:
  Boundary_Bucketing* boundary() {
    return (Boundary_Bucketing*) _boundary;
  }

  IShock_Bucketing (Boundary* bnd, SHOCK_ALGO_TYPE newAlgoType = LAGRANGIAN, bool isReal = true);
  ~IShock_Bucketing ();

  virtual bool DetectShocks (SHOCK_INIT_TYPE InitOption=DEFAULT_INIT, SHOCK_ALGO_TYPE Algorithm=DEFAULT_ALGO);
  virtual void InitializeShocks (SHOCK_INIT_TYPE InitOption=DEFAULT_INIT);
};

#include "boundary-dynval.h"

class IShock_DynVal : public IShock
{
public:
  IShock_DynVal(Boundary* bnd): IShock(bnd){}
  ~IShock_DynVal(){}

private:
  //virtual bool PropagateShocks ();

  virtual bool validateCandidateSource (SISource* source);
  virtual void moveASILinkToInfinity (SILink* elm);
  virtual void moveSILinksToALeftJunction (SILink* lslink, SILink* current, IntrinsicIntersection inter);
  virtual void moveSILinksToARightJunction (SILink* current, SILink* rslink, IntrinsicIntersection inter);

};

class IShock_CEDT : public IShock
{
public:
  IShock_CEDT(Boundary* bnd): IShock(bnd){}
  ~IShock_CEDT(){}
};

#endif
