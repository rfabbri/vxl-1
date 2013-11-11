// This is /lemsvxl/brcv/rec/dborl/algo/vox_train_xgraph_geometry_and_ccm_models/vox_train_xgraph_ccm_model/vox_train_ccm_model.h
#ifndef vox_train_ccm_model_h_
#define vox_train_ccm_model_h_

//:
// \file
// \brief A class to collect data and train CCM cost model
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date July 9, 2009
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsks/dbsks_subpix_ccm.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>

#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>


//: Parameters to train a ccm model for xgraph
struct vox_train_ccm_model_params
{
  // parameters
  int nbins_0topi;
  float distance_threshold;
  float distance_tol_near_zero;
  float orient_threshold;
  float orient_tol_near_zero;

  float weight_chamfer;
  float weight_edge_orient;
  float weight_contour_orient;

  float local_window_width;

  unsigned root_vid;
  double base_xgraph_size;

  // number of data point for groundtruth xgraph
  int num_samples_per_xgraph;
  double perturb_delta_x;
  double perturb_delta_y;

  vcl_string index_file_name;

  // specifying positive and negative images
  vcl_string list_pos_objs;
  vcl_string list_neg_objs;
  vcl_string image_ext;

  vcl_string edgemap_ext;
  vcl_string edgeorient_ext;
  vcl_string edgemap_orient_vox_label;

  // list of available groundtruth xgraph
  vcl_string list_gt_xgraph_objs;
  vcl_string gt_xgraph_vox_label;
  vcl_string prototype_xgraph_file_obj;

  // Output file
  vcl_string pos_output_file;
  vcl_string neg_output_file;
};


//: Default training parameters for various categories
vox_train_ccm_model_params vox_train_ccm_model_default_params();


// ============================================================================
class vox_train_ccm_model
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  vox_train_ccm_model(){};

  //: destructor
  virtual ~vox_train_ccm_model(){};
  
  // Access member-variables ---------------------------------------------------

  
  // Utility functions ---------------------------------------------------------

  //: Set info training info
  bool set_training_data_info(const vox_train_ccm_model_params& p);

  //: Collect positive training data
  bool collect_positive_data();

  //: Collect negative training data
  bool collect_negative_data();


  // Support functions ---------------------------------------------------------
protected:
  //: Form a list of boundary fragment labels from the prototype graph
  bool form_prototype_bnd_frag_labels(vcl_vector<vcl_string >& prototype_bnd_frag_labels, vcl_vector<unsigned >& list_prototype_eid);

  //: Gather list of groundtruth xgraph file names
  bool build_grouping_of_gt_xgraph_fnames(
    vcl_map<vcl_string, vcl_vector<vcl_string > >& map_image_name_to_xgraph_fname);

  //: Build a biarc sampler to sample fragment boundaries
  bool build_biarc_sampler(dbsks_biarc_sampler& biarc_sampler);

  
  //: Collect info about an edgemap pyramid (name and width)
  bool gather_edgemap_pyramid_info(const vcl_string& image_name, 
    vcl_vector<vcl_string >& list_edgemap_names,
    vcl_vector<vgl_box_2d<int > >& list_edgemap_bboxes);

  //: Construct a CCM cost calculator for a given edgemap.
  // If "Region of Interest" roi = 0, the whole edge map is computed
  bool build_subpix_ccm(const vcl_string& image_name, const vcl_string& edgemap_name,
    dbsks_subpix_ccm& ccm, const vgl_box_2d<int >* roi = 0 );

  //: Compute ccm cost for each boundary fragment of an xgraph
  bool compute_xgraph_ccm_cost(const dbsks_subpix_ccm& ccm, 
    const dbsks_biarc_sampler& biarc_sampler,
    const dbsksp_xshock_graph_sptr& xgraph,
    const vcl_vector<unsigned >& list_prototype_eid,
    vnl_vector<float >& bnd_frag_ccm_cost);

  //: Print header info in XML format
  vcl_ostream& print_xml_header(vcl_ostream& os);

  // Member variables ----------------------------------------------------------

public:  
  //: Training parameters
  vox_train_ccm_model_params params;
  
  // Internal params


  // list of filenames of positive and negative images
  vcl_vector<vcl_string > pos_image_fnames;
  vcl_vector<vcl_string > neg_image_fnames;
protected:


};


#endif


