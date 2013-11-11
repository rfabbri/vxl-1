// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph_streamlined.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_compute_composite_graph_streamlined.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/algo/dbskfg_region_growing_transforms.h>
// dbsk2d headers
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
// vcl headers
#include <vcl_sstream.h>
#include <vcl_limits.h>
// vsol headers
#include <vsol/vsol_point_2d_sptr.h>
// vgl headers
#include <vgl/vgl_lineseg_test.h>
#include <vgl/vgl_distance.h>

dbskfg_compute_composite_graph_streamlined::
dbskfg_compute_composite_graph_streamlined
( 
    dbsk2d_ishock_graph_sptr ishock_graph,
    dbskfg_composite_graph_sptr composite_graph,
    dbskfg_rag_graph_sptr rag_graph,
    bool adjacency_flag
)
 :composite_graph_(composite_graph),
  ishock_graph_(ishock_graph),
  rag_graph_(rag_graph),
  adjacency_flag_(adjacency_flag)
{
}

dbskfg_compute_composite_graph_streamlined::
~dbskfg_compute_composite_graph_streamlined()
{
    composite_graph_=0;
    ishock_graph_=0;
    rag_graph_=0;

    vcl_map<int,dbskfg_composite_link_sptr>::iterator it;
    for ( it = boundary_link_mapping_.begin() ; 
          it != boundary_link_mapping_.end()
              ; ++it )
    {
        (*it).second = 0;

    }

    boundary_link_mapping_.clear();
    
    vcl_map<int,dbskfg_composite_node_sptr>::iterator nit;
    for (nit = boundary_node_mapping_.begin(); 
         nit != boundary_node_mapping_.end(); ++nit)
    {

        (*nit).second = 0;
    }

    boundary_node_mapping_.clear();
        
}

bool dbskfg_compute_composite_graph_streamlined::compile_composite_graph(
dbsk2d_boundary_sptr boundary)
{
    // Keep track of status
    bool status_flag = true;

    // clear map before we start
    boundary_link_mapping_.clear();
    boundary_node_mapping_.clear();

    // 1. Lets compile all contour elements
    compile_contour_elements(boundary);

    // 2. Lets compile all shock elements
    compile_shock_elements();

    // Clear everything out right now
    vcl_map<int,dbskfg_composite_link_sptr>::iterator it;
    for ( it = boundary_link_mapping_.begin() ; 
          it != boundary_link_mapping_.end()
              ; ++it )
    {
        (*it).second = 0;

    }

    boundary_link_mapping_.clear();
    
    vcl_map<int,dbskfg_composite_node_sptr>::iterator nit;
    for (nit = boundary_node_mapping_.begin(); 
         nit != boundary_node_mapping_.end(); ++nit)
    {
        (*nit).second = 0;
    }

    boundary_node_mapping_.clear();
   
    //3: Lets classify nodes
    classify_nodes();

    vcl_cout<<"Finished classifying nodes"<<vcl_endl;

    //4. Lets compile all shock fragments
    compile_shock_fragments();
    
    dbskfg_region_growing_transforms region_growing(rag_graph_);
    region_growing.set_adjacency(adjacency_flag_);
    region_growing.grow_regions();

    vcl_cout<<"Printing out vertices: "<<
        composite_graph_->number_of_vertices()<<vcl_endl;

    vcl_cout<<"Printing out edges: "<<composite_graph_->number_of_edges()
            <<vcl_endl;

    vcl_cout<<"Number of regions: "<<rag_graph_->number_of_vertices()
            <<vcl_endl;

   
    return status_flag;
}

