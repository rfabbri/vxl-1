//: dbsk3d_FullShock.cxx
//: MingChing Chang
//  Apr 30, 2004        Creation

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>

//##################################################################
//               THE FULL SHOCK
//##################################################################

dbsk3d_fs_mesh::dbsk3d_fs_mesh (dbmsh3d_mesh* bnd)
{
  bnd_mesh_ = bnd;
  median_A122_dist_ = VALUE_NOT_DETERMINED;
}

dbsk3d_fs_mesh::~dbsk3d_fs_mesh ()
{
  //Go through all fs_edges and delete assigned generators.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    FE->clear_asgn_G_list ();
  }

  //Go through all fs_vertices and delete assigned generators.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    FV->clear_asgn_G_list ();
  }
}

//: Compute the median of A12-2 distance of all shock-sheet-elms of the full shock scaffold.
float dbsk3d_fs_mesh::compute_median_A122_dist ()
{
  if (median_A122_dist_ != VALUE_NOT_DETERMINED)
    return median_A122_dist_; //If already computed, return.

  vul_printf (vcl_cout, "  compute_median_A122_dist(): %u A12 shock sheet elms.\n", facemap_.size());
  assert (facemap_.size() != 0);

  //Loop through all shock sheet elements and compute the median of A12-2 time.
  vcl_vector<double> vals;
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;

    double time = FF->mid_pt_time();
    vals.push_back (time);
  }

  //Find the median of a vector of doubles:
  vcl_vector<double>::iterator medianIter=vals.begin() + vals.size()/2;
  vcl_nth_element (vals.begin(),medianIter,vals.end());
  median_A122_dist_ = (float) *medianIter;

  vul_printf (vcl_cout, "\tmedian A12-2 sample dist = %f\n", median_A122_dist_);
  return median_A122_dist_;
}

//: Detect valid shock link types by traversing incident shock patchs.
void dbsk3d_fs_mesh::detect_valid_FE_type ()
{
  vul_printf (vcl_cout, "  detect_valid_FE_type(): totally %u link-elms\n", edgemap_.size());

  //:2) ------- ID links as regular, axial, ridge or border --------
  unsigned int n_invalid = 0;
  unsigned int n_sheet_1 = 0;
  unsigned int n_sheet_2 = 0;
  unsigned int n_sheet_3 = 0;
  unsigned int n_sheet_4p = 0;

  vcl_map<int, dbmsh3d_edge*>::iterator lit = edgemap_.begin();
  for (; lit != edgemap_.end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    if (FE->is_valid_via_F() == false) {
      n_invalid++;
      continue;
    }

    FE->compute_e_type ();

    switch (FE->e_type()) {
    case E_TYPE_RIB:      
      n_sheet_1++;
    break;
    case E_TYPE_MANIFOLD:      
      n_sheet_2++;
    break;
    case E_TYPE_AXIAL:      
      n_sheet_3++;
    break;
    case E_TYPE_DEGE_AXIAL:      
      n_sheet_4p++;
    break;
    }
  }

  vul_printf (vcl_cout, "\tInvalid (pruned) fs_edges (with 0 sheet):\t%6u,\n", n_invalid);
  vul_printf (vcl_cout, "\tRib fs_edges (with 1 sheet):\t\t\t%6u,\n", n_sheet_1);
  vul_printf (vcl_cout, "\tManifold fs_edges (with 2 sheets):\t\t%6u,\n", n_sheet_2);
  vul_printf (vcl_cout, "\tAxial fs_edges (with 3 sheets):\t\t\t%6u,\n", n_sheet_3);
  vul_printf (vcl_cout, "\tDegenerate Axial fs_edges (with 4+ sheets):\t%6u.\n", n_sheet_4p);
}

void dbsk3d_fs_mesh::compute_all_FEs_FVs_type ()
{
  //Recompute shock type of each fs_edge.  
  //(Should only do this locally!!)
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbmsh3d_edge* E = (*eit).second;
    E->compute_e_type ();
  }

  //Recompute shock type of each fs_vertex.  
  //(Should only do this locally!!)
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->compute_v_type ();
  }
}

