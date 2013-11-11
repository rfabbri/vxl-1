// This is dbmsh3d/algo/dbmsh3d_mesh_algos.cxx
//:
// \file

#include "dbmsh3d_mesh_algos.h"

#include <vcl_list.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_utility.h>
#include <vcl_queue.h>
#include <vul/vul_printf.h>

#include <rsdl/rsdl_kd_tree.h>

#include <dbmsh3d/algo/dbmsh3d_face_algo.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>

//check integrity: remove erroneous faces.
//Return true if no error found.
bool remove_erroneous_Fs_IFS (dbmsh3d_mesh* M)
{
  unsigned int n_face_del = 0;
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  while (fit != M->facemap().end()) {
    dbmsh3d_face* F = (*fit).second;
    fit++;
    //Remove F that has invalid IFS structure (repeated or wrong Vids).
    if (F->_is_ifs_valid (M) == false) {
      M->remove_face (F);
      n_face_del++;
    }
  }
  return n_face_del==0;
}

//: Merge all faces that are co-planar and sharing a common edge.
void merge_mesh_coplanar_faces (dbmsh3d_mesh* M)
{
  vcl_cout << "merge_mesh_coplanar_faces():" << vcl_endl;

  //Add all the edges that have exactly 2 halfedges to the queue for processing.
  vcl_queue<dbmsh3d_edge*> e_queue;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = M->edgemap().begin();
  for (; eit != M->edgemap().end(); eit++) {
    dbmsh3d_edge* E = (*eit).second;
    if (E->n_incident_Fs() == 2) 
      e_queue.push (E);
  }

  //Now process the edges and merge the faces that are in the same plane
  unsigned int count = 0;
  while (!e_queue.empty()) {
    dbmsh3d_edge* E = e_queue.front();
    e_queue.pop();

    dbmsh3d_face* F1 = E->halfedge()->face(); 
    dbmsh3d_face* F2 = E->m2_other_face (F1);

    if (faces_in_same_plane (F1, F2)) {
      M->m2_mesh_merge_face (F1, F2, E);  
      count++;
    }
  }
 
  vcl_cout << "  # merge operation(s) done: " << count << vcl_endl; 
}

void add_mesh_faces_IFS (dbmsh3d_mesh* M, vcl_vector<vcl_vector<int> >& IFS_faces)
{
  vcl_map<int, dbmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    dbmsh3d_face* F = (*fit).second;

    F->_ifs_track_ordered_vertices();
    vcl_vector<int> vids;
    for (unsigned int i=0; i<F->vertices().size(); i++)
      vids.push_back (F->vertices(i)->id());
    IFS_faces.push_back (vids);
  }
}

void get_faces_intersect_box (const vcl_vector<vcl_vector<int> >& input_faces, 
                              const vcl_vector<vgl_point_3d<double> >& input_pts, 
                              const vgl_box_3d<double>& box,
                              vcl_vector<vcl_vector<int> >& in_faces)
{
  for (unsigned int i=0; i<input_faces.size(); i++) {
    //Brute-forcely check each IFS face.
    bool intersect_box = false;
    for (unsigned int j=0; j<input_faces[i].size(); j++) {
      int id = input_faces[i][j];
      vgl_point_3d<double> P = input_pts[id];
      if (box.contains (P)) {
        intersect_box = true;
        break;
      }
    }
    if (intersect_box)
      in_faces.push_back (input_faces[i]);
  }
}

int erode_mesh_boundary (dbmsh3d_mesh* M, const int steps)
{
  unsigned int count = 0;
  for (int i=0; i<steps; i++) {
    dbmsh3d_bnd_chain_set* bnd_chain_set = new dbmsh3d_bnd_chain_set (M);
    bnd_chain_set->detect_bnd_chains ();

    //Get all boundary triangles of chain >= 10 triangles.
    vcl_set<dbmsh3d_face*> faces_to_del;
    vcl_vector<dbmsh3d_bnd_chain*>::iterator bit = bnd_chain_set->chainset().begin();
    for (; bit != bnd_chain_set->chainset().end(); bit++) {
      dbmsh3d_bnd_chain* BC = (*bit);
      if (BC->HE_list().size() < 10)
        continue;

      for (unsigned int i=0; i<BC->HE_list().size(); i++) {
        faces_to_del.insert (BC->HE_list(i)->face());
      }
    }

    //Delete all faces_to_del
    vcl_set<dbmsh3d_face*>::iterator it = faces_to_del.begin();
    for (; it != faces_to_del.end(); it++) {      
      M->remove_F_del_isolated_Es (*it);
      count++;
    }

    delete bnd_chain_set;
  }

  assert (M->facemap().size() != 0);
  return count;
}

