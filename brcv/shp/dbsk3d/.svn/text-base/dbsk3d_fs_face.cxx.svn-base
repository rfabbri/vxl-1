// This is dbsk3d_fs_face.cxx
//: MingChing Chang
//  Nov. 14, 2004        Creation

#include <vcl_cstring.h>
#include <vcl_sstream.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon_test.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_barycentric.h>
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

//###############################################################
//####### Graph connectivity query functions #######
//###############################################################

void dbsk3d_fs_face::get_min_max_V_time (float& min_time, float& max_time) 
{
  min_time = FLT_MAX;
  max_time = FLT_MIN;

  //Traverse through the circular list of halfedges,
  //and find the vertex incident with both HE and next_he
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbmsh3d_halfedge* next_he = HE->next();

    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) Es_sharing_V (HE->edge(), next_he->edge());
    float time = (float) vgl_distance (FV->pt(), genes_[0]->pt()); ///FV->compute_time();

    if (time < min_time)
      min_time = time;
    if (time > max_time)
      max_time = time;

    HE = HE->next();
  }
  while (HE != halfedge_);
}

float dbsk3d_fs_face::max_time ()
{
  float mint, maxt;
  get_min_max_V_time (mint, maxt);
  return maxt;
}

double dbsk3d_fs_face::compute_center_pt_time()
{
  return vgl_distance (compute_center_pt(), genes(0)->pt());
}

bool dbsk3d_fs_face::is_elongate (const float ratio) const
{
  //Compute the farthest vertex Vf from centroid Cen.
  vcl_vector<dbmsh3d_vertex*> vertices;
  get_bnd_Vs (vertices);
  vgl_point_3d<double> Cen = compute_center_pt (vertices);
  int vfi = -1;
  double df = 0;
  for (unsigned int i=0; i<vertices.size(); i++) {
    dbmsh3d_vertex* V = vertices[i];
    double d = vgl_distance (V->pt(), Cen);
    if (d > df) {
      df = d;
      vfi = i;
    }
  }
  assert (vfi != -1);

  //Use Vf to partition the convex polygon (sheet) into triangles.
  //Compute avg. compactness from these triangles.
  double avg_C = 0;
  int count = 0;
  double side[3];
  for (int i=0; i<int(vertices.size()); i++) {
    int j = (i+1) % vertices.size();
    if (vfi != i && vfi != j) {
      //edge [i, i+1] and vfi span a triangle.
      side[0] = vgl_distance (vertices[vfi]->pt(), vertices[i]->pt());
      side[1] = vgl_distance (vertices[vfi]->pt(), vertices[j]->pt());
      side[2] = vgl_distance (vertices[i]->pt(), vertices[j]->pt());      
      double area = tri_area_d (side);
      double C = tri_compactness (side, area);
      assert (C < 1);
      avg_C += C;
      count++;
    }
  }
  assert (count != 0);
  avg_C /= count;
  assert (avg_C < 1);

  //Check if triangle elongates too much (low compactness).
  if (avg_C < ratio)
    return true;
  else
    return false;
}


//: Return true if this patch-elm is with zero cost:
//    - incident to both A13 and A3 shock link elements.
//    - unbounded shock sheet (incident to inf-shock-link).
//
bool dbsk3d_fs_face::is_splice_cost_zero ()
{
  if (b_finite_ == false)
    return true; //Unbounded shock sheet.

  bool on_A13_side = false;
  bool on_A3_side = false;

  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    unsigned int n = FE->count_valid_Fs ();
    if (n == 1) {
      //Check if FE has no assigned genes.
      if (FE->have_asgn_Gs() == false)
        on_A3_side = true;
    }
    //if n==2, FE is the manifold edge.
    if (n >= 2)
      on_A13_side = true;
    HE = HE->next();
  }
  while (HE != halfedge_);

  return on_A13_side && on_A3_side;
}

//###############################################################
//####### Generator handling functions #######
//###############################################################

bool dbsk3d_fs_face::remove_G (const dbmsh3d_vertex* G)
{
  for (unsigned int i=0; i<2; i++) {
    if (genes_[i] == G) {
      genes_[i] = NULL;
      return true;
    }
  }

  vul_printf (vcl_cout, "dbsk3d_fs_face %d: ERROR in removing Gene %d\n", id_, G->id());
  assert (0);
  return false;
}

