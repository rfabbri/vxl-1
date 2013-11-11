#include "dbdet_sel_base.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_algorithm.h>


//***********************************************************************//
// The DHT algorithm :: 
//          Hypothesis trees and Hypothesis graph based algorithm
// 
//***********************************************************************//

//: seperate the link graph into two separate ones based on linking direction
void dbdet_sel_base::separate_link_graphs_and_cvlet_maps()
{
  //Note: this is a connected components type algorithm
  //
  //  Basically start from a cvlet and give all the links on it a direction label
  //  Then, recurse over all the cvlets contained in these links that haven't been marked with the same label

  // 1) first form the link map from the link graph
  for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++)
  {
    dbdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
    for (;l_it!=edge_link_graph_.cLinks[i].end(); l_it++){
      (*l_it)->flag = false;
      link_map.insert(vcl_pair<vcl_pair<int, int>, dbdet_link*>(vcl_pair<int, int>((*l_it)->pe->id, (*l_it)->ce->id), (*l_it)));
    }
  }

  // 2) next form the link pair map
  vcl_map<vcl_pair<int, int>, dbdet_link*>::iterator ml_it = link_map.begin();
  for (; ml_it != link_map.end(); ml_it++)
  {
    dbdet_link* pair = link_map[vcl_pair<int, int>(ml_it->first.second, ml_it->first.first)];
    assert(pair!=0);
    link_pairs.insert(vcl_pair<dbdet_link*, dbdet_link*>(ml_it->second, pair));
  }

  // 3) now start the connected components algorithm from the first double unmarked link
  ml_it = link_map.begin();
  for (; ml_it != link_map.end(); ml_it++)
  {
    //this link has not yet been visited, start the CC algorithm from here
    if (!ml_it->second->flag && !link_pairs[ml_it->second]->flag)
      BFS_CC_links(ml_it->second); //BFS traversal
      //DFS_CC_links((ml_it->second); //DFS traversal
  }

  // 4) move all the curvelets in the cv_set1 to the second cvlet map
  vcl_set<dbdet_curvelet*>::iterator cvit = cv_set1.begin();
  for (; cvit != cv_set1.end(); cvit++){
    //remove it from the main cvlet map
    curvelet_map_.remove_curvelet(*cvit);

    //add it to the second cvlet map
    curvelet_map_.add_curvelet(*cvit, false);
  }

  // 5) Move all the links that were marked to the second link graph
  ml_it = link_map.begin();
  for (; ml_it != link_map.end(); ml_it++)
  {
    if (ml_it->second->flag)
      edge_link_graph_.move_link(ml_it->second);
  }

  // 6) examine the link graph to make sure that all links are singular
  for (unsigned i=0; i<edge_link_graph_.cLinks.size(); i++)
  {
    dbdet_link_list_iter l_it = edge_link_graph_.cLinks[i].begin();
    for (; l_it!=edge_link_graph_.cLinks[i].end(); l_it++)
      assert(!link_bidirectional(*l_it));
  }

  // 7) construct the second link graph from the links that were removed 

  // 8) some house cleaning
  //clear the cv_set
  cv_set1.clear();

  //clear the link map
  link_map.clear();

}

//: Recursive DFS search over the links to label the links and curvelets in a connected component scheme
void dbdet_sel_base::DFS_CC_links(dbdet_link* cur_link)
{
  // has this link been visited?
  if (cur_link->flag || link_pairs[cur_link]->flag)
    return; //if already visited ignore

  //mark this link as visited
  cur_link->flag = true;

  //visit each of the curvelets pointed to by this link
  cvlet_list_iter cvit = cur_link->curvelets.begin();
  for (; cvit != cur_link->curvelets.end(); cvit++)
  {
    dbdet_curvelet* cvlet = (*cvit);

    //add this curvelet to the cv_set1
    cv_set1.insert(cvlet);

    //visit every link in this curvelet
    for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++)
    {
      dbdet_link* new_link = link_map[vcl_pair<int, int>(cvlet->edgel_chain[i]->id, cvlet->edgel_chain[i+1]->id)];

      // has this link been visited?
      if (!new_link->flag && !link_pairs[new_link]->flag)
        DFS_CC_links(new_link); //if not already visited, visit it now
    }
  }
}

//: BFS search over the links to label the links and curvelets in a connected component scheme
void dbdet_sel_base::BFS_CC_links(dbdet_link* link)
{
  //initiaize the queue with the link
  BFS_links_queue.push(link);

  while (!BFS_links_queue.empty())
  {
    //take the first link from the queue
    dbdet_link* cur_link = BFS_links_queue.front();

    //if these links haven't been visited yet
    if (!cur_link->flag && !link_pairs[cur_link]->flag)
    {
      //mark the link as visited
      cur_link->flag = true;

      //add the curvelets from this link to the cv_set1
      cvlet_list_iter cvit = cur_link->curvelets.begin();
      for (; cvit != cur_link->curvelets.end(); cvit++)
      {
        dbdet_curvelet* cvlet = (*cvit);
        cv_set1.insert(cvlet);

        //add all the links from these curvelets to the queue
        for (unsigned i=0; i<cvlet->edgel_chain.size()-1; i++){
          dbdet_link* new_link = link_map[vcl_pair<int, int>(cvlet->edgel_chain[i]->id, cvlet->edgel_chain[i+1]->id)];

          // has this link been visited?
          if (!new_link->flag && !link_pairs[new_link]->flag)
            BFS_links_queue.push(new_link); //if not already visited, visit it now
        }
      }
    }

    BFS_links_queue.pop(); //remove this link from the queue
  }
}

//: clear the HTG and related data structures
void dbdet_sel_base::clear_HTG()
{
  //reset the HTG
  HTG.clear();

  //clear all the edge labels
  ELs.clear_labels();
  ELs.resize(edgemap_->num_edgels());

  //empty the queue too
  while (!BFS_queue_global.empty())
    BFS_queue_global.pop();

}

