//: dbsk3d_ms_xform_contract_a12a3i.cxx
//: MingChing Chang
//  Oct 31, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>

//#############################################################
//###### A12A3-I Curve Contract xform on ms_curve MC ######
//#############################################################

bool MC_valid_for_A12A3I_contract_xform (dbsk3d_ms_curve* MC)
{
  if (MC->c_type() != C_TYPE_AXIAL && MC->c_type() != C_TYPE_DEGE_AXIAL)
    return false;

  if (MC->have_shared_Es())
    return false; //Can not contract MC with a shared_E (hybrid xforms).

  if (MC->s_MN()->n_type() == N_TYPE_RIB_END && MC->e_MN()->n_type() == N_TYPE_RIB_END)
    return false;

  if (MC->s_MN()->n_type() == N_TYPE_AXIAL_END && MC->e_MN()->n_type() == N_TYPE_AXIAL_END)
    return false;

  //The A1A3 ending vertex has exactly 2 incident curves.
  bool rib_end_A1A3_found = false;
  if (MC->s_MN()->n_type() == N_TYPE_RIB_END && MC->s_MN()->n_incident_Es() == 2) 
    rib_end_A1A3_found = true;
  if (MC->e_MN()->n_type() == N_TYPE_RIB_END && MC->e_MN()->n_incident_Es() == 2)
    rib_end_A1A3_found = true;

  if (rib_end_A1A3_found == false)
    return false;

  //The MC has 1-incidence on the tab and 2-incidence on the underlie sheet.
  dbmsh3d_sheet *tabS, *baseS;
  bool r = C_get_non_dup_S (MC, tabS, baseS);
  if (r == false)
    return false;

  //the tabMS must have other consecutive bnd A13 curves other than MC.
  dbmsh3d_halfedge* curHE = MC->get_HE_of_F (tabS);
  assert (curHE);

  //Look for nextHE and prevHE not pointing to the same edge MC.
  dbmsh3d_halfedge* nextHE =_find_nextE_HE (curHE);
  if (nextHE == NULL)
    return false;
  dbsk3d_ms_curve* nextMC = (dbsk3d_ms_curve*) nextHE->edge();
  assert (MC != nextMC);

  dbmsh3d_halfedge* prevHE = _find_prevE_HE (curHE);
  if (prevHE == NULL)
    return false;
  dbsk3d_ms_curve* prevMC = (dbsk3d_ms_curve*) prevHE->edge();
  assert (MC != prevMC);

  if (prevMC->c_type() == C_TYPE_RIB && nextMC->c_type() == C_TYPE_RIB)
    return false;
      
  return true;
}

