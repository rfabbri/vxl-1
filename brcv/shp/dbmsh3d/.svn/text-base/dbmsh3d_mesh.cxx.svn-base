//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_mesh.cxx
//:
// \file
// \brief Mesh
//
//
// \author
//  MingChing Chang  April 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cstdio.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_queue.h>
#include <vcl_iostream.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/dbmsh3d_mesh.h>

double dbmsh3d_ifs_mesh::get_avg_edge_len_from_F ()
{
  double d, avg_len = 0;
  unsigned int count = 0;
  vgl_point_3d<double> P0, P1;
  assert (facemap_.size() != 0);

  //Loop through each mesh face and count bnd edge len.
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    int sz = F->vertices().size();
    assert (sz > 1);
    for (int i=0; i<sz-1; i++) {
      P0 = F->vertices(i)->pt();
      P1 = F->vertices(i+1)->pt();
      d = vgl_distance (P0, P1);
      avg_len += d;
    }
    P0 = F->vertices(0)->pt();
    P1 = F->vertices(sz-1)->pt();
    d = vgl_distance (P0, P1);
    avg_len += d;
    count += sz;
  }

  assert (count != 0);
  return avg_len / count;
}

bool dbmsh3d_ifs_mesh::check_integrity ()
{
  if (dbmsh3d_pt_set::check_integrity() == false)
    return false;

  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    if (F->id() != (*it).first)
      return false;
  }

  return true;
}

dbmsh3d_pt_set* dbmsh3d_ifs_mesh::clone ()
{
  dbmsh3d_ifs_mesh* M = new dbmsh3d_ifs_mesh ();
  assert (0);
  return M;
}

// ----------------------------------------------------------------------------


//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int dbmsh3d_ifs_mesh::_count_faces_indices_ifs() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    assert (F->vertices().size() != 0);
    unsigned int nVer = F->vertices().size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
//  Skip the unmarked face with false i_visited_
unsigned int dbmsh3d_ifs_mesh::_count_visited_faces_indices_ifs() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    if (F->b_visited() == false)
      continue; //skip the unmarked F.

    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);
    unsigned int nVer = vertices.size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

//: Assign IFS_vidx using the vertex ids
//  for instance, used in the surface mesh reconstruction,
//  where the indices of the vertices is known.
void dbmsh3d_ifs_mesh::ifs_assign_Vs_vid_by_id ()
{
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    F->_ifs_assign_Vs_vid_by_id ();
  }
}

//: Assign IFS_vid using the order of the vertex in the map
// The first vertex will have vid_ = 0 and the last in the map
// will have vid_ = (num_vertices - 1);
void dbmsh3d_ifs_mesh::assign_IFS_vertex_vid_by_vertex_order()
{  
  reset_vertex_traversal();
  int vid_count = 0;
  for (dbmsh3d_vertex* vb = 0; this->next_vertex(vb); vid_count ++) {
    dbmsh3d_vertex* vertex = (vb);
    vertex->set_vid(vid_count);
  }

  return;
}

void dbmsh3d_ifs_mesh::mark_unmeshed_pts ()
{
  //Reset the unmeshed flag of each point.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    V->set_meshed (false);
  }

  //Go through all mesh faces and mark incident points.
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (*fit).second;

    for (unsigned int i=0; i<F->vertices().size(); i++) {
      dbmsh3d_vertex* V = F->vertices(i);
      V->set_meshed (true);
    }
  }
}

void dbmsh3d_ifs_mesh::delete_unmeshed_pts ()
{
  mark_unmeshed_pts ();

  //Go through each point and delete unmeshed ones.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  while (vit != vertexmap_.end()) {
    dbmsh3d_vertex* V = (*vit).second;
    if (V->b_meshed() == false) {
      vit++;
      remove_vertex (V);
    }
    else
      vit++;
  }
}

//###################################################################

