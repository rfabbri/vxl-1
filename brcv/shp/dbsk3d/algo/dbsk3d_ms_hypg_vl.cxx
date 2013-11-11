//: This is dbsk3d/algo/dbsk3d_ms_hypg_trans.cxx
//  MingChing Chang 061117

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>

#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_algos.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>

//##############################################################################

//: Add virtual curves from the detected MS transition queue.
void dbsk3d_ms_hypg_trans::add_trans_virtual_curves ()
{
  vul_printf (vcl_cout, "add_trans_virtual_curves(). Q size %u.\n", ms_xform_Q_.size());
  int n_tab_s_success = 0;
  int n_a5_c_success = 0;
  int n_a12a3i_c_success = 0;
  int n_a15_c_c_success = 0;
  int n_a15_s_c_success = 0;
  int n_a14_s_c_success = 0;
  int n_vl = 0;

  while (ms_xform_Q_.size() != 0) {
    int r;
    float cost;
    MS_XFORM_TYPE type;
    dbsk3d_ms_sheet* MS;
    dbsk3d_ms_curve *MC1, *MC2;
    dbsk3d_ms_node *MN1, *MN2;

    _pop_xform_from_Q (cost, type, MS, MC1, MC2, MN1, MN2);
    
    switch (type) {
    case MSXT_TAB_SPLICE: //A1A3-I or A12A3-II sheet-splice transform.  
      r = add_vl_sheet_splice (MS);
      n_tab_s_success++;
      n_vl += r;
    break;
    case MSXT_A5_CURVE_CONTRACT: //A5 swallow-tail curve-contract transform.   
      r = add_vl_A5_curve_contract (MC1);
      n_a5_c_success++;
      n_vl += r;
    break;
    case MSXT_A12A3I_CURVE_CONTRACT: //A12A3-I curve-contract transform.     
      r = add_vl_A12A3I_curve_contract (MC1);
      n_a12a3i_c_success++;
      n_vl += r;
    break;
    case MSXT_A15_CURVE_CONTRACT: //A15 curve-contract transform.  
      r = add_vl_A15_curve_contract (MC1);
      n_a15_c_c_success++;
      n_vl += r;
    break;
    case MSXT_A15_SHEET_CONTRACT: //A15 sheet-contract transform.  
      r = add_vl_A15_sheet_contract (MS);
      n_a15_s_c_success++;
      n_vl += r;
    break;
    case MSXT_A14_SHEET_CONTRACT: //A14 sheet-contract transform.  
      r = add_vl_A14_sheet_contract (MS);
      n_a14_s_c_success++;
      n_vl += r;
    break;
    case MSXT_A1A3II_N_N_MERGE: //A1A3-II node-node merge transform. 
    break;
    case MSXT_A12A3I_N_C_MERGE: //A12A3-I node-curve merge transform. 
    break;    
    case MSXT_A14_C_C_MERGE: //A14 curve-curve merge transform. 
    break;
    case MSXT_A1A3II_C_C_MERGE: //A1A3-II curve-curve merge transform. 
    break;
    case MSXT_A1A5_N_C_MERGE: //A1A5 node-curve merge transform. 
    break; 
    default:
      assert (0);
    break;
    }

    //Debug: check if all elements in Q are valid.
    _check_integrity_Q ();

    //Brute-force debug:
    if (ms_hypg_->check_integrity() == false)
      vul_printf (vcl_cout, "ms_hypg integrity error! ");
  }

  vul_printf (vcl_cout, "    %3d A12A3-II Sheet-Splice xforms.\n", n_tab_s_success);  
  vul_printf (vcl_cout, "    %3d A5 Curve-Contract xforms.\n", n_a5_c_success);  
  vul_printf (vcl_cout, "    %3d A12A3-I Curve-Contract xforms.\n", n_a12a3i_c_success);  
  vul_printf (vcl_cout, "    %3d A15 Curve-Contract xforms.\n", n_a15_c_c_success);  
  vul_printf (vcl_cout, "    %3d A15 Sheet-Contract xforms.\n", n_a15_s_c_success);  
  vul_printf (vcl_cout, "    %3d A14 Sheet-Contract xforms.\n", n_a14_s_c_success);
  vul_printf (vcl_cout, "\n    total virtual links added: %d.\n", n_vl);
}

