// This is dbsks/algo/dbsks_detect_xgraph_using_edgemap.h
#ifndef dbsks_detect_xgraph_using_edgemap_h_
#define dbsks_detect_xgraph_using_edgemap_h_

//:
// \file
// \brief A system to detect xgraph in an image using edgemap
// load in edg from .cem
// use single scale edgemap, single scale xgraph
// aim to detection in sequential frames

// author Yuliang, 02/20/2014
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
//#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbsks/dbsks_xshock_det_record.h> 
#include <dbsks/dbsks_biarc_sampler.h>

#include <dbdet/pro/dbdet_sel_storage.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <vnl/vnl_vector.h>
//#include <vil/vil_pyramid_image_resource_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>

//: Run xgraph detection based on pyramid edgemap
class dbsks_detect_xgraph_using_edgemap
{
public:
  //: Constructors / Destructors
  dbsks_detect_xgraph_using_edgemap(){};
  ~dbsks_detect_xgraph_using_edgemap(){};

  // UTILITIES
  bool execute();

  //: Load all data from input files
  bool load_params_and_models();

  //: Load edge map pyramid
  bool load_edgemap_pyramid();

  //: Compute sizes of model xgraph to run
  bool compute_list_model_graph_size();

  //: Load edgemap corresponding to a target xgraph size to that the actual
  // xgraph size is about the same as the base xgraph size
  bool load_edgemap_in_pyramid_keeping_graph_size_fixed(double target_xgraph_scale, 
    double& actual_pyramid_scale,
    dbdet_edgemap_sptr& actual_edgemap,
    dbsksp_xshock_graph_sptr& actual_xgraph);

  bool load_edgemap_singe_scale(dbdet_edgemap_sptr& actual_edgemap,dbsksp_xshock_graph_sptr& actual_xgraph);

  bool load_initial_edgemap(dbdet_edgemap_sptr& actual_edgemap,dbsksp_xshock_graph_sptr& actual_xgraph);

  bool load_edgemap_from_curve_fragments(dbdet_sel_storage_sptr& actual_sel, dbdet_edgemap_sptr& actual_edgemap,dbsksp_xshock_graph_sptr& actual_xgraph);

  //: Perform detection on a specific edgemap
  bool run_detection_on(const dbdet_edgemap_sptr& edgemap, 
    const dbsksp_xshock_graph_sptr& xgraph,
    double confidence_lower_threshold,
    const vcl_string& work_dir,
    vcl_vector<dbsks_det_desc_xgraph_sptr >& dets,
	vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets);


  //: A Post Process of ranking results and print out cost terms
  bool rank_detection_results(const dbdet_edgemap_sptr& edgemap, 
    const dbsksp_xshock_graph_sptr& xgraph,
    double confidence_lower_threshold,
    const vcl_string& work_dir,
    vcl_vector<dbsks_det_desc_xgraph_sptr >& dets,
	vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets);


  // Utilities------------------------------------------------------------------

  //: Compute black/white mask for an edge map: 255 at edge pixels and 0 otherwise
  static bool convert_edgemap_to_bw(const dbdet_edgemap_sptr& edgemap,
    vil_image_view<vxl_byte >& bw_mask);

  bool load_appearance_model();
  bool load_bb_file();

  double compute_appearance_cost(dbsksp_xshock_graph_sptr& sol_xgraph, vil_image_view<vxl_byte>& L);
  vcl_vector<double> compute_bg_vec (dbsksp_xshock_graph_sptr& sol_xgraph, vil_image_view<vxl_byte>& L);
  bool update_appearance_model(vcl_vector<dbsks_det_desc_xgraph_sptr > prev_dets, vil_image_view<vxl_byte>& L);
  double compute_shape_trans_cost(dbsksp_xshock_graph_sptr& cur_xgraph, dbsksp_xshock_graph_sptr prev_xgraph);

  double compute_appearance_cost_v2(dbsksp_xshock_graph_sptr& cur_xgraph, vil_image_view<vxl_byte>& cur_image, dbsksp_xshock_graph_sptr prev_xgraph, vil_image_view<vxl_byte>& prev_image);

public:
  vcl_string image_file;
  vcl_string prev_image_file;
  vcl_string edgemap_folder;
  vcl_string initial_edge_folder;
  vcl_string object_id; // string id of the image running detection on
  vcl_string edgemap_ext;
  vcl_string edgeorient_ext;
  vcl_string input_cemv_extension;
  double edgemap_log2_scale_ratio;
  vcl_string xgraph_file;
  vcl_string xgraph_geom_file;
  vcl_string xgraph_geom_param_file;
  vcl_string xgraph_ccm_file;
  vcl_string xgraph_ccm_param_file;
  vcl_string xgraph_appearance_file;
  vcl_vector<vcl_string > cfrag_list_to_ignore;
  vcl_string bb_file;

  //: Min graph size
  double prototype_xgraph_min_size;

  //: Max graph size
  double prototype_xgraph_max_size;

  //: Increment step of graph size - computed as log2 of scale ratio
  double prototype_xgraph_log2_increment_step;
  
  //: Max ratio between largest and smallest model size - in log2
  double prototype_xgraph_ratio_max_size_to_min_size;

  //: width and height of detection window
  int det_window_width;
  int det_window_height;

  //: directory to save intermediate outputs
  vcl_string work_folder;

  //: directory contain prev outputs
  vcl_string prev_folder;

  //: lower threshold to accept a solution
  double min_accepted_confidence;

  //: Flag to run non-max-suppression based overlaps between solutions
  bool run_nms_based_on_overlap;

  //: Min overlap ratio between two solutions so that they are consider "the same",
  // in which case, one of them will be rejected.
  double min_overlap_ratio_for_rejection;

  //: Final detection list
  vcl_vector<dbsks_det_desc_xgraph_sptr > output_det_list;

  vcl_vector<int> appearance_model_node_id;
  vcl_vector<int> appearance_model_node_value;
  vcl_vector<int> bb_coordinates;

protected:
  
  //: Original image, can be color or gray
  vil_image_view<vxl_byte > source_image;
  vil_image_view<vxl_byte > prev_source_image;

  //: list of xgraphs to run detection on
  vcl_vector<double > xgraph_scales;


  //> edgemap pyramid -----------------------------------------------------------
  
  //: list of edgemap file names for each level
  vcl_vector<vcl_string > list_edgemap_base_name;

  //: list of edgemap width for each level
  vcl_vector<unsigned > list_edgemap_width;

  //: list of edge map scale (compared to original image) for each level
  vcl_vector<double > list_edgemap_scale;

  //> Components to build an xshock_ccm_likelihood calculator-------------------

  //: prototype xgraph of the category
  dbsksp_xshock_graph_sptr xgraph_prototype_;

  //: geometry (shape) model
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_xgraph_geom_model_sptr xgraph_geom_L;
  dbsks_xgraph_geom_model_sptr xgraph_geom_R;

  //: Contour chamfer matching (appearance) model
  dbsks_xgraph_ccm_model_sptr xgraph_ccm;

  //: An engine to compute samples of a biarc fast, at the expense of possible inaccuracy
  dbsks_biarc_sampler biarc_sampler;

};



#endif
