// This is brcv/rec/dbskr/algo/dbskr_rec_algs.h
#ifndef dbskr_rec_algs_h_
#define dbskr_rec_algs_h_
//:
// \file
// \brief Shock graph and tree algorithms used in recognition
// \author Ozge C. Ozcanli
//
// \verbatim
//  Modifications
//   O.C.Ozcanli  02/20/2007   
//
// \endverbatim 

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>

#include <dbskr/dbskr_scurve_sptr.h>
#include <dbskr/dbskr_shock_patch_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match.h>
#include <dbskr/algo/dbskr_shock_patch_curve_match.h>
#include <dbskr/algo/dbskr_shock_patch_curve_match_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>

#include <dbskr/dbskr_v_graph_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>

//: construct a virtual graph which will be Euler toured to get a trace of its outer boundary
//  v_graph is the sub shock graph where degree 2 nodes are merged
dbskr_v_graph_sptr construct_v_graph(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr node, int depth);

//: construct v_graph of the whole shock graph, starting from any of its nodes
dbskr_v_graph_sptr construct_v_graph(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr node);

//: min_full_depth of a shock graph is the min depth from a node at which v_graph covers the full shock tree
//  return the node with min full depth
int find_min_full_depth(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr& node);

//: for debugging
void print_v_graph(dbskr_v_graph_sptr v_g);
void print_shock_graph(dbsk2d_shock_graph_sptr v_g);

//: makes a wrong assignment if not in the graph
//  check the id of the returned pointer if not sure that a node with a given id exists in g
dbsk2d_shock_node_sptr& get_node_sptr(dbsk2d_shock_graph_sptr g, int id);

//: makes a wrong assignment if not in the graph
//  check the id of the returned pointer if not sure that a node with a given id exists in g
dbskr_v_node_sptr& get_node_sptr(dbskr_v_graph_sptr g, int id);

void get_node_map(vcl_map<int, dbsk2d_shock_node_sptr>& map, dbsk2d_shock_graph_sptr g);
void get_node_map(vcl_map<int, dbskr_v_node_sptr>& map, dbskr_v_graph_sptr g);

//: method that traverses a shock graph starting from a given node in a breadth first manner
//  visiting each edge (degree two nodes are merged) only once and saving the edges
//  visited_node_depth_map is used to visit each node once
//  node_edges_map keeps starting node of a vector of shock edges (degree twos are merged)
void visit_and_keep_edges(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr start_node, 
                             vcl_map<dbsk2d_shock_node_sptr, int>& visited_node_depth_map,
                             vcl_map<vcl_pair<dbsk2d_shock_node_sptr, dbsk2d_shock_node_sptr>, vcl_vector<dbsk2d_shock_edge_sptr> >& node_edges_map,
                             vcl_vector<vcl_pair<dbsk2d_shock_node_sptr, int> >& to_visit,
                             int depth = 1);

void get_scurves(dbsk2d_shock_graph_sptr sg, dbsk2d_shock_node_sptr start_node, 
                 vcl_vector<dbskr_scurve_sptr>& cur_scurves, 
                 vcl_vector<bool>& end_scurve, int depth, bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds);

vsol_polygon_2d_sptr trace_boundary_from_subgraph(dbsk2d_shock_graph_sptr sg, 
                                                  dbsk2d_shock_node_sptr start_node, 
                                                  int depth, bool circular_completions,
                                                  bool binterpolate, bool subsample, double interpolate_ds, double subsample_ds, double area_threshold);

//: required for visualization purposes
vsol_polygon_2d_sptr trace_boundary_from_graph(dbsk2d_shock_graph_sptr sg, 
                                                  bool binterpolate, bool subsample, 
                                                  double interpolate_ds, double subsample_ds, double poly_area_threshold);

//: the following methods is doind exact same thing as trace_boundary_from_subgraph, but creating a patch instance
dbskr_shock_patch_sptr extract_patch_from_subgraph(dbsk2d_shock_graph_sptr sg, 
                                                  dbsk2d_shock_node_sptr start_node, 
                                                  int depth, double area_threshold, bool circular_ends = false,
                                                  bool binterpolate = true, bool subsample = 1.0f, double interpolate_ds = 1.0f, double subsample_ds = 1.0f);

void get_edges_on_outer_face(dbskr_v_graph_sptr vg, vcl_vector<dbskr_v_edge_sptr> &edges);

//: the following methods is doind exact same thing as trace_boundary_from_subgraph, but creating a patch instance
dbskr_shock_patch_sptr extract_patch_from_v_graph(dbskr_v_graph_sptr v, int id, int depth,
                                                  double area_threshold, bool circular_ends = false,
                                                  bool binterpolate = true, bool subsample = 1.0f, double interpolate_ds = 1.0f, double subsample_ds = 1.0f);

//: the following methods is doind exact same thing as extract_patch_from_v_graph
//  but the input v graph may or may not have an edge with a degree 1 target to start the euler tour.
//  so it adds a dummy edge to the graph from the s
//dbskr_shock_patch_sptr extract_patch_from_v_graph_special(dbskr_v_graph_sptr v, int id, int depth,
//                                                  double area_threshold, bool circular_ends = false,
//                                                  bool binterpolate = true, bool subsample = 1.0f, double interpolate_ds = 1.0f, double subsample_ds = 1.0f);


