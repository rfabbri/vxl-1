//: dbsk3d_ms_xform_contract_a5.cxx
//: MingChing Chang
//  Oct 4, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_build.h>

//###############################################################
//  Functions Shared by A5 and A12A3-I Curve Contract xforms 
//###############################################################

//: Check if the input vector of fs_edges on a geodesic path bounding a trimmable region or not.
//  Return true if all the fs_edges are on the Rb on MC, thus not bounding a trimmable region.
bool FEs_all_on_rib (const vcl_vector<dbmsh3d_edge*>& Evec)
{
  for (unsigned int i=0; i<Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) Evec[i];
    assert (FE->halfedge());
    if (FE->halfedge()->pair() != NULL)
      return false; //FE is a inside the shock sheet, so there is a trimmable region.
  }
  return true;
}

void get_rib_trim_Evec (const dbsk3d_ms_node* MN, const dbsk3d_ms_curve* MC, 
                        const dbmsh3d_vertex* U, vcl_vector<dbmsh3d_edge*>& Evec)
{
  Evec.clear();

  if (MC->sV() == MN) {
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbmsh3d_edge* E = MC->E_vec(i);
      //Skip the degenerate shared_E
      if (MC->is_E_shared (E) == false)
        Evec.push_back (E);
      if (E->is_V_incident (U))
        break;
    }
  }
  else {
    assert (MC->eV() == MN);
    for (int i=int(MC->E_vec().size()-1); i>=0; i--) {
      dbmsh3d_edge* E = MC->E_vec(i);
      //Skip the degenerate shared_E
      if (MC->is_E_shared (E) == false)
        Evec.push_back (E);
      if (E->is_V_incident (U))
        break;
    }
  }
}

bool get_rib_trim_seedFF (const vcl_vector<dbmsh3d_edge*>& rib_Evec,
                          const vcl_vector<dbmsh3d_edge*>& bnd_Evec,
                          vcl_vector<dbsk3d_fs_face*>& seedFF_vec)
{
  if (bnd_Evec.size() == 0)
    return false; //the degenerate single_trimming case.

  vcl_set<dbmsh3d_edge*> bnd_Eset;
  for (unsigned int i=0; i<bnd_Evec.size(); i++)
    bnd_Eset.insert (bnd_Evec[i]);
  assert (bnd_Eset.size() == bnd_Evec.size());

  //Find the ribE from rib_Evec (along MC) that's not in bnd_Evec.
  for (unsigned int i=0; i<rib_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) rib_Evec[i];
    if (bnd_Eset.find (FE) == bnd_Eset.end()) {
      //Add rib FE's incident valid FF to be seed.
      int n = FE->count_valid_Fs();      
      if (n == 1) { //Skip degenerate shared_E.
        dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->get_1st_valid_F();
        seedFF_vec.push_back (FF);
      }
    }
  }  

  return seedFF_vec.size() != 0;
}

bool A12A3I_get_rib_trim_seedFF (const vcl_vector<dbmsh3d_edge*>& rib_Evec,
                                 const vcl_vector<dbmsh3d_edge*>& bnd_Evec,
                                 const dbmsh3d_face* F_M,
                                 vcl_vector<dbsk3d_fs_face*>& seedFF_vec)
{
  vcl_set<dbmsh3d_edge*> bnd_Eset;
  for (unsigned int i=0; i<bnd_Evec.size(); i++)
    bnd_Eset.insert (bnd_Evec[i]);
  assert (bnd_Eset.size() == bnd_Evec.size());

  //Find the ribE from rib_Evec (along MC) that's not in bnd_Evec.
  for (unsigned int i=0; i<rib_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) rib_Evec[i];
    if (bnd_Eset.find (FE) == bnd_Eset.end()) {
      //Add rib FE's incident valid FF to be seed.
      int n = FE->count_valid_Fs();      
      if (n == 1) { //Skip degenerate shared_E.
        dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->get_1st_valid_F();
        if (FF != (dbmsh3d_face*) F_M) //Skip the F_M as a seed.
          seedFF_vec.push_back (FF);
      }
    }
  }  

  return seedFF_vec.size() != 0;
}

//: Perform trim xform on the fine-scale shock elements.
void perform_trim_xform (dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_sheet* MS, 
                         vcl_set<dbmsh3d_face*>& FF_to_trim)
{
  //1) Use splice xform to prune the collected faces (ordered by max time).
  //Store all valid Ps in FF_to_trim in a map sorted by max_time.  
  vcl_multimap<float, dbsk3d_fs_face*> FF_mmap;
  vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim.begin();
  for (; it != FF_to_trim.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    if (FF->b_valid() == false)
      continue;

    float min_time, max_time;
    ///FF->get_min_max_V_time (min_time, max_time);
    get_FF_min_max_time (FF, min_time, max_time);
    FF_mmap.insert (vcl_pair<float, dbsk3d_fs_face*> (max_time, FF));
    
    //Remove FF from MS.facemap.
    MS->facemap().erase (FF->id());
  }

  //2) Perform splice xforms to trim labelled fs_faces in order.
  perform_rmin_trim_xforms (FF_mmap, FLT_MAX);
  
  //3) After trimming, delete all invalid fs_faces and their incident Ls and Ns.
  fs_mesh->del_invalid_FFs_complete();
  vul_printf (vcl_cout, "S%d has %dF. ", MS->id(), MS->facemap().size());
}

//: Perform trim xform on the fine-scale shock elements.
void perform_trim_xform (dbsk3d_fs_mesh* fs_mesh, vcl_set<dbmsh3d_face*>& FF_to_trim)
{
  //1) Use splice xform to prune the collected faces (ordered by max time).
  //Store all valid Ps in FF_to_trim in a map sorted by max_time.  
  vcl_multimap<float, dbsk3d_fs_face*> FF_mmap;
  vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim.begin();
  for (; it != FF_to_trim.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    if (FF->b_valid() == false)
      continue;

    float min_time, max_time;
    FF->get_min_max_V_time (min_time, max_time);
    FF_mmap.insert (vcl_pair<float, dbsk3d_fs_face*> (max_time, FF));
  }

  //2) Perform splice xforms to trim labelled fs_faces in order.
  perform_rmin_trim_xforms (FF_mmap, FLT_MAX);
  
  //3) After trimming, delete all invalid fs_faces and their incident Ls and Ns.
  fs_mesh->del_invalid_FFs_complete();
}

void _del_MC_from_MS (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, dbsk3d_ms_curve* MC)
{
  //Disconnect MC from both ends.
  MC->_disconnect_V_idx (0);
  MC->_disconnect_V_idx (1);

  //Delete the medial curve MC
  MC->_clear_E_vec();
  MS->disconnect_bnd_E (MC);    
  assert (MC->halfedge() == NULL);
  ms_hypg->edgemap().erase (MC->id());
  ms_hypg->_del_edge (MC);

  //Fix MS->halfedge if it is pointing to a 3-incidence edge.
  fix_headHE_3_incidence (MS->halfedge());
}

