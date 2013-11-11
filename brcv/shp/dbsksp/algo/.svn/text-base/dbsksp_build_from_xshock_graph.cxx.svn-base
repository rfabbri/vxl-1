// This is file shp/dbsksp/dbsksp_build_from_xshock_graph.cxx

//:
// \file

#include "dbsksp_build_from_xshock_graph.h"
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/algo/dbsksp_interp_two_xnodes.h>

#include <vgl/vgl_distance.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_compute_scurve.h>

// ----------------------------------------------------------------------------
//: Retrieve the list of edges corresponding to a dart
vcl_vector<dbsksp_shock_edge_sptr > dbsksp_build_from_xshock_graph::
get_shock_edges_of_dart(int dart)
{
  vcl_map<int, vcl_vector<dbsksp_shock_edge_sptr > >::iterator itr = 
    this->dart_to_edges_map_.find(dart);
  if (itr == this->dart_to_edges_map_.end())
    return vcl_vector<dbsksp_shock_edge_sptr >();
  else
    return itr->second;
}



// ----------------------------------------------------------------------------
//: Retrieve the list of edges corresponding to a dart
vcl_vector<dbsksp_shock_node_sptr > dbsksp_build_from_xshock_graph::
get_shock_nodes_of_dart(int dart)
{
  vcl_map<int, vcl_vector<dbsksp_shock_node_sptr > >::iterator itr = 
    this->dart_to_nodes_map_.find(dart);
  if (itr == this->dart_to_nodes_map_.end())
    return vcl_vector<dbsksp_shock_node_sptr >();
  else
    return itr->second;
}




// ----------------------------------------------------------------------------
//: Method to build from extrinsic shock graph
dbsksp_shock_graph_sptr dbsksp_build_from_xshock_graph::
build_from_xshock_graph_using_skr_tree(const dbsk2d_shock_graph_sptr& xshock)
{
  // 1) prepare the tree
  bool elastic_splice_cost = true;
  bool construct_circular_ends = false;

  //: ozge added 
  bool combined_edit = true; // use Amir and Ozge's combined cost function in interval cost computation during scurve matching

  dbskr_tree_sptr tree = new dbskr_tree(this->scurve_sample_ds_);
  tree->acquire(xshock, elastic_splice_cost, construct_circular_ends, combined_edit); 
  this->set_skr_tree(tree);

  // 2) Build from the tree

  //// typical method
  //dbsksp_shock_graph_sptr shock_graph = this->build_from_skr_tree(this->skr_tree());

  // Try the new method which requires the predefined number of segments for each dart
  vcl_vector<int > num_segments;
  this->compute_num_segments_for_darts(this->skr_tree(), this->shock_interp_sample_ds_,
    num_segments);

  dbsksp_shock_graph_sptr shock_graph = this->build_from_skr_tree(this->skr_tree(),
    num_segments);

  return shock_graph;
}


