//: MingChing Chang 
//  Oct 5, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_graph_sh_path.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>

#include <dbsk3d/algo/dbsk3d_ms_algos.h>
#include <dbsk3d/algo/dbsk3d_ms_xform.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_hypg_trans.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>

//: Visualization fs_faces that are with 3 or more coarse-scale shock curves.
SoSeparator* vis_SH_elms_n_SCxforms (dbsk3d_ms_hypg* ms_hypg, const int n_xform_th, 
                                     const float curve_width, const float r, const bool show_id)
{
  SoSeparator* root = new SoSeparator;
  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  vcl_vector<vcl_pair<dbsk3d_ms_sheet*, dbsk3d_fs_face*> > SS_P_n_SCxforms;
  get_SS_P_n_SCxforms (ms_hypg, n_xform_th, SS_P_n_SCxforms);

  for (unsigned int i=0; i<SS_P_n_SCxforms.size(); i++) {
    dbsk3d_fs_face* FF = SS_P_n_SCxforms[i].second;    
    root->addChild (draw_fs_face (FF, SbColor (1, 0, 0.5)));
  }
  return root;
}

SoSeparator* vis_A5_curve_contract_xform (dbsk3d_ms_hypg* ms_hypg, int MCid, 
                                          const float curve_width, const float R,
                                          const bool show_id)
{
  SoSeparator* root = new SoSeparator;

  vul_printf (vcl_cout, "\n\nvis_A5_curve_contract_xform(): MC %d.\n", MCid);
  dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap(MCid);
  assert (MC);

  if (MC->all_Es_shared()) {
    vul_printf (vcl_cout, "\tDege. case: all edges are shared_Es.!\n");
    return root;
  }

  //Get the start/end vertices (MN_S, MN_E) of MC.
  dbsk3d_ms_node* MN_S = MC->s_MN();
  dbsk3d_ms_node* MN_E = MC->e_MN();
  assert (MN_S->n_type() == N_TYPE_RIB_END);
  assert (MN_E->n_type() == N_TYPE_RIB_END);
  assert (MN_S->n_incident_Es() == 2);
  assert (MN_E->n_incident_Es() == 2);
  dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) MC->halfedge()->face();
  assert (MC->halfedge()->pair()->face() == MS);
  assert (MC->halfedge()->pair()->pair()->face() == MS);

  //Get the other A3 curves of SCS - MN_S:MC:MN_E - SCE.  
  dbsk3d_ms_curve* MC_S = (dbsk3d_ms_curve*) V_find_other_E (MN_S, MC);
  dbsk3d_ms_curve* MC_E = (dbsk3d_ms_curve*) V_find_other_E (MN_E, MC);
  assert (MC_S->c_type() == C_TYPE_RIB);
  assert (MC_E->c_type() == C_TYPE_RIB); 
  
  //3) Find the shortest path of MU and MV using Dijkstra's shortest path on elms of MS.
  dbsk3d_fs_vertex *M, *U, *V;  
  //Create a temporary mesh MSM (no need to destruct elements when deleting).
  dbmsh3d_mesh* MSM = new dbmsh3d_mesh (MS->facemap(), false);
  vcl_set<dbmsh3d_face*> FF_to_trim_U;
  vcl_set<dbmsh3d_face*> FF_to_trim_V;

  //Find a valid trimming path U-M by avoiding pathes causing non-local changes of ms_hypg topo. 
  vcl_vector<dbmsh3d_edge*> UM_Evec, VM_Evec;
  char s_trim;
  bool r = A5_find_trim_M_path (MC, MC_S, MC_E, MSM, false, M, U, V, UM_Evec, VM_Evec, s_trim);
  if (r == false) //try extensive search
    r = A5_find_trim_M_path (MC, MC_S, MC_E, MSM, true, M, U, V, UM_Evec, VM_Evec, s_trim);

  if (U == NULL) {
    vul_printf (vcl_cout, "\nError: U = NULL. Re-assign U to be MN_SS for visualization.\n");
    U = ((dbsk3d_ms_node*)MC_S->other_V(MN_S))->FV();
  }
  if (V == NULL) {
    vul_printf (vcl_cout, "\nError: V = NULL. Re-assign V to be MN_EE for visualization.\n");
    V = ((dbsk3d_ms_node*)MC_E->other_V(MN_E))->FV();
  }

  if (r)
    vul_printf (vcl_cout, "\nValid M %d, U %d, V %d found, s_trim = %c.\n", 
                M->id(), U->id(), V->id(), s_trim);
  else
    vul_printf (vcl_cout, "\nNo Valid M, U, V found!!\n", M->id(), U->id(), V->id());

  //If Any FE of UM_Evec is not a rib, propagate & label FF's to be pruned.
  if (FEs_all_on_rib (UM_Evec) == false) {
    //Label faces to be pruned in the U side of swallow-tail wing.
    //The starting (seed) fs_faces are on the wing bordering at MC_S.
    //Considering degenerate cases, need to add all possible seeds bordering MC_S.
    vcl_vector<dbsk3d_fs_face*> seedFF_vec;
    vcl_vector<dbmsh3d_edge*> SU_Evec;
    get_rib_trim_Evec (MN_S, MC_S, U, SU_Evec);
    get_rib_trim_seedFF (SU_Evec, UM_Evec, seedFF_vec);

    //The bnd_E_set is the bordering edges where propagation is stopped.
    vcl_set<dbmsh3d_edge*> bnd_E_set;
    bnd_E_set.insert (UM_Evec.begin(), UM_Evec.end());

    //Put all tabMS's incident MC's shared_Es into the bnd_E_set.
    MS->get_incident_C_shared_Es (bnd_E_set);

    for (unsigned int i=0; i<seedFF_vec.size(); i++) {
      dbsk3d_fs_face* seedFF = seedFF_vec[i];
      //Collect all the swallow-tail wing of fs_faces.
      //Stop propagation when the set of fs_edges in UM_Evec are reaches.
      collect_Fs_given_seed_bnd (seedFF, bnd_E_set, MSM, FF_to_trim_U);
    }
  }

  //If Any FE of VM_Evec is not a rib, propagate & label FF's to be pruned.
  if (FEs_all_on_rib (VM_Evec) == false) {
    //Label faces to be pruned in the V side of swallow-tail wing.
    //The starting (seed) fs_faces are on the wing bordering at MC_E.
    //Considering degenerate cases, need to add all possible seeds bordering MC_S.
    vcl_vector<dbsk3d_fs_face*> seedFF_vec;
    vcl_vector<dbmsh3d_edge*> EV_Evec;
    get_rib_trim_Evec (MN_E, MC_E, V, EV_Evec);
    get_rib_trim_seedFF (EV_Evec, VM_Evec, seedFF_vec);

    //The bnd_E_set is the bordering edges where propagation is stopped.
    vcl_set<dbmsh3d_edge*> bnd_E_set;
    bnd_E_set.insert (VM_Evec.begin(), VM_Evec.end());

    //Put all tabMS's incident MC's shared_Es into the bnd_E_set.
    MS->get_incident_C_shared_Es (bnd_E_set);

    for (unsigned int i=0; i<seedFF_vec.size(); i++) {
      dbsk3d_fs_face* seedFF = seedFF_vec[i];
      //Collect all the swallow-tail wing of fs_faces.
      //Stop propagation when the set of fs_edges in MU_Le are reaches.
      collect_Fs_given_seed_bnd (seedFF, bnd_E_set, MSM, FF_to_trim_V);
    }
  }

  assert (FF_to_trim_U.size() + FF_to_trim_V.size() != 0);  

  //========== Start Visualization Here ==========
  vul_printf (vcl_cout, "MC %d: <S %d (%d), E %d (%d)>.\n", 
              MC->id(), MN_S->id(), MN_S->V()->id(), MN_E->id(), MN_E->V()->id());
  dbsk3d_ms_node* MN_SS = (dbsk3d_ms_node*) MC_S->other_V (MC->sV());
  dbsk3d_ms_node* MN_EE = (dbsk3d_ms_node*) MC_E->other_V (MC->eV());
  vul_printf (vcl_cout, "SS %d (%d) - CS %d <%d, %d> -- CE %d <%d, %d> - EE %d (%d).\n",  
              MN_SS->id(), MN_SS->V()->id(), MC_S->id(), MC_S->sV()->id(), MC_S->eV()->id(),
              MC_E->id(), MC_E->sV()->id(), MC_E->eV()->id(), MN_EE->id(), MN_EE->V()->id());

  root->addChild (draw_vertex_vispt_SoCube (M, SbColor(0.75f,0,0), R, show_id));  
  vul_printf (vcl_cout, "M %d, ",  M->id());
  root->addChild (draw_vertex_vispt_SoCube (U, SbColor(0,0.75f,0), R, show_id));
  vul_printf (vcl_cout, "U %d, ",  U->id());
  root->addChild (draw_vertex_vispt_SoCube (V, SbColor(0,0,0.75f), R, show_id));
  vul_printf (vcl_cout, "V %d.\n",  V->id());

  //Draw UM_Evec and VM_Evec.
  vul_printf (vcl_cout, "UM: %d fs_edges: ", UM_Evec.size());
  for (unsigned int i=0; i<UM_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) UM_Evec[i];
    root->addChild (draw_fs_edge (FE, SbColor(1,0.5,0), curve_width, false));
    vul_printf (vcl_cout, "%d ", UM_Evec[i]->id());
  }
  vul_printf (vcl_cout, "\nVM: %d fs_edges: ", VM_Evec.size());
  for (unsigned int i=0; i<VM_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) VM_Evec[i];
    root->addChild (draw_fs_edge (FE, SbColor(0,0.5f,1), curve_width, false));
    vul_printf (vcl_cout, "%d ", VM_Evec[i]->id());
  }
  vul_printf (vcl_cout, "\n");

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //First draw the MS (so the FF_to_trim will show on top).  
  root->addChild (draw_ms_sheet (MS, SbColor (0.5, 0, 0), 0, true, true, true));

  //Draw the FF_to_trim. 
  vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim_U.begin();
  for (; it != FF_to_trim_U.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    root->addChild (draw_fs_face (FF, SbColor (0.5f, 0.5f, 0.5f)));
  }
  it = FF_to_trim_V.begin();
  for (; it != FF_to_trim_V.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    root->addChild (draw_fs_face (FF, SbColor (0.75f, 0.75f, 0.75f)));
  }

  delete MSM;
  return root;
}