//: initialize the hypothesis tree graph (basically mark the root nodes of each tree)
void dbdet_sel_base::initialize_HTG()
{
  vcl_cout << "Initializing the hypothesis trees ... ";

  // 1) preprocess the link graph (if not already done)

  // 2) find unambiguous 1-chains from the link graph to initialize the hypothesis trees
  //edge_linker->extract_regular_contours_from_the_link_graph();

  //first clear the existing HTG
  HTG.clear();
  ELs.clear_labels();
  while (!BFS_queue_global.empty())
    BFS_queue_global.pop();

  // 3) Initialize HT graphs from these 1-chains (root nodes have only one curvelet (in each direction))
  //go over all the 1-chains from the curve fragment graph
  dbdet_edgel_chain_list_iter cit = curve_frag_graph_.frags.begin();
  for (; cit != curve_frag_graph_.frags.end(); cit++)
  {
    if ((*cit)->edgels.size()<8) //arbitrary threshold for now
      continue;

    //of the remaining find the center edgel and make sure that it has a valid curvelet 
    unsigned ind = (*cit)->edgels.size()/2;

    cvlet_list& cvlist = curvelet_map_.curvelets((*cit)->edgels[ind]->id);
    cvlet_list& Rcvlist = curvelet_map_.Rcurvelets((*cit)->edgels[ind]->id);

    //there needs to be two curvelets in opposite directions for this to be a valid node
    if (!(cvlist.size()==1 && Rcvlist.size()==1)) //there is exactly one curvelet in each direction
      continue;

    dbdet_curvelet* cvlet1 = cvlist.front();
    dbdet_curvelet* cvlet2 = Rcvlist.front();
    //also ensure that there are no bifurcations from the first curvelets
    if (cvlet_has_bifurcations(cvlet1, true) || cvlet_has_bifurcations(cvlet2, false))
      continue;

    //all clear (i.e., seed is legal)

    ////compute bundles that are C1 between the two HT cvlets going in either directions
    //dbdet_curve_model* constrained_cb = C1_continuity_possible(cvlet1, cvlet2);
    //if (!constrained_cb)
    //  continue;

    //dbdet_curve_model* new_cb1 = constrained_cb->intersect(cvlet1->curve_model);
    //dbdet_curve_model* new_cb2 = constrained_cb->intersect(cvlet2->curve_model);

    //new_cb1->compute_best_fit();
    //new_cb2->compute_best_fit();

    //delete constrained_cb; //no more use for this

    dbdet_curve_model* new_cb1 = cvlet1->curve_model;
    dbdet_curve_model* new_cb2 = cvlet2->curve_model;

    //instantiate a HT node from cvlet1 (mark this as a forward tracing branch)
    dbdet_hyp_tree* HT1 = new dbdet_hyp_tree (  new dbdet_hyp_tree_node (  new dbdet_curvelet(*cvlet1, new_cb1), 
                                                                           HTG.nodes.size(), 
                                                                           true
                                                                         ), 
                                                HTG.nodes.size()
                                              );            
    HTG.insert_node(HT1);              //save it as a node in the HTG
    label_edgels(HT1->root);           //claim ownership of this cvlet by adding labels to its edgels
    BFS_queue_global.push(HT1->root);  //also add it to the global BFS queue

    //instantiate a HT node cvlet2 (mark this as a backward tracing branch)
    dbdet_hyp_tree* HT2 = new dbdet_hyp_tree (  new dbdet_hyp_tree_node (  new dbdet_curvelet(*cvlet2, new_cb2), 
                                                                           HTG.nodes.size(), 
                                                                           false
                                                                         ), 
                                                HTG.nodes.size()
                                              );
    HTG.insert_node(HT2);              //save it as a node in the HTG
    label_edgels(HT2->root);           //claim ownership of this cvlet by adding labels to its edgels
    BFS_queue_global.push(HT2->root);  //also add it to the global BFS queue     
  }

  //create the right size of container to hold all the links
  HTG.resize_links();

  vcl_cout << "done!" << vcl_endl;
}

//: check to see if here are any bifurcations within the path of this curvelet
bool dbdet_sel_base::cvlet_has_bifurcations(dbdet_curvelet* cvlet, bool dir)
{
  for (unsigned i=0; i<cvlet->edgel_chain.size(); i++){
    if (dir && edge_link_graph_.cLinks[cvlet->edgel_chain[i]->id].size()>1)
      return true;
    else if (!dir && edge_link_graph_.cLinks2[cvlet->edgel_chain[i]->id].size()>1)
      return true;
  }

  return false;
}

//: propagate all the hypothesis trees
void dbdet_sel_base::propagate_all_HTs()
{
  vcl_cout << "Propagating all HTs ...";

  //next propagate all the trees simultaneously using the same BFS queue 
  while (!BFS_queue_global.empty())
    propagate_HT_from_the_next_leaf_node();
  vcl_cout << "done!" <<vcl_endl;

}

void dbdet_sel_base::propagate_HTs_N_steps(int N)
{
  vcl_cout << "Propagating " << N << " steps...";

  //next propagate all the trees simultaneously using the same BFS queue 
  while (!BFS_queue_global.empty() && N>0){
    propagate_HT_from_the_next_leaf_node();
    N--;
  }
  vcl_cout << "done!";

  if (BFS_queue_global.empty())
    vcl_cout << " ---- No more branches to propagate!" << vcl_endl;
  else
    vcl_cout << vcl_endl;
}

//: attempt to grow the HTs from the leaf nodes
void dbdet_sel_base::propagate_HT_from_the_next_leaf_node()
{
  //propagate the node at the beginning of the queue
  dbdet_hyp_tree_node* cur_node = BFS_queue_global.front();
  BFS_queue_global.pop(); ////remove this node from the queue before proceeding

  dbdet_curvelet* cur_cvlet = cur_node->cvlet;

  //check to see if this curvelet is already part of some other HT
  //if it is, terminate this branch here and add a link to the HTG signalling HT interaction
  vcl_set<dbdet_hyp_tree_node*> int_HTs = check_for_interaction(cur_node);
  bool terminal_interaction = false;
  
  if (int_HTs.size()>0)//any interaction detected
  {
    //add interactions to HTG
    vcl_set<dbdet_hyp_tree_node*>::iterator hit = int_HTs.begin();
    for (; hit != int_HTs.end(); hit++)
    {
      if ((*hit)->dir != cur_node->dir){     //candidate CPL Link
        //CPL interaction is only valid if it is C1
        if (C1_CPL_interaction(cur_node, (*hit))){
          HTG.insert_CPL_link(cur_node->tree_id, (*hit)->tree_id);
        }
        terminal_interaction = true;
      }
      else                                   //CPT link
        HTG.insert_CPT_link(cur_node->tree_id, (*hit)->tree_id);
    }

    //if terminal interaction encountered, do not look further
    if (terminal_interaction)
      return;
  }

  //This curvelet did not interact with any other HT in a terminal manner, so continue its growth
  //check the appropriate link graph for any bifurcations from this curvelet
  for (unsigned i=1; i<cur_cvlet->edgel_chain.size(); i++)
  {
    dbdet_edgel* e = cur_cvlet->edgel_chain[i];

    //choose the right link graph
    dbdet_link_list& clinks = edge_link_graph_.cLinks[e->id];
    if (!cur_node->dir)    clinks = edge_link_graph_.cLinks2[e->id];

    //Regular continuation:: even if the final edgel does not have any bifurcations, explore its child link for continuing the curve
    if ((i!=cur_cvlet->edgel_chain.size()-1) && (clinks.size()<2)) continue; //no bifurcations: continue unless its the last edgel

    //explore all legal continuations from bifurcations
    for (dbdet_link_list_iter lit = clinks.begin(); lit != clinks.end(); lit++)
      explore_continuations(cur_node, *lit);
  }

  //if all the propagations required this curvelet to be chopped and the node recreated
  //then this node ought to be removed
  remove_node_if_redundant(cur_node);

}

