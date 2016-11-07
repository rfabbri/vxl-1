// This is brcv/shp/dbskfg/algo/dbskfg_region_growing_transforms

//:
// \file
#include <dbskfg/algo/dbskfg_region_growing_transforms.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <vcl_algorithm.h>

dbskfg_region_growing_transforms::dbskfg_region_growing_transforms
( 
    dbskfg_rag_graph_sptr rag_graph
)
    :rag_graph_(rag_graph),
     adjacency_flag_(true)
{
    

}

dbskfg_region_growing_transforms::~dbskfg_region_growing_transforms()
{

}

void dbskfg_region_growing_transforms::grow_regions(unsigned int id)
{
    // Destroy previous tree
    rag_graph_->destroy_map();

    // Lets keep a set of visited links
    vcl_map<unsigned int,dbskfg_rag_node_sptr*> visited_nodes;
    vcl_map<unsigned int,dbskfg_rag_node_sptr*>::iterator mit;

    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {
        if ( id != 0 )
        {
            if ( id == (*vit)->id())
            {
                (*vit)->update_outer_shock_nodes();
            }
        }
  
        visited_nodes[(*vit)->id()]=&(*vit);

    }

    for ( mit  = visited_nodes.begin() ; mit != visited_nodes.end() ; 
          ++ mit)
    {
        if ( (*mit).second != 0 )
        {
            dbskfg_rag_node_sptr rag_node=*(*mit).second;
            if ( !rag_node->endpoint_spawned_node())
            {
                expand_rag_node(*(*mit).second,visited_nodes);
            }
        }


    }
  
    vcl_cout<<"Rag graph vertices after Growing: "<<
        rag_graph_->number_of_vertices()<<vcl_endl;


    // push back node 
    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {  
        (*vit)->prune_wavefront_nodes();
        
        vcl_set<unsigned int> rag_con_ids;
        (*vit)->rag_contour_ids(rag_con_ids);
        vcl_set<unsigned int>::iterator sit;
        for ( sit = rag_con_ids.begin() ; sit != rag_con_ids.end() ; ++sit)
        {
            if ( !(*vit)->endpoint_spawned_node())
            {
                rag_graph_->contour_region_map(*sit,(*vit)->id());
            }
        }
    }

    

}


void dbskfg_region_growing_transforms::grow_regions(
    vcl_vector<unsigned int>& regions_to_grow)
{
    // Destroy previous tree
    rag_graph_->destroy_map();

    // Lets keep a set of visited links
    vcl_map<unsigned int,dbskfg_rag_node_sptr*> visited_nodes;
    vcl_map<unsigned int,dbskfg_rag_node_sptr*>::iterator mit;

    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {
        visited_nodes[(*vit)->id()]=&(*vit);

    }

    for ( mit  = visited_nodes.begin() ; mit != visited_nodes.end() ; 
          ++ mit)
    {
       
        if ( (*mit).second != 0 )
        {
            dbskfg_rag_node_sptr rag_node=*(*mit).second;
            if ( !rag_node->endpoint_spawned_node())
            {
                expand_rag_node(*(*mit).second,visited_nodes);
            }
        }


    }

    for ( unsigned int k=0; k < regions_to_grow.size() ; ++k)
    {
        unsigned int id_find = regions_to_grow[k];
        dbskfg_rag_node_sptr rag_node = rag_graph_->rag_node(id_find);
        
        vcl_set<unsigned int> rag_con_ids;
        rag_node->rag_contour_ids(rag_con_ids);
        vcl_set<unsigned int>::iterator sit;
        for ( sit = rag_con_ids.begin() ; sit != rag_con_ids.end() ; ++sit)
        {
            if ( !rag_node->endpoint_spawned_node())
            {
                rag_graph_->contour_region_map(*sit,rag_node->id());
            }
        }
        
    }
      
    vcl_cout<<"Rag graph vertices after Growing: "<<
        rag_graph_->number_of_vertices()<<vcl_endl;


    // push back node 
    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {  
        (*vit)->prune_wavefront_nodes();

        // vcl_set<unsigned int> rag_con_ids;
        // (*vit)->rag_contour_ids(rag_con_ids);
        // vcl_set<unsigned int>::iterator sit;
        // for ( sit = rag_con_ids.begin() ; sit != rag_con_ids.end() ; ++sit)
        // {
        //     if ( !(*vit)->endpoint_spawned_node())
        //     {
        //         rag_graph_->contour_region_map(*sit,(*vit)->id());
        //     }
        // }
    }

    

}

dbskfg_rag_node_sptr dbskfg_region_growing_transforms::grow_region(
    unsigned int id)
{


    // Grow region
    rag_graph_->destroy_map();

    // Find region
    dbskfg_rag_node_sptr rag_node(0);

    // Lets keep a set of visited links
    vcl_map<unsigned int,dbskfg_rag_node_sptr*> visited_nodes;
    vcl_map<unsigned int,dbskfg_rag_node_sptr*>::iterator mit;

    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {
        if ( (*vit)->id() == id )
        {
            rag_node = (*vit);
        }

        visited_nodes[(*vit)->id()]=&(*vit);

    }

    rag_node->update_outer_shock_nodes();

    expand_rag_node(rag_node,visited_nodes);
  
    rag_node->prune_wavefront_nodes();

    return rag_node;

}