SoSeparator* vis_A12A3I_curve_contract_xform (dbsk3d_ms_hypg* ms_hypg, int MCid, 
                                              const float curve_width, const float R,
                                              const bool show_id)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n\nvis_A12A3I_curve_contract_xform(): MC %d.\n", MCid);
  dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap(MCid);

  //1) Setup local configuration for transform.
  //Get the A14 vertex NA14, the A1A3 vertex NA1A3, and the other A1A3 vertex otherN.
  dbsk3d_ms_node *NA14, *NA1A3, *otherN;
  if (MC->s_MN()->n_type() == N_TYPE_RIB_END) {
    NA14 = MC->e_MN();
    NA1A3 = MC->s_MN();
  }
  else {
    NA14 = MC->s_MN();
    NA1A3 = MC->e_MN();
  }
  assert (NA14->n_type() == N_TYPE_AXIAL_END); // || NA14->n_type() == N_TYPE_DEGE
  assert (NA1A3->n_type() == N_TYPE_RIB_END);

  dbmsh3d_sheet *tabS, *baseS;
  bool r = C_get_non_dup_S (MC, tabS, baseS);
  assert (r);
  dbsk3d_ms_sheet *tabMS = (dbsk3d_ms_sheet*) tabS;
  dbsk3d_ms_sheet *baseMS = (dbsk3d_ms_sheet*) baseS;

  dbsk3d_ms_curve* ribMC = (dbsk3d_ms_curve*) tabMS->find_next_bnd_C (NA1A3, MC);
  otherN = (dbsk3d_ms_node*) ribMC->other_V (NA1A3);  

  //Find the other axial medial curve aMC.
  dbmsh3d_curve* otherC;
  dbmsh3d_face* F_M;
  r = (dbsk3d_ms_curve*) tabMS->get_otherC_via_F (MC, NA14, otherC, F_M);  
  assert (r);
  dbsk3d_ms_curve* aMC = (dbsk3d_ms_curve*) otherC;

  //2) Find a valid trimming path from NA14 to a vertex U on ribMC.
  dbmsh3d_vertex *M = NULL, *U = NULL;
  bool M_on_aMC;  
  vcl_vector<dbmsh3d_edge*> E_aMC;
  vcl_vector<dbmsh3d_edge*> UM_Evec; //trimming path (edges) from U to M.
  vcl_set<dbmsh3d_face*> FF_to_trim;

  r = A12A3I_valid_trim_M_U (tabMS, MC, ribMC, aMC, NA14, NA1A3, F_M, 
                             M, U, M_on_aMC, E_aMC, UM_Evec, FF_to_trim);

  if (M == NULL) {
    vul_printf (vcl_cout, "\nError: M = NULL. Re-assign M to be NA14 for visualization.\n");
    M = NA14->FV();
  }
  if (U == NULL) {
    vul_printf (vcl_cout, "\nError: U = NULL. Re-assign U to be NA1A3 for visualization.\n");
    U = NA1A3->FV();
  }
  
  vul_printf (vcl_cout, "MC %d: NA14 %d (%d), NA1A3 %d (%d)>.\n", 
              MC->id(), NA14->id(), NA14->V()->id(), NA1A3->id(), NA1A3->V()->id());
  vul_printf (vcl_cout, "ribMC %d (FE %d, ..., FE %d).\n",
              ribMC->id(), ribMC->E_vec(0)->id(), ribMC->E_vec_last()->id()); 
  vul_printf (vcl_cout, "aMC %d (FE %d, ..., FE %d).\n",
              aMC->id(), aMC->E_vec(0)->id(), aMC->E_vec_last()->id());

  if (r == false) {
    vul_printf (vcl_cout, "Can't perform A12A3I_curve_contract_xform()!\n");
    return root;
  }

  if (FF_to_trim.size() == 0) {
    vul_printf (vcl_cout, "FF_to_trim.size() == 0.\n");
  }

  //========== Start Visualization Here ==========
  root->addChild (draw_vertex_vispt_SoCube (M, SbColor(0.75f,0,0), R, show_id));  
  vul_printf (vcl_cout, "M %d, ",  M->id());
  root->addChild (draw_vertex_vispt_SoCube (U, SbColor(0,0.75f,0), R, show_id));
  vul_printf (vcl_cout, "U %d.\n",  U->id());
  vul_printf (vcl_cout, "F_M %d, M_on_aMC %s.\n", F_M->id(), M_on_aMC ? "true" : "false");

  //Draw E_aMC.
  vul_printf (vcl_cout, "E_aMC: %d fs_edges: ", E_aMC.size());
  for (unsigned int i=0; i<E_aMC.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) E_aMC[i];
    root->addChild (draw_fs_edge (FE, SbColor(0.5,1,0), curve_width, false));
    vul_printf (vcl_cout, "%d ", E_aMC[i]->id());
  }
  vul_printf (vcl_cout, "\n");

  //Draw UA_Evec.
  vul_printf (vcl_cout, "UM: %d fs_edges: ", UM_Evec.size());
  for (unsigned int i=0; i<UM_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) UM_Evec[i];
    root->addChild (draw_fs_edge (FE, SbColor(1,0.5,0), curve_width, false));
    vul_printf (vcl_cout, "%d ", UM_Evec[i]->id());
  }
  vul_printf (vcl_cout, "\n");

  //Draw F_M.
  dbsk3d_fs_face* FF_M = (dbsk3d_fs_face*) F_M;
  root->addChild (draw_fs_face (FF_M, SbColor (0.5f, 1.0f, 0.5f)));

  //Draw the FF_to_trim.  
  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);
  vcl_set<dbmsh3d_face*>::iterator it = FF_to_trim.begin();
  for (; it != FF_to_trim.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it);
    root->addChild (draw_fs_face (FF, SbColor (0.5f, 0.5f, 0.5f)));
  }

  return root;
}

