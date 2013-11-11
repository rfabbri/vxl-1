//: dbsk3d/dbsk3d_ms_xform.h
//  MingChing Chang
//  Sep 10, 2007        Creation.

#ifndef dbsk3d_ms_xform_h_
#define dbsk3d_ms_xform_h_

#include <dbsk3d/dbsk3d_ms_hypg.h>

//###### Splice xform on ms_sheet MS ######

bool MS_valid_for_splice_xform (dbsk3d_ms_sheet* MS);

bool get_2_A1A3s_from_axials (vcl_vector<dbsk3d_ms_curve*>& bnd_A13_MCs,
                              dbsk3d_ms_node*& A1A3_1, dbsk3d_ms_node*& A1A3_2);

//: Remove the shared_E on the splicing curve MC during a splice xform.
void splice_remove_MC_shared_Es (dbsk3d_ms_curve* MC, dbsk3d_ms_sheet* MS,
                                 vcl_set<dbsk3d_ms_curve*>& modified_MC_set);

//: Splice the two sheets together: merge the smaller to the larger sheet.
//  delete the MC after the merge (but leave the ending MN's, will be deleted later).
void MS_splice_ms_sheets (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_curve* MC, 
                          dbsk3d_ms_sheet* SS1, dbsk3d_ms_sheet* SS2);

//###### A5 Curve-Contract xform on ms_curve MC ######

bool MC_valid_for_A5_contract_xform (dbsk3d_ms_curve* MC);

bool A5_valid_trim_M_U_V (dbsk3d_ms_curve* MC, dbsk3d_fs_vertex* & M, 
                          vcl_set<dbmsh3d_face*>& Pset_to_prune);

//: Find a valid trimming path U-M for the A5 transform 
//  avoiding pathes causing non-local changes of ms_hypg topo.
bool A5_find_trim_M_path (const dbsk3d_ms_curve* MC, 
                          const dbsk3d_ms_curve* MC_S, const dbsk3d_ms_curve* MC_E, 
                          dbmsh3d_mesh* MSM, const bool b_extensive_search,
                          dbsk3d_fs_vertex*& M, dbsk3d_fs_vertex*& U, dbsk3d_fs_vertex*& V,
                          vcl_vector<dbmsh3d_edge*>& UM_Evec, vcl_vector<dbmsh3d_edge*>& VM_Evec,
                          char& b_single_trim);

//: Find a valid trimming path by avoiding pathes causing non-local changes of ms_hypg topo. 
bool A5_get_trim_path (const dbsk3d_ms_curve* MC, const bool MC_s_side, const dbsk3d_ms_curve* rib_MC, 
                       dbmsh3d_mesh* MSM, const dbsk3d_fs_vertex* M, const bool b_extensive_search,
                       dbsk3d_fs_vertex*& U, vcl_vector<dbmsh3d_edge*>& UM_Evec);

void A5_get_trim_path_constraints (const dbsk3d_ms_curve* MC, const bool MC_s_side,
                                   const dbsk3d_ms_curve* rib_MC,
                                   const dbsk3d_fs_vertex* M, const dbsk3d_fs_vertex* U,
                                   vcl_set<dbmsh3d_edge*>& avoid_Eset,
                                   vcl_set<dbmsh3d_vertex*>& avoid_Vset);

//If the A3 curve is a loop, make U as the loop_vertex.
void A5_contract_merge_rib_loop (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, 
                                 dbsk3d_ms_curve* SCs, dbsk3d_fs_vertex* M);

//Merge the 2 A3 shock ribs into a single curve passing through M.
//Return the merged curve.
dbsk3d_ms_curve* A5_contract_merge_ribs (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS,
                                         dbsk3d_ms_node* SVs, dbsk3d_ms_node* SVe, 
                                         dbsk3d_ms_curve* SCs, dbsk3d_ms_curve* SCe);

dbsk3d_fs_edge* FF_to_trim_incident_to_otherMC (vcl_set<dbmsh3d_face*> FF_to_trim, 
                                                const dbsk3d_ms_curve* MC);

