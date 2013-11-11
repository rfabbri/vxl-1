// This is brcv/shp/dbmsh3d/dbmsh3d_sheet_algo.cxx
//:
// \file
//   Author  Ming-Ching Chang
// \brief 

#include <vcl_queue.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_sheet_algo.h>

//: Merge two adjacent sheets sharing edge E.
//  Delete edge E and connecting the boundary chain of S2 to S1.
bool merge_sheets_sharing_E (dbmsh3d_hypg* H, dbmsh3d_edge* E, 
                             dbmsh3d_sheet* S1, dbmsh3d_sheet* S2,
                             const bool delete_E)
{
  //      +--------+
  // E2s  |   S2   | E2e
  //  Ve  +---E----+ Vs
  // E1e  |   S1   | E1s
  //      +--------+

  //Find the bordering edges (E1s -> E -> E1e) of S1
  //Note that the ms_hypg topology can allow multiple (>2) connected vertex,
  //So using the halfedge chain is necessary.
  dbmsh3d_halfedge* HE1 = E->get_HE_of_F (S1);
  dbmsh3d_halfedge* HE2 = E->get_HE_of_F (S2);
  dbmsh3d_halfedge* HE1s = _find_prev_in_next_chain (HE1);
  dbmsh3d_halfedge* HE1e = HE1->next();

  if (HE1s == NULL) { //The special one loop case for S1 and S2.
    assert (HE1e == NULL);
    S1->del_icurve_chain_HE (HE1); //Remove S1's icurve entry of HE1.
    E->_disconnect_HE (HE1);
    delete (HE1);
    S2->set_halfedge (NULL);
    E->_disconnect_HE (HE2);
    delete (HE2);
    if (delete_E)
      H->remove_edge (E);
    return true;
  }

  dbmsh3d_vertex* Vs = Es_sharing_V (HE1->edge(), HE1s->edge());
  assert (Vs);
  dbmsh3d_vertex* Ve = Es_sharing_V (HE1->edge(), HE1e->edge());
  assert (Ve);

  if (HE1s == HE1e) { //The rare loop case of S1 of only 2 edges.
    Vs = HE1->edge()->vertices(0);
    Ve = HE1->edge()->vertices(1);
  }

  //Now setup the halfedge ordering of S2.
  //Vs --> all HE's in S2 --> Ve
  dbmsh3d_halfedge* HE2s = _find_prev_in_next_chain (HE2);
  dbmsh3d_halfedge* HE2e = HE2->next();

  if (HE2s == NULL) { //The special one loop case for S2.
    assert (HE2e == NULL);
    assert (Vs == Ve);
    S1->_disconnect_HE (HE1);
    E->_disconnect_HE (HE1);
    delete HE1;
    S2->set_halfedge (NULL);
    E->_disconnect_HE (HE2);
    delete HE2;
    if (delete_E)
      H->remove_edge (E);
    return true;
  }
  
  if (HE2s != HE2e) { //Skip the rare case of S2 as a loop of only 2 edges.
    dbmsh3d_vertex* Ve2 = Es_sharing_V (HE2->edge(), HE2e->edge());
    assert (Ve2);
    dbmsh3d_vertex* Vs2 = Es_sharing_V (HE2->edge(), HE2s->edge());
    assert (Vs2);
    assert (Vs2 != Ve2);

    if (Ve2 == Vs) {
      assert (Vs2 == Ve);
      //S2's halfedge order is correct.
    }
    else {
      assert (Ve2 == Ve);
      assert (Vs2 == Vs);
      //Reverse the halfedge order of S2.
      S2->_reverse_bnd_HEs_chain ();
      
      //Swap (HE2s, HE2e)
      dbmsh3d_halfedge* tmp = HE2s;
      HE2s = HE2e;
      HE2e = tmp;
      assert (HE2s->next() == HE2);
      assert (HE2->next() == HE2e);
    }
  }
  
  //Now link halfedges in the order of
  //  HE1s --> HE2e --> all other HE's in S2 --> HE2s --> HE1e.
  HE1s->set_next (HE2e);
  HE2s->set_next (HE1e);

  //Set all HEs of (HE2e --> all other HE's in S2 --> HE2s) to point to face S1
  dbmsh3d_halfedge* HE = HE2e;
  while (HE != HE1s) {
    HE->set_face (S1);
    HE = HE->next();
  }

  //Delete S2 's pointer to the halfedge chain.
  //Assert S2 does not contain HE2 in the i-curves.
  bool r = S2->is_HE_in_icurves (HE2);
  assert (r == false);
  S2->set_halfedge (NULL);

  //Reset S1.halfedge if it is at HE1.
  S1->_set_headHE_to_next (HE1);

  //Delete E's incident halfedges HE1, HE2.
  E->_disconnect_HE (HE1);
  delete HE1;
  E->_disconnect_HE (HE2);
  delete HE2;

  //Delete the sharing edge E.
  if (delete_E)
    H->remove_edge (E);

  return true;
}

