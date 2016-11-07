//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_face.cxx
//:
// \file
// \brief 3D mesh face.
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

#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vgl/vgl_point_2d.h>

#include <vnl/vnl_math.h>
#include <dbgl/dbgl_triangle.h>
#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_sheet.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

//###############################################################
//###### Connectivity Query Functions ######  
//###############################################################

void dbmsh3d_face::get_bnd_HEs (vcl_vector<dbmsh3d_halfedge*>& bnd_HEs) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  if (HE->next() == NULL) {
    bnd_HEs.push_back (HE);
    return;
  }

  do {
    assert (HE != NULL);
    bnd_HEs.push_back (HE);
    HE = HE->next();
  }
  while (HE != halfedge_);
}

void dbmsh3d_face::get_bnd_Es (vcl_vector<dbmsh3d_edge*>& bnd_Es) const
{
  dbmsh3d_halfedge* HE = halfedge_;  
  if (HE->next() == NULL) {
    bnd_Es.push_back (HE->edge());
    return;
  }

  do {
    bnd_Es.push_back (HE->edge());
    HE = HE->next();
  }
  while (HE != halfedge_);
}

//: Assume the face has only one bnd_E_chains and extract the edges in order.
bool dbmsh3d_face::get_bnd_E_chains (vcl_vector<vcl_vector<dbmsh3d_edge*> >& bnd_E_chains) const
{
  if (halfedge_->next() == NULL)
    return false;

  //Find the startHE to point to non-bordering edge.
  dbmsh3d_halfedge* startHE = halfedge_;
  while (startHE->pair() == NULL)
    startHE = startHE->next();

  //Then extract the bordering edges to the chain.
  vcl_vector<dbmsh3d_edge*> E_chain;
  
  dbmsh3d_halfedge* HE = startHE;
  do {
    if (HE->pair() == NULL)
      E_chain.push_back (HE->edge());

    //check if end of this chain and need to create next.
    if (HE->pair() && E_chain.size() != 0) {
      bnd_E_chains.push_back (E_chain);
      E_chain.clear();
    }
    HE = HE->next();
  }
  while (HE != startHE);

  if (E_chain.size() != 0)
    bnd_E_chains.push_back (E_chain);
  assert (bnd_E_chains.size());
  return true;
}

void dbmsh3d_face::get_bnd_Es (vcl_set<dbmsh3d_edge*>& bnd_Es) const
{
  get_chain_Es (halfedge_, bnd_Es);
}

const unsigned int dbmsh3d_face::n_bnd_Es () const
{
  unsigned int count = 0;
  assert (halfedge_);
  dbmsh3d_halfedge* HE = halfedge_;  
  if (HE->next() == NULL) {
    return 1;
  }

  do {
    count++;
    HE = HE->next();
  }
  while (HE != halfedge_);
  return count;
}

bool dbmsh3d_face::is_bnd_E (const dbmsh3d_edge* E) const 
{
  dbmsh3d_halfedge* HE = find_E_in_next_chain (halfedge_, E);
  if (HE)
    return true;
  else
    return false;
}

dbmsh3d_halfedge* dbmsh3d_face::find_1st_bnd_HE () const 
{
  dbmsh3d_halfedge* HE = halfedge_;
  if (HE->next() == NULL) {
    if (HE->pair() == NULL)
      return HE;
    else
      return NULL;
  }
  do { //Traverse through the circular list of halfedges.
    if (HE->pair() == NULL)
      return HE;
    HE = HE->next();
  }
  while (HE != halfedge_);
  return NULL;
}

dbmsh3d_edge* dbmsh3d_face::get_prevE (const dbmsh3d_halfedge* inputHE) const
{
  dbmsh3d_halfedge* HE = _find_prev_in_next_chain (inputHE);
  if (HE == NULL)
    return NULL;
  else
    return HE->edge();
}

dbmsh3d_edge* dbmsh3d_face::get_nextE (const dbmsh3d_halfedge* inputHE) const
{
  dbmsh3d_halfedge* HE = inputHE->next();
  if (HE == NULL)
    return NULL;
  else
    return HE->edge();
}

//: Given a vertex V and an edge of this face incident to V,
//  find the other edge of this face incident to V.
dbmsh3d_edge* dbmsh3d_face::find_next_bnd_E (const dbmsh3d_vertex* inputV, 
                                             const dbmsh3d_edge* inputE) const 
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->next() == NULL)
    return NULL;

  dbmsh3d_edge* E = _find_next_E_chain (halfedge_, inputV, inputE);
  assert (E);
  return E;
}

//: Given a vertex V and a halfedge of this face incident to V,
//  find the other halfedge of this face incident of V.
dbmsh3d_halfedge* dbmsh3d_face::find_next_bnd_HE (const dbmsh3d_vertex* inputV,
                                                  const dbmsh3d_halfedge* inputHE) const 
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->next() == NULL)
    return NULL;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* E = HE->edge();
    if (HE != inputHE && E->is_V_incident(inputV))
      return HE;    
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return NULL;
}

dbmsh3d_halfedge* dbmsh3d_face::find_bnd_HE (const dbmsh3d_edge* inputE) const
{
  if (halfedge_ == NULL)
    return NULL;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* E = HE->edge();
    if (E == inputE)
      return HE;    
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

//: get the two edges incident at this vertex and compute their angle
double dbmsh3d_face::angle_at_bnd_V (const dbmsh3d_vertex* inputV) const
{
  dbmsh3d_edge* E1 = NULL;
  dbmsh3d_edge* E2 = NULL;

  //Get the two edges of the face incident at the inputV
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* E = HE->edge();
    if (E->is_V_incident (inputV)) {
      if (E1 == NULL)
        E1 = E;
      else if (E2 == NULL) {
        E2 = E;
      }
      else
        assert (0);
    }    
    HE = HE->next();
  }
  while (HE != halfedge_);

  dbmsh3d_vertex* v1 = E1->other_V (inputV);
  dbmsh3d_vertex* v2 = E2->other_V (inputV);
  double a = vgl_distance (v1->pt(), v2->pt());
  double b = E1->length();
  double c = E2->length();
  return vcl_acos ( (b*b + c*c - a*a)/(b*c*2) );
}

//: Return true if this face is incident to all given vertices.
int dbmsh3d_face::n_bnd_Vs_in_set (vcl_set<dbmsh3d_vertex*>& vertices) const
{
  int n_inc_V = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (vertices.find (V) != vertices.end())
      n_inc_V++;
    HE = nextHE;
  }
  while (HE != halfedge_);
  return n_inc_V;
}