//: set the matching parameters like shock_pruning_threshold, scurve_sample_ds and bool elastic_splice_cost ahead of time for each patch
bool find_patch_correspondences(dbskr_shock_patch_sptr s1, 
                                vcl_vector<dbskr_shock_patch_sptr>& s2, 
                                patch_cor_map_type& match_map, dbskr_tree_edit_params& edit_params);

//: set the matching parameters like shock_pruning_threshold, scurve_sample_ds and bool elastic_splice_cost ahead of time for each patch
dbskr_shock_patch_match_sptr find_all_patch_correspondences(vcl_vector<dbskr_shock_patch_sptr>& s1, 
                                                            vcl_vector<dbskr_shock_patch_sptr>& s2, dbskr_tree_edit_params& edit_params);

//: set the matching parameters like shock_pruning_threshold, scurve_sample_ds and bool elastic_splice_cost ahead of time for each patch
bool find_patch_correspondences_coarse_edit(dbskr_shock_patch_sptr s1, 
                                vcl_vector<dbskr_shock_patch_sptr>& s2, 
                                patch_cor_map_type& match_map, dbskr_tree_edit_params& edit_params);

//: find the corresondences using elastic curve matching between sets of real contours of patches
bool find_patch_correspondences_curve(dbskr_shock_patch_sptr s1, 
                                vcl_vector<dbskr_shock_patch_sptr>& s2, 
                                vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >& match_map, int n);

//: given an image (patch set) find its best match among a set of matches, votes are weighted by similarity measure
bool best_match_norm(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_vector<dbskr_shock_patch_match_sptr>& mv, int& best_id);

//: given an image (patch set) find its best match among a set of matches
//  use top n best patch matches between the patch set matches of two images
bool best_match_norm_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_vector<dbskr_shock_patch_match_sptr>& mv, int n, int& best_id);


bool match_strat1_simple_voting_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, 
                                      vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> >& mv, 
                                      int& best_cat_id, int n, int visualization_n, 
                                      vcl_vector<vcl_vector<vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > >& best_category_instance_ids);

bool create_html_top_n(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_string pv_patch_images_dir,
                       vcl_vector<vcl_vector<vcl_pair< vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> >, float > > >& best_category_instance_ids,
                       vcl_vector<vcl_vector<vcl_string> > ins_names, 
                       int visualization_n, vcl_string file_name, 
                       vcl_string table_caption, 
                       vcl_vector<vcl_vector<vcl_string> > patch_image_dirs);

// WARNING: for the trees: assuming tree parameters are already set properly depending on the match parameters
bool create_html_top_n_placements(dbskr_shock_patch_match_sptr new_match, 
                                  vil_image_resource_sptr img_test, 
                                  vil_image_resource_sptr img_model, 
                                  vcl_string model_patch_images_dir,
                                  int N, vcl_string out_html_images_dir, vcl_string out_html, vcl_string table_caption);


//: given an image (patch set), for each patch find its best patch and vote for the category of the best match
bool match_strat1_simple_voting(vcl_vector<dbskr_shock_patch_sptr>& pv, vcl_vector<vcl_vector<dbskr_shock_patch_match_sptr> >& mv, int& best_id, bool use_info = false);

//: create a white ps image, draw a base shock as black, draw a second shock graph with branches colored wrt a given dbskr_sm_cor
bool create_ps_shock_matching(vcl_string ps_file_name, dbsk2d_shock_graph_sptr base_sg1, dbskr_tree_sptr main_tree1, dbsk2d_shock_graph_sptr base_sg2, dbskr_tree_sptr main_tree2, dbskr_sm_cor_sptr sm);

bool create_ps_shock(vcl_string ps_file_name, dbsk2d_shock_graph_sptr base_sg1);

//: create image with traced boundaries
//: put white background of the img pointer is zero
bool create_ps_patches(vcl_string ps_file_name, vcl_vector<dbskr_shock_patch_sptr>& patches, vcl_vector<vil_rgb<int> >& colors, 
                       vil_image_resource_sptr background_img = 0);

bool create_ps_patches_with_scurve(vcl_string ps_file_name, vcl_vector<dbskr_shock_patch_sptr>& patches, vcl_vector<vil_rgb<int> >& colors,
                       dbskr_scurve_sptr& curve, vcl_vector<vil_rgb<int> >& curve_colors,
                       vil_image_resource_sptr background_img = 0);


//: create image with real boundaries
//  real boundaries are often not saved so the patches need to be reconstructed from the original shock graphs
//bool create_ps_patches_real(vcl_string ps_file_name, dbsk2d_shock_graph_sptr sg, vcl_vector<dbskr_shock_patch_sptr>& patches, vcl_vector<vil_rgb<int> >& colors);

//: tests whether the shock graph is created ok for recognition purposes 
bool test_shock_graph_for_rec(dbsk2d_shock_graph_sptr sg);

#endif // dbskr_rec_algs_h_

