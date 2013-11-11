//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>

#include <dbmsh3d/dbmsh3d_halfedge.h>

#include <dbgdt3d/dbgdt3d_interval.h>
#include <dbgdt3d/dbgdt3d_shock.h>
#include <dbgdt3d/dbgdt3d_edge.h>
#include <dbgdt3d/dbgdt3d_vertex.h>

gdt_shock::gdt_shock (const int id,
                      const gdt_welm* Wa, const gdt_welm* Wb, 
                      const dbmsh3d_gdt_edge* prje, const double& tauE,
                      const double& startT, const double& edgeT,
                      const dbmsh3d_gdt_vertex_3d* Snode) 
{
  id_             = (int) id;

  Wa_             = (gdt_welm*) Wa;
  Wb_             = (gdt_welm*) Wb;
  
  assert (startT < edgeT);
  startT_         = startT;
  simT_           = startT;
  endT_           = edgeT;
  edgeT_          = edgeT;

  b_propagated_   = false;

  bSVE_           = false;

  Sa_ = NULL;
  Sb_ = NULL;

  // The intrinsic starting parameters.
  Snode_          = (dbmsh3d_gdt_vertex_3d*) Snode;
  Enode_          = NULL;

  _compute_a ();
  _compute_c ();
  _compute_b2 ();

  tau_            = INVALID_TAU;

  prjE_           = (dbmsh3d_gdt_edge*) prje;
  tauE_           = tauE;

  gdt_selm* selm  = new gdt_selm (Wa_, Wb_, SELM_TAU_NONE, INVALID_TAU);
  selms_.push_back (selm);
}

gdt_shock::~gdt_shock ()
{
  //Go through the SElms_[] list and release memory.
  for (unsigned int i=0; i<selms_.size(); i++) {
    gdt_selm* selm = selms_[i];
    delete selm;
  }
}

dbmsh3d_gdt_vertex_3d* gdt_shock::_getV() const
{
  assert (_is_from_vertex());
  return (dbmsh3d_gdt_vertex_3d*) Es_sharing_V (Wa_->edge(), Wb_->edge());
}

bool gdt_shock::_is_Ia_outward() const 
{
  assert (Wa_ && Wb_);
  return Wb_->edge()->is_V_incident (Wa_->edge()->sV());
}

bool gdt_shock::_is_Ib_outward() const
{
  assert (Wa_ && Wb_);
  return Wa_->edge()->is_V_incident (Wb_->edge()->sV());
}

bool gdt_shock::_is_Ia_out_Ib_outward() const
{
  return Wa_->edge()->sV() == Wb_->edge()->sV();
}

bool gdt_shock::_is_Ia_in_Ib_outward() const
{
  return Wa_->edge()->eV() == Wb_->edge()->sV();
}

bool gdt_shock::_is_SV_I_III_SVE_II_IV () const
{
  if (bSVE_) {
    //for SVE_II, SVE_IV: Ia outward.
    //for SVE_I, SVE_III: Ia inward.
    return _is_Ia_outward();
  }
  else {
    //for SV_II, SV_IV: Ia outward.
    //for SV_I, SV_III: Ia inward.
    return _is_Ia_outward() == false;
  }
}

//: return true for both Ia Ib inward or outward.
//  true for SV_III, SVE_III, SV_IV, SVE_IV.
//  other return false.
bool gdt_shock::_is_SV_SVE_III_IV () const
{
  if (_is_L_RF() || _is_R_RF() || _is_from_edge())
    return false;
  if (_is_Ia_outward())
    return _is_Ia_out_Ib_outward();
  else
    return !_is_Ia_in_Ib_outward();
}

SET_TYPE gdt_shock::_detect_SE_type() const 
{
  assert (Wa_ != NULL && Wb_ != NULL);

  if (Wa_->_is_RF()) {
    if (Wb_->_is_RF())
      return SET_SRFRF;
    else { //Wb non-RF: SRF-L or SRF-L2
      if (_is_SRF_L_or_L2())
        return SET_SRF_L;
      else {
        assert (Wa_->psrc() != Wb_->eV());
        return SET_SRF_L2;
      }
    }
  }
  else {
    if (Wb_->_is_RF()) { //Wa non-RF: SRF-R or SRF-R2
      if (_is_SRF_R_or_R2())
        return SET_SRF_R;
      else {
        assert (Wb_->psrc() != Wa_->sV());
        return SET_SRF_R2;
      }
    }
    else { //The non-RF cases: SE, SV-, or SVE-.
      if (Wa_->edge() == Wb_->edge())
        return SET_SE;
      else {
        if (bSVE_) { //SVE_ I, II, III, IV
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward())
              return SET_SVE_IV;
            else
              return SET_SVE_II;
          }
          else {
            if (_is_Ia_in_Ib_outward())
              return SET_SVE_I;
            else
              return SET_SVE_III;
          }
        }
        else { //SV_ I, II, III, IV
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward())
              return SET_SV_IV;
            else
              return SET_SV_II;
          }
          else {
            if (_is_Ia_in_Ib_outward())
              return SET_SV_I;
            else
              return SET_SV_III;
          }
        }
      }
    }
  }
}

void gdt_shock::_get_local_orientation (const SOT_TYPE SOtype, 
                                        dbmsh3d_halfedge** curF_he, 
                                        dbmsh3d_face** curF, 
                                        dbmsh3d_gdt_edge** eC, 
                                        dbmsh3d_gdt_edge** eL, 
                                        dbmsh3d_gdt_edge** eR) const
{
  SOT_TYPE type = _detect_SO_type();

  switch (type) {
  case SOT_SRFRF:

  break;
  case SOT_SRF_L:
  break;
  case SOT_SRF_R:
  break;
  case SOT_SE:
  break;
  case SOT_SV_SVE:
  break;
  default:
    assert (0);
  break;
  }

  assert (*eC != *eL && *eC != *eR);
}

void gdt_shock::_get_local_orientation2 (const SOT_TYPE SOtype,
                                         dbmsh3d_halfedge** heC, 
                                         dbmsh3d_face** curF, 
                                         dbmsh3d_halfedge** heL, 
                                         dbmsh3d_halfedge** heR, 
                                         dbmsh3d_gdt_edge** eC, 
                                         dbmsh3d_gdt_edge** eL, 
                                         dbmsh3d_gdt_edge** eR) const
{
  switch (SOtype) {
  case SOT_SRFRF:
    *heL = Wb_->he();
    *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
    *heR = Wa_->he();
    *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
    *curF = (*heL)->face();
    *heC = (*curF)->find_next_bnd_HE (Wa_->psrc(), *heL);
    *eC = (dbmsh3d_gdt_edge*) (*heC)->edge();
  break;
  case SOT_SRF_L:
    *heC = Wb_->he()->pair();
    *curF = (*heC)->face();
    *eC = Wb_->edge();
    *heL = (*curF)->find_next_bnd_HE ((*eC)->sV(), *heC);
    *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
    *heR = (*curF)->find_next_bnd_HE ((*eC)->eV(), *heC);
    *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
  break;
  case SOT_SRF_R:
    *heC = Wa_->he()->pair();
    *curF = (*heC)->face();
    *eC = Wa_->edge();
    *heL = (*curF)->find_next_bnd_HE ((*eC)->sV(), *heC);
    *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
    *heR = (*curF)->find_next_bnd_HE ((*eC)->eV(), *heC);
    *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
  break;
  break;
  case SOT_SE:
    *heC = Wb_->he()->pair();
    *curF = (*heC)->face();
    *eC = Wb_->edge();
    *heL = (*curF)->find_next_bnd_HE ((*eC)->sV(), *heC);
    *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
    *heR = (*curF)->find_next_bnd_HE ((*eC)->eV(), *heC);
    *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
  break;
  case SOT_SV_SVE:
    *heL = Wa_->he()->pair();
    *eL = (dbmsh3d_gdt_edge*) (*heL)->edge();
    *heR = Wb_->he()->pair();
    *eR = (dbmsh3d_gdt_edge*) (*heR)->edge();
    
    *curF = (*heL)->face();
    if ((*eL)->is_V_incident ((*eR)->sV()))
      *heC = (*curF)->find_next_bnd_HE ((*eR)->eV(), *heR);
    else
      *heC = (*curF)->find_next_bnd_HE ((*eR)->sV(), *heR);
    *eC = (dbmsh3d_gdt_edge*) (*heC)->edge();    
  break;
  default:
    assert (0);
  break;
  }

  assert (*eC != *eL && *eC != *eR);
}

