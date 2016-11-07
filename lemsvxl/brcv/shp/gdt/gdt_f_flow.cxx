//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>

#include <gdt/gdt_manager.h>
#include <gdt/gdt_interval_section.h>

//##########################################################################
//: ====== Geodesic Wavefront Flow Testing Functions ======

//: Decide flow and retrack propagation if needed.
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
//  Rule: if the edge e is not propagated, e has flow out
//        if ei and ej are propagated, need to test propagate and decide
//          i) ei in, ii) ej in, iii) both ei and ej in
//  
//  Results in :
//    bool b_C_to_L_;
//    bool b_L_to_C_;
//    bool b_C_to_R_;
//    bool b_R_to_C_;
//    bool b_L_to_R_;
//    bool b_R_to_L_;
//    bool b_C_relay_;
//    bool b_L_relay_;
//    bool b_R_relay_;
//    bool b_ignore_L_;
//    bool b_ignore_R_;
//
PROP_CASES gdt_f_manager::decide_flow_handle_front (dbmsh3d_halfedge* cur_he, 
                                                    dbmsh3d_halfedge* left_he, 
                                                    dbmsh3d_halfedge* right_he)
{
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) cur_he->edge();
  dbmsh3d_gdt_edge* left_edge = (dbmsh3d_gdt_edge*) left_he->edge();
  dbmsh3d_gdt_edge* right_edge = (dbmsh3d_gdt_edge*) right_he->edge();
  assert (cur_edge->is_propagated());

  dbmsh3d_face* cur_face = left_he->face();

  //: cur_edge is always propagated before and possible to produce inward flow.
  //unused bool b_C_in = true;

  //: Deal with any existing 'same-direction' intervals.
  bool b_L_in;
  if (left_edge->is_propagated()) {
    //: if the input to L is inward from outside the cur_face, L flows in.
    if (!left_edge->any_interval_on_face (cur_face)) {
      b_L_in = true;
    }
    else {
      //: There exists a 'same-direction' propagation.
      //  Test propagate from both edges C and R (if exists) to decide if it is valid, 
      //  if so, ignore the Cur-Left update, only do Cur-Right
      //  otherwise, remove propagation of Left.

      //: test for possible C->same-dir L
      bool b_c_l_no_update = false;
      
      if (_test_samedir_flow (cur_he, left_he, alpha_cl_, &IS_C_to_L_, tau_c_C_to_L_))
        b_c_l_no_update = true;

      //: test for possible R->same-dir L
      bool b_r_l_no_update = false;
      if (right_edge->is_propagated() && !right_edge->any_interval_on_face (cur_face))
        if (_test_samedir_flow (right_he, left_he, alpha_lr_, &IS_R_to_L_, tau_r_R_to_L_))
          b_r_l_no_update = true;

      //: finalize the propagation on L
      if (b_c_l_no_update && b_r_l_no_update)
        //if both update is not necessary, ignore the propagation to L.
        b_ignore_L_ = true;
      else 
        //retract the propagation on L
        remove_E_propagation_on_edge (left_edge);

      b_L_in = false;
    }
  }
  else
    b_L_in = false;

  bool b_R_in;
  if (right_edge->is_propagated()) {
    //: if the input to R is inward from outside the cur_face, R flows in.
    if (!right_edge->any_interval_on_face (cur_face)) {
      b_R_in = true;
    }
    else {
      //: There exists a 'same-direction' propagation.
      //  Test propagate from both edges C and L (if exists) to decide if it is valid, 
      //  if so, ignore the Cur-Right update, only do Cur-Left
      //  otherwise, remove propagation of Right.

      //: test for possible C->same-dir R
      bool b_c_r_no_update = false;
      if (_test_samedir_flow (cur_he, right_he, alpha_cr_, &IS_C_to_R_, tau_c_C_to_R_))
        b_c_r_no_update = true;

      //: test for possible L->same-dir R 
      bool b_l_r_no_update = false;
      if (left_edge->is_propagated() && !left_edge->any_interval_on_face (cur_face))
        if (_test_samedir_flow (left_he, right_he, alpha_lr_, &IS_L_to_R_, tau_l_L_to_R_))
          b_l_r_no_update = true;

      //: finalize the propagation on R
      if (b_c_r_no_update && b_l_r_no_update)
        //if both update is not necessary, ignore the propagation to R.
        b_ignore_R_ = true;
      else
        //retract portion of propagation on R
        remove_E_propagation_on_edge (right_edge);

      b_R_in = false;
    }
  }
  else
    b_R_in = false;

  //: Analize the flow first based on existing propagations on left_edge or right_edge
  //  Also perform retraction of invalid propapgations.
  if (b_L_in) {
    if (b_R_in) {
      //:A) all 3 edges have existing IS's to merge
      FLOW_CASES result_cl = _test_flow (cur_he, left_he, alpha_cl_, b_C_to_L_, b_L_to_C_, 
                                         &IS_C_to_L_, &IS_L_to_C_, tau_c_C_to_L_, tau_l_L_to_C_);
      FLOW_CASES result_cr = _test_flow (cur_he, right_he, alpha_cr_, b_C_to_R_, b_R_to_C_, 
                                         &IS_C_to_R_, &IS_R_to_C_, tau_c_C_to_R_, tau_r_R_to_C_);
      FLOW_CASES result_lr = _test_flow (left_he, right_he, alpha_lr_, b_L_to_R_, b_R_to_L_, 
                                         &IS_L_to_R_, &IS_R_to_L_, tau_l_L_to_R_, tau_r_R_to_L_);

      //: the only complicated case!!
      PROP_CASES final_result = _determine_final_flow (result_cl, result_cr, result_lr);

      //: handle retraction of propagation if needed
      switch (final_result) {
      case PROP_CASES_ALL_IN:
      break;
      case PROP_CASES_CL_IN:
        if (!b_R_relay_) {
          retract_edge_propagation_other (right_he); //: retract the right_edge
          assert (cur_edge->interval_section()->size());
          assert (left_edge->interval_section()->size());
        }
      break;
      case PROP_CASES_CR_IN:
        if (!b_L_relay_) {
          retract_edge_propagation_other (left_he); //: retract the left_edge
          assert (cur_edge->interval_section()->size());
          assert (right_edge->interval_section()->size());
        }
      break;
      case PROP_CASES_LR_IN:
        if (!b_C_relay_) {
          retract_edge_propagation_other (cur_he); //: retract the cur_edge
          assert (left_edge->interval_section()->size());
          assert (right_edge->interval_section()->size());
        }
      break;
      case PROP_CASES_C_IN:
        if (!b_L_relay_)
          retract_edge_propagation_other (left_he); //: retract the left_edge
        if (!b_R_relay_)
          retract_edge_propagation_other (right_he); //: retract the right_edge
        assert (cur_edge->interval_section()->size());
      break;
      case PROP_CASES_L_IN:
        if (!b_C_relay_)
          retract_edge_propagation_other (cur_he); //: retract the cur_edge
        if (!b_R_relay_)
          retract_edge_propagation_other (right_he); //: retract the right_edge
        assert (left_edge->interval_section()->size());
      break;
      case PROP_CASES_R_IN:
        if (!b_C_relay_)
          retract_edge_propagation_other (cur_he); //: retract the cur_edge
        if (!b_L_relay_)
          retract_edge_propagation_other (left_he); //: retract the left_edge
        assert (right_edge->interval_section()->size());
      break;
      default:
        assert (0);
      break;
      }

      //: can be all possible valid cases.
      return final_result;
    }
    else {
      //:B) cur_edge and left_edge have existing IS's
      FLOW_CASES result = _test_flow (cur_he, left_he, alpha_cl_, b_C_to_L_, b_L_to_C_, 
                                      &IS_C_to_L_, &IS_L_to_C_, tau_c_C_to_L_, tau_l_L_to_C_);

      //: handle retraction of propagation if needed
      switch (result) {
      case FLOW_CASES_1_IN:
        retract_edge_propagation_other (left_he); //: retract the left_edge
        assert (cur_edge->interval_section()->size());
        return PROP_CASES_C_IN;
      case FLOW_CASES_2_IN:
        retract_edge_propagation_other (cur_he); //: retract the cur_edge
        assert (left_edge->interval_section()->size());
        return PROP_CASES_L_IN;
      case FLOW_CASES_12_IN:
        return PROP_CASES_CL_IN;
      case FLOW_CASES_1_IN_2_RELAY :
        b_L_relay_ = true;
        return PROP_CASES_C_IN;
      case FLOW_CASES_2_IN_1_RELAY:
        b_C_relay_ = true;
        return PROP_CASES_L_IN;
      default:
        break;
      }
    }
  }
  else {
    if (b_R_in) {
      //:C) cur_edge and right_edge have existing IS's
      FLOW_CASES result = _test_flow (cur_he, right_he, alpha_cr_, b_C_to_R_, b_R_to_C_, 
                                      &IS_C_to_R_, &IS_R_to_C_, tau_c_C_to_R_, tau_r_R_to_C_);

      //: handle retraction of propagation if needed
      switch (result) {
      case FLOW_CASES_1_IN:
        retract_edge_propagation_other (right_he); //: retract the right_edge
        assert (cur_edge->interval_section()->size());
        return PROP_CASES_C_IN;
      case FLOW_CASES_2_IN:
        retract_edge_propagation_other (cur_he); //: retract the cur_edge
        assert (right_edge->interval_section()->size());
        return PROP_CASES_R_IN;
      case FLOW_CASES_12_IN:
        return PROP_CASES_CR_IN;
      case FLOW_CASES_1_IN_2_RELAY:
        b_R_relay_ = true;
        return PROP_CASES_C_IN;
      case FLOW_CASES_2_IN_1_RELAY:
        b_C_relay_ = true;
        return PROP_CASES_R_IN;
      default:
        break;
      }
    }
    else {
      //:D) only cur_edge is in
      return PROP_CASES_C_IN;
    }
  }

  return BOGUS_PROP_CASES;
}

