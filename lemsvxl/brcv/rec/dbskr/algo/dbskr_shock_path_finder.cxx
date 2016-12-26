// This is brcv/rec/dbskr/algo/dbskr_shock_path_finder.cxx

#include <dbskr/algo/dbskr_shock_path_finder.h>
#include <dbskr/algo/dbskr_rec_algs.h>
#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_compute_scurve.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <vil/vil_image_resource.h>

#include <vcl_algorithm.h>


void dbskr_shock_path_finder::clear() {
  sg_ = 0;
  
  for (path_tree_map_type::iterator it = path_tree_map_.begin(); it != path_tree_map_.end(); it++) {
    vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >* vec = it->second;
    for (unsigned d = 0; d < vec->size(); d++) {
      vcl_vector<dbskr_path_tree_node_sptr>* depth_vec = (*vec)[d];
      depth_vec->clear();
      delete depth_vec;
    }
    vec->clear();
    delete vec;
  }
  path_tree_map_.clear();
}

bool dbskr_shock_path_finder::construct_v() {
  if (!sg_ || !sg_->number_of_vertices())
    return false;

  v_ = construct_v_graph(sg_, *(sg_->vertices_begin()));

  if (!v_)
    return false;

  return true;
}

vcl_vector<dbskr_path_tree_node_sptr>* dbskr_shock_path_finder::initialize_depth_zero(dbskr_v_node_sptr v1) 
{
  dbskr_path_tree_node_sptr root = new dbskr_path_tree_node(v1, 0, 0.0f);  // root has no parent
  vcl_vector<dbskr_path_tree_node_sptr>* zero_vec = new vcl_vector<dbskr_path_tree_node_sptr>();
  for (dbskr_v_graph::edge_iterator e_itr = v1->in_edges_begin(); e_itr != v1->in_edges_end(); e_itr++) {
    dbskr_v_node_sptr opp = (*e_itr)->opposite(v1);
    dbskr_path_tree_node_sptr node = new dbskr_path_tree_node(opp, root, (*e_itr)->length()); 
    zero_vec->push_back(node);
    vcl_pair<int, int> p(v1->id_, opp->id_);
    path_tree_multimap_.insert(vcl_pair<vcl_pair<int, int>, dbskr_path_tree_node_sptr>(p,node));
  }   
  return zero_vec;
}

//: advance one more depth
vcl_vector<dbskr_path_tree_node_sptr>* dbskr_shock_path_finder::advance(vcl_vector<dbskr_path_tree_node_sptr>* last_vec, dbskr_v_node_sptr v1, float norm, float threshold)
{
  vcl_vector<dbskr_path_tree_node_sptr>* new_vec = new vcl_vector<dbskr_path_tree_node_sptr>();
  for (unsigned i = 0; i < last_vec->size(); i++) {
    dbskr_path_tree_node_sptr parent = (*last_vec)[i];

    if (parent->node_->id_ == v1->id_)
      continue;

    for (dbskr_v_graph::edge_iterator e_itr = parent->node_->in_edges_begin(); e_itr != parent->node_->in_edges_end(); e_itr++) {
      dbskr_v_node_sptr opp = (*e_itr)->opposite(parent->node_);
      if (opp->id_ == parent->parent_->node_->id_)
        continue;  // skip the branch that we came from at this point in tree

      float length = (*e_itr)->length();

      //: do not create a new dbskr_tree_node if there exists this path tree node 
      vcl_pair<int, int> p_search(v1->id_, opp->id_);
      vcl_pair<vcl_multimap<vcl_pair<int, int>, dbskr_path_tree_node_sptr>::iterator, vcl_multimap<vcl_pair<int, int>, dbskr_path_tree_node_sptr>::iterator > itr_pair;  

      bool advance = true;
      vcl_multimap<vcl_pair<int, int>, dbskr_path_tree_node_sptr>::iterator itr;    
      itr_pair = path_tree_multimap_.equal_range(p_search);
      for (itr = itr_pair.first; itr != itr_pair.second; itr++) {
        if (itr->second->parent_->node_->id_ == parent->node_->id_) {
          //: make sure the path all the way to the root is the same
          dbskr_path_tree_node_sptr tn1 = itr->second->parent_;
          dbskr_path_tree_node_sptr tn2 = parent;
          advance = false;
          while (!advance && tn1->parent_ && tn2->parent_) {
            if (tn1->parent_->node_->id_ != tn2->parent_->node_->id_)
              advance = true;
            tn1 = tn1->parent_;
            tn2 = tn2->parent_;
          }
          
          if (!advance)
            break;
        }
      }
      if (!advance)
        continue;

      // if the same step exists (in either way) on its own path, do not advance
      dbskr_path_tree_node_sptr tn = parent;
      advance = true;
      while (tn->parent_) { 
        if ((tn->node_->id_ == opp->id_ && tn->parent_->node_->id_ == parent->node_->id_) ||
            (tn->node_->id_ == parent->node_->id_ && tn->parent_->node_->id_ == opp->id_)
            )
        {
          advance = false;
          break;
        }
        tn = tn->parent_;
      }   

      if (!advance)
        continue;

      dbskr_path_tree_node_sptr node = new dbskr_path_tree_node(opp, parent, parent->length_ + length); 
      new_vec->push_back(node);
      vcl_pair<int, int> p(v1->id_, opp->id_);
      path_tree_multimap_.insert(vcl_pair<vcl_pair<int, int>, dbskr_path_tree_node_sptr>(p,node));
    }
  }    
  return new_vec;
}

