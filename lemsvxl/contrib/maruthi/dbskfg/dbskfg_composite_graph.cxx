// This is file shp/dbskfg/dbskfg_composite_graph.cxx

//:
// \file

#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_shock_link.h>

// ----------------------------------------------------------------------------
//: Constructor
dbskfg_composite_graph::
dbskfg_composite_graph(bool locus): 
    dbgrl_graph<dbskfg_composite_node, dbskfg_composite_link>(),
    next_available_id_(0),
    construct_locus_(locus)
{

}

// ----------------------------------------------------------------------------
//: Destructor
dbskfg_composite_graph::
~dbskfg_composite_graph()
{
    all_nodes_.clear();
    all_links_.clear();

}

// ----------------------------------------------------------------------------
//: Copy constructor
// Need to fix this constructor so that it also works when the argument is
// "const dbskfg_composite_graph&"
dbskfg_composite_graph::
dbskfg_composite_graph(dbskfg_composite_graph& that):
dbgrl_graph<dbskfg_composite_node, dbskfg_composite_link>()
{
    // Reset current id
    this->set_current_id(that.current_id());

    // Set locus flag to whatever that is
    this->construct_locus_ = that.construct_locus_;

    // Keep track of all nodes
    vcl_map<unsigned int,dbskfg_composite_node_sptr> all_nodes;
   
    for (dbskfg_composite_graph::vertex_iterator vit = that.vertices_begin(); 
         vit != that.vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = (*vit);

        if ( node->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {            
            dbskfg_shock_node* that_snode = dynamic_cast<dbskfg_shock_node*>
                (&(*node));

            dbskfg_composite_node_sptr this_shock_node
                = new dbskfg_shock_node(
                    that_snode->id(),
                    that_snode->pt(),
                    that_snode->get_radius());
            this_shock_node->set_composite_degree(
                that_snode->get_composite_degree());
            this->add_vertex(this_shock_node);
            all_nodes[this_shock_node->id()]=this_shock_node;
            all_nodes_[this_shock_node->id()]=this_shock_node;
        }
        else
        {

            dbskfg_contour_node* that_cnode = dynamic_cast<dbskfg_contour_node*>
                (&(*node));

            dbskfg_composite_node_sptr this_contour_node
                = new dbskfg_contour_node(
                    that_cnode->id(),
                    that_cnode->pt(),
                    that_cnode->contour_id());
            this_contour_node->set_composite_degree(
                that_cnode->get_composite_degree());
            this->add_vertex(this_contour_node);
            all_nodes[this_contour_node->id()]=this_contour_node;
            all_nodes_[this_contour_node->id()]=this_contour_node;
        }

    }

    // Compile all contour elements
    vcl_map<unsigned int,dbskfg_composite_link_sptr> all_contour_links;

    for (dbskfg_composite_graph::edge_iterator eit = that.edges_begin(); 
         eit != that.edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link=*eit;

        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            dbskfg_composite_link_sptr this_contour_link
                = new dbskfg_contour_link(
                    all_nodes[link->source()->id()],
                    all_nodes[link->target()->id()],
                    link->id());
            this->add_edge(this_contour_link);

            all_nodes[link->source()->id()]->
                add_outgoing_edge(this_contour_link);
            all_nodes[link->target()->id()]->
                add_incoming_edge(this_contour_link);
            
            all_contour_links[this_contour_link->id()]=this_contour_link;
            all_links_[this_contour_link->id()]=this_contour_link;
            
            dbskfg_contour_link* con_link = dynamic_cast<dbskfg_contour_link*>
                (&(*this_contour_link));
            dbskfg_contour_link* that_con_link = 
                dynamic_cast<dbskfg_contour_link*>
                (&(*link));

            con_link->set_original_contour_id(that_con_link->contour_id());
        }

    }

    for (dbskfg_composite_graph::edge_iterator eit = that.edges_begin(); 
         eit != that.edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link=*eit;

        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* that_shock = dynamic_cast<dbskfg_shock_link*>
                (&(*link));

            // Add the shock link into the graph
            dbskfg_composite_link_sptr this_shock =
                new dbskfg_shock_link(
                    all_nodes[link->source()->id()],
                    all_nodes[link->target()->id()],
                    link->id(),
                    dbskfg_shock_link::SHOCK_EDGE,
                    that_shock->fragment_type());
            this->add_edge(this_shock);

            all_nodes[link->source()->id()]->
                add_outgoing_edge(this_shock);
            all_nodes[link->target()->id()]->
                add_incoming_edge(this_shock);
          
            dbskfg_shock_link* this_shock_edge = 
                dynamic_cast<dbskfg_shock_link*>(&(*this_shock));
       
            if ( that_shock->get_left_point() )
            {  
                unsigned int id = that_shock->get_left_point()->id();

                dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>
                    (&(*all_nodes[id]));

                cnode->shock_links_affected(this_shock_edge);
                this_shock_edge->add_left_point(cnode);          
            }


            if ( that_shock->get_right_point() )
            {
                unsigned int id = that_shock->get_right_point()->id();

                dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>
                    (&(*all_nodes[id]));

                cnode->shock_links_affected(this_shock_edge);
                this_shock_edge->add_right_point(cnode);     
                          
            }

            vcl_vector<dbskfg_composite_link_sptr>& left_links =
                that_shock->left_contour_links();
            
            for ( unsigned int i=0; i < left_links.size() ; ++i)
            {
                dbskfg_contour_link* this_clink = 
                    dynamic_cast<dbskfg_contour_link*>
                    (&(*all_contour_links[left_links[i]->id()]));
                this_clink->shock_links_affected(this_shock_edge);
                this_shock_edge->add_to_left_contour(this_clink);
            }

            vcl_vector<dbskfg_composite_link_sptr>& right_links =
                that_shock->right_contour_links();

            for ( unsigned int i=0; i < right_links.size() ; ++i)
            {
                dbskfg_contour_link* this_clink = 
                    dynamic_cast<dbskfg_contour_link*>
                    (&(*all_contour_links[right_links[i]->id()]));
                this_clink->shock_links_affected(this_shock_edge);
                this_shock_edge->add_to_right_contour(this_clink);
            }

            this_shock_edge->set_polygon(that_shock->polygon());
            this_shock_edge->set_left_boundary(that_shock->get_left_boundary());
            this_shock_edge->
                set_right_boundary(that_shock->get_right_boundary());
            if ( that.construct_locus_ )
            {
                this_shock_edge->construct_locus();
            }
            all_links_[this_shock->id()]=this_shock;
        }
  
    }
}


unsigned int dbskfg_composite_graph::next_available_id()
{
    next_available_id_++;
    return next_available_id_;


}

dbskfg_composite_node_sptr dbskfg_composite_graph::find_node(unsigned int id)
{
    if ( all_nodes_.count(id) )
    {
        
        return all_nodes_[id];
    }
    else
    {
        for (dbskfg_composite_graph::vertex_iterator vit 
                 = this->vertices_begin(); 
             vit != this->vertices_end(); ++vit)
        {
            dbskfg_composite_node_sptr node = (*vit);
       
            if ( node->id() == id)
            {
                
                return node;

            }
        }

    }

    return 0;
}

dbskfg_composite_link_sptr dbskfg_composite_graph::find_link(unsigned int id)
{
    if ( all_links_.count(id) )
    {
        
        return all_links_[id];
    }
    else
    {
        for (dbskfg_composite_graph::edge_iterator vit = this->edges_begin(); 
             vit != this->edges_end(); ++vit)
        {
            dbskfg_composite_link_sptr link = (*vit);
       
            if ( link->id() == id)
            {

                return link;

            }

        }
    }
    return 0;
}
