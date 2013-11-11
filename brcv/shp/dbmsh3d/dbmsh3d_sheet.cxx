//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_sheet.cxx
//:
// \file
// \brief Basic 3d surface (more complex than a face)
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

#include <vcl_vector.h>
#include <vcl_queue.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_halfedge.h>
#include <dbmsh3d/dbmsh3d_node.h>
#include <dbmsh3d/dbmsh3d_curve.h>
#include <dbmsh3d/dbmsh3d_sheet.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_hypg.h>

void dbmsh3d_sheet::get_F_set (vcl_set<dbmsh3d_face*>& F_set, const bool skip_shared_F)
{
  if (skip_shared_F == false) {
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
    for (; it != facemap_.end(); it++) {
      dbmsh3d_face* F = (*it).second;
      F_set.insert (F);
    }
  }
  else {
    vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
    for (; it != facemap_.end(); it++) {
      dbmsh3d_face* F = (*it).second;
      if (is_F_shared (F) == false)
        F_set.insert (F);
    }
  }
}

void dbmsh3d_sheet::get_fine_scale_vertices (vcl_map<int, dbmsh3d_vertex*>& V_map)
{
  //Put all vertices of this sheet into a map.
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    vcl_vector<dbmsh3d_vertex*> vertices; 
    F->get_bnd_Vs (vertices);
    for (unsigned int i=0; i<vertices.size(); i++) {
      dbmsh3d_vertex* V = vertices[i];
      V_map.insert (vcl_pair<int, dbmsh3d_vertex*> (V->id(), V));
    }
  }
}

//: Return the first found F of this sheet which is incident to E.
//  Note that there can exist multiple possible F's incident to E.
dbmsh3d_face* dbmsh3d_sheet::get_1st_F_incident_E (const dbmsh3d_edge* E) 
{
  //Traverse E's incident faces and find in the facemap_[].
  dbmsh3d_halfedge* HE = E->halfedge();
  do {
    if (contain_F (HE->face()->id()))
      return HE->face();
    HE = HE->pair();
  }
  while (HE != E->halfedge() && HE != NULL);
  return NULL;
}

//: Return the first found F not in inputF_set of this sheet which is incident to E.
//  Note that there can exist multiple possible F's incident to E.
dbmsh3d_face* dbmsh3d_sheet::get_1st_F_incident_E2 (const dbmsh3d_edge* E, 
                                                    vcl_set<dbmsh3d_face*>& inputF_set) 
{
  //Traverse E's incident faces and find in the facemap_[].
  dbmsh3d_halfedge* HE = E->halfedge();
  do {
    if (inputF_set.find (HE->face()) == inputF_set.end()) {
      if (contain_F (HE->face()->id()))
        return HE->face();
    }
    HE = HE->pair();
  }
  while (HE != E->halfedge());
  return NULL;
}

//: Check if all fine-scale F's are connected.
//  Start with a non-shared_F as seed.
//  For efficiency, first check for edge-connected components,
//  If failed, check for also vertex-connected components.
bool dbmsh3d_sheet::check_non_shared_Fs_connected ()
{
  //Find the first unlabelled F.
  dbmsh3d_face* seedF = get_1st_non_shared_F ();
  if (seedF == NULL)
    return true; //not handled.
  
  bool r = non_shared_Fs_connected (seedF, false);
  if (r == false)
    r = non_shared_Fs_connected (seedF, true);

  return r;
}

dbmsh3d_face* dbmsh3d_sheet::get_1st_non_shared_F ()
{
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    if (is_F_shared (F) == false)
      return F;
  }
  return NULL;
}
//: Check if all fine-scale F's are connected.
//  Skip checking all shared_F's.
//  If v_conn specified, also consider vertex-connected neighbors.
//  Otherwise only edge-connected neighbors are traversed.
bool dbmsh3d_sheet::non_shared_Fs_connected (dbmsh3d_face* seedF, const bool v_conn)
{
  //Reset visit flag of each F.
  reset_traverse_F ();

  //Propagate labelling from seedF across E's.
  vcl_set<dbmsh3d_face*> conn_Fset;
  bool r;

  if (v_conn == false) {
    r = _prop_label_Fs_e_conn (seedF, conn_Fset);
    if (r == false) {
      assert (0);
      return false;
    }
  }
  else {
    _prop_label_Fs_e_v_conn (seedF, conn_Fset);
  }

  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (is_F_shared (F))
      continue; //skip all shared_F's.
    if (conn_Fset.find (F) == conn_Fset.end()) {
      assert (0);
      return false; //F not in connected set.
    }
  }

  return true;
}

//###################################################################

void dbmsh3d_sheet::reset_traverse_F ()
{
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    F->set_i_visited (0);
  }
}

//##########################################################
//###### Connectivity Query Functions ######
//##########################################################

//: return the number of icurve_loops (1-incidence).
unsigned int dbmsh3d_sheet::n_IC_pairs () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      count++;
  }
  return count;
}

//: return the number of icurve_loops (1-incidence).
unsigned int dbmsh3d_sheet::n_IC_loops () const
{
  unsigned int count = 0;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE) == false)
      count++;
  }
  return count;
}

dbmsh3d_ptr_node* dbmsh3d_sheet::_find_1st_icurve_loop () const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE) == false)
      return cur;
  }
  return NULL;
}


unsigned int dbmsh3d_sheet::total_HEs_icurve_chains () const 
{
  unsigned int total = 0;  
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) cur->ptr();
    total += count_HEs_in_next_loop (HE);
  }
  return total;
}


bool dbmsh3d_sheet::is_HE_in_icurves (const dbmsh3d_halfedge* inputHE) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    assert (headHE);
    if (headHE->next() == NULL) {
      if (headHE == inputHE)
        return true;
    }
    else {
      dbmsh3d_halfedge* HE = headHE;
      do {
        if (headHE == inputHE)
          return true;
        HE = HE->next();
      }
      while (HE != headHE);
    }
  }
  return false;
}

bool dbmsh3d_sheet::is_E_in_icurves (const dbmsh3d_edge* inputE) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    assert (headHE);
    if (headHE->next() == NULL) {
      if (headHE->edge() == inputE)
        return true;
    }
    else {
      dbmsh3d_halfedge* HE = headHE;
      do {
        if (headHE->edge() == inputE)
          return true;
        HE = HE->next();
      }
      while (HE != headHE);
    }
  }
  return false;
}

bool dbmsh3d_sheet::is_E_in_icurve_pair (const dbmsh3d_edge* E) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) {
      if (headHE->edge() == E)
        return true;
    }
  }
  return false;
}
//: Find the bnd or internal-curve HE of the given edge.
//  Return 1 if found in bnd chain.
//  Return 2 if found in i-curve pair.
//  Return 3 if found in i-curve loop.
//  Return 0 if not found.
int dbmsh3d_sheet::find_bnd_IC_HE (const dbmsh3d_edge* inputE, dbmsh3d_halfedge*& foundHE) const
{
  dbmsh3d_halfedge* HE = find_bnd_HE (inputE);
  if (HE) { //found in the boundary chain.
    foundHE = HE;
    return 1;
  }

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();

    if (is_icurve_pair_HE (headHE)) { //found in the i-curve pair
      HE = (dbmsh3d_halfedge*) headHE;
      dbmsh3d_edge* E = HE->edge();  
      if (E == inputE) {
        foundHE = HE;
        return 2;    
      }
    }
    else { //found in the i-curve loop.
      HE = (dbmsh3d_halfedge*) headHE;
      do {
        dbmsh3d_edge* E = HE->edge();
        if (E == inputE) {
          foundHE = HE;
          return 3;    
        }
        HE = HE->next();
      }
      while (HE != (dbmsh3d_halfedge*) headHE && HE != NULL);
    }
  }

  //not found.
  foundHE = NULL;
  return 0;
}

void dbmsh3d_sheet::get_incident_Es (vcl_set<dbmsh3d_edge*>& incident_Es) const
{  
  //1) Add each bnd_curve
  dbmsh3d_face::get_bnd_Es (incident_Es);

  //2) Add each icurve
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();    
    get_chain_Es (headHE, incident_Es);
  }
}

void dbmsh3d_sheet::get_incident_Vs (vcl_set<dbmsh3d_vertex*>& incident_Vs) const
{
  assert (incident_Vs.size() == 0);

  //1) Add both vertices of each bnd_curve
  dbmsh3d_face::get_bnd_Vs (incident_Vs);

  //2) Add both vertices of each icurve_head.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    get_chain_Vs (headHE, incident_Vs);
  }
}

void dbmsh3d_sheet::get_incident_Sset_via_C_N (vcl_set<dbmsh3d_sheet*>& incident_Sset) const
{
  vcl_set<dbmsh3d_edge*> incident_Es;
  get_incident_Es (incident_Es);

  vcl_set<dbmsh3d_edge*>::iterator eit = incident_Es.begin();
  for (; eit != incident_Es.end(); eit++) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) (*eit);
    dbmsh3d_halfedge* HE = C->halfedge();
    do {
      dbmsh3d_sheet* S = (dbmsh3d_sheet*) HE->face();
      incident_Sset.insert (S);
      HE = HE->pair();
    }
    while (HE != C->halfedge() && HE != NULL);
  }

  vcl_set<dbmsh3d_vertex*> incident_Vs;
  get_incident_Vs (incident_Vs);

  vcl_set<dbmsh3d_vertex*>::iterator vit = incident_Vs.begin();
  for (; vit != incident_Vs.end(); vit++) {
    dbmsh3d_node* N = (dbmsh3d_node*) (*vit);
    vcl_set<dbmsh3d_face*> faceset;
    N->get_incident_Fs (faceset);

    vcl_set<dbmsh3d_face*>::iterator fit = faceset.begin();
    for (; fit != faceset.end(); fit++) {
      dbmsh3d_sheet* S = (dbmsh3d_sheet*) (*fit);
      incident_Sset.insert (S);
    }    
  }

  incident_Sset.erase ((dbmsh3d_sheet*) this);
}