void dbskfg_region_growing_transforms::
expand_rag_node(dbskfg_rag_node_sptr rag_node,
                vcl_map<unsigned int,dbskfg_rag_node_sptr*>& visited_nodes)
{
    
    
    // Grab wavefront
    vcl_map<unsigned int,dbskfg_shock_node*> wavefront = 
        rag_node->get_wavefront();

    // Put all nodes of wavefront on a stack
    vcl_vector<dbskfg_shock_node*> stack;
    vcl_map<unsigned int,dbskfg_shock_node*>::iterator mit;
    for ( mit = wavefront.begin() ; mit != wavefront.end() ; ++mit)
    {

        stack.push_back((*mit).second);
    }

    while(!stack.empty())
    {
        // Pop of current node
        dbskfg_shock_node* node = stack.back();
        stack.pop_back();

        // Expand this node
        expand_wavefront(node,rag_node,stack,visited_nodes);

    }

}

void dbskfg_region_growing_transforms::
expand_wavefront(dbskfg_shock_node* snode, 
                 dbskfg_rag_node_sptr rag_node,
                 vcl_vector<dbskfg_shock_node*>& stack,
                 vcl_map<unsigned int,dbskfg_rag_node_sptr*>& 
                 visited_nodes)
{

    dbskfg_shock_node::edge_iterator in;
    dbskfg_shock_node::edge_iterator out;

    // Look at in edges first
    for ( in = snode->in_edges_begin() ; in != snode->in_edges_end() 
              ; ++in)
    {
        dbskfg_composite_link_sptr link = *in;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK)
        {

            dbskfg_shock_link* slink=
                dynamic_cast<dbskfg_shock_link*>(&(*link));
           
            if ( slink->shock_link_type() == dbskfg_shock_link::SHOCK_EDGE )
            {
                if ( slink->get_rag_node()->id() != rag_node->id() )
                {

                    // This shock link will already be adjacent we just
                    // need to make sure the contours and endpoint conditions 
                    if ( rag_node->is_adjacent(slink))
                    {
                     
                        // Add this new shock link
                        rag_node->add_shock_link(slink);
                
                        // Delete from vertex
                        visited_nodes[slink->get_rag_node()->id()]=0;

                        // In the rag graph keep this merge order
                        rag_graph_->rag_ids_map(slink->get_rag_node()->id(),
                                                rag_node->id());

                        // Delete from graph
                        rag_graph_->remove_vertex(slink->get_rag_node());
                        
                        // Set the rag node for this shock link
                        slink->set_rag_node(&(*rag_node));

                        // Push onto stack opposite node
                        dbskfg_composite_node_sptr new_node = 
                            slink->opposite(snode);

                        if ( new_node->node_type()== 
                             dbskfg_composite_node::SHOCK_NODE )
                        {
                            dbskfg_shock_node* new_snode =
                                dynamic_cast<dbskfg_shock_node*>(&(*new_node));
                            stack.push_back(new_snode);

                        }
                    }
                }
            }
        }
    }
    
    // Look at in edges first
    for ( out = snode->out_edges_begin() ; out != snode->out_edges_end() 
              ; ++out)
    {

        dbskfg_composite_link_sptr link = *out;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK)
        {
            dbskfg_shock_link* slink=dynamic_cast<dbskfg_shock_link*>
                (&(*link));

            if ( slink->shock_link_type() == dbskfg_shock_link::SHOCK_EDGE )
            {

                if ( slink->get_rag_node()->id() != rag_node->id() )
                {

                    // This shock link will already be adjacent we just
                    // need to make sure the contours and endpoint conditions
                    if ( rag_node->is_adjacent(slink))
                    {
                        // Add this new shock link
                        rag_node->add_shock_link(slink);
                
                        // Delete from vertex
                        visited_nodes[slink->get_rag_node()->id()]=0;
                        
                        // In the rag graph keep this merge order
                        rag_graph_->rag_ids_map(slink->get_rag_node()->id(),
                                                rag_node->id());

                        // Delete from graph
                        rag_graph_->remove_vertex(slink->get_rag_node());
                        
                        // Set the rag node for this shock link
                        slink->set_rag_node(&(*rag_node));

                        // Push onto stack opposite node
                        dbskfg_composite_node_sptr new_node = 
                            slink->opposite(snode);

                        if ( new_node->node_type()== 
                             dbskfg_composite_node::SHOCK_NODE )
                        {
                            dbskfg_shock_node* new_snode =
                                dynamic_cast<dbskfg_shock_node*>(&(*new_node));
                            stack.push_back(new_snode);

                        }

                    }
 
                }
            }
        }

    }

}
