//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_vertex.cxx
//:
// \file
// \brief Basic 3d point sample
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
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>
#include <dbmsh3d/dbmsh3d_face.h>

unsigned int dbmsh3d_vertex::n_E_incidence () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->vertices(0) == this)
      count++;
    if (E->vertices(1) == this)
      count++;
  }
  return count;
}

//: return false if any incident_E is found not in E_set.
bool dbmsh3d_vertex::all_incident_Es_in_set (vcl_set<dbmsh3d_edge*>& E_set) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E_set.find (E) == E_set.end())
      return false;
  }
  return true;
}

//: function to return all incident faces of this vertex
int dbmsh3d_vertex::get_incident_Fs (vcl_set<dbmsh3d_face*>& face_set) const
{
  //: loop through all incident edges and put all faces into the set.
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    //loop through all incident halfedges of this edge and get the associated faces
    if (E->halfedge() == NULL) {
    }
    else if (E->halfedge()->pair() == NULL) {
      face_set.insert (E->halfedge()->face());
    }    
    else { //the last case, the associated halfedges form a circular list
      dbmsh3d_halfedge* HE = E->halfedge();
      do {
        face_set.insert (HE->face());
        HE = HE->pair();
      }
      while (HE != E->halfedge());
    }
  }
  return face_set.size();
}

//: Detect the typology of a mesh vertex.
//  VTOPO_ISOLATED: no incident edge.
//  VTOPO_EDGE_ONLY: any edge has no incident halfedge.
//  VTOPO_EDGE_JUNCTION: any incident edge has more than 2 faces.
//  VTOPO_2_MANIFOLD: one _check_2_manifold() loop to finish all traversing, start_e != end_e.
//  VTOPO_2_MANIFOLD_1RING: one _check_2_manifold() loop to finish all traversing, start_e == end_e.
//  VTOPO_NON_MANIFOLD: needs more than one _check_2_manifold() loop, none of them has start_e == end_e.
//  VTOPO_NON_MANIFOLD_1RING: needs more than one _check_2_manifold() loop, at least one of them has start_e == end_e.
//
VTOPO_TYPE dbmsh3d_vertex::detect_vtopo_type () const
{  
  //1) If there is no incident edge, return VTOPO_ISOLATED.
  if (have_incident_Es() == false)
    return VTOPO_ISOLATED;

  //2) Reset all incident edges to be unvisited.
  unsigned int countE = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    E->set_i_visited (0);
    countE++;
  }

  //3) Initialize firstE.
  const dbmsh3d_halfedge* firstHE = get_1st_bnd_HE();
  dbmsh3d_edge* firstE;
  if (firstHE)
    firstE = firstHE->edge();
  else
    firstE = get_1st_incident_E();

  //3) Do the first _check_2_manifold() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = _check_2_manifold (firstE, cond);

  if (cond == VTOPO_EDGE_ONLY)
    return VTOPO_EDGE_ONLY;
  else if (cond == VTOPO_EDGE_JUNCTION)
    return VTOPO_EDGE_JUNCTION;
  else if (nE == countE) {  
    if (cond == VTOPO_2_MANIFOLD_1RING)
      return VTOPO_2_MANIFOLD_1RING;
    else
      return VTOPO_2_MANIFOLD;
  }

  bool b_one_ring = false; //A flag to remember the first result.
  if (cond == VTOPO_2_MANIFOLD_1RING)    
    b_one_ring = true;

  //4) Remaining: VTOPO_NON_MANIFOLD and VTOPO_NON_MANIFOLD_1RING
  //   Run _check_2_manifold() on all remaining unvisited edges.
  dbmsh3d_edge* nextE = _find_unvisited_E ();
  while (nextE != NULL) {
    _check_2_manifold (nextE, cond);

    if (cond == VTOPO_EDGE_ONLY)
      return VTOPO_EDGE_ONLY;
    else if (cond == VTOPO_EDGE_JUNCTION)
      return VTOPO_EDGE_JUNCTION;
    else if (cond == VTOPO_2_MANIFOLD_1RING)
      return VTOPO_NON_MANIFOLD_1RING;

    nextE = _find_unvisited_E ();
  }

  if (b_one_ring)
    return VTOPO_NON_MANIFOLD_1RING;
  else
    return VTOPO_NON_MANIFOLD;
}