//: Physically delete all labelled fs_faces (and Es and Vs).
unsigned int dbsk3d_fs_mesh::del_invalid_FFs_complete()
{
  vcl_set<dbmsh3d_edge*> Eset_to_del;
  vcl_set<dbmsh3d_vertex*> Vset_to_del;

  //Go through all FF's and delete invalid FFs. 
  unsigned int nFF_del=0, nFE_del=0, nFV_del=0;
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  while (fit != facemap_.end()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    fit++;

    if (FF->b_valid() == false) { //delete FF
      //Add incident E's to Eset_to_del
      vcl_vector<dbmsh3d_edge*> bnd_Es;
      FF->get_bnd_Es (bnd_Es);
      Eset_to_del.insert (bnd_Es.begin(), bnd_Es.end());
      nFF_del++;
      //Add incident V's to Vset_to_del
      for (unsigned int i=0; i<bnd_Es.size(); i++) {
        Vset_to_del.insert (bnd_Es[i]->vertices(0));
        Vset_to_del.insert (bnd_Es[i]->vertices(1));
      }
      remove_face (FF);
    }
  }

  //Go through all Eset_to_del and delete the ones without incident FF's.
  vcl_set<dbmsh3d_edge*>::iterator eit = Eset_to_del.begin();
  while (eit != Eset_to_del.end()) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit);
    Eset_to_del.erase (eit);

    if (FE->halfedge() == NULL) { //delete FE
      if (FE->have_asgn_Gs())
        FE->clear_asgn_G_list ();
      remove_edge (FE);   
      nFE_del++;
    }
    eit = Eset_to_del.begin();
  }

  //Go through all Vset_to_del and delete the ones without incident FE's.
  vcl_set<dbmsh3d_vertex*>::iterator vit = Vset_to_del.begin();
  while (vit != Vset_to_del.end()) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit);
    Vset_to_del.erase (vit);

    if (FV->have_incident_Es() == NULL) { //delete FV
      if (FV->have_asgn_Gs())
        FV->clear_asgn_G_list ();
      remove_vertex (FV);
      nFV_del++;
    }
    vit = Vset_to_del.begin();
  }

  vul_printf (vcl_cout, "\n\t%dF, %dE, %dN deleted. ", nFF_del, nFE_del, nFV_del);
  return nFF_del;
}

// ###########  Functions to associate boundary and shocks  #################