//: Note that we simulate the shock segment, not the shock element.
//  While the W->S association is maintained, depending on the propagation, 
//  the association of S->W may not be consistent.
//  So need to double check the sharing wavefront in finding the neighboring shocks.
gdt_shock* gdt_shock::get_intersect_S_via_Wa (bool& Sa_on_left) const
{
  gdt_shock* S;
  S = Wa_->otherS (this);
  Sa_on_left = Wa_->Sr() == this;

  //Return the neighboring shock S only if they share the wavefront Wa.
  assert (S != this);
  if (S == NULL)
    return NULL;
  if (S->_sharingW (Wa_))
    return S;
  else
    return NULL;
}

gdt_shock* gdt_shock::get_intersect_S_via_Wb (bool& Sb_on_right) const
{
  gdt_shock* S;
  S = Wb_->otherS (this);
  Sb_on_right = Wb_->Sl() == this;

  //Return the neighboring shock S only if they share the wavefront Wa.
  if (S == NULL)
    return NULL;
  if (S->_sharingW (Wb_))
    return S;
  else
    return NULL;
}

void gdt_shock::replaceW (const gdt_welm* W, const gdt_welm* newW)
{
  //If one of Wa/Wb is W, replace with newW.
  if (Wa_ == W)
    Wa_ = (gdt_welm*) newW;
  if (Wb_ == W)
    Wb_ = (gdt_welm*) newW;

  //Also search and replace in the selm_[] structure.
  for (unsigned int i=0; i<selms_.size(); i++) {
    gdt_selm* selm = selms_[i];

    if (selm->Wa_ == W)
      selm->Wa_ = (gdt_welm*) newW;
    if (selm->Wb_ == W)
      selm->Wb_ = (gdt_welm*) newW;
  }
}

// ###############################################################

//: a_ = mu_B - mu_A
void gdt_shock::_compute_a ()
{
  double a = Wb_->mu() - Wa_->mu();
  a_ = a* 0.5;
}

//: c_ = dist(OL, OR) in the local coord. system.
void gdt_shock::_compute_c () 
{
  double c;
  if (Wa_->_is_RF() && Wb_->_is_RF()) { //SRF2
    c = vgl_distance (Wa_->psrc()->pt(), Wb_->psrc()->pt());
  }
  else if (Wa_->_is_RF()) { //SRF-L or SRF-L2
    if (_is_SRF_L_or_L2()) //SRF-L
      c = vnl_math_hypot (Wb_->L(), Wb_->H());
    else { //SRF-L2
      assert (Wa_->edge() == Wb_->edge());
      //Wa.psrc in the coordinate of Wb is (La, Ha). Wa.psrc is (La, -Ha).
      double La = Wa_->L();
      double Ha = Wa_->H();
      c = vnl_math_hypot (Wb_->L() - La, -Wb_->H() - Ha);
    }
  }
  else if (Wb_->_is_RF()) { //SRF-R or SRF-R2
    if (_is_SRF_R_or_R2()) //SRF-R
      c = vnl_math_hypot (Wa_->edge()->len() - Wa_->L(), Wa_->H());
    else { //SRF-R2
      assert (Wa_->edge() == Wb_->edge());
      //Wb.psrc in the coordinate of Wa is (Lb, Hb). Wa.psrc is (La, -Ha).
      double Lb = Wb_->L();
      double Hb = Wb_->H();
      c = vnl_math_hypot (Wa_->L() - Lb, -Wa_->H() - Hb);
    }
  }
  else if (_is_from_edge())
    c = vnl_math_hypot (Wa_->L() - Wb_->L(), Wa_->H() - Wb_->H());
  else { //SO_VERTEX
    const double thetav = _get_SV_thetav ();
    double Lb = Wb_->L();
    double Hb = Wb_->H();
    _SV_LbHb_to_Ia_coord (thetav, Lb, Hb);
    c = vnl_math_hypot (Wa_->L() - Lb, Wa_->H() - Hb);
  }
  c_ = c * 0.5;
}

//: Compute thetav for SV or SRF-L2 or SRF-R2.
double gdt_shock::_get_SV_thetav () const
{
  assert (!Wa_->_is_RF());
  assert (!Wb_->_is_RF());
  assert (_is_from_vertex());

  double len_a = Wa_->edge()->len();
  double len_b = Wb_->edge()->len();

  double len_c;
  dbmsh3d_vertex *v, *va, *vb;
  if (_is_Ia_outward()) {
    v = Wa_->edge()->sV();
    va = Wa_->edge()->eV();
    vb = Wb_->edge()->other_V (v);
    len_c = vgl_distance (va->pt(), vb->pt());
  }
  else {
    v = Wa_->edge()->eV();
    va = Wa_->edge()->sV();
    vb = Wb_->edge()->other_V (v);
    len_c = vgl_distance (va->pt(), vb->pt());
  }

  return m2t_compute_angle_cl (len_a, len_b, len_c);
}

// #############################################################

vgl_point_2d<double> gdt_shock::_SV_Ib_coord_to_Ia (const double& thetav, 
                                              const vgl_point_2d<double>& Pb) const
{
  assert (!Wa_->_is_RF());
  assert (!Wb_->_is_RF());
  assert (_is_from_vertex());
  assert (0 < thetav && thetav < vnl_math::pi);

  const double cos_theta = vcl_cos (thetav);
  const double sin_theta = vcl_sin (thetav);
  double xa, ya, xb, yb;

  if (_is_Ia_outward()) { //SO_LVERTEX
    xb = _is_Ia_out_Ib_outward() ? Pb.x() : Wb_->edge()->len() - Pb.x();
    yb = -Pb.y();

    // rotate (xb, yb) CCW by thetav to be (xa, ya)
    xa =  cos_theta*xb - sin_theta*yb;
    ya =  sin_theta*xb + cos_theta*yb;
  }
  else { //SO_RVERTEX
    xb = _is_Ia_in_Ib_outward() ? Pb.x() : Wb_->edge()->len() - Pb.x();
    yb = Pb.y();

    // rotate (xb, yb) by thetav and translate to be (xa, ya)
    xa = -cos_theta*xb - sin_theta*yb + Wa_->edge()->len();
    ya =  sin_theta*xb - cos_theta*yb;
  }

  return vgl_point_2d<double> (xa, ya);
}

void gdt_shock::_SV_LbHb_to_Ia_coord (const double& thetav, double& Lb, double& Hb) const
{
  assert (!Wa_->_is_RF());
  assert (!Wb_->_is_RF());
  assert (_is_from_vertex());
  assert (0 < thetav && thetav < vnl_math::pi);

  const double cos_theta = vcl_cos (thetav);
  const double sin_theta = vcl_sin (thetav);
  double xa, ya, xb, yb;

  if (_is_Ia_outward()) { //SO_LVERTEX
    xb = _is_Ia_out_Ib_outward() ? Lb : Wb_->edge()->len() - Lb;
    yb = Hb;

    // rotate (xb, yb) CCW by thetav to be (xa, ya)
    xa =  cos_theta*xb - sin_theta*yb;
    ya =  sin_theta*xb + cos_theta*yb;
  }
  else { //SO_RVERTEX
    xb = _is_Ia_in_Ib_outward() ? Lb : Wb_->edge()->len() - Lb;
    yb = -Hb;

    // rotate (xb, yb) by thetav and translate to be (xa, ya)
    xa = -cos_theta*xb - sin_theta*yb + Wa_->edge()->len();
    ya =  sin_theta*xb - cos_theta*yb;
  }

  // return transformed value
  Lb = xa;
  Hb = -ya;
}