//###### Sheet Topology in Merging /Splitting ######

//: Given a starting N and icurve C, find the icurve_vec (from N to some otherN_set) 
//  that separates the sheet into two, via BFS search on the tree network of icurve_pairs on the sheet.
//  It is an acyclic tree, otherwise the sheet is divided by icurve_pairs into two regions (a condraction)!
//
void get_S_icurve_vec_otherN (dbmsh3d_sheet* S, dbmsh3d_vertex* startN, dbmsh3d_edge* startC, 
                              vcl_vector<dbmsh3d_edge*>& icurve_otherN_N,
                              dbmsh3d_vertex*& otherN)
{
  otherN = NULL;

  //Get the set of i-curves of this sheet.
  vcl_set<dbmsh3d_edge*> icurve_pairs;
  S->get_icurve_pairs (icurve_pairs);

  //Get the set of bnd/icurve-loop nodes incident to i-curves as condidate target nodes.
  vcl_set<dbmsh3d_vertex*> otherN_set;
  S->get_bnd_iloop_N_icurve (otherN_set);
  otherN_set.erase (startN);

  //Initialization of the BFS searching.
  // - set all icurve_pairs's nodes to be unvisited.
  // - initialize each vertex.F_list_ as the prev_E pointer.
  vcl_set<dbmsh3d_edge*>::iterator it = icurve_pairs.begin();
  for (; it != icurve_pairs.end(); it++) {
    dbmsh3d_edge* E = (*it);
    E->sV()->set_visited (false);
    E->sV()->set_prev_E (NULL);
    E->eV()->set_visited (false);
    E->eV()->set_prev_E (NULL);
  }

  //Put the startN into the queue Q.
  vcl_queue<dbmsh3d_vertex*> Q;
  Q.push (startN);
  startN->set_prev_E (NULL);

  while (Q.empty() == false) {
    //Pop the first N.
    dbmsh3d_vertex* N = Q.front ();
    Q.pop ();
    assert (N->is_visited() == false);

    //Check if bndN found
    if (otherN_set.find (N) != otherN_set.end()) {
      otherN = N;
      break;
    }

    //Visit N.
    N->set_visited (true);

    //Put all unvisited neighboring Nb into Q.    
    for (dbmsh3d_ptr_node* cur = N->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      if (icurve_pairs.find (E) == icurve_pairs.end())
        continue; //skip if E is not in i-curve set.
      dbmsh3d_vertex* No = E->other_V (N);
      if (No->is_visited() == false) {
        No->set_prev_E (E);
        Q.push (No);
      }
    }
  }

  if (otherN) {
    //bndN found. Backtrack icurve_vec[].
    dbmsh3d_vertex* N = otherN;
    while (N->prev_E() != NULL) {
      icurve_otherN_N.push_back (N->prev_E());
      N = N->prev_E()->other_V (N);
    }
  }

  //Reset all icurve_pairs's node's F_list_[].
  it = icurve_pairs.begin();
  for (; it != icurve_pairs.end(); it++) {
    dbmsh3d_edge* E = (*it);
    E->sV()->set_prev_E (NULL);
    E->eV()->set_prev_E (NULL);
  }  
}