//: Run the 2-manifold checking loop.
//  return VTOPO_EDGE_ONLY if any encounter edge has no incident halfedge.
//  return VTOPO_EDGE_JUNCTION if any encounter edge has more than 2 incident halfedges.
//  return VTOPO_2_MANIFOLD_1RING if E goes back to startE.
//  otherwise return BOGUS_VTOPO_TYPE
//
unsigned int dbmsh3d_vertex::_check_2_manifold (const dbmsh3d_edge* startE, 
                                                VTOPO_TYPE& cond) const
{
  unsigned int nE = 0;
  dbmsh3d_edge* E = (dbmsh3d_edge*) startE;
  dbmsh3d_halfedge* startHE = E->halfedge();
  if (startHE == NULL) {
    cond = VTOPO_EDGE_ONLY;
    return nE;
  }

  dbmsh3d_halfedge* HE = startHE;
  do { 
    HE->edge()->set_i_visited (1); //During the checking, mark visited edges.
    nE++;
    if (HE->pair() && HE->pair()->pair() != HE) {
      cond = VTOPO_EDGE_JUNCTION;
      return nE;
    }

    dbmsh3d_halfedge* otherHE = HE->face()->find_next_bnd_HE (this, HE);
    dbmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL) { //hit the boundary
      cond = BOGUS_VTOPO_TYPE;
      return nE+1;
    }

    HE = nextHE;
  }
  while (HE->edge() != startHE->edge());

  //hit back to startE again.
  cond = VTOPO_2_MANIFOLD_1RING;
  return nE;
}

dbmsh3d_edge* dbmsh3d_vertex::_find_unvisited_E () const 
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->b_visited() == false)
      return E;
  }
  return NULL;
}

//###############################################################
//####### hypergraph/mesh vertex type handling functions #######
//###############################################################

const V_TYPE dbmsh3d_vertex::get_v_type ()
{
  if (c_value_ != V_TYPE_BOGUS)
    return c_value_; //A cache mechanism
  else
    return compute_v_type ();
}

const V_TYPE dbmsh3d_vertex::compute_v_type ()
{
  int nManifold, nRib, nAxial, nDege;
  n_incE_types (nManifold, nRib, nAxial, nDege);

  if (nDege==0) {
    //A12: if connected to no RIB or AXIAL
    if (nManifold!=0 && nRib==0 && nAxial==0)
      c_value_ = V_TYPE_MANIFOLD;
    //A1A3: if contains 2 connected E of RIB and AXIAL type
    else if (nAxial==1 && nRib==1) 
      c_value_ = V_TYPE_RIB_END;
    //A14: if contains 2+ connected E of AXIAL type
    else if (nAxial==4) 
      c_value_ = V_TYPE_AXIAL_END;
    //A3: if contains 2 connected E of RIB type
    else if (nRib==2) 
      c_value_ = V_TYPE_RIB;
    //A13: if contains 2 connected E of AXIAL type
    else if (nAxial==2) 
      c_value_ = V_TYPE_AXIAL;
    //The degenerate node
    else {
      if (nRib!=0)
        c_value_ = V_TYPE_DEGE_RIB_END;
      else
        c_value_ = V_TYPE_DEGE_AXIAL_END;
    }
  }  
  else { //The degenerate node    
    if (nRib!=0)
      c_value_ = V_TYPE_DEGE_RIB_END;
    else
      c_value_ = V_TYPE_DEGE_AXIAL_END;
  }

  return c_value_; 
}

void dbmsh3d_vertex::n_incE_types (int& nManifold, int& nRib, int& nAxial, int& nDege) const
{
  nManifold = 0;
  nRib = 0;
  nAxial = 0;
  nDege = 0;
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->e_type()==E_TYPE_MANIFOLD)
      nManifold++;
    else if (E->e_type()==E_TYPE_RIB)
      nRib++;
    else if (E->e_type()==E_TYPE_AXIAL)
      nAxial++;
    else if (E->e_type()==E_TYPE_DEGE_AXIAL)
      nDege++;
    else {
      vul_printf (vcl_cout, "Warning: E %d type %c, ", id_, E->e_type());
      ///assert (0);
    }
  }
}