//: if the edgels involved in this curvelet are also claimed by other HTs, check for competitive or completive interaction
vcl_set<dbdet_hyp_tree_node*> dbdet_sel_base::check_for_interaction(dbdet_hyp_tree_node* node)
{
  dbdet_curvelet* cvlet = node->cvlet;

  vcl_set<dbdet_hyp_tree_node*> int_HTs;
  for (unsigned i=1; i<cvlet->edgel_chain.size()-1; i++){
    vcl_set<dbdet_hyp_tree_node*>& HT_labels = ELs.labels[cvlet->edgel_chain[i]->id];
    for (vcl_set<dbdet_hyp_tree_node*>::iterator hit=HT_labels.begin(); hit != HT_labels.end(); hit++){
      if ((*hit)->tree_id != node->tree_id)
        int_HTs.insert((*hit));
    }
  }
  return int_HTs;
}

//: check to see if a CPL interaction is legal by checking for C1 continuity
bool dbdet_sel_base::C1_CPL_interaction(dbdet_hyp_tree_node* node1, dbdet_hyp_tree_node* node2)
{
  //go over all the edgels to find the set of interacting edgels

  dbdet_curvelet* cvlet1 = node1->cvlet;
  dbdet_curvelet* cvlet2 = node2->cvlet;

  vcl_vector<dbdet_edgel*> int_edgels;
  for (unsigned i=1; i<cvlet1->edgel_chain.size(); i++){
    vcl_set<dbdet_hyp_tree_node*>& HT_labels = ELs.labels[cvlet1->edgel_chain[i]->id];
    for (vcl_set<dbdet_hyp_tree_node*>::iterator hit=HT_labels.begin(); hit != HT_labels.end(); hit++){
      if ((*hit) == node2){
        int_edgels.push_back(cvlet1->edgel_chain[i]);
      }
    }
  }

  //try breaking the curvelets at each of the candidate overlapping points
  for (unsigned i=0; i<int_edgels.size(); i++)
  {
    dbdet_edgel* e = int_edgels[i];

    //construct curvelets chopped at this edgel
    dbdet_curvelet* chopped_cvlet1 = construct_chopped_cvlet(cvlet1, e);
    dbdet_curvelet* chopped_cvlet2 = construct_chopped_cvlet(cvlet2, e);

    //constrain their curve bundles from their parent's CB(if they exists)
    if (node1->parent && propagate_constraints)
      chopped_cvlet1->replace_curve_model(C1_continuity_possible(node1->parent->cvlet, chopped_cvlet1));

    if (node2->parent && propagate_constraints)
      chopped_cvlet2->replace_curve_model(C1_continuity_possible(node2->parent->cvlet, chopped_cvlet2));

    //check for C1 continuity
    dbdet_curve_model* constrained_cb = C1_continuity_possible(chopped_cvlet1, chopped_cvlet2);
    if (!constrained_cb){
      delete chopped_cvlet1;
      delete chopped_cvlet2;
      continue;
    }

    //C1 continuity possible (replace the curvelets in the nodes by the chopped curvelet)
    remove_labels(node1);
    remove_labels(node2);

    delete node1->cvlet;
    delete node2->cvlet;

    //replace the curvelets
    node1->cvlet = chopped_cvlet1;
    node2->cvlet = chopped_cvlet2;

    //replace the CBs in the curvelets
    node1->cvlet->replace_curve_model(constrained_cb);
    node2->cvlet->replace_curve_model(constrained_cb);

    //add new labels
    label_edgels(node1);
    label_edgels(node2);

    return true;
  }

  return false;
}

void dbdet_sel_base::explore_continuations(dbdet_hyp_tree_node* cur_node, dbdet_link* link)
{
  dbdet_curvelet* cur_cvlet = cur_node->cvlet;

  //if the link is in the current curvelet, ignore it because it has already been explored by the cur_cvlet
  for (unsigned i=0; i<cur_cvlet->edgel_chain.size()-1; i++){
    if (cur_cvlet->edgel_chain[i]==link->pe && cur_cvlet->edgel_chain[i+1]==link->ce)
      return;
  }

  //if this is an unexplored link, order the cvlets by the distance from the current edgel (either bifurcation or terminal edgel)
  //Note: if this is successful, we can do this just ONCE for all the links in the link graph before starting the algorithm or when they are added to the link
  vcl_map<int, dbdet_curvelet*> sorted_cvlist;

  //go through all the cvlets pointed to by the link and weed out the illegal ones
  for (cvlet_list_iter cvit = link->curvelets.begin(); cvit != link->curvelets.end(); cvit++)
  {
    //these curvelets can be overlapped with the current curvelet, but they have to advance the curve trace
    //otherwise they are useless
    if (!cvlet_advances_curve(cur_cvlet, *cvit)) //this curvelet does not go beyond the last curvelet
      continue;

    //the overlapped portion of it has to be valid symbolically, because it has to maintain the path
    if (!cvlets_overlap_legally(cur_cvlet, *cvit))
      continue;

    //if it passes all the tests, enter it into a sorted list
    sorted_cvlist.insert(vcl_pair<int, dbdet_curvelet*>(dist_from_edge(*cvit, link->pe), *cvit));
  }

  //check each of them for compatibility, add the first one that is compatible
  vcl_map<int, dbdet_curvelet*>::iterator cv_it = sorted_cvlist.begin();
  for (; cv_it != sorted_cvlist.end(); cv_it++)
  {
    dbdet_curvelet* cv = cv_it->second;

    //this curvelet has to start at least from the middle of the current curvelet
    //if (cv_it->first > cur_cvlet->edgel_chain.size()/2)
    //  continue;

    //check for overlap between the cur_cvlet and the candidate next cvlet
    if (cur_cvlet->edgel_chain.back() != cv->edgel_chain.front()) 
    { 
      //Since there is an overlap, we need to construct a new cvlet by removing the overlapped edgels first
      dbdet_curvelet* chopped_cvlet = construct_chopped_cvlet(cur_cvlet, cv->ref_edgel);

      //constrain its curve bundle from its parent's CB(if it exists)
      if (cur_node->parent && propagate_constraints)
        chopped_cvlet->replace_curve_model(C1_continuity_possible(cur_node->parent->cvlet, chopped_cvlet));

      //check for C1 continuity
      dbdet_curve_model* constrained_cb = C1_continuity_possible(chopped_cvlet, cv);
      if (!constrained_cb){
        delete chopped_cvlet;
        continue;
      }

      //C1 possible
      dbdet_hyp_tree_node* new_cur_node;

      //if cur_node is a root node, replace the root node of the HT
      if (!cur_node->parent)
      {
        //find the HT
        dbdet_hyp_tree* HT = HTG.nodes[cur_node->tree_id];

        new_cur_node = new dbdet_hyp_tree_node(chopped_cvlet, cur_node->tree_id, cur_node->dir);
        HT->root = new_cur_node;

        remove_labels(cur_node); //remove all its labels

        //sanity check
        //assert(cur_node->children.size()==0);
        //delete cur_node;
        
        //cur_node = 0;
      }
      else {
        //But first make a new branch from the parent of the cur_node with the new chopped cvlet
        new_cur_node = cur_node->parent->add_child(chopped_cvlet);
      }

      // Also claim ownership of this cvlet by adding labels to its edgels
      label_edgels(new_cur_node);

      //now add the candidate curvelet as a child on the new cur_node
      dbdet_hyp_tree_node* new_node;
      if (propagate_constraints)
        new_node = new_cur_node->add_child(new dbdet_curvelet(*cv, constrained_cb));
      else {
        new_node = new_cur_node->add_child(new dbdet_curvelet(*cv));
        delete constrained_cb;
      }

      BFS_queue_global.push(new_node);

      // Also claim ownership of this cvlet by adding labels to its edgels
      label_edgels(new_node);

      //special operation: If the cur_node got replaced by a chopped one after propagation, remove the 
      return;
    }
    else { //no overlap
      //check for C1 continuity
      dbdet_curve_model* constrained_cb = C1_continuity_possible(cur_cvlet, cv);
      if (!constrained_cb)
        continue;

      //C1 possible: add this curvelet as a node in the current hyp tree
      dbdet_hyp_tree_node* new_node;
      if (propagate_constraints)
        new_node = cur_node->add_child(new dbdet_curvelet(*cv, constrained_cb));
      else {
        new_node = cur_node->add_child(new dbdet_curvelet(*cv));
        delete constrained_cb;
      }
      BFS_queue_global.push(new_node);

      // Also claim ownership of this cvlet by adding labels to its edgels
      label_edgels(new_node);

      return;
    }
  }
}

