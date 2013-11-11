// This is brcv/rec/dbskr/dbskr_directed_tree.h

#ifndef dbskr_directed_tree_h_
#define dbskr_directed_tree_h_

//:
// \file
// \brief A base class for ordered directed tree which is used by edit distance algorithm
//        Different representation of shock graphs will be sub-classed from this class
// \author Nhon Trinh
// \date Oct 31, 2009
// \verbatim
// Modifications
//   Nhon Trinh - Oct 31, 2009      Initial version.
//                                  This version extracts the core variables and functions of the dbskr_tree class
//                                  (written by Ozge Ozcanli and Amir Tamrakar) while leaving those dependent on
//                                  dbsk2d_shock_graph to a sub-class.
//                                  
// \endverbatim



// Unrooted Tree Edit Distance Algorithm will work with instances of this class

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_sc_pair_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_array_2d.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#include <assert.h>


//: A class to represent an attributed directed tree.
// Each dart (directed edge) is associated with various types of costs (delete,
// contract, etc). 
class dbskr_directed_tree : public vbl_ref_count 
{
  // Customized types-----------------------------------------------------------

public:
  //: Cost types at an edge 
  // first: contract cost of the edge
  // second: delete cost of the edge
  typedef vcl_pair<float, float> dbskr_edge_info;


  // Constructors / Destructors / Initializers----------------------------------

public:
  //: Constructor
  dbskr_directed_tree(): dart_cnt_(0), node_cnt_(0){};

  //: Copy constructor
  explicit dbskr_directed_tree(const dbskr_directed_tree& other);

  //: Destructor;
  virtual ~dbskr_directed_tree() {}

  //: Initialization - acquire tree from the list of nodes
  // "nodes" is a storage for the neighrboring nodes and costs of neighboring edges around each node
  // Each member of the outer vector corresponds to one node in the tree, arranged
  // in the same order as in "nodes_to_retain".
  // Each member of the inner vector corresponds to an edge incident at the node,
  // arranged counter-clockwise
  bool acquire(vcl_vector< vcl_vector<vcl_pair<int, dbskr_edge_info> > >& nodes);


  // Virtual functions----------------------------------------------------------

  //: find and cache the shock curve for this pair of darts, if not already cached
  virtual dbskr_scurve_sptr get_curve(int start_dart, int end_dart, bool construst_circular_ends) = 0;

  //: returns both the coarse and dense version of shock curve
  virtual dbskr_sc_pair_sptr get_curve_pair(int start_dart, int end_dart, bool construct_circular_ends) = 0;

  // Access graph variables ----------------------------------------------------
  
  //: Number of darts
  int size() { return dart_cnt_; }

  //: number of nodes
  int node_size() { return node_cnt_; }
  
  //: next dart
  inline int next(int dart) { return (dart+1)%dart_cnt_; }

  //: prev dart
  inline int prev(int dart) { return (dart-1+dart_cnt_)%dart_cnt_; }

  //: return tail node of input dart
  int tail(int dart) 
  { assert((unsigned)dart<dart_cnt_); return tail_[dart]; }

  //: return head node of input dart
  int head(int dart) 
  { assert((unsigned)dart<dart_cnt_); return head_[dart]; }

  //: return mate dart of input dart
  int mate(int dart) 
  { assert((unsigned)dart<dart_cnt_); return mate_[dart]; }

  //: return true if leaf dart
  bool leaf(int dart) 
  { assert((unsigned)dart<dart_cnt_); return leaf_[dart]; }

  //: return surrogate
  int surrogate(int dart) 
  { assert((unsigned)dart<dart_cnt_); return surrogate_[dart]; }

  //: return the up flag of a dart (initially they're all false)
  bool up(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return up_[dart]; 
  }

  //: return the parent dart
  //  given a dart pointing at parent node (assuming the dart is the leftmost child of parent),
  //  return downward parent dart
  int parent_dart(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return mate_[next(dart)]; 
  }


  //: Order of children of a dart is reverse of order in which the children appear in an Euler
  // string that omits the dart.  In other words, children appear left to right.
  vcl_vector<int>& children(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return children_[dart]; 
  }
  
  //: given a NODE return its out darts, i.e. the darts whose tail is this node
  vcl_vector<int>& out_darts(int node) 
  { 
    assert((unsigned)node<node_cnt_); 
    return out_darts_[node]; 
  }

