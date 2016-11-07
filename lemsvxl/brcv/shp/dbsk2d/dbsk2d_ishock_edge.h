// This is brcv/shp/dbsk2d/dbsk2d_ishock_edge.h
#ifndef dbsk2d_ishock_edge_h_
#define dbsk2d_ishock_edge_h_
//:
// \file
// \brief Base class for all intrinsic shock edge elements
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
//
//   Amir Tamrakar 09/02/2005    added parameters to keep track of the lagrangian
//                               boundary it intersects with.
// \endverbatim

#include <vcl_list.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_elm.h>
#include <dbsk2d/dbsk2d_ishock_node.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd_sptr.h>
#include <dbsk2d/dbsk2d_lagrangian_cell_bnd.h>

//: Base class for all intrinsic shock edge classes
class dbsk2d_ishock_edge : public dbsk2d_ishock_elm
{
public:

  enum TAU_DIRECTION_TYPE
  {
    BOGUS_TAU_DIRECTION,
    TAU_INCREASING,
    TAU_DECREASING,
  };
  
protected:
  int  _edgeID; ///< ESF: common ID of the shock edges after they are grouped
  
  dbsk2d_ishock_belm* _lBElement;  ///< the left boundary element
  dbsk2d_ishock_belm* _rBElement;  ///< the right boundary element

  dbsk2d_ishock_edge*  _lNeighbor;    ///< left neighboring shock
  dbsk2d_ishock_edge*  _rNeighbor;    ///< right neighboring shock

  dbsk2d_ishock_edge*  _lShock;    ///< shock that it shares a wavefront with at some time
  dbsk2d_ishock_edge*  _rShock;    ///< shock that it shares a wavefront with at some time

  int _lShock_id;   // Keep track of lshock id
  int _rShock_id;   // Keep track of rshock id

  dbsk2d_ishock_node*  _pSNode;      ///< parent shock node.
  dbsk2d_ishock_node*  _cSNode;      ///< child shock node.

  double _H;         ///< the H=2R, distance between 2 boundary elements

  double _LsTau;     ///< intrinsic parameter at the starting point using
                     //left boundary element's coordinate system
  double _LeTau;     ///< parameter at end point
  double _RsTau;     ///< intrinsic parameter at the starting point using
                     //right boundary element's coordinate system
  double _ReTau;     ///< parameter at end point
  
  double _minLTau;   ///< minimum value of the left tau parameter allowed
  double _maxLTau;   ///< maxmium value of the left tau parameter allowed
  double _minRTau;   ///< minimum value of the right tau parameter allowed
  double _maxRTau;   ///< maximum value of the right tau parameter allowed

  double _bnd_intersect_pos; ///> extrinsic coordinate of the intersection with the cell boundary
  dbsk2d_lagrangian_cell_bnd_sptr _cell_bnd; ///> cell boundary it is intersecting with 

public:

  //: constructor
  dbsk2d_ishock_edge (int newid, double stime, dbsk2d_ishock_node* pse,
                      dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe);

  //: destructor
  virtual ~dbsk2d_ishock_edge (){
      _lBElement=NULL;
      _rBElement=NULL;
      _lNeighbor=NULL;
      _rNeighbor=NULL;
      _lShock=NULL;
      _rShock=NULL;
      _pSNode=NULL;
      _cSNode=NULL;}
  
  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  int  edgeID() { return _edgeID; }
  dbsk2d_ishock_belm* lBElement() { return _lBElement; }
  dbsk2d_ishock_belm* rBElement() { return _rBElement; }
  dbsk2d_ishock_edge* lNeighbor() { return _lNeighbor; }
  dbsk2d_ishock_edge* rNeighbor() { return _rNeighbor; }
  dbsk2d_ishock_edge* lShock() { return _lShock; }
  dbsk2d_ishock_edge* rShock() { return _rShock; }
  dbsk2d_ishock_node* pSNode() { return _pSNode; }
  dbsk2d_ishock_node* cSNode() { return _cSNode; }
  dbsk2d_ishock_node* source() { return _pSNode; } // alternative function names
  dbsk2d_ishock_node* target() { return _cSNode; }

