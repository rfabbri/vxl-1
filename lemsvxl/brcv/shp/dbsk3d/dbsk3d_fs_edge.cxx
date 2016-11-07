// This is dbsk3d_fs_edge.cxx
//: MingChing Chang
//  Nov. 14, 2004        Creation

#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vgl/vgl_vector_3d.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_barycentric.h>
#include <dbgl/dbgl_triangle.h>
#include <dbgl/algo/dbgl_intersect.h>

#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//###############################################################
//####### Graph connectivity query functions #######
//###############################################################

const dbmsh3d_vertex* dbsk3d_fs_edge::_get_one_G_via_FF () const 
{
  if (halfedge_ == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  return FE->genes(0);
}

dbsk3d_fs_face* dbsk3d_fs_edge::other_FF_of_G (const dbsk3d_fs_face* inputFF, 
                                               const dbmsh3d_vertex* G) const
{
  if (halfedge_ == NULL)
    return NULL;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    if (FE != inputFF && (FE->genes(0) == G || FE->genes(1) == G))
      return FE;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

const dbmsh3d_vertex* dbsk3d_fs_edge::A13_opposite_G (const dbsk3d_fs_face* FE) const
{
  if (halfedge_ == NULL)
    return NULL;
  const dbmsh3d_vertex* G = FE->genes(0);
  dbsk3d_fs_face* nextP = other_FF_of_G (FE, G);

  return nextP->other_G (G);
}

//: Get ordered generators via the connecting shock patchElm information.
//  Be careful on the A3-Rib-elm case.
//  return false if this operation can not be done (after pruning).
bool dbsk3d_fs_edge::get_ordered_Gs_via_FF (vcl_vector<dbmsh3d_vertex*>& genes) const 
{
  if (halfedge_ == NULL)
    return false;
  dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  const dbmsh3d_vertex* startG = FE->genes(0);  
  dbmsh3d_vertex* G = (dbmsh3d_vertex*) startG;
  do {
    G = (dbmsh3d_vertex*) FE->other_G (G);
    genes.push_back (G);    
    FE = other_FF_of_G (FE, G); //the next fs_face that connected to G
    if (FE == NULL)
      return false;
  }
  while (G != startG && FE != NULL);
  return true;
}

bool dbsk3d_fs_edge::get_ordered_Gs_via_FF (vcl_set<dbmsh3d_vertex*>& genes) const 
{
  dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  const dbmsh3d_vertex* startG = FE->genes(0);  
  dbmsh3d_vertex* G = (dbmsh3d_vertex*) startG;
  do {
    G = (dbmsh3d_vertex*) FE->other_G (G);
    genes.insert (G);    
    FE = other_FF_of_G (FE, G); //the next fs_face that connected to G
    if (FE == NULL)
      return false;
  }
  while (G != startG && FE != NULL);
  return true;
}

void dbsk3d_fs_edge::get_3_Gs_via_FF (dbmsh3d_vertex** G0, 
                                         dbmsh3d_vertex** G1, 
                                         dbmsh3d_vertex** G2) const
{
  const dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  *G0 = (dbmsh3d_vertex*) FE->genes(0);
  *G1 = (dbmsh3d_vertex*) FE->genes(1);
  dbsk3d_fs_face* P2 = other_FF_of_G (FE, *G1);
  assert (P2);

  *G2 = P2->other_G (*G1);
}

//###############################################################
//####### Generator handling functions #######
//###############################################################

//: Go through the set of generators asso_genes[] and try to
//  add to this link if all existing fs_face of it do not contain it.
void dbsk3d_fs_edge::check_add_asgn_Gs_to_set (vcl_map<int, dbmsh3d_vertex*>& asso_genes,
                                               const dbsk3d_fs_face* fromP)
{
  vcl_map<int, dbmsh3d_vertex*>::iterator it = asso_genes.begin();
  for (; it != asso_genes.end(); it++) {
    dbmsh3d_vertex* G = (*it).second;
    if (_is_G_asgn_in_FF (G, fromP))
      continue; //skip if G is assigned with any incident FE.
    if (is_G_asgn (G))
      continue; //skip if G is assigned with FE itself already.
    add_asgn_G (G);
  }  
}

//: Go through all incident valid fs_sheet_elms except the fromP
//  and check if the input G is associated.
bool dbsk3d_fs_edge::_is_G_asgn_in_FF (const dbmsh3d_vertex* G, 
                                       const dbsk3d_fs_face* fromP) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    if (FE->b_valid() && FE != fromP)
      if (FE->genes(0) == G || FE->genes(1) == G)
        return true;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return false;  
}

bool dbsk3d_fs_edge::prune_pass_Gs_to_FV (vcl_map<int, dbmsh3d_vertex*>& asso_genes,
                                          dbsk3d_fs_vertex* FV)
{
  #if DBMSH3D_DEBUG>3
  vul_printf (vcl_cout, "\n  prune_pass_Gs_to_FV(): %d genes to A14 %d.", asso_genes.size(), FV->id());
  #endif
  
  //Loop through FV to collect all current associated G's.
  vcl_map<int, dbmsh3d_vertex*> N_asso_genes;
  FV->get_asso_Gs_excld_FE (this, N_asso_genes);

  //Loop through all associated genes and check if it is required to pass to FV
  vcl_map<int, dbmsh3d_vertex*>::iterator it = asso_genes.begin();
  for (; it != asso_genes.end(); it++) {
    dbmsh3d_vertex* G = (*it).second;
    if (N_asso_genes.find (G->id()) == N_asso_genes.end()) {    
      #if DBMSH3d_DEBUG>2
      vul_printf (vcl_cout, "\n    FV %d add_asgn_G (G %d).", FV->id(), G->id());
      #endif
      FV->add_asgn_G (G);
    }
  }  

  return true;
}

//: Get all associated generators of this FE.
void dbsk3d_fs_edge::get_asso_Gs (vcl_map<int, dbmsh3d_vertex*>& asso_genes)
{  
  //Add all assigned genes of this fs_edge.
  for (dbmsh3d_ptr_node* cur = asgn_G_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*> (G->id(), G));
  }
}

