// This is brcv/shp/dbsk2d/dbsk2d_ishock_lineline.h
#ifndef dbsk2d_ishock_lineline_h_
#define dbsk2d_ishock_lineline_h_
//:
// \file
// \brief Intrinsic shock element due to 2 line segments
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

//: Intrinsic shock element due to 2 line segments
class dbsk2d_ishock_lineline : public  dbsk2d_ishock_edge 
{
private:
 
  VECTOR_TYPE _ul;  ///< angle of unit vcl_vector along left line
  VECTOR_TYPE _ur;  ///< angle of unit vcl_vector along right line
  double _sigma;    ///< ul dot ur

  double _thetaL;   ///< angle between ur and nl
  double _thetaR;   ///< angle between ul and nr
  double _phi;      ///< half of the angle between the lines

  double _N1L, _N1R; ///< the slope of the shock with respect to the line
  double _N2L, _N2R; ///< parameters for the intercepts

  vgl_point_2d<double> _Al, _Bl, _Ar, _Br; ///< cached points
  double _lL;        ///< length of the left line segment
  double _lR;        ///< length of the right line segment

public:
  //: Constructor
  dbsk2d_ishock_lineline (int newid, double stime, 
                          dbsk2d_ishock_node* pse,
                          dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                          double lsEta, double rsEta,
                          bool constrained=UNCONSTRAINED);
  //: destructor
  virtual ~dbsk2d_ishock_lineline () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE ul() { return _ul; }
  VECTOR_TYPE ur() { return _ur; }
  double sigma()   { return _sigma; }

  double thetaL() { return _thetaL; }
  double thetaR() { return _thetaR; }
  double phi()    { return _phi; }

  double N1L() { return _N1L; }
  double N1R() { return _N1R; }
  double N2L() { return _N2L; }
  double N2R() { return _N2R; }

  vgl_point_2d<double> Al() { return _Al; }
  vgl_point_2d<double> Bl() { return _Bl; }
  vgl_point_2d<double> Ar() { return _Ar; }
  vgl_point_2d<double> Br() { return _Br; }

  double lL() { return _lL; }
  double lR() { return _lR; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_bline*  lBLine() { return (dbsk2d_ishock_bline*)_lBElement; }
  dbsk2d_ishock_bline*  rBLine() { return (dbsk2d_ishock_bline*)_rBElement; }
  
  virtual bool is_third_order() { return (_phi<1e-7)? true : false; }

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
  //---------------------------------------------------------------------
  virtual double dFromLTau  (double Ltau) { return rFromLTau(Ltau); }
  virtual double dFromRTau  (double Rtau) { return rFromRTau(Rtau); }

  virtual double rFromLTau  (double Ltau);
  virtual double rFromRTau  (double Rtau);

  virtual double d  (double tau) { return dFromLTau(tau); }
  virtual double r  (double tau) { return rFromLTau(tau); } 
  virtual double rp (double tau);
  virtual double rpp(double tau);
  virtual double g  (double tau);
  virtual double tangent (double tau);
  virtual double k  (double tau);
  virtual double v  (double tau);
  virtual double a  (double tau);
  virtual double phi (double tau);

  //compute the intrinsic parameters from the radius
  virtual double getLTauFromTime (double time);
  virtual double getRTauFromTime (double time);

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------
  
  virtual TAU_DIRECTION_TYPE tauDir() {return TAU_INCREASING;}

  //functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau(LTau(rtau)); }
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromLTau(tau); }

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void) { return getPtFromLTau(_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void);
  virtual vgl_point_2d<double> getEndPtWithinRange (void);

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double tau) { return _translatePoint (_Bl, _ul+vnl_math::pi, tau); }
  virtual vgl_point_2d<double> getRFootPt (double tau) { return _translatePoint (_Ar, _ur, RTau(tau)); }

  virtual void compute_extrinsic_locus();

  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_lineline_h_