int dbdet_sel_base::dist_from_edge(dbdet_curvelet* cvlet, dbdet_edgel* e)
{
  for (unsigned i=0; i<cvlet->edgel_chain.size(); i++)
    if (cvlet->edgel_chain[i]==e)
      return i;

  //this should never happen, if it does return -1
  return -1;
}

bool dbdet_sel_base::cvlet_advances_curve(dbdet_curvelet* last_cvlet, dbdet_curvelet* cur_cvlet)
{
  //these curvelets can be overlapped with the current curvelet, but they have to advance the curve trace
  //otherwise they are useless
  dbdet_edgel* last_e = last_cvlet->edgel_chain.back();
  bool last_e_passed = false;
  for (unsigned i=1; i<cur_cvlet->edgel_chain.size(); i++){
    if (cur_cvlet->edgel_chain[i-1] == last_e){
      last_e_passed = true;
      break;
    }
  }

  return last_e_passed;
}

bool dbdet_sel_base::cvlets_overlap_legally(dbdet_curvelet* last_cvlet, dbdet_curvelet* cur_cvlet)
{
  //these curvelets can be overlapped with the current curvelet, but the overlap has to be consistent
  bool first_edgel_found = false;
  unsigned shift_ind = 0;  //the shift in the edgel chain betweel the last curvelet and the current curvelet
  for (unsigned i=1; i<last_cvlet->edgel_chain.size(); i++){
    if (last_cvlet->edgel_chain[i] == cur_cvlet->ref_edgel){
      first_edgel_found = true;
      shift_ind = i;
    }

    if (first_edgel_found && last_cvlet->edgel_chain[i] != cur_cvlet->edgel_chain[i-shift_ind])
      return false;
  }

  if (!first_edgel_found) //the curvelet does not overlap with the first edgel (therefore illegal)
    return false;
  else
    return true; //the curvelets overlap in a legal manner
}

//: construct a new curvelet by chopping a curvelet up to a given point
dbdet_curvelet* dbdet_sel_base::construct_chopped_cvlet(dbdet_curvelet* cvlet, dbdet_edgel* e)
{
  //construct a new cvlet out of the non-overlapped edgels
  vcl_deque<dbdet_edgel*> new_edgel_chain;
  for (unsigned i=0; i<=cvlet->edgel_chain.size(); i++){
    new_edgel_chain.push_back(cvlet->edgel_chain[i]);

    if (cvlet->edgel_chain[i] == e)
      break;
  }

  dbdet_curvelet* new_cvlet = form_an_edgel_grouping(cvlet->ref_edgel, new_edgel_chain, cvlet->forward, false);

  //assert(new_cvlet); //sanity check (this should always be possible!)
  if (!new_cvlet){ //this is a big bug !!! whyis this happening routinely?
    vcl_cout << "cvlet chopping failed!" << vcl_endl;

    //for now create a simple copy of the cvlet's CB
    new_cvlet = new dbdet_curvelet(*cvlet);

    //replace the edgel chain with the new one
    new_cvlet->edgel_chain.clear();
    for (unsigned i=0; i<new_edgel_chain.size(); i++)
      new_cvlet->edgel_chain.push_back(new_edgel_chain[i]);

    new_cvlet->compute_properties(rad_, token_len_);
  }

  return new_cvlet;
}

//: determine if C1 continuation is possible between a pair of curvelets
dbdet_curve_model* dbdet_sel_base::C1_continuity_possible(dbdet_curvelet* cur_cvlet, dbdet_curvelet* next_cvlet)
{
  //intersect the curve bundles at the common edgel
  //  a) transport the cb from CB1 to e
  //  b) intersect CB1 and CB2 (C^1)

  dbdet_edgel* e = next_cvlet->ref_edgel;

  //transport CB1 to current edgel
  dbdet_CC_curve_model_new* trans_cb = (dbdet_CC_curve_model_new*) cur_cvlet->curve_model->transport(e->pt, e->tangent);

  //check to see if there can be a legal C1 continuation between trans_cb and the next cb
  dbdet_curve_model* new_cb = trans_cb->C1_transition(next_cvlet->curve_model);
  
  delete trans_cb; //no need for this anymore

  if (new_cb && new_cb->bundle_is_valid()){ //if bundle is legal 
    new_cb->compute_best_fit();
    return new_cb;
  }

  //if an illegal CB is formed, delete it
  if (new_cb){
    delete new_cb;
    new_cb = 0;
  }

  return new_cb;
}