SoSeparator* vis_all_merge_xforms (dbsk3d_ms_hypg_trans* mht, 
                                   const int tab_th, const int A5_th,
                                   const int cc_th, const int sc_th, 
                                   const int nnm_th, const int ncm_th, const int ccm_th, 
                                   const int ncm_a1a5_th, const int cmxth, 
                                   const float curve_width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;

  //Brute-force debug:
  vul_printf (vcl_cout, "\tms_hypg.check_integrity() ");
  if (mht->ms_hypg()->check_integrity())
    vul_printf (vcl_cout, "pass.");
  else
    vul_printf (vcl_cout, "fail!");

  vul_printf (vcl_cout, "\nperform_ms_trans_regul():\n");
  assert (mht->fs_mesh()->is_MHE());

  mht->ms_trans_regul_set_params (0.5, tab_th, 1, cc_th, 0.3, A5_th, 0.8, sc_th,
                                  1, nnm_th, 1, ncm_th, 1, ccm_th, ncm_a1a5_th, cmxth);
  //Initialize the xform queue.
  mht->ms_trans_regul_init ();

  int n_tab_splice = 0;
  int n_a5_c_contract = 0;
  int n_a12a3i_c_contract = 0;
  int n_a15_c_contract = 0;
  int n_a15_s_contract = 0;
  int n_a14_s_contract = 0;
  int n_a1a3ii_nn_merge = 0;
  int n_a12a3i_nc_merge = 0;
  int n_a14_cc_merge = 0;
  int n_a1a3ii_cc_merge = 0;
  int n_a1a5_nc_merge = 0;

  //Visualize each merge transform in order.
  while (mht->ms_xform_Q().size() != 0) {
    float cost;
    MS_XFORM_TYPE type;
    dbsk3d_ms_sheet* MS;
    dbsk3d_ms_curve *MC1, *MC2;
    dbsk3d_ms_node *MN1, *MN2;

    mht->_pop_xform_from_Q (cost, type, MS, MC1, MC2, MN1, MN2);
    
    switch (type) {
    case MSXT_TAB_SPLICE: //A1A3-I or A12A3-II sheet-splice transform.
      n_tab_splice++;
    break;
    case MSXT_A5_CURVE_CONTRACT: //A5 swallow-tail curve-contract transform.
      n_a5_c_contract++;
    break;
    case MSXT_A12A3I_CURVE_CONTRACT: //A12A3-I curve-contract transform.
      n_a12a3i_c_contract++;
    break;
    case MSXT_A15_CURVE_CONTRACT: //A15 curve-contract transform.
      n_a15_c_contract++;
    break;
    case MSXT_A15_SHEET_CONTRACT: //A15 sheet-contract transform.
      n_a15_s_contract++;
    break;
    case MSXT_A14_SHEET_CONTRACT: //A14 sheet-contract transform.
      n_a14_s_contract++;
    break;
    case MSXT_A1A3II_N_N_MERGE: //A1A3-II node-node merge transform.
      root->addChild (vis_A1A3II_n_n_merge_xform (MN1, MN2, cost, curve_width));
      n_a1a3ii_nn_merge++;
    break;
    case MSXT_A12A3I_N_C_MERGE: //A12A3-I node-curve merge transform.
      root->addChild (vis_A12A3I_n_c_merge_xform (MN1, MC2, cost, mht->cmxth(), curve_width, cube_size));
      n_a12a3i_nc_merge++;
    break;    
    case MSXT_A14_C_C_MERGE: //A14 curve-curve merge transform.
      root->addChild (vis_A14_c_c_merge_xform (MC1, MC2, cost, mht->cmxth(), curve_width, cube_size));
      n_a14_cc_merge++;
    break;
    case MSXT_A1A3II_C_C_MERGE: //A1A3-II curve-curve merge transform.
      root->addChild (vis_A1A3II_c_c_merge_xform (MC1, MC2, cost, mht->cmxth(), curve_width, cube_size));
      n_a1a3ii_cc_merge++;
    break;
    case MSXT_A1A5_N_C_MERGE: //A1A5 node-curve merge transform.
      root->addChild (vis_A1A5_n_c_merge_xform (MN1, MC2, cost, mht->cmxth(), curve_width, cube_size));
      n_a1a5_nc_merge++;
    break; 
    default:
      assert (0);
    break;
    }
  }

  vul_printf (vcl_cout, "\n\n    Candidate xforms in queue:\n");
  vul_printf (vcl_cout, "    %3d A1A3-I or A12A3-II Sheet-Splice xforms.\n", n_tab_splice);
  vul_printf (vcl_cout, "    %3d A5 Curve-Contract xforms.\n", n_a5_c_contract);  
  vul_printf (vcl_cout, "    %3d A12A3-I Curve-Contract xforms.\n", n_a12a3i_c_contract);  
  vul_printf (vcl_cout, "    %3d A15 Curve-Contract xforms.\n", n_a15_c_contract);  
  vul_printf (vcl_cout, "    %3d A15 Sheet-Contract xforms.\n", n_a15_s_contract);  
  vul_printf (vcl_cout, "    %3d A14 Sheet-Contract xforms.\n", n_a14_s_contract);  
  vul_printf (vcl_cout, "    %3d A1A3-II Node-Node Merge xforms.\n", n_a1a3ii_nn_merge);  
  vul_printf (vcl_cout, "    %3d A12A3-I Node-Curve Merge xforms.\n", n_a12a3i_nc_merge);
  vul_printf (vcl_cout, "    %3d A14 Curve-Curve Merge xforms.\n", n_a14_cc_merge);
  vul_printf (vcl_cout, "    %3d A1A3-II Curve-Curve Merge xforms.\n", n_a1a3ii_cc_merge);
  vul_printf (vcl_cout, "    %3d A1A5 Node-Curve Merge xforms.\n\n", n_a1a5_nc_merge);

  return root;
}

