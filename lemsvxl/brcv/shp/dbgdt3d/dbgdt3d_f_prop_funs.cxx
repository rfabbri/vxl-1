//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>

#include <dbgdt3d/dbgdt3d_manager.h>

//##########################################################################
//: ====== Geodesic Wavefront Propagation Functions ======

//: propagate the intervals from cur_edge to left_edge
double gdt_f_manager::prop_to_left_edge (dbmsh3d_halfedge* cur_he, dbmsh3d_halfedge* left_he,
                                         const double& angle_cl,
                                         gdt_interval_section* left_interval_section,
                                         bool do_rarefaction_prop)
{
  double prop_tau;

  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();
  //unused dbmsh3d_face* cur_face = cur_he->face();

  if (_detect_left_dege (cur_edge, angle_cl)) {
    //: degenerate propagation to left_edge
    _prop_left_dege (cur_edge, left_he, left_interval_section);

    //: the degenerate propagation should leave room for possible rarefraction propagation.
    if (cur_edge->sV() == left_edge->sV())
      prop_tau = 0;
    else
      prop_tau = left_edge->len();
  }
  else {
    //: project cur_edge's intervals to left_edge
    prop_tau = _project_left_sections (cur_edge, left_he, angle_cl, left_interval_section);

    if (do_rarefaction_prop) {
      //: if cur_edge's eV is a boundary vertex (w.r.t. cur_he's surface), 
      //  possible pseudo-source propagation
      double b = left_interval_section->len();
      if (cur_edge->sV() == left_edge->sV()) {
        if (_lessT (prop_tau, b)) {
          gdt_interval* I = create_rf_I_to_L (cur_edge, left_he, angle_cl, prop_tau, b);
          intersect_I_with_IS (I, left_interval_section);
        }
        else {
          //else, fix the boundary of the last interval
          vcl_map<double, gdt_ibase*>::reverse_iterator rit = left_interval_section->I_map()->rbegin();
          gdt_ibase* I = (*rit).second;
          I->_set_etau (b);
        }
      }
      else {
        if (_lessT (0, prop_tau)) {
          gdt_interval* I = create_rf_I_to_L (cur_edge, left_he, angle_cl, 0, prop_tau);
          intersect_I_with_IS (I, left_interval_section);
        }
        else {
          //else, fix the boundary of the firat interval
          vcl_map<double, gdt_ibase*>::iterator it = left_interval_section->I_map()->begin();
          gdt_ibase* I = (*it).second;
          left_interval_section->I_map()->erase (it);
          I->_set_stau (0);
          left_interval_section->_add_interval (I);
        }
      }
    }
  }

  return prop_tau;
}

//: propagate the intervals from cur_edge to right_edge
double gdt_f_manager::prop_to_right_edge (dbmsh3d_halfedge* cur_he, dbmsh3d_halfedge* right_he,
                                          const double& angle_cr,
                                          gdt_interval_section* right_interval_section,
                                          bool do_rarefaction_prop)
{
  double prop_tau;

  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  //unused dbmsh3d_face* cur_face = cur_he->face();

  if (_detect_right_dege (cur_edge, angle_cr)) {
    //: degenerate propagation to right_edge
    _prop_right_dege (cur_edge, right_he, right_interval_section);

    //: the degenerate propagation should leave room for possible rarefraction propagation.
    if (cur_edge->eV() == right_edge->eV())
      prop_tau = right_edge->len();
    else
      prop_tau = 0;
  }
  else {
    //: project cur_edge's intervals to right_edge
    prop_tau = _project_right_sections (cur_edge, right_he, angle_cr, right_interval_section);

    if (do_rarefaction_prop) {
      //: if cur_edge's sV is a boundary vertex (w.r.t. cur_he's surface), 
      //  possible pseudo-source propagation
      double a = right_interval_section->len();
      if (cur_edge->eV() == right_edge->eV()) {
        if (_lessT (0, prop_tau)) {
          gdt_interval* I = create_rf_I_to_R (cur_edge, right_he, angle_cr, 0, prop_tau);
          intersect_I_with_IS (I, right_interval_section);
        }
        else {
          //else, fix the boundary of the last interval
          vcl_map<double, gdt_ibase*>::iterator it = right_interval_section->I_map()->begin();
          gdt_ibase* I = (*it).second;
          right_interval_section->I_map()->erase (it);
          I->_set_stau (0);
          right_interval_section->_add_interval (I);
        }
      }
      else {
        if (_lessT (prop_tau, a)) {
          gdt_interval* I = create_rf_I_to_R (cur_edge, right_he, angle_cr, prop_tau, a);
          intersect_I_with_IS (I, right_interval_section);
        }
        else {
          //else, fix the boundary of the last interval
          vcl_map<double, gdt_ibase*>::reverse_iterator rit = right_interval_section->I_map()->rbegin();
          gdt_ibase* I = (*rit).second;
          I->_set_etau (a);
        }
      }
    }
  }

  return prop_tau;
}

