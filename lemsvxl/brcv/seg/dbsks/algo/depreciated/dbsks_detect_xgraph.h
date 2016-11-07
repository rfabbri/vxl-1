// This is dbsks/algo/dbsks_detect_xgraph.h
#ifndef dbsks_detect_xgraph_h_
#define dbsks_detect_xgraph_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 2, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>


#include <vsol/vsol_polyline_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>


class dbsks_ccm;



//: Detect an object, represented as a shock graph, in an image
bool dbsks_detect_xgraph(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         float ocm_edge_threshold,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_vector<double >& xgraph_scales,
                         int root_vid,
                         int major_child_eid,
                         float ocm_lambda,
                         float ocm_gamma,
                         float ocm_tol_near_zero,
                         float ocm_distance_threshold,
                         vil_image_resource_sptr& image_resource, 
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& list_det);



//: Detect xgraph of in an iamge at a particular scale
bool dbsks_detect_xgraph(const vil_image_view<float >& image_src,
                         const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         float ocm_edge_threshold,
                         // dbdet_curve_fragment_graph* CFG,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         double xgraph_scale,
                         int root_vid,
                         int major_child_eid,
                         float ocm_lambda,
                         float ocm_gamma,
                         float ocm_tol_near_zero,
                         float ocm_distance_threshold,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& list_det);






//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_ccm(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_string& xgraph_ccm_file,
                         const vcl_vector<double >& xgraph_scales,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list);






//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_ccm(const vil_image_view<float >& image_src,
                         const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                         double xgraph_scale,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list);





//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_wcm(const vcl_string& image_file,
                         const vcl_string& edgemap_file,
                         const vcl_string& edgeorient_file,
                         const vcl_string& cemv_file,
                         const vcl_string& xgraph_file,
                         const vcl_string& xgraph_geom_file,
                         const vcl_string& xgraph_ccm_file,
                         const vcl_vector<vcl_string >& cfrag_list_to_ignore,
                         float wcm_weight_unmatched,
                         const vcl_vector<double >& xgraph_scales,
                         int det_window_width,
                         int det_window_height,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list);

//: Detect an object using both geometric model and contour chamfer matching cost model
bool dbsks_detect_xgraph_using_wcm(const vil_image_view<float >& edgemap,
                         const vil_image_view<float >& edge_angle,
                         const vcl_vector<vsol_polyline_2d_sptr >& polyline_list,
                         const dbsksp_xshock_graph_sptr& xgraph,
                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                         float wcm_weight_unmatched,
                         double xgraph_scale,
                         int det_window_width,
                         int det_window_height,
                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list);


//: Load data from a list of file names
bool dbsks_load_data(const vcl_string& image_file,
                     const vcl_string& edgemap_file,
                     const vcl_string& edgeorient_file,
                     const vcl_string& cemv_file,
                     const vcl_string& xgraph_file,
                     const vcl_string& xgraph_geom_file,
                     const vcl_string& xgraph_ccm_file,
                     const vcl_vector<vcl_string >& cfrag_list_to_ignore,
                     vil_image_view<float >& edgemap,
                     vil_image_view<float >& edge_angle,
                     vcl_vector<vsol_polyline_2d_sptr >& polyline_list,
                     dbsksp_xshock_graph_sptr& xgraph,
                     dbsks_xgraph_geom_model_sptr& xgraph_geom,
                     dbsks_xgraph_ccm_model_sptr& xgraph_ccm);



//: Prepare a CCM calculator from a CCM model and data sources
bool dbsks_prepare_ccm(dbsks_ccm* ccm, 
                       const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                       const vil_image_view<float >& edgemap,
                       const vil_image_view<float >& edge_angle);


//// -----------------------------------------------------------------------------
////: Detect an object using both geometric model and contour chamfer matching cost model
//bool dbsks_detect_xgraph_using_ccm_subpix(const vcl_string& image_file,
//                                          const vcl_string& edgemap_folder,
//                                          const vcl_string& object_id,
//                                          const vcl_string& edgemap_ext,
//                                          const vcl_string& edgeorient_ext,
//                         const vcl_string& xgraph_file,
//                         const vcl_string& xgraph_geom_file,
//                         const vcl_string& xgraph_ccm_file,
//                         const vcl_vector<vcl_string >& cfrag_list_to_ignore,
//                         double xgraph_base_scale,
//                         double min_xgraph_scale,
//                         double log2_scale_step,
//                         double max_xgraph_scale,
//                         int det_window_width,
//                         int det_window_height,
//                         const vcl_string& work_folder,
//                         double min_accepted_confidence,
//                         bool run_nms_based_on_overlap,
//                         double min_overlap_ratio_for_rejection,
//                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list);
//
//
//// -----------------------------------------------------------------------------
////: Detect an object using both geometric model and contour chamfer matching cost model
//bool dbsks_detect_xgraph_using_ccm_subpix(const dbdet_edgemap_sptr& edgemap,
//                         const dbsksp_xshock_graph_sptr& xgraph,
//                         const dbsks_xgraph_geom_model_sptr& xgraph_geom,
//                         const dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
//                         int det_window_width,
//                         int det_window_height,
//                         vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list,
//                         double confidence_lower_threshold,
//                         bool run_nms_based_on_overlap,
//                         double min_overlap_ratio_for_rejection
//                         );



//------------------------------------------------------------------------------
// Utilities - to be moved to a separate file



//: Construct an dbdet_edgemap from an edge image and an edge angle matrix
dbdet_edgemap_sptr dbsks_make_edgemap(const vil_image_view<float >& edgemap,
                                      const vil_image_view<float >& edge_angle,
                                      float lower_threshold = 1.0f,
                                      float max_edge_value = 255.0f);




#endif