SoSeparator* vis_A1A3II_n_n_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2, 
                                         const float cost, const float curve_width)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n  vis_A1A3II_n_n_merge_xform() on MN1 %d and MN2 %d cost %f: \n\t", 
              MN1->id(), MN2->id(), cost);

  dbsk3d_ms_node* MN2new;
  dbsk3d_ms_sheet* baseMS;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  bool r = find_A1A3II_n_n_merge_N (MN1, MN2new, baseMS, shortest_Evec);
  assert (r);
  assert (MN2new == MN2);

  //Visualize the shortest_Evec path in FE's.
  SbColor col = color_from_code (COLOR_RANDOM);
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) shortest_Evec[i];
    root->addChild (draw_fs_edge (FE, col, curve_width, false));
  }

  //Print path info (back tracking).
  vul_printf (vcl_cout, "MN2 %d (%d), MN1 %d (%d).", MN2->id(), MN2->V()->id(), MN1->id(), MN1->V()->id());
  vul_printf (vcl_cout, "path: ");
  dbmsh3d_vertex* V = MN2->V();
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    vul_printf (vcl_cout, "V%d - E%d - ", V->id(), E->id());
    V = E->other_V (V);
  }
  assert (V == MN1->V());
  vul_printf (vcl_cout, "V%d.\n", V->id());

  return root;
}

