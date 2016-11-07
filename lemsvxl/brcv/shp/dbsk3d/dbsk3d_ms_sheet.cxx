//: This is dbsk3d_scaffold_element_sheet.cxx
//  MingChing Chang
//  Nov 23, 2004.

#include <vcl_cstdio.h>
#include <vcl_algorithm.h>
#include <vcl_queue.h>
#include <vcl_set.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/dbsk3d_ms_sheet.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

bool dbsk3d_ms_sheet::has_incident_A3rib () const
{
  //incomplete!
  return has_bnd_A3rib ();
}

bool dbsk3d_ms_sheet::has_bnd_A3rib () const
{
  //Go through the bnd_curve list and see if there is any A3rib.
  //No need to check the internal_curves (can't be A3).
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_RIB)
      return true;
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;
}

void dbsk3d_ms_sheet::get_incident_A3ribs (vcl_set<dbmsh3d_edge*>& A3_MCs) const
{
  //incomplete!
  get_bnd_A3ribs (A3_MCs);
}

void dbsk3d_ms_sheet::get_bnd_A3ribs (vcl_set<dbmsh3d_edge*>& A3_MCs) const
{
  //Go through the bnd_curve list and see if there is any A3rib.
  //No need to check the internal_curves (can't be A3).
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_RIB)
      A3_MCs.insert (MC);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);
}

void dbsk3d_ms_sheet::get_incident_FEs (vcl_set<dbmsh3d_edge*>& incident_FE_set) const
{
  //1) Add all fs_vertices of the bnd_curve
  dbmsh3d_halfedge* HE = halfedge_;
  do { //traverse through the circular list of halfedges
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    MC->get_Eset (incident_FE_set);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);

  //2) Add all fs_vertices of each icurve_head
  //   each icurve chain is a pair of halfedges pointing to the same edge
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    HE = headHE;
    do { //traverse through the circular list of halfedges
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      MC->get_Eset (incident_FE_set);
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
  }
}

void dbsk3d_ms_sheet::get_incident_FVs (vcl_set<dbmsh3d_vertex*>& incident_Vs) const
{
  //1) Add all fs_vertices of the bnd_curve
  dbmsh3d_halfedge* HE = halfedge_;
  do { //traverse through the circular list of halfedges
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    MC->get_V_set (incident_Vs);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);

  //2) Add all fs_vertices of each icurve_head
  //   each icurve chain is a pair of halfedges pointing to the same edge
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    HE = headHE;
    do { //traverse through the circular list of halfedges
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      MC->get_V_set (incident_Vs);
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
  }
}

//: Get all associated generators of this MS.
//  If remove_A12_A3_Gs is specified, remove the generators of A12 and A3 elms.
int dbsk3d_ms_sheet::get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_Gs,
                                  const bool remove_A12_A3_Gs)
{
  int count = 0;
  vcl_set<void*> asgn_genes;

  //1) Put int all assigned genes of each fs_face.
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    dbmsh3d_vertex* G = FF->genes(0);
    asso_Gs.insert (vcl_pair<int, dbmsh3d_vertex*>(G->id(), G));
    G = FF->genes(1);
    asso_Gs.insert (vcl_pair<int, dbmsh3d_vertex*>(G->id(), G));

    //Also put possible assisnged genes of fs_edges and fs_vertices.
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();  
      if (FE->have_asgn_Gs()) {
        FE->get_asgn_Gs (asgn_genes);
        if (remove_A12_A3_Gs) {
          if (FE->e_type() == E_TYPE_RIB || FE->e_type() == E_TYPE_MANIFOLD)
            FE->clear_asgn_G_list();
        }
      }
      if (FE->s_FV()->have_asgn_Gs()) {
        dbsk3d_fs_vertex* FV = FE->s_FV();
        FV->get_asgn_Gs (asgn_genes);
        if (remove_A12_A3_Gs) {
          if (FV->v_type() == V_TYPE_RIB || FV->v_type() == V_TYPE_MANIFOLD)
            FV->clear_asgn_G_list();
        }
      }
      if (FE->e_FV()->have_asgn_Gs()) {
        dbsk3d_fs_vertex* FV = FE->e_FV();
        FV->get_asgn_Gs (asgn_genes);
        if (remove_A12_A3_Gs) {
          if (FV->v_type() == V_TYPE_RIB || FV->v_type() == V_TYPE_MANIFOLD)
            FV->clear_asgn_G_list();
        }
      }
      HE = HE->next();
    }
    while (HE != FF->halfedge());    
  }

  //2) Put in all assigned genes of each FE and FV.
  vcl_set<void*>::iterator it = asgn_genes.begin();
  for (; it != asgn_genes.end(); it++) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) (*it);
    asso_Gs.insert (vcl_pair<int, dbmsh3d_vertex*>(G->id(), G));
  }

  return int (asso_Gs.size());
}


