//: Aug 19, 2005 MingChing Chang
//  Goedesic Shock

#ifndef gdt_shock_h_
#define gdt_shock_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vgl/vgl_point_3d.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <gdt/gdt_numerical.h>
#include <gdt/gdt_welm.h>


// ##########################################################################
// Basic shock tau functions.

inline double _R_get_asymptote (const double& aRi, const double& b2)
{
  if (_eqT(aRi, 0)) // if aRi == 0, line shock.
    return vnl_math::pi_over_2;
  else
    return vcl_atan2 (vcl_sqrt(b2), aRi);
}

inline double _L_get_asymptote (const double& aLi, const double& b2)
{
  if (_eqT(aLi, 0)) // if aLi == 0, line shock.
    return vnl_math::pi_over_2;
  else
    return vcl_atan2 (vcl_sqrt(b2), -aLi);
}

// #####################################################################

inline double _get_dRR_from_tauRR (const double& aR, const double& bR2, 
                                   const double& cR, const double& tauRR)
{
  const double denom = -cR*vcl_cos(tauRR)+aR;
  assert (!_eqM(denom, 0));
  const double dRR = bR2 / denom;
  assert (dRR > 0);
  return dRR;
}

inline double _get_tauR_from_dR (const double& aR, const double& bR2, 
                                 const double& cR, const double& dR)
{
  const double cos_value = (bR2 + aR*dR)/(cR*dR);
  if (_eqF (cos_value, 1))
    return 0;
  if (_eqF (cos_value, -1))
    return vnl_math::pi;
  assert (-1 < cos_value && cos_value < 1);
  return vcl_acos (cos_value);
}

inline double _get_tauR_from_tauRR (const double& aR, const double& bR2, 
                                    const double& cR, const double& tauRR)
{
  assert (bR2 != 0); // This formula does not work for contact shock. 

  const double dRR = _get_dRR_from_tauRR (aR, bR2, cR, tauRR);
  const double dR = dRR + 2*aR;
  assert (dR > 0);
  return _get_tauR_from_dR (aR, bR2, cR, dR);
}

inline double _get_dR_from_tauR (const double& aR, const double& bR2, 
                                 const double& cR, const double& tauR)
{
  const double denom = cR*vcl_cos(tauR)-aR;
  assert (!_eqM(denom, 0));
  const double dR = bR2 / denom;
  assert (dR > 0);
  return dR;
}

inline double _get_tauRR_from_dRR (const double& aR, const double& bR2, 
                                   const double& cR, const double& dRR)
{
  const double cos_value = (-bR2 + aR*dRR)/(cR*dRR);
  if (_eqF (cos_value, 1))
    return 0;
  if (_eqF (cos_value, -1))
    return vnl_math::pi;
  assert (-1 < cos_value && cos_value < 1);
  return vcl_acos (cos_value);
}

inline double _get_tauRR_from_tauR (const double& aR, const double& bR2, 
                                    const double& cR, const double& tauR)
{
  assert (bR2 != 0); // This formula does not work for contact shock. 

  const double dR = _get_dR_from_tauR (aR, bR2, cR, tauR);
  const double dRR = dR - 2*aR;
  assert (dRR > 0);
  return _get_tauRR_from_dRR (aR, bR2, cR, dRR);
}

inline double _get_tauLL_from_dLL (const double& aL, const double& bL2, 
                                   const double& cL, const double& dLL)
{
  const double cos_value = (bL2 - aL*dLL)/(cL*dLL);
  if (_eqF (cos_value, 1))
    return 0;
  if (_eqF (cos_value, -1))
    return vnl_math::pi;
  assert (-1 < cos_value && cos_value < 1);
  return vcl_acos (cos_value);
}

inline double _get_dL_from_tauL (const double& aL, const double& bL2, 
                                 const double& cL, const double& tauL)
{
  const double denom = -cL*vcl_cos(tauL)-aL;
  assert (!_eqM(denom, 0));
  const double dL = bL2 / denom;
  assert (dL > 0);
  return dL;
}

inline double _get_tauLL_from_tauL (const double& aL, const double& bL2, 
                                    const double& cL, const double& tauL)
{
  assert (bL2 != 0); // This formula does not work for contact shock. 

  const double dL = _get_dL_from_tauL (aL, bL2, cL, tauL);
  const double dLL = dL - 2*aL;
  assert (dLL > 0);
  return _get_tauLL_from_dLL (aL, bL2, cL, dLL);
}

