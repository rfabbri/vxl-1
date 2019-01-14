#ifndef detect_xgraph_app_h_
#define detect_xgraph_app_h_

#include <string>
#include <vector>
#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <vil/vil_image_view.h>

//: Detect an object, represented as a shock graph, in an image by apperance
bool detect_xgraph_using_app(const std::string& image_file,
                             const std::string& xgraph_file,
                             const std::string& xgraph_geom_file,
                             const std::vector<double >& xgraph_scales,
                             int det_window_width,
                             int det_window_height,
                             std::vector<dbsks_det_desc_xgraph_sptr >
                             & output_det_list);

bool detect_xgraph_using_app(const std::string& image_file,
                             const dbsksp_xshock_graph_sptr& xgraph,
                             const dbsks_xgraph_geom_model_sptr& xgraph_geom,
                             double xgraph_scale,
                             int det_window_width,
                             int det_window_height,
                             std::vector<dbsks_det_desc_xgraph_sptr >
                             & output_det_list);



#endif
