//: This is 3DShock_Scaffold_Graph_SA_Transitions.cxx
//  MingChing Chang
//  040420 MingChing Chang

#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_sg_sa.h>
#include <dbsk3d/algo/dbsk3d_sg_sa_algos.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>


////////////////////////////////////////////////////////////////////
//: Transition operations. 

bool dbsk3d_sg_sa_trans::graph_trans_A5 (float A5_th)
{
  vul_printf (vcl_cout, "graph_trans_A5(): A5_th %f.\n", A5_th);

  //1) Go through all A13 ms_curves and find candidate A5 transition
  //   with cost under threshold.
  vcl_vector<int> A5_A13curve_ids;
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa_->edgemap().begin();
  for (; SC_it != sg_sa_->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;
    assert (MC->data_type() == C_DATA_TYPE_VERTEX);

    if (MC_valid_for_A5_contract_xform (MC) == false)
      continue;

    double len = MC->compute_length_Vs();
    if (len > A5_th)
      continue;

    //add to the removal list
    A5_A13curve_ids.push_back (MC->id());
  }

  //2) Go through the removal list and do the transition.
  for (unsigned int i=0; i<A5_A13curve_ids.size(); i++) {
    int id = A5_A13curve_ids[i];
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) sg_sa_->edgemap (id);

    dbsk3d_ms_node* SCsV = (dbsk3d_ms_node*) MC->s_MN();
    dbsk3d_ms_node* SCeV = (dbsk3d_ms_node*) MC->e_MN();

    //the two other A3 curves
    dbsk3d_ms_curve* A3_starting_curve = NULL;
    for (dbmsh3d_ptr_node* cur = SCsV->E_list(); cur != NULL; cur = cur->next()) {
      dbsk3d_ms_curve* scurve = (dbsk3d_ms_curve*) cur->ptr();
      if (scurve != MC) {
        A3_starting_curve = (dbsk3d_ms_curve*) scurve;
      }
    }
    assert (A3_starting_curve);

    dbsk3d_ms_curve* A3_ending_curve = NULL;
    for (dbmsh3d_ptr_node* cur = SCeV->E_list(); cur != NULL; cur = cur->next()) {
      dbsk3d_ms_curve* scurve = (dbsk3d_ms_curve*) cur->ptr();
      if (scurve != MC) {
        A3_ending_curve = scurve;
      }
    }
    assert (A3_ending_curve);

    //avoid the A1A3-I case.
    if (A3_starting_curve == A3_ending_curve)
      continue;

    //Now do the transition. Create a new A3_scaffold_curve
    dbsk3d_ms_curve* newA3SC = (dbsk3d_ms_curve*) sg_sa_->_new_edge (NULL, NULL);
    newA3SC->set_c_type (C_TYPE_RIB);
    newA3SC->set_data_type (C_DATA_TYPE_VERTEX);

    //Get the new starting and ending vertices
    dbsk3d_ms_node* new_SV = (dbsk3d_ms_node*) A3_starting_curve->other_V (SCsV);
    dbsk3d_ms_node* new_EV = (dbsk3d_ms_node*) A3_ending_curve->other_V (SCeV);

    //Build fs_vertices: insert fs_vertices from A3_starting_curve, A13curve, A3_ending_curve into it
    //  the insertion includes the starting point, but does not include the end point.
    newA3SC->insert_Vs_of_C (A3_starting_curve, new_SV);
    newA3SC->insert_Vs_of_C (MC, SCsV);
    newA3SC->insert_Vs_of_C (A3_ending_curve, SCeV);
    newA3SC->add_V_to_back (new_EV->V());

    assert (newA3SC->V_vec().size() == A3_starting_curve->V_vec().size() +
            MC->V_vec().size() + A3_ending_curve->V_vec().size() - 2);

    //Delete 3 curves: A3_starting_curve MC A3_ending_curve
    //       2 vertices: SCsV, SCeV
    sg_sa_->remove_edge (MC->id());
    sg_sa_->remove_edge (A3_starting_curve->id());
    sg_sa_->remove_edge (A3_ending_curve->id());
    sg_sa_->remove_vertex (SCsV->id());
    sg_sa_->remove_vertex (SCeV->id());

    //  connectivity
    if (new_SV != new_EV) {
      newA3SC->connect_V (0, new_SV);
      newA3SC->connect_V (1, new_EV);

      newA3SC->compute_length_Vs();
      sg_sa_->_add_edge (newA3SC);
    }
    else {
      //Temporarily, we do not handle loops in sg_sa!!
      delete newA3SC;
    }
  }

  return true;
}