inline double _get_dLL_from_tauLL (const double& aL, const double& bL2, 
                                   const double& cL, const double& tauLL)
{
  const double denom = cL*vcl_cos(tauLL)+aL;
  assert (!_eqM(denom, 0));
  const double d = bL2 / denom;
  assert (d > 0);
  return d;
}

inline double _get_tauL_from_dL (const double& aL, const double& bL2, 
                                 const double& cL, const double& dL)
{
  const double cos_value = (-bL2 - dL*aL)/(dL*cL);
  if (_eqF (cos_value, 1))
    return 0;
  if (_eqF (cos_value, -1))
    return vnl_math::pi;
  assert (-1 < cos_value && cos_value < 1);
  return vcl_acos (cos_value);
}

inline double _get_tauL_from_tauLL (const double& aL, const double& bL2, 
                                    const double& cL, const double& tauLL)
{
  assert (bL2 != 0); // This formula does not work for contact shock. 

  const double dLL = _get_dLL_from_tauLL (aL, bL2, cL, tauLL);
  const double dL = dLL + 2*aL;
  assert (dL > 0);
  return _get_tauL_from_dL (aL, bL2, cL, dL);
}

// #####################################################################

class gdt_path;
class dbmsh3d_gdt_edge;

//: The shock element orientation types, 5 cases.
typedef enum {
  BOGUS_SOTYPE,
  SOT_SRFRF           = 1,
  SOT_SRF_L           = 2,
  SOT_SRF_R           = 3,
  SOT_SE              = 4,
  SOT_SV_SVE          = 5,
} SOT_TYPE;

//: The shock element type, 14 cases.
typedef enum {
  BOGUS_SET_TYPE     = 0,
  SET_SRFRF           = 1,
  SET_SRF_L           = 2,
  SET_SRF_L2          = 3,
  SET_SRF_R           = 4,
  SET_SRF_R2          = 5,
  SET_SE              = 6,
  SET_SV_I            = 7,
  SET_SV_II           = 8,
  SET_SV_III          = 9,
  SET_SV_IV           = 10,
  SET_SVE_I           = 11,
  SET_SVE_II          = 12,
  SET_SVE_III         = 13,
  SET_SVE_IV          = 14,  
} SET_TYPE;

//: The SV sub-types, 8 cases.
typedef enum {
  SV_IA           = 1,
  SV_IB           = 2,
  SV_IIA          = 3,
  SV_IIB          = 4,
  SV_IIIA         = 5,
  SV_IIIB         = 6,
  SV_IVA          = 7,
  SV_IVB          = 8,
} SV_SUBTYPE;

typedef enum {
  SELM_TAU_NONE   = 0,  //use SNode.
  SELM_TAU_ON_EA  = 1,
  SELM_TAU_ON_EB  = 2,
} SELM_TAU_TYPE;

class gdt_selm
{
public:
  gdt_welm*       Wa_;
  gdt_welm*       Wb_;
  SELM_TAU_TYPE   tauType_;
  double          tauE_;

public:
  gdt_selm (const gdt_welm* Wa, 
            const gdt_welm* Wb,
            const SELM_TAU_TYPE tauType,
            const double tauE) {
    Wa_       = (gdt_welm*) Wa;
    Wb_       = (gdt_welm*) Wb;
    tauType_  = tauType;
    tauE_     = tauE;
  }
  ~gdt_selm () {
  }
};

class gdt_shock : public vispt_elm
{
protected:
  int                 id_;

  //: ===== Data member for the current shock element =====
  gdt_welm*           Wa_;
  gdt_welm*           Wb_;
  bool                b_propagated_;

  //: Two neighboring shocks.
  gdt_shock*          Sa_;
  gdt_shock*          Sb_;

  //: the angle paremeter of the S-S intersection point.
  double              tau_;

  //: the arc-length parameter of the S-E intersection point on prjE_.
  double              tauE_;

  //: the prjE that this shock will strike.
  dbmsh3d_gdt_edge*   prjE_;

