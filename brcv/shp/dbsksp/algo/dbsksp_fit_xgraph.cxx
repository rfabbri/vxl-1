// This is file shp/dbsksp/algo/dbsksp_fit_xgraph.cxx

//:
// \file

#include "dbsksp_fit_xgraph.h"

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

#include <dbsksp/algo/dbsksp_compute_scurve.h>
#include <dbsksp/algo/dbsksp_xgraph_algos.h>
#include <dbsksp/algo/dbsksp_fit_one_shock_branch_cost_function.h>

#include <dbsksp/algo/dbsksp_fit_shock_path.h>
#include <dbsksp/algo/dbsksp_shock_path.h>

#include <dbskr/dbskr_scurve.h>

#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbgl/algo/dbgl_compute_symmetry_point.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbnl/dbnl_angle.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vgl/vgl_distance.h>


//==============================================================================
// dbsksp_fit_xgraph
//==============================================================================



//------------------------------------------------------------------------------
//: Fit an extrinsic dbsk2d_shock_graph to a generative dbsksp_xshock_graph
dbsksp_xshock_graph_sptr dbsksp_fit_xgraph::
fit_to(const dbsk2d_shock_graph_sptr& graph)
{
  type_map_sksp_edge_to_sk2d_edges map_xe_to_orig_edges;
  type_map_sksp_node_to_sk2d_node map_sksp_node_to_sk2d_node;
  
  //1) Fit the coarse shock graph
  dbsksp_xshock_graph_sptr xgraph = this->convert_coarse_xgraph_using_euler_tour(graph, 
    false,
    map_sksp_node_to_sk2d_node, map_xe_to_orig_edges);

     
  //2) Fill in the coarse shock graphs with A12 nodes
  // Now each edge in xgraph corresponds to one or several edges in dbsk2d_shock_graph. 
  // However the reconstructed boundary in xgraph may not match well with the dbsk2d_shock_graph
  // We will compute middle shock points to xgraph and optimize to reduce error with
  // original samples

  // Iterate thru the edges and figure out the additional middle A12 xnodes to add
  vcl_map<dbsksp_xshock_edge_sptr, vcl_vector<dbsksp_xshock_node_descriptor > > map_edge_to_list_middle_xdesc;

  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit != 
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    // search for corresponding edges in dbsk2d graph
    type_map_sksp_edge_to_sk2d_edges::iterator iter = map_xe_to_orig_edges.find(xe);
    if (iter == map_xe_to_orig_edges.end())
      continue;

    // no terminal edge should have a corresponding edge in the original dbsk2d_shock_graph
    assert(! xe->is_terminal_edge());

    // List of edges in the original dbsk2d_shock_graph corresponding this edge
    vcl_vector<dbsk2d_xshock_edge_sptr > orig_edges = iter->second;

    // Collect shock samples from the list of shock edges
    vcl_vector<dbsksp_xshock_node_descriptor > xsamples;
    
    // allocate space
    unsigned total_samples = 0;
    for (unsigned i =0; i < orig_edges.size(); ++i)
    {
      total_samples += orig_edges[i]->num_samples();
    }
    xsamples.reserve(total_samples);


    // concatenate samples from the original shock graph
    dbsk2d_shock_node_sptr start_xv = map_sksp_node_to_sk2d_node[xe->source()];
    for (unsigned i =0; i < orig_edges.size(); ++i)
    {
      dbsk2d_xshock_edge_sptr sk2d_xe = orig_edges[i];

      // Determine the order of inserting the samples depending on node correspondence
      if (start_xv == sk2d_xe->source())
      {
        for (int k =0; k < sk2d_xe->num_samples(); ++k)
        {
          dbsk2d_xshock_sample_sptr sample = sk2d_xe->sample(k);
          dbsksp_xshock_node_descriptor xdesc(sample->pt, sample->left_bnd_pt, sample->right_bnd_pt);
          xsamples.push_back(xdesc);
        }
      }
      else
      {
        for (int k = sk2d_xe->num_samples()-1; k >= 0; --k)
        {
          dbsk2d_xshock_sample_sptr sample = sk2d_xe->sample(k);
          dbsksp_xshock_node_descriptor xdesc(sample->pt, sample->right_bnd_pt, sample->left_bnd_pt);
          xsamples.push_back(xdesc);
        }
      }

      start_xv = sk2d_xe->opposite(start_xv);
    }

    
    //Here comes the main task: determine intermediate A12 shock points
    dbsksp_xshock_node_descriptor start_xdesc = *(xe->source()->descriptor(xe));
    dbsksp_xshock_node_descriptor end_xdesc = xe->target()->descriptor(xe)->opposite_xnode();
    vcl_vector<dbsksp_xshock_node_descriptor > list_middle_xdesc;
    {
      ////
      //dbsksp_fit_one_shock_branch_with_power_of_2_intervals(start_xdesc, end_xdesc, 
      //  xsamples, this->distance_rms_error_threshold_, list_middle_xdesc);

      dbsksp_shock_path_sptr in_path = new dbsksp_shock_path(xsamples);
      dbsksp_shock_path_sptr out_path = 0;

      dbsksp_fit_shock_path_using_min_num_pts(in_path, start_xdesc, end_xdesc, 
        this->distance_rms_error_threshold_,
        out_path);

      list_middle_xdesc.clear();
      for (unsigned i =1; (i+1) < out_path->num_points(); ++i)
      {
        list_middle_xdesc.push_back(*out_path->xdesc(i));
      }
    }

    if (!list_middle_xdesc.empty())
    {
      map_edge_to_list_middle_xdesc.insert(vcl_make_pair(xe, list_middle_xdesc));
    }
  }

  // Insert the additional A12 xnodes
  for (vcl_map<dbsksp_xshock_edge_sptr, vcl_vector<dbsksp_xshock_node_descriptor > >::iterator iter =
    map_edge_to_list_middle_xdesc.begin(); iter != map_edge_to_list_middle_xdesc.end(); ++iter)
  {
    dbsksp_xshock_edge_sptr xe = iter->first;
    dbsksp_xshock_node_sptr target = xe->target();
    vcl_vector<dbsksp_xshock_node_descriptor >& list_middle_xdesc = iter->second;

    // Insert new node to the edge xe, one by one, in the order from source to target
    for (unsigned i =0; i < list_middle_xdesc.size(); ++i)
    {
      dbsksp_xshock_node_descriptor xdesc = list_middle_xdesc[i];
      dbsksp_xshock_node_sptr new_xnode = xgraph->insert_xshock_node(xe, xdesc);
      assert(new_xnode->degree() == 2);
      
      // Prepare to move on to the next edge
      // Figure out which edge connects the old "source" node and new node
      for (dbsksp_xshock_node::edge_iterator eit = new_xnode->edges_begin(); eit !=
        new_xnode->edges_end(); ++eit)
      {
        if ((*eit)->opposite(new_xnode) == target)
        {
          xe = *eit;
          break;
        }
      }
    }
  }

  return xgraph;  
}










