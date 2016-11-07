//: Aug 19, 2005 MingChing Chang
//  

#include <gdt/gdt_manager.h>
#include <gdt/gdt_interval.h>
#include <gdt/gdt_solve_intersect.h>
#include <gdt/gdt_fs_shock_prop.h>
#include <gdt/gdt_interval_section.h>

#if GDT_ALGO_FS

//: Initialize and propagate shocks to
//  decide flow and retrack propagation if needed.
//  The cur_he, left_he, right_he are all halfedges of the cur_face.
//  the case analysis for propagating each face of the mesh, 3 cases
//  A) 1 in
//     A-1) cur_edge in
//     A-2) left_edge in
//     A-3) right_edge in
//  B) 2 in
//     B-1) cur_edge and left_edge in
//     B-2) cur_edge and right_edge in
//     B-3) left_edge and right_edge in
//  C) 3 in a sink case.
//
void gdt_fs_manager::propagate_shocks_on_cur_face ()
{
  assert (cur_edge_->is_propagated());

  dbmsh3d_face* cur_face = left_he_->face();

  bool b_L_in;
  if (left_edge_->is_propagated())
    b_L_in = true;
  else
    b_L_in = false;

  bool b_R_in;
  if (right_edge_->is_propagated())
    b_R_in = true;
  else
    b_R_in = false;

  //////////////////////////////////////////////////
  //:1) Initialize all shocks
  gdt_shock_prop shock_prop (cur_face, 
                             cur_edge_,
                             left_edge_,
                             right_edge_,
                             &shocks_to_cur_edge_, 
                             &shocks_to_left_edge_, 
                             &shocks_to_right_edge_, 
                             n_verbose_);

  //: Initialize all corner shocks
  //  and put all shocks on edges into the shock queue
  //  The shock queue is a map of <time, shock>.
  cur_edge_->shock_section()->put_all_shocks_to_queue (shock_prop.shock_queue());
  
  gdt_shock *S = NULL;
  gdt_shock *S1 = NULL;
  gdt_shock *S2 = NULL;

  if (b_L_in) {
    left_edge_->shock_section()->put_all_shocks_to_queue (shock_prop.shock_queue());
    if (b_R_in) {
      right_edge_->shock_section()->put_all_shocks_to_queue (shock_prop.shock_queue());
      
      //:A) all 3 edges have existing interval_sections
      CLR_FLOW_CASES CLR_flow = determine_CLR_cases ();

      switch (CLR_flow) {
      case CLR_FLOW_CASES_C_IN:

        //: try forming possible contact shock on sV of C
        
        //: try forming possible contact shock on eV of C
      case CLR_FLOW_CASES_L_IN:

        //: try forming possible contact shock on sV of L
        
        //: try forming possible contact shock on eV of L
      case CLR_FLOW_CASES_R_IN:

        //: try forming possible contact shock on sV of R
        
        //: try forming possible contact shock on eV of R
      break;
      case CLR_FLOW_CASES_CL_IN:
      break;
      case CLR_FLOW_CASES_CL_IN_SADDLE:
      break;
      case CLR_FLOW_CASES_CR_IN:
      break;
      case CLR_FLOW_CASES_CR_IN_SADDLE:
      break;
      case CLR_FLOW_CASES_LR_IN:
      break;
      case CLR_FLOW_CASES_LR_IN_SADDLE:
      break;
      case CLR_FLOW_CASES_CLR_IN:
      break;
      case CLR_FLOW_CASES_CLR_IN_CL_SADDLE:
      break;
      case CLR_FLOW_CASES_CLR_IN_CR_SADDLE:
      break;
      case CLR_FLOW_CASES_CLR_IN_LR_SADDLE:
      break;
      }
    }
    else {
      //:B) cur_edge_ and left_edge_ have existing interval_sections
      //    Possible cases are
      //      - B1) CL in
      //      - B1S) CL in saddle
      //      - B2) C in: retract L
      //      - B2C) C in with contact shock: retract L
      //      - B3) L in: retract C
      //      - B3C) L in with contact shock: retract C
      CL_FLOW_CASES CL_flow = determine_CL_cases ();

      switch (CL_flow) {
      case CL_FLOW_CASES_CL_IN:
        ///S = create_CL_line_shock ();
        shock_prop._add_shock (S);
        cur_edge_->add_shock (S);
        left_edge_->add_shock (S);
      break;
      case CL_FLOW_CASES_CL_IN_SADDLE:
        ///create_CL_rarefaction (S1, S2);
      break;
      case CL_FLOW_CASES_C_IN:
        //: retract L
        fs_retract_edge_other (left_he_);

        //: try forming possible contact shock on the other side

      break;
      case CL_FLOW_CASES_L_IN:
        //: retract C
        fs_retract_edge_other (cur_he_);

        //: try forming possible contact shock on the other side

      break;
      }
    }
  }
  else {
    if (b_R_in) {
      right_edge_->shock_section()->put_all_shocks_to_queue (shock_prop.shock_queue());

      //:C) cur_edge_ and right_edge_ have existing interval_sections
      //    Possible cases are
      //      - C1) CR in
      //      - C1S) CR in saddle
      //      - C2) C in: retract R
      //      - C2C) C in with contact shock: retract R
      //      - C3) R in: retract C
      //      - C3C) R in with contact shock: retract C
      CR_FLOW_CASES CR_flow = determine_CR_cases ();

      switch (CR_flow) {
      case CR_FLOW_CASES_CR_IN:
        ///S = create_CR_line_shock ();
        shock_prop._add_shock (S);
        cur_edge_->add_shock (S);
        right_edge_->add_shock (S);
      break;
      case CR_FLOW_CASES_CR_IN_SADDLE:
        ///create_CR_rarefaction (S1, S2);
      break;
      case CR_FLOW_CASES_C_IN:
        //: retract R
        fs_retract_edge_other (right_he_);

        //: try forming possible contact shock on the other side
      break;
      case CR_FLOW_CASES_R_IN:
        //: retract C
        fs_retract_edge_other (cur_he_);

        //: try forming possible contact shock on the other side
      break;
      }
    }
    else {
      //:D) only cur_edge_ is in
      //    Here only one of the 3 cases D1, D2, D3 is possible.
      //    One can speed up the code here!

      //D1) No contact shock is formed.
      //D2) Try forming left contact shock at vertex_CL from cur_edge_ to right_edge_
      /*gdt_shock* S = try_form_left_contact_shock (cur_edge_, right_edge_);
      if (S) {
        shock_prop._add_shock (S);
        cur_edge_->add_shock (S);
      }

      //D3) Try forming right contact shock at vertex_CR from cur_edge_ to left_edge_
      S = try_form_right_contact_shock (cur_edge_, left_edge_);
      if (S) {
        shock_prop._add_shock (S);
        cur_edge_->add_shock (S);
      }*/
    }
  }

  //////////////////////////////////////////////////
  //:2) Propagate all shocks
  shocks_to_left_edge_.clear();
  shocks_to_right_edge_.clear();
  shocks_to_cur_edge_.clear();

  shock_prop.propagate_gdt_shocks ();

}