//: Check if the A12A3-I Curve-Contract transform is valid or not.
//  Return false if such transform can not be performed.
//
bool A12A3I_valid_trim_M_U (dbsk3d_ms_sheet* tabMS, const dbsk3d_ms_curve* MC, 
                            const dbsk3d_ms_curve* ribMC, const dbsk3d_ms_curve* aMC,
                            const dbsk3d_ms_node* NA14, const dbsk3d_ms_node* NA1A3, const dbmsh3d_face* F_M,
                            dbmsh3d_vertex*& M_final, dbmsh3d_vertex*& U_final, 
                            bool& M_on_aMC, vcl_vector<dbmsh3d_edge*>& E_aMC, 
                            vcl_vector<dbmsh3d_edge*>& UM_Evec_final, 
                            vcl_set<dbmsh3d_face*>& FF_to_trim)
{  
  //1) Find the trimming path UM_vec[] for each candidate M on face F_M.
  E_aMC.clear();
  dbmsh3d_vertex* U = NULL;
  assert (aMC->data_type() == C_DATA_TYPE_EDGE);

  //Check if MC only has shared_E's with ribMC and aMC.
  vcl_set<dbmsh3d_curve*> shared_E_Cset;
  shared_E_Cset.insert ((dbmsh3d_curve*) ribMC);
  shared_E_Cset.insert ((dbmsh3d_curve*) aMC);
  if (MC->shared_E_with_Cset (shared_E_Cset) == false)
    return false; //MS has shared_E other than ribMC or aMC.

  //1-1) Determine fine-scale edges E_MC[] on curve MC and E_aMC[] on curve aMC from face F_M.
  vcl_vector<dbmsh3d_edge*> E_MC;
  MC->get_Evec_incident_F (F_M, NA14, E_MC);
  aMC->get_Evec_incident_F (F_M, NA14, E_aMC);
  
  //1-2) Candidate M is any vertex of F_M which is
  //      - not on E_MC[] and not on E_aMC[].
  //      - not on any internal-curve of tabMS.
  vcl_set<dbmsh3d_vertex*> candidate_Ms;
  F_M->get_bnd_Vs (candidate_Ms);

  for (unsigned int i=0; i<E_MC.size(); i++) {
    candidate_Ms.erase (E_MC[i]->sV());
    candidate_Ms.erase (E_MC[i]->eV());
  }
  for (unsigned int i=0; i<E_aMC.size(); i++) {
    candidate_Ms.erase (E_aMC[i]->sV());
    candidate_Ms.erase (E_aMC[i]->eV());
  }

  //Remove candidate_Ms[] that is on any icurve vertices of tabMS.
  vcl_set<dbmsh3d_vertex*> tabMS_IC_Vset;
  tabMS->get_icurve_Vset (tabMS_IC_Vset);
  vcl_set<dbmsh3d_vertex*>::iterator it = tabMS_IC_Vset.begin();
  for (; it != tabMS_IC_Vset.end(); it++) {
    dbmsh3d_vertex* V = (*it);
    candidate_Ms.erase (V);
  }  

  //1-3) Create a temporary mesh MSM (no need to destruct elements when deleting).
  dbmsh3d_mesh* MSM = new dbmsh3d_mesh (tabMS->facemap(), false);
  bool r;

  //1-4) Find the shortest trimming path for each candidate M. Two cases.
  int trim_path_len = INT_MAX;
  int rib_path_len = 0;

  if (candidate_Ms.empty()) {
    //1-4-1) Enforce M on aMC.
    M_on_aMC = true;

    //M is the farthest vertex of F_M from NA14 on aMC.
    assert (E_aMC[0]->is_V_incident (NA14->V()));
    if (E_aMC.size() == 1)
      M_final = E_aMC[0]->other_V (NA14->V());
    else {
      int l = int(E_aMC.size()) - 1;
      dbmsh3d_vertex* V = Es_sharing_V (E_aMC[l], E_aMC[l-1]);
      M_final = E_aMC[l]->other_V (V);
    }

    //M is not allowed on a coarse-scale node.
    dbsk3d_ms_node* aMN_oN = (dbsk3d_ms_node*) aMC->other_V (NA14);
    if (M_final == aMN_oN->V()) {
      delete MSM;
      return false;
    }

    r = A12A3I_get_trim_path (MC, ribMC, tabMS, MSM, M_final, NA1A3, U, UM_Evec_final);
  }
  else {
    //1-4-2) Find the M of shortest trimming path.
    M_on_aMC = false;    
    vcl_set<dbmsh3d_vertex*>::iterator it = candidate_Ms.begin(); 
    for (; it != candidate_Ms.end(); it++) {
      dbmsh3d_vertex* M = (*it);

      //Find a valid trimming path U-M. Two cases.      
      if (ribMC->is_V_on_C (M)) {
        //1) M on ribMC. U = M. Choose the M that is farthest from A1A3 on ribMC.      
        //Test if M=U is valid (not on the end of this ribMC).
        dbmsh3d_node* otherN = (dbmsh3d_node*) ribMC->other_V (NA1A3);
        if (otherN->V() == M)
          continue; //Skip if M is not valid.

        int nE_rib_EU = ribMC->get_nE_of_V_from_N (M, NA1A3);
        if (nE_rib_EU > rib_path_len) {
          rib_path_len = nE_rib_EU;
          M_final = M;
          U_final = M;
          UM_Evec_final.clear();
        }
      }
      else {
        //2) M not on ribMC. U != M. Choose the M that gives shortest path.
        //Find a valid trimming path.
        vcl_vector<dbmsh3d_edge*> UM_Evec;        
        r = A12A3I_get_trim_path (MC, ribMC, tabMS, MSM, M, NA1A3, U, UM_Evec);
        if (r == false)
          continue; //Skip if no valid trimming path is found.
        assert (UM_Evec.empty() == false);

        //Don not consider this possibility if any M==U is found.
        if (int(UM_Evec.size()) < trim_path_len && rib_path_len == 0) {         
          trim_path_len = int(UM_Evec.size());
          M_final = M;
          U_final = U;
          UM_Evec_final.clear();
          UM_Evec_final.insert (UM_Evec_final.begin(), UM_Evec.begin(), UM_Evec.end());
        }
      }
    }
  }

  //1-5) Check if a valid trim curve UM_Evec[] is found.
  if (trim_path_len == INT_MAX && rib_path_len == 0) {
    delete MSM;
    return false;
  }

  //1-6) If U on F_M, make M to be on U.
  assert (F_M->is_bnd_V (M_final));
  if (F_M->is_bnd_V (U_final))
    M_final = U_final;
  vul_printf (vcl_cout, "M%d U%d, ", M_final->id(), U_final->id());

  //2) Label faces to be pruned.
  MSM->reset_traverse_f ();

  //2-1) The starting (seed) fs_faces are on the bordering at ribMC.
  //     Considering degenerate cases, need to add all possible seeds bordering ribMC.
  vcl_vector<dbsk3d_fs_face*> seedF_vec;
  vcl_vector<dbmsh3d_edge*> BU_Evec;
  get_rib_trim_Evec (NA1A3, ribMC, U_final, BU_Evec);
  A12A3I_get_rib_trim_seedFF (BU_Evec, UM_Evec_final, F_M, seedF_vec);
  
  //2-2) The bnd_E_set is the bordering edges where propagation is stopped.
  vcl_set<dbmsh3d_edge*> bnd_E_set;
  bnd_E_set.insert (UM_Evec_final.begin(), UM_Evec_final.end());

  //2-3) For M not on aMC, add all edges of F_M to avoid_Eset.
  //     This ensure F_M to be not in FF_to_trim[].
  if (M_on_aMC == false) {
    vcl_vector<dbmsh3d_edge*> bnd_Es;
    F_M->get_bnd_Es (bnd_Es);
    bnd_E_set.insert (bnd_Es.begin(), bnd_Es.end());
  }

  //2-4) Put all tabMS's incident MC's shared_Es into the bnd_E_set.
  tabMS->get_incident_C_shared_Es (bnd_E_set);

  //2-5) Collect the FF_to_trim[].
  for (unsigned int i=0; i<seedF_vec.size(); i++) {
    dbsk3d_fs_face* seedF = seedF_vec[i];    
    if (seedF != F_M) //skip if the seedF == F_M.
      collect_Fs_given_seed_bnd (seedF, bnd_E_set, MSM, FF_to_trim);
  }
  delete MSM;
  
  //The case of F_M as only seed: FF_to_trim.size() = 0.
  assert (FF_to_trim.find ((dbmsh3d_face*)F_M) == FF_to_trim.end());

  return true;
}