//: Get the associated boundary mesh faces (triangles) of this fs_sheet.
void dbsk3d_ms_sheet::get_bnd_mesh_Fs (vcl_set<dbmsh3d_vertex*>& Gset, vcl_set<dbmsh3d_face*>& Gfaces,
                                       vcl_set<dbmsh3d_face*>& Gfaces2, vcl_set<dbmsh3d_face*>& Gfaces1)
{
  vcl_vector<dbsk3d_fs_face*> fs_faces;
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    fs_faces.push_back (FF);
  }
  FFs_get_bnd_mesh_Fs (fs_faces, Gset, Gfaces, Gfaces2, Gfaces1);
}

//: Get the set of incident axials of this ms_sheet.
void dbsk3d_ms_sheet::get_axial (vcl_set<dbmsh3d_edge*>& axial_set) const
{
  vcl_vector<dbsk3d_ms_curve*> axial_vec;
  get_axial_bnd (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++)
    axial_set.insert (axial_vec[i]);

  axial_vec.clear();
  get_axial_icurve (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++)
    axial_set.insert (axial_vec[i]);
}

//: Get the set of boundary axials of this ms_sheet.
int dbsk3d_ms_sheet::get_axial_bnd (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const
{
  assert (axial_vec.empty());
  return _get_axial_chain (halfedge_, axial_vec);
}

//: Get the set of i-curve (pairs and loops) axials of this ms_sheet.
int dbsk3d_ms_sheet::get_axial_icurve (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const
{
  int count = 0;
  assert (axial_vec.empty());
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) { //i-curve pair.
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) headHE->edge();
      if (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL) {
        axial_vec.push_back (MC);
        count++;
      }
    }
    else //for i-curve loop.
      count += _get_axial_chain (headHE, axial_vec);
  }
  return count;
}

//: Get the set of non-swallowtail incident axials of this ms_sheet.
void dbsk3d_ms_sheet::get_axial_nonsw (vcl_set<dbmsh3d_edge*>& axial_set) const
{
  vcl_vector<dbsk3d_ms_curve*> axial_vec;
  get_axial_nonsw_bnd (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++)
    axial_set.insert (axial_vec[i]);

  axial_vec.clear();
  get_axial_nonsw_icurve (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++)
    axial_set.insert (axial_vec[i]);
}

//: Collect all boundary A13_ms_curves of MS (not including A13 swallowtails).
int dbsk3d_ms_sheet::get_axial_nonsw_bnd (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const
{
  assert (axial_vec.empty());
  return _get_axial_nonsw_chain (halfedge_, axial_vec);
}

int dbsk3d_ms_sheet::get_axial_nonsw_icurve (vcl_vector<dbsk3d_ms_curve*>& axial_vec) const
{
  int count = 0;
  assert (axial_vec.empty());
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    const dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE)) { //i-curve pair.
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) headHE->edge();
      if (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL) {
        axial_vec.push_back (MC);
        count++;
      }
    }
    else //for i-curve loop.
      count += _get_axial_nonsw_chain (headHE, axial_vec);
  }
  return count;
}