//: Check if all generators are currently assigned.
//  We have maintained only a one-way assignment (pointers from shock to the generators).
//  This function is to check if there is any generator that is not assigned (lost).
//  return true if all generators are assigned to some shock(s).
//
bool dbsk3d_fs_mesh::check_all_G_asgn (vcl_vector<dbmsh3d_vertex*>& unasgn_genes) 
{
  vul_printf (vcl_cout, "  check_all_G_asgn(): %d genes.\n", bnd_mesh_->vertexmap().size());

  //Go through all generators and initialize the G_asso_map_[] for all G's to G_ASSO_NONE. 
  vcl_map<int, char>  G_asso_map;

  vcl_map<int, dbmsh3d_vertex*>::iterator git = bnd_mesh_->vertexmap().begin();
  for (; git != bnd_mesh_->vertexmap().end(); git++) {
    dbmsh3d_vertex* G = (*git).second;
    G_asso_map.insert (vcl_pair<int, char> (G->id(), G_ASSO_NONE));
  }

  //Go through all shock patches and mark the associated generators as G_ASSO_PATCH.  
  unsigned int n_valid_P = 0;
  unsigned int n_G_P_asgn = 0;
  vcl_map<int, dbmsh3d_face*>::iterator pit = facemap_.begin();
  for (; pit != facemap_.end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->b_valid() == false)
      continue; //skip the pruned fs_faces.
    n_valid_P++;
    mark_G_asso (FF->genes(0), G_ASSO_PATCH, G_asso_map);
    mark_G_asso (FF->genes(1), G_ASSO_PATCH, G_asso_map);
    n_G_P_asgn+=2;
  }
  vul_printf (vcl_cout, "    %u valid fs_faces (total %u), %u FF-G asgn.\n", 
              n_valid_P, facemap_.size(), n_G_P_asgn);
  
  //Go through all shock links and mark the assigned generators as G_ASSO_LINK.  
  unsigned int n_valid_L = 0;
  unsigned int n_G_L_asgn = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    if (FE->is_valid_via_F())
      n_valid_L++;
    if (FE->have_asgn_Gs()) {
      bool r = FE->count_valid_Fs() >= 1;
      if (r == false) {
        ///assert (0);
        vul_printf (vcl_cout, "FE %d has G but not valid, ", FE->id());
      }

      //Also need to assert that the asgn_genes are not in each valid incident FF's genes.
      for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        mark_G_asso (G, G_ASSO_LINK, G_asso_map);
        n_G_L_asgn++;
        ///vul_printf (vcl_cout, "%d ", (*lgit)->id());
      }
    }
  }
  vul_printf (vcl_cout, "    %u valid fs_edges (total %u), %u FE-G asgn.\n", 
              n_valid_L, edgemap_.size(), n_G_L_asgn);

  //Go through all shock nodes and mark the assigned generators as G_ASSO_NODE. 
  unsigned int n_valid_N = 0; 
  unsigned int n_G_N_asgn = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    if (FV->is_valid_via_FF())
      n_valid_N++;
    if (FV->have_asgn_Gs()) {
      bool r = FV->count_valid_FEs() >= 2;
      if (r == false) {
        ///assert (0);
        vul_printf (vcl_cout, "FV %d has G but not valid, ", FV->id());
      }
      //Also need to assert that the asgn_genes are not in each valid incident FE's and FF's genes.
      for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        mark_G_asso (G, G_ASSO_NODE, G_asso_map);
        n_G_N_asgn++;
        ///vul_printf (vcl_cout, "%d ", (*lgit)->id());
      }
    }
  }
  vul_printf (vcl_cout, "    %u valid fs_vertices (total %u), %u FV-G asgn.\n", 
              n_valid_N, vertexmap_.size(), n_G_N_asgn);

  //Go through G_asso_map_[] again and check if any G unassociated.
  vul_printf (vcl_cout, "    Unassociated gene(s) id: ");
  vcl_map<int, char>::iterator gmit = G_asso_map.begin();
  for (; gmit != G_asso_map.end(); gmit++) {
    char asso = (*gmit).second;
    if (asso == G_ASSO_NONE) {
      int id = (*gmit).first;
      unasgn_genes.push_back (bnd_mesh_->vertexmap(id));
      #if DBMSH3D_DEBUG > 2
      vul_printf (vcl_cout, "%d ", id);
      #endif
    }
  }
  if (unasgn_genes.size() == 0)
    vul_printf (vcl_cout, "NONE.");  
  vul_printf (vcl_cout, "\n");  
  G_asso_map.clear();

  vul_printf (vcl_cout, "\t total # of unassociated gene(s): %d.\n", unasgn_genes.size());

  //Return if bnd-shock asso. is complete or not.
  return unasgn_genes.size() == 0;
}

// ##################################################################

bool dbsk3d_fs_mesh::check_integrity ()
{
  if (dbmsh3d_mesh::check_integrity() == false) {
    assert (0);
    return false;
  }

  //Check the integrity of the fine-scale shock elements.
  return true;
}