//: Return true if this face is incident to all given vertices.
bool dbmsh3d_face::all_bnd_Vs_incident (vcl_vector<dbmsh3d_vertex*>& vertices) const
{
  //Put all input vertices into the VSet.
  vcl_set<dbmsh3d_vertex*> VSet;
  for (unsigned int i=0; i<vertices.size(); i++)
    VSet.insert (vertices[i]);
  assert (VSet.size() == vertices.size());

  //Go through the halfedge loop and remove incident vertices from VSet.
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* E = HE->edge();
    VSet.erase (E->vertices(0));
    VSet.erase (E->vertices(1));    
    HE = HE->next();
  }
  while (HE != halfedge_);

  return VSet.empty();
}

void dbmsh3d_face::get_bnd_Vs (vcl_vector<dbmsh3d_vertex*>& vertices) const
{
  assert (vertices.size()==0);
  if (vertices_.size() != 0)
    _get_bnd_Vs_IFS (vertices);
  else
    _get_bnd_Vs_MHE (vertices);
  assert (vertices.size() > 2);
}

void dbmsh3d_face::_get_bnd_Vs_MHE (vcl_vector<dbmsh3d_vertex*>& vertices) const
{
  assert (vertices.empty());
  _get_Vs_in_chain (halfedge_, vertices);
}

void dbmsh3d_face::_get_bnd_Vs_IFS (vcl_vector<dbmsh3d_vertex*>& vertices) const
{
  for (unsigned int i=0; i<vertices_.size(); i++)
    vertices.push_back (vertices_[i]);
}

void dbmsh3d_face::get_bnd_V_ids (vcl_vector<int>& vids) const
{
  assert (vids.size()==0);
  if (vertices_.size() != 0)
    _get_bnd_V_ids_IFS (vids);
  else
    _get_bnd_V_ids_MHE (vids);
  assert (vids.size() > 2);
}

void dbmsh3d_face::_get_bnd_V_ids_MHE (vcl_vector<int>& vids) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  assert (HE->next());
  //Traverse through the circular list of halfedges,
  //and find the vertex incident with both HE and nextHE
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());    
    vids.push_back (V->id());
    HE = HE->next();
  }
  while (HE != halfedge_);
}

void dbmsh3d_face::_get_bnd_V_ids_IFS (vcl_vector<int>& vids) const
{
  for (unsigned int i=0; i<vertices_.size(); i++)
    vids.push_back (vertices_[i]->id());
}

void dbmsh3d_face::get_bnd_Vs (vcl_set<dbmsh3d_vertex*>& bnd_Vs) const
{
  get_chain_Vs (halfedge_, bnd_Vs);
}

bool dbmsh3d_face::is_bnd_V (const dbmsh3d_vertex* inputV) const 
{
  dbmsh3d_halfedge* HE = halfedge_;
  //if the next is NULL, it is a loop curve.
  if (HE->next() == NULL) {
    if (HE->edge()->sV() == inputV || HE->edge()->eV() == inputV)
      return true;
    else
      return false;
  }  
  do { //traverse through the circular list of halfedges
    if (HE->edge()->sV() == inputV || HE->edge()->eV() == inputV )
      return true;    
    HE = HE->next();
  }
  while (HE != halfedge_);
  return false;
}

//: loop through the halfedges and locate the inputV
dbmsh3d_vertex* dbmsh3d_face::next_bnd_V (const dbmsh3d_vertex* inputV) const 
{
  dbmsh3d_halfedge* HE = halfedge_;
  //: if the next is NULL, it is a loop curve.
  if (HE->next() == NULL)
    return NULL;
  do { //traverse through the circular list of halfedges,
    dbmsh3d_halfedge* nextHE = HE->next();
    //find the vertex incident with both HE and nextHE
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());    
    if (V == inputV) 
      return nextHE->edge()->other_V (V);
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return NULL;
}

//###############################################################
//###### Handle local list of incident vertices ######  
//###############################################################

//: track IFS ordered vertices use the halfedge data structure
//  store result in the vertices[] vector.
void dbmsh3d_face::_ifs_track_ordered_vertices ()
{
  vertices_.clear();

  //the starting halfedge is the face's pointing halfedge
  dbmsh3d_halfedge* HE = halfedge_;
  //if the next is NULL, it is a loop curve.
  //  this will not happen for the fullshock mesh.
  if (HE->next() == NULL) {
    vertices_.push_back (HE->edge()->eV());
    return;
  }
  //traverse through the circular list of halfedges,
  //and find the vertex incident with both HE and nextHE
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());    
    vertices_.push_back (V);
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (vertices_.size() > 2);
}

//Test if the face's IFS structure is correct (repeated or wrong Vids).
bool dbmsh3d_face::_is_ifs_valid (dbmsh3d_mesh* M)
{
  //Check if there is repeated vertices.
  vcl_set<dbmsh3d_vertex*> Vset;
  for (unsigned int i=0; i<vertices_.size(); i++) {
    dbmsh3d_vertex* V = vertices_[i];
    Vset.insert (V);
  }
  if (Vset.size() != vertices_.size())
    return false;

  //Check if each V is inside M.
  for (unsigned int i=0; i<vertices_.size(); i++) {
    const dbmsh3d_vertex* V = vertices_[i];
    if (M->contains_V (V->id()) == false)
      return false;
  }
  return true;
}

bool dbmsh3d_face::_ifs_inside_box (const vgl_box_3d<double>& box) const
{
  for (unsigned int i=0; i<vertices_.size(); i++) {
    const dbmsh3d_vertex* V = vertices_[i];
    if (!box.contains (V->pt()))
      return false;
  }
  return true;
}

bool dbmsh3d_face::_ifs_outside_box (const vgl_box_3d<double>& box) const
{
  for (unsigned int i=0; i<vertices_.size(); i++) {
    const dbmsh3d_vertex* V = vertices_[i];
    if (box.contains (V->pt()))
      return false;
  }
  return true;
}

//###############################################################
//###### Geometry Query Functions ######
//###############################################################

//: if any vertex out of the box return false, otherwise return true.
//  Use the halfedge_ to traverse the face.
//
bool dbmsh3d_face::is_inside_box (const vgl_box_3d<double>& box) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (!box.contains (V->pt()))
        return false;
    HE = nextHE;
  }  
  while (HE != halfedge_);
  return true;
}