double dbmsh3d_mesh::get_avg_edge_len_from_F ()
{
  if (is_MHE() == false)
    return dbmsh3d_ifs_mesh::get_avg_edge_len_from_F ();

  double avg_len = 0;
  unsigned int count = 0;
  assert (facemap_.size() != 0);

  //Loop through each mesh face and count bnd edge len.
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    assert (F->halfedge());
    assert (F->halfedge()->next());

    dbmsh3d_halfedge* HE = F->halfedge();
    do {
      avg_len += HE->edge()->length();
      count++;      
      HE = HE->next();
    }
    while (HE != F->halfedge());
  }

  assert (count != 0);
  return avg_len / count;
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int dbmsh3d_mesh::count_faces_indices() 
{
  if (is_MHE())
    return _count_faces_indices_mhe ();
  else
    return _count_faces_indices_ifs ();
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int dbmsh3d_mesh::_count_faces_indices_mhe() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    vcl_vector<dbmsh3d_vertex*> vertices;
    F->get_bnd_Vs (vertices);
    unsigned int nVer = vertices.size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

void dbmsh3d_mesh::remove_F_del_isolated_Es (dbmsh3d_face* F)
{ 
  //The list of edges to delete.
  vcl_vector<dbmsh3d_edge*> edges_to_del;
  dbmsh3d_halfedge* HE = F->halfedge();
  do {
    dbmsh3d_edge* E = HE->edge();
    if (E->halfedge()->pair() == NULL)
      edges_to_del.push_back (E);
    HE = HE->next();
  }
  while (HE != F->halfedge());

  remove_face (F);

  for (unsigned int i=0; i<edges_to_del.size(); i++) {
    remove_edge (edges_to_del[i]);
  }
}

void dbmsh3d_mesh::remove_F_complete (dbmsh3d_face* F)
{  
  vcl_vector<dbmsh3d_edge*> Evec_to_del;
  vcl_set<dbmsh3d_vertex*> Vset_to_del;

  F->get_bnd_Es (Evec_to_del);
  //Add incident V's to Vset_to_del
  for (unsigned int i=0; i<Evec_to_del.size(); i++) {
    Vset_to_del.insert (Evec_to_del[i]->vertices(0));
    Vset_to_del.insert (Evec_to_del[i]->vertices(1));
  }
  remove_face (F);

  //Go through all Evec_to_del and delete the ones without incident F's.
  for (unsigned int i=0; i<Evec_to_del.size(); i++) {
    dbmsh3d_edge* E = Evec_to_del[i];
    if (E->halfedge() == NULL) 
      remove_edge (E); //delete E
  }
  Evec_to_del.clear();

  //Go through all Vset_to_del and delete the ones without incident E's.
  vcl_set<dbmsh3d_vertex*>::iterator vit = Vset_to_del.begin();
  while (vit != Vset_to_del.end()) {
    dbmsh3d_vertex* V = (*vit);
    Vset_to_del.erase (vit);

    if (V->have_incident_Es() == NULL) 
      remove_vertex (V); //delete V
    vit = Vset_to_del.begin();
  }
}

void dbmsh3d_mesh::remove_E_complete (dbmsh3d_edge* E)
{
  assert (E->halfedge() == NULL);
  dbmsh3d_vertex* sV = E->sV();
  dbmsh3d_vertex* eV = E->eV();
  remove_edge(E);

  if (sV->have_incident_Es() == NULL)
    remove_vertex (sV);
  if (eV->have_incident_Es() == NULL)
    remove_vertex (eV);
}

//: Create and add a mesh face from a given set of ordered edges.
dbmsh3d_face* dbmsh3d_mesh::add_new_face (const vcl_vector<dbmsh3d_edge*>& ordered_edges)
{
  //Create and add the new face to the structure.
  dbmsh3d_face* F = _new_face();  
  _add_face (F);

  //Create the first halfedge of incidence relationship
  assert (ordered_edges.size() > 2);
  dbmsh3d_halfedge* HE = new dbmsh3d_halfedge (ordered_edges[0], F);
  F->set_halfedge (HE);  
  ordered_edges[0]->_connect_HE_end (HE); //add the HE to the edge's pair_ structure.

  //for each next_edge, create a halfedge and add the incidence connectivity.
  for (unsigned int i=1; i<ordered_edges.size(); i++) {
    dbmsh3d_edge* e = ordered_edges[i];
    dbmsh3d_halfedge* nextHE = new dbmsh3d_halfedge (e, F);

    //add the nextHE to the edge's pair_ structure.
    e->_connect_HE_end (nextHE);

    //add to the circular list of halfedges.
    HE->set_next (nextHE);
    HE = nextHE;
  }
    
  //finish the circular list of halfedges.
  HE->set_next (F->halfedge());

  return F;
}

//############################################################################

bool dbmsh3d_mesh::check_integrity ()
{
  if (dbmsh3d_ifs_mesh::check_integrity() == false) {
    assert (0);
    return false;
  }

  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbmsh3d_edge* E = (*eit).second;
    if (E->id() != (*eit).first) {
      assert (0);
      return false;
    }
    if (E->check_integrity() == false) {
      assert (0);
      return false;
    }
  }

  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    if (F->id() != (*fit).first) {
      assert (0);
      return false;
    }
    if (F->check_integrity() == false) {
      assert (0);
      return false;
    }
  }

  return true;
}

