//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_shock_patch_match.h
//:
// \file
// \brief Class to hold shock patch matches, Given two images, extract its patches
//                                           and match two patch sets
//                                           This class holds all the corresponding edit distance
//                                           matches in the second set sorted wrt 
//                                           some criteria, e.g. coarse edit distance, normalized edit distance cost
//            
//
// \author
//  O.C. Ozcanli - March 13, 2007
//
// \verbatim
//
//  Modifications
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_shock_patch_match_h
#define _dbskr_shock_patch_match_h

#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/vgl_polygon.h>

#include <vil/vil_image_view.h>

#include <dbskr/dbskr_shock_patch.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_sm_cor.h>

#include "dbskr_shock_patch_match_sptr.h"
#include <dbskr/algo/dbskr_shock_path_finder.h>

typedef vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* > patch_cor_map_type;
typedef vcl_map<int, vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* >::iterator patch_cor_map_iterator;

typedef vcl_map<int, vcl_vector<vcl_pair<int, float> >* > patch_cor_info_map_type;
typedef vcl_map<int, vcl_vector<vcl_pair<int, float> >* >::iterator patch_cor_info_map_iterator;

//: Shock Curve class
class dbskr_shock_patch_match : public vbl_ref_count 
{ 
public:
  
  //: default constructor
  dbskr_shock_patch_match() {}

  //: Interpolate/copy constructor
  //  Constructs and interpolated copy of an scurve
  //dbskr_shock_patch_match(dbskr_shock_patch_match const& old);

  //: Destructor
  ~dbskr_shock_patch_match() { clear(); }
  
  //: Assignment operator
  //dbskr_shock_patch_match& operator=(const dbskr_shock_patch_match &rhs);

  virtual void clear();
  void clear_edit_costs();
  void clear_sm_map_lists();

  patch_cor_map_type& get_map() { return patch_cor_map_; }
  vcl_map<int, dbskr_shock_patch_sptr>& get_id_map1() { return map1_; } 
  vcl_map<int, dbskr_shock_patch_sptr>& get_id_map2() { return map2_; } 
  void set_id_map1(vcl_map<int, dbskr_shock_patch_sptr>& map) { map1_ = map; } 
  void set_id_map2(vcl_map<int, dbskr_shock_patch_sptr>& map) { map2_ = map; } 

  void set_patch_set2(vcl_vector<dbskr_shock_patch_sptr>& patch_set) { patch_set2_ = patch_set; }
  vcl_vector<dbskr_shock_patch_sptr>& get_patch_set2(void) { return patch_set2_; }

  //: sort again wrt norm costs
  void resort_wrt_norm_cost();

  //: sort wrt costs
  void resort_wrt_cost();

  //: compute normalized fine costs of the corresponding branches
  bool compute_norm_fine_costs_of_cors();

  //: compute normalized costs of the corresponding branches
  //  normalize by the length of the total length of the reconstructed scurves used
  bool compute_length_norm_costs_of_cors();

  //: compute normalized costs of the corresponding branches
  //  normalize by the sum of total splice costs of the trees
  bool compute_splice_norm_costs_of_cors();

  //: compute normalized fine costs of the corresponding branches
  //  after finding the bug in total_splice_cost_ computation method of tree (in find_subtree_delete_cost() method)
  bool compute_just_norm_fine_costs_of_cors();

  //: get the best match of the patch with this id
  vcl_pair<int, dbskr_sm_cor_sptr>& get_best_match(int patch_id);

  //: get the top n best match of the patch with this id
  vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* get_best_n_match(int patch_id, int n); 

  //: change the norm costs in sm_cor's so that the value reflects the relative significance of this
  //  similarity value when the total area of the matching patches are concerned
  bool weight_norm_cost_wrt_area();

  //: construct the match for the second set of patches, does not sort wrt any type of cost
  dbskr_shock_patch_match_sptr construct_match_just_cost();

  //: construct the match for the second set of patches, replaces all matching maps with reverse maps and resorts
  dbskr_shock_patch_match_sptr construct_match();

  //: detect an instance of the category of the first storage in the second storage's image, implied by this match
  // use top N matches of the image to each model patch
  // put as a constraint that at least k model patches have top N matches less than threshold
  // In BMVC07 paper N = 3 and k = 2
  bool detect_instance(vsol_box_2d_sptr& detection_box, int N, int k, float threshold);

  //: detect an instance of the category of the first storage in the second storage's image, 
  //  implied by this match and the given input shock graphs
  //  find the best matching patch-quad such that p1 and p2 from storage 1 
  //  is most "consistent" with p3 and p4 from storage 2 as implied by the
  //  pathces that are formed in sg1 and sg2 using these 4 patches as anchor points
  //  put an upper threshold constraint so that not all match pairs are searched in sg1 and sg2
  //  if the best quad's similarity is less than threshold declare a detection
  //  this method requires at least two model patch to match to query patches
  //  constraint: p3 and p4 cannot be the same
  bool detect_instance(vsol_box_2d_sptr& detection_box, 
    dbskr_shock_path_finder& f1, 
    dbskr_shock_path_finder& f2, 
    float threshold, float upper_threshold, float interpolate_ds, float sample_ds, dbskr_tree_edit_params& edit_params, 
    bool normalize, float alpha,
    vil_image_resource_sptr img1 = 0, vil_image_resource_sptr img2 = 0, vcl_string out_img = "");