dbmsh3d_halfedge* dbmsh3d_sheet::get_1st_HE_incident_V (const dbmsh3d_halfedge* startHE, 
                                                        dbmsh3d_vertex* V) const
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) startHE;
  do {
    if (HE->edge()->is_V_incident (V))
      return HE;
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) startHE && HE != NULL);
  return NULL;
}

dbmsh3d_halfedge* dbmsh3d_sheet::get_1st_bnd_HE_incident_V (const dbmsh3d_halfedge* startHE, 
                                                            dbmsh3d_vertex* V) const
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) startHE;
  do {
    if (HE->pair() == NULL && HE->edge()->is_V_incident (V))
      return HE;
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) startHE && HE != NULL);
  return NULL;
}

int dbmsh3d_sheet::n_bnd_IC_chain_E_inc_V (const dbmsh3d_vertex* inputV) const
{
  int n = 0;
  if (_contain_V_in_chain (halfedge_, inputV))
    n++;

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (_contain_V_in_chain (headHE, inputV))
      n++;
  }
  return n;
}

//: get the set of i-curve pair curves.
void dbmsh3d_sheet::get_icurve_pairs (vcl_set<dbmsh3d_edge*>& icurves) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE) == false)
      continue; //Skip the none i-curve pair chain.
    icurves.insert (headHE->edge());
  }
}

//: get the set of i-curve pair and loops.
void dbmsh3d_sheet::get_icurves (vcl_set<dbmsh3d_edge*>& icurves) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    dbmsh3d_halfedge* HE = headHE;
    do {
      icurves.insert (HE->edge());
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);    
  }
}

//: get the set of fine-scale vertices in i-curve chains.
void dbmsh3d_sheet::get_icurve_Vset (vcl_set<dbmsh3d_vertex*>& Vset) const
{
  vcl_set<dbmsh3d_edge*> icurves;
  get_icurves (icurves);

  vcl_set<dbmsh3d_edge*>::iterator it = icurves.begin();
  for (; it != icurves.end(); it++) {
    dbmsh3d_curve* C = (dbmsh3d_curve*) (*it);
    C->get_V_set (Vset);
  }
}

//: Return the set of boundary nodes incident to any i-curves chain (pairs or loops).
void dbmsh3d_sheet::get_bnd_Ns_inc_ICchain (vcl_set<dbmsh3d_vertex*>& bnd_N_set) const
{
  vcl_set<dbmsh3d_vertex*> vertices;
  
  vcl_vector<dbmsh3d_vertex*> V_vec;
  _get_Vs_in_chain (halfedge_, V_vec);
  vertices.insert (V_vec.begin(), V_vec.end());

  vcl_set<dbmsh3d_vertex*>::iterator it = vertices.begin();
  for (; it != vertices.end(); it++) {
    dbmsh3d_vertex* V = (*it);
    vcl_set<dbmsh3d_edge*> ICchains;
    get_ICchains_inc_N (V, ICchains); //get_ICpairs_inc_N

    if (ICchains.empty() == false)
      bnd_N_set.insert (V);
  }
}

//: Return the set of boundary and icurve-loop nodes incident to any i-curves.
void dbmsh3d_sheet::get_bnd_iloop_N_icurve (vcl_set<dbmsh3d_vertex*>& bnd_iloop_N_set) const
{
  vcl_set<dbmsh3d_vertex*> vertices;
  get_bnd_icurveloop_Ns (vertices);

  vcl_set<dbmsh3d_vertex*>::iterator it = vertices.begin();
  for (; it != vertices.end(); it++) {
    dbmsh3d_vertex* V = (*it);
    vcl_set<dbmsh3d_edge*> ICpairs;
    get_ICpairs_inc_N (V, ICpairs);
    if (ICpairs.empty() == false)
      bnd_iloop_N_set.insert (V);
  }
}

bool dbmsh3d_sheet::Es_in_same_loop (dbmsh3d_edge* E1, dbmsh3d_edge* E2) const
{
  //Find the HE of E1-sheet.
  dbmsh3d_halfedge* HE1 = E1->get_HE_of_F (this);
  assert (HE1);

  //Traverse the HE loop of HE1 to find HE2.
  dbmsh3d_halfedge* HE = HE1;
  do {
    if (HE->edge() == E2)
      return true;
    HE = HE->next();
  }
  while (HE != HE1 && HE != NULL);

  return false;
}

//: get the set of i-curve pair curves incident to N.
void dbmsh3d_sheet::get_ICpairs_inc_N (const dbmsh3d_vertex* N, vcl_set<dbmsh3d_edge*>& ICpairs) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE) == false)
      continue; //Skip the none i-curve pair.

    dbmsh3d_edge* E = headHE->edge();
    if (E->is_V_incident (N))
      ICpairs.insert (E); //such i-curve pair found.
  }
}

//: get the set of i-curve chain curves incident to N.
void dbmsh3d_sheet::get_ICchains_inc_N (const dbmsh3d_vertex* N, vcl_set<dbmsh3d_edge*>& ICchains) const
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    dbmsh3d_edge* E = headHE->edge();
    if (E->is_V_incident (N))
      ICchains.insert (E); //such i-curve edge found.
  }
}

dbmsh3d_curve* dbmsh3d_sheet::get_incident_C_via_E (dbmsh3d_edge* E) const
{
  dbmsh3d_curve* C = _find_E_in_C_chain (halfedge_, E);
  if (C)
    return C;

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr(); 
    C = _find_E_in_C_chain (halfedge_, E);
    if (C)
      return C;
  }

  return NULL;
}

//: Get all incident MC's shared_Es.
bool dbmsh3d_sheet::get_incident_C_shared_Es (vcl_set<dbmsh3d_edge*>& shared_Es) const
{
  _get_chain_C_shared_Es (halfedge_, shared_Es);

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();    
    _get_chain_C_shared_Es (headHE, shared_Es);
  }

  return shared_Es.size() != 0;
}

dbmsh3d_curve* dbmsh3d_sheet::find_next_bnd_C (const dbmsh3d_node* inputN, 
                                               const dbmsh3d_curve* inputC) const
{
  dbmsh3d_curve* C = (dbmsh3d_curve*) _find_next_E_chain (halfedge_, inputN, inputC);
  if (C != NULL)
    return C;

  //Search in bnd_chain in icurve-loops.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue; //skip icurve-pairs.
    C = (dbmsh3d_curve*) _find_next_E_chain (headHE, inputN, inputC);
    if (C != NULL)
      return C;
  }
  assert (0);
  return NULL;
}

void dbmsh3d_sheet::get_bnd_icurveloop_Ns (vcl_set<dbmsh3d_vertex*>& N_set) const
{
  vcl_vector<dbmsh3d_vertex*> V_vec;
  _get_Vs_in_chain (halfedge_, V_vec);
  N_set.insert (V_vec.begin(), V_vec.end());

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue;
    V_vec.clear();
    _get_Vs_in_chain (headHE, V_vec);
    N_set.insert (V_vec.begin(), V_vec.end());
  }
}

//Find any duplicate E from icurve-pair in bnd-chain or icurve-loop.
dbmsh3d_halfedge* dbmsh3d_sheet::_find_dup_Es_icurve_pair () const
{
  vcl_set<dbmsh3d_edge*> icurve_pair_Es;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) {
      dbmsh3d_edge* E = headHE->edge();
      icurve_pair_Es.insert (E);
    }
  }

  if (icurve_pair_Es.size() == 0)
    return NULL;

  //Check if bnd_chain contains icurve-pair_Es.
  dbmsh3d_halfedge* HE = chain_contains_E_in_set (halfedge_, icurve_pair_Es);
  if (HE)
    return HE;

  //Go through 
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue;

    HE = chain_contains_E_in_set (headHE, icurve_pair_Es);
    if (HE)
      return HE;
  }

  return NULL;
}

//: Return the first found otherC of this sheet incident to N via F.
bool dbmsh3d_sheet::get_otherC_via_F (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                                      dbmsh3d_curve*& otherC, dbmsh3d_face*& F)
{
  dbmsh3d_edge* E = inputC->get_E_incident_N (N);
  assert (E);

  //Get the first found F of this sheet which is incident to E.
  F = get_1st_F_incident_E (E);
  if (F == NULL)
    return false;

  while (1) {      
    dbmsh3d_edge* otherE = F->find_next_bnd_E (N->V(), E);
    assert (otherE);
    otherC = (dbmsh3d_curve*) N->find_C_containing_E (otherE);
    if (otherC)
      return true;
    
    //Visit the next face of this sheet incident to N
    E = otherE;
    F = E->m2_other_face (F);
    if (F == NULL)
      return false;
  }
  assert (0);
  return false;
}

dbmsh3d_face* dbmsh3d_sheet::get_corner_F (const dbmsh3d_curve* C1, const dbmsh3d_node* N,
                                           const dbmsh3d_curve* C2)
{
  dbmsh3d_edge* inputE1 = C1->get_E_incident_N (N);  
  assert (inputE1);

  //Traverse E1's all incident F's.
  //Look for the F belongs to this sheet and is incident to E1 of C1 and E2 of C2.
  dbmsh3d_halfedge* HE = inputE1->halfedge();
  do {
    if (contain_F (HE->face()->id())) {
      dbmsh3d_face* F = HE->face();
      assert (F);
      dbmsh3d_edge* E1 = inputE1;

      while (1) {      
        dbmsh3d_edge* E2 = F->find_next_bnd_E (N->V(), E1);
        assert (E2);
        if (C2->contain_E (E2))
          return F;
        if (E2 == inputE1)
          break; //continue to search the next face F.
        
        //Visit the next face of this sheet incident to N
        E1 = E2;
        F = E1->m2_other_face (F);
        assert (F);
      }
    }
    HE = HE->pair();
  }
  while (HE != inputE1->halfedge() && HE != NULL);

  return NULL;
}