//// ----------------------------------------------------------------------------
////: Method to build from a shock matching tree
//dbsksp_shock_graph_sptr dbsksp_build_from_xshock_graph::
//build_from_skr_tree(const dbskr_tree_sptr& tree)
//{
//  // first thing first
//  this->set_xshock(tree->get_shock_graph());
//  this->set_gshock(new dbsksp_shock_graph());
//
//
//  // 1) Construct generative shock graph equivalent to the tree 
//  // iterate over the darts of this tree to reconstruct everything
//  // this is actually an Euler tour because of the way the tree is constructed
//  for (int dart=0; dart < tree->size(); dart++)
//  {
//    if (tree->mate(dart)<dart)
//      continue; //already drawn from the other side
//
//    //temp data structures
//    dbsk2d_shock_node_sptr start_node;
//    vcl_vector<dbsk2d_shock_edge_sptr> edges;
//    vcl_vector<int> dart_list;
//
//    //get shock edge list from this path
//    dart_list.clear();
//    dart_list.push_back(dart);
//
//    edges.clear();
//    tree->edge_list(dart_list, start_node, edges); 
//
//    double sampling_ds = 1.0;
//    dbskr_scurve_sptr sc = 
//      dbskr_compute_scurve(start_node, edges, false, true, true, 1.0, sampling_ds);
//
//    // reconstruct each dart separately
//    // keep track of the head and tail nodes, do not duplicate
//    
//    // a. Create head and tail shock nodes of the dart, if they have not been created yet
//    // tail
//    dbsksp_shock_node_sptr dart_tail;
//    vcl_map<int, dbsksp_shock_node_sptr >::iterator node_itr = 
//      this->coarse_graph_nodes_map_.find(tree->tail(dart));
//
//    if (node_itr == this->coarse_graph_nodes_map_.end())
//    {
//      // the node has not been visited, create a new one for it
//      dart_tail = new dbsksp_shock_node(this->gshock()->next_available_id());
//      this->coarse_graph_nodes_map_.insert(vcl_make_pair(tree->tail(dart), dart_tail));
//    }
//    else
//    {
//      // the node has been visited, just retrieve it
//      dart_tail = node_itr->second;
//    }
//
//    
//    // head
//    dbsksp_shock_node_sptr dart_head;
//    node_itr = this->coarse_graph_nodes_map_.find(tree->head(dart));
//    if (node_itr == this->coarse_graph_nodes_map_.end())
//    {
//      dart_head = new dbsksp_shock_node(this->gshock()->next_available_id());
//      this->coarse_graph_nodes_map_.insert(vcl_make_pair(tree->head(dart), dart_head));
//    }
//    else
//    {
//      dart_head = node_itr->second;
//    }
//
//    // b. Create edges and nodges for the shock cuve
//    vcl_vector<dbsksp_shock_edge_sptr > edge_list;
//    vcl_vector<dbsksp_shock_node_sptr > node_list;
//    
//    this->interpolate_shock_curve(sc, dart_tail, dart_head,
//      node_list,
//      edge_list);
//
//    // cache this mapping
//    this->dart_to_edges_map_.insert(vcl_make_pair(dart, edge_list));
//    this->dart_to_nodes_map_.insert(vcl_make_pair(dart, node_list));
//  }
//
//
//  // 2) Close the boundary at degree-one nodes
//  this->close_boundary_at_degree_one_nodes();
//
//
//  // 3) insert the nodes and edges to the graph
//  for (vcl_map<int, dbsksp_shock_node_sptr >::iterator itr = 
//    this->nodes_map_.begin(); itr != this->nodes_map_.end(); ++itr)
//  {
//    this->gshock()->add_vertex(itr->second);
//  }
//
//  for (vcl_map<int, dbsksp_shock_edge_sptr >::iterator itr =
//    this->edges_map_.begin(); itr != this->edges_map_.end(); ++itr)
//  {
//    this->gshock()->add_edge(itr->second);
//  }
//
//
//  // 4) Set the reference nodes and edges
//  dbsksp_shock_node_sptr ref_node = *this->gshock()->vertices_begin();
//  dbsksp_shock_edge_sptr ref_edge = *this->gshock()->edges_begin();
//
//  this->gshock()->set_ref_node(ref_node);
//  this->gshock()->set_ref_node_radius(ref_node->radius());
//  this->gshock()->set_ref_origin(ref_node->pt());
//
//  this->gshock()->set_ref_edge(ref_edge);
//  this->gshock()->set_ref_direction(ref_edge->chord_dir());
//
//  // 5) Compute all dependent variables
//  this->gshock()->compute_all_dependent_params();
//  
//  return this->gshock();
//}



//// ----------------------------------------------------------------------------
////: Convert a shock curve, typically from a dart, into a sequence of nodes
//// and edges in the generative shock graph
//void dbsksp_build_from_xshock_graph::
//interpolate_shock_curve(const dbskr_scurve_sptr& sc, 
//                    const dbsksp_shock_node_sptr& start_node,
//                    const dbsksp_shock_node_sptr& end_node,
//                    vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
//                    vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges)
//{
//  // 1) Extract the shock samples that will be used to construct the generative
//  // shock nodes and edges
//  vcl_vector<dbsksp_xshock_node_descriptor > xshock_node_list;
//  {
//    double distance_threshold = this->scurve_sample_ds();
//    vgl_point_2d<double > prev_pt(1e10, 1e10);
//    for (int i=0; (i+1) <sc->num_points(); i = i+1)
//    {
//      vgl_point_2d<double > sh_pt = sc->sh_pt(i);
//      vgl_point_2d<double > left_bnd_pt = sc->bdry_plus_pt(i);
//      vgl_point_2d<double > right_bnd_pt = sc->bdry_minus_pt(i);
//
//      // only insert if the points are far apart
//      if (vgl_distance(prev_pt, sh_pt) < distance_threshold)
//      {
//        //vcl_cerr << "ERROR: sampled points are too close to each other\n";
//        continue;
//      }
//
//      xshock_node_list.push_back(
//        dbsksp_xshock_node_descriptor(sh_pt, left_bnd_pt, right_bnd_pt));
//
//      prev_pt = sh_pt;
//    }
//
//    // make sure the last sample is inserted in the list
//    vgl_point_2d<double > last_sh_pt = sc->sh_pt(sc->num_points()-1);
//    vgl_point_2d<double > last_left_bnd_pt = sc->bdry_plus_pt(sc->num_points()-1);
//    vgl_point_2d<double > last_right_bnd_pt = sc->bdry_minus_pt(sc->num_points()-1);
//    if (vgl_distance(prev_pt, last_sh_pt) < distance_threshold && 
//      xshock_node_list.size() > 1)
//    {
//      xshock_node_list.pop_back();
//    }
//    xshock_node_list.push_back(dbsksp_xshock_node_descriptor(last_sh_pt, 
//        last_left_bnd_pt, last_right_bnd_pt));
//  }
//  
//  assert (xshock_node_list.size() > 1);
//
//  // 2) Interpolate the extrinsic shock samples
//
//  this->interpolate_xnodes(xshock_node_list, start_node, end_node,
//    ordered_nodes,
//    ordered_edges);
//  return;
//}