//------------------------------------------------------------------------------
//: Fit a generative dbsksp_xshock_graph
void dbsksp_fit_xgraph::
fit_to(const dbsksp_xshock_graph_sptr& old_xgraph,
       dbsksp_xshock_graph_sptr& new_xgraph,
       vcl_map<dbsksp_xshock_node_sptr, dbsksp_xshock_node_sptr >& map_new_node_to_old_node,
       double sample_ds)
{
  // rinse old data
  new_xgraph = 0;
  map_new_node_to_old_node.clear();


  // place holder for new graph
  dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph(*old_xgraph);

  // Compute the list of shock branches
  vcl_vector<type_branch_key > list_branch_key;
  vcl_vector<type_branch_edges > list_branch_edges;
  this->compute_shock_branches(xgraph, list_branch_key, list_branch_edges);

  // Now sample and fit each shock branch with
  for (unsigned k =0; k < list_branch_edges.size(); ++k)
  {
    vcl_vector<dbsksp_xshock_edge_sptr > list_edge = list_branch_edges[k];
    type_branch_key key = list_branch_key[k];
    dbsksp_xshock_node_sptr start_node = key.first;
    dbsksp_xshock_node_sptr end_node = key.second;

    // ignore the terminal edge (A_infty node) during fitting

    // if the branch ends with an A_infty node, remove the last edge
    if (end_node->degree() == 1)
    {
      dbsksp_xshock_edge_sptr last_edge = list_edge.back();
      end_node = last_edge->opposite(end_node);
      list_edge.pop_back();
    }

    // if the branch starts with an A_infty node, remove the first edge
    if (start_node->degree() == 1)
    {
      // move the start node
      dbsksp_xshock_edge_sptr first_edge = list_edge.front();
      start_node = first_edge->opposite(start_node);

      // delete from the branch
      vcl_vector<dbsksp_xshock_edge_sptr > temp = list_edge;
      
      list_edge.clear();
      for (unsigned i =1; i <temp.size(); ++i)
      {
        list_edge.push_back(temp[i]);
      }
    }

    // Sample the shock branch
    vcl_vector<dbsksp_xshock_node_descriptor > list_xsample;
    dbsksp_xgraph_algos::compute_xsamples(start_node, list_edge, sample_ds, list_xsample);

    if (list_xsample.empty())
    {
      continue;
    
    }

    // Fit the samples with the minimum number of fragments
    dbsksp_xshock_node_descriptor start_xdesc = list_xsample.front();
    dbsksp_xshock_node_descriptor end_xdesc = list_xsample.back();
    vcl_vector<dbsksp_xshock_node_descriptor > list_middle_xdesc;
    {
      //// Use equally-spaced samples
      //dbsksp_fit_one_shock_branch_with_power_of_2_intervals(start_xdesc, end_xdesc, 
      //list_xsample, this->distance_rms_error_threshold_, 
      //list_middle_xdesc);

      dbsksp_shock_path_sptr in_path = new dbsksp_shock_path(list_xsample);
      dbsksp_shock_path_sptr out_path = 0;

      dbsksp_fit_shock_path_using_min_num_pts(in_path, start_xdesc, end_xdesc, 
        this->distance_rms_error_threshold_,
        out_path);

      if (!out_path)
      {
        vcl_cout << "\nCould not fit a path.\n";
      }

      list_middle_xdesc.clear();
      for (unsigned i =1; (i+1) < out_path->num_points(); ++i)
      {
        list_middle_xdesc.push_back(*out_path->xdesc(i));
      }
    }
    

    // Modify the intermediate nodes one by one
    dbsksp_xshock_node_sptr last_node = end_node;
    while (!list_middle_xdesc.empty() && list_edge.size() > 1)
    {
      dbsksp_xshock_node_descriptor xdesc = list_middle_xdesc.back();
      list_middle_xdesc.pop_back();

      dbsksp_xshock_edge_sptr last_edge = list_edge.back();
      list_edge.pop_back();

      last_node = last_edge->opposite(last_node);
      dbsksp_xgraph_algos::update_degree2_node(last_node, last_edge, xdesc);
    }

    // we are done if the number of desired intervals in the same as the number of edges
    // Otherwise, there is work to do.

    // if number of middle xdesc (desired wanted number of middle nodes) is finished
    // before the number of edges, remove all the remaining nodes in the branch
    if (list_middle_xdesc.empty())
    {
      // Collect the nodes to remove
      vcl_vector<dbsksp_xshock_node_sptr > nodes_to_remove;
      while (list_edge.size() > 1) // 
      {
        last_node = list_edge.back()->opposite(last_node);
        list_edge.pop_back();

        nodes_to_remove.push_back(last_node);
      }

      while (!nodes_to_remove.empty())
      {
        dbsksp_xshock_edge_sptr xe = xgraph->remove_A12_node(nodes_to_remove.back());
        assert (xe);
        nodes_to_remove.pop_back();
      }
    }
    else // need to insert nodes in the remaining edge
    {
      assert(list_edge.size() == 1);
      dbsksp_xshock_edge_sptr xe = list_edge.back();

      // keep inserting until the list is cleared
      while (!list_middle_xdesc.empty())
      {
        dbsksp_xshock_node_descriptor xdesc = list_middle_xdesc.back();
        list_middle_xdesc.pop_back();

        dbsksp_xshock_edge_sptr xe1 = 0;
        dbsksp_xshock_edge_sptr xe2 = 0;
        xgraph->insert_xshock_node(xe, start_node, xdesc, xe1, xe2);
        // update xe to new "front" edge
        xe = xe1;
      } // while
    } // else
  }

  xgraph->update_all_degree_1_nodes();

  // record new graph
  if (xgraph)
  {
    new_xgraph = xgraph;
    for (unsigned i =0; i < list_branch_key.size(); ++i)
    {
      dbsksp_xshock_node_sptr start_node = list_branch_key[i].first;
      dbsksp_xshock_node_sptr end_node = list_branch_key[i].second;

      // the coarse graph's nodes are not changed during resampling so we can rely on their id
      // to establish correpondence
      dbsksp_xshock_node_sptr old_start_node = old_xgraph->node_from_id(start_node->id());
      dbsksp_xshock_node_sptr old_end_node = old_xgraph->node_from_id(end_node->id());

      map_new_node_to_old_node.insert(vcl_make_pair(start_node, old_start_node));
      map_new_node_to_old_node.insert(vcl_make_pair(end_node, old_end_node));
    }    
  }


  return;
}



