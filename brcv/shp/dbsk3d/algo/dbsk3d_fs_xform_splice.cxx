//: dbsk3d_fs_xform_splice.cxx
//: MingChing Chang
//  May 01, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>

//###############################################################
//  Splice xform on dbsk3d_fs_face FF
//###############################################################

//: Pass FF's unassociated generators to remaining adjacent shock links.
bool FF_prune_pass_Gs (dbsk3d_fs_face* FF, const bool reset_E_type)
{
  vcl_map<int, dbmsh3d_vertex*> asso_genes;
  FF_get_A3_asso_genes_FE_FV (FF, asso_genes, true);

  //Prune the fs_face FF.
  FF->set_valid (false);

  //Add this FF's 2 genes
  asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (FF->genes(0)->id(), FF->genes(0)));
  asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (FF->genes(1)->id(), FF->genes(1)));

  //Go through all bnd_links which will remain (after pruning of this FF) 
  //and check pass genes to it.
  //Do not pass generators if it is already in the adjacent patch's.
  bool success = false;
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    //Detect if FE remains after pruning of this FF.
    if (FE->count_valid_Fs() > 1) { 
      FE->check_add_asgn_Gs_to_set (asso_genes, FF);
      success = true;
    }
    if (reset_E_type) //Re-determine FE's type.
      FE->compute_e_type ();

    HE = HE->next();
  }
  while (HE != FF->halfedge());

  #if DBMSH3d_DEBUG>2
  if (success == false)
    vul_printf (vcl_cout, "\tpruned_pass_genes(): error !! FF %d.\n", FF->id());
  #endif
  ///assert (success == true);
  return success;
}

//: Get associated genes from incident A3 FE's and FV's 
//  (which will be lost if pruning this fs_face) to the given set.
void FF_get_A3_asso_genes_FE_FV (dbsk3d_fs_face* FF,
                                 vcl_map<int, dbmsh3d_vertex*>& A3_asso_genes,
                                 const bool remove_G_from_FE_FV)
{
  //Go through all incident shock links and look for A3-Rib-type FE.
  //Add all FE's assigned Gs to A3_asso_genes.
  //Also, add all genes of all A14 nodes between two Rib A3 shock links.
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    //1) For each shock link FE:
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();    
    if (FE->count_valid_Fs() == 1) { //Detect if FE is of A3-Rib-type.      
      if (FE->have_asgn_Gs()) { //put all FE's generators into asso_genes[].
        for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
          dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
          A3_asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (G->id(), G));
        }
        if (remove_G_from_FE_FV)  //Disconnect all asgn. G's from FV.
          FE->clear_asgn_G_list ();
      }

      //2) For a possible boundary shock node FV between FE and nL:
      dbmsh3d_halfedge* nHE = HE->next();
      dbsk3d_fs_edge* nL = (dbsk3d_fs_edge*) nHE->edge();
      if (nL->count_valid_Fs() == 1) { //Detect if nL is of A3-Rib-type.      
        dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) Es_sharing_V (FE, nL);
        if (FV->have_asgn_Gs()) { //put all FV's generators into asso_genes[].
          for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
            dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
            A3_asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (G->id(), G));
          }
          if (remove_G_from_FE_FV)  //Disconnect all asgn. G's from FV.
            FV->clear_asgn_G_list ();
        }
      }
    }

    HE = HE->next();
  }
  while (HE != FF->halfedge());
}


//###############################################################
//  Splice xform on fs_sheet S
//###############################################################

//: Return true if S is valid for a 3D splice transform.
//  To-Do: need to:
//    - Handle degenerate A1n Links.
//
bool S_valid_splice_xform (dbsk3d_fs_sheet* S,
                           vcl_list<vcl_pair<int, int> >& S_to_splice,
                           vcl_vector<dbsk3d_fs_edge*>& C_Lset)
{
  int S1id, S2id;

  for (dbmsh3d_ptr_node* cur = S->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();

    //Go through each triple junction of FE of S.
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      if (FE->count_valid_Fs() == 3) { 
        //Currently only looks for A13 FE (skip A1n degenerate FE).
        int result = S_valid_splice_on_FE (S, FE, FF, S1id, S2id);
        if (result == 0) {
          #if DBMSH3D_DEBUG > 2
          vul_printf (vcl_cout, "\tS %d FF %d FE %d not eligible for the 3D splice transform.\n", 
                       S->id(), FF->id(), FE->id());
          #endif
          return false;
        }
        else if (result == 1) {
          //FE is an internal A5 swallow tail link. Skip.
          #if DBMSH3D_DEBUG > 2
          vul_printf (vcl_cout, "\tS %d FF %d has an internal A5 swallow tail link FE %d.\n", 
                       S->id(), FF->id(), FE->id());
          #endif
        }
        else {
          //FE is eligible for splice xform.
          C_Lset.push_back (FE); //Add FE to the A13 Curve's fs_edge list.
          if (S1id != S2id)
            S_to_splice.push_back (vcl_pair<int, int> (S1id, S2id));
        }
      }
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }

  return true;
}

//: Testif S is valid for a 3D splice transform on fs_edge FE
//  by testing triple junction on FE (inputFF, S1, S2).
//  Return:
//    0: if it's not valid.
//    1: if this is an internal junction of S (A5 swallow-tail)
//    2: if it's valid.
//
int S_valid_splice_on_FE (dbsk3d_fs_sheet* S, dbsk3d_fs_edge* FE, 
                          dbsk3d_fs_face* inputFF, int& S1id, int& S2id)
{
  //Determine the other two shock-patch-elms FF1 and FF2.
  //Assumption: FE is an A13-link-elm.
  dbsk3d_fs_face* FF1 = NULL;
  dbsk3d_fs_face* FF2 = NULL;
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    if (FF != inputFF && FF->b_valid()) {
      if (FF1 == NULL)
        FF1 = FF;
      else if (FF2 == NULL)
        FF2 = FF;
      else
        assert (0);
    }
    HE = HE->pair();
  }
  while (HE != FE->halfedge() && HE != NULL);

  //The picture: prune S (inputFF) and splice S1 (FF1) and S2 (FF2).
  //Return false if 
  S1id = FF1->sid();
  S2id = FF2->sid();

  if (S1id == S2id && S->id() == S1id)
    return 1; //the case of internal A5 swallowtail.

  if (S1id == S->id())
    return 0; //S is not eligible for splice xform.
  if (S2id == S->id())
    return 0; //S is not eligible for splice xform.

  //Makes S1id < S2id
  if (S1id > S2id) {
    int temp = S1id;
    S1id = S2id;
    S2id = temp;
  }

  return 2; //S is eligible for splice xform.
}

void replace_S2_in_list (const int S1id, const int S2id, 
                         vcl_list<vcl_pair<int, int> >& S_to_splice)
{
  //Go through the list and replace all instance of S2id by S1id.
  //Can not set the value of a set, because it is internally sorted!
  //should remove it and insert it back!
  vcl_list<vcl_pair<int, int> >::iterator it = S_to_splice.begin();
  for (; it != S_to_splice.end(); it++) {
    if ((*it).first == S2id)
      (*it).first = S1id;
    if ((*it).second == S2id)
      (*it).second = S1id;
  }
}

//###############################################################
//  Splice xform on ms_sheet MS
//###############################################################




