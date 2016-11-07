//: Aug 19, 2005 MingChing Chang
//  Definitions for the wavefront element for the wavefront propagation algorithm
//  for computing the goedesic distance transform.

#ifndef gdt_welm_h_
#define gdt_welm_h_

#include <gdt/gdt_interval.h>

class gdt_shock;

//: enumator of all nextevents.
typedef enum {
  WENE_NA       = 0,
  WENE_RF_FPT   = 1,
  WENE_SV       = 2,
  WENE_EV       = 3,
  WENE_LE_FPT   = 5,
  WENE_RE_FPT   = 6,
  WENE_OV       = 7,
} WE_NEXTEVENT;

// ###############################################################
//: The circular wavefront element class
class gdt_welm : public gdt_interval
{
protected:
  //: is this wavefront element finalized?
  bool          final_;

  WE_NEXTEVENT  next_event_;
  
  //: the adjacent geodesic shocks.
  gdt_shock*    Sl_;
  gdt_shock*    Sr_;

  double        simT_;
  double        tVS_, tVE_, tEL_, tER_, tVO_;
  double        tauVO_;

public:
  gdt_welm (const INTERVAL_TYPE type,
            const double stau, const double etau,
            const dbmsh3d_halfedge* he,
            const dbmsh3d_gdt_vertex_3d* psource, 
            const double L, const double H, 
            const gdt_interval* prevI,
            const double simtime);
  virtual ~gdt_welm () {}

  //: ====== Data access functions ======

  bool _is_RF() const {
    //According to the definition, RF is the one with NULL prevI, 
    //Instead, it is a nextI of its psrcV.
    assert (!is_dege());
    return prevI_ == NULL;
  }
  
  //: return the current mesh face of this wavefront element
  dbmsh3d_face* curF() const;

  bool is_final() const {
    return final_;
  }
  void set_final() {
    final_ = true;
  }  
  bool try_finalize();

  WE_NEXTEVENT next_event() const {
    return next_event_;
  }
  void set_next_event (const WE_NEXTEVENT e) {
    next_event_ = e;
  }  
  
  gdt_shock* Sl() const {
    return Sl_;
  }
  gdt_shock* Sr() const {
    return Sr_;
  }
  gdt_shock* otherS (const gdt_shock* S) const {
    if (Sl_ == S)
      return Sr_;
    else
      return Sl_;
  }
  void _set_Sl (const gdt_shock* S) {
    Sl_ = (gdt_shock*) S;
  }
  void _set_Sr (const gdt_shock* S) {
    Sr_ = (gdt_shock*) S;
  }
  bool is_S_adjacent (const gdt_shock* S) const {
    if (S == Sl_ || S == Sr_)
      return true;
    else
      return false;
  }

  bool is_SS_finalized() const;
    
  double simT() const {
    return simT_;
  }
  void set_simT (const double t) {
    simT_ = t;
  }
  double tVS() const {
    return tVS_;
  }
  void set_tVS (const double t) {
    tVS_ = t;
  }
  double tVE() const {
    return tVE_;
  }
  void set_tVE (const double t) {
    tVE_ = t;
  }
  double tEL() const {
    return tEL_;
  }
  void set_tEL (const double t) {
    tEL_ = t;
  }
  double tER() const {
    return tER_;
  }
  void set_tER (const double t) {
    tER_ = t;
  }
  double tVO() const {
    return tVO_;
  }
  void set_tVO (const double t) {
    tVO_ = t;
  }

  double tauVO() const {
    return tauVO_;
  }
  void set_tauVO (const double tau) {
    tauVO_ = tau;
  }

  double _get_tauEL (const double& alphaCL) {
    return tau_from_theta (vnl_math::pi_over_2 - alphaCL);
  }      
  double _get_tauER (const double& alphaCR) {
    return tau_from_theta (vnl_math::pi_over_2 + alphaCR);
  }  
  