  //: given a dart pointing down from parent node (assuming the dart is the rightmost child of parent)
  //  return upward parent dart
  int parent_dart_ccw(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return mate_[prev(dart)]; 
  }

  //: given a rightmost child dart pointing down, return left most child dart pointing up
  int skip_parent_dart_ccw(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return prev(parent_dart_ccw(dart)); 
  }



  // Access cost variables -----------------------------------------------------

  //: delete cost of the edge that dart represents
  float delete_cost(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return info_[dart].second; 
  }


  //: contract cost of the edge that dart represents
  float contract_cost(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return info_[dart].first; 
  }

  //: delete cost of the subtree that dart represents
  float subtree_delete_cost(int dart) 
  { 
    assert((unsigned)dart<dart_cnt_); 
    return subtree_delete_table_[dart]; 
  }


  // Graph Utilities-----------------------------------------------------------

  //: find the centroid NODE (which is the best candidate to be the root)
  int centroid();

  //: given a list of darts pointing down, set up flags for these darts and their mates, 
  //  and the subtrees they point to  
  void set_up(vcl_vector<int>& down_darts);


  //: given two darts d1 and d2, find the unique path in the rooted tree from d1 to d2
  //  return nodes in this path
  //  (if d2 is not a descendant of d1, return empty list)
  vcl_vector<int> find_node_path(int d1, int d2);

  //: order the subproblems of a rooted tree (up flags are assumed to be set wrt the root)
  //  (this method is called for T1 in the algorithm)
  vcl_vector<int> order_subproblems();

  //: find a special order of subproblems of a tree with a fixed root 
  //  (this method is called for T2 in the algorithm)
  // this method also fills in the surrogate vector
  vcl_vector<int> find_special_darts(vcl_vector<int>& root_ch);

  //: given two darts d1 and d2, find the unique path in the rooted tree from d1 to d2
  //  return darts in this path
  //  (if d2 is not a descendant of d1, return empty list)
  vcl_vector<int>& get_dart_path(int d1, int d2);

  //: get dart path from end nodes  (only used in table writing part for debugging!)
  vcl_vector<int>& get_dart_path_from_nodes(int node1, int node2);

  //: given a NODE find its out darts, i.e. the darts whose tail is this node
  vcl_vector<int> find_out_darts(int node);


  // Cost Utilities-------------------------------------------------------------

  //: find the splice cost of branches if this branche's nodes are merged
  float get_splice_cost_for_merge(int td, int d);

  //: return total splice cost
  float total_splice_cost() 
  { return total_splice_cost_; }


  //: find the total splice cost of branches if this path's nodes are merged
  float get_splice_cost_for_merge(int td, int d, vcl_vector<bool>& used_darts);

  //: get the total contract cost for the unused portions of the tree
  float get_contract_cost(vcl_vector<bool>& used_darts);


  // Support functions----------------------------------------------------------
protected:
  
  // Graph related //////////////////////////

  
  //:
  void helper(const vcl_vector<int>& child_list, vcl_vector<int>& sofar);

  //:
  void siblings(const vcl_vector<int>& current_list, vcl_vector<int>& sofar);

  //:
  void left_path(int start, const vcl_vector<int>& current_list, vcl_vector<int>& sofar);


  // Cost related ///////////////////////////////
  
  //: find subtree delete costs, this function is called from acquire function.
  //  also finds total_splice_cost_ of the tree
  void find_subtree_delete_costs();

  

  // Member variables-----------------------------------------------------------
protected:

  // Graph variables ///////////////////
  unsigned dart_cnt_;
  unsigned node_cnt_;
  vcl_vector<int> mate_;
  vcl_vector<int> head_;
  vcl_vector<int> tail_;
  vcl_vector<int> surrogate_;
  vcl_vector<bool> leaf_;
  vcl_vector<bool> up_;

  //: list of children for the darts
  vcl_vector<vcl_vector<int> > children_;

  //: cache out darts of each node
  vcl_vector<vcl_vector<int> > out_darts_;

  // Cost variables ///////////////////////

  //: A list describing various costs associated with each dart
  // For now, only two: contract and delete
  vcl_vector<dbskr_edge_info> info_;

  //:
  vcl_vector<float> subtree_delete_table_;

  
  //: total splice cost of the tree
  float total_splice_cost_;

  // Cache data /////////////////

  //: cache the unique path between dart pairs once it is computed
  vbl_array_2d<vcl_vector<int> > dart_paths_;
};

#endif // dbskr_directed_tree_h_