void dbmsh3d_vertex::get_FEs_types (vcl_set<dbmsh3d_edge*>& manifold_E_set,
                                    vcl_set<dbmsh3d_edge*>& rib_E_set,
                                    vcl_set<dbmsh3d_edge*>& axial_E_set,                                      
                                    vcl_set<dbmsh3d_edge*>& dege_E_set) const
{
  manifold_E_set.clear();
  rib_E_set.clear();
  axial_E_set.clear();
  dege_E_set.clear();

  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->e_type()==E_TYPE_MANIFOLD)
      manifold_E_set.insert (E);
    else if (E->e_type()==E_TYPE_RIB)
      rib_E_set.insert (E);
    else if (E->e_type()==E_TYPE_AXIAL)
      axial_E_set.insert (E);
    else if (E->e_type()==E_TYPE_DEGE_AXIAL)
      dege_E_set.insert (E);
    else {
      vul_printf (vcl_cout, "Error FV %d get_FEs_types()!\n", id_);
      assert (0);
    }
  }
}

dbmsh3d_edge* dbmsh3d_vertex::other_rib_E (const dbmsh3d_edge* inputE) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->e_type()==E_TYPE_RIB)
      if (E != inputE)
        return E;
  }
  return NULL;
}

dbmsh3d_edge* dbmsh3d_vertex::other_rib_E_conn (const dbmsh3d_edge* inputE) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    int n = E->count_valid_Fs();
    if (n == 1)
      if (E != inputE)
        return E;
  }
  return NULL;
}

dbmsh3d_edge* dbmsh3d_vertex::other_axial_E (const dbmsh3d_edge* inputE) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->e_type()==E_TYPE_AXIAL)
      if (E != inputE)
        return E;
  }
  return NULL;
}

dbmsh3d_edge* dbmsh3d_vertex::other_dege_E (const dbmsh3d_edge* inputE) const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->e_type()==E_TYPE_DEGE_AXIAL)
      if (E != inputE)
        return E;
  }
  return NULL;
}

//###############################################################
//###### Other functions ######
//###############################################################

bool dbmsh3d_vertex::check_integrity () const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->is_V_incident (this) == false) {
      assert (0);
      return false;
    }
  }
  return true;
}

dbmsh3d_vertex* dbmsh3d_vertex::clone () const
{
  dbmsh3d_vertex* V2 = new dbmsh3d_vertex (pt_.x(), pt_.y(), pt_.z(), id_);
  return V2;
}

