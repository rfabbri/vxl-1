// This is /lemsvxl/contrib/firat/shape_factory_v1/dbsksp_shape_factory.h.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jan 19, 2011
//

#ifndef DBSKSP_SHAPE_FACTORY_H_
#define DBSKSP_SHAPE_FACTORY_H_

#include <vcl_string.h>
#include <vcl_vector.h>
#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <vnl/vnl_random.h>

class dbsksp_shape_factory
{
    public:
        dbsksp_shape_factory(const vcl_string& xgraph_geom_filename, const vcl_string& prototype_xgraph_filename);
        dbsksp_shape_factory(const vcl_string& xgraph_geom_filename, const vcl_string& prototype_xgraph_folder, const vcl_string& prototype_xgraph_list_file);
        dbsksp_xshock_graph_sptr generate_random_shape(int xgraph_id = -1);
        vcl_vector<dbsksp_xshock_graph_sptr> generate_random_shapes(int num, int xgraph_id = -1);
        vcl_vector<dbsksp_xshock_graph_sptr> generate_all_shapes();
    private:
        vcl_vector<dbsksp_xshock_graph_sptr> prototype_xgraphs_;
        dbsks_xgraph_geom_model_sptr xgeom_;
        bool active_;
        vnl_random random_engine_;

        void generate_xgraph_(dbsksp_xshock_graph_sptr xgraph, dbsksp_xshock_node_sptr root_node, int parent_edge, double graph_size);
};

#endif /* DBSKSP_SHAPE_FACTORY_H_ */
