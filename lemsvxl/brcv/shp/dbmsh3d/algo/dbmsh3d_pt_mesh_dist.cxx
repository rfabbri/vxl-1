//: This is dbmsh3d/algo/dbmsh3d_pt_mesh_dist.cxx
//

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>
#include <rsdl/rsdl_kd_tree.h>

#include <dbgl/algo/dbgl_distance.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_triangulate.h>

//: Compute the closest distance d of P to a mesh face polygon FF:
//  If the foot point F is inside the face FF, d = PF, F = G.
//  Otherwise d = PG, where G is on the boundary of FF.
double dbmsh3d_pt_face_dist_3d (const vgl_point_3d<double>& P, const dbmsh3d_face* FF,
                                vgl_point_3d<double>& G)
{
  vcl_vector<dbmsh3d_edge*> bnd_Es;
  FF->get_bnd_Es (bnd_Es);

  if (bnd_Es.size() == 3) {
    dbmsh3d_halfedge* HF = FF->halfedge();
    dbmsh3d_vertex* A = HF->edge()->sV();
    dbmsh3d_vertex* B = HF->edge()->eV();
    HF = HF->next();
    dbmsh3d_vertex* C;
    if (HF->edge()->is_V_incident(A))
      C = HF->edge()->other_V (A);
    else
      C = HF->edge()->other_V (B);

    return dbgl_pt_tri_dist_3d (P, A->pt(), B->pt(), C->pt(), G);
  }
  else {
    //Triangulate the mesh face and check all triangles.
    vcl_vector<dbmsh3d_vertex*> vertices;
    FF->get_bnd_Vs (vertices);
    vcl_vector<vcl_vector<int> > tri_faces;
    dbmsh3d_triangulate_face (vertices, tri_faces);

    double min_dist = DBL_MAX;
    vgl_point_3d<double> g;

    for (unsigned int i=0; i<tri_faces.size(); i++) {
      const dbmsh3d_vertex* A = vertices[tri_faces[i][0]];
      const dbmsh3d_vertex* B = vertices[tri_faces[i][1]];
      const dbmsh3d_vertex* C = vertices[tri_faces[i][2]];

      double d = dbgl_pt_tri_dist_3d (P, A->pt(), B->pt(), C->pt(), g);
      if (d < min_dist) {
        G = g;
        min_dist = d;
      }
    }
    return min_dist;
  }
}

rsdl_kd_tree* dbmsh3d_build_kdtree_pts (const vcl_vector<vgl_point_3d<double> >& pts)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  dbmsh3d_build_kdtree_idpts(): %d vertices.\n", pts.size());
  #endif

  //Store all the points in the kd-tree
  unsigned int total = pts.size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  for (unsigned int i=0; i<total; i++) {
    vnl_vector_fixed<double,3> P (pts[i].x(), pts[i].y(), pts[i].z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }
  rsdl_kd_tree* kd_tree = new rsdl_kd_tree (search_pts);
  return kd_tree;
}

rsdl_kd_tree* dbmsh3d_build_kdtree_idpts (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  dbmsh3d_build_kdtree_idpts(): %d vertices.\n",
              idpts.size());
  #endif

  //Store all the points in the kd-tree
  unsigned int total = idpts.size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  for (unsigned int i=0; i<total; i++) {
    vgl_point_3d<double> pt = idpts[i].second;
    vnl_vector_fixed<double,3> P (pt.x(), pt.y(), pt.z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }

  rsdl_kd_tree* kd_tree = new rsdl_kd_tree (search_pts);
  return kd_tree;
}

rsdl_kd_tree* dbmsh3d_build_kdtree_vertices (dbmsh3d_pt_set* PS)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  dbmsh3d_build_kdtree_vertices(): %d vertices.\n",
              PS->vertexmap().size());
  #endif

  //Store all the points in M in the kd-tree
  int total = PS->vertexmap().size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = PS->vertexmap().begin();
  for (unsigned int i=0; vit != PS->vertexmap().end(); vit++, i++) {
    dbmsh3d_vertex* V = (*vit).second;
    assert (V->id() == i);
    vnl_vector_fixed<double,3> P (V->pt().x(), V->pt().y(), V->pt().z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }

  rsdl_kd_tree* kd_tree = new rsdl_kd_tree (search_pts);
  return kd_tree;
}