  //intrinsic parameters
  double H() { return _H; }

  double LsTau() { return _LsTau; }
  double LeTau() { return _LeTau; } 
  double RsTau() { return _RsTau; }
  double ReTau() { return _ReTau; }

  double minLTau() { return _minLTau; }
  double maxLTau() { return _maxLTau; }
  double minRTau() { return _minRTau; }
  double maxRTau() { return _maxRTau; }

  //extrinsic parameters
  double bnd_intersect_pos() { return _bnd_intersect_pos; }
  dbsk2d_lagrangian_cell_bnd_sptr cell_bnd() { return _cell_bnd; }

  int lShock_id(){ return _lShock_id; }
  int rShock_id(){ return _rShock_id; }

  //-----------------------------------------------------------------------------
  // Set member variables
  //-----------------------------------------------------------------------------

  // set values of member variables
  void setEdgeID(int edgeid) { _edgeID = edgeid; }
  void set_lNeighbor(dbsk2d_ishock_edge* lneighbor) { _lNeighbor = lneighbor; }
  void clear_lNeighbor () { _lNeighbor = NULL; }
  void set_rNeighbor(dbsk2d_ishock_edge* rneighbor) { _rNeighbor = rneighbor; }
  void clear_rNeighbor () { _rNeighbor = NULL; }
  void set_lShock(dbsk2d_ishock_edge* lshock)
  {
      _lShock = lshock;
      _lShock_id=(_lShock)?_lShock->id():-1;
  }

  void set_rShock(dbsk2d_ishock_edge* rshock) 
  { 
      _rShock = rshock;
      _rShock_id=(_rShock)?_rShock->id():-1;
  }

  void set_pSNode(dbsk2d_ishock_node* psnode) { _pSNode = psnode; }
  void clear_pSNode () { _pSNode = NULL; }
  void set_cSNode(dbsk2d_ishock_node* csnode) { _cSNode = csnode; }
  void clear_cSNode () { _cSNode = NULL; }
  virtual void setSimTime (double stime);

  void setLsTau(double lstau) { _LsTau = lstau; }
  void setLeTau(double letau) { _LeTau = letau; }
  void setRsTau(double rstau) { _RsTau = rstau; }
  void setReTau(double retau) { _ReTau = retau; }

