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

//############################################################################
//  Transition Regularization Functions.
//############################################################################

//: Shock splice transforms for regularization: remove A1A3-I A12A3-II tabs.
// 1) Decide the list of dbsk3d_ms_sheet to be removed
//     - A1A3-I, A12A3-I A12A3-II tab: with at least one A3rib as boundary
//     - # of asso. genes < tab_th
// 2) NA14 while loop to iteratively remove next valid tabs until finish.
// 3) Modify the boundary (make the changes on boundary displayable - todo).

//: Setup parameters for MS transition regularization.
void dbsk3d_ms_hypg_trans::ms_trans_regul_set_params (const float Wts, const float Tts,
                                                      const float Wcc, const float Tcc,
                                                      const float Wc5, const float Tc5,
                                                      const float Wsc, const float Tsc,
                                                      const float Wnnm, const float Tnnm,
                                                      const float Wncm, const float Tncm,
                                                      const float Wccm, const float Tccm,
                                                      const float Tncm_a1a5, 
                                                      const float cmxth) 
{
  Wts_ = Wts;
  Tts_ = Tts;
  Wcc_ = Wcc;
  Tcc_ = Tcc;
  Wc5_ = Wc5;
  Tc5_ = Tc5;
  Wsc_ = Wsc;
  Tsc_ = Tsc;
  Wnnm_ = Wnnm;
  Tnnm_ = Tnnm;
  Wncm_ = Wncm;
  Tncm_ = Tncm;
  Wccm_ = Wccm;
  Tccm_ = Tccm;
  Tncm_a1a5_ = Tncm_a1a5;
  cmxth_ = cmxth;

  vul_printf (vcl_cout, "\n\nMedial scaffold transform parameters:\n");
  vul_printf (vcl_cout, "    tab splice cost: # asso genes,           weight %.1f, th %.1f.\n", Wts_, Tts_);
  vul_printf (vcl_cout, "    curve contract cost: # fs_edges,         weight %.1f, th %.1f.\n", Wcc_, Tcc_);
  vul_printf (vcl_cout, "    A5 curve contract cost: # fs_edges,      weight %.1f, th %.1f.\n", Wc5_, Tc5_);
  vul_printf (vcl_cout, "    sheet contract cost: # fs_faces,         weight %.1f, th %.1f.\n", Wsc_, Tsc_);
  vul_printf (vcl_cout, "    node-node merge cost: # fs_edges,        weight %.1f, th %.1f.\n", Wnnm_, Tnnm_);
  vul_printf (vcl_cout, "    node-curve merge cost: # fs_edges,       weight %.1f, th %.1f.\n", Wncm_, Tncm_);
  vul_printf (vcl_cout, "    curve-curve merge cost: # fs_edges,      weight %.1f, th %.1f.\n", Wccm_, Tccm_);
  vul_printf (vcl_cout, "    A1A5 node-curve merge cost: # fs_edges,  weight %.1f, th %.1f.\n", Wncm_, Tncm_a1a5_);
  vul_printf (vcl_cout, "    curve merge validity th: # fs_edges,     %.1f.\n", cmxth);
}

//: Initialization the greedy iteration of medial scaffold regularization.
void dbsk3d_ms_hypg_trans::ms_trans_regul_init ()
{
  vul_printf (vcl_cout, "ms_trans_regul_init(): %d MS, %d MC, %d MN.\n", 
              ms_hypg_->sheetmap().size(), ms_hypg_->edgemap().size(), ms_hypg_->vertexmap().size());
  vul_printf (vcl_cout, "  Adding candidate shock transforms to ms_xform_Q: \n");

  //1) Search all ms_sheets for all possible xforms.
  vcl_map<int, dbmsh3d_sheet*>::iterator sit = ms_hypg_->sheetmap().begin();
  for (; sit != ms_hypg_->sheetmap().end(); sit++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*sit).second;
    try_add_MS_to_Q (MS);
  }

  //2) Search all ms_curves for all possible xforms.
  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg_->edgemap().begin();
  for (; cit != ms_hypg_->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    try_add_MC_to_Q (MC);
  }

  //2) Search all ms_nodes for all possible xforms.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = ms_hypg_->vertexmap().begin();
  for (; vit != ms_hypg_->vertexmap().end();vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit).second;
    try_add_MN_to_Q (MN);
  }

  vul_printf (vcl_cout, "\n\n");
}

//: Medial scaffold regularization by applying shock transforms 
//  across transitions (topological changes) in a greey iteration.
void dbsk3d_ms_hypg_trans::ms_trans_regul_iters (const int debug_stop_id1, const int debug_stop_id2)
{  
  //NA14 while loop to go through all candidate MS's until there is no change.
  vul_printf (vcl_cout, "ms_trans_regul_iters(): Greedy iterative shock transition regularization:");
  int n_tab_s_success = 0, n_tab_s_fail = 0, n_tab_s_skip = 0;
  int n_a5_c_success = 0, n_a5_c_fail = 0, n_a5_c_skip = 0;
  int n_a12a3i_c_success = 0, n_a12a3i_c_fail = 0, n_a12a3i_c_skip = 0;
  int n_a15_c_c_success = 0, n_a15_c_c_fail = 0, n_a15_c_c_skip = 0;
  int n_a15_s_c_success = 0, n_a15_s_c_fail = 0, n_a15_s_c_skip = 0;
  int n_a14_s_c_success = 0, n_a14_s_c_fail = 0, n_a14_s_c_skip = 0;
  int n_a1a3ii_nn_m_success = 0, n_a1a3ii_nn_m_fail = 0, n_a1a3ii_nn_m_skip = 0;
  int n_a12a3i_nc_m_success = 0, n_a12a3i_nc_m_fail = 0, n_a12a3i_nc_m_skip = 0;
  int n_a14_cc_m_success = 0, n_a14_cc_m_fail = 0, n_a14_cc_m_skip = 0;
  int n_a1a3ii_cc_m_success = 0, n_a1a3ii_cc_m_fail = 0, n_a1a3ii_cc_m_skip = 0;
  int n_a1a5_nc_m_success = 0, n_a1a5_nc_m_fail = 0, n_a1a5_nc_m_skip = 0;

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
      if (MS->id()==debug_stop_id1) { // Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        return;
      }
      _remove_MS_from_Q (MS);
      r = sheet_splice_xform (MS, false);
      if (r == MSXT_SUCCESS)
        n_tab_s_success++;
      else if (r == MSXT_FAIL)
        n_tab_s_fail++;
      else if (r == MSXT_SKIP)
        n_tab_s_skip++;
    break;
    case MSXT_A5_CURVE_CONTRACT: //A5 swallow-tail curve-contract transform.   
      if (MC1->id()==debug_stop_id1) { // Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        return;
      }
      _remove_MC_from_Q (MC1);  
      r = A5_curve_contract_xform (MC1, false);
      if (r == MSXT_SUCCESS)
        n_a5_c_success++;
      else if (r == MSXT_FAIL)
        n_a5_c_fail++;
      else if (r == MSXT_SKIP)
        n_a5_c_skip++;
    break;
    case MSXT_A12A3I_CURVE_CONTRACT: //A12A3-I curve-contract transform.      
      if (MC1->id()==debug_stop_id1) { // Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        return;
      }
      _remove_MC_from_Q (MC1);
      r = A12A3I_curve_contract_xform (MC1, false);
      if (r == MSXT_SUCCESS)
        n_a12a3i_c_success++;
      else if (r == MSXT_FAIL)
        n_a12a3i_c_fail++;
      else if (r == MSXT_SKIP)
        n_a12a3i_c_skip++;
    break;
    case MSXT_A15_CURVE_CONTRACT: //A15 curve-contract transform.   
      if (MC1->id()==debug_stop_id1) { // Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        return;
      }
      _remove_MC_from_Q (MC1);
      r = A15_curve_contract_xform (MC1, false);
      if (r == MSXT_SUCCESS)
        n_a15_c_c_success++;
      else if (r == MSXT_FAIL)
        n_a15_c_c_fail++;
      else if (r == MSXT_SKIP)
        n_a15_c_c_skip++;
    break;
    case MSXT_A15_SHEET_CONTRACT: //A15 sheet-contract transform.   
      if (MS->id()==debug_stop_id1) { // Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        return;
      }
      _remove_MS_from_Q (MS);
      r = A15_sheet_contract_xform (MS, false);
      if (r == MSXT_SUCCESS)
        n_a15_s_c_success++;
      else if (r == MSXT_FAIL)
        n_a15_s_c_fail++;
      else if (r == MSXT_SKIP)
        n_a15_s_c_skip++;
    break;
    case MSXT_A14_SHEET_CONTRACT: //A14 sheet-contract transform.   
      //if (MS->id()==debug_stop_id1) { //Stop if speficied shock id is reached.
        //vul_printf (vcl_cout, "\n Stop at specified debug id = %d.\n", debug_stop_id1);
        //return;
      //}
      _remove_MS_from_Q (MS);
      r = A14_sheet_contract_xform (MS, false);
      if (r == MSXT_SUCCESS)
        n_a14_s_c_success++;
      else if (r == MSXT_FAIL)
        n_a14_s_c_fail++;
      else if (r == MSXT_SKIP)
        n_a14_s_c_skip++;
    break;
    case MSXT_A1A3II_N_N_MERGE: //A1A3-II node-node merge transform.   
      if (MN1->id()==debug_stop_id1 && MN2->id()==debug_stop_id2) { //Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug ids = %d and %d.\n", debug_stop_id1, debug_stop_id2);
        return;
      }
      _remove_MN_from_Q (MN1);
      _remove_MN_from_Q (MN2);
      r = A1A3II_n_n_merge_xform (MN1, MN2, cost, false);
      if (r == MSXT_SUCCESS)
        n_a1a3ii_nn_m_success++;
      else if (r == MSXT_FAIL)
        n_a1a3ii_nn_m_fail++;
      else if (r == MSXT_SKIP)
        n_a1a3ii_nn_m_skip++;
    break;
    case MSXT_A12A3I_N_C_MERGE: //A12A3-I node-curve merge transform. 
      if (MN1->id()==debug_stop_id1 && MC2->id()==debug_stop_id2) { //Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug ids = %d and %d.\n", debug_stop_id1, debug_stop_id2);
        return;
      }
      _remove_MN_from_Q (MN1);
      _remove_MC_from_Q (MC2);
      r = A12A3I_n_c_merge_xform (MN1, MC2, cost, false);
      if (r == MSXT_SUCCESS)
        n_a12a3i_nc_m_success++;
      else if (r == MSXT_FAIL)
        n_a12a3i_nc_m_fail++;
      else if (r == MSXT_SKIP)
        n_a12a3i_nc_m_skip++;
    break;    
    case MSXT_A14_C_C_MERGE: //A14 curve-curve merge transform. 
      if (MC1->id()==debug_stop_id1 && MC2->id()==debug_stop_id2) { //Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug ids = %d and %d.\n", debug_stop_id1, debug_stop_id2);
        return;
      }
      _remove_MC_from_Q (MC1);
      _remove_MC_from_Q (MC2);
      r = A14_c_c_merge_xform (MC1, MC2, cost, false);
      if (r == MSXT_SUCCESS)
        n_a14_cc_m_success++;
      else if (r == MSXT_FAIL)
        n_a14_cc_m_fail++;
      else if (r == MSXT_SKIP)
        n_a14_cc_m_skip++;
    break;
    case MSXT_A1A3II_C_C_MERGE: //A1A3-II curve-curve merge transform. 
      if (MC1->id()==debug_stop_id1 && MC2->id()==debug_stop_id2) { //Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug ids = %d and %d.\n", debug_stop_id1, debug_stop_id2);
        return;
      }
      _remove_MC_from_Q (MC1);
      _remove_MC_from_Q (MC2);
      r = A1A3II_c_c_merge_xform (MC1, MC2, cost, false);
      if (r == MSXT_SUCCESS)
        n_a1a3ii_cc_m_success++;
      else if (r == MSXT_FAIL)
        n_a1a3ii_cc_m_fail++;
      else if (r == MSXT_SKIP)
        n_a1a3ii_cc_m_skip++;
    break;
    case MSXT_A1A5_N_C_MERGE: //A1A5 node-curve merge transform. 
      if (MN1->id()==debug_stop_id1 && MC2->id()==debug_stop_id2) { //Stop if speficied shock id is reached.
        vul_printf (vcl_cout, "\n Stop at specified debug ids = %d and %d.\n", debug_stop_id1, debug_stop_id2);
        return;
      }
      _remove_MN_from_Q (MN1);
      _remove_MC_from_Q (MC2);
      r = A1A5_n_c_merge_xform (MN1, MC2, cost, false);
      if (r == MSXT_SUCCESS)
        n_a1a5_nc_m_success++;
      else if (r == MSXT_FAIL)
        n_a1a5_nc_m_fail++;
      else if (r == MSXT_SKIP)
        n_a1a5_nc_m_skip++;
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

  vul_printf (vcl_cout, "\n\n    success / fail / skip / total xforms done:\n");
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A1A3-I or A12A3-II Sheet-Splice xforms done.\n", 
              n_tab_s_success, n_tab_s_fail, n_tab_s_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A5 Curve-Contract xforms done.\n", 
              n_a5_c_success, n_a5_c_fail, n_a5_c_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A12A3-I Curve-Contract xforms done.\n", 
              n_a12a3i_c_success, n_a12a3i_c_fail, n_a12a3i_c_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A15 Curve-Contract xforms done.\n", 
              n_a15_c_c_success, n_a15_c_c_fail, n_a15_c_c_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A15 Sheet-Contract xforms done.\n",
              n_a15_s_c_success, n_a15_s_c_fail, n_a15_s_c_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A14 Sheet-Contract xforms done.\n", 
              n_a14_s_c_success, n_a14_s_c_fail, n_a14_s_c_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A1A3-II Node-Node Merge xforms done.\n", 
              n_a1a3ii_nn_m_success, n_a1a3ii_nn_m_fail, n_a1a3ii_nn_m_skip);  
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A12A3-I Node-Curve Merge xforms done.\n", 
              n_a12a3i_nc_m_success, n_a12a3i_nc_m_fail, n_a12a3i_nc_m_skip);
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A14 Curve-Curve Merge xforms done.\n", 
              n_a14_cc_m_success, n_a14_cc_m_fail, n_a14_cc_m_skip);
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A1A3-II Curve-Curve Merge xforms done.\n", 
              n_a1a3ii_cc_m_success, n_a1a3ii_cc_m_fail, n_a1a3ii_cc_m_skip);
  vul_printf (vcl_cout, "    %3d / %3d / %3d  A1A5 Node-Curve Merge xforms done.\n", 
              n_a1a5_nc_m_success, n_a1a5_nc_m_fail, n_a1a5_nc_m_skip);
}

//############################################################################
//  A1A3-I or A12A3-II splice transform functions for general cases
//############################################################################