dbmsh3d_mesh* build_mesh_from_IFS (const vcl_vector<vgl_point_3d<double> >& input_pts,
                                   const vcl_vector<vcl_vector<int> >& input_faces)
{
  unsigned int i, j;
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  //Put all IFS points into a set
  vcl_set<int> pt_ids;
  for (i=0; i<input_faces.size(); i++) {
    for (j=0; j<input_faces[i].size(); j++)
      pt_ids.insert (input_faces[i][j]);
  }

  //Create all mesh vertices.
  vcl_set<int>::iterator it = pt_ids.begin();
  for (; it != pt_ids.end(); it++) {
    int id = (*it);
    dbmsh3d_vertex* V = M->_new_vertex (id);
    vgl_point_3d<double> P = input_pts[id];
    V->get_pt().set (P.x(), P.y(), P.z());
    M->_add_vertex (V);
  }
  pt_ids.clear();

  //Create all mesh faces
  for (unsigned int i=0; i<input_faces.size(); i++) {
    dbmsh3d_face* F = M->_new_face ();

    for (unsigned j=0; j<input_faces[i].size(); j++) {
      int id = input_faces[i][j];
      dbmsh3d_vertex* V = M->vertexmap (id);
      F->_ifs_add_bnd_V (V);
    }
    M->_add_face (F);
  }

  return M;
}

//: Check if introducing a face of vector<vids> to mesh M violates 2-manifold topology.
//  return true if no topological conflict is found.
bool check_F_M_topo (vcl_vector<int>& vids, dbmsh3d_mesh* M)
{
  //Note that the last element of vids[] = vids[0]
  assert (vids[0] == vids[vids.size()-1]);

  //Check if the vids[] coincides with any existing mesh face.
  //This only happens at the 'interior boundary' triangles.
  //Should avoid this to improve speed!!
  vcl_vector<dbmsh3d_vertex*> vertices;
  for (unsigned int i=0; i<vids.size()-1; i++)
    vertices.push_back (M->vertexmap(i));
  dbmsh3d_face* F = find_F_sharing_Vs (vertices);
  if (F)
    return false;

  //Check if any vertex violates the 1-ring nbr assumption.
  for (unsigned int i=0; i<vids.size()-1; i++) {
    dbmsh3d_vertex* V = M->vertexmap (vids[i]);
    VTOPO_TYPE type = V->detect_vtopo_type();
    if (type == VTOPO_2_MANIFOLD_1RING ||
        type == VTOPO_NON_MANIFOLD_1RING)
      return false;
  }

  //Check if any edge violate the 2-manifold assumption.
  for (unsigned int i=1; i<vids.size(); i++) {
    dbmsh3d_vertex* newV1 = M->vertexmap (vids[i-1]);
    dbmsh3d_vertex* newV2 = M->vertexmap (vids[i]);
    dbmsh3d_edge* newE = E_sharing_2V (newV1, newV2);

    //Only need to check topology if such newE exists.
    if (newE && newE->n_incident_Fs() > 1)
      return false;
  }

  return true;
}