  void set_bnd_intersect_pos(double pos) { _bnd_intersect_pos = pos; }
  void set_cell_bnd(dbsk2d_lagrangian_cell_bnd_sptr cell_bnd) { _cell_bnd = cell_bnd; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  virtual bool is_a_node() { return false; }
  virtual bool is_a_link() { return true; }
  virtual bool is_a_contact() { return false; }
  virtual bool is_third_order() { return false; }

  //: reset all the parameters of this shock (called during reactivation of a shock)
  void reset_shock();

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------
  
  //: Only checks the tau with respect to intrinsic parameter limits
  // \todo The utility of this function needs to be examined
  virtual bool isLSTauValid ()=0;
  virtual bool isRSTauValid ()=0;

  //: checks tau wrt value range
  virtual bool isLTauValid_MinMax (double ltau);
  virtual bool isRTauValid_MinMax (double rtau);

  //: Test the validity of solutions at intersections
  virtual bool isTauValid_MinMax (double ltau, double rtau);

  //: compute the range of the intrinsic parameters
  virtual void compute_tau_ranges()=0;

  //: correct intrinsic parameters at the start of the shock 
  //  (so that the left and right parameters agree)
  virtual void correct_intrinsic_parameters_at_init()=0;

  //: set the end taus to their limits during init
  //  They will be brought to the correct limit after intersection with the cell bnd
  virtual void set_end_taus_at_init()=0;

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------
  
  //: return right tau given left tau
  virtual double RTau(double ltau)=0;
  //: return left tau given right tau
  virtual double LTau(double rtau)=0;

  //-----------------------------------------------------------------------------
  // default tau functions
  //-----------------------------------------------------------------------------

  //one of the taus is used as default tau depending on the ease of
  //parameterization. this varies for different models of shocks
  virtual double sTau ()=0;
  virtual double eTau ()=0;
  virtual bool isTauValid (double tau)=0;

  //-----------------------------------------------------------------------------
  // Boundary parameters (eta) for wavefront operations
  //-----------------------------------------------------------------------------
  
  //: left bnd parameter corresponding to the start point of the shock
  virtual double LsEta() { return LTauToLEta(_LsTau, true); }
  //: left bnd parameter corresponding to the end point of the shock
  virtual double LeEta() { return LTauToLEta(_LeTau, false); }
  //: right bnd parameter corresponding to the start point of the shock
  virtual double RsEta() { return RTauToREta(_RsTau, true); }
  //: right bnd parameter corresponding to the end point of the shock
  virtual double ReEta() { return RTauToREta(_ReTau, false); }
  //: get the start eta corresponding to the shock's orientation
  double sEta(dbsk2d_ishock_bnd_key::shock_type Stype);
  //: get the end eta corresponding to the shock's orientation
  double eEta(dbsk2d_ishock_bnd_key::shock_type Stype);
  
  //-----------------------------------------------------------------------------
  // tau and eta conversion functions
  //-----------------------------------------------------------------------------

  //: convert eta to tau (returns default tau i.e., same as sTau())
  virtual double EtaToTau(double eta, DIRECTION dir, bool constrained=CONSTRAINED)=0;
  //: convert left eta to left tau
  virtual double LEtaToLTau(double eta, bool constrained=CONSTRAINED)=0;
  //: convert right eta to right tau
  virtual double REtaToRTau(double eta, bool constrained=CONSTRAINED)=0;

  //: convert tau to eta
  double TauToEta(double tau, DIRECTION dir, bool start=false);
  //: convert left tau to left eta
  virtual double LTauToLEta(double tau, bool start=false)=0;
  //: convert right tau to right eta
  virtual double RTauToREta(double tau, bool start=false)=0;

  //-----------------------------------------------------------------------------
  // Dynamics of this shock edge
  // the taus here are "default taus" that sTau() returns
  //-----------------------------------------------------------------------------
  
  virtual double dFromLTau  (double Ltau)=0;
  virtual double dFromRTau  (double Rtau)=0;

  virtual double rFromLTau  (double Ltau)=0;
  virtual double rFromRTau  (double Rtau)=0;

  virtual double d (double tau)=0;
  virtual double r (double tau)=0;

  // rp and rpp are computed with respect to tau, not arclength. 
  // This is not the same as the rp in Giblin and Kimia's paper (2D intrinsic reconstruction...)
  virtual double rp (double tau)=0; 
  virtual double rpp(double tau)=0;
  virtual double tangent (double tau)=0;
  virtual double g  (double tau)=0;
  virtual double k  (double tau)=0;
  virtual double v  (double tau)=0;
  virtual double phi (double tau)=0;
  virtual double a  (double tau)=0;

  //compute the intrinsic parameters from the radius
  virtual double getLTauFromTime (double time)=0;
  virtual double getRTauFromTime (double time)=0;
  
  //-----------------------------------------------------------------------------
  // Topology related functions
  //-----------------------------------------------------------------------------

  bool isSemiInfinite(){ return _cSNode==NULL; }  
  //: Return child edge only if it is a PrunedJunction (degree 2 junction)
  dbsk2d_ishock_edge* get_next_edge(); 
  //: Return parent edge only if it is a PrunedJunction (degree 2 junction)
  dbsk2d_ishock_edge* get_previous_edge(); 

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  //: returns whether the tau paramter is increasing or decreasing
  //used in conjunction with sTau() and eTau() functions
  //so check them for the appropriate convention
  virtual TAU_DIRECTION_TYPE tauDir()=0;

  //functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau)=0;
  virtual vgl_point_2d<double> getPtFromRTau (double rtau)=0;
  virtual vgl_point_2d<double> getPtFromTau (double tau)=0;

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void)   { return getPtFromLTau((_LsTau+_LeTau)/2); }

  //functions to get boundary points from shocks 
  virtual vgl_point_2d<double> getLFootPt (double tau=0)=0;
  virtual vgl_point_2d<double> getRFootPt (double tau=0)=0;

};

#endif // dbsk2d_ishock_edge_h_