dbmsh3d_pt_set* dbmsh3d_mesh::clone ()
{
  dbmsh3d_mesh* M2 = new dbmsh3d_mesh ();

  //deep-copy all vertices.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    dbmsh3d_vertex* V = (dbmsh3d_vertex*) (*vit).second;
    dbmsh3d_vertex* V2 = V->clone ();
    M2->_add_vertex (V2);
  }
  M2->set_vertex_id_counter (vertex_id_counter_);
  assert (M2->vertexmap().size() == vertexmap_.size());

  //deep-copy all edges.
  vcl_map<int, dbmsh3d_edge*>::iterator eit = edgemap_.begin();
  for (; eit != edgemap_.end(); eit++) {
    dbmsh3d_edge* E = (dbmsh3d_edge*) (*eit).second;
    dbmsh3d_edge* E2 = E->clone (M2);
    M2->_add_edge (E2);
  }
  M2->set_edge_id_counter (edge_id_counter_);
  assert (M2->edgemap().size() == edgemap_.size());

  //deep-copy all faces.
  vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
    dbmsh3d_face* F2 = F->clone (M2);
    M2->_add_face (F2);
  }
  M2->set_face_id_counter (face_id_counter_);
  assert (M2->facemap().size() == facemap_.size());

  /*if (is_MHE()) { //1) deep-copy the MHE data structure.
    //First copy the IFS then convert to MHE.
    vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
    for (; fit != facemap_.end(); fit++) {
      dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;  
      vcl_vector<int> vids;
      F->_get_bnd_V_ids_MHE (vids);
      dbmsh3d_face* F2 = M2->facemap (F->id());
      for (unsigned int i=0; i<vids.size(); i++) {
        dbmsh3d_vertex* V = M2->vertexmap (vids[i]);
        F2->_ifs_add_bnd_V (V);
      }
    }
    M2->IFS_to_MHE ();
    assert (M2->edgemap().size() == edgemap_.size());
  }
  else { //2) deep-copy the IFS data structure.
    vcl_map<int, dbmsh3d_face*>::iterator fit = facemap_.begin();
    for (; fit != facemap_.end(); fit++) {
      dbmsh3d_face* F = (dbmsh3d_face*) (*fit).second;
      dbmsh3d_face* F2 = M2->facemap (F->id());
      for (unsigned int i=0; i<F->vertices().size(); i++) {
        dbmsh3d_vertex* V = F->vertices (i);
        F2->_ifs_add_bnd_V (V);
      }
    }
  }*/

  return M2;
}

