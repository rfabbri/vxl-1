//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_edge.cxx
//:
// \file
// \brief Basic 3d edge
//
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_sstream.h>

#include <vgl/vgl_distance.h>

#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_sheet.h>
#include <dbmsh3d/dbmsh3d_pt_set.h>

//###############################################################
//###### Constructor/Destructor ######
//###############################################################

dbmsh3d_edge::~dbmsh3d_edge () 
{ 
  //when destruct this edge, also destruct all associated halfedges
  if (halfedge_ == NULL) {
  }  
  else if (halfedge_->pair() == NULL) {
    //if it has only one halfedge)
    delete halfedge_;
  }
  else { //delete all associated halfedges pairs
    dbmsh3d_halfedge* cur_he = halfedge_->pair();
    while (cur_he != halfedge_) {
      dbmsh3d_halfedge* todel = cur_he;
      cur_he = cur_he->pair();
      delete todel;
    }
    //delete the last one
    delete halfedge_;
  }

  //remove the pointer from two ending vertices to this
  if (vertices_[0])
    vertices_[0]->del_incident_E (this);
  if (vertices_[1])
    if (vertices_[1] != vertices_[0])
      vertices_[1]->del_incident_E (this);
}

//###############################################################
//###### Connectivity query functions ######
//###############################################################

unsigned int dbmsh3d_edge::n_incident_Fs () const 
{  
  if (halfedge_ == NULL)
    return 0; //no associated halfedge 

  if (halfedge_->pair() == NULL)
    return 1; //only one associated halfedge (no loop)

  unsigned int count = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    count++;
    HE = HE->pair();
  }
  while (HE != halfedge_);

  return count;
}

bool dbmsh3d_edge::is_F_incident (const dbmsh3d_face* F) const
{ 
  if (halfedge_ == NULL) {
    return false;
  }
  else if (halfedge_->pair() == NULL) {
    if (halfedge_->face() == F)
      return true;
  }
  else {
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      if (HE->face() == F)
        return true;
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }
  return false;
}

bool dbmsh3d_edge::is_Fset_incident (vcl_set<dbmsh3d_face*>& Fset) const
{
  if (halfedge_ == NULL)
    return false;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (Fset.find (HE->face()) != Fset.end())
      return true; //found incident face in Fset.
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

//: loop through all halfedges and find the one incident to F.
dbmsh3d_halfedge* dbmsh3d_edge::get_HE_of_F (const dbmsh3d_face* F,
                                             dbmsh3d_halfedge* startHE) const
{
  if (startHE == NULL)
    startHE = halfedge_;
  if (startHE == NULL)
    return NULL;  
  dbmsh3d_halfedge* HE = startHE;
  do { //Loop through the circular list of halfedges.
    if (HE->face() == F)
      return HE;
    HE = HE->pair();
  }
  while (HE != startHE && HE != NULL);
  return NULL;
}

//: For a geometric hypergraph, there can be multiple of HE_set of a single F.
void dbmsh3d_edge::get_HEset_of_F (const dbmsh3d_face* F, 
                                   vcl_set<dbmsh3d_halfedge*>& HEset) const
{
  assert (HEset.empty());

  if (halfedge_ == NULL) {
    return;
  }
  else if (halfedge_->pair() == NULL) {
    if (halfedge_->face() == F)
      HEset.insert (halfedge_);
    return;
  }
  else { //Loop through the circular list of halfedges.
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      if (HE->face() == F)
        HEset.insert (HE);
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }
}

bool dbmsh3d_edge::only_incident_to_Fset_in_Fmap (vcl_set<dbmsh3d_face*>& F_set, 
                                                  vcl_map<int, dbmsh3d_face*>& F_map) const
{
  if (halfedge_ == NULL)
    return true;

  dbmsh3d_halfedge* HE = halfedge_;
  if (HE->pair() == NULL) {
    dbmsh3d_face* F = HE->face();
    if (F_set.find(F) == F_set.end() && F_map.find(F->id()) != F_map.end())
      return false;
    else 
      return true;
  }

  do {
    dbmsh3d_face* F = HE->face();
    if (F_set.find(F) == F_set.end() && F_map.find(F->id()) != F_map.end())
      return false;
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return true;
}

//: find the inputHE and the prev_pair_HE of this edge.
//  return false if not found.
bool dbmsh3d_edge::_find_HE_prev_pair (const dbmsh3d_halfedge* inputHE, 
                                       dbmsh3d_halfedge* & prev_pair_HE) const
{
  prev_pair_HE = NULL;

  if (halfedge_ == NULL)
    return false;
  else if (halfedge_->pair() == NULL) {
    if (halfedge_ == inputHE)
      return true;
    else
      return false;
  }
  else {
    prev_pair_HE = halfedge_;
    do {
      dbmsh3d_halfedge* HE = prev_pair_HE->pair();
      if (inputHE == HE)
        return true;
      prev_pair_HE = HE;
      HE = HE->pair();
    }
    while (prev_pair_HE != halfedge_);
  }
  return false;
}

void dbmsh3d_edge::get_incident_Fs (vcl_vector<dbmsh3d_face*>& incident_faces) const
{
  //If there's no associated halfedge 
  if (halfedge_ == NULL) {
    return;
  }
  //If there's only one associated halfedge (no loop)
  else if (halfedge_->pair() == NULL) {
    incident_faces.push_back (halfedge_->face());
    return;
  }
  //The last case, the associated halfedges form a circular list
  else {
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      incident_faces.push_back (HE->face());
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }
  return;
}

//: Check if all incident fs_faces are in the given set.
const bool dbmsh3d_edge::all_incident_Fs_in_set (vcl_set<dbmsh3d_face*>& Fset) const
{
  if (halfedge_ == NULL)
    return 0;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (Fset.find (F) == Fset.end())
      return false; //not found
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return true;
}

bool dbmsh3d_edge::all_incident_Fs_visited () const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F->b_visited() == false)
      return false;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return true;
}