//: Return the first found otherC of this sheet incident to N via F 
//  with a constraint that F != inputF in the input set.
dbmsh3d_curve* dbmsh3d_sheet::get_1st_other_C_via_F2 (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                                                      vcl_set<dbmsh3d_face*>& inputF_set)
{
  dbmsh3d_edge* E = inputC->get_E_incident_N (N);
  assert (E);

  //Get the first found F not in inputF_set of this sheet which is incident to E.
  dbmsh3d_face* F = get_1st_F_incident_E2 (E, inputF_set);
  if (F == NULL)
    return NULL;

  while (1) {      
    dbmsh3d_edge* otherE = F->find_next_bnd_E (N->V(), E);
    dbmsh3d_curve* otherC = (dbmsh3d_curve*) N->find_C_containing_E (otherE);
    if (otherC) {
      //Add this F used to find otherC into the inputF_set.
      inputF_set.insert (F);
      return otherC;
    }
    
    //Visit the next face of this sheet incident to N
    E = otherE;
    F = E->m2_other_face (F);
  }
  assert (0);
  return NULL;
}

//: Check if any of E's incident F belongs to this sheet.
bool dbmsh3d_sheet::fine_scale_E_incident (const dbmsh3d_edge* E)
{
  dbmsh3d_halfedge* HE = E->halfedge();
  do {
    dbmsh3d_face* F = HE->face();
    if (contain_F (F->id()))
      return true;
    HE = HE->pair();
  }
  while (HE != E->halfedge() && HE != NULL);
  return false;
}

bool dbmsh3d_sheet::find_bnd_chain (dbmsh3d_vertex* sV, dbmsh3d_vertex* eV, 
                                    vcl_vector<dbmsh3d_halfedge*>& HEvec)
{
  if (_find_in_chain (halfedge_, sV, eV, HEvec))
    return true;

  //Search in each icurve-loop.  
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();    
    if (is_icurve_pair_HE (headHE))
      continue;
    if (_find_in_chain (headHE, sV, eV, HEvec))
      return true;
  }
  return false;
}

//: Return true if all shared_F[] are with one of the given set of S's.
bool dbmsh3d_sheet::shared_F_with_Sset (vcl_set<dbmsh3d_sheet*>& shared_F_Sset) const
{
  //Loop through each shared_E of MC and try to remove isolated entry from other curves.
  for (dbmsh3d_ptr_node* cur = shared_F_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    bool F_in_Sset = false;
    vcl_set<dbmsh3d_sheet*>::iterator it = shared_F_Sset.begin();
    for (; it != shared_F_Sset.end(); it++) {
      dbmsh3d_sheet* S = (*it);
      if (S->is_F_shared (F))
        F_in_Sset = true;
    }    
    if (F_in_Sset == false)
      return false;
  }
  return true;
}

//##########################################################
//###### Connectivity Modification Functions ######
//##########################################################

//: Set the referencing headHE to the next if its equal to inputHE.
void dbmsh3d_sheet::_set_headHE_to_next (dbmsh3d_halfedge* inputHE)
{ 
  if (halfedge_ == inputHE) {
    //If the merging edge is at the main boundary, point to the next HE.
    assert (halfedge_->next());
    //Need to avoid pointing the bnd halfedge_ to the 3-incidence edge.
    dbmsh3d_halfedge* nextHE = halfedge_->next();
    if (nextHE != nextHE->next())
      halfedge_ = nextHE;
    else {
      nextHE = nextHE->next();
      nextHE = nextHE->next();
      nextHE = nextHE->next();
      assert (nextHE != inputHE);
      halfedge_ = nextHE;
    }
    return;
  }

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (headHE != inputHE)
      continue;    
    if (headHE->next() == NULL) { //Not considering single icurve HE here.
      assert (inputHE != headHE);
      continue;
    }        
    if (is_icurve_pair_HE(headHE)) //Not considering the icurve-pair (no possible).
      continue;

    //set cur point to headHE->next().
    cur->set_ptr (headHE->next());
    return;
  }
}

bool dbmsh3d_sheet::_set_headHE_to_E (const dbmsh3d_edge* inputE)
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->edge() == (dbmsh3d_edge*) inputE) {
      assert (is_HE_3_incidence (HE) == false);
      halfedge_ = HE;
      return true;
    }
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

//: Disconnect HE from this sheet: either from the bnd chain or the i-curve chains.
bool dbmsh3d_sheet::_disconnect_HE (dbmsh3d_halfedge* inputHE)
{
  if (dbmsh3d_face::_disconnect_bnd_HE (inputHE))
    return true;

  if (_disconnect_HE_icurve (inputHE))
    return true;

  return false;
}

//: Disconnect HE from the i-curve chain. Two cases:
//    1) inputHE is the headHE: 
//       - for the single HE case: need to remove the cur i-curve entry.
//    2) inputHE is in the pair loop of i-curve entry.
//
bool dbmsh3d_sheet::_disconnect_HE_icurve (dbmsh3d_halfedge* inputHE)
{
  dbmsh3d_ptr_node* prev = NULL;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    dbmsh3d_halfedge* nextHE = headHE->next();

    if (nextHE == NULL) { //case I: a single headHE
      if (headHE == inputHE) { 
        inputHE->set_next (NULL);
        //Remove the cur entry in i-curve chain.
        cur->set_ptr (NULL);
        //Remove current pointer from the i-curve chain.
        if (prev == NULL)
          icurve_chain_list_ = cur->next();
        else
          prev->set_next (cur->next());
        delete cur;
        return true;
      }
    }
    else if (nextHE->next() == headHE) { //case II: headHE and nextHE in a loop.
      if (inputHE == headHE) {
        cur->set_ptr (nextHE);
        nextHE->set_next (NULL);
        inputHE->set_next (NULL);
        return true;
      }
      else if (inputHE == nextHE) {
        headHE->set_next (NULL);
        inputHE->set_next (NULL);
        return true;
      }
    }
    else { //case III: more than 3 HEs in a circular loop.
      if (headHE == inputHE)
        cur->set_ptr (headHE->next());
      //look for inputHE and and remove from the HE loop.
      dbmsh3d_halfedge* HE = headHE; 
      do {
        dbmsh3d_halfedge* nextHE = HE->next();
        if (nextHE == inputHE) {           
          //Remove nextHE from the i-curve loop.
          HE->set_next (nextHE->next());
          inputHE->set_next (NULL);
          return true;
        }
        HE = nextHE;
      }
      while (HE != headHE);
    }
    prev = cur;
  }

  return false;
}

bool dbmsh3d_sheet::connect_adj_HE (dbmsh3d_halfedge* HE0, dbmsh3d_halfedge* HEn)
{  
  //1) Try connect HEn to the icurve chain.
  if (is_E_in_icurves (HE0->edge())) {
    //Find HE02: the other halfedge pointing to HE0.E.
    dbmsh3d_halfedge* HE02 = HE0->edge()->get_HE_of_F (this, HE0->pair());
    assert (HE02 != HE0);

    //Find HEn2: the other halfedge pointing to HEn.E.
    dbmsh3d_halfedge* HEn2 = HEn->edge()->get_HE_of_F (this, HEn->pair());
    assert (HEn2 != HEn);

    HE0->set_next (HE02->next());
    HE02->set_next (HE0->next());

    HEn->set_next (HEn2->next());
    HEn2->set_next (HEn->next());

    //Make HEn a standalone i-curve.
    add_icurve_chain_HE (HEn);
    return true;
  }
  //2) Connect to the bnd or i-curve loop chain.
  else if (connect_adj_bnd_HE (HE0, HEn)) {    
    return true;
  }
  
  assert (0);
  return false;
}

bool dbmsh3d_sheet::disconnect_bnd_E (dbmsh3d_edge* E)
{  
  bool r = _disconnect_E_chain (halfedge_, E);
  //Need to handle the case if halfedge_ == NULL!!
  assert (halfedge_);
  if (r)
    return true;

  //Disconnect from icurve-loops.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE(headHE)) 
      continue; //Not considering icurve-pairs.

    r = _disconnect_E_chain (headHE, E);
    if (r) {
      assert (headHE); //Need to handle the case if headHE == NULL!!
      cur->set_ptr (headHE);
      return true;
    }
  }
  return false;
}

//: Disconnect a sheet and a boundary edge in a tri-incidence.
bool dbmsh3d_sheet::disconnect_bnd3_E (dbmsh3d_edge* E)
{  
  bool r = _disconnect_bnd3_E_chain (halfedge_, E);
  //Need to handle the case if halfedge_ == NULL!!
  assert (halfedge_);
  if (r)
    return true;

  //Disconnect from icurve-loops.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE(headHE)) 
      continue; //Not considering icurve-pairs.

    r = _disconnect_bnd3_E_chain (headHE, E);
    assert (headHE);
    cur->set_ptr (headHE);
    if (r)
      return true;
  }
  return false;
}

//: Connect E as a internal break curve (2-incidence).
dbmsh3d_halfedge* dbmsh3d_sheet::connect_icurve_pair_E (dbmsh3d_edge* E)
{
  dbmsh3d_halfedge* he1 = new dbmsh3d_halfedge (E, this);
  dbmsh3d_halfedge* he2 = new dbmsh3d_halfedge (E, this);
  he1->set_next (he2);
  he2->set_next (he1);
  E->_connect_HE_end (he1);
  E->_connect_HE_end (he2);
  add_icurve_chain_HE (he1);
  return he1;
}

//: Connect a loop of edges as a internal curve chain (1-incidence).
void dbmsh3d_sheet::connect_icurve_loop_Es (vcl_vector<dbmsh3d_edge*>& icurve_loop_Es)
{
  bool result = _verify_loop_bnd_Es (icurve_loop_Es);
  assert (result);

  dbmsh3d_edge* E = icurve_loop_Es[0];  
  dbmsh3d_halfedge* headHE = new dbmsh3d_halfedge (E, this);
  E->_connect_HE_end (headHE);

  if (icurve_loop_Es.size() > 1) {
    dbmsh3d_halfedge* prevHE = headHE;
    for (unsigned int i=1; i<icurve_loop_Es.size(); i++) {
      E = icurve_loop_Es[i];

      dbmsh3d_halfedge* HE = new dbmsh3d_halfedge (E, this);    
      prevHE->set_next (HE);
      E->_connect_HE_end (HE);
      prevHE = HE;
    }    
    prevHE->set_next (headHE); //Make a loop.
  }

  add_icurve_chain_HE (headHE);
}