// Print a summary of mesh properties
void dbmsh3d_mesh::print_summary(vcl_ostream& str)
{
  str << "\n-------------------------------------------------\n";
  str << "Mesh info \n" ;
  str << "Number of vertices = " << this->vertexmap().size() << "\n";
  str << "Number of faces = " << this->facemap().size() << "\n";
  str << "Number of edges = " << this->edgemap().size() << "\n";
}

void dbmsh3d_mesh::print_topo_summary (void)
{ 
  vul_printf (vcl_cout, "\nThe reconstructed surface mesh is:\n");

  bool b_2_manifold;
  unsigned int n_bnd_edges = count_bnd_edges (b_2_manifold);

  if (b_2_manifold)
    vul_printf (vcl_cout, "\t- 2-manifold\n");
  else
    vul_printf (vcl_cout, "\t- non-2-manifold\n");

  int max_sides = count_max_polygon_sides ();

  if (max_sides == 3)
    vul_printf (vcl_cout, "\t- triangular\n");
  else
    vul_printf (vcl_cout, "\t- non-triangular, max %u polygon sides.\n", max_sides);


  if (n_bnd_edges == 0)
    vul_printf (vcl_cout, "\t- closed watertight surface.\n\n");
  else
    vul_printf (vcl_cout, "\t- non-watertight, %u boundary edges.\n\n", n_bnd_edges);
}

dbmsh3d_mesh* clone_mesh_ifs (dbmsh3d_mesh* M)
{
  //Assume M is in data structure mode of IFS, no isolated edge.
  dbmsh3d_mesh* newM = new dbmsh3d_mesh ();

  //Clone all vertices of M.
  //Note: use _new_vertex() to create a new vertex.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* V = (*vit).second;
    dbmsh3d_vertex* newV = newM->_new_vertex (V->id());
    newV->set_pt (V->pt());
    newM->_add_vertex (newV);
  }
  newM->set_vertex_id_counter (M->vertex_id_counter());

  //Clone all faces of M.
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    dbmsh3d_face* newF = newM->_new_face (F->id());
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      int vid = F->vertices(i)->id();
      dbmsh3d_vertex* newV = newM->vertexmap (vid);
      newF->_ifs_add_bnd_V (newV);
    }
    newM->_add_face (newF);
  }
  newM->set_face_id_counter (M->face_id_counter());

  return newM;
}

dbmsh3d_face* add_F_to_M (vcl_vector<int>& vids, dbmsh3d_mesh* M)
{  
  //First find or create the set of order edges of this face.
  //assumption: vids is ordered a circular way.
  vcl_vector<dbmsh3d_edge*> ordered_edges;

  //Add one more vertex to the end for circulation.
  vids.push_back (vids[0]);
  for (unsigned int i=0; i<vids.size()-1; i++) {
    dbmsh3d_vertex* V0 = M->vertexmap (vids[i]);
    if (V0 == NULL) { //if vertex not found, create a new vertex.
      V0 = M->_new_vertex (vids[i]);
      M->_add_vertex (V0);
    }
    dbmsh3d_vertex* V1 = M->vertexmap (vids[i+1]);
    if (V1 == NULL) { //if vertex not found, create a new vertex.
      V1 = M->_new_vertex (vids[i+1]);
      M->_add_vertex (V1);
    }

    dbmsh3d_edge* E = E_sharing_2V (V0, V1);
    if (E == NULL) //if edge not found, create a new edge.
      E = M->add_new_edge (V0, V1);

    ordered_edges.push_back (E);
  }

  //Create the face from the set of ordered edges.
  dbmsh3d_face* newF = M->add_new_face (ordered_edges);
  return newF;
}