//: detect if the propagation of cur_edge to left_edge is degenerate
bool gdt_f_manager::_detect_left_dege (dbmsh3d_gdt_edge* cur_edge, const double& alpha_l)
{
  //: test the first I of cur_edge
  assert (cur_edge->interval_section()->I_map()->size() != 0);
  vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
  gdt_interval* I = (gdt_interval*) (*it).second;
  assert (I->stau() == 0);

  //: if the current I is degenerate
  if (I->is_dege()) {
    if (I->psrc() == cur_edge->sV())
      return true;
    else
      return false;
  }

  assert (I->is_psrc());
  return I->detect_project_to_L_dege (alpha_l);
}

//: detect if the propagation of cur_edge to right_edge is degenerate
bool gdt_f_manager::_detect_right_dege (dbmsh3d_gdt_edge* cur_edge, const double& alpha_r)
{
  //: test the last I of cur_edge
  assert (cur_edge->interval_section()->I_map()->size() != 0);
  vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
  gdt_interval* I = (gdt_interval*) (*rit).second;
  assert (_eqT (I->etau(), cur_edge->len()));
    
  //: if the current I is degenerate
  if (I->is_dege()) {
    if (I->psrc() == cur_edge->eV())
      return true;
    else
      return false;
  }

  assert (I->is_psrc());
  return I->detect_project_to_R_dege (alpha_r);
}

void gdt_f_manager::_prop_left_dege (dbmsh3d_gdt_edge* cur_edge, 
                                     dbmsh3d_halfedge* left_he,
                                     gdt_interval_section* left_interval_section)
{
  dbmsh3d_gdt_vertex_3d* psrc = (dbmsh3d_gdt_vertex_3d*) cur_edge->sV();  
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();

  //:1) If the destination section is empty, add the new degenerate interval.
  if (left_interval_section->I_map()->size()==0) {
    //the prev is the first interval of cur_edge
    vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
    gdt_interval* prev = (gdt_interval*) (*it).second;

    get_psrc_dist (psrc, cur_edge);
    gdt_interval* I = create_dege_I (left_he, psrc == left_edge->sV(), prev);
    left_interval_section->_add_interval (I);
  }
  else {
    //: This is required in the degenerate propagation near saddle.
    //  If there exists any existing degenerate interval withe the same psrc, 
    //  keep it and set it's prev_ flag.
    vcl_map<double, gdt_ibase*>::iterator it = left_interval_section->I_map()->begin();
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (I->is_dege() && I->psrc() == psrc) {
      //: need to change the I's prev to this propagation!!
      I->_set_prevI (cur_edge->get_I_incident_vertex (psrc));
      return;
    }
    else { //Propagate the degenerate interval and intersect with the existing one
      //the prev is the first interval of cur_edge
      vcl_map<double, gdt_ibase*>::iterator it = cur_edge->interval_section()->I_map()->begin();
      gdt_interval* prev = (gdt_interval*) (*it).second;

      get_psrc_dist (psrc, cur_edge);
      gdt_interval* I = create_dege_I (left_he, psrc == left_edge->sV(), prev);
      intersect_I_with_IS (I, left_interval_section);
    }
  }
}