//: Splice transform: splice two ms_sheets together.
//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::sheet_splice_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  sheet_splice_xform() on %d: \n\t", MS->id());

  //1) Determine if this transform is valid.
  //1-1) Skip if the tab MS has any IC-chain.
  if (MS->have_icurve_chain()) {
    vul_printf (vcl_cout, "Skip xform: S%d has icurve.\n", MS->id());
    return MSXT_SKIP;
  }

  //1-2) Determine the vector bnd_A13_MCs[] of all non-swallow-tail boundary A13 axials of MS.
  //     Since MS is a tab, bnd_A13_MCs[] is a well-defined continuous chain.
  vcl_vector<dbsk3d_ms_curve*> bnd_A13_MCs;
  MS->get_axial_nonsw_bnd (bnd_A13_MCs);

  //1-3) Determine the two A1A3 nodes from bnd_A13_MCs[].
  dbsk3d_ms_node *A1A3_1, *A1A3_2;
  if (get_2_A1A3s_from_axials (bnd_A13_MCs, A1A3_1, A1A3_2) == false) {
    //Skip if the two nodes can not be found (should not happen).
    vul_printf (vcl_cout, "Fail: A1A3_1 and A1A3_2 not found!\n");
    assert (0);
    return MSXT_FAIL;
  }

  //1-4) Determine any ms_sheet sharing fine-scale faces with this sheet in the neighboring sheets.
  //     Gather all neighboring sheets sharing curves and nodes as candidate sheets.
  //     Here the use of sheets with incident nodes is required.    
  vcl_set<dbmsh3d_sheet*> MS_shared_F_Sset;
  if (MS->have_shared_Fs())    
    MS->get_incident_Sset_via_C_N (MS_shared_F_Sset);
  
  //1-5) Check if any shared_F should remain after splicing S.
  //     Can't apply splice transform if so.
  for (dbmsh3d_ptr_node* cur = MS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    //Get the set of sheets sharing F.
    vcl_set<dbmsh3d_sheet*> Sset_sharing_F;
    vcl_set<dbmsh3d_sheet*>::iterator it = MS_shared_F_Sset.begin();
    for (; it != MS_shared_F_Sset.end(); it++) {
      dbmsh3d_sheet* S = (*it);
      if (S->is_F_shared (F))
        Sset_sharing_F.insert (S);
    }
    if (Sset_sharing_F.size() > 1) {
      vul_printf (vcl_cout, "Skip xform: F %d shared by 3 or more sheets!\n", F->id());
      return MSXT_FAIL;
    }
  }

  //1-6) Check if each bnd_A13_MC[] is in MS1 and MS2's bnd_chain.
  for (unsigned int i=0; i<bnd_A13_MCs.size(); i++) {
    dbsk3d_ms_curve* MC = bnd_A13_MCs[i];
    dbsk3d_ms_sheet *MS1 = NULL, *MS2 = NULL;

    //Get MS1 and MS2
    vcl_vector<dbmsh3d_face*> MC_incident_Fs;
    MC->get_incident_Fs (MC_incident_Fs);

    if (MC_incident_Fs.size() == 3) {
      for (unsigned int j=0; j<MC_incident_Fs.size(); j++) {        
        if (MC_incident_Fs[j] == MS)
          continue; //Skip the MS itself
        else if (MS1 == NULL)
          MS1 = (dbsk3d_ms_sheet*) MC_incident_Fs[j];
        else if (MS2 == NULL)
          MS2 = (dbsk3d_ms_sheet*) MC_incident_Fs[j];
      }
           
      if (MS1 != NULL) {
        if (MS1->is_E_in_icurves (MC) == false) {
          vul_printf (vcl_cout, "Skip xform: MC %d not in MS %d's bnd-chain!\n", MC->id(), MS1->id());
          return MSXT_FAIL;
        }
      }
      
      if (MS2 != NULL) {
        if (MS2->is_E_in_icurves (MC) == false) {
          vul_printf (vcl_cout, "Skip xform: MC %d not in MS %d's bnd-chain!\n", MC->id(), MS2->id());
          return MSXT_FAIL;
        }
      }
    }    
  }

  //2) Perform the transform.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-1) Pass all generators {G} of MS to bnd_A13_MCs[].
  //2-1-1) Collect all generators of MS from all its A12 faces and 
  //       A3 rib edge elements to {G} and remove them from the shocks.
  vcl_map<int, dbmsh3d_vertex*> MS_asso_G;
  MS->get_asso_Gs (MS_asso_G, true);

  //2-1-2) Collect all generators of MS from all elements in bnd_A13_MCs[] 
  //       to {GA13} but do not remove them from the shocks.
  vcl_map<int, dbmsh3d_vertex*> A13_asso_G;
  for (unsigned int i=0; i<bnd_A13_MCs.size(); i++) {
    dbsk3d_ms_curve* MC = bnd_A13_MCs[i];
    MC->get_asso_Gs_incld_FFs (A13_asso_G);
  }

  //2-1-3) Subtract {GA13} from {G}. The remaining {G} is the generator to pass.
  vcl_map<int, dbmsh3d_vertex*>::iterator gsit = A13_asso_G.begin();
  for (; gsit != A13_asso_G.end(); gsit++) {
    int id = (*gsit).first;
    MS_asso_G.erase (id);
  }

  //2-1-4) Mark all MS's fine-scale faces to be invalid (required here for passing generators.)
  vcl_set<dbmsh3d_face*> FF_to_trim;
  MS->get_F_set (FF_to_trim, true);

  //2-1-5) Assign all {G} to each edge in bnd_A13_MCs[].
  /*if (MS_asso_G.size() > 0) {
    for (unsigned int i=0; i<bnd_A13_MCs.size(); i++) {
      dbsk3d_ms_curve* MC = bnd_A13_MCs[i];
      assert (MC->data_type() == C_DATA_TYPE_EDGE);
      assert (MC->type() == C_TYPE_AXIAL || MC->type() == C_TYPE_DEGE_AXIAL);
      for (unsigned int j=0; j<MC->E_vec().size(); j++) {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec (j);

        vcl_map<int, dbmsh3d_vertex*>::iterator it = MS_asso_G.begin();
        for (; it != MS_asso_G.end(); it++) {
          dbmsh3d_vertex* G = (*it).second;
          //If G is assigned int FE's valid incident FF's, skip.
          if (FE->_is_G_asgn_in_FF (G, NULL)) //fromP=NULL
            continue;
          FE->check_add_asgn_G (G);
        }
      }
    }
  }*/

  //2-2) Modify the object boundary.
  if (modify_bnd) {
  }

  //2-3) Modify the medial scaffold hypergraph topology.

  //2-3-0) Loop through each bnd_A3_MCs[i] and handle shared_E's.  
  vcl_set<dbmsh3d_edge*> bnd_A3_ribs;
  MS->get_bnd_A3ribs (bnd_A3_ribs);

  vcl_set<dbmsh3d_edge*>::iterator rit = bnd_A3_ribs.begin();
  for (; rit != bnd_A3_ribs.end(); rit++) {
    dbsk3d_ms_curve* ribMC = (dbsk3d_ms_curve*) (*rit);
    //Remove shared_E (if any) from ribMC and neighboring MS's
    splice_remove_MC_shared_Es (ribMC, MS, modified_MC_set);
      if (ribMC->have_shared_Es()) {
        vul_printf (vcl_cout, "Error: Can't remove shared_Es at C%d!\n", ribMC->id());
        assert (0);
      }
  }

  //2-3-1) Loop through each bnd_A13_MCs[i] and splice the remaining 2 sheets. 
  vcl_set<dbmsh3d_edge*> splicing_Es;

  for (unsigned int i=0; i<bnd_A13_MCs.size(); i++) {
    dbsk3d_ms_curve* MC = bnd_A13_MCs[i];
    
    //Add the MC.FE[i] to the splicing_Es[].
    for (unsigned int i=0; i<MC->E_vec().size(); i++)
      splicing_Es.insert (MC->E_vec(i));

    vcl_vector<dbmsh3d_face*> MC_incident_Fs;
    MC->get_incident_Fs (MC_incident_Fs);
    
    //2-3-1-1) Determine the two sheets MS1 and MS2 to splice.
    //Skip the splicing of degenerate A1n curves (since it is not required).
    //Skip the splicing if any of MS1 and MS2 is NULL. (Should not happen). This include:
    //  the case MS=MS1 (or equivalently MS2): MS1 is identical to MS.
    //  the case of triple incidence: MS=MS1=MS2.
    dbsk3d_ms_sheet *MS1 = NULL, *MS2 = NULL;
    if (MC_incident_Fs.size() == 3) {
      for (unsigned int j=0; j<MC_incident_Fs.size(); j++) {        
        if (MC_incident_Fs[j] == MS)
          continue; //Skip the MS itself
        else if (MS1 == NULL)
          MS1 = (dbsk3d_ms_sheet*) MC_incident_Fs[j];
        else if (MS2 == NULL)
          MS2 = (dbsk3d_ms_sheet*) MC_incident_Fs[j];
      }
      
      //For the case that one of MS1 or MS2 not NULL.
      //The MS to be pruned and one of MS1 or MS2 are identical.
      //Not a valid splice transform case. Should skip.      
      if (MS1 == NULL || MS2 == NULL)
        continue; //the triple incidence: MS1 == MS2 == NULL.

      //2-3-1-2) If MC has shared E's, remove them if they are shared by only curves of MS, MS1 and MS2.
      //         MC should now contain no shared E's.
      splice_remove_MC_shared_Es (MC, MS, modified_MC_set);
      if (MC->have_shared_Es()) {
        vul_printf (vcl_cout, "Error: Can't remove shared_Es at C%d!\n", MC->id());
        assert (0);
      }

      //Prior to splicing MS1 and MS2 on MC, Remove all transforms involving MC in QT..
      _remove_MC_from_Q (MC);

      if (MS1 == MS2) {         
        //2-3-1-3) If MS1 = MS2, MC is an IC-pair of MS1: 
        //No need to merge sheets MS1 and MS2 (they are the same).
        //Remove the two halfedges of MC from MS1's internal-curve list.
        vul_printf (vcl_cout, "S%d remove I-Curve%d, ", MS1->id(), MC->id());
        MS1->disconnect_icurve_pair_E (MC);
        
        //Remove all transforms involving MS1 in QT.
        _remove_MS_from_Q (MS1); 
        modified_MS_set.insert (MS1);
        continue; 
      }
      else {        
        //2-3-1-4) Splice the two sheets together: merge the smaller to the larger sheet.
        //Determine the large sheet as MS1 to keep (and the smaller one as MS2 to remove) 
        //by e.g., comparing their number of fine-scale face elements.
        if (MS1->facemap().size() < MS2->facemap().size()) {
          dbsk3d_ms_sheet* tmp = MS2;
          MS2 = MS1;
          MS1 = tmp;
        }
        //Assert that MC are in MS2's NA1A3-chain (not in IC-chains).
        bool r = MS2->is_E_in_icurves (MC);
        assert (r == false);

        //remove all transforms involving MS1 and MS2 in QT.
        _remove_MS_from_Q (MS1);
        _remove_MS_from_Q (MS2);
        modified_MS_set.erase (MS2);
        MS_shared_F_Sset.erase (MS2);
        
        //Splice MS1 and MS2 at MC. MC is deleted after the merging.
        MS_splice_ms_sheets (ms_hypg_, MC, MS1, MS2);

        //This is required here for the next step of merging curves.
        MS1->canonicalization ();
        modified_MS_set.insert (MS1);
      }
    }
    else {
      //Dege. MC with > 3 sheets. Add this MC to re-compute its c_type.
      assert (MC_incident_Fs.size() > 3);
      modified_MC_set.insert (MC);
    }
  }

  //2-3-2) Loop through MS's bnd nodes and merge the other 2 ms_curves on other sheets.
  //       This is for A12A3-II sheet-splice transform (not required for A1A3-I sheet splice).
  vcl_vector<dbmsh3d_edge*> MS_bnd_Cs;
  MS->get_bnd_Es (MS_bnd_Cs);
  dbmsh3d_vertex* loop2_prevN = NULL;
  for (int i=0; i<int(MS_bnd_Cs.size()); i++) {
    //2-3-2-1) For each consecutive curve MC1 and MC2 on MS's bnd-chain (MC1 != MC2), 
    //         find the node MN in between and deter determine the two curves MC3 and MC4 to merge (splice).
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
    _remove_MN_from_Q (MN);

    //Detect the other two ms_curves of the A14 MN to merge.
    //If MC1 and MC2 is found, find MC3 and MC4 at N. 
    //Skip if N's number of incident edges is not 4. 
    //Skip if MC3=MC4. 
    //Skip if MC3 and MC4 not sharing same incident faces. 
    //(The checking here might be redundant.)
    dbsk3d_ms_curve* MC3 = NULL;
    dbsk3d_ms_curve* MC4 = NULL;
    //Skip the degenerate A1n MN or a loop MC
    if (MN->A14_get_other_2_MCs (MC1, MC2, MC3, MC4) == false)
      continue; //No need to merge curves for a junction.        
    //Skip merging MC3 and MC4 if they don't have the same incident faces.
    if (same_incident_Fs (MC3, MC4) == false) {
      modified_MN_set.insert (MN);
      continue; 
    }

    //2-3-2-2) Merge MC3 and MC4. MN will be deleted in removing sheet MS.
    //remove all transforms involving MC3 and MC4 from QT.
    _remove_MC_from_Q (MC3);
    _remove_MC_from_Q (MC4);
    modified_MC_set.erase (MC4);

    //Merge MC3 and MC4 at MN.
    ms_hypg_->merge_Cs_sharing_N (MN, MC3, MC4);

    modified_MC_set.insert (MC3);
  }

  //2-3-3) Remove MS from the ms_hypg.

  //2-3-3-1) Check and fix the topology of modified MS (canonicalization).
  vcl_set<dbsk3d_ms_sheet*>::iterator sit = modified_MS_set.begin();
  for (; sit != modified_MS_set.end(); sit++) {
    dbsk3d_ms_sheet* mMS = (*sit);
    mMS->canonicalization ();
  }

  //2-3-3-2) Remove MS's all incident C's shared edges. 
  //         This will remove the shared_E on MS;s swallowtails (if any).
  //         Any remaining shared_E incident to MS will cause problem in deleting it.
  MS->_clear_all_Cs_shared_Es ();

  //2-3-3-3) Determine change of shock type for the two A1A3 nodes.
  //         If any of them will remain after the deletion of MS 
  //         (by checking number of edge-inc. > 2), add them to modified_MN_set and 
  //         later re-compute types for all nodes there.
  if (A1A3_1->n_E_incidence() > 2)
    modified_MN_set.insert (A1A3_1);
  if (A1A3_2->n_E_incidence() > 2)
    modified_MN_set.insert (A1A3_2);

  //2-3-3-4) Remove any shared fine-scale faces from MS_shared_F_Sset[].
  vcl_set<dbmsh3d_face*> shared_F_del;
  for (dbmsh3d_ptr_node* cur = MS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    //Get the set of sheets sharing F.
    vcl_set<dbmsh3d_sheet*> Sset_sharing_F;
    vcl_set<dbmsh3d_sheet*>::iterator it = MS_shared_F_Sset.begin();
    for (; it != MS_shared_F_Sset.end(); it++) {
      dbmsh3d_sheet* S = (*it);
      if (S->is_F_shared (F))
        Sset_sharing_F.insert (S);
    }
    assert (Sset_sharing_F.size() == 1);
    //Remove F from the other sheet.
    it = Sset_sharing_F.begin();
    dbmsh3d_sheet* S = (*it);
    S->del_shared_F (F);
    shared_F_del.insert (F);
  }  
  vcl_set<dbmsh3d_face*>::iterator it = shared_F_del.begin();
  for (; it != shared_F_del.end(); it++) {
    dbmsh3d_face* F = (*it);
    //Remove F from FF_to_trim.
    FF_to_trim.erase (F);
    //Remove F from MS.
    MS->del_shared_F (F);
    MS->facemap().erase (F->id());
  }
  assert (MS->have_shared_Fs () == false); 

  //2-3-3-5) Completely remove the sheet MS from the hypergraph ms_hypg_. 
  //         (Delete all incident curves and nodes which are not incident 
  //         to other elements in the hypergraph.)
  assert (MS->have_icurve_chain() == false);
  vcl_set<dbmsh3d_edge*> remaining_Cs;
  vcl_set<dbmsh3d_vertex*> remaining_Ns;
  ms_hypg_->remove_S_complete_hypg (MS, remaining_Cs, remaining_Ns);
  
  vcl_set<dbmsh3d_edge*> removed_Cs;
  vcl_set<dbmsh3d_vertex*> removed_Ns;
  ms_hypg_->remove_S_complete_fix (remaining_Cs, remaining_Ns, removed_Cs, removed_Ns);

  //Remove all removed C's and N's from modified_MC_set and modified_MN_set.
  vcl_set<dbmsh3d_edge*>::iterator eit = removed_Cs.begin();
  for (; eit != removed_Cs.end(); eit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*eit);
    modified_MC_set.erase (MC);
  }
  vcl_set<dbmsh3d_vertex*>::iterator vit = removed_Ns.begin();
  for (; vit != removed_Ns.end(); vit++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*vit);
    modified_MN_set.erase (MN);
  }

  //2-3-3-6) Remove (and delete) all faces of MS (except all shared_F's).
  perform_trim_xform (ms_hypg_->fs_mesh(), FF_to_trim);
  FF_to_trim.clear();

  //2-3-4) Re-compute shock types in modified_MN_set and modified_MC_set.
  
  //2-3-4-1) Re-compute modified shock node types  
  vcl_set<dbsk3d_ms_node*>::iterator nit = modified_MN_set.begin();
  for (; nit != modified_MN_set.end(); nit++) {
    dbsk3d_ms_node* MN = (*nit);
    MN->compute_n_type ();
  }

  //2-3-4-2) Re-compute shock type of edges in splicing_Es[].  
  eit = splicing_Es.begin();
  for (; eit != splicing_Es.end(); eit++) {
    dbmsh3d_edge* E = (*eit);
    E->compute_e_type ();
  }

  //2-3-4-3) Traverse modified_MC_set and 
  // - compute_c_type ().
  // - set shared_E's type to SHARED.
  // - for A13 MC, set FE of non-A13 topology to be type SPECIAL.
  // - degenerte splice xform: if any E has only one incident S, make this C a rib type!
  //This is required because in the previous step over-writes this flag for shared_E's.
  vcl_set<dbsk3d_ms_curve*>::iterator cit = modified_MC_set.begin();
  for (; cit != modified_MC_set.end(); cit++) {
    dbsk3d_ms_curve* MC = (*cit);
    MC->compute_c_type ();

    for (dbmsh3d_ptr_node* cur = MC->shared_E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      E->set_e_type (E_TYPE_SHARED);
      assert (E->e_type() == E_TYPE_SHARED);
    }
    
    for (unsigned int i=0; i<MC->E_vec().size(); i++) {
      dbmsh3d_edge* E = MC->E_vec(i);
      if (E->e_type() == E_TYPE_SHARED)
        continue;
      //Setup possible 'special' edge type of curve MC.
      int n = E->n_incident_Fs();
      switch (MC->c_type()) {
      case C_TYPE_RIB:
        if (n != 1)
          E->set_e_type (E_TYPE_SPECIAL);
      break;
      case C_TYPE_AXIAL:
        if (n != 3)
          E->set_e_type (E_TYPE_SPECIAL);
      break;
      case C_TYPE_DEGE_AXIAL:
        if (n != 4)
          E->set_e_type (E_TYPE_SPECIAL);
      break;
      default:
        assert (0);
      break;
      }
    }
  }

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "Splice integrity error! ");
    assert (0);
  }

  ///assert (fs_mesh()->check_all_FFs_valid());  
  //Debug the bnd-shock association
  //vcl_vector<dbmsh3d_vertex*> unasgn_genes;
  //ms_hypg->fs_mesh()->check_all_G_asgn (unasgn_genes);

  return MSXT_SUCCESS;
}