vgl_point_2d<double> gdt_shock::_SV_Ia_coord_to_Ib (const double& thetav, 
                                               const vgl_point_2d<double>& Pa) const
{
  assert (!Wa_->_is_RF());
  assert (!Wb_->_is_RF());
  assert (_is_from_vertex());
  assert (0 < thetav && thetav < vnl_math::pi);

  const double cos_theta = vcl_cos (thetav);
  const double sin_theta = vcl_sin (thetav);
  double xa, ya, xb, yb;

  if (_is_Ia_outward()) { //SO_LVERTEX
    xa = Pa.x();
    ya = Pa.y();

    xb =  cos_theta*xa + sin_theta*ya;
    yb = -sin_theta*xa + cos_theta*ya;

    xb = _is_Ia_out_Ib_outward() ? xb : Wb_->edge()->len() - xb;
    yb = -yb;
  }
  else { //SO_RVERTEX
    xa = Wa_->edge()->len() - Pa.x();
    ya = - Pa.y();

    xb =  cos_theta*xa - sin_theta*ya;
    yb =  sin_theta*xa + cos_theta*ya;

    xb = _is_Ia_in_Ib_outward() ? xb : Wb_->edge()->len() - xb;
  }

  return vgl_point_2d<double> (xb, yb);
}

void gdt_shock::_SV_LaHa_to_Ib_coord (const double& thetav, double& La, double& Ha) const
{
  assert (!Wa_->_is_RF());
  assert (!Wb_->_is_RF());
  assert (_is_from_vertex());
  assert (0 < thetav && thetav < vnl_math::pi);

  const double cos_theta = vcl_cos (thetav);
  const double sin_theta = vcl_sin (thetav);
  double xa, ya, xb, yb;

  if (_is_Ia_outward()) { //SO_LVERTEX
    xa = La;
    ya = -Ha;

    xb =  cos_theta*xa + sin_theta*ya;
    yb = -sin_theta*xa + cos_theta*ya;

    La = _is_Ia_out_Ib_outward() ? xb : Wb_->edge()->len() - xb;
    Ha = yb;
  }
  else { //SO_RVERTEX
    xa = Wa_->edge()->len() - La;
    ya = Ha;

    xb =  cos_theta*xa - sin_theta*ya;
    yb =  sin_theta*xa + cos_theta*ya;

    La = _is_Ia_in_Ib_outward() ? xb : Wb_->edge()->len() - xb;
    Ha = -yb;
  }
}

// #############################################################

//: Convert tauB to tau. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//
double gdt_shock::_get_tau_from_tauB (const double& tauB) const
{
  assert (!_is_contact());

  if (_is_L_RF() || _is_R_RF() || _is_from_edge()) //tau = tauR, tauB = tauRR
    return _get_tauR_from_tauRR (a_, b2_, c_, tauB);
  else {
    if (_is_SV_I_III_SVE_II_IV ()) //tau = tauR, tauB = tauRR
      return _get_tauR_from_tauRR (a_, b2_, c_, tauB);
    else //tau = tauL, tauB = tauLL
      return _get_tauL_from_tauLL (a_, b2_, c_, tauB);
  }
}

//: Convert tau to tauB. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//
double gdt_shock::_get_tauB_from_tau (const double& tau) const
{
  assert (!_is_contact());

  if (_is_L_RF() || _is_R_RF() || _is_from_edge()) //tauB = tauRR, tau = tauR
    return _get_tauRR_from_tauR (a_, b2_, c_, tau);
  else {
    if (_is_SV_I_III_SVE_II_IV ()) //tauB = tauRR, tau = tauR
      return _get_tauRR_from_tauR (a_, b2_, c_, tau);
    else //tauB = tauLL, tau = tauL
      return _get_tauLL_from_tauL (a_, b2_, c_, tau); 
  }
}

//: Get local distance from tau. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//
double gdt_shock::_get_d_from_tau (const double& tau) const
{
  assert (b2_ != 0);

  if (_is_L_RF() || _is_R_RF() || _is_from_edge())
    return _get_dR_from_tauR (a_, b2_, c_, tau);
  else {
    if (_is_SV_I_III_SVE_II_IV ())
      return _get_dR_from_tauR (a_, b2_, c_, tau);
    else
      return _get_dL_from_tauL (a_, b2_, c_, tau);
  }
}

//: Get local distance from tauB. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//
double gdt_shock::_get_dB_from_tauB (const double& tauB) const
{
  assert (b2_ != 0);

  if (_is_L_RF() || _is_R_RF() || _is_from_edge())
    return _get_dRR_from_tauRR (a_, b2_, c_, tauB);
  else {
    if (_is_SV_I_III_SVE_II_IV ())
      return _get_dRR_from_tauRR (a_, b2_, c_, tauB);
    else
      return _get_dLL_from_tauLL (a_, b2_, c_, tauB);
  }
}

//: Get tau range for Wa. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//  Need to use the full range from 0 or pi, since the parent shock can also invalidate events.
//
void gdt_shock::get_Wa_tau_range (double& tau_min, double& tau_max) const 
{
  assert (!_is_contact());
  //unused const double ds = startT_ - Wa_->mu();

  if (_is_L_RF() || _is_R_RF() || _is_from_edge()) { //R-side formula
    tau_min = 0;
    tau_max = _R_get_asymptote (a_, b2_);
  }
  else {
    if (_is_SV_I_III_SVE_II_IV ()) { //R-side formula
      tau_min = 0;
      tau_max = _R_get_asymptote (a_, b2_);
    }
    else { //L-side formula
      tau_min = _L_get_asymptote (a_, b2_);
      tau_max = vnl_math::pi;
    }      
  }
  assert (tau_min < tau_max);
}

//: Get tau range for Wa. Among the 14 types of shocks,
//   - for SE, SRF2, SRF-L, SRF-L2, SRF-R, SRF-R2, use the R-side formula with aR = a.
//   - for SV-I, SV-III, SVE-II, SVE-IV, use the R-side formula with aR = a.
//   - for SV-II, SV-IV, SVE-I, SVE-III, use the L-side formula with aL = a.
//  Need to use the full range from 0 or pi, since the parent shock can also invalidate events.
//
void gdt_shock::get_Wb_tau_range (double& tau_min, double& tau_max) const 
{
  assert (!_is_contact());
  //unused const double ds = startT_ - Wb_->mu();

  if (_is_L_RF() || _is_R_RF() || _is_from_edge()) { //R-side formula
    tau_min = _R_get_asymptote (a_, b2_);  
    tau_max = vnl_math::pi;
  }
  else {
    if (_is_SV_I_III_SVE_II_IV ()) { //R-side formula
      tau_min = _R_get_asymptote (a_, b2_);  
      tau_max = vnl_math::pi;
    }
    else { //L-side formula
      tau_min = 0;
      tau_max = _L_get_asymptote (a_, b2_);
    }
  }
  assert (tau_min < tau_max);
}

// #############################################################

//: Note on the intrinsic coordinate system for shock intersection.
//
//  Use Wc to orient the local coordinate system.
//    - Use Wc->sV as origin.
//    - Use Wc->edge's direction as x-axis.
//  - For Wc being RF, need to 
//    - project WL (L, H) into (WL_nL, WL_nH) to get OL (WL_nL, -WL_nH).
//    - project WR (L, H) into (WR_nL, WR_nH) to get OR (WR_nL, -WR_nH).
//    - OC (WC_L, -WC_H).
//
void gdt_shock::Sl_get_OL_OC_abc (const gdt_welm* Wc, const double& alphaCL, 
                                  vgl_point_2d<double>& OL, vgl_point_2d<double>& OC)
{
  double old_a = a_;
  double old_b2 = b2_;
  double old_c = c_;

  _compute_a();
  assert (vcl_fabs(old_a) == vcl_fabs(a_));
  _compute_c();
  assert (_eqM (old_c, c_));
  _compute_b2();
  assert (_eqM (old_b2, b2_));

  OC.set (Wc->L(), -Wc->H());

  //Compute OL  
  Sl_get_OL (Wc, alphaCL, OL);

  #if GDT_DEBUG_MSG
  // Assert dist(OL, OC) = 2c.
  double dist_OLOC = vgl_distance (OL, OC);
  assert (_eqM (dist_OLOC, c_*2));
  #endif
}

