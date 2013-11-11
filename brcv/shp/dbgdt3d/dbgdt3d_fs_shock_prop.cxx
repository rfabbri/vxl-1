//: Aug 19, 2005 MingChing Chang
//  

#include <vnl/vnl_math.h>
#include <dbgdt3d/dbgdt3d_fs_shock_prop.h>
#include <dbgdt3d/dbgdt3d_edge.h>

#if GDT_ALGO_FS

// ###########################################
// ====== Shock propagation functions ======

//: The propagation should be linear as follows.
//  For each shock, try to intersect with 
//  left_neighbor, right_neighbor, and the two other edges.
void gdt_shock_prop::propagate_gdt_shocks ()
{
#if GDT_DEBUG_MSG
  if (n_verbose_>1)
    vul_printf (vcl_cout, "\n     ----> Total shocks to propagate: %d\n", shock_queue_.size());

  int n_prop_iter = 0;

  while (shock_queue_.size() != 0) {
    if (n_verbose_>1)
      vul_printf (vcl_cout, "           s_iter %d \n", n_prop_iter);
    propagate_gdt_shock_one_step ();
    n_prop_iter++;
  }
  vul_printf (vcl_cout, "     ----> Total s_iter %d.\n", n_prop_iter);
#else
  while (shock_queue_.size() != 0)
    propagate_gdt_shock_one_step ();
#endif
}

void gdt_shock_prop::propagate_gdt_shock_one_step ()
{
#if GDT_DEBUG_MSG
  if (n_verbose_>1) {
    vul_printf (vcl_cout, "shock_queue %d: ", shock_queue_.size());
    vcl_map<vcl_pair<double, int>, gdt_shock*>::iterator sit =  shock_queue_.begin();
    if (n_verbose_>2) {
      for (; sit != shock_queue_.end(); sit++) {
        gdt_shock* S = (*sit).second;
        vul_printf (vcl_cout, "%d(%f) ", S->id(), (float)(*sit).first.first);
      }
    }
    vul_printf (vcl_cout, "\n");
  }
#endif

  //: pop the closest shock from the shock_queue_ and propagate it.
  vcl_map<vcl_pair<double, int>, gdt_shock*>::iterator sit = shock_queue_.begin();

  // ASSUME ALL SHOCK ARE LINEAR NOW !!
  gdt_shock* S = (gdt_shock*) (*sit).second;
  shock_queue_.erase (sit); //: remove wit from the shock_queue_

  //: Two types of propagations,
  //  - A) for shock from a single edge (including constact shock), determine the cur_edge, left_edge, and right_edge
  //  - B) for shock between two edges, determine the from_vertex and dest_edge

  if (S->leftI() == NULL || S->rightI() == NULL ||
      S->leftI()->edge() == S->rightI()->edge()) { //: A) shock from an edge
    dbmsh3d_gdt_edge* s_cur_edge;
  
    if (S->leftI() == NULL) //Case L1, L2, L3)
      s_cur_edge = S->rightI()->edge();
    else if (S->rightI() == NULL) //Case R1, R2, R3)
      s_cur_edge = S->leftI()->edge();
    else if (S->leftI()->edge() == S->rightI()->edge()) //Case M1, M2, M3)
      s_cur_edge = S->leftI()->edge();
    
    if (s_cur_edge == cur_edge_) //CLR
      _propagate_shock_from_edge (S, cur_edge_, left_edge_, right_edge_,
                                  shocks_to_cur_edge_, shocks_to_left_edge_, shocks_to_right_edge_);
    else if (s_cur_edge == left_edge_)
      if (cur_edge_->sV() == left_edge_->sV()) //LCR
        _propagate_shock_from_edge (S, left_edge_, cur_edge_, right_edge_,
                                    shocks_to_left_edge_, shocks_to_cur_edge_, shocks_to_right_edge_);
      else //LRC
        _propagate_shock_from_edge (S, left_edge_, right_edge_, cur_edge_,
                                    shocks_to_left_edge_, shocks_to_right_edge_, shocks_to_cur_edge_);
    else {
      assert (s_cur_edge == right_edge_);
      if (cur_edge_->eV() == right_edge_->eV()) //RLC
        _propagate_shock_from_edge (S, right_edge_, left_edge_, cur_edge_, 
                                    shocks_to_right_edge_, shocks_to_left_edge_, shocks_to_cur_edge_);
      else //RCL
        _propagate_shock_from_edge (S, right_edge_, cur_edge_, left_edge_,
                                    shocks_to_right_edge_, shocks_to_cur_edge_, shocks_to_left_edge_);
    }
  }
  else { //: B) shock from a vertex: Case CL, CR, LR
    dbmsh3d_gdt_edge* s_left_from_edge = S->leftI()->edge();
    dbmsh3d_gdt_edge* s_right_from_edge = S->rightI()->edge();

    if (S->leftI()->edge() == cur_edge_) {
      if (S->rightI()->edge() == left_edge_) //CL->R
        _propagate_shock_from_vertex (S, cur_edge_, left_edge_, right_edge_, shocks_to_right_edge_);
      else { //CR->L
        assert (S->rightI()->edge() == right_edge_);
        _propagate_shock_from_vertex (S, cur_edge_, right_edge_, left_edge_, shocks_to_left_edge_);
      }
    }
    else if (S->leftI()->edge() == left_edge_) {
      if (S->rightI()->edge() == cur_edge_) //LC->R
        _propagate_shock_from_vertex (S, left_edge_, cur_edge_, right_edge_, shocks_to_right_edge_);
      else { //LR->C
        assert (S->rightI()->edge() == right_edge_);
        _propagate_shock_from_vertex (S, left_edge_, right_edge_, cur_edge_, shocks_to_cur_edge_);
      }
    }
    else {
      assert (S->leftI()->edge() == right_edge_);
      if (S->rightI()->edge() == left_edge_) //RC->L
        _propagate_shock_from_vertex (S, right_edge_, cur_edge_, left_edge_, shocks_to_left_edge_);
      else { //RL->C
        assert (S->rightI()->edge() == left_edge_);
        _propagate_shock_from_vertex (S, right_edge_, left_edge_, cur_edge_, shocks_to_cur_edge_);
      }
    }
  }
}

