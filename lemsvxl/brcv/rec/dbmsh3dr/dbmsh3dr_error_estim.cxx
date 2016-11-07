//: This is dbsk3dr_error_estim.cxx
//  MingChing Chang
//  Apr, 09, 2007.

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_random.h>
#include <vgl/vgl_point_3d.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_kd_tree.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3d/algo/dbmsh3d_face_algo.h>

#include <dbmsh3dr/dbmsh3dr_match.h>


//: Find the min_dist of each point in pts2 to points in pts1.
//  result in min_dists[], size = pts2.size()
//  corresponding point ids in min_ids, size = pts2.size()
//
void compute_pp_min_dist (const vcl_vector<vgl_point_3d<double> >& pts1, 
                          const vcl_vector<vgl_point_3d<double> >& pts2, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  compute_pp_min_dist(): %d points to %d points.\n",
              pts1.size(), pts2.size());
  #endif
  min_dists.clear();
  min_ids.clear();

  //Store all the points in pts1 in the kd-tree
  const unsigned int total = pts1.size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  for (unsigned int i=0; i<pts1.size(); i++) {
    vnl_vector_fixed<double,3> P (pts1[i].x(), pts1[i].y(), pts1[i].z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }
  rsdl_kd_tree kd_tree (search_pts);

  //Loop through each pt in pts2 and find the min_dist pt in pts1
  int num_nb = 1;
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;
  const unsigned int total2 = pts2.size();
  min_dists.resize (total2);
  min_ids.resize (total2);

  for (unsigned int i=0; i<pts2.size(); i++) {
    vnl_vector_fixed<double,3> P2 (pts2[i].x(), pts2[i].y(), pts2[i].z());

    rsdl_point query_pt (3, 0);
    query_pt.set_cartesian (P2);
    kd_tree.n_nearest (query_pt, num_nb, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == 1);
    int id = near_neighbor_indices[0];
    min_ids[i] = id;
    min_dists[i] = vgl_distance (pts2[i], pts1[id]);
  }
}

//: Find the min_dist of each point in idpts2 to points in idpts1.
//  result in min_dists[], size = idpts2.size()
//  corresponding point ids in min_ids, size = idpts2.size()
//
void compute_pp_min_dist (const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts1, 
                          const vcl_vector<vcl_pair<int, vgl_point_3d<double> > >& idpts2, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  compute_pp_min_dist(): %d points to %d points.\n",
              idpts1.size(), idpts2.size());
  #endif
  min_dists.clear();
  min_ids.clear();

  //Store all the points in idpts1 in the kd-tree
  const unsigned int total = idpts1.size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  for (unsigned int i=0; i<idpts1.size(); i++) {
    vnl_vector_fixed<double,3> P (idpts1[i].second.x(), idpts1[i].second.y(), idpts1[i].second.z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }
  rsdl_kd_tree kd_tree (search_pts);

  //Loop through each pt in idpts2 and find the min_dist pt in idpts1
  int num_nb = 1;
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;
  const unsigned int total2 = idpts2.size();
  min_dists.resize (total2);
  min_ids.resize (total2);

  for (unsigned int i=0; i<idpts2.size(); i++) {
    vnl_vector_fixed<double,3> P2 (idpts2[i].second.x(), idpts2[i].second.y(), idpts2[i].second.z());

    rsdl_point query_pt (3, 0);
    query_pt.set_cartesian (P2);
    kd_tree.n_nearest (query_pt, num_nb, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == 1);
    int id = near_neighbor_indices[0];
    min_ids[i] = idpts1[id].first;
    min_dists[i] = vgl_distance (idpts2[i].second, idpts1[id].second);
  }
}