// Compute the intrinsic parameters for the right shock
void gdt_shock::Sr_get_OC_OR_abc (const gdt_welm* Wc, const double& alphaCR,
                                  vgl_point_2d<double>& OC, vgl_point_2d<double>& OR)
{
  double old_a = a_;
  double old_b2 = b2_;
  double old_c = c_;

  _compute_a();
  assert (vcl_fabs(old_a) == vcl_fabs(a_));
  _compute_c();
  assert (_eqM (old_c, c_));
  _compute_b2();
  assert (_eqM (old_b2, b2_));

  OC.set (Wc->L(), -Wc->H());

  //Compute OR
  Sr_get_OR (Wc, alphaCR, OR);

  #if GDT_DEBUG_MSG
  // Assert dist(OC, OR) = 2c.
  double dist_OCOR = vgl_distance (OC, OR);
  assert (_eqM (dist_OCOR, c_*2));
  #endif
}


void gdt_shock::Sl_get_OL (const gdt_welm* Wc, const double& alphaCL, 
                           vgl_point_2d<double>& OL) const
{
  if (Wc->_is_RF())
    _Sl_get_OL_WcRF (Wc, alphaCL, OL);
  else {
    double thetav;
    if (!Wa_->_is_RF() && !Wb_->_is_RF() && !_is_from_edge())
      thetav = _get_SV_thetav ();

    _Sl_get_OL (Wc, thetav, OL);
  }
}

void gdt_shock::Sr_get_OR (const gdt_welm* Wc, const double& alphaCR, 
                           vgl_point_2d<double>& OR) const
{
  if (Wc->_is_RF())
    _Sr_get_OR_WcRF (Wc, alphaCR, OR);
  else {
    double thetav;
    if (!Wa_->_is_RF() && !Wb_->_is_RF() && _is_from_vertex())
      thetav = _get_SV_thetav ();

    _Sr_get_OR (Wc, thetav, OR);
  }
}

void gdt_shock::_Sl_get_OL (const gdt_welm* Wc, const double& thetav,
                            vgl_point_2d<double>& OL) const
{
  assert (Wc->_is_RF() == false);
  assert (Wb_->_is_RF() == false);

  if (Wa_->_is_RF()) {    
    if (_is_SRF_L_or_L2()) //For SRF-L, OL = (0,0)
      OL.set (0, 0);
    else //For SRF-L2, OL is vA in the coord of eC = (Wa.L, Wa.H)
      OL.set (Wa_->L(), Wa_->H());
  }
  else if (_is_from_edge()) { //For SE
    OL.set (Wa_->L(), -Wa_->H());
  }
  else { //For SV
    double L, H;
    if (Wa_ == Wc) {
      L = Wb_->L();
      H = Wb_->H();
      _SV_LbHb_to_Ia_coord (thetav, L, H);
    }
    else {
      assert (Wb_ == Wc);
      L = Wa_->L();
      H = Wa_->H();
      _SV_LaHa_to_Ib_coord (thetav, L, H);
    }
    OL.set (L, -H);
  }
}

void gdt_shock::_Sr_get_OR (const gdt_welm* Wc, const double& thetav,
                            vgl_point_2d<double>& OR) const
{
  assert (Wc->_is_RF() == false);
  assert (Wa_->_is_RF() == false);

  if (Wb_->_is_RF()) {
    if (_is_SRF_R_or_R2()) //For SRF-R, OR = (Wa.len, 0)
      OR.set (Wa_->edge()->len(), 0);
    else //For SRF-R2, OR is vB in the coord of eC = (Wb.L, Wb.H)
      OR.set (Wb_->L(), Wb_->H());
  }
  else if (_is_from_edge()) { //For SE
    OR.set (Wb_->L(), -Wb_->H());
  }
  else { //For SV
    double L, H;
    if (Wa_ == Wc) {
      L = Wb_->L();
      H = Wb_->H();
      _SV_LbHb_to_Ia_coord (thetav, L, H);
    }
    else {
      assert (Wb_ == Wc);
      L = Wa_->L();
      H = Wa_->H();
      _SV_LaHa_to_Ib_coord (thetav, L, H);
    }
    OR.set (L, -H);
  }
}

// #############################################################

// get_left_OL for the case when Wc is RF. 5 cases is possible:
//   - SRFRF as leftS, RF- as Wc
//   - SRFRF as leftS, RF+ as Wc
//   - SRF-L as leftS, RF- as Wc
//   - SRF-R as leftS, RF+ as Wc
//   - SRF-L2 as leftS, RF- as Wc 
//
void gdt_shock::_Sl_get_OL_WcRF (const gdt_welm* Wc, const double& alphaOL,
                                 vgl_point_2d<double>& OL) const
{
  assert (Wc->_is_RF());
  const dbmsh3d_gdt_edge* eC = Wc->edge(); 
  double WL_nL, WL_nH;
  
  if (Wa_->_is_RF() && Wb_->_is_RF()) {
    if (Wa_ == Wc) { //SRFRF: Wb as WL
      if (Wb_->psrc() == Wa_->sV())
        OL.set (0, 0);
      else
        OL.set (Wa_->edge()->len(), 0);
    }
    else { //SRFRF: Wa as WL
      if (Wa_->psrc() == Wb_->sV())
        OL.set (0, 0);
      else
        OL.set (Wb_->edge()->len(), 0);
    }    
    return;
  }
  else if (Wa_ == Wc) { //SRF-L or SRF-L2: Wb_ as WL
    if (_is_SRF_L_or_L2()) { //SRF-L: Project Wb (L, H) into (WL_nL, WL_nH)
      if (Wb_->edge()->sV() == eC->sV()) //L-proj to eC      
        Wb_->get_left_nL_nH (alphaOL, eC, WL_nL, WL_nH);
      else //R-proj to eC
        Wb_->get_right_nL_nH (alphaOL, eC, WL_nL, WL_nH);
    }
    else { //SRF-L2: Wb (L, -H) is (WL_nL, WL_nH)
      OL.set (Wb_->L(), Wb_->H());
      return;
    }
  }
  else { //SRF-R: Wa_ as WL: Project Wa (L, H) into (WL_nL, WL_nH)
    assert (Wb_ == Wc);
    assert (_is_SRF_R_or_R2());
    if (Wa_->edge()->sV() == eC->sV()) //L-proj to eC      
      Wa_->get_left_nL_nH (alphaOL, eC, WL_nL, WL_nH);
    else //R-proj to eC
      Wa_->get_right_nL_nH (alphaOL, eC, WL_nL, WL_nH);
  }

  OL.set (WL_nL, -WL_nH);
}

