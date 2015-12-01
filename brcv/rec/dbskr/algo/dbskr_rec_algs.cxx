// This is brcv/rec/dbskr/algo/dbskr_rec_algs.cxx
#include <vcl_algorithm.h>

#include "dbskr_rec_algs.h"
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/dbskr_sm_cor.h>
#include <dbskr/dbskr_tree_edit_combined.h>
#include <dbskr/dbskr_tree_edit_coarse.h>

#include <dbgrl/algo/dbgrl_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>

#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <bsol/bsol_intrinsic_curve_2d.h>

#include <dbcvr/dbcvr_cvmatch_sptr.h>
#include <dbcvr/dbcvr_cvmatch.h>
#include <dbcvr/dbcvr_cv_cor.h>

#include <vgl/vgl_distance.h>
#include <vul/vul_timer.h>

#include <vil/vil_save.h>
#include <vul/vul_psfile.h>
#include <vil/vil_image_resource.h>

#if 0
//: method that traverses a shock graph starting from a given node in a breadth first manner
//  visiting each edge (degree two nodes are merged) only once and saving the edges
//  visited_node_depth_map is used to visit each node once
//  node_edges_map keeps starting node of a vector of shock edges (degree twos are merged)
void visit_and_keep_edges(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr start_node, 
                             vcl_map<dbsk2d_shock_node_sptr, int>& visited_node_depth_map,
                             vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >& node_edges_map,
                             vcl_vector<vcl_pair<dbsk2d_shock_node_sptr, int> >& to_visit,
                             int depth)
{
  if (depth == 0) {
    visited_node_depth_map[start_node] = 0;
    return;
  }

  dbsk2d_shock_edge_sptr first_edge_outer = sg->first_adj_edge(start_node);
  dbsk2d_shock_edge_sptr current_edge_outer = first_edge_outer;
  do {
    
    // collect all the edges till the next degree three or degree 1 node on this branch
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    dbsk2d_shock_node_sptr current_node = start_node;
    dbsk2d_shock_edge_sptr current_edge = current_edge_outer;  // first edge on the path is current_edge_outer
    dbsk2d_shock_node_sptr next_node = current_edge->opposite(current_node);
    while (next_node && next_node->degree() == 2) { 
      edges.push_back(current_edge);    
      current_node = next_node;
      current_edge = sg->cyclic_adj_succ(current_edge, current_node); 
      if (current_edge)
        next_node = current_edge->opposite(current_node);
      else 
        break;
    }

    if (next_node) {
      //if (next_node->degree() >= 3 && next_node->degree() == 1)
      edges.push_back(current_edge);

      vcl_map<dbsk2d_shock_node_sptr, int>::iterator iter = visited_node_depth_map.find(next_node);
      if (iter == visited_node_depth_map.end()) { // not visited before
        vcl_pair< dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> p(start_node, next_node);
        node_edges_map[p] = edges;
        //if (depth-1 != 0) {
          vcl_pair<dbsk2d_shock_node_sptr, int> pp(next_node, depth-1);
          to_visit.push_back(pp);
        //} else
        //  visited_node_depth_map[next_node] = 0;
      } 
    }
    
    current_edge_outer = sg->cyclic_adj_succ(current_edge_outer, start_node);
  } while (first_edge_outer != current_edge_outer);

  visited_node_depth_map[start_node] = depth;
  return;
}

void get_scurves(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr start_node,  
                 vcl_vector<dbskr_scurve_sptr>& cur_scurves, 
                 vcl_vector<bool>& end_scurve, int depth, bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds)
{

  vcl_vector<vcl_pair<dbsk2d_shock_node_sptr, int> > to_visit;
  to_visit.push_back(vcl_pair<dbsk2d_shock_node_sptr, int> (start_node, depth));

  vcl_map<dbsk2d_shock_node_sptr, int> visited_node_depth_map;
  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> > node_edges_map;

  while (to_visit.size() != 0) {
    vcl_pair<dbsk2d_shock_node_sptr, int> p = to_visit[0];
    dbsk2d_shock_node_sptr node = p.first;
    to_visit.erase(to_visit.begin());
    visit_and_keep_edges(sg, node, visited_node_depth_map, node_edges_map, to_visit, p.second);
  }

  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >::iterator iter;
  iter = node_edges_map.begin();
  for ( ; iter != node_edges_map.end(); iter++) {
    vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> node_pair = iter->first;
    vcl_vector<dbsk2d_shock_edge_sptr> edges = iter->second;
      
    dbskr_scurve_sptr scurve = dbskr_compute_scurve(node_pair.first, edges, false, 
                                                      binterpolate, subsample, 
                                                      interpolate_ds, subsample_ds);
    cur_scurves.push_back(scurve);
    // check if the end_node was visited at depth 0
    vcl_map<dbsk2d_shock_node_sptr, int>::iterator visited_iter = visited_node_depth_map.find(node_pair.second);
    if (node_pair.second->degree() == 1)
      end_scurve.push_back(true);
    else {  // there should not be any other edge that starts with this node for it to be an end-node
      vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >::iterator i;
      i = node_edges_map.begin();
      bool no_other = true;
      for ( ; i != node_edges_map.end(); i++) {
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np = i->first;
        if (np.first == node_pair.second) {
          no_other = false;
          break;
        }
      }
      end_scurve.push_back(no_other);
    }
  }

  //print for debuggin
  vcl_cout << "----------------visited nodes:--------------\n";
  for (vcl_map<dbsk2d_shock_node_sptr, int>::iterator vi = visited_node_depth_map.begin(); vi != visited_node_depth_map.end(); vi++) {
    vcl_cout << "id: " << vi->first->id() << " depth: " << vi->second << vcl_endl;
  }

  return;
}

dbsk2d_shock_edge_sptr get_first_edge(vcl_map<dbskr_scurve_sptr, vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> >& scurve_nodes_map,
                                      vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >& node_edges_map,
                                      dbskr_scurve_sptr scurve)
{
  vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np = scurve_nodes_map[scurve];
  return (node_edges_map[np])[0];
}

#define LEFT true
#define RIGHT false

#define START_FROM_BEGINNING true
#define START_FROM_END       false

void add_points(vcl_vector<vsol_point_2d_sptr>& pts, dbskr_scurve_sptr scurve, bool side, bool dir)
{
  int size = scurve->num_points();

  if (side && dir) { // traverse this scurve in order and take its LEFT boundary
    for (int j = 0; j<size; j++)
      pts.push_back(new vsol_point_2d(scurve->bdry_plus_pt(j)));
    pts.push_back(new vsol_point_2d(scurve->sh_pt(size-1)));

  } else if (side && !dir) { // traverse this scurve in reverse order and take its LEFT boundary
    pts.push_back(new vsol_point_2d(scurve->sh_pt(size-1)));
    for (int j = size-1; j>=0; j--)
      pts.push_back(new vsol_point_2d(scurve->bdry_plus_pt(j)));

  } else if (!side && dir) { // traverse this scurve in order and take its RIGHT boundary
    for (int j = 0; j<size; j++)
      pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));
    pts.push_back(new vsol_point_2d(scurve->sh_pt(size-1)));

  } else if (!side && !dir) {
    pts.push_back(new vsol_point_2d(scurve->sh_pt(size-1)));
    for (int j = size-1; j>=0; j--)
      pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));
  } 
}

void add_points_no_shock(vcl_vector<vsol_point_2d_sptr>& pts, dbskr_scurve_sptr scurve, bool side, bool dir)
{
  int size = scurve->num_points();

  if (side && dir) { // traverse this scurve in order and take its LEFT boundary
    for (int j = 0; j<size; j++)
      pts.push_back(new vsol_point_2d(scurve->bdry_plus_pt(j)));

  } else if (side && !dir) { // traverse this scurve in reverse order and take its LEFT boundary
    for (int j = size-1; j>=0; j--)
      pts.push_back(new vsol_point_2d(scurve->bdry_plus_pt(j)));

  } else if (!side && dir) { // traverse this scurve in order and take its RIGHT boundary
    for (int j = 0; j<size; j++)
      pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));

  } else if (!side && !dir) {
    for (int j = size-1; j>=0; j--)
      pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));
  } 
}

// find the next scurve with respect to this scurve_node_pair starting with next_edge
dbskr_scurve_sptr get_next_scurve(dbsk2d_shock_edge_sptr next_edge, dbsk2d_shock_node_sptr start_node,
                                  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >& node_edges_map,
                                  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, dbskr_scurve_sptr >& nodes_scurve_map)
{
  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >::iterator iter2;
  iter2 = node_edges_map.begin();
  vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np2;
  bool found_it = false;
  for ( ; iter2 != node_edges_map.end(); iter2++) {
    np2 = iter2->first;
    if (np2.first != start_node)
      continue;
    //if (np2.second == scurve_node_pair.second)
    //  continue;
    if (iter2->second[0] == next_edge) {
      found_it = true;
      break;  // found our iterator
    }
  }
  if (found_it)
    return nodes_scurve_map[np2];
  else 
    return 0;
}

bool an_end_scurve(dbskr_scurve_sptr next_scurve, vcl_vector<dbskr_scurve_sptr>& cur_scurves, vcl_vector<bool>& end_scurve,
                   vcl_map<dbskr_scurve_sptr, vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>> & scurve_nodes_map,
                   vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, dbskr_scurve_sptr > & nodes_scurve_map
                   ) {
  for (unsigned i = 0; i < cur_scurves.size(); i++)
    if (cur_scurves[i] == next_scurve)
      return end_scurve[i];
  
  // check its reverse as well
  vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np = scurve_nodes_map[next_scurve];
  vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np_rev;
  np_rev.first = np.second;
  np_rev.second = np.first;
  dbskr_scurve_sptr new_scurve = nodes_scurve_map[np_rev];
  for (unsigned i = 0; i < cur_scurves.size(); i++)
    if (cur_scurves[i] == new_scurve)
      return end_scurve[i];
  
  return false;
}

