// This is /lemsvxl/contrib/firat/dbsksp_object_warehouse/dbsksp_object_warehouse.h.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Apr 8, 2011

#ifndef DBSKSP_OBJECT_WAREHOUSE_H_
#define DBSKSP_OBJECT_WAREHOUSE_H_

#include <string>
#include <vector>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <vnl/vnl_random.h>
#include <vsol/vsol_point_2d_sptr.h>

class dbsksp_object_warehouse
{
    public:
        dbsksp_object_warehouse(const std::string& exemplar_xgraph_folder, const std::string& exemplar_xgraph_list_file);
        ~dbsksp_object_warehouse();
        bool perturb_exemplar(int examplar_id, int num_perturbations = 1);
        bool perturb_all_exemplars(int num_perturbations);
        dbsksp_xshock_graph_sptr get_object(int exemplar_id = 0, int child_id = -1);
        bool save(const std::string& folder);
        static dbsksp_object_warehouse* load(const std::string& folder);
    private:
        std::vector<dbsksp_xshock_graph_sptr> exemplar_xgraphs_;
        std::vector<std::vector<dbsksp_xshock_graph_sptr>* > child_xgraphs_;
        bool active_;
        vnl_random random_engine_;

        void perturb_xgraph_(dbsksp_xshock_graph_sptr xgraph, dbsksp_xshock_node_sptr root_node, int parent_edge);
        void save_contour_file_(std::vector<vsol_point_2d_sptr>& point_list, const std::string& filename);
};

#endif /* DBSKSP_OBJECT_WAREHOUSE_H_ */