// get_left_OR for the case when Wc is RF. 5 cases is possible:
//   - SRFRF as rightS, RF- as Wc
//   - SRFRF as rightS, RF+ as Wc
//   - SRF-L as rightS, RF- as Wc
//   - SRF-R as rightS, RF+ as Wc
//   - SRF-R2 as rightS, RF+ as Wc 
//
void gdt_shock::_Sr_get_OR_WcRF (const gdt_welm* Wc, const double& alphaOR,
                                 vgl_point_2d<double>& OR) const
{
  assert (Wc->_is_RF());
  const dbmsh3d_gdt_edge* eC = Wc->edge(); 
  double WR_nL, WR_nH;

  if (Wa_->_is_RF() && Wb_->_is_RF()) {
    if (Wa_ == Wc) { //SRFRF: Wb as WL
      if (Wb_->psrc() == Wa_->sV())
        OR.set (0, 0);
      else
        OR.set (Wa_->edge()->len(), 0);
    }
    else { //SRFRF: Wa as WL
      if (Wa_->psrc() == Wb_->sV())
        OR.set (0, 0);
      else
        OR.set (Wb_->edge()->len(), 0);
    }    
    return;
  }
  if (Wa_ == Wc) { //SRF-L: Wb_ as WR: Project Wb (L, H) into (WR_nL, WR_nH)
    assert (_is_SRF_L_or_L2());
    if (Wb_->edge()->eV() == eC->eV()) //R-proj to eC      
      Wb_->get_right_nL_nH (alphaOR, eC, WR_nL, WR_nH);
    else //L-proj to eC
      Wb_->get_left_nL_nH (alphaOR, eC, WR_nL, WR_nH);
  }
  else { //SRF-R or SRF-R2: Wa_ as WR
    assert (Wb_ == Wc);
    if (_is_SRF_R_or_R2()) { //SRF-R: Project Wa (L, H) into (WR_nL, WR_nH)
      if (Wa_->edge()->eV() == eC->eV()) //R-proj to eC      
        Wa_->get_right_nL_nH (alphaOR, eC, WR_nL, WR_nH);
      else //L-proj to eC
        Wa_->get_left_nL_nH (alphaOR, eC, WR_nL, WR_nH);
    }
    else { //SRF-R2: Wa (L, -H) is (WR_nL, WR_nH)
      OR.set (Wa_->L(), Wa_->H());
      return;
    }
  }
  OR.set (WR_nL, -WR_nH);
}

// #############################################################

//: For a left intersection shock, convert the intersection tauL
//  to the intrinsic shock tau
void gdt_shock::set_Sl_tau_from_tauL (const gdt_welm* Wc, const double& tauL)
{
  assert (!_is_contact());

  if (_is_L_RF()) {
    if (_is_R_RF()) //SRFRF: tauB = tauL
      tau_ = _get_tau_from_tauB (tauL);
    else {      
      if (Wa_ == Wc) //SRF-L, SRF-L2 as leftS, RFa as Wc: tau = pi-tauL
        tau_ = vnl_math::pi - tauL;
      else //SRF-L, SRF-L2 as elftS, Wb as Wc: tauB = tauL
        tau_ = _get_tau_from_tauB (tauL);
    }
  }
  else {
    if (_is_R_RF()) //SRF-R, SRF-R2 as leftS: tauB = tauL
      tau_ = _get_tau_from_tauB (tauL);
    else {
      if (_is_from_edge()) //SE: tauB = tauL
        tau_ = _get_tau_from_tauB (tauL);
      else {
        if (bSVE_) {
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) //SVE-IV: can not be leftS
              assert (0);
            else //SVE-II: tauB = tauL
              tau_ = _get_tau_from_tauB (tauL);
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SVE-I: tau = tauL
              tau_ = tauL;
            else { //SVE-III:
              if (Wa_ == Wc) //SVE-III Wa as Wc: tau = tauL
                tau_ = tauL;
              else //SVE-III Wb as Wc: tauB = pi-tauL
                tau_ = _get_tau_from_tauB (vnl_math::pi-tauL);
            }
          }
        }
        else { //SV:
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SV-IV: 
              if (Wa_ == Wc) //SV-IV Wa as Wc: tau = tauL
                tau_ = tauL;
              else //SV-IV Wb as Wc: tauB = pi-tauL
                tau_ = _get_tau_from_tauB (vnl_math::pi-tauL);
            }
            else //SV-II: tau = tauL
              tau_ = tauL;
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SV-I: tauB = tauL
              tau_ = _get_tau_from_tauB (tauL);
            else //SV-III: only Wb outward is possible.
              assert (0);
          }
        }
      }
    }
  }
}

//: For a left intersection shock, convert the intrinsic shock tau to
//  the intersection tauL
double gdt_shock::recover_Sl_tauL_from_tau (const gdt_welm* Wc) const
{
  assert (!_is_contact());

  if (_is_L_RF()) {
    if (_is_R_RF()) //SRFRF: tauL = tauB
      return _get_tauB_from_tau (tau_);
    else {      
      if (Wa_ == Wc) //SRF-L, SRF-L2 as leftS, RFa as Wc: tauL = pi-tau
        return vnl_math::pi - tau_; 
      else //SRF-L, SRF-L2 as leftS, Wb as Wc: tauL = tauB
        return _get_tauB_from_tau (tau_);
    }
  }
  else {
    if (_is_R_RF()) //SRF-R, SRF-R2: tauL = tauB
      return _get_tauB_from_tau (tau_);
    else {
      if (_is_from_edge()) //SE: tauL = tauB
        return _get_tauB_from_tau (tau_);
      else {
        if (bSVE_) {
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SVE-IV: can not be leftS
              assert (0); 
              return INVALID_TAU;
            }
            else //SVE-II: tauL = tauB
              return _get_tauB_from_tau (tau_);
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SVE-I: tauL = tau
              return tau_;
            else { //SVE-III:
              if (Wa_ == Wc) //SVE-III Wa as Wc: tauL = tau
                return tau_;
              else //SVE-III Wb as Wc: tauL = pi-tauB
                return vnl_math::pi - _get_tauB_from_tau (tau_);
            }
          }
        }
        else { //SV:
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SV-IV:
              if (Wa_ == Wc) //SV-IV Wa as Wc: tauL = tau
                return tau_;
              else //SV-IV Wb as Wc: tauL = pi-tauB
                return vnl_math::pi - _get_tauB_from_tau (tau_);
            }
            else //SV-II: tauL = tau
              return tau_;
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SV-I: tauL = tauB
              return _get_tauB_from_tau (tau_);
            else { //SV-III: only Wb outward is possible.
              assert (0);
              return INVALID_TAU;
            }
          }
        }
      }
    }
  }
}

//: For a right intersection shock, convert the intersection tauR
//  to the intrinsic shock tau
void gdt_shock::set_Sr_tau_from_tauR (const gdt_welm* Wc, const double& tauR)
{
  assert (!_is_contact());

  if (_is_L_RF()) {
    if (_is_R_RF()) //SRF2: tau = tauR
      tau_ = tauR;
    else //SRF-L, SRF-L2 as rightS: tau = tauR
      tau_ = tauR;
  }
  else {
    if (_is_R_RF()) { 
      if (Wa_ == Wc) //SRF-R, SRF-R2 as rightS, Wa as Wc: tau = tauR
        tau_ = tauR;
      else //SRF-R, SRF-R2 as rightS, RFb as Wc: tauB = pi-tauR
        tau_ = _get_tau_from_tauB (vnl_math::pi-tauR);
    }
    else {
      if (_is_from_edge()) //SE: tau = tauR
        tau_ = tauR;
      else {
        if (bSVE_) {
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SVE-IV: 
              if (Wa_ == Wc) //SVE-IV Wa as Wc: tau = tauR
                tau_ = tauR;
              else //SVE-IV Wb as Wc: tauB = pi-tauR
                tau_ = _get_tau_from_tauB (vnl_math::pi-tauR);
            }
            else //SVE-II: tau = tauR
              tau_ = tauR;
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SV-IE: tauB = tauR
              tau_ = _get_tau_from_tauB (tauR);
            else //SVE-III: can not be rightS
              assert (0);
          }
        }
        else { //SV:
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) //SV-IV: only Wb inward is possible.
              assert (0);
            else //SV-II: tauB = tauR
              tau_ = _get_tau_from_tauB (tauR);      
          }
          else {
            if (_is_Ia_in_Ib_outward()) { //SV-I: tau = tauR
              tau_ = tauR;
            }
            else { //SV-III: 
              if (Wa_ == Wc) //SV-III Wa as Wc: tau = tauR
                tau_ = tauR;
              else //SV-III Wb as WC: tauB = pi-tauR
                tau_ = _get_tau_from_tauB (vnl_math::pi-tauR);
            }
          }
        }
      }
    }
  }
}