//: claim ownership of this cvlet by adding labels to its edgels
void dbdet_sel_base::label_edgels(dbdet_hyp_tree_node* node)
{
  //if the edgels are labeled with its parent node, remove those labels and add its own label
  //the first edgel belongs to its parent
  for (unsigned i=1; i<node->cvlet->edgel_chain.size(); i++){
    //if (ELs.labels[node->cvlet->edgel_chain[i]->id].find(node->parent) != ELs.labels[node->cvlet->edgel_chain[i]->id].end())
    //  ELs.labels[node->cvlet->edgel_chain[i]->id].erase(node->parent);

    //now add its own label
    ELs.labels[node->cvlet->edgel_chain[i]->id].insert(node);
  }
}

//: remove ownership labels for the edgels in this HT branch
void dbdet_sel_base::remove_labels(dbdet_hyp_tree_node* node)
{
  //remove the label from all the edgels in this curvelet
  for (unsigned i=1; i<node->cvlet->edgel_chain.size(); i++)
    ELs.labels[node->cvlet->edgel_chain[i]->id].erase(node);

  //recursively travel through this branch and remove labels from all the HT nodes
  vcl_list<dbdet_hyp_tree_node*>::iterator nit = node->children.begin();
  for (; nit != node->children.end(); nit++)
    remove_labels(*nit);
}

//: if this HT node is redundant remove it
void dbdet_sel_base::remove_node_if_redundant(dbdet_hyp_tree_node* cur_node)
{
  //if all the propagations required this curvelet to be chopped and the node recreated
  //then this node ought to be removed

  // Note: An HT node is redundant if an alternate pathway completely overlaps with it

  //specal case: if the cur_node used to be a root node but it was replaced, it is redundant
  if (!cur_node->parent){
    if (HTG.nodes[cur_node->tree_id]->root != cur_node){
      delete cur_node;
    }
    return;
  }

  if (cur_node->parent->children.size()==1) //no need to prune it
    return;

  if (cur_node->children.size()>0) //has a child node, cannot be duplicate
    return;

  vcl_list<dbdet_hyp_tree_node*>::iterator cit = cur_node->parent->children.begin();
  for (; cit != cur_node->parent->children.end(); cit++)
  {
    if (*cit == cur_node) continue;

    //check for duplicate edgel trace
    bool duplicate_path = true;
    dbdet_hyp_tree_node* node = *cit;
    unsigned j=0;
    for (unsigned i=0; i<cur_node->cvlet->edgel_chain.size(); i++){
      //compare with the alternate trace
      if (node->cvlet->edgel_chain[j] != cur_node->cvlet->edgel_chain[i]){
        duplicate_path = false;
        break;
      }

      j++; //increment trace index
      if (j==node->cvlet->edgel_chain.size()){
        if (node->children.size()!=1)
          break; //end of the trace
        else
          node = node->children.front(); //go to next node

        j=1;  //reset trace index to the second edgel on the next node (the first should be repeated)
      }
    }

    if (duplicate_path){
      //remove labels first
      remove_labels(cur_node);

      //then delete this node
      delete cur_node;

      return;
    }
  }
}

//: perform gradient descent disambiguation of the HTG to segment the contours and resolve all linking ambiguities
void dbdet_sel_base::disambiguate_the_HTG()
{
  vcl_cout << "Disambiguating the HTG..." << vcl_endl;

  vcl_cout << "1. Resolving completion HTs...";
  resolve_HTG_completion();
  vcl_cout << "done!" << vcl_endl;

  //once all the completion edges are handled, resolve the paths of competing edges
  vcl_cout << "2. Resolving competing HTs...";
  resolve_HTG_competition();
  vcl_cout << "done!" << vcl_endl;

  //now prune all the remaining HTs to select the best paths (these are the HTG nodes with no adjacent nodes)
  vcl_cout << "3. Resolving non-interacting HTs...";
  for (unsigned i=0; i<HTG.nodes.size(); i++)
    resolve_HT(HTG.nodes[i]);

  //clear the BFS queue
  while(!BFS_queue_global.empty())
    BFS_queue_global.pop();

  vcl_cout << "done!" << vcl_endl;
}

//: Resolve the CPL links 
void dbdet_sel_base::resolve_HTG_completion()
{
  // 1) make a list of all the completion edges
  vcl_list<dbdet_HTG_link_path*> CPL_links;
  vcl_set <vcl_pair<int, int> > existing_links;
  for (unsigned i=0; i<HTG.nodes.size(); i++)
  {
    dbdet_hyp_tree* HT1 = HTG.nodes[i];

    vcl_set<int>::iterator lit = HTG.CPL_links[i].begin();
    for (; lit != HTG.CPL_links[i].end(); lit++)
    {
      dbdet_hyp_tree* HT2 = HTG.nodes[*lit];

      //links are undirected, so this link might already have been considered
      if (existing_links.find(vcl_pair<int, int>(*lit, i)) == existing_links.end()){
        existing_links.insert(vcl_pair<int, int>(i, *lit)); //insert it into the list
        CPL_links.push_back(new dbdet_HTG_link_path(HT1, HT2));
      }
    }
  }

  // 2) compute their path metrics
  vcl_list<dbdet_HTG_link_path*>::iterator HTlit = CPL_links.begin();
  for (; HTlit != CPL_links.end(); HTlit++)
    determine_optimal_HTG_completion_path(*HTlit);

  // 3) sort the list by path metric
  CPL_links.sort(path_metric_less_than);

  // 4) gradient descent to resolve ambiguities
  vcl_vector<dbdet_HTG_link_path*> paths_to_del;
  HTlit = CPL_links.begin();
  for (; HTlit != CPL_links.end(); HTlit++)
  {
    //check for ambiguity
    if (HTG.CPL_links[(*HTlit)->src->tree_id].size()>1 || HTG.CPL_links[(*HTlit)->tgt->tree_id].size()>1){
      //resolve ambiguity by removing this link
      HTG.remove_CPL_link((*HTlit)->src->tree_id, (*HTlit)->tgt->tree_id);

      paths_to_del.push_back(*HTlit);
    }
  }
  //delete the ambiguous paths
  for (unsigned i=0; i<paths_to_del.size(); i++){
    CPL_links.remove(paths_to_del[i]);
    delete paths_to_del[i];
  }
  paths_to_del.clear();

  // 5) once the completions have been sorted out, prune the HTs to retain only the best path
  vcl_list<dbdet_HTG_link_path*>::iterator pit = CPL_links.begin();
  for (; pit != CPL_links.end(); pit++)
    resolve_HTG_completion_path(*pit);

}