int dbsk3d_ms_sheet::_get_axial_chain (const dbmsh3d_halfedge* headHE, 
                                       vcl_vector<dbsk3d_ms_curve*>& axial_set) const
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  assert (HE);
  int count = 0;  
  do { //Traverse the bnd_chain.
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL) {      
      axial_set.push_back (MC);
      count++;
    }
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) headHE && HE != NULL);
  return count;
}

int dbsk3d_ms_sheet::_get_axial_nonsw_chain (const dbmsh3d_halfedge* headHE, 
                                             vcl_vector<dbsk3d_ms_curve*>& axial_set) const
{
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  assert (HE);
  int count = 0;

  if (HE->next() == NULL) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_AXIAL || MC->c_type() == C_TYPE_DEGE_AXIAL) {
      axial_set.push_back (MC);
      count++;
    }
    return count;
  }

  //Traverse the bnd_chain.
  //Not to include swallowtail A13 SCurves.
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_AXIAL) {
      if (MC->is_n_incident_to_one_S(3) == false) { //non A5-swallowtail.
        axial_set.push_back (MC);
        count++;
      }      
    }
    else if (MC->c_type() == C_TYPE_DEGE_AXIAL) {
      unsigned int n = MC->n_incident_Fs();
      if (MC->is_n_incident_to_one_S(n) == false) { //non A5-swallowtail.
        axial_set.push_back (MC);
        count++;
      }      
    }
    HE = HE->next();
  }
  while (HE != (dbmsh3d_halfedge*) headHE);

  return count;
}

//: Get the set of incident axials of this ms_sheet incident to N.
void dbsk3d_ms_sheet::get_axial_inc_N (const dbmsh3d_vertex* V, vcl_set<dbmsh3d_edge*>& axial_N_set) const
{
  vcl_vector<dbsk3d_ms_curve*> axial_vec;
  get_axial_bnd (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++) {
    if (axial_vec[i]->is_V_incident (V))
      axial_N_set.insert (axial_vec[i]);
  }

  axial_vec.clear();
  get_axial_icurve (axial_vec);
  for (unsigned int i=0; i<axial_vec.size(); i++) {
    if (axial_vec[i]->is_V_incident (V))
      axial_N_set.insert (axial_vec[i]);
  }
}

void dbsk3d_ms_sheet::get_bndN_inc_axial (vcl_set<dbmsh3d_vertex*>& bnd_N_axial_set) const
{
  vcl_vector<dbmsh3d_vertex*> vertices;
  get_bnd_Vs (vertices);
  for (unsigned int i=0; i<vertices.size(); i++) {
    dbmsh3d_vertex* V = vertices[i];
    vcl_set<dbmsh3d_edge*> ICpairs;
    get_ICpairs_inc_N (V, ICpairs);
    if (ICpairs.empty() == false)
      bnd_N_axial_set.insert (V);
  }
}


void dbsk3d_ms_sheet::find_MCs_sharing_E (dbsk3d_ms_curve* inputMC, 
                                          vcl_vector<dbsk3d_ms_curve*>& shared_MCs) const
{
  //Loop through all incident MCs and find the one sharing E with inputMC.
  dbmsh3d_halfedge* HE = halfedge_;
  dbmsh3d_halfedge* prevHE = NULL;
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (prevHE==NULL || prevHE->edge() != MC) { //skip the already tested MC   
      if (MC != inputMC && Cs_sharing_E (MC, inputMC))
        shared_MCs.push_back (MC);
    }
    prevHE = HE;
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != NULL);

  //Loop through all icurve MCs and find the one sharing E with inputMC.
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();

    dbmsh3d_halfedge* HE = headHE;
    dbmsh3d_halfedge* prevHE = NULL;
    do {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      if (prevHE==NULL || prevHE->edge() != MC) { //skip the already tested MC  
        if (MC != inputMC && Cs_sharing_E (MC, inputMC))
          shared_MCs.push_back (MC);
      }
      prevHE = HE;
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
  }
}