//Label mesh components and save large components to various mesh files.
bool cclabel_save_mesh (dbmsh3d_mesh* M, const vcl_string& prefix,
                        const int th_n, const int option)
{
  vul_printf (vcl_cout, "cclabel_save_mesh(): M %d vertices %d faces, th_n %d.\n",
              M->vertexmap().size(), M->facemap().size(), th_n);

  //option 1: use IFS_to_MHE().
  //option 2: use IFS_to_MHE_bf().
  if (option==2)
    M->IFS_to_MHE_bf();    
  else
    M->IFS_to_MHE();

  //Label connected component & save then delete the component until finish.
  int label;
  int count = 0;
  while (M->facemap().size()) {
    //Use the first face F as seed.
    vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
    dbmsh3d_face* F = (*it).second;
    label = M->init_traverse();
    int nF = label_mesh_component (M, F, label);

    //Save the labelled mesh faces into file, if nF > th_n
    if (nF > th_n) {
      char file[256];
      vcl_sprintf (file, "%s-%02d.ply2", prefix.c_str(), count);
      dbmsh3d_save_label_faces_ply2 (M, label, file);
      count++;
    }

    //Delete the labelled mesh faces from M.
    dbmsh3d_del_label_faces (M, label);
  }

  vul_printf (vcl_cout, "%d components created, %d mesh files saved.\n", 
              label, count);

  return true;
}

int label_mesh_component (dbmsh3d_mesh* M, dbmsh3d_face* seedF, const int label)
{
  //Initialize the face_queue with seedF.
  assert (seedF->is_visited (label) == false);  
  vcl_queue<dbmsh3d_face*> face_queue;
  face_queue.push (seedF);
  int n_label = 0;
  
  while (face_queue.size() > 0) {
    dbmsh3d_face* F = face_queue.front();
    face_queue.pop();
    if (F->is_visited(label))
      continue;

    //Label F
    F->set_i_visited (label);
    n_label++;

    //Add all unvisited adjacent faces of F into face_queue.  
    dbmsh3d_halfedge* cur_he = F->halfedge();
    do {
      //label all pairs of he.      
      dbmsh3d_halfedge* he = cur_he->pair();
      while (he && he != cur_he) {
        dbmsh3d_face* curF = he->face();        
        //Add curF to face_queue, if not visited.
        if (curF->is_visited (label) == false)
          face_queue.push (curF);
        he = he->pair();
      }

      cur_he = cur_he->next();
    }
    while (cur_he != F->halfedge());
  }

  return n_label;
}

//Delete the labelled mesh faces from M.
int dbmsh3d_del_label_faces (dbmsh3d_mesh* M, const int label)
{
  int n_label = 0;
  
  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  while (it != M->facemap().end()) {
    dbmsh3d_face* F = (*it).second;

    if (F->is_visited (label)) {
      //delete F
      vcl_map<int, dbmsh3d_face*>::iterator tmp = it;
      it++;
      M->remove_F_del_isolated_Es (F);
      n_label++;
    }
    else
      it++;
  }

  return n_label;
}

int remove_extraneous_faces (dbmsh3d_mesh* M)
{
  assert (M->is_MHE());
  vul_printf (vcl_cout, "remove_extraneous_faces: %d faces, %d edges.\n",
              M->facemap().size(), M->edgemap().size());
  unsigned int n_edge_del = 0;

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  while (it != M->facemap().end()) {
    dbmsh3d_face* F = (*it).second;
    if (is_F_extraneous (F)) {
      //delete F
      vcl_map<int, dbmsh3d_face*>::iterator tmp = it;
      it++;
      M->remove_F_del_isolated_Es (F);
      n_edge_del++;
    }
    else
      it++;
  }

  vul_printf (vcl_cout, "\t%d extraneous faces removed.\n\n", n_edge_del);
  return n_edge_del;
}

//: Compute the centroid of all mesh vertices.
vgl_point_3d<double> compute_centroid (dbmsh3d_mesh* M)
{
  vul_printf (vcl_cout, "compute_centroid of %d points.\n", M->vertexmap().size());
  double x=0, y=0, z=0;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    x += V->pt().x();
    y += V->pt().y();
    z += V->pt().z();
  }
  x /= M->vertexmap().size();
  y /= M->vertexmap().size();
  z /= M->vertexmap().size();
  return vgl_point_3d<double> (x, y, z);
}


//: Compute the varance of all mesh vertices, given the centroid.
//  varance ~= avg. dist. of points.
double compute_var (dbmsh3d_mesh* M, vgl_point_3d<double>& C)
{  
  vul_printf (vcl_cout, "compute_var of %d points.\n", M->vertexmap().size());
  double sum_d = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
  for (; it != M->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    sum_d += vgl_distance (V->pt(), C);
  }

  sum_d /= M->vertexmap().size();
  return sum_d;
}