double dbmsh3d_pt_mesh_dist (const vgl_point_3d<double>& P, 
                             dbmsh3d_vertex* closestV, vgl_point_3d<double>& G)
{
  double min_dist = DBL_MAX;

  double d = vgl_distance (P, closestV->pt());
  if (d < min_dist) { //Update the closest point G and min_dist.
    min_dist = d;
    G = closestV->pt();
  }

  //Loop through the 1-ring faces of V.
  vcl_set<dbmsh3d_face*> incident_faces;
  closestV->get_incident_Fs (incident_faces);

  //Loop through the check_faces and try if a shorter point-to-face dist can be found.  
  vcl_set<dbmsh3d_face*>::iterator fit = incident_faces.begin();
  for (; fit != incident_faces.end(); fit++) {
    dbmsh3d_face* FF = *fit;
    vgl_point_3d<double> GF;
    d = dbmsh3d_pt_face_dist_3d (P, FF, GF);
    assert (d >= 0);

    if (d < min_dist) {
      min_dist = d;
      G = GF;
    }    
  }

  return min_dist;
}

//: Approximate the closest point G from a point P to a mesh M:
//  Use kd-tree to store all vertices of M and determine the 
//  closest n vertices [V1, ..., vn] from P.
//  Search the 1-ring faces of each Vi using the above point-triangle distance.
//  d (P, M) = min [d (P, Vi), d (P, Gj)].
//
double dbmsh3d_pt_mesh_dist (const vgl_point_3d<double>& P, 
                             dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree,
                             const int top_n, vgl_point_3d<double>& G)
{
  //Find the closest top_n vertces [V1, ..., Vn] from P to M
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;

  rsdl_point query_pt (3, 0);
  vnl_vector_fixed<double,3> P3 (P.x(), P.y(), P.z());
  query_pt.set_cartesian (P3);
  kd_tree->n_nearest (query_pt, top_n, near_neighbor_pts, near_neighbor_indices);
  assert (near_neighbor_indices.size() == top_n);

  double d, min_dist = DBL_MAX;
    
  //Check the top_n vertices and their 1-ring faces.
  vcl_set<dbmsh3d_face*> check_faces;
  unsigned int total_faces_examined = 0;

  for (unsigned int i=0; i<near_neighbor_indices.size(); i++) {
    int id = near_neighbor_indices[i];
    dbmsh3d_vertex* closestV = M->vertexmap (id);
    assert (closestV->id() == id);    

    d = vgl_distance (P, closestV->pt());
    if (d < min_dist) { //Update the closest point G and min_dist.
      min_dist = d;
      G = closestV->pt();
    }

    //Loop through the 1-ring faces of V.
    vcl_set<dbmsh3d_face*> incident_faces;
    incident_faces.clear();
    total_faces_examined += closestV->get_incident_Fs (incident_faces);
    check_faces.insert (incident_faces.begin(), incident_faces.end());    
  }

  //Make sure that faces of non-isolated vertices are visited!
  assert (M->is_MHE());
  if (M->is_MHE() == false) {
    vul_printf (vcl_cout, "\n\t ERROR: mesh is not MHE in computing pt-mesh distance.\n");
  }

  //Loop through the check_faces and try if a shorter point-to-face dist can be found.  
  vcl_set<dbmsh3d_face*>::iterator fit = check_faces.begin();
  for (; fit != check_faces.end(); fit++) {
    dbmsh3d_face* FF = *fit;
    vgl_point_3d<double> GF;
    d = dbmsh3d_pt_face_dist_3d (P, FF, GF);
    assert (d >= 0);

    if (d < min_dist) {
      min_dist = d;
      G = GF;
    }    
  }

  return min_dist;
}

//: Brute-forcely compute the exact closest point G from a point P to a mesh M.
double dbmsh3d_pt_mesh_dist_bf (const vgl_point_3d<double>& P, 
                                dbmsh3d_mesh* M, vgl_point_3d<double>& G)
{
  double min_dist = DBL_MAX;

  //Loop through all mesh faces and compute the closest point G

  vcl_map<int, dbmsh3d_face*>::iterator it = M->facemap().begin();
  for (; it != M->facemap().end(); it++) {
    dbmsh3d_face* F = (*it).second;
    vgl_point_3d<double> GF;
    double d = dbmsh3d_pt_face_dist_3d (P, F, GF);
    assert (d >= 0);

    if (d < min_dist) {
      min_dist = d;
      G = GF;
    } 
  }

  return min_dist;
}