//##############################################################################

int dbsk3d_ms_hypg_trans::add_vl_sheet_splice (dbsk3d_ms_sheet* MS)
{
  vcl_vector<dbmsh3d_edge*> MS_bnd_Cs;
  MS->get_bnd_Es (MS_bnd_Cs);
  dbmsh3d_vertex* loop2_prevN = NULL;
  int n_vl = 0;

  for (int i=0; i<int(MS_bnd_Cs.size()); i++) {
    // For each consecutive curve MC1 and MC2 on MS's bnd-chain (MC1 != MC2), 
    // find the node MN in between and deter determine the two curves MC3 and MC4 to merge (splice).
    dbsk3d_ms_curve* MC1 = (dbsk3d_ms_curve*) MS_bnd_Cs[i];
    int i2 = (i+1) % MS_bnd_Cs.size();
    assert (i != i2);
    dbsk3d_ms_curve* MC2 = (dbsk3d_ms_curve*) MS_bnd_Cs[i2];
    if (MC1 == MC2)
      continue; //skip if MC1==MC2.

    //Use Es_sharing_V_check() if MS has only two curves in its bnd-chain loop.
    bool loop2;
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) Es_sharing_V_check (MC1, MC2, loop2);    
    if (loop2) { //Handle the case of only 2 MS_bnd_Cs.
      if (loop2_prevN == NULL)
        loop2_prevN = MN;
      else
        MN = (dbsk3d_ms_node*) MC1->other_V (loop2_prevN);
    }

    //Detect the other two ms_curves of the A14 MN to merge.
    //If MC1 and MC2 is found, find MC3 and MC4 at N. 
    //Skip if N's number of incident edges is not 4. 
    //Skip if MC3=MC4. 
    //Skip if MC3 and MC4 not sharing same incident faces. 
    //(The checking here might be redundant.)
    dbsk3d_ms_curve* MC3 = NULL;
    dbsk3d_ms_curve* MC4 = NULL;
    //Skip the degenerate A1n MN or a loop MC
    if (MN->A14_get_other_2_MCs_nv (MC1, MC2, MC3, MC4) == false)
      continue; //No need to merge curves for a junction.      

    //Add virtual link of (MN3 - MC3 - MN - MC4 - MN4).
    dbsk3d_ms_node* MN3 = (dbsk3d_ms_node*) MC3->other_V (MN);
    dbsk3d_ms_node* MN4 = (dbsk3d_ms_node*) MC4->other_V (MN);
    
    if (MN3 == MN4)
      continue; //Skip if MN3 == MN4.
    
    ms_hypg_->add_virtual_curve (MN3, MN4, MC3, MC4);
    n_vl++;
  }

  return n_vl;
}

int dbsk3d_ms_hypg_trans::add_vl_A5_curve_contract (dbsk3d_ms_curve* MC)
{
  int n_vl = 0;
  dbsk3d_ms_node* Ns = MC->s_MN();
  dbsk3d_ms_node* Ne = MC->e_MN();
  assert (Ns->n_incident_Cs_nv() == 2);
  assert (Ne->n_incident_Cs_nv() == 2);
  dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) N_find_other_C_nv (Ns, MC);
  dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) N_find_other_C_nv (Ne, MC);
  dbsk3d_ms_node* Nss = (dbsk3d_ms_node*) Cs->other_V (Ns);
  dbsk3d_ms_node* Nee = (dbsk3d_ms_node*) Ce->other_V (Ne);

  //Add virtual link of (Nss - Cs - Ns - MC - Ne - Ce - Nee)
  vcl_vector<dbmsh3d_curve*> sup_curves;
  sup_curves.push_back (Cs);
  sup_curves.push_back (MC);
  sup_curves.push_back (Ce);
  ms_hypg_->add_virtual_curve (Nss, Nee, sup_curves);
  n_vl++;

  return n_vl;
}