//: Compute the varance of all mesh vertices.
double compute_cen_var (dbmsh3d_mesh* M, vgl_point_3d<double>& C)
{
  C = compute_centroid (M);
  return compute_var (M, C);
}

//######################################################################
// Mesh Traversing Algorithms

//: Collect the set of faces given a seedF and a bounding curve (links).
//  Assume M->reset_traverse_f() is already done.
void collect_Fs_given_seed_bnd (dbmsh3d_face* seedF, vcl_set<dbmsh3d_edge*>& bnd_E_set, 
                                dbmsh3d_mesh* M, vcl_set<dbmsh3d_face*>& Fset)
{
  //Start propagate form seedF until any edge in bnd_E_set is reached.
  vcl_queue<dbmsh3d_face*> FQ;
  FQ.push (seedF);
  while (FQ.size() > 0) {
    dbmsh3d_face* frontF = FQ.front();
    FQ.pop();
    
    if (frontF->b_visited())
      continue; //Needed, because a F can be on the front for multiple times.

    Fset.insert (frontF);
    frontF->set_i_visited (1);

    //Debug:
    ///if (M->facemap().size() > 20)
      ///assert (Fset.size() < double(M->facemap().size())/1.3);

    //propagate the frontF (through 2-manifold fs_edges)
    dbmsh3d_halfedge* HE = frontF->halfedge();
    do {
      dbmsh3d_edge* E = HE->edge();
      if (bnd_E_set.find (E) != bnd_E_set.end()) {         
        HE = HE->next(); //Skip E in bnd_E_set.
        continue;
      }

      dbmsh3d_face* otherF = E->other_2_manifold_F (frontF);
      if (otherF) {
        if (otherF->b_visited() == false)
          FQ.push (otherF);
      }
      HE = HE->next();
    }
    while (HE != frontF->halfedge());
  }
}

//######################################################################

#define CROSS_R 30 //100
#define CROSS_L 50 //150
#define CROSS_T 0.02 //0.01 //twisting angle parameter

dbmsh3d_mesh* create_twist_cross ()
{
  dbmsh3d_mesh* M = new dbmsh3d_mesh;
  double t;
  double px, py;

  for (int z=0; z<CROSS_L; z++) {
    t = CROSS_T*z;

    //Generate the cross in x-axis.
    for (int x=-CROSS_R; x<=CROSS_R; x++) {
      px = x;

      //Rotate the generated point by theta.
      px = vcl_cos(t) * x;
      py = vcl_sin(t) * x;

      //add point
      vgl_point_3d<double> p = vgl_point_3d<double> (px, py, z);
      M->add_vertex (p);
    }

    //Generate the cross in y-axis.
    for (int y=-CROSS_R; y<=CROSS_R; y++) {
      py = y;

      //Rotate the generated point by theta.
      px = - vcl_sin(t) * y;
      py = vcl_cos(t) * y;

      //add point
      vgl_point_3d<double> p = vgl_point_3d<double> (px, py, z);
      M->add_vertex (p);
    }

  }

  return M;
}

//######################################################################

