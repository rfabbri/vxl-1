// This is brcv/shp/dbsk2d/dbsk2d_ishock_contact.h
#ifndef dbsk2d_ishock_contact_h_
#define dbsk2d_ishock_contact_h_
//:
// \file
// \brief Intrinsic contact shock element  
// \author Amir Tamrakar
// \date 02/02/05
//
// 
// \verbatim
//  Modifications
//   Amir Tamrakar 02/02/2005    Initial version. Conversion to VXL standard.
// \endverbatim

#include <dbsk2d/dbsk2d_ishock_edge.h>

//: Intrinsic contact shock element 
class dbsk2d_ishock_contact : public dbsk2d_ishock_edge
{
protected:
  VECTOR_TYPE  _n;  ///< angle of contact shock with X-axis (extrinsic info)
                    //this might need a left and right n because of LL contacts

  double _LsEta;   ///< eta parameter of the left element
  double _RsEta;   ///< eta parameter of the right element
                   //these have to be stored because they cannot be inferred from the taus
public:
  //: Constructor
  dbsk2d_ishock_contact (int newid, dbsk2d_ishock_belm* lbe, dbsk2d_ishock_belm* rbe,
                         vgl_point_2d<double> origin, double n, 
                         double lstau, double rstau,
                         double lseta, double rseta);
  //: destructor
  virtual ~dbsk2d_ishock_contact (){} 

  //-----------------------------------------------------------------------------
  // Access member variables
  //-----------------------------------------------------------------------------
  
  //: get the direction of this contact
  VECTOR_TYPE n() { return _n; }
  
  virtual bool is_a_contact() { return true; }

  //-----------------------------------------------------------------------------
  // functions to check the validity of intrinsic parameters
  //-----------------------------------------------------------------------------

  virtual bool isLSTauValid() { return true; } //always valid (?)
  virtual bool isRSTauValid() { return true; } //always valid (?)

  virtual bool isTauValid_MinMax (double ltau, double rtau) { return true; }
  virtual void compute_tau_ranges();
  virtual void correct_intrinsic_parameters_at_init() {}
  virtual void set_end_taus_at_init(){} //FIX ME

  //-----------------------------------------------------------------------------
  // Tau conversion functions
  //-----------------------------------------------------------------------------
  
  virtual double RTau(double ltau) { return _RsTau; }
  virtual double LTau(double rtau) { return _LsTau; }

  //-----------------------------------------------------------------------------
  // default tau functions
  //-----------------------------------------------------------------------------

  virtual double sTau () { return _LsTau; }
  virtual double eTau () { return _LeTau; }
  virtual bool isTauValid (double tau) { return isLTauValid_MinMax(tau); }

  //-----------------------------------------------------------------------------
  // Boundary parameters (eta) for wavefront operations
  //-----------------------------------------------------------------------------
  virtual double LsEta() { return _LsEta; }
  virtual double LeEta() { return _LsEta; }
  virtual double RsEta() { return _RsEta; }
  virtual double ReEta() { return _RsEta; }
  
  //-----------------------------------------------------------------------------
  // tau and eta conversion functions
  //-----------------------------------------------------------------------------

  virtual double EtaToTau(double eta, DIRECTION dir, bool constrained=CONSTRAINED);
  virtual double LEtaToLTau(double eta, bool constrained=CONSTRAINED) { return _LsTau; }
  virtual double REtaToRTau(double eta, bool constrained=CONSTRAINED) { return _RsTau; }

  virtual double LTauToLEta(double tau, bool start=false) { return _LsEta; }
  virtual double RTauToREta(double tau, bool start=false) { return _RsEta; }

  //-----------------------------------------------------------------------------
  // Dynamics of this shock edge
  // the taus here are "default taus" that sTau() returns
  //-----------------------------------------------------------------------------
  virtual double dFromLTau  (double Ltau){ return ISHOCK_DIST_HUGE; } //undefined
  virtual double dFromRTau  (double Rtau){ return ISHOCK_DIST_HUGE; } //undefined

  virtual double rFromLTau  (double Ltau){ return ISHOCK_DIST_HUGE; } //undefined
  virtual double rFromRTau  (double Rtau){ return ISHOCK_DIST_HUGE; } //undefined

  virtual double d (double tau) { return ISHOCK_DIST_HUGE; } //undefined
  virtual double r (double tau) { return ISHOCK_DIST_HUGE; } //undefined
  virtual double rp (double tau) { return 0; }//FIX ME!
  virtual double rpp(double tau) { return 0; }//FIX ME!
  virtual double tangent (double tau) { return _n; }
  virtual double g  (double tau) { return 0; }//FIX ME!
  virtual double k  (double tau) { return 0; }//FIX ME!
  virtual double v  (double tau) { return 0; }//FIX ME!
  virtual double phi (double tau) { return vnl_math::pi; } 
  virtual double a  (double tau) { return 0; }//FIX ME!

  //compute the intrinsic parameters from the radius
  virtual double getLTauFromTime (double time) { return _LsTau; }
  virtual double getRTauFromTime (double time) { return _RsTau; }

  //-----------------------------------------------------------------------------
  // Functions for Sampling the shock (Extrinsic)
  //-----------------------------------------------------------------------------

  virtual TAU_DIRECTION_TYPE tauDir() { return TAU_INCREASING; }

  // functions to compute extrinsic shock locus
  virtual vgl_point_2d<double> getPtFromLTau (double ltau) { return _origin; } //FIX ME!
  virtual vgl_point_2d<double> getPtFromRTau (double rtau) { return _origin; } //FIX ME!
  virtual vgl_point_2d<double> getPtFromTau (double tau) { return _origin; } //FIX ME!

  virtual vgl_point_2d<double> getStartPt (void) { return _origin; }
  virtual vgl_point_2d<double> getEndPt (void)   { return _translatePoint (_origin, _n, _endTime); }
  virtual vgl_point_2d<double> getMidPt (void)   { return _translatePoint (_origin, _n,  vnl_math_min (_endTime, MAX_RADIUS)/2); }
  virtual vgl_point_2d<double> getEndPtWithinRange (void) { return _translatePoint (_origin, _n,  vnl_math_min (_endTime,MAX_RADIUS)); }

  virtual vgl_point_2d<double> getLFootPt (double tau=0) { return _origin; }
  virtual vgl_point_2d<double> getRFootPt (double tau=0) { return _origin; }

  virtual void compute_extrinsic_locus();
  
  virtual void getInfo (vcl_ostream& ostrm);
};

#endif // dbsk2d_ishock_contact_h_