void _trace_rib_MC_FEs (dbsk3d_ms_curve* ribMC, const dbsk3d_ms_node* startMN, const dbmsh3d_vertex* endV)
{
  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "fs_edges: ");
  #endif

  //Makes the fs_edges in ribMC ordered from startMN.
  if (ribMC->eV() == startMN)
    ribMC->_reverse_E_vec();

  //Initialization
  dbmsh3d_vertex* FV = startMN->FV();
  dbmsh3d_edge* FE = NULL;  
  unsigned int i;
  for (i=0; i<ribMC->E_vec().size(); i++) {
    FE = ribMC->E_vec(i);
    if (ribMC->is_E_shared (FE) == false)
      break;
    FV = FE->other_V (FV);
  }
  //delete ribMC.E_vec [i to end].
  ribMC->_del_E_vec_i_to_end (i);
  
  //Trace the rib until the endV is reached.
  assert (FE->n_incident_Fs() == 1);
  do {
    assert (FE != NULL);
    ribMC->add_E_to_back (FE);
    #if DBMSH3D_DEBUG > 3
    vul_printf (vcl_cout, "FE %d FV%d, ", FE->id(), FV->id());
    #endif
    FE->set_e_type (E_TYPE_RIB);
    FV = FE->other_V (FV);
    assert (FV != NULL);
    FE = FV->other_rib_E_conn (FE);
  }
  while (FV != endV);

  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "\n");
  #endif
}


//###############################################################
//  Functions shared by the merge transforms 
//###############################################################

//: The node-node merging operation
void MN_MN_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2,
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec)
{
  vul_printf (vcl_cout, "merge N%d to N%d, ", MN1->id(), MN2->id());

  //Make Evec_N1_N2 to be from MN1 to MN2 and Evec_N2_N1 to be from MN2 to MN1.
  vcl_vector<dbmsh3d_edge*> Evec_N1_N2, Evec_N2_N1;
  Evec_N1_N2.insert (Evec_N1_N2.begin(), shortest_Evec.begin(), shortest_Evec.end());
  Evec_N2_N1.insert (Evec_N2_N1.begin(), shortest_Evec.begin(), shortest_Evec.end());
  assert (shortest_Evec[0]->is_V_incident (MN2->V()));
  vcl_reverse (Evec_N1_N2.begin(), Evec_N1_N2.end());
  assert (Evec_N1_N2[0]->is_V_incident(MN1->V()));
  assert (Evec_N2_N1[0]->is_V_incident(MN2->V()));

  //Extend all MN1's incident curves along the shortest_Evec to MN2.
  vcl_set<void*> incident_Es;
  MN1->get_incident_Es (incident_Es);
  assert (incident_Es.size() == 2);

  vcl_set<void*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*it);
    if (MC->s_MN() == MN1) {
      //add Evec_N2_N1 to the front of MC.
      MC->add_E_vec_to_front (Evec_N2_N1);
      for (unsigned int i=0; i<Evec_N2_N1.size(); i++)
        MC->add_shared_E (Evec_N2_N1[i]);
    }
    else { 
      //add Evec_N1_N2 to the end of MC.
      assert (MC->e_MN() == MN1);
      MC->add_E_vec_to_back (Evec_N1_N2);
      for (unsigned int i=0; i<Evec_N1_N2.size(); i++)
        MC->add_shared_E (Evec_N1_N2[i]);
    }

    //Disconnect MC from MN1 and connect to MN2.
    if (MC->s_MN() == MN1) {
      MC->_disconnect_V_idx (0);
      MC->connect_V (0, MN2);
    }
    else {
      assert (MC->e_MN() == MN1);
      MC->_disconnect_V_idx (1);
      MC->connect_V (1, MN2);
    }
  }

  //Delete MN1.
  assert (MN1->have_incident_Es() == false);
  MSH->remove_vertex (MN1);

  MN2->compute_n_type();
}

//: The node-curve merging operation
void MN_MC_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2,                               
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                  const dbmsh3d_vertex* closest_V, dbsk3d_ms_curve*& MC2n)
{
  vul_printf (vcl_cout, "merge N%d with C%d, ", MN1->id(), MC2->id());
  MC2n = NULL;

  //Make Evec_N_C to be from MN1 to MC2 (at closest_V)
  // and Evec_C_N to be from MC2 (at closest_V) to MN1.
  vcl_vector<dbmsh3d_edge*> Evec_N_C, Evec_C_N;
  Evec_N_C.insert (Evec_N_C.begin(), shortest_Evec.begin(), shortest_Evec.end());
  Evec_C_N.insert (Evec_C_N.begin(), shortest_Evec.begin(), shortest_Evec.end());
  if (shortest_Evec[0]->is_V_incident (closest_V))
    vcl_reverse (Evec_N_C.begin(), Evec_N_C.end());
  else
    vcl_reverse (Evec_C_N.begin(), Evec_C_N.end());
  assert (Evec_N_C[0]->is_V_incident(MN1->V()));
  assert (Evec_C_N[0]->is_V_incident(closest_V));

  //Extend all MN1's incident curves along the shortest_Evec to closest_V.
  assert (MN1->n_E_incidence() == 2);
  for (dbmsh3d_ptr_node* cur = MN1->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    if (MC->s_MN() == MN1) {
      //add Evec_C_N to the front of MC.
      MC->add_E_vec_to_front (Evec_C_N);
      for (unsigned int i=0; i<Evec_C_N.size(); i++)
        MC->add_shared_E (Evec_C_N[i]);
    }
    else { 
      //add Evec_N_C to the end of MC.
      assert (MC->e_MN() == MN1);
      MC->add_E_vec_to_back (Evec_N_C);
      for (unsigned int i=0; i<Evec_N_C.size(); i++)
        MC->add_shared_E (Evec_N_C[i]);
    }
  }

  //Point MN1->V to closest_V.
  MN1->set_V (closest_V);

  //Merge MN1 with MC2. 
  MC2n = merge_MN_to_MC (MSH, MN1, MC2, baseMS, closest_V);

  MN1->compute_n_type (); ///set_n_type (N_TYPE_DEGE);
}