int dbsk3d_ms_hypg_trans::add_vl_A12A3I_curve_contract (dbsk3d_ms_curve* MC)
{
  int n_vl = 0;

  //1-1) Setup local configuration for transform.
  //     Get the A14 vertex NA14, the A1A3 vertex NA1A3, and the other A1A3 vertex otherN.
  //     Skip xform if no valid configuration is found. 
  dbsk3d_ms_node *NA14, *NA1A3, *otherN;
  if (MC->s_MN()->n_type() == N_TYPE_RIB_END) {
    NA1A3 = MC->s_MN();
    NA14 = MC->e_MN();
  }
  else {
    NA14 = MC->s_MN();
    NA1A3 = MC->e_MN();
    assert (NA1A3->n_type() == N_TYPE_RIB_END);
  }
  assert (NA14->n_type() == N_TYPE_AXIAL_END ||
          NA14->n_type() == N_TYPE_DEGE_RIB_END ||
          NA14->n_type() == N_TYPE_DEGE_AXIAL_END);

  //1-2) Detect the tabS, baseS, ribC.
  dbmsh3d_sheet *tabS, *baseS;
  bool r = C_get_non_dup_S (MC, tabS, baseS);
  assert (r);

  dbsk3d_ms_sheet *tabMS = (dbsk3d_ms_sheet*) tabS;
  dbsk3d_ms_sheet *baseMS = (dbsk3d_ms_sheet*) baseS;

  dbsk3d_ms_curve* ribMC = (dbsk3d_ms_curve*) tabMS->find_next_bnd_C (NA1A3, MC);
  otherN = (dbsk3d_ms_node*) ribMC->other_V (NA1A3);  

  assert (otherN != NA14);

  //Add virtual link of (NA14 - MC - NA1A3 - ribMC - otherN)
  ms_hypg_->add_virtual_curve (NA14, otherN, MC, ribMC);
  n_vl++;

  return n_vl;
}