//: For a right intersection shock, convert the intrinsic shock tau to
//  the intersection tauR
double gdt_shock::recover_Sr_tauR_from_tau (const gdt_welm* Wc) const
{
  assert (!_is_contact());

  if (_is_L_RF()) {
    if (_is_R_RF()) //SRF2: tauR = tau
      return tau_;
    else //SRF-L, SRF-L2 as rightS: tauR = tau
      return tau_;
  }
  else {
    if (_is_R_RF()) { 
      if (Wa_ == Wc) //SRF-R, SRF-R2 as rightS, Wa as Wc: tauR = tau
        return tau_;
      else //SRF-R, SRF-R2 as rightS: tauR = pi-tauB
        return vnl_math::pi - _get_tauB_from_tau (tau_);
    }
    else {
      if (_is_from_edge()) //SE: tauR = tau
        return tau_;
      else {
        if (bSVE_) {
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SVE-IV: 
              if (Wa_ == Wc) //SVE-IV Wa as Wc: tauR = tau
                return tau_;
              else //SVE-IV Wb as Wc: tauR = pi-tauB
                return vnl_math::pi - _get_tauB_from_tau (tau_);
            }
            else //SVE-II: tauR = tau
              return tau_;
          }
          else {
            if (_is_Ia_in_Ib_outward()) //SVE-I: tauR = tauB
              return _get_tauB_from_tau (tau_);
            else { //SVE-III: can not be rightS
              assert (0);
              return INVALID_TAU;
            }
          }
        }
        else { //SV:
          if (_is_Ia_outward()) {
            if (_is_Ia_out_Ib_outward()) { //SV-IV: only Wb inward is possible.
              assert (0);
              return INVALID_TAU;
            }
            else //SV-II: tauR = tauB
              return _get_tauB_from_tau (tau_);      
          }
          else {
            if (_is_Ia_in_Ib_outward()) { //SV-I: tauR = tau
              return tau_;
            }
            else { //SV-III: 
              if (Wa_ == Wc) //SV-III Wa as Wc: tauR = tau
                return tau_;
              else //SV-III Wb as WC: tauR = pi-tauB
                return vnl_math::pi - _get_tauB_from_tau (tau_);
            }
          }
        }
      }
    }
  }
}

// ##################################################################

// Compute the extrinsic 2D coord of shock point P given tau
vgl_point_2d<double> gdt_shock::get_ext_pt_2d (const gdt_welm* Wc,
                                               const vgl_point_2d<double>& OL,
                                               const vgl_point_2d<double>& OR, 
                                               const double& tauC, const double& dC,
                                               const bool b_as_rightS) const
{
  assert (b2_ != 0);

  // F: the foot point on Wc
  vgl_point_2d<double> F (Wc->L(), 0);
  // phi = CCW (OC->OR, OC->F)
  const double atan2_OL_OR = atan2_vector (OL, OR);
  const double atan2_OC_F = b_as_rightS ? atan2_vector (OL, F) : atan2_vector (OR, F);
  const double phi = CCW_angle (atan2_OL_OR, atan2_OC_F);
  const double phip = vnl_math::pi_over_2 - phi;

  //unused const double atan2_OR_OL = atan2_vector (OR, OL);
  //unused const double phi2 = CCW_angle (atan2_OC_F, atan2_OR_OL);

  // Rotate (dc*vcl_cos(tauC), dc*vcl_sin(tauC)) CCW for phip and 
  // translate by (Lc, -hc) to get the 2d point P
  double px = dC*vcl_cos(tauC);
  double py = dC*vcl_sin(tauC);

  const double cos_phip = vcl_cos (phip);
  const double sin_phip = vcl_sin (phip);

  // (pxy, pyr) is the 2D point for P
  double pxr = cos_phip*px - sin_phip*py + Wc->L();
  double pyr = sin_phip*px + cos_phip*py - Wc->H();

  vgl_point_2d<double> P (pxr, pyr);

  return P;
}

// ###############################################################

//: Determine the sign of a in intersection for this shock.
//  Here is the analysis of all possible shock intersection cases.
//  Totally 32 cases:
//  SRFRF (4), 
//  SRF-L (3), SRF-R(3), SRF-L2 (2), SRF-R2 (2)
//  SE (2), 
//  SV-I (2), SV-II (2), SV-III (2), SV-IV (2),
//  SVE-I (2), SVE-II (2), SVE-III (2), SVE-IV (2),

//  If it is a leftS/rightS, determine aL/aR is +/- a.
//  - SRFRF:
//    - SRFRF as leftS, RF+ as Wc: aL = -a
//    - SRFRF as leftS, RF- as Wc: aL = a
//    - SRFRF as rightS, RF- as Wc: aR = a
//    - SRFRF as rightS, RF+ as Wc: aR = -a
//
//  - SRF-L:
//    - SRF-L as leftS, RF- as Wc: aL = a
//    - SRF-L as rightS, RF- as Wc: aR = a
//    - SRF-L as leftS, W+ as Wc: aL = -a
//
//  - SRF-R:
//    - SRF-R as leftS, RF+ as Wc: aL = -a
//    - SRF-R as rightS, RF+ as Wc: aR = -a
//    - SRF-R as rightS, W- as Wc: aR = a
//
//  - SRF-L2:
//    - SRF-L2 as leftS, RF- as Wc: aL = a
//    - SRF-L2 as leftS, W+ as Wc: aL = -a
//
//  - SRF-R2:
//    - SRF-R2 as rightS, RF+ as Wc: aR = -a
//    - SRF-R2 as rightS, W- as Wc: aR = a
//
//  - SE:
//    - SE as leftS, W+ as Wc: aL = -a
//    - SE as rightS, W- as Wc: aR = a
//
//  - SV-I:
//    - SV-I as leftS, W+ as Wc: aL = -a
//    - SV-I as rightS, W- as Wc: aR = a
//
//  - SV-II:
//    - SV-II as leftS, W- as Wc: aL = a
//    - SV-II as rightS, W+ as Wc: aR = -a
//    
//  - SV-III:
//    - SV-III as rightS, W- as Wc: aR = a
//    - SV-III as rightS, W+ as Wc: aR = -a
//
//  - SV-IV:
//    - SV-IV as leftS, W- as Wc: aL = a
//    - SV-IV as leftS, W+ as Wc: aL = -a
//
//  - SVE-I:
//    - SVE-I as leftS, W- as Wc: aL = a
//    - SVE-I as rightS, W+ as Wc: aR = -a
//
//  - SVE-II:
//    - SVE-II as leftS, W+ as Wc: aL = -a
//    - SVE-II as rightS, W- as Wc: aR = a
//
//  - SVE-III:
//    - SV-III as leftS, W- as Wc: aL = a
//    - SV-III as leftS, W+ as Wc: aL = -a
//
//  - SVE-IV:
//    - SVE-IV as rightS, W- as Wc: aR = a
//    - SVE-IV as rightS, W+ as Wc: aR = -a
//

double gdt_shock::_Sl_get_intersect_a (const SET_TYPE seType, const gdt_welm* Wc) const
{
  switch (seType) {
  case SET_SRFRF:
    if (Wb_ == Wc)
      return -a_;
    else
      return a_;
  break;
  case SET_SRF_L:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SRF_L2:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SRF_R:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SRF_R2:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SE:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SV_I:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SV_II:
    assert (Wa_ == Wc);
    return a_;
  break;
  case SET_SV_III:
    assert (0);
  break;
  case SET_SV_IV:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SVE_I:
    assert (Wa_ == Wc);
    return a_;
  break;
  case SET_SVE_II:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SVE_III:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SVE_IV:
    assert (0);
  break;
  default:
    break;
  }

  return GDT_HUGE;
}

