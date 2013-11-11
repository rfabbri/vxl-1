//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_curve.cxx
//:
// \file
// \brief Basic 3D curve (polyline) to describe e.g. mesh boundary.
//
//
// \author
//  MingChing Chang  Oct. 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>
#include <vul/vul_printf.h>
#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_hypg.h>

//###############################################################
//###### Fine-scale fs_edges (E_vec) Query Functions ######
//###############################################################

bool dbmsh3d_curve::contain_E (const dbmsh3d_edge* inputE) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    if (E == inputE)
      return true;
  }
  return false;
}

void dbmsh3d_curve::get_Evec (vcl_vector<dbmsh3d_edge*>& Evec) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    Evec.push_back (E);
  }
}

void dbmsh3d_curve::get_Eset (vcl_set<dbmsh3d_edge*>& Eset) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    Eset.insert (E);
  }
}

bool dbmsh3d_curve::all_Es_in_vec (const vcl_vector<dbmsh3d_edge*>& Evec) const
{
  vcl_set<dbmsh3d_edge*> Eset;
  get_Eset (Eset);
  for (unsigned int i=0; i<Evec.size(); i++) {
    Eset.erase (Evec[i]);
  }
  return Eset.empty();
}

dbmsh3d_edge* dbmsh3d_curve::get_E_incident_N (const dbmsh3d_node* N) const
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  assert (E_vec_.size() > 0);
  if (N == s_N()) {
    return E_vec_[0];
  }
  else {
    assert (N == e_N());
    return E_vec_[ E_vec_.size()-1 ];
  }
}

dbmsh3d_edge* _remove_E_inc_V_from_set (vcl_set<dbmsh3d_edge*>& Eset, const dbmsh3d_vertex* V)
{
  vcl_set<dbmsh3d_edge*>::iterator it = Eset.begin();
  for (; it != Eset.end(); it++) {
    dbmsh3d_edge* E = (*it);
    if (E->is_V_incident (V)) {
      Eset.erase (it);
      return E;
    }
  }
  assert (0);
  return NULL;
}

void dbmsh3d_curve::get_Evec_incident_F (const dbmsh3d_face* F, const dbmsh3d_node* N,
                                         vcl_vector<dbmsh3d_edge*>& Evec) const
{
  vcl_set<dbmsh3d_edge*> Eset;
  get_Eset (Eset);

  vcl_set<dbmsh3d_edge*> Eset_incF;
  //Loop through F.he chain and put all E's in Eset to Evec.
  dbmsh3d_halfedge* HE = F->halfedge();
  do {
    if (Eset.find (HE->edge()) != Eset.end())
      Eset_incF.insert (HE->edge());
    HE = HE->next();
  }
  while (HE != F->halfedge() && HE != NULL);

  //trace from node N for Eset_incF[] to make a vector Evec[] starting from N.
  dbmsh3d_vertex* V = N->V();
  while (Eset_incF.empty() == false) {
    //Get one edge E from Eset_incF incident to V.
    dbmsh3d_edge* E = _remove_E_inc_V_from_set (Eset_incF, V);
    Evec.push_back (E);
    V = E->other_V (V);
  }
}

void dbmsh3d_curve::get_Eset_S_M (const dbmsh3d_vertex* M, 
                                  vcl_set<dbmsh3d_edge*>& Eset) const
{
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    if (E->is_V_incident (M)) {
      if (M != s_N()->V())
        Eset.insert (E);
      return;
    }
    Eset.insert (E);
  }
}

void dbmsh3d_curve::get_Eset_M_E (const dbmsh3d_vertex* M, 
                                  vcl_set<dbmsh3d_edge*>& Eset) const
{
  for (int i=int(E_vec_.size())-1; i>=0; i--) {
    dbmsh3d_edge* E = E_vec_[i];
    if (E->is_V_incident (M)) {
      if (M != e_N()->V())
        Eset.insert (E);
      return;
    }
    Eset.insert (E);
  }
}

double dbmsh3d_curve::compute_length_Es () const
{
  //Go through all fs_edges and add up the length.
  double length = 0.0f;
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    length += (float) E->length();
  }
  return length;
}

dbmsh3d_edge* dbmsh3d_curve::get_1st_non_shared_E () const
{
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    if (is_E_shared (E) == false)
      return E;
  }
  return NULL;
}

//###############################################################
//  Fine-scale fs_edges (E_vec) Modification Functions 
//###############################################################