// ----------------------------------------------------------------------------
//: Interpolate an order list of xnodes with a sequence of shock edges
void  dbsksp_build_from_xshock_graph::
interpolate_xnodes(const vcl_vector<dbsksp_xshock_node_descriptor >& xshock_node_list,
                   const dbsksp_shock_node_sptr& start_node,
                   const dbsksp_shock_node_sptr& end_node,
                   vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
                   vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges)
{
  // use the extract samples to construct the generative shock edges.
  // We will first create the topology to support the interpolation
  // Then we fill in the data using xnode interpolator
  // Note that each pair of samples requires two shock edges to interpolate (twoshapelet)
  // 1) create the nodes.
  vcl_vector<dbsksp_shock_node_sptr > node_list;
  node_list.push_back(start_node);
  for (unsigned i=1; (i+1)<xshock_node_list.size(); ++i)
  {
    node_list.push_back(new dbsksp_shock_node(this->gshock()->next_available_id()));    
    node_list.push_back(new dbsksp_shock_node(this->gshock()->next_available_id()));    
  }
  node_list.push_back(new dbsksp_shock_node(this->gshock()->next_available_id()));    
  node_list.push_back(end_node);

  // 2) create the edges connecting the nodes
  vcl_vector<dbsksp_shock_edge_sptr > edge_list;
  for (unsigned i=0; (i+1)< node_list.size(); ++i)
  {
    dbsksp_shock_node_sptr source = node_list[i];
    dbsksp_shock_node_sptr target = node_list[i+1];
    dbsksp_shock_edge_sptr new_edge = 
      new dbsksp_shock_edge(source, target, this->gshock()->next_available_id());
    source->add_edge(new_edge);
    target->add_edge(new_edge);
    edge_list.push_back(new_edge);
  }

  // 3) Fill in the data for nodes and edges using xnode interpolator
  for (unsigned int i=0; (i+1)<xshock_node_list.size(); ++i)
  {
    // a) Interpolate each pair of two samples using a twoshapelet
    dbsksp_xshock_node_descriptor xnode_start = xshock_node_list[i];
    dbsksp_xshock_node_descriptor xnode_end = xshock_node_list[i+1];

    // xnode interpolator
    dbsksp_optimal_interp_two_xnodes interpolator(xnode_start, xnode_end);
    dbsksp_twoshapelet_sptr ss = interpolator.optimize();

    if (!ss || !ss->is_legal())
    {
      vcl_cerr << "ERROR: xnode interpolation failed.\n";
      
      //return;
    }


    ////////////////////////////////////////////////////////////
    // for debug
    // balanced length
    double length_ratio = ss->len0() / (ss->len0() + ss->len1());

    vcl_cout << "i = " << i << " len0 = " << ss->len0() 
      << " len1 = " << ss->len1() << vcl_endl;

    if (length_ratio < 0.1 || length_ratio > 0.9)
    {
      vcl_cerr << "ERROR: the twoshapelet is not well balanced.\n";
      vcl_cerr << "start_error = " << interpolator.start_error_
        << " end_error = " << interpolator.end_error_ << vcl_endl;

      
    }

    ///////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////
    // for debug
    this->shapelets.push_back(ss->shapelet_start());
    this->shapelets.push_back(ss->shapelet_end());
    ///////////////////////////////////////////////////////////////

    
    // b) Plug data into the nodes and edges
    dbsksp_shock_node_sptr v0 = node_list[2*i];
    dbsksp_shock_node_sptr v1 = node_list[2*i+1];
    dbsksp_shock_node_sptr v2 = node_list[2*i+2];
    dbsksp_shock_edge_sptr e0 = edge_list[2*i];
    dbsksp_shock_edge_sptr e1 = edge_list[2*i+1];

    v0->descriptor(e0)->phi = ss->phi0();
    v0->set_pt(ss->shapelet_start()->start());
    v0->set_radius(ss->r0());

    v1->descriptor(e0)->phi = vnl_math::pi - ss->phi1();
    v1->descriptor(e1)->phi = ss->phi1();
    v1->set_pt(ss->shapelet_start()->end());
    v1->set_radius(ss->shapelet_start()->radius_end());

    v2->descriptor(e1)->phi = vnl_math::pi - ss->phi2();
    v2->set_pt(ss->shapelet_end()->end());
    v2->set_radius(ss->shapelet_end()->radius_end());
    
    e0->set_chord_length(ss->len0());
    e0->set_param_m(ss->m0());
    e0->set_chord_dir(ss->shapelet_start()->chord_dir());

    e1->set_chord_length(ss->len1());
    e1->set_param_m(ss->m1());  
    e1->set_chord_dir(ss->shapelet_end()->chord_dir());
  }

  // 4. Cache the edges and nodes
  for (unsigned i=0; i<node_list.size(); ++i)
  {
    this->nodes_map_.insert(vcl_make_pair(node_list[i]->id(), node_list[i]));
  }

  for (unsigned i=0; i<edge_list.size(); ++i)
  {
    this->edges_map_.insert(vcl_make_pair(edge_list[i]->id(), edge_list[i]));
  }

  // 5. Return the edges
  ordered_nodes = node_list;
  ordered_edges = edge_list;
  
  return;
}








