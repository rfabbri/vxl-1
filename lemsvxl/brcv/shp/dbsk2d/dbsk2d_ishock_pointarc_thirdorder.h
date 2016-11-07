// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointarc_thirdorder.h
#ifndef dbsk2d_ishock_pointarc_thirdorder_h_
#define dbsk2d_ishock_pointarc_thirdorder_h_
//:
// \file
// \brief Intrinsic 3rd order shock element due to an arc segment and the point right at the center
// \author Amir Tamrakar
// \date 05/05/08
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 05/05/2008    Initial version. Rare degeneracy but can occur.
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

//: Intrinsic 3rd order shock element due to an arc segment and a point at its center
class dbsk2d_ishock_pointarc_thirdorder : public  dbsk2d_ishock_edge
{
protected:
  VECTOR_TYPE _u;        ///< start vector for LsTau and RsTau
  int _nu;               ///< 1:Point on the left, 2: Point on the right
  double _Rl, _Rr;

public:
  //: Constructor
  dbsk2d_ishock_pointarc_thirdorder (int newid, double stime, 
                                   dbsk2d_ishock_node* pse,
                                   dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                                   double lsEta, double rsEta);
  //: destructor
  virtual ~dbsk2d_ishock_pointarc_thirdorder () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  double Rl() { return _Rl; }
  double Rr() { return _Rr; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_bpoint*  lBPoint() { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_bpoint*) _lBElement; }
  dbsk2d_ishock_barc*    rBArc()   { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_barc*)   _rBElement; }
  dbsk2d_ishock_barc*    lBArc()   { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_barc*)   _lBElement; }
  dbsk2d_ishock_bpoint*  rBPoint() { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_bpoint*) _rBElement; }

  virtual bool is_third_order() { return true; }

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------
  
  // functions to check validity of the intrinsic parameters
  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();
  virtual void compute_tau_ranges();
  virtual void correct_intrinsic_parameters_at_init();
  virtual void set_end_taus_at_init();

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------
  
  virtual double RTau(double ltau) { return ltau; }
  virtual double LTau(double rtau) { return rtau; }
  
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
  virtual double dFromLTau  (double Ltau) { return vcl_fabs(_Rl-_Rr)/2; }
  virtual double dFromRTau  (double Rtau) { return vcl_fabs(_Rl-_Rr)/2; }

  virtual double rFromLTau  (double Ltau) { return vcl_fabs(_Rl-_Rr)/2; }
  virtual double rFromRTau  (double Rtau) { return vcl_fabs(_Rl-_Rr)/2; }

  virtual double d  (double tau) { return vcl_fabs(_Rl-_Rr)/2; }
  virtual double r  (double tau) { return vcl_fabs(_Rl-_Rr)/2; }
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double tangent (double tau);
  virtual double g  (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);

  virtual double getLTauFromTime (double time) { return -1.0; } //undefined
  virtual double getRTauFromTime (double time) { return -1.0; } //undefined
 
  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  //functions to compute extrinsic shock locus
  virtual TAU_DIRECTION_TYPE tauDir(){ return TAU_DECREASING;}

  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau (LTau(rtau));}
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromLTau(tau); }

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void)   { return getPtFromLTau(_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void) {return getPtFromLTau ((_LsTau+_LeTau)/2);}

  virtual vgl_point_2d<double> getEndPtWithinRange (void) { return getEndPt (); }

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double ltau);
  virtual vgl_point_2d<double> getRFootPt (double rtau);

  virtual void compute_extrinsic_locus();
  virtual void getInfo (vcl_ostream& ostrm);

};

#endif // dbsk2d_ishock_pointarc_thirdorder_h_