void dbmsh3d_curve::add_E_vec_to_front (const vcl_vector<dbmsh3d_edge*>& E_vec)
{
  if (E_vec.size() == 0)
    return;
  if (E_vec_.size() != 0)
    assert (E_vec_[0] != E_vec[E_vec.size()-1]);
  E_vec_.insert (E_vec_.begin(), E_vec.begin(), E_vec.end());
}

void dbmsh3d_curve::check_add_E_vec_to_front (vcl_vector<dbmsh3d_edge*>& E_vec)
{
  //Fix identical entries at the end of E_vec and and the front of E_vec_.
  while (E_vec_.size() && E_vec.size() && E_vec_[0] == E_vec.back()) {
    E_vec_.erase (E_vec_.begin(), E_vec_.begin()+1);
    E_vec.pop_back ();
  }

  add_E_vec_to_front (E_vec);
}

void dbmsh3d_curve::add_E_vec_to_back (const vcl_vector<dbmsh3d_edge*>& E_vec)
{
  if (E_vec.size() == 0)
    return;
  if (E_vec_.size() != 0)
    assert (E_vec_[E_vec_.size()-1] != E_vec[0]);
  E_vec_.insert (E_vec_.end(), E_vec.begin(), E_vec.end());
}

void dbmsh3d_curve::check_add_E_vec_to_back (vcl_vector<dbmsh3d_edge*>& E_vec)
{
  //Fix identical entries at the end of E_vec_ and the front of E_vec.
  while (E_vec_.size() && E_vec.size() && E_vec[0] == E_vec_.back()) {
    E_vec.erase (E_vec.begin(), E_vec.begin()+1);
    E_vec_.pop_back ();
  }

  add_E_vec_to_back (E_vec);
}

//##########################################################
//  Fine-scale fs_vertices (V_vec) Query Functions 
//##########################################################

void dbmsh3d_curve::get_V_vec (vcl_vector<dbmsh3d_vertex*>& Vvec) const
{
  assert (Vvec.size() == 0);

  if (data_type_ == C_DATA_TYPE_VERTEX) {
    //1) data type is on nodes.
    assert (E_vec_.size() == 0);
    for (unsigned int i=0; i<V_vec_.size(); i++)
      Vvec.push_back (V_vec_[i]);
  }
  else {
    //2) data type is on links.
    assert (data_type_ == C_DATA_TYPE_EDGE);
    assert (V_vec_.size() == 0);

    //The case of only one fs_edge.
    if (E_vec_.size() == 1) {
      dbmsh3d_edge* E = E_vec_[0];
      Vvec.push_back (E->vertices(0));
      Vvec.push_back (E->vertices(1));
      return;
    }

    //Add the first fs_vertex N0
    dbmsh3d_edge* E = E_vec_[0];
    dbmsh3d_edge* E2 = E_vec_[1];
    assert (E != E2);
    dbmsh3d_vertex* V = (dbmsh3d_vertex*) Es_sharing_V (E, E2);
    dbmsh3d_vertex* N0 = (dbmsh3d_vertex*) E->other_V (V);
    Vvec.push_back (N0);

    //Add N1 to V[n-1] through this MC's E_vec_[].
    for (unsigned int i=0; i<E_vec_.size()-1; i++) {
      E = E_vec_[i];
      E2 = E_vec_[i+1];
      ///assert (E != E2); !!!
      V = (dbmsh3d_vertex*) Es_sharing_V (E, E2);
      Vvec.push_back (V);
    }

    //Add the last fs_vertex V[n]
    V = (dbmsh3d_vertex*) E2->other_V (V);
    Vvec.push_back (V);
  }
}

void dbmsh3d_curve::get_V_set (vcl_set<dbmsh3d_vertex*>& Vset) const
{
  if (data_type_ == C_DATA_TYPE_EDGE) {
    assert (E_vec_.size() != 0);
    //Put in the starting V and ending V.
    Vset.insert (s_N()->V());
    Vset.insert (e_N()->V());
    //Put in each V between a consecutive Li and E[i+1].
    for (int i=0; i<int(E_vec_.size())-1; i++) {
      dbmsh3d_edge* Li = E_vec_[i];
      dbmsh3d_edge* Lj = E_vec_[i+1];
      bool loop;
      dbmsh3d_vertex* V = Es_sharing_V_check (Li, Lj, loop);
      assert (loop == false);
      Vset.insert (V);
    }
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    for (unsigned int i=0; i<V_vec_.size(); i++) {
      dbmsh3d_vertex* V = V_vec_[i];
      Vset.insert (V);
    }
  }
}