int dbsk3d_ms_hypg_trans::add_vl_A15_curve_contract (dbsk3d_ms_curve* MC)
{
  int n_vl = 0;

  //1) Determine if this transform is valid.
  assert (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL);
 
  //1-2) MC with shared_E (hybrid xforms) not handled yet.
  //     Should perform transform if all shared_Es are in the C's to merge.
  assert (MC->have_shared_Es() == false);

  assert (MC->s_MN()->n_type() != N_TYPE_RIB_END && MC->e_MN()->n_type() != V_TYPE_RIB_END);
  assert (MC->s_MN()->has_rib_C() == false);
  assert (MC->e_MN()->has_rib_C() == false);

  //1-3) Go through each incident MS of MC and check Cs and Ce is of type axial.
  //     Also check Cs and Ce is not on MS's icurve-pair and decide
  //     possible valid merges at either Ns or Ne.
  dbmsh3d_vertex* Ns = MC->s_MN();
  dbmsh3d_vertex* Ne = MC->e_MN();
  vcl_set<dbsk3d_ms_sheet*> MS_merge_set;

  dbmsh3d_halfedge* HE = MC->halfedge();
  assert (is_HE_pair_3p_inc (HE)); //the pair loop contains >=3 halfedges with the same edge.
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
    int n = MS->n_bnd_Es();
    assert (n >= 3);
    
    //1-3-1) Determine the other curve Cs and Ce to merge.
    dbmsh3d_curve* otherC;
    dbmsh3d_face* MF;
    bool r = MS->get_otherC_via_F_nv (MC, MC->s_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) otherC;

    r = MS->get_otherC_via_F_nv (MC, MC->e_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) otherC;

    assert (Cs->c_type() == C_TYPE_AXIAL || Cs->c_type() == C_TYPE_DEGE_AXIAL);
    assert (Ce->c_type() == C_TYPE_AXIAL || Ce->c_type() == C_TYPE_DEGE_AXIAL);
    assert (MS->is_E_in_icurve_pair (MC) == false);

    MS_merge_set.insert (MS);

    HE = HE->pair();
  }
  while (HE != MC->halfedge() && HE != NULL);
  assert (MS_merge_set.size() == MC->n_incident_Fs());

  //1-3-4) Check if xfrom is valid, and if so, determine MN_del and MN_final.
  dbsk3d_ms_node* MN_del = NULL;
  if (Ns) {
    if (Ne) {
      //Perform xform: MN_del = Ns. 
      //Can pick the one with lower degree of symmetry or larger radius.
      MN_del = (dbsk3d_ms_node*) Ns;
    }
    else {
      //Perform xform: MN_del = Ns.
      MN_del = (dbsk3d_ms_node*) Ns;
    }
  }
  else {
    if (Ne) {
      //Perform xform: MN_del = Ne.
      MN_del = (dbsk3d_ms_node*) Ne;
    }
    else {
      assert (0);
      return 0; //no valid xform for merging curves in either ends.
    }
  }
  dbsk3d_ms_node* MN_final = (dbsk3d_ms_node*) MC->other_V (MN_del);
  
  //MC_merge_set[] is the set of curves to merge with MC.
  vcl_set<dbsk3d_ms_curve*> MC_merge_set;
  vcl_set<dbsk3d_ms_sheet*>::iterator sit = MS_merge_set.begin();
  for (; sit != MS_merge_set.end(); sit++) {
    dbsk3d_ms_sheet* MS = (*sit);

    dbmsh3d_curve* otherC;
    dbmsh3d_face* MF;
    bool r = MS->get_otherC_via_F_nv (MC, MC->s_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) otherC;

    r = MS->get_otherC_via_F_nv (MC, MC->e_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) otherC;

    assert (Cs->c_type() == C_TYPE_AXIAL || Cs->c_type() == C_TYPE_DEGE_AXIAL);
    assert (Ce->c_type() == C_TYPE_AXIAL || Ce->c_type() == C_TYPE_DEGE_AXIAL);
    assert (MS->is_E_in_icurve_pair (MC) == false);

    if (MN_del == MC->s_MN()) { //2-1-1) Merge MC to Cs at Ns.
      ///assert (MS->is_E_in_icurve_pair (Cs) == false); OK here. 
      MC_merge_set.insert (Cs); //Put Cs into MC_merge_set.
    }
    else { //2-1-2) Merge MC to Ce at Ne.
      assert (MN_del == MC->e_MN());
      ///assert (MS->is_E_in_icurve_pair (Ce) == false); OK here.
      MC_merge_set.insert (Ce); //Put Ce into MC_merge_set.
    }
  }

  //Add virtual link of (MN_final - MC - MN_del - MC_merge_set[] - otherN[])
  vcl_set<dbsk3d_ms_curve*>::iterator cit = MC_merge_set.begin();
  for (; cit != MC_merge_set.end(); cit++) {
    dbsk3d_ms_curve* mergeMC = (*cit);
    dbmsh3d_node* otherN = (dbmsh3d_node*) mergeMC->other_V (MN_del);
    ms_hypg_->add_virtual_curve (MN_final, otherN, MC, mergeMC);
    n_vl++;
  }

  return n_vl;
}