//: return true if the existing dest_IS is valid
//  i.e., if from_IS -> dest_IS has no update to the existing dest_IS 
bool gdt_f_manager::_test_samedir_flow (dbmsh3d_halfedge* from_he, dbmsh3d_halfedge* dest_he,
                                        const double& angle_fd,
                                        gdt_interval_section* IS_F_to_D, double& tau_f_F_to_D)
{
  dbmsh3d_gdt_edge* from_edge = (dbmsh3d_gdt_edge*) from_he->edge();
  dbmsh3d_gdt_edge* dest_edge = (dbmsh3d_gdt_edge*) dest_he->edge();

  //:1) test propagation from from_edge to dest_edge
  ///gdt_interval_section IS_f_to_d (dest_edge->len());
  if (dest_edge->is_V_incident (from_edge->sV()))
    tau_f_F_to_D = prop_to_left_edge (from_he, dest_he, angle_fd, IS_F_to_D, true);
  else
    tau_f_F_to_D = prop_to_right_edge (from_he, dest_he, angle_fd, IS_F_to_D, true);

  gdt_interval_section IS_d_existing (dest_edge->len());
  clone_intervals (dest_edge->interval_section(), &IS_d_existing);

  //: decide flow by comparing f->d and existing_d
  gdt_interval_section IS_d_intersected (dest_edge->len());
  INTERSECT_RESULT result = intersect_interval_sections (IS_F_to_D, &IS_d_existing, &IS_d_intersected);

  if (result == INTERSECT_RESULT_2 || result == INTERSECT_RESULT_0)
    return true;
  else
    return false;
}