//: prune and pass associated generators to remaining shock curves.
bool dbsk3d_ms_sheet::prune_pass_Gs (vcl_set<dbmsh3d_vertex*>& asso_Gs)
{
  vul_printf (vcl_cout, "ERROR: prune_pass_Gs() not implemented!\n");
  assert (0);
  return true;
}

//: Re-organize this MS's bnd_HE_chain and icurve_HE_chain such that
//  there are no duplicate HE in bnd_HE_chain and icurve_loop_chain.
void dbsk3d_ms_sheet::canonicalization ()
{
  dbmsh3d_sheet::canonicalization ();

  //If the bnd_chain does not contain any rib, switch with any i-curve with rib curves.
  move_rib_to_bnd_chain ();

  //The check_integrity() will modify the traverse status, skip here.
  //if (check_integrity() == false) {
    //vul_printf (vcl_cout, "MS %d canonicalization() error!");
  //}
}

//: If the bnd_chain does not contain any rib, switch with any i-curve with rib curves.
//  Return true if a swapping is done.
bool dbsk3d_ms_sheet::move_rib_to_bnd_chain ()
{
  //Find an icurve loop with rib.
  dbmsh3d_ptr_node* rib_headHE_ptr = NULL;
  
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (MC_chain_contains_rib (headHE)) {
      rib_headHE_ptr = cur;
      break;
    }
  }

  if (rib_headHE_ptr) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) rib_headHE_ptr->ptr();
    unsigned int n_loop_HE = count_HEs_in_next_loop (headHE);
    unsigned int n_bnd_HE = count_HEs_in_next_loop (halfedge_);

    if (n_loop_HE > n_bnd_HE) {
      //Swap rib_headHE_ptr->ptr() and halfedge_.
      dbmsh3d_halfedge* tmp = halfedge_;
      halfedge_ = (dbmsh3d_halfedge*) rib_headHE_ptr->ptr();
      rib_headHE_ptr->set_ptr (tmp);
      return true;
    }
  }

  return false;
}

double dbsk3d_ms_sheet::compute_cost ()
{
  vcl_map<int, dbmsh3d_vertex*> asso_genes;
  cost_ = (float) get_asso_Gs (asso_genes, false);
  return cost_;
}

//: if length_ != 0, no need to re-compute.
double dbsk3d_ms_sheet::get_cost ()
{
  if (cost_ == 0)
    cost_ = (float) compute_cost ();
  return cost_;
}

//###############################################################

void dbsk3d_ms_sheet::mark_all_FFs_invalid ()
{
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
    FF->set_valid (false);
  }
}

dbmsh3d_face* dbsk3d_ms_sheet::clone () const
{
  dbsk3d_ms_sheet* MS2 = new dbsk3d_ms_sheet (id_);
  MS2->set_sid (i_value_);
  MS2->set_cost (cost_);
  return MS2;
}

dbsk3d_ms_sheet* dbsk3d_ms_sheet::clone (dbmsh3d_hypg* HG2, dbmsh3d_mesh* M2)
{
  dbsk3d_ms_sheet* MS2 = (dbsk3d_ms_sheet*) clone ();
  _clone_S_C_conn (MS2, HG2); //clone the halfedge[] and icurve_chain_list[].
  _clone_S_F_conn (MS2, M2); //clone the fine-scale mesh face connectivity.
  return MS2;
}

#define MAX_ELMS_INFO  30

