//: gdt_shock_edge.cxx
//  Apr 26, 2006 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vcl_string.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>

#include <dbmsh3d/dbmsh3d_halfedge.h>

#include <gdt/gdt_interval.h>
#include <gdt/gdt_shock.h>
#include <gdt/gdt_edge.h>
#include <gdt/gdt_vertex.h>

//: These 9 functions are used in S-E to detect the next shock-edge intersection.
void next_SRF_L_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                       double& tauL, double& tL)
{
  //Project Wb to eL
  double B_nL, B_nH, B_min, B_max;
  if (Wb->L_proj_tauS_tauE (alphaCL, eL, B_nL, B_nH, B_min, B_max) != IP_VALID)
    return;

  //A_nL and A_nH is w.r.t. Wa.psrc, tau is within (0, eL.len).
  double A_nL, A_nH, A_min, A_max;
  A_nL = (Wa->psrc() == eL->sV()) ? 0 : eL->len();
  A_nH = 0;
  A_min = 0;
  A_max = eL->len();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);  

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);    
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

void next_SRF_L_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                       double& tauR, double& tR)
{
  //Project Wb to eR
  double B_nL, B_nH, B_min, B_max;
  if (Wb->R_proj_tauS_tauE (alphaCR, eR, B_nL, B_nH, B_min, B_max) != IP_VALID)
    return;

  //Wa is a RF, just use its (L, H).
  double A_nL, A_nH, A_min, A_max;
  A_nL = Wa->L();
  A_nH = Wa->H();
  A_min = Wa->stau();
  A_max = Wa->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);    
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}

void next_SRF_R_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                       double& tauL, double& tL)
{
  //Project Wa to eL
  double A_nL, A_nH, A_min, A_max;
  if (Wa->L_proj_tauS_tauE (alphaCL, eL, A_nL, A_nH, A_min, A_max) != IP_VALID)
    return;

  //Wb is a RF, just use its (L, H).
  double B_nL, B_nH, B_min, B_max;
  B_nL = Wb->L();
  B_nH = Wb->H();
  B_min = Wb->stau();
  B_max = Wb->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);    
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

void next_SRF_R_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                       const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                       double& tauR, double& tR)
{
  //Project Wa to eR
  double A_nL, A_nH, A_min, A_max;
  if (Wa->R_proj_tauS_tauE (alphaCR, eR, A_nL, A_nH, A_min, A_max) != IP_VALID)
    return;

  //B_nL and B_nH is w.r.t. Wb.psrc, tau is within (0, eR.len).
  double B_nL, B_nH, B_min, B_max;
  B_nL = (Wb->psrc() == eR->sV()) ? 0 : eR->len();
  B_nH = 0;
  B_min = 0;
  B_max = eR->len();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);  

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);    
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}


void next_SE_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eL, const double& alphaCL, 
                    double& tauL, double& tL)
{  
  assert (Wa->edge() == Wb->edge());

  double A_nH, A_nL, A_min, A_max;
  if (Wa->L_proj_tauS_tauE (alphaCL, eL, A_nL, A_nH, A_min, A_max) != IP_VALID)
    return;

  double B_nH, B_nL, B_min, B_max;
  if (Wb->L_proj_tauS_tauE (alphaCL, eL, B_nL, B_nH, B_min, B_max) != IP_VALID)
    return;

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

void next_SE_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eR, const double& alphaCR, 
                    double& tauR, double& tR)
{
  assert (Wa->edge() == Wb->edge());

  double B_nH, B_nL, B_min, B_max;
  if (Wb->R_proj_tauS_tauE (alphaCR, eR, B_nL, B_nH, B_min, B_max) != IP_VALID)
    return;

  double A_nH, A_nL, A_min, A_max;
  if (Wa->R_proj_tauS_tauE (alphaCR, eR, A_nL, A_nH, A_min, A_max) != IP_VALID)
    return;

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}

void next_SV_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                    const dbmsh3d_gdt_edge* eC, 
                    const double& alphaCL, const double& alphaCR, 
                    double& tauC, double& tC)
{
  assert (Wa->is_psrc());
  assert (Wb->is_psrc());
  assert (Wa->edge() != Wb->edge());

  //Initialize tC to be infinity
  tC = GDT_HUGE;

  // Project the Wa to eC
  double A_nL, A_nH, A_min, A_max;
  if (eC->is_V_incident (Wa->sV())) {
    if (Wa->L_proj_tauS_tauE (alphaCL, eC, A_nL, A_nH, A_min, A_max) == IP_INVALID)
      return;
  }
  else {
    if (Wa->R_proj_tauS_tauE (alphaCL, eC, A_nL, A_nH, A_min, A_max) == IP_INVALID)
      return;
  }

  // Project the Wb to eC
  double B_nL, B_nH, B_min, B_max;
  if (eC->is_V_incident (Wb->sV())) {
    if (Wb->L_proj_tauS_tauE (alphaCR, eC, B_nL, B_nH, B_min, B_max) == IP_INVALID)
      return;
  }
  else {
    if (Wb->R_proj_tauS_tauE (alphaCR, eC, B_nL, B_nH, B_min, B_max) == IP_INVALID)
      return;
  }
  
  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eC->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauC = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauC && tauC <= tau_max) {
    tC = Wa->mu() + vnl_math_hypot (tauC-A_nL, A_nH);    
    assert (_eqD (tC, Wb->mu() + vnl_math_hypot (tauC-B_nL, B_nH)));
  }
}

