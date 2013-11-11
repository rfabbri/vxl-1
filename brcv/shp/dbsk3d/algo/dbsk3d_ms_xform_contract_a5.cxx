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

//###############################################################
//  A5 Contract xform on ms_curve MC
//###############################################################

bool MC_valid_for_A5_contract_xform (dbsk3d_ms_curve* MC)
{  
  if (MC->c_type() != C_TYPE_AXIAL)
    return false;

  if (MC->s_MN()->n_type() != N_TYPE_RIB_END || MC->e_MN()->n_type() != V_TYPE_RIB_END)
    return false;

  if (MC->s_MN()->n_incident_Es() != 2 || MC->e_MN()->n_incident_Es() != 2)
    return false;

  //The MC has triple incidence to a single ms_sheet.
  if (MC->halfedge() == NULL)
    return false;
  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();
  dbmsh3d_halfedge* HE = MC->halfedge();
  if (HE->pair() == NULL)
    return false;
  int count = 0;
  do {
    if (HE->face() != MS)
      return false;
    count++;
    HE = HE->pair();
  }
  while (HE != MC->halfedge());
  assert (count == 3);

  return true;
}

//: Find a valid trimming path for the A5-contract-xform.
//  Return false if fail to find a trimming path for the xform.
bool A5_valid_trim_M_U_V (dbsk3d_ms_curve* MC, dbsk3d_fs_vertex*& M,
                          vcl_set<dbmsh3d_face*>& FF_to_trim)
{
  //0) Setup the local config: SCS - Ns:MC:Ne - SCE.  
  dbsk3d_ms_node* Ns = MC->s_MN();
  dbsk3d_ms_node* Ne = MC->e_MN();
  dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) V_find_other_E (Ns, MC);
  dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) V_find_other_E (Ne, MC);
  assert (Cs->c_type() == C_TYPE_RIB);
  assert (Ce->c_type() == C_TYPE_RIB);
  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();
   
  //1) Find the middle fine-scale fs_vertex M and two valid trimming path 
  //MUvec[] and MVvec[] by a zig-zag search and via Dijkstra's shortest path on elms of MS.
  dbsk3d_fs_vertex *U = NULL, *V = NULL;  
  vcl_vector<dbmsh3d_edge*> UM_Evec, VM_Evec;
  //Create a temporary mesh MSM (no need to destruct elements when deleting).
  dbmsh3d_mesh* MSM = new dbmsh3d_mesh (MS->facemap(), false);

  //1-1) First perform a quick search for a valid trimming path.
  //     s_trim indicates the degenerate case if UMvec[] or VMvec[] completely covers MC. 
  char s_trim;
  bool result = A5_find_trim_M_path (MC, Cs, Ce, MSM, false,
                                     M, U, V, UM_Evec, VM_Evec, s_trim);
  //1-2) If the quick search does not return a valid path, perform an extensive search.
  if (result == false)
    result = A5_find_trim_M_path (MC, Cs, Ce, MSM, true,
                                  M, U, V, UM_Evec, VM_Evec, s_trim);

  if (result == false) {
    //1-3) No valid trimming vertex M and paths M-U and M-V found.
    delete MSM;
    return false; 
  }
  else {
    //1-4) Handle UMvec[] and VMvec[] for the special cases.
    vul_printf (vcl_cout, "Valid M %d, U %d, V %d found, ", M->id(), U->id(), V->id());
    if (s_trim != '0') {
      vul_printf (vcl_cout, "s_trim: %c, ",  s_trim);      
      if (s_trim == 'U') {
        //For s_trim == U: clean up the VM_Evec. set V = E
        VM_Evec.clear();
        V = MC->e_MN()->FV();
      }
      else if (s_trim == 'V') {
        //For s_trim == V: clean up the UM_Evec. set U = S.
        UM_Evec.clear();
        U = MC->s_MN()->FV();
      }
      else
        assert (0);
    }
  }

  //1-5) If MC has shared_E, try to remove it from Cs and Ce.
  if (MC->have_shared_Es()) {
    vul_printf (vcl_cout, "MC has shared_E. Skip xform! ", M->id(), U->id(), V->id());
    //Remove shared_E from MC and Cs.
    //Remove shared_E from MC and Ce.
    ///assert (0); possible
    delete MSM;
    return false;
  }

  //1-6) If U == Nss or V == Nee, skip transform.
  dbmsh3d_node* Nss = (dbmsh3d_node*) Cs->other_V (Ns);
  dbmsh3d_node* Nee = (dbmsh3d_node*) Ce->other_V (Ne);
  if (Nss->V() == U) {
    vul_printf (vcl_cout, "Nss %d's V = U %d. Skip xform! ", Nss->id(), U->id());
    delete MSM;
    return false;
  }
  if (Nee->V() == V) {
    vul_printf (vcl_cout, "Nee %d's V = V %d. Skip xform! ", Nee->id(), V->id());
    delete MSM;
    return false;
  }

  //2) Label fine-scale faces to be pruned at two swallow-tail wings.

  //2A-1) Label faces to be pruned in the U side of swallow-tail wing.
  //     The starting (seed) fs_faces are on the wing bordering at Cs.
  //     Considering degenerate cases, need to add all possible seeds bordering Cs.
  vcl_vector<dbsk3d_fs_face*> seedFF_vec;
  vcl_vector<dbmsh3d_edge*> SU_Evec;
  get_rib_trim_Evec (Ns, Cs, U, SU_Evec);
  get_rib_trim_seedFF (SU_Evec, UM_Evec, seedFF_vec);
  
  //The bnd_E_set is the bordering edges where propagation should stop.
  vcl_set<dbmsh3d_edge*> bnd_E_set;
  bnd_E_set.insert (UM_Evec.begin(), UM_Evec.end());

  //Put all MS's incident MC's shared_Es into the bnd_E_set.
  vcl_set<dbmsh3d_edge*> MS_shared_Es;
  MS->get_incident_C_shared_Es (MS_shared_Es);
  bnd_E_set.insert (MS_shared_Es.begin(), MS_shared_Es.end());

  MSM->reset_traverse_f(); //Reset MSM face traversal (may not need this).
  vcl_set<dbmsh3d_face*> FF_to_trim_U;

  //2A-2) Collect all the swallow-tail wing of fs_faces.
  //      Stop propagation when the set of fs_edges in UM_Evec are reaches.
  for (unsigned int i=0; i<seedFF_vec.size(); i++) {
    dbsk3d_fs_face* seedFF = seedFF_vec[i];
    collect_Fs_given_seed_bnd (seedFF, bnd_E_set, MSM, FF_to_trim_U);
  }

  bool U_side_trim_FF_valid = true;
  //2A-3) U side: For the case if M not on E,
  //make sure no face in FF_to_trim_U incident to the immediate FE of E on MC.
  dbmsh3d_edge* eFE = MC->E_vec_last();
  if (M != Ne->FV() && eFE->only_incident_to_Fset_in_Fmap (FF_to_trim_U, MS->facemap())) {
    vul_printf (vcl_cout, "U side no valid trim path: eFE %d only incident to FF_to_trim! ", eFE->id());
    U_side_trim_FF_valid = false;
  }

  //check if any FF_to_trim_U incident to A13 other than MC.
  dbsk3d_fs_edge* otherFE = FF_to_trim_incident_to_otherMC (FF_to_trim_U, MC);
  if (otherFE) { 
    vul_printf (vcl_cout, "U side no valid trim path: FF_to_trim incident to other A13 FE %d! ", otherFE->id());
    U_side_trim_FF_valid = false;
  }

  if (U_side_trim_FF_valid == false) {
    delete MSM;
    return false;
  }
  FF_to_trim.insert (FF_to_trim_U.begin(), FF_to_trim_U.end());

  //2B-1) Label faces to be pruned in the V side of swallow-tail wing.
  //      The starting (seed) fs_faces are on the wing bordering at Ce.
  //      Considering degenerate cases, need to add all possible seeds bordering Cs.  
  seedFF_vec.clear();
  vcl_vector<dbmsh3d_edge*> EV_Evec;
  get_rib_trim_Evec (Ne, Ce, V, EV_Evec);
  get_rib_trim_seedFF (EV_Evec, VM_Evec, seedFF_vec);

  //The bnd_E_set is the bordering edges where propagation is stopped.
  bnd_E_set.clear();
  bnd_E_set.insert (VM_Evec.begin(), VM_Evec.end());

  //Put all MS's incident MC's shared_Es into the bnd_E_set.
  bnd_E_set.insert (MS_shared_Es.begin(), MS_shared_Es.end());

  MSM->reset_traverse_f(); //Reset MSM face traversal.
  vcl_set<dbmsh3d_face*> FF_to_trim_V;

  //2B-2) Collect all the swallow-tail wing of fs_faces.
  //      Stop propagation when the set of fs_edges in MU_Le are reaches.
  for (unsigned int i=0; i<seedFF_vec.size(); i++) {
    dbsk3d_fs_face* seedFF = seedFF_vec[i];
    collect_Fs_given_seed_bnd (seedFF, bnd_E_set, MSM, FF_to_trim_V);
  }
  delete MSM;

  bool V_side_trim_FF_valid = true;
  //2B-3) V side: For the case if M not on S,
  //make sure no face in FF_to_trim_V incident to the immediate FE of S on MC.
  dbmsh3d_edge* sFE = MC->E_vec(0);
  if (M != Ns->FV() && sFE->only_incident_to_Fset_in_Fmap (FF_to_trim_V, MS->facemap())) {
    vul_printf (vcl_cout, "V side no valid trim path: sFE %d only incident to FF_to_trim! ", sFE->id());
    U_side_trim_FF_valid = false;
  }
  
  //check if any FF_to_trim_V incident to A13 other than MC.
  otherFE = FF_to_trim_incident_to_otherMC (FF_to_trim_V, MC);
  if (otherFE) { 
    vul_printf (vcl_cout, "V side no valid trim path: FF_to_trim incident to other A13 FE %d! ", otherFE->id());
    V_side_trim_FF_valid = false;
  }

  if (V_side_trim_FF_valid == false)
    return false;
  FF_to_trim.insert (FF_to_trim_V.begin(), FF_to_trim_V.end());

  //3) Check if any MS's i-curve loop fuly contained in the trimming faces.
  if (MS->n_IC_loops() != 0) {    
    //If i-curve loop exists, go through each icurve-loop.
    for (dbmsh3d_ptr_node* cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();      
      if (is_icurve_pair_HE (headHE))
        continue;

      //Check if all fine-scale edges of this icurve-loop incident to FF_to_trim[]
      vcl_vector<dbmsh3d_edge*> Evec;
      _get_chain_C_Evec (headHE, Evec);

      //If all edges element in Eset are incident to any of FF_to_trim[], return false.
      bool skip_xform = true;
      for (unsigned int i=0; i<Evec.size(); i++) {
        dbmsh3d_edge* E = Evec[i];
        //if E incident to any F in FF_to_trim[]
        if (E->is_Fset_incident (FF_to_trim))
          skip_xform = false;
      }

      if (skip_xform)
        return false;
    }    
  }

  assert (FF_to_trim.size() != 0);
  return true;
}


