// This is brcv/rec/dbskr/dbskr_tree_edit.h

#ifndef dbskr_tree_edit_h_
#define dbskr_tree_edit_h_


//:
// \file
// \brief The unrooted tree edit distance algorithm class  
//
// \author
//  O.C. Ozcanli - August 05, 2005
//
// \verbatim
//   Modifications
//   O.C. Ozcanli Dec 09, 2005, modified to  both minimization or maximization
//                              of some cost function
//
//  Ozge C Ozcanli March 29, 07          Removed the scurve pair class and all related functionality
//                                       The dense one is almost always the same as the coarse one
//                                       and the dense one is never used
//                                       In the original shock edit code "localized matching" cost
//                                       was used, its a speed up which get the correpondence from
//                                       coarse scurve matching, then computes a finer cost using this correspondence
//                                       on a densely sampled scurve. This portion was removed by Amir Tamrakar & Ozge Can Ozcanli
//                                       due to insignificant contribution (when the sampling in coarse curve
//                                       is almost always the same as the dense one, the contribution is zero).
//
//  Ozge C Ozcanli April 21, 2007        added construct_circular_ends_ flag, if true the leaf scurves are constructed with circular completions
//                                       in normal edit distance algorithm (Sebastian, Klein, Kimia PAMI) all the scurves are supposed
//                                       to be constructed with circular completions, since the algorithm's basic assumption is that
//                                       simpled closed curves are being matched to each other, hence leaves should be closed
//                                       however in some applications this assumption is no longer valid
//                                       for instance in "Shock Patches" (Ozcanli, Kimia, BMVC07) the real boundaries being matched are open.
//                                       In such applications all the scurves are constructed as open.
//                                       its users responsibility to set the flag accordingly, the default is true
//
//  Ozge C Ozcanli October 24, 2007      Added curve matching as a parameter, default value is 6.0f as in dbskr_dpmatch default
//
//  Ozge C Ozcanli October 30, 2007      Added localize_match_ option, default is false
//
//  Ozge C Ozcanli Nov 12, 2007          made adjustments to use dbskr_tree_edit_params while setting dbskr_sm_cor
//                                       WARNING: assumes both tree1 and tree2 are constructed with the same
//                                                scurve_sample_ds, scurve_interpolate_ds and elastic_splice_cost parameters
//                                                and sets the parameters in dbskr_sm_cor using the construction params of tree1
//
//  Nhon Trinh   - Oct 31, 2009         Separate the class into two layers - the parent layer (dbskr_edit_distance_base)
//                                      is purely an edit-distance algorithm. There is dependency on shock graphs.
//                                      This child layer (this file) works specifically with shock tree (dbskr_tree).
//
//
//  Anil Usumezbas Dec 03, 2009         Added use_approx parameter and related member functions to be able to turn off the
//                                      cost approximation step that seems to be hurting our performance for some cases
//
// \endverbatim
//


#include <dbskr/dbskr_edit_distance_base.h>
#include <dbskr/dbskr_tree_sptr.h>


//: A class to compute the minimum edit distance between two shock graphs (trees)
class dbskr_tree_edit : public dbskr_edit_distance_base
{
public:

  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbskr_tree_edit(const dbskr_tree_sptr& tree1, 
    const dbskr_tree_sptr& tree2, 
    bool construct_circular_ends = true, bool localize_match = false);

  //: Constructor
  dbskr_tree_edit(const dbskr_directed_tree_sptr& tree1, 
    const dbskr_directed_tree_sptr& tree2, 
    bool construct_circular_ends = true, bool localize_match = false);

  //: Destructor
  virtual ~dbskr_tree_edit() {}

  // Data access----------------------------------------------------------------

  //: Set tree 1
  void set_tree1(const dbskr_tree_sptr& tree1);
 
  //: Set tree 1
  void set_tree1(const dbskr_directed_tree_sptr& tree1){tree1_=tree1.ptr();}
  
  //: Get tree1
  dbskr_tree_sptr tree1() const;

  //: Set tree 2
  void set_tree2(const dbskr_tree_sptr& tree2);

  //: Set tree 2
  void set_tree2(const dbskr_directed_tree_sptr& tree2){tree2_=tree2.ptr();}

  //: Get tree2
  dbskr_tree_sptr tree2() const;

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

  //: Set use_approx
  void set_use_approx(bool val)
  { use_approx_ = val;}

  //: Return true if approximation of the cost is used
  bool get_use_approx(void)
  { return use_approx_; }  

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

  //: Flag to determine whether the cost approximation will be made use of
  bool use_approx_;

  //////////////////////

  //:
  float LARGE_COST_RATIO; //          (0.3)
  
  //:
  float LARGE_BOUNDARY_COST; //       (150)
  
  //:
  float VERY_LARGE_BOUNDARY_COST; //  (300)

  
};

#endif // dbskr_tree_edit_h_
