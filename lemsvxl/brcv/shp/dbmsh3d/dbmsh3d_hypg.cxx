//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_hypermesh.cxx
//:
// \file
// \brief Hypergraph
//
//
// \author
//  MingChing Chang  July 05, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vul/vul_printf.h>
#include <dbmsh3d/dbmsh3d_hypg.h>

void dbmsh3d_hypg::remove_S_complete_hypg (dbmsh3d_sheet* S)
{
  vcl_set<dbmsh3d_edge*> rem_C;
  vcl_set<dbmsh3d_vertex*> rem_N;
  remove_S_complete_hypg (S, rem_C, rem_N);

  vcl_set<dbmsh3d_edge*> removed_Cs;
  vcl_set<dbmsh3d_vertex*> removed_Ns;
  remove_S_complete_fix (rem_C, rem_N, removed_Cs, removed_Ns);
}

//: Delete the sheet S, also remove all MC's and SV's only incident to S.
//  This ensures the hypergraph is a complete geometric hypergraph.
void dbmsh3d_hypg::remove_S_complete_hypg (dbmsh3d_sheet* S,
                                           vcl_set<dbmsh3d_edge*>& rem_C,
                                           vcl_set<dbmsh3d_vertex*>& rem_N)
{
  rem_C.clear();
  rem_N.clear();

  //If S has icurve_chain, it is not the 'leave (of a hyper-tree)' of the hypergraph.
  if (S->have_icurve_chain()) {    
    for (dbmsh3d_ptr_node* cur = S->icurve_chain_list(); cur != NULL; cur = cur->next()) {
      const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();    
      dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;  
      //Add all icurve edges into rem_C and all their vertices to rem_N.  
      do {
        rem_C.insert (HE->edge());
        rem_N.insert (HE->edge()->sV());
        rem_N.insert (HE->edge()->eV());
        HE = HE->next();
      }
      while (HE != headHE && HE != NULL);     
    }
    
    //Disconnect S from its icurve_chain breaks the hypergraph into 2 more components.
    S->disconnect_all_icurve_Es ();
  }

  //1) Loop through the bnd_chain and disconnect each curve.
  //   Remove the curve if its n_incidence == 1.
  vcl_vector<dbmsh3d_edge*> incident_edge_list;
  _delete_HE_chain (S->halfedge(), incident_edge_list);
  assert (S->halfedge() ==  NULL);

  //2) Make incident_edge_set from incident_edge_list.
  //   Consider the A5 swallow-tail case,there could be multiple (triple) instances.
  vcl_set<dbmsh3d_edge*> incident_E_set;
  for (unsigned int i=0; i<incident_edge_list.size(); i++) {
    dbmsh3d_edge* E = incident_edge_list[i];
    incident_E_set.insert (E);
  }

  vcl_set<dbmsh3d_vertex*> cand_N_rem;

  //Go through the incident_edge_set and remove each edge.
  vcl_set<dbmsh3d_edge*>::iterator it = incident_E_set.begin();
  while (it != incident_E_set.end()) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) (*it);
    incident_E_set.erase (it);
    dbmsh3d_vertex* sV = C->sV();
    dbmsh3d_vertex* eV = C->eV();

    //Remove the C that's isolated or internal to only one sheet.    
    dbmsh3d_face* F = C->is_n_incident_to_one_S (2);
    if (F) {
      dbmsh3d_sheet* S = dynamic_cast<dbmsh3d_sheet*> (F);
      bool result = S->disconnect_icurve_pair_E (C);
      assert (result);
    }
    
    //If C is isolated, clear C's shared_E
    if (C->halfedge() == NULL) {
      if (C->have_shared_Es())
        vul_printf (vcl_cout, "Error: isolated C%d has shared_Es!\n", C->id());
    }

    if (try_remove_edge (C)) {
      //add sV and eV to the cand_N_rem list
      cand_N_rem.insert (sV);
      cand_N_rem.insert (eV);
    }
    else
      rem_C.insert (C);

    it = incident_E_set.begin();
  }

  remove_sheet (S->id());

  //3) Also remove isolated vertices
  vcl_set<dbmsh3d_vertex*>::iterator nit = cand_N_rem.begin();
  while (nit != cand_N_rem.end()) {
    dbmsh3d_vertex* V = (*nit);
    if (try_remove_vertex (V) == false) {
      rem_N.insert (V);
    }
    cand_N_rem.erase (nit);
    nit = cand_N_rem.begin();
  }
}