dbmsh3d_face* add_F_to_M_check_topo (vcl_vector<int>& vids, dbmsh3d_mesh* M)
{
  //First find or create the set of order edges of this face.
  //assumption: vids is ordered a circular way.
  vcl_vector<dbmsh3d_edge*> ordered_edges;

  //Add one more vertex to the end for circulation.
  vids.push_back (vids[0]);

  //The topo. error only occur for existing edges or vertices.
  VTOPO_TYPE vt;

  for (unsigned int i=0; i<vids.size()-1; i++) {
    dbmsh3d_vertex* V0 = M->vertexmap (vids[i]);
    if (V0 == NULL) { //if vertex not found, create a new vertex.
      V0 = M->_new_vertex (vids[i]);
      M->_add_vertex (V0);
    }
    else { //Check V0 for non-2-manifold 1-ring
      vt = V0->detect_vtopo_type();
      if (vt==VTOPO_2_MANIFOLD_1RING || vt==VTOPO_NON_MANIFOLD_1RING)
        return NULL;
    }

    dbmsh3d_vertex* V1 = M->vertexmap (vids[i+1]);
    if (V1 == NULL) { //if vertex not found, create a new vertex.
      V1 = M->_new_vertex (vids[i+1]);
      M->_add_vertex (V1);
    }
    else { //Check V1 for non-2-manifold 1-ring
      vt = V1->detect_vtopo_type();
      if (vt==VTOPO_2_MANIFOLD_1RING || vt==VTOPO_NON_MANIFOLD_1RING)
        return NULL;
    }

    dbmsh3d_edge* E = E_sharing_2V (V0, V1);
    if (E == NULL) { //if edge not found, create a new edge.
      E = M->add_new_edge (V0, V1);
    }
    else { //Check E for non-2-manifold topology.
      if (E->n_incident_Fs() > 1)
        return NULL;
    }

    ordered_edges.push_back (E);
  }

  //Create the face from the set of ordered edges.
  dbmsh3d_face* newF = M->add_new_face (ordered_edges);
  return newF;
}

void add_M_faces_to_IFSset (dbmsh3d_mesh* M, vcl_vector<vcl_vector<int> >& faces)
{
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;
    vcl_vector<int> vids;
    F->get_bnd_V_ids (vids);
    faces.push_back (vids);
  }
}

// ----------------------------------------------------------------------------
void bmsh3d_mesh_print_object_size ()
{
  vul_printf (vcl_cout, "\n\n");
  vcl_vector<dbmsh3d_edge*> tmp_vector;
  vcl_cout << "size of vcl_vector: "<< sizeof(tmp_vector) << vcl_endl;  
  vcl_list<dbmsh3d_edge*> tmp_list;
  vcl_cout << "size of vcl_list: "<< sizeof(tmp_list) << vcl_endl;
  vcl_set<dbmsh3d_edge*> tmp_set;
  vcl_cout << "size of vcl_set: "<< sizeof(tmp_set) << vcl_endl;
  vcl_set<dbmsh3d_edge*> tmp_map;
  vcl_cout << "size of vcl_map: "<< sizeof(tmp_map) << vcl_endl;
  vul_printf (vcl_cout, "\n");

  vul_printf (vcl_cout, "    Object          Size (bytes)\n");
  vul_printf (vcl_cout, "------------------------------------\n");
  vul_printf (vcl_cout, "vispt_elm                %3d\n", sizeof (vispt_elm));
  vul_printf (vcl_cout, "dbmsh3d_vertex       --  %3d\n", sizeof (dbmsh3d_vertex));
  vul_printf (vcl_cout, "\n");
  vul_printf (vcl_cout, "dbmsh3d_halfedge     --  %3d\n", sizeof (dbmsh3d_halfedge));
  vul_printf (vcl_cout, "dbmsh3d_edge         --  %3d\n", sizeof (dbmsh3d_edge));
  vul_printf (vcl_cout, "\n");
  vul_printf (vcl_cout, "dbmsh3d_face         --  %3d\n", sizeof (dbmsh3d_face));
  vul_printf (vcl_cout, "\n");
  
  vul_printf (vcl_cout, "dbmsh3d_pt_set           %3d\n", sizeof (dbmsh3d_pt_set));
  vul_printf (vcl_cout, "dbmsh3d_ifs_mesh         %3d\n", sizeof (dbmsh3d_ifs_mesh));
  vul_printf (vcl_cout, "dbmsh3d_mesh             %3d\n", sizeof (dbmsh3d_mesh));
}