int dbsk3d_ms_hypg_trans::add_vl_A15_sheet_contract (dbsk3d_ms_sheet* MS)
{
  int n_vl = 0;
  
  //1-1) MS can not have i-curve chain.
  assert (MS->have_icurve_chain() == false); 

  //1-2) MS can not have incident A3 ribs.
  vcl_set<dbsk3d_ms_node*> MNset;
  int count = 0;
  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    assert (MC->c_type() != C_TYPE_RIB);
    MNset.insert (MC->s_MN());
    MNset.insert (MC->e_MN());
    count++;
    assert (MC->n_incident_Fs() > 2);
    HE = HE->next();
  }
  while (HE != MS->halfedge());
  assert (count == 3);
  
  //Determine the ms_curve MCd to delete.
  dbsk3d_ms_curve* MCd = (dbsk3d_ms_curve*) MS->halfedge()->edge();

  //Determine the two merging node MNp and MNq from MC.
  dbsk3d_ms_node* MNp = MCd->s_MN();
  dbsk3d_ms_node* MNq = MCd->e_MN();

  //1-4) Skip transform if MNp and MNq have diff. # of incident C's.
  assert (MNp->n_E_incidence_nv() == MNq->n_E_incidence_nv());

  //1-5) Determine the merging sheets (MS1, MS2, ...).
  vcl_vector<dbmsh3d_halfedge*> MS_merge_HE;
  HE = MCd->halfedge();
  do {
    dbsk3d_ms_sheet* MSi = (dbsk3d_ms_sheet*) HE->face();
    if (MSi != MS)
      MS_merge_HE.push_back (HE);
    HE = HE->pair();
  }
  while (HE != MCd->halfedge());

  assert (MS_merge_HE.size() > 1);

  //1-6) Determine the curves MCp and MCq incident to MS.
  dbsk3d_ms_curve* MCq = (dbsk3d_ms_curve*) MS->halfedge()->next()->edge();
  assert (MCq);
  dbsk3d_ms_curve* MCp = (dbsk3d_ms_curve*) _find_prev_in_next_chain (MS->halfedge())->edge();
  assert (MCp);
  if (MCp->is_V_incident (MNp) == false) 
    _swap_ms_curve (MCp, MCq); //swap    

  //1-7) Determine the final A15 ms_node MNA15.
  bool loop2;
  dbmsh3d_vertex* Vpq = Es_sharing_V_check (MCd, MCq, loop2);
  assert (loop2 == false);
  dbsk3d_ms_node* MNA15 = (dbsk3d_ms_node*) MCq->other_V (Vpq);

  //1-8) Determine the MCpm[i] and MCqm[i] for each MSi to be merged.
  vcl_set<dbmsh3d_face*> FFpm_set_visited, FFqm_set_visited;
  vcl_vector<dbsk3d_ms_curve*> MCpm_vec, MCqm_vec;
  vcl_set<dbmsh3d_sheet*> MS_merge_set;
  for (unsigned int i=0; i<MS_merge_HE.size(); i++) {
    dbsk3d_ms_sheet* MSi = (dbsk3d_ms_sheet*) MS_merge_HE[i]->face();
    MS_merge_set.insert (MSi);

    //1-8-1) Determine the curves MCpm and MCqm incident to MSi via the fine-scale elements.
    //the HE.next does not always work here, since MCd can be an icurve of the MSi.
    dbsk3d_ms_curve* MCpm = (dbsk3d_ms_curve*) MSi->get_1st_other_C_via_F2 (MCd, MNp, FFpm_set_visited);
    assert (MCpm);

    //Skip if MCp and MCpm is a loop, this will result in a loop curve.
    bool loop2;
    Es_sharing_V_check (MCp, MCpm, loop2);
    assert (loop2 == false);

    MCpm_vec.push_back (MCpm);
    assert (MCpm != MCd && MCpm != MCp);

    dbsk3d_ms_curve* MCqm = (dbsk3d_ms_curve*) MSi->get_1st_other_C_via_F2 (MCd, MNq, FFqm_set_visited);
    assert (MCqm);

    //Skip if MCq and MCqm is a loop, this will result in a loop curve.
    Es_sharing_V_check (MCq, MCqm, loop2);
    assert (loop2 == false);

    MCqm_vec.push_back (MCqm);
    assert (MCqm != MCd && MCqm != MCp);

    assert (MCpm != MCqm);
  }

  //1-9) If MCd has shared_E[], check if they are only with MCpm[] and MCqm[].
  if (MCd->have_shared_Es()) {
    vcl_set<dbmsh3d_curve*> shared_E_Cset;
    for (unsigned int i=0; i<MCpm_vec.size(); i++)
      shared_E_Cset.insert (MCpm_vec[i]);
    for (unsigned int i=0; i<MCqm_vec.size(); i++)
      shared_E_Cset.insert (MCqm_vec[i]);

    assert (MCd->shared_E_with_Cset (shared_E_Cset));
  }

  //1-10) If any shared_F of MS not shared by MS_merge_set[], skip xform.
  assert (MS->shared_F_with_Sset (MS_merge_set));

  //Go through each ms_sheet in MS_merge.
  //Add virtual link of (MNA15 - MCp - MNp - MCpm[] - otherN[])
  //Add virtual link of (MNA15 - MCq - MNq - MCqm[] - otherN[])
  assert (MS_merge_HE.size() > 1);
  for (unsigned int i=0; i<MS_merge_HE.size(); i++) {
    dbsk3d_ms_sheet* MSi = (dbsk3d_ms_sheet*) MS_merge_HE[i]->face();
    dbsk3d_ms_curve* MCpm = MCpm_vec[i];
    dbsk3d_ms_curve* MCqm = MCqm_vec[i];

    dbmsh3d_node* otherN = (dbmsh3d_node*) MCpm->other_V (MNp);
    ms_hypg_->add_virtual_curve (MNA15, otherN, MCp, MCpm);
    n_vl++;

    otherN = (dbmsh3d_node*) MCqm->other_V (MNq);
    ms_hypg_->add_virtual_curve (MNA15, otherN, MCq, MCqm);
    n_vl++;
  }

  return n_vl;
}
  
