// This is brcv/shp/dbskfg/algo/dbskfg_prune_composite_graph.h
#ifndef dbskfg_prune_composite_graph_h_
#define dbskfg_prune_composite_graph_h_
//:
// \file
// \brief Algorithm to prune a composite graph based on splice cost 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 10/22/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <vcl_map.h>

class dbskfg_shock_link;

//: Prune composite graph
class dbskfg_prune_composite_graph
{

public:

    //: Constructor
    dbskfg_prune_composite_graph(dbskfg_rag_node_sptr rag_node);

    //: Destructor
    ~dbskfg_prune_composite_graph();

    //: Return cost for an id
    double splice_cost(unsigned int id){
        return cgraph_saliency_map_[id].dPnCost;}

private:

    //: Shock Saliency params structure
    // parameters needed to compute shock saliency
    struct cgraph_saliency_params {
        double dOC;
        double dNC;
        double dPnCost;
    };

    // Holds rag node to prune
    dbskfg_rag_node_sptr rag_node_;

    // Holds a shock saliency map
    vcl_map<unsigned int,cgraph_saliency_params> cgraph_saliency_map_;

    void prune();

    //: Method to set large costs and sources and sinks
    void update_costs_at_source_sink_nodes();

    //: Method to update costs at links
    void update_costs_at_junctions();

    // prune node
    void prune_node(dbskfg_composite_node_sptr node);

    void prune_link(dbskfg_shock_link* link);

    // Make copy ctor private
    dbskfg_prune_composite_graph(const dbskfg_prune_composite_graph&);

    // Make assign operator private
    dbskfg_prune_composite_graph& operator
        =(const dbskfg_prune_composite_graph& );

   
};

#endif //dbsk2d_ishock_prune_h_