double gdt_shock::_Sr_get_intersect_a (const SET_TYPE seType, const gdt_welm* Wc) const
{
  switch (seType) {
  case SET_SRFRF:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SRF_L:
    assert (Wa_ == Wc);
      return a_;
  break;
  case SET_SRF_L2:
    assert (0);
  break;
  case SET_SRF_R:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SRF_R2:
    if (Wb_ == Wc)
      return -a_;
    else
      return a_;
  break;
  case SET_SE:
    assert (Wa_ == Wc);
    return a_;
  break;
  case SET_SV_I:
    assert (Wa_ == Wc);
    return a_;
  break;
  case SET_SV_II:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SV_III:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  case SET_SV_IV:
    assert (0);
  break;
  case SET_SVE_I:
    assert (Wb_ == Wc);
    return -a_;
  break;
  case SET_SVE_II:
    assert (Wa_ == Wc);
    return a_;
  break;
  case SET_SVE_III:
    assert (0);
  break;
  case SET_SVE_IV:
    if (Wa_ == Wc)
      return a_;
    else
      return -a_;
  break;
  default:
    break;
  }

  return GDT_HUGE;
}

// ################################################################

void gdt_shock::detect_next_prjE ()
{ 
  double tauL, tauR, tauC;
  double tL = GDT_HUGE;
  double tR = GDT_HUGE;
  double tC = GDT_HUGE;

  SOT_TYPE SOtype = _detect_SO_type();

  dbmsh3d_face* curF;
  dbmsh3d_halfedge *heC, *heL, *heR;
  dbmsh3d_gdt_edge *eC, *eL, *eR;
  _get_local_orientation2 (SOtype, &heC, &curF, &heL, &heR, &eC, &eL, &eR);

  double alphaCL, alphaCR, alphaLR;
  m2t_compute_tri_angles (eC->len(), eL->len(), eR->len(), alphaCL, alphaCR, alphaLR);

  switch (SOtype) {
  case SOT_SRFRF:
    next_SRF2_to_eL (Wa_, Wb_, eL, tauL, tL);
    next_SRF2_to_eR (Wa_, Wb_, eR, tauR, tR);
  break;
  case SOT_SRF_L:
    if (_is_SRF_L_or_L2()) { //SRF-L
      next_SRF_L_to_eL (Wa_, Wb_, eL, alphaCL, tauL, tL);
      next_SRF_L_to_eR (Wa_, Wb_, eR, alphaCR, tauR, tR);
    }
    else { //SRF-L2
      next_SRF_L2_to_eR (Wa_, Wb_, eR, alphaCR, tauR, tR);
      next_SRF_L2_to_eC (Wa_, Wb_, eC, tauC, tC);
    }     
  break;
  case SOT_SRF_R:
    if (_is_SRF_R_or_R2()) { //SRF-R
      next_SRF_R_to_eL (Wa_, Wb_, eL, alphaCL, tauL, tL);
      next_SRF_R_to_eR (Wa_, Wb_, eR, alphaCR, tauR, tR);
    }
    else { //SRF-R2
      next_SRF_R2_to_eL (Wa_, Wb_, eL, alphaCL, tauL, tL);
      next_SRF_R2_to_eC (Wa_, Wb_, eC, tauC, tC);
    }
  break;
  case SOT_SE:
    next_SE_to_eL (Wa_, Wb_, eL, alphaCL, tauL, tL);
    next_SE_to_eR (Wa_, Wb_, eR, alphaCR, tauR, tR);
  break;
  case SOT_SV_SVE:
  {
    next_SV_to_eC (Wa_, Wb_, eC, alphaCL, alphaCR, tauC, tC);

    //the existing S-E tau is not valid.
    double existingTau = GDT_HUGE;
    if (prjE_ == eL)
      existingTau = tauE_;
    next_SV_SVE_to_eL (Wa_, Wb_, eL, alphaLR, tauL, tL, existingTau);

    if (prjE_ == eR)
      existingTau = tauE_;
    next_SV_SVE_to_eR (Wa_, Wb_, eR, alphaLR, tauR, tR, existingTau);
  }
  break;
  default:
    assert (0);
  break;
  }
  
  //the existing S-E is not a solustion
  if (_leqF (tC, startT_))
    tC = GDT_HUGE;
  if (_leqF (tL, startT_))
    tL = GDT_HUGE;
  if (_leqF (tR, startT_))
    tR = GDT_HUGE;

  _set_next_prjE (eL, tauL, tL, eR, tauR, tR, eC, tauC, tC);
}

void gdt_shock::_set_next_prjE (const dbmsh3d_gdt_edge* eL, const double& tauL, const double& tL, 
                                const dbmsh3d_gdt_edge* eR, const double& tauR, const double& tR, 
                                const dbmsh3d_gdt_edge* eC, const double& tauC, const double& tC)
{
  // Choose the intersection with smaller simtime to be the next prjE
  // Be careful for the case that the shock intersects at the ending vertex!
  if (tL == GDT_HUGE && tR == GDT_HUGE && tC == GDT_HUGE) {
    // It is possible that S intersects with no prjE.
    // It will intersect other shocks later on.
    prjE_ = NULL;
    set_edgeT_endT (GDT_HUGE);
  }
  else if (tL <= tR && tL <= tC) {
    prjE_ = (dbmsh3d_gdt_edge*) eL;
    tauE_ = tauL;
    set_edgeT_endT (tL);
  }
  else if (tR <= tL && tR <= tC){
    prjE_ = (dbmsh3d_gdt_edge*) eR;
    tauE_ = tauR;
    set_edgeT_endT (tR);
  }
  else if (tC <= tL && tC <= tR){
    prjE_ = (dbmsh3d_gdt_edge*) eC;
    tauE_ = tauC;
    set_edgeT_endT (tC);
  }
  else {
    assert (0);
  }
}

gdt_welm* gdt_shock::_get_nWa_on_prjE () const 
{
  //Loop through Wa.nextI[] to find the one adjacent to tauE on prjE.
  vcl_vector<gdt_interval*>::iterator it = Wa_->nextIs().begin();
  for (; it != Wa_->nextIs().end(); it++) {
    gdt_interval* I = (*it);
    if (I->edge() != prjE_)
      continue;

    if (_leqT(I->stau(), tauE_) && _leqT(tauE_, I->etau()))
      return (gdt_welm*) I;
  }

  return NULL;
}
gdt_welm* gdt_shock::_get_nWb_on_prjE () const 
{
  //Loop through Wb.nextI[] to find the one adjacent to tauE on prjE.
  vcl_vector<gdt_interval*>::iterator it = Wb_->nextIs().begin();
  for (; it != Wb_->nextIs().end(); it++) {
    gdt_interval* I = (*it);
    if (I->edge() != prjE_)
      continue;

    if (_leqT(I->stau(), tauE_) && _leqT(tauE_, I->etau()))
      return (gdt_welm*) I;
  }

  return NULL;
}

// ################################################################

//: Wavefront Dynamic Validation function.
//  Also return the time of the corresponding point of tau on S, timePS.
//    - If the given tauP is valid,
//        return true. timePS is GDT_HUGE.
//    - If tauP is invalid,
//        return false. Also set timeP to be the corresponding time on shock.
//
//  Need to use pi-tauP as intrinsic iTau for SV_IV W+ = WC case and SV_III W+ = WC case!
//  Need to consider the SVE- I, II, III, IV cases.
//  
bool gdt_shock::validate_W_event (const gdt_welm* W, const double& tauP, double& timeP) const
{
  double tau_min, tau_max;
  double iTau;
  double timePS = GDT_HUGE;

  //Two cases depending on W is S->Wa or S->Wb.
  if (Wa_ == W) { 
    //For Wa_as_Wc case, intrinsic iTau is tau.
    iTau = tauP;
    
    //Compute the valid iTau range of S on Wa side.
    get_Wa_tau_range (tau_min, tau_max);

    //If projection of iTau on S does not exist, iTau is not invalidated by S.
    if (_leqT (iTau, tau_min) || _leqT (tau_max, iTau))
      return true;

    //Compute the distance of iTau's project point Ps on S.
    timePS = _get_d_from_tau (iTau);
  }
  else {
    assert (Wb_ == W);

    //Note that for Wb_as_Wc case for SV_III, SV_IV, SVE_III, SVE_IV, intrinsic iTau = pi-tau.
    if (_is_SV_SVE_III_IV())
      iTau = vnl_math::pi - tauP;
    else
      iTau = tauP;

    get_Wb_tau_range (tau_min, tau_max);

    //If projection of iTau on S does not exist, iTau is not invalidated by S.
    if (_leqT (iTau, tau_min) || _leqT (tau_max, iTau))
      return true;

    //Compute the distance of iTau's project point Ps on S.
    timePS = _get_dB_from_tauB (iTau);
  }
  timePS += W->mu();
 
  //If timePS strictly less than the given timeP, P is invalid.
  if (_lessD (timePS, timeP)) {
    //set timeP to be timePS
    timeP = timePS;
    return false;
  }
  else
    return true;  
}