// #######################################################################

CL_FLOW_CASES gdt_fs_manager::determine_CL_cases ()
{
  vcl_map<double, gdt_ibase*>::iterator it;
  vcl_map<double, gdt_ibase*>::reverse_iterator rit;

  //: the first I of cur_edge_
  it = cur_edge_->interval_section()->I_map()->begin();
  curI_ = (gdt_interval*) (*it).second;

  //: the other I of left_edge_
  if (cur_edge_->sV() == left_edge_->sV()) { // I, II
    it = left_edge_->interval_section()->I_map()->begin();
    leftI_ = (gdt_interval*) (*it).second; 
  }
  else { // III, IV
    rit = left_edge_->interval_section()->I_map()->rbegin();
    leftI_ = (gdt_interval*) (*rit).second; 
  }

  //: the case of degenerate curI_ should be handled in other module.
  assert (curI_->is_dege() == false);
  gdt_interval* CI = (gdt_interval*) curI_;

  //project C to R
  double C_proj_h, C_proj_l, C_proj_tau_s, C_proj_tau_e;
  CI->do_R_projections (alpha_cr_, right_edge_, C_proj_l, C_proj_h, C_proj_tau_s, C_proj_tau_e);

  //: if leftI_ is degenerate, curI_ should dominate.
  if (leftI_->is_dege()) {
    //: 3 cases of projection of CI to R.
    if (cur_edge_->eV() == right_edge_->eV()) { // I, III: check C_proj_tau_s

    }
    else { // II, IV: check C_proj_tau_e
    }

    return CL_FLOW_CASES_CL_IN; ///!!!CL_FLOW_CASES_C_IN;
  }

  //////////////////////////////////////////////////////////////
  //: now handle the non-degenerate leftI_.
  gdt_interval* LI = (gdt_interval*) leftI_;

  //project L to R
  double L_proj_h, L_proj_l, L_proj_tau_s, L_proj_tau_e;
  if (cur_edge_->sV() == left_edge_->sV()) // I, II
    LI->do_R_projections (alpha_lr_, right_edge_, L_proj_l, L_proj_h, L_proj_tau_s, L_proj_tau_e);
  else // III, IV
    LI->do_L_projections (alpha_lr_, right_edge_, L_proj_l, L_proj_h, L_proj_tau_s, L_proj_tau_e);

  // !! now only handle line shocks.
  assert (CI->psource() == LI->psource());

  //: decide the solution cases A, B, C.
  if (cur_edge_->eV() == right_edge_->eV()) { // I, III
    //(min, max) of the equal dist point
    tau_min_ = C_proj_tau_s;
    tau_max_ = L_proj_tau_e;

    //: if tau_min_ fuzzily equal to tau_max_, it's the planar-vertex case.
    if (_eqT (tau_min_, tau_max_)) {
      sol_tau_ = (tau_min_ + tau_max_)*0.5;
      dbmsh3d_gdt_vertex_3d* psource = (dbmsh3d_gdt_vertex_3d*) cur_edge_->sV();
      sol_dist_ = get_psrc_dist (psource, cur_edge_);
      return CL_FLOW_CASES_CL_IN;
    }

    //: if tau_min_ greater than tau_max_, it's the saddle case.
    if (tau_max_ < tau_min_)
      return CL_FLOW_CASES_CL_IN_SADDLE;

    //: now solve for the equal-dist tau.
    sol_tau_ = solve_eqdist_tau (C_proj_l, C_proj_h, CI->mu(), L_proj_l, L_proj_h, LI->mu(),
                                 tau_min_, tau_max_);
    assert (sol_tau_ != INVALID_TAU);

    if (_lessT (sol_tau_, 0)) { // A: CL_FLOW_CASES_C_IN
      return CL_FLOW_CASES_C_IN;
    }
    else if (_lessT (right_edge_->len(), sol_tau_)) { // C: CL_FLOW_CASES_L_IN
      return CL_FLOW_CASES_L_IN;
    }
    else { // B: CL_FLOW_CASES_CL_IN: a line shock will form later
      return CL_FLOW_CASES_CL_IN;
    }
  }
  else { // II, IV
    //(min, max) of the equal dist point
    tau_min_ = L_proj_tau_s;    
    tau_max_ = C_proj_tau_e;

    //: if tau_min_ fuzzily equal to tau_max_, it's the planar-vertex case.
    if (_eqT (tau_min_, tau_max_)) {
      sol_tau_ = (tau_min_ + tau_max_)*0.5;
      dbmsh3d_gdt_vertex_3d* psource = (dbmsh3d_gdt_vertex_3d*) cur_edge_->sV();
      sol_dist_ = get_psrc_dist (psource, cur_edge_);
      return CL_FLOW_CASES_CL_IN;
    }

    //: if tau_min_ greater than tau_max_, it's the saddle case.
    if (tau_max_ < tau_min_)
      return CL_FLOW_CASES_CL_IN_SADDLE;

    //: now solve for the equal-dist tau.
    sol_tau_ = solve_eqdist_tau (C_proj_l, C_proj_h, CI->mu(), L_proj_l, L_proj_h, LI->mu(),
                                 tau_min_, tau_max_);
    assert (sol_tau_ != INVALID_TAU);

    if (_lessT (sol_tau_, 0)) { // C: CL_FLOW_CASES_L_IN
      return CL_FLOW_CASES_L_IN;
    }
    else if (_lessT (right_edge_->len(), sol_tau_)) { // A: CL_FLOW_CASES_C_IN
      return CL_FLOW_CASES_C_IN;
    }
    else { // B: CL_FLOW_CASES_CL_IN: a line shock will form later
      return CL_FLOW_CASES_CL_IN;
    }
  }
}