bool dbskr_shock_path_finder::get_shortest_v_node_path(dbsk2d_shock_node_sptr n1, 
                              dbsk2d_shock_node_sptr n2, 
                              vcl_vector<dbskr_v_node_sptr>& path)
{
  if (!v_ || !construct_v())
      return false;

  //: locate n1 and n2 on v graph
  dbskr_v_node_sptr& v1 = get_node_sptr(v_, n1->id());
  dbskr_v_node_sptr& v2 = get_node_sptr(v_, n2->id());
  
  if (!v1 || !v2)  // now we know v2 exists in v_graph 
    return false;

  if (v1 == v2) {
    path.push_back(v1);
    return true;
  }

  path_tree_map_type::iterator it = path_tree_map_.find(v1->id_);
  vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >* main_vec;
  bool hit_v2 = false;
  int ind1, ind2;
  if (it == path_tree_map_.end()) { // create for the first time
    //: create depth 0 vector

    vcl_vector<dbskr_path_tree_node_sptr>* zero_vec = initialize_depth_zero(v1);

    for (unsigned i = 0; i < zero_vec->size(); i++) {
      if (v2->id_ == (*zero_vec)[i]->node_->id_) {
        hit_v2 = true;
        ind1 = 0;
        ind2 = i;
      }
    }
     
    main_vec = new vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >(1, zero_vec);
    path_tree_map_[v1->id_] = main_vec;

    //: proceed till we hit v2, for the first time at any of the depths
    while (!hit_v2) {
      //: pick the last depth row and advance all its elements one more level
      vcl_vector<dbskr_path_tree_node_sptr>* last_vec = ((*main_vec)[main_vec->size()-1]);
      vcl_vector<dbskr_path_tree_node_sptr>* new_vec = advance(last_vec, v1, 1.0f, 10000.0f);  // we don't impose any max here

      for (unsigned i = 0; i < new_vec->size(); i++) {
        if (v2->id_ == (*new_vec)[i]->node_->id_) {
          hit_v2 = true;
          ind1 = main_vec->size();
          ind2 = i;
        }
      }
      
      if (new_vec->size() > 0)
        main_vec->push_back(new_vec);
    }
  } else {  // use the existing tree for this node
    // search for the existing depths if v2 is already found before
    main_vec = it->second;
    for (unsigned d = 0; d < main_vec->size(); d++) {
      vcl_vector<dbskr_path_tree_node_sptr>* depth_vec = (*main_vec)[d];
      for (unsigned i = 0; i < depth_vec->size(); i++) {
        if ((*depth_vec)[i]->node_->id_ == v2->id_) {
          hit_v2 = true;
          ind1 = d;
          ind2 = i;
          break;
        }
      }
      if (hit_v2)
        break;
    }

    while (!hit_v2) {  // keep searching if still not hit
      //: pick the last depth row and advance all its elements one more level
      vcl_vector<dbskr_path_tree_node_sptr>* last_vec = ((*main_vec)[main_vec->size()-1]);

      vcl_vector<dbskr_path_tree_node_sptr>* new_vec = advance(last_vec, v1, 1.0f, 10000.0f);  // we don't impose any max here

      for (unsigned i = 0; i < new_vec->size(); i++) {
        if (v2->id_ == (*new_vec)[i]->node_->id_) {
          hit_v2 = true;
          ind1 = main_vec->size();
          ind2 = i;
        }
      }
      if (new_vec->size() > 0)
        main_vec->push_back(new_vec);
    }
  
  }

  float length;
  return get_path(main_vec, ind1, ind2, path, length, n1->id(), n2->id());
}