//: Merge a node N to a curve C and return the newly created ms_curve.
dbsk3d_ms_curve* merge_MN_to_MC (dbsk3d_ms_hypg* MSH, dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2,
                                 const dbsk3d_ms_sheet* baseMS, const dbmsh3d_vertex* closest_V)
{
  //3 cases:
  if (MC2->self_loop_on_LV()) {
    //1) MC2 is a self-loop and the loop node is a dummy one.
    //1-1) Make MC2 the loop-vertex and delete the existing LV.
    dbsk3d_ms_node* LN = (dbsk3d_ms_node*) MC2->s_MN();
    MC2->disconnect_loop_V (LN);
    MC2->connect_loop_V (MN1);
    MSH->remove_vertex (LN);

    //1-2) Rotate MC2.Evec to be starting with closest_V.
    vcl_vector<dbmsh3d_edge*>::iterator eitV = MC2->E_vec().begin();
    for (; eitV != MC2->E_vec().end(); eitV++) {
      if ((*eitV)->is_V_incident (closest_V))
        break;
    }
    eitV++;
    //Move MC2.Evec[0 to eitV] to a temp, and move to the right place.
    vcl_vector<dbmsh3d_edge*> tmp; 
    tmp.insert (tmp.begin(), MC2->E_vec().begin(), eitV);
    MC2->E_vec().erase (MC2->E_vec().begin(), eitV);
    MC2->E_vec().insert (MC2->E_vec().end(), tmp.begin(), tmp.end());

    return NULL;
  }
  else {
    //2) MC2 is in a regular bnd or internal (pair or loop) curve. 
    dbmsh3d_halfedge* HE2;
    int r = baseMS->find_bnd_IC_HE (MC2, HE2);
    
    //2-1) Determine if MC2 and MC2add is in order: either (MC2, MC2n) or (MC2n, MC).
    bool b_MC2_MC2add_order = true;
    if (HE2->next())
      if (Es_sharing_V (HE2->edge(), HE2->next()->edge()) != HE2->edge()->eV())
        b_MC2_MC2add_order = false;

    //2-2) Break MC2 into two: make MC2 (MC2.s, MN1) and create MC2n (MN1, MC2.e).
    dbsk3d_ms_curve* MC2n = (dbsk3d_ms_curve*) MSH->_new_edge (MN1, MC2->e_MN());
    MSH->_add_edge (MC2n);
    vul_printf (vcl_cout, "create C%d (out of C%d), ", MC2n->id(), MC2->id());
    MC2n->set_c_type (MC2->c_type());
    if (MC2->is_self_loop() == false)
      MC2->e_MN()->del_incident_E (MC2);
    MC2->e_MN()->add_incident_E (MC2n);
    MC2->_set_vertex (1, MN1);
    MN1->add_incident_E (MC2);
    MN1->add_incident_E (MC2n);

    //2-3) Clone the halfedges of MC2n and link them to MC2.
    if (r == 1 || r == 3) {
      //2-3a) MC2 is in a regular bnd or IC-loop curve, including the loop case 
      //      where the loop node is not a dummy one.
      //Loop through MC2's each halfedge in the pair loop.
      HE2 = MC2->halfedge();
      do {
        //Clone HE2 (MC2) to HE2n (MC2n) and link them in HE2's next chain.
        dbmsh3d_halfedge* HE2n;
        if (b_MC2_MC2add_order) {
          HE2n = new dbmsh3d_halfedge (NULL, HE2->next(), MC2n, HE2->face());
          HE2->set_next (HE2n);
        }
        else {
          dbmsh3d_halfedge* HE2_prev = _find_prev_in_next_chain (HE2);
          HE2n = new dbmsh3d_halfedge (NULL, HE2, MC2n, HE2->face());
          HE2_prev->set_next (HE2n);
        }
        MC2n->_connect_HE_end (HE2n);

        //Case of breaking a single-edge loop into more than one edges.
        if (HE2n->next() == NULL)
          HE2n->set_next (HE2);

        HE2 = HE2->pair();
      }
      while (HE2 != MC2->halfedge() && HE2 != NULL);      
    }
    else {
      //2-3b) MC2 is an icurve-pair curve with halfedge HE2.
      assert (r == 2);
      //Loop through MC2's each halfedge in the pair loop.
      vcl_vector<dbmsh3d_halfedge*> baseMS_IC_pair_vec;
      HE2 = MC2->halfedge();
      do {
        //Clone the halfedges of MC2 to MC2n and link each as a separate i-curve.
        dbmsh3d_halfedge* HE2n = new dbmsh3d_halfedge (NULL, NULL, MC2n, HE2->face());

        //Setup the 'next' pointers.
        ///MC2n_IC_pair_vec.push_back (HE2n);
        if (HE2->face() == baseMS) {
          //For halfedge of the baseMS, store in a vector and do this later.
          baseMS_IC_pair_vec.push_back (HE2n);
        }
        else {
          //For halfedge of the other tabMS, add HE2n after HE2.
          if (b_MC2_MC2add_order) {
            HE2n->set_next (HE2->next());
            HE2->set_next (HE2n);
          }
          else {
            dbmsh3d_halfedge* HE2_prev = _find_prev_in_next_chain (HE2);
            HE2n->set_next (HE2);
            HE2_prev->set_next (HE2n);
          }
        }
        //Connect HE2n to MC2n and setup HE2n's 'pair' pointer.
        MC2n->_connect_HE_end (HE2n);

        HE2 = HE2->pair();
      }
      while (HE2 != MC2->halfedge() && HE2 != NULL);

      //Go through baseMS_IC_pair_vec[] fix the 'next' and 'pair' pointers as an i-curve-pair.
      assert (baseMS_IC_pair_vec.size() == 2);
      dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) baseMS_IC_pair_vec[0]->face();
      assert (MS == baseMS);
      baseMS_IC_pair_vec[0]->set_next (baseMS_IC_pair_vec[1]);
      baseMS_IC_pair_vec[1]->set_next (baseMS_IC_pair_vec[0]);  
      MS->add_icurve_chain_HE (baseMS_IC_pair_vec[0]);
    }    

    //2-4) Move MC2's Evec[] from closest_V to end to MC2n.
    vcl_vector<dbmsh3d_edge*>::iterator eitV = MC2->E_vec().begin();
    for (; eitV != MC2->E_vec().end(); eitV++) {
      if ((*eitV)->is_V_incident (closest_V))
        break;
    }
    eitV++;
    
    //2-5) Also move relevant shareE from MC2 to MC2n.
    vcl_set<void*> shared_Es;
    MC2->get_shared_Es (shared_Es);

    //Move MC2.Evec[i to end] to MC2n.
    vcl_vector<dbmsh3d_edge*>::iterator eit = eitV;
    for (; eit != MC2->E_vec().end(); eit++) {
      dbmsh3d_edge* E = (*eit);
      MC2n->add_E_to_back (E);
      //Move MC2's shared E to MC2n.
      if (shared_Es.find (E) != shared_Es.end()) {
        MC2->del_shared_E (E);
        shared_Es.erase (E);
        MC2n->add_shared_E (E);
      }
    }
    MC2->E_vec().erase (eitV, MC2->E_vec().end());

    return MC2n;
  }
}


//: The curve-curve merging operation
void MC_MC_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                  const dbmsh3d_vertex* V1c, const dbmsh3d_vertex* V2c,
                  dbsk3d_ms_curve*& MC1n, dbsk3d_ms_curve*& MC2n,
                  dbsk3d_ms_node*& MNn)
{
  vul_printf (vcl_cout, "merge C%d to C%d, ", MC1->id(), MC2->id());

  //Create a new ms_node for MNn.
  MNn = (dbsk3d_ms_node*) MSH->_new_vertex ();
  MSH->_add_vertex (MNn);
  MNn->set_V (V2c);

  //Merge MC1 to MNn. 
  MC1n = merge_MN_to_MC (MSH, MNn, MC1, baseMS, V1c);

  //Merge MC2 to MNn.
  MC2n = merge_MN_to_MC (MSH, MNn, MC2, baseMS, V2c);

  //Make Evec_C1_C2 to be from MC1 (at V1c) to MC2 (at V2c)
  // and Evec_C2_C1 to be from MC2 (at V2c) to MC1 (at V1c).
  vcl_vector<dbmsh3d_edge*> Evec_C1_C2, Evec_C2_C1;
  Evec_C1_C2.insert (Evec_C1_C2.begin(), shortest_Evec.begin(), shortest_Evec.end());
  Evec_C2_C1.insert (Evec_C2_C1.begin(), shortest_Evec.begin(), shortest_Evec.end());
  if (shortest_Evec[0]->is_V_incident (V2c))
    vcl_reverse (Evec_C1_C2.begin(), Evec_C1_C2.end());
  else
    vcl_reverse (Evec_C2_C1.begin(), Evec_C2_C1.end());
  assert (Evec_C1_C2[0]->is_V_incident(V1c));
  assert (Evec_C2_C1[0]->is_V_incident(V2c));

  //Extend MC1.E_vec to pass V1c along the shortest_Evec to V2c.
  assert (MC1->e_MN() == MNn);
  //add Evec_N_C to the end of MC1.  
  MC1->add_E_vec_to_back (Evec_C1_C2);
  for (unsigned int i=0; i<Evec_C1_C2.size(); i++)
    MC1->add_shared_E (Evec_C1_C2[i]);
  
  //Extend MC1n.E_vec to pass V1c along the shortest_Evec to V2c.
  assert (MC1n->s_MN() == MNn);
  //add Evec_C_N to the front of MC.
  MC1n->add_E_vec_to_front (Evec_C2_C1);
  for (unsigned int i=0; i<Evec_C2_C1.size(); i++)
    MC1n->add_shared_E (Evec_C2_C1[i]);  
}