//------------------------------------------------------------------------------
//: Convert the coarse shock graph
dbsksp_xshock_graph_sptr dbsksp_fit_xgraph::
convert_coarse_xgraph(const dbsk2d_shock_graph_sptr& graph,
                      type_map_sksp_edge_to_sk2d_edges& map_xe_to_orig_edges,
                      type_map_sksp_node_to_sk2d_node& map_sksp_node_to_sk2d_node)
{
  // Preliminary check
  if (!graph) return 0;

  // Place holder for new graph
  dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph();

  //>> Graph data structure
  
  //a) create the extrinsic nodes
  vcl_map<dbsk2d_shock_node_sptr, dbsksp_xshock_node_sptr > node_map;
  for (dbsk2d_shock_graph::vertex_iterator vit = graph->vertices_begin(); vit != 
    graph->vertices_end(); ++vit)
  {
    dbsk2d_shock_node_sptr v = *vit; 
    dbsksp_xshock_node_sptr xv = new dbsksp_xshock_node(xgraph->next_available_id());
    node_map.insert(vcl_make_pair(v, xv));
  }

  // create the extrinsic edges
  vcl_map<dbsk2d_shock_edge_sptr, dbsksp_xshock_edge_sptr > edge_map;
  for (dbsk2d_shock_graph::edge_iterator eit = graph->edges_begin(); eit != 
    graph->edges_end(); ++eit)
  {
    dbsk2d_shock_edge_sptr e = *eit;
    dbsksp_xshock_node_sptr xsource = node_map[e->source()];
    assert(xsource);
    
    dbsksp_xshock_node_sptr xtarget = node_map[e->target()];
    assert(xtarget);
    
    dbsksp_xshock_edge_sptr xe = new dbsksp_xshock_edge(xsource, xtarget, 
      xgraph->next_available_id());

    edge_map.insert(vcl_make_pair(e, xe));
  }
  
  // add the nodes and edges to the graph
  for (vcl_map<dbsk2d_shock_node_sptr, dbsksp_xshock_node_sptr >::iterator it = 
    node_map.begin(); it != node_map.end(); ++it)
  {
    xgraph->add_vertex(it->second);
  }

  for (vcl_map<dbsk2d_shock_edge_sptr, dbsksp_xshock_edge_sptr >::iterator it = 
    edge_map.begin(); it != edge_map.end(); ++it)
  {
    xgraph->add_edge(it->second);
  }

  // add node-edge connectivity and properties for the new graph
  for (vcl_map<dbsk2d_shock_node_sptr, dbsksp_xshock_node_sptr >::iterator it = 
    node_map.begin(); it != node_map.end(); ++it)
  {
    dbsk2d_shock_node_sptr v = it->first;
    dbsksp_xshock_node_sptr xv = it->second;
 
    // a) iterate thru the existing edges and copy their graph and geometric properties
    dbsk2d_shock_edge_sptr e_begin = graph->first_adj_edge(v);
    dbsk2d_shock_edge_sptr e = e_begin;
    vcl_vector<dbsksp_xshock_node_descriptor > list_xnode_descriptor;
    do
    {
      // add corresponding adjacency to xgraph
      dbsksp_xshock_edge_sptr xe = edge_map[e];
      dbsksp_xshock_node_descriptor* xdesc = xv->insert_shock_edge(xe, 0);
      dbsk2d_xshock_edge* xshock_e = static_cast<dbsk2d_xshock_edge* >(e.ptr());


      // copy node properties
      if (v == e->source()) // use first sample
      {
        dbsk2d_xshock_sample_sptr sample = xshock_e->first_sample();
        *xdesc = dbsksp_xshock_node_descriptor(sample->pt, sample->left_bnd_pt, sample->right_bnd_pt);
      }
      else // use last sample
      {
        dbsk2d_xshock_sample_sptr sample = xshock_e->last_sample();
        *xdesc = dbsksp_xshock_node_descriptor(sample->pt, sample->right_bnd_pt, sample->left_bnd_pt);
      }
      list_xnode_descriptor.push_back(*xdesc);

   
      // move to the next edge
      e = graph->cyclic_adj_succ(e, v);
    }
    while (e != e_begin);

    // Compute position and radius of the shock point by "averaging" the xnode descriptors
    double sum_x = 0;
    double sum_y = 0;
    double sum_r = 0;
    unsigned num_edges = list_xnode_descriptor.size();
    for (unsigned i =0; i < list_xnode_descriptor.size(); ++i)
    {
      sum_x += list_xnode_descriptor[i].pt().x();
      sum_y += list_xnode_descriptor[i].pt().y();
      sum_r += list_xnode_descriptor[i].radius();
    }
    vgl_point_2d<double > xv_pt(sum_x / num_edges, sum_y / num_edges);
    double xv_radius = sum_r / num_edges;

    // reset radius and shock point for all edges
    xv->set_pt(xv_pt);
    xv->set_radius(xv_radius);

    // b) Special treatment: Add terminal-edges to degree-1 node in dbsk2d_shock_graph
    // Unlike dbsk2d_shock_graph where each A_infty fragment corresponds an "A_infty" node,
    // in dbsksp_shock_graph each A_infty fragment corresponds to an edge, aka, a "terminal edge".
    // For this reason we need to add an extra edge to dbsksp_shock_graph
    // for each degree-1 node of dbsk2d

    if (xv->degree() == 1)
    {
      dbsksp_xshock_edge_sptr xe = *xv->edges_begin();
      dbsksp_xshock_node_descriptor* xe_descriptor = xv->descriptor(xe); 

      // i) add 1 new (degree-1) node and 1 new (terminal) edge to the shock graph
      dbsksp_xshock_node_sptr terminal_xv = new dbsksp_xshock_node(xgraph->next_available_id());
      dbsksp_xshock_edge_sptr terminal_xe = new dbsksp_xshock_edge(xv, terminal_xv, xgraph->next_available_id());
      terminal_xv->insert_shock_edge(terminal_xe, 0);

      // add the new terminal edge to the adjacency list of the node
      dbsksp_xshock_node_descriptor* terminal_xdesc = xv->insert_shock_edge(terminal_xe, xe);

      // add to the global edge list and node list
      xgraph->add_edge(terminal_xe);
      xgraph->add_vertex(terminal_xv);

      // ii) Copy geometric properties of the terminal edge
      *terminal_xdesc = xe_descriptor->opposite_xnode();
    }


    // b) Check for "gaps" in between the branches. Add an A_infty branch when there is a gap
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      // pointers to two currently consecutive edges
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_edge_sptr next_xe = xgraph->cyclic_adj_succ(cur_xe, xv);

      // retrieve the two edges' descriptors
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      dbsksp_xshock_node_descriptor* next_xdesc = xv->descriptor(next_xe);

      // assuming the edges are arranged in a counter-clockwise order, the gap is
      // between the left boundary point of current edge and right boundary point of next edge
      double angle_gap = signed_angle(-cur_xdesc->bnd_tangent_left(), next_xdesc->bnd_tangent_right());

      if (angle_gap > 0.01) // there is a gap! add A_infty branch
      {
        // i) add 1 new (degree-1) node and 1 new (terminal) edge to the shock graph
        dbsksp_xshock_node_sptr terminal_xv = new dbsksp_xshock_node(xgraph->next_available_id());
        dbsksp_xshock_edge_sptr terminal_xe = new dbsksp_xshock_edge(xv, terminal_xv, xgraph->next_available_id());
        terminal_xv->insert_shock_edge(terminal_xe, 0);

        // add the new terminal edge to the adjacency list of the node
        dbsksp_xshock_node_descriptor* terminal_xdesc = xv->insert_shock_edge(terminal_xe, next_xe);

        // add to the global edge list and node list
        xgraph->add_edge(terminal_xe);
        xgraph->add_vertex(terminal_xv);

        // ii) Copy geometric properties of the terminal edge
        terminal_xdesc->set_phi(angle_gap/2);
        terminal_xdesc->set_shock_tangent(rotated(cur_xdesc->shock_tangent(), cur_xdesc->phi() + angle_gap/2));
      }
      else if (angle_gap < -0.01) // big trouble !! // \todo take of this
      {
        vcl_cout << "\nERROR: overlap between samples at xnode_id = " << xv->id() << vcl_endl;
      }
    }
    xv->set_pt(xv_pt);
    xv->set_radius(xv_radius);

    // c) Clean up any discrepancy among the branches.
    
    // Check sum of phi's. If different from pi, distribute the difference
    double sum_phi = 0;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      sum_phi += cur_xdesc->phi();
    }
    // amount each branch should reduce
    double avg_diff = (sum_phi - vnl_math::pi) / xv->degree();

    // reduce avg_diff from each branch
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      cur_xdesc->set_phi(cur_xdesc->phi() - avg_diff);
    }

    // Now fix the shock tangent. (Arbitrarily) use the first branch as reference
    dbsksp_xshock_node::edge_iterator xedge_iter = xv->edges_begin();
    dbsksp_xshock_edge_sptr prev_xe = *xedge_iter;
    dbsksp_xshock_node_descriptor* prev_desc = xv->descriptor(prev_xe);

    // compute others' tangent according to phi's and the first tangent
    ++xedge_iter;
    for (; xedge_iter != xv->edges_end(); ++xedge_iter)
    {
      dbsksp_xshock_edge_sptr cur_xe = *xedge_iter;
      dbsksp_xshock_node_descriptor* cur_desc = xv->descriptor(cur_xe);
      cur_desc->set_shock_tangent(rotated(prev_desc->shock_tangent(), prev_desc->phi() + cur_desc->phi()));

      // update for next iteration
      prev_xe = cur_xe;
      prev_desc = cur_desc;
    }
  }

  xgraph->update_all_degree_1_nodes();

  // Build an inverse map from edges of new xgraph to the shock edge(s) of the original shock graph
  // For now, just inverse the original edgemap
  for (vcl_map<dbsk2d_shock_edge_sptr, dbsksp_xshock_edge_sptr >::iterator eit =
    edge_map.begin(); eit != edge_map.end(); ++eit)
  {
    dbsk2d_xshock_edge_sptr sk2d_xe = static_cast<dbsk2d_xshock_edge* >(eit->first.ptr());
    map_xe_to_orig_edges[eit->second].push_back(sk2d_xe);
  }

  for (vcl_map<dbsk2d_shock_node_sptr, dbsksp_xshock_node_sptr >::iterator vit =
    node_map.begin(); vit != node_map.end(); ++vit)
  {
    map_sksp_node_to_sk2d_node[vit->second] = vit->first;
  }

  return xgraph;
}