//: Close the boundary at degree-one nodes
void dbsksp_build_from_xshock_graph::
close_boundary_at_degree_one_nodes()
{
  vcl_vector<dbsksp_shock_node_sptr > terminal_nodes;
  for (vcl_map<int, dbsksp_shock_node_sptr >::iterator itr = 
    this->nodes_map_.begin(); itr != this->nodes_map_.end(); ++itr)
  {
    dbsksp_shock_node_sptr node = itr->second;
    if (node->degree() > 1)
      continue;
    if (node->degree() == 0)
    {
      vcl_cerr << "ERROR: there are degree-0 nodes in the nodes_map.\n";
      continue;
    }

    assert(node->degree() == 1);

    // Now we are sure node has degree 1. Let's close it
    dbsksp_shock_node_sptr terminal_node = 
      new dbsksp_shock_node(this->gshock()->next_available_id());
    dbsksp_shock_edge_sptr terminal_edge = 
      new dbsksp_shock_edge(terminal_node, node, this->gshock()->next_available_id());
    terminal_node->add_edge(terminal_edge);
    double phi_bar = node->descriptor_list().front()->phi;
    node->add_edge(terminal_edge);

    // data for the new edge and new node
    dbsksp_shock_node_descriptor_sptr d0 = terminal_node->descriptor(terminal_edge);
    d0->phi = vnl_math::pi;
    d0->alpha = 0;
    d0->shock_flow_dir = 1;
    d0->rot_angle_to_succ_edge = 2*vnl_math::pi;


    dbsksp_shock_node_descriptor_sptr d1 = node->descriptor(terminal_edge);
    d1->phi = vnl_math::pi - phi_bar;
    d1->alpha = 0;
    d1->shock_flow_dir = -1;
    d1->rot_angle_to_succ_edge = vnl_math::pi; // this may not be accurate but will be resolved

    terminal_edge->set_chord_length(0);
    terminal_edge->set_param_m(0);

    // cache the new node and edges
    this->edges_map_.insert(vcl_make_pair(terminal_edge->id(), terminal_edge));
    terminal_nodes.push_back(terminal_node);
  }

  for (unsigned i=0; i<terminal_nodes.size(); ++i)
  {
    dbsksp_shock_node_sptr node = terminal_nodes[i];
    this->nodes_map_.insert(vcl_make_pair(node->id(), node));
  }






}