vsol_polygon_2d_sptr trace_boundary_from_subgraph(dbsk2d_shock_graph_sptr sg, 
                                                  dbsk2d_shock_node_sptr start_node, 
                                                  int depth, 
                                                  bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds)
{
  vcl_vector<dbskr_scurve_sptr> cur_scurves;
  vcl_vector<bool> end_scurve;

// same as  get_scurves
///////////////////////////////////////
  vcl_vector<vcl_pair<dbsk2d_shock_node_sptr, int> > to_visit;
  to_visit.push_back(vcl_pair<dbsk2d_shock_node_sptr, int> (start_node, depth));

  vcl_map<dbsk2d_shock_node_sptr, int> visited_node_depth_map;
  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> > node_edges_map;
  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, dbskr_scurve_sptr > nodes_scurve_map;
  vcl_map<dbskr_scurve_sptr, vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>> scurve_nodes_map;

  while (to_visit.size() != 0) {
    vcl_pair<dbsk2d_shock_node_sptr, int> p = to_visit[0];
    dbsk2d_shock_node_sptr node = p.first;
    to_visit.erase(to_visit.begin());
    visit_and_keep_edges(sg, node, visited_node_depth_map, node_edges_map, to_visit, p.second);
  }

  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >::iterator iter;
  iter = node_edges_map.begin();
  vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> > rev_nodes_edges_map;
  for ( ; iter != node_edges_map.end(); iter++) {
    vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> node_pair = iter->first;
    vcl_vector<dbsk2d_shock_edge_sptr> edges = iter->second;
      
    dbskr_scurve_sptr scurve = dbskr_compute_scurve(node_pair.first, edges, false, 
                                                      binterpolate, subsample, 
                                                      interpolate_ds, subsample_ds);
    cur_scurves.push_back(scurve);
    nodes_scurve_map[node_pair] = scurve;
    scurve_nodes_map[scurve] = node_pair;

    // at reverse sides as well for quick access later
    vcl_vector<dbsk2d_shock_edge_sptr> rev_edges(edges.rbegin(), edges.rend());
    vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> node_pair_rev;
    node_pair_rev.first = node_pair.second;
    node_pair_rev.second = node_pair.first;

    dbskr_scurve_sptr rev_scurve = dbskr_compute_scurve(node_pair_rev.first, rev_edges, false, 
                                                      binterpolate, subsample, 
                                                      interpolate_ds, subsample_ds);
    nodes_scurve_map[node_pair_rev] = rev_scurve;
    scurve_nodes_map[rev_scurve] = node_pair_rev;
    rev_nodes_edges_map[node_pair_rev] = rev_edges;

    // check if the end_node was visited at depth 0
    vcl_map<dbsk2d_shock_node_sptr, int>::iterator visited_iter = visited_node_depth_map.find(node_pair.second);
    if (node_pair.second->degree() == 1)
      end_scurve.push_back(true);
    else {  // there should not be any other edge that starts with this node for it to be an end-node
      vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >::iterator i;
      i = node_edges_map.begin();
      bool no_other = true;
      for ( ; i != node_edges_map.end(); i++) {
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> np = i->first;
        if (np.first == node_pair.second) {
          no_other = false;
          break;
        }
      }
      end_scurve.push_back(no_other);
    }
  }
  
  //print for debuggin
  vcl_cout << "----------------visited nodes:--------------\n";
  for (vcl_map<dbsk2d_shock_node_sptr, int>::iterator vi = visited_node_depth_map.begin(); vi != visited_node_depth_map.end(); vi++) {
    vcl_cout << "id: " << vi->first->id() << " depth: " << vi->second << vcl_endl;
  }

////////////////////////////////////  
  //now trace the boundary

  // points of the final boundary
  vcl_vector<vsol_point_2d_sptr> pts;

  // start from an end point // any of them is fine!!
  unsigned i;
  for (i = 0; i < end_scurve.size(); i++)
    if (end_scurve[i]) 
      break;

  dbskr_scurve_sptr scurve_start = cur_scurves[i];

  // get the first edge of the edge group of this scurve
  dbsk2d_shock_edge_sptr edge0 = get_first_edge(scurve_nodes_map, node_edges_map, scurve_start);

  //add_points(pts, scurve_start, LEFT, START_FROM_END);  
  bool current_dir = RIGHT;
  dbskr_scurve_sptr current_scurve = scurve_start;
  //dbsk2d_shock_node_sptr next_node = scurve_nodes_map[current_scurve].first;
  while (1) {
    
    dbskr_scurve_sptr next_scurve;
    do {
      // get the next edge group using start node 
      vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> scurve_node_pair = scurve_nodes_map[current_scurve];
      dbsk2d_shock_edge_sptr next_edge = sg->cyclic_adj_succ(edge0, scurve_node_pair.first);
      
      // find the next scurve with respect to this scurve_node_pair starting with next_edge
      next_scurve = get_next_scurve(next_edge, scurve_node_pair.first, node_edges_map, nodes_scurve_map);
      if (!next_scurve) {
        dbskr_scurve_sptr rev_next_scurve = get_next_scurve(next_edge, scurve_node_pair.first, rev_nodes_edges_map, nodes_scurve_map);
        if (!rev_next_scurve)
          return 0; // for now
        add_points_no_shock(pts, rev_next_scurve, current_dir, START_FROM_BEGINNING);
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> rev_snp = scurve_nodes_map[rev_next_scurve];
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> snp;
        snp.first = rev_snp.second;
        snp.second = rev_snp.first;
        next_scurve = nodes_scurve_map[snp];
        edge0 = get_first_edge(scurve_nodes_map, node_edges_map, next_scurve);
      } else {
        add_points_no_shock(pts, next_scurve, current_dir, START_FROM_BEGINNING);
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> rev_snp = scurve_nodes_map[next_scurve];
        vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> snp;
        snp.first = rev_snp.second;
        snp.second = rev_snp.first;
        next_scurve = nodes_scurve_map[snp];
        edge0 = get_first_edge(scurve_nodes_map, rev_nodes_edges_map, next_scurve);
      }

      current_scurve = next_scurve;
      //edge0 = get_first_edge(scurve_nodes_map, node_edges_map, next_scurve); 
    }
    while (!an_end_scurve(next_scurve, cur_scurves, end_scurve, scurve_nodes_map, nodes_scurve_map));
    
    
    //add_points(pts, next_scurve, current_dir, START_FROM_BEGINNING);
    
    // reverse current again cause we're going back now!! (recovering from an end scurve)
    vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> rev_snp = scurve_nodes_map[next_scurve];
    vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr> snp;
    snp.first = rev_snp.second;
    snp.second = rev_snp.first;
    current_scurve = nodes_scurve_map[snp];

    current_dir = !current_dir;
    add_points(pts, current_scurve, current_dir, START_FROM_END);
    current_dir = !current_dir;

    if (current_scurve == scurve_start)
      break;

    edge0 = get_first_edge(scurve_nodes_map, node_edges_map, current_scurve); // this should be in our normal list
  }

  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(pts);
  return poly;
}

#endif



dbskr_v_graph_sptr construct_v_graph(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr node, int depth)
{
  // use shock node ids 
  vcl_map<dbsk2d_shock_node_sptr, vcl_pair<dbskr_v_node_sptr, int> > visited_nodes;
  vcl_map<dbsk2d_shock_node_sptr, vcl_pair<dbskr_v_node_sptr, int> >::iterator vi, vi2;
  // visit this node at this node coming from this parent edge and the created v_node
  vcl_vector<vcl_pair< vcl_pair<dbsk2d_shock_node_sptr, int>, vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> > > to_visit;
  typedef vcl_pair< vcl_pair<dbsk2d_shock_node_sptr, int>, vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> > to_visit_type;
  vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr> graph_nodes;
  //vcl_map< vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, dbskr_v_edge_sptr > edges_to_be_added;
  vcl_multimap< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_to_be_added;
  typedef vcl_multimap< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_multiset_type;
  typedef vcl_pair< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_multiset_insertion_type;

  dbskr_v_graph_sptr v_g = new dbskr_v_graph;
  dbskr_v_node_sptr root = new dbskr_v_node(node);
  v_g->add_vertex(root);
  graph_nodes[node] = root;
  
  to_visit_type vp;
  vp.first = vcl_pair<dbsk2d_shock_node_sptr, int> (node, depth);
  vp.second = vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> ((dbsk2d_shock_edge*)(0), root);  // first node has no parent edge
  to_visit.push_back(vp);

  while (to_visit.size() > 0) {
    // visit the first node on the list
    dbsk2d_shock_node_sptr current_node = to_visit[0].first.first;
    int current_depth = to_visit[0].first.second;
    dbsk2d_shock_edge_sptr parent_edge = to_visit[0].second.first;
    dbskr_v_node_sptr current_vn = to_visit[0].second.second;
    to_visit.erase(to_visit.begin());
#if 0
    vcl_cout << "--------- visiting: " << current_node->id() << " edges_to_be_added are:\n";
    for (edges_multiset_type::iterator it = edges_to_be_added.begin(); it != edges_to_be_added.end(); it++) {
      vcl_cout << it->first->id_ << " <" << it->second.first->id_ << ", (";
      for (unsigned k = 0; k < it->second.second->edges_.size(); k++)
        vcl_cout << (it->second.second->edges_)[k]->id() << ", ";
      vcl_cout << ") >\n";
    }
#endif

    vi = visited_nodes.find(current_node);
    
    if (vi == visited_nodes.end()) { // not in visited nodes add to the graph with edges to children
      vcl_pair<dbskr_v_node_sptr, int> p(current_vn, current_depth);
      visited_nodes[current_node] = p;

      dbsk2d_shock_edge_sptr first_edge_outer, last_edge_outer;
      if (parent_edge) 
        first_edge_outer = parent_edge;
      else 
        first_edge_outer = sg->first_adj_edge(current_node);
      last_edge_outer = first_edge_outer;

      dbsk2d_shock_edge_sptr current_edge_outer = first_edge_outer;
      do {

        vcl_vector<dbsk2d_shock_edge_sptr> edges;
        dbsk2d_shock_node_sptr other_node = sg->get_other_end_merging_degree_twos(current_node, current_edge_outer, edges);
        
        vi2 = visited_nodes.find(other_node);
        if (vi2 == visited_nodes.end()) { // other node has not been visited yet, the edge to it should be created

          if (current_depth > 0) {
            // although it has not been visited, it might have been added to the graph already 
            dbskr_v_node_sptr vn_sptr;
            vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr>::iterator it = graph_nodes.find(other_node);
            if (it == graph_nodes.end()) {
              vn_sptr = new dbskr_v_node(other_node);
              v_g->add_vertex(vn_sptr);
              graph_nodes[other_node] = vn_sptr;
            } else 
              vn_sptr = it->second;

            // its the first time this edge is being created
            dbskr_v_edge_sptr ve_sptr = new dbskr_v_edge(current_vn, vn_sptr, edges, current_node->id(), other_node->id());
            v_g->add_edge(ve_sptr);
            
            current_vn->add_incoming_edge(ve_sptr);  // always add as incoming edge to make undirected graphs
            
            vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> vnep(current_vn, ve_sptr);
            edges_to_be_added.insert(edges_multiset_insertion_type (vn_sptr, vnep));   
            
            to_visit_type vp;
            vp.first = vcl_pair<dbsk2d_shock_node_sptr, int> (other_node, current_depth-1);
            vp.second = vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> (edges[edges.size()-1], vn_sptr);  // parent of other node is the last edge on the path to it
            to_visit.push_back(vp);       
          }

        } else {
          dbskr_v_node_sptr vn_sptr = graph_nodes[other_node];

          vcl_pair<edges_multiset_type::iterator, edges_multiset_type::iterator> itp = edges_to_be_added.equal_range(current_vn);
          if (itp.first != edges_to_be_added.end()) {
            edges_multiset_type::iterator i;
            bool found = false;
            for (i = itp.first; i != itp.second; i++) {
              vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> p = i->second;
              if (p.first == vn_sptr && p.second->edges_[0] == edges[edges.size()-1]) {
                current_vn->add_incoming_edge(p.second);
                found = true;
                break;
              }
            }
            if (found)
              edges_to_be_added.erase(i);
          }
        }

        current_edge_outer = sg->cyclic_adj_succ(current_edge_outer, current_node);
      } while (current_edge_outer != last_edge_outer);

    } else
      continue; 
    
  }

  return v_g;
}

