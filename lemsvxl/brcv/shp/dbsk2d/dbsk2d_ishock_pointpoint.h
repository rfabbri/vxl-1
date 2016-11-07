// This is brcv/shp/dbsk2d/dbsk2d_ishock_pointpoint.h
#ifndef dbsk2d_ishock_pointpoint_h_
#define dbsk2d_ishock_pointpoint_h_
//:
// \file
// \brief Intrinsic shock element due to 2 points 
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

//: Intrinsic shock element due to 2 points 
class dbsk2d_ishock_pointpoint : public  dbsk2d_ishock_edge 
{
private:
   VECTOR_TYPE _u; ///< x-axis of the local coordinate system
   VECTOR_TYPE _n; ///< tangent vector wrt the image X-axis
   double _ldelta; ///< offset of _u from vref of the left point
   double _rdelta; ///< offset of _u+pi from vref of right point

public:
  //: Constructor
  dbsk2d_ishock_pointpoint (int newid, double stime, 
                dbsk2d_ishock_node* pse,
                dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                double lsEta, double rsEta,
                bool constrained=UNCONSTRAINED);
  //: destructor
  virtual ~dbsk2d_ishock_pointpoint () {}

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------

  VECTOR_TYPE u() { return _u; }
  VECTOR_TYPE n() { return _n; }

  //-----------------------------------------------------------------------------
  // Useful functions
  //-----------------------------------------------------------------------------

  dbsk2d_ishock_bpoint* lBPoint() { return (dbsk2d_ishock_bpoint*)_lBElement; }
  dbsk2d_ishock_bpoint* rBPoint() { return (dbsk2d_ishock_bpoint*)_rBElement; }

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

  virtual double RTau(double ltau) { return 2*vnl_math::pi - ltau; }
  virtual double LTau(double rtau) { return 2*vnl_math::pi - rtau; }

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

  virtual double dFromLTau  (double Ltau) { return rFromLTau(Ltau); }
  virtual double dFromRTau  (double Rtau) { return rFromRTau(Rtau); }

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
  
  //compute the intrinsic parameters from the radius
  virtual double getLTauFromTime (double time);
  virtual double getRTauFromTime (double time);
  virtual double getTauFromTime (double time) { return getLTauFromTime(time); }

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  virtual TAU_DIRECTION_TYPE tauDir() { return TAU_INCREASING; }

  //functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau);
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return getPtFromLTau (LTau(rtau)); }
  virtual vgl_point_2d<double> getPtFromTau (double tau)   { return getPtFromLTau(tau); }
  virtual double getMidTau (double curtime) { return (sTau()+getTauFromTime(curtime))/2; }

  virtual vgl_point_2d<double> getStartPt (void) { return getPtFromLTau (_LsTau); }
  virtual vgl_point_2d<double> getEndPt (void)   { return getPtFromLTau (_LeTau); }
  virtual vgl_point_2d<double> getMidPt (void);
  virtual vgl_point_2d<double> getEndPtWithinRange (void);

  //functions to get boundary points from shocks 
  virtual vgl_point_2d<double> getLFootPt (double tau=0)   { return lBPoint()->pt(); }
  virtual vgl_point_2d<double> getRFootPt (double tau=0)   { return rBPoint()->pt(); }

  virtual void compute_extrinsic_locus();

  virtual void getInfo (vcl_ostream& ostrm);

};

#endif // dbsk2d_ishock_pointpoint_h_
