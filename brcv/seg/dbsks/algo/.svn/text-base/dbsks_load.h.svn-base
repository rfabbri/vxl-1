// This is dbsks/algo/dbsks_load.h
#ifndef dbsks_load_h_
#define dbsks_load_h_

//:
// \file
// \brief Functions to load various data types used in dbsks
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date July 7, 2009
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
#include <vcl_string.h>


//: load the image
bool dbsks_load_image_resource(const vcl_string& image_file, 
                               vil_image_resource_sptr& image_resource);

//: Load edgemap
bool dbsks_load_edgemap(const vcl_string& edgemap_file, 
                        vil_image_view<float >& edgemap);

//: Load edge angle
bool dbsks_load_edge_angle(const vcl_string& edgeorient_file, 
                           vil_image_view<float >& edge_angle);

//: Load an sub-pixel edgemap from 2 files: a binary image of edge location
// and a text file of edge orientation at each location
dbdet_edgemap_sptr dbsks_load_subpix_edgemap(const vcl_string edgemap_file, 
                                             const vcl_string edgeorient_file,
                                             float lower_threshold = 1.0f,
                                             float max_edge_value = 255.0f);

//: Load linked-edge list
bool dbsks_load_polyline_list(const vcl_string& cemv_file, 
                              vcl_vector<vsol_polyline_2d_sptr >& polyline_list);

//: Load the shock graph
bool dbsks_load_xgraph(const vcl_string& xgraph_file, dbsksp_xshock_graph_sptr& xgraph);

//: Load the xgraph geometric model
bool dbsks_load_xgraph_geom_model(const vcl_string& xgraph_geom_file, 
                                  const vcl_string& xgraph_geom_param_file,
                                  dbsks_xgraph_geom_model_sptr& xgraph_geom);

//: Load the xgraph CCM model
bool dbsks_load_xgraph_ccm_model(const vcl_string& xgraph_ccm_file, 
                                 const vcl_string& xgraph_ccm_param_file,
                                 dbsks_xgraph_ccm_model_sptr& xgraph_ccm);





#endif