//: return all the paths whose normalized length is within length_thres
//  restrict the search with this length constraint
//  required because number of paths explode in complicated shock graphs
bool dbskr_shock_path_finder::get_all_v_node_paths(dbsk2d_shock_node_sptr n1, 
                              dbsk2d_shock_node_sptr n2, 
                              vcl_vector<vcl_vector<dbskr_v_node_sptr> >& paths, vcl_vector<float>& abs_lengths, float normalization_length, float length_thres)
{
  if (!v_ || !construct_v())
      return false;

  //: locate n1 and n2 on v graph
  dbskr_v_node_sptr& v1 = get_node_sptr(v_, n1->id());
  dbskr_v_node_sptr& v2 = get_node_sptr(v_, n2->id());
  
  if (!v1 || !v2)  // now we know v2 exists in v_graph 
    return false;

  if (v1 == v2) {
    vcl_vector<dbskr_v_node_sptr> path;
    path.push_back(v1);
    paths.push_back(path);
    return true;
  }

  path_tree_map_type::iterator it = path_tree_map_.find(v1->id_);
  vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >* main_vec;
  bool advanced = true;
  vcl_vector<vcl_pair<int, int> > indices;

  if (it == path_tree_map_.end()) { // create for the first time
    //: create depth 0 vector

    vcl_vector<dbskr_path_tree_node_sptr>* zero_vec = initialize_depth_zero(v1);

    for (unsigned i = 0; i < zero_vec->size(); i++) {
      if (v2->id_ == (*zero_vec)[i]->node_->id_) {
        if ((*zero_vec)[i]->length_/normalization_length < length_thres) {
          vcl_pair<int, int> p(0, i);
          indices.push_back(p);
        }
      }
    }
     
    main_vec = new vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >(1, zero_vec);
    path_tree_map_[v1->id_] = main_vec;

    //: proceed till we advance all branches as long as possible
    while (advanced) {
      advanced = false;

      //: pick the last depth row and advance all its elements one more level
      vcl_vector<dbskr_path_tree_node_sptr>* last_vec = ((*main_vec)[main_vec->size()-1]);

      //: advance only if there exists at least one in the last_vec less than the length threshold
      for (vcl_vector<dbskr_path_tree_node_sptr>::iterator it = last_vec->begin(); it != last_vec->end(); it++) {
        if ((*it)->length_/normalization_length < length_thres) {
          advanced = true;
          break;
        }
      }

      if (advanced) {
        advanced = false;
      } else
        break;    // none of the ones in the last_vec are shorter than the length_thres
      
      vcl_vector<dbskr_path_tree_node_sptr>* new_vec = advance(last_vec, v1, normalization_length, length_thres);

      for (unsigned i = 0; i < new_vec->size(); i++) {
        if (v2->id_ == (*new_vec)[i]->node_->id_) {
          if ((*new_vec)[i]->length_/normalization_length < length_thres) {
            vcl_pair<int, int> p(main_vec->size(), i);
            indices.push_back(p);
          }
        }
      }
      
      if (new_vec->size() > 0) {
        advanced = true;
        main_vec->push_back(new_vec);
      }
    }
  } else {  // use the existing tree for this node
    // search for the existing depths if v2 is already found before
    main_vec = it->second;
    for (unsigned d = 0; d < main_vec->size(); d++) {
      vcl_vector<dbskr_path_tree_node_sptr>* depth_vec = (*main_vec)[d];

      //: don't check the next depth if all the members at this depth were longer than thres
      bool keep_going = false;
      for (unsigned i = 0; i < depth_vec->size(); i++) {

        if ((*depth_vec)[i]->length_/normalization_length < length_thres) {
          keep_going = true;
          if ((*depth_vec)[i]->node_->id_ == v2->id_) {
            vcl_pair<int, int> p(d, i);
            indices.push_back(p);
          }
        }
      }

      if (!keep_going)  // none satisfying depth condition at the last depth
        break;
    }

    while (advanced) {  // keep searching if still possible to advance
      advanced = false;

      //: pick the last depth row and advance all its elements one more level
      vcl_vector<dbskr_path_tree_node_sptr>* last_vec = ((*main_vec)[main_vec->size()-1]);

       //: advance only if there exists at least one in the last_vec less than the length threshold
      for (vcl_vector<dbskr_path_tree_node_sptr>::iterator it = last_vec->begin(); it != last_vec->end(); it++) {
        if ((*it)->length_/normalization_length < length_thres) {
          advanced = true;
          break;
        }
      }

      if (advanced) {
        advanced = false;
      } else
        break;    // none of the ones in the last_vec are shorter than the length_thres

      vcl_vector<dbskr_path_tree_node_sptr>* new_vec = advance(last_vec, v1, normalization_length, length_thres);

      for (unsigned i = 0; i < new_vec->size(); i++) {
        if (v2->id_ == (*new_vec)[i]->node_->id_) {
          vcl_pair<int, int> p(main_vec->size(), i);
          indices.push_back(p);
        }
      }
      if (new_vec->size() > 0) {
        advanced = true;
        main_vec->push_back(new_vec);
      }
    }
  
  }

  for (unsigned i = 0; i < indices.size(); i++) {
    vcl_vector<dbskr_v_node_sptr> path;
    float length;
    if (get_path(main_vec, indices[i].first, indices[i].second, path, length, n1->id(), n2->id())) {

      //: same path might be encountered with different routes, prune same paths
      bool found_it = false;
      for (unsigned j = 0; j < paths.size(); j++) {
        if (path.size() != paths[j].size())
          continue;
        bool not_equal = false;
        for (unsigned ii = 0; ii < path.size(); ii++) {
          if (path[ii]->id_ != paths[j][ii]->id_) {
            not_equal = true;
            break;
          }
        }
        if (!not_equal) {
          found_it = true; 
          break;
        }
      }

      if (!found_it) {
        paths.push_back(path);
        abs_lengths.push_back(length);
      }

    }
  }
  return true;
}

void add_a_dummy_degree_one(dbskr_v_graph_sptr& new_g, int id) {

  bool found_it = false;
  for (dbskr_v_graph::edge_iterator eit = new_g->edges_begin(); eit != new_g->edges_end(); eit++) {
    if ((*eit)->target()->degree() == 1) {
      found_it = true;
      break;
    }
  }

  if (!found_it) { // add a dummy edge with a degree 1 target

    dbskr_v_graph::vertex_iterator v1_itr;
    for (v1_itr = new_g->vertices_begin(); v1_itr != new_g->vertices_end(); v1_itr++)
      if ((*v1_itr)->id_ == id)
        break;

    dbskr_v_node_sptr n = new dbskr_v_node(-1);

    dbskr_v_node_sptr n1 = (*v1_itr);
    dbskr_v_edge_sptr ne = new dbskr_v_edge(n1, n);
    new_g->add_edge(ne);
    new_g->add_vertex(n);
    n->add_incoming_edge(ne);
    n1->add_incoming_edge(ne);
  }
  
}

