// This is dbskr/dbskr_directed_tree.cxx

//:
// \file


#include "dbskr_directed_tree.h"

#include <vcl_iostream.h>









//------------------------------------------------------------------------------
//: Copy constructor
dbskr_directed_tree::
dbskr_directed_tree(const dbskr_directed_tree& other)
{
  // Copy everything except for cache data
  *this = other;

  // clear cache data
  vcl_vector<int > empty_vector;
  empty_vector.clear();
  this->dart_paths_.fill(empty_vector);
}



//------------------------------------------------------------------------------
//: acquire tree from the list of nodes
// "nodes" is a storage for the neighrboring nodes and costs of neighboring edges around each node
// Each member of the outer vector corresponds to one node in the tree, arranged
// in the same order as in "nodes_to_retain".
// Each member of the inner vector corresponds to an edge incident at the node,
// arranged counter-clockwise
bool dbskr_directed_tree::
acquire(vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > >& nodes) 
{
  if (nodes.size() == 0) 
  {
    vcl_cout << "Not able to recover nodes from shock graph, returning\n";
    return false;
  }

#if 0
  vcl_cout << "number of nodes: " << nodes.size() << " ";
  for (unsigned int i = 0; i<nodes.size(); i++) {
    for (unsigned int j = 0; j<nodes[i].size(); j++) {
      vcl_cout << nodes[i][j].first << " " << nodes[i][j].second.first << " " << nodes[i][j].second.second << "\n";
    }
    vcl_cout << vcl_endl;
  }
#endif

  dart_cnt_ = 2*(nodes.size()-1);
  node_cnt_ = nodes.size();

  //: initialize the arrays
  vcl_vector<int> tmp(dart_cnt_, -1);
  mate_ = tmp;
  tail_ = tmp;
  head_ = tmp;
  //TODO: check this -1 initialization is not causing problems
  surrogate_ = tmp;
  vcl_vector<bool> tmp2(dart_cnt_, false);
  up_ = tmp2;
  leaf_ = tmp2;
  vcl_pair<float, float> p(-1,-1);
  vcl_vector<dbskr_edge_info> tmp3(dart_cnt_, p);
  info_ = tmp3;

  // find the first leaf node
  unsigned int first = 0;
  for (unsigned int i = 0; i<nodes.size(); i++)
    if (nodes[i].size() == 1) {
      first = i;
      break;
    }
  //: traverse the tree in a depth first manner, stop when next node is 3
  unsigned int current_dart = 0;
  unsigned int current = first;
  unsigned int next_child = 0;
  int head = 0;
  do {
    head = nodes[current][next_child].first;
    head_[current_dart] = head;
    tail_[current_dart] = current;
    info_[current_dart] = nodes[current][next_child].second;
    int tmp = -1;
    if (nodes[head].size() == 1) {// head is a leaf node
      next_child = 0;
      leaf_[current_dart] = true;
      mate_[current_dart] = next(current_dart);
      mate_[next(current_dart)] = current_dart;
    } else {
      for (unsigned int i = 0; i<nodes[head].size(); i++)
        if (nodes[head][i].first == int(current)) {
          tmp = i;
          break;
        }
      next_child = unsigned(((tmp-1)+nodes[head].size())%nodes[head].size());
    }
    current = head;
    current_dart++;
  } while (head != int(first));

  if (current_dart != dart_cnt_) {
    vcl_cout << "traversal give more darts!\n";
    return false;
  }

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "head[" << i << "]: " << head_[i] << vcl_endl;
    vcl_cout << "info[" << i << "]: " << info_[i].first << " " << info_[i].second << vcl_endl;
  }
#endif

  //: fill in mate array
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    if (mate_[i] >= 0) continue;
    
    for (unsigned int j = 0; j<dart_cnt_; j++) 
      if (head_[i] == tail_[j] && tail_[i] == head_[j]) {
        mate_[i] = j;
        mate_[j] = i;
        break;
      }
  }

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "mate[" << i << "]: " << mate_[i] << vcl_endl;
  }
