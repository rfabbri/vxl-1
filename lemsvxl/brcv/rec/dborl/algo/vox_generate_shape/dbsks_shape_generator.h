// This is dbsks/algo/dbsks_shape_generator.h
#ifndef dbsks_shape_generator_h_
#define dbsks_shape_generator_h_

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

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment.h>

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

#include <pdf1d/pdf1d_flat.h>
#include <pdf1d/pdf1d_gaussian.h>
#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_pdf.h>
#include <pdf1d/pdf1d_sampler.h>

#include <vnl/vnl_random.h>
#include <vbl/vbl_ref_count.h>

//: Run xgraph detection based on pyramid edgemap
class dbsks_shape_generator
{
public:
  //: Constructors / Destructors
  dbsks_shape_generator(){};
  ~dbsks_shape_generator(){};

  // UTILITIES
  bool execute();

  //: Load all data from input files
  bool load_params_and_models();

  //: Perform detection on a specific edgemap
  bool sample_shapes_from_geom_model(dbsksp_xshock_graph_sptr& xgraph);

  bool sample_child_frags_and_nodes (dbsksp_xshock_node_descriptor xd_p, dbsksp_xshock_edge_sptr& xe_c, dbsksp_xshock_node_sptr& xv_c, dbsksp_xshock_graph_sptr& xgraph, dbsks_xfrag_geom_model_sptr geom_model);


public:

  vcl_string xgraph_file;
  vcl_string xgraph_geom_file;
  vcl_string xgraph_geom_param_file;

  //: Final detection list
  vcl_vector<dbsks_det_desc_xgraph_sptr > output_shape_list;

  // Graph-tree info
  unsigned root_vid_;
  unsigned major_child_eid_;

  int num_samples;



protected:

  //: prototype xgraph of the category
  dbsksp_xshock_graph_sptr xgraph_prototype_;

  //: geometry (shape) model
  dbsks_xgraph_geom_model_sptr xgraph_geom;

  //: An engine to compute samples of a biarc fast, at the expense of possible inaccuracy
  dbsks_biarc_sampler biarc_sampler;

};



#endif