//: takes in a new_g with vertices and edges but no edges to edge lists of vertices
//  the order of edges in node's edge list is important to generate accurate euler tours of the graphs
void add_incoming_edges_in_correct_order_of_original_graph(dbskr_v_graph_sptr original_graph, dbskr_v_graph_sptr& new_g) 
{ 
  //: map from source to target ids in the original graph to the source and target pointers on the new graph
  vcl_map<vcl_pair<int, int> , vcl_pair<int, dbskr_v_edge_sptr> > nodes_to_edges;
  for (dbskr_v_graph::edge_iterator e_it = new_g->edges_begin(); e_it != new_g->edges_end(); e_it++) {
    vcl_pair<int, int> p((*e_it)->source()->id_, (*e_it)->target()->id_);
    vcl_pair<int, dbskr_v_edge_sptr> p2((*e_it)->source()->id_, *e_it);
    nodes_to_edges[p] = p2;
    p.first = (*e_it)->target()->id_;
    p.second = (*e_it)->source()->id_;
    p2.first = (*e_it)->target()->id_;
    nodes_to_edges[p] = p2;
  }

  //: now go through vertices of original graph and add the edges in correct orders to the new_g
  for (dbskr_v_graph::vertex_iterator vv = original_graph->vertices_begin(); vv != original_graph->vertices_end(); vv++) {
    for (dbskr_v_node::edge_iterator e_vv = (*vv)->in_edges_begin(); e_vv != (*vv)->in_edges_end(); e_vv++) {
      vcl_pair<int, int> p1((*vv)->id_, ((*e_vv)->opposite(*vv))->id_);
      vcl_map<vcl_pair<int, int> , vcl_pair<int, dbskr_v_edge_sptr> >::iterator vv_it = 
        nodes_to_edges.find(p1);
      if (vv_it != nodes_to_edges.end()) {
        //: add only if this is exactly the same edge
        if ((*e_vv)->edges_.size() == (vv_it->second.second)->edges_.size() && (*e_vv)->edges_.size() > 0 && (*e_vv)->edges_[0]->id() == (vv_it->second.second)->edges_[0]->id()) {
          dbskr_v_node_sptr& n = get_node_sptr(new_g, vv_it->second.first);      
          n->add_incoming_edge((vv_it->second).second);
        }
      }
    }
  }

  //: correct the source and target pointers of the edges
  for (dbskr_v_graph::edge_iterator e_it = new_g->edges_begin(); e_it != new_g->edges_end(); e_it++) {
    dbskr_v_node_sptr& sn = get_node_sptr(new_g, (*e_it)->source()->id_);
    dbskr_v_node_sptr& tn = get_node_sptr(new_g, (*e_it)->target()->id_);
    (*e_it)->set_source(sn);
    (*e_it)->set_target(tn);
  }

}

//: only creates the nodes and edges and adds them to the graph, but does not populate the edge list of individual vertices
void merge(dbskr_v_graph_sptr new_g, dbskr_v_graph_sptr n2_v2_graph, int id)
{
  dbskr_v_graph::vertex_iterator v1_itr;
  for (v1_itr = new_g->vertices_begin(); v1_itr != new_g->vertices_end(); v1_itr++)
    if ((*v1_itr)->id_ == id)
      break;

  vcl_map<int, dbskr_v_node_sptr> v2_map;
  dbskr_v_graph::vertex_iterator v2_itr;
  for (v2_itr = n2_v2_graph->vertices_begin(); v2_itr != n2_v2_graph->vertices_end(); v2_itr++) {
    if ((*v2_itr)->id_ == id)
      v2_map[id] = (*v1_itr);
    else {
      dbskr_v_node_sptr new_node = new dbskr_v_node(*(*v2_itr));
      new_g->add_vertex(new_node);
      v2_map[new_node->id_] = new_node;
    }
  }

  dbskr_v_graph::edge_iterator e_itr;
  for (e_itr = n2_v2_graph->edges_begin(); e_itr != n2_v2_graph->edges_end(); e_itr++) {
    dbskr_v_edge_sptr e = (*e_itr);
    dbskr_v_node_sptr source = v2_map[e->source()->id_];
    dbskr_v_node_sptr target = v2_map[e->target()->id_];
    dbskr_v_edge_sptr e_new = new dbskr_v_edge(source, target, e->edges_, e->start_node_id_, e->end_node_id_);
    //source->add_incoming_edge(e_new);
    //target->add_incoming_edge(e_new);
    new_g->add_edge(e_new);
  }
}

bool dbskr_shock_path_finder::get_all_scurves(dbsk2d_shock_node_sptr n1, 
                       dbsk2d_shock_node_sptr n2, 
                       vcl_vector<dbskr_scurve_sptr>& scurves, 
                       vcl_vector<float>& abs_lengths, float norm_length, float length_threshold, float interpolate_ds, float sample_ds)
{
  vcl_vector<vcl_vector<dbskr_v_node_sptr> > all_paths;
  vcl_vector<float> abs_lengths_temp;
  get_all_v_node_paths(n1, n2, all_paths, abs_lengths_temp, norm_length, length_threshold);
  abs_lengths.clear();

  for (unsigned i = 0; i < all_paths.size(); i++) {
    vcl_vector<dbsk2d_shock_edge_sptr> edges;
    if (get_edges_on_path(all_paths[i], edges)) {
      dbskr_scurve_sptr s = dbskr_compute_scurve(n1, edges, false, true, true, vcl_min((float)sample_ds, interpolate_ds), sample_ds);
      scurves.push_back(s);
      abs_lengths.push_back(abs_lengths_temp[i]);
    }
  }

  return true;
}