bool dbmsh3d_mesh::is_2_manifold ()
{
  vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    dbmsh3d_edge* edge = (*it).second;
    if (edge->n_incident_Fs() > 2)
      return false;
  }
  return true;
}

unsigned int dbmsh3d_mesh::count_max_polygon_sides ()
{
  unsigned int max_sides = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;

    unsigned int sides = F->n_bnd_Es();
    if (sides > max_sides)
      max_sides = sides;
  }

  return max_sides;
}

//: Return number of edges in traversing the IFS data structure.
//  Note that an internal edge can be used twice.
unsigned int dbmsh3d_mesh::count_ifs_dup_edges ()
{
  unsigned int count = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    dbmsh3d_face* F = (*it).second;
    count += F->vertices().size();
  }
  return count;
}

//: return total boundary edges
//  also determine if this mesh is watertight or not.
unsigned int dbmsh3d_mesh::count_bnd_edges (bool& b_2_manifold)
{
  b_2_manifold = true;
  unsigned int bnd_edges = 0;

  vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    dbmsh3d_edge* edge = (*it).second;

    unsigned int count = edge->n_incident_Fs();
    if (count > 2)
      b_2_manifold = false;
    if (count < 2)
      bnd_edges++;
  }
  b_watertight_ = (bnd_edges == 0);
  return bnd_edges;
}

double dbmsh3d_mesh::get_avg_edge_len ()
{
  unsigned int  count_edge = 0;
  double        sum_length = 0;

  assert (edgemap_.size() != 0);
  vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    dbmsh3d_edge* edge = (*it).second;
    
    double len = edge->length();
    count_edge++;
    sum_length += len;
  }

  double avg_len = sum_length / count_edge;
  return avg_len;
}

//: Merge two faces who sharing an edge together.
//  Only valid if E is a 2-manifold edge.
//  Delete edge E and add all remaining halfedges of F2 to F1, and then delete E and F2.
//  Result of merged face is in F1.
//
void dbmsh3d_mesh::m2_mesh_merge_face (dbmsh3d_face* F1, dbmsh3d_face* F2 ,dbmsh3d_edge* E)

{
  assert (F1 != F2);
  vcl_cout << "mesh_merge_faces " << F1->id() << " with " << F2->id() << vcl_endl;

  dbmsh3d_vertex* Vs = E->sV();
  dbmsh3d_vertex* Ve = E->eV();
  dbmsh3d_halfedge* HE1 = E->get_HE_of_F (F1);
  dbmsh3d_halfedge* HE2 = E->get_HE_of_F (F2);

  dbmsh3d_halfedge* F1_HE_head = F1->find_next_bnd_HE (Vs, HE1);
  dbmsh3d_halfedge* F1_HE_tail = F1->find_next_bnd_HE (Ve, HE1);
  //Swap the head & tail if necessary
  if (F1_HE_head->next() != HE1) {
    assert (F1_HE_tail->next() == HE1);
    dbmsh3d_halfedge* temp = F1_HE_tail;
    F1_HE_tail = F1_HE_head;
    F1_HE_head = temp;
  }

  //Traverse all halfedges of F2 and add to F1 in order.
  dbmsh3d_halfedge* HE = HE2->next();
  while (HE != HE2) {
    F1_HE_head->set_next (HE);
    HE->set_face (F1);
    F1_HE_head = HE;
    HE = HE->next();
  }
  F1_HE_head->set_next (F1_HE_tail);

  //Delete F2. Note that most of F2's halfedges now belongs to F1.
  F2->set_halfedge (HE2);
  HE2->set_next (NULL);
  remove_face (F2);

  //Remove HE1: the other incidence of edge E.
  E->_disconnect_HE (HE1);
  delete HE1;
  assert (E->halfedge() == NULL);
  remove_edge (E);

  //Sort F1's HE list.
  F1->set_halfedge (F1_HE_tail);
  F1->_sort_bnd_HEs_chain ();
  F1->_ifs_clear_vertices ();
}