//: if any vertex inside box return false, otherwise return true.
//  Use the halfedge_ to traverse the face.
//
bool dbmsh3d_face::is_outside_box (const vgl_box_3d<double>& box) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (box.contains (V->pt()))
        return false;
    HE = nextHE;
  }  
  while (HE != halfedge_);
  return true;
}

vgl_point_3d<double> dbmsh3d_face::compute_center_pt () const
{
  vcl_vector<dbmsh3d_vertex*> vertices;
  get_bnd_Vs (vertices);
  return compute_cen (vertices);
}

vgl_point_3d<double> dbmsh3d_face::compute_center_pt (const vcl_vector<dbmsh3d_vertex*>& vertices) const
{
  return compute_cen (vertices);
}


//: Compute face normal using the order of halfedges
//: This function works for both convex and non-convex faces.
vgl_vector_3d<double> dbmsh3d_face::compute_normal()
{
  vcl_vector<dbmsh3d_edge*> inc_edges;
  this->get_bnd_Es (inc_edges);
  vgl_point_3d<double> centroid = this->compute_center_pt();
  vgl_vector_3d<double> normal(0.0, 0.0, 0.0);

  for (unsigned i=0; i<inc_edges.size(); i++) {
    dbmsh3d_edge* E = inc_edges[i];
    dbmsh3d_halfedge* HE = E->get_HE_of_F (this);
    dbmsh3d_vertex* sV = (dbmsh3d_vertex*)HE->s_vertex();
    dbmsh3d_vertex* eV = (dbmsh3d_vertex*)HE->e_vertex();
    vgl_point_3d<double> p0 = sV->pt();
    vgl_point_3d<double> p1 = eV->pt();
    vgl_vector_3d<double> v0 = p0 - centroid;
    vgl_vector_3d<double> v1 = p1 - centroid;      
    normal += cross_product(v0,v1);
  }
  return normal;
}

//###############################################################
//###### Connectivity Modification Functions ######
//###############################################################

bool dbmsh3d_face::point_halfedge_to_E (const dbmsh3d_edge* E)
{
  if (halfedge_->edge() == E)
    return true;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->edge() == E) {
      halfedge_ = HE;
      return true;
    }
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

//: Connect a halfedge to this mesh face.
//  Note that the link list is circular, but not necessarily geometrically ordered!
//  Also be careful in the empty and starting cases.
void dbmsh3d_face::_connect_bnd_HE_end (dbmsh3d_halfedge* inputHE) 
{  
  assert (inputHE != NULL);
  if (halfedge_ == NULL) { //1)
    halfedge_ = inputHE;
    return;
  }  
  else if (halfedge_->next() == NULL) { //2) Only one halfedge there
    halfedge_->set_next (inputHE);
    inputHE->set_next (halfedge_);
    return;
  }  
  else { //3) The general circular list case, add to the end!
    dbmsh3d_halfedge* HE = halfedge_;
    while (HE->next() != halfedge_)
      HE = HE->next();
    HE->set_next (inputHE);
    inputHE->set_next (halfedge_);
    return;
  }
}

//: Disconnect the input halfedge from the face's halfedge list 
//  (memory of inputHE not released)
//  return true if success.
bool dbmsh3d_face::_disconnect_bnd_HE (dbmsh3d_halfedge* inputHE) 
{
  if (halfedge_ == NULL) //1)
    return false;
  else if (halfedge_->next() == NULL) { //2)
    if (halfedge_ == inputHE) {
      halfedge_ = NULL;
      return true;
    }
    else
      return false;
  }
  else { //3)
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      dbmsh3d_halfedge* nextHE = HE->next();
      if (nextHE == inputHE) {         
        //found. remove and fix pointers.
        if (nextHE->next() == HE)
          HE->set_next (NULL);
        else
          HE->set_next (nextHE->next()); 
        if (nextHE == halfedge_)
          halfedge_ = HE; //fix the case if inputHE is halfedge_
        return true;
      }
      HE = HE->next();
    }
    while (HE != halfedge_);
    return false;
  }
}

//: Sort the incident halfedges to form a circular list
bool dbmsh3d_face::_sort_bnd_HEs_chain ()
{
  if (halfedge_ == NULL)
    return false;

  //put all halfedges into a vector
  vcl_vector<dbmsh3d_halfedge*> bnd_HEs;
  get_bnd_HEs (bnd_HEs);
    
  if (bnd_HEs.size() == 1)
    return false; //Skip face with only one edge.
  if (bnd_HEs.size() == 2)
    return false; //Skip face with only two edges.

  //Now reset the halfedges in a correct order.
  //The current halfedge_ is still the starting halfedge.
  dbmsh3d_halfedge* HE = halfedge_;
  dbmsh3d_vertex* eV = HE->edge()->eV();
  do {
    //Find the next halfedge incident to the eV from the vector
    dbmsh3d_halfedge* nextHE;
    bool r = _find_nextHE_in_loop (HE, eV, bnd_HEs, nextHE);
    assert (r);
    HE->set_next (nextHE);

    HE = HE->next();
    eV = HE->edge()->other_V (eV);
  }
  while (HE != halfedge_);
  return true;
}

//: reverse the orientation of chain of halfedges of this face.
void dbmsh3d_face::_reverse_bnd_HEs_chain () 
{
  if (halfedge_ == NULL)
    return;
  if (halfedge_->next() == NULL)
    return;

  vcl_vector<dbmsh3d_halfedge*> HE_chain;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    HE_chain.push_back (HE);
    HE = HE->next();
  }
  while (HE != halfedge_);

  //Build the circular list of halfedges in reverse order.
  HE = halfedge_;
  for (int i=(int) HE_chain.size()-1; i>=0; i--) {
    dbmsh3d_halfedge* nextHE = HE_chain[i];
    HE->set_next (nextHE);
    HE = nextHE;
  }
  assert (HE == halfedge_);
}

void dbmsh3d_face::set_orientation (dbmsh3d_halfedge* new_start_he,
                                    dbmsh3d_vertex*   new_next_v) 
{
  if (halfedge_ == NULL)
    return;
  if (halfedge_->next() == NULL)
    return;

  //: set the new_start_he (should check)
  assert (new_start_he != NULL);
  halfedge_ = new_start_he;

  dbmsh3d_halfedge* nextHE = new_start_he->next();
  if (!nextHE->edge()->is_V_incident (new_next_v))
    _reverse_bnd_HEs_chain ();
}