//: the input v graphs and all the paths between n1 and n2 induce various v graphs 
//  as the portion of the shape that connects n1 and n2 which may or may not be the root of v1 and v2
//  find all such v graphs using the paths and various unions of paths
bool dbskr_shock_path_finder::get_all_v_graphs(dbsk2d_shock_node_sptr n1, 
                              dbsk2d_shock_node_sptr n2, dbskr_v_graph_sptr v1, dbskr_v_graph_sptr v2,
                              vcl_vector<dbskr_v_graph_sptr>& graphs, vcl_vector<float>& abs_lengths, float norm_length, float length_threshold)
{
  vcl_vector<vcl_vector<dbskr_v_node_sptr> > all_paths;
  vcl_vector<float> abs_lengths_temp;
  get_all_v_node_paths(n1, n2, all_paths, abs_lengths_temp, norm_length, length_threshold);
  abs_lengths.clear();

 /* for (unsigned i = 0; i < all_paths.size(); i++) {
    vcl_cout << "path " << i << ": ";
    for (unsigned j = 0; j < all_paths[i].size(); j++) {
      vcl_cout << all_paths[i][j]->id_ << " ";
    }
    vcl_cout << vcl_endl;
  }*/
  
  vcl_vector<dbskr_v_node_sptr> v1_verts, v2_verts;
  
  v1->get_all_degree_one_vertices(v1_verts);
  //vcl_cout << "v1 degree one nodes: ";
  //for (unsigned i = 0; i < v1_verts.size(); i++) 
  //  vcl_cout << v1_verts[i]->id_ << " ";
  //vcl_cout << vcl_endl;

  v2->get_all_degree_one_vertices(v2_verts);
  //vcl_cout << "v2 degree one nodes: ";
  //for (unsigned i = 0; i < v2_verts.size(); i++) 
  //  vcl_cout << v2_verts[i]->id_ << " ";
  //vcl_cout << vcl_endl;

  vcl_vector<bool> v1_verts_valid(v1_verts.size(), false);
  vcl_vector<bool> v2_verts_valid(v2_verts.size(), false);
  

  //: mark the v1 degree 1 nodes which are on a path from n1 to n2
  //  and the initial part of the path from n1 to v1 should consist of v1 nodes and edges
  vcl_multimap<int, vcl_pair<unsigned, unsigned> > v1_vert_id_to_path_ind;
  for (unsigned ii = 0; ii < v1_verts.size(); ii++) {
    for (unsigned i = 0; i < all_paths.size(); i++) {

      for (unsigned j = 0; j < all_paths[i].size(); j++) {
        if (all_paths[i][j]->id_ == v1_verts[ii]->id_) {

          //: check if all the nodes from the beginning consists of v1 nodes
          bool skip = false;
          for (unsigned kkk = 1; kkk <= j; kkk++) {
            dbskr_v_node_sptr look_for = all_paths[i][kkk];  // if cannot find look_for anywhere in v1, skip this path
            dbskr_v_node_sptr prev = all_paths[i][kkk-1];  // if cannot find the edge that connects look_for to prev then skip this path
            bool found_it = false;
            for (dbskr_v_graph::vertex_iterator v_itr = v1->vertices_begin(); v_itr != v1->vertices_end(); v_itr++) {
              if ((*v_itr)->id_ == look_for->id_) {
                //: check if any of its edges are connected to prev
                for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
                  if ((*e_itr)->opposite((*v_itr))->id_ == prev->id_) {
                    found_it = true;
                    break;
                  }
                }
                if (found_it) // found both the node and the edge to the prev as one of its edges, so this path stays
                  break;
              }
            }

            if (!found_it)
              skip = true;

            if (skip)
              break;
          }
          if (skip)
            break;
          //: all the nodes from the beginning consists of v1 nodes and edges so add this to multimap
          vcl_pair<unsigned, unsigned> p(i, j);
          v1_vert_id_to_path_ind.insert(vcl_pair<int, vcl_pair<unsigned, unsigned> >(v1_verts[ii]->id_, p));
          v1_verts_valid[ii] = true;
          break;
        }
      }
    }
  }
     


  //: mark the v2 degree 1 nodes which are on a path from n1 to n2
  //  and the final part of the path from v2 node to n2 should consist of v2 nodes and edges
  vcl_multimap<int, vcl_pair<unsigned, unsigned> > v2_vert_id_to_path_ind;
  for (unsigned ii = 0; ii < v2_verts.size(); ii++) {
    for (unsigned i = 0; i < all_paths.size(); i++) {

      for (unsigned j = 0; j < all_paths[i].size(); j++) {
        if (all_paths[i][j]->id_ == v2_verts[ii]->id_) {

          //: check if all the nodes in the final portion consists of v2 nodes
          bool skip = false;
          for (unsigned kkk = j; kkk < all_paths[i].size()-1; kkk++) {
            dbskr_v_node_sptr look_for = all_paths[i][kkk];  // if cannot find look_for anywhere in v2, skip this path
            dbskr_v_node_sptr next = all_paths[i][kkk+1];  // if cannot find the edge that connects look_for to next then skip this path
            bool found_it = false;
            for (dbskr_v_graph::vertex_iterator v_itr = v2->vertices_begin(); v_itr != v2->vertices_end(); v_itr++) {
              if ((*v_itr)->id_ == look_for->id_) {
                //: check if any of its edges are connected to next
                for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
                  if ((*e_itr)->opposite((*v_itr))->id_ == next->id_) {
                    found_it = true;
                    break;
                  }
                }
                if (found_it) // found both the node and the edge to the next as one of its edges, so this path stays for now
                  break;
              }
            }

            if (!found_it)
              skip = true;

            if (skip)
              break;
          }
          if (skip)
            break;
          //: all the nodes in the final portion consists of v2 nodes and edges so add this to multimap
          vcl_pair<unsigned, unsigned> p(i, j);
          v2_vert_id_to_path_ind.insert(vcl_pair<int, vcl_pair<unsigned, unsigned> >(v2_verts[ii]->id_, p));
          v2_verts_valid[ii] = true;
          break;
        }
      }
    }
  }

  vcl_vector<dbskr_v_graph_sptr> graphs_temp;

  //: find n1_v1 and n2_v2's
  vcl_vector<dbskr_v_graph_sptr> n1_v1_graphs(v1_verts.size(), 0);
  vcl_vector<dbskr_v_graph_sptr> n2_v2_graphs(v2_verts.size(), 0);

  for (unsigned ii = 0; ii < v1_verts.size(); ii++) {
    if (!v1_verts_valid[ii]) 
      continue;
    

    dbskr_v_node_sptr v1_node = v1_verts[ii];

    // find portion between n1 and v1_vert as a v graph
    // use the edges on the paths as edge proposals and add them
    dbskr_v_graph_sptr n1_v1_graph = new dbskr_v_graph();
    vcl_pair<vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator, 
             vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator > it_p = v1_vert_id_to_path_ind.equal_range(v1_node->id_);
    vcl_map<vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, bool> proposals;
    for (vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator it = it_p.first; it != it_p.second; it++) {
      for (unsigned kkk = 1; kkk <= it->second.second; kkk++) {
        vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr> p(all_paths[it->second.first][kkk-1], all_paths[it->second.first][kkk]); 
        proposals[p] = true;
      }   
    }
   
    for (vcl_map<vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, bool>::iterator it = proposals.begin(); it != proposals.end(); it++) {
      dbskr_v_node_sptr from = it->first.first;
      dbskr_v_node_sptr to = it->first.second;
      //: find the edge between them which is in v1
      dbskr_v_edge_sptr edge = 0;
      for (dbskr_v_node::edge_iterator e_itr = from->in_edges_begin(); e_itr != from->in_edges_end(); e_itr++) 
        if ((*e_itr)->opposite(from)->id_ == to->id_) 
          edge = *e_itr;
      
      if (!edge)
        continue;  // pass this pair
      dbskr_v_node_sptr from_new = new dbskr_v_node(*from);
      dbskr_v_node_sptr to_new = new dbskr_v_node(*to);
      dbskr_v_edge_sptr edge_new;
      if (from_new->id_ == edge->start_node_id_)
        edge_new = new dbskr_v_edge(from_new, to_new, edge->edges_, edge->start_node_id_, edge->end_node_id_);
      else if (from_new->id_ == edge->end_node_id_)
        edge_new = new dbskr_v_edge(to_new, from_new, edge->edges_, edge->start_node_id_, edge->end_node_id_);
      else 
        continue;  // no edge between these guys

      dbskr_v_node_sptr from_new_found = from_new;
      dbskr_v_node_sptr to_new_found = to_new;
      //: add if not already added
      for (dbskr_v_graph::vertex_iterator v_itr = n1_v1_graph->vertices_begin(); v_itr != n1_v1_graph->vertices_end(); v_itr++) {
        if (from_new && from_new->id_ == (*v_itr)->id_) {
          from_new = 0;
          from_new_found = (*v_itr);
          for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
            if ((*e_itr)->opposite((*v_itr))->id_ == to_new_found->id_) {
              edge_new = 0;
              to_new = 0;
              break;
            }
          }
          if (!edge_new)
            break;

        } else if (to_new && to_new->id_ == (*v_itr)->id_) {
          to_new = 0;
          to_new_found = (*v_itr);
          for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
            if ((*e_itr)->opposite((*v_itr))->id_ == from_new_found->id_) {
              edge_new = 0;
              from_new = 0;
              break;
            }
          }
          if (!edge_new)
            break; 
        }
      }
      if (from_new)
        n1_v1_graph->add_vertex(from_new);
      if (to_new)
        n1_v1_graph->add_vertex(to_new);
      
      if (edge_new) {
        n1_v1_graph->add_edge(edge_new);
        //from_new_found->add_incoming_edge(edge_new);
        //to_new_found->add_incoming_edge(edge_new);
      }
    }
      
    //vcl_cout << "n1_v1_graph: " << vcl_endl;
    //print_v_graph(n1_v1_graph);
    //vcl_cout << "-------\n";
    n1_v1_graphs[ii] = n1_v1_graph;

    if (v1_verts[ii]->id_ == n2->id()) {// this portion of v1 graph is already a solution
      graphs_temp.push_back(n1_v1_graph);
      vsol_box_2d_sptr b = n1_v1_graph->bounding_box();
      abs_lengths.push_back((float)(b->height() + b->width()));
      v1_verts_valid[ii] = false;
    }

  }

  for (unsigned jj = 0; jj < v2_verts.size(); jj++) {
    if (!v2_verts_valid[jj])
      continue;

    dbskr_v_node_sptr v2_node = v2_verts[jj];

    // find portion between n1 and v1_vert as a v graph
    // use the edges on the paths as edge proposals and add them only if they are already edges in v1 
    dbskr_v_graph_sptr n2_v2_graph = new dbskr_v_graph();
    vcl_pair<vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator, 
             vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator > it2_p = v2_vert_id_to_path_ind.equal_range(v2_node->id_);
    vcl_map<vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, bool> proposals;
    for (vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator it = it2_p.first; it != it2_p.second; it++) {
      for (unsigned kkk = it->second.second; kkk < all_paths[it->second.first].size()-1; kkk++) {
        vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr> p(all_paths[it->second.first][kkk], all_paths[it->second.first][kkk+1]);
        proposals[p] = true;
      }
    }

    //: add the nodes and edges in the proposals which are actually in v2
    for (vcl_map<vcl_pair<dbskr_v_node_sptr, dbskr_v_node_sptr>, bool>::iterator it = proposals.begin(); it != proposals.end(); it++) {
      dbskr_v_node_sptr from = it->first.first;
      dbskr_v_node_sptr to = it->first.second;
    
      //: find the edge between them which is in v1
      dbskr_v_edge_sptr edge = 0;
      for (dbskr_v_node::edge_iterator e_itr = from->in_edges_begin(); e_itr != from->in_edges_end(); e_itr++) 
        if ((*e_itr)->opposite(from)->id_ == to->id_) 
          edge = *e_itr;
    
      if (!edge)
        continue;  // pass this pair

      dbskr_v_node_sptr from_new = new dbskr_v_node(*from);
      dbskr_v_node_sptr to_new = new dbskr_v_node(*to);
      dbskr_v_edge_sptr edge_new;
      if (from_new->id_ == edge->start_node_id_)
        edge_new = new dbskr_v_edge(from_new, to_new, edge->edges_, edge->start_node_id_, edge->end_node_id_);
      else if (from_new->id_ == edge->end_node_id_)
        edge_new = new dbskr_v_edge(to_new, from_new, edge->edges_, edge->start_node_id_, edge->end_node_id_);
      else 
        continue;  // no edge between these guys

      dbskr_v_node_sptr from_new_found = from_new;
      dbskr_v_node_sptr to_new_found = to_new;
      //: add if not already added
      for (dbskr_v_graph::vertex_iterator v_itr = n2_v2_graph->vertices_begin(); v_itr != n2_v2_graph->vertices_end(); v_itr++) {
        if (from_new && from_new->id_ == (*v_itr)->id_) {
          from_new = 0;
          from_new_found = (*v_itr);
          for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
            if ((*e_itr)->opposite((*v_itr))->id_ == to_new_found->id_) {
              edge_new = 0;
              to_new = 0;
              break;
            }
          }
          if (!edge_new)
            break;

        } else if (to_new && to_new->id_ == (*v_itr)->id_) {
          to_new = 0;
          to_new_found = (*v_itr);
          for (dbskr_v_node::edge_iterator e_itr = (*v_itr)->in_edges_begin(); e_itr != (*v_itr)->in_edges_end(); e_itr++) {
            if ((*e_itr)->opposite((*v_itr))->id_ == from_new_found->id_) {
              edge_new = 0;
              from_new = 0;
              break;
            }
          }
          if (!edge_new)
            break; 
        }
      }
      if (from_new)
        n2_v2_graph->add_vertex(from_new);
      if (to_new)
        n2_v2_graph->add_vertex(to_new);
      
      if (edge_new) {
        n2_v2_graph->add_edge(edge_new);
        //from_new_found->add_incoming_edge(edge_new);
        //to_new_found->add_incoming_edge(edge_new);
      }
    }

    //vcl_cout << "n2_v2_graph: " << vcl_endl;
    //print_v_graph(n2_v2_graph);
    //vcl_cout << "-------\n";
    n2_v2_graphs[jj] = n2_v2_graph;

    if (v2_verts[jj]->id_ == n1->id()) {// this portion of v2 graph is already a solution
      graphs_temp.push_back(n2_v2_graph);
      //: calculate an estimate path length for n2_v2_graph and push to abs_lengths, estimate could be the diagonal of its bbox
      vsol_box_2d_sptr b = n2_v2_graph->bounding_box();
      abs_lengths.push_back((float)(b->height() + b->width()));
      v2_verts_valid[jj] = false;
    }
  }

  //: pick one v1 node and one v2 node and find all the paths in between, and create a dbskr_v_graph for each unique path
  for (unsigned ii = 0; ii < v1_verts.size(); ii++) {
    if (!v1_verts_valid[ii])
      continue;

    dbskr_v_node_sptr v1_node = v1_verts[ii];
    dbskr_v_graph_sptr n1_v1_graph = n1_v1_graphs[ii];

    vcl_pair<vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator, 
             vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator > it_p = v1_vert_id_to_path_ind.equal_range(v1_node->id_);
      
    //: pick a v2 node
    for (unsigned jj = 0; jj < v2_verts.size(); jj++) {
      if (!v2_verts_valid[jj])
        continue;

      dbskr_v_node_sptr v2_node = v2_verts[jj];
      dbskr_v_graph_sptr n2_v2_graph = n2_v2_graphs[jj];

      //: find all the common paths between the two
      vcl_vector<vcl_pair<int, vcl_pair<unsigned, unsigned> > > common_paths;

      vcl_pair<vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator, 
               vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator > it2_p = v2_vert_id_to_path_ind.equal_range(v2_node->id_);

      for (vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator it = it_p.first; it != it_p.second; it++) {
          for (vcl_multimap<int, vcl_pair<unsigned, unsigned> >::iterator it2 = it2_p.first; it2 != it2_p.second; it2++) {
            if (it->second.first == it2->second.first) { 
              vcl_pair<unsigned, unsigned> pp(it->second.second, it2->second.second);
              if (pp.second > pp.first) {
                vcl_pair<int, vcl_pair<unsigned, unsigned> > p(it->second.first, pp);
                common_paths.push_back(p);
              }
            }
          }
      }

      //: for each unique common_path create a dbskr_v_graph
      vcl_vector<bool> common_paths_use(common_paths.size(), true);
      for (unsigned p = 0; p < common_paths.size(); p++) {
        if (!common_paths_use[p])
          continue;

        // invalidate all the paths which are exactly the same as this one between v1_node and v2_node
        for (unsigned kk = p+1; kk < common_paths.size(); kk++) {

          //: on both paths make sure the size of the portion from v1_node to v2_node is the same
          if ((common_paths[p].second.second - common_paths[p].second.first) != (common_paths[kk].second.second - common_paths[kk].second.first))
            continue;

          bool break_it = false;
          unsigned kkk2 = common_paths[p].second.first;
          for (unsigned kkk = common_paths[kk].second.first; 
            kkk <= common_paths[kk].second.second; kkk++, kkk2++) {
            if (all_paths[common_paths[kk].first][kkk]->id_ != all_paths[common_paths[p].first][kkk2]->id_) {
              break_it = true;
              break;
            }
          }
          if (!break_it)
            common_paths_use[kk] = false;
        }

        //: create the dbskr_v_graph
        dbskr_v_graph_sptr new_g = new dbskr_v_graph();
        unsigned kkk = common_paths[p].second.first;

        dbskr_v_node_sptr prev_node =(all_paths[common_paths[p].first][kkk]);
        dbskr_v_node_sptr prev_node_new = new dbskr_v_node(*prev_node);
        new_g->add_vertex(prev_node_new);
        kkk++;
        dbskr_v_node_sptr current_node, current_node_new;
        for ( ; kkk <= common_paths[p].second.second; kkk++) {
          current_node = all_paths[common_paths[p].first][kkk];
          current_node_new = new dbskr_v_node(*current_node);
          new_g->add_vertex(current_node_new);
          //: find the edge between these two in the original graph
          for (dbskr_v_node::edge_iterator e_itr = current_node->in_edges_begin(); e_itr != current_node->in_edges_end(); e_itr++) {
            if ((*e_itr)->opposite(current_node) == prev_node) {
              dbskr_v_edge_sptr edge;
              if (current_node_new->id_ == (*e_itr)->start_node_id_)
                edge = new dbskr_v_edge(current_node_new, prev_node_new, (*e_itr)->edges_, (*e_itr)->start_node_id_, (*e_itr)->end_node_id_);
              else 
                edge = new dbskr_v_edge(prev_node_new, current_node_new, (*e_itr)->edges_, (*e_itr)->start_node_id_, (*e_itr)->end_node_id_);
              new_g->add_edge(edge);
              //current_node_new->add_incoming_edge(edge);
              //prev_node_new->add_incoming_edge(edge);
              break;
            }
          }
          prev_node = current_node;
          prev_node_new = current_node_new;
        }           

        merge(new_g, n1_v1_graph, v1_node->id_);
        merge(new_g, n2_v2_graph, v2_node->id_);

        //vcl_cout << "new_g: " << vcl_endl;
        //print_v_graph(new_g);
        //vcl_cout << "-----\n";
        graphs_temp.push_back(new_g);
        abs_lengths.push_back(abs_lengths_temp[common_paths[p].first]);
      }
   
    }
  }
 
  vcl_vector<float> abs_lengths_temp2(abs_lengths);
  abs_lengths.clear();

  graphs.clear();
  //: purge the graphs which are the same
  for (unsigned i = 0; i < graphs_temp.size(); i++) {
    if (!graphs_temp[i])
      continue;
    for (unsigned j = i+1; j < graphs_temp.size(); j++) {
      if (!graphs_temp[j])
        continue;

      if (graphs_temp[i]->same_vertex_and_edges(*graphs_temp[j]))
        graphs_temp[j] = 0;
    }
  }
  for (unsigned i = 0; i < graphs_temp.size(); i++) {
    if (!graphs_temp[i] || !graphs_temp[i]->number_of_vertices() || !graphs_temp[i]->number_of_edges())
      continue;
    graphs.push_back(graphs_temp[i]);
    abs_lengths.push_back(abs_lengths_temp2[i]);
  }

  //: correct the graphs and return 
  for (unsigned i = 0; i < graphs.size(); i++) {
    dbskr_v_graph_sptr new_g = graphs[i];

    //: now populate the edge lists of each vertex in the new graph according to the original v graph of the whole shape
    add_incoming_edges_in_correct_order_of_original_graph(v_, new_g);

    //: shock patch extractor finds the euler tour of these graphs and it looks for an edge degree 1 target to start
    //  the euler tour, these graphs may not have such nodes so create a dummy one after merge
    add_a_dummy_degree_one(new_g, n1->id());
    //vcl_cout << "new_g: " << vcl_endl;
    //print_v_graph(new_g);
    //vcl_cout << "-----\n";
  }

  return true;
}

