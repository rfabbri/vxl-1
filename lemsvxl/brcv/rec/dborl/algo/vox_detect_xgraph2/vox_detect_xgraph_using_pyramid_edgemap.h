#ifndef vox_detect_xgraph_using_pyramid_edgemap_h_
#define vox_detect_xgraph_using_pyramid_edgemap_h_

//:
// \file
// \brief A system to detect xgraph in an image using edgemap pyramid
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Mar 19, 2010
//
// \verbatim
//  Adapted from Nhon's code.
// \endverbatim


#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <dbsks/dbsks_xshock_det_record.h> 

#include <vnl/vnl_vector.h>
//#include <vil/vil_pyramid_image_resource_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>

//: Run xgraph detection based on pyramid edgemap
class vox_detect_xgraph_using_pyramid_edgemap
{
public:
  //: Constructors / Destructors
  vox_detect_xgraph_using_pyramid_edgemap(){};
  ~vox_detect_xgraph_using_pyramid_edgemap(){};

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

  //: Perform detection on a specific edgemap
  bool run_detection_on(const dbdet_edgemap_sptr& edgemap, 
    const dbsksp_xshock_graph_sptr& xgraph,
    double confidence_lower_threshold,
    const vcl_string& work_dir,
    vcl_vector<dbsks_det_desc_xgraph_sptr >& dets);


public:
  vcl_string image_file;
  vcl_string edgemap_folder;
  vcl_string object_id; // string id of the image running detection on
  vcl_string edgemap_ext;
  vcl_string edgeorient_ext;
  double edgemap_log2_scale_ratio;
  vcl_string xgraph_file;
  vcl_string xgraph_geom_file;
  vcl_string xgraph_ccm_file;
  vcl_vector<vcl_string > cfrag_list_to_ignore;

  ////> Model graph sizes
  //double xgraph_base_scale;
  //double min_xgraph_scale;
  //double log2_scale_step;
  //double max_xgraph_scale;


  //: Base size of prototype xgraph - all detections will be run on this size
  double prototype_xgraph_base_size;

  //: Min graph size
  double prototype_xgraph_min_size;

  //: Max graph size
  double prototype_xgraph_max_size;

  //: Increment step of graph size - computed as log2 of scale ratio
  double prototype_xgraph_log2_increment_step;
  
  //: Max ratio between largest and smallest model size - in log2
  double prototype_xgraph_ratio_max_size_to_min_size;



  int det_window_width;
  int det_window_height;
  vcl_string work_folder;
  double min_accepted_confidence;
  bool run_nms_based_on_overlap;
  double min_overlap_ratio_for_rejection;

  vcl_vector<dbsks_det_desc_xgraph_sptr > output_det_list;

protected:
  // Intermediate data
  vil_image_view<vxl_byte > source_image;

  vcl_vector<double > xgraph_scales;

  ////
  //vil_pyramid_image_resource_sptr edgemap_pyramid;
  //vnl_vector<double > pyramid_scales;
  //vcl_map<unsigned, vcl_string > map_width2name;

  //> edgemap pyramid -----------------------------------------------------------
  
  //: list of edgemap file names for each level
  vcl_vector<vcl_string > list_edgemap_base_name;

  //: list of edgemap width for each level
  vcl_vector<unsigned > list_edgemap_width;

  //: list of edge map scale (compared to original image) for each level
  vcl_vector<double > list_edgemap_scale;

  //> shock graph
  dbsksp_xshock_graph_sptr xgraph_prototype_;
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_xgraph_ccm_model_sptr xgraph_ccm;
};



#endif