void gdt_f_manager::_prop_right_dege (dbmsh3d_gdt_edge* cur_edge, 
                                      dbmsh3d_halfedge* right_he,
                                      gdt_interval_section* right_interval_section)
{
  dbmsh3d_gdt_vertex_3d* psrc = (dbmsh3d_gdt_vertex_3d*) cur_edge->eV();  
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();

  //:1) If the destination section is empty, add the new degenerate interval.
  if (right_interval_section->I_map()->size()==0) {
    //the prev is the first interval of cur_edge
    vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
    gdt_interval* prev = (gdt_interval*) (*rit).second;

    get_psrc_dist (psrc, cur_edge);
    gdt_interval* I = create_dege_I (right_he, psrc == right_edge->sV(), prev);
    right_interval_section->_add_interval (I);
  }
  else {
    //: This is required in the degenerate propagation near saddle.
    //  If there exists any existing degenerate interval withe the same psrc, 
    //  keep it and set it's prev_ flag.
    vcl_map<double, gdt_ibase*>::iterator it = right_interval_section->I_map()->begin();
    gdt_interval* I = (gdt_interval*) (*it).second;
    if (I->is_dege() && ((gdt_interval*)I)->psrc() == psrc) {
      //: need to change the I's prev to this propagation!!
      I->_set_prevI (cur_edge->get_I_incident_vertex (psrc));
      return;
    }
    else { //Propagate the degenerate interval and intersect with the existing one
      //the prev is the first interval of cur_edge
      vcl_map<double, gdt_ibase*>::reverse_iterator rit = cur_edge->interval_section()->I_map()->rbegin();
      gdt_interval* prev = (gdt_interval*) (*rit).second;

      get_psrc_dist (psrc, cur_edge);
      gdt_interval* I = create_dege_I (right_he, psrc == right_edge->sV(), prev);
      intersect_I_with_IS (I, right_interval_section);
    }
  }
}

//: Proopagate intervals to the left_edge
//  1) Return the propagated tau for creating psrc I of B.
//     See notes. prop_tau can mean min_tau or max_tau depends on
//     the orientation of left_edge.
//  2) The modified results is in left_edge, which should be the left_he's edge
//     but can be some temporary edge, too.
//  3) Need to handle merging, or trimming of two adjacent intervals.
//     See notes for convex vertex cases.
//     Need to treat the propagation of each I as a full merging event.

//     For the first I, try to merge with all existing ones on the right_edge.
//     For the rest, just merge with the prev one.

double gdt_f_manager::_project_left_sections (const dbmsh3d_gdt_edge* cur_edge, 
                                              const dbmsh3d_halfedge* left_he,
                                              const double& angle_cl,
                                              gdt_interval_section* left_interval_section)
{
  double prop_tau;
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();
  dbmsh3d_face* cur_face = left_he->face();

  //: decide the min or max case of the prop_tau
  //  default is to create a whole section of rarefraction.
  if (cur_edge->sV() == left_edge->sV())
    prop_tau = 0;
  else
    prop_tau = left_edge->len();

  //: Propagation order: from the first one to the last.
  vcl_map<double, gdt_ibase*>::iterator it = 
    ((dbmsh3d_gdt_edge*)cur_edge)->interval_section()->I_map()->begin();
  gdt_interval* II = (gdt_interval*) (*it).second;

  //: if I is on the cur_face, the propagation is invalid.
  if (II->is_on_face (cur_face))
    return prop_tau;

  for (; it!=((dbmsh3d_gdt_edge*)cur_edge)->interval_section()->I_map()->end(); it++) {
    II = (gdt_interval*) (*it).second;

    //: For degenerate one, return to create a full rarafaction interval.
    if (II->is_dege())
      return prop_tau;

    gdt_interval* I = (gdt_interval*) (*it).second;

    double nH, nL, nStau, nEtau;
    bool b_proj_success = I->do_L_projections (angle_cl, left_edge, nL, nH, nStau, nEtau);
    if (b_proj_success == false)
      continue;

    //: error correction with prev projection.
    if (cur_edge->sV() == left_edge->sV()) {
      //modify nStau to avoid unnecessary intersection.
      if (_eqT (nStau, prop_tau))
        nStau = prop_tau;
    }
    else {
      //modify nEtau to avoid unnecessary intersection.
      if (_eqT (nEtau, prop_tau))
        nEtau = prop_tau;
    }

    //: if the project tau range is valid, create a new interval
    if (_lessT (nStau, nEtau)) {

      //: fix numerical error at boundary
      if (cur_edge->sV() == left_edge->sV()) {
        if (nStau > prop_tau)
          nStau = prop_tau;
      }
      else {        
        if (nEtau < prop_tau)
          nEtau = prop_tau;
      }

      gdt_interval* prop_I = new gdt_interval (ITYPE_PSRC, nStau, nEtau, left_he, I->psrc(), nL, nH, I);
      int result = intersect_I_with_IS (prop_I, left_interval_section);

      //: If propagation successful, remove possible invalid intervals
      if (result != 2) {
        if (cur_edge->sV() == left_edge->sV()) {
          if (nEtau < prop_tau)
            left_interval_section->delete_tau_greater_than (nEtau);
          prop_tau = nEtau;
        }
        else {
          if (nStau > prop_tau)
            left_interval_section->delete_tau_less_than (nStau);
          prop_tau = nStau;
        }
      }
    }
    
  }

  return prop_tau;
}