void dbsk3d_ms_sheet::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_sprintf (s, "==============================\n"); ostrm<<s;
  vcl_sprintf (s, "Scaffold Sheet id: %d    ", id_); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  //boundary ms_curves
  vcl_sprintf (s, "# Boundary Scaffold Curves:"); ostrm<<s;
  dbmsh3d_halfedge* HE = halfedge_;
  if (halfedge_) {
    do {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();      
      if (MC->c_type() == C_TYPE_RIB)
        vcl_sprintf (s, " R%d", MC->id());
      else if (MC->c_type() == C_TYPE_AXIAL)
        vcl_sprintf (s, " A%d", MC->id());
      else if (MC->c_type() == C_TYPE_DEGE_AXIAL)
        vcl_sprintf (s, " D%d", MC->id());
      else {
        vcl_sprintf (s, " ERROR");
        assert (0);
      }
      ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_ && HE != NULL);
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;
  
  //interior ms_curves
  vcl_sprintf (s, "# Interior Scaffold Curves (%u chains): ", n_icurve_chains()); ostrm<<s;
  for (dbmsh3d_ptr_node* cur = icurve_chain_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    dbmsh3d_halfedge* HE = headHE;
    do {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();   
      if (MC->c_type() == C_TYPE_RIB)
        vcl_sprintf (s, " R%d", MC->id());
      else if (MC->c_type() == C_TYPE_AXIAL)
        vcl_sprintf (s, " A%d", MC->id());
      else if (MC->c_type() == C_TYPE_DEGE_AXIAL)
        vcl_sprintf (s, " D%d", MC->id());
      else {
        vcl_sprintf (s, " ERROR");
        assert (0);
      }
      ostrm<<s;
      HE = HE->next();
    }
    while (HE != headHE && HE != NULL);
    vcl_sprintf (s, ","); ostrm<<s;    
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;

  //ms_vertices
  vcl_set<dbmsh3d_vertex*> incident_Vs;
  get_incident_Vs (incident_Vs);

  vcl_sprintf (s, "# Scaffold Vertices %u:", incident_Vs.size()); ostrm<<s;
  vcl_set<dbmsh3d_vertex*>::iterator it = incident_Vs.begin();
  for (; it != incident_Vs.end(); it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*it);
    vcl_sprintf (s, " %d", MN->id()); ostrm<<s;
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;
  
  //fs_faces
  vcl_sprintf (s, "# fs_faces %u:", facemap_.size()); ostrm<<s;
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (int i=0; fit != facemap_.end(); fit++, i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    vcl_sprintf (s, " %d", FF->id()); ostrm<<s;
    if (i > MAX_ELMS_INFO) {
      vcl_sprintf (s, " ..."); ostrm<<s;
      break;
    }
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;

  //asso. generators.
  vcl_map<int, dbmsh3d_vertex*> asso_Gs;
  get_asso_Gs (asso_Gs);
  vcl_sprintf (s, "# asso_Gs %u:", asso_Gs.size()); ostrm<<s;
  vcl_map<int, dbmsh3d_vertex*>::iterator git = asso_Gs.begin();
  for (int i=0; git != asso_Gs.end(); git++, i++) {
    dbmsh3d_vertex* G = (*git).second;
    vcl_sprintf (s, " %d", G->id()); ostrm<<s;
    if (i > MAX_ELMS_INFO) {
      vcl_sprintf (s, " ...");
      break;
    }
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//###############################################################
//       dbsk3d_ms_sheet TEXT FILE I/O FUNCTIONS
//###############################################################

void ms_save_text_file (vcl_FILE* fp, dbsk3d_ms_sheet* MS)
{
  dbmsh3d_ptr_node* cur;
  vcl_fprintf (fp, " %d:\n", MS->id());

  //1) Output the boundary chain of halfedges.
  unsigned int n_bnd_he = MS->n_bnd_Es ();
  vcl_fprintf (fp, "\tbnd_curves %u:", n_bnd_he);

  //Avoid duplicate incidence of S-E by checking the pre HE.
  dbmsh3d_halfedge* preHE = NULL;
  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    const dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      
    if (MC->c_type() == C_TYPE_RIB)
      vcl_fprintf (fp, " R%d", MC->id());
    else if (MC->c_type() == C_TYPE_AXIAL)
      vcl_fprintf (fp, " A%d", MC->id());
    else if (MC->c_type() == C_TYPE_DEGE_AXIAL)
      vcl_fprintf (fp, " D%d", MC->id());
    else {
      vul_printf (vcl_cout, " ERROR");
      assert (0);
    }

    preHE = HE;
    HE = HE->next();
  }
  while (HE != MS->halfedge() && HE != NULL);
  vcl_fprintf(fp, "\n");

  //2) Output each chain of icurve (2-incident break curve).  
  unsigned int n_ICpairs = MS->n_IC_pairs();
  vcl_fprintf (fp, "\ti_curve_pairs %u:", n_ICpairs);

  unsigned int count = 0;
  for (cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE) == false)
      continue;

    const dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) headHE->edge();
    if (MC->c_type() == C_TYPE_AXIAL)
      vcl_fprintf (fp, " A%d", MC->id());
    else if (MC->c_type() == C_TYPE_DEGE_AXIAL)
      vcl_fprintf (fp, " D%d", MC->id());
    else {
      vul_printf (vcl_cout, " ERROR");
      assert (0);
    }
    count++;
  }
  vcl_fprintf(fp, "\n");
  assert (count == n_ICpairs);

  //3) Output each icurve loop (1-incident)
  unsigned int n_ICloops = MS->n_IC_loops();
  vcl_fprintf (fp, "\ti_curve_loops %u:", n_ICloops);

  count = 0;
  for (cur = MS->icurve_chain_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_halfedge* headHE = (dbmsh3d_halfedge*) cur->ptr();
    if (is_icurve_pair_HE (headHE))
      continue;

    unsigned int n_loop = count_HEs_in_next_loop (headHE);
    vcl_fprintf (fp, " %u:", n_loop);

    unsigned int count_loop = 0;
    assert (headHE != NULL);
    dbmsh3d_halfedge* HE = headHE;
    do {
      const dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      if (MC->c_type() == C_TYPE_RIB)
        vcl_fprintf (fp, " R%d", MC->id());
      else if (MC->c_type() == C_TYPE_AXIAL)
        vcl_fprintf (fp, " A%d", MC->id());
      else if (MC->c_type() == C_TYPE_DEGE_AXIAL)
        vcl_fprintf (fp, " D%d", MC->id());
      else {
        vul_printf (vcl_cout, " ERROR, ");
        assert (0);
      }
      HE = HE->next();
      count_loop++;
    }
    while (HE != headHE && HE != NULL);
    assert (count_loop == n_loop);
    count++;
  }
  vcl_fprintf(fp, "\n");
  assert (count == n_ICloops);

  //4) Output each fs_faces
  vcl_fprintf (fp, "\tface_elms %u:", MS->facemap().size());
  vcl_map<int, dbmsh3d_face*>::iterator it = MS->facemap().begin();
    for (; it != MS->facemap().end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
    vcl_fprintf (fp, " %d", FF->id());
  }
  vcl_fprintf (fp, "\n");

  //5) Write shared face elements.
  int n_shared_Fs = MS->n_shared_Fs();
  vcl_fprintf (fp, "\tshared_F %d:", n_shared_Fs);
  for (dbmsh3d_ptr_node* cur = MS->shared_F_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_face* F = (dbmsh3d_face*) cur->ptr();
    vcl_fprintf (fp, " %d", F->id());
  }
  vcl_fprintf (fp, "\n");
}