bool dbmsh3d_curve::is_V_on_C (const dbmsh3d_vertex* inputV) const
{
  if (data_type_ == C_DATA_TYPE_EDGE) {
    assert (E_vec_.size() != 0);
    //Put in the starting V and ending V.
    if (s_N()->V() == inputV)
      return true;
    if (e_N()->V() == inputV)
      return true;
    for (int i=0; i<int(E_vec_.size())-1; i++) {
      dbmsh3d_edge* Li = E_vec_[i];
      dbmsh3d_edge* Lj = E_vec_[i+1];
      bool loop;
      dbmsh3d_vertex* V = Es_sharing_V_check (Li, Lj, loop);
      assert (loop == false);
      if (V == inputV)
        return true;
    }
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    for (unsigned int i=0; i<V_vec_.size(); i++) {
      dbmsh3d_vertex* V = V_vec_[i];
      if (V == inputV)
        return true;
    }
  }
  return false;
}

void dbmsh3d_curve::get_V_set_S_M (const dbmsh3d_vertex* M, 
                                  vcl_set<dbmsh3d_vertex*>& Vset) const
{  
  dbmsh3d_vertex* V = s_N()->V();  
  if (V == M) {
    Vset.insert (V);
    return;
  }
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    Vset.insert (V);
    V = E->other_V (V);
    if (V == M) {
      Vset.insert (V);
      return;
    }
  }
}

void dbmsh3d_curve::get_V_set_M_E (const dbmsh3d_vertex* M, 
                                  vcl_set<dbmsh3d_vertex*>& Vset) const
{
  dbmsh3d_vertex* V = e_N()->V();
  if (V == M) {
    Vset.insert (V);
    return;
  }
  for (int i=int(E_vec_.size())-1; i>=0; i--) {
    dbmsh3d_edge* E = E_vec_[i];
    Vset.insert (V);
    V = E->other_V (V);
    if (V == M) {
      Vset.insert (V);
      return;
    }
  }
}

int dbmsh3d_curve::get_nE_of_V_from_N (const dbmsh3d_vertex* inputV, const dbmsh3d_node* N) const
{
  if (vertices_[0] == N) {
    for (int i=0; i<int(E_vec_.size()); i++) {
      if (E_vec_[i]->is_V_incident (inputV))
        return i+1;
    }
  }
  else {
    assert (vertices_[1] == N);
    for (int i=int(E_vec_.size())-1; i>=0; i--) {
      if (E_vec_[i]->is_V_incident (inputV))
        return int(E_vec_.size())-i;
    }
  }
  
  return -1; //not found,
}

dbmsh3d_vertex* dbmsh3d_curve::get_middle_V () const 
{
  if (data_type_ == C_DATA_TYPE_EDGE) {
    assert (E_vec_.size() != 0);
    if (E_vec_.size() == 1) {
      return E_vec_[0]->sV();
    }
    else if (E_vec_.size() == 2) {
      dbmsh3d_edge* L0 = E_vec_[0];
      dbmsh3d_edge* L1 = E_vec_[1];
      dbmsh3d_vertex* V = Es_sharing_V (L0, L1);
      assert (V);
      return (dbmsh3d_vertex*) V;
    }
    else {
      unsigned int mid = (unsigned int) (E_vec_.size() / 2);
      dbmsh3d_edge* L0 = E_vec_[mid];
      dbmsh3d_edge* L1 = E_vec_[mid+1];
      dbmsh3d_vertex* V = Es_sharing_V (L0, L1);
      assert (V);
      return (dbmsh3d_vertex*) V;
    }
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    assert (V_vec_.size() != 0);
    unsigned int mid = (unsigned int) (V_vec_.size() / 2);
    return V_vec_[mid];
  }
}

double dbmsh3d_curve::compute_length_Vs () const
{
  //Go through all fs_vertices and add up the length
  //0-1, 1-2, ... n-2 to n-1
  assert (V_vec_.size() > 1);
  double length = 0.0f;
  for (unsigned int i=0; i<V_vec_.size()-1; i++) { 
    dbmsh3d_vertex* V = V_vec_[i];
    dbmsh3d_vertex* V2 = V_vec_[i+1];
    length += (float) vgl_distance (V->pt(), V2->pt());
  }
  return length;
}