//############################################################################
//  A5 curve contract transform functions 
//############################################################################

//: A5 Contract transform on ms_curve (and neighboring swallow-tail.
//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A5_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A5_curve_contract_xform() on C %d: \n\t", MC->id());

  //1) Determine if this transform is valid.

  //1-2) Setup configuration of the swallow-tail: 2 A1A3 nodes Ns and Ne 
  //   as the starting/ending vertices of MC.
  dbsk3d_ms_node* Ns = MC->s_MN();
  dbsk3d_ms_node* Ne = MC->e_MN();

  //1-3) Skip xform if NS or NE not of type A1A3 (this ensures MC to be A13).
  if (Ns->n_type() != N_TYPE_RIB_END) {
    vul_printf (vcl_cout, "Skip xform: S %d not A1A3 type!\n", Ns->id());
    return MSXT_SKIP;
  }
  if (Ne->n_type() != N_TYPE_RIB_END) {
    vul_printf (vcl_cout, "Skip xform: E %d not A1A3 type!\n", Ne->id());
    return MSXT_SKIP;
  }
  //1-4) Skip xform if all edges of MC are shared Es. (unlikely)
  if (MC->all_Es_shared()) {
    vul_printf (vcl_cout, "Skip xform: Dege. case: all edges are shared_Es.!\n");
    return MSXT_SKIP;
  }

  //1-5) Setup  configuration: curves CS, CE, sheet S.
  assert (Ns->n_incident_Es() == 2);
  assert (Ne->n_incident_Es() == 2);
  dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) V_find_other_E (Ns, MC);
  dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) V_find_other_E (Ne, MC);

  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();
  assert (MC->halfedge()->pair()->face() == MS);
  assert (MC->halfedge()->pair()->pair()->face() == MS);

  //1-6) Find a valid trimming path, detect the trimming point M on MC and 
  //     the set of fine-scale faces to trim to remove the A5 swallow-tail 3-incidence on MC).
  dbsk3d_fs_vertex* M = NULL;
  vcl_set<dbmsh3d_face*> FF_to_trim;

  if (A5_valid_trim_M_U_V (MC, M, FF_to_trim) == false) {
    vul_printf (vcl_cout, "Fail: can't find valid trim path!\n");
    return MSXT_FAIL; //1-7) Skip xform if such trimming path can't be found.
  }

  //1-4) Determine any ms_sheet sharing fine-scale faces with this sheet in the neighboring sheets.
  //     Gather all neighboring sheets sharing curves and nodes as candidate sheets.
  //     Here the use of sheets with incident nodes is required.    
  vcl_set<dbmsh3d_sheet*> MS_shared_F_Sset;
  if (MS->have_shared_Fs())    
    MS->get_incident_Sset_via_C_N (MS_shared_F_Sset);

  //1-5) Check if any shared_F in FF_to_trim[] should remain after the trimming.
  //     Can't apply splice transform if so.
  //1-6) Skip xform if FF_to_trim contains shared_F.
  for (dbmsh3d_ptr_node* cur = MS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    if (FF_to_trim.find(F) == FF_to_trim.end())
      continue; //Skip F not in FF_to_trim[].

    //Skip this transform, since we're not sure if the swallow-tail topology can be removed.    
    vul_printf (vcl_cout, "Fail: can't F %d shared by other S!\n", F->id());
    return MSXT_FAIL;
  }

  //2) Perform the transform:

  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //2-2) Transform the MS scaffold hypergraph topology.

  //2-2-0) Remove any shared fine-scale faces in FF_to_trim[] from MS_shared_F_Sset[].
  vcl_set<dbmsh3d_face*> shared_F_del;
  for (dbmsh3d_ptr_node* cur = MS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    if (FF_to_trim.find(F) == FF_to_trim.end())
      continue; //Skip F not in FF_to_trim[].

    //Get the set of sheets sharing F.
    vcl_set<dbmsh3d_sheet*> Sset_sharing_F;
    vcl_set<dbmsh3d_sheet*>::iterator it = MS_shared_F_Sset.begin();
    for (; it != MS_shared_F_Sset.end(); it++) {
      dbmsh3d_sheet* S = (*it);
      if (S->is_F_shared (F))
        Sset_sharing_F.insert (S);
    }
    assert (Sset_sharing_F.size() == 1);
    //Remove F from the other sheet.
    it = Sset_sharing_F.begin();
    dbmsh3d_sheet* S = (*it);
    S->del_shared_F (F);
    shared_F_del.insert (F);
  }
  vcl_set<dbmsh3d_face*>::iterator it = shared_F_del.begin();
  for (; it != shared_F_del.end(); it++) {
    dbmsh3d_face* F = (*it);
    //Remove F from FF_to_trim.
    FF_to_trim.erase (F);
    //Remove F from MS.
    MS->del_shared_F (F);
    MS->facemap().erase (F->id());
  }

  //2-2-1) Trim the fine-scale faces in FF_to_trim[] to remove the swallow-tail.
  perform_trim_xform (ms_hypg_->fs_mesh(), MS, FF_to_trim);

  //2-2-2) Merge two A3 rib curves Cs and Ce.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;
  dbsk3d_ms_node* Nss = (dbsk3d_ms_node*) Cs->other_V (Ns);
  dbsk3d_ms_node* Nee = (dbsk3d_ms_node*) Ce->other_V (Ne);
  if (Cs == Ce) {    
    //2-2-2-1) If the A3 rib is a loop, make M as the loop_end node.
    A5_contract_merge_rib_loop (ms_hypg_, MS, Cs, M);
  }
  else {
    //2-2-2-2) Merge the 2 A3 ribs into a single curve passing through M.
    dbsk3d_ms_curve* merged_MC = A5_contract_merge_ribs (ms_hypg_, MS, Nss, Nee, Cs, Ce);
    modified_MC_set.insert (merged_MC);
  }
  
  //Remove related MS, MC, and MN from queue Q.
  _remove_MS_from_Q (MS);
  _remove_MC_from_Q (Cs);
  _remove_MC_from_Q (Ce);
  _remove_MN_from_Q (Ns);
  _remove_MN_from_Q (Ne);
  
  //2-2-3) Delete MC, delete shock nodes Ns and Ne.
  MS->disconnect_bnd3_E (MC);  
  ms_hypg_->remove_edge (MC);
  ms_hypg_->remove_vertex (Ns);
  ms_hypg_->remove_vertex (Ne);
  
  //2-2-4) Check and fix the topology of the resulting MS. 
  MS->canonicalization();
  modified_MS_set.insert (MS);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///assert (fs_mesh()->check_all_FFs_valid());

  return MSXT_SUCCESS;
}

//############################################################################
//  A12A3-I curve contract transform functions 
//############################################################################

//: A12A3-I curve-contract transform.
//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A12A3I_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A12A3I_curve_contract_xform() on C %d: \n\t", MC->id());

  //1) Determine if this transform is valid:

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
    if (NA1A3->n_type() != N_TYPE_RIB_END) {
      vul_printf (vcl_cout, "Skip xform: NA1A4 %d type not allowed for xform()!\n", NA1A3->id());
      return MSXT_SKIP;
    }
  }
  if (NA14->n_type() != N_TYPE_AXIAL_END && 
      NA14->n_type() != N_TYPE_DEGE_RIB_END && 
      NA14->n_type() != N_TYPE_DEGE_AXIAL_END) {
    vul_printf (vcl_cout, "Skip xform: NA14 %d type not allowed for xform()!\n", NA14->id());
    return MSXT_SKIP;
  }
  
  //1-2) Detect the tabS, baseS, ribC.
  dbmsh3d_sheet *tabS, *baseS;
  bool r = C_get_non_dup_S (MC, tabS, baseS);
  if (r == false) {
    vul_printf (vcl_cout, "Skip xform: tabS and baseS not found.\n");
    return MSXT_SKIP;
  }
  dbsk3d_ms_sheet *tabMS = (dbsk3d_ms_sheet*) tabS;
  dbsk3d_ms_sheet *baseMS = (dbsk3d_ms_sheet*) baseS;

  dbsk3d_ms_curve* ribMC = (dbsk3d_ms_curve*) tabMS->find_next_bnd_C (NA1A3, MC);
  otherN = (dbsk3d_ms_node*) ribMC->other_V (NA1A3);  

  if (otherN == NA14) {
    vul_printf (vcl_cout, "Skip xform: NA14 = otherN %d.\n", NA14->id());
    return MSXT_SKIP;
  }
  
  //1-3) Determine the other axial medial curve aMC and the fine-scale face MF.
  dbmsh3d_curve* otherC;
  dbmsh3d_face* F_M;
  r = tabMS->get_otherC_via_F (MC, NA14, otherC, F_M);  
  if (r == false) {
    vul_printf (vcl_cout, "Can't find valid aMC (NULL)!\n");
    return MSXT_FAIL;
  }
  dbsk3d_ms_curve* aMC = (dbsk3d_ms_curve*) otherC;

  //1-4) If MC has shared_E with ms_curves other than ribMC and axialMC, skip xform.
  if (MC->have_shared_Es()) {
    vcl_set<dbmsh3d_curve*> shared_E_Cset;
    shared_E_Cset.insert (ribMC);
    shared_E_Cset.insert (aMC);
    if (MC->shared_E_with_Cset (shared_E_Cset) == false) {
      vul_printf (vcl_cout, "Skip xform: C %d has shared_Es other than ribC %d and aC %d.\n", 
                  MC->id(), ribMC->id(), aMC->id());
      return MSXT_SKIP;
    }

    //Remove the shared_Es by MC and ribMC.
    remove_Cs_sharing_E (MC, ribMC);
    //The shared_E's of (MC, aMC) should be trasferred to (ribMC, aMC), or simply de-couple them!
    remove_Cs_sharing_E (MC, aMC);
  }
  assert (MC->have_shared_Es() == false);

  //1-5) Find a valid trimming path from NA14 to a vertex U on ribMC.
  dbmsh3d_vertex *M = NULL, *U = NULL;
  bool M_on_aMC;  
  vcl_vector<dbmsh3d_edge*> E_aMC;
  vcl_vector<dbmsh3d_edge*> UM_Evec; //trimming path (edges) from U to M.
  vcl_set<dbmsh3d_face*> FF_to_trim;

  r = A12A3I_valid_trim_M_U (tabMS, MC, ribMC, aMC, NA14, NA1A3, F_M, 
                             M, U, M_on_aMC, E_aMC, UM_Evec, FF_to_trim);
  if (r == false) {
    vul_printf (vcl_cout, "Can't find valid trim path!\n");
    return MSXT_FAIL;
  }
  
  //1-6) Determine any ms_sheet sharing fine-scale faces with this sheet in the neighboring sheets.
  //     Gather all neighboring sheets sharing curves and nodes as candidate sheets.
  //     Here the use of sheets with incident nodes is required.    
  vcl_set<dbmsh3d_sheet*> MS_shared_F_Sset;
  if (tabMS->have_shared_Fs())    
    tabMS->get_incident_Sset_via_C_N (MS_shared_F_Sset);

  //1-7) Check if any shared_F in FF_to_trim[] should remain after the trimming.
  //     Can't apply splice transform if so.
  for (dbmsh3d_ptr_node* cur = tabMS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    if (FF_to_trim.find(F) == FF_to_trim.end())
      continue; //Skip F not in FF_to_trim[].

    //Skip this transform, since we're not sure if the swallow-tail topology can be removed.    
    vul_printf (vcl_cout, "Fail: can't F %d shared by other S!\n", F->id());
    return MSXT_FAIL;
  }

  //2) Perform the transform.

  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //Remove the shared_MC of (ribMC, MC) and (MC, aMC).
  remove_Cs_sharing_E (MC, ribMC);
  remove_Cs_sharing_E (MC, aMC);

  //Remove related MS, MC, and MN from queue Q.
  _remove_MS_from_Q (tabMS);
  _remove_MS_from_Q (baseMS);
  _remove_MC_from_Q (MC);
  _remove_MC_from_Q (aMC);
  _remove_MN_from_Q (NA1A3);
  _remove_MN_from_Q (NA14);

  //2-2) Remove any shared fine-scale faces in FF_to_trim[] from MS_shared_F_Sset[].
  vcl_set<dbmsh3d_face*> shared_F_del;
  for (dbmsh3d_ptr_node* cur = tabMS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    if (FF_to_trim.find(F) == FF_to_trim.end())
      continue; //Skip F not in FF_to_trim[].

    //Get the set of sheets sharing F.
    vcl_set<dbmsh3d_sheet*> Sset_sharing_F;
    vcl_set<dbmsh3d_sheet*>::iterator it = MS_shared_F_Sset.begin();
    for (; it != MS_shared_F_Sset.end(); it++) {
      dbmsh3d_sheet* S = (*it);
      if (S->is_F_shared (F))
        Sset_sharing_F.insert (S);
    }
    assert (Sset_sharing_F.size() == 1);
    //Remove F from the other sheet.
    it = Sset_sharing_F.begin();
    dbmsh3d_sheet* S = (*it);
    S->del_shared_F (F);
    shared_F_del.insert (F);
  }
  vcl_set<dbmsh3d_face*>::iterator it = shared_F_del.begin();
  for (; it != shared_F_del.end(); it++) {
    dbmsh3d_face* F = (*it);
    //Remove F from FF_to_trim.
    FF_to_trim.erase (F);
    //Remove F from MS.
    tabMS->del_shared_F (F);
    tabMS->facemap().erase (F->id());
  }

  //2-3) Trim fine-scale elements to remove the MC.
  perform_trim_xform (ms_hypg_->fs_mesh(), tabMS, FF_to_trim);

  //Determine the MA_Evec[]. Two cases.
  vcl_vector<dbmsh3d_edge*> E_to_del;
  vcl_vector<dbmsh3d_vertex*> V_to_del;
  vcl_vector<dbmsh3d_edge*> MA_Evec;
  if (M_on_aMC == false) {
    //2-3-1) trim the F_M from M to NA14 and keep the side of edge EaMC.
    dbmsh3d_edge* EaMC = aMC->get_E_incident_N (NA14);
    dbsk3d_fs_face* FF_M = (dbsk3d_fs_face*) F_M;
    dbmsh3d_edge* lastE = trim_FF (FF_M, NA14->V(), EaMC, M, true, E_to_del, V_to_del);
    //MA_Evec[] contains a single edge lastE.
    MA_Evec.push_back (lastE);
  }
  else {    
    //2-3-2) MA_Evec[] is the reverse of E_aMC[].
    for (int i=int(E_aMC.size())-1; i>=0; i--)
      MA_Evec.push_back (E_aMC[i]);
  }

  //2-4) Trace ribMC to contract the MC.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  A12A3I_contract_trace_rib (ms_hypg_, tabMS, baseMS, MC, ribMC, aMC, 
                             M, M_on_aMC, MA_Evec, NA14, otherN);
  modified_MS_set.insert (tabMS);
  modified_MC_set.insert (ribMC);
  modified_MC_set.insert (aMC);
  modified_MN_set.insert (NA14);

  //2-5) Delete E_to_del[] and V_to_del[] from fs_mesh.

  //2-6) Delete shock vertiex NA1A3.
  ms_hypg_->remove_vertex (NA1A3);

  //2-7) Check and fix the topology of resulting tabMS and baseMS. 
  tabMS->canonicalization();
  baseMS->canonicalization();
  modified_MS_set.insert (baseMS);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  ///assert (fs_mesh()->check_all_FFs_valid());

  return MSXT_SUCCESS;
}