bool dbmsh3d_sheet::disconnect_icurve_pair_E (dbmsh3d_edge* E)
{
  //Search E in the icurve chain.
  dbmsh3d_ptr_node* prev = NULL;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (headHE->edge() == E) {    
      //If found, delete this entry from the icurve chain.
      dbmsh3d_halfedge* nextHE = headHE->next();
      assert (nextHE->next() == headHE);
      assert (nextHE->edge() == E);

      //Remove headHE from E's pair list.
      bool result = E->_disconnect_HE (headHE);
      assert (result);
      delete headHE;

      //Remove nextHE from E's pair list.
      result = E->_disconnect_HE (nextHE);
      assert (result);
      delete nextHE;
      
      //Remove the cur entry in i-curve chain.
      cur->set_ptr (NULL);
      //Remove current pointer from the i-curve chain.
      if (prev == NULL)
        icurve_chain_list_ = cur->next();
      else
        prev->set_next (cur->next());
      delete cur;
      return true;
    }
    prev = cur;
  }
  return false;
}


//: Re-organize this sheet's bnd_HE_chain and icurve_HE_chain such that
//  there are no duplicate HE in bnd_HE_chain and icurve_loop_chain.
//  Also subdivide all IC_loop with self-intersections.
void dbmsh3d_sheet::canonicalization ()
{
  bool change, r;
  //Repeatedly detect and fix duplicate HEs until no change can be made.
  do {
    change = false;
    //1) Detect and move all successive same HEs from bnd_chain to icurve_chain.
    do {
      r = detect_fix_2_suc_Es (halfedge_);
      change |= r;
    }
    while (r == true);

    //Detect and move all successive same HEs from icurve_chain to a separate icurve_chain.
    for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
      dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
      if (is_icurve_pair_HE (headHE))
        continue; //skip the icurve-pair of HEs.
      do {      
        r = detect_fix_2_suc_Es (headHE); 
        change |= r;
      }
      while (r == true);
    }

    //2) Detect and move all duplicate HEs to from bnd_chain to icurve_chain and
    //move everything in between to icurve_loop.
    do {
      r = detect_fix_2_sep_Es (halfedge_);
      change |= r;
    }
    while (r == true);

    //Detect and move all successive same HEs from icurve_chain to a separate icurve_chain.
    for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
      dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
      if (is_icurve_pair_HE (headHE))
        continue; //skip the icurve-pair of HEs.
      do {      
        r = detect_fix_2_sep_Es (headHE); 
        change |= r;
      }
      while (r == true);
    }
  }
  while (change == true);

  //Repeatedly detect and fix duplicate HEs until no change can be made.
  do {
    change = false;
    //3) Detect and fix the bnd_chain on any self-intersections.
    do {
      r = detect_fix_loop_self_int (halfedge_);
      change |= r;
    }
    while (r == true);

    //Detect and fix the icurve_loop_chain on any self-intersections.
    for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
      dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
      if (is_icurve_pair_HE (headHE))
        continue; //skip the icurve-pair of HEs.
      do {      
        r = detect_fix_loop_self_int (headHE); 
        cur->set_ptr (headHE);
        change |= r;
      }
      while (r == true);
    }

  }
  while (change == true);

  //Fix the case the bnd_chain is an icurve-pair.
  _fix_bnd_icurve_pair ();

  //Merge 3-incidence HEs into bnd_chain or icurve-loop.
  dbmsh3d_halfedge* HE = _find_dup_Es_icurve_pair ();
  while (HE) {
    fix_dup_Es_icurve_pair (HE);
    HE = _find_dup_Es_icurve_pair ();
  }

  //Check and fix if the halfedge_ or headHE is pointing to 3-incidence.
  if (is_HE_3_incidence (halfedge_))
    fix_headHE_3_incidence (halfedge_);
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_HE_3_incidence (headHE)) {
      fix_headHE_3_incidence (headHE);
      cur->set_ptr (headHE);
    }
  }
}

//: Fix the case the bnd_chain is a icurve-pair.
bool dbmsh3d_sheet::_fix_bnd_icurve_pair ()
{
  if (halfedge_ == NULL) {
    //Find the first icurve-loop and move to the bnd_chain.
    dbmsh3d_ptr_node* prev = NULL;
    for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
      dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
      if (is_icurve_pair_HE (headHE) == false) {
        //move headHE to bnd_chain and delete cur.
        halfedge_ = headHE;
        if (prev == NULL) {
          icurve_chain_list_ = cur->next();
          delete cur;
          return true;
        }
        else {
          prev->set_next (cur->next());
          delete cur;
          return true;
        }
      }
      prev = cur;
    }

    assert (0);
    return false;
  }

  if (halfedge_->next() == NULL)
    return false;
  if (halfedge_->edge() != halfedge_->next()->edge())
    return false;

  //Swap the bnd_chain with the longest icurve-loop.
  dbmsh3d_ptr_node* icurve_loop_ptr = _find_1st_icurve_loop();
  if (icurve_loop_ptr == NULL) {
    vul_printf (vcl_cout, "_fix_bnd_icurve_pair() error!\n");
    assert (0);
    return false;
  }
  else {
    dbmsh3d_halfedge* tmp = halfedge_;
    halfedge_ = (dbmsh3d_halfedge*) icurve_loop_ptr->ptr();
    icurve_loop_ptr->set_ptr (tmp);
    return true;
  }

  //Swap the bnd_chain with the first found icurve-loop.
  /*for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue;
    //swap with the halfedge.
    dbmsh3d_halfedge* tmp = (dbmsh3d_halfedge*) headHE;
    cur->set_ptr (halfedge_);
    halfedge_ = tmp;
    return true;
  }

  vul_printf (vcl_cout, "_fix_bnd_icurve_pair() error!\n");
  assert (0);
  return false;*/
}


void dbmsh3d_sheet::fix_dup_Es_icurve_pair (dbmsh3d_halfedge* inputHE)
{
  dbmsh3d_edge* E = inputHE->edge();

  //remove the HE's of E from icurve-pair
  dbmsh3d_halfedge *HE1 = NULL, *HE2 = NULL;
  dbmsh3d_ptr_node* prev = NULL;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) {
      if (headHE->edge() == E) {
        HE1 = (dbmsh3d_halfedge*) headHE;
        HE2 = headHE->next();
        assert (HE1->edge() == HE2->edge());

        //remove the cur entry.
        if (prev == NULL) {
          icurve_chain_list_ = cur->next();
          delete cur;
        }
        else {
          prev->set_next (cur->next());
          delete cur;
        }
        break; //quit the loop
      }
    }
    prev = cur;
  }

  //Put HE1 and HE2 input inputHE's chain.
  dbmsh3d_halfedge* nextHE = inputHE->next();
  inputHE->set_next (HE1);
  assert (HE1->next() == HE2);
  HE2->set_next (nextHE);

  if (halfedge_ == inputHE)
    halfedge_ = nextHE;
}

//: Re-organize this sheet's bnd_HE_chain and icurve_HE_chain such that
//  there are no duplicate HE in bnd_HE_chain and icurve_loop_chain.
//   - the 3-incidence (3 consecutive edges) is not considered!
//   - Detect a pair of consecutive bnd-SCs and move it to i-curve.
//   - Detect a pair of HEs and move the chain in between into icurve_loop.
//  Return true if a change is being made.
bool dbmsh3d_sheet::detect_fix_2_suc_Es (const dbmsh3d_halfedge* headHE)
{
  if (headHE == NULL)
    return false;
  if (headHE->next() == NULL)
    return false;

  dbmsh3d_halfedge* HE1 = (dbmsh3d_halfedge*) headHE;  
  dbmsh3d_halfedge* HE2 = HE1->next();
  dbmsh3d_halfedge* HE3 = HE2->next();
  dbmsh3d_halfedge* HE4 = HE3->next();
  if (HE1 == HE3) 
    return false; //Only 2 halfedges in loop, no need to fix. Done.

  //The 3-edge case possibly resulting in a loop after fixing.
  if (HE1 == HE4)
    return detect_fix_2_suc_Es_loop3 (headHE);

  do { //traverse through the circular list of halfedges
    HE2 = HE1->next();
    HE3 = HE2->next();
    HE4 = HE3->next();
    if (HE1->edge() != HE2->edge() &&
        HE2->edge() == HE3->edge() && 
        HE3->edge() != HE4->edge()) { 
      //Detected 2-incident edges in bnd-curve:
      dbmsh3d_vertex* V = Es_sharing_V (HE1->edge(), HE4->edge());
      assert (V);
      //remove HE2 and HE3 from bnd-curve.
      HE1->set_next (HE4);
      HE3->set_next (HE2);

      //Create a new i-curve chain.
      if (headHE == HE2 || headHE == HE3) {
        //Move the chain of HE1 to a new i-curve chain.
        add_icurve_chain_HE (HE1);
        vul_printf (vcl_cout, "S%d: E%d HE-pair to i-curve, ", id_, HE1->edge()->id());
      }
      else {
        //Move the chain of HE2 and HE3 to a new i-curve chain.
        add_icurve_chain_HE (HE2);
        vul_printf (vcl_cout, "S%d: E%d HE-pair to i-curve, ", id_, HE2->edge()->id());
      }      
      return true;
    }
    HE1 = HE2;
  }
  while (HE1 != headHE);
  return false;
}