  //: The flag to distinguish shock type SV and SVE.
  //  This flag will be maintained and propagated to the childS.
  bool                bSVE_;

  //: ===== Data member for the whole shock segment =====
  double              startT_;
  double              simT_;
  double              endT_;
  double              edgeT_;

  //: The intrinsic parameter of this shock.
  double              a_;
  double              b2_;
  double              c_;

  dbmsh3d_gdt_vertex_3d*  Snode_;
  dbmsh3d_gdt_vertex_3d*  Enode_;

  vcl_vector<gdt_selm*>   selms_;
  
public:
  //: ====== Constructor/Destructor ======
  gdt_shock (const int id, 
             const gdt_welm* Wa, const gdt_welm* Wb,
             const dbmsh3d_gdt_edge* prjE, const double& tau,
             const double& startT, const double& edgeT,
             const dbmsh3d_gdt_vertex_3d* Snode);
  ~gdt_shock ();

  //: ====== Data access functions ======
  int id () const {
    return id_;
  }

  double tau() const {
    return tau_;
  }
  void set_tau (double tau) {
    tau_ = tau;
  }

  double tauE() const {
    return tauE_;
  }
  void set_tauE (double tauE) {
    tauE_ = tauE;
  }
  dbmsh3d_gdt_edge* prjE() const {
    return prjE_;
  }
  void set_prjE (const dbmsh3d_gdt_edge* e) {
    prjE_ = (dbmsh3d_gdt_edge*) e;
  }

  //: The unoriented intervals of the shock.
  gdt_welm* Wa() const {
    return Wa_;
  }
  gdt_welm* Wb() const {
    return Wb_;
  }

  void _set_Wa (const gdt_welm* W) {
    Wa_ = (gdt_welm*) W;
  }
  void _set_Wb (const gdt_welm* W) {
    Wb_ = (gdt_welm*) W;
  }

  gdt_welm* otherW (const gdt_welm* W) const {
    if (Wa_ == W)
      return Wb_;

    assert (Wb_ == W);
    return Wa_;
  }
  //: If both Wa and Wb are not W, return NULL.
  gdt_welm* otherW2 (const gdt_welm* W) const {
    if (Wa_ == W)
      return Wb_;    
    if (Wb_ == W)
      return Wa_;
    return NULL;
  }
  bool _sharingW (const gdt_welm* W) const {
    if (Wa_ == W)
      return true;
    if (Wb_ == W)
      return true;
    
    return false;
  }
  
  vcl_vector<gdt_selm*>& selms() {
    return selms_;
  }

  void _add_cur_selm () {
    gdt_selm* selm;
    if (prjE_ == Wa_->edge())
      selm = new gdt_selm (Wa_, Wb_, SELM_TAU_ON_EA, tauE_);
    else {
      assert (prjE_ == Wb_->edge());
      selm = new gdt_selm (Wa_, Wb_, SELM_TAU_ON_EB, tauE_);
    }
    selms_.push_back (selm);
  }

  void replaceW (const gdt_welm* W, const gdt_welm* newW);

  gdt_shock* Sa () const {
    return Sa_;
  }
  gdt_shock* Sb () const {
    return Sb_;
  }
  void _set_Sa (const gdt_shock* S) {
    Sa_ = (gdt_shock*) S;
  }
  void _set_Sb (const gdt_shock* S) {
    Sb_ = (gdt_shock*) S;
  }

  gdt_shock* _get_intersected_S_via_W (const gdt_welm* W) {
    gdt_shock* Si = W->otherS (this);
    if (Si == Sa_ || Si == Sb_)
      return Si;
    else
      return NULL;
  }

  double a() const {
    return a_;
  }
  double b2() const {
    return b2_;
  }
  double c() const {
    return c_;
  }

#if GDT_ALGO_FS
  void set_I_less_new (gdt_welm* I) {
    I_less_new_ = I;
  }
  void set_I_greater_new (gdt_welm* I) {
    I_greater_new_ = I;
  }
  void update_I_less_greater () {
    Wa_ = I_less_new_;
    I_less_new_ = NULL;
    Wb_ = I_greater_new_;
    I_greater_new_ = NULL;
  }
#endif

  double startT() const {
    return startT_;
  }
  void set_startT (const double t) {
    startT_ = t;
  }

