// This is brcv/shp/dbsk2d/dbsk2d_ishock_arcarc.h
#ifndef dbsk2d_ishock_arcarc_h_
#define dbsk2d_ishock_arcarc_h_
//:
// \file
// \brief Intrinsic shock element due to 2 arc segments
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

//: Intrinsic shock element due to 2 arc segments
class dbsk2d_ishock_arcarc : public dbsk2d_ishock_edge 
{
private:
  VECTOR_TYPE _u;  ///< angle of unit vector from left Arc to Right arc
  int _nu;         ///< +1: LeftSmallerR, -1: RightSmallerR
  int _s;          ///< sigma: +1:H>(R1+R2), -1:H<|R1-R2| and
                   //   ELSE   +2: if hyperbola and -2 if ellipse

  int _case;       ///< 12 cases for ArcArc shocks
  int _MU;         ///< +1: like Point-arc shocks (not intersections), -1: Circles intersect
  int _hmu;        ///< Inner hyperbola cases
  int _emu;        ///< Inner ellipse cases
  ARC_NUD _nudl;   ///< lBarc.nud
  ARC_NUD _nudr;   ///< rBarc.nud
  double _Rl, _Rr; ///< radii of the arcs for easy access

  double _a;
  double _b2;      ///< b^2
  double _b;
  double _c;

  double _LAsymTau; ///< The Ltau of the asymptote (for hyperbolic shocks)
  double _RAsymTau; ///< The Rtau of the asymptote (for hyperbolic shocks)

  double _LIntTau;  ///< The Ltau of the circle intersection
  double _RIntTau;  ///< The Rtau of the circle intersection

public:
  //: Constructor
  dbsk2d_ishock_arcarc (int newid, double stime, 
                        dbsk2d_ishock_node* pse,
                        dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe, 
                        double lsEta, double rsEta);
  //: destructor
  virtual ~dbsk2d_ishock_arcarc () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE u() { return _u; }
  int nu()        { return _nu; }
  int s()         { return _s; }
  int MU()        { return _MU; }
  int hmu()       { return _hmu; }
  int emu()       { return _emu; }
  ARC_NUD nudl()  { return _nudl; }
  ARC_NUD nudr()  { return _nudr; }
  double Rl()     { return _Rl; }
  double Rr()     { return _Rr; }
  int Case()      { return _case; }

  double a()      { return _a; }
  double b()      { return _b; }
  double b2()     { return _b2; }
  double c()      { return _c; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_barc*  lBArc() { return (dbsk2d_ishock_barc*) _lBElement; }
  dbsk2d_ishock_barc*  rBArc() { return (dbsk2d_ishock_barc*) _rBElement; }

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

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------
  
  virtual double RTau(double Ltau);
  virtual double LTau(double Rtau);

  //-----------------------------------------------------------------------------
  // default tau functions
  //-----------------------------------------------------------------------------
  
  virtual double sTau () { return _LsTau; }
  virtual double eTau () { return _LeTau; }
  virtual bool isTauValid (double tau) { return isLTauValid_MinMax(tau); }

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

  virtual double d  (double tau) { return dFromLTau(tau); }
  virtual double r  (double tau) { return rFromLTau(tau); }
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double tangent (double tau);
  virtual double g  (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);

  virtual double getLTauFromTime (double time);
  virtual double getRTauFromTime (double time) { return RTau(getLTauFromTime(time)); }

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  //functions to compute extrinsic shock locus
  virtual TAU_DIRECTION_TYPE tauDir();

  //: See notes: u for drawing, only different in s=-1&nu=1 case.
  VECTOR_TYPE ud() { return (_s==-1&&_nu==1) ? (_u+vnl_math::pi) : _u; }

  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau (LTau(rtau)); }
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromLTau(tau); }

  virtual vgl_point_2d<double> getStartPt (void) { return _pSNode->origin(); }
  virtual vgl_point_2d<double> getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void);
  virtual vgl_point_2d<double> getEndPtWithinRange (void);

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double ltau);
  virtual vgl_point_2d<double> getRFootPt (double rtau);

  virtual void compute_extrinsic_locus(); 
  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_arcarc_h_