//: Find a valid trimming path U-M for the A5 transform 
//  avoiding pathes causing non-local changes of ms_hypg topo.
//
bool A5_find_trim_M_path (const dbsk3d_ms_curve* MC, 
                          const dbsk3d_ms_curve* Cs, const dbsk3d_ms_curve* Ce, 
                          dbmsh3d_mesh* MSM, const bool b_extensive_search,
                          dbsk3d_fs_vertex*& M, dbsk3d_fs_vertex*& U, dbsk3d_fs_vertex*& V,
                          vcl_vector<dbmsh3d_edge*>& UM_Evec, vcl_vector<dbmsh3d_edge*>& VM_Evec,
                          char& s_trim)
{
  s_trim = '0';
  vcl_vector<dbmsh3d_vertex*> MC_Vvec;
  MC->get_V_vec (MC_Vvec);
  int pos = int (MC_Vvec.size() / 2);

  //Parameter: M_SEARCH_RANGE = 2.
  int M_range = 2;
  if (b_extensive_search)
    M_range = pos + 1; //search the whole MC curve.

  bool resultU = false;
  bool resultV = false;
  for (int r=0; r<M_range; r++) {
    int prev = -1;
    for (int sign=-1; sign<=1; sign+=2) {  
      int p = pos + r*sign;
      if (p == prev)
        continue; //to skip the end r=0 case.
      prev = p;
      if (p <0 || p > int(MC_Vvec.size())-1)
        continue; //skip the invalid position

      //Candidate mid vertex M on medial curve MC found.
      M = (dbsk3d_fs_vertex*)MC_Vvec[p];

      //Find candidate trim path UM.
      bool resultU = A5_get_trim_path (MC, true, Cs, MSM, M, b_extensive_search, U, UM_Evec);
      //if UM completely covers SE, let E=V=M.
      if (MC->all_Es_in_vec (UM_Evec)) {
        V = MC->e_MN()->FV();
        s_trim = 'U'; //make M=V later.
        return true;
      }

      //Find candidate trim path VM.
      bool resultV = A5_get_trim_path (MC, false, Ce, MSM, M, b_extensive_search, V, VM_Evec);
      //if VM completely covers SE, let S=U=M.
      if (MC->all_Es_in_vec (VM_Evec)) {
        U = MC->s_MN()->FV();
        s_trim = 'V'; //make M=U later.
        return true;
      }

      if (resultU && resultV)
        return true;
    }
  }

  //No valid trimming vertex M and paths M-U and M-V found.
  vul_printf (vcl_cout, "M (pos %d, M_range %d to %d): No valid trim_M_path found!\n", 
              pos, -M_range, M_range);
  return false; 
}