void ms_load_text_file (vcl_FILE* fp, dbsk3d_ms_sheet* MS,
                        dbsk3d_fs_mesh* fs_mesh, dbsk3d_ms_hypg* ms_hypg)
{
  int id;
  char type;
  vcl_fscanf (fp, " %d:\n", &id);
  MS->set_id (id);

  //1) Read the bounary chain of halfedges.
  unsigned int nSC;
  vcl_fscanf (fp, "\tbnd_curves %u:", &nSC);

  for (unsigned int i=0; i<nSC; i++) {
    vcl_fscanf (fp, " %c%d", &type, &id);

    //Recover the connectivity.
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap(id);
    assert (MC->c_type() == type);
    MS->connect_bnd_E_end (MC);
  }
  vcl_fscanf (fp, "\n");

  //2) Read each chain of icurve.
  unsigned int n_ICpairs;
  vcl_fscanf (fp, "\ti_curve_pairs %u:", &n_ICpairs);

  for (unsigned int i=0; i<n_ICpairs; i++) {
    vcl_fscanf (fp, " %c%d", &type, &id);

    //Recover the connectivity.
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap(id);
    assert (MC->c_type() == type);
    MS->connect_icurve_pair_E (MC);
  }
  vcl_fscanf (fp, "\n");
  
  //3) Read each icurve loop (1-incident)
  unsigned int n_ICloops;
  vcl_fscanf (fp, "\ti_curve_loops %u:", &n_ICloops);

  for (unsigned int i=0; i<n_ICloops; i++) {
    unsigned int n;
    vcl_fscanf (fp, " %u:", &n);
    vcl_vector<dbmsh3d_edge*> icurve_loop_Es;
    for (unsigned int j=0; j<n; j++) {
      vcl_fscanf (fp, " %c%d", &type, &id);
      //Recover the connectivity.
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) ms_hypg->edgemap(id);
      assert (MC->c_type() == type);
      icurve_loop_Es.push_back (MC);
    }
    MS->connect_icurve_loop_Es (icurve_loop_Es);    
  }
  vcl_fscanf (fp, "\n");
  assert (MS->n_icurve_chains() == n_ICpairs + n_ICloops);


  //4) Read each fs_faces
  int nP;
  vcl_fscanf (fp, "\tface_elms %d:", &nP);
  for (int i=0; i<nP; i++) {
    vcl_fscanf (fp, " %d", &id);
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->facemap(id);
    assert (FF->id() == id);
    MS->add_F (FF);
  }
  vcl_fscanf(fp, "\n");
  
  //5) Read shared face elements.
  int n_shared_Fs;
  vcl_fscanf (fp, "\tshared_F %d:", &n_shared_Fs);
  for (int j=0; j<n_shared_Fs; j++) {
    vcl_fscanf (fp, " %d", &id);
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->facemap(id);
    MS->add_shared_F (FF);
  }  
  vcl_fscanf (fp, "\n");

  //Avoid MS->halfedge to be on the 3-incidence edges.
  fix_headHE_3_incidence (MS->halfedge());
}

