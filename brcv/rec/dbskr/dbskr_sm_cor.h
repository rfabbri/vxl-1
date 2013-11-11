// This is brcv/rec/dbskr/dbskr_sm_cor.h
#ifndef dbskr_sm_cor_h_
#define dbskr_sm_cor_h_
//:
// \file
// \brief A class to store shock match correspondence data structures
// \author Ozge Can Ozcanli
// \date Nov 28 2005
//
// \verbatim
//  Modifications
//
//  Ozge C Ozcanli March 29, 07          Removed the scurve pair class and all related functionality
//                                       The dense one is almost always same as the coarse one
//                                       and the dense one is never used
//
//
// \endverbatim

//#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_tree.h>
#include <vbl/vbl_ref_count.h>
#include <dbskr/dbskr_sm_cor_sptr.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <dborl/dborl_algo_params_base.h>

typedef vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > pathtable_key;

class dbskr_tree_edit_params
{
public:

  dbskr_tree_edit_params() : circular_ends_(true), combined_edit_(false), localized_edit_(false), curve_matching_R_(6.0),
                             scurve_sample_ds_(1.0f), scurve_interpolate_ds_(1.0f), elastic_splice_cost_(false), 
                             coarse_edit_(false) {}

  float scurve_sample_ds_;  // Sampling ds to reconstruct the scurve, def: 1.0
  float scurve_interpolate_ds_;  // Interpolation ds to get densely interpolated versions of the scurves, def: 1.0 --> meaningful if localized_edit option is ON
  bool localized_edit_; // improve elastic matching cost of scurves using the densely interpolated versions, def: false
  bool elastic_splice_cost_; // use the elastic splice cost computation, def: false
  //: the defualt for this R parameter is 6.0f in original Sebastian implementation of shock matching
  double curve_matching_R_; // curve matching R that weighs bending over stretching, default 6.0f
  
  //: the following option is only false when shock fragments are being matched, 
  //  to match shock graphs of simpled closed contours, i.e. shock trees, this option should always be TRUE
  bool circular_ends_; // use circular completions at leaf branches during shock matching, def: true 

  bool combined_edit_; // use combined edit cost during shock matching, def: false
  bool coarse_edit_; // run coarse edit distance algorithm, def: false

  vcl_string output_file_postfix_;

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbskr_sm_cor";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);
};



//: Class to hold shock matching data structures
class dbskr_sm_cor : public vbl_ref_count
{
public:

  //: Constructor
  dbskr_sm_cor();
  dbskr_sm_cor(dbskr_tree_sptr t1, dbskr_tree_sptr t2);

  //: Destructor
  virtual ~dbskr_sm_cor();

  //: return a new instance with all the maps reversed
  dbskr_sm_cor_sptr new_sm_reverse_maps();

  //: get tree1
  dbskr_tree_sptr get_tree1() { return tree1_; }

  //: get tree2
  dbskr_tree_sptr get_tree2() { return tree2_; }

  //: set tree1
  void set_tree1( dbskr_tree_sptr tp ) { tree1_ = tp;  recover_dart_ids_and_scurves(); }

  //: set tree2
  void set_tree2( dbskr_tree_sptr tp ) { tree2_ = tp;  recover_dart_ids_and_scurves(); }

  //: get the coarse shock graph1
  dbsk2d_shock_graph_sptr get_shock_graph1() { return tree1_->get_shock_graph(); }

  //: get the coarse shock graph2
  dbsk2d_shock_graph_sptr get_shock_graph2() { return tree2_->get_shock_graph(); }

  //: get the tree path correspondence map
  vcl_vector<pathtable_key>& get_map() { return dart_path_map_; }

  //: set the tree path correspondence map
  void set_map(vcl_vector<pathtable_key>& map) { dart_path_map_ = map; }

  //: set curve list 1
  void set_curve_list1(vcl_vector<dbskr_scurve_sptr>& cl) { curve_list1_ = cl; }

  //: set curve list 2
  void set_curve_list2(vcl_vector<dbskr_scurve_sptr>& cl) { curve_list2_ = cl; }

  //: get curve list 1
  vcl_vector<dbskr_scurve_sptr>& get_curve_list1(void) { return curve_list1_; }

  //: get curve list 2
  vcl_vector<dbskr_scurve_sptr>& get_curve_list2(void) { return curve_list2_; }

  //: set map list
  void set_map_list(vcl_vector<vcl_vector < vcl_pair <int,int> > >& ml) { map_list_ = ml; }

  //: get map list
  vcl_vector<vcl_vector < vcl_pair <int,int> > >& get_map_list(void) { return map_list_; }

  void add_to_curve_list1(dbskr_scurve_sptr sc) { curve_list1_.push_back(sc); }
  void add_to_curve_list2(dbskr_scurve_sptr sc) { curve_list2_.push_back(sc); }
  void add_to_map_list( vcl_vector< vcl_pair <int, int> >& m) { map_list_.push_back(m); }
  void clear_lists(void) { curve_list1_.clear(); curve_list2_.clear(); map_list_.clear(); }