  //: ====== Local orientation functions ======
  void _get_RF_orientation (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                            dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;
  void _get_RF_orientation2 (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                             dbmsh3d_halfedge** heL, dbmsh3d_halfedge** heR, 
                             dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;
  bool _get_W_orientation (dbmsh3d_halfedge** heC, dbmsh3d_face** F, 
                           dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;
  bool _get_W_orientation2 (dbmsh3d_halfedge** heC, dbmsh3d_face** F,  
                            dbmsh3d_halfedge** heL, dbmsh3d_halfedge** heR, 
                            dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;
  bool _get_nextF_alphaLR_vO (double& alphaCL, double& alphaCR, dbmsh3d_gdt_vertex_3d** vO) const;

  vgl_point_3d<double> get_ext_pt_3d (const vgl_point_2d<double>& P2,
                                      const vgl_point_3d<double>& vO) const;

  //: ====== Compute next event ======  
  void compute_tVS ();
  void compute_tVE ();
  void compute_tEL_tER_tOV ();

  //: ====== Validation functions ======

  //: Validate the input point P against the leftS.
  //  input: local alphaCL and alphaCR, the input point P and its time timeP.
  //  output: true if P is valid.
  //          false if P is not valid. Also return the corresponding point PS 
  //          on shock and its time timePS in the input variables P and timeP, respectively.
  bool W_validate_P_on_Sl (const double& alphaCL, const double& alphaCR, 
                           vgl_point_2d<double>& P, double& timeP) const;
  bool W_validate_P_on_Sr (const double& alphaCL, const double& alphaCR, 
                           vgl_point_2d<double>& P, double& timeP) const;

  bool RF_validate_P_on_Sl (const double& alphaOL, vgl_point_2d<double>& P, double& timeP) const;
  bool RF_validate_P_on_Sr (const double& alphaOR, vgl_point_2d<double>& P, double& timeP) const;

  //: Subroutine to validate the input point P against the leftS.
  //  input: the local (OL, OR) of leftS, point P and its time timeP.
  //  output: true if P is valid.
  //          false if P is not valid. Also return the corresponding point PS 
  //          on shock and its time timePS in the input variables P and timeP, respectively.
  bool _validate_P_leftS (const vgl_point_2d<double>& OL, const vgl_point_2d<double>& OC,
                          vgl_point_2d<double>& P, double& timeP) const;

  bool _validate_P_rightS (const vgl_point_2d<double>& OC, const vgl_point_2d<double>& OR,
                           vgl_point_2d<double>& P, double& timeP) const;

  //: return true if need to run validation against one of the shocks.
  bool _check_need_valid_Sl_Sr (bool& validate_on_Sl, bool& validate_on_Sr) const;

  //: validate tEL using the left shock
  bool validate_tEL (const double& alphaCL, const double& alphaCR); 
    vgl_point_2d<double> _compute_FL (const double& alphaCL) const;


  //: validate tER using the right shock
  bool validate_tER (const double& alphaCL, const double& alphaCR);
    vgl_point_2d<double> _compute_FR (const double& alphaCR) const;

  //: validate tER using both the left and right shock
  bool validate_tVO (const double& eL_len, const double& alphaCL, 
                     const double& eR_len, const double& alphaCR); 

  bool validate_RF_FPT ();
  bool validate_RF_SPT ();
  bool validate_RF_EPT ();

  vgl_point_3d<double> _get_eP_from_tau (const double& tau,
                                         const double& alphaCL, 
                                         const double& alphaCR, 
                                         const dbmsh3d_gdt_vertex_3d* vO);

  //: ====== State modification functions ======

  void _set_simT_to_closest () {
    // Determine the status for Ip using the closet point.
    if (L_ < stau_)
      simT_ = mu() + vnl_math_hypot (stau_-L_, H_);
    else if (L_ > etau_)
      simT_ = mu() + vnl_math_hypot (L_-etau_, H_);
    else
      simT_ = mu() + H_;
  }

  double validate_WW_OP (const vgl_point_2d<double>& OP, const double& muP,
                         const double& alphaCL, const double& alphaCR) const;

  //: Validate the point OP with muP on this interval.
  //  First comupute the WW-strike mid point M.
  //  Return timeM if M is valid, otherwise, return GDT_HUGE.
  double _validate_WW_on_I (const vgl_point_2d<double>& OP, const double& muP,
                            vgl_point_2d<double>& M) const;

  //: Validate the mid point M with timeM against the leftS and rightS.
  //  Return GDT_HUGE if it is valid, otherwise, return the timeM on shock.
  double _validate_WW_on_Sl_Sr (const double& alphaCL, const double& alphaCR,
                                vgl_point_2d<double>& M, double timeM) const;

  double _compute_tauP_on_I (const double& px, const double& py) const;

  //: ====== getInfo ======
  virtual void getInfo (vcl_ostringstream& ostrm);
};

inline double _local_closest_dist (const double& stau,
                                   const double& etau,
                                   const double& L,
                                   const double& H)
{
  if (L<stau)
    return vnl_math_hypot (stau - L, H);
  else if (L>etau)
    return vnl_math_hypot (L - etau, H);
  else
    return H;
}

#endif