//: construct v_graph of the whole shock graph, starting from any of its nodes
dbskr_v_graph_sptr construct_v_graph(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr node)
{
  // use shock node ids 
  vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr > visited_nodes;
  vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr >::iterator vi, vi2;
  // visit this node at this node coming from this parent edge and the created v_node
  vcl_vector<vcl_pair< dbsk2d_shock_node_sptr, vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> > > to_visit;
  typedef vcl_pair< dbsk2d_shock_node_sptr, vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> > to_visit_type;
  vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr> graph_nodes;
  //vcl_map< vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, dbskr_v_edge_sptr > edges_to_be_added;
  vcl_multimap< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_to_be_added;
  typedef vcl_multimap< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_multiset_type;
  typedef vcl_pair< dbskr_v_node_sptr, vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> > edges_multiset_insertion_type;

  dbskr_v_graph_sptr v_g = new dbskr_v_graph;
  dbskr_v_node_sptr root = new dbskr_v_node(node);
  v_g->add_vertex(root);
  graph_nodes[node] = root;
  
  to_visit_type vp;
  vp.first = node;
  vp.second = vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> ((dbsk2d_shock_edge*)(0), root);  // first node has no parent edge
  to_visit.push_back(vp);

  //: make sure all the shock graph is covered, there may be disconnected components in the shock graph
  bool all_covered = false;
  while (!all_covered) {

  while (to_visit.size() > 0) {
    // visit the first node on the list
    dbsk2d_shock_node_sptr current_node = to_visit[0].first;
    dbsk2d_shock_edge_sptr parent_edge = to_visit[0].second.first;
    dbskr_v_node_sptr current_vn = to_visit[0].second.second;
    to_visit.erase(to_visit.begin());

    vi = visited_nodes.find(current_node);
    
    if (vi == visited_nodes.end()) { // not in visited nodes add to the graph with edges to children
      visited_nodes[current_node] = current_vn;

      dbsk2d_shock_edge_sptr first_edge_outer, last_edge_outer;
      if (parent_edge) 
        first_edge_outer = parent_edge;
      else 
        first_edge_outer = sg->first_adj_edge(current_node);
      last_edge_outer = first_edge_outer;

      dbsk2d_shock_edge_sptr current_edge_outer = first_edge_outer;
      do {

        vcl_vector<dbsk2d_shock_edge_sptr> edges;
        dbsk2d_shock_node_sptr other_node = sg->get_other_end_merging_degree_twos(current_node, current_edge_outer, edges);
        
        vi2 = visited_nodes.find(other_node);
        if (vi2 == visited_nodes.end()) { // other node has not been visited yet, the edge to it should be created

          //if (current_depth > 0) {
            // although it has not been visited, it might have been added to the graph already 
            dbskr_v_node_sptr vn_sptr;
            vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr>::iterator it = graph_nodes.find(other_node);
            if (it == graph_nodes.end()) {
              vn_sptr = new dbskr_v_node(other_node);
              v_g->add_vertex(vn_sptr); 
              graph_nodes[other_node] = vn_sptr;
            } else 
              vn_sptr = it->second;

            // its the first time this edge is being created
            dbskr_v_edge_sptr ve_sptr = new dbskr_v_edge(current_vn, vn_sptr, edges, current_node->id(), other_node->id());
            v_g->add_edge(ve_sptr);
            
            current_vn->add_incoming_edge(ve_sptr);  // always add as incoming edge to make undirected graphs
            
            vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> vnep(current_vn, ve_sptr);
            edges_to_be_added.insert(edges_multiset_insertion_type (vn_sptr, vnep));   
            
            to_visit_type vp;
            vp.first = other_node;
            vp.second = vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> (edges[edges.size()-1], vn_sptr);  // parent of other node is the last edge on the path to it
            to_visit.push_back(vp);       
          //}

        } else {
          dbskr_v_node_sptr vn_sptr = graph_nodes[other_node];

          vcl_pair<edges_multiset_type::iterator, edges_multiset_type::iterator> itp = edges_to_be_added.equal_range(current_vn);
          if (itp.first != edges_to_be_added.end()) {
            edges_multiset_type::iterator i;
            bool found = false;
            for (i = itp.first; i != itp.second; i++) {
              vcl_pair<dbskr_v_node_sptr, dbskr_v_edge_sptr> p = i->second;
              if (p.first == vn_sptr && p.second->edges_[0] == edges[edges.size()-1]) {
                current_vn->add_incoming_edge(p.second);
                found = true;
                break;
              }
            }
            if (found)
              edges_to_be_added.erase(i);
          }
        }

        current_edge_outer = sg->cyclic_adj_succ(current_edge_outer, current_node);
      } while (current_edge_outer != last_edge_outer);

    } else
      continue; 
    
  }

  //: check if all degree 1 nodes in the shock graph are covered
  all_covered = true;
  for (dbsk2d_shock_graph::vertex_iterator it = sg->vertices_begin(); it != sg->vertices_end(); it++) {
    if ((*it)->degree() != 1)
      continue;
    dbsk2d_shock_node_sptr node = (*it);
    vcl_map<dbsk2d_shock_node_sptr, dbskr_v_node_sptr>::iterator m_itr = graph_nodes.find(node);
    if (m_itr == graph_nodes.end()) {  // not covered
      dbskr_v_node_sptr new_root = new dbskr_v_node(node);
      v_g->add_vertex(new_root);
      graph_nodes[node] = new_root;
  
      to_visit_type vp;
      vp.first = node;
      vp.second = vcl_pair<dbsk2d_shock_edge_sptr, dbskr_v_node_sptr> ((dbsk2d_shock_edge*)(0), new_root);  // first node has no parent edge
      to_visit.push_back(vp);

      all_covered = false;
      break;
    }
      
  }
  
  }

  return v_g;
}

//: min_full_depth of a shock graph is the min depth from a node at which v_graph covers the full shock tree, 
//  i.e. when the v_graph at a depth one step higher is extracted, it is exactly the same tree 
int find_min_full_depth(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr& final_node)
{
  final_node = 0;
  int min_depth = 1000000;  // CAUTION: assuming will never deal with a tree of depth 1,000,000
  bool no_degree_3 = true;
  for (dbsk2d_shock_graph::vertex_iterator v_itr = sg->vertices_begin(); v_itr != sg->vertices_end(); v_itr++)
  { 
    if (!final_node && (*v_itr)->degree() == 1) {
      final_node = (*v_itr);  // initialize final_node to the first degree 1 node
    }

    if ((*v_itr)->degree() < 3)
      continue;
    
    no_degree_3 = false;
    int depth = 1;
    dbskr_v_graph_sptr current_v = construct_v_graph(sg, *v_itr, depth);
    while (1) {
      dbskr_v_graph_sptr new_v = construct_v_graph(sg, *v_itr, depth+1);
      if (current_v->same(*new_v))
        break;
      depth++;
      current_v = new_v;
    }

    if (depth < min_depth) {
      min_depth = depth;
      final_node = *v_itr;
    }
  }

  if (no_degree_3)
    return 1;             // if no degree 3 nodes then the previous for loop exits without doing anything
                          // then true min_full_depth is 1 --> a shock graph with only degree 1 and degree 2 nodes --> a single branch in v_graph
  
  return min_depth;
}

//: for debugging
void print_v_graph(dbskr_v_graph_sptr v_g)
{
  vcl_cout << "---------------------------------\nvertices: ";
  dbskr_v_graph::vertex_iterator v = v_g->vertices_begin();
  for ( ; v != v_g->vertices_end(); v++) {
    vcl_cout << (*v)->id_ << " " << (*v)->degree() << " ";
  }
  vcl_cout << "\nedges: ";
  dbskr_v_graph::edge_iterator e = v_g->edges_begin();
  for ( ; e != v_g->edges_end(); e++) {
    vcl_cout << "(" << (*e)->start_node_id_ << ", " << (*e)->end_node_id_ << ") "; 
  }
  vcl_cout << "\n-------------------------------\n";
  
}

//: for debugging
void print_shock_graph(dbsk2d_shock_graph_sptr v_g)
{
  vcl_cout << "---------------------------------\nvertices: ";
  dbsk2d_shock_graph::vertex_iterator v = v_g->vertices_begin();
  for ( ; v != v_g->vertices_end(); v++) {
    vcl_cout << (*v)->id() << " " << (*v)->degree() << " ";
  }
  vcl_cout << "\nedges: ";
  dbsk2d_shock_graph::edge_iterator e = v_g->edges_begin();
  for ( ; e != v_g->edges_end(); e++) {
    vcl_cout << (*e)->id() << ": (" << (*e)->source()->id() << ", " << (*e)->target()->id() << ") "; 
  }
  vcl_cout << "\n-------------------------------\n";
  
}

//: makes a wrong assignment if not in the graph
//  check the id of the returned pointer if not sure that a node with a given id exists in g
dbsk2d_shock_node_sptr& get_node_sptr(dbsk2d_shock_graph_sptr g, int id)
{
  dbsk2d_shock_graph::vertex_iterator v = g->vertices_begin();
  for ( ; v != g->vertices_end(); v++) {
    if ((*v)->id() == id)
      return (*v);
  }
  return *(g->vertices_begin());
}

void get_node_map(vcl_map<int, dbsk2d_shock_node_sptr>& map, dbsk2d_shock_graph_sptr g)
{
  dbsk2d_shock_graph::vertex_iterator v = g->vertices_begin();
  for ( ; v != g->vertices_end(); v++) {
    map[(*v)->id()] = (*v);
  }
}

//: makes a wrong assignment if not in the graph
//  check the id of the returned pointer if not sure that a node with a given id exists in g
dbskr_v_node_sptr& get_node_sptr(dbskr_v_graph_sptr g, int id)
{
  dbskr_v_graph::vertex_iterator v = g->vertices_begin();
  for ( ; v != g->vertices_end(); v++) {
    if ((*v)->id_ == id)
      return (*v);
  }
  return *(g->vertices_begin());
}

void get_node_map(vcl_map<int, dbskr_v_node_sptr>& map, dbskr_v_graph_sptr g)
{
  dbskr_v_graph::vertex_iterator v = g->vertices_begin();
  for ( ; v != g->vertices_end(); v++) {
    map[(*v)->id_] = (*v);
  }
}


void get_scurves(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr start_node, 
                 vcl_vector<dbskr_scurve_sptr>& cur_scurves, 
                 vcl_vector<bool>& end_scurve, int depth, bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds)
{
  dbskr_v_graph_sptr vg = construct_v_graph(sg, start_node, depth);
  //print_v_graph(vg);
  dbskr_v_graph::edge_iterator eit = vg->edges_begin();
  for ( ; eit != vg->edges_end(); eit++) {
    dbskr_v_edge_sptr e = (*eit);
    dbsk2d_shock_node_sptr start_node;
    if (e->source()->original_shock_node_->id() == e->start_node_id_)
      start_node = e->source()->original_shock_node_;
    else
      start_node = e->target()->original_shock_node_;
    dbskr_scurve_sptr scurve = dbskr_compute_scurve(start_node, e->edges_, false, 
                                                      binterpolate, subsample, 
                                                      interpolate_ds, subsample_ds);

    cur_scurves.push_back(scurve);
    if ((*eit)->target()->degree() == 1) 
      end_scurve.push_back(true);
    else
      end_scurve.push_back(false); 
  }
}

vsol_polygon_2d_sptr trace_boundary_from_subgraph(dbsk2d_shock_graph_sptr sg, 
                                                  dbsk2d_shock_node_sptr start_node, 
                                                  int depth, bool circular_completions,
                                                  bool binterpolate, bool subsample, 
                                                  double interpolate_ds, double subsample_ds, double poly_area_threshold)
{
  dbskr_v_graph_sptr vg = construct_v_graph(sg, start_node, depth);
  //print_v_graph(vg);

  //: find an edge with an end node to start the euler tour
  dbskr_v_edge_sptr start_edge = 0;
  vsol_polygon_2d_sptr poly = 0;
  dbskr_v_graph::edge_iterator eit = vg->edges_begin();
  for ( ; eit != vg->edges_end(); eit++) {
    start_edge = (*eit);
    if (start_edge->target()->degree() != 1) {
      continue;
    }

    if (!start_edge)
      return 0;

    int start_node_id = start_edge->source()->id_;
    vcl_vector<dbskr_v_edge_sptr> edges;
    edges.push_back(start_edge);  // I want this edge to be repeated at the beginning
    euler_tour(vg, start_edge, start_edge->source(), edges);

    if (!edges.size())
      return 0;

    vcl_vector<vsol_point_2d_sptr> pts;

    dbskr_v_edge_sptr e, e_next;
    int current_node_id = start_node_id;
    for (unsigned i = 0; i < edges.size(); i++) {
      //vcl_cout << "(" << edges[i]->start_node_id_ << ", " << edges[i]->end_node_id_ << ")\n";
      
      // if e and e_next are the same then this is an end scurve
      e = edges[i];
      e_next = edges[(i+1)%edges.size()];
      
      bool end_scurve = (e == e_next);

      dbsk2d_shock_node_sptr start_node, end_node;
      vcl_vector<dbsk2d_shock_edge_sptr> scurve_edges;
      start_node = (e->source()->original_shock_node_->id() == current_node_id) ? e->source()->original_shock_node_ : e->target()->original_shock_node_;
      end_node = (e->source()->original_shock_node_->id() == current_node_id) ? e->target()->original_shock_node_ : e->source()->original_shock_node_;
      if (e->start_node_id_ == current_node_id) {
        scurve_edges.insert(scurve_edges.begin(), e->edges_.begin(), e->edges_.end());
      } else {
        scurve_edges.insert(scurve_edges.begin(), e->edges_.rbegin(), e->edges_.rend());
      }
    
      if (end_scurve) {
        dbskr_scurve_sptr scurve;
        if (end_node->degree() == 1 && circular_completions) 
          scurve = dbskr_compute_scurve(start_node, scurve_edges, true, binterpolate, subsample, interpolate_ds, subsample_ds);
        else 
          scurve = dbskr_compute_scurve(start_node, scurve_edges, false, binterpolate, subsample, interpolate_ds, subsample_ds);

        for (int j = 0; j<scurve->num_points(); j++)
          pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));

        if (end_node->degree() != 1 || !circular_completions)  
          pts.push_back(new vsol_point_2d(scurve->sh_pt(scurve->num_points()-1)));

        for (int j = scurve->num_points()-1; j>=0; j--)
          pts.push_back(new vsol_point_2d(scurve->bdry_plus_pt(j)));

        i++; // skip next edge!!  current_node_id does not change
      } else {
        dbskr_scurve_sptr scurve = dbskr_compute_scurve(start_node, scurve_edges, false, 
                                                        binterpolate, subsample, 
                                                        interpolate_ds, subsample_ds);
        for (int j = 0; j<scurve->num_points(); j++)
          pts.push_back(new vsol_point_2d(scurve->bdry_minus_pt(j)));

        current_node_id = (e->start_node_id_ == current_node_id) ? e->end_node_id_ : e->start_node_id_;
      }
    }

    poly = new vsol_polygon_2d(pts);
    double area = poly->area();
    //vcl_cout << "area is " << area << vcl_endl;
    if (area > poly_area_threshold)
      break;
    else 
      poly = 0;
  }

  return poly;
}