  double simT() const {
    return simT_;
  }
  void set_simT (const double t) {
    assert (startT_ <= t);
    assert (t <= endT_);
    simT_ = t;
  }

  double endT() const {
    return endT_;
  }
  void set_endT (const double t) {
    assert (startT_ <= t);
    assert (simT_ <= t);
    endT_ = t;
  }
  double edgeT() const {
    return edgeT_;
  }
  void set_edgeT (const double t) {
    assert (startT_ <= t);
    assert (simT_ <= t);
    assert (endT_ <= t);
    edgeT_ = t;
  }
  
  //: For update S to next intersection.
  void set_simT_endT (const double t) {
    assert (startT_ < t);
    assert (t <= endT_);
    simT_ = t;
    endT_ = t;
  }

  //: For update S to the next edge.
  void set_edgeT_endT (const double t) {
    assert (startT_ < t);
    assert (simT_ <= t);
    edgeT_ = t;
    endT_ = t;
  }

  bool b_propagated() const {
    return b_propagated_;
  }
  void set_propagated (const bool b) {
    b_propagated_ = b;
  }
  
  dbmsh3d_gdt_vertex_3d* Snode() const {
    return Snode_;
  }
  dbmsh3d_gdt_vertex_3d* Enode() const {
    return Enode_;
  }
  void set_Enode (const dbmsh3d_gdt_vertex_3d* eN) {
    Enode_ = (dbmsh3d_gdt_vertex_3d*) eN;
  }

  bool bSVE() const {
    return bSVE_;
  }
  void set_bSVE (const bool b) {
    bSVE_ = b;
  }
    
  //: ====== Shock orientation functions ======
  bool _is_contact() const {
    return b2_ == 0;
  }
  bool _is_L_RF() const {
    return Wa_->_is_RF();
  }
  bool _is_R_RF() const {
    return Wb_->_is_RF();
  }
  bool _is_from_edge() const {
    assert (Wa_->_is_RF()==false && Wb_->_is_RF()==false);
    return Wa_->edge() == Wb_->edge();
  }

  bool _is_from_vertex() const {
    assert (Wa_->_is_RF()==false && Wb_->_is_RF()==false);
    return Wa_->edge() != Wb_->edge();
  }

  dbmsh3d_gdt_vertex_3d* _getV() const;

  bool _is_Ia_outward() const;
  bool _is_Ib_outward() const;
  bool _is_Ia_out_Ib_outward() const;
  bool _is_Ia_in_Ib_outward() const;

  bool _is_SV_I_III_SVE_II_IV () const;
  bool _is_SV_SVE_III_IV () const;

  SOT_TYPE _detect_SO_type() const {
    assert (Wa_ && Wb_);

    if (Wa_->_is_RF()) {
      if (Wb_->_is_RF())
        return SOT_SRFRF;
      else
        return SOT_SRF_L;
    }
    else {
      if (Wb_->_is_RF()) {
        return SOT_SRF_R;
      }
      else { //The non-RF cases: SE, SV-, or SVE-.
        if (Wa_->edge() == Wb_->edge())
          return SOT_SE;
        else 
          return SOT_SV_SVE;
      }
    }
  }
  SET_TYPE _detect_SE_type() const;

  bool _is_SRF_L_or_L2() const {
    return Wa_->psrc() == Wb_->sV();
  }
  bool _is_SRF_R_or_R2() const {
    return Wb_->psrc() == Wa_->eV();
  }