//: Extract boundary of vertices of a manifold mesh
bool dbmsh3d_mesh_algos::find_boundary(dbmsh3d_mesh& mesh, 
              vcl_vector< vcl_vector< dbmsh3d_vertex* > >& boundary_list)
{
  // preliminary clean-up
  boundary_list.clear();

  // 2-manifold check
  if (!mesh.is_2_manifold()) return false;

  // traverse edge list to find an edge on the boundary
  mesh.reset_traverse ();
  mesh.reset_edge_traversal();
  dbmsh3d_edge* i_edge = 0;
  while(mesh.next_edge(i_edge))
  {
    assert(i_edge);

    // traverse each edge only once
    if (i_edge->is_visited(mesh.i_traverse_flag())) continue;
    i_edge->set_i_visited(mesh.i_traverse_flag());

    // continue searching if this is not a boundary edge
    if (i_edge->halfedge()->pair()) continue;
    

    // when hitting a boundary edge, trace out the boundary
    vcl_list<dbmsh3d_vertex* > boundary;
    dbmsh3d_edge* start_bnd_edge = i_edge;
    dbmsh3d_vertex* start_bnd_vertex = i_edge->sV();

    dbmsh3d_vertex* cur_bnd_vertex = start_bnd_vertex;
    dbmsh3d_edge* cur_bnd_edge = start_bnd_edge;

    do {
      assert (cur_bnd_edge);

      boundary.push_back(cur_bnd_vertex);
      dbmsh3d_vertex* next_bnd_vertex = cur_bnd_edge->other_V(cur_bnd_vertex);
      
      // traverse thru the edges incident to next_vertex to find the next
      // boundary edge
      // for 2-manifold, any boundary vertex must be incident to exactly two
      // boundary edge
      dbmsh3d_edge* next_bnd_edge = 0;
      for (dbmsh3d_ptr_node* cur = next_bnd_vertex->E_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_edge* E = (dbmsh3d_edge*) cur->ptr();
        if (E->is_visited(mesh.i_traverse_flag())) 
          continue;
        E->set_i_visited(mesh.i_traverse_flag());

        // bypass the current boundary edge
        if (E == cur_bnd_edge) 
          continue;

        // stop iterating when hitting a boundary edge
        if (!E->halfedge()->pair()) {
          next_bnd_edge = E;
          break;
        }
      }

      // update current edge and vertex
      cur_bnd_vertex = next_bnd_vertex;
      cur_bnd_edge = next_bnd_edge;
    }
    while (cur_bnd_vertex != start_bnd_vertex);

    // convert list to vector and insert to the boundary list
    vcl_vector<dbmsh3d_vertex* > vertex_vector(boundary.begin(), boundary.end());

    boundary_list.push_back(vertex_vector);
  }

  return true;

}



//: Triangulate two "parallel" curves in space and create a "band-like" mesh
bool dbmsh3d_mesh_algos::
make_band_trimesh(const vcl_vector<dbmsh3d_vertex* >& poly1,
               const vcl_vector<dbmsh3d_vertex* >& poly2,
               vcl_vector<dbmsh3d_face* >& new_faces)
{
  // 1. Use poly1 as reference, re-arrange poly2 so that its first vertex matches with the first vertex of poly1 and the two orientations match
  vcl_cout << "This function has not been implemented.\n";
  return false;
}



// ----------------------------------------------------------------------------
//: Compute volume of a mesh when it is a polyhedron
// Algorithm: from Matlab code of John Burkardt
// http://www.scs.fsu.edu/~burkardt/m_src/geometry/geometry.html
// File : polyhedron_volume_3d_2.m
// TODO
double dbmsh3d_mesh_algos::polyhedron_volume(dbmsh3d_mesh& mesh)
{
  // 2-manifold check
  if (mesh.is_2_manifold())
    return -1;

  // check closeness (watertight)
  vcl_vector< vcl_vector< dbmsh3d_vertex* > > boundary_list;
  dbmsh3d_mesh_algos::find_boundary(mesh, boundary_list);
  if (! boundary_list.empty()) 
    return -1;

  // compute volume using formula of a polyhedron

  // assume the faces are oriented properly

  double volume = 0;
  mesh.reset_face_traversal();
  for (dbmsh3d_face* face = 0; mesh.next_face(face);)
  {
    // Compute the area vector for this face.
  
  
  }
  

//    dim_num = 3;
//
//  volume = 0.0;
//
//  for face = 1 : face_num
//
//    v(1:dim_num) = 0.0;
//%
//%  Compute the area vector for this face.
//%
//    for j = 1 : order(face)
//
//      k1 = node(face,j);
//
//      if ( j < order(face) )
//        k2 = node(face,j+1);
//      else
//        k2 = node(face,1);
//      end
//%
//%  Compute the cross product.
//%
//      normal(1) = coord(2,k1) * coord(3,k2) - coord(3,k1) * coord(2,k2);
//      normal(2) = coord(3,k1) * coord(1,k2) - coord(1,k1) * coord(3,k2);
//      normal(3) = coord(1,k1) * coord(2,k2) - coord(2,k1) * coord(1,k2);
//
//      v(1:dim_num) = v(1:dim_num) + normal(1:dim_num);
//
//    end
//%
//%  Area vector dot any vertex.
//%
//    k = node(face,1);
//    volume = volume + v(1:dim_num) * coord(1:dim_num,k);
//
//  end
//
//  volume = volume / 6.0;
//

  return -1;

}