//: required for visualization purposes
vsol_polygon_2d_sptr trace_boundary_from_graph(dbsk2d_shock_graph_sptr sg, 
                                                  bool binterpolate, bool subsample, 
                                                  double interpolate_ds, double subsample_ds, double poly_area_threshold)
{
  dbsk2d_shock_node_sptr start_node = *(sg->vertices_begin());
  
  //: find a degree 3 node to start
  for (dbsk2d_shock_graph::vertex_iterator itr = sg->vertices_begin(); itr != sg->vertices_end(); itr++) {
    if ((*itr)->degree() == 3) {
      start_node = (*itr);
      break;
    }
  }
  
  dbsk2d_shock_edge_sptr start_edge = 0;  // start with zero for the root to find depth
  // if (sg->has_cycle()) {
  //   vcl_cout << "This graph contains a loop!! Cannot save the image\n";
  //   return 0;
  // }

  //vcl_cout << "finding depth of the following graph: " << vcl_endl;
  //print_shock_graph(sg);
  int depth = depth_no_loop(sg, start_edge, start_node);
  // //vcl_cout << "depth found to be: " << depth << vcl_endl;

  // if (depth <= 0)
  //   return 0;

  
  return trace_boundary_from_subgraph(sg, 
                                      start_node,
                                      depth,
                                      true,
                                      binterpolate, 
                                      subsample, 
                                      interpolate_ds, 
                                      subsample_ds,
                                      0);

}


//: bool circular_ends: complete scurves with circular arcs at the ends to be able to extract a shock graph closer to the subgraph that
//                      gave rise to it. After tracing though all the branches in the intrinsic shock graph which are not supported 
//                      by "real contours" of the patch (boundaries with no circular completions) are erased from the graph
//                      during matching no circular completions should be put at the scurves of this shock graph so that open curve matching is done
//                      This is a problem for end scurves that were also end scurves in the original scene shock graph
//                      however this is the price we pay to handle open portions of the real boundaries of this patch correctly
//                      See (Ozcanli, Kimia, Shock Patch Tech report) for an explanation of this issue.
dbskr_shock_patch_sptr extract_patch_from_subgraph(dbsk2d_shock_graph_sptr sg, 
                                                  dbsk2d_shock_node_sptr start_node, 
                                                  int depth, double poly_area_threshold, bool circular_ends,
                                                  bool binterpolate, bool subsample,  
                                                  double interpolate_ds, double subsample_ds)
{
  dbskr_v_graph_sptr vg = construct_v_graph(sg, start_node, depth);
  if (!vg)
    return 0;
  //print_v_graph(vg);

  dbskr_shock_patch_sptr patch = extract_patch_from_v_graph(vg, start_node->id()*100+depth, depth,
                                          poly_area_threshold, circular_ends,
                                          binterpolate, subsample, interpolate_ds, subsample_ds); 

  return patch;
}

void get_edges_on_outer_face(dbskr_v_graph_sptr vg, vcl_vector<dbskr_v_edge_sptr> &edges) {
  
  dbskr_v_graph::edge_iterator eit = vg->edges_begin();
  for ( ; eit != vg->edges_end(); eit++) {
    dbskr_v_edge_sptr start_edge = (*eit);
    if (start_edge->target()->degree() != 1) {
      continue;
    }

    edges.push_back(start_edge);  // I want this edge to be repeated at the beginning
    euler_tour(vg, start_edge, start_edge->source(), edges);
    break;
  }
}

//: the following methods is doind exact same thing as trace_boundary_from_subgraph, but creating a patch instance
dbskr_shock_patch_sptr extract_patch_from_v_graph(dbskr_v_graph_sptr vg, int id, int depth,
                                                  double poly_area_threshold, bool circular_ends,
                                                  bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds)
{
  //: find an edge with an end node to start the euler tour
  dbskr_v_edge_sptr start_edge = 0;
  vsol_polygon_2d_sptr poly = 0;
  dbskr_shock_patch_sptr patch = new dbskr_shock_patch(id, depth);  // node_id*100 + depth is a unique id
  dbskr_v_graph::edge_iterator eit = vg->edges_begin();
  for ( ; eit != vg->edges_end(); eit++) {
    start_edge = (*eit);
    if (start_edge->target()->degree() != 1) {
      continue;
    }

    if (!start_edge)
      return 0;

    int start_node_id = start_edge->source()->id_;
    vcl_vector<dbskr_v_edge_sptr> edges;
    edges.push_back(start_edge);  // I want this edge to be repeated at the beginning
    euler_tour(vg, start_edge, start_edge->source(), edges);

    //vcl_cout << "edges on outer face of patch: " << vcl_endl;
    //for (unsigned i = 0; i < edges.size(); i++) {
    //  vcl_cout << "i: " << i << " s: " << edges[i]->source()->id_ << " t: " << edges[i]->target()->id_ << vcl_endl;
    //}
    //vcl_cout << "-----------------\n";

    if (!edges.size())
      return 0;

    vcl_vector<vsol_point_2d_sptr> pts;
    

    dbskr_v_edge_sptr e, e_next;
    int current_node_id = start_node_id;
    //vsol_polyline_2d_sptr current_real_boundary = new vsol_polyline_2d();
    for (unsigned i = 0; i < edges.size(); i++) {
      //vcl_cout << "(" << edges[i]->start_node_id_ << ", " << edges[i]->end_node_id_ << ")\n";
      e = edges[i];

      if (e->source()->id_ < 0 || e->target()->id_ < 0)
        continue;

      // if e and e_next are the same then this is an end scurve
      e_next = edges[(i+1)%edges.size()];
      
      bool end_scurve = (e == e_next);

      dbsk2d_shock_node_sptr start_node, end_node;
      vcl_vector<dbsk2d_shock_edge_sptr> scurve_edges;
      start_node = (e->source()->original_shock_node_->id() == current_node_id) ? e->source()->original_shock_node_ : e->target()->original_shock_node_;
      end_node = (e->source()->original_shock_node_->id() == current_node_id) ? e->target()->original_shock_node_ : e->source()->original_shock_node_;
      if (e->start_node_id_ == current_node_id) {
        scurve_edges.insert(scurve_edges.begin(), e->edges_.begin(), e->edges_.end());
      } else {
        scurve_edges.insert(scurve_edges.begin(), e->edges_.rbegin(), e->edges_.rend());
      }
    
      if (end_scurve) {
        dbskr_scurve_sptr scurve;

        //: find the real shock curve
        if (end_node->degree() == 1) 
          scurve = dbskr_compute_scurve(start_node, scurve_edges, true, binterpolate, subsample, interpolate_ds, subsample_ds);
        else 
          scurve = dbskr_compute_scurve(start_node, scurve_edges, false, binterpolate, subsample, interpolate_ds, subsample_ds);

        vsol_polyline_2d_sptr plm = new vsol_polyline_2d();
        vcl_vector<vgl_point_2d<double> >& vm = scurve->bdry_minus();
        for (unsigned j = 0; j < vm.size(); j++) {
          vsol_point_2d_sptr dum_pt = new vsol_point_2d(vm[j]);
          plm->add_vertex(dum_pt);
        }
          //current_real_boundary->add_vertex(new vsol_point_2d(vm[j]));
        
        //patch->add_real_boundary(new vsol_polyline_2d(*current_real_boundary));
        //current_real_boundary = 0;
        //current_real_boundary = new vsol_polyline_2d();

        vsol_polyline_2d_sptr plp = new vsol_polyline_2d();
        vcl_vector<vgl_point_2d<double> >& vp = scurve->bdry_plus();
        for (int j = int(vp.size()-1); j >=0 ; j--) {
          vsol_point_2d_sptr dum_pt = new vsol_point_2d(vp[j]);
          plp->add_vertex(dum_pt);
        }
          //current_real_boundary->add_vertex(new vsol_point_2d(vm[j]));
        
        patch->add_real_boundary(plp);
        patch->add_real_boundary(plm);

        if (circular_ends) {
          scurve = dbskr_compute_scurve(start_node, scurve_edges, true, binterpolate, subsample, interpolate_ds, subsample_ds);
          for (int j = 0; j<scurve->num_points(); j++) {
            vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->bdry_minus_pt(j));
            pts.push_back(dum_pt);
          }
          for (int j = scurve->num_points()-1; j>=0; j--) {
            vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->bdry_plus_pt(j));
            pts.push_back(dum_pt);
          }
        } else {
          
          for (int j = 0; j<scurve->num_points(); j++) {
            vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->bdry_minus_pt(j));
            pts.push_back(dum_pt);
          }

          if (end_node->degree() != 1) {
            vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->sh_pt(scurve->num_points()-1));
            pts.push_back(dum_pt);
          }

          for (int j = scurve->num_points()-1; j>=0; j--) {
            vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->bdry_plus_pt(j));
            pts.push_back(dum_pt);
          }
        }
        
        i++; // skip next edge!!  current_node_id does not change
      } else {
        dbskr_scurve_sptr scurve = dbskr_compute_scurve(start_node, scurve_edges, false, 
                                                        binterpolate, subsample, 
                                                        interpolate_ds, subsample_ds);
        for (int j = 0; j<scurve->num_points(); j++) {
          vsol_point_2d_sptr dum_pt = new vsol_point_2d(scurve->bdry_minus_pt(j));
          pts.push_back(dum_pt);
        }

        vsol_polyline_2d_sptr plm = new vsol_polyline_2d();
        vcl_vector<vgl_point_2d<double> >& vm = scurve->bdry_minus();
        for (unsigned j = 0; j < vm.size(); j++) {
          vsol_point_2d_sptr dum_pt = new vsol_point_2d(vm[j]);
          plm->add_vertex(dum_pt);
        }
          //current_real_boundary->add_vertex(new vsol_point_2d(vm[j]));

        patch->add_real_boundary(plm);

        current_node_id = (e->start_node_id_ == current_node_id) ? e->end_node_id_ : e->start_node_id_;
      }
    }

    poly = new vsol_polygon_2d(pts);
    //clear the points
    for (unsigned kkk = 0; kkk < pts.size(); kkk++)
      pts[kkk] = 0;
    pts.clear();
    double area = poly->area();
    //vcl_cout << "area is " << area << vcl_endl;
    if (area > poly_area_threshold)
      break;
    else { 
      poly = 0;
      patch->clear();
    }
  }

  patch->set_outer_boundary(poly);
  //: saved cause its being used in the pruning stages
  patch->set_v_graph(vg);

