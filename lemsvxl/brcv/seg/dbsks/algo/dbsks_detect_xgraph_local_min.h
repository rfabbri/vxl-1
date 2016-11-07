// This is dbsks/algo/dbsks_detect_xgraph_local_min.h
#ifndef dbsks_detect_xgraph_local_min_h_
#define dbsks_detect_xgraph_local_min_h_

//:
// \file
// \brief Optimize an xgraph locally using an edgemap pyramid
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 11, 2010
//
// \verbatim
//  Modifications
// \endverbatim


//#include <dbsks/dbsks_det_desc_xgraph_sptr.h>
//#include <dbsks/dbsks_xfrag_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
//#include <dbdet/edge/dbdet_edgemap_sptr.h>
//#include <dbsks/dbsks_xshock_det_record.h> 
#include <dbsks/dbsks_biarc_sampler.h>

//#include <vnl/vnl_vector.h>
//#include <vil/vil_image_resource_sptr.h>
//#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vcl_string.h>
//#include <vcl_map.h>

//: Run xgraph detection based on pyramid edgemap
class dbsks_detect_xgraph_local_min
{
public:
  //: Constructors / Destructors
  dbsks_detect_xgraph_local_min(){};
  ~dbsks_detect_xgraph_local_min(){};

  // UTILITIES
  bool execute();

  //: Load all data from input files
  bool load_params_and_models();

  //: Load edge map pyramid
  bool load_edgemap_pyramid();

  //: Optimize xgraph
  bool optimize_xgraph();

public:
  vcl_string object_id; // string id of the image running detection on
  vcl_string edgemap_folder;
  vcl_string edgemap_ext;
  vcl_string edgeorient_ext;
  double edgemap_log2_scale_ratio;
  vcl_string xgraph_file;
  vcl_string xgraph_geom_file;
  vcl_string xgraph_ccm_file;
  

  vcl_string work_folder;

  //vcl_vector<dbsks_det_desc_xgraph_sptr > output_det_list;

protected:

  //> edgemap pyramid -----------------------------------------------------------
  
  //: list of edgemap file names for each level
  vcl_vector<vcl_string > list_edgemap_base_name;

  //: list of edgemap width for each level
  vcl_vector<unsigned > list_edgemap_width;

  //: list of edge map scale (compared to original image) for each level
  vcl_vector<double > list_edgemap_scale;

  // Components to build an xshock_ccm_likelihood calculator
  dbsksp_xshock_graph_sptr xgraph_prototype_;
  dbsks_xgraph_geom_model_sptr xgraph_geom;
  dbsks_xgraph_ccm_model_sptr xgraph_ccm;
  dbsks_biarc_sampler biarc_sampler;

  // Loaded xgraph - used as initialization for gradient descent
  dbsksp_xshock_graph_sptr init_xgraph_;


  //

};



#endif