void gdt_shock_prop::_propagate_shock_from_edge (gdt_shock* S, 
                                                 dbmsh3d_gdt_edge* s_cur_edge,
                                                 dbmsh3d_gdt_edge* s_left_edge,
                                                 dbmsh3d_gdt_edge* s_right_edge,
                                                 gdt_shock_section* shocks_to_s_cur_edge,
                                                 gdt_shock_section* shocks_to_s_left_edge,
                                                 gdt_shock_section* shocks_to_s_right_edge)
{
  //: compute the 3 angles for the triangle cur_face
  m2t_compute_tri_angles (s_cur_edge->len(), s_left_edge->len(), s_right_edge->len(), 
                          s_alpha_cl_, s_alpha_cr_, s_alpha_lr_);

  //:1) Intersect S with leftS

  //:2) Intersect S with rightS

  //:3) Propagate S to both edges for each case
  const double curS_from_tau = 0; ////!! S->get_from_tau();

  if (_eqT(curS_from_tau, 0)) { //L) S at A
    //: check angle to see if it's case L1, L2, or L3.
    //compute theta_l via the rightI
    gdt_interval* rightI = (gdt_interval*) S->rightI();
    double theta_l = rightI->theta_from_tau (curS_from_tau);
    double angle = s_alpha_cl_ + theta_l;

    if (_eqM (angle, vnl_math::pi)) {
      //L2) propagate S along left_edge to s_right_edge
      ///if (S->prjE() == s_right_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_R (curS_from_tau, s_right_edge, s_alpha_cr_);

      s_right_edge->add_shock (S);
      shocks_to_s_right_edge->_add_shock (S);
    }
    else if (angle > vnl_math::pi) {
      //L1) propagate S to intersect the interior of s_right_edge
      ///if (S->prjE() == s_right_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_R (curS_from_tau, s_right_edge, s_alpha_cr_);    
      
      s_right_edge->add_shock (S);
      shocks_to_s_right_edge->_add_shock (S);
    }
    else {
      //L3) re-parametrize S to be on left_edge
      ///if (S->prjE() == s_left_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_L (curS_from_tau, s_left_edge, s_alpha_cl_);

      s_left_edge->add_shock (S);
      shocks_to_s_left_edge->_add_shock (S);
    }
  }
  else if (_eqM(curS_from_tau, s_cur_edge->len())) { //R) S at B
    //: check angle to see if it's case R1, R2, or R3.
    //compute theta_r via the leftI
    gdt_interval* leftI = (gdt_interval*) S->leftI();
    double theta_r = leftI->theta_from_tau (curS_from_tau);
    theta_r = vnl_math::pi - theta_r;
    double angle = s_alpha_cr_ + theta_r;

    if (_eqM (angle, vnl_math::pi)) {
      //R2) propagate S along s_right_edge to s_left_edge
      ///if (S->prjE() == s_left_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_L (curS_from_tau, s_left_edge, s_alpha_cl_);

      s_left_edge->add_shock (S);
      shocks_to_s_left_edge->_add_shock (S);
    }
    else if (angle > vnl_math::pi) {
      //R1) propagate S to intersect the interior of s_left_edge
      ///if (S->prjE() == s_left_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_L (curS_from_tau, s_left_edge, s_alpha_cl_);

      s_left_edge->add_shock (S);
      shocks_to_s_left_edge->_add_shock (S);
    }
    else {
      //R3) re-parametrize S to be on s_right_edge
      ///if (S->prjE() == s_right_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_R (curS_from_tau, s_right_edge, s_alpha_cr_);

      s_right_edge->add_shock (S);
      shocks_to_s_right_edge->_add_shock (S);
    }
  }
  else { //M) S inside the s_cur_edge
    //: Propagate to see if it's case M1, M2, or M3.
    //  Handle numerical issues of M2.

    //:1) first do left propagation
    ///if (S->prjE() == s_left_edge)
      ///S->update_to_prjE ();
    ///else
      ///S->advance_to_next_L (curS_from_tau, s_left_edge, s_alpha_cl_);

    //: handle (M2) of left propagation
    if (s_cur_edge->sV() == s_left_edge->sV()) { // I, II
      if (_eqT(S->tau(), s_left_edge->len())) {
        S->set_tau (s_left_edge->len());
      }
    }
    else { // III, IV
      if (_eqT(S->tau(), 0)) {
        S->set_tau (0);
      }
    }

    if (S->tau() > s_left_edge->len()) {
      //:2) if left propagation is invalid, do right propagation
      ///if (S->prjE() == s_right_edge)
        ///S->update_to_prjE ();
      ///else
        ///S->advance_to_next_R (curS_from_tau, s_right_edge, s_alpha_cr_);

      //: handle (M2) of right propagation
      if (s_cur_edge->eV() == s_right_edge->eV()) { // I, III
        if (_eqT(S->tau(), 0)) {
          S->set_tau (0);
        }
      }
      else { // II, IV
        if (_eqT(S->tau(), s_right_edge->len())) {
          S->set_tau (s_right_edge->len());
        }
      }

      assert (S->tau() >= 0);
      s_right_edge->add_shock (S);
      shocks_to_s_right_edge->_add_shock (S);
    }
    else {
      //:1) continue on case 1), use the old s_left_edge intersection
      s_left_edge->add_shock (S);
      shocks_to_s_left_edge->_add_shock (S);
    }
  }
}


void gdt_shock_prop::_propagate_shock_from_vertex (gdt_shock* S, 
                                                   dbmsh3d_gdt_edge* s_from_left_edge,
                                                   dbmsh3d_gdt_edge* s_from_right_edge,
                                                   dbmsh3d_gdt_edge* s_dest_edge,
                                                   gdt_shock_section* shocks_to_s_dest_edge)
{
  //: compute the 3 angles for the triangle cur_face
  m2t_compute_tri_angles (s_from_left_edge->len(), s_from_right_edge->len(), s_dest_edge->len(), 
                          s_alpha_cl_, s_alpha_cr_, s_alpha_lr_);

  //:1) Intersect S with leftS

  //:2) Intersect S with rightS

  //:3) Propagate S to dest_edge for either the init. case or the normal shock case.
  /*if (S->prjE() == s_dest_edge) {
    S->update_to_prjE ();
  }
  else {
    ///S->project_dest (s_dest_edge, s_alpha_cl_, s_alpha_cr_);
  }*/

  s_dest_edge->add_shock (S);
  shocks_to_s_dest_edge->_add_shock (S);
}

#endif
