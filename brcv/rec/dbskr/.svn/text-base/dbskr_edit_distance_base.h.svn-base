// This is brcv/rec/dbskr/dbskr_edit_distance_base.h

#ifndef dbskr_edit_distance_base_h_
#define dbskr_edit_distance_base_h_


//:
// \file
// \brief An attempt to generalize the dbskr_directed_tree_edit class originally written
//        by Ozge Ozcanli (ozge@lems.brown.edu)
//        The motivation for this generalization is to make the algorithm works
//        for other representation of shock graphs, e.g., dbsksp_xshock_graph.
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Nov 1, 2009
//
// \verbatim
//   Modifications
//
// \endverbatim




#include <dbskr/dbskr_directed_tree_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>

// first: node pair of tree1
// second: node pair of tree2
typedef vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > pathtable_key;


//: A class to compute the minimum edit distance between two shock graphs (trees)
class dbskr_edit_distance_base
{
public:

  //: Enum Root Node Selection
  enum RootNodeSelection
  {
      DEFAULT, // largest vs centroid
      LARGEST_RADIUS_BOTH, //Picks the largest radius for both
      CENTROID_BOTH 
  };

  //: Constructor 1
  dbskr_edit_distance_base() : tree1_(0), tree2_(0), 
      root_node_selection_(DEFAULT)
  {
    return_path_ = false;
    final_a1_ = -1;

    LARGE = 1e7;
    MIN_VALUE = 0.0f;
  }


  //: Destructor
  virtual ~dbskr_edit_distance_base() 
  { this->clear(); }

  //: Clear all internal variables
  void clear();

  //: set path table
  void set_path_table(vcl_map<pathtable_key, float>& pathtable) 
  { pathtable_ = pathtable; }
  
  //: set root pick for second shock graph to be first in tree
  void set_root_node_selection(RootNodeSelection scheme){
      root_node_selection_=scheme;}

  // UTILITIES------------------------------------------------------------------

  //: Main function: edit tree1 and tree2 so that their topologies are identical with the least cost possible
  bool edit();

  //: return cost
  float final_cost() { return final_cost_; }

  //: set the return cost flag, if not set default is false
  //  CAUTION: set this flag BEFORE calling edit() obviously!!
  void save_path(bool save) 
  { return_path_ = save; }
  
protected:

  //: find and cache the cost of matching curve pair induced by given dart paths
  virtual float get_cost(int td1, int d1, int td2, int d2) = 0;


  //:
  void solve_incomplete_subproblems(vcl_vector<int>& a1_list, vcl_vector<int>& a2_list);
  
  //: solve the problem such that a1 is the last dart in the ES of tree1
  //  and a2 is the last dart in ES of tree2 
  //  if complete is true then solving a complete subproblem otherwise solving an incomplete subproblem
  void phase(int a1, int a2, bool complete);
  
  //: version returning paths
  void phase_path(int a1, int a2, bool complete);

  //: calls basic and just sets A array and M and C arrays if necessary 
  void subphase(int a1, int a2, int d1, int d2);
  
  //: version returning paths
  void subphase_path(int a1, int a2, int d1, int d2);

  //: solve the problem for T1[d1] (left tree of T1 wrt d1) and T2[d2] (left tree of T2 wrt d2)
  //  such that a1 is the last dart in the ES of tree1
  //  and a2 is the last dart in ES of tree2 (d1 and d2 are any darts in the ES as we go backwards
  //  starting from a1 and a2)
  //  this is the version not returning paths
  float basic(int a1, int a2, int d1, int d2);
  
  //: version returning paths
  float basic_path(int a1, int a2, int d1, int d2);

  //: if next(d1) is down, there is a left branch of head(d1), try splicing it
  virtual float maybe_left_splice_out_tree1(int d1, int d2);
  
  //: version returning paths
  virtual float maybe_left_splice_out_tree1_vec(int d1, int d2);

  //: if next(d2) is down, there is a left branch of head(d2), try splicing it
  virtual float maybe_left_splice_out_tree2(int d1, int d2);
  
  //: version returning paths
  virtual float maybe_left_splice_out_tree2_vec(int d1, int d2);

  //: find the min deformation cost in matching branches starting with down pointing darts d1 and d2
  float witht(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  
  //: for speed do the operations without vectors first
  float witht(int a1, int a2, int d1, int d2, int td1, int td2);

  //: compute cost of matching path in T1 from td1 through d1 
  //                   against path in T2 from td2 through d2
  float match(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  
  //: for speed do the operations without vectors first
  float match(int a1, int a2, int d1, int d2, int td1, int td2);
  
  //:
  virtual float merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  
  //: for speed do the operations without vectors first
  virtual float merge_down_tree1(int a1, int a2, int d1, int d2, int td1, int td2);
  
  //:
  virtual float merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2, vcl_vector<pathtable_key>& vec);
  
  //: for speed do the operations without vectors first
  virtual float merge_down_tree2(int a1, int a2, int d1, int d2, int td1, int td2);

  //:
  virtual vcl_vector<pathtable_key>& maybe_right_splice_out_tree1_vec(int a1, int d1, int d2);
  
  //:
  virtual float maybe_right_splice_out_tree1(int a1, int d1, int d2);
  
  //:
  virtual vcl_vector<pathtable_key>& maybe_right_splice_out_tree2_vec(int a2, int d2, int d1);
  
  //:
  virtual float maybe_right_splice_out_tree2(int a2, int d2, int d1);

  //: initially do not save the paths even if user requires them, but then resolve the best case to 
  //  get the paths
  void solve_complete_subproblems();

  

  
  //: is less than function with a precision argument
  bool isL(float A, float B, float E) { return (((A)+E) < (B) ) ? true : false; }

  
  // Member variables-----------------------------------------------------------
protected:
  
  //: Shock tree 1
  dbskr_directed_tree_sptr tree1_;

  //: Shock tree 2
  dbskr_directed_tree_sptr tree2_;

  //: 
  vcl_map<pathtable_key, float> pathtable_;

  //: the vector to hold solutions of incomplete subproblems
  vcl_vector<vcl_vector<float> > M_;
  
  //: the vector to hold paths of solutions of incomplete subproblems
  vcl_vector<vcl_vector< vcl_vector<pathtable_key> > > MP_;
  
  //: the vector to hold paths solutions of complete subproblems
  vcl_vector<float> C_;
  vcl_vector<vcl_vector<pathtable_key> > CP_;
  
  //: the vector to hold solutions of front temporarily
  vcl_vector<vcl_vector<float> > A_;
  
  //: the vector to hold paths of solutions of front temporarily
  vcl_vector<vcl_vector< vcl_vector<pathtable_key> > > AP_;

  //: the map that holds the final mappings of dense shock curves 
  vcl_map<pathtable_key, vcl_vector < vcl_pair <int,int> > > shock_curve_map_;  

  int root2_;
  int size1_, size2_;

  //: keeping and saving paths takes a lot of time so keep and return them only if user needs
  //  each function has two versions, one which is saving the paths, one not saving
  bool return_path_;

  float final_cost_;
  int final_a1_;

  RootNodeSelection root_node_selection_;

  // Constants ////////////////
  
  float LARGE; // (1e7);
  float MIN_VALUE;   // (0.0f);
  
};

#endif // dbskr_edit_distance_base_h_