void gdt_shock::getInfo (vcl_ostringstream& ostrm) 
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  char tmp[512];
  if (Enode_)
    vcl_sprintf (tmp, "%d", Enode_->id());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, "gdt_shock id: %d (Snode %d, Enode %s) SVE: %s\n", 
               id_, Snode_->id(), tmp,
               bSVE_ ? "true" : "false"); ostrm<<s;

  vcl_sprintf (s, " startT: %f, simT: %f, endT: %f, edgeT: %f\n", 
               startT_, simT_, endT_, edgeT_); ostrm<<s;

  if (Wa_)
    vcl_sprintf (tmp, "%s%d (%.3f, %.3f), psrc %d, mu: %f, L: %f, H: %f", Wa_->_is_RF() ? "RF " : "",
                 Wa_->edge()->id(), Wa_->stau(), Wa_->etau(), Wa_->psrc()->id(), Wa_->mu(), Wa_->L(), Wa_->H());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " Wa: %s\n", tmp); ostrm<<s;

  if (Wb_)
    vcl_sprintf (tmp, "%s%d (%.3f, %.3f), psrc %d, mu: %f, L: %f, H: %f\n", Wb_->_is_RF() ? "RF " : "",
                 Wb_->edge()->id(), Wb_->stau(), Wb_->etau(), Wb_->psrc()->id(), Wb_->mu(), Wb_->L(), Wb_->H()); 
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " Wb: %s\n", tmp); ostrm<<s;

  if (b2_ == 0)
    vcl_sprintf (tmp, "Contact");
  else if (a_ == 0)
    vcl_sprintf (tmp, "Line");
  else
    vcl_sprintf (tmp, "Hypb");
  vcl_sprintf (s, "%s: a %f, b %f, c %f.\n", tmp, a_, b2_, c_); ostrm<<s;

  if (Sa_)
    vcl_sprintf (tmp, "%d", Sa_->id());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " Sa: %s,", tmp); ostrm<<s;
  if (Sb_)
    vcl_sprintf (tmp, "%d", Sb_->id());
  else
    vcl_sprintf (tmp, "NULL");
  vcl_sprintf (s, " Sb: %s,", tmp); ostrm<<s;

  if (prjE_)
    sprintf (tmp, "%d", prjE_->id());
  else
    sprintf (tmp, "NULL");
  vcl_sprintf (s, " propagated: %s. tau %f, tauE %f, prjE: %s", 
              b_propagated_ ? "true" : "false", tau_, tauE_, tmp); ostrm<<s;

  vcl_sprintf (s, "\n"); ostrm<<s;
}

// ################################################################

void associate_W_S (gdt_welm* W, gdt_shock* S)
{
  S->_set_Wa (W);
  W->_set_Sr (S);
}

void associate_S_W (gdt_shock* S, gdt_welm* W)
{
  S->_set_Wb (W);
  W->_set_Sl (S);
}

void associate_ep_W_S_W (gdt_welm* Wa, gdt_shock* S, gdt_welm* Wb)
{
  assert (Wa->edge() == S->prjE());
  assert (Wb->edge() == S->prjE());
  S->_set_Wa (Wa);
  Wa->_set_Sr (S);
  S->_set_Wb (Wb);
  Wb->_set_Sl (S);
}

void associate_Wa_S (gdt_welm* W, gdt_shock* S)
{
  S->_set_Wa (W);
  W->_set_Sr (S);
}

void associate_S_Wa (gdt_shock* S, gdt_welm* W)
{
  S->_set_Wa (W);
  W->_set_Sl (S);
}

void associate_Wb_S (gdt_welm* W, gdt_shock* S)
{
  S->_set_Wb (W);
  W->_set_Sr (S);
}

void associate_S_Wb (gdt_shock* S, gdt_welm* W)
{
  S->_set_Wb (W);
  W->_set_Sl (S);
}

void SVtoE_associate_Wa_S (gdt_welm* Wa, gdt_shock* S)
{
  //Associate Wa to S
  Wa->_set_Sr (S);

  //Associate the new Wa_prev to S
  assert (Wa->prevI());
  gdt_welm* Wa_prev = (gdt_welm*) Wa->prevI();

  if (Wa->edge()->is_V_incident (Wa_prev->eV())) {
    Wa_prev->_set_Sr (S);
  }
  else {
    assert (Wa->edge()->is_V_incident (Wa_prev->sV()));
    Wa_prev->_set_Sl (S);
  }

  //Associate S to the new Wa_prev
  S->_set_Wa (Wa_prev);
}

void SVtoE_associate_S_Wb (gdt_shock* S, gdt_welm* Wb)
{
  //Associate Wb to S
  Wb->_set_Sl (S);

  //Associate the new Wb_prev to S
  assert (Wb->prevI());
  gdt_welm* Wb_prev = (gdt_welm*) Wb->prevI();

  if (Wb->edge()->is_V_incident (Wb_prev->eV())) {
    Wb_prev->_set_Sr (S);
  }
  else {
    assert (Wb->edge()->is_V_incident (Wb_prev->sV()));
    Wb_prev->_set_Sl (S);
  }

  //Associate S to the new Wb_prev
  S->_set_Wb (Wb_prev);
}

void RFStoE_associate_Wa_S (gdt_welm* degeWa, gdt_welm* nWa, gdt_shock* S)
{
  //Associate degeWa to S
  degeWa->_set_Sr (S);

  //Associate nWa to S
  if (degeWa->edge()->is_V_incident (nWa->eV())) {
    nWa->_set_Sr (S);
  }
  else {
    assert (degeWa->edge()->is_V_incident (nWa->sV()));
    nWa->_set_Sl (S);
  }

  //Associate S to nWa
  S->_set_Wa (nWa);
}

void RFStoE_associate_S_Wb (gdt_shock* S, gdt_welm* degeWb, gdt_welm* nWb)
{
  //Associate degeWb to S
  degeWb->_set_Sl (S);

  //Associate nWb to S
  if (degeWb->edge()->is_V_incident (nWb->eV())) {
    nWb->_set_Sr (S);
  }
  else {
    assert (degeWb->edge()->is_V_incident (nWb->sV()));
    nWb->_set_Sl (S);
  }

  //Associate S to nWb
  S->_set_Wb (nWb);
}

void tie_neighbor_S (gdt_shock* S1, const gdt_welm* W, gdt_shock* S2)
{
  if (S1->Wa() == W)
    S1->_set_Sa (S2);
  else {
    assert (S1->Wb() == W);
    S1->_set_Sb (S2);
  }

  if (S2->Wa() == W)
    S2->_set_Sa (S1);
  else {
    assert (S2->Wb() == W);
    S2->_set_Sb (S1);
  }
}

void untie_neighbor_S (gdt_shock* S1, const gdt_welm* W, gdt_shock* S2)
{
  if (S1->Wa() == W) {
    assert (S1->Sa() == S2);
    S1->_set_Sa (NULL);
  }
  else {
    assert (S1->Wb() == W);
    assert (S1->Sb() == S2);
    S1->_set_Sb (NULL);
  }

  if (S2->Wa() == W) {
    assert (S2->Sa() == S1);
    S2->_set_Sa (NULL);
  }
  else {
    assert (S2->Wb() == W);
    assert (S2->Sb() == S1);
    S2->_set_Sb (NULL);
  }
}