//############################################################################}
//  A15 curve-contract transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A15_curve_contract_xform (dbsk3d_ms_curve* MC, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A15_curve_contract_xform() on C %d: \n\t", MC->id());

  //1) Determine if this transform is valid.
  bool init_check = true;
  if (MC->c_type() != C_TYPE_AXIAL && MC->c_type() != C_TYPE_DEGE_AXIAL)
    init_check = false;
 
  //1-2) MC with shared_E (hybrid xforms) not handled yet.
  //     Should perform transform if all shared_Es are in the C's to merge.
  if (MC->have_shared_Es())
    init_check = false;

  if (MC->s_MN()->n_type() == N_TYPE_RIB_END || MC->e_MN()->n_type() == V_TYPE_RIB_END)
    init_check = false;
  if (MC->s_MN()->has_rib_C())
    init_check = false;
  if (MC->e_MN()->has_rib_C())
    init_check = false;

  if (init_check == false) {
    vul_printf (vcl_cout, "Skip xform: initial check on C %d, C_sN %d, C_eN %d not pass.\n", 
                MC->id(), MC->s_MN()->id(), MC->e_MN()->id());
    return MSXT_SKIP;
  }

  //1-3) Go through each incident MS of MC and check Cs and Ce is of type axial.
  //     Also check Cs and Ce is not on MS's icurve-pair and decide
  //     possible valid merges at either Ns or Ne.
  dbmsh3d_vertex* Ns = MC->s_MN();
  dbmsh3d_vertex* Ne = MC->e_MN();
  vcl_set<dbsk3d_ms_sheet*> MS_merge_set;
  vcl_set<dbmsh3d_edge*> Cs_merge_set;
  vcl_set<dbmsh3d_edge*> Ce_merge_set;

  dbmsh3d_halfedge* HE = MC->halfedge();
  assert (is_HE_pair_3p_inc (HE)); //the pair loop contains >=3 halfedges with the same edge.
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
    int n = MS->n_bnd_Es();
    if (n < 3) {
      vul_printf (vcl_cout, "Skip xform: S %d has only %d bnd curves.\n", MS->id(), n);
      return MSXT_SKIP; //Avoiding leaving MS with 1 boundary MC.
    }
    
    //1-3-1) Determine the other curve Cs and Ce to merge.
    dbmsh3d_curve* otherC;
    dbmsh3d_face* MF;
    bool r = MS->get_otherC_via_F (MC, MC->s_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) otherC;

    r = MS->get_otherC_via_F (MC, MC->e_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) otherC;

    assert (Cs->c_type() == C_TYPE_AXIAL || Cs->c_type() == C_TYPE_DEGE_AXIAL);
    assert (Ce->c_type() == C_TYPE_AXIAL || Ce->c_type() == C_TYPE_DEGE_AXIAL);
    assert (MS->is_E_in_icurve_pair (MC) == false);

    if (MS->is_E_in_icurve_pair (Cs))
      Ns = NULL;
    if (MS->is_E_in_icurve_pair (Ce))
      Ne = NULL;

    MS_merge_set.insert (MS);
    Cs_merge_set.insert (Cs);
    Ce_merge_set.insert (Ce);

    HE = HE->pair();
  }
  while (HE != MC->halfedge() && HE != NULL);
  assert (MS_merge_set.size() == MC->n_incident_Fs());

  Cs_merge_set.insert (MC);
  Ce_merge_set.insert (MC);

  //1-3-2) If MC.S has any incident curve not in Cs_merge_set, not a valid xform at MC.S.
  if (MC->s_MN()->all_incident_Es_in_set (Cs_merge_set) == false)
    Ns = NULL;

  //1-3-3) If MC.E has any incident curve not in Ce_merge_set, not a valid xform at MC.E.
  if (MC->e_MN()->all_incident_Es_in_set (Ce_merge_set) == false)
    Ne = NULL;

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
      vul_printf (vcl_cout, "Skip xform: both start/end vertices not valid for xform!\n");
      return MSXT_SKIP; //no valid xform for merging curves in either ends.
    }
  }
  dbsk3d_ms_node* MN_final = (dbsk3d_ms_node*) MC->other_V (MN_del);
  
  //2) Perform the transform.
  
  //Modify shape boundary.
  if (modify_bnd) {
  }

  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-1) Setup local configuration to perform the A15-c-c-xform on each incident MS.
  //MC_merge_set[] is the set of curves to merge with MC.
  vcl_set<dbsk3d_ms_curve*> MC_merge_set;
  vcl_set<dbsk3d_ms_sheet*>::iterator sit = MS_merge_set.begin();
  for (; sit != MS_merge_set.end(); sit++) {
    dbsk3d_ms_sheet* MS = (*sit);

    dbmsh3d_curve* otherC;
    dbmsh3d_face* MF;
    bool r = MS->get_otherC_via_F (MC, MC->s_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) otherC;

    r = MS->get_otherC_via_F (MC, MC->e_MN(), otherC, MF);
    assert (r);
    dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) otherC;

    assert (Cs->c_type() == C_TYPE_AXIAL || Cs->c_type() == C_TYPE_DEGE_AXIAL);
    assert (Ce->c_type() == C_TYPE_AXIAL || Ce->c_type() == C_TYPE_DEGE_AXIAL);
    assert (MS->is_E_in_icurve_pair (MC) == false);

    _remove_MS_from_Q (MS);
    modified_MS_set.insert (MS);
    MS->disconnect_bnd_E (MC); //Disconnect MC from MS.

    if (MN_del == MC->s_MN()) { //2-1-1) Merge MC to Cs at Ns.
      assert (MS->is_E_in_icurve_pair (Cs) == false);      
      MC_merge_set.insert (Cs); //Put Cs into MC_merge_set.
    }
    else { //2-1-2) Merge MC to Ce at Ne.
      assert (MN_del == MC->e_MN());
      assert (MS->is_E_in_icurve_pair (Ce) == false);      
      MC_merge_set.insert (Ce); //Put Ce into MC_merge_set.
    }
  }

  //assert the shared_E of MC is shared by 2+ MC_merged's.
  assert (MC_merge_set.size() > 1);
  //Assert MC has no incident MS and ready to be merged.
  assert (MC->halfedge() == NULL);

  //2-2) Go through the MC_merge_set and merge MC with each entry.
  vcl_set<dbsk3d_ms_curve*>::iterator it = MC_merge_set.begin();
  for (; it != MC_merge_set.end(); it++) {
    dbsk3d_ms_curve* MC_merge = (*it);
    vul_printf (vcl_cout, "connect C%d to N%d, ", MC_merge->id(), MN_final->id());

    //Merge MC to MC_merge and make each MC.E_vec[i] shared_E in MC_merge.
    merge_C1_C2_Es (MC_merge, MC, MN_del, true);

    //Disconnect MC_merge from MN_del.
    int idx = MC_merge->_disconnect_V (MN_del);
    assert (idx != -1);

    //Connect MC_merge to to MN_final.
    if (MC_merge->is_V_incident (MN_final)) //resulting MC_merge as a loop.
      MC_merge->_set_vertex (idx, MN_final);
    else
      MC_merge->connect_V (idx, MN_final);
    _remove_MC_from_Q (MC_merge);
    modified_MC_set.insert (MC_merge);

    //Put all incident sheet of MC_merge to modified_MS_set.
    dbmsh3d_halfedge* HE = MC_merge->halfedge();
    do {
      dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
      modified_MS_set.insert (MS);
      HE = HE->pair();
    }
    while (HE != MC_merge->halfedge() && HE != NULL);
  }

  //2-3) Disconnect MC from both ending nodes and delete MC.
  vul_printf (vcl_cout, "remove C%d, ", MC->id());
  _remove_MC_from_Q (MC);
  ms_hypg_->remove_edge (MC);
  
  //Delete the MN_del.
  vul_printf (vcl_cout, "remove N%d, ", MN_del->id());
  assert (MN_del->n_incident_Es() == 0);
  _remove_MN_from_Q (MN_del);
  ms_hypg_->remove_vertex (MN_del);  

  //In many cases, MN_final should be dege. but in some case it can be A14.
  MN_final->compute_n_type ();
  _remove_MN_from_Q (MN_final);
  modified_MN_set.insert (MN_final);

  //Check and fix the topology of modified MS.  
  sit = modified_MS_set.begin();
  for (; sit != modified_MS_set.end(); sit++) {
    dbsk3d_ms_sheet* mMS = (*sit);    
    mMS->canonicalization (); //Fix topology of mMS.
  }

  //Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);
      
  //Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();

  return MSXT_SUCCESS;
}