//------------------------------------------------------------------------------
//: Convert a (coarse) extrinsic dbsk2d shock graph to a dbsksp_shock_graph by
// using an Euler tour to explore the dbsks2d graph and construct new nodes and edges
// as they are discovered.
// If "keep_degree_2_nodes = false", all A12 nodes in the original dbsk2d shock graph will be ignored.
// Only terminal nodes and junctions will be retained.
// "map_xv_to_orig_node" maps each node in the new graph to a node in the original dbsk2d graph
// "map_xe_to_orig_edges" maps each edge in the new graph to a list of edges
// in the original dbsk2d graph that it is constructed from.
dbsksp_xshock_graph_sptr dbsksp_fit_xgraph::
convert_coarse_xgraph_using_euler_tour(const dbsk2d_shock_graph_sptr& graph,
                                       bool keep_degree_2_nodes,
                                       type_map_sksp_node_to_sk2d_node& map_xv_to_orig_node,
                                       type_map_sksp_edge_to_sk2d_edges& map_xe_to_orig_edges)
{
  map_xv_to_orig_node.clear();
  map_xe_to_orig_edges.clear();

  // Preliminary check
  if (!graph) return 0;

  // Place holder for new graph
  dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph();

  //1) Topology of the new graph
  
  //a) The vertex set
  // This is affected by the option "keep_degree_2_nodes"
  typedef vcl_map<dbsk2d_shock_node_sptr, dbsksp_xshock_node_sptr > type_map_v_to_xv;
  type_map_v_to_xv map_v_to_xv;

  for (dbsk2d_shock_graph::vertex_iterator vit = graph->vertices_begin(); vit !=
    graph->vertices_end(); ++vit)
  {
    dbsk2d_shock_node_sptr v = *vit;

    if (v->degree() == 2 && !keep_degree_2_nodes)
      continue;

    dbsksp_xshock_node_sptr xv = new dbsksp_xshock_node(xgraph->next_available_id());
    
    // save to graph
    xgraph->add_vertex(xv);

    // save the correspondence between old node and new node
    map_v_to_xv[v] = xv;
  }

  //b) The edge set
  // Use Euler tour to explore all connectivity between the retained nodes

  // anchor point marking the beginning of the tour
  dbsk2d_shock_edge_sptr start_e = *graph->edges_begin();
  dbsk2d_shock_node_sptr start_v = start_e->source();

  // running node and edge to explore the graph
  dbsk2d_shock_edge_sptr cur_e = start_e;
  dbsk2d_shock_node_sptr cur_v = start_v;

  // running node and edges for sksp shock graph
  dbsksp_xshock_node_sptr prev_xv = 0;
  vcl_vector<dbsk2d_xshock_edge_sptr > shock_links_of_cur_branch;

  // do the Euler tour
  do
  {
    // Start a new branch when we encounter a node in the coarse shock graph
    type_map_v_to_xv::iterator iter = map_v_to_xv.find(cur_v);

    if (iter != map_v_to_xv.end()) // hit a coarse-graph-node, start a new branch
    {
      dbsksp_xshock_node_sptr cur_xv = iter->second;

      // save info about previous branch
      if (prev_xv != 0)
      {
        // Add a new edge and the list of original shock links corresponding to it
        
        //a) First make sure this edge has not been created
        bool edge_exists = false;
        for (dbsksp_xshock_node::edge_iterator eit = cur_xv->edges_begin(); eit !=
          cur_xv->edges_end(); ++eit)
        {
          if ((*eit)->opposite(cur_xv) == prev_xv)
          {
            edge_exists = true;
            break;
          }
        }

        //b) Only add when the edge has not been created before
        if (!edge_exists)
        {
          // new edge
          dbsksp_xshock_edge_sptr xe = new dbsksp_xshock_edge(prev_xv, cur_xv, xgraph->next_available_id());

          // Attach new edge to the nodes
          // Setting ref_edge = 0 to put the new edge at the end of the node's
          // edge list. Since we're doing an Euler tour, this order will follow
          // the order in the original graph, up to rotation difference.
          prev_xv->insert_shock_edge(xe, 0);
          cur_xv->insert_shock_edge(xe, 0);

          // add to graph
          xgraph->add_edge(xe);
          
          // save the list of links corresponding to the new edge
          map_xe_to_orig_edges[xe] = shock_links_of_cur_branch;
        }
      }
      shock_links_of_cur_branch.clear();
      shock_links_of_cur_branch.push_back(static_cast<dbsk2d_xshock_edge* >(cur_e.ptr()));

      // update prev_xv before moving on
      prev_xv = cur_xv;
    }
    else
    {
      shock_links_of_cur_branch.push_back(static_cast<dbsk2d_xshock_edge* >(cur_e.ptr()));
    }

    // move on to the next edge and node in the Euler tour//////////////////////
    cur_v = cur_e->opposite(cur_v);
    cur_e = graph->cyclic_adj_succ(cur_e, cur_v);
  }
  // condition indicating we have return the original location
  while (cur_e != start_e || cur_v != start_v);

  // Create an inverse-mapping for the node set
  for (type_map_v_to_xv::iterator iter = map_v_to_xv.begin(); iter != 
    map_v_to_xv.end(); ++iter)
  {
    map_xv_to_orig_node[iter->second] = iter->first;
  }


  // Special treatment: Add terminal-edges to the end of degree-1 nodes
  // Unlike dbsk2d_shock_graph where each A_infty fragment corresponds an "A_infty" node,
  // in dbsksp_shock_graph each A_infty fragment corresponds to an edge, aka, a "terminal edge".
  // For this reason we need to add an extra edge to dbsksp_shock_graph
  // for each degree-1 node of dbsk2d
  for (type_map_v_to_xv::iterator iter = map_v_to_xv.begin(); iter != 
    map_v_to_xv.end(); ++iter)
  {
    dbsksp_xshock_node_sptr xv = iter->second;
    if (xv->degree() == 1)
    {
      dbsksp_xshock_edge_sptr xe = *xv->edges_begin();
      dbsksp_xshock_node_descriptor* xe_descriptor = xv->descriptor(xe); 

      // i) add 1 new (degree-1) node and 1 new (terminal) edge to the shock graph
      dbsksp_xshock_node_sptr terminal_xv = new dbsksp_xshock_node(xgraph->next_available_id());
      dbsksp_xshock_edge_sptr terminal_xe = new dbsksp_xshock_edge(xv, terminal_xv, xgraph->next_available_id());

      // add the new terminal edge to the adjacency list of its two end nodes
      terminal_xv->insert_shock_edge(terminal_xe, 0);
      dbsksp_xshock_node_descriptor* terminal_xdesc = xv->insert_shock_edge(terminal_xe, xe);

      // add to the global edge list and node list
      xgraph->add_edge(terminal_xe);
      xgraph->add_vertex(terminal_xv);
    }    
  }
  

  //2) Geometry
  // We're done with the topology. Now determine the geometric properties
  // The main source of info will come from the list of shock links corresponding
  // to each newly created edge

  // a) Get node descriptions from the original shock links
  for (type_map_sksp_edge_to_sk2d_edges::iterator iter = map_xe_to_orig_edges.begin();
    iter != map_xe_to_orig_edges.end(); ++iter)
  {
    dbsksp_xshock_edge_sptr xe = iter->first;
    vcl_vector<dbsk2d_xshock_edge_sptr >& list_shock_links = iter->second;

    assert(!list_shock_links.empty());

    //i) Geometric description at the source node
    {
      dbsksp_xshock_node_descriptor* source_xdesc = xe->source()->descriptor(xe);
      dbsk2d_xshock_edge_sptr source_e = list_shock_links.front();
      dbsk2d_shock_node_sptr source_v = map_xv_to_orig_node[xe->source()];

      // determine which sample of source_e to use for describing xe
      if (source_v == source_e->source()) // use first sample
      {
        dbsk2d_xshock_sample_sptr sample = source_e->first_sample();
        *source_xdesc = dbsksp_xshock_node_descriptor(sample->pt, 
          sample->left_bnd_pt, sample->right_bnd_pt);
      }
      else if (source_v == source_e->target()) // use last sample
      {
        dbsk2d_xshock_sample_sptr sample = source_e->last_sample();
        *source_xdesc = dbsksp_xshock_node_descriptor(sample->pt, 
          sample->right_bnd_pt, sample->left_bnd_pt);
      }
      else // something went terribly wrong. This should NEVER happen.
      {
        assert(false);
      }
    }

    //ii) Geometric description at the target node
    {
      dbsksp_xshock_node_descriptor* target_xdesc = xe->target()->descriptor(xe);
      dbsk2d_xshock_edge_sptr target_e = list_shock_links.back();
      dbsk2d_shock_node_sptr target_v = map_xv_to_orig_node[xe->target()];

      // determine which sample to use
      if (target_v == target_e->source()) // use first sample
      {
        dbsk2d_xshock_sample_sptr sample = target_e->first_sample();
        *target_xdesc = dbsksp_xshock_node_descriptor(sample->pt, 
          sample->left_bnd_pt, sample->right_bnd_pt);
      }
      else if (target_v == target_e->target()) // use last sample
      {
        dbsk2d_xshock_sample_sptr sample = target_e->last_sample();
        *target_xdesc = dbsksp_xshock_node_descriptor(sample->pt, 
          sample->right_bnd_pt, sample->left_bnd_pt);
      }
      else // something went terribly wrong. This should NEVER happen.
      {
        assert(false);
      }
    }
  }


  //b) Geometric description for the terminal edges (didn't exist in the original graph)

  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
    xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree() != 1)
      continue;

    // The ONLY edge incident at xv
    dbsksp_xshock_edge_sptr xe = *xv->edges_begin();

    // the other end of xe
    dbsksp_xshock_node_sptr xv1 = xe->opposite(xv);

    // Since all terminal edges were manually created by extending degree-1 nodes,
    // xv1 should have degree-2
    assert(xv1->degree() == 2);

    // Determine descriptor for xe using descriptor of its opposite edge
    dbsksp_xshock_node_descriptor* xe_xdesc = xv1->descriptor(xe); // has not been specified

    dbsksp_xshock_edge_sptr xe1 = xgraph->cyclic_adj_succ(xe, xv1);
    dbsksp_xshock_node_descriptor* xe1_xdesc = xv1->descriptor(xe1); // should have been specified

    // At degree-2 node, two descriptors are opposite of each other
    *xe_xdesc = xe1_xdesc->opposite_xnode();
  }

  // Compute position and radius of the shock point by "averaging" the xnode descriptors
  this->compute_node_radius_and_position_from_descriptors(xgraph);
  
  // c) Iron out the differences among the descriptors around a node
  // Check for "gaps" in between the branches. Add an A_infty branch when there is a gap
  this->make_consistent_node_tangent_and_phi(xgraph, 0.01);


  // \TODO d) Correct "ILLEGAL" fragments where boundary form swallowtails
  //this->fix_illegal_edge_fragments


  return xgraph;
}