int dbsk3d_ms_hypg_trans::add_vl_A14_sheet_contract (dbsk3d_ms_sheet* MS)
{
  int n_vl = 0;
  assert (MS->have_icurve_chain() == false); //MS can not have i-curve chain.

  //1-1) MS can not have incident A3 ribs.
  //Determine the two ms_curves MCp and MCq of MS.
  dbsk3d_ms_curve *MCp = NULL, *MCq = NULL;
  vcl_set<dbsk3d_ms_node*> MNset;
  int n_bnd = MS->n_bnd_Es();
  assert (n_bnd == 2);

  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    assert (MC->c_type() != C_TYPE_RIB); //MS can not have incident A3 ribs.
    MNset.insert (MC->s_MN());
    MNset.insert (MC->e_MN());
    assert (MC->n_incident_Fs() > 2);
    if (MCp == NULL)
      MCp = MC;
    else if (MCq == NULL)
      MCq = MC;
    else
      assert (0);

    HE = HE->next();
  }
  while (HE != MS->halfedge());
  assert (MCp != NULL && MCq != NULL && MCp != MCq);

  //1-3) Determine the ms_node NA14 (of A14) to keep and Nm to be merged.
  dbsk3d_ms_node *NA14, *Nm;
  assert (MCp->is_self_loop() == false);
  if (MCp->s_MN()->n_type() == N_TYPE_AXIAL_END) {
    NA14 = MCp->e_MN();
    Nm = MCp->s_MN();
  }
  else {
    Nm = MCp->e_MN();
    NA14 = MCp->s_MN();
  }

  assert (NA14->is_E_incident (MCp));
  assert (NA14->is_E_incident (MCq));
  assert (Nm->is_E_incident (MCp));
  assert (Nm->is_E_incident (MCq));

  //1-4) Determine the merging ms_curves MCpm and MCqm.
  assert (Nm->n_incident_Cs_nv() == 4);

  dbsk3d_ms_curve *MCpm = NULL, *MCqm = NULL;
  for (dbmsh3d_ptr_node* cur = Nm->E_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) cur->ptr();
    if (MC == MCp || MC == MCq)
      continue;
    if (MCpm == NULL)
      MCpm = MC;
    else if (MCqm == NULL)
      MCqm = MC;
    else
      assert (0);
  }
  assert (MCpm && MCqm);
  
  //Add virtual link of (NA14 - MCp - Nm - MCpm - otherN)
  dbmsh3d_node* otherN = (dbmsh3d_node*) MCpm->other_V (Nm);
  ms_hypg_->add_virtual_curve (NA14, otherN, MCp, MCpm);
  n_vl++;

  //Add virtual link of (NA14 - MCq - Nm - MCqm - otherN)
  otherN = (dbmsh3d_node*) MCqm->other_V (Nm);
  ms_hypg_->add_virtual_curve (NA14, otherN, MCq, MCqm);
  n_vl++;

  return n_vl;
}