//: Note that only SV_SVE to eL/eR and SRF-L2 R2 to eC can
//  intersect the from edge twice.
void next_SV_SVE_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eL, const double& alphaLR, 
                        double& tauL, double& tL,
                        const double& existingTau)
{
  //Find v
  dbmsh3d_vertex* v = incident_V_of_Es (Wa->edge(), Wb->edge());

  //Project Wb to eL
  double B_nL, B_nH, B_min, B_max;
  if (Wb->sV() == v) {
    if (Wb->L_proj_tauS_tauE (alphaLR, eL, B_nL, B_nH, B_min, B_max) != IP_VALID)
      return;
  }
  else {
    if (Wb->R_proj_tauS_tauE (alphaLR, eL, B_nL, B_nH, B_min, B_max) != IP_VALID)
      return;
  }

  //Wa is nWa
  double A_nL, A_nH, A_min, A_max;
  A_nL = Wa->L();
  A_nH = Wa->H();
  A_min = Wa->stau();
  A_max = Wa->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(),
                                   tau_min, tau_max, existingTau);

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);    
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

void next_SV_SVE_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eR, const double& alphaLR,
                        double& tauR, double& tR, 
                        const double& existingTau)
{  
  //Find v
  dbmsh3d_vertex* v = incident_V_of_Es (Wa->edge(), Wb->edge());

  //Project Wa to eR
  double A_nL, A_nH, A_min, A_max;
  if (Wa->sV() == v) {
    if (Wa->L_proj_tauS_tauE (alphaLR, eR, A_nL, A_nH, A_min, A_max) != IP_VALID)
      return;
  }
  else {
    if (Wa->R_proj_tauS_tauE (alphaLR, eR, A_nL, A_nH, A_min, A_max) != IP_VALID)
      return;
  }

  //Wb is nWb
  double B_nL, B_nH, B_min, B_max;
  B_nL = Wb->L();
  B_nH = Wb->H();
  B_min = Wb->stau();
  B_max = Wb->etau();
  
  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), 
                                   tau_min, tau_max, existingTau);

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}

//: These additional 6 functions together with the previous 9 ones
//  are used in the child shock creation to detect the next S-E intersection.
void next_SRF2_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                      const dbmsh3d_gdt_edge* eL,
                      double& tauL, double& tL)
{
  //A_nL and A_nH is w.r.t. Wa.psrc, tau is within (0, eL.len).
  double A_nL, A_nH, A_min, A_max;
  A_nL = (Wa->psrc() == eL->sV()) ? 0 : eL->len();
  A_nH = 0;
  A_min = 0;
  A_max = eL->len();

  //Wb is a RF, just use its (L, H).
  double B_nL, B_nH, B_min, B_max;
  B_nL = Wb->L();
  B_nH = Wb->H();
  B_min = Wb->stau();
  B_max = Wb->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);    
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

void next_SRF2_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                      const dbmsh3d_gdt_edge* eR,
                      double& tauR, double& tR)
{
  //Wa is a RF, just use its (L, H).
  double A_nL, A_nH, A_min, A_max;
  A_nL = Wa->L();
  A_nH = Wa->H();
  A_min = Wa->stau();
  A_max = Wa->etau();

  //B_nL and B_nH is w.r.t. Wb.psrc, tau is within (0, eR.len).
  double B_nL, B_nH, B_min, B_max;
  B_nL = (Wb->psrc() == eR->sV()) ? 0 : eR->len();
  B_nH = 0;
  B_min = 0;
  B_max = eR->len();
  
  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);    
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}

void next_SRF_L2_to_eR (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eR, const double& alphaCR,
                        double& tauR, double& tR)
{
  //Project Wb to eR
  double B_nL, B_nH, B_min, B_max;
  if (Wb->R_proj_tauS_tauE (alphaCR, eR, B_nL, B_nH, B_min, B_max) == IP_INVALID) ///!= IP_VALID) ///== IP_INVALID)
    return;

  //A_nL and A_nH is w.r.t. Wa.psrc, tau is within (0, eR.len).
  double A_nL, A_nH, A_min, A_max;
  A_nL = (Wa->psrc() == eR->sV()) ? 0 : eR->len();
  A_nH = 0;
  A_min =0;
  A_max = eR->len();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eR->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauR = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauR && tauR <= tau_max) {
    tR = Wb->mu() + vnl_math_hypot (tauR-B_nL, B_nH);    
    assert (_eqD (tR, Wa->mu() + vnl_math_hypot (tauR-A_nL, A_nH)));
  }
}