//: divide the ms_sheet MS into two and return the new ms_sheet MSn.
dbsk3d_ms_sheet* MC_MC_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, 
                                        dbsk3d_ms_node* MNn, 
                                        dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC1n, 
                                        dbsk3d_ms_curve* MC2, dbsk3d_ms_curve* MC2n)
{
  vul_printf (vcl_cout, "divide S%d at N%d, ", MS->id(), MNn->id());

  //Create a new ms_sheet MSn.
  dbsk3d_ms_sheet* MSn = (dbsk3d_ms_sheet*) MSH->_new_sheet ();
  MSH->_add_sheet (MSn);
  vul_printf (vcl_cout, "create S%d, ", MSn->id());

  //Reset MS.halfedge to point to MC1.
  bool r = MS->_set_headHE_to_E (MC1);
  assert (r);

  //Normalize the halfedge loop order of MS.
  if (MS->halfedge()->next()->edge() != MC1n)
    MS->_reverse_bnd_HEs_chain();

  //Now the order is MC1 -> MC1n -> ... -> _ -> _ -> ... -> MC1.  
  //Divide the bnd_curve of MS at MNn.
  //Set MSn.halfedge to MC1n.  
  dbmsh3d_halfedge* HE = MS->halfedge();
  HE = HE->next();
  MSn->set_halfedge (HE);

  HE->set_face (MSn);
      
  //Find the first MC incident to MNn
  HE = HE->next();
  do {
    HE->set_face (MSn);
    if (HE->edge()->is_V_incident (MNn))
      break;
    HE = HE->next();
  }
  while (HE != MS->halfedge());

  assert (HE->edge() == MC2 || HE->edge() == MC2n); 
  assert (HE->next()->edge() == MC2 || HE->next()->edge() == MC2n);

  //MS chain: MS.halfedge -> HE.next -> ...
  MS->halfedge()->set_next (HE->next());

  //MSn chain: MSn.halfedge -> HE ...
  HE->set_next (MSn->halfedge());
  HE->set_face (MSn);

  //Re-run connected component of fine-scale fs_faces on MSn.
  //Seed: first found face in MS incident to MC1n.Evec[[0].
  dbsk3d_fs_face* seedFF = (dbsk3d_fs_face*) MS->get_1st_F_incident_E (MC1n->get_1st_non_shared_E());
  assert (seedFF);
    
  MS->reset_traverse_F (); //Reset all FF's flag of MS.
  prop_add_Fs_from_seed (MSn, seedFF);
  assert (MSn->facemap().size() > 0);

  //Remove all fs_faces of MSn from MS.
  vcl_map<int, dbmsh3d_face*>::iterator it = MSn->facemap().begin();
  while (it != MSn->facemap().end()) {
    dbmsh3d_face* F = (*it).second;
    it++;
    if (MS->contain_F(F->id()))
      MS->facemap().erase (F->id());
  }
  
  //Move i-curves not belonging to MS to MSn.
  split_S_move_icurves (MS, MSn);

  return MSn;
}

//: check if the ms_sheet MS is divided into two.
//  If so a new ms_sheet MSn should be created and returned.
dbsk3d_ms_sheet* MN_MC_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, 
                                        dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC, 
                                        dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MCn)
{
  //Check if labeling-propagation from MC covers MS.
  dbmsh3d_edge* E = MC->get_1st_non_shared_E ();
  dbmsh3d_face* seedF = MS->get_1st_F_incident_E (E);

  MS->reset_traverse_F ();
  vcl_set<dbmsh3d_face*> Fset;
  _prop_label_Fs_e_conn (seedF, Fset);
  
  //Put all MS.faces not in Fset to Fset2.
  vcl_set<dbmsh3d_face*> Fset2;

  vcl_map<int, dbmsh3d_face*>::iterator fit = MS->facemap().begin();
  for (; fit != MS->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (Fset.find (F) == Fset.end())
      Fset2.insert (F);
  }

  if (Fset2.empty())
    return NULL; //No division of MS is required.

  if (MCn == NULL) {
    vul_printf (vcl_cout, " ERROR: MCn NULL but MS has 2 components!!\n");
    return NULL;
  }
  
  ///////////////////////////////////////////////////////////////////
  vul_printf (vcl_cout, "divide S%d at N%d C%d, ", MS->id(), MN->id(), tabMC->id());

  //Create a new ms_sheet MSn.
  dbsk3d_ms_sheet* MSn = (dbsk3d_ms_sheet*) MSH->_new_sheet ();
  MSH->_add_sheet (MSn);
  vul_printf (vcl_cout, "create S%d, ", MSn->id());

  //Put Fset2 to MSn and delete from MS.
  vcl_set<dbmsh3d_face*>::iterator it = Fset2.begin();
  for (; it != Fset2.end(); it++) {
    dbmsh3d_face* F = (*it);
    MSn->add_F (F);
    MS->remove_F (F);
    //if F is a shared_F of MS, delete it and add to MSn.
    if (MS->is_F_shared (F)) {
      assert (MSn->is_F_shared (F) == false);
      MS->del_shared_F (F);
      MSn->add_shared_F (F);
    }
  }

  //Update the halfedges of MS and MSn. Two cases.
  //1) tabMC is an i-curve of MS.
  //2) tabMC is a 3-incidence swallow-tail curve of MS.
  
  //Reset MS.halfedge to pointing to MC.
  bool r = MS->_set_headHE_to_E (MC);
  assert (r);
  
  //Normalize the halfedge loop order of MS.
  assert (MS->halfedge()->edge() == MC);
  if (MS->halfedge()->next()) //if next exists.
    if (MS->halfedge()->next()->edge() != MCn)
      MS->_reverse_bnd_HEs_chain();
  
  if (MS->is_E_in_icurve_pair (tabMC)) {
    //1) tabMC is an i-curve of MS.
    update_bnd_HE_MS_MSn_icurve (MS, MSn, MN, tabMC, MC, MCn);
  }
  else {
    //2) tabMC is a 3-incidence swallow-tail curve of MS.
    update_bnd_HE_MS_MSn_3inc (MS, MSn, MN, tabMC, MC, MCn);
  }
  
  //Move i-curves not belonging to MS to MSn.
  split_S_move_icurves (MS, MSn);

  //Check split S the 3-incidence swallow-tail case.
  split_S_check_3inc (MS, MSn);
  
  return MSn;
}

//: Update the boundary halfedges of MS and MSn for the case
//  that the tabMC is an i-curve of MS.
void update_bnd_HE_MS_MSn_icurve (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                  dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC,
                                  dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MCn)
{  
  //Determine the vector of icurve_otherN_N[].
  vcl_vector<dbmsh3d_edge*> icurve_otherN_N;
  dbmsh3d_vertex* otherN;
  dbmsh3d_vertex* bndN;  
  get_S_icurve_vec_otherN (MS, MN, tabMC, icurve_otherN_N, otherN);

  //Determine the vector IC_pairs_bndN_N[][] in the general case.
  vcl_vector<vcl_vector<dbmsh3d_edge*> > IC_pairs_bndN_N;
  vcl_vector<dbmsh3d_edge*> IC_loop_E_heads;
  bool r = get_S_icurve_vec_bndN (MS, MN, tabMC, IC_pairs_bndN_N, IC_loop_E_heads, bndN);
  assert (r);
  
  //Update the halgedges of MS and MSn.
  if (otherN == bndN)
    update_HE_single_IC_chain (MS, MSn, icurve_otherN_N, otherN);
  else //A complex case if otherN != bndN.
    update_HE_general (MS, MSn, IC_pairs_bndN_N, IC_loop_E_heads, bndN, MN);
}