//: Proopagate intervals to the right_edge
//  1) Return the propagated tau for creating psrc I of A.
//     See notes. prop_tau can mean min_tau or max_tau depends on
//     the orientation of right_edge.
//  2) The modified results is in right_edge, which should be the right_he's edge
//     but can be some temporary edge, too.
//  3) Need to handle merging, or trimming of two adjacent intervals.
//     See notes for convex vertex cases.
//     For the first I, try to merge with all existing ones on the right_edge.
//     For the rest, just merge with the prev one.
double gdt_f_manager::_project_right_sections (const dbmsh3d_gdt_edge* cur_edge, 
                                               const dbmsh3d_halfedge* right_he,
                                               const double& angle_cr,
                                               gdt_interval_section* right_interval_section)
{
  double prop_tau;
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  dbmsh3d_face* cur_face = right_he->face();

  //: decide the min or max case of the prop_tau
  //  default is to create a whole section of rarefraction.
  if (cur_edge->eV() == right_edge->eV())
    prop_tau = right_edge->len();
  else
    prop_tau = 0;

  //: Propagation order: from the last one to the first.
  vcl_map<double, gdt_ibase*>::reverse_iterator it = 
    ((dbmsh3d_gdt_edge*)cur_edge)->interval_section()->I_map()->rbegin();
  gdt_interval* II = (gdt_interval*) (*it).second;

  //: if I is on the cur_face, the propagation is invalid.
  if (II->is_on_face (cur_face))
    return prop_tau;

  for (; it!=((dbmsh3d_gdt_edge*)cur_edge)->interval_section()->I_map()->rend(); it++) {
    II = (gdt_interval*) (*it).second;

    //: For degenerate one, return to create a full rarafaction interval.
    if (II->is_dege())
      return prop_tau;

    gdt_interval* I = (gdt_interval*) (*it).second;

    double nH, nL, nStau, nEtau;
    bool b_proj_success = I->do_R_projections (angle_cr, right_edge, nL, nH, nStau, nEtau);
    if (b_proj_success == false)
      continue;

    //: error correction with prev projection.
    if (cur_edge->eV() == right_edge->eV()) {
      //modify nEtau to avoid unnecessary intersection.
      if (_eqT (nEtau, prop_tau))
        nEtau = prop_tau;
    }
    else {
      //modify nStau to avoid unnecessary intersection.
      if (_eqT (nStau, prop_tau))
        nStau = prop_tau;
    }

    //: if the project tau range is valid, create a new interval
    if (_lessT (nStau, nEtau)) {

      if (cur_edge->eV() == right_edge->eV()) {
        if (nEtau < prop_tau)
          nEtau = prop_tau;
      }
      else {
        if (nStau > prop_tau)
          nStau = prop_tau;
      }

      gdt_interval* prop_I = new gdt_interval (ITYPE_PSRC, nStau, nEtau, right_he, I->psrc(), nL, nH, I);
      int result = intersect_I_with_IS (prop_I, right_interval_section);

      //: If propagation successful, remove possible invalid intervals
      if (result != 2) {
        if (cur_edge->eV() == right_edge->eV()) {
          if (nStau > prop_tau)
            right_interval_section->delete_tau_less_than (nStau);
          prop_tau = nStau;
        }
        else {
          if (nEtau < prop_tau)
            right_interval_section->delete_tau_greater_than (nEtau);
          prop_tau = nEtau;
        }
      }
    }

  }

  return prop_tau;
}