//: Fix the topology resulting in a loop case.
//  HE1 - HE2 - nextHE
//  Return true if a change is being made.
bool dbmsh3d_sheet::detect_fix_2_suc_Es_loop3 (const dbmsh3d_halfedge* headHE)
{
  dbmsh3d_halfedge* HE1 = (dbmsh3d_halfedge*) headHE;  
  dbmsh3d_halfedge* HE2 = HE1->next();
  dbmsh3d_halfedge* HE3 = HE2->next();
  assert (HE3->next() == HE1);
  //E1, E2, E3 can not be all identical.
  assert (HE1->edge() != HE2->edge() || HE2->edge() != HE3->edge() || HE3->edge() != HE1->edge());

  if (HE1->edge() == HE2->edge()) {
    //Keep the pair (HE1, HE2) and move HE3 to a new icurve-chain.
    HE2->set_next (HE1);
    HE3->set_next (NULL);
    add_icurve_chain_HE (HE3);
    return true;
  }
  else if (HE2->edge() == HE3->edge()) {
    //Keep H1 and move the pair (HE2, HE3) to a new icurve-chain.
    HE1->set_next (NULL);
    HE3->set_next (HE2);
    add_icurve_chain_HE (HE2);
    return true;
  }
  else if (HE1->edge() == HE3->edge()) {
    //Keep the pair (HE1, HE3) and move HE2 to a new icurve-chain.
    HE2->set_next (NULL);
    HE1->set_next (HE3);
    add_icurve_chain_HE (HE2);    
    return true;
  }
  else //HE1 != HE2 != HE3
    return false; //no need to fix
}

unsigned int _get_round (const int sz, int i)
{
  return static_cast<unsigned int>((i + sz) % sz);
}

//: Detect and fix any two HEs pointing to the same edge.
//   - the 3-incidence (3 consecutive edges) is not considered!
//  Return true if a change is being made.
bool dbmsh3d_sheet::detect_fix_2_sep_Es (const dbmsh3d_halfedge* headHE)
{
  vcl_vector<dbmsh3d_halfedge*> HE_vec;
  get_HEvec_next_chain (headHE, HE_vec);

  if (HE_vec.size() < 2)
    return false;

  for (unsigned int i=0; i<HE_vec.size()-1; i++) {
    for (unsigned int j=i+1; j<HE_vec.size(); j++) {
      if (HE_vec[i]->edge() != HE_vec[j]->edge())
        continue;
      if (i+1 == j) //Skip 2_suc_HEs. return false;
        continue;
      if (_check_3_incidence (HE_vec, i, j))
        continue;

      //Skip if adjacent HE's not sharing V's.
      unsigned int im = _get_round (HE_vec.size(), int(i-1));
      unsigned int ip = _get_round (HE_vec.size(), int(i+1));
      unsigned int jm = _get_round (HE_vec.size(), int(j-1));
      unsigned int jp = _get_round (HE_vec.size(), int(j+1));
      if (Es_sharing_V (HE_vec[im]->edge(), HE_vec[jp]->edge()) == false)
        continue;
      if (Es_sharing_V (HE_vec[ip]->edge(), HE_vec[jm]->edge()) == false) 
        continue;

      //Move HE_vec[i+1] to HE_[jm] to a new circular icurve-loop.
      if (ip == jm) {
        //Add HE_vec[i+1] as a new icurve chain.
        HE_vec[ip]->set_next (NULL);
        add_icurve_chain_HE (HE_vec[ip]);
        vul_printf (vcl_cout, "S %d: move a single HE of E %d to i-curve, ", id_, HE_vec[ip]->edge()->id());
      }
      else {
        //Add HE_vec[ip] ... HE_vec[jm] as a new icurve chain.
        HE_vec[jm]->set_next (HE_vec[ip]);
        add_icurve_chain_HE (HE_vec[ip]);
        vul_printf (vcl_cout, "S %d: move a chain of %d HEs to i-curve, ", id_, jm-i);
      }
      HE_vec[i]->set_next (HE_vec[j]);

      //Move HE_vec[i] and HE_vec[j] to a new icurve-pair.
      if (is_icurve_pair_HE (headHE) == false) {
        //Simply use the function detect_fix_2_suc_Es() to save time.
        bool result = detect_fix_2_suc_Es (headHE);
        assert (result);
      }
      return true;
    }
  }

  return false;
}

//: Detect and fix any self-intersection of the chain of a halfedge loop.
//  Return true if any change is made.
bool dbmsh3d_sheet::detect_fix_loop_self_int (dbmsh3d_halfedge*& headHE)
{
  if (is_HE_3_incidence(headHE)) {
    fix_headHE_3_incidence (headHE);
    return true;
  }

  vcl_vector<dbmsh3d_halfedge*> HEvec;
  get_HEvec_next_chain (headHE, HEvec);

  assert (HEvec.empty() == false);
  if (HEvec.size() == 1)
    return false;
  else if (HEvec.size() == 2) {
    //The special case of 2 halfedges in a loop.
    if (HEvec[1]->edge()->is_self_loop()) {
      assert (HEvec[0]->edge()->is_self_loop());
      HEvec[0]->set_next (NULL);
      HEvec[1]->set_next (NULL);
      add_icurve_chain_HE (HEvec[1]);
      return true;
    }
    else
      return false;
  }
  else {
    //The case of 3+ halfedges in a loop.

    //If the headHE itself is a self-loop, make the headHE pointint to the next.
    if (headHE->edge()->is_self_loop()) {
      headHE = headHE->next();
      return true;
    }

    //If any single HEvec[i] is a self_loop, make it a stand-alone icurve-loop.
    for (int i=1; i<int(HEvec.size()); i++) {      
      if (HEvec[i]->edge()->is_self_loop()) {
        int p = i-1;
        int n = (i+1) % HEvec.size();
        HEvec[i]->set_next (NULL);
        add_icurve_chain_HE (HEvec[i]);
        HEvec[p]->set_next (HEvec[n]);        
        return true;
      }
    }

    //Detect and remove any loop of 2 edges, skipping the 3-incidences.
    bool loop2;
    vcl_vector<dbmsh3d_halfedge*>::iterator it = HEvec.begin();
    vcl_vector<dbmsh3d_halfedge*>::iterator itn = it;
    itn++;
    for (; itn != HEvec.end(); it++, itn++) {
      
      dbmsh3d_halfedge* HE1 = (*it);
      dbmsh3d_halfedge* HE2 = (*itn);
      if (HE1->edge() == HE2->edge())
        continue; //skip the possible 3-incidences.
      if (is_HE_3_incidence(HE2))
        continue; //skip the possible 3-incidences.
      if (is_HE_3_incidence(_find_prev2_in_next_chain(HE1)))
        continue; //skip the possible 3-incidences.

      Es_sharing_V_check (HE1->edge(), HE2->edge(), loop2);
      if (loop2) {        
        HEvec.erase (it, itn+1);        
        assert (HEvec.empty() == false);

        //Separate the halfedge's next chain into two loops.
        dbmsh3d_halfedge* HEp = _find_prev_in_next_chain (HE1);
        HEp->set_next (HE2->next());
        HE2->set_next (HE1);

        if (headHE == HE1 || headHE == HE2) //1) Add the remaining halfedges to a new chain.
          add_icurve_chain_HE (HEvec[0]);
        else //2) Add HE1 and HE2 to a new chain.
          add_icurve_chain_HE (HE1);
        return true;
      }
    }

    //Detect and handle the general intersection of 3 more edges.
    //Assumption: HEvec[0] not 3-incidence.
    assert (is_HE_3_incidence (HEvec[0]) == false); 
    dbmsh3d_vertex* startV = Es_sharing_V_check (HEvec[0]->edge(), HEvec[1]->edge(), loop2);
    //In the special case of a loop of 4 HE's : one HE and another 3-inc HE, loop2 is true but ok!

    //Trace the HE loop in the order of startV -> HEvec[1] -> ... -> HEvec[0].
    dbmsh3d_halfedge* startHE = HEvec[0];
    dbmsh3d_halfedge* HE = startHE;
    dbmsh3d_vertex* eV = startV;    
    do {
      //Only check the last HE of the 3-incidence.
      if (is_HE_3_incidence (HE)) {
        HE = HE->next()->next();
        continue;
      }

      dbmsh3d_halfedge* HEnn2 = NULL;
      dbmsh3d_halfedge* HEnn = _find_nextHE_in_loop_3inc (HE, eV, HEvec, HEnn2);

      if (HEnn2) { //self-intersection found at eV.
        //Divide this chain loop into two.

        //Re-find the HEnn from (startV -> startHE -> ... to eV).to close the loop.
        HEnn = _find_lastHE_in_chain (startHE, startHE->edge()->other_V (startV), eV, HEvec);

        //Find the next of HE.
        dbmsh3d_halfedge* HEn = HE->next();
        //Find the prev of HEn.
        dbmsh3d_halfedge* HEnnp = _find_prev_in_next_chain (HEnn);
        assert (HEnnp->edge() != HEn->edge());

        //Close the two loops.
        HE->set_next (HEnn);
        HEnnp->set_next (HEn);
        //Add the other icurve-loop as an entry.
        check_add_icurve_chain_HE (HEn); //add_icurve_chain_HE (HEn);

        return true;
      }

      HE = HEnn;
      eV = HE->edge()->other_V (eV);
    }
    while (HE != startHE);
  }

  return false;
}


void dbmsh3d_sheet::disconnect_all_icurve_Es ()
{
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    _delete_HE_chain (headHE);
  }
  clear_icurve_chains();
}

//: disconnect all associated halfedges from their edges and delete them.
void dbmsh3d_sheet::disconnect_all_incident_Es () 
{
  if (halfedge_ == NULL)
    return;
  //disconnect and delete the boundary chain.
  _delete_HE_chain (halfedge_);
  //disconnect each internal curve chain.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    _delete_HE_chain (headHE);
  }
  clear_icurve_chains();
  halfedge_ = NULL;
}

void dbmsh3d_sheet::_clear_all_Cs_shared_Es ()
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    C->clear_shared_E_list();
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);

  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    HE = headHE;
    do {
      dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
      C->clear_shared_E_list();
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
  }
}

void dbmsh3d_sheet::add_shared_Fs_to_S (dbmsh3d_sheet* S)
{
  for (dbmsh3d_ptr_node* cur = shared_F_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    S->check_add_shared_F (F); //add_shared_F
  }
}