//###############################################################
//  Fine-scale fs_vertices (V_vec) Modification Functions 
//###############################################################

void dbmsh3d_curve::_reverse_V_vec()
{
  assert (data_type_ == C_DATA_TYPE_VERTEX);
  vcl_reverse (V_vec_.begin(), V_vec_.end());
}

//: insert the curve C's vertices to the current dbmsh3d_curve
//  the insertion includes the starting point, but does not include the end point.
void dbmsh3d_curve::insert_Vs_of_C (dbmsh3d_curve* C, dbmsh3d_node* N)
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  assert (C->sV()->id() == N->id() || C->eV()->id() == N->id());

  vcl_vector<dbmsh3d_vertex*> Vvec;
  C->get_V_vec (Vvec);

  const dbmsh3d_vertex* C_s_N = Vvec[0];
  const dbmsh3d_vertex* C_e_N = Vvec[Vvec.size()-1];

  //Two cases: s_N->e_N or e_N->s_N.
  if (C_s_N == N->V()) {
    for (unsigned int i=0; i<Vvec.size()-1; i++) {
      dbmsh3d_vertex* V = Vvec[i];
      V_vec_.push_back (V);
    }
  }
  else if (C_e_N == N->V()) {
    for (unsigned int i=Vvec.size()-1; i>0; i--) {
      dbmsh3d_vertex* V = Vvec[i];
      V_vec_.push_back (V);
    }
  }
  else
    assert (0);
}

//###############################################################
//  Connectivity Query Functions
//###############################################################

bool dbmsh3d_curve::contain_V (const dbmsh3d_vertex* inputV) const
{  
  if (data_type_ == C_DATA_TYPE_EDGE) {
    for (unsigned int i=0; i<E_vec_.size(); i++) {
      if (E_vec_[i]->is_V_incident (inputV))
        return true;
    }
    return false;
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    for (unsigned int i=0; i<V_vec_.size(); i++) {
      if (V_vec_[i] == inputV)
        return true;
    }
    return false;
  }
}

bool dbmsh3d_curve::all_Es_shared () const
{
  //Not all E_vec_[] are shared_E.
  if (E_vec_.size() == n_shared_Es())
    return true;
  else
    return false;
}

bool dbmsh3d_curve::shared_E_incident_to_V (const dbmsh3d_vertex* V) const
{
  for (dbmsh3d_ptr_node* cur = shared_E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->is_V_incident (V))
      return true;
  }
  return false;
}

  //: Return true if all shared_E[] are with one of the given set of C's.
bool dbmsh3d_curve::shared_E_with_Cset (vcl_set<dbmsh3d_curve*>& shared_E_Cset) const
{
  //Loop through each shared_E of MC and try to remove isolated entry from other curves.
  for (dbmsh3d_ptr_node* cur = shared_E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    bool E_in_Cset = false;
    vcl_set<dbmsh3d_curve*>::iterator it = shared_E_Cset.begin();
    for (; it != shared_E_Cset.end(); it++) {
      dbmsh3d_curve* C = (*it);
      if (C->is_E_shared (E))
        E_in_Cset = true;
    }    
    if (E_in_Cset == false)
      return false;
  }
  return true;
}

//###############################################################
//  Connectivity Modification Functions
//###############################################################

void dbmsh3d_curve::switch_sV_eV ()
{
  dbmsh3d_edge::switch_sV_eV();
  if (data_type_ == C_DATA_TYPE_EDGE)
    _reverse_E_vec ();
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    _reverse_V_vec ();
  }
}

void dbmsh3d_curve::add_shared_Es_to_C (dbmsh3d_curve* C)
{
  for (dbmsh3d_ptr_node* cur = shared_E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    C->check_add_shared_E (E); //add_shared_E
  }
}

void dbmsh3d_curve::pass_shared_Es_to_C (dbmsh3d_curve* C)
{
  add_shared_Es_to_C (C);
  clear_shared_E_list ();
}

void dbmsh3d_curve::check_add_all_Es_to_C (dbmsh3d_curve* C)
{
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    C->check_add_shared_E (E);
  }
}