//: Method to build from a shock matching tree and the number of segments
// corresponding to each dart
dbsksp_shock_graph_sptr dbsksp_build_from_xshock_graph::
build_from_skr_tree(const dbskr_tree_sptr& tree, const vcl_vector<int >& num_segments)
{
  if (num_segments.size() != tree->size())
  {
    vcl_cerr << "ERROR: in dbsksp_build_from_xshock_graph::build_from_skr_tree \n"
      << "  num_segments.size() != tree->size() \n";
    return 0;
  }

  // first thing first
  this->set_xshock(tree->get_shock_graph());
  this->set_gshock(new dbsksp_shock_graph());

  // 1) Construct generative shock graph equivalent to the tree 
  // iterate over the darts of this tree to reconstruct everything
  // this is actually an Euler tour because of the way the tree is constructed
  for (int dart=0; dart < tree->size(); dart++)
  //for (int dart=0; dart < 2; dart++)
  {
    if (tree->mate(dart)<dart)
      continue; //already drawn from the other side

    //temp data structures
    dbsk2d_shock_node_sptr start_node;
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    vcl_vector<int> dart_list;

    //get shock edge list from this path
    dart_list.clear();
    dart_list.push_back(dart);

    edges.clear();
    tree->edge_list(dart_list, start_node, edges); 

    double sampling_ds = 1.0;
    dbskr_scurve_sptr sc = 
      dbskr_compute_scurve(start_node, edges, false, true, true, 1.0, sampling_ds);

    // reconstruct each dart separately
    // keep track of the head and tail nodes, do not duplicate
    
    // a. Create head and tail shock nodes of the dart, if they have not been created yet
    // tail
    dbsksp_shock_node_sptr dart_tail;
    vcl_map<int, dbsksp_shock_node_sptr >::iterator node_itr = 
      this->coarse_graph_nodes_map_.find(tree->tail(dart));

    if (node_itr == this->coarse_graph_nodes_map_.end())
    {
      // the node has not been visited, create a new one for it
      dart_tail = new dbsksp_shock_node(this->gshock()->next_available_id());
      this->coarse_graph_nodes_map_.insert(vcl_make_pair(tree->tail(dart), dart_tail));
    }
    else
    {
      // the node has been visited, just retrieve it
      dart_tail = node_itr->second;
    }

    
    // head
    dbsksp_shock_node_sptr dart_head;
    node_itr = this->coarse_graph_nodes_map_.find(tree->head(dart));
    if (node_itr == this->coarse_graph_nodes_map_.end())
    {
      dart_head = new dbsksp_shock_node(this->gshock()->next_available_id());
      this->coarse_graph_nodes_map_.insert(vcl_make_pair(tree->head(dart), dart_head));
    }
    else
    {
      dart_head = node_itr->second;
    }

    // b. Create edges and nodges for the shock cuve
    vcl_vector<dbsksp_shock_edge_sptr > edge_list;
    vcl_vector<dbsksp_shock_node_sptr > node_list;
    
    this->interpolate_shock_curve(sc, dart_tail, dart_head,
      num_segments[dart],
      node_list,
      edge_list);

    // cache this mapping
    this->dart_to_edges_map_.insert(vcl_make_pair(dart, edge_list));
    this->dart_to_nodes_map_.insert(vcl_make_pair(dart, node_list));
  }


  // 2) Close the boundary at degree-one nodes
  this->close_boundary_at_degree_one_nodes();


  // 3) insert the nodes and edges to the graph
  for (vcl_map<int, dbsksp_shock_node_sptr >::iterator itr = 
    this->nodes_map_.begin(); itr != this->nodes_map_.end(); ++itr)
  {
    this->gshock()->add_vertex(itr->second);
  }

  for (vcl_map<int, dbsksp_shock_edge_sptr >::iterator itr =
    this->edges_map_.begin(); itr != this->edges_map_.end(); ++itr)
  {
    this->gshock()->add_edge(itr->second);
  }


  // 4) Set the reference nodes and edges
  dbsksp_shock_node_sptr ref_node = *this->gshock()->vertices_begin();
  dbsksp_shock_edge_sptr ref_edge = *this->gshock()->edges_begin();

  this->gshock()->set_ref_node(ref_node);
  this->gshock()->set_ref_node_radius(ref_node->radius());
  this->gshock()->set_ref_origin(ref_node->pt());

  this->gshock()->set_ref_edge(ref_edge);
  this->gshock()->set_ref_direction(ref_edge->chord_dir());

  // 5) Compute all dependent variables
  this->gshock()->compute_all_dependent_params();
  
  return this->gshock();
}