//: Return the approximate closest distance between each vertex of M2
//  to the closest point (can be vertex or interior pt on a face) of M1
//  Return the min_dist.
//
double dbmsh3d_mesh_mesh_dist (dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree, dbmsh3d_mesh* M2, 
                               const int top_n, vcl_vector<double>& min_dists,
                               vcl_vector<vgl_point_3d<double> >& closest_pts)
{
  double min_dist = DBL_MAX;
  vgl_point_3d<double> G;
  assert (min_dists.size() == 0);
  assert (closest_pts.size() == 0);

  //Loop through all vertices of M2
  vcl_map<int, dbmsh3d_vertex*>::iterator it = M2->vertexmap().begin();
  for (; it != M2->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    double dist = dbmsh3d_pt_mesh_dist (V->pt(), M, kd_tree, top_n, G);
    assert (dist >= 0);
    min_dists.push_back (dist);
    closest_pts.push_back (G);

    if (dist < min_dist)
      min_dist = dist;
  }

  return min_dist;
}

//: Estimate avg_samp_dist by avg. of k-N-N.
double estimate_avg_samp_dist (const vcl_vector<vgl_point_3d<double> >& pts, const int k)
{
  double avg_dist = 0;
  unsigned int count = 0;

  //Build a kd-tree on all input points for k-Nearest Neighbor query.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_pts (pts);

  for (unsigned int i=0; i<pts.size(); i++) {
    //Find the k closest points.
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;
    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (pts[i].x(), pts[i].y(), pts[i].z());
    query_pt.set_cartesian (P3);
    kdtree->n_nearest (query_pt, k, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == k);

    //The closest point is pts[i] itself
    assert (near_neighbor_indices[0] == i);

    //Count other k-1 nearest points for avg_dist.
    for (int j=1; j<k; j++) {
      double dist = vgl_distance (pts[i], pts[near_neighbor_indices[j]]);
      avg_dist += dist;
      count++;
    }
  }

  avg_dist /= count;
  delete kdtree;
  return avg_dist;
}

//: Estimate avg_samp_dist by avg. of k-N-N.
double estimate_avg_samp_dist (dbmsh3d_pt_set* PS, const int k)
{
  double avg_dist = 0;
  unsigned int count = 0;

  //Build a kd-tree on all input points for k-Nearest Neighbor query.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_vertices (PS);

  vcl_map<int, dbmsh3d_vertex*>::iterator it = PS->vertexmap().begin();
  for (unsigned int i=0; it != PS->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;

    //Find the k closest points.
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;
    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (V->pt().x(), V->pt().y(), V->pt().z());
    query_pt.set_cartesian (P3);
    kdtree->n_nearest (query_pt, k, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == k);

    //The closest point is pts[i] itself
    assert (near_neighbor_indices[0] == i);

    //Count other k-1 nearest points for avg_dist.
    for (int j=1; j<k; j++) {
      int id = near_neighbor_indices[j];
      dbmsh3d_vertex* Vj = PS->vertexmap(id);
      double dist = vgl_distance (V->pt(), Vj->pt());
      avg_dist += dist;
      count++;
    }
  }

  avg_dist /= count;
  delete kdtree;
  return avg_dist;
}

double dbmsh3d_mesh_mesh_avg_dist (dbmsh3d_mesh* M, rsdl_kd_tree* kd_tree, 
                                   dbmsh3d_mesh* M2, const int top_n, const double dist_th)
{
  vcl_vector<double> min_dists;
  vcl_vector<vgl_point_3d<double> > closest_pts;

  dbmsh3d_mesh_mesh_dist (M, kd_tree, M2, top_n, min_dists, closest_pts);

  double avg_dist = 0;
  unsigned int sz = 0;
  for (unsigned int i=0; i< min_dists.size(); i++) {
    if (min_dists[i] < dist_th) {
      avg_dist += min_dists[i];
      sz++;
    }
  }
  assert (avg_dist >= 0);

  if (sz > 0)
    avg_dist /= sz;
  return avg_dist;
}


