// This is brcv/shp/dbskfg/algo/dbskfg_prune_composite_graph.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_prune_composite_graph.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbsk2d/dbsk2d_defines.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>

dbskfg_prune_composite_graph::dbskfg_prune_composite_graph
( 
    dbskfg_rag_node_sptr rag_node
    ):rag_node_(rag_node)
{
    prune();
}

dbskfg_prune_composite_graph::~dbskfg_prune_composite_graph()
{
    rag_node_ = 0;
    cgraph_saliency_map_.clear();
}

void dbskfg_prune_composite_graph::prune()
{

    // Step 1 
    // Set all splice costs at all sources and sinks
    update_costs_at_source_sink_nodes();

    // Step 2
    // Update all the junction nodes
    update_costs_at_junctions();

}

void dbskfg_prune_composite_graph::prune_node(dbskfg_composite_node_sptr node)
{
    double  dOC = 0;      // change in original contour length by removal
    double  dNC = 0;      // length of the new contour added
    double  dPnCost = 0;  // cost of pruning this shock
    
    //dPnCost = |_dOC + _dNC(parents) - _dNC| + dPnCost(parents)

    
    if (dbskfg_utilities::is_a_source(node) 
        || dbskfg_utilities::is_a_sink(node))
    {
        dPnCost = ISHOCK_DIST_HUGE;
    }
    else
    {
        dbskfg_composite_node::edge_iterator eit;

        for (eit  = node->in_edges_begin(); 
             eit != node->in_edges_end() ; ++eit)
        {
            dOC += cgraph_saliency_map_[(*eit)->id()].dOC;
            dNC += cgraph_saliency_map_[(*eit)->id()].dNC;
            dPnCost += cgraph_saliency_map_[(*eit)->id()].dPnCost;
        }

    }

    cgraph_saliency_map_[node->id()].dOC = dOC;
    cgraph_saliency_map_[node->id()].dNC = dNC;
    cgraph_saliency_map_[node->id()].dPnCost = dPnCost;

}

void dbskfg_prune_composite_graph::prune_link(dbskfg_shock_link* link)
{

    double ps_dNC = 0;
    double ps_dPnCost = 0;

    if ( link->source()->node_type() == dbskfg_composite_node::SHOCK_NODE )
    {
        ps_dNC = cgraph_saliency_map_[link->source()->id()].dNC;
        ps_dPnCost = cgraph_saliency_map_[link->source()->id()].dPnCost;
    }

    vcl_vector<double> costs;
    link->splice_cost(costs);

    double dOC = costs[0];
    double dNC = costs[1];
    double dPnCost(0.0);
    dPnCost = vcl_fabs(dOC + ps_dNC - dNC) + ps_dPnCost;

    cgraph_saliency_map_[link->id()].dOC = dOC;
    cgraph_saliency_map_[link->id()].dNC = dNC;
    cgraph_saliency_map_[link->id()].dPnCost = dPnCost;
    
}


void dbskfg_prune_composite_graph::update_costs_at_source_sink_nodes()
{
    //: Return all shock links of this node
    //: inline function
    vcl_map<unsigned int,dbskfg_shock_link*> shock_links = rag_node_->
        get_shock_links();

    //: Create ordered list
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator eit;
    
    for ( eit = shock_links.begin() ; eit != shock_links.end() ; ++eit)
    {
        
        dbskfg_composite_node_sptr source_node = (*eit).second->source();
        dbskfg_composite_node_sptr target_node = (*eit).second->target();
        
        if ( source_node->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( dbskfg_utilities::is_a_source(source_node) )
            {
                prune_node(source_node);
            }
        }

        if ( target_node->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( dbskfg_utilities::is_a_sink(target_node))
            {
                prune_node(target_node);
            }
        }

    }
    
}

void dbskfg_prune_composite_graph::update_costs_at_junctions()
{
    //: Return all shock links of this node
    //: inline function
    vcl_map<unsigned int,dbskfg_shock_link*> shock_links = rag_node_->
        get_shock_links();

    //: Create ordered list
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator eit;
    
    // Keep a sorted list by radius
    vcl_map<double,dbskfg_composite_node_sptr> junction_nodes;
    for ( eit = shock_links.begin() ; eit != shock_links.end() ; ++eit)
    {
        
        dbskfg_composite_node_sptr source_node = (*eit).second->source();
        dbskfg_composite_node_sptr target_node = (*eit).second->target();
        
        if ( source_node->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( !dbskfg_utilities::is_a_source(source_node)  )
            {
                dbskfg_shock_node* shock_node =
                    dynamic_cast<dbskfg_shock_node*>(&(*source_node));
                double radius = shock_node->get_radius();
                junction_nodes[radius]=source_node;
            
            }
        }

        if ( target_node->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            if ( !dbskfg_utilities::is_a_source(target_node) )
            {
                dbskfg_shock_node* shock_node =
                    dynamic_cast<dbskfg_shock_node*>(&(*target_node));
                double radius = shock_node->get_radius();
                junction_nodes[radius]=target_node;
            
            }
        }

    }

    vcl_map<double,dbskfg_composite_node_sptr>::iterator it;
    for ( it = junction_nodes.begin() ; it != junction_nodes.end() ; ++it)
    {

        dbskfg_composite_node_sptr node = it->second;

        dbskfg_composite_node::edge_iterator eit;
        for (eit  = node->in_edges_begin(); 
             eit != node->in_edges_end() ; ++eit)
        {
            dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
                (&(*(*eit)));
            if ( cgraph_saliency_map_.count(slink->id()) == 0 )
            {
                prune_link(slink);
            }
        }
        
        if ( !dbskfg_utilities::is_a_sink(node) )
        {
            prune_node(node);
        }

    }
    
}