// ----------------------------------------------------------------------------
//: Convert a shock curve, typically from a dart, consisting of ``num_segments''
// into a sequence nodes and edges in the generative shock graph
void dbsksp_build_from_xshock_graph::
interpolate_shock_curve(const dbskr_scurve_sptr& sc, 
                        const dbsksp_shock_node_sptr& start_node,
                        const dbsksp_shock_node_sptr& end_node,
                        int num_segments,
                        vcl_vector<dbsksp_shock_node_sptr >& ordered_nodes,
                        vcl_vector<dbsksp_shock_edge_sptr >& ordered_edges)
{
  assert (sc->num_points() > 1);
  assert (num_segments > 0);
  // 1) Create a xshock_node_list from the shock samples 
  // These xshock_nodes will be used to construct the generative shock nodes and edges
  vcl_vector<dbsksp_xshock_node_descriptor > xshock_node_list;
  
  double arclength_per_segment = (sc->arclength(sc->num_points()-1))  / num_segments;
  

  // The assumption is the that no two consecutive samples are more than 
  // the arclength_per_segment apart so that each interval has at least 
  // one sample in between.
  // There is no better way to assure this assumption than checking it
  bool sampling_dense_enough = true;
  for (int i=1; i<sc->num_points(); ++i)
  {
    double len = sc->arclength(i) - sc->arclength(i-1);
    sampling_dense_enough = sampling_dense_enough && (len <= arclength_per_segment);
  }

  if (!sampling_dense_enough)
  {
    vcl_cerr << "ERROR: sampling along the scuve is not dense enough.\n";
    assert(false);
    return;
  }

  // create a vector of "stopping locations" for the xnodes. 
  // Need to make sure the end points match
  vcl_vector<int > xnode_locations(num_segments + 1, 0);
  xnode_locations[0] = 0;

  int xnode_running_index = 1;
  for (int i=1; (i<sc->num_points()) && (xnode_running_index < num_segments); ++i)
  {
    double len = sc->arclength(i);
    if (len > (xnode_running_index * arclength_per_segment))
    {
      xnode_locations[xnode_running_index] = i;
      ++xnode_running_index;
    }
  }
  // make sure the end points match
  xnode_locations[xnode_running_index] = sc->num_points()-1;
  if (xnode_running_index != num_segments)
  {
    vcl_cout << "ERROR: something wrong with the xnode location assignment.\n";
    assert(false);
  }

  // Now we have the sample locations of the xnodes, we iterate and create the xnodes
  
  for (int xnode_index =0; xnode_index < (num_segments + 1); ++xnode_index)
  {
    int sample_index = xnode_locations[xnode_index];
    vgl_point_2d<double > sh_pt = sc->sh_pt(sample_index);
    vgl_point_2d<double > left_bnd_pt = sc->bdry_plus_pt(sample_index);
    vgl_point_2d<double > right_bnd_pt = sc->bdry_minus_pt(sample_index);

    xshock_node_list.push_back(
      dbsksp_xshock_node_descriptor(sh_pt, left_bnd_pt, right_bnd_pt));
  }

  // 2) Interpolate the extrinsic shock samples

  this->interpolate_xnodes(xshock_node_list, start_node, end_node,
    ordered_nodes,
    ordered_edges);
  return;
}









//: Compute number of segments for each dart in the tree given a sampling rate
void dbsksp_build_from_xshock_graph::
compute_num_segments_for_darts(const dbskr_tree_sptr& tree,
                               double segment_ds, 
                               vcl_vector<int >& num_segments)
{
  num_segments.resize(tree->size(), 0);

  if (segment_ds < 1)
  {
    vcl_cerr << "ERROR: segment_ds should be at least 1.\n";
    return;
  }

  for (int dart=0; dart < tree->size(); dart++)
  {
    if (tree->mate(dart)<dart)
      continue; //already drawn from the other side

    //temp data structures
    dbsk2d_shock_node_sptr start_node;
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    vcl_vector<int> dart_list;

    //get shock edge list from this path
    dart_list.clear();
    dart_list.push_back(dart);

    edges.clear();
    tree->edge_list(dart_list, start_node, edges); 

    double sampling_ds = 1.0;
    dbskr_scurve_sptr sc = 
      dbskr_compute_scurve(start_node, edges, false, true, true, 1.0, sampling_ds);

    int n = (int) vcl_floor(sc->arclength(sc->num_points()-1) / segment_ds);

    // make sure each dart has at least one segment
    n = vnl_math_max(n , 1);

    // save the number of segments for this dart
    num_segments[dart] = n;
    num_segments[tree->mate(dart)] = n;
  }


  return;
}