void dbmsh3d_vertex::getInfo (vcl_ostringstream& ostrm) const
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbmsh3d_vertex id: %d (%.12f, %.12f, %.12f)\n", id_,
               pt_.x(), pt_.y(), pt_.z()); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n", result ? "pass." : "fail!"); ostrm<<s;
  
  vcl_sprintf (s, "  topology type: "); ostrm<<s;
  VTOPO_TYPE type = detect_vtopo_type ();
  switch (type) {
  case BOGUS_VTOPO_TYPE:
    vcl_sprintf (s, "BOGUS_VTOPO_TYPE"); ostrm<<s;
  break;
  case VTOPO_ISOLATED:
    vcl_sprintf (s, "VTOPO_ISOLATED"); ostrm<<s;
  break;
  case VTOPO_EDGE_ONLY:
    vcl_sprintf (s, "VTOPO_EDGE_ONLY"); ostrm<<s;
  break;
  case VTOPO_EDGE_JUNCTION:
    vcl_sprintf (s, "VTOPO_EDGE_JUNCTION"); ostrm<<s;
  break;
  case VTOPO_2_MANIFOLD:
    vcl_sprintf (s, "VTOPO_2_MANIFOLD"); ostrm<<s;
  break;
  case VTOPO_2_MANIFOLD_1RING:
    vcl_sprintf (s, "VTOPO_2_MANIFOLD_1RING"); ostrm<<s;
  break;
  case VTOPO_NON_MANIFOLD:
    vcl_sprintf (s, "VTOPO_NON_MANIFOLD"); ostrm<<s;
  break;
  case VTOPO_NON_MANIFOLD_1RING:
    vcl_sprintf (s, "VTOPO_NON_MANIFOLD_1RING"); ostrm<<s;
  break;
  }

  
  vcl_sprintf (s, "\n %u edge incidence.", n_E_incidence()); ostrm<<s;

  //: the incident edges
  vcl_set<void*> incident_Es;
  get_incident_Es (incident_Es);
  vcl_sprintf (s, "\n %u incident edges (unordered): ", incident_Es.size()); ostrm<<s;
  vcl_set<void*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) (*it);
    vcl_sprintf (s, "%d ", E->id()); ostrm<<s;
  }

  //: the ordered incident faces (for 2-manifold mesh)
  vcl_vector<dbmsh3d_halfedge*> ordered_halfedges;
  m2_get_ordered_HEs (ordered_halfedges);  

  vcl_sprintf (s, "\n (2-manifold) %d ordered incident faces: ", ordered_halfedges.size()); ostrm<<s;
  for (unsigned int i=0; i<ordered_halfedges.size(); i++) {
    dbmsh3d_halfedge* HE = ordered_halfedges[i];
    vcl_sprintf (s, "%d ", ((dbmsh3d_face*)HE->face())->id()); ostrm<<s;
  }
  
  vcl_sprintf (s, "\n (2-manifold) %d ordered incident edges: ", ordered_halfedges.size()); ostrm<<s;
  for (unsigned int i=0; i<ordered_halfedges.size(); i++) {
    dbmsh3d_halfedge* HE = ordered_halfedges[i];
    vcl_sprintf (s, "%d ", HE->edge()->id()); ostrm<<s;
  }
  
  vcl_sprintf (s, "\n"); ostrm<<s;
}

dbmsh3d_edge* dbmsh3d_vertex::get_valid_incident_E() const 
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->halfedge())
      return E;
  }
  return NULL;
}

dbmsh3d_halfedge* dbmsh3d_vertex::get_1st_bnd_HE () const
{
  for (dbmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->halfedge())
      if (E->halfedge()->pair() == NULL)
        return E->halfedge();
  }
  return NULL;
}

//: for 2-manifold mesh, return all incident halfedges (without duplicate pairs) in order
//  return the last halfedge
dbmsh3d_halfedge* dbmsh3d_vertex::m2_get_ordered_HEs (vcl_vector<dbmsh3d_halfedge*>& ordered_halfedges) const
{
  dbmsh3d_halfedge* startHE = get_1st_bnd_HE();
  
  if (startHE == NULL) { //if no boundary halfedge, just get any halfedge.
    dbmsh3d_edge* E = (dbmsh3d_edge*) get_1st_incident_E();
    if (E == NULL)
      return NULL;
    startHE = E->halfedge();
  }

  dbmsh3d_halfedge* HE = startHE;
  do {
    ordered_halfedges.push_back (HE);
    dbmsh3d_halfedge* otherHE = HE->face()->find_next_bnd_HE (this, HE);
    dbmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL)
      return otherHE; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != startHE->edge());

  return NULL;
}

dbmsh3d_halfedge* dbmsh3d_vertex::m2_get_next_bnd_HE (const dbmsh3d_halfedge* inputHE) const
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) inputHE;
  do {
    dbmsh3d_halfedge* otherHE = HE->face()->find_next_bnd_HE (this, HE);
    dbmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL)      
      return otherHE; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != inputHE->edge());

  return NULL;
}