//###### Functions Shared by A5 and A12A3-I Curve-Contract xforms ######

bool FEs_all_on_rib (const vcl_vector<dbmsh3d_edge*>& Evec);

void get_rib_trim_Evec (const dbsk3d_ms_node* MN, const dbsk3d_ms_curve* MC, 
                        const dbmsh3d_vertex* U, vcl_vector<dbmsh3d_edge*>& Evec);

bool get_rib_trim_seedFF (const vcl_vector<dbmsh3d_edge*>& rib_Evec,
                          const vcl_vector<dbmsh3d_edge*>& bnd_Evec,
                          vcl_vector<dbsk3d_fs_face*>& seedFF_vec);

bool A12A3I_get_rib_trim_seedFF (const vcl_vector<dbmsh3d_edge*>& rib_Evec,
                                 const vcl_vector<dbmsh3d_edge*>& bnd_Evec,
                                 const dbmsh3d_face* F_M,
                                 vcl_vector<dbsk3d_fs_face*>& seedFF_vec);

//: Perform trim xform on the fine-scale shock elements.
void perform_trim_xform (dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_sheet* MS, 
                         vcl_set<dbmsh3d_face*>& FF_to_trim);

void perform_trim_xform (dbsk3d_fs_mesh* fs_mesh, vcl_set<dbmsh3d_face*>& FF_to_trim);

void _del_MC_from_MS (dbsk3d_ms_hypg* ms_hypg, dbsk3d_ms_sheet* MS, dbsk3d_ms_curve* MC);

void _trace_rib_MC_FEs (dbsk3d_ms_curve* MC, const dbsk3d_ms_node* startMN, const dbmsh3d_vertex* endV);

//###### A12A3-I Curve-Contract xform on ms_curve MC ######

bool MC_valid_for_A12A3I_contract_xform (dbsk3d_ms_curve* MC);

//: Check if the A12A3-I Curve-Contract transform is valid or not.
//  Return false if such transform can not be performed.
bool A12A3I_valid_trim_M_U (dbsk3d_ms_sheet* tabMS, const dbsk3d_ms_curve* MC, 
                            const dbsk3d_ms_curve* ribMC, const dbsk3d_ms_curve* aMC,
                            const dbsk3d_ms_node* NA14, const dbsk3d_ms_node* NA1A3, const dbmsh3d_face* F_M,
                            dbmsh3d_vertex*& M_final, dbmsh3d_vertex*& U_final, 
                            bool& M_on_aMC, vcl_vector<dbmsh3d_edge*>& E_aMC, 
                            vcl_vector<dbmsh3d_edge*>& UM_Evec_final, vcl_set<dbmsh3d_face*>& FF_to_trim);

//: Find a valid trimming path by avoiding pathes causing non-local changes of ms_hypg topo. 
bool A12A3I_get_trim_path (const dbsk3d_ms_curve* MC, const dbsk3d_ms_curve* ribMC, 
                           dbsk3d_ms_sheet* tabMS, dbmsh3d_mesh* MSM, 
                           const dbmsh3d_vertex* M, const dbsk3d_ms_node* B, 
                           dbmsh3d_vertex* & U, vcl_vector<dbmsh3d_edge*>& UA_Evec);

void A12A3I_contract_trace_rib (dbsk3d_ms_hypg* ms_hypg, 
                                dbsk3d_ms_sheet* tabMS, dbsk3d_ms_sheet* baseMS, 
                                dbsk3d_ms_curve* MC, dbsk3d_ms_curve* ribMC, dbsk3d_ms_curve* aMC, 
                                const dbmsh3d_vertex* M, const bool M_on_aMC, vcl_vector<dbmsh3d_edge*>& MA_Evec, 
                                dbsk3d_ms_node* NA14, const dbsk3d_ms_node* otherN);

//###### A15 Curve-Contract xform on ms_curve MC ######

bool MC_valid_for_A15_contract_xform (dbsk3d_ms_curve* MC);