void dbmsh3d_sheet::pass_shared_Fs_to_S (dbmsh3d_sheet* S)
{
  add_shared_Fs_to_S (S);
  clear_shared_F_list ();
}


//##########################################################
//  Other functions 
//##########################################################

bool dbmsh3d_sheet::check_integrity ()
{
  //1) Check the boundary curve.
  if (dbmsh3d_face::check_integrity() == false) {
    assert (0);
    return false;
  }

  vcl_set<dbmsh3d_halfedge*> HE_set;
  
  if (halfedge_->next() == NULL) {    
    if (halfedge_->edge()->is_self_loop() == false) {
      assert (0);
      return false; //A loop of a single edge.
    }
    if (halfedge_->face() != this) {      
      assert (0);
      return false;
    }

    //The i-curve chain of a sheet of single loop is not checked!
  }
  else {
    //the halfedge_ should not point to a 3-incidence edge!
    if (is_HE_3_incidence (halfedge_)) {
      assert (0);
      return false;
    }
    //Check that no successive Es in the bnd_chain.
    if (_detect_2_suc_Es (halfedge_)) {
      assert (0);
      return false;
    }
    
    //The general case of loop of > 2 edges.
    dbmsh3d_halfedge* HE = halfedge_;
    do {
      dbmsh3d_halfedge* nextHE = HE->next();
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

      //Check that if HE is 3-incidence, they point to the same sheet (itself).
      if (is_HE_3_incidence (HE)) {
        if (HE->face() != HE->next()->face()) {
          assert (0);
          return false;
        }
        if (HE->next()->face() != HE->next()->next()->face()) {
          assert (0);
          return false;
        }
      }

      HE_set.insert (HE);
      HE = nextHE;
    }
    while (HE != halfedge_);    
  }

  //2) Check each i-curve chain (either an icurve-pair or an icurve-loop.  
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (headHE == NULL) {      
      assert (0);
      return false;
    }

    if (headHE->next() == NULL) {
      if (headHE->edge()->is_self_loop() == false) {
        assert (0);
        return false; //A loop of a single edge.
      }
      if (headHE->face() != this) {
        assert (0);
        return false;
      }
      if (HE_set.find (headHE) != HE_set.end()) {
        assert (0);
        return false; //Duplicate HEs found.
      }
      HE_set.insert (headHE);
    }
    else { 
      //the headHE should not point to a 3-incidence edge!
      if (is_HE_3_incidence (headHE)) {
        assert (0);
        return false;
      }

      if (headHE->next() == NULL) { 
        //I) a self loop of a single edge.
        if (headHE->edge()->is_self_loop() == false) {
          assert (0);
          return false; //A loop of a single edge.
        }
        if (headHE->face() != this) {
          assert (0);
          return false;
        }
      }
      else if (headHE->next()->next() == headHE) {
        //The special case of loop of two edges.
        if (_check_loop_of_2_HEs (headHE) == false) {
          assert (0);
          return false;
        }

        //II) The icurve-pair case of two halfedges.
        //III) The loop of two different edges.
        if (headHE->face() != this) {
          assert (0);
          return false;
        }
        if (HE_set.find (headHE) != HE_set.end()) {
          assert (0);
          return false; //Duplicate HEs found.
        }
        HE_set.insert (headHE);

        dbmsh3d_halfedge* nextHE = headHE->next();
        if (nextHE->face() != this) {
          assert (0);
          return false;
        }
        if (HE_set.find (nextHE) != HE_set.end()) {
          assert (0);
          return false; //Duplicate HEs found.
        }
        HE_set.insert (nextHE);
      }
      else { 
        //IV) The icurve-loop case of 3+ halfedges.   

        //Check that no success Es in the bnd_chain.
        if (_detect_2_suc_Es (headHE)) {
          assert (0);
          return false;
        }
        
        //The general case of loop of > 2 edges.
        ///vcl_set<dbmsh3d_halfedge*> HE_set;
        dbmsh3d_halfedge* HE = headHE;
        do {
          dbmsh3d_halfedge* nextHE = HE->next();
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

          //Check that if HE is 3-incidence, they point to the same sheet (itself).
          if (is_HE_3_incidence (HE)) {
            if (HE->face() != HE->next()->face()) {
              assert (0);
              return false;
            }
            if (HE->next()->face() != HE->next()->next()->face()) {
              assert (0);
              return false;
            }
          }

          HE_set.insert (HE);
          HE = nextHE;
        }
        while (HE != headHE);    
      }
    }
  }

  //Check that both the bnd_chain and all icurve-loop chains not self-intersecting.
  if (loop_self_int (halfedge_)) {
    assert (0);
    return false;
  }
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue;
    if (loop_self_int (headHE)) {
      assert (0);
      return false;
    } 
  }

  //Verify that no C from icurve-pair is also in bnd-chain or icurve-loop.
  //This is not required since the dup-check is done in HE_set check.
  if (_find_dup_Es_icurve_pair()) {
    assert (0);
    return false;
  }  

  //Verify that all FE's in the curves in B-chain and IC-chains are incident to the faces in S.F_map[].
  //This may not be guaranteed, since the shared_E's and shared_F's can violate this constraint in some cases.
  //Need to investigate more!
  /*if (check_Es_in_C_chain (halfedge_) == false) {
    assert (0);
    return false;
  }
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (check_Es_in_C_chain (headHE) == false) {
      assert (0);
      return false;
    }
  }*/

  //Verify that all non-shared fine-scale FF's are connected.
  //This may not be guaranteed, since the shared_E's and shared_F's can violate this constraint in some cases.
  //Need to investigate more!
  ///if (check_non_shared_Fs_connected () == false) {
    ///assert (0);
    ///return false;
  ///}

  return true;
}

bool dbmsh3d_sheet::check_Es_in_C_chain (const dbmsh3d_halfedge* headHE)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    for (unsigned int i=0; i<C->E_vec().size(); i++) {
      dbmsh3d_edge* E = C->E_vec(i);
      if (C->is_E_shared (E))
        continue; //skip shared E.

      //If none of E's incident F is in this sheet's facemap[], return false.
      bool E_incident_to_S = false;
      dbmsh3d_halfedge* he = E->halfedge();
      do {
        if (facemap_.find (he->face()->id()) != facemap_.end())
          E_incident_to_S = true;
        he = he->pair();
      }
      while (he != E->halfedge() && he != NULL);

      if (E_incident_to_S == false) {
        ///assert (0);
        return false;
      }
    }
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return true;
}

dbmsh3d_face* dbmsh3d_sheet::clone () const
{
  dbmsh3d_sheet* S2 = new dbmsh3d_sheet (id_);
  S2->set_sid (i_value_);
  return S2;
}

void dbmsh3d_sheet::_clone_S_C_conn (dbmsh3d_sheet* S2, dbmsh3d_hypg* HG2) const
{
  //deep-copy the halfedge[] connectivity to S2.
  assert (halfedge_);
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_edge* C2 = HG2->edgemap (HE->edge()->id());
    //create a halfedge to connect F2 and E2.
    S2->connect_bnd_E_end (C2);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);

  //deep-copy the icurve_chain_list[] connectivity.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) {
      //deep-copy the icurve_pair case.
      dbmsh3d_edge* C2 = HG2->edgemap (headHE->edge()->id());
      S2->connect_icurve_pair_E (C2);
    }
    else {
      //deep-copy the internal icurve-chain case.
      vcl_vector<dbmsh3d_edge*> icurve_loop_Es;
      dbmsh3d_halfedge* HE = headHE;
      do {
        dbmsh3d_edge* C2 = HG2->edgemap(HE->edge()->id());
        icurve_loop_Es.push_back (C2);
        HE = HE->next();
      }
      while (HE != headHE && HE != NULL);

      S2->connect_icurve_loop_Es (icurve_loop_Es);
    }
  }
}

void dbmsh3d_sheet::_clone_S_F_conn (dbmsh3d_sheet* S2, dbmsh3d_mesh* M2)
{
  //clone the fine-scale mesh face connectivity.
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    dbmsh3d_face* F2 = M2->facemap (F->id());
    S2->add_F (F2);
  }
  assert (S2->facemap().size() == facemap_.size());
}

dbmsh3d_sheet* dbmsh3d_sheet::clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2) 
{
  dbmsh3d_sheet* S2 = (dbmsh3d_sheet*) clone ();  
  _clone_S_C_conn (S2, HG2); //clone the halfedge[] and icurve_chain_list[] connectivity.
  _clone_S_F_conn (S2, M2); //clone the fine-scale mesh face connectivity.
  return S2;
}

void dbmsh3d_sheet::getInfo (vcl_ostringstream& ostrm)
{
  //not yet implemented.
  assert (0);
}

//######################################################################
//  Handle Incident Virtual Curves 
//######################################################################

//: Return the first found otherC of this sheet incident to N via F.
bool dbmsh3d_sheet::get_otherC_via_F_nv (const dbmsh3d_curve* inputC, const dbmsh3d_node* N,
                                         dbmsh3d_curve*& otherC, dbmsh3d_face*& F)
{
  dbmsh3d_edge* E = inputC->get_E_incident_N (N);
  assert (E);

  //Get the first found F of this sheet which is incident to E.
  F = get_1st_F_incident_E (E);
  if (F == NULL)
    return false;

  while (1) {      
    dbmsh3d_edge* otherE = F->find_next_bnd_E (N->V(), E);
    assert (otherE);
    otherC = (dbmsh3d_curve*) N->find_C_containing_E_nv (otherE);
    if (otherC)
      return true;
    
    //Visit the next face of this sheet incident to N
    E = otherE;
    F = E->m2_other_face (F);
  }
  assert (0);
  return false;
}

//######################################################################

bool fix_headHE_3_incidence (dbmsh3d_halfedge*& headHE)
{
  bool r = is_HE_3_incidence (headHE);
  if (r == false)
    return false;

  while (r) { //Find the next non 3-incidence HE as headHE.
    dbmsh3d_halfedge* HE = headHE;
    assert (HE->edge() == HE->next()->edge());
    HE = HE->next();
    assert (HE->edge() == HE->next()->edge());
    HE = HE->next();
    assert (HE->edge() != HE->next()->edge());
    HE = HE->next();
  
    headHE = HE;
    r = is_HE_3_incidence (headHE);
  }
  return true;
}