//: Get all associated generators of this FE and incident valid FE's.
void dbsk3d_fs_edge::get_asso_Gs_incld_FFs (vcl_map<int, dbmsh3d_vertex*>& asso_genes)
{  
  //Add all assigned genes of valid incident fs_faces.
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    if (FE->b_valid()) {
      asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*>(FE->genes(0)->id(), FE->genes(0)));
      asso_genes.insert (vcl_pair<int, dbmsh3d_vertex*>(FE->genes(1)->id(), FE->genes(1)));
    }

    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);

  //Add all assigned genes of this fs_edge.
  get_asso_Gs (asso_genes);
}

unsigned int dbsk3d_fs_edge::get_pass_Gs (vcl_set<dbmsh3d_vertex*>& pass_genes) const
{
  assert (pass_genes.size() == 0);
  for (dbmsh3d_ptr_node* cur = s_FV()->asgn_G_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    pass_genes.insert (G);
  }
  get_ordered_Gs_via_FF (pass_genes);
  return pass_genes.size();
}

//###############################################################
//####### Flow type computation functions #######
//###############################################################

//: Compute circum center from the associated generators.
//  Get three generator points and compute circum center from them.
//  return false if this can't be done (on boundary shock curves).
// 
bool dbsk3d_fs_edge::compute_circum_cen (vgl_point_3d<double>& C) const
{
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) halfedge_->face();
  const dbmsh3d_vertex* G0 = FE->genes(0);
  const dbmsh3d_vertex* G1 = FE->genes(1);
  dbsk3d_fs_face* P2 = other_FF_of_G (FE, G1);
  if (P2 == NULL)
    return false;

  const dbmsh3d_vertex* G2 = P2->other_G (G1);
  C = circum_center_3pts (G0->pt(), G1->pt(), G2->pt());
  return true;
}