//############################################################################
//  A15 curve-contract transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A15_sheet_contract_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A15_sheet_contract_xform() on S %d: \n\t", MS->id()); 

  //1) Determine if this transform is valid.
  assert (MS->have_icurve_chain() == false); //MS can not have i-curve chain.

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
  if (count != 3) { //The A15-sheet-contract is defined only on ms_sheet with 3 bnd curves.
    vul_printf (vcl_cout, "Skip xform: S %d has %d bnd curves.\n", MS->id(), count);
    return MSXT_SKIP;
  }
  
  //1-3) MS's incident N can not have incident A3 ribs.
  vcl_set<dbsk3d_ms_node*>::iterator nit = MNset.begin();
  for (; nit != MNset.end(); nit++) {
    dbsk3d_ms_node* MN = (*nit);
    if (MN->has_rib_C()) {
      vul_printf (vcl_cout, "Skip xform: N %d has incident ribC.\n", MN->id());
      return MSXT_SKIP;
    }
  }

  //Determine the ms_curve MCd to delete.
  dbsk3d_ms_curve* MCd = (dbsk3d_ms_curve*) MS->halfedge()->edge();

  //Determine the two merging node MNp and MNq from MC.
  dbsk3d_ms_node* MNp = MCd->s_MN();
  dbsk3d_ms_node* MNq = MCd->e_MN();

  //1-4) Skip transform if MNp and MNq have diff. # of incident C's.
  if (MNp->n_E_incidence() != MNq->n_E_incidence()) {
    vul_printf (vcl_cout, "Skip xform: Np %d and Nq %d have diff. # incident C's.\n", 
                MNp->id(), MNq->id());
    return MSXT_SKIP;
  }

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
    if (MCpm == NULL) {
      vul_printf (vcl_cout, "Si %d, Cp %d, Np %d: Can't find valid Cpm (NULL)!\n", 
                  MSi->id(), MCp->id(), MNp->id());
      return MSXT_FAIL;
    }

    //Skip if MCp and MCpm is a loop, this will result in a loop curve.
    bool loop2;
    Es_sharing_V_check (MCp, MCpm, loop2);
    if (loop2) {
      vul_printf (vcl_cout, "Skip xform: Cp %d and Cpm %d is a loop!\n", 
                  MCp->id(), MCpm->id());
      return MSXT_FAIL;
    }

    MCpm_vec.push_back (MCpm);
    assert (MCpm != MCd && MCpm != MCp);

    dbsk3d_ms_curve* MCqm = (dbsk3d_ms_curve*) MSi->get_1st_other_C_via_F2 (MCd, MNq, FFqm_set_visited);
    if (MCqm == NULL) {
      vul_printf (vcl_cout, "Si %d, Cq %d, Nq %d: Can't find valid Cqm (NULL)!\n", 
                  MSi->id(), MCq->id(), MNq->id());
      return MSXT_FAIL;
    }

    //Skip if MCq and MCqm is a loop, this will result in a loop curve.
    Es_sharing_V_check (MCq, MCqm, loop2);
    if (loop2) {
      vul_printf (vcl_cout, "Skip xform: Cq %d and Cqm %d is a loop!\n", 
                  MCq->id(), MCqm->id());
      return MSXT_FAIL;
    }

    MCqm_vec.push_back (MCqm);
    assert (MCqm != MCd && MCqm != MCp);

    if (MCpm == MCqm) {
      vul_printf (vcl_cout, "Skip xform: MCpm == MCqm %d.\n", MCpm->id());
      return MSXT_SKIP;
    }
  }

  //1-9) If MCd has shared_E[], check if they are only with MCpm[] and MCqm[].
  if (MCd->have_shared_Es()) {
    vcl_set<dbmsh3d_curve*> shared_E_Cset;
    for (unsigned int i=0; i<MCpm_vec.size(); i++)
      shared_E_Cset.insert (MCpm_vec[i]);
    for (unsigned int i=0; i<MCqm_vec.size(); i++)
      shared_E_Cset.insert (MCqm_vec[i]);

    if (MCd->shared_E_with_Cset (shared_E_Cset) == false) {
      vul_printf (vcl_cout, "Skip xform: Cd %d has non-local shared_E[].\n", MCd->id());
      return MSXT_SKIP;
    }
  }

  //1-10) If any shared_F of MS not shared by MS_merge_set[], skip xform.
  if (MS->shared_F_with_Sset (MS_merge_set) == false) {
    vul_printf (vcl_cout, "Skip xform: S %d has shared_F not in the merging sheet set.\n", MS->id());
    return MSXT_SKIP;
  }

  //1-11) If MCpm_vec[] has repeated entry, not valid xform.
  for (int i=0; i<int(MCpm_vec.size())-1; i++)
    for (int j=1; j<int(MCpm_vec.size()); j++)
      if (MCpm_vec[i] == MCpm_vec[j]) {
        vul_printf (vcl_cout, "Skip xform: MCpm_vec[%d] %d == MCpm_vec[%d] %d.\n", 
                    i, MCpm_vec[i]->id(), j, MCpm_vec[j]->id());
        return MSXT_SKIP;
      }

  //1-12) If MCqm_vec[] has repeated entry, not valid xform. Only check the regular 2-entry case.
  for (int i=0; i<int(MCqm_vec.size())-1; i++)
    for (int j=1; j<int(MCqm_vec.size()); j++)
      if (MCqm_vec[i] == MCqm_vec[j]) {
        vul_printf (vcl_cout, "Skip xform: MCqm_vec[%d] %d == MCqm_vec[%d] %d.\n", 
                    i, MCqm_vec[i]->id(), j, MCqm_vec[j]->id());
        return MSXT_SKIP;
      }


  //1-13) Skip transform if # of MNp.inc_E != MCpm_vec[]+2.
  if (MNp->n_E_incidence() != MCpm_vec.size()+2) {
    vul_printf (vcl_cout, "Skip xform: Np %d # incident C's %d != MCpm_vec[]+2.\n", 
                MNp->id(), MCpm_vec.size()+2);
    return MSXT_SKIP;
  }
  
  //1-14) Skip transform if # of MNq.inc_E != MCqm_vec[]+2.
  if (MNq->n_E_incidence() != MCqm_vec.size()+2) {
    vul_printf (vcl_cout, "Skip xform: Nq %d # incident C's %d != MCqm_vec[]+2.\n", 
                MNq->id(), MCqm_vec.size()+2);
    return MSXT_SKIP;
  }

  //2) Perform the transform.

  //Modify shape boundary.
  if (modify_bnd) {
  }

  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  assert (MS_merge_HE.size() == MCpm_vec.size());
  assert (MS_merge_HE.size() == MCqm_vec.size());
  
  _remove_MC_from_Q (MCd);
  _remove_MC_from_Q (MCp);
  _remove_MC_from_Q (MCq);
  _remove_MN_from_Q (MNp);
  _remove_MN_from_Q (MNq);
  _remove_MN_from_Q (MNA15);

  //2-1) Go through each ms_sheet in MS_merge.
  assert (MS_merge_HE.size() > 1);
  for (unsigned int i=0; i<MS_merge_HE.size(); i++) {
    dbsk3d_ms_sheet* MSi = (dbsk3d_ms_sheet*) MS_merge_HE[i]->face();
    dbsk3d_ms_curve* MCpm = MCpm_vec[i];
    dbsk3d_ms_curve* MCqm = MCqm_vec[i];

    //2-1-1) Merge MCp to MCpm at p.    
    vul_printf (vcl_cout, "merge C%d to C%d at N%d, ", MCp->id(), MCpm->id(), MNp->id());
    _merge_E_vec_C2_to_C1 (MNp, MCpm, MCp);
    MCp->check_add_all_Es_to_C (MCpm);

    _remove_MC_from_Q (MCpm);
    modified_MC_set.insert (MCpm);

    if (MCpm->s_MN() == MNp)
      MCpm->_set_vertex (0, MNA15);
    else {
      assert (MCpm->e_MN() == MNp);
      MCpm->_set_vertex (1, MNA15);
    }
    MNp->del_incident_E (MCpm);
    MNA15->add_incident_E (MCpm);
    
    //2-1-2) Merge MCq to MCqm at q.
    vul_printf (vcl_cout, "merge C%d to C%d at N%d, ", MCq->id(), MCqm->id(), MNq->id());
    _merge_E_vec_C2_to_C1 (MNq, MCqm, MCq);
    MCq->check_add_all_Es_to_C (MCqm);

    _remove_MC_from_Q (MCqm);
    modified_MC_set.insert (MCqm);

    if (MCqm->s_MN() == MNq)
      MCqm->_set_vertex (0, MNA15);
    else {
      assert (MCqm->e_MN() == MNq);
      MCqm->_set_vertex (1, MNA15);
    }
    MNq->del_incident_E (MCqm);
    MNA15->check_add_incident_E (MCqm); //add_incident_E, possible MCpm = MCqm.
  }

  //2-2) Go through MS_merge_set to insert MS.FF to it (as a shared Fs).
  vcl_set<dbmsh3d_sheet*>::iterator it = MS_merge_set.begin();
  for (; it != MS_merge_set.end(); it++) {
    dbsk3d_ms_sheet* MSi = (dbsk3d_ms_sheet*) (*it);
    //Merge MS.FF into MSi. Each FF is shared by 2 or more MS[i].
    vul_printf (vcl_cout, "merge S%d to S%d at C%d, ", MS->id(), MSi->id(), MCd->id());
    vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbmsh3d_face* F = (*it).second;
      MSi->add_F (F);
      MSi->check_add_shared_F (F); //add_shared_F
    }
    _remove_MS_from_Q (MSi);
    modified_MS_set.insert (MSi);
  }

  //2-3) Delete MCp and MCq.
  //Clear all E_vec and shared_E of MCp and MCq.
  MCp->clear_shared_E_list();
  MCq->clear_shared_E_list();
  MCp->_clear_E_vec();
  MCq->_clear_E_vec();

  //Disconnect MCp and MCq from all incident ms_sheets.
  vcl_set<dbmsh3d_face*> disconn_faces;
  MCp->_disconnect_all_Fs (disconn_faces);
  //assert all faces incident to MCp are incident to MCpm.  
  disconn_faces.clear();
  MCq->_disconnect_all_Fs (disconn_faces);
  //assert all faces incident to MCq are incident to MCqm.
  
  ms_hypg_->remove_edge (MCp);
  ms_hypg_->remove_edge (MCq);

  //2-4) Disconnect MS from all incident ms_curves and delete it.
  //Clear all FF's and shared_F's from the MS.
  MS->facemap().clear();
  MS->clear_shared_F_list ();
  ms_hypg_->remove_sheet (MS);

  //2-5) Disconnect MCd from all incident ms_sheets and delete it.
  MCd->_disconnect_all_Fs ();
  ms_hypg_->remove_edge (MCd);

  //2-6) Delete the ms_nodes MNp and MNq.
  assert (MNp->have_incident_Es() == false);
  ms_hypg_->remove_vertex (MNp);
  assert (MNq->have_incident_Es() == false);
  ms_hypg_->remove_vertex (MNq);

  //Determine the final A15 node type (dege.).
  MNA15->compute_n_type (); ///set_n_type (N_TYPE_DEGE);
  modified_MN_set.insert (MNA15);

  //Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);
      
  //Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  return MSXT_SUCCESS;
}

//############################################################################
//  A14 curve-contract transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A14_sheet_contract_xform (dbsk3d_ms_sheet* MS, const bool modify_bnd)
{
  //0) Should skip all A14 s-c-transform to avoid v-conn fine-scale components? !!
  return MSXT_SKIP; 

  vul_printf (vcl_cout, "\n  A14_sheet_contract_xform() on S %d: \n\t", MS->id());
  assert (MS->have_icurve_chain() == false); //MS can not have i-curve chain.

  //1) Determine if this transform is valid.
  //1-1) MS can not have incident A3 ribs.
  //Determine the two ms_curves MCp and MCq of MS.
  dbsk3d_ms_curve *MCp = NULL, *MCq = NULL;
  vcl_set<dbsk3d_ms_node*> MNset;
  int n_bnd = MS->n_bnd_Es();
  if (n_bnd != 2) {
    vul_printf (vcl_cout, "Skip xform: S %d has %d bnd curves!\n", MS->id(), n_bnd);
    return MSXT_SKIP;
  }
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

  //1-2) MS's incident N can not have incident A3 ribs.
  vcl_set<dbsk3d_ms_node*>::iterator nit = MNset.begin();
  for (; nit != MNset.end(); nit++) {
    dbsk3d_ms_node* MN = (*nit);
    if (MN->has_rib_C()) {
      vul_printf (vcl_cout, "Skip xform: N %d has incident ribC.\n", MN->id());
      return MSXT_SKIP;
    }
  }

  //1-3) Determine the ms_node NA14 (of A14) to keep and Nm to be merged.
  dbsk3d_ms_node *NA14, *Nm;
  assert (MCp->is_self_loop() == false);
  if (MCp->s_MN()->n_type() == N_TYPE_AXIAL_END) {
    NA14 = MCp->e_MN();
    Nm = MCp->s_MN();
  }
  else {
    if (MCp->e_MN()->n_type() != N_TYPE_AXIAL_END) {
      vul_printf (vcl_cout, "Skip xform: both ends of C %d not A14.\n", MCp->id());
      return MSXT_SKIP;
    }
    Nm = MCp->e_MN();
    NA14 = MCp->s_MN();
  }

  if (NA14->n_type() != N_TYPE_AXIAL_END && 
      NA14->n_type() != N_TYPE_DEGE_RIB_END && 
      NA14->n_type() != N_TYPE_DEGE_AXIAL_END) {
    vul_printf (vcl_cout, "Skip xform: NA14 %d type not allowed for xform()!\n", NA14->id());
    return MSXT_SKIP;
  }

  assert (NA14->is_E_incident (MCp));
  assert (NA14->is_E_incident (MCq));
  assert (Nm->is_E_incident (MCp));
  assert (Nm->is_E_incident (MCq));

  //1-4) Determine the merging ms_curves MCpm and MCqm.
  assert (Nm->n_incident_Es() == 4);
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

  //1-5) Determine the merging ms_sheet MSm.
  dbsk3d_ms_sheet* MSm = (dbsk3d_ms_sheet*) get_F_from_E1_E2 (MCpm, MCqm);
  if (MSm == NULL) {
    vul_printf (vcl_cout, "Can't find valid Sm (NULL)!\n");
    return MSXT_FAIL;
  }

  //1-6) If any shared_F of MS not shared by MSm, skip xform.
  vcl_set<dbmsh3d_sheet*> MS_merge_set;
  MS_merge_set.insert (MSm);
  if (MS->shared_F_with_Sset (MS_merge_set) == false) {
    vul_printf (vcl_cout, "Skip xform: S %d has shared_F not with MSm %d.\n", MSm->id());
    return MSXT_SKIP;
  }

  //2) Perform the transform.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  _remove_MC_from_Q (MCp);
  _remove_MC_from_Q (MCq);
  _remove_MN_from_Q (Nm);
  _remove_MN_from_Q (NA14);
  _remove_MC_from_Q (MCpm);
  _remove_MC_from_Q (MCqm);
  _remove_MS_from_Q (MSm);

  //2-1) Merge MCp to MCpm at Nm.
  vul_printf (vcl_cout, "merge C%d to C%d at N%d, ", MCp->id(), MCpm->id(), Nm->id());
  _merge_E_vec_C2_to_C1 (Nm, MCpm, MCp);
  MCp->add_shared_Es_to_C (MCpm);

  if (MCpm->s_MN() == Nm)
    MCpm->_set_vertex (0, NA14);
  else {
    assert (MCpm->e_MN() == Nm);
    MCpm->_set_vertex (1, NA14);
  }
  Nm->del_incident_E (MCpm);
  NA14->add_incident_E (MCpm);
  modified_MC_set.insert (MCpm);
  
  //2-2) Merge MCq to MCqm at Nm.
  vul_printf (vcl_cout, "merge C%d to C%d at N%d, ", MCq->id(), MCqm->id(), Nm->id());
  _merge_E_vec_C2_to_C1 (Nm, MCqm, MCq);
  MCq->add_shared_Es_to_C (MCqm);

  if (MCqm->s_MN() == Nm)
    MCqm->_set_vertex (0, NA14);
  else {
    assert (MCqm->e_MN() == Nm);
    MCqm->_set_vertex (1, NA14);
  }
  Nm->del_incident_E (MCqm);
  NA14->add_incident_E (MCqm);
  modified_MC_set.insert (MCqm);

  //2-3) Merge MS.FF into MSm.
  vul_printf (vcl_cout, "merge S%d to S%d at N%d, ", MS->id(), MSm->id(), Nm->id());
  vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
  for (; it != MS->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    assert (MSm->facemap (F->id()) == NULL);
    MSm->add_F (F);
  }
  modified_MS_set.insert (MSm);

  //2-4) Delete MCp and MCq.
  //Clean all E_vec and shared_E of MCp and MCq.
  MCp->clear_shared_E_list();
  MCq->clear_shared_E_list();
  MCp->_clear_E_vec();
  MCq->_clear_E_vec();

  //Disconnect MCp and MCq from all incident ms_sheets.
  vcl_set<dbmsh3d_face*> disconn_faces;
  MCp->_disconnect_all_Fs (disconn_faces);
  //assert all faces incident to MCp are incident to MCpm.
  disconn_faces.clear();
  MCq->_disconnect_all_Fs (disconn_faces);
  //assert all faces incident to MCq are incident to MCqm.

  ms_hypg_->remove_edge (MCp);
  ms_hypg_->remove_edge (MCq);

  //2-5) Disconnect MS from all incident ms_curves and delete it.
  //Clean all FF from the MS.
  MS->facemap().clear();
  ms_hypg_->remove_sheet (MS);

  //2-6) Delete the ms_node Nm.
  assert (Nm->have_incident_Es() == false);
  ms_hypg_->remove_vertex (Nm);

  //2-7) Determine NA14 type.
  NA14->compute_n_type ();
  modified_MN_set.insert (NA14);

  //Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);
      
  //Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  return MSXT_SUCCESS;
}

//############################################################################
//  A1A3-II node-node merge transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A1A3II_n_n_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2i, 
                                                  const float cost, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A1A3II_n_n_merge_xform() on N1 %d and N2 %d cost %f: \n\t", 
              MN1->id(), MN2i->id(), cost);

  //1) Determine if this transform is valid.
  dbsk3d_ms_node* MN2;
  dbsk3d_ms_sheet* baseMS;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  //1-1) Given MN1, find MN2 for the node-node merge xform.
  if (!find_A1A3II_n_n_merge_N (MN1, MN2, baseMS, shortest_Evec)) {
    vul_printf (vcl_cout, "Skip xform: merging path not found.\n");
    return MSXT_SKIP; //Return if no candidate can be found.
  }
  
  //1-2) In theory, MN2 should be MN2i.
  //     Perform xform if cost (MN1, MN2) < cost (MN1, MN2i).
  if (MN2 != MN2i) {
    if (float(shortest_Evec.size()) < cost + MERGE_XFORM_COST_DELTA) {
      vul_printf (vcl_cout, "N2i %d N2 %d, cost_in_Q %d, new cost %f is smaller (delta = %f).\n",
                  MN2i->id(), MN2->id(), cost, shortest_Evec.size(), MERGE_XFORM_COST_DELTA);
    }
    else {
      vul_printf (vcl_cout, "Skip xform: N2i %d N2 %d, cost_in_Q %d, new cost %f too large.\n",
                  MN2i->id(), MN2->id(), cost, shortest_Evec.size());
      return MSXT_SKIP;
    }
  }

  //1-3) Determine the tabMC1 incident to MN1.  
  dbsk3d_ms_curve* tabMC1 = NULL;
  vcl_set<dbmsh3d_edge*> axial_N_set;
  baseMS->get_axial_inc_N (MN1, axial_N_set);
  if (axial_N_set.size() == 1) {
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC1 = (dbsk3d_ms_curve*) (*cit);
  }
  else {
    vul_printf (vcl_cout, "Skip xform: tabMC1 not found.\n");
    return MSXT_SKIP;
  }

  //1-4) Determine the tabMC2 incident to MN2i.  
  dbsk3d_ms_curve* tabMC2 = NULL;
  baseMS->get_axial_inc_N (MN2, axial_N_set);
  if (axial_N_set.size() == 1) {
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC2 = (dbsk3d_ms_curve*) (*cit);
  }
  else {
    vul_printf (vcl_cout, "Skip xform: tabMC2 not found.\n");
    return MSXT_SKIP;
  }

  //1-5) Skip xform if baseMS will be divided into two.
  if (merge_test_divide_MS (baseMS, shortest_Evec) == false) {
    vul_printf (vcl_cout, "Fail: MS%d will be divided into two.\n", baseMS->id());
    return MSXT_FAIL;
  }

  //2) Perform the transform.

  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //Remove related MS and MC from queue Q.
  _remove_MS_from_Q (baseMS);
  _remove_MN_from_Q (MN1);
  _remove_MN_from_Q (MN2);

  //2-2) Modify the hypergraph structure.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-2-1) The node-node merging operation
  MN_MN_merge (ms_hypg_, baseMS, MN1, MN2, shortest_Evec);  
  //MN1 should be deleted.
  modified_MN_set.insert (MN2);

  //2-2-2) Handle the possible division of bastMS into two sheets.
  dbsk3d_ms_sheet* MSn = MN_MN_merge_divide_MS (ms_hypg_, baseMS, MN1, tabMC1, MN2, tabMC2);
  if (MSn)
    modified_MS_set.insert (MSn);

  //2-2-3) Check and fix the topology of the baseMS and MSn. 
  baseMS->canonicalization();
  modified_MS_set.insert (baseMS);
  MSn->canonicalization();
  modified_MS_set.insert (MSn);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  return MSXT_SUCCESS;
}