bool dbmsh3d_face::connect_adj_bnd_HE (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn)
{
  return _connect_adj_HE_to_chain (HE0, HEn);
}

bool dbmsh3d_face::disconnect_bnd_E (dbmsh3d_edge* E)
{
  if (halfedge_ == NULL)
    return false;
  return _disconnect_E_chain (halfedge_, E);
}

//: disconnect all associated halfedges from their edges and delete them.
void dbmsh3d_face::disconnect_all_bnd_Es () 
{
  _delete_HE_chain (halfedge_);
}

//###############################################################
//###### Other functions ######
//###############################################################

bool dbmsh3d_face::check_integrity ()
{
  if (halfedge_ == NULL) {
    assert (0);
    return false;
  }

  dbmsh3d_halfedge* HE = halfedge_;
  dbmsh3d_halfedge* nextHE = HE->next();
  dbmsh3d_sheet* S = dynamic_cast<dbmsh3d_sheet*>(this);

  if (nextHE == NULL) {
    //A sheet can have a single boundary edge, while a face can't.
    if (S == NULL) {
      assert (0); 
      return false;
    }
  }
  else if (nextHE->next() == HE) {
    if (HE->face() != this) {
      assert (0);
      return false;
    }
    if (nextHE->face() != this) {
      assert (0);
      return false;
    }
    if (HE->edge() == nextHE->edge()) {
      assert (0); //The two edges of loop can't be the same.
      return false;
    }
  }
  else {
    //The general case of loop of > 2 edges.
    vcl_set<dbmsh3d_halfedge*> HE_set;
    do {
      nextHE = HE->next();
      if (HE->face() != this) {
        assert (0);
        return false;
      }
      if (Es_sharing_V (HE->edge(), nextHE->edge()) == false) {
        assert (0);
        return false;      
      }
      //Check that no duplicate HEs are in the chain.
      if (HE_set.find (HE) != HE_set.end()) {
        assert (0);
        return false; //Duplicate HEs found.
      }
      HE_set.insert (HE);
      HE = nextHE;
    }
    while (HE != halfedge_);    
  }

  return true;
}

dbmsh3d_face* dbmsh3d_face::clone () const
{
  dbmsh3d_face* F2 = new dbmsh3d_face (id_);
  F2->set_sid (i_value_);
  return F2;
}

void dbmsh3d_face::_clone_F_E_conn (dbmsh3d_face* F2, dbmsh3d_mesh* M2) const
{
  //deep-copy the halfedge[] connectivity to F2.
  assert (halfedge_);
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* E2 = M2->edgemap(HE->edge()->id());
    //create a halfedge to connect F2 and E2.
    F2->connect_bnd_E_end (E2);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
}

dbmsh3d_face* dbmsh3d_face::clone (dbmsh3d_mesh* M2) const
{
  dbmsh3d_face* F2 = clone ();
  _clone_F_E_conn (F2, M2);
  return F2;
}

void dbmsh3d_face::getInfo (vcl_ostringstream& ostrm) 
{
  char s[1024];
  
  dbmsh3d_halfedge* HE = halfedge_;
  dbmsh3d_edge* e0 = HE->edge();
  HE = HE->next();
  dbmsh3d_edge* e1 = HE->edge();
  HE = HE->next();
  dbmsh3d_edge* e2 = HE->edge();
  unsigned int c0 = e0->n_incident_Fs(); 
  unsigned int c1 = e1->n_incident_Fs();
  unsigned int c2 = e2->n_incident_Fs();

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbmsh3d_face id: %d, type: %s (%u-%u-%u)    \n", 
               id_, tri_get_topo_string().c_str(), c0, c1, c2); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  //: the incident edges via halfedges
  int n_sides = n_bnd_Es ();
  vcl_sprintf (s, " %d incident edges in order: ", n_sides); ostrm<<s;

  if (halfedge_ == NULL) {
    vcl_sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->next() == NULL) {
    vcl_sprintf (s, "%d ", (halfedge_)->edge()->id()); ostrm<<s;
  }
  else {
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      vcl_sprintf (s, "%d ", HE->edge()->id()); ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_);
  }

  //: the incident vertices in order
  vcl_sprintf (s, "\n %d incident vertices in order: ", n_sides); ostrm<<s;
  if (halfedge_ == NULL) {
    vcl_sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->next() == NULL) {
    dbmsh3d_halfedge* HE = halfedge_;
    assert (HE->edge()->sV() == HE->edge()->eV());
    vcl_sprintf (s, "%d ", HE->edge()->sV()->id()); ostrm<<s;
  }
  else {
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      dbmsh3d_halfedge* nextHE = HE->next();
      dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());

      vcl_sprintf (s, "%d ", V->id()); ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_);
  }
}

//###############################################################
//###### For triangular face only ######
//###############################################################