void update_HE_single_IC_chain (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                vcl_vector<dbmsh3d_edge*>& icurve_N_otherN,
                                dbmsh3d_vertex* otherN)
{
  //Determine the MCo1 and MCo2 incident to otherN.
  dbmsh3d_halfedge* MSn_HE = MS->halfedge()->next();
  dbmsh3d_halfedge* HEon = MS->get_1st_HE_incident_V (MSn_HE, otherN);
  dbmsh3d_halfedge* HEo = HEon->next();
  assert (HEo->edge()->is_V_incident (otherN));

  //reverse icurve_vec[].
  vcl_reverse (icurve_N_otherN.begin(), icurve_N_otherN.end());

  //Prepare the vectors of halfedges MS_HEs[] and MSn_HEs[].
  vcl_vector<dbmsh3d_halfedge*> MS_HEs, MSn_HEs;
  for (unsigned int i=0; i<icurve_N_otherN.size(); i++) {
    dbmsh3d_edge* C = icurve_N_otherN[i];
    //Get the icurve C's 2 HE's incident to MS.
    vcl_set<dbmsh3d_halfedge*> HEset;
    C->get_HEset_of_F (MS, HEset);
    assert (HEset.size() == 2);
    vcl_set<dbmsh3d_halfedge*>::iterator it = HEset.begin();
    dbmsh3d_halfedge* HE1 = (*it);
    it++;
    dbmsh3d_halfedge* HE2 = (*it);
    MS_HEs.push_back (HE1);
    MSn_HEs.push_back (HE2);
    assert (HE1->next() == HE2);
    assert (HE2->next() == HE1);
  }

  //Remove the MS_HEs[] and MSn_HEs[] from the icurve of MS.
  for (unsigned int i=0; i<MS_HEs.size(); i++) {
    if (MS->del_icurve_chain_HE (MS_HEs[i]) == false) {
      bool r = MS->del_icurve_chain_HE (MSn_HEs[i]);
      assert (r);
    }
  }

  //The halfedge loop of MS is:
  // MS.he(MC) -> MS_HEs[0] -> ... -> MS_HEs[n-1] -> HEo -> ... -> MS.he(MC).
  MS->halfedge()->set_next (MS_HEs[0]);
  for (int i=0; i<int(MS_HEs.size())-1; i++)
    MS_HEs[i]->set_next (MS_HEs[i+1]);
  MS_HEs[MS_HEs.size()-1]->set_next (HEo);

  //The halfedge loop of MSn is:
  // HEon -> MSn_HEs[n-1] ... -> MSn_HEs[0] -> MCadd -> ... -> HEon.
  MSn->set_halfedge (MSn_HE);
  HEon->set_next (MSn_HEs[MSn_HEs.size()-1]);
  for (int i=MSn_HEs.size()-1; i>0; i--)
    MSn_HEs[i]->set_next (MSn_HEs[i-1]);
  MSn_HEs[0]->set_next (MSn_HE);

  //Reset the new HE chain to point to MSn.
  _set_HE_chain_face (MSn_HE, MSn);
}

void update_HE_general (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                        vcl_vector<vcl_vector<dbmsh3d_edge*> >& IC_pairs_bndN_N,
                        vcl_vector<dbmsh3d_edge*>& IC_loop_E_heads,
                        dbmsh3d_vertex* bndN, dbmsh3d_vertex* endN)
{
  //Determine the MCo1 and MCo2 incident to otherN.
  dbmsh3d_halfedge* MSn_HE = MS->halfedge()->next();
  dbmsh3d_halfedge* HEon = MS->get_1st_HE_incident_V (MSn_HE, bndN);
  dbmsh3d_halfedge* HEo = HEon->next();
  assert (HEo->edge()->is_V_incident (bndN));

  //Prepare the the vectors of halfedges MS_HEs[] and MSn_HEs[].
  //These are chains of halfedges from bndN to N.
  vcl_vector<dbmsh3d_halfedge*> MS_HEs, MSn_HEs;
  
  //The Ne starts at the bndN.
  dbmsh3d_vertex* Ne = bndN;

  //The starting and ending of the 'breaking curve set' can be either icurve_pairs or icurve_loops.
  if (IC_pairs_bndN_N.size() != IC_loop_E_heads.size() + 1) {
    //Handle the starting icurve-loop.
    assert (IC_pairs_bndN_N.size() == IC_loop_E_heads.size());
    
    //The starting node Ns is the old Ne.
    dbmsh3d_vertex* Ns = Ne;

    //Determine the new ending node Ne from the next chain.
    dbmsh3d_edge* C0 = IC_pairs_bndN_N[0][0];
    if (C0->is_V_incident (endN))
      Ne = endN; //Finish.
    else {
      dbmsh3d_edge* C1 = IC_pairs_bndN_N[0][1];
      Ne = Es_sharing_V (C0, C1);
    }
    Ne = C0->other_V (Ne);

    //Determine the halfedge of lsHE of IC_loop_E_heads[i].
    vcl_set<dbmsh3d_halfedge*> HEset;
    IC_loop_E_heads[0]->get_HEset_of_F (MS, HEset);
    assert (HEset.size() == 1);
    vcl_set<dbmsh3d_halfedge*>::iterator it = HEset.begin();
    dbmsh3d_halfedge* lsHE = (*it);
    
    //Connect each IC_loop_E_heads[i] to MS_HEs[] and MSn_HEs[].    
    connect_breaking_HE_loop (MS, MSn, lsHE, Ns, Ne, MS_HEs, MSn_HEs);

    //remove the entry of IC_loop_E_heads[0].
    IC_loop_E_heads.erase (IC_loop_E_heads.begin());
  }

  //Start with IC_pairs_bndN_N[0] for the HE chains until reaching IC_pairs_bndN_N[n-1].
  assert (IC_pairs_bndN_N.size() == IC_loop_E_heads.size() + 1);

  for (unsigned int i=0; i<IC_pairs_bndN_N.size(); i++) {    
    //Assume the 'breaking curve' always starts at IC_pairs_bndN_N[0].
    for (unsigned int j=0; j<IC_pairs_bndN_N[i].size(); j++) {
      dbmsh3d_edge* C = IC_pairs_bndN_N[i][j];
      Ne = C->other_V (Ne);
      //Get the icurve C's 2 HE's incident to MS.
      vcl_set<dbmsh3d_halfedge*> HEset;
      C->get_HEset_of_F (MS, HEset);
      assert (HEset.size() == 2);
      vcl_set<dbmsh3d_halfedge*>::iterator it = HEset.begin();
      dbmsh3d_halfedge* HE1 = (*it);
      it++;
      dbmsh3d_halfedge* HE2 = (*it);
      assert (HE1->next() == HE2);
      assert (HE2->next() == HE1);

      MS_HEs.push_back (HE1);
      MSn_HEs.push_back (HE2);
      
      //Remove the i-curve-pairs of C from MS.
      if (MS->del_icurve_chain_HE (HE1) == false) {
        bool r = MS->del_icurve_chain_HE (HE2);
        assert (r);
      }
    }

    //At the finishing of each icurve_N_otherN[i], connect each IC_loop_E_heads[i]
    if (i == IC_pairs_bndN_N.size()-1)
      continue; // Skip the last one.

    //The starting node Ns is the old Ne.
    dbmsh3d_vertex* Ns = Ne;

    //Determine the new ending node Ne from the next chain.
    dbmsh3d_edge* C0 = IC_pairs_bndN_N[i+1][0];
    if (C0->is_V_incident (endN))
      Ne = endN; //Finish.
    else {
      dbmsh3d_edge* C1 = IC_pairs_bndN_N[i+1][1];
      Ne = Es_sharing_V (C0, C1);
    }
    Ne = C0->other_V (Ne);

    //Determine the halfedge of lsHE of IC_loop_E_heads[i].
    vcl_set<dbmsh3d_halfedge*> HEset;
    IC_loop_E_heads[i]->get_HEset_of_F (MS, HEset);
    assert (HEset.size() == 1);
    vcl_set<dbmsh3d_halfedge*>::iterator it = HEset.begin();
    dbmsh3d_halfedge* lsHE = (*it);
    
    //Connect each IC_loop_E_heads[i] to MS_HEs[] and MSn_HEs[].    
    connect_breaking_HE_loop (MS, MSn, lsHE, Ns, Ne, MS_HEs, MSn_HEs);
  }

  //Before the xform, the halfedge loop of MS is:
  // MC -> MCn -> ... -> HEon -> HEo -> ... -> MC

  //The new halfedge loop of MS is:
  // MC-> HE[n-1] -> ... -> HE[0] -> HEo -> ... -> MC
  MS->halfedge()->set_next (MS_HEs[MS_HEs.size()-1]);
  for (int i=int(MS_HEs.size())-1; i>0; i--)
    MS_HEs[i]->set_next (MS_HEs[i-1]);
  MS_HEs[0]->set_next (HEo);

  //The new halfedge loop of MSn is:
  // MCn -> ... -> HEon -> HEn[0] -> ... -> HEn[n-1] -> MCn  
  MSn->set_halfedge (MSn_HE);
  HEon->set_next (MSn_HEs[0]);
  for (int i=0; i<int(MSn_HEs.size())-1; i++)
    MSn_HEs[i]->set_next (MSn_HEs[i+1]);
  MSn_HEs[MSn_HEs.size()-1]->set_next (MSn_HE);
  
  //Reset the MSn's HE chain to point to its face MSn.
  _set_HE_chain_face (MSn_HE, MSn);
}

