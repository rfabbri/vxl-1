//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>

#include <gdt/gdt_manager.h>

// ##########################################################################
//   Propagation Subfunctions.

// Description: 
//   For the initialization or a wavefront-vertex strike, create a degenerate 
//   interval from the pseudo-source v on the edge e.
//   In this case the psrc is one of the ending vertex.
//
//   The prevI is NULL by default, and can be used in the face-based propagation.
//
// Return: void.
//
gdt_interval* gdt_manager_i_based::create_dege_I (const dbmsh3d_halfedge* cur_he,
                                                  const bool b_psrc_s, 
                                                  const gdt_interval* prevI)
{
  gdt_interval* I;
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();

  if (b_psrc_s) { // If sV is the source
    assert (cur_edge->sV()->dist() != INVALID_DIST);
    I = new gdt_interval (ITYPE_DEGE, 0, cur_edge->len(), cur_he, cur_edge->sV(), 0, 0, prevI);
  }
  else { // Else, eV is the source
    assert (cur_edge->eV()->dist() != INVALID_DIST);
    I = new gdt_interval (ITYPE_DEGE, 0, cur_edge->len(), cur_he, cur_edge->eV(), cur_edge->len(), 0, prevI);
  }
  return I;
}

// Description: 
//   In a wavefront-vertex strike at vertex v, v is now a pseudo-source,
//   create the rarefaction interval on the opposite edge e of v covering range 
//   [stau, etau].
// Return: void.
//
gdt_interval* gdt_manager_i_based::create_rf_I (const dbmsh3d_halfedge* cur_he, 
                                                dbmsh3d_gdt_vertex_3d* psrc, 
                                                const double& stau, 
                                                const double& etau)
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  const double c = cur_edge->length();
  const double a = vgl_distance (psrc->pt(), cur_edge->eV()->pt());
  const double b = vgl_distance (psrc->pt(), cur_edge->sV()->pt());
  const double alpha0 = vcl_acos ((b*b + c*c - a*a) / (2*b*c));
  const double nH = b * vcl_sin (alpha0);
  const double nL = b * vcl_cos (alpha0);
  return new gdt_interval (ITYPE_PSRC, stau, etau, cur_he, psrc, nL, nH, NULL);
}

//: propagate pseudo-source rarefaction I to left_edge (from cur_edge on the left side)
gdt_interval* gdt_manager_i_based::create_rf_I_to_L (const dbmsh3d_gdt_edge* cur_edge, 
                                                     const dbmsh3d_halfedge* left_he,
                                                     const double& angle_cl, 
                                                     const double stau, const double etau)
{
  if (_eqT (stau, etau))
    return NULL;
  assert (stau < etau);

  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();
  double nH = cur_edge->len() * vcl_sin (angle_cl);
  double nL = cur_edge->len() * vcl_cos (angle_cl);

  if (cur_edge->sV() != left_edge->sV())
    nL = left_edge->len() - nL;

  get_psrc_dist (cur_edge->eV(), (dbmsh3d_gdt_edge*) cur_edge);
  return new gdt_interval (ITYPE_PSRC, stau, etau, left_he, cur_edge->eV(), nL, nH, NULL);
}

//: propagate pseudo-source rarefaction I to right_edge (from cur_edge on the right side)
gdt_interval* gdt_manager_i_based::create_rf_I_to_R (const dbmsh3d_gdt_edge* cur_edge, 
                                                     const dbmsh3d_halfedge* right_he,
                                                     const double& angle_cr, 
                                                     const double stau, const double etau)
{
  if (_eqT (stau, etau))
    return NULL;
  assert (stau < etau);

  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  double nH = cur_edge->len() * vcl_sin (angle_cr);
  double nL = cur_edge->len() * vcl_cos (angle_cr);

  if (cur_edge->eV() == right_edge->eV())
    nL = right_edge->len() - nL;

  get_psrc_dist (cur_edge->sV(), (dbmsh3d_gdt_edge*) cur_edge);
  return new gdt_interval (ITYPE_PSRC, stau, etau, right_he, cur_edge->sV(), nL, nH, NULL);
}