void dbsk3d_fs_face::get_incoming_Gs (vcl_set<dbmsh3d_vertex*>& incomingG)
{
  //Add this fs_face's G0 and G1 to incomingG.
  incomingG.insert (genes_[0]);
  incomingG.insert (genes_[1]);
  
  if (flow_type_ != FF_FT_II_A13_3) 
    return; //Skip the type I and III fs_face.

  //Find the incoming fs_edge iFE containing the A13-3 relay.
  //iFE is the bi-directional fs_edges with smallest A13-3 radius.
  dbsk3d_fs_edge* iFE = type_II_find_incoming_FE ();
  assert (iFE->flow_type() == FE_FT_II_A13_3_OBT);

  //Add all iFE's asgn_genes to incomingG
  for (dbmsh3d_ptr_node* cur = iFE->asgn_G_list(); cur != NULL; cur = cur->next()) {
    dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
    incomingG.insert (G);
  }
}

//###############################################################
//####### Flow type computation functions #######  
//###############################################################

//: Return true if the shock sheet element contains the A12-2 source point.
bool dbsk3d_fs_face::contain_A12_2 ()
{
  vcl_vector<dbmsh3d_vertex*> vertices;
  get_bnd_Vs (vertices);
  return contain_A12_2 (vertices);
}

//: Return true if the shock sheet element contains the A12-2 source point.
bool dbsk3d_fs_face::contain_A12_2 (const vcl_vector<dbmsh3d_vertex*>& vertices)
{
  //Get the projected 2D polygon of this polygon.
  vcl_vector<double> xs, ys;  
  get_2d_polygon (vertices, xs, ys);

  //Project the mid point M to the 2D polygon.     
  vgl_vector_3d<double> N, AX, AY;
  get_2d_coord (vertices, N, AX, AY);
  vgl_point_3d<double> M = mid_pt();
  dbmsh3d_vertex* A = vertices[0];
  vgl_point_2d<double> M2 = get_2d_proj_pt (M, A->pt(), AX, AY);

  //Test if M2 is inside the 2D polygon of shock sheet.
  int size = xs.size();
  double* xarray = new double[size];
  double* yarray = new double[size];
  for (unsigned int i=0; i<xs.size(); i++)
    xarray[i] = xs[i];
  for (unsigned int i=0; i<ys.size(); i++)
    yarray[i] = ys[i];

  bool A12_2_inside = vgl_polygon_test_inside (xarray, yarray, size, M2.x(), M2.y());
  if (A12_2_inside)
    flow_type_ = FF_FT_I_A12_2;

  delete xarray;
  delete yarray;
  return A12_2_inside;
}

//: Detect the flow type of this fs_face.
//    1. Check if the A12-2 mid-point is inside the sheet polygon.
//       If so, return FF_FT_I_A12_2.
//       Otherwise, the smallest radius pt S is on sheet boundary.
//    2. If S is on a fs_edge of type II, return FF_FT_II_A13_3.
//       Otherwise, S is on a fs_vertex, return FF_FT_III_A14_2.
//
//  For type II fs_face, return the incoming fs_edge.
//  For type III fs_face, return the incoming fs_vertex.
//
FF_FLOW_TYPE dbsk3d_fs_face::detect_flow_type (vispt_elm*& elm)
{
  const dbmsh3d_vertex* G0 = genes_[0];

  vcl_vector<dbmsh3d_vertex*> vertices;
  get_bnd_Vs (vertices);

  //If the A12-2 mid-point is inside the sheet polygon, return FF_FT_I_A12_2.
  if (contain_A12_2 (vertices)) {
    assert (flow_type_ == FF_FT_I_A12_2);
    return flow_type_;
  }
  
  //Loop through all incident fs_edges and 
  //look for the smallest radius pt SL if it is an A13-3 relay (none-A14 node).    
  double radius_SL = DBL_MAX;
  dbsk3d_fs_edge* iFE = NULL;
  dbmsh3d_halfedge* HE = halfedge_;  
  assert (HE->next());
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();    
    vgl_point_3d<double> C;
    if (FE->compute_circum_cen (C)) {
      if (FE->is_flow_bidirect(C) == 2) {
        double r = vgl_distance (C, G0->pt());
        if (r < radius_SL) {
          radius_SL = r;
          iFE = FE;
        }
      }
    }    
    HE = HE->next();
  }
  while (HE != halfedge_);
  
  //Loop through all incident fs_vertices and 
  //find the smallest radius pt SN.
  double radius_SN = DBL_MAX;
  dbsk3d_fs_vertex* Ni = NULL;
  for (unsigned int i=0; i<vertices.size(); i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) vertices[i];
    double r = vgl_distance (G0->pt(), FV->pt());
    if (r < radius_SN) {
      radius_SN = r;
      Ni = FV;
    }
  }

  if (radius_SL < radius_SN) {
    flow_type_ = FF_FT_II_A13_3;
    elm = iFE;
  }
  else {
    flow_type_ = FF_FT_III_A14_2;
    elm = Ni;
  }
  return flow_type_;
}