//: trace all paths between the HT root nodes and evaluate their path metrics
// report the best path and cost
void dbdet_sel_base::determine_optimal_HTG_completion_path(dbdet_HTG_link_path* link)
{
  //visit all the leaf nodes of HT1 to determine which ones interact with HT2
  //make a list of all the independent paths
  vcl_list<vcl_vector<dbdet_curvelet*> > paths;

  //traverse the hyp tree to find the leaf nodes
  dbdet_hyp_tree::iterator pit = link->src->begin();
  for ( ; pit != link->src->end(); pit++){
    if ((*pit)->is_leaf()){
      //check to see if it interacts with HT2
      vcl_set<dbdet_hyp_tree_node*> ints = check_for_interaction(*pit); 

      vcl_set<dbdet_hyp_tree_node*>::iterator iit = ints.begin();
      for (; iit != ints.end(); iit++)
      {
        //if interaction with HT2 found, record the path
        if ((*iit)->tree_id == link->tgt->tree_id){

          //make sure this is a leaf node of HT2
          //if (!(*iit)->is_leaf()) continue;

          //record this path
          paths.push_back(pit.get_cur_path());
          vcl_vector<dbdet_curvelet*>& cur_path = paths.back();

          //back track through HT2 to get a list of curvelets from the HT2 path and add it to the current path
          dbdet_hyp_tree_node* parent = (*iit);
          while (parent != 0){
            cur_path.push_back(parent->cvlet);
            parent = parent->parent;
          }
        }
      }
    }
  }

  //compute the path cost of all the completion paths found and record the least cost path
  link->cost = 1000;
  for (vcl_list<vcl_vector<dbdet_curvelet*> >::iterator pit = paths.begin(); pit != paths.end(); pit++){
    double path_cost = compute_path_metric(*pit);
    if (path_cost<link->cost){
      link->cost = path_cost; //also save the path cost
      link->cvlets = (*pit); //save the best path
    }
  }

}

//: resolve the CPL path by pruning the HTs to keep only the optimal path
void dbdet_sel_base::resolve_HTG_completion_path(dbdet_HTG_link_path* link)
{
  //traverse both HT trees and prune all other paths
  //I'm assuming that all cvlet overlaps have been worked out by now
  
  //traverse the HT along with the optimal curvelet trace
  dbdet_hyp_tree_node* h1 = link->src->root;
  for (unsigned i=0; i<link->cvlets.size()-1; i++)
  {
    assert(h1->cvlet == link->cvlets[i]); //sanity check

    //claim all the edgels in this cvlet permanently
    claim_edgels(h1);

    //delete the branches that do not agree with the optimal path
    vcl_vector<dbdet_hyp_tree_node*> branches_to_del;
    vcl_list<dbdet_hyp_tree_node*>::iterator cit = h1->children.begin();
    for (; cit != h1->children.end(); cit++){
      if ((*cit)->cvlet != link->cvlets[i+1])
        branches_to_del.push_back(*cit); //delete this branch from the tree
    }
    //remove the labels on the link graph due to the branches that are going to be deleted
    for (unsigned i=0; i<branches_to_del.size(); i++)
      remove_labels(branches_to_del[i]);

    //now delete the branches
    for (unsigned i=0; i<branches_to_del.size(); i++)
      delete branches_to_del[i];
    
    //break when the leaf node is encountered (or all pruning is over)
    if (h1->is_leaf())
      break;
    else
      h1 = h1->children.front(); //advance the pointer
  }

  //now from the other end
  dbdet_hyp_tree_node* h2 = link->tgt->root;
  for (unsigned i=link->cvlets.size()-1; i>0; i--)
  {
    assert(h2->cvlet == link->cvlets[i]); //sanity check

    //claim all the edgels in this cvlet permanently
    claim_edgels(h2);

    //delete the branches that do not agree with the optimal path
    vcl_vector<dbdet_hyp_tree_node*> branches_to_del;
    vcl_list<dbdet_hyp_tree_node*>::iterator cit = h2->children.begin();
    for (; cit != h2->children.end(); cit++){
      if ((*cit)->cvlet != link->cvlets[i-1])
        branches_to_del.push_back(*cit); //delete this branch from the tree
    }
    //remove the labels on the link graph due to the branches that are going to be deleted
    for (unsigned i=0; i<branches_to_del.size(); i++)
      remove_labels(branches_to_del[i]);

    //now delete the branches
    for (unsigned i=0; i<branches_to_del.size(); i++)
      delete branches_to_del[i];

    //break when the leaf node is encountered (all pruning is over)
    if (h2->is_leaf())
      break;
    else
      h2 = h2->children.front(); //advance the pointer
  }

  //mark this HT has resolved
  link->src->resolved = true;
  link->tgt->resolved = true;
}

//: a path is described as a sequence of curvelets for now, but this can be optimized since multiple paths through
// the HT will have common elements so there is no real need to redo the same computations
void dbdet_sel_base::back_propagate_solution(vcl_vector<dbdet_curvelet*>& path, vgl_point_2d<double> sol)
{
  //we need to resolve the contour to C1 explicitly first
  //We can achieve this by backtracking from the last curvelet

  //back propagate the solution
  bool sgn_change;

  if (path.size()<=1)
    return ;

  // get a single curve solution by backtracking a single curve through the curvelet sequence
  for (int i=path.size()-1; i>0; i--)
  {
    //transport the solution backward to update the curve models
    dbdet_CC_curve_model_new* cur_cm = (dbdet_CC_curve_model_new*)path[i]->curve_model;
    dbdet_CC_curve_model_new* prev_cm = (dbdet_CC_curve_model_new*)path[i-1]->curve_model;

    //find the best sub-bundle to transition to
    //ideally this should depend on the cost function
    unsigned best_transition;
    double best_local_cost = 1000;
    for (int j=0; j<NkClasses; j++)
    {
      if (prev_cm->cv_bundles[j].num_sheets()==0)
        continue;
 
      vgl_point_2d<double> temp_sol = prev_cm->transport_CC(sol, dbdet_k_classes[j], cur_cm->ref_pt, cur_cm->ref_theta, 
                                                                                      prev_cm->ref_pt, prev_cm->ref_theta, 
                                                                                      sgn_change);
      //if this solution cannot be transported, ignore
      int nj;
      if (sgn_change) 
        nj = NkClasses-1-j;
      else            
        nj = j;

      if (!prev_cm->cv_bundles[nj].contains(temp_sol))
        continue;
        
      double local_cost = vcl_fabs(dbdet_k_classes[nj] - cur_cm->k)*(path[i]->length+path[i-1]->length)/2.0 + //Int (dk/ds * l) ds
                          dbdet_k_classes[nj]*dbdet_k_classes[nj]*path[i-1]->length; //Int k^2 ds

      if (local_cost < best_local_cost)
      {
        best_local_cost = local_cost;
        best_transition = nj;
      }
    }

    //legal transition found
    if (best_local_cost < 1000)
    { 
      //apply this transition
      sol = prev_cm->transport_CC(sol, dbdet_k_classes[best_transition], cur_cm->ref_pt, cur_cm->ref_theta, 
                                                                          prev_cm->ref_pt, prev_cm->ref_theta, 
                                                                          sgn_change);
      //set this constrained solution as the best fit
      prev_cm->set_best_fit(sol, dbdet_k_classes[best_transition]);
    }
    else { // back tracking went wrong
      sol = prev_cm->compute_best_fit(); //just use the default solution
      vcl_cout << " Bracktracking solution failed!" << vcl_endl;
    }
  }
}

