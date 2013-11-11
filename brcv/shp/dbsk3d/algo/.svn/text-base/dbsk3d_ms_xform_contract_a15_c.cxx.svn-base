//: dbsk3d_ms_xform_contract_a15_c.cxx
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
//  A15 Curve Contract xform on ms_curve MC
//#############################################################

bool MC_valid_for_A15_contract_xform (dbsk3d_ms_curve* MC)
{
  if (MC->c_type() != C_TYPE_AXIAL && MC->c_type() != C_TYPE_DEGE_AXIAL)
    return false;
  if (MC->have_shared_Es())
    return false; //Can not contract MC with a shared_E (hybrid xforms).
  if (MC->s_MN()->n_type() == N_TYPE_RIB_END || MC->e_MN()->n_type() == N_TYPE_RIB_END)
    return false;
  if (MC->s_MN()->has_rib_C())
    return false;
  if (MC->e_MN()->has_rib_C())
    return false;

  //Check if the pair loop contains >=3 halfedges with the same edge.
  if (is_HE_pair_3p_inc (MC->halfedge()) == false)
    return false; 

  //Go through each incident MS of MC and check if
  // - Cs and Ce is of type axial.
  // - Cs and Ce is not on MS's icurve-pair 
  // - decide possible valid merges at either Ns_valid_merge or Ne_valid_merge.
  // - MS has 2 or more boundary MCs.
  dbmsh3d_vertex* Ns_valid_merge = MC->s_MN();
  dbmsh3d_vertex* Ne_valid_merge = MC->e_MN();
  dbmsh3d_halfedge* HE = MC->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
    if (MS->n_bnd_Es() < 3)
      return false; //Avoiding leaving MS with 1 boundary MC.
    
    dbmsh3d_curve* otherC;
    dbmsh3d_face* MF;
    bool r = MS->get_otherC_via_F (MC, MC->s_MN(), otherC, MF);
    if (r == false)
      return false; //Can't find otherC on MC.sN via MF.
    dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) otherC;

    r = MS->get_otherC_via_F (MC, MC->e_MN(), otherC, MF);
    if (r == false)
      return false; //Can't find otherC on MC.eN via MF.
    dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) otherC;

    if (Cs->c_type() != C_TYPE_AXIAL && Cs->c_type() != C_TYPE_DEGE_AXIAL)
      return false;
    if (Ce->c_type() != C_TYPE_AXIAL && Ce->c_type() != C_TYPE_DEGE_AXIAL)
      return false;
    if (MS->is_E_in_icurve_pair (MC))
      return false;

    if (MS->is_E_in_icurve_pair (Cs))
      Ns_valid_merge = NULL;
    if (MS->is_E_in_icurve_pair (Ce))
      Ne_valid_merge = NULL;

    if (Ns_valid_merge==NULL && Ne_valid_merge==NULL)
      return false; //no valid xform for merging curves in either ends.

    HE = HE->pair();
  }
  while (HE != MC->halfedge() && HE != NULL);
     
  return true;
}