void dbmsh3d_curve::compute_c_type ()
{
  assert (data_type_ == C_DATA_TYPE_EDGE);
  dbmsh3d_edge* E;

  for (unsigned int i=0; i<E_vec_.size(); i++) {
    E = E_vec_[i];
    E->compute_e_type ();
    E->sV()->compute_v_type ();
    E->eV()->compute_v_type ();
  }
  
  if (E->e_type() == E_TYPE_RIB)
    c_type_ = C_TYPE_RIB;
  else if (E->e_type() == E_TYPE_AXIAL)
    c_type_ = C_TYPE_AXIAL;
  else if (E->e_type() == E_TYPE_DEGE_AXIAL)
    c_type_ = C_TYPE_DEGE_AXIAL;
  else
    ; ///assert (0); !! DEBUG THIS bones1
}

//##########################################################
//###### Other functions ######
//##########################################################

bool dbmsh3d_curve::check_integrity ()
{
  if (dbmsh3d_edge::check_integrity() == false) {
    assert (0);
    return false;
  }

  //Check the incidence integrity of the ending fine-scale elements.
  if (data_type_ == C_DATA_TYPE_EDGE) {
    if (E_vec_[0]->is_V_incident (s_N()->V()) == false) {
      assert (0);
      return false;
    }
    if (E_vec_[E_vec_.size()-1]->is_V_incident (e_N()->V()) == false) {
      assert (0);
      return false;
    }
  }
  else if (data_type_ == C_DATA_TYPE_VERTEX) {
    if (V_vec_[0] != s_N()->V()) {
      assert (0);
      return false;
    }
    if (V_vec_[V_vec_.size()-1] != e_N()->V()) {
      assert (0);
      return false;
    }
  }
  else { //no possible.
    assert (0);
    return false;
  }

  //Check integrity of all fine-scale edges.
  for (unsigned int i=0; i<E_vec_.size(); i++) {
    dbmsh3d_edge* E = E_vec_[i];
    if (E->check_integrity () == false) {
      assert (0);
      return false;
    }
  }

  //Check integrity of all shared and special edges.
  vcl_set<dbmsh3d_edge*> shared_Eset;
  for (dbmsh3d_ptr_node* cur = shared_E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    shared_Eset.insert (E);
    if (E->halfedge() == NULL) {
      assert (0);
      return false;
    }
    if (E->e_type() != E_TYPE_SHARED) {
      assert (0);
      return false;
    }
  }

  if (c_type_ != C_TYPE_VIRTUAL) {
    //Assert that all E's not in shared_E_list[] is not of type shared.
    for (unsigned int i=0; i<E_vec_.size(); i++) {
      if (shared_Eset.find (E_vec_[i]) == shared_Eset.end()) {
        if (E_vec_[i]->e_type() == E_TYPE_SHARED) {
          assert (0);
          return false;
        }
      }
    }
  }

  return true;
}

dbmsh3d_edge* dbmsh3d_curve::clone () const
{
  dbmsh3d_curve* C2 = new dbmsh3d_curve (NULL, NULL, id_);
  C2->set_data_type (data_type_);
  return C2;
}

void dbmsh3d_curve::_clone_C_EV_conn (dbmsh3d_curve* C2, dbmsh3d_mesh* M2) const
{
  if (data_type_ == C_DATA_TYPE_EDGE) {
    for (unsigned int i=0; i<E_vec_.size(); i++) {
      dbmsh3d_edge* E = E_vec_[i];
      dbmsh3d_edge* E2 = M2->edgemap (E->id());
      C2->add_E_to_back (E2);
    }
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    for (unsigned int i=0; i<V_vec_.size(); i++) {
      dbmsh3d_vertex* V = V_vec_[i];
      dbmsh3d_vertex* V2 = M2->vertexmap (V->id());
      C2->add_V_to_back (V2);
    }
  }

  //the shared_E_list_[]  
  for (dbmsh3d_ptr_node* cur = shared_E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    dbmsh3d_edge* E2 = M2->edgemap (E->id());
    C2->add_shared_E (E2);
  }
}

dbmsh3d_curve* dbmsh3d_curve::clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2) const
{
  dbmsh3d_curve* C2 = (dbmsh3d_curve*) clone ();
  _clone_E_V_conn (C2, HG2);
  _clone_C_EV_conn (C2, M2);  
  return C2;
}

void dbmsh3d_curve::getInfo (vcl_ostringstream& ostrm)
{
  //not yet implemented.
  assert (0);
}