//: Find a valid trimming path by avoiding pathes causing non-local changes of ms_hypg topo. 
bool A12A3I_get_trim_path (const dbsk3d_ms_curve* MC, const dbsk3d_ms_curve* ribMC, 
                           dbsk3d_ms_sheet* tabMS, dbmsh3d_mesh* MSM, 
                           const dbmsh3d_vertex* M, const dbsk3d_ms_node* NA1A3, 
                           dbmsh3d_vertex*& U, vcl_vector<dbmsh3d_edge*>& UM_Evec)
{
  assert (MC->data_type() == C_DATA_TYPE_EDGE); 
  const bool MC_s_side = (MC->s_MN() == NA1A3) ? true : false;
  UM_Evec.clear();

  //Parameters.
  float U_start_pos_ratio = 1.2f;
  float U_find_max_ratio = 2.0f;
  int   U_range_min = 5; //at least search for this many steps in each dir.

  //Estimate the trim_vertex U position.
  int pos = int (MC->E_vec().size() * U_start_pos_ratio);  
  vcl_vector<dbmsh3d_vertex*> ribMC_Vvec;
  ribMC->get_V_vec (ribMC_Vvec);
  const int sz = (int) ribMC_Vvec.size();

  if (ribMC->eV() == NA1A3)
    pos = sz-1 - pos;
  if (pos < 0)
    pos = 0;
  if (pos >= sz)
    pos = sz-1;
    
  //Only allow the trim path on MS's interior (avoid incident MC's and MN's elements).  
  vcl_set<dbmsh3d_edge*> avoid_Eset;
  vcl_set<dbmsh3d_vertex*> avoid_Vset;
  tabMS->get_incident_FEs (avoid_Eset);
  tabMS->get_incident_FVs (avoid_Vset);

  avoid_Vset.erase ((dbmsh3d_vertex*)M);

  //Search -/+ i candidate FV's near the pos on ribMC.
  int U_range = vcl_max (int(MC->E_vec().size() * U_find_max_ratio), U_range_min);
  U_range = vcl_min (sz-1, U_range);
  for (int r=0; r<U_range; r++) {
    int prev = -1;
    for (int sign=-1; sign<=1; sign+=2) {      
      //Search near the estimated position for a valid trim vertex U on ribMC.  
      int p = pos + r*sign;
      if (p == prev)
        continue;
      prev = p;
      if (p <= 0 || p >= sz-1)
        continue; //skip the invalid position and starting/ending positions. 
      //Find the valid candidate trimming vertex U.
      U = (dbsk3d_fs_vertex*) ribMC_Vvec[p];
      //Skip U if U is not in mesh MSM (if U on a shared_E of ribMC).
      if (MSM->contains_V (U->id()) == false)
        continue; 

      if (M == U)
        return true; //Return true if M is at U.

      //Skip if U inicdent to any shared_E of ribMC.
      if (ribMC->shared_E_incident_to_V (U))
        continue;

      //Next: find a valid shortest trimming path from M to U.
      avoid_Vset.erase (U);
      MSM->reset_face_traversal (); //Reset all faces in MSM to be unvisited.
      UM_Evec.clear();

      bool result = find_shortest_Es_on_M_restrained (MSM, M, U, avoid_Eset, avoid_Vset, UM_Evec);
      if (result) {
        //Check if the input vector of edges and the rib bounding a trimmable region.
        if (FEs_all_on_rib (UM_Evec) == false)
          return true;
      }
    }
  }
  
  //Can't find a valid trim curve via shortest path.
  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "  M %d, U (pos %d, U_range %d to %d): No valid trim path found.\n", 
              M->id(), pos, -U_range, U_range);
  #endif
  U = NULL;
  return false; 
}