FF_FLOW_TYPE dbsk3d_fs_face::detect_flow_type ()
{
  vispt_elm* elm;
  return detect_flow_type (elm);
}

//For flow_type_ II A12 shock sheet element,
//find the incoming fs_edge iFE containing the A13-3 relay.
//iFE is the bi-directional fs_edges with smallest A13-3 radius.
dbsk3d_fs_edge* dbsk3d_fs_face::type_II_find_incoming_FE ()
{
  vispt_elm* elm;
  detect_flow_type (elm);
  assert (flow_type_ == FF_FT_II_A13_3);
  dbsk3d_fs_edge* iFE = (dbsk3d_fs_edge*) elm;
  bool result = iFE->is_flow_bidirect() != 0;
  assert (result);
  return iFE;
}

//###############################################################
//####### Surface meshing functions #######
//###############################################################

//: Return true if one of this FF's incident fs_edge is of type A3.
bool dbsk3d_fs_face::is_on_shock_bnd () const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
  
    if (FE->count_valid_Fs()==1)
      return true;
    HE = HE->next();
  }
  while (HE != halfedge_);

  return false;
}

const unsigned int dbsk3d_fs_face::count_dual_surf_DT() const
{
  unsigned int count = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
  
    if (FE->bnd_face())
      count++;
    HE = HE->next();
  }
  while (HE != halfedge_);

  return count;
}

const bool dbsk3d_fs_face::not_meshed() const
{
  unsigned int count = count_dual_surf_DT();
  return count==0;
}

const bool dbsk3d_fs_face::one_side_meshed() const
{
  unsigned int count = count_dual_surf_DT();
  return count==1;
}

const bool dbsk3d_fs_face::both_sides_meshed() const
{
  unsigned int count = count_dual_surf_DT();
  return count>1;
}

//: return true if this FF has 2 more existing shock links being meshed.
//  also return the shock links in L1 and L2.
const bool dbsk3d_fs_face::both_sides_meshed (dbsk3d_fs_edge** L1, 
                                                   dbsk3d_fs_edge** L2) const
{
  *L1 = NULL;
  *L2 = NULL;  
  unsigned int count = 0;
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();  
    if (FE->bnd_face()) {
      if (*L1 == NULL)
        *L1 = FE;
      else if (*L2 == NULL)
        *L2 = FE;
      count++;
    }
    HE = HE->next();
  }
  while (HE != halfedge_);
  return count>1;
}

void dbsk3d_fs_face::get_FEs_with_bnd_F (vcl_set<dbsk3d_fs_edge*>& FE_with_bnd_F_set) const
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();  
    if (FE->bnd_face())
      FE_with_bnd_F_set.insert (FE);
    HE = HE->next();
  }
  while (HE != halfedge_);
}

//: Add the boundary fs_edges associated with genes (if any) to the set.
void dbsk3d_fs_face::get_bnd_FE_FV_with_Gs (vcl_set<dbsk3d_fs_edge*>& FE_with_G_set,
                                            vcl_set<dbsk3d_fs_vertex*>& FV_with_G_set) const 
{
  dbmsh3d_halfedge* HE = halfedge_;
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();  
    if (FE->have_asgn_Gs())
      FE_with_G_set.insert (FE);
    if (FE->s_FV()->have_asgn_Gs())
      FV_with_G_set.insert (FE->s_FV());
    if (FE->e_FV()->have_asgn_Gs())
      FV_with_G_set.insert (FE->e_FV());
    HE = HE->next();
  }
  while (HE != halfedge_);
}

