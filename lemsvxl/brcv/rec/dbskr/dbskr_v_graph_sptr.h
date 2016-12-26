#ifndef _dbskr_v_graph_sptr_h
#define _dbskr_v_graph_sptr_h

class dbskr_v_edge;
class dbskr_v_node;
class dbskr_v_graph;

#include <vbl/vbl_smart_ptr.h>
#include <dbgrl/dbgrl_graph.h>

typedef vbl_smart_ptr<dbskr_v_edge> dbskr_v_edge_sptr;
typedef vbl_smart_ptr<dbskr_v_node> dbskr_v_node_sptr;
typedef vbl_smart_ptr<dbskr_v_graph> dbskr_v_graph_sptr;

#endif // _dbskr_v_graph_sptr_h
