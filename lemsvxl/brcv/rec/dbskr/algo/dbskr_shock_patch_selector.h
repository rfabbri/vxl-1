// This is brcv/rec/dbskr/algo/dbskr_shock_patch_selector.h
#ifndef dbskr_shock_patch_selector_h_
#define dbskr_shock_patch_selector_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief A class that extracts a set of patches from a shock graph 
//        and returns a subset based on various criteria
//
//        after the patches are sorted wrt some criteria, pruning works as follows
//        given a pruning depth, a tree is constructed at the root of a given patch and all the other patches rooted at a node
//        covered by this pruning tree are pruned
//
// \author Ozge C. Ozcanli March 29, 07
//
// \verbatim
//  Modifications
//
//
// \endverbatim 
//-----------------------------------------------------------------------------

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_string.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_storage_sptr.h>
#include <dbskr/dbskr_shock_patch_sptr.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>


class dbskr_shock_patch_selector 
{
public:

  //: Default constructor
  dbskr_shock_patch_selector() : image_set_(false), area_threshold_(100000.0f) {}

  dbskr_shock_patch_selector(dbsk2d_shock_graph_sptr sg) : sg_(sg), image_set_(false), area_threshold_(100000.0f) {}

  ~dbskr_shock_patch_selector() { clear(); }

  void set_image(vil_image_resource_sptr img);

  void clear();

  //: extract all the patches at a given depth
  bool extract(int depth, bool circular_ends = false);

  //: eliminate all the patches with an edge on the bounding box of the contour set
  bool prune_bounding_box_patches(int depth, vsol_box_2d_sptr bbox);

  //: clear all the bbox contours from the real boundary sets of the patches
  bool clear_bounding_box_contours(int depth, vsol_box_2d_sptr bbox);

  //: sort all the patches at a given depth
  //  works if a color image is available
  //  if the color_threshold is greater than zero then prune out the ones which are greater than the threshold
  bool find_and_sort_wrt_color_contrast(int depth, float color_threshold = 0);

  //: sort all the patches at a given depth
  //  works if grey image is available
  //  if the app_threshold is greater than zero then prune out the ones which are greater than the threshold
  bool find_and_sort_wrt_app_contrast(int depth, float app_threshold = 0);

  //: use the real contour to total length ratio to sort the patches
  bool find_and_sort_wrt_contour_ratio(int depth, float contour_rat_thres = 0);

  //: given a patch, prune all the patches at that depth whose roots are within the pruning depth of it, 
  //  use the ordering in the vector of patches
  bool prune(int depth, int pruning_depth, bool keep_pruned);  

  //: prune all the patches at this depth for which all the nodes in v_graph's are overlapping
  bool prune_same_patches(int depth);  

  //: prune all the patches at all depths whose virtual graphs share 80% of their nodes 
  //  use the sorting measure to pick the bests
  //: CAUTION: the following code assumes that depths are in increasing order
  //  in the patch_sets_ vector
  bool prune_overlaps(float overlap_ratio = 0.8f, bool keep_pruned = false, bool kill_v_graphs = true);  

  //: add the patches that are kept at this depth to the storage
  bool create_shocks_and_add_to_storage(int depth, dbskr_shock_patch_storage_sptr str);
  bool add_discarded_to_storage(int depth, dbskr_shock_patch_storage_sptr str);

  //: return the patch set pruned due to a given patch
  vcl_vector<dbskr_shock_patch_sptr>* pruned_set(dbskr_shock_patch_sptr sp);

  //: this parameter is set when the patches are extracted 
  void set_area_threshold(float val) { area_threshold_ = val; } 
  float get_area_threshold() { return area_threshold_; }

protected:
  dbsk2d_shock_graph_sptr sg_;

  //: keep sets of patches at various depths
  vcl_vector<vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >*> patch_sets_;

  //: keep the ids for easy access
  vcl_vector<vcl_map<int, vcl_pair<dbskr_shock_patch_sptr, dbsk2d_shock_node_sptr> >*> patch_set_id_maps_;

  //: keep the depth to vector index map for quick access
  vcl_map<int, int> depth_index_map_;

  // keep the following lists for visualization

  //: discarded patches at various depths
  vcl_vector<vcl_vector<vcl_pair<float, dbskr_shock_patch_sptr> >*> disc_patch_sets_;

  //: discarded patches for a given patch
  vcl_map<dbskr_shock_patch_sptr, vcl_vector<dbskr_shock_patch_sptr>*> disc_patch_map_;
  
  float area_threshold_;

  //  image variables

  //: some selection criteria depends on appearance
  vil_image_view<float> L_, A_, B_;
  vil_image_view<vxl_byte> I_;
  bool color_image_, image_set_;
};

dbsk2d_shock_graph_sptr read_esf_from_file(vcl_string fname);
void set_images(vil_image_resource_sptr img_sptr,
                vil_image_view<vxl_byte> & I_, 
                vil_image_view<float> & L_, 
                vil_image_view<float> & A_, 
                vil_image_view<float> & B_,
                vil_image_resource_sptr& img_r, 
                vil_image_resource_sptr& img_g, 
                vil_image_resource_sptr& img_b);
void save_image_poly(dbskr_shock_patch_sptr sp, 
                     vcl_string name_initial, 
                     vil_image_resource_sptr img_r, 
                     vil_image_resource_sptr img_g, 
                     vil_image_resource_sptr img_b);
void extract_subgraph_and_find_shock_patches(vcl_string image_file, 
                                             vcl_string esf_file, 
                                             vcl_string boundary_file,
                                             vcl_string kept_dir_name, 
                                             vcl_string discarded_dir_name, 
                                             vcl_string output_name,
                                             bool contour_ratio,
                                             bool circular_ends, 
                                             float area_threshold_ratio,
                                             float overlap_threshold,
                                             int min_depth, int max_depth, int depth_int, int pruning_depth, float sort_threshold,
                                             bool keep_pruned,
                                             bool save_images, 
                                             bool save_discarded_images);

bool find_shock_patches(vcl_string image_file, 
                                             dbsk2d_shock_graph_sptr sg, 
                                             vsol_box_2d_sptr bbox,
                                             vcl_string kept_dir_name, 
                                             vcl_string discarded_dir_name, 
                                             vcl_string output_name,
                                             bool contour_ratio,
                                             bool circular_ends, 
                                             float area_threshold_ratio,
                                             float overlap_threshold,
                                             int min_depth, int max_depth, int depth_int, int pruning_depth, float sort_threshold,
                                             bool keep_pruned,
                                             bool save_images, 
                                             bool save_discarded_images);


#endif // dbskr_shock_patch_selector_h_