//############################################################################
//  A12A3-I node-curve merge transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A12A3I_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2i, 
                                                  const float cost, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A12A3I_n_c_merge_xform() on N1 %d and C2 %d cost %f: \n\t", 
              MN1->id(), MC2i->id(), cost);

  //1) Determine if this transform is valid.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  //1-1) Given MN1, find MC2 for the node-curve merge xform.
  if (!find_A12A3I_n_c_merge_C (MN1, MC2, baseMS, closest_V, shortest_Evec, cmxth_)) {
    vul_printf (vcl_cout, "Skip xform: merging path not found.\n");
    return MSXT_SKIP; //Return if no candidate can be found.
  }
  
  //1-2) In theory, MC2 should be MC2i.
  //     Perform xform if cost (MN1, MC2) < cost (MN1, MC2i).
  if (MC2 != MC2i) {
    if (float(shortest_Evec.size()) < cost + MERGE_XFORM_COST_DELTA) {
      vul_printf (vcl_cout, "C2i %d C2 %d, cost_in_Q %d, new cost %f is smaller (delta = %f).\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size(), MERGE_XFORM_COST_DELTA);
    }
    else {
      vul_printf (vcl_cout, "Skip xform: C2i %d C2 %d, cost_in_Q %d, new cost %f too large.\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size());
      return MSXT_SKIP;
    }
  }
  
  //Determine the tabMC incident to N.  
  dbsk3d_ms_curve* tabMC;
  vcl_set<dbmsh3d_edge*> axial_N_set;
  baseMS->get_axial_inc_N (MN1, axial_N_set);
  if (axial_N_set.size() == 1) {
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC = (dbsk3d_ms_curve*) (*cit);
  }
  else { //The degenerate case if there exists multiple tabMC's.
    //Find the only one valid tabMS to divide MS into two.
    assert (0);
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC = (dbsk3d_ms_curve*) (*cit);
  }

  if (is_HE_3_incidence (tabMC->halfedge())) {
    vul_printf (vcl_cout, "Xform fails: tabC %d 3-incidence.\n", tabMC->id());
    return MSXT_FAIL;
  }

  //2) Perform the transform.

  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //Remove related MS and MC from queue Q.
  //_remove_MS_from_Q (baseMS); no need, will be done later.
  _remove_MC_from_Q (MC2);
  _remove_MN_from_Q (MN1);

  //2-2) Modify the hypergraph structure.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-2-1) The node-curve merging operation
  dbsk3d_ms_curve* MC2n;
  MN_MC_merge (ms_hypg_, baseMS, MN1, MC2, shortest_Evec, closest_V, MC2n);

  if (MC2n)
    modified_MC_set.insert (MC2n);
  modified_MC_set.insert (MC2);
  modified_MN_set.insert (MN1);
  
  //2-2-2) Handle the possible division of bastMS into two sheets.
  dbsk3d_ms_sheet* MSn = MN_MC_merge_divide_MS (ms_hypg_, baseMS, MN1, tabMC, MC2, MC2n);
  if (MSn)
    modified_MS_set.insert (MSn);

  //2-2-3) Check and fix the topology of all MS's incident to MC2. 
  dbmsh3d_halfedge* HE = MC2->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
    MS->canonicalization();
    _remove_MS_from_Q (MS);
    modified_MS_set.insert (MS);
    HE = HE->next();
  }
  while (HE != MC2->halfedge() && HE != NULL);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  return MSXT_SUCCESS;
}

//############################################################################
//  A14 curve-curve merge transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A14_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2i, 
                                               const float cost, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A14_c_c_merge_xform() on C1 %d and C2 %d cost %f: \n\t", 
              MC1->id(), MC2i->id(), cost);

  //1) Determine if this transform is valid.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  //1-1) Given MC1, find MC2 for the curve-curve merge xform.
  if (!find_A14_c_c_merge_C (MC1, MC2, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth_)) {
    vul_printf (vcl_cout, "Skip xform: merging path not found.\n");
    return MSXT_SKIP; //Return if no candidate can be found.
  }
  
  //1-2) In theory, MC2 should be MC2i.
  //     Perform xform if cost (MN1, MC2) < cost (MN1, MC2i).
  if (MC2 != MC2i) {
    if (float(shortest_Evec.size()) < cost + MERGE_XFORM_COST_DELTA) {
      vul_printf (vcl_cout, "C2i %d C2 %d, cost_in_Q %d, new cost %f is smaller (delta = %f).\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size(), MERGE_XFORM_COST_DELTA);
    }
    else {
      vul_printf (vcl_cout, "Skip xform: C2i %d C2 %d, cost_in_Q %d, new cost %f too large.\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size());
      return MSXT_SKIP;
    }
  }

  //1-3) Skip xform if MC1 and MC2 not in baseMS's main halfedge chain.
  if (find_E_in_next_chain (baseMS->halfedge(), MC1) == false) {
    vul_printf (vcl_cout, "Skip xform: C%d not in MS%d's he chain.\n", MC1->id(), baseMS->id());
    return MSXT_SKIP;
  }
  if (find_E_in_next_chain (baseMS->halfedge(), MC2) == false) {
    vul_printf (vcl_cout, "Skip xform: C%d not in MS%d's he chain.\n", MC2->id(), baseMS->id());
    return MSXT_SKIP;
  }

  //2) Perform the transform.
  
  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //Remove related MS and MC from queue Q.
  //_remove_MS_from_Q (baseMS); no need, will be done later.
  _remove_MC_from_Q (MC1);
  _remove_MC_from_Q (MC2);  

  //2-2) Modify the hypergraph structure.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-2-1) The curve-curve merging operation
  dbsk3d_ms_curve* MC1n = NULL;
  dbsk3d_ms_curve* MC2n = NULL;
  dbsk3d_ms_node* MNn = NULL;

  MC_MC_merge (ms_hypg_, baseMS, MC1, MC2, shortest_Evec, closest_V1, closest_V2, MC1n, MC2n, MNn);
  MNn->compute_n_type();

  if (MC1n)
    modified_MC_set.insert (MC1n);
  if (MC2n)
    modified_MC_set.insert (MC2n);  
  modified_MC_set.insert (MC1);
  modified_MC_set.insert (MC2);
  assert (MNn);
  modified_MN_set.insert (MNn);

  //2-3-2) Divide baseMS into two parts, separated by MNn.
  //       The HE chain order of baseMS is: MC1 -> MC1n -> ... -> MC1.
  dbsk3d_ms_sheet* MSn = MC_MC_merge_divide_MS (ms_hypg_, baseMS, MNn, MC1, MC1n, MC2, MC2n);
  modified_MS_set.insert (MSn);
  
  //2-3-3) Check and fix the topology of all incident sheets of MC1 and MC2.
  bool r;
  dbmsh3d_halfedge* HE = MC1->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();    
    if (MS != baseMS && MS != MSn) {
      //Add MC1n to MS in a correct order.
      dbmsh3d_halfedge* MC1n_HE = MC1n->get_HE_of_F (MS);
      assert (MC1n_HE);
      r = MS->connect_adj_HE (HE, MC1n_HE);
      assert (r);

      _remove_MS_from_Q (MS);
      modified_MS_set.insert (MS);
    }
    HE = HE->pair();
  }
  while (HE != MC1->halfedge() && HE != NULL);

  HE = MC2->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();      
    if (MS != baseMS && MS != MSn) { 
      //Add MC2n to MS in a correct order.
      dbmsh3d_halfedge* MC2n_HE = MC2n->get_HE_of_F (MS);
      assert (MC2n_HE);
      if (HE->next() == MC2n_HE)
        r = MS->connect_adj_HE (HE, MC2n_HE);
      else {
        assert (MC2n_HE->next() == HE);
        r = MS->connect_adj_HE (MC2n_HE, HE);
      }
      assert (r);

      _remove_MS_from_Q (MS);
      modified_MS_set.insert (MS);
    }
    HE = HE->pair();
  }
  while (HE != MC2->halfedge() && HE != NULL);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  return MSXT_SUCCESS;
}

//############################################################################
//  A1A3-II curve-curve merge transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A1A3II_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2i, 
                                                  const float cost, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A1A3II_c_c_merge_xform() on C1 %d and C2 %d cost %f: \n\t", 
              MC1->id(), MC2i->id(), cost);

  //1) Determine if this transform is valid.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  //1-1) Given MC1, find MC2 for the curve-curve merge xform.
  if (!find_A1A3II_c_c_merge_C (MC1, MC2, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth_)) {
    vul_printf (vcl_cout, "Skip xform: merging path not found.\n");
    return MSXT_SKIP; //Return if no candidate can be found.
  }
  
  //1-2) In theory, MC2 should be MC2i.
  //     Perform xform if cost (MN1, MC2) < cost (MN1, MC2i).
  if (MC2 != MC2i) {
    if (float(shortest_Evec.size()) < cost + MERGE_XFORM_COST_DELTA) {
      vul_printf (vcl_cout, "C2i %d C2 %d, cost_in_Q %d, new cost %f is smaller (delta = %f).\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size(), MERGE_XFORM_COST_DELTA);
    }
    else {
      vul_printf (vcl_cout, "Skip xform: C2 %d C2n %d, cost_in_Q %d, new cost %f too large.\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size());
      return MSXT_SKIP;
    }
  }

  //1-3) Skip xform if MC1 and MC2 not in baseMS's main halfedge chain.
  if (find_E_in_next_chain (baseMS->halfedge(), MC1) == false) {
    vul_printf (vcl_cout, "Skip xform: C%d not in S%d's he chain.\n", MC1->id(), baseMS->id());
    return MSXT_SKIP;
  }
  if (find_E_in_next_chain (baseMS->halfedge(), MC2) == false) {
    vul_printf (vcl_cout, "Skip xform: C%d not in S%d's he chain.\n", MC2->id(), baseMS->id());
    return MSXT_SKIP;
  }

  //2) Perform the transform.
  
  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //2-2) Make the rib curve as MC1 and axial as MC2.
  if (MC2->halfedge()->pair() == NULL) {
    dbsk3d_ms_curve* tmpc = MC2;
    MC2 = MC1;
    MC1 = tmpc;
    dbmsh3d_vertex* tmpv = closest_V1;
    closest_V1 = closest_V2;
    closest_V2 = tmpv;
    vcl_reverse (shortest_Evec.begin(), shortest_Evec.end());
  }
  assert (MC1->halfedge()->pair() == NULL);
  assert (MC2->halfedge()->pair() != NULL);

  //Remove related MS and MC from queue Q.
  //_remove_MS_from_Q (baseMS); no need, will be done later.
  _remove_MC_from_Q (MC1);
  _remove_MC_from_Q (MC2);  

  //2-3) Modify the hypergraph structure.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-3-1) The curve-curve merging operation
  dbsk3d_ms_curve* MC1n = NULL;
  dbsk3d_ms_curve* MC2n = NULL;
  dbsk3d_ms_node* MNn = NULL;

  MC_MC_merge (ms_hypg_, baseMS, MC1, MC2, shortest_Evec, closest_V1, closest_V2, MC1n, MC2n, MNn);
  MNn->compute_n_type();

  if (MC1n)
    modified_MC_set.insert (MC1n);
  if (MC2n)
    modified_MC_set.insert (MC2n);
  modified_MC_set.insert (MC1);
  modified_MC_set.insert (MC2);
  assert (MNn);
  modified_MN_set.insert (MNn);

  //2-3-2) Divide baseMS into two parts, separated by MNn.
  //       The HE chain order of baseMS is: MC1 -> MC1n -> ... -> MC1.
  dbsk3d_ms_sheet* MSn = MC_MC_merge_divide_MS (ms_hypg_, baseMS, MNn, MC1, MC1n, MC2, MC2n);
  modified_MS_set.insert (MSn);

  //2-3-3) Check and fix the topology of all incident sheets of MC1 and MC2.
  bool r;
  dbmsh3d_halfedge* HE = MC1->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();    
    if (MS != baseMS && MS != MSn) {
      //Add MC1n to MS in a correct order.
      dbmsh3d_halfedge* MC1n_HE = MC1n->get_HE_of_F (MS);
      assert (MC1n_HE);
      r = MS->connect_adj_HE (HE, MC1n_HE);
      assert (r);

      _remove_MS_from_Q (MS);
      modified_MS_set.insert (MS);
    }
    HE = HE->pair();
  }
  while (HE != MC1->halfedge() && HE != NULL);

  HE = MC2->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();      
    if (MS != baseMS && MS != MSn) { 
      //Add MC2n to MS in a correct order.
      dbmsh3d_halfedge* MC2n_HE = MC2n->get_HE_of_F (MS);
      assert (MC2n_HE);
      if (HE->next() == MC2n_HE)
        r = MS->connect_adj_HE (HE, MC2n_HE);
      else {
        assert (MC2n_HE->next() == HE);
        r = MS->connect_adj_HE (MC2n_HE, HE);
      }
      assert (r);

      _remove_MS_from_Q (MS);
      modified_MS_set.insert (MS);
    }
    HE = HE->pair();
  }
  while (HE != MC2->halfedge() && HE != NULL);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  return MSXT_SUCCESS;
}

//############################################################################
//  A1A5 node-curve merge transform.
//############################################################################

