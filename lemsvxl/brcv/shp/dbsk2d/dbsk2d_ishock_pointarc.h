// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointarc.h
#ifndef dbsk2d_ishock_pointarc_h_
#define dbsk2d_ishock_pointarc_h_
//:
// \file
// \brief Intrinsic shock element due to a point and an arc segment
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
#include <dbsk2d/dbsk2d_ishock_barc.h>

//: Intrinsic shock element due to a point and an arc segment
class dbsk2d_ishock_pointarc : public  dbsk2d_ishock_edge 
{
private:
  VECTOR_TYPE _u;   ///< The local x-axis vector  
  int _nu;          ///< +1: (LeftPoint, RightArc), -1: (LeftArc, RightPoint)
  int _s;           ///< sigma: +1: (H>R), -1: (R>H)
  int _case;        ///< 4 cases for PointArc

  double _a;
  double _b2;       ///< b^2
  double _b;
  double _c;

  double _LAsymTau; ///< The Ltau of the asymptote 
  double _RAsymTau; ///< The Rtau of the asymptote 

public:
  //: Constructor
  dbsk2d_ishock_pointarc (int newid, double stime, 
                          dbsk2d_ishock_node* pse,
                          dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                          double lsEta, double rsEta);
  //: destructor
  virtual ~dbsk2d_ishock_pointarc () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE u() { return _u; }
  int nu() { return _nu; }
  int s() { return _s; }

  double a() { return _a; }
  double b2() { return _b2; }
  double c() { return _c; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_bpoint*  lBPoint() { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_bpoint*)_lBElement; }
  dbsk2d_ishock_barc*    rBArc()   { dbsk2d_assert(_nu==1);  return (dbsk2d_ishock_barc*)  _rBElement; }
  dbsk2d_ishock_barc*    lBArc()   { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_barc*)  _lBElement; }
  dbsk2d_ishock_bpoint*  rBPoint() { dbsk2d_assert(_nu==-1); return (dbsk2d_ishock_bpoint*)_rBElement; }  

  //: radius of the left arc
  double Rl() { 
    if (_nu==1) return 0;//LeftPoint, RightArc 
    else        return lBArc()->R(); 
  }

  //: radius of the right arc
  double Rr(){
    if (_nu==1) return rBArc()->R(); //LeftPoint, RightArc
    else        return 0;
  }

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
  
  //functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau(LTau(rtau)); }
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return getPtFromLTau(tau); }

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau(_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void) { return getPtFromLTau(_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void);
  virtual vgl_point_2d<double> getEndPtWithinRange (void);

  //functions to get boundary points from shocks
  virtual vgl_point_2d<double> getLFootPt (double ltau);
  virtual vgl_point_2d<double> getRFootPt (double rtau);

  virtual void compute_extrinsic_locus();
  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_pointarc_h_
