// This is /lemsvxl/contrib/firat/dbsks_xgraph_likelihood_calculator/dbsks_xgraph_likelihood_calculator_utils.h.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Feb 24, 2011
//

#ifndef DBSKS_XGRAPH_LIKELIHOOD_CALCULATOR_UTILS_H_
#define DBSKS_XGRAPH_LIKELIHOOD_CALCULATOR_UTILS_H_

#include<dbdet/edge/dbdet_edgemap_sptr.h>
#include<dbsksp/dbsksp_xshock_graph_sptr.h>
#include<vector>
#include<string>
#include <map>
#include<dbsks/dbsks_xgraph_ccm_model_sptr.h>
#include<dbsks/dbsks_xshock_ccm_likelihood.h>

class dbsks_xgraph_likelihood_calculator_utils
{
    public:
        static bool load_edgemap_pyramid(std::vector<std::string >& list_edgemap_base_name,
                std::vector<unsigned >& list_edgemap_width,
                std::vector<double >& list_edgemap_scale,
                const std::string& edgemap_folder,
                const std::string& edgemap_ext,
                const std::string& object_id,
                double image_width);

        static dbsks_xshock_ccm_likelihood* get_ccm_like_keeping_graph_size_fixed(dbsksp_xshock_graph_sptr& input_xgraph,
                double target_xgraph_size,
                double& actual_pyramid_scale,
                dbdet_edgemap_sptr& actual_edgemap,
                dbsksp_xshock_graph_sptr& actual_xgraph,
                dbsks_xgraph_ccm_model_sptr& xgraph_ccm,
                std::vector<std::string >& list_edgemap_base_name,
                std::vector<double >& list_edgemap_scale,
                const std::string& edgemap_folder,
                const std::string& edgemap_ext,
                const std::string& edgeorient_ext,
                const std::string& object_id,
                double edgemap_log2_scale_ratio,
                dbsks_biarc_sampler* biarc_sampler_ptr,
                std::map<int, dbsks_xshock_ccm_likelihood*>& ccm_like_db,
                int precision,
                double cut_off_target_pyramid_scale);
};

// local data structure to facilitate sorting the edgemap by their width
struct edgemap_level_info
{
  std::string base_name;
  int width;
  double scale;
};


#endif /* DBSKS_XGRAPH_LIKELIHOOD_CALCULATOR_UTILS_H_ */