bool dbsk3d_sg_sa_trans::graph_trans_A15 (float A15_th)
{
  vul_printf (vcl_cout, "graph_trans_A15(): A15_th %f.\n", A15_th);

  //: Make the transition an iterative approach.
  int count = 0;
  dbsk3d_ms_curve* A13 = _detect_next_A15 (A15_th);
  while (A13 != NULL) {
    count++;
    vul_printf (vcl_cout, "\nA15 iteration: %d, A13 id: %d\n", count, A13->id());
    _run_trans_A15 (A13);
    A15_Transition_List_.erase (A15_Transition_List_.begin());

    A13 = _detect_next_A15 (A15_th);
  }

  //: Remove any zero-length A13 curve if any (bad but needed)
  vcl_vector<int> A13_self_loop_ids;

  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa_->edgemap().begin();
  for (; SC_it != sg_sa_->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;
    if (MC->c_type() != C_TYPE_AXIAL)
      continue;
    
    ///if (MC->is_self_loop())
      ///A13_self_loop_ids.push_back (MC->id());
  }

  /*for (unsigned int i=0; i<A13_self_loop_ids.size(); i++) {
    int id = A13_self_loop_ids[i];
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) sg_sa_->edgemap (id);
    assert (MC->s_MN() == MC->e_MN());
    sg_sa_->remove_edge (MC->id());
    sg_sa_->try_remove_vertex (MC->s_N());
  }*/

  return true;
}

bool lessLength (const dbsk3d_ms_curve* x, const dbsk3d_ms_curve* y)
{
  return x->length() < y->length();
}

/////////////////////////////////////////////////////////
//: Detect the potential A15 Transitions to remove.
//  Store the results in the flag in the A13ScaffoldCurve::iA15Transition_ToBeRemoved_
//  Use A15_TransitionThreshold_ as threshold
//  Retuen the next best A13 Link to remove.
//  If there's no next one, return NULL.
dbsk3d_ms_curve* dbsk3d_sg_sa_trans::_detect_next_A15 (float A15_th)
{
  vul_printf (vcl_cout, "\ndbsk3d_ms_graph_sa::_detect_next_A15()\n");

  A15_Transition_List_.clear ();

  //:1) go through all shock curves and look for A13 axials whose arclength less than the threshold
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa_->edgemap().begin();
  for (; SC_it != sg_sa_->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;
    if (MC->c_type() != C_TYPE_AXIAL)
      continue;

    assert (MC->data_type() == C_DATA_TYPE_VERTEX);
    double length = MC->compute_length_Vs();
    if (length > A15_th)
      continue;

    if (MC->s_MN()->n_type() != N_TYPE_AXIAL_END || MC->e_MN()->n_type() != N_TYPE_AXIAL_END)
      continue;

    A15_Transition_List_.push_back (MC);
  }

  //:2) Sort the A15_Transition_List_ 
  vcl_sort (A15_Transition_List_.begin(), A15_Transition_List_.end(), lessLength);

  ///vul_printf (vcl_cout, "\nA15_Transition_List_ after sorting:\n");
  ///for (unsigned int i=0; i<A15_Transition_List_.size() && i<5; i++) {
  ///  dbsk3d_ms_curve* MC = A15_Transition_List_[i];
  ///  vul_printf (vcl_cout, "%d(%2.2f) ", MC->id(), MC->length());
  ///}

  //: return the next shortest dbsk3d_ms_curve to do transition.
  if (A15_Transition_List_.size() > 0) {
    dbsk3d_ms_curve* MC = A15_Transition_List_[0];
    return MC;
  }
  else
    return NULL;
}

