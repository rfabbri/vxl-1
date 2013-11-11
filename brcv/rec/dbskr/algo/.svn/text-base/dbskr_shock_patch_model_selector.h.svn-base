// This is brcv/rec/dbskr/algo/dbskr_shock_patch_model_selector.h
#ifndef dbskr_shock_patch_model_selector_h_
#define dbskr_shock_patch_model_selector_h_

//-----------------------------------------------------------------------------
//:
// \file
// \brief A class that extracts a set of patches from a model shock graph 
//        and returns a subset based on various criteria
//
// \author Ozge C. Ozcanli July 27, 07
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


class dbskr_shock_patch_model_selector 
{
public:

  //: Default constructor
  dbskr_shock_patch_model_selector() {}

  dbskr_shock_patch_model_selector(dbsk2d_shock_graph_sptr sg) : sg_(sg) {}

  ~dbskr_shock_patch_model_selector() { clear(); }

  void clear();

  //: extract all the patches at a given depth
  //  area_threshold default is 0.1 and no need to pass anything
  //  since anything larger than zero works since this is a shock graph of a simple closed
  //  curve, there are no inner loops which causes outer traces with nearly zero areas
  //  as in the extraction of patches from natural image shock graphs
  bool extract(int depth, bool circular_ends = false, double area_threshold = 0.1);

  //: prune all the patches at this depth for which all the nodes in v_graph's are overlapping
  bool prune_same_patches(int depth);  
  bool prune_same_patches_at_all_depths();  

  //: add the patches to the storage
  bool add_to_storage(int depth, dbskr_shock_patch_storage_sptr str);

  vcl_vector<vcl_vector<dbskr_shock_patch_sptr > *>& patch_sets() { return patch_sets_; }

protected:
  dbsk2d_shock_graph_sptr sg_;

  //: keep sets of patches at various depths
  vcl_vector<vcl_vector<dbskr_shock_patch_sptr > *> patch_sets_;

};


#endif // dbskr_shock_patch_model_selector_h_