//###############################################################
//####### Reconstruction Functions #######
//###############################################################

vgl_vector_3d<double> dbsk3d_fs_face::get_shock_flow (const vgl_point_3d<double>& pt)
{
  vgl_point_3d<double> mid = mid_pt();
  vgl_vector_3d<double> dir = pt-mid;  
  vgl_point_3d<double> gene = genes(0)->pt();  
  vgl_vector_3d<double> hyp = gene-pt;  
  hyp = hyp*(1.0 / dbgl_dist_3d (hyp.x(), hyp.y(), hyp.z(), 0, 0, 0 ));
  dir = dir*(1.0 / dbgl_dist_3d (dir.x(), dir.y(), dir.z(), 0, 0, 0 ));
  double cosine = dir.x()*hyp.x() + dir.y()*hyp.y() + dir.z()*hyp.z(); 
  dir = dir*(-1/cosine);  
  return dir;
}

//: should compute theta for a given point.
//: \todo not numerically efficient!!
double dbsk3d_fs_face::compute_theta (const vgl_point_3d<double>& pt)
{
  vgl_point_3d<double> gene1 = genes(0)->pt();
  vgl_vector_3d<double> Ga = gene1 - pt;
  vgl_vector_3d<double> flow = get_shock_flow (pt);
  normalize(flow);
  double cosTheta = dot_product( flow, Ga ) / (Ga-flow).length();
  double theta = acos( cosTheta );
  return theta;
}

void dbsk3d_fs_face::reconstruct_two_gene (
                                vgl_point_3d<double>& center_pt,
                                vgl_point_3d<double>& p1, 
                                vgl_point_3d<double>& p2)
{
  vgl_point_3d<double> gene1 = genes(0)->pt();

  vgl_vector_3d<double> flow = get_shock_flow( center_pt );
  vgl_vector_3d<double> rVec = gene1-center_pt;
  double r = rVec.length();
  
  // tangent
  vgl_vector_3d<double> T = normalize(flow); 
  
  // normal
  dbmsh3d_edge* edge = (halfedge_)->edge();
  const dbmsh3d_vertex* v = edge->eV();
  vgl_vector_3d<double> cross = cross_product( (v->pt() - center_pt), flow );
  vgl_vector_3d<double> N = normalize( cross );

  double theta = compute_theta (center_pt);
  double cosTheta = cos (theta);
  double sinTheta = sin (theta);
  
  vgl_point_3d<double> base = center_pt + r*cosTheta*T;
  vgl_vector_3d<double> addSubtract = r*sinTheta*N;

  p1 = base + addSubtract;
  p2 = base - addSubtract;
}

//###############################################################
//####### Other Functions #######
//###############################################################

dbmsh3d_face* dbsk3d_fs_face::clone () const
{
  dbsk3d_fs_face* FF2 = new dbsk3d_fs_face (id_);
  //The gene info will be set in other places.  
  FF2->set_flow_type (flow_type_);
  FF2->set_valid (b_valid_);
  FF2->set_finite (b_finite_);
  return FF2;
}

void dbsk3d_fs_face::_clone_FF_G_conn (dbsk3d_fs_face* FF2, dbmsh3d_pt_set* BND2) const
{
  //deep-copy the genes[2] to FF2
  dbmsh3d_vertex* G = BND2->vertexmap (genes_[0]->id());
  FF2->set_G (0, G);
  G = BND2->vertexmap (genes_[1]->id());
  FF2->set_G (1, G);
}

dbsk3d_fs_face* dbsk3d_fs_face::clone (dbmsh3d_mesh* M2, dbmsh3d_pt_set* BND2) const
{
  dbsk3d_fs_face* FF2 = (dbsk3d_fs_face*) clone ();
  _clone_F_E_conn (FF2, M2);
  _clone_FF_G_conn (FF2, BND2);
  return FF2;
}

