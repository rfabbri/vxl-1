#include <bgrld/bgrld_graph.hxx>

#include "../dbskr_shock_patch.h"
#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

BGRLD_GRAPH_INSTANTIATE(dbskr_v_node, dbskr_v_edge);

#include <bgrld/algo/bgrld_algs.cxx>
BGRLD_EULER_TOUR_INSTANTIATE( dbskr_v_graph, dbskr_v_edge, dbskr_v_node );

BGRLD_DEPTH_NO_LOOP_INSTANTIATE( dbsk2d_shock_graph, dbsk2d_shock_edge, dbsk2d_shock_node );