// ----------------------------------------------------------------------------
//: Compute centroid of a point set
vgl_point_3d<double > dbmsh3d_mesh_algos::compute_centroid(dbmsh3d_mesh* mesh)
{
  double sum[] = {0, 0, 0};
  mesh->reset_vertex_traversal();
  for (dbmsh3d_vertex* vbase = 0; mesh->next_vertex(vbase);)
  {
    dbmsh3d_vertex* v = vbase;
    vgl_point_3d<double > pt = v->pt();
    sum[0] += pt.x();
    sum[1] += pt.y();
    sum[2] += pt.z();  
  }
  int num_pts = mesh->num_vertices();
  return vgl_point_3d<double >(sum[0]/num_pts, sum[1]/num_pts, sum[2]/num_pts);
}






// ----------------------------------------------------------------------------
//: Create a new mesh whose faces are a subset of the original mesh
// Assumption: all faces in the face_list belong to ``mesh"
bool dbmsh3d_mesh_algos::
submesh(dbmsh3d_mesh& mesh, 
        const vcl_vector<dbmsh3d_face* >& face_list,
        dbmsh3d_mesh& new_mesh)
{
  // Iterate thru the faces
  new_mesh.set_face_id_counter(mesh.face_id_counter());
  new_mesh.set_vertex_id_counter(mesh.vertex_id_counter());
  for (unsigned i=0; i < face_list.size(); ++i)
  {
    dbmsh3d_face* face = face_list[i];
    dbmsh3d_face* new_face = new dbmsh3d_face(face->id());

    // enter the vertices for the face
    for (unsigned j=0; j < face->vertices().size(); ++j) 
    {
      const dbmsh3d_vertex* vb = face->vertices(j);
      dbmsh3d_vertex* new_vb = new_mesh.vertexmap(vb->id());

      // create a new vertex if it has not been created
      if (!new_vb)
      {
        const dbmsh3d_vertex* v = static_cast<const dbmsh3d_vertex* >(vb);
        vgl_point_3d<double > pt = v->pt();
        dbmsh3d_vertex* new_v = new dbmsh3d_vertex(pt.x(), pt.y(), pt.z(), v->id());
        new_vb = new_v;
        new_mesh._add_vertex(new_vb);
      }

      new_face->_ifs_add_bnd_V (new_vb);
    }
    new_mesh._add_face(new_face);
  }

  return true;
}

// ----------------------------------------------------------------------------
//: crop a mesh with a retangular box
// only faces that lie completely inside the box will retain
bool dbmsh3d_mesh_algos::crop_mesh (dbmsh3d_mesh& source_mesh,
                                    const vgl_box_3d<double >& box,
                                    dbmsh3d_mesh& new_mesh)
{
  vcl_vector<dbmsh3d_face* > face_list;
  source_mesh.reset_face_traversal();
  for ( dbmsh3d_face* face = 0; source_mesh.next_face(face); )
  {
    bool inside = true;
    for (unsigned i=0; i < face->vertices().size() && inside; ++i)
    {
      const dbmsh3d_vertex* vertex = 
        static_cast<const dbmsh3d_vertex* >(face->vertices(i));
      vgl_point_3d<double > pt = vertex->pt();
      inside = inside && box.contains(pt);
    }

    if (inside)
    {
      face_list.push_back(face);
    }
  }
  return dbmsh3d_mesh_algos::submesh(source_mesh, face_list, new_mesh);
}
  