//###### A15 Sheet-Contract xform on ms_sheet MS ######

bool MS_valid_for_A15_contract_xform (dbsk3d_ms_sheet* MS);

//###### A14 Sheet-Contract xform on ms_sheet MS ######

bool MS_valid_for_A14_contract_xform (dbsk3d_ms_sheet* MS);

//###### A1A3-II Node-Node Merge xform on ms_nodes MN and MN2 ######

#define MERGE_XFORM_COST_DELTA  0.01f

//: Given a ms_node MN, try finding a ms_node MN2 for the transform.
bool find_A1A3II_n_n_merge_N (const dbsk3d_ms_node* MN, dbsk3d_ms_node*& closest_MN2, 
                              dbsk3d_ms_sheet*& baseMS, vcl_vector<dbmsh3d_edge*>& shortest_Evec);

//###### A12A3-I Node-Curve Merge xform on ms_node MN and ms_curve MC ######

//: Given a ms_node MN, try finding a ms_curve MC for the transform.
bool find_A12A3I_n_c_merge_C (const dbsk3d_ms_node* MN1, dbsk3d_ms_curve*& MC2, 
                              dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& closestV, 
                              vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                              const int cmxth);

//: Given a ms_curve MC, try finding a ms_node MN for the transform.
bool find_A12A3I_c_n_merge_N (const dbsk3d_ms_curve* MC1, dbsk3d_ms_node*& closest_MN2, 
                              dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& closestV, 
                              vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                              const int cmxth);

//###### A14 Curve-Curve Merge xform on ms_curves MC and MC2 ######

//: Given a ms_curve MC, try finding a ms_curve MC2 for the transform.
bool find_A14_c_c_merge_C (const dbsk3d_ms_curve* MC, dbsk3d_ms_curve*& MC2, 
                           dbsk3d_ms_sheet*& baseMS, 
                           dbmsh3d_vertex*& closest_V1, dbmsh3d_vertex*& closest_V2, 
                           vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                           const int cmxth);

//###### A1A3-II Curve-Curve Merge xform on ms_curves MC and MC2 ######
//: Given a ms_curve MC, try finding a ms_curve MC2 for the transform.
bool find_A1A3II_c_c_merge_C (const dbsk3d_ms_curve* MC, dbsk3d_ms_curve*& MC2, 
                              dbsk3d_ms_sheet*& baseMS, 
                              dbmsh3d_vertex*& closest_V1, dbmsh3d_vertex*& closest_V2, 
                              vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                              const int cmxth);

//###### A1A5 Node-Curve Merge xform on ms_node MN and ms_curve MC ######

//: Given an A1A3 (or AmAn) node N, search for an A3 curve C to merge.
bool find_A1A5_n_c_merge_C (const dbsk3d_ms_node* MN1, dbsk3d_ms_curve*& MC2, 
                            dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& closestV, 
                            vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                            const int cmxth);

//: Given an A3 curve C, search for an A1A3 (or AmAn) node N to merge.
bool find_A1A5_n_c_merge_N (const dbsk3d_ms_curve* MC1, dbsk3d_ms_node*& closest_MN2, 
                            dbsk3d_ms_sheet*& baseMS, dbmsh3d_vertex*& closestV, 
                            vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                            const int cmxth);

//###### Functions shared by the merge transforms  ######

//: The node-node merging operation
void MN_MN_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_node* MN1, dbsk3d_ms_node* MN2,
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec);

//: The node-curve merging operation
void MN_MC_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2,               
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                  const dbmsh3d_vertex* closest_V, dbsk3d_ms_curve*& MC2_add);

//: return the newly created ms_curve.
dbsk3d_ms_curve* merge_MN_to_MC (dbsk3d_ms_hypg* MSH, dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC2,
                                 const dbsk3d_ms_sheet* baseMS, const dbmsh3d_vertex* closest_V);

