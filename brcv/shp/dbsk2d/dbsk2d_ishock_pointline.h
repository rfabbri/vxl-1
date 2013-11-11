// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointline.h
#ifndef dbsk2d_ishock_pointline_h_
#define dbsk2d_ishock_pointline_h_
//:
// \file
// \brief Intrinsic shock element due to a point and a line  
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>

//: Intrinsic shock element due to a point and a line 
class dbsk2d_ishock_pointline : public  dbsk2d_ishock_edge 
{
private:
  VECTOR_TYPE _n;  ///< angle of unit vector in the direction AB
  VECTOR_TYPE _u;  ///< angle of unit vector normal to n (i.e. - PI/2)
  int _nu;         ///< +1 if shock is in the direction of n, else -1.
  double _ldelta;  ///< If a point, delta=offset of _u from vref of the left point
  double _rdelta;  ///< If a line, delta=distance from A to M (foot)
  double _l;       ///< length of the line

public:
  //: Constructor
  dbsk2d_ishock_pointline (int newid, double stime, 
                dbsk2d_ishock_node* pse,
                dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                double lsEta, double rsEta,
                bool constrained=UNCONSTRAINED);
  //: destructor
  virtual ~dbsk2d_ishock_pointline () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE n() { return _n; }
  VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  double ldelta() { return _ldelta; }
  double rdelta() { return _rdelta; }
  double l() { return _l; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_bpoint* lBPoint() { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_bpoint*)_lBElement; }
  dbsk2d_ishock_bline*  rBLine()  { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_bline*) _rBElement; }
  dbsk2d_ishock_bline*  lBLine()  { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_bline*) _lBElement; }
  dbsk2d_ishock_bpoint* rBPoint() { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_bpoint*)_rBElement; }

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------

  virtual bool isLSTauValid();
  virtual bool isRSTauValid();
  virtual void compute_tau_ranges();
  virtual void correct_intrinsic_parameters_at_init();
  virtual void set_end_taus_at_init();

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------

  virtual double  RTau(double Ltau);
  virtual double  LTau(double Rtau);

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

  virtual double dFromLTau  (double Ltau) { return rFromLTau(Ltau); }
  virtual double dFromRTau  (double Rtau) { return rFromRTau(Rtau); }

  virtual double rFromLTau  (double Ltau);
  virtual double rFromRTau  (double Rtau);

  virtual double d (double tau) { return r(tau); }
  virtual double r  (double tau); 
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double tangent (double tau);
  virtual double g  (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);
  
  //compute the intrinsic parameters from the radius
  virtual double getLTauFromTime (double time);
  virtual double getRTauFromTime (double time);
  virtual double getTauFromTime (double time);

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  virtual TAU_DIRECTION_TYPE tauDir() {return _nu==1 ? TAU_INCREASING:TAU_DECREASING;}

  //functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau);
  virtual vgl_point_2d<double> getPtFromTau (double tau);

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

#endif // dbsk2d_ishock_pointline_h_