void dbskfg_compute_composite_graph_streamlined::compile_contour_elements(
    dbsk2d_boundary_sptr boundary)
{
    // Grap all boundary elements
    vcl_vector<dbsk2d_ishock_belm* > belm_list = boundary->belm_list();

    // Keep a local reference of all line ids seen
    vcl_map<unsigned int, unsigned int> lines_visited;
    vcl_map<int,dbskfg_composite_node_sptr> nodes_visited;

    for ( unsigned int i=0; i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_line() )
        {
            dbsk2d_ishock_bline* line_element = 
                dynamic_cast<dbsk2d_ishock_bline*>(belm_list[i]);
            unsigned int line_id      = line_element->id();
            unsigned int twin_line_id = line_element->twinLine()->id();
            
            const vcl_list<vtol_topology_object*>* contour_list= 
            line_element->bnd_edge()->superiors_list();
            unsigned int conid=contour_list->front()->get_id();
         
            if ( lines_visited.count(line_id)==0 &&
                 lines_visited.count(twin_line_id)==0)
            {

                dbskfg_composite_node_sptr start_cnode(0);
                dbskfg_composite_node_sptr end_cnode(0);

                if ( nodes_visited.count(line_element->s_pt()->id())==0)
                {
                    start_cnode = 
                        new dbskfg_contour_node(
                            composite_graph_->next_available_id(),
                            line_element->s_pt()->pt(),
                            0);
                    composite_graph_->add_vertex(start_cnode);
                    nodes_visited[line_element->s_pt()->id()]=start_cnode;
                }
                else
                {
                    start_cnode=nodes_visited[line_element->s_pt()->id()];
                }

                if ( nodes_visited.count(line_element->e_pt()->id())==0)
                {
                    end_cnode = 
                        new dbskfg_contour_node(
                            composite_graph_->next_available_id(),
                            line_element->e_pt()->pt(),
                            0);
                    composite_graph_->add_vertex(end_cnode);
                    nodes_visited[line_element->e_pt()->id()]=end_cnode;
                }
                else
                {
                    end_cnode=nodes_visited[line_element->e_pt()->id()];
                }

                // Create first link
                dbskfg_composite_link_sptr contour_link =
                    new dbskfg_contour_link(
                        start_cnode,
                        end_cnode,
                        composite_graph_->next_available_id());
                composite_graph_->add_edge(contour_link);

                dbskfg_contour_link* clink = 
                    dynamic_cast<dbskfg_contour_link*>(&(*contour_link));
                clink->set_original_contour_id(conid);
                
                dbskfg_contour_node* scnode=
                           dynamic_cast<dbskfg_contour_node*>(
                               &(*clink->source()));
                scnode->set_contour_id(conid);

                dbskfg_contour_node* tcnode=
                    dynamic_cast<dbskfg_contour_node*>(
                        &(*clink->target()));
                tcnode->set_contour_id(conid);

                
                start_cnode->add_outgoing_edge(contour_link);
                end_cnode->add_incoming_edge(contour_link);

                lines_visited[line_id]     =0;
                lines_visited[twin_line_id]=0;
             
                // Keep track of original id
                boundary_link_mapping_[line_id]=contour_link;
                boundary_link_mapping_[twin_line_id]=contour_link;
             
                // Keep track of original id for contour start and end points
                boundary_node_mapping_[line_element->s_pt()->id()]=start_cnode;
                boundary_node_mapping_[line_element->e_pt()->id()]=end_cnode;
                
            }

        }
        else
        {
            dbsk2d_ishock_bpoint* point_element = 
                dynamic_cast<dbsk2d_ishock_bpoint*>(belm_list[i]);
         
            if ( boundary_node_mapping_.count(point_element->id())==0)
            {
                dbskfg_composite_node_sptr single_point = 
                    new dbskfg_contour_node(
                        composite_graph_->next_available_id(),
                        point_element->pt(),
                        point_element->id());
                composite_graph_->add_vertex(single_point);
                boundary_node_mapping_[point_element->id()]=single_point;
            }
          

        }
    }

}

