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
#include <vector>
#include <string>
#include <map>


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

  std::string index_file_name;

  // specifying positive and negative images
  std::string list_pos_objs;
  std::string list_neg_objs;
  std::string image_ext;

  std::string edgemap_ext;
  std::string edgeorient_ext;
  std::string edgemap_orient_vox_label;

  // list of available groundtruth xgraph
  std::string list_gt_xgraph_objs;
  std::string gt_xgraph_vox_label;
  std::string prototype_xgraph_file_obj;

  // Output file
  std::string pos_output_file;
  std::string neg_output_file;
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
  bool form_prototype_bnd_frag_labels(std::vector<std::string >& prototype_bnd_frag_labels, std::vector<unsigned >& list_prototype_eid);

  //: Gather list of groundtruth xgraph file names
  bool build_grouping_of_gt_xgraph_fnames(
    std::map<std::string, std::vector<std::string > >& map_image_name_to_xgraph_fname);

  //: Build a biarc sampler to sample fragment boundaries
  bool build_biarc_sampler(dbsks_biarc_sampler& biarc_sampler);

  
  //: Collect info about an edgemap pyramid (name and width)
  bool gather_edgemap_pyramid_info(const std::string& image_name, 
    std::vector<std::string >& list_edgemap_names,
    std::vector<vgl_box_2d<int > >& list_edgemap_bboxes);

  //: Construct a CCM cost calculator for a given edgemap.
  // If "Region of Interest" roi = 0, the whole edge map is computed
  bool build_subpix_ccm(const std::string& image_name, const std::string& edgemap_name,
    dbsks_subpix_ccm& ccm, const vgl_box_2d<int >* roi = 0 );

  //: Compute ccm cost for each boundary fragment of an xgraph
  bool compute_xgraph_ccm_cost(const dbsks_subpix_ccm& ccm, 
    const dbsks_biarc_sampler& biarc_sampler,
    const dbsksp_xshock_graph_sptr& xgraph,
    const std::vector<unsigned >& list_prototype_eid,
    vnl_vector<float >& bnd_frag_ccm_cost);

  //: Print header info in XML format
  std::ostream& print_xml_header(std::ostream& os);

  // Member variables ----------------------------------------------------------

public:  
  //: Training parameters
  vox_train_ccm_model_params params;
  
  // Internal params


  // list of filenames of positive and negative images
  std::vector<std::string > pos_image_fnames;
  std::vector<std::string > neg_image_fnames;
protected:


};


#endif