  void _get_local_orientation (const SOT_TYPE SOtype, dbmsh3d_halfedge** heC, dbmsh3d_face** curF, 
                               dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;
  void _get_local_orientation2 (const SOT_TYPE SOtype, dbmsh3d_halfedge** heC, dbmsh3d_face** curF, 
                                dbmsh3d_halfedge** heL, dbmsh3d_halfedge** heR, 
                                dbmsh3d_gdt_edge** eC, dbmsh3d_gdt_edge** eL, dbmsh3d_gdt_edge** eR) const;

  gdt_shock* get_intersect_S_via_Wa (bool& Sa_on_left) const;
  gdt_shock* get_intersect_S_via_Wb (bool& Sb_on_right) const;

  //: ====== Handle intrinsic parameters ======
  void _compute_a ();
  void _compute_c ();
  void _compute_b2 () {
    double diff2 = c_*c_ - a_*a_;
    if (_eqM (0, diff2)) {
      //Should reset a_ or c_ here?
      b2_ = 0;
    }
    else {
      assert (diff2 > 0);
      b2_ = diff2;
    }
  }
  
  double _Sl_get_intersect_a (const SET_TYPE seType, const gdt_welm* Wc) const;
  double _Sr_get_intersect_a (const SET_TYPE seType, const gdt_welm* Wc) const;

  //: Compute thetav for SV or SRF-L2 or SRF-R2.
  double _get_SV_thetav () const;

  vgl_point_2d<double> _SV_Ib_coord_to_Ia (const double& thetav, const vgl_point_2d<double>& Pb) const;
  void _SV_LbHb_to_Ia_coord (const double& thetav, double& Lb, double& Hb) const;
  vgl_point_2d<double> _SV_Ia_coord_to_Ib (const double& thetav, const vgl_point_2d<double>& Pa) const;
  void _SV_LaHa_to_Ib_coord (const double& thetav, double& La, double& Ha) const;

  double _get_tau_from_tauB (const double& tauB) const;
  double _get_tauB_from_tau (const double& tau) const;

  double _get_d_from_tau (const double& tau) const;
  double _get_dB_from_tauB (const double& tauB) const;

  void get_Wa_tau_range (double& tau_min, double& tau_max) const;
  void get_Wb_tau_range (double& tau_min, double& tau_max) const;

  // return the positive tau range if this is a leftS: (tauL_asym_min, tauL_s_max)
  void _Sl_get_tau_range (const double& aLi, const double& commonW_mu,
                          double& tauL_s_max, double& tauL_asym_min) const {
    assert (b2_ != 0);
    tauL_asym_min = _L_get_asymptote (aLi, b2_);
    tauL_s_max = _get_tauL_from_dL (aLi, b2_, c_, startT_ - commonW_mu);
  }

  // return the positive tau range if this is a rightS: (value of tau limitation of 
  // starting time and asymptote on the left side.
  void _Sr_get_tau_range (const double& aRi, const double& commonW_mu,
                          double& tauR_s_min, double& tauR_asym_max) const {
    assert (b2_ != 0);
    tauR_asym_max = _R_get_asymptote (aRi, b2_);
    tauR_s_min = _get_tauR_from_dR (aRi, b2_, c_, startT_ - commonW_mu);
  }

  //: ====== Handle S-S intersection parameters ======

  void Sl_get_OL_OC_abc (const gdt_welm* Wc, const double& alphaCL, 
                         vgl_point_2d<double>& OL, vgl_point_2d<double>& OC);
  void Sr_get_OC_OR_abc (const gdt_welm* Wc, const double& alphaCR,
                         vgl_point_2d<double>& OC, vgl_point_2d<double>& OR);

  void Sl_get_OL (const gdt_welm* Wc, const double& alphaCL, vgl_point_2d<double>& OL) const;
  void Sr_get_OR (const gdt_welm* Wc, const double& alphaCR, vgl_point_2d<double>& OR) const;

  void _Sl_get_OL (const gdt_welm* Wc, const double& thetav, vgl_point_2d<double>& OL) const;
  void _Sr_get_OR (const gdt_welm* Wc, const double& thetav, vgl_point_2d<double>& OR) const;

  void _Sl_get_OL_WcRF (const gdt_welm* Wc, const double& alphaOL, vgl_point_2d<double>& OL) const;
  void _Sr_get_OR_WcRF (const gdt_welm* Wc, const double& alphaOR, vgl_point_2d<double>& OR) const;

  void set_Sl_tau_from_tauL (const gdt_welm* Wc, const double& tauL);
  double recover_Sl_tauL_from_tau (const gdt_welm* Wc) const;

  void set_Sr_tau_from_tauR (const gdt_welm* Wc, const double& tauR);
  double recover_Sr_tauR_from_tau (const gdt_welm* Wc) const;

  vgl_point_2d<double> get_ext_pt_2d (const gdt_welm* Wc, 
                                      const vgl_point_2d<double>& Ol,
                                      const vgl_point_2d<double>& Or,                                      
                                      const double& tauC, const double& dC,
                                      const bool b_right_side) const;

  //: ====== Handle S-E functions ======

  //: ====== Compute the upper bound S-E intersection tauE and time ======
  void detect_next_prjE ();

  void _set_next_prjE (const dbmsh3d_gdt_edge* neL, const double& tauL, const double& tL, 
                       const dbmsh3d_gdt_edge* neR, const double& tauR, const double& tR, 
                       const dbmsh3d_gdt_edge* neC, const double& tauC, const double& tC); 

  gdt_welm* _get_nWa_on_prjE () const;
  gdt_welm* _get_nWb_on_prjE () const;

  //: ====== Dynamic Validation functions ======

  //: return true if the given tauP is valid.
  //  return false if it is invalid. Also set timeP to be the corresponding time on shock.
  bool validate_W_event (const gdt_welm* W, const double& tauP, double& timeP) const;

  //: ====== getInfo ======
  virtual void getInfo (vcl_ostringstream& ostrm);
};

void associate_W_S (gdt_welm* W, gdt_shock* S);
void associate_S_W (gdt_shock* S, gdt_welm* W);

void associate_ep_W_S_W (gdt_welm* Wa, gdt_shock* S, gdt_welm* Wb);

void associate_Wa_S (gdt_welm* W, gdt_shock* S);
void associate_S_Wa (gdt_shock* S, gdt_welm* W);
void associate_Wb_S (gdt_welm* W, gdt_shock* S);
void associate_S_Wb (gdt_shock* S, gdt_welm* W);

void SVtoE_associate_Wa_S (gdt_welm* Wa, gdt_shock* S);
void SVtoE_associate_S_Wb (gdt_shock* S, gdt_welm* Wb);

void RFStoE_associate_Wa_S (gdt_welm* degeWa, gdt_welm* nWa, gdt_shock* S);
void RFStoE_associate_S_Wb (gdt_shock* S, gdt_welm* degeWb, gdt_welm* nWb);

void tie_neighbor_S (gdt_shock* S1, const gdt_welm* W, gdt_shock* S2);
void untie_neighbor_S (gdt_shock* S1, const gdt_welm* W, gdt_shock* S2);


//: These 9 functions are used in S-E to detect the next shock-edge intersection.
void next_SRF_L_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                       double& tauL, double& tL);
void next_SRF_L_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                       double& tauR, double& tR);