void dbskfg_compute_composite_graph_streamlined::compile_shock_elements()
{

    // Keep a local map of ids encountered
    vcl_map<int,dbskfg_composite_node_sptr> temp_shock_node_map;

    // Iterate thru all edges
    dbsk2d_ishock_graph::edge_iterator eit;
    for ( eit = ishock_graph_->all_edges().begin(); 
          eit != ishock_graph_->all_edges().end();
          ++eit)
    {
        dbsk2d_ishock_edge* edge = *eit;
    
        if ( edge->cSNode() != 0  && edge->pSNode() != 0 && edge->isNotHidden())
        {
           
            dbskfg_composite_node_sptr source_node(0);
            dbskfg_composite_node_sptr target_node(0);

            // Determine if source node is a3
            if ( edge->lBElement()->is_a_line() && 
                 edge->rBElement()->is_a_line())
            {
                dbskfg_composite_link_sptr left_link =
                    boundary_link_mapping_[edge->lBElement()->id()];
                dbskfg_composite_link_sptr right_link =
                    boundary_link_mapping_[edge->rBElement()->id()];

                if ( left_link->shared_vertex(*right_link) )
                {
                    source_node = left_link->shared_vertex(*right_link);
                }
             
            }
            
          
            // Determine whether we need to rechange source node and target
            // node
            if ( !source_node )
            { 
              
                if ( temp_shock_node_map.count(edge->pSNode()->id())==0)
                {
                    source_node=
                        new dbskfg_shock_node(
                            composite_graph_->next_available_id(),
                            edge->pSNode()->origin(),
                            edge->pSNode()->startTime());
                    composite_graph_->add_vertex(source_node);
                    temp_shock_node_map[edge->pSNode()->id()]=source_node;
                
                }
                else
                {
                    source_node = temp_shock_node_map[edge->pSNode()->id()];
                }
            }

            // Determine whether we need to rechange target node
            if ( temp_shock_node_map.count(edge->cSNode()->id())==0)
            {
                target_node=
                    new dbskfg_shock_node(
                        composite_graph_->next_available_id(),
                        edge->cSNode()->origin(),
                        edge->cSNode()->startTime());
                composite_graph_->add_vertex(target_node);
                temp_shock_node_map[edge->cSNode()->id()]=target_node;
            } 
            else
            {
                target_node = temp_shock_node_map[edge->cSNode()->id()];
            }


            // // Determine fragment type
            dbskfg_utilities::Fragment_Type frag_type;

            frag_type = 
                ( source_node->node_type() == 
                  dbskfg_composite_node::CONTOUR_NODE ||
                  target_node->node_type() == 
                  dbskfg_composite_node::CONTOUR_NODE )
                ? dbskfg_utilities::DEGENERATE : dbskfg_utilities::REGULAR ;

            // Add the shock link into the graph
            dbskfg_composite_link_sptr shock_edge =
                new dbskfg_shock_link(
                    source_node,
                    target_node,
                    composite_graph_->next_available_id(),
                    dbskfg_shock_link::SHOCK_EDGE,
                    frag_type);
            composite_graph_->add_edge(shock_edge);

            source_node->add_outgoing_edge(shock_edge);
            target_node->add_incoming_edge(shock_edge);


            // Determine type of shock this is 
            dbskfg_shock_link_boundary left_boundary,right_boundary;

            // Create shock link
            dbskfg_shock_link* slink=
                dynamic_cast<dbskfg_shock_link*>(&(*shock_edge));

            //Line/Line
            if ( edge->lBElement()->is_a_line() && 
                 edge->rBElement()->is_a_line())
            {
                slink->add_to_left_contour(
                    boundary_link_mapping_[edge->lBElement()->id()]);
                slink->add_to_right_contour(
                    boundary_link_mapping_[edge->rBElement()->id()]);
       
                // create polygon
                vgl_polygon<double> poly(1);

                poly.push_back(source_node->pt());
                poly.push_back(edge->getLFootPt(edge->sTau()));
                poly.push_back(edge->getLFootPt(edge->eTau()));
                poly.push_back(target_node->pt());
                poly.push_back(edge->getRFootPt(edge->eTau()));
                poly.push_back(edge->getRFootPt(edge->sTau()));
                slink->set_polygon(poly);

                left_boundary.contour_.push_back(
                    edge->getLFootPt(edge->sTau()));
                left_boundary.contour_.push_back(
                    edge->getLFootPt(edge->eTau()));
        
                right_boundary.contour_.push_back(
                    edge->getRFootPt(edge->eTau()));
                right_boundary.contour_.push_back(
                    edge->getRFootPt(edge->sTau()));
        
                dbskfg_contour_link* lclink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*boundary_link_mapping_[edge->lBElement()->id()]));
                dbskfg_contour_link* rclink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*boundary_link_mapping_[edge->rBElement()->id()]));
                lclink->shock_links_affected(slink);
                rclink->shock_links_affected(slink);

            }
            //Left line/Right Point
            else if ( edge->lBElement()->is_a_line() && 
                      edge->rBElement()->is_a_point())
            {
                // Right Point
                vgl_point_2d<double> right_point = 
                    reinterpret_cast<dbsk2d_ishock_bpoint*>
                    (edge->rBElement())->pt();
                dbskfg_contour_node* cnode_right = 
                    dynamic_cast<dbskfg_contour_node*>
                    (&(*boundary_node_mapping_[edge->rBElement()->id()]));
 
                slink->add_to_left_contour(
                    boundary_link_mapping_[edge->lBElement()->id()]);
                slink->add_right_point(cnode_right);

                // create polygon
                vgl_polygon<double> poly(1);

                poly.push_back(source_node->pt());
                poly.push_back(edge->getLFootPt(edge->sTau()));
                poly.push_back(edge->getLFootPt(edge->eTau()));
                poly.push_back(target_node->pt());
                poly.push_back(right_point);
                slink->set_polygon(poly);

                right_boundary.point_ = right_point;
                left_boundary.contour_.push_back(
                    edge->getLFootPt(edge->sTau()));
                left_boundary.contour_.push_back(
                    edge->getLFootPt(edge->eTau()));

                dbskfg_contour_link* lclink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*boundary_link_mapping_[edge->lBElement()->id()]));
                lclink->shock_links_affected(slink);
                cnode_right->shock_links_affected(slink);

            }
            //Right Line/Left Point
            else if ( edge->lBElement()->is_a_point() && 
                      edge->rBElement()->is_a_line())
            {       
                // Left Point
                vgl_point_2d<double> left_point = 
                    reinterpret_cast<
                    dbsk2d_ishock_bpoint*>(edge->lBElement())->pt();
                dbskfg_contour_node* cnode_left = 
                    dynamic_cast<dbskfg_contour_node*>
                    (&(*boundary_node_mapping_[edge->lBElement()->id()]));
                slink->add_left_point(cnode_left);
                slink->add_to_right_contour(
                    boundary_link_mapping_[edge->rBElement()->id()]);
                
      
                // create polygon
                vgl_polygon<double> poly(1);

                poly.push_back(source_node->pt());
                poly.push_back(edge->getRFootPt(edge->sTau()));
                poly.push_back(edge->getRFootPt(edge->eTau()));
                poly.push_back(target_node->pt());
                poly.push_back(left_point);
                slink->set_polygon(poly);

                left_boundary.point_ = left_point;
                right_boundary.contour_.push_back(
                    edge->getRFootPt(edge->eTau()));
                right_boundary.contour_.push_back(
                    edge->getRFootPt(edge->sTau()));
                
                dbskfg_contour_link* rclink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*boundary_link_mapping_[edge->rBElement()->id()]));
                rclink->shock_links_affected(slink);
                cnode_left->shock_links_affected(slink);

            }
            else 
            {

                vgl_point_2d<double> left_point = 
                    reinterpret_cast<dbsk2d_ishock_bpoint*>(
                        edge->lBElement())->pt();
                vgl_point_2d<double> right_point = 
                    reinterpret_cast<dbsk2d_ishock_bpoint*>(
                        edge->rBElement())->pt();
                left_boundary.point_  = left_point;
                right_boundary.point_ = right_point;

                dbskfg_contour_node* cnode_left = 
                    dynamic_cast<dbskfg_contour_node*>
                    (&(*boundary_node_mapping_[edge->lBElement()->id()]));
                dbskfg_contour_node* cnode_right = 
                    dynamic_cast<dbskfg_contour_node*>
                    (&(*boundary_node_mapping_[edge->rBElement()->id()]));
                           
                // create polygon
                vgl_polygon<double> poly(1);

                poly.push_back(source_node->pt());
                poly.push_back(left_point);
                poly.push_back(target_node->pt());
                poly.push_back(right_point);   
                slink->set_polygon(poly);

                left_boundary.point_ = left_point;
                right_boundary.point_ = right_point;

                slink->add_left_point(cnode_left);
                slink->add_right_point(cnode_right);
                
                cnode_left->shock_links_affected(slink);
                cnode_right->shock_links_affected(slink);

            }
            
            slink->set_left_boundary(left_boundary);
            slink->set_right_boundary(right_boundary);
            if ( composite_graph_->construct_locus())
            {
                slink->construct_locus();
            }
        }
    }
  
}