//: do not generate a new path if n2 or n1 repeats in the path, which means there was a loop around n2 node
bool dbskr_shock_path_finder::get_path(vcl_vector<vcl_vector<dbskr_path_tree_node_sptr>* >* vec, 
                                       int ind1, int ind2, vcl_vector<dbskr_v_node_sptr>& path, float& length, int n1_id, int n2_id)
{
  // return the path
  vcl_vector<dbskr_v_node_sptr> temp;
  dbskr_path_tree_node_sptr tn = (*((*vec)[ind1]))[ind2];
  temp.push_back(tn->node_);
  length = tn->length_;
  
  while (tn->parent_->parent_) {
    tn = tn->parent_;
    if (tn->node_->id_ == n2_id) 
      return false;

    if (tn->node_->id_ == n1_id) 
      return false;
    
    temp.push_back(tn->node_);
  }

  tn = tn->parent_;
  temp.push_back(tn->node_);
  
  //: reverse temp and return 
  path.clear();
  path.insert(path.begin(), temp.rbegin(), temp.rend());
  return true;
}

bool dbskr_shock_path_finder::get_edges_on_path(vcl_vector<dbskr_v_node_sptr>& path, 
                         vcl_vector<dbsk2d_shock_edge_sptr>& edges)
{
  for (unsigned i = 1; i < path.size(); i++) {
    dbskr_v_node_sptr prev_v = path[i-1];
    dbskr_v_node_sptr v = path[i];

    for (dbskr_v_graph::edge_iterator e_itr = prev_v->in_edges_begin(); e_itr != prev_v->in_edges_end(); e_itr++) {
          dbskr_v_node_sptr opp = (*e_itr)->opposite(prev_v);
          if (opp != v)
            continue;  

          if (!(*e_itr)->edges_.size())
            return false;

          //: push them in the correct order, so check if first edge is adjacent to original shock node of prev_v
          dbsk2d_shock_edge_sptr first_edge = (*e_itr)->edges_[0];
          bool found_it = false;
          for (dbsk2d_shock_graph::edge_iterator se_itr = prev_v->original_shock_node_->in_edges_begin(); se_itr != prev_v->original_shock_node_->in_edges_end(); se_itr++) {
            if (first_edge == (*se_itr)) {
              found_it = true;
              break;
            }
          }
          if (!found_it) {
            for (dbsk2d_shock_graph::edge_iterator se_itr = prev_v->original_shock_node_->out_edges_begin(); se_itr != prev_v->original_shock_node_->out_edges_end(); se_itr++) {
              if (first_edge == (*se_itr)) {
                found_it = true;
                break;
              }
            }
          }

          if (found_it) {
            for (unsigned j = 0; j < (*e_itr)->edges_.size(); j++)
              edges.push_back((*e_itr)->edges_[j]);
          } else {
            for (int j = (*e_itr)->edges_.size()-1; j >=0; j--)
              edges.push_back((*e_itr)->edges_[j]);
          }

          break;
    }
  }
  return true;
}