//########################################################################

//: For 2-manifold mesh only.
//  This function fix each face's halfedges in a consistent order.
//  Pick one face and fix the orientation of its halfedges using right hand rule on it.
//  propagation the orientation to all adjacent faces
//  result: for a 2-manifold mesh, all edges in general has two incident faces
//          (otherwise this edge is on the boundary of the mesh)
//          after this orientation fix, for all edges, the 2 sides (halfedges) of it 
//          on both adjacent faces has different direction.
//  Parameter:
//    mesh:         the mesh under work
//    sfaceid:      the id of the starting face
//    b_flip_sface: flip the orientation of the starting face or not.
void manifold_fix_faces_orientation (dbmsh3d_mesh* mesh, int sfaceid, bool b_use_e_vertex_as_next)
{
  mesh->reset_traverse ();
  
  //: fix the starting_face
  dbmsh3d_face* starting_face = mesh->facemap (sfaceid);
  dbmsh3d_halfedge* startHE = starting_face->halfedge(); 
  if (b_use_e_vertex_as_next)
    starting_face->set_orientation (startHE, startHE->edge()->eV());
  else
    starting_face->set_orientation (startHE, startHE->edge()->sV());

  starting_face->set_i_visited (mesh->i_traverse_flag());

  //: propagation of the orientation to all other faces
  //  the front here is the halfedge with associated faces to propagate.
  vcl_queue<dbmsh3d_halfedge*> he_queue;

  //: put all adjacent faces into the queue
  dbmsh3d_halfedge* HE = startHE;
  do {
    dbmsh3d_edge* edge = HE->edge();
    //: assume 2-manifold mesh
    if (edge->halfedge()->face() != starting_face)
      he_queue.push (edge->halfedge());
    else if (edge->halfedge()->pair())
      he_queue.push (edge->halfedge()->pair());

    HE = HE->next();
  }
  while (HE != startHE);

  //: the main while loop.
  while (!he_queue.empty()) {
    //: pop one
    dbmsh3d_halfedge* front_he = he_queue.front();
    he_queue.pop();

    //: find the already oriented next_v on the other side
    dbmsh3d_halfedge* sorted_he = front_he->pair();
    dbmsh3d_halfedge* sorted_he_next = sorted_he->next();
    
    dbmsh3d_vertex* next_v = Es_sharing_V (sorted_he->edge(), sorted_he_next->edge());
    next_v = front_he->edge()->other_V (next_v);

    dbmsh3d_face* front_face = front_he->face();
    //: this check is necessary. note that a face can be traversed more than once.
    if (front_face->is_visited (mesh->i_traverse_flag()))
      continue;

    //: fix orientation of it
    front_face->set_orientation (front_he, next_v);

    front_face->set_i_visited (mesh->i_traverse_flag());

    //: put all non-visited face in the queue
    dbmsh3d_halfedge* HE = front_face->halfedge();
    do {
      dbmsh3d_edge* edge = HE->edge();
      //: assume 2-manifold mesh
      if (edge->halfedge()->face() != front_face) {
        if (edge->halfedge()->face()->is_visited (mesh->i_traverse_flag()) == false)
          he_queue.push (edge->halfedge());
      }
      else if (edge->halfedge()->pair()) {
        dbmsh3d_face* F = edge->halfedge()->pair()->face();
        assert (F != front_face);
        if (!F->is_visited (mesh->i_traverse_flag()))
          he_queue.push (edge->halfedge()->pair());
      }

      HE = HE->next();
    }
    while (HE != front_face->halfedge());
  }
}