bool dbmsh3d_edge::all_incident_Fs_visited_except (const dbmsh3d_face* inputF) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F != inputF) {
      if (F->b_visited() == false)
        return false;
    }
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return true;
}

dbmsh3d_face* dbmsh3d_edge::incident_F_given_E (dbmsh3d_edge* other_incident_E) const
{
  if (halfedge_ == NULL)
    return NULL;
  //if there's only one associated halfedge (no loop)
  if (halfedge_->pair() == NULL) {
    if (halfedge_->face()->is_bnd_E (other_incident_E))
      return halfedge_->face();
    else
      return NULL;
  }
  //the last case, the associated halfedges form a circular list
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->is_bnd_E (other_incident_E))
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

dbmsh3d_face* dbmsh3d_edge::incident_F_given_V (dbmsh3d_vertex* incident_V) const
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL) {
    if (halfedge_->face()->is_bnd_V (incident_V))
      return halfedge_->face();
    else
      return NULL;
  }
  //the last case, the associated halfedges form a circular list
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->is_bnd_V (incident_V))
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//: If E is 2-incident to one sheet
//  (internal to only one sheet), return the sheet.
//  Else, return NULL.
dbmsh3d_face* dbmsh3d_edge::is_n_incident_to_one_S (const unsigned int n) const
{
  vcl_vector<dbmsh3d_face*> incident_faces;
  get_incident_Fs (incident_faces);
  assert (n != 0);

  if (n == 1) {
    if (incident_faces.size() == 1)
      return incident_faces[0];
    else
      return NULL;
  }
  else {
    if (incident_faces.size() == n) { //n >= 2
      dbmsh3d_face* F = incident_faces[0];    
      for (unsigned int i=1; i<n; i++) {
        if (incident_faces[i] != F)
          return NULL; //More than one face found, return NULL.
      }
      return F;
    }
  }
  return NULL;
}

dbmsh3d_face* dbmsh3d_edge::other_2_manifold_F (dbmsh3d_face* inputF) const
{
  if (n_incident_Fs() > 2)
    return NULL;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F != inputF)
      return F;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

//: given a tau, return the extrinsic coordinate
//  A --- p --- B
//  p = A + AP / AB;
vgl_point_3d<double> dbmsh3d_edge::_point_from_tau (const double tau) const
{
  dbmsh3d_vertex* sv = sV();
  dbmsh3d_vertex* ev = eV();
  vgl_point_3d<double> a = sv->pt();
  vgl_point_3d<double> b = ev->pt();
  const vgl_vector_3d<double> vAB = b - a;
  return a + vAB * tau / vgl_distance (a, b);
}

vgl_point_3d<double> dbmsh3d_edge::mid_pt() const
{
  dbmsh3d_vertex* sv = vertices_[0];
  dbmsh3d_vertex* ev = vertices_[1];
  return centre (sv->pt(), ev->pt());
}

const double dbmsh3d_edge::length () const
{
  double d = vgl_distance (sV()->pt(), eV()->pt());
  return d;
}