#endif

  //: initialize children array
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_vector<int> tmp;
    children_.push_back(tmp);
  }

  //: fill in children array, if leaf dart then its children array is empty
  //  children are in order from left to right (i.e. the left most child is first in the vector)
  //  (actually this child is traversed later than all other children of this node in the euler tour)
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    if (leaf_[i]) continue;
    
    vcl_vector<int> tmp2;
    int next_dart = next(i);
    
    while (next_dart != mate_[i]) {
      tmp2.push_back(next_dart);
      next_dart = next(mate_[next_dart]);
    }
    // push into the children array in reverse order of Euler tour 
    // (i.e. from left to right if we were to draw this tree such that Euler tour traverses it starting
    // from right most child always)
    for (int j = (tmp2.size()-1); j>=0; j--)
      children_[i].push_back(tmp2[j]);
  }

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "children[" << i << "]: ";
    for (unsigned int j = 0; j<children_[i].size(); j++)
      vcl_cout << children_[i][j] << " ";
    vcl_cout << vcl_endl;
  }
#endif

  

  //: initialize out_darts array
  //for (unsigned int i = 0; i<node_cnt_; i++) {
  //}
  for (unsigned int i = 0; i<node_cnt_; i++) {
    out_darts_.push_back(find_out_darts(i));
  }

#if 0
  for (unsigned int i = 0; i<node_cnt_; i++) {
    vcl_cout << "out_darts[" << i << "]: ";
    for (unsigned int j = 0; j<out_darts_[i].size(); j++)
      vcl_cout << out_darts_[i][j] << " ";
    vcl_cout << vcl_endl;
  }
#endif
  
  vcl_vector<int> tmp4;
  dart_paths_.resize(dart_cnt_, dart_cnt_);
  dart_paths_.fill(tmp4);

#if 0
  //simple check
  for (unsigned int i = 0; i<dart_cnt_; i++)
    for (unsigned int j = 0; j<dart_cnt_; j++)
      if (dart_paths_[i][j].size() != 0) 
        vcl_cout << "PROBLEM!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
#endif

  return true;
}





//------------------------------------------------------------------------------
//: find the centroid NODE (which is the best candidate to be the root)
// for a given node of the tree, if the node has degree 3 (i.e. degree greater than 2),
// assign the minimum of its three incident darts' subtree delete costs. If the
// node has lower degree, assign 0
// then select the node with max assigned cost
int dbskr_directed_tree::centroid() {
  // the number of nodes in the tree is half the number of darts
  int size = dart_cnt_/2;
  vcl_vector<float> costs;
  for (int i = 0; i<size; i++) {
    vcl_vector<int> outs = out_darts(i);
    if (outs.size() > 2) {

      float min_cost = subtree_delete_cost(outs[0]);
      for (unsigned int j = 1; j<outs.size(); j++) 
        if (subtree_delete_cost(outs[j]) < min_cost) 
          min_cost = subtree_delete_cost(outs[j]);
      
      costs.push_back(min_cost);
    } else {
      costs.push_back(0);
    }
  }

  int max_id = 0;
  float max_cost = costs[0];
  for (int i = 1; i<size; i++) 
    if (costs[i] > max_cost) {
      max_id = i;
      max_cost = costs[i];
    }
  
  return max_id;
}










//------------------------------------------------------------------------------
//: given a list of darts pointing down, set up flags for these darts and their mates, 
//  and the subtrees they point to
void dbskr_directed_tree::
set_up(vcl_vector<int>& down_darts) 
{
  //: traverse the tree down and collect the children 
  vcl_vector<int> stack(down_darts);
  while (!stack.empty()) 
  {
    int dart = stack.back();
    up_[dart] = false;
    up_[mate_[dart]] = true;
    stack.pop_back();

    for (unsigned int i = 0; i<children_[dart].size(); i++) 
    {
      stack.push_back(children_[dart][i]);
    }
  }  
  return;
}