//Connect each IC_loop_E_heads[i].
void connect_breaking_HE_loop (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                               dbmsh3d_halfedge* loop_HE_start, 
                               dbmsh3d_vertex* Ns, dbmsh3d_vertex* Ne,
                               vcl_vector<dbmsh3d_halfedge*>& MS_HEs, 
                               vcl_vector<dbmsh3d_halfedge*>& MSn_HEs)
{
  //Create a vector of halfedges of this loop and setup their face pointer.
  bool loop_head_removed_from_MS = false;
  vcl_vector<dbmsh3d_halfedge*> HEvec;

  dbmsh3d_halfedge* HE = loop_HE_start;
  do {
    //Determine whether HE point to MS or MSn via checking the fine-scale faces.
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    dbmsh3d_edge* E = C->get_1st_non_shared_E ();
    dbmsh3d_halfedge* heE = E->halfedge();
    do {
      dbmsh3d_face* F = heE->face();
      bool b_in_MS = MS->contain_F (F->id());
      bool b_in_MSn = MSn->contain_F (F->id());
      if (b_in_MS) {
        HE->set_face (MS);
        assert (b_in_MSn == false);
      }
      else if (b_in_MSn) {
        HE->set_face (MSn);
        assert (b_in_MS == false);
      }
      heE = heE->pair();
    }
    while (heE != E->halfedge());
  

    //Remove the HE from the icurve-chain head from MS.
    bool r = MS->del_icurve_chain_HE (HE);
    loop_head_removed_from_MS |= r;

    HE = HE->next();
  }
  while (HE != loop_HE_start);

  //Assert that the icurve-loop head is removed from MS.
  assert (loop_head_removed_from_MS);

  //Determine HEs, HEsn, HEe, HEen.
  dbmsh3d_halfedge *HEs = NULL, *HEsn = NULL, *HEe = NULL, *HEen = NULL;
  HE = loop_HE_start;
  do {
    //Find the starting HEs and HEsn.
    if (HE->edge()->is_V_incident (Ns)) {
      if (HE->face() == MS) {
        assert (HEs == NULL);
        HEs = HE;
      }
      else {
        assert (HEsn == NULL);
        assert (HE->face() == MSn);
        HEsn = HE;
      }
    }

    //Find the ending HEe and HEen.
    if (HE->edge()->is_V_incident (Ne)) {
      if (HE->face() == MS) {
        assert (HEe == NULL);
        HEe = HE;
      }
      else {
        assert (HEen == NULL);
        assert (HE->face() == MSn);
        HEen = HE;
      }
    }
    HEvec.push_back (HE);
    HE = HE->next();
  }
  while (HE != loop_HE_start);
  assert (HEs);
  assert (HEe);
  assert (HEsn);
  assert (HEen);

  //Add halfedges from HEs to HEe to MS_HEs.
  dbmsh3d_vertex* eV = Ns;
  dbmsh3d_halfedge* nextHE;
  HE = HEs;
  while (HE != HEe) {
    MS_HEs.push_back (HE);
    eV = HE->edge()->other_V (eV);    
    dbmsh3d_halfedge* nextHE2;
    nextHE = _find_nextHE_in_loop_3inc (HE, eV, HEvec, nextHE2);
    assert (nextHE2 == false);
    HE = nextHE;
  }
  MS_HEs.push_back (HEe);

  //Add halfedges from HEsn to HEen to MSn_HEs.
  eV = Ns;
  HE = HEsn;
  while (HE != HEen) {
    MSn_HEs.push_back (HE);
    eV = HE->edge()->other_V (eV);
    dbmsh3d_halfedge* nextHE2;
    nextHE = _find_nextHE_in_loop_3inc (HE, eV, HEvec, nextHE2);
    assert (nextHE2);
    HE = nextHE;
  }
  MSn_HEs.push_back (HEen);
}

//: Update the boundary halfedges of MS and MSn for the case
//  that tabMC is a 3-incidence swallow-tail curve of MS.
//  Currently only handle a single swallow-tail case.
void update_bnd_HE_MS_MSn_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC,
                                dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MCn)
{
  assert (MS->halfedge()->edge() == MC);
  dbmsh3d_halfedge* MSn_HE = MS->halfedge()->next();

  dbmsh3d_vertex* otherN = tabMC->other_V (MN);
  
  //Get the otherHE incident to otherN other than tabMC.
  dbmsh3d_halfedge* otherHE = MS->get_1st_HE_incident_V (MSn_HE, otherN);
  if (otherHE->edge() == tabMC) {

  }
  otherHE = otherHE->next();

  dbmsh3d_halfedge* MSn_HEs = otherHE->next();
  assert (otherHE->edge() == MSn_HEs->edge());
  assert (MSn_HEs->edge() == MSn_HEs->next()->edge());

  //The halfedge loop of MS is:
  // MS.he(MC) -> MSn_HEs -> ... -> MS.he(MC).
  MS->halfedge()->set_next (MSn_HEs);

  //The halfedge loop of MSn is:
  // otherHE -> MSn.he (MCadd) -> ... -> otherHE.
  MSn->set_halfedge (MSn_HE);
  otherHE->set_next (MSn_HE);

  //Reset the new HE chain to point to MSn.
  _set_HE_chain_face (MSn_HE, MSn);
}