double dbmsh3d_curve::compute_length () const
{
  if (data_type_ == C_DATA_TYPE_EDGE) {
    return compute_length_Es ();
  }
  else {
    assert (data_type_ == C_DATA_TYPE_VERTEX);
    return compute_length_Vs ();
  }
}

//###############################################################

bool Cs_sharing_E (const dbmsh3d_curve* C1, const dbmsh3d_curve* C2)
{
  vcl_set<void*> C1_shared_Es, C2_shared_Es;
  C1->get_shared_Es (C1_shared_Es);
  C2->get_shared_Es (C2_shared_Es);
  vcl_set<void*> result;
  vcl_set_intersection (C1_shared_Es.begin(), C1_shared_Es.end(),
                        C2_shared_Es.begin(), C2_shared_Es.end(),
                        vcl_inserter(result, result.end()));
  return result.size() != 0;
}

void remove_Cs_sharing_E (dbmsh3d_curve* C1, dbmsh3d_curve* C2)
{
  //Loop through C1's shared_E_list_ and find in C2's and remove both.
  dbmsh3d_ptr_node* cur1 = C1->shared_E_list();
  dbmsh3d_ptr_node* prev1 = NULL;
  while (cur1 != NULL) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur1->ptr();
    //Find cur1->ptr() in C2->shared_E_list and remove it.
    bool r = C2->del_shared_E (E);
    if (r) { //Also remove cur1 from C1->shared_E_list.
      if (prev1 == NULL) {
        C1->set_shared_E_list (cur1->next());
        delete cur1;
        cur1 = C1->shared_E_list();
      }
      else {
        prev1->set_next (cur1->next());
        delete cur1;
        cur1 = prev1->next();
      }
    }
    else { //visit the next.
      prev1 = cur1;    
      cur1 = cur1->next();
    }
  }
}

//: For the case the input curve C has 3 incident sheets, 
//  where 2 of them are identical (2-incidence), e.g. (tabS, baseS, baseS).
//  Get the non-duplicated sheet tabS and the duplicated sheet baseS, and return true.
//  Return false if the local config is not in such case.
bool C_get_non_dup_S (dbmsh3d_curve* C, dbmsh3d_sheet* & tabS, dbmsh3d_sheet* & baseS)
{
  tabS = NULL;
  baseS = NULL;
  vcl_vector<dbmsh3d_face*> S_vec;
  C->get_incident_Fs (S_vec);
  if (S_vec.size() != 3)
    return false;

  if (S_vec[0] == S_vec[1] && S_vec[1] == S_vec[2])
    return false; //if all 3 S are the same.

  if (S_vec[0] == S_vec[1]) {
    tabS = (dbmsh3d_sheet*) S_vec[2];
    baseS = (dbmsh3d_sheet*) S_vec[0];
    return true;
  }

  if (S_vec[1] == S_vec[2]) {
    tabS = (dbmsh3d_sheet*) S_vec[0];
    baseS = (dbmsh3d_sheet*) S_vec[1];
    return true;
  }

  if (S_vec[0] == S_vec[2]) {
    tabS = (dbmsh3d_sheet*) S_vec[1];
    baseS = (dbmsh3d_sheet*) S_vec[0];
    return true;
  }

  return false;
}

//: merge E_vec[] of C2 to C1 at node N, i.e., C1 - N - C2.
void merge_C1_C2_Es (dbmsh3d_curve* C1, dbmsh3d_curve* C2,
                     dbmsh3d_node* N, const bool make_shared_E)
{
  assert (C1->data_type() == C_DATA_TYPE_EDGE);
  assert (C2->data_type() == C_DATA_TYPE_EDGE);

  if (N == C1->s_N()) 
    C1->switch_sV_eV (); //swap C1's sN and eN
  
  //Add C2's E_vec[] to C1's end.
  if (N == C2->s_N()) {    
    for (int i=0; i<int(C2->E_vec().size()); i++) {
      dbmsh3d_edge* E = C2->E_vec(i);
      C1->add_E_to_back (E);
      if (make_shared_E)
        C1->check_add_shared_E (E);
    }
  }
  else {
    assert (N == C2->e_N());
    for (int i=int(C2->E_vec().size())-1; i>=0; i--) {
      dbmsh3d_edge* E = C2->E_vec(i);
      C1->add_E_to_back (E);
      if (make_shared_E)
        C1->check_add_shared_E (E);
    }
  }
}