//: for 2-manifold mesh, check if this vertex is a boundary vertex
//  or an internal vertex of the surface (mesh)
//  start tracing from inputHE to see if the loop back to inputHE
bool dbmsh3d_vertex::m2_is_on_bnd (dbmsh3d_halfedge* inputHE) const
{
  dbmsh3d_halfedge* HE = inputHE;
  do {
    dbmsh3d_halfedge* otherHE = HE->face()->find_next_bnd_HE (this, HE);
    dbmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL) 
      return true; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != inputHE->edge());  
  return false; //back to inputHE again.
}

//: return the sum_theta at this vertex
double dbmsh3d_vertex::m2_sum_theta () const
{
  vcl_vector<dbmsh3d_halfedge*> ordered_halfedges;
  dbmsh3d_halfedge* last_he = m2_get_ordered_HEs (ordered_halfedges);
  double sum_theta = 0;

  //loop through each adjacent face face_j.
  for (unsigned int j=0; j<ordered_halfedges.size(); j++) {
    dbmsh3d_halfedge* he_d = ordered_halfedges[j];
    double theta = he_d->face()->angle_at_bnd_V (this);
    sum_theta += theta;
  }

  return sum_theta;
}

dbmsh3d_edge* E_sharing_2V (const dbmsh3d_vertex* V1, 
                            const dbmsh3d_vertex* V2)
{
  for (dbmsh3d_ptr_node* cur = V1->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->is_V_incident (V2))
      return E;
  }
  return NULL;
}

dbmsh3d_face* find_F_sharing_Vs (vcl_vector<dbmsh3d_vertex*>& vertices)
{
  dbmsh3d_vertex* G = vertices[0];
  vcl_set<dbmsh3d_face*> incident_faces;
  G->get_incident_Fs (incident_faces);

  vcl_set<dbmsh3d_face*>::iterator it = incident_faces.begin();
  for (unsigned int i=0; i<incident_faces.size(); i++) {
    dbmsh3d_face* F = (*it);
    if (F->all_bnd_Vs_incident (vertices))
      return F;
  }
  return NULL;
}

dbmsh3d_face* get_non_manifold_1ring_extra_Fs (dbmsh3d_vertex* V)
{
  assert (V->have_incident_Es());

  //Reset all incident edges to be unvisited.
  const dbmsh3d_edge* firstE = V->get_1st_incident_E ();
  for (dbmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->b_visited() == false)
      E->set_i_visited (0);
  }

  //Do the first _check_2_manifold() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = V->_check_2_manifold (firstE, cond);

  assert (cond != VTOPO_EDGE_ONLY);
  assert (cond != VTOPO_EDGE_JUNCTION);
  assert (nE != V->n_incident_Es());
  if (cond != VTOPO_2_MANIFOLD_1RING)
    return firstE->halfedge()->face();

  //Remaining: VTOPO_NON_MANIFOLD and VTOPO_NON_MANIFOLD_1RING
  dbmsh3d_edge* nextE = V->_find_unvisited_E ();
  return nextE->halfedge()->face();
}

//: return true if the face of e1-V-e2 is a vertex-only-incidence at V.
//  a simple check here is to check if 
//    - there is at least one incident edge or face of V
//    - no V's incident edge is on vertices (V, V1) or (V, V2).
//
bool is_F_V_incidence (dbmsh3d_vertex* V, const dbmsh3d_vertex* V1, const dbmsh3d_vertex* V2)
{
  assert (V != V1);
  assert (V != V2);
  if (V->have_incident_Es() == false)
    return false; //if V has no incident edges or faces, no problem.

  //Go through V's incident edges and check if any one is (V, V1) or (V, V2).
  for (dbmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E->both_Vs_incident (V, V1) || E->both_Vs_incident (V, V2))
      return false;
  }
  return true;
}

dbmsh3d_edge* V_find_other_E (const dbmsh3d_vertex* V, const dbmsh3d_edge* inputE)
{
  for (dbmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    if (E == inputE)
      continue;
    assert (E);
    return E;
  }
  return NULL;
}

int n_E_V_incidence (vcl_set<void*>& incident_Es, const dbmsh3d_vertex* V)
{
  int count = 0;
  vcl_set<void*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) (*it);
    if (E->vertices(0) == V)
      count++;
    if (E->vertices(1) == V)
      count++;
  }
  return count;
}