//  return 0: failed, 1: skipped, 2: successfully done.
int dbsk3d_ms_hypg_trans::A1A5_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2i, 
                                                const float cost, const bool modify_bnd)
{
  vul_printf (vcl_cout, "\n  A1A5_n_c_merge_xform() on N1 %d and C2 %d cost %f: \n\t", 
              MN1->id(), MC2i->id(), cost);

  //1) Determine if this transform is valid.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  //1-1) Given MN1, find MC2 for the node-curve merge xform.
  if (!find_A1A5_n_c_merge_C (MN1, MC2, baseMS, closest_V, shortest_Evec, cmxth_)) {
    vul_printf (vcl_cout, "Skip xform: merging path not found.\n");
    return MSXT_SKIP; //Return if no candidate can be found.
  }
  
  //1-2) In theory, MC2 should be MC2i.
  //     Perform xform if cost (MN1, MC2) < cost (MN1, MC2i).
  if (MC2 != MC2i) {
    if (float(shortest_Evec.size()) < cost + MERGE_XFORM_COST_DELTA) {
      vul_printf (vcl_cout, "C2i %d C2 %d, cost_in_Q %d, new cost %f is smaller (delta = %f).\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size(), MERGE_XFORM_COST_DELTA);
    }
    else {
      vul_printf (vcl_cout, "Skip xform: C2 %d C2n %d, cost_in_Q %d, new cost %f too large.\n",
                  MC2i->id(), MC2->id(), cost, shortest_Evec.size());
      return MSXT_SKIP;
    }
  }

  //1-3) Avoid a complicate case where the possible splitting of MS is difficult to implement.
  
  //Determine the tabMC incident to N.  
  dbsk3d_ms_curve* tabMC;
  vcl_set<dbmsh3d_edge*> axial_N_set;
  baseMS->get_axial_inc_N (MN1, axial_N_set);
  if (axial_N_set.size() == 1) {
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC = (dbsk3d_ms_curve*) (*cit);
  }
  else { //The degenerate case if there exists multiple tabMC's.
    //Find the only one valid tabMS to divide MS into two.
    assert (0);
    vcl_set<dbmsh3d_edge*>::iterator cit = axial_N_set.begin();
    tabMC = (dbsk3d_ms_curve*) (*cit);
  }

  //Determine the vector IC_pairs_bndN_N[][] in the general case.
  vcl_vector<vcl_vector<dbmsh3d_edge*> > IC_pairs_bndN_N;
  vcl_vector<dbmsh3d_edge*> IC_loop_E_heads;
  dbmsh3d_vertex* bndN;
  bool r = get_S_icurve_vec_bndN (baseMS, MN1, tabMC, IC_pairs_bndN_N, IC_loop_E_heads, bndN);

  if (r == false) {
    vul_printf (vcl_cout, "Xform fails: > 3 incident bnd or IC-chain incident to bndN %d!\n", bndN->id());
    return MSXT_FAIL; //Return if no candidate can be found.
  }

  if (is_HE_3_incidence (tabMC->halfedge())) {
    vul_printf (vcl_cout, "Xform fails: tabC %d 3-incidence.\n", tabMC->id());
    return MSXT_FAIL;
  }

  //2) Perform the transform.

  //2-1) Modify shape boundary.
  if (modify_bnd) {
  }

  //Remove related MS and MC from queue Q.
  ///_remove_MS_from_Q (baseMS); no need, will be done later.
  _remove_MC_from_Q (MC2);
  _remove_MN_from_Q (MN1);

  //2-2) Modify the hypergraph structure.
  vcl_set<dbsk3d_ms_sheet*> modified_MS_set;
  vcl_set<dbsk3d_ms_curve*> modified_MC_set;
  vcl_set<dbsk3d_ms_node*> modified_MN_set;

  //2-2-1) The node-curve merging operation
  dbsk3d_ms_curve* MC2n;
  MN_MC_merge (ms_hypg_, baseMS, MN1, MC2, shortest_Evec, closest_V, MC2n);

  if (MC2n)
    modified_MC_set.insert (MC2n);  
  modified_MC_set.insert (MC2);
  modified_MN_set.insert (MN1);

  //2-2-2) Handle the possible division of bastMS into two sheets.
  dbsk3d_ms_sheet* MSn = MN_MC_merge_divide_MS (ms_hypg_, baseMS, MN1, tabMC, MC2, MC2n);
  if (MSn)
    modified_MS_set.insert (MSn);

  //2-2-3) Check and fix the topology of all MS's incident to MC2. 
  dbmsh3d_halfedge* HE = MC2->halfedge();
  do {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) HE->face();
    MS->canonicalization();
    _remove_MS_from_Q (MS);
    modified_MS_set.insert (MS);
    HE = HE->next();
  }
  while (HE != MC2->halfedge() && HE != NULL);

  //3) Try add modified elements to Q
  try_add_modified_to_Q (modified_MS_set, modified_MC_set, modified_MN_set);

  //4) Check integrity on remaining modified MS, MC, and MN.
  if (check_integrity (modified_MS_set, modified_MC_set, modified_MN_set) == false) {
    vul_printf (vcl_cout, "check_integrity fails! ");
    assert (0);
  }

  ///ms_hypg_->check_integrity();
  return MSXT_SUCCESS;
}

//############################################################################
//  Functions for hybrid case involving two or more xforms 
//############################################################################

//: A5 Contract transform on ms_curve with a single fs_edge.
void dbsk3d_ms_hypg_trans::MC_A5_trim_single_L (dbsk3d_ms_curve* MC)
{
  //Get the other A3 curves of SCS - Ns:MC:Ne - SCE.  
  dbsk3d_ms_node* Ns = MC->s_MN();
  dbsk3d_ms_node* Ne = MC->e_MN();
  dbsk3d_ms_curve* Cs = (dbsk3d_ms_curve*) V_find_other_E (Ns, MC);
  dbsk3d_ms_curve* Ce = (dbsk3d_ms_curve*) V_find_other_E (Ne, MC);
  assert (Cs->c_type() == C_TYPE_RIB);
  assert (Ce->c_type() == C_TYPE_RIB);
  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();

  assert (MC->E_vec().size() == 1);
  dbsk3d_fs_edge* A13L = (dbsk3d_fs_edge*) MC->E_vec(0);

  //Choose one fs_face to remove.
  dbsk3d_fs_face* trimFF = A5_contract_L_find_trim_P (A13L);

  //Perform fine-scale splice xform on trimFF.
  FF_prune_pass_Gs (trimFF);

  //Reconnect A3 rib curves.
  assert (0);

}


//###############################################################
//  MS transition queue handling 
//###############################################################

void dbsk3d_ms_hypg_trans::try_add_MS_to_Q (dbsk3d_ms_sheet* MS)
{
  //A1A3-I, A12A3-II medial sheet splice xforms.
  try_add_MS_splice_to_Q (MS);

  //A15 medial sheet contract xforms.
  try_add_MS_A15_contract_to_Q (MS);
  
  //A14 medial sheet contract xforms.
  try_add_MS_A14_contract_to_Q (MS);
}