void dbskfg_compute_composite_graph_streamlined::compile_shock_fragments()
{

    for (dbskfg_composite_graph::edge_iterator eit =
             composite_graph_->edges_begin();
         eit != composite_graph_->edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link = *eit;

        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
  
            dbskfg_shock_link* shock_link=
                dynamic_cast<dbskfg_shock_link*>(&(*link));
            if ( shock_link->shock_link_type() == 
                 dbskfg_shock_link::SHOCK_EDGE )
            {
       
                
                dbskfg_rag_node_sptr rag_node = new dbskfg_rag_node(
                    rag_graph_->next_available_id());

                rag_node->add_shock_link(shock_link);

                rag_graph_->add_vertex(rag_node);
        
                shock_link->set_rag_node(&(*rag_node));
          }
      }
  }
}

void dbskfg_compute_composite_graph_streamlined::classify_nodes()
{

    for (dbskfg_composite_graph::vertex_iterator vit = 
             composite_graph_->vertices_begin(); 
         vit != composite_graph_->vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = *vit;
        if ( node->node_type() == dbskfg_composite_node::CONTOUR_NODE )
        {
            unsigned int contour_degree(0);
            dbskfg_composite_node::edge_iterator eit;

            vcl_vector<dbskfg_composite_link_sptr> edges;

            for (eit = (*vit)->out_edges_begin(); 
                 eit != (*vit)->out_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::CONTOUR_LINK)
                {
                    contour_degree++;
                }
            }

            for (eit = (*vit)->in_edges_begin(); 
                 eit != (*vit)->in_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::CONTOUR_LINK)
                {
                    contour_degree++;
                }
            }

            (*vit)->set_composite_degree(contour_degree);
        }
        else
        {
            unsigned int shock_degree(0);
            dbskfg_composite_node::edge_iterator eit;
            
            for (eit = (*vit)->out_edges_begin(); 
                 eit != (*vit)->out_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::SHOCK_LINK)
                {
                    dbskfg_composite_link_sptr link=*eit;
                    dbskfg_shock_link* slink=
                        dynamic_cast<dbskfg_shock_link*>(&(*link));
                    if ( slink->shock_link_type() 
                         == dbskfg_shock_link::SHOCK_EDGE)
                    {
                        shock_degree++;
                    }
                }
            }

            for (eit = (*vit)->in_edges_begin(); 
                 eit != (*vit)->in_edges_end() ; ++eit)
            {

                if ( (*eit)->link_type() 
                     == dbskfg_composite_link::SHOCK_LINK)
                {
                    dbskfg_composite_link_sptr link=*eit;
                    dbskfg_shock_link* slink=
                        dynamic_cast<dbskfg_shock_link*>(&(*link));
                    if ( slink->shock_link_type() 
                         == dbskfg_shock_link::SHOCK_EDGE)
                    {
                        shock_degree++;
                    }
                }
            }
            
            (*vit)->set_composite_degree(shock_degree);

        }
    }


}