void dbmsh3d_hypg::remove_S_complete_fix (vcl_set<dbmsh3d_edge*>& rem_C, vcl_set<dbmsh3d_vertex*>& rem_N,
                                          vcl_set<dbmsh3d_edge*>& removed_Cs, vcl_set<dbmsh3d_vertex*>& removed_Ns)
{
  //4) A rarely happened case: rem_N's only has 2-incident curves, where their incident sheets are identical. 
  //   Should merge these two curves and delete the remaining_V.
  vcl_set<dbmsh3d_vertex*>::iterator nit = rem_N.begin();
  while (nit != rem_N.end()) {
    dbmsh3d_node* N = (dbmsh3d_node*) (*nit);
    vcl_set<void*> incident_Es;
    unsigned int ne = N->get_incident_Es (incident_Es);
    if (ne != 2) {
      nit++;
      continue;
    }

    vcl_set<void*>::iterator it = incident_Es.begin();
    dbmsh3d_curve* C1 = (dbmsh3d_curve*) (*it);
    it++;
    dbmsh3d_curve* C2 = (dbmsh3d_curve*) (*it);
    assert (C1 != C2);

    if (same_incident_Fs (C1, C2) == false) {
      //If if C1 and C2 not sharing the same incident sheets.
      nit++;
      continue;
    }

    if (nit == rem_N.begin()) {
      rem_N.erase (N);
      nit = rem_N.begin();
    }
    else {
      vcl_set<dbmsh3d_vertex*>::iterator tmp = nit;
      nit--;
      rem_N.erase (N);
      nit++;
    }

    //Merge C1 and C2.
    merge_Cs_sharing_N (N, C1, C2);
    rem_C.erase (C2);
    
    removed_Ns.insert (N);
    removed_Cs.insert (C2);
  }
  
  //Re-compute MC.type for remaining curves.
  vcl_set<dbmsh3d_edge*>::iterator cit = rem_C.begin();
  for (; cit != rem_C.end(); cit++) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) (*cit);
    C->compute_c_type();
  }

  //Re-compute MN.type for remaining nodes.
  nit = rem_N.begin();
  for (; nit != rem_N.end(); nit++) {
    dbmsh3d_node* N = (dbmsh3d_node*) (*nit);
    N->compute_n_type();
  }
}

//: Merge two adjacent curves C1 and C2 sharing a node N.
//  Keep C1 and merge all edge elements of C2 to C1.
void dbmsh3d_hypg::merge_Cs_sharing_N (dbmsh3d_node* N, dbmsh3d_curve* C1, dbmsh3d_curve* C2)
{
  vul_printf (vcl_cout, "merge C%d to C%d at N%d, ", C2->id(), C1->id(), N->id());

  _merge_E_vec_C2_to_C1 (N, C1, C2);

  //2) Put all C2's shared_E_list_ to C1's shared_E_list_
  C2->pass_shared_Es_to_C (C1);

  //3) Disconnect/delete C2 and connect C1.
  merge_Es_sharing_V (N, C1, C2);  
}

//: Merge two adjacent edges sharing a vertex V. Keep E1 and merge E2 to E1.
void dbmsh3d_hypg::merge_Es_sharing_V (const dbmsh3d_vertex* V, dbmsh3d_edge* E1, dbmsh3d_edge* E2)
{
  // E1 -- inputV -- E2.
  dbmsh3d_vertex* E1V = E1->other_V (V);
  dbmsh3d_vertex* E2V = E2->other_V (V);
  bool loopV = (E1V == E2V);

  //disconnect and delete E2.
  //No need to loop through disconn_faces and fix their halfedge_.
  vcl_set<dbmsh3d_face*> disconn_faces;
  E2->_disconnect_all_Fs (disconn_faces);

  remove_edge (E2);
  //disconnect and delete inputV.
  for (unsigned int i=0; i<2; i++) {
    if (E1->vertices(i) == V) {
      E1->_disconnect_V_idx (i);
      break;
    }
  }  
  //connect E1V -- E1 -- E2V.
  for (unsigned int i=0; i<2; i++) {
    if (E1->vertices(i) == NULL) {
      if (loopV) 
        E1->_set_vertex (i, E2V);
      else
        E1->connect_V (i, E2V);
      break;
    }
  }   
}