//------------------------------------------------------------------------------
//: Determine radius and position of a graph's shock nodes from their descriptors
  // Use averaging if there are differences among the descriptors
void dbsksp_fit_xgraph::
compute_node_radius_and_position_from_descriptors(const dbsksp_xshock_graph_sptr& xgraph)
{
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); 
    vit != xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;
    
    // skip degree-1 node
    if (xv->degree() == 1)
      continue;

    // find average of x, y, and r
    double sum_x = 0;
    double sum_y = 0;
    double sum_r = 0;
    unsigned num_edges = 0;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != 
      xv->edges_end(); ++eit)
    {
      ++num_edges;
      dbsksp_xshock_node_descriptor* xdesc = xv->descriptor(*eit);
      sum_x += xdesc->pt().x();
      sum_y += xdesc->pt().y();
      sum_r += xdesc->radius();
    }
    vgl_point_2d<double > xv_pt(sum_x / num_edges, sum_y / num_edges);
    double xv_radius = sum_r / num_edges;

    // reset radius and shock point for all edges
    xv->set_pt(xv_pt);
    xv->set_radius(xv_radius);
  }

  // Take care of terminal nodes
  xgraph->update_all_degree_1_nodes();
  return;
}









//------------------------------------------------------------------------------
//: Iron out the differences in shock tangent and phi angles between the 
// descriptors around the shock nodes in an xgraph
// Add an A_infty branch when there gaps are too big.
void dbsksp_fit_xgraph::
make_consistent_node_tangent_and_phi(const dbsksp_xshock_graph_sptr& xgraph,
                                     double min_angle_gap_to_add_Ainfty_branch)
{
  // Collect the list of nodes in the graph
  vcl_vector<dbsksp_xshock_node_sptr > list_xnode;
  list_xnode.reserve(xgraph->number_of_vertices());
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin();
    vit != xgraph->vertices_end(); ++vit)
  {
    if ((*vit)->degree() != 1)
      list_xnode.push_back(*vit);
  }


  //a) Check for incosistency around each node. Add A_infty when needed.
  for (unsigned i =0; i < list_xnode.size(); ++i)
  {
    dbsksp_xshock_node_sptr xv = list_xnode[i];
    if (xv->degree() == 1)
      continue;

    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      // pointers to two currently consecutive edges
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_edge_sptr next_xe = xgraph->cyclic_adj_succ(cur_xe, xv);

      // retrieve the two edges' descriptors
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      dbsksp_xshock_node_descriptor* next_xdesc = xv->descriptor(next_xe);

      // assuming the edges are arranged in a counter-clockwise order, the gap is
      // between the left boundary point of current edge and right boundary point of next edge
      double angle_gap = signed_angle(-cur_xdesc->bnd_tangent_left(), next_xdesc->bnd_tangent_right());

      if (angle_gap >= min_angle_gap_to_add_Ainfty_branch) // there is a gap! add A_infty branch
      {
        // i) add 1 new (degree-1) node and 1 new (terminal) edge to the shock graph
        dbsksp_xshock_node_sptr terminal_xv = new dbsksp_xshock_node(xgraph->next_available_id());
        dbsksp_xshock_edge_sptr terminal_xe = new dbsksp_xshock_edge(xv, terminal_xv, xgraph->next_available_id());
        terminal_xv->insert_shock_edge(terminal_xe, 0);

        // add the new terminal edge to the adjacency list of the node
        dbsksp_xshock_node_descriptor* terminal_xdesc = xv->insert_shock_edge(terminal_xe, next_xe);

        // add to the global edge list and node list
        xgraph->add_edge(terminal_xe);
        xgraph->add_vertex(terminal_xv);

        // ii) Copy geometric properties of the terminal edge
        terminal_xdesc->set_phi(angle_gap/2);
        terminal_xdesc->set_shock_tangent(rotated(cur_xdesc->shock_tangent(), cur_xdesc->phi() + angle_gap/2));
        terminal_xdesc->set_radius(xv->radius());
        terminal_xdesc->set_pt(xv->pt());
      }
      else if (angle_gap <= -min_angle_gap_to_add_Ainfty_branch) // big trouble !! // \todo take of this
      {
        vcl_cout << "\nERROR: overlap between samples at xnode_id = " << xv->id() << vcl_endl;
      }
    }
  }



  //b) Clean up any remaining (small) discrepancy in angle among the branches.
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
    xgraph->vertices_end(); ++vit)
  { 
    dbsksp_xshock_node_sptr xv = *vit;
    if (xv->degree() == 1)
      continue;

    // Check sum of phi's. If different from pi, distribute the difference
    double sum_phi = 0;
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      sum_phi += cur_xdesc->phi();
    }
    // amount each branch should reduce
    double avg_diff = (sum_phi - vnl_math::pi) / xv->degree();

    // reduce avg_diff from each branch
    for (dbsksp_xshock_node::edge_iterator eit = xv->edges_begin(); eit != xv->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr cur_xe = *eit;
      dbsksp_xshock_node_descriptor* cur_xdesc = xv->descriptor(cur_xe);
      cur_xdesc->set_phi(cur_xdesc->phi() - avg_diff);
    }

    // Now fix the shock tangent. (Arbitrarily) use the first branch as reference
    dbsksp_xshock_node::edge_iterator xedge_iter = xv->edges_begin();
    dbsksp_xshock_edge_sptr prev_xe = *xedge_iter;
    dbsksp_xshock_node_descriptor* prev_desc = xv->descriptor(prev_xe);

    // compute others' tangent according to phi's and the first tangent
    ++xedge_iter;
    for (; xedge_iter != xv->edges_end(); ++xedge_iter)
    {
      dbsksp_xshock_edge_sptr cur_xe = *xedge_iter;
      dbsksp_xshock_node_descriptor* cur_desc = xv->descriptor(cur_xe);
      cur_desc->set_shock_tangent(rotated(prev_desc->shock_tangent(), prev_desc->phi() + cur_desc->phi()));

      // update for next iteration
      prev_xe = cur_xe;
      prev_desc = cur_desc;
    }
  }

  xgraph->update_all_degree_1_nodes();
  return;
}