void A12A3I_contract_trace_rib (dbsk3d_ms_hypg* ms_hypg, 
                                dbsk3d_ms_sheet* tabMS, dbsk3d_ms_sheet* baseMS, 
                                dbsk3d_ms_curve* MC, dbsk3d_ms_curve* ribMC, dbsk3d_ms_curve* aMC, 
                                const dbmsh3d_vertex* M, const bool M_on_aMC, vcl_vector<dbmsh3d_edge*>& MA_Evec, 
                                dbsk3d_ms_node* NA14, const dbsk3d_ms_node* otherN)
{
  //Disconnect MC from baseMS.
  bool r = baseMS->disconnect_icurve_pair_E (MC);
  assert (r);

  _del_MC_from_MS (ms_hypg, tabMS, MC);

  //Trace ribMC fs_edges in the order of otherN - U - M - NA14.
  vul_printf (vcl_cout, "connect C%d to N%d, ", ribMC->id(), NA14->id());
  _trace_rib_MC_FEs (ribMC, otherN, M);

  //Add the MA_Evec[] into ribMC.
  for (unsigned int i=0; i<MA_Evec.size(); i++) {
    dbmsh3d_edge* E = MA_Evec[i];
    ribMC->add_E_to_back (E);
    if (M_on_aMC)
      check_add_Cs_sharing_E (aMC, ribMC, E); 
  }

  //Fix ribMC's starting/ending nodes.
  if (ribMC->sV() == otherN) {
    ribMC->_disconnect_V_idx (1);
    ribMC->connect_V (1, NA14);
  }
  else {
    assert (ribMC->eV() == otherN);
    ribMC->_disconnect_V_idx (0);
    ribMC->connect_V (0, NA14);
  }

  if (ribMC->eV() == otherN) { //Swap ribMC's s_MN and e_MN.
    ribMC->_set_vertex (0, otherN);
    ribMC->_set_vertex (1, NA14);
  }

  //Re-determine type of this NA14: should be Degenerate A12A3 but can be others.
  NA14->compute_n_type ();
}