dbsk3d_fs_mesh* dbsk3d_fs_mesh::clone (dbmsh3d_mesh* BND2)
{
  dbsk3d_fs_mesh* FS2 = new dbsk3d_fs_mesh (BND2);
  
  //deep-copy all fs_vertices.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    dbsk3d_fs_vertex* FV2 = FV->clone (BND2);
    FS2->_add_vertex (FV2);
  }
  FS2->set_vertex_id_counter (vertex_id_counter_);
  assert (FS2->vertexmap().size() == vertexmap_.size());

  //deep-copy all edges.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    dbsk3d_fs_edge* FE2 = FE->clone (FS2, BND2);
    FS2->_add_edge (FE2);
  }
  FS2->set_edge_id_counter (edge_id_counter_);
  assert (FS2->edgemap().size() == edgemap_.size());

  //deep-copy all fs_faces.
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    dbsk3d_fs_face* FF2 = (dbsk3d_fs_face*) FF->clone ();
    FS2->_add_face (FF2);
    //deep-copy FF's gene[2] to FF2. 
    dbmsh3d_vertex* G = FF->genes(0);
    dbmsh3d_vertex* G2 = BND2->vertexmap (G->id());
    FF2->set_G (0, G2);
    G = FF->genes(1);
    G2 = BND2->vertexmap (G->id());
    FF2->set_G (1, G2);
  }
  FS2->set_face_id_counter (face_id_counter_);
  assert (FS2->facemap().size() == facemap_.size());

  //deep-copy the MHE data structure.
  assert (is_MHE());
  //First copy the IFS then convert to MHE.
  fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;  
    vcl_vector<int> vids;
    F->_get_bnd_V_ids_MHE (vids);
    dbmsh3d_face* F2 = FS2->facemap (F->id());
    for (unsigned int i=0; i<vids.size(); i++) {
      dbmsh3d_vertex* V = FS2->vertexmap (vids[i]);
      F2->_ifs_add_bnd_V (V);
    }
  }
  FS2->IFS_to_MHE ();
  assert (FS2->edgemap().size() == edgemap_.size());
  
  return FS2;
}

dbmsh3d_pt_set* dbsk3d_fs_mesh::clone ()
{
  dbmsh3d_mesh* BND2 = (dbmsh3d_mesh*) bnd_mesh()->clone ();
  dbsk3d_fs_mesh* FS2 = clone (BND2);
  FS2->set_median_A122_dist (median_A122_dist_);
  return FS2;
}

bool dbsk3d_fs_mesh::check_all_FFs_valid ()
{
  //assert that all fs_faces are valid.
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->id() != (*fit).first) {
      assert (0);
      return false;
    }
    if (FF->b_valid() == false) {
      assert (0);
      return false;
    }
  }
  return true;
}

void dbsk3d_fs_mesh::summary_report ()
{
  vul_printf (vcl_cout, "\ndbsk3d_fs_mesh::summary_report()\n");
  vul_printf (vcl_cout, "\t# of faces: %u.\n", facemap_.size());
  vul_printf (vcl_cout, "\t# of edges: %u.\n", edgemap_.size());
  vul_printf (vcl_cout, "\t# of vertices: %u.\n", vertexmap_.size());
}


int IFS_to_HE_print_mem_usage (dbsk3d_fs_mesh* fs_mesh)
{
  int total = 0;
  vul_printf (vcl_cout, "\n\n");
  vul_printf (vcl_cout, "     Map            Object            Size    Map Size    Total (bytes)\n");
  vul_printf (vcl_cout, "-------------------------------------------------------------------------\n");
  int sum = sizeof (dbsk3d_fs_vertex) * fs_mesh->vertexmap().size();
  total += sum;
  vul_printf (vcl_cout, " vertexmap_  dbsk3d_fs_vertex    %3d    %9d     %9d\n", 
               sizeof (dbsk3d_fs_vertex), fs_mesh->vertexmap().size(), sum);
  sum = sizeof (dbsk3d_fs_face) * fs_mesh->facemap().size();
  total += sum;
  vul_printf (vcl_cout, " facemap_    dbsk3d_fs_face   %3d    %9d     %9d\n", 
               sizeof (dbsk3d_fs_face), fs_mesh->facemap().size(), sum);
  vul_printf (vcl_cout, "-------------------------------------------------------------------------\n");

  return total;
}