SoSeparator* vis_A12A3I_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                                         const float cost, const float cmxth, 
                                         const float curve_width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n  vis_A12A3I_n_c_merge_xform() on MN1 %d and MC2 %d cost %f: \n\t", 
              MN1->id(), MC2->id(), cost);

  dbsk3d_ms_curve* MC2new;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  bool r = find_A12A3I_n_c_merge_C (MN1, MC2new, baseMS, closest_V, shortest_Evec, cmxth);
  if (r == false) {
    vul_printf (vcl_cout, "no MC2new found.\n");
    return root;
  }
  vul_printf (vcl_cout, "MC2new %d different from MC2 %d.\n\t", MC2new->id(), MC2->id());

  //Visualize the shortest_Evec path in FE's.
  SbColor col = color_from_code (COLOR_RANDOM);
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) shortest_Evec[i];
    root->addChild (draw_fs_edge (FE, col, curve_width, false));
  }

  //Visualize the closest_V in FV.
  dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) closest_V;
  root->addChild (draw_fs_vertex (FV, col, cube_size));

  //Print path info (back tracking).
  vul_printf (vcl_cout, "MC2 %d (FV %d), MN1 %d (%d).", MC2->id(), closest_V->id(), MN1->id(), MN1->V()->id());
  vul_printf (vcl_cout, "path: ");
  dbmsh3d_vertex* V = closest_V;
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    vul_printf (vcl_cout, "V%d - E%d - ", V->id(), E->id());
    V = E->other_V (V);
  }
  assert (V == MN1->V());
  vul_printf (vcl_cout, "V%d.\n", V->id());

  return root;
}

