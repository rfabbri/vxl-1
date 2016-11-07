//: dbsk3d_ms_xform_contract_a14_s.cxx
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
//  A14 Curve Contract xform on ms_sheet MS
//#############################################################

bool MS_valid_for_A14_contract_xform (dbsk3d_ms_sheet* MS)
{
  //MS can not have i-curve chain.
  if (MS->have_icurve_chain())
    return false;
  
  //Determine the two ms_curves MCp and MCq of MS.
  dbsk3d_ms_curve *MCp = NULL, *MCq = NULL;
  vcl_set<dbsk3d_ms_node*> MNset;
  int n_bnd = MS->n_bnd_Es();
  if (n_bnd != 2)
    return false;

  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_RIB)
      return false; //MS can not have incident A3 ribs.
    MNset.insert (MC->s_MN());
    MNset.insert (MC->e_MN());
    if (MC->n_incident_Fs() <= 2)
      return false;
    if (MCp == NULL)
      MCp = MC;
    else if (MCq == NULL)
      MCq = MC;
    else
      return false;

    HE = HE->next();
  }
  while (HE != MS->halfedge());
  if (MCp == NULL || MCq == NULL || MCp == MCq)
    return false;

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
  if (Nm->n_incident_Es() != 4)
    return false;

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
      return false;
  }
  if (MCpm==NULL || MCqm==NULL)
    return false;

  return true;
}