//------------------------------------------------------------------------------
//: given two darts d1 and d2, find the unique path in the rooted tree from d1 to d2
//  return nodes in this path
//  (if d2 is not a descendent of d1, return empty list)
vcl_vector<int> dbskr_directed_tree::
find_node_path(int d1, int d2) 
{
  vcl_vector<int> node_list;
  
  //: first find the darts 
  vcl_vector<int> dart_list;
  dart_list.push_back(d1);

  vcl_vector<int> stack = children(d1);
  while (!stack.empty()) {
    int dart = stack.back();
    stack.pop_back();
    
    // if dart list's last element equals mate of this, it means we're coming back from the children of mate of this dart
    // unsuccessfully, remove mate of this dart from dart_list
    if (dart_list.back() == mate_[dart]) {
      dart_list.pop_back();
      continue;
    }
     
    //: else we're entering into a child's branch
    dart_list.push_back(dart);

    if (dart == d2) {
      break;
    }

    vcl_vector<int> tmp = children(dart);
    if (tmp.empty()) {
      dart_list.pop_back();
    } else {  // push its mate onto the stack as a sign to remove it if coming back
      stack.push_back(mate(dart));
      for (unsigned int i = 0; i<tmp.size(); i++)
        stack.push_back(tmp[i]);
    }
  }

  for (unsigned int i = 0; i<dart_list.size(); i++) {
    node_list.push_back(tail(dart_list[i]));
    node_list.push_back(head(dart_list[i]));
  }

  return node_list;
}




#define IS_EARLY(X) ((up_[next(X)] && !leaf_[X]) ? true : false )
#define IS_LATE(X) ((!up_[next(X)] && !leaf_[X]) ? true : false )

//------------------------------------------------------------------------------
//: order the subproblems of a rooted tree (up flags are assumed to be set wrt the root)
//  (this method is called for T1 in the algorithm)
vcl_vector<int> dbskr_directed_tree::
order_subproblems() 
{  
  vcl_vector<int> list;
  for (unsigned int i = 0; i<dart_cnt_; i++) 
    if (IS_EARLY(i))
      list.push_back(i);

  for (unsigned int i = 0; i<dart_cnt_; i++) 
    if (IS_LATE(i))
      list.push_back(i);

  return list;
}



//------------------------------------------------------------------------------
//: find a special order of subproblems of a tree with a fixed root 
//  (this method is called for T2 in the algorithm)
vcl_vector<int> dbskr_directed_tree::
find_special_darts(vcl_vector<int>& root_ch) 
{
  vcl_vector<int> list;
  helper(root_ch, list);
  return list;
}






//------------------------------------------------------------------------------
//:
void dbskr_directed_tree::
helper(const vcl_vector<int>& child_list, vcl_vector<int>& sofar) 
{  
  if (child_list.empty())
    return;
  int first = child_list[0];
  int start = mate_[first];
  surrogate_[start] = start;
  
  vcl_vector<int> rest;
  for (unsigned int i = 1; i<child_list.size(); i++)
    rest.push_back(child_list[i]);

  sofar.insert(sofar.begin(), start);
  // update sofar
  siblings(rest, sofar);
  left_path(start, children(first), sofar);
  return;
}


//------------------------------------------------------------------------------
//:
void dbskr_directed_tree::
left_path(int start, const vcl_vector<int> &current_list, 
          vcl_vector<int>& sofar) 
{
  if (current_list.empty())
    return;
  int first = current_list[0];
  surrogate_[mate_[first]] = start;

  vcl_vector<int> rest;
  for (unsigned int i = 1; i<current_list.size(); i++)
    rest.push_back(current_list[i]);

  // update sofar
  siblings(rest, sofar);
  left_path(start, children(first), sofar);
  return;
}


//------------------------------------------------------------------------------
//:
void dbskr_directed_tree::
siblings(const vcl_vector<int> &current_list, 
         vcl_vector<int>& sofar) 
{
  if (current_list.empty())
    return;
  int first = current_list[0];

  vcl_vector<int> rest;
  for (unsigned int i = 1; i<current_list.size(); i++)
    rest.push_back(current_list[i]);

  // update sofar
  siblings(rest, sofar);
  helper(children(first), sofar);
}