SoSeparator* vis_A14_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                      const float cost, const float cmxth,
                                      const float curve_width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n  vis_A14_c_c_merge_xform() on MC1 %d and MC2 %d cost %f: \n\t", 
              MC1->id(), MC2->id(), cost);

  dbsk3d_ms_curve* MC2new;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  bool r = find_A14_c_c_merge_C (MC1, MC2new, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth);
  if (r == false) {
    vul_printf (vcl_cout, "no MC2new found.\n");
    return root;
  }
  vul_printf (vcl_cout, "MC2new %d different from MC2 %d.\n\t", MC2new->id(), MC2->id());

  //Visualize the shortest_Evec path in FE's.
  SbColor col = color_from_code (COLOR_RANDOM);
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) shortest_Evec[i];
    root->addChild (draw_fs_edge (FE, col, curve_width, false));
  }

  //Visualize the closest_V1 and closest_V2 in FV.
  dbsk3d_fs_vertex* FV1 = (dbsk3d_fs_vertex*) closest_V1;
  dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) closest_V2;
  root->addChild (draw_fs_vertex (FV1, col, cube_size));
  root->addChild (draw_fs_vertex (FV2, col, cube_size));

  //Print path info (back tracking).
  vul_printf (vcl_cout, "MC2 %d (FV2 %d), MC1 %d (FV1 %d).", MC2->id(), closest_V2->id(), MC1->id(), closest_V1->id());
  vul_printf (vcl_cout, "path: ");
  dbmsh3d_vertex* V = closest_V2;
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    vul_printf (vcl_cout, "V%d - E%d - ", V->id(), E->id());
    V = E->other_V (V);
  }
  assert (V == closest_V1);
  vul_printf (vcl_cout, "V%d.\n", V->id());

  return root;
}