#if 0 // not using contours 
  //: at this point, all the real boundaries are scurves coming from different shock branches
  //  when two shock branches are the neighbors of each other then their scurves are end to end
  //  merge the real boundaries, when they are end to end 
  vcl_vector<vsol_polyline_2d_sptr>& rbs = patch->get_real_boundaries();
  if (rbs.size() > 0) {
    vcl_vector<vcl_vector<vsol_polyline_2d_sptr> > new_rbs;
    
    vcl_vector<vsol_polyline_2d_sptr> remaining(rbs);
    
    while (!remaining.empty()) {

      vcl_vector<vsol_polyline_2d_sptr> current_set;
      current_set.push_back(remaining[0]);

      //: now add all the other curves which are ending or starting at the current sets ending or starting points
      vgl_point_2d<double> current_end = remaining[0]->vertex(remaining[0]->size()-1)->get_p();
      vgl_point_2d<double> current_start = remaining[0]->vertex(0)->get_p();

      remaining.erase(remaining.begin());
      
      bool added_new = true;
      while(added_new) {
        added_new = false;
        for (unsigned i = 0; i < remaining.size(); i++) {
          vsol_polyline_2d_sptr rbs_i = remaining[i];

          //: if last point of prev is really close to first point of current then add current to the current_set
          if (vgl_distance(current_end, rbs_i->vertex(0)->get_p()) < 0.1) {
            current_set.push_back(rbs_i);
            current_end = rbs_i->vertex(rbs_i->size()-1)->get_p();
            remaining.erase(remaining.begin()+i);
            added_new = true;
            break;
          } else if (vgl_distance(current_start, rbs_i->vertex(rbs_i->size()-1)->get_p()) < 0.1) {
            current_set.insert(current_set.begin(), rbs_i);
            current_start = rbs_i->vertex(0)->get_p();
            remaining.erase(remaining.begin()+i);
            added_new = true;
            break;
          }
        }
      }

      new_rbs.push_back(current_set);
      //remaining.clear();
      //remaining.insert(remaining.begin(), kept_polys.begin(), kept_polys.end());
    }

    rbs.clear();

    // now make longer polylines
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > rbs_points;
    for (unsigned i = 0; i < new_rbs.size(); i++) {
      //vsol_polyline_2d_sptr poly = new vsol_polyline_2d();
      vcl_vector<vsol_point_2d_sptr> points;
      for (unsigned j = 0; j < new_rbs[i].size(); j++) {
        //: not including the first points in the polylines, cause they are causing self crossings at the minuscule level
        for (unsigned k = 0; k < new_rbs[i][j]->size(); k++)
          //poly->add_vertex(new_rbs[i][j]->vertex(k));
          points.push_back(new_rbs[i][j]->vertex(k));
      }
      //rbs.push_back(poly);
      rbs_points.push_back(points);
    }

    //: eliminate the portions which are crossing themselves
    for (unsigned i = 0; i < rbs_points.size(); i++) {
      bool erased = true;
      while (erased) {
        erased = false;
        for (unsigned k = 0; k < rbs_points[i].size(); k++) {
          for (unsigned j = k+1; j < rbs_points[i].size(); j++) {
            //if (vgl_distance(rbs_points[i]->vertex(k)->get_p(), rbs[i]->vertex(j)->get_p()) < 0.1) {
            if (vgl_distance(rbs_points[i][k]->get_p(), rbs_points[i][j]->get_p()) < 0.1) {
              //: erase all the portino in between k and j
              rbs_points[i].erase(rbs_points[i].begin()+k+1, rbs_points[i].begin()+j+1);
              erased = true;
              break;
            }
          }
          if (erased)
            break;
        }
      }
    }

    // now make longer polylines
    for (unsigned i = 0; i < rbs_points.size(); i++) {
      vsol_polyline_2d_sptr poly = new vsol_polyline_2d();
      for (unsigned j = 0; j < rbs_points[i].size(); j++) {
        poly->add_vertex(rbs_points[i][j]);
      }
      rbs.push_back(poly);
    }

  }
#endif

  return patch;
}


//: For sorting pairs by their second elements cost
inline bool
final_cost_less( const vcl_pair<int, dbskr_sm_cor_sptr>& left,
                 const vcl_pair<int, dbskr_sm_cor_sptr>& right )
{
  return (left.second)->final_cost() < (right.second)->final_cost();
}

//: CAUTION: assumes tree computation parameters are already set properly for the patches
//           only sets the tree_edit parameters from edit_params argument
//           MAKE SURE: tree computation parameters in dbskr_shock_patch and the ones in edit_params are exactly the same
bool
find_patch_correspondences(dbskr_shock_patch_sptr s1, 
                           vcl_vector<dbskr_shock_patch_sptr>& s2, 
                           patch_cor_map_type& match_map, dbskr_tree_edit_params& edit_params)
{ 
  dbskr_tree_sptr tree1 = s1->tree();  // forces computation if tree is not available
  if (!tree1) {
    vcl_cout << "Tree is not available for patch: " << s1->id() << " skipping\n" << vcl_endl;
    return false;
  }

  //vcl_cout << s1->id() << " ";

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> > * patch_v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
  for (unsigned j = 0; j < s2.size(); j++) {
    dbsk2d_shock_graph_sptr sg2;

    dbskr_tree_sptr tree2 = s2[j]->tree();  // forces computation if tree is not available
    if (!tree2) {
      vcl_cout << "Tree 2 is not available for patch: " << s2[j]->id() << " skipping\n" << vcl_endl;
      continue;
    }
    //vcl_cout << "\t\tpatch: " << s2[j]->id() << " d: " << s2[j]->depth() << " ";

    vul_timer t;
    t.mark();

    //instantiate the edit distance algorithm
    dbskr_tree_edit* edit;
    if (edit_params.combined_edit_)
      edit = new dbskr_tree_edit_combined(tree1, tree2, edit_params.circular_ends_, edit_params.localized_edit_);  
    else
      edit = new dbskr_tree_edit(tree1, tree2, edit_params.circular_ends_, edit_params.localized_edit_);

    edit->save_path(true);
    if (!edit->edit()) {
      vcl_cout << "Problems in editing trees\n";
      continue;
    }
    float val = edit->final_cost();
    float norm_val = val/(tree1->total_splice_cost()+tree2->total_splice_cost());
    //vcl_cout << "norm cost: " << norm_val << " time: "<< t.real()/1000.0f << " secs.\n";
    dbskr_sm_cor_sptr sm_cor = edit->get_correspondence_just_map();  // sets the parameters
    sm_cor->set_final_cost(val);
    sm_cor->set_final_norm_cost(norm_val);
    
    vcl_pair<int, dbskr_sm_cor_sptr> pp;
    pp.first = s2[j]->id();
    pp.second = sm_cor;
    patch_v->push_back(pp);
    delete edit;
    tree2 = 0;
    s2[j]->kill_tree();

  }
  s1->kill_tree();
  
  vcl_sort(patch_v->begin(), patch_v->end(), final_cost_less );
  match_map[s1->id()] = patch_v;
  return true;
}

//: CAUTION: assumes tree computation parameters are already set properly for the patches
//           only sets the tree_edit parameters from edit_params argument
//           MAKE SURE: tree computation parameters in dbskr_shock_patch and the ones in edit_params are exactly the same
dbskr_shock_patch_match_sptr 
find_all_patch_correspondences(vcl_vector<dbskr_shock_patch_sptr>& s1, 
                               vcl_vector<dbskr_shock_patch_sptr>& s2, dbskr_tree_edit_params& edit_params)
{ 
  dbskr_shock_patch_match_sptr match = new dbskr_shock_patch_match();
  match->edit_params_ = edit_params;
  patch_cor_map_type& match_map = match->get_map();

  for (unsigned i = 0; i < s1.size(); i++) {
    find_patch_correspondences(s1[i], s2, match_map, edit_params);
  }

  return match;
}

//: CAUTION: assumes tree computation parameters are already set properly for the patches
//           only sets the tree_edit parameters from edit_params argument
//           MAKE SURE: tree computation parameters in dbskr_shock_patch and the ones in edit_params are exactly the same
//           combined_edit should be false in edit_params since coarse editing 
//           (interval cost functions in Sebastian et al. PAMI06 will be used and splice cost function will use original dpmatch)
bool find_patch_correspondences_coarse_edit(dbskr_shock_patch_sptr s1, 
                                vcl_vector<dbskr_shock_patch_sptr>& s2, 
                                patch_cor_map_type& match_map, dbskr_tree_edit_params& edit_params)
{
  dbskr_tree_sptr tree1 = s1->tree();  // forces computation of tree if its not available
                                                                      // dpmatch_combined = false since coarse editing (interval cost functions in Sebastian et al. PAMI06 will be used and splice cost function will use original dpmatch)
  if (!tree1) { 
    vcl_cout << "Tree is not available for patch: " << s1->id() << " skipping\n" << vcl_endl;
    return false;
  }
  //vcl_cout << "patch: " << s1->id() << " d: " << s1->depth() << "\n";

  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> > * patch_v = new vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >();
  for (unsigned j = 0; j < s2.size(); j++) {
    dbskr_tree_sptr tree2 = s2[j]->tree();  // forces computation of tree if its not available                                            
    if (!tree2) {
      vcl_cout << "Tree 2 is not available for patch: " << s2[j]->id() << " skipping\n" << vcl_endl;
      continue;
    }
    //vcl_cout << "\t\tpatch: " << s2[j]->id() << " d: " << s2[j]->depth() << " ";

    vul_timer t;
    t.mark();

    //instantiate the edit distance algorithm
    dbskr_tree_edit_coarse edit(tree1, tree2, edit_params.circular_ends_);  // WARNING: using s1's flag to match both s1 and s2[j]!!!!
    edit.save_path(true);
    if (!edit.edit()) {
      vcl_cout << "Problems in editing trees\n";
      continue;
    }
    float val = edit.final_cost();
    float norm_val = val/(tree1->total_splice_cost()+tree2->total_splice_cost());
    //vcl_cout << "norm cost: " << norm_val << " time: "<< t.real()/1000.0f << " secs.\n";
    dbskr_sm_cor_sptr sm_cor = edit.get_correspondence_just_map();
    sm_cor->set_final_cost(val);
    sm_cor->set_final_norm_cost(norm_val);
    vcl_pair<int, dbskr_sm_cor_sptr> pp;
    pp.first = s2[j]->id();
    pp.second = sm_cor;
    patch_v->push_back(pp);
  }

  vcl_sort(patch_v->begin(), patch_v->end(), final_cost_less );
  match_map[s1->id()] = patch_v;
  return true;
}

//: given an image (patch set) find its best match among a set of matches
bool best_match_norm(vcl_vector<dbskr_shock_patch_sptr>& pv, 
                vcl_vector<dbskr_shock_patch_match_sptr>& mv, int& best_id)
{
  vcl_vector<float> mv_counts(mv.size(), 0);

  for (unsigned i = 0; i < pv.size(); i++) {
    dbskr_shock_patch_sptr sp = pv[i];

    float best_norm_val = 100000.0f;
    int best_j = -1;
    for (unsigned j = 0; j < mv.size(); j++) {
      
      if (!mv[j]) {  // the match does not exist for this pair
        vcl_cout << "match is not complete!\n";
        return false;
      }

      //: get the best match of the patch with this id
      vcl_pair<int, dbskr_sm_cor_sptr>& bp = mv[j]->get_best_match(sp->id());  // there might be run time errors if it cannot find this instance in the map of match
                                                                               // make sure the patch storages are the ones used to create this match instance
      //vcl_cout << "best match of " << sp->id() << " is: " << bp.first << " cost: " << bp.second->final_norm_cost() << "\n"; 
      if (bp.second) {
        float norm_val = bp.second->final_norm_cost();
        if (norm_val < best_norm_val) {
          best_norm_val = norm_val;
          best_j = j;
        }
      }
    }
    if (best_j < 0) {
      vcl_cout << "match is not complete!\n";
      return false; // size of the match vector is zero  
    }

    //: vote for the image of the best value weighted by the strength of the similarity
    mv_counts[best_j] += (1.0f - best_norm_val);  // best norm val is in [0, 1], the smaller the better
  }
  //: return the id of the set with the highest (weighted) votes from the query patch vector
  best_id = 0;
  float best_val = mv_counts[0];
  for (unsigned i = 0; i < mv_counts.size(); i++) {
    if (best_val < mv_counts[i]) {
      best_val = mv_counts[i];
      best_id = i;
    }
  }

  return true;
}