TRIFACE_TYPE dbmsh3d_face::tri_get_topo_type () const
{
  TRIFACE_TYPE type = BOGUS_TRIFACE;

  dbmsh3d_halfedge* HE = halfedge_;
  dbmsh3d_edge* e0 = HE->edge();
  HE = HE->next();
  dbmsh3d_edge* e1 = HE->edge();
  HE = HE->next();
  dbmsh3d_edge* e2 = HE->edge();

  //Non-triangular face: more than 3 edges.
  HE = HE->next();
  if (HE->edge() != e0)
    return TRIFACE_E4P;

  unsigned int c0 = e0->n_incident_Fs(); 
  unsigned int c1 = e1->n_incident_Fs();
  unsigned int c2 = e2->n_incident_Fs();

  if (c0 == 1) {
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_111;
      else if (c2 == 2)
        return TRIFACE_112;
      else { assert (c2 > 2);
        return TRIFACE_113P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_112;
      else if (c2 == 2)
        return TRIFACE_122;
      else { assert (c2 > 2);
        return TRIFACE_123P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_111;
      else if (c2 == 2)
        return TRIFACE_112;
      else { assert (c2 > 2);
        return TRIFACE_113P; }
    }
  }
  else if (c0 == 2) {
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_112;
      else if (c2 == 2)
        return TRIFACE_122;
      else { assert (c2 > 2);
        return TRIFACE_123P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_122;
      else if (c2 == 2)
        return TRIFACE_222;
      else { assert (c2 > 2);
        return TRIFACE_223P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_123P;
      else if (c2 == 2)
        return TRIFACE_223P;
      else { assert (c2 > 2);
        return TRIFACE_23P3P; }
    }
  }
  else { assert (c0 > 2);
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_113P;
      else if (c2 == 2)
        return TRIFACE_123P;
      else { assert (c2 > 2);
        return TRIFACE_13P3P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_123P;
      else if (c2 == 2)
        return TRIFACE_223P;
      else { assert (c2 > 2);
        return TRIFACE_23P3P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_13P3P;
      else if (c2 == 2)
        return TRIFACE_123P;
      else { assert (c2 > 2);
        return TRIFACE_3P3P3P; }
    }
  }

  return type;
}

vcl_string dbmsh3d_face::tri_get_topo_string () const
{
  TRIFACE_TYPE type = tri_get_topo_type();
  switch (type) {
  case BOGUS_TRIFACE:
    return "BOGUS_TRIFACE";
  case TRIFACE_111:
    return "1_1_1";
  case TRIFACE_112:
    return "1_1_2";
  case TRIFACE_113P:
    return "1_1_3+";
  case TRIFACE_122:
    return "1_2_2";
  case TRIFACE_123P:
    return "1_2_3+";
  case TRIFACE_13P3P:
    return "1_3+_3+";
  case TRIFACE_222:
    return "2_2_2";
  case TRIFACE_223P:
    return "2_2_3+";
  case TRIFACE_23P3P:
    return "2_3+_3+";
  case TRIFACE_3P3P3P:
    return "3+_3+_3+";
  case TRIFACE_E4P:
    return "edges_4+";
  default:
    assert (0);
    return "";
  }
}

//###############################################################
//###### For the face of a 2-manifold triangular mesh only ######
//###############################################################

dbmsh3d_edge* dbmsh3d_face::m2t_edge_against_vertex (dbmsh3d_vertex* inputV)
{
  //: loop through all incident edges, look for the one sharing inputV
  dbmsh3d_halfedge* HE = halfedge_;

  //: traverse through the circular list of halfedges,
  do {
    dbmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV))
      return E;

    HE = HE->next();
  }
  while (HE != halfedge_);

  return NULL;
}

dbmsh3d_halfedge* dbmsh3d_face::m2t_halfedge_against_vertex (dbmsh3d_vertex* inputV)
{
  //: loop through all incident edges, look for the one sharing inputV
  dbmsh3d_halfedge* HE = halfedge_;

  //: traverse through the circular list of halfedges,
  do {
    dbmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV))
      return HE;

    HE = HE->next();
  }
  while (HE != halfedge_);

  return NULL;
}

//: for 2-manifold mesh, given input_face, find the neighboring face sharing the inputV
//  it is possible that nothing could be found, if the inputV is not correct.
dbmsh3d_face* dbmsh3d_face::m2t_nbr_face_against_vertex (dbmsh3d_vertex* inputV)
{
  //: loop through all incident edges, look for the one sharing inputV
  dbmsh3d_halfedge* HE = halfedge_;

  //: traverse through the circular list of halfedges,
  do {
    dbmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV)) {
      //: found it, return the other face
      //  traverse through edge's list of halfedges.
      //  Only work for 2-manifold!!
      if (E->halfedge()->face() != this) 
        return E->halfedge()->face();
      else if (E->halfedge()->pair()) {
        dbmsh3d_halfedge* HE = E->halfedge()->pair();
        assert (HE->face() != this);
        return HE->face();
      }
      else {
        return NULL;
      }
    }
    HE = HE->next();
  }
  while (HE != halfedge_);

  return NULL;
}

//: (redundent) for 2-manifold mesh, given input_face, find the neighboring face sharing incident v1 and v2
dbmsh3d_face* dbmsh3d_face::m2t_nbr_face_sharing_edge (dbmsh3d_vertex* v1, dbmsh3d_vertex* v2)
{
  //: loop through all incident edges, look for the one sharing inputV
  dbmsh3d_halfedge* HE = halfedge_;

  //: traverse through the circular list of halfedges,
  do {
    dbmsh3d_edge* E = HE->edge();
    if (E->is_V_incident (v1) && E->is_V_incident (v2)) {
      //: found it, return the other face
      //  traverse through edge's list of halfedges.
      //  Only work for 2-manifold!!
      if (E->halfedge()->face() != this) 
        return E->halfedge()->face();
      else if (E->halfedge()->pair()) {
        dbmsh3d_halfedge* HE = E->halfedge()->pair();
        return HE->face();
      }
      else {
        ///assert (0);
        return NULL;
      }
    }
    HE = HE->next();
  }
  while (HE != halfedge_);

  return NULL;
}

//: for triangular mesh only, given v1, v2, find v3
dbmsh3d_vertex* dbmsh3d_face::t_3rd_vertex (const dbmsh3d_vertex* V1, const dbmsh3d_vertex* V2) const
{
  //loop through all incident edges, look for the one sharing inputV
  dbmsh3d_halfedge* HE = halfedge_;

  //traverse through the circular list of halfedges,
  do {
    dbmsh3d_edge* E = HE->edge();
    //: some duplication, check both s_v and e_v.
    if (E->sV() != V1 && E->sV() != V2)
      return E->sV();
    if (E->eV() != V1 && E->eV() != V2)
      return E->eV();
    HE = HE->next();
  }
  while (HE != halfedge_);

  assert (0);
  return NULL;
}

dbmsh3d_vertex* dbmsh3d_face::t_vertex_against_edge (const dbmsh3d_edge* E) const
{
  return t_3rd_vertex (E->sV(), E->eV());
}

//##########################################################
//###### Additional Functions ######
//##########################################################

dbmsh3d_halfedge* find_E_in_next_chain (const dbmsh3d_halfedge* headHE,
                                        const dbmsh3d_edge* E)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;  
  if (HE->next() == NULL) { //If the next is NULL, it is a loop curve.
    if (HE->edge() == E)
      return HE;
    else
      return NULL;
  }  
  do { //Traverse through the circular list of halfedges
    if (HE->edge() == E)
      return HE;
    HE = HE->next();
  }
  while (HE != headHE);
  return NULL;
}

dbmsh3d_halfedge* chain_contains_E_in_set (const dbmsh3d_halfedge* headHE,
                                           vcl_set<dbmsh3d_edge*>& E_set)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;  
  if (HE->next() == NULL) { //If the next is NULL, it is a loop curve.
    if (E_set.find(HE->edge()) != E_set.end()) {
      return HE;
    }
    else
      return NULL;
  }  
  do { //Traverse through the circular list of halfedges
    if (E_set.find(HE->edge()) != E_set.end()) {
      return HE;
    }
    HE = HE->next();
  }
  while (HE != headHE);
  return NULL;
}