SoSeparator* vis_A1A3II_c_c_merge_xform (dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                                         const float cost, const float cmxth,
                                         const float curve_width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n  vis_A1A3II_c_c_merge_xform() on MC1 %d and MC2 %d cost %f: \n\t", 
              MC1->id(), MC2->id(), cost);

  dbsk3d_ms_curve* MC2new;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex *closest_V1, *closest_V2;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  bool r = find_A1A3II_c_c_merge_C (MC1, MC2new, baseMS, closest_V1, closest_V2, shortest_Evec, cmxth);
  if (r == false) {
    vul_printf (vcl_cout, "no MC2new found.\n");
    return root;
  }
  vul_printf (vcl_cout, "MC2new %d different from MC2 %d.\n\t", MC2new->id(), MC2->id());

  //Visualize the shortest_Evec path in FE's.
  SbColor col = color_from_code (COLOR_RANDOM);
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) shortest_Evec[i];
    root->addChild (draw_fs_edge (FE, col, curve_width, false));
  }

  //Visualize the closest_V1 and closest_V2 in FV.
  dbsk3d_fs_vertex* FV1 = (dbsk3d_fs_vertex*) closest_V1;
  dbsk3d_fs_vertex* FV2 = (dbsk3d_fs_vertex*) closest_V2;
  root->addChild (draw_fs_vertex (FV1, col, cube_size));
  root->addChild (draw_fs_vertex (FV2, col, cube_size));

  //Print path info (back tracking).
  vul_printf (vcl_cout, "MC2 %d (FV2 %d), MC1 %d (FV1 %d).", MC2->id(), closest_V2->id(), MC1->id(), closest_V1->id());
  vul_printf (vcl_cout, "path: ");
  dbmsh3d_vertex* V = closest_V2;
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    vul_printf (vcl_cout, "V%d - E%d - ", V->id(), E->id());
    V = E->other_V (V);
  }
  assert (V == closest_V1);
  vul_printf (vcl_cout, "V%d.\n", V->id());

  return root;
}