//: assume both from_edge and dest_edge are propagated
//  test propagate f -> d and d -> f to decide the true flow
//  The following results are possible:
//  - 1 in
//  - 2 in
//  - Both 1 and 2 in
//  - 1 in, 2 relay
//  - 2 in, 1 relay
FLOW_CASES gdt_f_manager::_test_flow (dbmsh3d_halfedge* he1, dbmsh3d_halfedge* he2,
                                      const double& angle_12,
                                      bool& b_1_to_2, bool& b_2_to_1,
                                      gdt_interval_section* IS_1_to_2,
                                      gdt_interval_section* IS_2_to_1,
                                      double& tau1_1_to_2, double& tau2_2_to_1)
{
  dbmsh3d_gdt_edge* edge1 = (dbmsh3d_gdt_edge*) he1->edge();
  dbmsh3d_gdt_edge* edge2 = (dbmsh3d_gdt_edge*) he2->edge();

  //:1) test propagation from edge1 to edge2
  if (edge2->is_V_incident (edge1->sV()))
    tau1_1_to_2 = prop_to_left_edge (he1, he2, angle_12, IS_1_to_2, true);
  else
    tau1_1_to_2 = prop_to_right_edge (he1, he2, angle_12, IS_1_to_2, true);

  gdt_interval_section IS_2_existing (edge2->len());
  clone_intervals (edge2->interval_section(), &IS_2_existing);

  //: decide flow by intersecting propagation of IS_1_to_2 and IS_2_existing
  gdt_interval_section IS_2_intersected (edge2->len());
  INTERSECT_RESULT result_2 = intersect_interval_sections (IS_1_to_2, &IS_2_existing, &IS_2_intersected);

  switch (result_2) {
  case INTERSECT_RESULT_1: // 1->2 wins
    return FLOW_CASES_1_IN;
  break;
  case INTERSECT_RESULT_2: // IS_2_existing wins, no decision can be made yet, test 2->1
  {
    //:2) test propagation from edge2 to edge1
    if (edge1->is_V_incident (edge2->sV()))
      tau2_2_to_1 = prop_to_left_edge (he2, he1, angle_12, IS_2_to_1, true);
    else
      tau2_2_to_1 = prop_to_right_edge (he2, he1, angle_12, IS_2_to_1, true);

    gdt_interval_section IS_1_existing (edge1->len());
    clone_intervals (edge1->interval_section(), &IS_1_existing);

    //: decide flow by trying to merge propagation of IS_2_to_1 and IS_1_existing
    gdt_interval_section IS_1_intersected (edge1->len());
    INTERSECT_RESULT result_1 = intersect_interval_sections (IS_2_to_1, &IS_1_existing, &IS_1_intersected);
    switch (result_1) {
    case INTERSECT_RESULT_1:
      return FLOW_CASES_2_IN; //existing_2 wins
    case INTERSECT_RESULT_2: 
      return FLOW_CASES_12_IN; //1->2,2 win and 2->1,1win: both 1 and 2 wins
    case INTERSECT_RESULT_3:
      b_2_to_1 = true;
      return FLOW_CASES_12_IN; //2->1 and existing_1 merges
    case INTERSECT_RESULT_4:
      b_1_to_2 = true;
      b_2_to_1 = true;
      return FLOW_CASES_12_IN;
    case INTERSECT_RESULT_0:
      return FLOW_CASES_2_IN_1_RELAY; //dege equal of 2->1 and existing_1
    default:
        break;
    }
  }
  break;
  case INTERSECT_RESULT_3: // 1->2 and existing_2 merges, test 2->1
  {
    b_1_to_2 = true;

    //: test if b_2_to_1 true or not, i.e., 2->1 a merge or not
    if (edge1->is_V_incident (edge2->sV()))
      tau2_2_to_1 = prop_to_left_edge (he2, he1, angle_12, IS_2_to_1, true);
    else
      tau2_2_to_1 = prop_to_right_edge (he2, he1, angle_12, IS_2_to_1, true);

    gdt_interval_section IS_1_existing (edge1->len());
    clone_intervals (edge1->interval_section(), &IS_1_existing);

    //: decide flow by trying to merge propagation of IS_2_to_1 and IS_1_existing
    gdt_interval_section IS_1_intersected (edge1->len());
    INTERSECT_RESULT result_1 = intersect_interval_sections (IS_2_to_1, &IS_1_existing, &IS_1_intersected);
    switch (result_1) {
    case INTERSECT_RESULT_1: //2->1 wins, no possible
      ///assert (0); //??? possible?
      return FLOW_CASES_2_IN; //bad fix??
    case INTERSECT_RESULT_2: //existing_1 wins
      return FLOW_CASES_12_IN;
    case INTERSECT_RESULT_3: //again a merge
      b_2_to_1 = true;
      return FLOW_CASES_12_IN;
    case INTERSECT_RESULT_4:
      b_1_to_2 = true;
      b_2_to_1 = true;
      return FLOW_CASES_12_IN;
    case INTERSECT_RESULT_0: //relay, this is possible!
      ///assert (0);
    break;
    default:
        break;
    }
    return FLOW_CASES_12_IN;
  }
  break;
  case INTERSECT_RESULT_4: // 1->2 and existing_2 forms a partition
  {
    b_1_to_2 = true;
    b_2_to_1 = true;
    return FLOW_CASES_12_IN;
  }
  break;
  case INTERSECT_RESULT_0: // dege equal of 1->2 and existing_2
    return FLOW_CASES_1_IN_2_RELAY;
  break;    
  default:
  break;
  }

  return BOGUS_FLOW_CASES;
}