//###############################################################
//  Validity query / computation functions
//###############################################################

//: return if this fs_edge is valid or not.
//  if any of the incident patchElm is valid, this linkElm is valid.
bool dbmsh3d_edge::is_valid_via_F () const
{
  if (halfedge_ == NULL)
    return false;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->b_valid()) {
      return true;
    }
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

//: return if this fs_edge is finite or not.
//  if any of the incident patchElm is finite, this linkElm is finite.
//
bool dbmsh3d_edge::is_finite_via_F () const
{
  if (halfedge_ == NULL)
    return false;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->b_finite())
      return true;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

//: Return true if this fs_edge is incident to a valid finite fs_face.
//
bool dbmsh3d_edge::is_valid_finite_via_F () const
{
  if (halfedge_ == NULL)
    return false;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->b_valid() && HE->face()->b_finite())
      return true;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

const unsigned int dbmsh3d_edge::count_valid_Fs () const
{  
  if (halfedge_ == NULL)
    return 0;

  unsigned int count = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F->b_valid())
      count++;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return count;
}

dbmsh3d_face* dbmsh3d_edge::get_1st_valid_F () const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F->b_valid())
      return F;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

void dbmsh3d_edge::get_valid_Fs (vcl_set<dbmsh3d_face*>& valid_F_set) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F->b_valid())
      valid_F_set.insert (F);
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
}

//: For NORMAL type linkElm, there are only two valid patchElms on the linkElm.
//  return the other valid patchElm.
dbmsh3d_face* dbmsh3d_edge::other_valid_F (const dbmsh3d_face* inputF) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_face* F = HE->face();
    if (F->b_valid() && F != inputF)
      return F;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

void dbmsh3d_edge::compute_e_type (const bool override_s, const bool only_valid_F)
{
  if (override_s == false)
    if (e_type() == E_TYPE_SPECIAL && e_type() == E_TYPE_SHARED)
      return;

  int n;
  if (only_valid_F)
    n = count_valid_Fs();
  else
    n = n_incident_Fs ();

  switch (n) {
  case 0: //isolated edge
    set_e_type (E_TYPE_BOGUS);
  break;
  case 1: //Bordering A3_rib_elm
    set_e_type (E_TYPE_RIB);
  break;
  case 2: //Part of the interior of coarse-scale MA sheets 
    set_e_type (E_TYPE_MANIFOLD);
  break;
  case 3: //Axial A13 curve
    set_e_type (E_TYPE_AXIAL);
  break;
  default: //Degenerate A1n curve
    assert (n > 3);
    set_e_type (E_TYPE_DEGE_AXIAL);
  break;
  }
}


//###############################################################
//###### Connectivity Modification Functions ######
//###############################################################

void dbmsh3d_edge::_connect_HE_end (dbmsh3d_halfedge* HE) 
{  
  //Note that the link list is circular, but not necessarily geometrically ordered!
  if (halfedge_ == NULL) { //1)
    halfedge_ = HE;
    return;
  }  
  else if (halfedge_->pair() == NULL) { //2)
    halfedge_->set_pair (HE);
    HE->set_pair (halfedge_);
    return;
  }  
  else { //3) The general circular list case
    //Assume the order of the halfedges is not important.
    //Just insert it to the 2nd.
    HE->set_pair (halfedge_->pair());
    halfedge_->set_pair (HE);
    return;
  }
}

//: Delete one of the halfedge of this edge.
//  also fix the circular list of halfedge::pair_'s
bool dbmsh3d_edge::_disconnect_HE (dbmsh3d_halfedge* inputHE) 
{
  //Find the inputHE and the prev_pair_HE of this edge.
  dbmsh3d_halfedge* prev_pair_HE = NULL;
  if (_find_HE_prev_pair (inputHE, prev_pair_HE) == false)
    return false; //Can't find inputHE on this edge.

  //First set its edge pointer to NULL (disconnect)
  inputHE->set_edge (NULL);
 
  if (inputHE->pair() == NULL) {  //The only-one-inputHE case
    assert (prev_pair_HE == NULL);
    halfedge_ = NULL;
    return true;
  }

  dbmsh3d_halfedge* next_pair_HE = inputHE->pair();

  //if the one to be disconnected is the one the edge pointing to,
  //  replace the pointer with the next_pair_HE available pointer.
  if (halfedge_ == inputHE)
    halfedge_ = next_pair_HE;

  //The case that inputHE becomes the only-one halfedge.
  if (prev_pair_HE == next_pair_HE) {
    halfedge_->set_pair (NULL);
    inputHE->set_pair (NULL);
    return true;
  }

  //The last case: general circular loop case
  prev_pair_HE->set_pair (next_pair_HE);

  // disconnect this inputHE from its pair
  inputHE->set_pair (NULL);

  return true;
}