bool merge_test_divide_MS (dbsk3d_ms_sheet* baseMS,
                           const vcl_vector<dbmsh3d_edge*>& shortest_Evec)
{

  vcl_set<dbmsh3d_edge*> avoid_Eset;
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    avoid_Eset.insert (E);
  }

  vcl_set<dbmsh3d_face*> conn_Fset;

  //Reset visit flag of each F.
  baseMS->reset_traverse_F ();

  dbmsh3d_face* seedF = baseMS->get_1st_non_shared_F ();

  vcl_queue<dbmsh3d_face*> frontF_queue;
  frontF_queue.push (seedF);

  while (frontF_queue.size() > 0) {
    dbmsh3d_face* frontF = frontF_queue.front();
    frontF_queue.pop();    
    if (frontF->b_visited())
      continue; //A FF can be on the front for multiple times.

    conn_Fset.insert (frontF);
    frontF->set_visited (true);

    //propagate the frontF (through regular manifold fs_edge)
    dbmsh3d_halfedge* HE = frontF->halfedge();
    do {
      dbmsh3d_edge* E = HE->edge();
      if (E->b_inf()) { //Skip the edge that's marked as 'infinity'
        HE = HE->next();
        continue;
      }

      if (E->halfedge()->pair()) {  
        //Propagate to non-shared fine-scale edge with two incident faces (inside a manifold).
        if (E->e_type() == E_TYPE_MANIFOLD) {
          assert (E->halfedge()->pair()->pair() == E->halfedge());
          if (avoid_Eset.find (E) != avoid_Eset.end())
            continue; //skip
          dbmsh3d_face* otherF = E->m2_other_face (frontF); //other_valid_FF
          assert (otherF != NULL);
          if (!otherF->b_visited())
            frontF_queue.push (otherF);
        }
        else {
          //Only exception is the 2-manifold shared or special edges.
          assert (E->e_type() == E_TYPE_SHARED || E->e_type() == E_TYPE_SPECIAL || 
                  E->halfedge()->pair()->pair() != E->halfedge());
        }
      }      
      
      HE = HE->next();
    }
    while (HE != frontF->halfedge());
  }

  if (conn_Fset.size() != baseMS->facemap().size())
    return true;
  else
    return false;
}

//: check if the ms_sheet MS is divided into two.
//  If so a new ms_sheet MSn should be created and returned.
dbsk3d_ms_sheet* MN_MN_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, 
                                        dbsk3d_ms_node* MN1, dbsk3d_ms_curve* tabMC1,
                                        dbsk3d_ms_node* MN2, dbsk3d_ms_curve* tabMC2)
{
  //Check if labeling-propagation from tabMC1 covers MS.
  dbmsh3d_edge* E = tabMC1->get_1st_non_shared_E ();
  dbmsh3d_face* seedF = MS->get_1st_F_incident_E (E);

  MS->reset_traverse_F ();
  vcl_set<dbmsh3d_face*> Fset;
  _prop_label_Fs_e_conn (seedF, Fset);
  
  //Put all MS.faces not in Fset to Fset2.
  vcl_set<dbmsh3d_face*> Fset2;

  vcl_map<int, dbmsh3d_face*>::iterator fit = MS->facemap().begin();
  for (; fit != MS->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (Fset.find (F) == Fset.end())
      Fset2.insert (F);
  }

  if (Fset2.empty())
    return NULL; //No division of MS is required.

  ///////////////////////////////////////////////////////////////////
  vul_printf (vcl_cout, "divide S%d at N%d and N%d, ", MS->id(), MN1->id(), MN2->id());

  //Create a new ms_sheet MSn.
  dbsk3d_ms_sheet* MSn = (dbsk3d_ms_sheet*) MSH->_new_sheet ();
  MSH->_add_sheet (MSn);
  vul_printf (vcl_cout, "create S%d, ", MSn->id());

  //Put Fset2 to MSn and delete from MS.
  vcl_set<dbmsh3d_face*>::iterator it = Fset2.begin();
  for (; it != Fset2.end(); it++) {
    dbmsh3d_face* F = (*it);
    MSn->add_F (F);
    MS->remove_F (F);
    //if F is a shared_F of MS, delete it and add to MSn.
    if (MS->is_F_shared (F)) {
      assert (MSn->is_F_shared (F) == false);
      MS->del_shared_F (F);
      MSn->add_shared_F (F);
    }
  }

  //Update the halfedges of MS and MSn. Two cases.
  //1) tabMC is an i-curve of MS.
  //2) tabMC is a 3-incidence swallow-tail curve of MS.
  vul_printf (vcl_cout, "\n\nSplitting baseMS in node-node is NOT implemented!\n\n");
  assert (0);
  
  //Reset MS.halfedge to pointing to MC.
  /*bool r = MS->_set_headHE_to_E (MC);
  assert (r);
  
  //Normalize the halfedge loop order of MS.
  assert (MS->halfedge()->edge() == MC);
  if (MS->halfedge()->next()) //if next exists.
    if (MS->halfedge()->next()->edge() != MCn)
      MS->_reverse_bnd_HEs_chain();
  
  if (MS->is_E_in_icurve_pair (tabMC)) {
    //1) tabMC is an i-curve of MS.
    update_bnd_HE_MS_MSn_icurve (MS, MSn, MN, tabMC, MC, MCn);
  }
  else {
    //2) tabMC is a 3-incidence swallow-tail curve of MS.
    update_bnd_HE_MS_MSn_3inc (MS, MSn, MN, tabMC, MC, MCn);
  }
  
  //Move i-curves not belonging to MS to MSn.
  split_S_move_icurves (MS, MSn);

  //Check split S the 3-incidence swallow-tail case.
  split_S_check_3inc (MS, MSn);*/
  return NULL;
}

//: Move i-curves (pairs and loops) not belonging to MS to MSn.
void split_S_move_icurves (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn)
{
  vcl_set<dbmsh3d_halfedge*> icurve_head_to_move;
  vcl_set<dbmsh3d_halfedge*> IC_pair_to_split;

  for (dbmsh3d_ptr_node* cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    //Get the first fine-scale face of this i-curve.
    dbmsh3d_curve* C = (dbmsh3d_curve*) headHE->edge();
    dbmsh3d_edge* E = C->get_1st_non_shared_E ();
    assert (E);

    bool b_MS = MS->fine_scale_E_incident (E);
    bool b_MSn = MSn->fine_scale_E_incident (E);

    if (b_MS && b_MSn) {
      //Special case: Divide this icurve to bnd chain of MS and MSn.
      IC_pair_to_split.insert (headHE);
    }
    else {
      if (b_MS) { 
        //No need to move.
      }
      else if (b_MSn) {
        icurve_head_to_move.insert (headHE);    
      }
      else
        assert (0);
    }
  }

  //Move icurve in the icurve_head_to_move set to MSn.
  vcl_set<dbmsh3d_halfedge*>::iterator it = icurve_head_to_move.begin();
  for (; it != icurve_head_to_move.end(); it++) {
    dbmsh3d_halfedge* headHE = (*it);
    MS->del_icurve_chain_HE (headHE);
    MSn->add_icurve_chain_HE (headHE);
    _set_HE_chain_face (headHE, MSn);
  }
  
  //Special case: Divide this icurve-pairs and dispatch to the bnd_chain of either MS or MSn.
  while (IC_pair_to_split.empty() == false) {
    //Determine a valid chain to dispatch and the other curve-chain with (sV, eV).
    dbmsh3d_vertex *C_sV, *C_eV;
    vcl_vector<dbmsh3d_halfedge*> C_HEvec;
    vcl_vector<dbmsh3d_halfedge*> Co_HEvec;
    get_dispatch_bnd_chain (MS, MSn, IC_pair_to_split, C_HEvec, Co_HEvec, C_sV, C_eV);

    dbmsh3d_halfedge* otherC_HE_next = Co_HEvec[0]->next();
    
    //Update C_HEvec and Co_HEvec to MS
    update_bnd_chain (MS, C_HEvec, Co_HEvec, otherC_HE_next);

    //Update C_HEvec and Co_HEvec to MSn
    update_bnd_chain (MSn, C_HEvec, Co_HEvec, otherC_HE_next);
  }
}