//: Look for the nextHE not pointing to the same edge,
//  considering the 2-incidence and 3-incidence cases.
dbmsh3d_halfedge* _find_nextE_HE (const dbmsh3d_halfedge* inputHE)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) inputHE;
  while (HE->next() != inputHE) {
    if (HE->edge() != inputHE->edge())
      return HE;
    HE = HE->next();
  }
  return NULL;
}

//: Look for the prevHE not pointing to the same edge,
//  considering the 2-incidence and 3-incidence cases.
dbmsh3d_halfedge* _find_prevE_HE (const dbmsh3d_halfedge* inputHE)
{
  vcl_vector<dbmsh3d_halfedge*> HE_vec;
  get_HEvec_next_chain (inputHE, HE_vec);

  assert (HE_vec.size() > 0);
  for (int i = HE_vec.size()-1; i>=0; i++) {
    dbmsh3d_halfedge* HE = HE_vec[i];
    if (HE->edge() != inputHE->edge())
      return HE;
  }
  return NULL;
}

//: Return true if a consecutive pair of identical HEs is found in the chain.
//   - the 3-incidence (3 consecutive edges) is not considered!
bool _detect_2_suc_Es (const dbmsh3d_halfedge* headHE)
{
  if (headHE->next() == NULL)
    return false;

  //Check no consecutive pair of i-curve halfedges.
  dbmsh3d_halfedge* HE1 = (dbmsh3d_halfedge*) headHE;  
  dbmsh3d_halfedge* HE2 = HE1->next();
  dbmsh3d_halfedge* HE3 = HE2->next();
  dbmsh3d_halfedge* HE4 = HE3->next();
  if (HE1 == HE3) {    
    assert (HE1 != HE2); //assert no duplicate HEs.
    return false; //loop of 2 HEs.
  }
  else if (HE1 == HE4) {    
    assert (HE1 != HE2 && HE2 != HE3 && HE1 != HE3); //assert no duplicate HEs.    
    if (HE1->edge() == HE2->edge() || HE2->edge() == HE3->edge() || HE1->edge() == HE3->edge()) {
      //The 3-incidence of a single loop is not allowed.
      assert (HE1->edge() != HE2->edge() || HE2->edge() != HE3->edge() || HE1->edge() != HE3->edge());
      return true; //identical HEs found
    }
    else
      return false;
  }
  do { //traverse through the circular list of halfedges
    HE2 = HE1->next();
    HE3 = HE2->next();
    HE4 = HE3->next();    
    if (HE1->edge() != HE2->edge() &&
        HE2->edge() == HE3->edge() && 
        HE3->edge() != HE4->edge()) 
      return true; //identical HEs found: HE2 and He3
    HE1 = HE2;
  }
  while (HE1 != headHE);

  return false;
}

//: Return true if [i,j] belongs to any conesecutive 3-incident edges.
bool _check_3_incidence (const vcl_vector<dbmsh3d_halfedge*>& HE_vec, 
                         const unsigned int i, const unsigned int j)
{
  unsigned int m = HE_vec.size();
  dbmsh3d_halfedge* HEi = HE_vec[i];
  dbmsh3d_halfedge* HEj = HE_vec[j];
  assert (HEi->edge() == HEj->edge());

  if (i==0 && j==m-2)
    if (HE_vec[m-1]->edge() == HEi->edge())
      return true;

  if (j==m-1 && i==1)
    if (HE_vec[0]->edge() == HEi->edge())
      return true;

  if (i+1==j) { //Consecutive (i,j)
    if (i>0)
      if (HE_vec[i-1]->edge() == HEi->edge())
        return true;
    if (j<m-1)
      if (HE_vec[j+1]->edge() == HEi->edge())
        return true;
  }

  if (j-i != 2)
    return false;

  if (HE_vec[i+1]->edge() == HEi->edge())
    return true;

  return false;
}

dbmsh3d_curve* _find_E_in_C_chain (const dbmsh3d_halfedge* headHE, dbmsh3d_edge* E)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    if (C->contain_E (E))
      return C;
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return NULL;
}

//: Return true if the headHE next loop contains self intersection.
bool loop_self_int (const dbmsh3d_halfedge* headHE)
{
  vcl_vector<dbmsh3d_halfedge*> HEvec;
  get_HEvec_next_chain (headHE, HEvec);

  vcl_set<dbmsh3d_halfedge*> HEset;
  HEset.insert (HEvec.begin(), HEvec.end());

  if (HEvec.size() == 1)
    return false;
  else {
    //Check if the headHE itself is a self-loop.
    if (HEvec[0]->edge()->is_self_loop())
      return true;
    if (HEvec.size() == 2) { //HEvec.size = 2.
      if (HEvec[1]->edge()->is_self_loop())
        return true;
    }
    else {
      //HEvec.size >= 3.
      //If any single HEvec[i] is a self_loop, return true.
      for (int i=1; i<int(HEvec.size()); i++) {      
        if (HEvec[i]->edge()->is_self_loop())
          return true;
      }

      //Check if there exists any loop of 2 edges.
      bool loop2;
      vcl_vector<dbmsh3d_halfedge*>::iterator it = HEvec.begin();
      vcl_vector<dbmsh3d_halfedge*>::iterator itn = it;
      itn++;
      for (; itn != HEvec.end(); it++, itn++) {
        //skip the possible 3-incidences.
        dbmsh3d_halfedge* HE1 = (*it);
        dbmsh3d_halfedge* HE2 = (*itn);
        if (HE1->edge() == HE2->edge())
          continue; //skip the possible 3-incidences.
        if (is_HE_3_incidence(HE2))
          continue; //skip the possible 3-incidences.
        if (is_HE_3_incidence(_find_prev2_in_next_chain(HE1)))
          continue; //skip the possible 3-incidences.

        Es_sharing_V_check (HE1->edge(), HE2->edge(), loop2);
        if (loop2)
          return true;
      }

      //Check the general intersection of 3 more edges.
      //Assumption: HEvec[0] not 3-incidence.
      assert (is_HE_3_incidence (HEvec[0]) == false); 
      dbmsh3d_vertex* startV = Es_sharing_V_check (HEvec[0]->edge(), HEvec[1]->edge(), loop2);
      //In the special case of a loop of 4 HE's : one HE and another 3-inc HE, loop2 is true but ok!

      //Trace the HE loop in the order of startV -> HEvec[1] -> ... -> HEvec[0].
      dbmsh3d_halfedge* HE = HEvec[1];
      dbmsh3d_vertex* eV = startV;
      do {
        //Only check the last HE of the 3-incidence.
        if (is_HE_3_incidence (HE)) {
          HE = HE->next()->next();
          continue;
        }

        dbmsh3d_halfedge* HEnn2;
        dbmsh3d_halfedge* nextHE = _find_nextHE_in_loop_3inc (HE, eV, HEvec, HEnn2);
        if (HEnn2) 
          return true; //self-intersection found at eV.

        HE = nextHE;
        eV = HE->edge()->other_V (eV);
      }
      while (HE != HEvec[1]);
    }
  }

  return false;
}

//: Given the inputHE and eV, find the next HE in the HE_loop_vec[].
//  Return true if exactly one such nextHE is found.
bool _find_nextHE_in_loop (const dbmsh3d_halfedge* inputHE, const dbmsh3d_vertex* eV, 
                           const vcl_vector<dbmsh3d_halfedge*>& HE_loop_vec,
                           dbmsh3d_halfedge*& nextHE)
{
  nextHE = NULL;
  //Search for the next halfedge that's not the inputHE  
  for (unsigned int i=0; i<HE_loop_vec.size(); i++) {
    dbmsh3d_halfedge* HE = HE_loop_vec[i];
    if (HE == inputHE)
      continue; //Skip if HE is the inputHE
    if (HE->edge()->is_V_incident (eV)) {
      if (nextHE != NULL)
        return false; //multiple nextHE found
      nextHE = HE;
    }
  }
  assert (nextHE);
  return true;
}

//: Given the inputHE and eV, find the next HE in the HE_loop_vec[], considering 
//  the case of 3-incidence (should skip the 3-incidence).
//  Return the nextHE, and return the additional HEnn2 if it is found.
dbmsh3d_halfedge* _find_nextHE_in_loop_3inc (const dbmsh3d_halfedge* inputHE, 
                                             const dbmsh3d_vertex* eV, 
                                             const vcl_vector<dbmsh3d_halfedge*>& HEvec,
                                             dbmsh3d_halfedge*& HEnn2)
{
  dbmsh3d_halfedge* HEnn = NULL;
  HEnn2 = NULL;

  for (unsigned int i=0; i<HEvec.size(); i++) {
    dbmsh3d_halfedge* HE = HEvec[i];
    if (HE->edge() == inputHE->edge())
      continue; //Skip if HE.E is the inputHE.E (works for the 3-incidence case).

    if (HE->edge()->is_V_incident (eV)) {
      if (HEnn != NULL) {
        HEnn2 = HE; //multiple HEnn found
        return HEnn;
      }
      HEnn = HE;
      ///HEset.erase (HEnn);
    }
    
    //If HE is 3-incidence, skip other 3-incidence HE's.
    if (is_HE_3_incidence (HE))
      i+=2; //Skip the 3-incidence HEs.
  }

  assert (HEnn);
  return HEnn;
}