//: Find the min_dist of each point in PS2 to points in PS1.
//  result in min_dists[], size = PS2.size()
//  corresponding point ids in min_ids, size = PS2.size()
//
void compute_pp_min_dist (dbmsh3d_pt_set* PS1, dbmsh3d_pt_set* PS2,
                          vcl_vector<double>& min_dists,
                          vcl_vector<int>& min_ids)
{
  
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  compute_pp_min_dist(): %d points to %d points.\n",
              PS1->vertexmap().size(), PS2->vertexmap().size());
  #endif
  min_dists.clear();
  min_ids.clear();

  //Store all the points in PS1 in the kd-tree
  int total = PS1->vertexmap().size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  vcl_map<int, dbmsh3d_vertex*>::iterator it = PS1->vertexmap().begin();
  for (unsigned int i=0; it != PS1->vertexmap().end(); it++, i++) {
    dbmsh3d_vertex* V = (*it).second;
    assert (V->id() == i);
    vnl_vector_fixed<double,3> P (V->pt().x(), V->pt().y(), V->pt().z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (P);
  }
  rsdl_kd_tree kd_tree (search_pts);

  //Loop through each pt in PS2 and find the min_dist pt in PS1
  int num_nb = 1;
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;
  int total2 = PS2->vertexmap().size();
  min_dists.resize (total2);
  min_ids.resize (total2);

  vcl_map<int, dbmsh3d_vertex*>::iterator it2 = PS2->vertexmap().begin();
  for (unsigned int i=0; it2 != PS2->vertexmap().end(); it2++, i++) {
    dbmsh3d_vertex* V2 = (*it2).second;
    vnl_vector_fixed<double,3> P2 (V2->pt().x(), V2->pt().y(), V2->pt().z());

    rsdl_point query_pt (3, 0);
    query_pt.set_cartesian (P2);
    kd_tree.n_nearest (query_pt, num_nb, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == 1);
    int id = near_neighbor_indices[0];
    dbmsh3d_vertex* closestV = PS1->vertexmap (id);
    assert (closestV->id() == id);
    min_ids[i] = id;
    min_dists[i] = vgl_distance (V2->pt(), closestV->pt());
  }
}

//: Find the min_dist of each point in M2 to faces in M1.
//  result in min_dists[], size = pts2.size()
//  corresponding point ids in min_ids, size = pts2.size()
//
void compute_pf_min_dist (dbmsh3d_mesh* M1, dbmsh3d_mesh* M2, const int top_n, 
                          vcl_vector<double>& min_dists,
                          vcl_vector<vgl_point_3d<double> >& closest_pts)
{
  
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  compute_pf_min_dist(): top_n %d M1: V %d F %d M2: V %d F %d.\n",
              top_n, M1->vertexmap().size(), M1->facemap().size(), 
              M2->vertexmap().size(), M2->facemap().size());
  #endif
  min_dists.clear();
  closest_pts.clear();

  //Put all vertices of mesh 1 into kd-tree.
  rsdl_kd_tree* kd_tree = dbmsh3d_build_kdtree_vertices (M1);

  //mesh-mesh distance.
  dbmsh3d_mesh_mesh_dist (M1, kd_tree, M2, top_n, min_dists, closest_pts);

  delete kd_tree;
}

bool get_error_estim (const vcl_vector<double>& min_dists, const double& dist_th, 
                      double& min, double& mean, double& median, double& RMS, double& max)
{
  //Only count the dist with dist_th.
  unsigned int count = 0;
  vcl_vector<double> tmp;
  //Compute min, mean, RMS, and max dist.
  min = DBL_MAX;
  mean = 0;
  RMS = 0;
  max = 0;
  for (unsigned int i=0; i<min_dists.size(); i++) {
    if (min_dists[i] > dist_th)
      continue;
    count++;
    tmp.push_back (min_dists[i]);

    if (min > min_dists[i])
      min = min_dists[i];
    mean += min_dists[i];
    RMS += min_dists[i]*min_dists[i];
    if (max < min_dists[i])
      max = min_dists[i];
  }
  vul_printf (vcl_cout, "  get_error_estim(): Among %u dists, %u within th %f\n",
              min_dists.size(), count, dist_th);
  if (count == 0)
    return false;
  mean /= count;
  RMS /= count;
  RMS = vcl_sqrt (RMS);
  //Compute median dist (of the ones within dist_th) on a temp vector.
  //Note that vcl_nth_element() will distory the order in the vector.  
  vcl_nth_element (tmp.begin(),
                   tmp.begin() + int(tmp.size()/2), 
                   tmp.end());
  median = *(tmp.begin() + int(tmp.size()/2));
  tmp.clear();  
  vul_printf (vcl_cout, "  min: %f, mean: %f, median: %f, RMS: %f, max: %f.\n", 
              min, mean, median, RMS, max);
  return true;
}

void normalize_min_dist (const double dist_th, vcl_vector<double>& min_dists)
{
  vul_printf (vcl_cout, "  normalize_min_dist(): dist_th %f on %u measurements.\n", 
              dist_th, min_dists.size());

  for (unsigned int i=0; i<min_dists.size(); i++)
    min_dists[i] = min_dists[i] / dist_th;
}