CR_FLOW_CASES gdt_fs_manager::determine_CR_cases ()
{
  vcl_map<double, gdt_ibase*>::iterator it;
  vcl_map<double, gdt_ibase*>::reverse_iterator rit;

  //: the last I of cur_edge_
  rit = cur_edge_->interval_section()->I_map()->rbegin();
  curI_ = (gdt_interval*) (*rit).second;

  //: the other I of right_edge_
  if (cur_edge_->eV() == right_edge_->eV()) { // I, III
    rit = right_edge_->interval_section()->I_map()->rbegin();
    rightI_ = (gdt_interval*) (*rit).second; 
  }
  else { // II, IV
    it = right_edge_->interval_section()->I_map()->begin();
    rightI_ = (gdt_interval*) (*it).second; 
  }

  //: the case of degenerate curI_ should be handled in other module.
  assert (curI_->is_dege() == false);

  //: if leftI_ is degenerate, curI_ should dominate.
  if (rightI_->is_dege()) {
    return CR_FLOW_CASES_C_IN; //!!
  }

  gdt_interval* CI = (gdt_interval*) curI_;
  gdt_interval* RI = (gdt_interval*) rightI_;

  //: project curI_ and rightI_ to left_edge_
  //project C to L
  double C_proj_h, C_proj_l, C_proj_tau_s, C_proj_tau_e;
  CI->do_L_projections (alpha_cl_, left_edge_, C_proj_l, C_proj_h, C_proj_tau_s, C_proj_tau_e);
  //project R to L
  double R_proj_h, R_proj_l, R_proj_tau_s, R_proj_tau_e;
  if (cur_edge_->eV() == right_edge_->eV()) // I, III
    RI->do_L_projections (alpha_lr_, left_edge_, R_proj_l, R_proj_h, R_proj_tau_s, R_proj_tau_e);
  else // II, IV
    RI->do_R_projections (alpha_lr_, left_edge_, R_proj_l, R_proj_h, R_proj_tau_s, R_proj_tau_e);

  // !! now only handle line shocks.
  assert (CI->psource() == RI->psource());

  //: decide the solution cases A, B, C.
  if (cur_edge_->sV() == left_edge_->sV()) { // I, II
    //(min, max) of the equal dist point
    tau_min_ = R_proj_tau_s;
    tau_max_ = C_proj_tau_e;

    //: if tau_min_ fuzzily equal to tau_max_, it's the planar-vertex case.
    if (_eqT (tau_min_, tau_max_)) {
      sol_tau_ = (tau_min_ + tau_max_)*0.5;
      dbmsh3d_gdt_vertex_3d* psource = (dbmsh3d_gdt_vertex_3d*) cur_edge_->eV();
      sol_dist_ = get_psrc_dist (psource, cur_edge_);
      return CR_FLOW_CASES_CR_IN;
    }

    //: if tau_min_ greater than tau_max_, it's the saddle case.
    if (tau_max_ < tau_min_)
      return CR_FLOW_CASES_CR_IN_SADDLE;

    //: now solve for the equal-dist tau.
    sol_tau_ = solve_eqdist_tau (C_proj_l, C_proj_h, CI->mu(), R_proj_l, R_proj_h, RI->mu(),
                                 tau_min_, tau_max_);
    assert (sol_tau_ != INVALID_TAU);

    if (_lessT (sol_tau_, 0)) { // C: CR_FLOW_CASES_R_IN
      return CR_FLOW_CASES_R_IN;
    }
    else if (_lessT (left_edge_->len(), sol_tau_)) { // A: CR_FLOW_CASES_C_IN
      return CR_FLOW_CASES_C_IN;
    }
    else { // B: CR_FLOW_CASES_CR_IN: a line shock will form later
      return CR_FLOW_CASES_CR_IN;
    }
  }
  else { // III, IV
    //(min, max) of the equal dist point
    tau_min_ = C_proj_tau_s;
    tau_max_ = R_proj_tau_e;

    //: if tau_min_ greater than tau_max_, it's the saddle case.
    if (tau_max_ < tau_min_) {
      sol_tau_ = (tau_min_ + tau_max_)*0.5;
      dbmsh3d_gdt_vertex_3d* psource = (dbmsh3d_gdt_vertex_3d*) cur_edge_->eV();
      sol_dist_ = get_psrc_dist (psource, cur_edge_);
      return CR_FLOW_CASES_CR_IN;
    }

    //: if tau_min_ greater than tau_max_, it's the saddle case.
    if (tau_max_ < tau_min_)
      return CR_FLOW_CASES_CR_IN_SADDLE;

    //: now solve for the equal-dist tau.
    sol_tau_ = solve_eqdist_tau (C_proj_l, C_proj_h, CI->mu(), R_proj_l, R_proj_h, RI->mu(),
                                 tau_min_, tau_max_);
    assert (sol_tau_ != INVALID_TAU);

    if (_lessT (sol_tau_, 0)) { // A: CR_FLOW_CASES_C_IN
      return CR_FLOW_CASES_C_IN;
    }
    else if (_lessT (right_edge_->len(), sol_tau_)) { // C: CR_FLOW_CASES_R_IN
      return CR_FLOW_CASES_R_IN;
    }
    else { // B: CR_FLOW_CASES_CR_IN: a line shock will form later
      return CR_FLOW_CASES_CR_IN;
    }
  }
}

CLR_FLOW_CASES gdt_fs_manager::determine_CLR_cases ()
{
  return BOGUS_CLR_FLOW_CASES;
}

// #######################################################################


#endif