PROP_CASES gdt_f_manager::_determine_final_flow (FLOW_CASES result_cl, FLOW_CASES result_cr, FLOW_CASES result_lr)
{
  bool b_C_in = false;
  bool b_L_in = false;
  bool b_R_in = false;

  //:1) For all possible 'in' case, set the 'in' flag to be true.
  switch (result_cl) {
  case FLOW_CASES_1_IN: b_C_in = true; break;
  case FLOW_CASES_2_IN: b_L_in = true; break;
  case FLOW_CASES_12_IN: b_C_in = true; b_L_in = true; break;
  case FLOW_CASES_1_IN_2_RELAY: b_C_in = true; break;
  case FLOW_CASES_2_IN_1_RELAY: b_L_in = true; break;
  default: assert (0); break;
  }
  switch (result_cr) {
  case FLOW_CASES_1_IN: b_C_in = true; break;
  case FLOW_CASES_2_IN: b_R_in = true; break;
  case FLOW_CASES_12_IN: b_C_in = true; b_R_in = true; break;
  case FLOW_CASES_1_IN_2_RELAY: b_C_in = true; break;
  case FLOW_CASES_2_IN_1_RELAY: b_R_in = true; break;
  default: assert (0); break;
  }
  switch (result_lr) {
  case FLOW_CASES_1_IN: b_L_in = true; break;
  case FLOW_CASES_2_IN: b_R_in = true; break;
  case FLOW_CASES_12_IN: b_L_in = true; b_R_in = true; break;
  case FLOW_CASES_1_IN_2_RELAY: b_L_in = true; break;
  case FLOW_CASES_2_IN_1_RELAY: b_R_in = true; break;
  default: assert (0); break;
  }

  //:2) For all possible not 'in' case, set the 'in' flag to be false.
  switch (result_cl) {
  case FLOW_CASES_1_IN: b_L_in = false; break;
  case FLOW_CASES_2_IN: b_C_in = false; break;
  default:
        break;
  }
  switch (result_cr) {
  case FLOW_CASES_1_IN: b_R_in = false; break;
  case FLOW_CASES_2_IN: b_C_in = false; break;
  default:
        break;
  }
  switch (result_lr) {
  case FLOW_CASES_1_IN: b_R_in = false; break;
  case FLOW_CASES_2_IN: b_L_in = false; break;
  default:
        break;
  }

  //:3) For all 'relay' cases, set the 'in' flag to be false,
  //    and set the 'relay' flag to be 'possible' true.
  bool b_C_relay_from_L = false;
  bool b_C_relay_from_R = false;
  bool b_L_relay_from_C = false;
  bool b_L_relay_from_R = false;
  bool b_R_relay_from_C = false;
  bool b_R_relay_from_L = false;
  if (result_cl == FLOW_CASES_1_IN_2_RELAY) {
    b_L_in = false;
    b_L_relay_from_C = true;
  }
  else if (result_cl == FLOW_CASES_2_IN_1_RELAY) {
    b_C_in = false;
    b_C_relay_from_L = true;
  }

  if (result_cr == FLOW_CASES_1_IN_2_RELAY) {
    b_R_in = false;
    b_R_relay_from_C = true;
  }
  else if (result_cr == FLOW_CASES_2_IN_1_RELAY) {
    b_C_in = false;
    b_C_relay_from_R = true;
  }

  if (result_lr == FLOW_CASES_1_IN_2_RELAY) {
    b_R_in = false;
    b_R_relay_from_L = true;
  }
  else if (result_lr == FLOW_CASES_2_IN_1_RELAY) {
    b_L_in = false;
    b_L_relay_from_R = true;
  }

  //: C is a relay if one of the following is true:
  //  - b_C_relay_from_L and b_R_to_C_ is false
  //  - b_C_relay_from_R and b_L_to_C_ is false
  if (b_C_relay_from_L && b_R_to_C_ == false ||
      b_C_relay_from_R && b_L_to_C_ == false)
    b_C_relay_ = true;

  //: L is a relay if one of the following is true:
  //  - b_L_relay_from_C and b_R_to_L_ is false
  //  - b_L_relay_from_R and b_C_to_L_ is false
  if (b_L_relay_from_C && b_R_to_L_ == false ||
      b_L_relay_from_R && b_C_to_L_ == false)
    b_L_relay_ = true;

  //: R is a relay if one of the following is true:
  //  - b_R_relay_from_C and b_L_to_R_ is false
  //  - b_R_relay_from_L and b_C_to_R_ is false
  if (b_R_relay_from_C && b_L_to_R_ == false ||
      b_R_relay_from_L && b_C_to_R_ == false)
    b_R_relay_ = true;

  //:4) Decide the final result case.
  if (b_C_in && b_L_in && b_R_in)
    return PROP_CASES_ALL_IN;
  else if (b_C_in && b_L_in)
    return PROP_CASES_CL_IN;
  else if (b_C_in && b_R_in)
    return PROP_CASES_CR_IN;
  else if (b_L_in && b_R_in)
    return PROP_CASES_LR_IN;
  else if (b_C_in)
    return PROP_CASES_C_IN;
  else if (b_L_in)
    return PROP_CASES_L_IN;
  else if (b_R_in)
    return PROP_CASES_R_IN;
  else {
    assert (0);
    return BOGUS_PROP_CASES;
  }
}