//: get one generator from one of the connected sheet
//  and compute time
double dbsk3d_fs_edge::circum_cen_time () const
{
  vgl_point_3d<double> C;
  compute_circum_cen (C);
  return circum_cen_time (C);
}

double dbsk3d_fs_edge::circum_cen_time (const vgl_point_3d<double>& C) const
{
  const dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  const dbmsh3d_vertex* G = FE->genes(0);
  const double time = vgl_distance (C, G->pt());
  return time;
}

//: Check if the flow is bi-directional, i.e. containing the circumcenter.
//  Checking the dot products.
//  return 0: uni-directional
//         1: center C fuzzily on boundary
//         2: bi-directional
//  
int dbsk3d_fs_edge::is_flow_bidirect ()
{
  vgl_point_3d<double> C;
  bool b = compute_circum_cen (C);
  assert (b);
  return is_flow_bidirect (C);
}

int dbsk3d_fs_edge::is_flow_bidirect (const vgl_point_3d<double>& C)
{
  vgl_vector_3d<double> CS (s_FV()->pt() - C);
  vgl_vector_3d<double> CE (e_FV()->pt() - C);
  double dot = dot_product (CS, CE);
  if (dbgl_eq_m (dot, 0))
    return 1; //Case of center C fuzzily on boundary
  else if (dot > 0) {
    return 0; //Uni-directional
  }
  else
    return 2; //Bi-directional
}

//: Detect the flow type of this fs_edge
FE_FLOW_TYPE dbsk3d_fs_edge::detect_flow_type ()
{
  vgl_point_3d<double> C;
  if (compute_circum_cen (C) == false) {
    //fs_edge at infinity
    assert (b_inf() == true);
    flow_type_ = FE_FT_INF;
    return flow_type_;
  }
  else
    return detect_flow_type (C);
}

//: type 1: A13-2 point included and inside dual Delaunay triangle.
//  type 2: A13-2 point included and outside dual Delaunay triengle.
//  type 3: A13-2 point not included.
FE_FLOW_TYPE dbsk3d_fs_edge::detect_flow_type (const vgl_point_3d<double>& C)
{
  bool bi_direct = is_flow_bidirect();

  //Determine if the dual Delaunay triangle is acute or obtuse.
  //If the dual is not a triangle, 
  double max_side, perimeter;
  unsigned int nG;
  double Side[3];
  dbmsh3d_vertex* Gene[3];
  bool result = get_tri_params (max_side, perimeter, nG, Side, Gene);
  assert (result);
  bool b_acute = dual_DT_contains_A13_2 (nG, Gene);

  if (bi_direct) {
    if (b_acute)
      flow_type_ = FE_FT_I_A13_2_ACUTE; //'1';
    else
      flow_type_ = FE_FT_II_A13_3_OBT; //'2';
  }
  else {
    if (b_acute)
      flow_type_ = FE_FT_III_A14_ACUTE; //'3';
    else
      flow_type_ = FE_FT_IV_A14_OBT; //'4';
  }

  return flow_type_;
}

dbsk3d_fs_face* dbsk3d_fs_edge::type_II_get_outgoing_P () const
{
  assert (flow_type_ == FE_FT_II_A13_3_OBT);
  //Go through all fs_faces and return the one with longest GiGj.
  dbsk3d_fs_face* outgoingP = NULL;
  double maxd = DBL_MIN;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    double d = FE->dist_Gs ();
    if (d > maxd) {
      maxd = d;
      outgoingP = FE;
    }
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return outgoingP;
}

//###############################################################
//####### Surface meshing functions #######  
//###############################################################