void IFS_to_HE_print_mem_usage (dbsk3d_fs_mesh* fs_mesh, int seid_edge_map_size, 
                                int V_F_incidence_size, int total)
{
  int sum = sizeof (dbsk3d_fs_edge) * fs_mesh->edgemap().size();
  total += sum;
  vul_printf (vcl_cout, " edgemap_    dbsk3d_fs_edge    %3d    %9d     %9d\n", 
               sizeof (dbsk3d_fs_edge), fs_mesh->edgemap().size(), sum);
  vul_printf (vcl_cout, "\n");

  sum = sizeof(vcl_pair<vcl_pair<int,int>, dbmsh3d_edge*>) * seid_edge_map_size;
  total += sum;
  vul_printf (vcl_cout, " seid_edge_map <<int,int>,pointer>    %3d    %9d     %9d\n", 
               sizeof(vcl_pair<vcl_pair<int,int>, dbmsh3d_edge*>),
               seid_edge_map_size, sum);
  vul_printf (vcl_cout, "-------------------------------------------------------------------------\n");
  vul_printf (vcl_cout, "                Total                                      %9d\n", total);
}

//######################################################################
//  Get associated boundary IFS mesh 
//######################################################################

void FFs_get_bnd_mesh_Fs (const vcl_vector<dbsk3d_fs_face*>& fs_faces, 
                          vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                          vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1)
{
  //1) Insert all assigned generators into Gset.
  vcl_set<dbsk3d_fs_edge*> FE_with_G_set; //Set of fs_edges associated with generators.  
  vcl_set<dbsk3d_fs_vertex*> FV_with_G_set; //Set of fs_vertices associated with generators.

  vcl_set<dbmsh3d_vertex*> Gset_add; //Additional generators to add for Gfaces2 and Gfaces1.

  for (unsigned int i=0; i<fs_faces.size(); i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_faces[i];
    assert (FF->b_valid());    
    Gset.insert (FF->genes(0));
    Gset.insert (FF->genes(1));
    FF->get_bnd_FE_FV_with_Gs (FE_with_G_set, FV_with_G_set);
  }
  
  //Add all genes assigned to fs_edges.
  vcl_set<dbsk3d_fs_edge*>::iterator lit = FE_with_G_set.begin();
  for (; lit != FE_with_G_set.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);
    assert (FE->have_asgn_Gs());    
    for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      Gset.insert (G);
    }
  }
  FE_with_G_set.clear();

  //Add all genes assigned to fs_vertices.
  vcl_set<dbsk3d_fs_vertex*>::iterator nit = FV_with_G_set.begin();
  for (; nit != FV_with_G_set.end(); nit++) {
    dbsk3d_fs_vertex* FV = (*nit);
    assert (FV->have_asgn_Gs());
    for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      Gset.insert (G);
    }
  }
  FV_with_G_set.clear();

  //2) Loop through Gset and find all bnd_mesh faces on it to Gfaces.
  vcl_set<dbmsh3d_vertex*>::iterator git = Gset.begin();
  for (; git != Gset.end(); git++) {
    dbmsh3d_vertex* G = *git;
    //Loop through all incident faces of G.
    vcl_set<dbmsh3d_face*> inc_faces;
    G->get_incident_Fs (inc_faces);

    vcl_set<dbmsh3d_face*>::iterator fit = inc_faces.begin();
    for (; fit != inc_faces.end(); fit++) {
      dbmsh3d_face* F = *fit;
      vcl_vector<dbmsh3d_vertex*> V_vec;
      F->get_bnd_Vs (V_vec);
      ///assert (F->vertices().size());
      int n_inc_V = F->n_bnd_Vs_in_set (Gset);
      
      //1) Put faces incident with Gset to Gfaces, Gfaces2, Gfaces1.
      if (n_inc_V == V_vec.size()) ///F->vertices().size()) 
        Gfaces.insert (F);
      else if (n_inc_V >= 2) {
        Gfaces2.insert (F);
        //Expand the Gset
        for (unsigned int i=0; i<V_vec.size(); i++) //F->vertices
          Gset_add.insert (V_vec[i]); ///F->vertices(i));
      }
      else if (n_inc_V == 1) {
        Gfaces1.insert (F);
        //Espand the Gset
        for (unsigned int i=0; i<V_vec.size(); i++) //F->vertices()
          Gset_add.insert (V_vec[i]); ///F->vertices(i));
      }
    }
  }

  //Add Gset_add to Gset.
  Gset.insert (Gset_add.begin(), Gset_add.end());
}