//-----------------------------------------------------------------------------
//: find the total splice cost of branches if this path's nodes are merged
float dbskr_directed_tree::
get_splice_cost_for_merge(int td, int d) 
{
  vcl_vector<int>& dart_path = get_dart_path(td, d);
  float cost = 0;
  for (unsigned int i = 0; i<dart_path.size()-1; i++) 
  {
    int dart = dart_path[i];
    int next = dart_path[i+1];
    vcl_vector<int>& ch = children_[dart];
    for (unsigned int j = 0; j<ch.size(); j++) 
    {
      if (ch[j] == next) continue;
      cost += subtree_delete_table_[ch[j]];
    }
  }
  return cost;
}




//------------------------------------------------------------------------------
//: find the total splice cost of branches if this path's nodes are merged
float dbskr_directed_tree::
get_splice_cost_for_merge(int td, int d, vcl_vector<bool>& used_darts) 
{
  vcl_vector<int>& dart_path = get_dart_path(td, d);
  for (unsigned int i = 0; i<dart_path.size(); i++) {
    used_darts[dart_path[i]] = true;
  }
  float cost = 0;
  for (unsigned int i = 0; i<dart_path.size()-1; i++) {
    int dart = dart_path[i];
    int next = dart_path[i+1];
    vcl_vector<int>& ch = children_[dart];
    for (unsigned int j = 0; j<ch.size(); j++) {
      if (ch[j] == next) continue;
      cost += subtree_delete_table_[ch[j]];
      used_darts[ch[j]] = true;
      //: push the darts in the subtree as well to the used darts
      vcl_vector<int> expand_list = children(ch[j]);
      while (expand_list.size() > 0) {
        int current = expand_list.back(); //expand_list.size()-1];
        used_darts[current] = true;
        expand_list.pop_back();
        vcl_vector<int>& tmp = children(current);
        for (unsigned int k = 0; k<tmp.size(); k++)
          expand_list.push_back(tmp[k]);
      }
    }
  }
  return cost;
}


//------------------------------------------------------------------------------
//: find the total splice cost of branches if this path's nodes are merged
float dbskr_directed_tree::
get_contract_cost(vcl_vector<bool>& used_darts) 
{
  float cost = 0;
  for (unsigned i = 0; i <dart_cnt_; i++) {
    if (!used_darts[i] && !used_darts[mate_[i]]) {
      if (leaf_[i]) {
        cost += delete_cost(i);
        used_darts[i] = true;
        used_darts[mate_[i]] = true;
      } else if (leaf_[mate_[i]]) {
        cost += delete_cost(mate_[i]);
        used_darts[i] = true;
        used_darts[mate_[i]] = true;
      } else {
        cost += contract_cost(i); 
        used_darts[i] = true;
        used_darts[mate_[i]] = true;
      }

    }
  }
  return cost;
}






//------------------------------------------------------------------------------
//: given two darts d1 and d2, find the unique path in the rooted tree from d1 to d2
//  return darts in this path
//  (if d2 is not a descendant of d1, return empty list)
//  if path is cached return directly
vcl_vector<int>& dbskr_directed_tree::
get_dart_path(int d1, int d2) 
{  
  if (dart_paths_[d1][d2].size() > 0) 
  {
    return dart_paths_[d1][d2];
  }

  //: find the darts 
  vcl_vector<int> dart_list;
  dart_list.push_back(d1);

  vcl_vector<int> stack = children(d1);
  while (!stack.empty()) 
  {
    int dart = stack.back();
    stack.pop_back();
    
    // if dart list's last element equals mate of this, it means we're coming back from the children of mate of this dart
    // unsuccessfully, remove mate of this dart from dart_list
    if (dart_list.back() == mate_[dart]) 
    {
      dart_list.pop_back();
      continue;
    }
     
    //: else we're entering into a child's branch
    dart_list.push_back(dart);

    if (dart == d2) 
    {
      break;
    }

    vcl_vector<int> tmp = children(dart);
    if (tmp.empty()) 
    {
      dart_list.pop_back();
    } else {  // push its mate onto the stack as a sign to remove it if coming back
      stack.push_back(mate(dart));
      for (unsigned int i = 0; i<tmp.size(); i++)
      {
        stack.push_back(tmp[i]);
      }
    }
  }

  if (d1 != d2 && dart_list.size() == 1)
  {
    dart_list.clear();
  }

  dart_paths_[d1][d2] = dart_list;
  return dart_paths_[d1][d2];
}