//: given an image (patch set) find its best match among a set of matches
//  use top n best patch matches between the patch set matches of two images
bool best_match_norm_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, 
                vcl_vector<dbskr_shock_patch_match_sptr>& mv, int n, int& best_id)
{
  vcl_vector<float> mv_counts(mv.size(), 0);

  
  for (unsigned i = 0; i < pv.size(); i++) {
    dbskr_shock_patch_sptr sp = pv[i];

    float best_norm_val = 100000.0f;
    int best_j = -1;
    for (unsigned j = 0; j < mv.size(); j++) {
      
      if (!mv[j]) {  // the match does not exist for this pair
        vcl_cout << "match is not complete!\n";
        return false;
      }

      vcl_map<int, dbskr_shock_patch_sptr>& map1 = mv[j]->get_id_map1();
      vcl_map<int, dbskr_shock_patch_sptr>& map2 = mv[j]->get_id_map2();

      //: get the best match of the patch with this id
      vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* bpv = mv[j]->get_best_n_match(sp->id(), n);  // there might be run time errors if it cannot find this instance in the map of match
                                                                               // make sure the patch storages are the ones used to create this match instance
      //vcl_cout << "best match of " << sp->id() << " is: " << bp.first << " cost: " << bp.second->final_norm_cost() << "\n"; 
      float norm_val = 0;
      for (unsigned k = 0; k < bpv->size(); k++) {
        /*//: find the areas of the patches and signify the match depending on the size of the patch
        dbskr_shock_patch_sptr p1 = map1[sp->id()];
        dbskr_shock_patch_sptr p2 = map2[(*bpv)[k].first];
        float area1 = p1->get_traced_boundary()->area();
        float area2 = p2->get_traced_boundary()->area();
        float ratio = (area1 > area2) ? area1/area2 : area2/area1;
        norm_val += ratio*((*bpv)[k].second->final_norm_cost());*/
        norm_val += ((*bpv)[k].second->final_norm_cost());
      }
      delete bpv;
      if (norm_val < best_norm_val) {
        best_norm_val = norm_val;
        best_j = j;
      }
    }
    if (best_j < 0) {
      vcl_cout << "match is not complete!\n";
      return false; // size of the match vector is zero  
    }

    //: vote for the image of the best value weighted by the strength of the similarity
    mv_counts[best_j] += (n*1.0f - best_norm_val);  // best norm val is in [0, 1], the smaller the better
  }
  //: return the id of the set with the highest (weighted) votes from the query patch vector
  best_id = 0;
  float best_val = mv_counts[0];
  for (unsigned i = 0; i < mv_counts.size(); i++) {
    if (best_val < mv_counts[i]) {
      best_val = mv_counts[i];
      best_id = i;
    }
  }

  return true;
}


//: given an image (patch set), for each patch find its best patch and vote for the category of the best match
//  if use_info then use the mutual information scores for sorting
bool match_strat1_simple_voting(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> >& mv, int& best_cat_id, bool use_info)
{
  // weight the votes wrt to the areas of the patches
  float largest_area = float(pv[0]->get_traced_boundary()->area());
  for (unsigned i = 1; i < pv.size(); i++) {
    float a = float(pv[i]->get_traced_boundary()->area());
    if (a > largest_area)
      largest_area = a;
  }
  vcl_vector<float> category_votes(mv.size(), 0.0f);
  for (unsigned i = 0; i < pv.size(); i++) {
    //: try each category patches
    float val;
    if (use_info)
      val = 0.0f;  // maximize info
    else
      val = 100000.0f;  // minimize edit distance
    best_cat_id = -1;
    for (unsigned c = 0; c < mv.size(); c++) {
      //: try each instance of the category
      for (unsigned j = 0; j < mv[c].size(); j++) {
        dbskr_shock_patch_match_sptr m = mv[c][j];
        if (!m)
          return false;
        if (use_info) {
          vcl_pair<int, float> p = m->get_best_match_info(pv[i]->id());
          if (p.first > 0) {
            if (p.second > val) {  // find the patch with max mutual info
              val = p.second;
              best_cat_id = c;
            }
          }
        } else {
          vcl_pair<int, dbskr_sm_cor_sptr> p = m->get_best_match(pv[i]->id());
          if (p.second) {
            if (p.second->final_norm_cost() < val) {
              val = p.second->final_norm_cost();
              best_cat_id = c;
            }
          }
        }
      }
    }

    if (best_cat_id >= 0) {
      category_votes[best_cat_id] += float(1.0f*(pv[i]->get_traced_boundary()->area()/largest_area));
    }
  }

  best_cat_id = 0;
  float vote_cnt = category_votes[0];
  vcl_cout << "total patch (vote) cnt: " << pv.size() << "\n";
  vcl_cout << "0: " << category_votes[0] << vcl_endl;
  for (unsigned i = 1; i < category_votes.size(); i++) {
    vcl_cout << i << ": " << category_votes[i] << vcl_endl;
    if (category_votes[i] > vote_cnt) {
      best_cat_id = i;
      vote_cnt = category_votes[i];
    }
  }

  return true;
}

//: given an image (patch set), for each patch find its best n patch and vote for the category of the best n matches
//  if use_info then use the mutual information scores for sorting
bool match_strat1_simple_voting_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, 
                                      vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> >& mv, 
                                      int& best_cat_id, int n,
                                      int visualization_n, vcl_vector<vcl_vector<vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > >& best_category_instance_ids)
{
  // weight the votes wrt to the areas of the patches
  /*float largest_area = float(pv[0]->get_traced_boundary()->area());
  for (unsigned i = 1; i < pv.size(); i++) {
    float a = float(pv[i]->get_traced_boundary()->area());
    if (a > largest_area)
      largest_area = a;
  }*/
  double max_width = 0;
  vcl_vector<double> widths;
  for (unsigned i = 0; i < pv.size(); i++) {
    if (pv[i]->shock_graph()) {
      double a = dbsk2d_compute_total_width(pv[i]->shock_graph());
      widths.push_back(a);
      if (a > max_width)
        max_width = a;
    }
  }

  if (!max_width) {
    vcl_cout << "max width could not be computed!!!!!!!!!!!!!!!!!!!!!\n";
    return false;
  }

  int nnn = visualization_n > n ? visualization_n : n;

  vcl_vector<float> category_votes(mv.size(), 0.0f);
  for (unsigned i = 0; i < pv.size(); i++) {
    //: try each category patches
    vcl_vector<float> vals(nnn, 100000.0f); // minimize edit distance
    vcl_vector<int> best_cat_ids(nnn, -1);
    
    vcl_vector<vcl_pair<vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > best_cat_ins_ids;
    
      //: find top n best matches
    for (unsigned nn = 0; int(nn) < nnn; nn++) {

      int best_ins_id = -1;
      for (unsigned c = 0; c < mv.size(); c++) {
        //: try each instance of the category
        for (unsigned j = 0; j < mv[c].size(); j++) {
          dbskr_shock_patch_match_sptr m = mv[c][j];
          if (!m)
            return false;
          vcl_pair<int, dbskr_sm_cor_sptr> p = m->get_best_match(pv[i]->id());
          if (p.second) {
            if (p.second->final_norm_cost() < vals[nn]) {
              vals[nn] = p.second->final_norm_cost();
              best_cat_ids[nn] = c; 
              best_ins_id = j;
            }
          }
        }
      }
      if (best_ins_id < 0)
        continue;

      vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > pppp;

      dbskr_shock_patch_match_sptr m = mv[best_cat_ids[nn]][best_ins_id];
      vcl_pair<int, dbskr_sm_cor_sptr> p = m->get_best_match(pv[i]->id());

      pppp.second = p.second->final_norm_cost();

      p.second->set_final_norm_cost(10000.0f);
      m->resort_wrt_norm_cost();
      
      vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > ppp;
      vcl_pair<int, int> pp(best_cat_ids[nn], best_ins_id);
      dbskr_shock_patch_sptr best_sp = m->get_id_map2()[p.first];
      vcl_pair<int, int> pp2(best_sp->id(), best_sp->depth());
      ppp.first = pp;
      ppp.second = pp2;

      pppp.first = ppp;
      
      best_cat_ins_ids.push_back(pppp);
    }

    for (unsigned k = 0; k < best_cat_ids.size(); k++) {
      if (best_cat_ids[k] >= 0) {
        //category_votes[best_cat_ids[k]] += float(1.0f*(pv[i]->shock_graph()->get_bounding_box()->area()/largest_area));
        category_votes[best_cat_ids[k]] += float(1.0f*widths[i]/max_width);
      }
    }

    best_category_instance_ids.push_back(best_cat_ins_ids);
  }

  best_cat_id = 0;
  float vote_cnt = category_votes[0];
  vcl_cout << "total patch (vote) cnt: " << pv.size() << "\n";
  vcl_cout << "0: " << category_votes[0] << vcl_endl;
  for (unsigned i = 1; i < category_votes.size(); i++) {
    vcl_cout << i << ": " << category_votes[i] << vcl_endl;
    if (category_votes[i] > vote_cnt) {
      best_cat_id = i;
      vote_cnt = category_votes[i];
    }
  }

  return true;
}

//: given an image (patch set), for each patch find its best n patch and create an html file that displays images of top n
bool create_html_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_string pv_patch_images_dir, 
                       vcl_vector<vcl_vector<vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > >& best_category_instance_ids,
                       vcl_vector<vcl_vector<vcl_string> > ins_names, 
                       int visualization_n, vcl_string file_name, vcl_string table_caption, vcl_vector<vcl_vector<vcl_string> > patch_image_dirs)
{
 
  vcl_ofstream tf(file_name.c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << file_name << " for write " << vcl_endl;
    return false;
  }

  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << table_caption << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  for (unsigned i = 0; int(i) < visualization_n; i++) 
    tf << "<TH> Match " << i+1 << " ";
  tf << "</TH> </TR>\n";
  
  for (unsigned i = 0; i < pv.size(); i++) {
    vcl_ostringstream oss1, oss2;
    oss1 << pv[i]->id();
    oss2 << pv[i]->depth();
    vcl_string patch_image_file = pv_patch_images_dir + oss1.str() + "_" + oss2.str() + ".png";
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "\"> ";
    tf << pv[i]->id() << " " << pv[i]->depth() << " </TD> "; 
    for (unsigned nn = 0; int(nn) < visualization_n; nn++) {
      vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > pppp = best_category_instance_ids[i][nn];
      vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > pp = pppp.first;
      vcl_ostringstream oss11, oss22;      
      oss11 << pp.second.first;
      oss22 << pp.second.second;
      vcl_string patch_image_file2 = patch_image_dirs[pp.first.first][pp.first.second] + oss11.str() + "_" + oss22.str() + ".png";
      tf << "<TD> <img src=\"" << patch_image_file2 << "\"> ";
      tf << ins_names[pp.first.first][pp.first.second] << " " << pp.second.first << " " << pp.second.second << " " << pppp.second << " </TD> ";
    }
    tf << "</TR>\n";
  }
  
  tf << "</TABLE>\n";
  tf.close();
  return true;
}