//------------------------------------------------------------------------------
//: Comparison function for keys of shock branches
// Two keys are the same if the two end nodes are the same, regardless of their order
bool dbsksp_fit_xgraph::compare_unordered_node_pair::
operator ()(dbsksp_fit_xgraph::type_branch_key key1, dbsksp_fit_xgraph::type_branch_key key2) const
{
  // remove the "order" by making each key arranged in increasing order of ids
  unsigned key1_lower_id = vnl_math_min(key1.first->id(), key1.second->id());
  unsigned key1_upper_id = vnl_math_max(key1.first->id(), key1.second->id());

  unsigned key2_lower_id = vnl_math_min(key2.first->id(), key2.second->id());
  unsigned key2_upper_id = vnl_math_max(key2.first->id(), key2.second->id());

  vcl_pair<unsigned, unsigned > pair1(key1_lower_id, key1_upper_id);
  vcl_pair<unsigned, unsigned > pair2(key2_lower_id, key2_upper_id);
  return pair1 < pair2;
}



//------------------------------------------------------------------------------
//: Compute the list of shock branches in a undirected shock graph
// Each branch key is a pair of (start-end) nodes.
void dbsksp_fit_xgraph::
compute_shock_branches(const dbsksp_xshock_graph_sptr& xgraph,
                       vcl_vector<type_branch_key >& list_branch_key,
                       vcl_vector<type_branch_edges >& list_branch_edges)
{
  list_branch_key.clear();
  list_branch_edges.clear();

  // Preliminary check
  if (!xgraph) return;


  //a) The vertex set of the coarse graph
  vcl_set<dbsksp_xshock_node_sptr > list_coarse_node;

  // Iterate thru the vertices, only leaf (degree-1) and junction nodes (degree-3)
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin(); vit !=
    xgraph->vertices_end(); ++vit)
  {
    dbsksp_xshock_node_sptr xv = *vit;

    if (xv->degree() == 2)
      continue;

    list_coarse_node.insert(xv);
  }

  //b) The branches of the shock graph
  // Use Euler tour to explore all connectivity between the retained nodes

  // anchor point marking the beginning of the tour
  dbsksp_xshock_edge_sptr start_xe = *xgraph->edges_begin();
  dbsksp_xshock_node_sptr start_xv = start_xe->source();

  // running node and edge to explore the graph
  dbsksp_xshock_edge_sptr cur_xe = start_xe;
  dbsksp_xshock_node_sptr cur_xv = start_xv;

  // running node and edges for sksp shock graph
  type_branch_edges cur_branch_edges;
  dbsksp_xshock_node_sptr cur_branch_start_xv = 0;

  // use a customized comparator to prevent duplication of shock branches
  typedef vcl_map<type_branch_key, type_branch_edges, dbsksp_fit_xgraph::compare_unordered_node_pair> type_map_branch_key_to_edges;
  type_map_branch_key_to_edges map_branch_key_to_edges;

  // do the Euler tour
  do
  {
    // Start a new branch when we encounter a node in the coarse shock graph
    vcl_set<dbsksp_xshock_node_sptr >::iterator iter = list_coarse_node.find(cur_xv);
    if (iter != list_coarse_node.end()) // hit a coarse-graph-node, start a new branch
    {
      // save info about previous branch
      if (cur_branch_start_xv != 0)
      {
        // save the list of links corresponding to the new edge
        type_branch_key key(cur_branch_start_xv, cur_xv);
        map_branch_key_to_edges.insert(vcl_make_pair(key, cur_branch_edges));
      }

      // edge list for the new branch
      cur_branch_edges.clear();
      cur_branch_edges.push_back(cur_xe);

      // record the starting node of the new branch
      cur_branch_start_xv = cur_xv;
    }
    else
    {
      cur_branch_edges.push_back(cur_xe);
    }

    // move on to the next edge and node in the Euler tour//////////////////////
    cur_xv = cur_xe->opposite(cur_xv);
    cur_xe = xgraph->cyclic_adj_succ(cur_xe, cur_xv);
  }
  // condition indicating we have return the original location
  while (cur_xe != start_xe || cur_xv != start_xv);


  // Output the list of branches
  for (type_map_branch_key_to_edges::iterator iter = map_branch_key_to_edges.begin();
    iter != map_branch_key_to_edges.end(); ++iter)
  {
    list_branch_key.push_back(iter->first);
    list_branch_edges.push_back(iter->second);  
  }
    
  return;
}



