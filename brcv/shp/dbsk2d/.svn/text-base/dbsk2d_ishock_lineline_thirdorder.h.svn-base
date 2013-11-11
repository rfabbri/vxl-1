// This is brcv/shp/dbsk2d/dbsk2d_ishock_lineline_thirdorder.h
#ifndef dbsk2d_ishock_lineline_thirdorder_h_
#define dbsk2d_ishock_lineline_thirdorder_h_
//:
// \file
// \brief Intrinsic 3rd order shock element due to 2 degenerate line segments
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

//: ntrinsic 3rd order shock element due to 2 degenerate line segments
class dbsk2d_ishock_lineline_thirdorder : public  dbsk2d_ishock_edge 
{
protected:
  VECTOR_TYPE _nl;  ///< angle of unit vcl_vector along left line
  VECTOR_TYPE _ul;  ///< nl - Pi/2
  double  _lL;      ///<  length of the left line segment
  double  _lR;      ///<  length of the right line segment
  vgl_point_2d<double> _Al, _Bl, _Ar, _Br; ///< cached points

public:
  //: Constructor
  dbsk2d_ishock_lineline_thirdorder (int newid, double stime, 
                        dbsk2d_ishock_node* pse,
                        dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                        double lsEta, double rsEta);
  //: destructor
  virtual ~dbsk2d_ishock_lineline_thirdorder () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE nl() { return _nl; }
  VECTOR_TYPE ul() { return _ul; }
  double lL() { return _lL; }
  double lR() { return _lR; }
  vgl_point_2d<double> Al() { return _Al; }
  vgl_point_2d<double> Bl() { return _Bl; }
  vgl_point_2d<double> Ar() { return _Ar; }
  vgl_point_2d<double> Br() { return _Br; }

  virtual bool is_third_order() { return true; }

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------

  virtual bool isLSTauValid ();
  virtual bool isRSTauValid ();
  virtual void compute_tau_ranges();
  virtual void correct_intrinsic_parameters_at_init();
  virtual void set_end_taus_at_init();

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------

  virtual double RTau (double ltau);
  virtual double LTau (double rtau);

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
  virtual double LEtaToLTau(double eta, bool constrained=CONSTRAINED) { return eta; }
  virtual double REtaToRTau(double eta, bool constrained=CONSTRAINED) { return eta; }

  virtual double LTauToLEta(double tau, bool start=false) { return tau; }
  virtual double RTauToREta(double tau, bool start=false) { return tau; }

  //-----------------------------------------------------------------------------
  // Dynamics of this shock edge
  // the taus here are "default taus" that sTau() returns
  //-----------------------------------------------------------------------------
  virtual double dFromLTau  (double Ltau) { return _H/2; }
  virtual double dFromRTau  (double Rtau) { return _H/2; }

  virtual double rFromLTau  (double Ltau) { return _H/2; }
  virtual double rFromRTau  (double Rtau) { return _H/2; }

  virtual double d (double tau) { return _H/2; }
  virtual double r  (double tau); 
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double g  (double tau);
  virtual double tangent (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);
  
  //compute the intrinsic parameters from the radius
  //no real solution so just return the tau at starting point
  virtual double getLTauFromTime (double time) { return _LsTau; }
  virtual double getRTauFromTime (double time) { return _RsTau; }

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  //functions to compute extrinsic shock locus
  virtual TAU_DIRECTION_TYPE tauDir(){ return TAU_DECREASING;}

  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau (LTau(rtau)); }
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromLTau(tau); }

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau (_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void) { return getPtFromLTau (_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void) { return getPtFromLTau ((_LsTau+_LeTau)/2); }
  virtual vgl_point_2d<double> getEndPtWithinRange (void) { return getEndPt (); }

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double tau) { return _translatePoint (_Al, _nl, tau); }
  virtual vgl_point_2d<double> getRFootPt (double tau) { return _translatePoint (_Ar, _nl+vnl_math::pi, RTau(tau)); }

  virtual void compute_extrinsic_locus();

  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_lineline_thirdorder_h_