// WARNING: for the trees: assuming tree parameters are already set properly depending on the match parameters
bool create_html_top_n_placements(dbskr_shock_patch_match_sptr new_match, 
                                  vil_image_resource_sptr img_test, 
                                  vil_image_resource_sptr img_model, 
                                  vcl_string model_patch_images_dir,
                                  int visualization_n, vcl_string out_html_images_dir, vcl_string out_html, vcl_string table_caption)
{
  vcl_ofstream tf(out_html.c_str(), vcl_ios::app);
  
  if (!tf) {
    vcl_cout << "Unable to open output html file " << out_html << " for write " << vcl_endl;
    return false;
  }

  tf << "<TABLE BORDER=\"1\">\n";
  tf << "<caption align=\"top\">" << table_caption << "</caption>\n";
  tf << "<TR> <TH>   ";  // leave the first cell of first row empty
  // write top n to the first row
  for (unsigned i = 0; int(i) < visualization_n; i++) 
    tf << "<TH> Match " << i+1 << " ";
  tf << "</TH> </TR>\n";
  
  //vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > 
  patch_cor_map_type& map = new_match->get_map();
  vcl_map<int, dbskr_shock_patch_sptr>& id_map_test = new_match->get_id_map2();
  vcl_map<int, dbskr_shock_patch_sptr>& id_map_model = new_match->get_id_map1();  // first one is model
  patch_cor_map_iterator iter;
  for (iter = map.begin(); iter != map.end(); iter++) {
    dbskr_shock_patch_sptr msp = id_map_model[iter->first];
    vcl_ostringstream oss1, oss2;
    oss1 << msp->id();
    oss2 << msp->depth();
    vcl_string patch_image_file = model_patch_images_dir + oss1.str() + "_" + oss2.str() + ".png";
    tf << "<TR> <TD> <img src=\"" << patch_image_file << "\"> ";
    tf << msp->id() << " " << msp->depth() << " </TD> "; 
    vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* match_vec = iter->second;
    for (unsigned nn = 0; int(nn) < visualization_n; nn++) {
      if (nn >= match_vec->size()) {
        tf << "<TD> <img src=\"unknown\"> </TD> ";
      } else {
        dbskr_shock_patch_sptr tsp = id_map_test[(*match_vec)[nn].first];
        dbskr_sm_cor_sptr sm = (*match_vec)[nn].second;

        sm->set_tree1(msp->tree()); // assuming tree parameters are already set properly
        sm->set_tree2(tsp->tree()); // assuming tree parameters are already set properly  // all the maps are recomputed based on dart correspondence

        vgl_h_matrix_2d<double> H; 
        vcl_cout << "computing homography.. ";
        if (sm->compute_homography(H, true, 5, false, false)) {  // homography that maps model onto test image
          vcl_cout << " done.\n";
          vil_image_resource_sptr new_img = msp->mapped_image(img_model, img_test, H, true); // no need to recompute observation if already exists

          vcl_ostringstream oss11, oss22;      
          oss11 << tsp->id();
          oss22 << tsp->depth();
          vcl_string out_img = out_html_images_dir + oss1.str() + "_" + oss2.str() + "_mapped_with_test_patch_" + oss11.str() + "_" + oss22.str() + ".png";

          vil_save_image_resource(new_img, out_img.c_str());

          tf << "<TD> <img src=\"" << out_img << "\"> ";
          tf << msp->id() << " mapped with test patch: " << tsp->id() << " " << tsp->depth() << " </TD> ";
        } else {
          vcl_cout << " homography not computed!!\n";
          tf << "<TD> <img src=\"unknown\"> homography not computed!! </TD> ";
        } 
      }
    }
    tf << "</TR>\n";
  }
  
  tf << "</TABLE>\n";
  tf.close();
  return true;
}



inline bool 
norm_curve_cost_less(const dbcvr_cv_cor_sptr& cor1,
                     const dbcvr_cv_cor_sptr& cor2) {
  return (cor1->final_norm_cost_ < cor2->final_norm_cost_);
}

inline bool 
length_more(const vsol_polyline_2d_sptr& l1,
            const vsol_polyline_2d_sptr& l2) {
  return (l1->length() > l2->length());
}

//: find the corresondences using elastic curve matching between sets of real contours of patches
bool find_patch_correspondences_curve(dbskr_shock_patch_sptr s1, 
                                vcl_vector<dbskr_shock_patch_sptr>& s2, 
                                vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >& match_map, int n)
{
  vcl_vector<vsol_polyline_2d_sptr> &real_set1 = s1->get_real_boundaries();
  vcl_cout << "patch: " << s1->id() << " d: " << s1->depth() << " size: " << real_set1.size() << "\n";

  vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > > * patch_v = new vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >();
  for (unsigned j = 0; j < s2.size(); j++) {
    vcl_vector<vsol_polyline_2d_sptr> &real_set2 = s2[j]->get_real_boundaries();
    vcl_cout << "\t\tpatch: " << s2[j]->id() << " d: " << s2[j]->depth() << " size: " << real_set2.size() << "\n";

    vul_timer t;
    t.mark();

    //: find the distances between longest top n contours of real_set1 and real_set2
    vcl_sort(real_set1.begin(), real_set1.end(), length_more);
    int max_size = int(real_set1.size()) < n ? real_set1.size() : n;

    vcl_vector<dbcvr_cv_cor_sptr> cors;
    for (int ri = 0; ri < max_size; ri++) {
      if (!real_set1[ri]->size())
        continue;

      bsol_intrinsic_curve_2d_sptr curve1 = new bsol_intrinsic_curve_2d(real_set1[ri]);
      curve1->computeProperties();
      
      if (curve1->length() < 5)  // ignore curves less than 5 pixel
        continue;

      for (unsigned rj = 0; rj < real_set2.size(); rj++) {
        
        if (!real_set2[rj]->size())
          continue;

        dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();
        
        bsol_intrinsic_curve_2d_sptr curve2 = new bsol_intrinsic_curve_2d(real_set2[rj]);
        curve2->computeProperties();

        if (curve2->length() < 5)  // ignore curves less than 5 pixel
          continue;


        curveMatch->setCurve1 (curve1);
        curveMatch->setCurve2 (curve2);
        curveMatch->Match ();

        double cost = curveMatch->finalCost();
        double norm_cost = cost/(curve1->length()+curve2->length());
        vcl_cout<< " curve_2d matching cost: " << cost << " norm cost: " << norm_cost << vcl_endl;
        dbcvr_cv_cor_sptr cor = curveMatch->get_cv_cor();
        cor->set_final_cost(cost);
        cor->set_final_norm_cost(norm_cost);
        delete curveMatch;
        cors.push_back(cor);
      }
    }
    if (cors.size() > 0) {
      vcl_sort(cors.begin(), cors.end(), norm_curve_cost_less);
      cors.erase(cors.begin()+n, cors.end());
    }

    vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > pp;
    pp.first = s2[j]->id();
    pp.second = cors;
    patch_v->push_back(pp);
  }

  match_map[s1->id()] = patch_v;  // patch_v is NOT sorted!!!
  return true;
}

void draw_shock_graph_into_ps(dbsk2d_shock_graph_sptr sg, 
                              vul_psfile& psfile1, 
                              vcl_map<int, int>& edge_color_map, 
                              vcl_vector<vcl_vector<float> >& rnd_colormap, int offsetx, int offsety, bool line, float size, bool draw_shock_nodes, float point_size) 
{
  int color;
 // draw shock graph edges
  for ( dbsk2d_shock_graph::edge_iterator curE = sg->edges_begin();
        curE != sg->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr selm = (*curE);

    //use the correspondence color scheme
    vcl_map<int, int>::iterator iter = edge_color_map.find(selm->id());
    if (iter == edge_color_map.end()) 
      color = rnd_colormap.size()-1;                  // if no correspondence assign black
    else 
      color = iter->second;           //assign the match color

    //draw the edge
    psfile1.set_fg_color( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
    if (line) {
      psfile1.set_line_width(size);
      for( unsigned int i = 1 ; i < selm->ex_pts().size() ; i++ ) {
        vgl_point_2d<double> p1 = (selm->ex_pts())[i-1];
        vgl_point_2d<double> p2 = (selm->ex_pts())[i];
        psfile1.line((float)(p1.x()+offsetx), (float)(p1.y()+offsety), (float)(p2.x()+offsetx), (float)(p2.y()+offsety));
      }
    } else {
      for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
        vgl_point_2d<double> p = (selm->ex_pts())[i];
        psfile1.point((float)p.x()+offsetx, (float)(p.y()+offsety), size);
      }
    }
  }
  
  // draw shock graph nodes
  if (draw_shock_nodes) {
    for ( dbsk2d_shock_graph::vertex_iterator curN = sg->vertices_begin(); curN != sg->vertices_end(); curN++ ) 
    {
      dbsk2d_shock_node_sptr snode = (*curN);
      color = rnd_colormap.size()-1;                  // assign black
      psfile1.set_fg_color( rnd_colormap[color][0] , rnd_colormap[color][1] , rnd_colormap[color][2] );
      psfile1.point((float)(snode->ex_pts()[0].x()+offsetx), (float)(snode->ex_pts()[0].y()+offsety), point_size);
    }
  }

}

//: create a white ps image, draw the base shocks as black, draw main shock graphs with branches colored wrt a given dbskr_sm_cor
bool create_ps_shock(vcl_string ps_file_name, 
                    dbsk2d_shock_graph_sptr base_sg1)
{
  //1)If file open fails, return.
    vul_psfile psfile1(ps_file_name.c_str(), false);

    if (!psfile1) {
      vcl_cout << " Error opening file  " << ps_file_name.c_str() << vcl_endl;
      return false;
    }

    //: create a white background image
    if (!dbsk2d_compute_bounding_box(base_sg1)) {
      vcl_cout << "In create_ps_shock_matching() -- cannot compute bounding box of base_sg1 in: " << ps_file_name << vcl_endl;
      return false;
    }
    
    vsol_box_2d_sptr b1 = base_sg1->get_bounding_box();
    
    double w1 = b1->width() + b1->width()/5;
    double h1 = b1->height() + b1->height()/5;

    int offset = 10;  // offset for the second tree 
    int sizex = (int)vcl_floor(w1 + 0.5) + 2*offset;   //off + w1 + off + off + w2 + off
    int sizey = (int)vcl_floor(h1 + 0.5) + 2*offset;
    unsigned char *buf = new unsigned char[sizex*sizey*3];
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
          buf[3*(x+sizex*y)  ] = 255;
          buf[3*(x+sizex*y)+1] = 255;
          buf[3*(x+sizex*y)+2] = 255;
      }

    psfile1.print_color_image(buf,sizex,sizey);
    delete [] buf;
    psfile1.reset_bounding_box();

    psfile1.set_scale_x(50);
    psfile1.set_scale_y(50);
    
    vcl_vector<float> tmp(3, 0);
    vcl_vector<vcl_vector<float> > rnd_colormap(1, tmp);

    vcl_map<int, int> edge_color_map1;
    for ( dbsk2d_shock_graph::edge_iterator curE = base_sg1->edges_begin(); curE != base_sg1->edges_end(); curE++ ) 
    {
      dbsk2d_shock_edge_sptr selm = (*curE);
      edge_color_map1[selm->id()] = 0;        // draw the base shock graph as completely black
    }

    draw_shock_graph_into_ps(base_sg1, psfile1, edge_color_map1, rnd_colormap, offset, 0, true, 0.5f, true, 0.8f);
    
    //close file
    psfile1.close();

    return true;
}