void dbsk3d_ms_hypg_trans::try_add_MS_splice_to_Q (dbsk3d_ms_sheet* MS)
{
  //Check if the ms_sheet MS valid for a sheet_splice_xform().
  if (MS_valid_for_splice_xform (MS) == false)
    return;
  
  //Compute splice cost.
  vcl_map<int, dbmsh3d_vertex*> asso_Gs;
  MS->get_asso_Gs (asso_Gs);
  float cost = float (asso_Gs.size());
  cost *= Wts_;

  //If cost < splice threshold Tts_.
  if (cost < Tts_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_TAB_SPLICE, (void*) MS, NULL);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "splice (S %d, %.2f), ", MS->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MS_A15_contract_to_Q (dbsk3d_ms_sheet* MS)
{
  //Check if the ms_sheet MS valid for a MS_contract_xform().
  if (MS_valid_for_A15_contract_xform (MS) == false)
    return;
  
  //Compute splice cost.
  vcl_map<int, dbmsh3d_vertex*> asso_Gs;
  MS->get_asso_Gs (asso_Gs);
  double cost = asso_Gs.size();
  cost *= Wsc_;

  //If cost < sheet-contract threshold Tsc_.
  if (cost < Tsc_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (float(cost), MSXT_A15_SHEET_CONTRACT, (void*) MS, NULL);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A15_s_contract (S %d, %.2f), ", MS->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MS_A14_contract_to_Q (dbsk3d_ms_sheet* MS)
{
  //Check if the ms_sheet MS valid for a MS_contract_xform().
  if (MS_valid_for_A14_contract_xform (MS) == false)
    return;
  
  //Compute splice cost.
  vcl_map<int, dbmsh3d_vertex*> asso_Gs;
  MS->get_asso_Gs (asso_Gs);
  double cost = asso_Gs.size();
  //Make the cost of A14-s-c 10% more than A15-s-c xform (results more degenerate).
  cost *= (Wsc_ * 1.1); 

  //If cost < sheet-contract threshold Tsc_.
  if (cost < Tsc_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (float(cost), MSXT_A14_SHEET_CONTRACT, (void*) MS, NULL);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A14_s_contract (S %d, %.2f), ", MS->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MC_to_Q (dbsk3d_ms_curve* MC)
{
  //A5 curve-contract xform.
  try_add_MC_A5_contract_to_Q (MC);

  //A12A3-I curve-contract xform.
  try_add_MC_A12A3I_contract_to_Q (MC);

  //A15 curve-contract xform.
  try_add_MC_A15_contract_to_Q (MC);

  //A12A3-I node-curve merge xform.
  try_add_A12A3I_n_c_merge_to_Q (MC);

  //A14 curve-curve merge xform.
  try_add_A14_c_c_merge_to_Q (MC);

  //A1A3-II curve-curve merge xform.
  try_add_A1A3II_c_c_merge_to_Q (MC);
  
  //A1A5 node-curve merge xform.
  try_add_A1A5_n_c_merge_to_Q (MC);
}

void dbsk3d_ms_hypg_trans::try_add_MC_A5_contract_to_Q (dbsk3d_ms_curve* MC)
{
  //Check if the ms_curve MC valid for a A5_curve_contract_xform.
  if (MC_valid_for_A5_contract_xform (MC) == false)
    return;

  //Compute A5 contrat cost.
  assert (MC->data_type() == C_DATA_TYPE_EDGE);
  float cost = float (MC->E_vec().size());
  cost *= Wc5_;

  //If cost < A5 contract cost Tc5_.
  if (cost < Tc5_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A5_CURVE_CONTRACT, MC, NULL);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A5_c_contract (C %d, %.2f), ", MC->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MC_A12A3I_contract_to_Q (dbsk3d_ms_curve* MC)
{
  //Check if the ms_curve MC valid for a A12A3I_curve_contract_xform.
  if (MC_valid_for_A12A3I_contract_xform (MC) == false)
    return;

  //Compute A12A3-I curve-contract cost.
  assert (MC->data_type() == C_DATA_TYPE_EDGE);
  float cost = float (MC->E_vec().size());
  cost *= Wcc_;

  //If cost < curve-contract cost Tcc_.
  if (cost < Tcc_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A12A3I_CURVE_CONTRACT, MC, NULL);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A12A3-I_c_contract (C %d, %.2f), ", MC->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MC_A15_contract_to_Q (dbsk3d_ms_curve* MC)
{
  //Check if the ms_curve MC valid for a A15_curve_contract_xform.
  if (MC_valid_for_A15_contract_xform (MC) == false)
    return;

  //Compute A15 curve-contract cost.
  assert (MC->data_type() == C_DATA_TYPE_EDGE);
  float cost = float (MC->E_vec().size());
  cost *= Wcc_;

  //If cost < curve contract cost Tcc_.
  if (cost < Tcc_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A15_CURVE_CONTRACT, MC, NULL);
    vul_printf (vcl_cout, "A15_c_contract (C %d, %.2f), ", MC->id(), cost);
  }
}

void dbsk3d_ms_hypg_trans::try_add_A12A3I_n_c_merge_to_Q (dbsk3d_ms_curve* MC1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MN and try add to Q.
  dbsk3d_ms_node* MN2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V; 
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A12A3I_c_n_merge_N (MC1, MN2, baseMS, closest_V, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MN2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wncm_;

  //If cost < curve contract cost Tncm_.
  if (cost < Tncm_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A12A3I_N_C_MERGE, MN2, MC1);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A12A3-I_n_c_merge (N %d C %d, %.2f), ", 
                MN2->id(), MC1->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_A1A5_n_c_merge_to_Q (dbsk3d_ms_curve* MC1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MN and try add to Q.
  dbsk3d_ms_node* MN2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V; 
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A1A5_n_c_merge_N (MC1, MN2, baseMS, closest_V, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MN2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wncm_;

  //If cost < curve contract cost Tncm_.
  if (cost < Tncm_a1a5_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A1A5_N_C_MERGE, MN2, MC1);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A1A5_n_c_merge (N %d C %d, %.2f), ", 
                MN2->id(), MC1->id(), cost);
    #endif
  }
}


void dbsk3d_ms_hypg_trans::try_add_A14_c_c_merge_to_Q (dbsk3d_ms_curve* MC1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MC2 and try add to Q.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2; 
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A14_c_c_merge_C (MC1, MC2, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MC2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wccm_;

  //If cost < curve contract cost Tccm_.
  if (cost < Tccm_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A14_C_C_MERGE, MC1, MC2);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A14_c_c_merge (C %d C2 %d, %.2f), ", 
                MC1->id(), MC2->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_A1A3II_c_c_merge_to_Q (dbsk3d_ms_curve* MC1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MC2 and try add to Q.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2; 
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A1A3II_c_c_merge_C (MC1, MC2, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MC2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wccm_;

  //If cost < curve contract cost Tccm_.
  if (cost < Tccm_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A1A3II_C_C_MERGE, MC1, MC2);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A1A3-II_c_c_merge (C %d C2 %d, %.2f), ", 
                MC1->id(), MC2->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_MN_to_Q (dbsk3d_ms_node* MN)
{
  //A1A3-II node-node merge xform.
  try_add_A1A3II_n_n_merge_to_Q (MN);

  //A12A3-I node-curve merge xform.
  try_add_A12A3I_n_c_merge_to_Q (MN);

  //A1A5 node-curve merge xform.
  try_add_A1A5_n_c_merge_to_Q (MN);
}

void dbsk3d_ms_hypg_trans::try_add_A1A3II_n_n_merge_to_Q (dbsk3d_ms_node* MN1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MN2 and try add to Q.
  dbsk3d_ms_node* MN2;
  dbsk3d_ms_sheet* baseMS;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A1A3II_n_n_merge_N (MN1, MN2, baseMS, shortest_Evec))
    return; //Return if no candidate can be found.

  assert (MN2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wnnm_;

  //If cost < node-node merge cost Tnnm_.
  if (cost < Tnnm_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A1A3II_N_N_MERGE, MN1, MN2);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A1A3-II_n_n_merge (N %d N %d, %.2f), ", 
                MN1->id(), MN2->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_A12A3I_n_c_merge_to_Q (dbsk3d_ms_node* MN1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MC2 and try add to Q.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A12A3I_n_c_merge_C (MN1, MC2, baseMS, closest_V, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MC2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wncm_;

  //If cost < node-curve merge cost Tncm_.
  if (cost < Tncm_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A12A3I_N_C_MERGE, MN1, MC2);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A12A3-I_n_c_merge (N %d C %d, %.2f), ", 
                MN1->id(), MC2->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_A1A5_n_c_merge_to_Q (dbsk3d_ms_node* MN1)
{
  if (b_merge_xform_ == false)
    return; //skip

  //Find a close-by MC2 and try add to Q.
  dbsk3d_ms_curve* MC2;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  if (!find_A1A5_n_c_merge_C (MN1, MC2, baseMS, closest_V, shortest_Evec, cmxth_))
    return; //Return if no candidate can be found.

  assert (MC2);
  float cost = float (shortest_Evec.size());
  assert (cost > 0);
  cost *= Wncm_;

  //If cost < curve contract cost Tncm_.
  if (cost < Tncm_a1a5_) { //add to queue ms_xform_Q.
    _add_xform_to_Q (cost, MSXT_A1A5_N_C_MERGE, MN1, MC2);
    #if DBMSH3D_DEBUG > 1
    vul_printf (vcl_cout, "A1A5_n_c_merge (N %d C %d, %.2f), ", 
                MN1->id(), MC2->id(), cost);
    #endif
  }
}

void dbsk3d_ms_hypg_trans::try_add_modified_to_Q (vcl_set<dbsk3d_ms_sheet*>& modified_MS_set, 
                                                  vcl_set<dbsk3d_ms_curve*>& modified_MC_set, 
                                                  vcl_set<dbsk3d_ms_node*>& modified_MN_set)
                                                  
{  
  vcl_set<dbsk3d_ms_sheet*>::iterator sit = modified_MS_set.begin();
  for (; sit != modified_MS_set.end(); sit++) {
    dbsk3d_ms_sheet* MS = (*sit);
    try_add_MS_to_Q (MS);
  }

  vcl_set<dbsk3d_ms_curve*>::iterator cit = modified_MC_set.begin();
  for (; cit != modified_MC_set.end(); cit++) {
    dbsk3d_ms_curve* MC = (*cit);
    try_add_MC_to_Q (MC);
  }

  vcl_set<dbsk3d_ms_node*>::iterator nit = modified_MN_set.begin();
  for (; nit != modified_MN_set.end(); nit++) {
    dbsk3d_ms_node* MN = (*nit);
    try_add_MN_to_Q (MN);
  }
}

void dbsk3d_ms_hypg_trans::_pop_xform_from_Q (float& cost, char& type, dbsk3d_ms_sheet* & MS, 
                                              dbsk3d_ms_curve* & MC1, dbsk3d_ms_curve* & MC2,
                                              dbsk3d_ms_node* & MN1, dbsk3d_ms_node* & MN2)
{
  MS = NULL;
  MC1 = NULL;
  MC2 = NULL;
  MN1 = NULL;
  MN2 = NULL;

  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  cost = (*it).first;
  type = (*it).second.first;
  vcl_pair<void*, void*> data = (*it).second.second;  
  void* data1 = data.first;
  void* data2 = data.second;

  switch (type) {
  case MSXT_TAB_SPLICE:
  case MSXT_A15_SHEET_CONTRACT:
  case MSXT_A14_SHEET_CONTRACT:
    MS = (dbsk3d_ms_sheet*) data1;
  break;
  case MSXT_A5_CURVE_CONTRACT:
  case MSXT_A12A3I_CURVE_CONTRACT:
  case MSXT_A15_CURVE_CONTRACT:
    MC1 = (dbsk3d_ms_curve*) data1;
  break;
  case MSXT_A1A3II_N_N_MERGE:
    MN1 = (dbsk3d_ms_node*) data1;
    MN2 = (dbsk3d_ms_node*) data2;
  break;
  case MSXT_A12A3I_N_C_MERGE:
  case MSXT_A1A5_N_C_MERGE:
    MN1 = (dbsk3d_ms_node*) data1;
    MC2 = (dbsk3d_ms_curve*) data2;
  break;
  case MSXT_A14_C_C_MERGE:
  case MSXT_A1A3II_C_C_MERGE:
    MC1 = (dbsk3d_ms_curve*) data1;
    MC2 = (dbsk3d_ms_curve*) data2;
  break;
  default:
    assert (0);
  break;
  }
  
  //Remove this entry from the ms_xform_Q.
  ms_xform_Q_.erase (it);
}

//: Brutely search the ms_xform_Q[] for inputMS and remove the entry.
//  Note that a MS can be in the Q for multiple instances (diff. xforms),
//  so a brute force search is required.
void dbsk3d_ms_hypg_trans::_remove_MS_from_Q (const dbsk3d_ms_sheet* inputMS)
{
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  while (it != ms_xform_Q_.end()) {
    vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator cur = it;
    char type = (*cur).second.first;
    vcl_pair<void*, void*> data = (*cur).second.second;  
    void* data1 = data.first;
    void* data2 = data.second;
    it++;
    dbsk3d_ms_sheet* MS = NULL;

    switch (type) {
    case MSXT_TAB_SPLICE:
    case MSXT_A15_SHEET_CONTRACT:
    case MSXT_A14_SHEET_CONTRACT:
      MS = (dbsk3d_ms_sheet*) data1;
      if (MS == inputMS) {
        ms_xform_Q_.erase (cur);
        ///vul_printf (vcl_cout, "remove MS %d from Q, ", inputMS->id());
      }
    break;
    case MSXT_A5_CURVE_CONTRACT:
    case MSXT_A12A3I_CURVE_CONTRACT:
    case MSXT_A15_CURVE_CONTRACT:
    break;
    case MSXT_A1A3II_N_N_MERGE:
    break;
    case MSXT_A12A3I_N_C_MERGE:
    break;
    case MSXT_A14_C_C_MERGE:
    case MSXT_A1A3II_C_C_MERGE:
    break;
    case MSXT_A1A5_N_C_MERGE:
    break;
    default:
      assert (0);
    break;
    }
  }
}

//: Brutely search the ms_xform_Q[] for inputMC and remove the entry.
void dbsk3d_ms_hypg_trans::_remove_MC_from_Q (const dbsk3d_ms_curve* inputMC)
{
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  while (it != ms_xform_Q_.end()) {
    vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator cur = it;
    char type = (*cur).second.first;
    vcl_pair<void*, void*> data = (*cur).second.second;  
    void* data1 = data.first;
    void* data2 = data.second;
    it++;
    dbsk3d_ms_curve* MC = NULL;

    switch (type) {
    case MSXT_TAB_SPLICE:
    case MSXT_A15_SHEET_CONTRACT:
    case MSXT_A14_SHEET_CONTRACT:
    break;
    case MSXT_A5_CURVE_CONTRACT:
    case MSXT_A12A3I_CURVE_CONTRACT:
    case MSXT_A15_CURVE_CONTRACT:
      MC = (dbsk3d_ms_curve*) data1;
      if (MC == inputMC) {
        ms_xform_Q_.erase (cur);
        #if DBMSH3d_DEBUG > 2
        vul_printf (vcl_cout, "remove MC %d from Q, ", inputMC->id());
        #endif
      }
    break;
    case MSXT_A1A3II_N_N_MERGE:
    break;
    case MSXT_A12A3I_N_C_MERGE:
      MC = (dbsk3d_ms_curve*) data2;
      if (MC == inputMC) {
        ms_xform_Q_.erase (cur);
        #if DBMSH3d_DEBUG > 2
        vul_printf (vcl_cout, "remove MC %d from Q, ", inputMC->id());
        #endif
      }
    break;
    case MSXT_A14_C_C_MERGE:
    case MSXT_A1A3II_C_C_MERGE:
      MC = (dbsk3d_ms_curve*) data1;
      if (MC == inputMC) {
        ms_xform_Q_.erase (cur);
        #if DBMSH3d_DEBUG > 2
        vul_printf (vcl_cout, "remove MS %d from Q, ", inputMC->id());
        #endif
      }
      else if (data2 == inputMC) {
        MC = (dbsk3d_ms_curve*) data2;
        if (MC == inputMC) {
          ms_xform_Q_.erase (cur);
          #if DBMSH3d_DEBUG > 2
          vul_printf (vcl_cout, "remove MS %d from Q, ", inputMC->id());
          #endif
        }
      }
    break;
    case MSXT_A1A5_N_C_MERGE:
      MC = (dbsk3d_ms_curve*) data2;
      if (MC == inputMC) {
        ms_xform_Q_.erase (cur);
        #if DBMSH3d_DEBUG > 2
        vul_printf (vcl_cout, "remove MC %d from Q, ", inputMC->id());
        #endif
      }
    break;
    default:
      assert (0);
    break;
    }
  }
}

//: Brutely search the ms_xform_Q[] for inputSV and remove the entry.
void dbsk3d_ms_hypg_trans::_remove_MN_from_Q (const dbsk3d_ms_node* inputSV)
{
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  while (it != ms_xform_Q_.end()) {
    vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator cur = it;
    char type = (*cur).second.first;
    vcl_pair<void*, void*> data = (*cur).second.second;  
    void* data1 = data.first;
    void* data2 = data.second; 
    it++;
    dbsk3d_ms_node* MN = NULL;

    switch (type) {
    case MSXT_TAB_SPLICE:
    case MSXT_A15_SHEET_CONTRACT:
    case MSXT_A14_SHEET_CONTRACT:
    break;
    case MSXT_A5_CURVE_CONTRACT:
    case MSXT_A12A3I_CURVE_CONTRACT:
    case MSXT_A15_CURVE_CONTRACT:
    break;
    case MSXT_A1A3II_N_N_MERGE:
      MN = (dbsk3d_ms_node*) data1;
      if (MN == inputSV) {
        ms_xform_Q_.erase (cur);
        #if DBMSH3d_DEBUG > 2
        vul_printf (vcl_cout, "remove MN %d from Q, ", inputSV->id());
        #endif
      }
      else if (data2 == inputSV) {
        MN = (dbsk3d_ms_node*) data2;
        if (MN == inputSV) {
          ms_xform_Q_.erase (cur);
          #if DBMSH3d_DEBUG > 2
          vul_printf (vcl_cout, "remove MS %d from Q, ", inputSV->id());
          #endif
        }
      }
    break;
    case MSXT_A12A3I_N_C_MERGE:
      MN = (dbsk3d_ms_node*) data1;
      if (MN == inputSV) {
        ms_xform_Q_.erase (cur);
        ///vul_printf (vcl_cout, "remove MN %d from Q, ", inputSV->id());
      }
    break;
    case MSXT_A14_C_C_MERGE:
    case MSXT_A1A3II_C_C_MERGE:
    break;
    case MSXT_A1A5_N_C_MERGE:
      MN = (dbsk3d_ms_node*) data1;
      if (MN == inputSV) {
        ms_xform_Q_.erase (cur);
        ///vul_printf (vcl_cout, "remove MN %d from Q, ", inputSV->id());
      }
    break;
    default:
      assert (0);
    break;
    }
  }
}

bool dbsk3d_ms_hypg_trans::_check_integrity_Q ()
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "dbsk3d_ms_hypg_trans::_check_integrity_Q(). Q size %u.\n", ms_xform_Q_.size());
  #endif

  dbsk3d_ms_sheet* MS = NULL;
  dbsk3d_ms_curve* MC1 = NULL;
  dbsk3d_ms_curve* MC2 = NULL;
  dbsk3d_ms_node* MN1 = NULL;
  dbsk3d_ms_node* MN2 = NULL;
  
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  for (; it != ms_xform_Q_.end(); it++) {
    float cost = (*it).first;
    MS_XFORM_TYPE type = (*it).second.first;
    vcl_pair<void*, void*> data = (*it).second.second;  
    void* data1 = data.first;
    void* data2 = data.second;

    switch (type) {
    case MSXT_TAB_SPLICE:
    case MSXT_A15_SHEET_CONTRACT:
    case MSXT_A14_SHEET_CONTRACT:
      MS = (dbsk3d_ms_sheet*) data1;
      assert (data2 == NULL);
      if (MS != ms_hypg_->sheetmap(MS->id())) { //MS->check_integrity()
        assert (0);
        return false;
      }
    break;
    case MSXT_A5_CURVE_CONTRACT:
    case MSXT_A12A3I_CURVE_CONTRACT:
    case MSXT_A15_CURVE_CONTRACT:
      MC1 = (dbsk3d_ms_curve*) data1;
      assert (data2 == NULL);
      if (MC1 != ms_hypg_->edgemap(MC1->id())) {
        assert (0);
        return false;
      }
    break;
    case MSXT_A1A3II_N_N_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MN2 = (dbsk3d_ms_node*) data2;
      if (MN1 != ms_hypg_->vertexmap(MN1->id())) {
        assert (0);
        return false;
      }
      if (MN2 != ms_hypg_->vertexmap(MN2->id())) {
        assert (0);
        return false;
      }
    break;
    case MSXT_A12A3I_N_C_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      if (MN1 != ms_hypg_->vertexmap(MN1->id())) {
        assert (0);
        return false;
      }
      if (MC2 != ms_hypg_->edgemap(MC2->id())) {
        assert (0);
        return false;
      }
    break;
    case MSXT_A14_C_C_MERGE:
    case MSXT_A1A3II_C_C_MERGE:
      MC1 = (dbsk3d_ms_curve*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      if (MC1 != ms_hypg_->edgemap(MC1->id())) {
        assert (0);
        return false;
      }
      if (MC2 != ms_hypg_->edgemap(MC2->id())) {
        assert (0);
        return false;
      }
    break;
    case MSXT_A1A5_N_C_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      if (MN1 != ms_hypg_->vertexmap(MN1->id())) {
        assert (0);
        return false;
      }
      if (MC2 != ms_hypg_->edgemap(MC2->id())) {
        assert (0);
        return false;
      }
    break;
    default:
      assert (0);
    break;
    }
  }
  return true;
}

void dbsk3d_ms_hypg_trans::_print_Q ()
{
  vul_printf (vcl_cout, "dbsk3d_ms_hypg_trans::print_Q(). Q size %u.\n", ms_xform_Q_.size());
  dbsk3d_ms_sheet* MS = NULL;
  dbsk3d_ms_curve* MC1 = NULL;
  dbsk3d_ms_curve* MC2 = NULL;
  dbsk3d_ms_node* MN1 = NULL;
  dbsk3d_ms_node* MN2 = NULL;
  
  vcl_multimap<float, vcl_pair<char, vcl_pair<void*, void*> > >::iterator it = ms_xform_Q_.begin();
  for (; it != ms_xform_Q_.end(); it++) {
    float cost = (*it).first;
    MS_XFORM_TYPE type = (*it).second.first;
    vcl_pair<void*, void*> data = (*it).second.second;  
    void* data1 = data.first;
    void* data2 = data.second;

    switch (type) {
    case MSXT_TAB_SPLICE:
      MS = (dbsk3d_ms_sheet*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %.2f TAB_SPLICE: S %d.\n", cost, MS->id());
    break;
    case MSXT_A5_CURVE_CONTRACT:
      MC1 = (dbsk3d_ms_curve*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %f.2 A5_CURVE_CONTRACT: C %d.\n", cost, MC1->id());
    break;
    case MSXT_A12A3I_CURVE_CONTRACT:
      MC1 = (dbsk3d_ms_curve*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %f.2 A12A3I_CURVE_CONTRACT: C %d.\n", cost, MC1->id());
    break;
    case MSXT_A15_CURVE_CONTRACT:
      MC1 = (dbsk3d_ms_curve*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %f.2 A15_CURVE_CONTRACT: C %d.\n", cost, MC1->id());
    break;
    case MSXT_A15_SHEET_CONTRACT:
      MS = (dbsk3d_ms_sheet*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %f.2 A15_SHEET_CONTRACT: S %d.\n", cost, MS->id());
    break;
    case MSXT_A14_SHEET_CONTRACT:
      MS = (dbsk3d_ms_sheet*) data1;
      assert (data2 == NULL);
      vul_printf (vcl_cout, "\t %f.2 A14_SHEET_CONTRACT: S %d.\n", cost, MS->id());
    break;
    case MSXT_A1A3II_N_N_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MN2 = (dbsk3d_ms_node*) data2;
      vul_printf (vcl_cout, "\t %f.2 A1A3II_N_N_MERGE: M1 %d N2 %d.\n", cost, MN1->id(), MN2->id());
    break;
    case MSXT_A12A3I_N_C_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      vul_printf (vcl_cout, "\t %f.2 A12A3I_N_C_MERGE: N %d C %d.\n", cost, MN1->id(), MC2->id());
    break;
    case MSXT_A14_C_C_MERGE:
      MC1 = (dbsk3d_ms_curve*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      vul_printf (vcl_cout, "\t %f.2 A14_C_C_MERGE: C1 %d C2 %d.\n", cost, MC1->id(), MC2->id());
    break;
    case MSXT_A1A3II_C_C_MERGE:
      MC1 = (dbsk3d_ms_curve*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      vul_printf (vcl_cout, "\t %f.2 A1A3II_C_C_MERGE: C1 %d C2 %d.\n", cost, MC1->id(), MC2->id());
    break;
    case MSXT_A1A5_N_C_MERGE:
      MN1 = (dbsk3d_ms_node*) data1;
      MC2 = (dbsk3d_ms_curve*) data2;
      vul_printf (vcl_cout, "\t %f.2 A1A5_N_C_MERGE: N %d C %d.\n", cost, MN1->id(), MC2->id());
    break;
    default:
      assert (0);
    break;
    }
  }
  vul_printf (vcl_cout, "\n");
}



