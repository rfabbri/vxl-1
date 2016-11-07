//: dbsk3d_ms_xform_splice.cxx
//: MingChing Chang
//  Sep 10, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>

//###############################################################
//  Splice xform on ms_sheet MS
//###############################################################

//: Return true if the medial sheet is valid for a splice transform.
bool MS_valid_for_splice_xform (dbsk3d_ms_sheet* MS)
{
  //If the MS has any internal curve, not valid.
  if (MS->have_icurve_chain())
    return false;

  //If the MS is completely interior (not sharing any A3 rib), not valid.
  if (MS->has_incident_A3rib() == false)
    return false;
  
  return true;
}

bool get_2_A1A3s_from_axials (vcl_vector<dbsk3d_ms_curve*>& bnd_A13_MCs,
                              dbsk3d_ms_node*& A1A3_1, dbsk3d_ms_node*& A1A3_2)
{
  int nA3, nA13, nDege, nVirtual;
  A1A3_1 = NULL;
  A1A3_2 = NULL;

  for (unsigned int i=0; i<bnd_A13_MCs.size(); i++) {
    dbsk3d_ms_curve* A13MC = (dbsk3d_ms_curve*) bnd_A13_MCs[i];
    for (unsigned int j=0; j<2; j++) {
      dbsk3d_ms_node* MN = (dbsk3d_ms_node*) A13MC->vertices(j);    
      MN->count_incident_Cs (nA3, nA13, nDege, nVirtual);
      if (nA3 != 0) { //A1A3 found
        if (A1A3_1 == NULL) {
          A1A3_1 = MN;
        }
        else if (A1A3_2 == NULL) {
          A1A3_2 = MN;
          return true;
        }
        else
          return false;
      }
    }    
  }

  return false;
}

//: Remove the shared_E on the splicing curve MC during a splice xform.
void splice_remove_MC_shared_Es (dbsk3d_ms_curve* MC, dbsk3d_ms_sheet* MS,
                                 vcl_set<dbsk3d_ms_curve*>& modified_MC_set)
{
  //Collect shared_C_set from incident curves of MC.s and MC.e
  //as the set of possible curves to share E with MC.
  vcl_set<void*> shared_C_set;
  MC->s_MN()->get_incident_Es (shared_C_set);
  MC->e_MN()->get_incident_Es (shared_C_set);
  assert (MS->have_icurve_chain() == false);

  //The shared_Es on all related ms_curves of MS should be removed.
  //Loop through each shared_E of MC and try to remove isolated entry from other curves.
  for (dbmsh3d_ptr_node* cur = MC->shared_E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();

    //Collect the set of curves sharing E in Cs_share_E.
    vcl_set<dbmsh3d_curve*> Cs_share_E;
    vcl_set<void*>::iterator it = shared_C_set.begin();
    for (; it != shared_C_set.end(); it++) {
      dbmsh3d_curve* C = (dbmsh3d_curve*) (*it);
      if (C->is_E_shared (E))
        Cs_share_E.insert (C);
    }
    if (Cs_share_E.size() == 0)
      continue; //skip

    //Go through Cs_share_E and remove entry incident to MS.
    vcl_set<dbmsh3d_curve*>::iterator cit = Cs_share_E.begin();
    while (cit != Cs_share_E.end()) {
      dbmsh3d_curve* C = (dbmsh3d_curve*) (*cit);
      if (C == MC || MS->is_bnd_E (C)) {
        if (cit == Cs_share_E.begin()) {
          Cs_share_E.erase (cit);
          cit = Cs_share_E.begin();
        }
        else {
          vcl_set<dbmsh3d_curve*>::iterator tmp = cit;
          tmp--;
          Cs_share_E.erase (cit);
          tmp++;
          cit = tmp;
        }
      }
      else
        cit++;
    }

    //If there is only one curve sharing E, remove its entry.
    //Otherwise keep the shared_E there.
    if (Cs_share_E.size() == 1) {
      cit = Cs_share_E.begin();
      dbsk3d_ms_curve* C = (dbsk3d_ms_curve*) (*cit);
      C->del_shared_E (E);
    }
    
    //Add all Cs_share_E[] to modified_MC_set,
    //because the shared_E's type will be reset and might need to be re-determined.
    cit = Cs_share_E.begin();
    for (; cit != Cs_share_E.end(); cit++) {
      dbsk3d_ms_curve* C = (dbsk3d_ms_curve*) (*cit);
      modified_MC_set.insert (C);
    }
  }

  //Delete all shared_E on MC with other ms_curves on MS.
  vcl_vector<dbsk3d_ms_curve*> shared_MCs;
  MS->find_MCs_sharing_E (MC, shared_MCs);
  //Remove the sharing_E from MC to shared_MC_set.
  for (unsigned int i=0; i<shared_MCs.size(); i++) {
    dbsk3d_ms_curve* sMC = shared_MCs[i];
    remove_Cs_sharing_E (MC, sMC);
  }

  //Delete all shared_Es on MC.
  MC->clear_shared_E_list ();
}

//: Splice the two sheets together: merge the smaller (MS2) to the larger sheet (MS1).
//  Delete the incident MCs after the merging and splice the remaining shock axials.
void MS_splice_ms_sheets (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_curve* MC, 
                          dbsk3d_ms_sheet* MS1, dbsk3d_ms_sheet* MS2)
{
  assert (MS1 != MS2);

  vul_printf (vcl_cout, "splice S%d to S%d at C%d, ", MS2->id(), MS1->id(), MC->id());

  //Disconnect the bordering curve MC and connecting the boundary chain of MS2 to MS1.
  //MC is not deleted, will be deleted later in remove_S_complete_hypg().  
  merge_sheets_sharing_E (ms_hypg, MC, MS1, MS2, false);

  //Transfer all MS2's icurves to MS1.  
  for (dbmsh3d_ptr_node* cur = MS2->icurve_chain_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    MS1->add_icurve_chain_HE (headHE);
    //Go through each icurve HE and modify the face pointer.
    _modify_F_icurve_chain (headHE, MS1);
  }
  MS2->clear_icurve_chains ();

  //Transfer all MS2's sharedF's to MS1.
  MS2->pass_shared_Fs_to_S (MS1);

  //Merge fine-scale mesh elements.
  //Transfer all facemap, edgemap, vertexmap of MS2 to MS1.  
  vcl_map<int, dbmsh3d_face*>::iterator fit = MS2->facemap().begin();
  for (; fit != MS2->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    MS1->add_F (F);
  }
  MS2->facemap().clear();

  //Set all MS2.FF's to invalid.
  fit = MS2->facemap().begin();
  for (; fit != MS2->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    FF->set_valid (false);
  }

  //Delete MS2, MC's and SV's only incident to MS2 will be removed.
  ms_hypg->remove_sheet (MS2->id());

  //The merging of remaining MC's on MN's will be handled later.
}