//: create a white ps image, draw the base shocks as black, draw main shock graphs with branches colored wrt a given dbskr_sm_cor
bool create_ps_shock_matching(vcl_string ps_file_name, 
                              dbsk2d_shock_graph_sptr base_sg1, 
                              dbskr_tree_sptr tree1, 
                              dbsk2d_shock_graph_sptr base_sg2, 
                              dbskr_tree_sptr tree2, 
                              dbskr_sm_cor_sptr sm)
{
  //1)If file open fails, return.
    vul_psfile psfile1(ps_file_name.c_str(), false);

    if (!psfile1) {
      vcl_cout << " Error opening file  " << ps_file_name.c_str() << vcl_endl;
      return false;
    }

    //: create a white background image
    if (!dbsk2d_compute_bounding_box(base_sg1)) {
      vcl_cout << "In create_ps_shock_matching() -- cannot compute bounding box of base_sg1 in: " << ps_file_name << vcl_endl;
      return false;
    }
    if (!dbsk2d_compute_bounding_box(base_sg2)) {
      vcl_cout << "In create_ps_shock_matching() -- cannot compute bounding box of base_sg1 in: " << ps_file_name << vcl_endl;
      return false;
    }
    vsol_box_2d_sptr b1 = base_sg1->get_bounding_box();
    vsol_box_2d_sptr b2 = base_sg2->get_bounding_box();
    
    double w1 = b1->width() + b1->width()/5;
    double w2 = b2->width() + b2->width()/5;
    double h1 = b1->height() + b1->height()/5;
    double h2 = b2->height() + b2->height()/5;

    int offset = 10;  // offset for the second tree 
    int sizex = (int)vcl_floor(w1 + w2 + 0.5) + 4*offset;   //off + w1 + off + off + w2 + off
    int sizey = (int)vcl_floor((h1 > h2 ? h1 : h2) + 0.5) + 4*offset;
    unsigned char *buf = new unsigned char[sizex*sizey*3];
    for (int x=0; x<sizex; ++x) 
      for (int y=0; y<sizey; ++y) {
          buf[3*(x+sizex*y)  ] = 255;
          buf[3*(x+sizex*y)+1] = 255;
          buf[3*(x+sizex*y)+2] = 255;
      }

    psfile1.print_color_image(buf,sizex,sizey);
    delete [] buf;
    psfile1.reset_bounding_box();

    psfile1.set_scale_x(50);
    psfile1.set_scale_y(50);
    
    vcl_vector<float> tmp(3, 0);
    vcl_vector<vcl_vector<float> > rnd_colormap(101, tmp);

    //fill in the randomized color table
    for (int i=0; i<100; i++) {
      for (int j=0; j<3;j++)
        rnd_colormap[i][j] = (float)(rand()/double(RAND_MAX));
    }

    vcl_map<int, int> edge_color_map1, edge_color_map2;
    for ( dbsk2d_shock_graph::edge_iterator curE = base_sg1->edges_begin(); curE != base_sg1->edges_end(); curE++ ) 
    {
      dbsk2d_shock_edge_sptr selm = (*curE);
      edge_color_map1[selm->id()] = 100;        // draw the base shock graph as completely black
    }

    draw_shock_graph_into_ps(base_sg1, psfile1, edge_color_map1, rnd_colormap, offset, 0, true, 0.5f, true, 0.8f);
    edge_color_map1.clear();

    for ( dbsk2d_shock_graph::edge_iterator curE = base_sg2->edges_begin(); curE != base_sg2->edges_end(); curE++ ) 
    {
      dbsk2d_shock_edge_sptr selm = (*curE);
      edge_color_map2[selm->id()] = 100;        // draw the base shock graph as completely black
    }

    draw_shock_graph_into_ps(base_sg2, psfile1, edge_color_map2, rnd_colormap, (int)vcl_floor(w1+0.5) + 3*offset, 0, true, 0.5f, true, 0.8f);
    edge_color_map2.clear();

    //get dart path mapping from the shock correspondence 
    vcl_vector<pathtable_key>& dart_path_map = sm->get_map();

    //temp data structures
    dbsk2d_shock_node_sptr start_node;
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    vcl_vector<int> dart_list;

    // go over all the corresponding paths and draw the corresponding 
    // boundary curves visual fragments and intrinsic fragement coordinates
    for (unsigned int i = 0; i<dart_path_map.size(); i++) 
    {
      int color = i%100; //pick a color for this path (may not be unique)

      //---------------
      // Shock graph 1
      //---------------

      //get dart path
      pathtable_key key = dart_path_map[i];
      dart_list.clear();
      dart_list = tree1->get_dart_path(key.first.first, key.first.second);
      
      //get shock edge list from this path
      edges.clear();
      tree1->edge_list(dart_list, start_node, edges); 

      //assign the current color to all the shock edges in this path
      for (unsigned int j = 0; j<edges.size(); j++) 
        edge_color_map1[edges[j]->id()] = color;

      //---------------
      // Shock graph 2
      //---------------

      //get dart path
      dart_list.clear();
      dart_list = tree2->get_dart_path(key.second.first, key.second.second); 

      //get shock edge list from this path
      edges.clear();
      tree2->edge_list(dart_list, start_node, edges);

      //assign the current color to all the shock edges in this path
      for (unsigned int j = 0; j<edges.size(); j++) 
        edge_color_map2[edges[j]->id()] = color;
    }
    
    draw_shock_graph_into_ps(tree1->get_shock_graph(), psfile1, edge_color_map1, rnd_colormap, offset, 0, false, 1.0f, false, 1.0f);
    draw_shock_graph_into_ps(tree2->get_shock_graph(), psfile1, edge_color_map2, rnd_colormap, (int)vcl_floor(w1+0.5) + 3*offset, 0, false, 1.0f, false, 1.0f);

    //close file
    psfile1.close();

    return true;
}

//: create image with traced boundaries
bool create_ps_patches(vcl_string ps_file_name, vcl_vector<dbskr_shock_patch_sptr>& patches, vcl_vector<vil_rgb<int> >& colors,
                       vil_image_resource_sptr background_img)
{
  if (patches.size() != colors.size()) {
    vcl_cout << "colors array is not same size as pathces array, image not written\n";
    return false;
  }

 //1)If file open fails, return.
  vul_psfile psfile1(ps_file_name.c_str(), false);

  if (!psfile1) {
    vcl_cout << " Error opening file  " << ps_file_name.c_str() << vcl_endl;
    return false;
  }

  //: put the background
  //: determine size
  int sizex, sizey; // (int)vcl_floor(w1 + w2 + 0.5) + 4*offset;   //off + w1 + off + off + w2 + off
                    //int sizey = (int)vcl_floor((h1 > h2 ? h1 : h2) + 0.5) + 4*offset;

  if (background_img == 0) {
    vsol_box_2d_sptr box = new vsol_box_2d();
    for (unsigned i = 0; i < patches.size(); i++) {
      vsol_polygon_2d_sptr poly = patches[i]->get_traced_boundary();
      poly->compute_bounding_box();
      box->grow_minmax_bounds(poly->get_bounding_box());
    }
    sizex = (int)vcl_ceil(box->get_max_x()-box->get_min_x())+10;
    sizey = (int)vcl_ceil(box->get_min_x()-box->get_min_y())+10;
  } else {
    sizex = background_img->ni();
    sizey = background_img->nj();
  }

  unsigned char *buf = new unsigned char[sizex*sizey*3];
  if (background_img == 0) {
    for (int x=0; x<sizex; x++) {
      for (int y=0; y<sizey; y++) {
        buf[3*(x+sizex*y)  ] = 255;
        buf[3*(x+sizex*y)+1] = 255;
        buf[3*(x+sizex*y)+2] = 255;
      }
    }
  } else {

    vil_image_view<vxl_byte> image = background_img->get_view(0, background_img->ni(), 0, background_img->nj());
    int planes = image.nplanes();

    if (planes == 3) {
      vcl_cout << "processing color image\n";
      for (int x=0; x<sizex; x++) {
        for (int y=0; y<sizey; y++) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,1);
          buf[3*(x+sizex*y)+2] = image(x,y,2);
        }
      }
    } else if (planes == 1) {
      vcl_cout << "processing grey image\n";
      for (int x=0; x<sizex; x++) {
        for (int y=0; y<sizey; y++) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,0);
          buf[3*(x+sizex*y)+2] = image(x,y,0);
        }
      }
    }
  }

  psfile1.print_color_image(buf,sizex,sizey);
  delete [] buf;
  psfile1.reset_bounding_box();

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);

  for (unsigned i = 0; i < patches.size(); i++) {
    vsol_polygon_2d_sptr poly = patches[i]->get_traced_boundary();
    vil_rgb<int> color = colors[i];

    // parse through all the vsol classes and save curve objects only
    psfile1.set_fg_color(1, 1, 1);
    psfile1.set_line_width(4.0);
    for (unsigned int i=1; i<poly->size();i++)
    {
        vsol_point_2d_sptr p1 = poly->vertex(i-1);
        vsol_point_2d_sptr p2 = poly->vertex(i);
        psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
    psfile1.set_line_width(2.0);
    psfile1.set_fg_color((float)color.R(),(float)color.G(),(float)color.B());
    
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
  }

  //close file
  psfile1.close();
  return true;
}

//: create image with traced boundaries
bool create_ps_patches_with_scurve(vcl_string ps_file_name, vcl_vector<dbskr_shock_patch_sptr>& patches, vcl_vector<vil_rgb<int> >& colors,
                       dbskr_scurve_sptr& curve, vcl_vector<vil_rgb<int> >& curve_colors,
                       vil_image_resource_sptr background_img)
{
  if (patches.size() != colors.size()) {
    vcl_cout << "colors array is not same size as pathces array, image not written\n";
    return false;
  }
  if (curve_colors.size() != 3) {
    vcl_cout << "curve colors array is not of size 3, image not written\n";
    return false;
  }

 //1)If file open fails, return.
  vul_psfile psfile1(ps_file_name.c_str(), false);

  if (!psfile1) {
    vcl_cout << " Error opening file  " << ps_file_name.c_str() << vcl_endl;
    return false;
  }

  //: put the background
  //: determine size
  int sizex, sizey; // (int)vcl_floor(w1 + w2 + 0.5) + 4*offset;   //off + w1 + off + off + w2 + off
                    //int sizey = (int)vcl_floor((h1 > h2 ? h1 : h2) + 0.5) + 4*offset;

  if (background_img == 0) {
    vsol_box_2d_sptr box = new vsol_box_2d();
    for (unsigned i = 0; i < patches.size(); i++) {
      vsol_polygon_2d_sptr poly = patches[i]->get_traced_boundary();
      poly->compute_bounding_box();
      box->grow_minmax_bounds(poly->get_bounding_box());
    }
    sizex = (int)vcl_ceil(box->get_max_x()-box->get_min_x())+10;
    sizey = (int)vcl_ceil(box->get_min_x()-box->get_min_y())+10;
  } else {
    sizex = background_img->ni();
    sizey = background_img->nj();
  }

  unsigned char *buf = new unsigned char[sizex*sizey*3];
  if (background_img == 0) {
    for (int x=0; x<sizex; x++) {
      for (int y=0; y<sizey; y++) {
        buf[3*(x+sizex*y)  ] = 255;
        buf[3*(x+sizex*y)+1] = 255;
        buf[3*(x+sizex*y)+2] = 255;
      }
    }
  } else {

    vil_image_view<vxl_byte> image = background_img->get_view(0, background_img->ni(), 0, background_img->nj());
    int planes = image.nplanes();

    if (planes == 3) {
      vcl_cout << "processing color image\n";
      for (int x=0; x<sizex; x++) {
        for (int y=0; y<sizey; y++) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,1);
          buf[3*(x+sizex*y)+2] = image(x,y,2);
        }
      }
    } else if (planes == 1) {
      vcl_cout << "processing grey image\n";
      for (int x=0; x<sizex; x++) {
        for (int y=0; y<sizey; y++) {
          buf[3*(x+sizex*y)  ] = image(x,y,0);
          buf[3*(x+sizex*y)+1] = image(x,y,0);
          buf[3*(x+sizex*y)+2] = image(x,y,0);
        }
      }
    }
  }

  psfile1.print_color_image(buf,sizex,sizey);
  delete [] buf;
  psfile1.reset_bounding_box();

  psfile1.set_scale_x(50);
  psfile1.set_scale_y(50);

  for (unsigned i = 0; i < patches.size(); i++) {
    vsol_polygon_2d_sptr poly = patches[i]->get_traced_boundary();
    vil_rgb<int> color = colors[i];

    // parse through all the vsol classes and save curve objects only
    psfile1.set_fg_color(1, 1, 1);
    psfile1.set_line_width(4.0);
    for (unsigned int i=1; i<poly->size();i++)
    {
        vsol_point_2d_sptr p1 = poly->vertex(i-1);
        vsol_point_2d_sptr p2 = poly->vertex(i);
        psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
    psfile1.set_line_width(2.0);
    psfile1.set_fg_color((float)color.R(),(float)color.G(),(float)color.B());
    
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
  }

  vcl_vector<vsol_polygon_2d_sptr> polys;
  curve->get_polys(polys);
  for (unsigned kk = 0; kk < polys.size(); kk++) {
    vsol_polygon_2d_sptr poly = fit_lines_to_contour(polys[kk], 0.05f);
    vil_rgb<int> color = curve_colors[kk];

    // parse through all the vsol classes and save curve objects only
    psfile1.set_fg_color(1, 1, 1);
    psfile1.set_line_width(4.0);
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
    psfile1.set_line_width(2.0);
    psfile1.set_fg_color((float)color.R(),(float)color.G(),(float)color.B());
    
    for (unsigned int i=1; i<poly->size();i++)
    {
      vsol_point_2d_sptr p1 = poly->vertex(i-1);
      vsol_point_2d_sptr p2 = poly->vertex(i);
      psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
    }
  }

  //close file
  psfile1.close();
  return true;
}

//: tests whether the shock graph is created ok for recognition purposes 
bool test_shock_graph_for_rec(dbsk2d_shock_graph_sptr sg)
{
  if (!test_xshock_graph(sg)) 
    return false;

  //: reconstruct edges
  for ( dbsk2d_shock_graph::edge_iterator curE = sg->edges_begin();
        curE != sg->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr selm = (*curE);
    dbsk2d_shock_node_sptr n = selm->source();
    vcl_vector<dbsk2d_shock_edge_sptr> edges(1, selm);
    dbskr_scurve_sptr sc = dbskr_compute_scurve(n, edges, true, true, true, 1.0f, 1.0f);
    if (!sc)
      return false;
    if ( !(sc->arclength(sc->num_points()-1) > 0) )
      return false;
  }

  return true;
}