//: disconnect all incident faces and return the vector of all such faces.
void dbmsh3d_edge::_disconnect_all_Fs ()
{
  vcl_set<dbmsh3d_face*> disconn_faces;
  _disconnect_all_Fs (disconn_faces);
}

//: disconnect all incident faces and return the vector of all such faces.
void dbmsh3d_edge::_disconnect_all_Fs (vcl_set<dbmsh3d_face*>& disconn_faces)
{
  //Repeatly disconnect all incident faces until finish.
  dbmsh3d_halfedge* HE = halfedge_;
  while (HE != NULL) {
    dbmsh3d_halfedge* pair = HE->pair();    
    disconn_faces.insert (HE->face());
    bool result = _disconnect_remove_HE (HE);
    assert (result);
    HE = pair;
  }
  assert (halfedge_ == NULL);
}

//###############################################################
//###### For the edge of a 2-manifold triangular mesh only ######
//###############################################################

dbmsh3d_halfedge* dbmsh3d_edge::m2_other_HE (dbmsh3d_halfedge* inputHE)
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE != inputHE)
      return HE;
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//: for 2-manifold mesh only
dbmsh3d_face* dbmsh3d_edge::m2_other_face (dbmsh3d_face* inputF)
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face() != inputF)
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//###############################################################
//###### Other functions ######
//###############################################################

bool dbmsh3d_edge::check_integrity () const
{
  if (vertices_[0]->is_E_incident (this) == false) {
    assert (0);
    return false;
  }
  if (vertices_[1]->is_E_incident (this) == false) {
    assert (0);
    return false;
  }

  if (halfedge_ == NULL)
    return true;
  if (halfedge_->pair() == NULL) {
    if (halfedge_->edge() != this) {
      assert (0);
      return false;
    }
    else
      return true;
  }
  dbmsh3d_halfedge* HE = halfedge_;
  vcl_set<dbmsh3d_halfedge*> HE_set;
  do {
    if (halfedge_->edge() != this) {
      assert (0);
      return false;
    }
    //Check that no duplicate HEs are in the chain.
    if (HE_set.find (HE) != HE_set.end()) {
      assert (0);
      return false; //Duplicate HEs found.
    }
    HE_set.insert (HE);
    HE = HE->pair();
  }
  while (HE != halfedge_);

  return true;
}

dbmsh3d_edge* dbmsh3d_edge::clone() const
{
  dbmsh3d_edge* E2 = new dbmsh3d_edge (NULL, NULL, id_);
  return E2; 
}

void dbmsh3d_edge::_clone_E_V_conn (dbmsh3d_edge* E2, dbmsh3d_pt_set* PS2) const 
{  
  dbmsh3d_vertex* sV2 = PS2->vertexmap (sV()->id());
  dbmsh3d_vertex* eV2 = PS2->vertexmap (eV()->id());

  E2->_set_vertex (0, sV2);
  sV2->add_incident_E (E2);

  E2->_set_vertex (1, eV2);  
  eV2->add_incident_E (E2);    
}

dbmsh3d_edge* dbmsh3d_edge::clone (dbmsh3d_pt_set* PS2) const
{
  dbmsh3d_edge* E2 = clone ();
  _clone_E_V_conn (E2, PS2);
  return E2;
}