//: Check split S 3-inc.
void split_S_check_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn)
{
  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    if (is_HE_3_incidence(HE))
      check_split_3inc (MS, MSn, HE);
    HE = HE->next();
  }
  while (HE != MS->halfedge() && HE != NULL);

  for (dbmsh3d_ptr_node* cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    HE = headHE;
    do {
      if (is_HE_3_incidence(HE))
        check_split_3inc (MS, MSn, HE);
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
  }
}

void check_split_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, dbmsh3d_halfedge* HE1)
{
  //Get the HE2 and HE3 incident to HE1.E, considering the dege. case.
  dbmsh3d_halfedge *HE2, *HE3;
  
  HE2 = HE1->next();
  HE3 = HE2->next();
  assert (HE1->edge() == HE2->edge());
  assert (HE2->edge() == HE3->edge());

  dbmsh3d_curve* C = (dbmsh3d_curve*) HE1->edge();
  dbmsh3d_edge* E = C->get_1st_non_shared_E ();
  assert (E);

  //Check if the three incident faces of E are all in MS or not.
  vcl_vector<dbmsh3d_face*> incident_Fs;
  E->get_incident_Fs (incident_Fs);

  /*if (incident_Fs.size() != 3) {
    //In the dege case, there should be only 3 HE1's incident to MS.
    //Remove non-MS halfedges.
    vcl_vector<dbmsh3d_face*>::iterator it = incident_Fs.begin();
    while (it != incident_Fs.end()) {
      dbmsh3d_face* F = (*it);
      if (F != MS) { //delete it.
        if (it == incident_Fs.begin()) {
          incident_Fs.erase (it);
          it = incident_Fs.begin();
        }
        else {
          vcl_vector<dbmsh3d_face*>::iterator tmp = it;
          tmp--;
          incident_Fs.erase (it);
          tmp++;
          it = tmp;
        }
      }
    }
  }
  assert (incident_Fs.size() == 3);*/

  
  //In the dege case, there should be only 3 HE's incident to MS.
  // (or in the fine-scale, 3 HE's incident to E).
  //Remove non-MS halfedges.
  vcl_vector<bool> FMS;
  for (unsigned int i=0; i<incident_Fs.size(); i++) {
    dbmsh3d_face* F = incident_Fs[i];
    bool b = MS->contain_F (F->id());
    //Skip of F is not contained in MS and MSn.
    if (b || MSn->contain_F (F->id()))
      FMS.push_back (b);
  }
  assert (FMS.size() == 3);

  if (FMS[0] && FMS[1] && FMS[2])
    return; //no need to splice this 3-incidence of HE.

  if (!FMS[0] && !FMS[1] && !FMS[2]) {
    //make them all point to MSn.
    assert (0);
    return; 
  }

  //Find the only entry that FMS[i] is different
  if ((!FMS[0] && FMS[1] && FMS[2]) ||
      (FMS[0] && !FMS[1] && FMS[2]) ||
      (FMS[0] && FMS[1] && !FMS[2])) {
    //Keep HE in MSn.he chain and move HE2 and HE3 to MS.i-curve-pair.
    HE1->set_next (HE3->next());
    HE1->set_face (MSn);

    HE3->set_next (HE2);
    MS->add_icurve_chain_HE (HE2);
  }
  else {
    assert ((FMS[0] && !FMS[1] && !FMS[2]) ||
            (!FMS[0] && FMS[1] && !FMS[2]) ||
            (!FMS[0] && !FMS[1] && FMS[2]));
    //Keep HE1 in MS.he chain and move HE2 and HE3 to MSn.i-curve-pair.
    HE1->set_next (HE3->next());

    HE3->set_next (HE2);
    HE2->set_face (MSn);
    HE3->set_face (MSn);
    MSn->add_icurve_chain_HE (HE2);
  }
}

//: Given a set of icurve-pairs to split, determine the chunk of chains from the topology of MS.
void get_dispatch_bnd_chain (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn,
                             vcl_set<dbmsh3d_halfedge*>& IC_pair_to_split, 
                             vcl_vector<dbmsh3d_halfedge*>& C_HEvec, 
                             vcl_vector<dbmsh3d_halfedge*>& Co_HEvec, 
                             dbmsh3d_vertex*& C_sV, dbmsh3d_vertex*& C_eV)
{
  //For now only detect chunk of a single HE.
  //Chunk of multiple HE's should be able to handle in multiple iterations!
  vcl_set<dbmsh3d_halfedge*>::iterator it = IC_pair_to_split.begin();
  dbmsh3d_halfedge* HE1 = (*it);
  assert (is_icurve_pair_HE (HE1));
  C_HEvec.push_back (HE1);
  //Determine C_sV and C_eV from the ending vertices of HE1.
  C_sV = HE1->edge()->sV();
  C_eV = HE1->edge()->eV();
    
  //Remove the i-curve-pair to split from either MS or MSn.
  bool r = MS->del_icurve_chain_HE (C_HEvec[0]);
  if (r == false)
    r = MSn->del_icurve_chain_HE (C_HEvec[0]);
  assert (r);

  //Search on the bnd_chain and icurve-loop chain of MS and MSn for Co_HEvec. 
  r = MS->find_bnd_chain (C_sV, C_eV, Co_HEvec);  
  if (r == false)
    r = MSn->find_bnd_chain (C_sV, C_eV, Co_HEvec);
  assert (r);

  //erase C_HEvec[] from IC_pair_to_split.
  for (unsigned int i=0; i<C_HEvec.size(); i++)
    IC_pair_to_split.erase (C_HEvec[i]);
}

void update_bnd_chain (dbsk3d_ms_sheet* MS, 
                       vcl_vector<dbmsh3d_halfedge*>& C_HEvec, 
                       vcl_vector<dbmsh3d_halfedge*>& Co_HEvec,
                       dbmsh3d_halfedge* otherC_HE_next)
{
  //Test if Co_HEvec belongs to MS.
  bool C2_HEvec_MS = false;

  //For now only handle single edge.
  assert (C_HEvec.size() == 1);
  assert (Co_HEvec.size() == 1);

  //Make C_HEvec and Co_HEvec same order.

  dbmsh3d_halfedge* prevHE;
  for (unsigned int i=0; i<Co_HEvec.size(); i++) {
    dbmsh3d_halfedge* HE = Co_HEvec[i];
    //For now only check the bnd chain (skipping the icurve-loop)!!
    prevHE = _find_prevHE_in_chain (MS->halfedge(), HE);
    if (prevHE)
      C2_HEvec_MS = true;
  }
  
  //Dispatch the icurve-pair C_HEvec[i] to case I and the other, C_HEvec[i]->next() to case II.
  if (C2_HEvec_MS) {
    //I) If C2_HE_vec belongs to MS, remove Co_HEvec from existing HE chain and replace with C_HEvec.
    prevHE->set_next (C_HEvec[0]);
    C_HEvec[0]->set_face (MS);
    assert (otherC_HE_next->face() == MS);
    C_HEvec[0]->set_next (otherC_HE_next);
  }
  else {  
    //II) Otherwise, make a new icurve-loop of C_HE_vec[i]->next() and Co_HEvec[i].
    dbmsh3d_halfedge* C_HEvec2 = C_HEvec[0]->next();
    C_HEvec2->set_face (MS);
    Co_HEvec[0]->set_face (MS);
    C_HEvec2->set_next (Co_HEvec[0]);
    Co_HEvec[0]->set_next (C_HEvec2);
    MS->add_icurve_chain_HE (C_HEvec2);
  }
}

