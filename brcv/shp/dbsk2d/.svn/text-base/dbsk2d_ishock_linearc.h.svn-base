// This is brcv/shp/dbsk2d/dbsk2d_ishock_linearc.h
#ifndef dbsk2d_ishock_linearc_h_
#define dbsk2d_ishock_linearc_h_
//:
// \file
// \brief Intrinsic shock element due to a line segment and an arc segment
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

//: Intrinsic shock element due to a line segment and an arc segment
class dbsk2d_ishock_linearc : public dbsk2d_ishock_edge 
{
protected:
  VECTOR_TYPE _n;  ///< angle of unit vector in the direction AB
  VECTOR_TYPE _u;  ///< angle of unit vector normal to n (i.e. - PI/2)
  
  double _delta;   ///< delta = distance from A to M (foot)
  double _l;       ///< length of the line
  double _R;       ///< R of the arc
  
  int _mu;         ///< +1: H>R, -1:H<R (intersecting line arc)
  int _nu;         ///< +1: arc on the left
  int _s;          ///< sigma = dot(u, nL)
                   // +1: center is on the same side of the line as the arc
                   // -1: center is on the other side of the line 

  int _nud;        ///< nud = barc.nud
  
  int _case;       ///< 10 types of LineArc (for easy debug)
  
  double _c;       ///< Parabola intrinsic parameter

  vgl_point_2d<double> _foot; ///< projection of center of arc to line.

public:
  //: Constructor
  dbsk2d_ishock_linearc (int newid, double stime, 
                          dbsk2d_ishock_node* pse,
                          dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                          double lsEta, double rsEta);
  //: destructor
  virtual ~dbsk2d_ishock_linearc () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE n() { return _n; }
  VECTOR_TYPE u() { return _u; }

  int nu() { return _nu; }
  int s() { return _s; }
  int nud() { return _nud; }
  int snud() { return _s*_nud; }
  
  double delta() { return _delta; }
  double l() { return _l; }
  double R() { return _R; }
  double c() { return _c; }

  vgl_point_2d<double> foot() { return _foot; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_barc*    lBArc() { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_barc*)  _lBElement; }
  dbsk2d_ishock_bline*  rBLine() { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_bline*) _rBElement; }
  dbsk2d_ishock_bline*  lBLine() { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_bline*) _lBElement; }
  dbsk2d_ishock_barc*    rBArc() { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_barc*)  _rBElement; }

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------
  
  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();
  virtual void compute_tau_ranges();
  virtual void correct_intrinsic_parameters_at_init();
  virtual void set_end_taus_at_init();

  double computeMinLTau ();
  double computeMaxLTau ();
  double computeMinRTau ();
  double computeMaxRTau ();

  virtual bool isLTauValid_MinMax (double ltau);
  virtual bool isRTauValid_MinMax (double rtau);
  virtual bool isTauValid_MinMax (double letau, double retau);


  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------
  
  virtual double RTau(double Ltau);
  virtual double LTau(double Rtau);

  //-----------------------------------------------------------------------------
  // default tau functions
  //-----------------------------------------------------------------------------

  virtual double sTau() { return _nu==1 ? _LsTau : _RsTau; }
  virtual double eTau() { return _nu==1 ? _LeTau : _ReTau; }
  virtual bool isTauValid (double tau) { return _nu==1 ? isLTauValid_MinMax(tau): isRTauValid_MinMax(tau); }

  //-----------------------------------------------------------------------------
  // tau and eta conversion functions
  //-----------------------------------------------------------------------------

  virtual double EtaToTau(double eta, DIRECTION dir, bool constrained=CONSTRAINED);
  virtual double LEtaToLTau(double eta, bool constrained=CONSTRAINED);
  virtual double REtaToRTau(double eta, bool constrained=CONSTRAINED);

  virtual double LTauToLEta(double tau, bool start=false);
  virtual double RTauToREta(double tau, bool start=false);

  //-----------------------------------------------------------------------------
  // Dynamics of this shock edge
  // the taus here are "default taus" that sTau() returns
  //-----------------------------------------------------------------------------
  
  virtual double dFromLTau (double Ltau);
  virtual double dFromRTau (double Rtau);

  virtual double rFromLTau  (double Ltau);
  virtual double rFromRTau  (double Rtau);
  
  virtual double d (double tau);
  virtual double r  (double tau); 
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double tangent(double tau);
  virtual double g  (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);

  virtual double getPointTauFromTime (double time);

  virtual double getLTauFromTime (double time);
  virtual double getRTauFromTime (double time);
  
  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  //functions to compute extrinsic shock locus
  virtual TAU_DIRECTION_TYPE tauDir();

  virtual vgl_point_2d<double> getPtFromLTau (double ltau) { return _origin; } //FIX ME!
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return _origin; } //FIX ME!

  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromPointTau(tau); }
  virtual vgl_point_2d<double> getPtFromPointTau (double ptau);
  
  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromTau (sTau()); }
  virtual vgl_point_2d<double> getEndPt (void)   { return getPtFromTau (eTau()); }
  virtual vgl_point_2d<double> getMidPt (void);
  virtual vgl_point_2d<double> getEndPtWithinRange (void);

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double ptau);
  virtual vgl_point_2d<double> getRFootPt (double ptau);

  virtual void compute_extrinsic_locus();
  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_linearc_h_
