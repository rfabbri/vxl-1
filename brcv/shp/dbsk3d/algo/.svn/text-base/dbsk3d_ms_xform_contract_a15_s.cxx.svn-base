//: dbsk3d_ms_xform_contract_a15_s.cxx
//: MingChing Chang
//  Nov. 11, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>

//#############################################################
//  A15 sheet-contract xform on ms_sheet MS
//#############################################################

bool MS_valid_for_A15_contract_xform (dbsk3d_ms_sheet* MS)
{
  //1-1) MS can not have i-curve chain.
  if (MS->have_icurve_chain())
    return false;
  
  //1-2) MS can not have incident A3 ribs.
  vcl_set<dbsk3d_ms_node*> MNset;
  int count = 0;
  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_RIB)
      return false;
    MNset.insert (MC->s_MN());
    MNset.insert (MC->e_MN());
    count++;
    assert (MC->n_incident_Fs() > 2);
    HE = HE->next();
  }
  while (HE != MS->halfedge() && HE != NULL);

  //The A15-sheet-contract is defined only on ms_sheet with 3 bnd curves.
  if (count != 3)
    return false;

  //Determine the ms_curve MCd to delete.
  dbsk3d_ms_curve* MCd = (dbsk3d_ms_curve*) MS->halfedge()->edge();

  //Determine the two merging node MNp and MNq from MC.
  dbsk3d_ms_node* MNp = MCd->s_MN();
  dbsk3d_ms_node* MNq = MCd->e_MN();

  //1-4) Skip transform if MNp and MNq have diff. # of incident C's.
  if (MNp->n_E_incidence() != MNq->n_E_incidence())
    return false;

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
    if (MCpm == NULL)
      return false;

    //Skip if MCp and MCpm is a loop, this will result in a loop curve.
    bool loop2;
    Es_sharing_V_check (MCp, MCpm, loop2);
    if (loop2)
      return false;

    MCpm_vec.push_back (MCpm);
    assert (MCpm != MCd && MCpm != MCp);

    dbsk3d_ms_curve* MCqm = (dbsk3d_ms_curve*) MSi->get_1st_other_C_via_F2 (MCd, MNq, FFqm_set_visited);
    if (MCqm == NULL)
      return false;

    //Skip if MCq and MCqm is a loop, this will result in a loop curve.
    Es_sharing_V_check (MCq, MCqm, loop2);
    if (loop2)
      return false;

    MCqm_vec.push_back (MCqm);
    assert (MCqm != MCd && MCqm != MCp);

    if (MCpm == MCqm)
      return false;
  }

  //1-9) If MCd has shared_E[], check if they are only with MCpm[] and MCqm[].
  if (MCd->have_shared_Es()) {
    vcl_set<dbmsh3d_curve*> shared_E_Cset;
    for (unsigned int i=0; i<MCpm_vec.size(); i++)
      shared_E_Cset.insert (MCpm_vec[i]);
    for (unsigned int i=0; i<MCqm_vec.size(); i++)
      shared_E_Cset.insert (MCqm_vec[i]);

    if (MCd->shared_E_with_Cset (shared_E_Cset) == false)
      return false;
  }

  //1-10) If any shared_F of MS not shared by MS_merge_set[], skip xform.
  if (MS->shared_F_with_Sset (MS_merge_set) == false)
    return false;

  return true;
}