SoSeparator* vis_A1A5_n_c_merge_xform (dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2, 
                                       const float cost, const float cmxth,
                                       const float curve_width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;
  vul_printf (vcl_cout, "\n  vis_A1A5_n_c_merge_xform() on MN1 %d and MC2 %d cost %f: \n\t", 
              MN1->id(), MC2->id(), cost);

  dbsk3d_ms_curve* MC2new;
  dbsk3d_ms_sheet* baseMS;
  dbmsh3d_vertex* closest_V;
  vcl_vector<dbmsh3d_edge*> shortest_Evec;
  bool r = find_A1A5_n_c_merge_C (MN1, MC2new, baseMS, closest_V, shortest_Evec, cmxth);
  if (r == false) {
    vul_printf (vcl_cout, "no MC2new found.\n");
    return root;
  }
  vul_printf (vcl_cout, "MC2new %d different from MC2 %d.\n\t", MC2new->id(), MC2->id());

  //Visualize the shortest_Evec path in FE's.
  SbColor col = color_from_code (COLOR_RANDOM);
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) shortest_Evec[i];
    root->addChild (draw_fs_edge (FE, col, curve_width, false));
  }

  //Visualize the closest_V in FV.
  dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) closest_V;
  root->addChild (draw_fs_vertex (FV, col, cube_size));

  //Print path info (back tracking).
  vul_printf (vcl_cout, "MC2 %d (FV %d), MN1 %d (%d).", MC2->id(), closest_V->id(), MN1->id(), MN1->V()->id());
  vul_printf (vcl_cout, "path: ");
  dbmsh3d_vertex* V = closest_V;
  for (unsigned int i=0; i<shortest_Evec.size(); i++) {
    dbmsh3d_edge* E = shortest_Evec[i];
    vul_printf (vcl_cout, "V%d - E%d - ", V->id(), E->id());
    V = E->other_V (V);
  }
  assert (V == MN1->V());
  vul_printf (vcl_cout, "V%d.\n", V->id());

  return root;
}



