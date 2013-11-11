//: dbsk3d/dbsk3d_fs_xform.h
//  MingChing Chang
//  May 01, 2007        Creation.

#ifndef dbsk3d_fs_xform_h_
#define dbsk3d_fs_xform_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_sheet.h>

//###### Gap xform on fs_edge FE ######

dbmsh3d_face* L_3d_gap_xform (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE);

void undo_L_3d_gap_xform (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE);

dbmsh3d_face* L_add_Delaunay_facet (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE);

//###### Splice xform on fs_face FF ######

//: prune and pass associated generators to remaining shock curves.
bool FF_prune_pass_Gs (dbsk3d_fs_face* FF, const bool reset_E_type = true);

//: Pass associated genes from incident A3 FE's and FV's 
//  (which will be lost if pruning this fs_face) to the given set.
void FF_get_A3_asso_genes_FE_FV (dbsk3d_fs_face* FF,
                                 vcl_map<int, dbmsh3d_vertex*>& A3_asso_genes,
                                 const bool remove_G_from_FE_FV);

//###### Splice xform on fs_sheet S ######

//: Return true if S is valid for a 3D splice transform.
bool S_valid_splice_xform (dbsk3d_fs_sheet* S,
                           vcl_list<vcl_pair<int, int> >& S_to_splice,
                           vcl_vector<dbsk3d_fs_edge*>& C_Lset);

//: Test S is valid for a 3D splice transform on fs_edge FE
//  by testing triple junction on FE (inputP, P1, P2).
int S_valid_splice_on_FE (dbsk3d_fs_sheet* S, dbsk3d_fs_edge* FE, 
                          dbsk3d_fs_face* inputFF, int& S1id, int& S2id);

void replace_S2_in_list (const int S1id, const int S2id, 
                         vcl_list<vcl_pair<int, int> >& S_to_splice);

#endif