void dbmsh3d_edge::getInfo (vcl_ostringstream& ostrm) const
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbmsh3d_edge id: %d (vertices [%d] - [%d])     ", id_,
               vertices_[0]->id(), vertices_[1]->id()); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  //: the incident faces via halfedges
  int n_halfedges = n_incident_Fs ();
  vcl_sprintf (s, " %d HEs: ", n_halfedges); ostrm<<s;
  
  if (halfedge_ == NULL) {
    vcl_sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->pair() == NULL) {
    vcl_sprintf (s, "%d ", halfedge_->face()->id()); ostrm<<s;
  }
  else {
    //The last case, the associated halfedges form a circular list
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      vcl_sprintf (s, "%d ", HE->face()->id()); ostrm<<s;
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//###############################################################

bool same_incident_Fs (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2)
{  
  vcl_vector<dbmsh3d_face*> E1_incident_Fs;
  vcl_vector<dbmsh3d_face*> E2_incident_Fs;
  E1->get_incident_Fs (E1_incident_Fs);
  E2->get_incident_Fs (E2_incident_Fs);
  if (E1_incident_Fs.size() != E2_incident_Fs.size())
    return false;

  //: Build multi_sets for comparison.
  vcl_multiset<dbmsh3d_face*> E1_F_set;
  E1_F_set.insert (E1_incident_Fs.begin(), E1_incident_Fs.end());
  vcl_multiset<dbmsh3d_face*> E2_F_set;
  E2_F_set.insert (E2_incident_Fs.begin(), E2_incident_Fs.end());

  return E1_F_set == E2_F_set;
}

//: Remove a halfedge (Disconnect a face and an edge and delete the halfedge).
bool _disconnect_remove_HE (dbmsh3d_halfedge* HE)
{  
  bool result;
  dbmsh3d_face* F = HE->face();
  dbmsh3d_sheet* S = dynamic_cast<dbmsh3d_sheet*>(F);
  if (S != NULL) { //Disconnect HE from a sheet.
    result = S->_disconnect_HE (HE);
    assert (result);
  }
  else { //Disconnect HE from a face.
    result = F->_disconnect_bnd_HE (HE);
    assert (result);
  }

  //Disconnect HE from E.
  result = HE->edge()->_disconnect_HE (HE);
  assert (result);

  delete HE;
  return true;
}

dbmsh3d_face* get_F_from_E1_E2 (const dbmsh3d_edge* E1, const dbmsh3d_edge* E2)
{
  dbmsh3d_halfedge* HE = E1->halfedge();
  do {
    dbmsh3d_face* F = HE->face();
    if (F->is_bnd_E (E2))
      return F;
    HE = HE->pair();
  }
  while (HE != E1->halfedge() && HE != NULL);
  return NULL;
}

bool get_digi_curve_E_chain (const vcl_vector<dbmsh3d_edge*>& E_chain, 
                             vcl_vector<vgl_point_3d<double> >& curve)
{
  if (E_chain.size() == 0)
    return false;
  if (E_chain.size() == 1) {
    if (E_chain[0]->is_self_loop() == false) {
      curve.push_back (E_chain[0]->sV()->pt());
      curve.push_back (E_chain[0]->eV()->pt());
      return true;
    }
    else
      return false;
  }
  if (E_chain.size() == 2) {
    bool loop2;
    Es_sharing_V_check (E_chain[0], E_chain[1], loop2);
    if (loop2)
      return false;
  }

  //Now extract E_chain's ordered vertices.
  //Detect the first vertex v0 and the second vertex v1.
  dbmsh3d_vertex* V1 = Es_sharing_V (E_chain[0], E_chain[1]);
  assert (V1);
  dbmsh3d_vertex* V0 = E_chain[0]->other_V (V1);

  curve.push_back (V0->pt());
  curve.push_back (V1->pt());

  dbmsh3d_vertex* V = V1;
  for (unsigned int i=1; i<E_chain.size(); i++) {
    V = E_chain[i]->other_V (V);
    curve.push_back (V->pt());
  }

  return true;
}

void update_digi_curve_E_chain (const vcl_vector<dbmsh3d_edge*>& E_chain, 
                                const vcl_vector<vgl_point_3d<double> >& curve)
{
  assert (E_chain.size() + 1 == curve.size());
  if (E_chain.size() == 1) {
    assert (E_chain[0]->is_self_loop() == false);
    E_chain[0]->sV()->set_pt (curve[0]);
    E_chain[1]->sV()->set_pt (curve[1]);
  }
  if (E_chain.size() == 2) {
    bool loop2;
    Es_sharing_V_check (E_chain[0], E_chain[1], loop2);
    assert (loop2 == false);
  }

  //Now update E_chain by the ordered curve pts.
  //Detect the first vertex v0 and the second vertex v1.
  dbmsh3d_vertex* V1 = Es_sharing_V (E_chain[0], E_chain[1]);
  assert (V1);
  dbmsh3d_vertex* V0 = E_chain[0]->other_V (V1);

  V0->set_pt (curve[0]);
  V1->set_pt (curve[1]);

  dbmsh3d_vertex* V = V1;
  for (unsigned int i=1; i<E_chain.size(); i++) {
    V = E_chain[i]->other_V (V);
    V->set_pt (curve[i+1]);
  }
}