//------------------------------------------------------------------------------
//: get dart path from end nodes  (only used in table writing part for debugging!)
vcl_vector<int>& dbskr_directed_tree::
get_dart_path_from_nodes(int node1, int node2) 
{
  vcl_vector<int>& out_darts1 = out_darts(node1);
  vcl_vector<int> in_darts2;
  for (unsigned int i = 0; i<dart_cnt_; i++) 
  {
    if (head_[i] == node2)
      in_darts2.push_back(i);
  }

  for (unsigned int i = 0; i<out_darts1.size(); i++) 
  {
    for (unsigned int j = 0; j<in_darts2.size(); j++) 
    {
      vcl_vector<int>& tmp_path = get_dart_path(out_darts1[i], in_darts2[j]);
      if (tmp_path.size() > 0) 
      {
        return tmp_path;
        //path = tmp_path;
        //break;
      }
    }
  }

  return get_dart_path(out_darts1[0], in_darts2[0]);
}




//------------------------------------------------------------------------------
//: given a NODE return its out darts, i.e. the darts whose tail is this node
vcl_vector<int> dbskr_directed_tree::
find_out_darts(int node) 
{
  // find the first dart whose tail is node
  int first = -1;
  for (unsigned int i = 0; i<dart_cnt_; i++) 
    if (tail_[i] == node) {
      first = i;
      break;
    }

  vcl_vector<int> tmp;
  tmp.push_back(first);
  int prev_dart = mate_[prev(first)];
    
  while (prev_dart != first) {
    tmp.push_back(prev_dart);
    prev_dart = mate_[prev(prev_dart)];
  }  

  return tmp;
}




//------------------------------------------------------------------------------
//: find subtree delete costs, this function is called from acquire function.
//  also finds total_splice_cost_ of the tree
void dbskr_directed_tree::
find_subtree_delete_costs() 
{
  subtree_delete_table_.clear();
  total_splice_cost_ = 0;
  //TODO: make this function more efficient by ordering the darts and reusing delete costs

  //: find the subtree delete costs and fill subtree_delete_table_
  //  this is the splice cost of the branch starting with the given dart
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    float cost = delete_cost(i);
    vcl_vector<int> expand_list = children(i);
    while (expand_list.size() > 0) {
      int current = expand_list.back(); //expand_list.size()-1];
      cost += delete_cost(current);
      expand_list.pop_back();
      vcl_vector<int>& tmp = children(current);
      for (unsigned int j = 0; j<tmp.size(); j++)
        expand_list.push_back(tmp[j]);
    }
    subtree_delete_table_.push_back(cost);
  }

  // find the dart with largest subtree delete cost
  float max = subtree_delete_table_[0];
  unsigned dart_id = 0;
  for (unsigned int i = 1; i<dart_cnt_; i++) 
    if (max < subtree_delete_table_[i]) {
      max = subtree_delete_table_[i];
      dart_id = i;
    }
  
  //: Ozge: April 23, 07: correcting the bug in the following commented line, that is adding cost of dart with dart_id twice!!
  //total_splice_cost_ = max + subtree_delete_table_[mate_[dart_id]];
  total_splice_cost_ = max + subtree_delete_table_[mate_[dart_id]] - delete_cost(mate_[dart_id]);

#if 0
  for (unsigned int i = 0; i<dart_cnt_; i++) {
    vcl_cout << "subtreedeletecost[" << i << "]: " << subtree_delete_table_[i] << vcl_endl;
  }
  vcl_cout << "total_splice_cost: " << total_splice_cost_ << " from dart_id: " << dart_id << " mate: " << mate_[dart_id] << vcl_endl;
#endif
}