void get_chain_Es (const dbmsh3d_halfedge* headHE,
                   vcl_set<dbmsh3d_edge*>& incident_Es)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;  
  assert (headHE);
  if (HE->next() == NULL) {
    incident_Es.insert (HE->edge());
    return;
  }

  do {
    incident_Es.insert (HE->edge());
    HE = HE->next();
  }
  while (HE != headHE);
}

void get_chain_Vs (const dbmsh3d_halfedge* headHE,
                   vcl_set<dbmsh3d_vertex*>& incident_Vs)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  assert (headHE);

  if (HE->next() == NULL) { //1) loop of a single edge.
    assert (HE->edge()->is_self_loop());
    incident_Vs.insert (HE->edge()->sV());
  }
  else if (HE->next()->next() == HE) { //2) Two edges make a circular loop.
    incident_Vs.insert (HE->edge()->sV());
    incident_Vs.insert (HE->edge()->eV());
  }
  else { //3) Loop of 3 or more edges.
    do { //traverse through the circular list of halfedges
      dbmsh3d_halfedge* nextHE = HE->next();
      dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
      assert (V);
      incident_Vs.insert (V);
      HE = HE->next();
    }
    while (HE != headHE);
  }
}

//: disconnect all associated halfedges from their edges from the given headHE.
void _delete_HE_chain (dbmsh3d_halfedge* & headHE) 
{
  if (headHE == NULL)
    return;

  dbmsh3d_edge* E;
  dbmsh3d_halfedge* nextHE = headHE->next();
  if (nextHE) {
    do {
      dbmsh3d_halfedge* he_to_remove = nextHE;
      nextHE = nextHE->next();
      E = he_to_remove->edge();
      E->_disconnect_HE (he_to_remove);
      delete he_to_remove;
    }
    while (nextHE != headHE && nextHE != NULL);
  }

  //Delete the headHE and set to NULL.
  E = headHE->edge();
  E->_disconnect_HE (headHE);
  delete headHE;
  headHE = NULL;
}

//  Return: the set of incident edges that get disconnected.
//  Also set the headHE to be NULL after calling it.
void _delete_HE_chain (dbmsh3d_halfedge* & headHE,
                       vcl_vector<dbmsh3d_edge*>& incident_edge_list) 
{
  if (headHE == NULL)
    return;

  dbmsh3d_halfedge* nextHE = headHE->next();
  if (nextHE) {
    do {
      dbmsh3d_halfedge* he_to_remove = nextHE;
      nextHE = nextHE->next();
      dbmsh3d_edge* E = he_to_remove->edge();
      E->_disconnect_HE (he_to_remove);
      delete he_to_remove;
      incident_edge_list.push_back (E);
    }
    while (nextHE != headHE && nextHE != NULL);
  }

  //Delete the headHE and set to NULL.
  dbmsh3d_edge* E = headHE->edge();
  E->_disconnect_HE (headHE);
  delete headHE;
  headHE = NULL;
  incident_edge_list.push_back (E);
}

dbmsh3d_edge* _find_next_E_chain (const dbmsh3d_halfedge* headHE,
                                  const dbmsh3d_vertex* inputV, 
                                  const dbmsh3d_edge* inputE)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_edge* E = HE->edge();
    if (E != inputE && E->is_V_incident(inputV))
      return E;    
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return NULL;
}

bool _disconnect_E_chain (dbmsh3d_halfedge*& headHE, dbmsh3d_edge* E)
{
  dbmsh3d_halfedge* HE = headHE;
  if (HE->next() == NULL) { //case I
    if (HE->edge() == E) {
      E->_disconnect_HE (HE);
      delete HE;
      headHE = NULL;
      return true;
    }
    else
      return false;
  }
  else if (HE->next()->next() == HE) { //case II
    if (HE->edge() == E) {
      headHE = HE->next();
      E->_disconnect_HE (HE);
      headHE->set_next (NULL);
      delete HE;
      return true;
    }
    else if (HE->next()->edge() == E) {      
      E->_disconnect_HE (HE->next());
      delete HE->next();
      headHE->set_next (NULL);      
      return true;
    }
    else
      return false;
  }
  else { //case III
    if (HE->edge() == E)
      headHE = HE->next();

    unsigned int count = 0;
    HE = headHE;
    dbmsh3d_halfedge* nextHE = HE->next();
    do {
      nextHE = HE->next();
      if (nextHE->edge() == E) { 
        //remove nextHE from loop
        HE->set_next (nextHE->next());
        E->_disconnect_HE (nextHE);
        delete nextHE;
        count++;
      }
      else
        HE = HE->next();
    }
    while (nextHE != headHE);

    if (count) {
      assert (count == 1);
      return true;
    }
    else
      return false;
  }
}

//: Assume the mesh face is planar and compute a 2D planar coordinate for it.
void get_2d_coord (const vcl_vector<dbmsh3d_vertex*>& vertices, vgl_vector_3d<double>& N,
                   vgl_vector_3d<double>& AX, vgl_vector_3d<double>& AY)
{
  assert (vertices.size() > 2);
  //The first vertex A is the origin.
  dbmsh3d_vertex* A = vertices[0];

  //The second vertex B identifies the x-axis.
  dbmsh3d_vertex* B = vertices[1];
  //Make AB the unit vector in the 2D x-direction.
  AX = B->pt() - A->pt();
  AX = AX / AX.length();

  //Use The 3rd vertex C to identify normal N.
  dbmsh3d_vertex* C = vertices[2];
  vgl_vector_3d<double> AC = C->pt() - A->pt();
  N = cross_product (AX, AC);
  N = N / N.length();

  //The unit vector in the 2D y-direction.
  AY = cross_product (N, AX);
  AY = AY / AY.length();
}