dbmsh3d_halfedge* _find_lastHE_in_chain (const dbmsh3d_halfedge* startHE, const dbmsh3d_vertex* startV, 
                                         const dbmsh3d_vertex* endV, const vcl_vector<dbmsh3d_halfedge*>& HEvec)
{ 
  //once a halfedge HE is used, remove it from HEset to avoid multiple intersection during tracing.
  vcl_vector<dbmsh3d_halfedge*> HEvec2;
  HEvec2.insert (HEvec2.begin(), HEvec.begin(), HEvec.end());

  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) startHE;
  dbmsh3d_vertex* eV = (dbmsh3d_vertex*) startV;    
  do {
    if (eV == (dbmsh3d_vertex*) endV)
      return HE;

    //Only check the last HE of the 3-incidence.
    if (is_HE_3_incidence (HE)) {
      HE = HE->next()->next();
      continue;
    }

    dbmsh3d_halfedge* HEnn2 = NULL;
    dbmsh3d_halfedge* HEnn = _find_nextHE_in_loop_3inc (HE, eV, HEvec2, HEnn2);
    //erase HEnn from HEvec2.
    vcl_vector<dbmsh3d_halfedge*>::iterator it = HEvec2.begin();
    for (; it != HEvec2.end(); it++) {
      if ((*it) == HEnn) {
        HEvec2.erase (it);
        break;
      }
    }

    HE = HEnn;
    eV = HE->edge()->other_V (eV);
  }
  while (HE != startHE);

  return NULL;
}

bool _check_loop_of_2_HEs (const dbmsh3d_halfedge* headHE)
{
  dbmsh3d_halfedge* nextHE = headHE->next();
  bool loop;
  Es_sharing_V_check (headHE->edge(), nextHE->edge(), loop);
  if (loop == false)
    return false; //Loop topology error.
  return true;
}

bool _find_in_chain (const dbmsh3d_halfedge* headHE,
                     const dbmsh3d_vertex* sV, const dbmsh3d_vertex* eV, 
                     vcl_vector<dbmsh3d_halfedge*>& HEvec)
{
  assert (HEvec.empty());
  bool b_sV_found = false;
  bool b_eV_found = false;
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_edge* E = HE->edge();

    if (b_sV_found || b_eV_found)
      HEvec.push_back (HE);

    if (E->is_V_incident (sV))
      b_sV_found = true;
    if (E->is_V_incident (eV))
      b_eV_found = true;

    if (b_sV_found && b_eV_found) //done.
      return true;
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return false;
}

//: Go through each icurve HE and modify the face pointer.
void _modify_F_icurve_chain (dbmsh3d_halfedge* headHE, dbmsh3d_face* F)
{
  assert (headHE);
  if (headHE->next() == NULL) {
    headHE->set_face (F);
    return;
  }

  dbmsh3d_halfedge* HE = headHE;
  do {
    HE->set_face (F);
    HE = HE->next();
  }
  while (HE != headHE);
}

//: Make sure that the loop_bnd_Es[] are problem free (for debugging).
bool _verify_loop_bnd_Es (const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es)
{
  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "\t loop edges: ");  
  #endif
  if (loop_bnd_Es.size() == 0) {
    assert (0);
    return false;
  }

  if (loop_bnd_Es.size() == 1) {
    dbmsh3d_edge* E = loop_bnd_Es[0];
    dbmsh3d_vertex* V = E->vertices(0);
    #if DBMSH3D_DEBUG > 3
    vul_printf (vcl_cout, "E%d V%d E%d", E->id(), V->id(), E->id());
    #endif
    if (E->is_self_loop() == false) {
      assert (0);
      return false;
    }
  }
  else if (loop_bnd_Es.size() == 2) {
    //The rare case of 2 edges in a loop.
    dbmsh3d_edge* E1 = loop_bnd_Es[0];
    dbmsh3d_edge* E2 = loop_bnd_Es[1];
    dbmsh3d_vertex* V1 = E1->vertices(0);
    dbmsh3d_vertex* V2 = E1->vertices(1);
    #if DBMSH3D_DEBUG > 3
    vul_printf (vcl_cout, "E%d V%d E%d V%d E%d", E1->id(), V1->id(), E2->id(), V2->id(), E1->id());
    #endif
    if (E2->is_V_incident (V1) == false) {
      assert (0);
      return false;
    }
    if (E1->is_V_incident (V2) == false) {
      assert (0);
      return false;
    }
  }
  else {
    unsigned int i, j;
    for (i=0; i<loop_bnd_Es.size(); i++) {
      j = (i+1) % loop_bnd_Es.size();
      dbmsh3d_edge* E1 = loop_bnd_Es[i];
      dbmsh3d_edge* E2 = loop_bnd_Es[j];
      dbmsh3d_vertex* V = Es_sharing_V (E1, E2);
      #if DBMSH3D_DEBUG > 3
      vul_printf (vcl_cout, "E%d V%d ", E1->id(), V->id());
      #endif
      if (V == NULL) {
        assert (0);
        return false;
      }
    }
    #if DBMSH3D_DEBUG > 3
    vul_printf (vcl_cout, "E%d ", loop_bnd_Es[j]->id());
    #endif
  }

  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "\n");
  #endif
  return true;
}

bool _find_V_reorder_Es (const dbmsh3d_vertex* Vc, const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es, 
                         vcl_vector<dbmsh3d_edge*>& reordered_Es)
{
  //1) The case of one single edge.
  if (loop_bnd_Es.size() == 1) {
    if (loop_bnd_Es[0]->is_V_incident (Vc) == false)
      return false;
    else {
      reordered_Es.push_back (loop_bnd_Es[0]);
      return true;
    }
  }

  //2) The case of a loop with only 2 edges.
  if (loop_bnd_Es.size() == 2) {
    if (loop_bnd_Es[0]->is_V_incident (Vc) == false)
      return false;
    else {
      reordered_Es.push_back (loop_bnd_Es[0]);
      reordered_Es.push_back (loop_bnd_Es[1]);
      return true;
    }
  }

  //3) The other cases of more than 3 edges in loop.
  bool found = false;
  int idx = -1;    
  for (int i=0; i<int(loop_bnd_Es.size()); i++) {
    int j = (i+1) % loop_bnd_Es.size();
    dbmsh3d_vertex* V = Es_sharing_V (loop_bnd_Es[i], loop_bnd_Es[j]);
    if (V == Vc) {
      idx = j;
      found = true;
      break;
    }
  }
  if (found == false)
    return false;

  assert (idx != -1);
  assert (reordered_Es.size() == 0);
  for (int i=idx; i<int(loop_bnd_Es.size()); i++)
    reordered_Es.push_back (loop_bnd_Es[i]);
  for (int i=0; i<idx; i++)
    reordered_Es.push_back (loop_bnd_Es[i]);
  return true;
}

void _get_chain_C_Evec (const dbmsh3d_halfedge* headHE, vcl_vector<dbmsh3d_edge*>& Evec)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    C->get_Evec (Evec);
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
}

void _get_chain_C_shared_Es (const dbmsh3d_halfedge* headHE, vcl_set<dbmsh3d_edge*>& shared_Es)
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbmsh3d_curve* C = (dbmsh3d_curve*) HE->edge();
    _get_C_shared_Es (C, shared_Es);
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
}

void _get_C_shared_Es (dbmsh3d_curve* C, vcl_set<dbmsh3d_edge*>& shared_Es)
{  
  for (dbmsh3d_ptr_node* cur = C->shared_E_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
    shared_Es.insert (E);
  }
}

bool _disconnect_bnd3_E_chain (dbmsh3d_halfedge*& headHE, dbmsh3d_edge* E)
{
  if (headHE == NULL)
    return false;  

  dbmsh3d_halfedge* HE = headHE;
  if (HE->next() == NULL) {
    assert (HE->edge() != E);
    return false;
  }

  //Handle the triple incidence!
  if (HE->edge() == E) {
    HE = HE->next();  
    HE = HE->next();
    HE = HE->next();
    headHE = HE;
  }

  unsigned int count = 0;
  HE = headHE;
  dbmsh3d_halfedge* nextHE;
  do {
    nextHE = HE->next();
    if (nextHE == NULL)
      break;
    if (nextHE->edge() == E) { //remove nextHE from loop
      if (HE == nextHE->next())
        HE->set_next (NULL);
      else
        HE->set_next (nextHE->next());
      E->_disconnect_HE (nextHE);
      delete nextHE;
      count++;
    }
    else
      HE = HE->next();
  }
  while (nextHE != headHE);

  fix_headHE_3_incidence (headHE);

  if (count) {
    assert (count == 3);
    return true;
  }
  else
    return false;
}

bool _prop_label_Fs_e_conn (dbmsh3d_face* seedF, vcl_set<dbmsh3d_face*>& conn_Fset)
{
  vcl_queue<dbmsh3d_face*> frontF_queue;
  frontF_queue.push (seedF);

  while (frontF_queue.size() > 0) {
    dbmsh3d_face* frontF = frontF_queue.front();
    frontF_queue.pop();    
    if (frontF->b_visited())
      continue; //A FF can be on the front for multiple times.

    conn_Fset.insert (frontF);
    frontF->set_visited (true);

    //propagate the frontF (through regular manifold fs_edge)
    dbmsh3d_halfedge* HE = frontF->halfedge();
    do {
      dbmsh3d_edge* E = HE->edge();
      if (E->b_inf()) { //Skip the FE that's marked as 'infinity'
        HE = HE->next();
        continue;
      }

      if (E->halfedge()->pair()) {  
        //Propagate to non-shared fine-scale edge with two incident faces (inside a manifold).
        if (E->e_type() == E_TYPE_MANIFOLD) {
          assert (E->halfedge()->pair()->pair() == E->halfedge());
          dbmsh3d_face* otherF = E->m2_other_face (frontF); //other_valid_FF
          assert (otherF != NULL);
          if (!otherF->b_visited())
            frontF_queue.push (otherF);
        }
        else {
          //Only exception is the 2-manifold shared or special edges.
          assert (E->e_type() == E_TYPE_SHARED || E->e_type() == E_TYPE_SPECIAL || 
                  E->halfedge()->pair()->pair() != E->halfedge());
        }
      }      
      
      HE = HE->next();
    }
    while (HE != frontF->halfedge());
  }
  return true;
}

void _prop_label_Fs_e_v_conn (dbmsh3d_face* seedF, vcl_set<dbmsh3d_face*>& Fset)
{
  assert (0);
}