void next_SRF_R2_to_eL (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eL, const double& alphaCL,
                        double& tauL, double& tL)
{
  //Project Wa to eL
  double A_nL, A_nH, A_min, A_max;
  if (Wa->L_proj_tauS_tauE (alphaCL, eL, A_nL, A_nH, A_min, A_max) == IP_INVALID) ///!= IP_VALID) ///== IP_INVALID)
    return;

  //B_nL and B_nH is w.r.t. Wb.psrc, tau is within (0, eL.len).
  double B_nL, B_nH, B_min, B_max;
  B_nL = (Wb->psrc() == eL->sV()) ? 0 : eL->len();
  B_nH = 0;
  B_min = 0;
  B_max = eL->len();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eL->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauL = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);

  if (tau_min <= tauL && tauL <= tau_max) {
    tL = Wa->mu() + vnl_math_hypot (tauL-A_nL, A_nH);    
    assert (_eqD (tL, Wb->mu() + vnl_math_hypot (tauL-B_nL, B_nH)));
  }
}

//: Note that only SV_SVE to eL/eR and SRF-L2 R2 to eC can
//  intersect the from edge twice.
//
void next_SRF_L2_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eC,
                        double& tauC, double& tC)
{
  //Wa is a RF, just use its (L, H).
  double A_nL, A_nH, A_min, A_max;
  A_nL = Wa->L();
  A_nH = Wa->H();
  A_min = Wa->stau();
  A_max = Wa->etau();

  //Wb is nWb
  double B_nL, B_nH, B_min, B_max;
  B_nL = Wb->L();
  B_nH = Wb->H();
  B_min = Wb->stau();
  B_max = Wb->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eC->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauC = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);  

  if (tau_min <= tauC && tauC <= tau_max) {
    tC = Wa->mu() + vnl_math_hypot (tauC-A_nL, A_nH);    
    assert (_eqD (tC, Wb->mu() + vnl_math_hypot (tauC-B_nL, B_nH)));
  }
}

void next_SRF_R2_to_eC (const gdt_welm* Wa, const gdt_welm* Wb,
                        const dbmsh3d_gdt_edge* eC,
                        double& tauC, double& tC)
{
  //Wa is nWa
  double A_nL, A_nH, A_min, A_max;
  A_nL = Wa->L();
  A_nH = Wa->H();
  A_min = Wa->stau();
  A_max = Wa->etau();

  //Wb is a RF, just use its (L, H).
  double B_nL, B_nH, B_min, B_max;
  B_nL = Wb->L();
  B_nH = Wb->H();
  B_min = Wb->stau();
  B_max = Wb->etau();

  // Determine min_tau and max_tau
  double tau_min, tau_max;
  _determine_min_max_tau (A_min, A_max, B_min, B_max, eC->len(), tau_min, tau_max);

  //Solve for the intersection point.
  tauC = _solve_tau_fix_bnd_value (A_nL, A_nH, Wa->mu(), B_nL, B_nH, Wb->mu(), tau_min, tau_max);  

  if (tau_min <= tauC && tauC <= tau_max) {
    tC = Wb->mu() + vnl_math_hypot (tauC-B_nL, B_nH);    
    assert (_eqD (tC, Wa->mu() + vnl_math_hypot (tauC-A_nL, A_nH)));
  }
}

void _determine_min_max_tau (const double& A_min, const double& A_max, 
                             const double& B_min, const double& B_max, 
                             const double& e_len,
                             double& tau_min, double& tau_max)
{
  tau_min = vcl_max (A_min, B_min);
  tau_max = vcl_min (A_max, B_max);
  tau_min = vcl_max (tau_min, 0.0);
  tau_max = vcl_min (tau_max, e_len);
  if (_eqT (tau_min, 0))
    tau_min = 0;
  if (_eqT (tau_max, e_len))
    tau_max = e_len;
}

double _solve_tau_fix_bnd_value (const double& A_nL, const double& A_nH, const double& muA,
                                 const double& B_nL, const double& B_nH, const double& muB,
                                 double& tau_min, double& tau_max,
                                 const double& existingTau)
{
  double tau;

  if (_eqT (tau_min, tau_max)) {    
    if (tau_min > tau_max) { //Swap tau_min and tau_max if needed
      double temp = tau_min;
      tau_min = tau_max;
      tau_max = temp;
    }

    tau = (tau_min + tau_max)/2;
  }
  else {    
    assert (!_eqM(A_nH, B_nH) || !_eqM(A_nL, B_nL));
    tau = solve_eqdist_tau (A_nL, A_nH, muA, B_nL, B_nH, muB, 
                            tau_min, tau_max, existingTau);
  }

  //Fix tau if close to boundary
  if (_eqT (tau, tau_min))
    tau = tau_min;
  else if (_eqT (tau, tau_max))
    tau = tau_max;

  return tau;
}