//###############################################################

//: Loop through all elements and check integrity
bool dbmsh3d_hypg::check_integrity ()
{
  if (dbmsh3d_graph::check_integrity() == false)
    return false;
  
  vcl_map<int, dbmsh3d_sheet*>::iterator it = sheetmap_.begin();
  for (; it != sheetmap_.end(); it++) {
    dbmsh3d_sheet* S = (*it).second;
    if (S->id() != (*it).first)
      return false;
    if (S->check_integrity() == false)
      return false;
  }

  return true;
}
  
dbmsh3d_hypg* dbmsh3d_hypg::clone (dbmsh3d_mesh* M2)
{
  dbmsh3d_hypg* HG2 = new dbmsh3d_hypg (M2);

  //deep-copy all vertices.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbmsh3d_node* N = (dbmsh3d_node*) (*vit).second;
    dbmsh3d_node* N2 = (dbmsh3d_node*) N->clone (M2);
    HG2->_add_vertex (N2);
  }
  HG2->set_vertex_id_counter (vertex_id_counter_);
  assert (HG2->vertexmap().size() == vertexmap_.size());

  //deep-copy all curves.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) (*eit).second;
    dbmsh3d_curve* C2 = (dbmsh3d_curve*) C->clone (HG2, M2);
    HG2->_add_edge (C2);
  }
  HG2->set_edge_id_counter (edge_id_counter_);
  assert (HG2->edgemap().size() == edgemap_.size());

  //deep-copy all sheets.
  vcl_map<int, dbmsh3d_sheet*>::iterator sit = sheetmap_.begin();
  for (; sit != sheetmap_.end(); sit++) {
    dbmsh3d_sheet* S = (dbmsh3d_sheet*) (*sit).second;
    dbmsh3d_sheet* S2 = (dbmsh3d_sheet*) S->clone (HG2, M2);
    HG2->_add_sheet (S2);
  }
  HG2->set_sheet_id_counter (sheet_id_counter_);
  assert (HG2->sheetmap().size() == sheetmap_.size());
  
  return HG2;
}

dbmsh3d_pt_set* dbmsh3d_hypg::clone ()
{
  //deep-copy fs_mesh and bnd_mesh.
  dbmsh3d_mesh* M2 = (dbmsh3d_mesh*) mesh_->clone ();
  dbmsh3d_hypg* HG2 = clone (M2);
  return HG2;
}

//###################################################################

void _merge_E_vec_C2_to_C1 (dbmsh3d_node* N, dbmsh3d_curve* C1, dbmsh3d_curve* C2)
{
  //1) Prepare the array of edge elements of C2 in both order.
  vcl_vector<dbmsh3d_edge*> C2_Evec_N_N2;
  vcl_vector<dbmsh3d_edge*> C2_Evec_N2_N;
  if (C2->s_N() == N) {
    for (unsigned int i=0; i<C2->E_vec().size(); i++)
      C2_Evec_N_N2.push_back (C2->E_vec(i));
    for (int i=C2->E_vec().size()-1; i>=0; i--)
      C2_Evec_N2_N.push_back (C2->E_vec(i));
  }
  else {
    assert (C2->e_N() == N);
    for (unsigned int i=0; i<C2->E_vec().size(); i++)
      C2_Evec_N2_N.push_back (C2->E_vec(i));
    for (int i=C2->E_vec().size()-1; i>=0; i--)
      C2_Evec_N_N2.push_back (C2->E_vec(i));
  }

  //Merge fs_edges of C2 to C1.
  if (C1->s_N() == N) {
    C1->check_add_E_vec_to_front (C2_Evec_N2_N);
  }
  else {
    assert (C1->e_N() == N);
    C1->check_add_E_vec_to_back (C2_Evec_N_N2);
  }
}

void dbmsh3d_hypg_print_object_size ()
{  
  vul_printf (vcl_cout, "------------------------------------\n");
  vul_printf (vcl_cout, "dbmsh3d_sheet        --  %3d\n", sizeof (dbmsh3d_sheet));  
  vul_printf (vcl_cout, "\n");
  vul_printf (vcl_cout, "dbmsh3d_graph            %3d\n", sizeof (dbmsh3d_graph));
  vul_printf (vcl_cout, "dbmsh3d_hypg             %3d\n", sizeof (dbmsh3d_hypg));
}