//###############################################################
//       OTHER FUNCTIONS
//###############################################################

bool MC_chain_contains_rib (const dbmsh3d_halfedge* headHE)
{
  if (headHE == NULL)
    return false;
  //Count until reaching the end
  dbmsh3d_halfedge* HE = (dbmsh3d_halfedge*) headHE;
  do {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
    if (MC->c_type() == C_TYPE_RIB)
      return true;
    HE = HE->next();
  }
  while (HE != headHE && HE != NULL);
  return false;
}

//: pick the first FE, test if the valid FF of that FE belongs to the MS.
int n_incidence_MC_MS (const dbsk3d_ms_curve* MC, dbsk3d_ms_sheet* MS)
{
  //Here we only test the scaffold_curve's first FE.
  //Should check all FE of MC.
  const dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(0);

  int n_incidence_count = 0;
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    if (FF->b_valid()) {
      if (MS->contain_F (FF->id()))
        n_incidence_count++;
    }
    HE = HE->pair();
  }
  while (HE != FE->halfedge() && HE != NULL);

  return n_incidence_count;
}

//: Merge a boundary loop (created during tracing) to the main boundary halfedge chain.
bool _insert_bnd_loop (dbsk3d_ms_sheet* MS, const vcl_vector<dbmsh3d_edge*>& loop_bnd_Es)
{
  vul_printf (vcl_cout, "    _insert_bnd_loop(): MS %d, loop_bnd_Es (%d edges).\n",
              MS->id(), loop_bnd_Es.size());
  bool result = _verify_loop_bnd_Es (loop_bnd_Es);
  assert (result);

  //Find the connecting vertex Vc.  
  //boundary edges before merging: HE - Vc - nextHE.
  //boundary edges after merging: HE - Vc - loop_bnd_Es - Vc - nextHE.
  //Assumptions: there is exactly one such Vc in S and in loop_bnd_Es.
  //Be careful on the case that either MS or the bnd_SC are a 2-edge loop.
  dbmsh3d_halfedge* HE = MS->halfedge();
  do {
    dbmsh3d_halfedge* nextHE = HE->next();
    dbmsh3d_vertex* Vc;
    if (nextHE == NULL) {
      //The special case that MS has only one boundary loop curve.
      Vc = HE->edge()->vertices(0);
    } 
    else {
      Vc = Es_sharing_V (HE->edge(), nextHE->edge());
      //No need to use Es_sharing_V_check().
    }
    if (Vc==NULL)
      vul_printf (vcl_cout, "V ERROR, ");
    assert (Vc);

    //Find Vc in loop_bnd_Es, re-order it into reordered_Es[].
    vcl_vector<dbmsh3d_edge*> reordered_Es;
    result = _find_V_reorder_Es (Vc, loop_bnd_Es, reordered_Es);

    if (result) { //If Vc is found, perform the merging of this loop.
      #if DBMSH3D_DEBUG > 3
      vul_printf (vcl_cout, "    insert edge into: V%d E%d V%d ...... V%d E%d V%d.\n\t", 
                  HE->edge()->other_V(Vc)->id(), HE->edge()->id(), Vc->id(), 
                  Vc->id(), nextHE->edge()->id(), nextHE->edge()->other_V(Vc)->id());
      #endif
      //Insert reordered_Es[].
      dbmsh3d_halfedge* iHE = HE;
      for (unsigned int i=0; i<reordered_Es.size(); i++) {   
        dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) reordered_Es[i];
        #if DBMSH3D_DEBUG > 3
        vul_printf (vcl_cout, "E%d ", MC->id());
        #endif

        assert (Es_sharing_V (iHE->edge(), MC));
        int n_incidence = n_incidence_MC_MS (MC, MS);
        if (n_incidence == 1) {        
          dbmsh3d_halfedge* he1 = new dbmsh3d_halfedge (MC, MS);
          iHE->set_next (he1); //Insert he1 after HE.          
          MC->_connect_HE_end (he1);
          iHE = he1;
        }
        else if (n_incidence == 3) {
          //This part of code is similar to connect_bnd3_E_end (MS, nextSC);
          dbmsh3d_halfedge* he1 = new dbmsh3d_halfedge (MC, MS);
          iHE->set_next (he1); //Insert he1 after HE.
          MC->_connect_HE_end (he1);
          dbmsh3d_halfedge* he2 = new dbmsh3d_halfedge (MC, MS);
          he1->set_next (he2);
          MC->_connect_HE_end (he2);
          dbmsh3d_halfedge* he3 = new dbmsh3d_halfedge (MC, MS);
          he2->set_next (he3);
          MC->_connect_HE_end (he3);
          iHE = he3;          
        }
        else {
          vul_printf (vcl_cout, "ERROR, ");
          assert (0);        
        }
      }
      iHE->set_next (nextHE);
      assert (iHE->edge()->is_V_incident (Vc));
      assert (nextHE->edge()->is_V_incident (Vc));
      #if DBMSH3D_DEBUG > 3
      vul_printf (vcl_cout, "\n");
      #endif
      return true;
    }    
    HE = HE->next();
  }
  while (HE != MS->halfedge() && HE != NULL);
  return false;
}