// ----------------------------------------------------------------------------
//: Compute distance between two meshes
// Compute closest distances from each vertex of "ref_mesh" to "data_mesh"
// and stored the distance at each vertex with property "verror"
bool dbmsh3d_mesh_algos::
mesh2mesh_distance_map(dbmsh3d_richmesh& ref_mesh, 
                       dbmsh3d_mesh& data_mesh)
{
  //Put all vertices of mesh 1 into kd-tree.
  rsdl_kd_tree* kd_tree = dbmsh3d_build_kdtree_vertices (&data_mesh);

  int top_n = 10;


  // keep track of progress
  int num_vertices = ref_mesh.num_vertices();
  int vertex_count = 0;
  double current_percentage = 0;
  

  ref_mesh.reset_vertex_traversal();
  for (dbmsh3d_vertex* vb = 0; ref_mesh.next_vertex(vb); ++vertex_count)
  {
    double percent_processed_vertices = vertex_count * 100.0/ num_vertices;
    if (percent_processed_vertices > (current_percentage + 5))
    {
      current_percentage += 5;
      vcl_cout << current_percentage << "%  ";
    }



    if (vb->is_a() != "dbmsh3d_richvertex")
    {
      vcl_cerr << "Found vertex of ref_mesh not of type 'dbmsh3d_richvertex'."
        << " Computing mesh2mesh distance stopped.\n";
      return false;
    }
    dbmsh3d_richvertex* v = static_cast<dbmsh3d_richvertex* >(vb);
    vgl_point_3d<double > pt = v->pt();
    vgl_point_3d<double > closest_pt;
    double min_dist = dbmsh3d_pt_mesh_dist(pt, &data_mesh, kd_tree, top_n, closest_pt);
    v->add_scalar_property("verror_abs", min_dist);
  }

  delete kd_tree;
  return true;
}




// ==================================================================
// support functions

//: re-arrange a 3D polyline/polygon so that it initial vertex and its
// orientation match with a reference 3D polyline/polygon
// Return re-arranged copy of moving_poly
vcl_vector<dbmsh3d_vertex* > dbmsh3d_mesh_algos::
align_2_polys(const vcl_vector<dbmsh3d_vertex* >& ref_poly,
              const vcl_vector<dbmsh3d_vertex* >& moving_poly)
{
  // 1. Align initial vertices
  dbmsh3d_vertex* ref_vertex = ref_poly[0];
  double min_distance = 1e10;
  vcl_vector<dbmsh3d_vertex* >::const_iterator min_distance_iter;
  for (vcl_vector<dbmsh3d_vertex* >::const_iterator vit = moving_poly.begin();
    vit != moving_poly.end(); ++vit)
  {
    dbmsh3d_vertex* i_vertex = *vit;
    if (vgl_distance(i_vertex->pt(), ref_vertex->pt()) < min_distance)
    {
      min_distance = vgl_distance(i_vertex->pt(), ref_vertex->pt());
      min_distance_iter = vit;
    }
  }

  // create a copy of moving_poly so that its initial vertex matches with that 
  // of ref_poly
  vcl_vector<dbmsh3d_vertex* > aligned_poly;
  aligned_poly.reserve(ref_poly.size());
  vcl_copy(min_distance_iter, ref_poly.end(), aligned_poly.begin());
  vcl_copy(ref_poly.begin(), min_distance_iter, aligned_poly.end());

  // 2. Align orientation
  
  // a crude method to determine orientation
  // compare the distance between ref_poly(length/4) to two vertices:
  // aligned_poly(length /4) and aligned_poly(3/4 * length)
  // if the former is shorter than the orientation is correct
  // otherwise, a direction reversal is neccessary

  dbmsh3d_vertex* ref_v1 = ref_poly[ref_poly.size() / 4];
  dbmsh3d_vertex* test_v1 = aligned_poly[aligned_poly.size()/4];
  dbmsh3d_vertex* test_v2 = aligned_poly[3*aligned_poly.size()/4];

  double d1 = vgl_distance(ref_v1->pt(), test_v1->pt());
  double d2 = vgl_distance(ref_v1->pt(), test_v2->pt());

  // if no orientation reversal is needed
  if (d1 < d2) 
    return aligned_poly;
  // otherwise, reverse, then return
  else 
    return vcl_vector<dbmsh3d_vertex* >(aligned_poly.rbegin(), 
    aligned_poly.rend());
}

// #################################################################

void unify_mesh_orientation (dbmsh3d_mesh *M, dbmsh3d_face* seedF)
{
}