void dbsk3d_sg_sa_trans::_run_trans_A15 (dbsk3d_ms_curve* A13_to_remove)
{
  //Pick the start node to be the fixed one, the end node to be removed.
  dbsk3d_ms_node* MN = (dbsk3d_ms_node*) A13_to_remove->sV();
  dbsk3d_ms_node* EV = (dbsk3d_ms_node*) A13_to_remove->eV();

  //check for the degenerate case that if SScaffoldVertex==EScaffoldVertex
  //  just remove this link
  if (MN==EV) {
    sg_sa_->remove_edge (A13_to_remove->id());
    return;
  }

  //go through all A13 edgemap the EV, 
  //  connect them through the end_FVs to the start_FVs
  //  (except the A13_to_remove itself)
  ///assert (EV->nA3() == 0);

  dbmsh3d_ptr_node* cur = EV->E_list(); 
  while (cur != NULL) {
    dbsk3d_ms_curve* A13_to_merge = (dbsk3d_ms_curve*) cur->ptr();
    if (A13_to_merge == A13_to_remove) {
      cur = cur->next();
      continue;
    }

    //: shouldn't happen but it happens
    if (same_incident_Vs (A13_to_merge, A13_to_remove)) {
      cur = cur->next();
      sg_sa_->remove_edge (A13_to_merge->id());
      continue;
    }
  
    assert (A13_to_merge->sV() != A13_to_merge->eV());
    //check to add ids at the head/tail of the A13_to_merge.
    //1)Add to the tail
    if (EV == A13_to_merge->eV()) {       
      A13_to_merge->remove_last_V (); //Remove the last nodeElm
      //Push_back in reverse order
      for (int j = (int) A13_to_remove->V_vec().size()-1; j>=0; j--) {
        const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) A13_to_remove->V_vec(j);
        assert (sg_sa_->FV_id_counter() > FV->id());

        dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) FV->clone ();
        FV2->set_id (sg_sa_->FV_id_counter());
        sg_sa_->_add_FV (FV2);
        A13_to_merge->add_V_to_back (FV2);
      }
      //Add to the tail
      A13_to_merge->connect_V (1, MN);
      assert (A13_to_merge->sV() != A13_to_merge->eV());
    }
    else { //2)Add to the head
      assert (EV == A13_to_merge->sV());
      A13_to_merge->remove_first_V(); //Remove the first nodeElm
      //Insert in reverse order
      for (int j = (int) A13_to_remove->V_vec().size()-1; j>=0; j--) {
        const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) A13_to_remove->V_vec(j);
        assert (sg_sa_->FV_id_counter() > FV->id());

        dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) FV->clone ();
        FV2->set_id (sg_sa_->FV_id_counter());
        sg_sa_->_add_FV (FV2);
        A13_to_merge->add_V_to_front (FV2); //Add to the head
      }
      A13_to_merge->connect_V (0, MN);
      assert (A13_to_merge->sV() != A13_to_merge->eV());
    }
    cur = cur->next();
  }//end for each A13_to_merge

  sg_sa_->remove_edge (A13_to_remove->id());

  //disconnect the EV
  //  note that the connectivity info of EV here is old. 
  //  just remove the connectivity.
  while (EV->have_incident_Es()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) EV->get_1st_incident_E();
    EV->del_incident_E (MC);
  }

  //delete the EV
  sg_sa_->try_remove_vertex (EV);
}

bool dbsk3d_sg_sa_trans::graph_trans_remove_dummy_nodes ()
{
  bool dummy_note_removed = false;
  //Loop through all nodes.
  vcl_map<int, dbmsh3d_vertex*>::iterator SN_it = sg_sa_->vertexmap().begin();
  for (; SN_it != sg_sa_->vertexmap().end(); SN_it++) {
    dbsk3d_ms_node* SN = (dbsk3d_ms_node*) (*SN_it).second;

    dbsk3d_ms_curve *SC1 = NULL, *SC2 = NULL;
    bool dummy = SN->is_dummy_node (SC1, SC2);
    if (dummy) {
      //merge SC2 to SC1.
      dummy_note_removed = true;
    }
  }

  return dummy_note_removed;
}