void compute_imprv_of_error (const vcl_vector<double>& min_dists_1,
                             const vcl_vector<double>& min_dists_2,
                             const double& dist_th, 
                             double& min, double& mean, double& median, double& RMS, double& max)
{

  //Only count the distance of i-th vertex if di1 or di2 within dist_th.
  unsigned count = 0;
  vcl_vector<double> tmp;

  //Compute min, mean, RMS, and max dist.
  min = DBL_MAX;
  mean = 0;
  RMS = 0;
  max = 0;
  assert (min_dists_1.size() == min_dists_2.size());
  for (unsigned int i=0; i<min_dists_1.size(); i++) {
    if (min_dists_1[i] > dist_th || min_dists_2[i] > dist_th)
      continue;

    //Note that the diff is a signed measurement!
    double diff = min_dists_2[i] - min_dists_1[i];
    count++;
    tmp.push_back (diff);

    if (min > diff)
      min = diff;
    mean += diff;
    RMS += diff*diff;
    if (max < diff)
      max = diff;
  }
  mean /= count;
  RMS /= count;
  RMS = vcl_sqrt (RMS);

  //Compute median diff on a temp vector.
  vcl_nth_element (tmp.begin(),
                   tmp.begin() + int(tmp.size()/2), 
                   tmp.end());
  median = *(tmp.begin() + int(tmp.size()/2));
  tmp.clear();
  
  vul_printf (vcl_cout, "  compute_imprv_of_error(): Among %u dists, %u out of th %f\n",
              min_dists_1.size(), min_dists_1.size()-count, dist_th);
  vul_printf (vcl_cout, "    min: %f, mean: %f, median: %f,\n", min, mean, median);
  vul_printf (vcl_cout, "    RMS: %f, max: %f.\n", RMS, max);
}

// ###################################################################

//: return false and the problematic quert pt if test fail.
bool test_perturb_compute_dist (dbmsh3d_mesh* M, const int iteration,
                                const int top_n, const float perturb,
                                vgl_point_3d<double>& pt)
{
  vul_printf (vcl_cout, "\ntest_perturb_compute_dist(): top_n: %d, mesh P: %d F: %d.\n",
              top_n, M->vertexmap().size(), M->facemap().size());
  
  //Put all vertices of mesh 1 into kd-tree.
  rsdl_kd_tree* kd_tree = dbmsh3d_build_kdtree_vertices (M);
  
  vgl_point_3d<double> G;
  vnl_random rand (12345);

  vul_printf (vcl_cout, "  Performing test...\n");

  for (int i=0; i<iteration; i++) {
    //Loop through each vertex of M.
    vcl_map<int, dbmsh3d_vertex*>::iterator it = M->vertexmap().begin();
    for (; it != M->vertexmap().end(); it++) {
      dbmsh3d_vertex* V = (*it).second;

      //perturb the point
      double x = V->pt().x() + perturb * rand.drand32 (perturb);
      double y = V->pt().y() + perturb * rand.drand32 (perturb);
      double z = V->pt().z() + perturb * rand.drand32 (perturb);
      pt.set (x, y, z);

      //compute the approx. point-mesh distance.
      double d = dbmsh3d_pt_mesh_dist (pt, M, kd_tree, top_n, G);

      //brute-forcely compute the exact point-mesh distance.
      double d_exact = dbmsh3d_pt_mesh_dist_bf (pt, M, G);

      if (dbgl_eq_m (d, d_exact) == false) {
        vul_printf (vcl_cout, "\n  Problematic query point: (%lf, %lf, %lf).\n",
                    pt.x(), pt.y(), pt.z());
        vul_printf (vcl_cout, "     perturbed from V %d, iter %d, top_n %d.\n", V->id(), i, top_n);
        delete kd_tree;
        return false; //fail.
      }
    }

    vul_printf (vcl_cout, "  iter %d\n", i);
  }
  
  delete kd_tree;
  return true;
}

//: find the closest face in the 1-ring
void find_P_closest_face (const vgl_point_3d<double>& queryP, 
                          dbmsh3d_vertex* V,
                          double& dist_f, int& fid)
{
  vcl_set<dbmsh3d_face*> incident_faces;
  V->get_incident_Fs (incident_faces);

  dist_f = DBL_MAX;
  vcl_set<dbmsh3d_face*>::iterator it = incident_faces.begin();
  for (; it != incident_faces.end(); it++) {
    dbmsh3d_face* F = (*it);

    //find the dist (P, F)    
    vgl_point_3d<double> footpt;
    double dist;
    dbmsh3d_get_footpt_face (queryP, F, footpt, dist);

    if (dist < dist_f) {
      dist_f = dist;
      fid = F->id();
    }
  }
}

// ###################################################################