//: a path is described as a sequence of curvelets for now, but this can be optimized since multiple paths through
// the HT will have common elements so there is no real need to redo the same computations
double dbdet_sel_base::compute_path_metric(vcl_vector<dbdet_curvelet*>& path)
{
  ////for now just compute the length of the chain 
  //double cost = 0;
  //for (unsigned i=1; i<path.size(); i++){
  //  double c = dist_from_edge(path[i-1], path[i]->ref_edgel);
  //  if (c>=0) 
  //    cost += c;
  //  else {
  //    double c = dist_from_edge(path[i], path[i-1]->ref_edgel);
  //    if (c>=0) cost += c;
  //    else cost += path[i-1]->order(); //hack (not true)
  //  }
  //}

  //in order to compute the path metric, we need to resolve the contour to C1 explicitly first
  //We can achieve this by backtracking from the last curvelet

  //back propagate the solution
  vgl_point_2d<double> sol = path.back()->curve_model->compute_best_fit();
  double total_length = path.back()->length;
  double total_cost = 1 + ((dbdet_CC_curve_model_new*)path.back()->curve_model)->k*((dbdet_CC_curve_model_new*)path.back()->curve_model)->k*path.back()->length;
  bool sgn_change;

  if (path.size()<=1)
    return total_cost/total_length;

  // get a single curve solution by backtracking a single curve through the curvelet sequence
  for (int i=path.size()-1; i>0; i--)
  {
    //transport the solution backward to update the curve models
    dbdet_CC_curve_model_new* cur_cm = (dbdet_CC_curve_model_new*)path[i]->curve_model;
    dbdet_CC_curve_model_new* prev_cm = (dbdet_CC_curve_model_new*)path[i-1]->curve_model;

    total_length += path[i-1]->length;

    //find the best sub-bundle to transition to
    //ideally this should depend on the cost function
    unsigned best_transition;
    double best_local_cost = 1000;
    for (int j=0; j<NkClasses; j++)
    {
      if (prev_cm->cv_bundles[j].num_sheets()==0)
        continue;
 
      vgl_point_2d<double> temp_sol = prev_cm->transport_CC(sol, dbdet_k_classes[j], cur_cm->ref_pt, cur_cm->ref_theta, 
                                                                                      prev_cm->ref_pt, prev_cm->ref_theta, 
                                                                                      sgn_change);
      //if this solution cannot be transported, ignore
      int nj;
      if (sgn_change) 
        nj = NkClasses-1-j;
      else            
        nj = j;

      if (!prev_cm->cv_bundles[nj].contains(temp_sol))
        continue;
        
      double local_cost = 1 + vcl_fabs(dbdet_k_classes[nj] - cur_cm->k)/(path[i]->length+path[i-1]->length) + //Int |dk/ds| ds
                          dbdet_k_classes[nj]*dbdet_k_classes[nj]*path[i-1]->length; //Int k^2 ds

      if (local_cost < best_local_cost)
      {
        best_local_cost = local_cost;
        best_transition = nj;
      }
    }

    //legal transition found
    if (best_local_cost < 1000)
    { 
      //apply this transition
      sol = prev_cm->transport_CC(sol, dbdet_k_classes[best_transition], cur_cm->ref_pt, cur_cm->ref_theta, 
                                                                          prev_cm->ref_pt, prev_cm->ref_theta, 
                                                                          sgn_change);
      //set this constrained solution as the best fit
      prev_cm->set_best_fit(sol, dbdet_k_classes[best_transition]);
      total_cost += best_local_cost;
    }
    else { // back tracking went wrong
      sol = prev_cm->compute_best_fit(); //just use the default solution
      vcl_cout << " Bracktracking solution failed!" << vcl_endl;

      double local_cost = 1+ vcl_fabs(prev_cm->k - cur_cm->k)/(path[i]->length+path[i-1]->length) + //Int |dk/ds| ds
                          prev_cm->k*prev_cm->k*path[i-1]->length; //Int k^2 ds
      total_cost += local_cost;
    }
  }

  return total_cost/total_length;
}

//: Resolve the competing HTs
void dbdet_sel_base::resolve_HTG_competition()
{
  // Assume that all the HT completions have been sorted out and the best paths resolved and claimed

  // 1) First make a list of all HTs involved in competitive interactions
  vcl_vector<dbdet_hyp_tree* > CPT_HTs;
  for (unsigned i=0; i<HTG.nodes.size(); i++){
    if (HTG.CPT_links[i].size()>0)
      CPT_HTs.push_back(HTG.nodes[i]);
  }

  // 2) For each HT, if the other HT involved in the link has already been resolved, prune the paths
  //    that are in conflict with the claimed edgels.
  for (unsigned i=0; i<CPT_HTs.size(); i++){
    if (!CPT_HTs[i]->resolved)
      continue;

    //if this HT has already been resolved, prune the other trees
    vcl_set<int>::iterator lit = HTG.CPT_links[CPT_HTs[i]->tree_id].begin();
    for (; lit != HTG.CPT_links[CPT_HTs[i]->tree_id].end(); lit++)
      prune_HTs_of_claimed(HTG.nodes[*lit]);
  }

  // 3) Compute the best paths for each CPT HTs and its cost
  vcl_map<double, dbdet_hyp_tree*> sorted_CPT_HTs;
  for (unsigned i=0; i<CPT_HTs.size(); i++){
    //only need to keep the unresolved ones
    if (CPT_HTs[i]->resolved)
      continue;

    double cost = determine_best_path(CPT_HTs[i]);
    sorted_CPT_HTs.insert(vcl_pair<double, dbdet_hyp_tree*>(cost, CPT_HTs[i]));
  }

  // 4) sort the paths by cost (automatic because of the map structure)

  // 5) Gradient descent to resolve the paths (least cost path to the highest cost path)
  while (sorted_CPT_HTs.size()>0)
  {
    // 5a) claim the least cost path for this HT
    dbdet_hyp_tree* cur_HT = sorted_CPT_HTs.begin()->second;
    claim_best_path(cur_HT);

    // 5b) remove it from the gradient-descent list
    sorted_CPT_HTs.erase(sorted_CPT_HTs.begin());

    // 5c) If the HT node is involved in any CPT links with other HTs, prune the other HTs to shed
    //     the claimed edgels and recompute its cost
    vcl_set<int>::iterator lit = HTG.CPT_links[cur_HT->tree_id].begin();
    for (; lit != HTG.CPT_links[cur_HT->tree_id].end(); lit++)
    {
      dbdet_hyp_tree* HT1 = HTG.nodes[*lit];
      if (HT1->resolved)
        continue;
      
      //if these HTs have not been resolved, check for any pruning required

      //find the HT and remove it from the sorted list
      vcl_map<double, dbdet_hyp_tree*>::iterator tit = sorted_CPT_HTs.find(HT1->least_cost);
      assert(tit->second == HT1); //sanity check
      sorted_CPT_HTs.erase(tit);

      //if pruning is done, recompute its cost
      if (prune_HTs_of_claimed(HT1))
      {
        double cost = determine_best_path(HT1);
        sorted_CPT_HTs.insert(vcl_pair<double, dbdet_hyp_tree*>(cost, HT1));
      }
      else {
        sorted_CPT_HTs.insert(vcl_pair<double, dbdet_hyp_tree*>(HT1->least_cost, HT1));
      }
    }
  }

  // 6) remove all completion links from the HTG
  HTG.clear_all_CPT_links();

}