//: Compute parameters for the corresponding surface triangle
//  return false if this operation can't be done (after pruning).
//  min_side: 
//  max_side:
//  side[0], side[1], side[2]
//  Gene[0], Gene[1], Gene[2]
bool dbsk3d_fs_edge::get_tri_params (double& max_side, double& perimeter, unsigned int& nG, 
                                         double* Side, dbmsh3d_vertex** Gene) const
{    
  //Go through all assigned generators once.
  max_side = 0;
  perimeter = 0;
  nG = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
  const dbmsh3d_vertex* startG = FE->genes(0);

  dbmsh3d_vertex* G = (dbmsh3d_vertex*) startG;
  do {
    dbmsh3d_vertex* nextG = (dbmsh3d_vertex*) FE->other_G (G);
    FE = other_FF_of_G (FE, nextG);
    if (FE == NULL)
      return false;
    
    double d = vgl_distance (G->pt(), nextG->pt()); //side length 
    if (nG < 3) {
      Gene[nG] = G;
      Side[nG] = d;
    }
    nG++;

    if (d > max_side)
      max_side = d;
    perimeter += d;
    G = nextG;
  }
  while (G != startG);
  return true;
}

double dbsk3d_fs_edge::compute_tri_compactness (const double nG, double* Side)
{  
  double compactness;
  if (nG == 3) {
    double area = tri_area_d (Side);
    compactness = tri_compactness (Side, area);
  }
  else { //for polygonal surface interpolant, set compactness to 1.0.
    compactness = 1.0;
  }
  assert (compactness != 0);
  return compactness;
}

//: Compute the min_shock_radius.
double dbsk3d_fs_edge::compute_min_radius (const int nG, dbmsh3d_vertex** G) const
{
  assert (nG >= 3);
  vgl_point_3d<double> C = circum_center_3pts (G[0]->pt(), G[1]->pt(), G[2]->pt());
  const double SC = vgl_distance (s_FV()->pt(), C);
  const double EC = vgl_distance (e_FV()->pt(), C);
  return vcl_min (SC, EC);
}

//: Return if the surface interpolant is acute or not.
bool dbsk3d_fs_edge::dual_DT_contains_A13_2 (const int nG, dbmsh3d_vertex** G) const
{
  if (nG == 3) { //test if dual Delaunay triangle is acute or not.
    if (dbgl_is_tri_non_acute (G[0]->pt(), G[1]->pt(), G[2]->pt()))
      return false;
    else
      return true;
  }
  else { //for surface polygon 
    assert (nG > 3);
    vcl_vector<dbmsh3d_vertex*> genes;
    bool result = get_ordered_Gs_via_FF (genes);
    assert (result);

    if (_dual_polygon_contains_A13_2 (genes))
      return true;
    else
      return false;
  }
}

//: return true if the surface face contains the A13-2 point.
//  shoot the ray of shock-link to intersect with the surface polygon.
bool dbsk3d_fs_edge::_dual_polygon_contains_A13_2 (const vcl_vector<dbmsh3d_vertex*>& genes) const
{    
  dbmsh3d_vertex* G0 = (dbmsh3d_vertex*) genes[0];
  double t, u, v;
  //For non-triangular face (they are convex), triangulate them and test each triangle.
  for (unsigned int i=0; i<genes.size()-2; i++) {
    dbmsh3d_vertex* G1 = (dbmsh3d_vertex*) genes[i+1];
    dbmsh3d_vertex* G2 = (dbmsh3d_vertex*) genes[i+2];
    vgl_vector_3d<double> D = e_FV()->pt() - s_FV()->pt();
    D /= D.length();

    if (dbgl_intersect_ray_tri (s_FV()->pt(), D, G0->pt(), G1->pt(), G2->pt(), t, u, v))
      return true;
  }
  return false;
}

bool dbsk3d_fs_edge::face_on_mesh_bnd () const
{
  //Loop through all incident shock sheets.
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    if (FE->count_dual_surf_DT() != 2)
      return true;
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return false;
}