  //: detect an instance of the category of the first storage in the second storage's image, 
  //  implied by this match and the given input shock graphs
  //  find the best matching patch-quad such that p1 and p2 from storage 1 
  //  is most "consistent" with p3 and p4 from storage 2 as implied by the
  //  paths that are formed in sg1 and sg2 using these 4 patches as anchor points
  //  put an upper threshold constraint so that not all match pairs are searched in sg1 and sg2
  //  if the best quad's similarity is less than threshold declare a detection
  //  this method requires at least two model patch to match to query patches
  //  constraint: p3 and p4 cannot be the same
  bool detect_instance_using_paths(vsol_box_2d_sptr& detection_box, 
    dbskr_shock_path_finder& f1, 
    dbskr_shock_path_finder& f2, 
    float threshold, float upper_threshold, float interpolate_ds, float sample_ds, dbskr_tree_edit_params& edit_params, 
    bool normalize, bool use_approx_cost, bool impose_geom_consistency, float Lie_dist_threshold, float alpha,
    vil_image_resource_sptr img1 = 0, vil_image_resource_sptr img2 = 0, vcl_string out_img = "");

  void find_valid_pairs(float upper_threshold, vcl_vector<vcl_vector<float> >& valid, unsigned int& valid_cnt);
  bool find_geom_consistent_quads(vcl_vector<vcl_vector<float> >& valid, 
    vcl_map<vcl_pair<vcl_pair<unsigned, unsigned>, vcl_pair<unsigned, unsigned> >, bool>& cons_quads, 
    float upper_threshold, float Lie_dist_threshold);
  bool compute_similarity_transformations(int sampling_interval = 5);
  bool get_rotation_angle(vnl_matrix<double>& G, double& theta);
  bool get_rotation_angle(vgl_h_matrix_2d<double>& G, double& theta);
  bool get_rotation_angle_degree(vgl_h_matrix_2d<double>& G, double& theta);
  bool get_Lie_dist(vnl_matrix<double>& G1, vnl_matrix<double>& G2, double& dist);
  bool get_Lie_dist_to_identity(vgl_h_matrix_2d<double>& G, double& dist);
  bool get_Lie_dist_to_identity_only_rotation(vgl_h_matrix_2d<double>& G, double& dist);
  bool clear_similarity_transformations();

  //: create a rank ordered vector of "other" model patches and their matching query patches wrt to 
  //  their similarity transformation's similarity to the given pairs transformation
  //  the distance between the transformations is computed via Lie Algebra and the Lie Distance
  //  (similarity transformations form a Lie Group, hence we can find distances between them in the Lie Space)
  //  vcl_vector<vcl_pair< vcl_pair<int, int>, double > > vector of < <mod_id, q_id>, distance to the given model_id, query_id >
  //bool rank_order_other_patch_pairs_wrt_sim_trans_lie(int model_id, int query_id, 
  //              vcl_vector<vcl_pair< vcl_pair<int, int>, double > >& out_vec, float sim_threshold);

  //bool rank_order_other_patch_pairs_wrt_sim_trans(int model_id, int query_id, 
  //              vcl_vector<vcl_pair< vcl_pair<int, int>, double > >& out_vec, float sim_threshold);

  bool rank_order_other_patch_pairs_wrt_sim_trans(int model_id, int query_id, 
                vcl_vector<vcl_pair< vcl_pair<int, int>, vcl_pair<double, vnl_matrix<double> > > >& out_vec, float sim_threshold);

  //: detect an instance of the category of the first storage in the second storage's image, implied by this match
  //  use the consistency similarity transformation's of patch pairs
  //  algorithm explained in Kimia's NSF december proposal 2007
  bool detect_instance_wrt_trans(vcl_vector<vsol_box_2d_sptr>& detection_box, int k, float trans_threshold);

  //: mutual info methods
  //bool compute_mutual_infos();

  //: sort again wrt infos
  void resort_wrt_info();

  //: get the best match of the patch with this id
  vcl_pair<int, float>& get_best_match_info(int patch_id);

  patch_cor_info_map_type& get_info_map() { return patch_cor_info_map_; }

  bool create_match_ps_images(vcl_string image_dir, vcl_string name1, dbsk2d_shock_graph_sptr base_sg1, vcl_string name2, dbsk2d_shock_graph_sptr base_sg2);
  bool create_html_table(vcl_string image_dir, vcl_string name1, vcl_string name2, vcl_string out_html, vcl_string table_caption, vcl_string image_ext, int image_width, bool put_match_images);

  bool create_html_rank_order_table(vcl_string image_dir, 
                                    vcl_string name1, 
                                    vcl_string name2, 
                                    vcl_string out_html, 
                                    vcl_string table_caption, 
                                    vcl_string image_ext, 
                                    bool put_match_images);


  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 2;}  // version 2: added dbskr_tree_edit_params

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbskr_shock_patch_match";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);
  
public:
  
  //----------------------------------------
  // Shock Patch Match Data
  //----------------------------------------

  //: patch correspondence map, the vector in this map is always sorted wrt some criteria
  patch_cor_map_type patch_cor_map_;

  //: keep the id to sptr map if available
  vcl_map<int, dbskr_shock_patch_sptr> map1_;
  vcl_map<int, dbskr_shock_patch_sptr> map2_;

  vcl_vector<dbskr_shock_patch_sptr> patch_set2_;

  //: required for visualization
  dbskr_shock_patch_sptr left_, right_;
  dbskr_sm_cor_sptr cor_;
  vcl_vector<vcl_pair<int, dbskr_sm_cor_sptr> >* left_v_;
  int current_left_v_id_;

  float shock_pruning_threshold_;
  //float scurve_sample_ds_;
  //bool elastic_splice_cost_;

  dbskr_tree_edit_params edit_params_;  // all the patches matched via this class will use the same tree edit param set
                                        // and this will be transferred to dbskr_sm_cor via dbskr_tree_edit
  
  //: mutual info results
  patch_cor_info_map_type patch_cor_info_map_;

};

#endif  // _dbskr_shock_patch_match_h