  //: save pathtable
  void save_pathtable(vcl_map<pathtable_key, float>& table);

  //: get pathtable
  vcl_map<pathtable_key, float>& get_pathtable(void) { return pathtable_; }

  //: query into the table
  //  CAUTION: the key should be formed by NODE IDS (not dart ids)
  float get_pathtable_val(pathtable_key key);

  float get_cost(unsigned int i) { if (i < match_costs_.size()) return match_costs_[i]; else return -1; } 
  float get_cost_d(unsigned int i) { if (i < match_costs_d_.size()) return match_costs_d_[i]; else return -1; } 
  float get_cost_localized(unsigned int i) { if (i < match_costs_localized_.size()) return match_costs_localized_[i]; else return -1; } 
  float get_cost_init_dr(unsigned int i) { if (i < match_costs_init_dr_.size()) return match_costs_init_dr_[i]; else return -1; } 
  float get_cost_init_alp(unsigned int i) { if (i < match_costs_init_alp_.size()) return match_costs_init_alp_[i]; else return -1; } 

  void clear() { dart_path_map_.clear(); }
  void clear_map_list() { map_list_.clear(); }

  //void write_shgm(double matching_cost, vcl_string fname);
  void write_shgm(vcl_string fname);
  //: WARNING: curve_matching_R should be called before read_and_construct_from_shgm() if it is different from the default value
  float read_and_construct_from_shgm(vcl_string fname, bool recover_dart_ids_and_scurves = true);
  bool recover_dart_ids_and_scurves();

  void set_final_cost(float cost) { final_cost_ = cost; }
  float final_cost(void) { return final_cost_; }

  void set_final_norm_cost(float cost) { final_norm_cost_ = cost; }
  float final_norm_cost(void) { return final_norm_cost_; }

  //: compute the homography induced by this tree correspondence
  //  if use_shock_points = true --> uses shock branch points for correspondence
  //  otherwise uses plus and minus reconstructed boundary points
  bool compute_homography(vgl_h_matrix_2d<double>& H, bool tree1_to_tree2, unsigned sampling_interval, bool use_shock_points = false, bool use_ransac = false);

  //: compute the similarity transformation induced by this tree correspondence
  //  if use_shock_points = true --> uses shock branch points for correspondence
  //  otherwise uses plus and minus reconstructed boundary points
  bool compute_similarity2D(vgl_h_matrix_2d<double>& H, bool tree1_to_tree2, unsigned sampling_interval, bool use_shock_points = false, bool save_trans = false);
  void clear_transformation() { trans_matrix_.clear(); }

  //void set_curvematching_R(double R) { curve_matching_R_ = R; }
  //double get_curvematching_R(void) { return curve_matching_R_; }
  void set_tree_edit_params(dbskr_tree_edit_params edit_params) { edit_params_ = edit_params; }
  void set_tree_edit_params(float scurve_sample_ds, 
                            float scurve_interpolate_ds, 
                            bool localized_edit, 
                            bool elastic_splice_cost,
                            bool circular_ends,
                            bool combined_edit, 
                            bool coarse_edit,
                            double curve_matching_R);

  bool get_similarity_trans(vgl_h_matrix_2d<double>& H); 
  vnl_matrix<double>& get_similarity_trans() { return trans_matrix_; }

                           

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 4;}  // version 2: added final_norm_cost_
                                                // version 3: added fine_final_norm_costs_computed_
                                                // version 4: added dbskr_tree_edit_params

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbskr_sm_cor";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  void kill_trees() { if (tree1_) tree1_ = 0; if (tree2_) tree2_ = 0; }

private:

  dbskr_tree_sptr tree1_;
  dbskr_tree_sptr tree2_;
  
  bool dart_path_map_has_node_ids_;
  vcl_vector<pathtable_key> dart_path_map_;  // the vector which holds the mapping between tree dart paths

  bool pathtable_saved_;
  vcl_map<pathtable_key, float> pathtable_;

  vcl_vector<dbskr_scurve_sptr> curve_list1_;
  vcl_vector<dbskr_scurve_sptr> curve_list2_;

  //: this vector holds the correspondence map of each shock curve pair in curve_list1 and curve_list2
  vcl_vector<vcl_vector < vcl_pair <int,int> > > map_list_;

  //: save the matching costs used in edit distance for final corresponding branches
  vcl_vector<float> match_costs_;

  // these will only be available if reading from file
  vcl_vector<float> match_costs_d_;
  vcl_vector<float> match_costs_localized_;
  vcl_vector<float> match_costs_init_dr_;
  vcl_vector<float> match_costs_init_alp_;

  float final_cost_, final_norm_cost_;

  //double curve_matching_R_;

public:
  vnl_matrix<double> trans_matrix_;

  dbskr_tree_edit_params edit_params_;  

  //bool use_dpmatch_combined_;
  //bool contstruct_circular_ends_;
  bool fine_final_norm_costs_computed_;
  //bool localize_match_;
};

#endif //dbskr_sm_cor_h_