//: Update the validity of each incident fs_face by checking
//  if each bnd_edge exists or not.
void dbsk3d_fs_edge::update_validity_FFs ()
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    dbmsh3d_edge* E = E_sharing_2V (FE->genes(0), FE->genes(1));
    FE->set_valid (E==NULL);
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
}

bool dbsk3d_fs_edge::check_fit_hole (const dbmsh3d_vertex* Vo, const dbmsh3d_edge* E) const
{
  vcl_vector<dbmsh3d_vertex*> genes;
  get_ordered_Gs_via_FF (genes);
  if (genes.size() != 3)
    return false; //Skip unbounded fs_edge
  
  for (unsigned int i=0; i<genes.size(); i++) {
    if (genes[i] == Vo) {
      dbmsh3d_vertex* V1 = genes[(i+1) % genes.size()];
      dbmsh3d_vertex* V2 = genes[(i+2) % genes.size()];

      if (E->vertices(0) == V1 && E->vertices(1) == V2)
        return true;
      if (E->vertices(0) == V2 && E->vertices(1) == V1)
        return true;
    }  
  }
  return false;
}

dbmsh3d_edge* dbsk3d_fs_edge::clone () const
{
  dbsk3d_fs_edge* FE2 = new dbsk3d_fs_edge (NULL, NULL, id_);
  FE2->set_bnd_face (bnd_face_);
  FE2->set_cost (cost_);
  FE2->set_s_value (s_value_);
  FE2->set_e_type (c_value_);
  FE2->set_flow_type (flow_type_);
  return FE2;
}

void dbsk3d_fs_edge::_clone_FE_G_conn (dbsk3d_fs_edge* FE2, dbmsh3d_pt_set* BND2) const
{    
  //deep-copy the gene_list to FE2.   
  vcl_vector<dbmsh3d_vertex*> G_list; 
  for (dbmsh3d_ptr_node* cur = asgn_G_list_; cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    dbmsh3d_vertex* G2 = BND2->vertexmap (G->id());
    G_list.push_back (G2);
  }
  for (int i=G_list.size()-1; i>=0; i--)
    FE2->add_asgn_G (G_list[i]);
}

dbsk3d_fs_edge* dbsk3d_fs_edge::clone (dbmsh3d_pt_set* PS2, dbmsh3d_pt_set* BND2)
{
  dbsk3d_fs_edge* FE2 = (dbsk3d_fs_edge*) clone ();
  _clone_E_V_conn (FE2, PS2);
  _clone_FE_G_conn (FE2, BND2);
  return FE2;
}

void dbsk3d_fs_edge::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbsk3d_fs_edge id: %d (s_FV %d, e_FV %d)    ", id_,
               s_FV()->id(), e_FV()->id()); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  vcl_sprintf (s, "%s, ", is_valid_via_F() ? "Valid" : "Invalid"); ostrm<<s;

  //Compute the surface triangle parameters:
  double max_side, perimeter;
  unsigned int nG;
  double Side[3];
  dbmsh3d_vertex* Gene[3];
  if (get_tri_params (max_side, perimeter, nG, Side, Gene)) {
    vcl_sprintf (s, "%s", dual_DT_contains_A13_2 (nG, Gene) ? "Acute" : "Obtuse"); ostrm<<s;
    vcl_sprintf (s, " interpolant with %u sides.\n", nG); ostrm<<s;
  }

  //Incident fs_edges
  unsigned int numP = n_incident_Fs();
  unsigned int nv = count_valid_Fs ();
  vcl_sprintf (s, "%u incident fs_faces (%u valid):", numP, nv); ostrm<<s;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_face* FE = (dbsk3d_fs_face*) HE->face();
    vcl_sprintf (s, " %d", FE->id()); ostrm<<s;
    if (FE->b_valid())
      ostrm<<"v";
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);

  vcl_sprintf (s, "\n"); ostrm<<s;

  //Ordered boundary generators via shock sheets.
  vcl_vector<dbmsh3d_vertex*> genes;
  if (get_ordered_Gs_via_FF (genes)) {
    vcl_sprintf (s, "%u Ordered Genes: ", genes.size()); ostrm<<s;
    for (unsigned int j=0; j<genes.size(); j++) {
      vcl_sprintf (s, " %d", genes[j]->id()); ostrm<<s;
    }
    vcl_sprintf (s, "\n"); ostrm<<s;
  }

  //Assigned boundary generators.
  if (have_asgn_Gs() == false) {
    vcl_sprintf (s, "0 Assigned Genes:"); ostrm<<s;
  }
  else {
    vcl_set<void*> asgn_genes;
    get_asgn_Gs (asgn_genes);    
    vcl_set<void*>::iterator vit = asgn_genes.begin();
    for (; vit != asgn_genes.end(); vit++) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*)(*vit);
      vcl_sprintf (s, " %d", G->id()); ostrm<<s;
    }
  }
  vcl_sprintf (s, "\n"); ostrm<<s;
}