//: The curve-curve merging operation
void MC_MC_merge (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* baseMS,
                  dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC2, 
                  const vcl_vector<dbmsh3d_edge*>& shortest_Evec,
                  const dbmsh3d_vertex* closest_V1, const dbmsh3d_vertex* closest_V2,
                  dbsk3d_ms_curve*& MC1_add, dbsk3d_ms_curve*& MC2_add,
                  dbsk3d_ms_node*& MN_add);

//: divide the ms_sheet MS into two and return the new ms_sheet MSn.
dbsk3d_ms_sheet* MC_MC_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, dbsk3d_ms_node* MN_add, 
                                        dbsk3d_ms_curve* MC1, dbsk3d_ms_curve* MC1_add, 
                                        dbsk3d_ms_curve* MC2, dbsk3d_ms_curve* MC2_add);

//: check and divide the ms_sheet MS into two.
//  If a new ms_sheet MSn is created, return it.
dbsk3d_ms_sheet* MN_MC_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, 
                                        dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC, 
                                        dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MC_add);

//: Update the boundary halfedges of MS and MSn for the case
//  that the tabMC is an i-curve of MS.
void update_bnd_HE_MS_MSn_icurve (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                  dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC,
                                  dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MC_add);

void update_HE_single_IC_chain (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                vcl_vector<dbmsh3d_edge*>& icurve_otherN_N,
                                dbmsh3d_vertex* otherN);

void update_HE_general (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                        vcl_vector<vcl_vector<dbmsh3d_edge*> >& icurve_bndN_otherN,
                        vcl_vector<dbmsh3d_edge*>& icurve_loop_E_heads,
                        dbmsh3d_vertex* bndN, dbmsh3d_vertex* endN);

//Connect each icurve_loop_E_heads[i].
void connect_breaking_HE_loop (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                               dbmsh3d_halfedge* loop_HE_start, 
                               dbmsh3d_vertex* Ns, dbmsh3d_vertex* Ne,
                               vcl_vector<dbmsh3d_halfedge*>& MS_HEs, 
                               vcl_vector<dbmsh3d_halfedge*>& MSn_HEs);

//: Update the boundary halfedges of MS and MSn for the case
//  that the tabMC is an i-curve of MS.
void update_bnd_HE_MS_MSn_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, 
                                dbsk3d_ms_node* MN, dbsk3d_ms_curve* tabMC,
                                dbsk3d_ms_curve* MC, dbsk3d_ms_curve* MC_add);

bool merge_test_divide_MS (dbsk3d_ms_sheet* baseMS, 
                           const vcl_vector<dbmsh3d_edge*>& shortest_Evec);

dbsk3d_ms_sheet* MN_MN_merge_divide_MS (dbsk3d_ms_hypg* MSH, dbsk3d_ms_sheet* MS, 
                                        dbsk3d_ms_node* MN1, dbsk3d_ms_curve* MC1, 
                                        dbsk3d_ms_node* MN2, dbsk3d_ms_curve* MC2);

void split_S_move_icurves (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn);

//: Check split S 3-inc.
void split_S_check_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn);

void check_split_3inc (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn, dbmsh3d_halfedge* HE);

void get_dispatch_bnd_chain (dbsk3d_ms_sheet* MS, dbsk3d_ms_sheet* MSn,
                             vcl_set<dbmsh3d_halfedge*>& IC_pair_to_split, 
                             vcl_vector<dbmsh3d_halfedge*>& C_HEvec, 
                             vcl_vector<dbmsh3d_halfedge*>& Co_HEvec, 
                             dbmsh3d_vertex*& C_sV, dbmsh3d_vertex*& C_eV);

void update_bnd_chain (dbsk3d_ms_sheet* MS, 
                       vcl_vector<dbmsh3d_halfedge*>& C_HEvec, 
                       vcl_vector<dbmsh3d_halfedge*>& C2_HEvec,
                       dbmsh3d_halfedge* otherC_HE_next);

//###### Functions for special case involving two or more xforms ######

dbsk3d_fs_face* A5_contract_L_find_trim_P (dbsk3d_fs_edge* A13L);

#endif