//: Return ordered set of vertices in 2D (x,y) coord.
//  Assume mesh face is planar. First identify the plane normal N = AB * AC.
//  Use the first vertex A as origin (0,0),
//  the second vertex B to identify x-axis, B(d, 0).
//  the y-axis is AY = cross (N, AB).
//  
void get_2d_polygon (const vcl_vector<dbmsh3d_vertex*>& vertices,
                     vcl_vector<double>& xs, vcl_vector<double>& ys)
{
  assert (vertices.size() > 2);
  xs.resize (vertices.size());
  ys.resize (vertices.size());

  //The first vertex A is the origin.
  dbmsh3d_vertex* A = vertices[0];
  xs[0] = 0;
  ys[0] = 0;

  //The second vertex B identifies the x-axis.
  dbmsh3d_vertex* B = vertices[1];
  double dAB = vgl_distance (A->pt(), B->pt());
  vgl_vector_3d<double> AB = B->pt() - A->pt();
  //Make AB the unit vector in the 2D x-direction.
  AB = AB / dAB;
  xs[1] = dAB;
  ys[1] = 0;

  //Use The 3rd vertex C to identify normal N.
  dbmsh3d_vertex* C = vertices[2];
  vgl_vector_3d<double> AC = C->pt() - A->pt();
  vgl_vector_3d<double> N = cross_product (AB, AC);
  //Check that N is valid.
  //The unit vector in the 2D y-direction.
  vgl_vector_3d<double> AY = cross_product (N, AB);
  AY = AY / AY.length();

  //Loop through all other vertices.
  for (unsigned int i=2; i<vertices.size(); i++) {
    dbmsh3d_vertex* C = vertices[i];
    vgl_vector_3d<double> AC = C->pt() - A->pt();    
    //cx = d * cos theta = dot (AB, AC) / AB.
    xs[i] = dot_product (AC, AB);
    //cy = sqrt (AC^2 - cx^2)
    ys[i] = dot_product (AC, AY);
  }
}

//: Return the projected point in the local 2D (x,y) coord.
vgl_point_2d<double> get_2d_proj_pt (vgl_point_3d<double> P, const vgl_point_3d<double>& A,
                                     const vgl_vector_3d<double>& AX, 
                                     const vgl_vector_3d<double>& AY)
{
  double x = dot_product (P - A, AX);
  double y = dot_product (P - A, AY);
  return vgl_point_2d<double> (x, y);
}

//: determine the center point of the patch
vgl_point_3d<double> compute_cen (const vcl_vector<dbmsh3d_vertex*>& vertices)
{
  double x=0.0, y=0.0, z=0.0;
  assert (vertices.size() != 0);
  
  for (unsigned int i=0; i<vertices.size(); i++) {
    x += vertices[i]->pt().x();
    y += vertices[i]->pt().y();
    z += vertices[i]->pt().z();
  }
      
  x /= vertices.size();
  y /= vertices.size();
  z /= vertices.size();

  return vgl_point_3d<double>(x,y,z);
}

//: compute the normal of the face
vgl_vector_3d<double> compute_normal_ifs (const vcl_vector<dbmsh3d_vertex*>& vertices)
{
  vgl_vector_3d<double> normal;
  //for P[0..n], compute cross product of (P[1]-P[0])*(P[2]-P[1]) ...
  assert (vertices.size() > 2);
  for (int i=0; i< ((int)vertices.size())-2; i++) {
    const dbmsh3d_vertex* v0 = vertices[i];
    const dbmsh3d_vertex* v1 = vertices[i+1];
    const dbmsh3d_vertex* v2 = vertices[i+2];
    vgl_vector_3d<double> a = v1->pt() - v0->pt();
    vgl_vector_3d<double> b = v2->pt() - v1->pt();
    vgl_vector_3d<double> n = cross_product (a, b);
    normal += n;
  }
  return normal;
}

//: Compute face normal using the given edge and starting node.
vgl_vector_3d<double> compute_normal (const vgl_point_3d<double>& C,
                                      const dbmsh3d_edge* E, 
                                      const dbmsh3d_vertex* startV)
{
  const dbmsh3d_vertex* Es = startV;
  const dbmsh3d_vertex* Ee = E->other_V (Es);
  return cross_product (Es->pt() - C, Ee->pt() - C);
}

//: Return true if vertices represent a polygon (Vs > 3) or obtuse triangle.
bool is_tri_non_acute (const vcl_vector<dbmsh3d_vertex*>& vertices)
{
  if (vertices.size() > 3)
    return true;
  dbmsh3d_vertex* V0 = vertices[0]; 
  dbmsh3d_vertex* V1 = vertices[1]; 
  dbmsh3d_vertex* V2 = vertices[2]; 
  return dbgl_is_tri_non_acute (V0->pt(), V1->pt(), V2->pt());
}

//: Return true if F (triangle) is (1,1,3+) or (1,3+,3+) extraneous
//  For a general polygon, return true if F has only edges of 1 or 3 incidences.
//
bool is_F_extraneous (dbmsh3d_face* F)
{
  assert (F->halfedge() != NULL);
  assert (F->halfedge()->next() != NULL);  
  dbmsh3d_halfedge* HE = F->halfedge();
  do {
    dbmsh3d_edge* E = HE->edge();
    int n = E->n_incident_Fs();
    //if (n != 1 && n >= 3)
    if (n==2)
      return false;
    HE = HE->next();
  }
  while (HE != F->halfedge());
  return true;
}

dbmsh3d_face* get_F_sharing_Es (dbmsh3d_edge* E1, dbmsh3d_edge* E2)
{
  //Loop through all incident faces of E1 and find the face incident to E2.  
  if (E1->halfedge() == NULL) {
    return NULL;
  }  
  else if (E1->halfedge()->pair() == NULL) {
    dbmsh3d_face* F = E1->halfedge()->face();
    if (F->is_bnd_E (E2))
      return F;
  }
  else {    
    dbmsh3d_halfedge* HE = E1->halfedge();
    do {
      dbmsh3d_face* F = HE->face();
      if (F->is_bnd_E (E2))
        return F;
      HE = HE->pair();
    }
    while (HE != E1->halfedge());
  }
  return NULL;
}

void _get_Vs_in_chain (const dbmsh3d_halfedge* headHE, 
                       vcl_vector<dbmsh3d_vertex*>& vertices)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  if (HE->next() == NULL) {
    dbmsh3d_edge* E = HE->edge();    
    vertices.push_back (HE->edge()->sV());
    if (E->is_self_loop() == false)
      vertices.push_back (HE->edge()->eV());
    return;
  }

  //Traverse through the circular list of halfedges,
  //and find the vertex incident with both HE and nextHE
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    bool loop2;
    dbmsh3d_vertex* V = Es_sharing_V_check (HE->edge(), nextHE->edge(), loop2);
    if (loop2) { //The case of loop of two edges.
      if (vertices.empty() == false)
        if (V == vertices[vertices.size()-1])
          V = nextHE->edge()->other_V (V);
    }
    vertices.push_back (V);
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) headHE && HE != NULL);
}