//###############################################################
//       dbsk3d_ms_edge TEXT FILE I/O FUNCTIONS
//###############################################################

void fe_save_text_file (vcl_FILE* fp, dbsk3d_fs_edge* FE)
{
  //For each dbsk3d_fs_edge
  const dbsk3d_fs_vertex* s_FV = FE->s_FV();
  const dbsk3d_fs_vertex* e_FV = FE->e_FV();

  vcl_fprintf (fp, "e %d (%d %d) %c\n", 
               FE->id(), s_FV->id(), e_FV->id(), (char)FE->e_type());

  //Assigned genes
  unsigned int nG = FE->n_asgn_Gs();
  if (nG == 0) {
    vcl_fprintf (fp, "\t0:");
  }
  else {
    vcl_fprintf (fp, "\t%u:", nG);
    for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      vcl_fprintf (fp, " %d", G->id());
    }
  }
  vcl_fprintf(fp, "\n");
}

void fe_load_text_file (vcl_FILE* fp, dbsk3d_fs_edge* FE, 
                        vcl_map <int, dbmsh3d_vertex*>& vertexmap,
                        vcl_map <int, dbmsh3d_vertex*>& genemap)
{
  //For each dbsk3d_fs_edge
  int id, nodeIds[2];
  char type;
  vcl_fscanf (fp, "e %d (%d %d) %c\n", &id, &nodeIds[0], &nodeIds[1], &type);
  FE->set_id (id); //Use the id from the file
  
  for (unsigned int j=0; j<2; j++) {
    dbsk3d_fs_vertex* FV = NULL;
    vcl_map<int, dbmsh3d_vertex*>::iterator nit = vertexmap.find (nodeIds[j]);
    if (nit != vertexmap.end())
      FV = (dbsk3d_fs_vertex*) (*nit).second;

    FE->_set_vertex (j, FV);
    //Note that the order of FE's in FV can be different after load/save file!
    FV->add_incident_E (FE);
  }

  FE->set_e_type (type);

  //Read associated genes  
  unsigned int nG;
  vcl_vector<dbmsh3d_vertex*> asgn_G;
  vcl_fscanf (fp, "\t%u:", &nG);
  if (nG != 0) {
    for (unsigned int j=0; j<nG; j++) {
      int gid;
      vcl_fscanf (fp, " %d", &gid);
      dbmsh3d_vertex* G = NULL;
      vcl_map <int, dbmsh3d_vertex*>::iterator git = genemap.find (gid);
      if (git != genemap.end()) {
        G = (dbmsh3d_vertex*) (*git).second;      
        asgn_G.push_back (G);
      }
    }
  }
  vcl_fscanf(fp, "\n");

  //Note that the adding order of FV.asgn_G_ is inversed.
  for (int j=int(asgn_G.size()-1); j>=0; j--)
    FE->add_asgn_G (asgn_G[j]); //Assign generator G to shock link FE
}