void next_SRF_R_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                       double& tauL, double& tL);
void next_SRF_R_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                       double& tauR, double& tR);

void next_SE_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                     double& tauL, double& tL);
void next_SE_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                    double& tauR, double& tR);

void next_SV_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eC, 
                    const double& alphaCL, const double& alphaCR, 
                    double& tauC, double& tC);

//: Note that only SV_SVE to eL/eR and SRF-L2 R2 to eC can
//  intersect the from edge twice.
void next_SV_SVE_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eL, const double& alphaLR, 
                        double& tauL, double& tL,
                        const double& existingTau = GDT_HUGE);
void next_SV_SVE_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eR, const double& alphaLR, 
                        double& tauR, double& tR,
                        const double& existingTau = GDT_HUGE);

//: These additional 6 functions together with the previous 9 ones
//  are used in the child shock creation to detect the next S-E intersection.
void next_SRF2_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                      const dbmsh3d_gdt_edge* eL,
                      double& tauL, double& tL);
void next_SRF2_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                      const dbmsh3d_gdt_edge* eR,
                      double& tauR, double& tR);

void next_SRF_L2_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eR, const double& alphaCR,
                        double& tauR, double& tR);
void next_SRF_R2_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eL, const double& alphaCL,
                        double& tauL, double& tL);

void next_SRF_L2_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eC,
                        double& tauC, double& tC);
void next_SRF_R2_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eC,
                        double& tauC, double& tC);

void _determine_min_max_tau (const double& A_min, const double& A_max, 
                             const double& B_min, const double& B_max, 
                             const double& e_len,
                             double& tau_min, double& tau_max);

double _solve_tau_fix_bnd_value (const double& A_nL, const double& A_nH, const double& muA,
                                 const double& B_nL, const double& B_nH, const double& muB,
                                 double& tau_min, double& tau_max,
                                 const double& existingTau = GDT_HUGE);

#endif