//: Find a valid trimming path by avoiding pathes causing non-local changes of ms_hypg topo. 
bool A5_get_trim_path (const dbsk3d_ms_curve* MC, const bool MC_s_side, const dbsk3d_ms_curve* ribMC, 
                       dbmsh3d_mesh* MSM, const dbsk3d_fs_vertex* M, const bool b_extensive_search,
                       dbsk3d_fs_vertex*& U, vcl_vector<dbmsh3d_edge*>& UM_Evec)
{
  assert (MC->data_type() == C_DATA_TYPE_EDGE); 
  const dbsk3d_ms_node* MN = MC_s_side ? MC->s_MN() : MC->e_MN(); 
  const dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();

  //Parameters: search starting point and M_range.
  const float U_start_pos_ratio = 1.2f;
  const float U_find_max_ratio = 2.0f;
  int   U_range_min = 5; //at least search for this many steps in each dir.
  if (b_extensive_search)
    U_range_min = ribMC->V_vec().size(); //extensively search the whole ribMC.

  //Estimate the trim_vertex U position.
  int pos = int (MC->E_vec().size() * U_start_pos_ratio);  
  vcl_vector<dbmsh3d_vertex*> ribMC_Vvec;
  ribMC->get_V_vec (ribMC_Vvec);
  int sz = (int) ribMC_Vvec.size();

  if (ribMC->eV() == MN)
    pos = sz-1 - pos;
  if (pos < 0)
    pos = 0;
  if (pos >= sz)
    pos = sz-1;
    
  //Only allow the trim path on MS's interior (avoid incident MC's and MN's elements).  
  vcl_set<dbmsh3d_edge*> MS_incident_FE_set;
  vcl_set<dbmsh3d_vertex*> MS_incident_FV_set;
  MS->get_incident_FEs (MS_incident_FE_set);
  MS->get_incident_FVs (MS_incident_FV_set);

  //Search -/+ i candidate FV's near the pos on ribMC.
  int U_range = vcl_max (int(MC->E_vec().size() * U_find_max_ratio), U_range_min);
  U_range = vcl_min (sz-1, U_range);
  for (int r=0; r<U_range; r++) {
    int prev = -1;
    for (int sign=-1; sign<=1; sign+=2) {      
      //Search near the estimated position for a valid trim vertex U on ribMC.  
      int p = pos + r*sign;
      if (p == prev)
        continue; //to skip the end r=0 case.
      prev = p;
      //U can be on any point between [SS to S), including SS but not S.
      //if U is on S, the trimming path UM is not guaranteed to be local.
      if (p < 0 || p > sz-1)
        continue; //skip the invalid position. 
      U = (dbsk3d_fs_vertex*) ribMC_Vvec[p];
      if (U == MN->V())
        continue; //skip if U is on S.
      //Skip if U inicdent to any shared_E of ribMC.
      if (ribMC->shared_E_incident_to_V (U))
        continue;

      //Candidate trimming vertex U determined.
      //Next: find a valid shortest trimming path from M to U.                 
      vcl_set<dbmsh3d_edge*> avoid_Eset;
      avoid_Eset.insert (MS_incident_FE_set.begin(), MS_incident_FE_set.end());
      vcl_set<dbmsh3d_vertex*> avoid_Vset;
      avoid_Vset.insert (MS_incident_FV_set.begin(), MS_incident_FV_set.end());

      //Determine the constraint on the trim path.
      A5_get_trim_path_constraints (MC, MC_s_side, ribMC, M, U, avoid_Eset, avoid_Vset);

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
  vul_printf (vcl_cout, "  M %d, %s (pos %d, U_range %d to %d): No valid trim path found.\n", 
              M->id(), MC_s_side ? "U" : "V", pos, -U_range, U_range);
  #endif
  U = NULL;
  return false; 
}

void A5_get_trim_path_constraints (const dbsk3d_ms_curve* MC, const bool MC_s_side,
                                   const dbsk3d_ms_curve* ribMC,
                                   const dbsk3d_fs_vertex* M, const dbsk3d_fs_vertex* U,
                                   vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                   vcl_set<dbmsh3d_vertex*>& avoid_Vset)
{
  //1) Constraint on mesh edges.
  //Allow the trim path U-M to be possibly on the E-M part of the curve MC.
  //Allow the trim path U-M to be possibly on the S-U (or E-V) part of the rib.
  vcl_set<dbmsh3d_edge*> allow_Eset;
  if (MC_s_side) {
    MC->get_Eset_M_E (M, allow_Eset);
    if (ribMC->sV() == MC->sV())
      ribMC->get_Eset_S_M (U, allow_Eset);
    else {
      assert (ribMC->eV() == MC->sV());
      ribMC->get_Eset_M_E (U, allow_Eset);
    }
  }
  else {
    MC->get_Eset_S_M (M, allow_Eset);  
    if (ribMC->sV() == MC->eV())
      ribMC->get_Eset_S_M (U, allow_Eset);
    else {
      assert (ribMC->eV() == MC->eV());
      ribMC->get_Eset_M_E (U, allow_Eset);
    }
  }

  vcl_set<dbmsh3d_edge*>::iterator eit = allow_Eset.begin();
  for (; eit != allow_Eset.end(); eit++) {
    dbmsh3d_edge* E = (*eit);
    avoid_Eset.erase (E);
  }  

  //2) Constraint on mesh vertices.
  //Allow the trim path U-M to be possibly on the E-M part of the curve MC.
  //Allow the trim path U-M to be possibly on the S-U (or E-V) part of the rib.
  vcl_set<dbmsh3d_vertex*> allow_Vset;
  if (MC_s_side) {
    MC->get_V_set_M_E (M, allow_Vset);
    if (ribMC->sV() == MC->sV())
      ribMC->get_V_set_S_M (U, allow_Vset);
    else {
      assert (ribMC->eV() == MC->sV());
      ribMC->get_V_set_M_E (U, allow_Vset);
    }
  }
  else {
    MC->get_V_set_S_M (M, allow_Vset);
    if (ribMC->sV() == MC->eV())
      ribMC->get_V_set_S_M (U, allow_Vset);
    else {
      assert (ribMC->eV() == MC->eV());
      ribMC->get_V_set_M_E (U, allow_Vset);
    }
  }

  //Not allow the start/end A1A3 vertices on ribMC.
  allow_Vset.erase (ribMC->s_MN()->V());
  allow_Vset.erase (ribMC->e_MN()->V());

  vcl_set<dbmsh3d_vertex*>::iterator vit = allow_Vset.begin();
  for (; vit != allow_Vset.end(); vit++) {
    dbmsh3d_vertex* V = (*vit);
    avoid_Vset.erase (V);
  }
  
  //Not allow ribMC's shared_E and shared_E's incident V's.
  for (dbmsh3d_ptr_node* cur = ribMC->shared_E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    avoid_Eset.insert (E);
    avoid_Vset.insert (E->sV());
    avoid_Vset.insert (E->eV());
  }

  //Allow M (if M is on ribMC's S or E).
  avoid_Vset.erase ((dbsk3d_fs_vertex*) M);
}

//If the A3 curve is a loop, make M as the loop_vertex.
void A5_contract_merge_rib_loop (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, 
                                 dbsk3d_ms_curve* Cs, dbsk3d_fs_vertex* M)
{
  //Create a new loop end node at M.    
  dbsk3d_ms_node* LN = (dbsk3d_ms_node*) ms_hypg->_new_vertex ();
  ms_hypg->_add_vertex (LN);
  LN->set_n_type (N_TYPE_LOOP_END);
  LN->set_V (M);
  LN->compute_radius();

  //Make MC a loop.
  Cs->_disconnect_V_idx (0);
  Cs->_disconnect_V_idx (1);
  Cs->connect_loop_V (LN);

  //Trace rib fs_edges from M.
  Cs->_clear_E_vec();

  dbmsh3d_halfedge* bndHE = M->get_1st_bnd_HE ();
  assert (bndHE);
  dbmsh3d_edge* startL = bndHE->edge();
  dbmsh3d_vertex* FV = M;  
  
  dbmsh3d_edge* FE = startL;
  do {
    assert (FE != NULL);
    Cs->add_E_to_back (FE);
    FE->set_e_type (E_TYPE_RIB);
    FV = FE->other_V (FV);
    FE = FV->other_rib_E_conn (FE);
  }
  while (FV != M);
}

//Merge the 2 A3 shock ribs into a single curve passing through M.
//Return the merged curve.
dbsk3d_ms_curve* A5_contract_merge_ribs (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS,
                                         dbsk3d_ms_node* Nss, dbsk3d_ms_node* Nee, 
                                         dbsk3d_ms_curve* Cs, dbsk3d_ms_curve* Ce)
{
  //Merge Ce to Cs.
  vul_printf (vcl_cout, "merge C%d to %d, ", Ce->id(), Cs->id());

  //Save Ce's sharedEs in vector and determine endV for tracing ribMC.
  dbmsh3d_vertex* endV = Nee->FV();
  vcl_vector<dbmsh3d_edge*> last_shared_Es;

  //Makes the fs_edges in Ce ordered from Nee.
  if (Ce->e_MN() == Nee)
    Ce->_reverse_E_vec();

  for (unsigned int i=0; i<Ce->E_vec().size(); i++) {
    dbmsh3d_edge* E = Ce->E_vec(i);
    if (Ce->is_E_shared (E) == false)
      break;
    last_shared_Es.push_back (E);
    Ce->del_shared_E (E);
    endV = E->other_V (endV);
  }
  assert (Ce->have_shared_Es() == false);

  _del_MC_from_MS (ms_hypg, MS, Ce);

  //Trace Cs rib fs_edges from Nss-Cs-...M...-Ce-Nee.
  _trace_rib_MC_FEs (Cs, Nss, endV);

  //add last_shared_Es (from Ce) to Cs.
  for (int i=int(last_shared_Es.size())-1; i>=0; i--) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) last_shared_Es[i];
    Cs->add_E_to_back (FE);
    Cs->add_shared_E (FE);
  }

  //Fix Cs's starting/ending nodes. Be careful on possible loop case.
  if (Cs->sV() == Nss) {
    //Coarse-scale MC's ending vertex.
    Cs->_disconnect_V_idx (1);
    if (Nss != Nee)
      Cs->connect_V (1, Nee);
    else //Make Cs a loop at Nss.
      Cs->_set_vertex (1, Nee);
  }
  else {
    assert (Cs->eV() == Nss);
    //Coarse-scale MC's starting vertex.
    Cs->_disconnect_V_idx (0);
    if (Nss != Nee)
      Cs->connect_V (0, Nee);
    else //Make Cs a loop at Nss.
      Cs->_set_vertex (0, Nee);
  }

  if (Cs->eV() == Nss) { //Swap Cs's Nss and Nee.
    Cs->_set_vertex (0, Nss);
    Cs->_set_vertex (1, Nee);
  }

  return Cs;
}

dbsk3d_fs_edge* FF_to_trim_incident_to_otherMC (vcl_set<dbmsh3d_face*> FF_to_trim, 
                                                const dbsk3d_ms_curve* MC)
{
  vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim.begin();
  for (; it != FF_to_trim.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    //check ech FE of this FF.
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      unsigned int nF = FE->n_incident_Fs();
      if (nF>2) {
        //if FE does not belong to MC, return error!
        if (MC->contain_E (FE) == false)
          return FE;
      }
      HE = HE->next();
    }
    while (HE != FF->halfedge()); 
  }
  return NULL;
}