//: go over the HT and delete any path involving edgels that have already been claimed
bool dbdet_sel_base::prune_HTs_of_claimed(dbdet_hyp_tree* HT)
{
  bool pruning_done = false;

  //if this HT has already been resolved, there is no need to prune
  if (HT->resolved)
    return false;

  dbdet_hyp_tree::iterator pit = HT->begin();
  for ( ; pit != HT->end(); pit++){
    dbdet_hyp_tree_node* cur_node = (*pit);

    //check to see if the edgels corresponding to this curvelet have been permanently claimed by any HT
    //if it has, delete the subtree starting from here
    for (unsigned i=0; i<cur_node->cvlet->edgel_chain.size(); i++){
      if (ELs.claimed[cur_node->cvlet->edgel_chain[i]->id]){

        if (cur_node == HT->root){
          //if this is the root node, it cannot be deleted
          //just delete its children, it will have to be replaced with a chopped curvelet
          while (cur_node->children.size()>0){
            delete_HT_subtree(cur_node->children.front());
          }
        }
        else {
          //first remove the labels from the subtree
          remove_labels(cur_node);

          //delete HT subtree
          HT->delete_subtree(pit); //this will make sure that the iterator is valid after deletion
        }
        pruning_done = true;
        break;
      }
    }
  }

  HT->best_path = 0;
  HT->least_cost = 1000.0;

  //recompute best path after pruning
  determine_best_path(HT);

  return pruning_done;
}

//: delete the subtree rooted at the given tree and also remove all labels associated with it
void dbdet_sel_base::delete_HT_subtree(dbdet_hyp_tree_node* cur_node)
{
  //first remove the labels
  remove_labels(cur_node);

  //then delete the subtree
  delete cur_node;
}

//: compute the best path amongst the alternate paths represented in a hyp tree
double dbdet_sel_base::determine_best_path(dbdet_hyp_tree* HT)
{
  //traverse all the paths and compute the path cost storing the path of least cost
  dbdet_hyp_tree::iterator pit = HT->begin();
  for ( ; pit != HT->end(); pit++){
    if ((*pit)->is_leaf()){
      double path_cost = compute_path_metric(pit.get_cur_path());
      if (path_cost<HT->least_cost){
        HT->least_cost = path_cost; //save the path cost
        HT->best_path = (*pit); //save the best path
      }
    } 
  }

  return HT->least_cost; //return the path cost of the best path
}


//: claim the best path for this HT and delete all other branches
void dbdet_sel_base::claim_best_path(dbdet_hyp_tree* HT)
{
  assert(HT->best_path != 0); //sanity check

  //find the curvelet list corresponding to the best path
  vcl_vector<dbdet_curvelet*> best_path;
  dbdet_hyp_tree::iterator pit = HT->begin();
  for ( ; pit != HT->end(); pit++){
    if ((*pit)==HT->best_path)
      best_path = pit.get_cur_path();
  }

  //now prune all paths except the best path

  //traverse the HT along with the optimal curvelet trace
  dbdet_hyp_tree_node* h1 = HT->root;
  for (unsigned i=0; i<best_path.size()-1; i++)
  {
    assert(h1->cvlet == best_path[i]); //sanity check

    //claim all the edgels in this cvlet permanently
    claim_edgels(h1);

    //delete the branches that do not agree with the optimal path
    vcl_vector<dbdet_hyp_tree_node*> branches_to_del;
    vcl_list<dbdet_hyp_tree_node*>::iterator cit = h1->children.begin();
    for (; cit != h1->children.end(); cit++){
      if ((*cit)->cvlet != best_path[i+1])
        branches_to_del.push_back(*cit); //delete this branch from the tree
    }
    //remove the labels on the link graph due to the branches that are going to be deleted
    for (unsigned i=0; i<branches_to_del.size(); i++)
      remove_labels(branches_to_del[i]);

    //now delete the branches
    for (unsigned i=0; i<branches_to_del.size(); i++)
      delete branches_to_del[i];
    
    //break when the leaf node is encountered (or all pruning is over)
    if (h1->is_leaf())
      break;
    else
      h1 = h1->children.front(); //advance the pointer
  }

  //mark this tree as resolved
  HT->resolved = true;

  //update the curvelets to reflect a continuous contour
  compute_path_metric(best_path);

}

//: claim permanenet ownership of the edgels in this cvlet
void dbdet_sel_base::claim_edgels(dbdet_hyp_tree_node* node)
{
  //remove the label from all the edgels in this curvelet
  for (unsigned i=0; i<node->cvlet->edgel_chain.size(); i++)
    ELs.claimed[node->cvlet->edgel_chain[i]->id] = true;
}


//: this is a simple function to trace all the paths represented by the HT (i.e., HT leaf nodes)
// and compute the path metric for each. Then remove all but the best path from the HT 
void dbdet_sel_base::resolve_HT(dbdet_hyp_tree* HT)
{
  if (HT->resolved)
    return;

  // 1) determine the best path in this HT
  determine_best_path(HT);

  // 2) claim the best path and prune the rest
  claim_best_path(HT);
}

void dbdet_sel_base::print_all_trees()
{
  for (unsigned i=0; i< HTG.nodes.size(); i++)
  {
    dbdet_hyp_tree* HT1 = HTG.nodes[i];
    HT1->print_all_paths();
  }
}

