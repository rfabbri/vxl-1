// This is brcv/rec/dbskr/dbskr_edit_distance_shock_tree.h

#ifndef dbskr_edit_distance_shock_tree_h_
#define dbskr_edit_distance_shock_tree_h_


//:
// \file
// \brief A class to compute edit distance between two shock trees
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Oct 30, 2009
//
// \verbatim
//   Modifications
//
// \endverbatim



#include <dbskr/dbskr_edit_distance_base.h>
#include <dbskr/dbskr_shock_tree_sptr.h>


//: A class to compute the minimum edit distance between two shock graphs (trees)
class dbskr_edit_distance_shock_tree : public dbskr_edit_distance_base
{
public:

  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbskr_edit_distance_shock_tree(const dbskr_shock_tree_sptr& tree1, 
    const dbskr_shock_tree_sptr& tree2, 
    bool construct_circular_ends = true, bool localize_match = false);

  //: Destructor
  virtual ~dbskr_edit_distance_shock_tree() {}

  // Data access----------------------------------------------------------------

  //: Set tree 1
  void set_tree1(const dbskr_shock_tree_sptr& tree1);
  
  //: Get tree1
  dbskr_shock_tree_sptr tree1() const;

  //: Set tree 2
  void set_tree2(const dbskr_shock_tree_sptr& tree2);

  //: Get tree2
  dbskr_shock_tree_sptr tree2() const;

  //: Set circular ends
  void set_circular_ends(bool val) 
  { construct_circular_ends_ = val; }

  //: Return true if circular ends is used
  bool get_circular_ends(void) 
  { return construct_circular_ends_; }

  //: Return true if localize_match is used
  bool get_localize_match()
  { return this->localize_match_; }


  //: Set parameter R for curve matching
  void set_curvematching_R(double R) 
  { curve_matching_R_ = R; }

  //: Return parameter R for curve matching
  double get_curvematching_R(void) 
  { return curve_matching_R_; }



  

  // UTILITIES------------------------------------------------------------------

  //: return the corresponding shock curves and their maps
  virtual bool get_correspondence_just_map_helper(vcl_vector<vcl_vector <vcl_pair<int,int> > >& map_list,
                                   vcl_vector< pathtable_key >& path_map); 


  //: return the corresponding shock curves and their maps
  virtual bool get_correspondence(vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                          vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                          vcl_vector< vcl_vector < vcl_pair <int,int> > >& map_list,
                          vcl_vector< pathtable_key >& path_map); 

  //: Return correspondence between two trees
  virtual dbskr_sm_cor_sptr get_correspondence(bool save_pathtable = false);


  //: Return correspondence between the two shock trees
  virtual dbskr_sm_cor_sptr get_correspondence_just_map();


  //: read path table
  bool read_data(vcl_string treefile1, vcl_string treefile2, 
    vcl_string pathtable_file, bool elastic_splice_cost);


  //: write shgm output file
  bool write_shgm(vcl_string shgm_filename);

  //: compare path 
  bool compare_path(vcl_string shgm_filename, double precision);

  //: compare costs 
  bool compare_cost(vcl_string shgm_filename);

  //: create and write table file in advance (for debugging purposes!!)
  bool populate_table(vcl_string fname);

protected:

  //: find and cache the cost of matching curve pair induced by given dart paths
  virtual float get_cost(int td1, int d1, int td2, int d2);


  
  // Member variables-----------------------------------------------------------
protected:

  //: flag - whether to match with circular completions at the scurve ends
  bool construct_circular_ends_;  

  //: flag 
  bool localize_match_;

  //: parameter R in the deformation cost
  double curve_matching_R_;

  //: Flag to indicate the order of the two input trees has been switched
  bool switched_;

  //////////////////////

  //:
  float LARGE_COST_RATIO; //          (0.3)
  
  //:
  float LARGE_BOUNDARY_COST; //       (150)
  
  //:
  float VERY_LARGE_BOUNDARY_COST; //  (300)

  
};

#endif // dbskr_edit_distance_shock_tree_h_