//: Given a starting N and icurve C, find the breaking i-curves till the end.
//  Return false if the found bndN has more than 2 icurve or bnd_chain incident to it!
bool get_S_icurve_vec_bndN (dbmsh3d_sheet* S, dbmsh3d_vertex* startN, dbmsh3d_edge* startC,
                            vcl_vector<vcl_vector<dbmsh3d_edge*> >& IC_pairs_bndN_N,
                            vcl_vector<dbmsh3d_edge*>& IC_loop_E_heads,
                            dbmsh3d_vertex*& bndN)
{
  bndN = NULL;

  //Get the set of i-curve pairs and loops of this sheet.
  vcl_set<dbmsh3d_edge*> icurve_pairs;
  S->get_icurve_pairs (icurve_pairs);
  vcl_set<dbmsh3d_edge*> icurves;
  S->get_icurves (icurves);

  //Get the set of bnd nodes on the bnd_curve loop of bndMC.
  //bndN can be at the end of (i) an i-curve pair or (ii) an i-curve loop.
  ///assert (S->halfedge()->edge() == bndMC);
  vcl_set<dbmsh3d_vertex*> bndN_set;
  //Get the set of boundary nodes incident to any i-curves chain (pairs or loops).
  S->get_bnd_Ns_inc_ICchain (bndN_set);
  bndN_set.erase (startN);

  //Initialization of the BFS searching.
  // - set all icurves's nodes to be unvisited.
  // - initialize each vertex.F_list_ as the prev_E pointer.
  vcl_set<dbmsh3d_edge*>::iterator it = icurves.begin();
  for (; it != icurves.end(); it++) {
    dbmsh3d_edge* E = (*it);
    E->sV()->set_visited (false);
    E->sV()->set_prev_E (NULL);
    E->eV()->set_visited (false);
    E->eV()->set_prev_E (NULL);
  }

  //Put the startN into the queue Q.
  vcl_queue<dbmsh3d_vertex*> Q;
  Q.push (startN);
  startN->set_prev_E (NULL);

  while (Q.empty() == false) {
    //Pop the first N.
    dbmsh3d_vertex* N = Q.front ();
    Q.pop ();
    if (N->is_visited())
      continue; //skip already visited N (possible at the end end of loop).

    //Check if bndN found
    if (bndN_set.find (N) != bndN_set.end()) {
      bndN = N;
      break;
    }

    //Visit N.
    N->set_visited (true);

    //Put all unvisited neighboring Nb into Q.    
    for (dbmsh3d_ptr_node* cur = N->E_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
      if (icurves.find (E) == icurves.end())
        continue; //skip if E is not in i-curve set.
      dbmsh3d_vertex* No = E->other_V (N);
      if (No->is_visited() == false) {
        No->set_prev_E (E);
        Q.push (No);
      }
    }
  }

  if (bndN) { //bndN found. 
    //Check if the found bndN has more than 2 icurve or bnd_chain of S incident to it!
    int n = S->n_bnd_IC_chain_E_inc_V (bndN);
    if (n > 2) {      
      //Reset all icurves's node's F_list_[].
      it = icurves.begin();
      for (; it != icurves.end(); it++) {
        dbmsh3d_edge* E = (*it);
        E->sV()->set_prev_E (NULL);
        E->eV()->set_prev_E (NULL);
      }  
      return false;
    }
    
    //Backtrack icurve_vec[].
    dbmsh3d_vertex* N = bndN;
    dbmsh3d_edge* ic_pair_prevE = NULL;
    dbmsh3d_edge* ic_loop_prevE = NULL;

    vcl_vector<dbmsh3d_edge*> icurve_tmp;

    while (N->prev_E() != NULL) {
      dbmsh3d_edge* E = N->prev_E();
      //E can be either an icurve-pair or an icurve-loop.
      if (icurve_pairs.find (E) != icurve_pairs.end()) {
        //1) icurve-pair.
        if (ic_pair_prevE == NULL)
          IC_pairs_bndN_N.push_back (icurve_tmp);
        IC_pairs_bndN_N[IC_pairs_bndN_N.size()-1].push_back (E);
        ic_pair_prevE = E;
        ic_loop_prevE = NULL;
      }
      else {
        //2) icurve-loop.
        if (ic_loop_prevE == NULL)
          IC_loop_E_heads.push_back (E);
        else {          
          //only add E if E and ic_loop_prevE not in same loop.
          if (S->Es_in_same_loop(E, ic_loop_prevE) == false) 
            IC_loop_E_heads.push_back (E);
        }
        ic_loop_prevE = E;
        ic_pair_prevE = NULL;
      }      
      N = N->prev_E()->other_V (N);
    }
  }

  //Assure that starting and ending of the 'breaking curve' is icurve_pairs,
  //i.e., number of loops is one less than the array of icurve_pairs.
  assert (IC_pairs_bndN_N.size() == IC_loop_E_heads.size() ||
          IC_pairs_bndN_N.size() == IC_loop_E_heads.size() + 1);

  //Reset all icurves's node's F_list_[].
  it = icurves.begin();
  for (; it != icurves.end(); it++) {
    dbmsh3d_edge* E = (*it);
    E->sV()->set_prev_E (NULL);
    E->eV()->set_prev_E (NULL);
  }  

  return true;
}