void dbsk3d_fs_face::getInfo (vcl_ostringstream& ostrm) 
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "dbsk3d_fs_face id: %d Gene (%d-%d)    ", 
               id_, genes_[0]->id(), genes_[1]->id()); ostrm<<s;
  bool result = check_integrity();
  vcl_sprintf (s, "check_integrity: %s\n\n", result ? "pass." : "fail!"); ostrm<<s;

  vcl_sprintf (s, "%s, %s, sheet id %d,\n",
               b_valid() ? "Valid" : "Invalid", 
               b_finite() ? "Finite" : "Unbounded",
               i_value_); ostrm<<s;
  vgl_point_3d<double> mid = mid_pt ();
  double time = mid_pt_time ();
  vcl_sprintf (s, "mid_pt (%.10f %.10f %.10f), mid_pt time: %lf\n",
               mid.x(), mid.y(), mid.z(), time); ostrm<<s;
  
  //incident link-elms in order via halfedges
  unsigned int n_sides = n_bnd_Es ();
  vcl_sprintf (s, "\n %u fs_edges: ", n_sides); ostrm<<s;

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

  //incident fs_vertices in order
  vcl_sprintf (s, "\n %d fs_vertices: ", n_sides); ostrm<<s;
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
      dbmsh3d_halfedge* next_he = HE->next();
      dbmsh3d_vertex* V = Es_sharing_V (HE->edge(), next_he->edge());
      vcl_sprintf (s, "%d ", V->id()); ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_);
  }

  vcl_sprintf (s, "\n"); ostrm<<s;
}

//###############################################################
//####### dbsk3d_fs_face TEXT FILE I/O FUNCTIONS #######
//###############################################################

void ff_save_text_file (vcl_FILE* fp, const dbsk3d_fs_face* FF)
{
  char valid = '?';
  if (FF->b_valid())
    valid = 'V';
  else
    valid = 'I';

  char finite = '?';
  if (FF->b_finite())
    finite = 'F';
  else
    finite = 'I';

  vcl_fprintf (fp, "f %d (%d-%d) %c %c\n", 
               FF->id(), FF->genes(0)->id(), FF->genes(1)->id(), valid, finite);

  //Incident boundary fs_edges
  int nL = FF->n_bnd_Es ();
  vcl_fprintf (fp, "\t%u:", nL);
  dbmsh3d_halfedge* HE = FF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
    vcl_fprintf (fp, " %d", FE->id());
    HE = HE->next();
  }
  while (HE != FF->halfedge() && HE != NULL);

  vcl_fprintf(fp, "\n");
}

void ff_load_text_file (vcl_FILE* fp, dbsk3d_fs_face* FF, 
                        vcl_map<int, dbmsh3d_edge*>& edgemap,
                        vcl_map<int, dbmsh3d_vertex*>& gene_set)
{
  //For each dbsk3d_fs_face
  int   id;
  int   geneIds[2];
  char  valid, finite;
  vcl_fscanf (fp, "f %d (%d-%d) %c %c\n", 
              &id, &geneIds[0], &geneIds[1], &valid, &finite);
  FF->set_id (id);
  for (unsigned int j=0; j<2; j++) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = gene_set.find (geneIds[j]);
    if (it != gene_set.end()) {
      dbmsh3d_vertex* G = (*it).second;
      FF->set_G (j, G);
    }
  }

  if (valid == 'V')
    FF->set_valid (true);
  else {
    assert (valid == 'I');
    FF->set_valid (false);
  }

  if (finite == 'F')
    FF->set_finite (true);
  else {
    assert (finite == 'I');
    FF->set_finite (false);
  }

  //Inident boundary fs_edges
  unsigned int nL;
  vcl_fscanf (fp, "\t%u:", &nL);
  for (unsigned int j=0; j<nL; j++) {
    vcl_fscanf (fp, " %d", &id);

    dbsk3d_fs_edge* FE = NULL;
    vcl_map<int, dbmsh3d_edge*>::iterator lit = edgemap.find (id);
    if (lit != edgemap.end()) {
      FE = (dbsk3d_fs_edge*) (*lit).second;
      //Connect the face to the edge through the half-edge
      FF->connect_bnd_E_end (FE);
    }
  }
  vcl_fscanf(fp, "\n");
}
