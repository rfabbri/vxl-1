//: dbsk3d_fs_xform_gap.cxx
//: MingChing Chang
//  May 01, 2007

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>


//###############################################################
//  Gap xform on fs_edge FE
//###############################################################


//: Perform the 3D Gap transform for the input shock-link-elm FE.
//    - create the corresponding boundary face F to fill the gap.
//        - associate FE with F.
//        - handle the halfedge connectivity of F.
//    - prune all incident shock-patch-elms of the shock-link-elm.
//      also pass generators to 2 A14 nodes (if need).
//
dbmsh3d_face* L_3d_gap_xform (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE)
{  
  //1) Collect the order set of bnd_edges surrounding FE in edges_of_F[]
  //   If any shock_patch FF is still valid, prune FF and create bnd_edge E.
  //   Assign 2 genes of FF if necessary.
  vcl_vector<dbmsh3d_edge*> edges_of_F;
  dbmsh3d_halfedge* he = FE->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) he->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;
  
  //set of all generators that will be passed to the two A14 nodes.
  vcl_map<int, dbmsh3d_vertex*> asso_genes;
  do {
    dbmsh3d_vertex* nG = FF->other_G (G);
    asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (nG->id(), nG));
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FE %d gap xform: add G %d to asso_genes.\n", FE->id(), nG->id());
    #endif

    //If the patch FF is valid, prune it and create the bnd_edge.
    //Otherwise just use the existing bnd_edge.
    dbmsh3d_edge* curE;
    if (FF->b_valid()) {
      curE = BM->add_new_edge (G, nG);
      //Put all associated G's from A3 FE's and FV's to asso_genes.
      FF_get_A3_asso_genes_FE_FV (FF, asso_genes, false);

      //Pass FF's unassociated generators to remaining adjacent shock links.
      FF_prune_pass_Gs (FF, false);
    }
    else
      curE = FF->get_bnd_E ();

    //add the bnd_edge of patch to the collection.
    edges_of_F.push_back (curE);

    //the next_patch that connected to G
    FF = FE->other_FF_of_G (FF, nG);
    G = nG;
  }
  while (G != startG && FF != NULL);

  //Pass genes (that will be lost) to FE's A14 shock nodes s_FV and e_FV,
  //if the shock node is valid after the gap xform.
  dbsk3d_fs_vertex* s_FV = FE->s_FV();
  dbsk3d_fs_vertex* e_FV = FE->e_FV();
  if (s_FV->valid_after_gap_xform_FE (FE))
    FE->prune_pass_Gs_to_FV (asso_genes, s_FV);
  if (e_FV->valid_after_gap_xform_FE (FE))
    FE->prune_pass_Gs_to_FV (asso_genes, e_FV);

  //2) Create the surface face F and add to the structure.
  dbmsh3d_face* F = BM->add_new_face (edges_of_F);
  FE->set_bnd_face (F);
  
  //3) Initialize the bnd_face's indexed-face-set (IFS) vertices.
  //   set the link's generators to be meshed.
  vcl_vector<dbmsh3d_vertex*> genes;
  bool result = FE->get_ordered_Gs_via_FF (genes);
  assert (result);

  for (unsigned int i=0; i<genes.size(); i++) {
    dbmsh3d_vertex* G = genes[i];
    F->_ifs_add_bnd_V (G);
    G->set_meshed (true);
  }

  return F;
}

dbmsh3d_face* L_add_Delaunay_facet (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE)
{  
  //1) Collect the order set of bnd_edges surrounding FE in edges_of_F[]
  //   If any shock_patch FF is still valid, prune FF and create bnd_edge E.
  //   Assign 2 genes of FF if necessary.
  vcl_vector<dbmsh3d_edge*> edges_of_F;
  dbmsh3d_halfedge* he = FE->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) he->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;
  
  //set of all generators that will be passed to the two A14 nodes.
  do {
    dbmsh3d_vertex* nG = FF->other_G (G);

    //If the patch FF is valid, prune it and create the bnd_edge.
    //Otherwise just use the existing bnd_edge.
    dbmsh3d_edge* curE;
    if (FF->b_valid()) {
      curE = BM->add_new_edge (G, nG);

      //Pass FF's unassociated generators to remaining adjacent shock links.
      FF->set_valid (false);
    }
    else
      curE = FF->get_bnd_E ();

    //add the bnd_edge of patch to the collection.
    edges_of_F.push_back (curE);

    //the next_patch that connected to G
    FF = FE->other_FF_of_G (FF, nG);
    G = nG;
  }
  while (G != startG && FF != NULL);

  //2) Create the surface face F and add to the structure.
  dbmsh3d_face* F = BM->add_new_face (edges_of_F);
  FE->set_bnd_face (F);
  
  //3) Initialize the bnd_face's indexed-face-set (IFS) vertices.
  //   set the link's generators to be meshed.
  vcl_vector<dbmsh3d_vertex*> genes;
  bool result = FE->get_ordered_Gs_via_FF (genes);
  assert (result);

  for (unsigned int i=0; i<genes.size(); i++) {
    dbmsh3d_vertex* G = genes[i];
    F->_ifs_add_bnd_V (G);
    G->set_meshed (true);
  }

  return F;
}

//: Undo the 3D gap transform for the input shock-link-elm.
//    - Delete the bnd_face.
//      For each bnd_edges, if it becomes isolated after removal of the bnd_face, delete it.
//    - Go through all incident FF's of the FE,
//      if the corresponding bnd_edge is deleted, set the FF to be valid.
//    - Pass generator of FE's two A14 nodes back to valid FF's.
//
void undo_L_3d_gap_xform (dbmsh3d_mesh* BM, dbsk3d_fs_edge* FE)
{
  
  //Delete F and delete the isolated edge after removal of F.
  dbmsh3d_face* F = (dbmsh3d_face*) FE->bnd_face();
  BM->remove_F_del_isolated_Es (F); 
  FE->set_bnd_face (NULL);

  //Set all shock-patch-elms with no bnd_edge to be valid.
  FE->update_validity_FFs ();

  //Pass generator of FE's two A14 nodes back to valid FF's.
  dbsk3d_fs_vertex* s_FV = (dbsk3d_fs_vertex*) FE->sV();
  dbsk3d_fs_vertex* e_FV = (dbsk3d_fs_vertex*) FE->eV();

  //if (s_FV->asgn_genes() != NULL)
  if (s_FV->have_asgn_Gs())
    s_FV->del_asgn_G_of_FE (FE);
  if (e_FV->have_asgn_Gs())
    e_FV->del_asgn_G_of_FE (FE);
}