bool _contain_V_in_chain (const dbmsh3d_halfedge* headHE, const dbmsh3d_vertex* inputV)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  if (HE->next() == NULL) {
    dbmsh3d_edge* E = HE->edge();    
    if (HE->edge()->sV() == inputV)
      return true;
    if (HE->edge()->eV() == inputV)
      return true;
  }

  //Traverse through the circular list of halfedges,
  //and find the vertex incident with both HE and nextHE
  do { 
    if (HE->edge()->sV() == inputV)
      return true;
    if (HE->edge()->eV() == inputV)
      return true;
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) headHE && HE != NULL);
  return false;
}

//Reset the new HE chain to point to MSn.
void _set_HE_chain_face (dbmsh3d_halfedge* startHE, dbmsh3d_face* F)
{
  dbmsh3d_halfedge* HE = startHE;
  do {
    HE->set_face (F);
    HE = HE->next();
  }
  while (HE != startHE && HE != NULL);
}

bool _connect_adj_HE_to_chain (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn)
{
  assert (HE0->next() == HEn);
  assert (HE0->face() == HEn->face());
  assert (Es_sharing_V (HE0->edge(), HEn->edge()));

  dbmsh3d_halfedge* prevHE = _find_prev_in_next_chain (HE0);
  dbmsh3d_halfedge* nextHE = HEn->next ();
  
  //1) HE0 is a single loop.
  if (nextHE == HE0) {
    //Just mount HEn next to HE0.
    assert (HE0->next() == HEn);
    assert (HEn->next() == HE0);
    return true;
  }

  //2) prevHE == nextHE
  if (prevHE == nextHE) {
    assert (HE0->next() == HEn);
    HEn->set_next (nextHE);
    return true;
  }

  //3) prevHE.E incident to HEn.E
  bool loop2;
  dbmsh3d_vertex* V;
  V = Es_sharing_V_check (prevHE->edge(), HEn->edge(), loop2);
  assert (loop2 == false);
  if (V) {
    prevHE->set_next (HEn);
    HEn->set_next (HE0);
    HE0->set_next (nextHE);
    return true;
  }
  //4) nextHE.E incident to HEn.E
  V = Es_sharing_V_check (nextHE->edge(), HEn->edge(), loop2);
  assert (loop2 == false);
  if (V) {
    assert (HE0->next() == HEn);
    HEn->set_next (nextHE);
    return true;
  }

  assert (0);
  return false;
}


//###############################################################
// Old code from Frederic
//###############################################################
#if 0
//: compute the normalized N = V1 * V2
void _cross_product (double V1x, double V1y, double V1z, 
                     double V2x, double V2y, double V2z, 
                     double& Nx, double& Ny, double& Nz) 
{
  //: Vector product : NormTri = V1 x V2 
  Nx = (V1y * V2z) - (V1z * V2y);
  Ny = (V1z * V2x) - (V1x * V2z);
  Nz = (V1x * V2y) - (V1y * V2x); 
}

#define VECTOR_LENGTH_EPSILON       1E-14

void _normalize_vector (double& Vx, double& Vy, double& Vz) 
{
  //: Normalize the normal vector
  double dLength = vcl_sqrt (Vx*Vx + Vy*Vy + Vz*Vz);
  if (dLength < VECTOR_LENGTH_EPSILON) {
    vul_printf (vcl_cout, "NUMERICAL ERROR in computing vector length.\n");
    ///assert (0);
  }
  Vx = Vx / dLength;
  Vy = Vy / dLength;
  Vz = Vz / dLength;
}

void _cross_product_normalized (double V1x, double V1y, double V1z, 
                                double V2x, double V2y, double V2z, 
                                double& Nx, double& Ny, double& Nz) {
  _cross_product (V1x, V1y, V1z, V2x, V2y, V2z, Nx, Ny, Nz);
  _normalize_vector (Nx, Ny, Nz);
}

//--------------------------------------------------------------------

// Usage: Given an ordered quadruplet (A,B,C,D) with C, the "left" gene,
//    A and B, the edge pair, and D the next one the "right", find
//    the dihedral angle between the pair of triangular faces
//    (ABC) and (BCD), N1 = BC x BA, N2 = BD x BC ,
//    that is the angle the two faces make with respect to the
//    shared edge BC.
//
//    GeneA:        on the linkElm triangle
//    edgePatchElm: GeneB, GeneC
//    GeneD:        the Gene to test
//
//: return the angle
double computeDotNormal (const dbmsh3d_vertex* GeneC, 
                         const dbsk3d_fs_face* edgePatchElm, 
                         const dbmsh3d_vertex* GeneD)
{  
  const dbmsh3d_vertex* GeneA = edgePatchElm->genes(0);
  const dbmsh3d_vertex* GeneB = edgePatchElm->genes(1);

  // --- Compute oriented normal to 1st triangle: N1 = AB x AC ---
  double ABx = GeneB->pt().x() - GeneA->pt().x();
  double ABy = GeneB->pt().y() - GeneA->pt().y();
  double ABz = GeneB->pt().z() - GeneA->pt().z();
  
  double ACx = GeneC->pt().x() - GeneA->pt().x();
  double ACy = GeneC->pt().y() - GeneA->pt().y();
  double ACz = GeneC->pt().z() - GeneA->pt().z();
  
  //: Vector product : BC x BA = N1
  double N1x, N1y, N1z;
  _cross_product_normalized (ABx, ABy, ABz, ACx, ACy, ACz, N1x, N1y, N1z);
  
  //: --- Compute oriented normal to 2nd triangle: N2 = BD x BC --- 
  double fBDx = GeneD->pt().x() - GeneA->pt().x();
  double fBDy = GeneD->pt().y() - GeneA->pt().y();
  double fBDz = GeneD->pt().z() - GeneA->pt().z();

  //: Vector product : BD x BC = N2
  double N2x, N2y, N2z;
  _cross_product_normalized (fBDx, fBDy, fBDz, ABx, ABy, ABz, N2x, N2y, N2z);

  //: scalar product of N1 and N2
  double dDotProduct = (N1x * N2x) + (N1y * N2y) + (N1z * N2z);
  
  //: dot product of unit vector can not be greater than 1
  if (dDotProduct > 1.0)
    dDotProduct = 1.0; 
  else if (dDotProduct < -1.0)
    dDotProduct = -1.0;

  return dDotProduct;
}

#endif

