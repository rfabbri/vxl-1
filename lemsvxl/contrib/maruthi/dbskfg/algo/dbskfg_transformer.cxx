// This is brcv/shp/dbskfg/algo/dbskfg_transformer.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_transformer.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_form_composite_graph_process.h>

#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <dbskfg/dbskfg_rag_graph.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/dbsk2d_boundary.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vgl/vgl_area.h>
#include <vgl/vgl_distance.h>

dbskfg_transformer::dbskfg_transformer
( 
    dbskfg_transform_descriptor_sptr transform,
    dbskfg_composite_graph_sptr composite_graph,
    dbskfg_rag_graph_sptr rag_graph
):transform_(transform),
  original_graph_(composite_graph),
  local_graph_(0),
  shock_storage_(0),
  rag_graph_(rag_graph),
  status_flag_(true)
{
    transform_->compute_polygon_string();

    // Lets populate contour nodes and links
    for ( unsigned int i=0 ;i < transform_->contours_affected_.size(); ++i)
    {
        orig_contour_links_[transform_->contours_affected_[i]->id()]=
            transform_->contours_affected_[i];
        vcl_stringstream source_stream;
        vcl_stringstream target_stream;
        source_stream<<
            transform_->contours_affected_[i]->source()->pt();

        dbskfg_contour_node* source_cnode =
            dynamic_cast<dbskfg_contour_node*>
            (&(*transform_->contours_affected_[i]->source()));
     
        vcl_vector<dbskfg_shock_link*> shocks = 
            source_cnode->shocks_affected();
        
        if ( shocks.size() == 0 )
        {
            // This node affects nothing, so we have to see whether in polygon
            if ( transform_->polygon_string_rep_.count(source_stream.str())
                 ||
                 transform_->poly_.contains(
                     source_cnode->pt().x(),
                     source_cnode->pt().y()))
            {
                all_nodes_[source_stream.str()]=
                            transform_->contours_affected_[i]->source();
            }
        }
        else
        {
            bool flag=false;
            for ( unsigned int s=0 ; s < shocks.size() ; ++s)
            {
              
                if ( dbskfg_utilities::is_link_in_set_of_links(
                         transform_->shock_links_affected_,
                         shocks[s] ))
                {
                    all_nodes_[source_stream.str()]=
                        transform_->contours_affected_[i]->source();
                    flag=true;
                    break;
                }
            }
            
            if ( !flag )
            {
                
                dbskfg_composite_link_sptr degen_shock
                    = source_cnode->degenerate_shock();
                
                if ( degen_shock )
                {
                    if ( dbskfg_utilities::is_link_in_set_of_links(
                             transform_->shock_links_affected_,
                             degen_shock) || 
                         transform_->polygon_string_rep_.count(
                             source_stream.str())
                         ||
                         transform_->poly_.contains(
                             source_cnode->pt().x(),
                             source_cnode->pt().y())
                        )
                    {
                        all_nodes_[source_stream.str()]=
                            transform_->contours_affected_[i]->source();
                        
                    }
                }
                else if (  
                    transform_->polygon_string_rep_.count(source_stream.str())
                    ||
                    transform_->poly_.contains(
                        source_cnode->pt().x(),
                        source_cnode->pt().y()))
                {
                    all_nodes_[source_stream.str()]=
                        transform_->contours_affected_[i]->source();
                        
                }
            }
          
        }
        

        target_stream<<
            transform_->contours_affected_[i]->target()->pt();

        dbskfg_contour_node* target_cnode =
            dynamic_cast<dbskfg_contour_node*>
            (&(*transform_->contours_affected_[i]->target()));
       
        vcl_vector<dbskfg_shock_link*> tshocks = 
            target_cnode->shocks_affected();

        if ( tshocks.size() == 0 )
        {
            // This node affects nothing, so we have to see whether in polygon
            if (transform_->polygon_string_rep_.count(target_stream.str())
                || 
                transform_->poly_.contains(
                    target_cnode->pt().x(),
                    target_cnode->pt().y()))
            {
                all_nodes_[target_stream.str()]=
                            transform_->contours_affected_[i]->target();
            }
        }
        else
        {
            bool flag=false;
            for ( unsigned int s=0 ; s < tshocks.size() ; ++s)
            {
                if ( dbskfg_utilities::is_link_in_set_of_links(
                         transform_->shock_links_affected_,
                         tshocks[s] ))
           
                {
                    all_nodes_[target_stream.str()]=
                        transform_->contours_affected_[i]->target();
                    flag=true;
                    break;
                }
            }

            if ( !flag )
            {
                    
                dbskfg_composite_link_sptr degen_shock
                    = target_cnode->degenerate_shock();
            
                if ( degen_shock)
                {
                    if ( dbskfg_utilities::is_link_in_set_of_links(
                             transform_->shock_links_affected_,
                             degen_shock) ||
                         transform_->polygon_string_rep_.count(
                             target_stream.str())
                         ||
                         transform_->poly_.contains(
                             target_cnode->pt().x(),
                             target_cnode->pt().y()))

                    {
                        all_nodes_[target_stream.str()]=
                            transform_->contours_affected_[i]->target();
                 
                    }
                }
                else if (transform_->polygon_string_rep_.count(
                             target_stream.str())
                         ||
                         transform_->poly_.contains(
                             target_cnode->pt().x(),
                             target_cnode->pt().y()))
                {
                    all_nodes_[target_stream.str()]=
                        transform_->contours_affected_[i]->target();
              
                }
            }
        }
      
    }

    // Lets populate contour nodes and links
    for ( unsigned int j=0 ;j < transform_->contours_nodes_affected_.size(); 
          ++j)
    {
        vcl_stringstream stream;
        stream<<transform_->contours_nodes_affected_[j]->pt();
        all_nodes_[stream.str()]=transform_->contours_nodes_affected_[j];
    }

    // Lets populate contour nodes and links
    for ( unsigned int k=0 ;k < transform_->contours_to_remove_.size(); 
          ++k)
    {
        if ( transform_->contours_to_remove_[k]->degree() > 2 )
        {
            vcl_stringstream stream;
            stream<<transform_->contours_to_remove_[k]->pt();
            all_nodes_[stream.str()]=transform_->contours_to_remove_[k];
        }
    }


    // Populate wavefront with outer shock nodes
    for ( unsigned b=0; b < transform_->outer_shock_nodes_.size() ; ++b)
    {
        wavefront_[transform_->outer_shock_nodes_[b]->id()]=vcl_make_pair
            (transform_->outer_shock_nodes_[b],false);


    }
    
    // 1) Compute the local graph based on the contours
    status_flag_ = compute_local_graph();
    
    if ( status_flag_ )
    {
        // 2) Remove existing shock nodes in the graph 
        remove_segs_original_graph();
    
        // 3) Add new contours in optional if loop
        if ( transform_->new_contours_spatial_objects_.size() )
        {
            add_new_contours();
        }

        // 4) Perform the local surgery
        local_surgery();

        // 5) Make sure wavefront is complete after surgery

        // grab shock graph
        dbsk2d_ishock_graph_sptr ishock_graph = shock_storage_->
            get_ishock_graph();
    
        // Iterate thru all edges
        dbsk2d_ishock_graph::edge_iterator eit;
        for ( eit = ishock_graph->all_edges().begin(); 
              eit != ishock_graph->all_edges().end();++eit)
        {
            wavefront_complete((*eit));
        }
        
        // verify surgery
        verify_surgery();
        
        // 6) Classify nodes
        classify_nodes();

        // 7) Add the rag nodes
        add_rag_nodes();

        vcl_cout<<vcl_endl;
        vcl_cout<<"After everything"<<vcl_endl;

        vcl_cout<<"Printing out vertices: "<<
            original_graph_->number_of_vertices()<<vcl_endl;

        vcl_cout<<"Printing out edges: "<<original_graph_->number_of_edges()
                <<vcl_endl;
        vcl_cout<<vcl_endl;
    }
    else
    {

        vcl_cout<<"Error performing transorm "<<transform_->id_<<vcl_endl;
    }
    
 
    shock_storage_=0;
    original_graph_=0;
    if ( local_graph_)
    {
        local_graph_->clear();
    }
    local_graph_=0;
    all_nodes_.clear();
    orig_contour_links_.clear();
    contours_removed_.clear();
    new_contour_links_.clear();
    new_shock_links_.clear();
    wavefront_.clear();
 
}

dbskfg_transformer::~dbskfg_transformer()
{

    shock_storage_=0;
    if ( local_graph_)
    {
        local_graph_->clear();
    }
    local_graph_=0;
    all_nodes_.clear();
    orig_contour_links_.clear();
    contours_removed_.clear();
    new_contour_links_.clear();
    new_shock_links_.clear();
    wavefront_.clear();
   
}

void dbskfg_transformer::remove_segs_original_graph()
{
    
    vcl_map<unsigned int,vcl_string> rag_map;
    vcl_vector<dbskfg_rag_node_sptr> rag_to_delete;

    unsigned int orig_size = transform_->shock_links_affected_.size();

    // Remove extra shcoks
    //remove_extra_shocks();

    // Remove shock links
    vcl_vector<dbskfg_composite_link_sptr> slinks
        = transform_->shock_links_affected_;

    // Test if shock ray lies in vertex
    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {
        for ( unsigned int j=0; j < slinks.size() ; ++j )
        {
            if ( (*vit)->contains_shock(slinks[j]))
            {
                (*vit)->delete_shock(slinks[j]->id());
                if ( rag_map.count((*vit)->id())==0)
                {
                    rag_to_delete.push_back(*vit);
                    rag_map[(*vit)->id()]="temp";
                }
            }
        }      
    }

    for ( unsigned int k=0 ; k < rag_to_delete.size() ; ++k)
    {

        vcl_map<unsigned int,dbskfg_shock_link*> slinks = rag_to_delete[k]->
            get_shock_links();
        
        if ( slinks.size() )
        {
            vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
            for ( it=slinks.begin(); it != slinks.end() ; ++it)
            {
                new_shock_links_[(*it).second->id()]=(*it).second;
            }
        }
        rag_graph_->remove_vertex(rag_to_delete[k]);
    }

    // Remove in contours affected and nodes any shock links that have 
    // been removed
    for ( unsigned int j=0; j < slinks.size() ; ++j )
    {

        for ( unsigned int c=0; c < transform_->contours_affected_.size(); ++c)
        {

            dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
                (&(*transform_->contours_affected_[c]));
            clink->delete_shock(slinks[j]->id());

            // Look at source and target
            dbskfg_contour_node* source = dynamic_cast<dbskfg_contour_node*>
                (&(*clink->source()));
            source->delete_shock(slinks[j]->id());
            
            // Look at source and target
            dbskfg_contour_node* target = dynamic_cast<dbskfg_contour_node*>
                (&(*clink->target()));
            target->delete_shock(slinks[j]->id());


        }

        for ( unsigned int d=0 ; d < transform_->contours_nodes_affected_.size()
                  ; ++d)
        {
            dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>
                (&(*transform_->contours_nodes_affected_[d]));
            cnode->delete_shock(slinks[j]->id());
        }

        for ( unsigned int k=0; k < transform_->all_gaps_.size() ; ++k)
        {
            vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr>
                tgap = transform_->all_gaps_[k];

            // For gap remove all nodes
            dbskfg_contour_node* gap1 = dynamic_cast<dbskfg_contour_node*>
                (&(*tgap.first));
            dbskfg_contour_node* gap2 = dynamic_cast<dbskfg_contour_node*>
                (&(*tgap.second));
            gap1->delete_shock(slinks[j]->id());
            gap2->delete_shock(slinks[j]->id());
        }
    }

    // Remove internal shock nodes
    vcl_vector<dbskfg_composite_node_sptr> snodes
        = transform_->shock_nodes_affected_;
   
    for ( unsigned int j=0; j < snodes.size() ; ++j )
    {
   
        original_graph_->remove_vertex(snodes[j]);
 
    }

  
    for ( unsigned int j=0; j < slinks.size() ; ++j )
    {
        original_graph_->remove_edge(slinks[j]);

    }

    vcl_vector<unsigned int> index_to_remove;

    for ( unsigned int j=0; j < transform_->contours_to_remove_.size();
          ++j)
    {
        if ( transform_->contours_to_remove_[j]->get_composite_degree() >= 3 )
        {
            
            dbskfg_contour_node* test_node =  
                dynamic_cast<dbskfg_contour_node*>(
                    &(*transform_->contours_to_remove_[j]));
            
            dbskfg_composite_node::edge_iterator eit;
            
            vcl_map<unsigned int,vcl_string> other_cids;

            bool flag_delete=true;
            unsigned int num_nodes_to_remove=0;

            for (eit = test_node->out_edges_begin(); 
                 eit != test_node->out_edges_end() ; ++eit)
            {
                if ( (*eit)->link_type() == 
                     dbskfg_composite_link::CONTOUR_LINK )
                {

                    dbskfg_contour_node* cnode = dynamic_cast<
                        dbskfg_contour_node*>(&(*((*eit)->opposite(test_node)
                                                    )));

                    if ( cnode->contour_id() != test_node->contour_id())
                    {
                        other_cids[cnode->contour_id()]="temp";
                    }
                    
                    if (  dbskfg_utilities::is_node_in_set_of_nodes(
                              transform_->contours_to_remove_,
                              cnode))
                    {
                        num_nodes_to_remove++;
                        flag_delete=flag_delete & true;
                    }
                    else
                    {
                        flag_delete=flag_delete & false;
                    }

                }
            }

            for (eit = test_node->in_edges_begin(); 
                 eit != test_node->in_edges_end() ; ++eit)
            {
                if ( (*eit)->link_type() == 
                     dbskfg_composite_link::CONTOUR_LINK )
                {

                    dbskfg_contour_node* cnode = dynamic_cast<
                        dbskfg_contour_node*>(&(*((*eit)->opposite(test_node)
                                                    )));

                    if ( cnode->contour_id() != test_node->contour_id())
                    {
                        other_cids[cnode->contour_id()]="temp";
                    }
                    
                    if (  dbskfg_utilities::is_node_in_set_of_nodes(
                              transform_->contours_to_remove_,
                              cnode))
                    {
                        num_nodes_to_remove++;
                        flag_delete=flag_delete & true;
                    }
                    else
                    {
                        flag_delete=flag_delete & false;
                    }

                }
           

            }
          
            if ( flag_delete)
            {
                index_to_remove.push_back(j);

            }
            else
            {
                unsigned int degree = transform_->contours_to_remove_[j]
                    ->get_composite_degree();
                transform_->contours_to_remove_[j]
                    ->set_composite_degree(degree-num_nodes_to_remove);
            
                test_node->set_contour_id((*other_cids.begin()).first);
            }

        }
        else
        {
            index_to_remove.push_back(j);
        }
    }

    for ( unsigned int k=0;k < index_to_remove.size() ; ++k)
    {
        original_graph_->remove_vertex(transform_->contours_to_remove_[
                                           index_to_remove[k]]);
    }

    if (  transform_->shock_links_affected_.size() > orig_size )
    {
        transform_->shock_links_affected_.erase(
            transform_->shock_links_affected_.begin()+orig_size,
            transform_->shock_links_affected_.end());
    }
}


void dbskfg_transformer::wavefront_complete(dbsk2d_ishock_edge* edge)
{

    // First check if edge is viable
    vcl_stringstream source_stream;
    vcl_stringstream target_stream;

    dbskfg_composite_node_sptr target(0);
    dbskfg_composite_node_sptr source(0);

    dbskfg_composite_node_sptr source_outer_node(0);
    dbskfg_composite_node_sptr target_outer_node(0);
    bool found=false;

    // Grab source node
    if ( edge->pSNode() != 0)
    {
        source_stream<<edge->source()->origin();
        source_outer_node = dbskfg_utilities::get_node_in_set_of_nodes(
            transform_->outer_shock_nodes_,
            edge->source()->origin());
    }
    else
    {
        source_stream<<"temp";
    }
    
    // Grab target node
    if ( edge->cSNode() != 0)
    {
        target_stream<<edge->target()->origin();
        target_outer_node = dbskfg_utilities::get_node_in_set_of_nodes(
            transform_->outer_shock_nodes_,
            edge->target()->origin());
    }
    else
    {
        target_stream<<"temp";
    }

    bool source_outer = source_outer_node &&
        all_nodes_.count(target_stream.str()) == 0;

    bool target_outer = target_outer_node &&
        all_nodes_.count(source_stream.str()) == 0;

    if ( all_nodes_.count(source_stream.str()))
    {
        if (all_nodes_[source_stream.str()]->node_type() == 
            dbskfg_composite_node::CONTOUR_NODE)
        {
            return;
        }
        
    }

    if ( all_nodes_.count(target_stream.str()))
    {
        if (all_nodes_[target_stream.str()]->node_type() == 
            dbskfg_composite_node::CONTOUR_NODE)
        {
            return;
        }
        
    }

    vcl_map<double,vcl_pair<unsigned int,
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr> >
        > distance_map;
    vcl_map<double,bool> case_to_consider;

    if ( source_outer || target_outer || 
         ((all_nodes_.count(source_stream.str()) == 1 ) != 
          (all_nodes_.count(target_stream.str()) == 1 ) ))
    {

        vcl_map<unsigned int,vcl_pair<dbskfg_composite_node_sptr,bool> >
            ::iterator it;

        for ( it = wavefront_.begin() ; it != wavefront_.end() ; ++it )
        {
            
            if ( (*it).second.second==false )
            {

                dbskfg_shock_node* shock_node = dynamic_cast<dbskfg_shock_node*>
                    (&(*(*it).second.first));

                vcl_stringstream temp;
                vgl_point_2d<double> point;

                vcl_pair<dbskfg_composite_node_sptr,
                    dbskfg_composite_node_sptr> local_pair;
                local_pair.first=0;
                local_pair.second=0;
                bool case_flag=false;
             
                if ( source_outer )
                {
                    if ( source_outer_node->id() == shock_node->id() )
                    {
                        return;
                    }

                    source=source_outer_node;
                    target=(*it).second.first;

                    local_pair.first = source_outer_node;
                    local_pair.second = (*it).second.first;
             
                    // We need to set the end tau for this thing
                    edge->setLeTau(edge->getLTauFromTime(
                                       shock_node->get_radius()));
                    edge->setReTau(edge->getRTauFromTime(
                                       shock_node->get_radius()));
                    edge->compute_extrinsic_locus();
                    temp<<edge->ex_pts().back();
                    point = edge->ex_pts().back();
 
                }
                else if ( target_outer )
                {
                    if ( target_outer_node->id() == shock_node->id() )
                    {
                        return;
                    }
                    
                    target=target_outer_node;
                    source=(*it).second.first;

                    local_pair.first = (*it).second.first;
                    local_pair.second = target_outer_node;


                    // We need to set the end tau for this thing           
                    edge->setLsTau(edge->getLTauFromTime(
                                       shock_node->get_radius()));
                    edge->setRsTau(edge->getRTauFromTime(
                                       shock_node->get_radius()));
                    edge->compute_extrinsic_locus();
                    temp<<edge->ex_pts().front();
                    point = edge->ex_pts().front();
                    
                    case_flag=true;

                }
                // One of these exist inside lets find out which
                else if ( all_nodes_.count(source_stream.str()) &&
                          source_outer_node ==0 && target_outer_node == 0 )
                {
                    // Exists inside
                    source=all_nodes_[source_stream.str()];                
                    target=(*it).second.first;
              
                    local_pair.first = all_nodes_[source_stream.str()];
                    local_pair.second = (*it).second.first;

                    // We need to set the end tau for this thing
                    edge->setLeTau(edge->getLTauFromTime(
                                       shock_node->get_radius()));
                    edge->setReTau(edge->getRTauFromTime(
                                       shock_node->get_radius()));
                    edge->compute_extrinsic_locus();
                    temp<<edge->ex_pts().back();
                    point = edge->ex_pts().back();
                }
                else if ( all_nodes_.count(target_stream.str()) && 
                          source_outer_node == 0 && target_outer_node ==0 )
                {
                    target=all_nodes_[target_stream.str()];
                    source=(*it).second.first;

                    local_pair.first = (*it).second.first;
                    local_pair.second = all_nodes_[target_stream.str()];
             
                    // We need to set the end tau for this thing           
                    edge->setLsTau(edge->getLTauFromTime(
                                       shock_node->get_radius()));
                    edge->setRsTau(edge->getRTauFromTime(
                                       shock_node->get_radius()));
                    edge->compute_extrinsic_locus();
                    temp<<edge->ex_pts().front();
                    point = edge->ex_pts().front();

                    case_flag=true;
                }

                vcl_stringstream outer_shock;
                outer_shock<<shock_node->pt();
                
                if ( !local_pair.first && !local_pair.second )
                {
                    continue;
                }

                if ( temp.str() == outer_shock.str() ||
                     point == shock_node->pt() ||
                     vgl_distance(shock_node->pt(),point) < 0.02)
                {
                    vcl_pair<unsigned int,
                        vcl_pair<dbskfg_composite_node_sptr,
                        dbskfg_composite_node_sptr> > pair 
                        = vcl_make_pair(shock_node->id(),local_pair);

                    distance_map[vgl_distance(shock_node->pt(),point)]
                        = pair;
                    case_to_consider[vgl_distance(shock_node->pt(),point)]
                        = case_flag;
                }
            }

        }
 
        if ( distance_map.size() > 0 )
        {
            source = (*distance_map.begin()).second.second.first;
            target = (*distance_map.begin()).second.second.second;

            bool case_flag = (*case_to_consider.begin()).second;
            unsigned int id = (*distance_map.begin()).second.first;

            wavefront_[id].second = true;
            found = true;
        
            dbskfg_shock_node* shock_node = dynamic_cast<dbskfg_shock_node*>
                (&(*wavefront_[id].first));

            if ( case_flag)
            {
                // We need to set the end tau for this thing           
                edge->setLsTau(edge->getLTauFromTime(
                                   shock_node->get_radius()));
                edge->setRsTau(edge->getRTauFromTime(
                                   shock_node->get_radius()));
             
            }
            else
            {
                // We need to set the end tau for this thing
                edge->setLeTau(edge->getLTauFromTime(
                                   shock_node->get_radius()));
                edge->setReTau(edge->getRTauFromTime(
                                   shock_node->get_radius()));


            }
            edge->compute_extrinsic_locus();

            if ( edge->rBElement()->is_a_point() )
            {

                // Right Point
                vgl_point_2d<double> right_point = 
                    reinterpret_cast<dbsk2d_ishock_bpoint*>(
                        edge->rBElement())->pt();
                vcl_stringstream righty; righty<<right_point; 
        
                if ( !all_nodes_.count(righty.str()) )
                {
                    found = false;
                }
            }
   
            if ( edge->lBElement()->is_a_point() )
            {

                // Left Point
                vgl_point_2d<double> left_point = 
                    reinterpret_cast<dbsk2d_ishock_bpoint*>(
                        edge->lBElement())->pt();
                vcl_stringstream lefty; lefty<<left_point; 
        
                if ( !all_nodes_.count(lefty.str()) )
                {
                    found = false;
                }
            }
        }
    }

    if ( !found )
    {
        return;
    }

    // Add the shock link into the graph
    dbskfg_composite_link_sptr shock_edge =
        new dbskfg_shock_link(
            source,
            target,
            original_graph_->next_available_id(),
            dbskfg_shock_link::SHOCK_EDGE);

    bool flag = original_graph_->add_edge(shock_edge);

    source->add_outgoing_edge(shock_edge);
    target->add_incoming_edge(shock_edge);
    
    // Determine type of shock this is 
    dbskfg_shock_link_boundary left_boundary,right_boundary;

    // Create shock link
    dbskfg_shock_link* slink=
        dynamic_cast<dbskfg_shock_link*>(&(*shock_edge));

    // Grab original contour ids
    vcl_pair<int,int> ids = dbskfg_utilities::get_contour_id_from_shock_edge
        (edge);
 
    //Line/Line
    if ( edge->lBElement()->is_a_line() && edge->rBElement()->is_a_line())
    {
        if (orig_contour_links_.count(ids.first))
        {
            slink->add_to_left_contour(
                orig_contour_links_[ids.first]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.first]));
            clink->shock_links_affected(slink);
        }
        else
        {
            vgl_point_2d<double> source_node=edge->lBElement()->s_pt()->pt();
            vgl_point_2d<double> target_node=edge->lBElement()->e_pt()->pt();
            vcl_stringstream stream;
            stream<<source_node<<target_node;
            
            if (new_contour_links_points_.count(stream.str()))
            {
                slink->add_to_left_contour(
                    new_contour_links_points_[stream.str()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_points_[stream.str()]));
                clink->shock_links_affected(slink);
            }
            
            
        }

        if (orig_contour_links_.count(ids.second))
        {
            slink->add_to_right_contour(
                orig_contour_links_[ids.second]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.second]));
            clink->shock_links_affected(slink);
        }
        else
        {
            vgl_point_2d<double> source_node=edge->rBElement()->s_pt()->pt();
            vgl_point_2d<double> target_node=edge->rBElement()->e_pt()->pt();
            vcl_stringstream stream;
            stream<<source_node<<target_node;
            
            if (new_contour_links_points_.count(stream.str()))
            {
                slink->add_to_right_contour(
                    new_contour_links_points_[stream.str()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_points_[stream.str()]));
                clink->shock_links_affected(slink);
            }
        }

        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getLFootPt(edge->sTau()));
        poly.push_back(edge->getLFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(edge->getRFootPt(edge->eTau()));
        poly.push_back(edge->getRFootPt(edge->sTau()));
        slink->set_polygon(poly);

        left_boundary.contour_.push_back(edge->getLFootPt(edge->sTau()));
        left_boundary.contour_.push_back(edge->getLFootPt(edge->eTau()));
        
        right_boundary.contour_.push_back(edge->getRFootPt(edge->eTau()));
        right_boundary.contour_.push_back(edge->getRFootPt(edge->sTau()));
        


    }
    //Left line/Right Point
    else if ( edge->lBElement()->is_a_line() && edge->rBElement()->is_a_point())
    {
        // Right Point
        vgl_point_2d<double> right_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->rBElement())->pt();
        vcl_stringstream righty; righty<<right_point; 
        right_boundary.point_ = right_point;
        dbskfg_contour_node* cnode_right = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[righty.str()]));
        slink->add_right_point(cnode_right);
        cnode_right->shock_links_affected(slink);

        // Left line
        if (orig_contour_links_.count(ids.first))
        {
            slink->add_to_left_contour(
                orig_contour_links_[ids.first]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.first]));
            clink->shock_links_affected(slink);
        }
        else
        {
            vgl_point_2d<double> source_node=edge->lBElement()->s_pt()->pt();
            vgl_point_2d<double> target_node=edge->lBElement()->e_pt()->pt();
            vcl_stringstream stream;
            stream<<source_node<<target_node;
            
            if (new_contour_links_points_.count(stream.str()))
            {
                slink->add_to_left_contour(
                    new_contour_links_points_[stream.str()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_points_[stream.str()]));
                clink->shock_links_affected(slink);
            }
            
            
        }

        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getLFootPt(edge->sTau()));
        poly.push_back(edge->getLFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(right_point);
        slink->set_polygon(poly);

        left_boundary.contour_.push_back(edge->getLFootPt(edge->sTau()));
        left_boundary.contour_.push_back(edge->getLFootPt(edge->eTau()));
  
    }
    //Right Line/Left Point
    else if ( edge->lBElement()->is_a_point() && edge->rBElement()->is_a_line())
    {
       
        // Left Point
        vgl_point_2d<double> left_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->lBElement())->pt();
        vcl_stringstream lefty; lefty<<left_point; 
        left_boundary.point_  = left_point;
        dbskfg_contour_node* cnode_left = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[lefty.str()]));
        slink->add_left_point(cnode_left);
        cnode_left->shock_links_affected(slink);

        //Right line
        if (orig_contour_links_.count(ids.second))
        {
            slink->add_to_right_contour(
                orig_contour_links_[ids.second]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.second]));
            clink->shock_links_affected(slink);
        }
        else
        {
            vgl_point_2d<double> source_node=edge->rBElement()->s_pt()->pt();
            vgl_point_2d<double> target_node=edge->rBElement()->e_pt()->pt();
            vcl_stringstream stream;
            stream<<source_node<<target_node;
            
            if (new_contour_links_points_.count(stream.str()))
            {
                slink->add_to_right_contour(
                    new_contour_links_points_[stream.str()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_points_[stream.str()]));
                clink->shock_links_affected(slink);
            }
        }

        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getRFootPt(edge->sTau()));
        poly.push_back(edge->getRFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(left_point);
        slink->set_polygon(poly);

        right_boundary.contour_.push_back(edge->getRFootPt(edge->eTau()));
        right_boundary.contour_.push_back(edge->getRFootPt(edge->sTau()));
     
    }
    // Point / Point
    else 
    {

        vgl_point_2d<double> left_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->lBElement())->pt();
        vgl_point_2d<double> right_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->rBElement())->pt();
        left_boundary.point_  = left_point;
        right_boundary.point_ = right_point;

        vcl_stringstream lefty,righty;
        lefty<<left_point; righty<<right_point;

        dbskfg_contour_node* cnode_left = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[lefty.str()]));
        dbskfg_contour_node* cnode_right = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[righty.str()]));
                           
        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(left_point);
        poly.push_back(target->pt());
        poly.push_back(right_point);   
        slink->set_polygon(poly);

        slink->add_left_point(cnode_left);
        slink->add_right_point(cnode_right);
        cnode_left->shock_links_affected(slink);
        cnode_right->shock_links_affected(slink);
    }

    slink->set_left_boundary(left_boundary);
    slink->set_right_boundary(right_boundary);
    if ( original_graph_->construct_locus() )
    {
        slink->construct_locus();
    }

    // add link to map
    new_shock_links_[slink->id()]=shock_edge;

}

void dbskfg_transformer::wavefront_complete(dbskfg_composite_node_sptr& node)
{
    dbsk2d_ishock_edge* edge(0);
    bool found=false;
    dbskfg_composite_node_sptr target(0);
    dbskfg_composite_node_sptr source(0);
    
    // grab shock graph
    dbsk2d_ishock_graph_sptr ishock_graph = shock_storage_->get_ishock_graph();

    dbskfg_shock_node* shock_node = dynamic_cast<dbskfg_shock_node*>
        (&(*node));
    
    // Iterate thru all edges
    dbsk2d_ishock_graph::edge_iterator eit;
    for ( eit = ishock_graph->all_edges().begin(); 
          eit != ishock_graph->all_edges().end();++eit)
    {
        edge = *eit;

        vcl_stringstream source_stream;
        vcl_stringstream target_stream;

        // Grab source node
        if ( edge->pSNode() != 0)
        {
            source_stream<<edge->source()->origin();
        }
        else
        {
            source_stream<<"temp";
        }
   
        // Grab target node
        if ( edge->cSNode() != 0)
        {
            target_stream<<edge->target()->origin();
        }
        else
        {
            target_stream<<"temp";
        }

        if ( (all_nodes_.count(source_stream.str()) == 1 ) != 
             (all_nodes_.count(target_stream.str()) == 1 ) )
        {

            if ( dbskfg_utilities::is_node_in_set_of_nodes(
                     transform_->outer_shock_nodes_,
                     source_stream.str()) || 
                 dbskfg_utilities::is_node_in_set_of_nodes(
                     transform_->outer_shock_nodes_,
                     target_stream.str())

                )
            {
                continue;

            }


            vcl_stringstream temp;

            // One of these exist inside lets find out which
            if ( all_nodes_.count(source_stream.str()))
            {
                // Exists inside
                source=all_nodes_[source_stream.str()];                
                target=node;
                
                // We need to set the end tau for this thing
                edge->setLeTau(edge->getLTauFromTime(
                                   shock_node->get_radius()));
                edge->setReTau(edge->getRTauFromTime(
                                   shock_node->get_radius()));
                edge->compute_extrinsic_locus();
                temp<<edge->ex_pts().back();
           
            }
            else
            {
                target=all_nodes_[target_stream.str()];
                source=node;

                // We need to set the end tau for this thing           
                edge->setLsTau(edge->getLTauFromTime(
                                   shock_node->get_radius()));
                edge->setRsTau(edge->getRTauFromTime(
                                   shock_node->get_radius()));
                edge->compute_extrinsic_locus();
                temp<<edge->ex_pts().front();
            }
                    
            vcl_stringstream outer_shock;
            outer_shock<<shock_node->pt();

            if ( temp.str() == outer_shock.str() ||
                 shock_node->pt() == edge->ex_pts().back())
            {
                found = true;
                break;
            }
            

        }
    }

    if ( !found )
    {
        return;
    }

    // Add the shock link into the graph
    dbskfg_composite_link_sptr shock_edge =
        new dbskfg_shock_link(
            source,
            target,
            original_graph_->next_available_id(),
            dbskfg_shock_link::SHOCK_EDGE);

    bool flag = original_graph_->add_edge(shock_edge);

    source->add_outgoing_edge(shock_edge);
    target->add_incoming_edge(shock_edge);
    
    // Determine type of shock this is 
    dbskfg_shock_link_boundary left_boundary,right_boundary;

    // Create shock link
    dbskfg_shock_link* slink=
        dynamic_cast<dbskfg_shock_link*>(&(*shock_edge));

    // Grab original contour ids
    vcl_pair<int,int> ids = dbskfg_utilities::get_contour_id_from_shock_edge
        (edge);
 
    //Line/Line
    if ( edge->lBElement()->is_a_line() && edge->rBElement()->is_a_line())
    {
        if (orig_contour_links_.count(ids.first))
        {
            slink->add_to_left_contour(
                orig_contour_links_[ids.first]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.first]));
            clink->shock_links_affected(slink);
        }
      
        if (orig_contour_links_.count(ids.second))
        {
            slink->add_to_right_contour(
                orig_contour_links_[ids.second]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.second]));
            clink->shock_links_affected(slink);
        }
      
        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getLFootPt(edge->sTau()));
        poly.push_back(edge->getLFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(edge->getRFootPt(edge->eTau()));
        poly.push_back(edge->getRFootPt(edge->sTau()));
        slink->set_polygon(poly);

        left_boundary.contour_.push_back(edge->getLFootPt(edge->sTau()));
        left_boundary.contour_.push_back(edge->getLFootPt(edge->eTau()));
        
        right_boundary.contour_.push_back(edge->getRFootPt(edge->eTau()));
        right_boundary.contour_.push_back(edge->getRFootPt(edge->sTau()));
        


    }
    //Left line/Right Point
    else if ( edge->lBElement()->is_a_line() && edge->rBElement()->is_a_point())
    {
        // Right Point
        vgl_point_2d<double> right_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->rBElement())->pt();
        vcl_stringstream righty; righty<<right_point; 
        right_boundary.point_ = right_point;
        dbskfg_contour_node* cnode_right = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[righty.str()]));
        slink->add_right_point(cnode_right);
        cnode_right->shock_links_affected(slink);

        // Left line
        if (orig_contour_links_.count(ids.first))
        {
            slink->add_to_left_contour(
                orig_contour_links_[ids.first]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.first]));
            clink->shock_links_affected(slink);
        }
      
        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getLFootPt(edge->sTau()));
        poly.push_back(edge->getLFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(right_point);
        slink->set_polygon(poly);

        left_boundary.contour_.push_back(edge->getLFootPt(edge->sTau()));
        left_boundary.contour_.push_back(edge->getLFootPt(edge->eTau()));
  
    }
    //Right Line/Left Point
    else if ( edge->lBElement()->is_a_point() && edge->rBElement()->is_a_line())
    {
       
        // Left Point
        vgl_point_2d<double> left_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->lBElement())->pt();
        vcl_stringstream lefty; lefty<<left_point; 
        left_boundary.point_  = left_point;
        dbskfg_contour_node* cnode_left = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[lefty.str()]));
        slink->add_left_point(cnode_left);
        cnode_left->shock_links_affected(slink);

        //Right line
        if (orig_contour_links_.count(ids.second))
        {
            slink->add_to_right_contour(
                orig_contour_links_[ids.second]);
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &(*orig_contour_links_[ids.second]));
            clink->shock_links_affected(slink);
        }
      
        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(edge->getRFootPt(edge->sTau()));
        poly.push_back(edge->getRFootPt(edge->eTau()));
        poly.push_back(target->pt());
        poly.push_back(left_point);
        slink->set_polygon(poly);

        right_boundary.contour_.push_back(edge->getRFootPt(edge->eTau()));
        right_boundary.contour_.push_back(edge->getRFootPt(edge->sTau()));
     
    }
    // Point / Point
    else 
    {

        vgl_point_2d<double> left_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->lBElement())->pt();
        vgl_point_2d<double> right_point = 
            reinterpret_cast<dbsk2d_ishock_bpoint*>(edge->rBElement())->pt();
        left_boundary.point_  = left_point;
        right_boundary.point_ = right_point;

        vcl_stringstream lefty,righty;
        lefty<<left_point; righty<<right_point;

        dbskfg_contour_node* cnode_left = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[lefty.str()]));
        dbskfg_contour_node* cnode_right = dynamic_cast<dbskfg_contour_node*>
            (&(*all_nodes_[righty.str()]));
                           
        // create polygon
        vgl_polygon<double> poly(1);

        poly.push_back(source->pt());
        poly.push_back(left_point);
        poly.push_back(target->pt());
        poly.push_back(right_point);   
        slink->set_polygon(poly);

        slink->add_left_point(cnode_left);
        slink->add_right_point(cnode_right);
        cnode_left->shock_links_affected(slink);
        cnode_right->shock_links_affected(slink);
    }

    slink->set_left_boundary(left_boundary);
    slink->set_right_boundary(right_boundary);
    if ( original_graph_->construct_locus() )
    {
        slink->construct_locus();
    }

    // add link to map
    new_shock_links_[slink->id()]=shock_edge;



}

void dbskfg_transformer::add_new_contours()
{
    int gap1_id(0);

    for ( unsigned int k=0; k < transform_->all_gaps_.size() ; ++k)
    {
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr>
            tgap = transform_->all_gaps_[k];

        vcl_stringstream leftpt_stream;
        vcl_stringstream rightpt_stream;
        leftpt_stream<<tgap.first->pt();
        rightpt_stream<<tgap.second->pt();
        dbskfg_contour_node* gap1=dynamic_cast<dbskfg_contour_node*>
            (&(*tgap.first));
        dbskfg_contour_node* gap2=dynamic_cast<dbskfg_contour_node*>
            (&(*tgap.second));
        gap1_id = gap1->contour_id();
        int gap2_id = gap2->contour_id();

        // Insert the two gaps first
        all_nodes_[leftpt_stream.str()]=tgap.first;
        new_contour_nodes_.insert(leftpt_stream.str());
        all_nodes_[rightpt_stream.str()]=tgap.second;
        new_contour_nodes_.insert(rightpt_stream.str());
    }

    // We know all shock edges cames from the contours affected
    // We only insert those within the polygon
    for (dbskfg_composite_graph::edge_iterator eit =
             local_graph_->edges_begin();
         eit != local_graph_->edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link = *eit;
        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            dbskfg_contour_link* clink=dynamic_cast<dbskfg_contour_link*>
                (&(*link));

            if ( clink->contour_id() == 0 )
            {
                // Lets add in contour nodes
                // Must make sure it is not gap nodes
                vcl_stringstream sourcestr,targetstr;
                sourcestr<<link->source()->pt();
                targetstr<<link->target()->pt();
                
                dbskfg_composite_node_sptr source_node(0);
                dbskfg_composite_node_sptr target_node(0);

                //*********** Source Node ************************************
                if ( all_nodes_.count(sourcestr.str())==0)
                {
                    source_node =
                        new dbskfg_contour_node(
                            original_graph_->next_available_id(),
                            link->source()->pt(),
                            0);
                    original_graph_->add_vertex(source_node);
                    all_nodes_[sourcestr.str()]=source_node;
                    new_contour_nodes_.insert(sourcestr.str());
                }
                else
                {
                    source_node=all_nodes_[sourcestr.str()];
                }

                //************** Target Node *********************************
                if ( all_nodes_.count(targetstr.str())==0)
                {
                    target_node =
                        new dbskfg_contour_node(
                            original_graph_->next_available_id(),
                            link->target()->pt(),
                            0);
                    original_graph_->add_vertex(target_node);
                    all_nodes_[targetstr.str()]=target_node;
                    new_contour_nodes_.insert(targetstr.str());
                }
                else
                {
                    target_node=all_nodes_[targetstr.str()];
                }
                
                // Set degrees of node
                source_node->set_composite_degree(
                    source_node->get_composite_degree()+1);
              

                target_node->set_composite_degree(
                    target_node->get_composite_degree()+1);
               
                // Now add in contour link
                dbskfg_composite_link_sptr clink =
                    new dbskfg_contour_link(
                        source_node,
                        target_node,
                        original_graph_->next_available_id());
                original_graph_->add_edge(clink);
                source_node->add_outgoing_edge(clink);
                target_node->add_incoming_edge(clink);
                
                // Store a mapping from old contour id to current
                new_contour_links_[link->id()]=clink;

                // Also store contour links by start end end point
                vcl_stringstream stream1,stream2;
                stream1<<source_node->pt()<<target_node->pt();
                stream2<<target_node->pt()<<source_node->pt();
                new_contour_links_points_[stream1.str()]=clink;
                new_contour_links_points_[stream2.str()]=clink;
            }
        }
    }
}

bool dbskfg_transformer::compute_local_graph()
{
    transform_->convert_contours_to_vsol();
    
    // 1) Get image storage class
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

    // 2) Get vsol storage class 
    // Create vsol storage class with affected contour for transforms
    vidpro1_vsol2D_storage_sptr input_vsol = vidpro1_vsol2D_storage_new();
    input_vsol->add_objects(transform_->contours_spatial_objects_);
    if ( transform_->new_contours_spatial_objects_.size())
    {
        input_vsol->add_objects(transform_->new_contours_spatial_objects_);
    }

    vsol_box_2d_sptr bbox = transform_->bounding_box_context();

    /*********************** Shock Compute **********************************/
    // 3) Create shock pro process and assign inputs 
    dbsk2d_compute_ishock_process shock_pro;

    shock_pro.clear_input();
    shock_pro.clear_output();

    shock_pro.add_input(image_storage);
    shock_pro.add_input(input_vsol);

    // Set params
    shock_pro.parameters()->set_value("-exist_ids",true);
    shock_pro.parameters()->set_value("-b_growbbox",true);
    shock_pro.parameters()->set_value("-bbox_minx",bbox->get_min_x());
    shock_pro.parameters()->set_value("-bbox_miny",bbox->get_min_y());
    shock_pro.parameters()->set_value("-bbox_maxx",bbox->get_max_x());
    shock_pro.parameters()->set_value("-bbox_maxy",bbox->get_max_y());

    bool status_flag=true;
    status_flag = shock_pro.execute();
    
    if ( !status_flag) 
    {
        return status_flag;
    }

    shock_pro.finish();

    //assert(status_flag == true);

    // Grab output from shock computation
    vcl_vector<bpro1_storage_sptr> shock_results;

    shock_results = shock_pro.get_output();

    // Clean up after ourselves
    shock_pro.clear_input();
    shock_pro.clear_output();

    // Lets vertical cast to shock stroge
    shock_storage_.vertical_cast(shock_results[0]);

    /*********************** Compute Composite Graph ************************/
    dbskfg_form_composite_graph_process cg_pro;

    cg_pro.clear_input();
    cg_pro.clear_output();

    cg_pro.add_input(shock_storage_);
    cg_pro.add_input(image_storage);

    status_flag = cg_pro.execute();
    if ( !status_flag)
    {
        return status_flag;
    }

    cg_pro.finish();

    vcl_vector<bpro1_storage_sptr> cg_results;
    cg_results = cg_pro.get_output();

    cg_pro.clear_input();
    cg_pro.clear_output();

    dbskfg_composite_graph_storage_sptr cg_storage;
    cg_storage.vertical_cast(cg_results[0]);

    local_graph_ = cg_storage->get_composite_graph();

    input_vsol->clear_all();

    transform_->contours_spatial_objects_.clear();

    return status_flag;

}

void dbskfg_transformer::local_surgery()
{
 
    // We know all shock edges cames from the contours affected
    // We only insert those within the polygon
    for (dbskfg_composite_graph::edge_iterator eit =
             local_graph_->edges_begin();
         eit != local_graph_->edges_end(); ++eit)
    {

        dbskfg_composite_link_sptr link = *eit;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* shock_link=
                dynamic_cast<dbskfg_shock_link*>(&(*link));
    
            if ( shock_link->shock_link_type() == 
                 dbskfg_shock_link::SHOCK_EDGE )
            {

                // Make sure we havent seen nodes and them in
                dbskfg_composite_node_sptr source_node(0);
                dbskfg_composite_node_sptr target_node(0);

                // Test four possible cases
                // Both Source/Target IN Polygon
                // Source Outer Polygon, Target Inner Polygon
                // Target Outer Polygon, Source Outer Polygon
                // Both Outer Polygon
                shock_link_add(link,source_node,target_node);

                // If both are not null then we are fine
                if ( source_node && target_node)
                    
                {

                    create_shock_link(source_node,target_node,shock_link);
                
                }
            }
        }
    }

}

void dbskfg_transformer::shock_link_add(dbskfg_composite_link_sptr link,
                                        dbskfg_composite_node_sptr& source_node,
                                        dbskfg_composite_node_sptr& target_node)
{
    // For source and target test three things
    // Did the source come from the outer shock polygon?
    // Did the source come from a contour node?
    // Does it exist inside the polygon?

    vcl_stringstream targetstream,sourcestream;
    targetstream<<(*link).target()->pt();
    sourcestream<<(*link).source()->pt();
    
    // Set source node
    dbskfg_composite_node_sptr temp_source_node =dbskfg_utilities::
        get_node_in_set_of_nodes
        (transform_->outer_shock_nodes_,
         (*link).source(),
         dbskfg_utilities::POINT);

    // Set target node
    dbskfg_composite_node_sptr temp_target_node = dbskfg_utilities::
        get_node_in_set_of_nodes
        (transform_->outer_shock_nodes_,
         (*link).target(),
         dbskfg_utilities::POINT);

    //******************** Source Node ********************************
    if (temp_source_node)
    {
        source_node = temp_source_node;
        wavefront_[source_node->id()].second = true;
        
    }
    else if (transform_->poly_.contains((*link).source()->pt().x(),
                                        (*link).source()->pt().y()))
    {
        if ( all_nodes_.count(sourcestream.str())==0)
        {
            dbskfg_shock_node* source_shock =
                dynamic_cast<dbskfg_shock_node*>(&(*(*link).source())); 
            source_node=
                new dbskfg_shock_node(
                    original_graph_->next_available_id(),
                    (*link).source()->pt(),
                    source_shock->get_radius());
            all_nodes_[sourcestream.str()]=source_node;
            original_graph_->add_vertex(source_node);
        }
        else
        {

            source_node=all_nodes_[sourcestream.str()];
        }

    }
    else if ( new_contour_nodes_.count(sourcestream.str()) > 0 )
    {
        // This is a special case for the completing contour is not
        // contained within the polygon
        if ( transform_->poly_.contains((*link).target()->pt().x(),
                                        (*link).target()->pt().y()))
        {

            source_node=all_nodes_[sourcestream.str()];
        }

    }

    //******************** Target Node ********************************
    if (temp_target_node)
    {
        if ( temp_source_node )
        {
            if ( temp_source_node->id() == temp_target_node->id())
            {
         
                // Both are going to same node
                source_node=0;
                target_node=0;
                wavefront_[temp_source_node->id()].second = false;
            }
            else
            {
                // Set target node
                target_node= temp_target_node;
                wavefront_[target_node->id()].second = true;
                        
            }
        }
        else
        {
            // Set target node
            target_node= temp_target_node;
            wavefront_[target_node->id()].second = true;
                  
            
        }


    }

    else if (transform_->poly_.contains((*link).target()->pt().x(),
                                        (*link).target()->pt().y()))
    {
        if ( all_nodes_.count(targetstream.str())==0)
        {
            dbskfg_shock_node* target_shock =
                dynamic_cast<dbskfg_shock_node*>(&(*(*link).target())); 
            target_node=
                new dbskfg_shock_node(
                    original_graph_->next_available_id(),
                    (*link).target()->pt(),
                    target_shock->get_radius());
            all_nodes_[targetstream.str()]=target_node;
            original_graph_->add_vertex(target_node);
        }
        else
        {

            target_node=all_nodes_[targetstream.str()];
        }

    }
    else if ( new_contour_nodes_.count(targetstream.str()) > 0 )
    {
        // This is a special case for the completing contour is not
        // contained within the polygon 
        if ( transform_->poly_.contains((*link).source()->pt().x(),
                                        (*link).source()->pt().y()))
        {

            target_node=all_nodes_[targetstream.str()];
        }

    }


}

void dbskfg_transformer::create_shock_link(dbskfg_composite_node_sptr source,
                                           dbskfg_composite_node_sptr target,
                                           dbskfg_shock_link* shock_link)
{

    // Add the shock link into the graph
    dbskfg_composite_link_sptr shock_edge =
        new dbskfg_shock_link(
            source,
            target,
            original_graph_->next_available_id(),
            dbskfg_shock_link::SHOCK_EDGE);

    bool flag = original_graph_->add_edge(shock_edge);

    source->add_outgoing_edge(shock_edge);
    target->add_incoming_edge(shock_edge);
                    
    dbskfg_shock_link* slink=
        dynamic_cast<dbskfg_shock_link*>(&(*shock_edge));

    slink->set_polygon(shock_link->polygon());
    slink->set_left_boundary(shock_link->get_left_boundary());
    slink->set_right_boundary(shock_link->get_right_boundary());
                 
    // add link to map
    new_shock_links_[slink->id()]=shock_edge;

    // Grab contours right line, left point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::RLLP )
    {
        vcl_vector<dbskfg_composite_link_sptr>
            clinks = shock_link->right_contour_links();
                        
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            dbskfg_contour_link* tclink= dynamic_cast<dbskfg_contour_link*>
                (&(*clinks[i]));
    
            if (orig_contour_links_.count(tclink->contour_id()))
            {
                slink->add_to_right_contour(
                    orig_contour_links_[tclink->contour_id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*orig_contour_links_[tclink->contour_id()]));
                clink->shock_links_affected(slink);
            }
            else if (new_contour_links_.count(clinks[i]->id()))
            {
                slink->add_to_right_contour(
                    new_contour_links_[clinks[i]->id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_[clinks[i]->id()]));
                clink->shock_links_affected(slink);
          
            }
        }                                

        // add left point
        vcl_stringstream temp_stream;
        temp_stream<<shock_link->get_left_point()->pt();

        dbskfg_contour_node* cnode = 
            dynamic_cast<dbskfg_contour_node*>(
                &(*all_nodes_[temp_stream.str()]));
        slink->add_left_point(cnode);
        cnode->shock_links_affected(slink);
          
    }
    
    // Grab contours left line right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::LLRP )
    {
        
        vcl_vector<dbskfg_composite_link_sptr>
            clinks = shock_link->left_contour_links();
         
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            dbskfg_contour_link* tclink= dynamic_cast<dbskfg_contour_link*>
                (&(*clinks[i]));
  
            if (orig_contour_links_.count(tclink->contour_id()))
            {
                slink->add_to_left_contour(
                    orig_contour_links_[tclink->contour_id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*orig_contour_links_[tclink->contour_id()]));
                clink->shock_links_affected(slink);
       
            }
            else if (new_contour_links_.count(clinks[i]->id()))
            {
                slink->add_to_left_contour(
                    new_contour_links_[clinks[i]->id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_[clinks[i]->id()]));
                clink->shock_links_affected(slink);
       
            }

        }                                
                        
        // add right point
        vcl_stringstream temp_stream;
        temp_stream<<shock_link->get_right_point()->pt();
        dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>(
                &(*all_nodes_[temp_stream.str()]));
        slink->add_right_point(cnode);
        cnode->shock_links_affected(slink);
    }

    // Grab contours left line and right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::LL )
    {

        // Left Line
        vcl_vector<dbskfg_composite_link_sptr> clinks = 
            shock_link->left_contour_links();
                        
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            dbskfg_contour_link* tclink= dynamic_cast<dbskfg_contour_link*>
                (&(*clinks[i]));
  
            if (orig_contour_links_.count(tclink->contour_id()))
            {
                slink->add_to_left_contour(
                    orig_contour_links_[tclink->contour_id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*orig_contour_links_[tclink->contour_id()]));
                clink->shock_links_affected(slink);
       
            }
            else if ( new_contour_links_.count(clinks[i]->id()))
            {
                slink->add_to_left_contour(
                    new_contour_links_[clinks[i]->id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_[clinks[i]->id()]));
                clink->shock_links_affected(slink);
       
            }
        }                                
        
        // Right line
        vcl_vector<dbskfg_composite_link_sptr>
            rclinks = shock_link->right_contour_links();
                     
        for ( unsigned int i=0; i<rclinks.size() ; ++i)
        {
            dbskfg_contour_link* tclink= dynamic_cast<dbskfg_contour_link*>
                (&(*rclinks[i]));
  
            if (orig_contour_links_.count(tclink->contour_id()))
            {
                slink->add_to_right_contour(
                    orig_contour_links_[tclink->contour_id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*orig_contour_links_[tclink->contour_id()]));
                clink->shock_links_affected(slink);
       
            }
            else if ( new_contour_links_.count(rclinks[i]->id()))
            {
                slink->add_to_right_contour(
                    new_contour_links_[rclinks[i]->id()]);
                dbskfg_contour_link* clink=
                    dynamic_cast<dbskfg_contour_link*>(
                        &(*new_contour_links_[rclinks[i]->id()]));
                clink->shock_links_affected(slink);
       
            }
        }


    }
                    
    // Grab contours left line and right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::PP )
    {
        
        // add left point
        vcl_stringstream temp_stream;
        temp_stream<<shock_link->get_left_point()->pt();
        dbskfg_contour_node* cnode = 
            dynamic_cast<dbskfg_contour_node*>(
                &(*all_nodes_[temp_stream.str()]));
        slink->add_left_point(cnode);
        cnode->shock_links_affected(slink);

        // add right point
        vcl_stringstream tempr_stream;
        tempr_stream<<shock_link->get_right_point()->pt();
        dbskfg_contour_node* rcnode = 
            dynamic_cast<dbskfg_contour_node*>(
                &(*all_nodes_[tempr_stream.str()]));
        slink->add_right_point(rcnode);
        rcnode->shock_links_affected(slink);
    }
                         
    // construct locus
    if ( original_graph_->construct_locus())
    {
        slink->construct_locus();
    }


}


void dbskfg_transformer::classify_nodes()
{
    vcl_map<vcl_string,dbskfg_composite_node_sptr>::iterator vit;
    for ( vit = all_nodes_.begin() ; vit != all_nodes_.end() ; ++vit)
    {
        if ( (*vit).second->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            unsigned int shock_degree(0);
            dbskfg_composite_node::edge_iterator eit;
            
            for (eit = (*vit).second->out_edges_begin(); 
                 eit != (*vit).second->out_edges_end() ; ++eit)
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

            for (eit = (*vit).second->in_edges_begin(); 
                 eit != (*vit).second->in_edges_end() ; ++eit)
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
            
            (*vit).second->set_composite_degree(shock_degree);

        }
    }

    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;
    for ( nit = transform_->outer_shock_nodes_.begin() ; 
          nit != transform_->outer_shock_nodes_.end() ; ++nit )
    {
        unsigned int shock_degree(0);
        dbskfg_composite_node::edge_iterator eit;
            
        for (eit = (*nit)->out_edges_begin(); 
             eit != (*nit)->out_edges_end() ; ++eit)
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

        for (eit = (*nit)->in_edges_begin(); 
             eit != (*nit)->in_edges_end() ; ++eit)
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
       
        if ( shock_degree == 0 )
        {
            original_graph_->remove_vertex(*nit);
        }
        else
        {
            (*nit)->set_composite_degree(shock_degree);
        }
        
    }

}

void dbskfg_transformer::verify_surgery()
{

    // Keep two pairs
    vcl_map<unsigned int,dbskfg_shock_link*> shocks_to_delete;
    vcl_map<unsigned int,dbskfg_composite_node_sptr> nodes_to_delete;

    vcl_map<vcl_string,dbskfg_composite_node_sptr>::iterator vit;
    for ( vit = all_nodes_.begin() ; vit != all_nodes_.end() ; ++vit)
    {

        if ( (*vit).second->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            unsigned int shock_degree(0);
            dbskfg_shock_link* link_to_kill(0);
            dbskfg_composite_node::edge_iterator eit;
            
            for (eit = (*vit).second->out_edges_begin(); 
                 eit != (*vit).second->out_edges_end() ; ++eit)
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
                    link_to_kill=slink;
                }
            }

            for (eit = (*vit).second->in_edges_begin(); 
                 eit != (*vit).second->in_edges_end() ; ++eit)
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
                    link_to_kill=slink;
                }
            }
            
            if (shock_degree == 1 )
            {
                shocks_to_delete[link_to_kill->id()]=link_to_kill;
                nodes_to_delete[(*vit).second->id()]=(*vit).second;
            }

        }
        
    }

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator sit;
    // Loop over all degree one nodes and delete
    for ( sit = shocks_to_delete.begin() ; sit != shocks_to_delete.end() ; 
        ++sit)
    {
        dbskfg_shock_link* slink = (*sit).second;

        if ( slink->left_contour_links().size() )
        {
             vcl_vector<dbskfg_composite_link_sptr> left_links=
                 slink->left_contour_links();

             for ( unsigned int i=0; i < left_links.size() ; ++i)
             {
                 dbskfg_contour_link* clink= dynamic_cast<dbskfg_contour_link*>
                     (&(*left_links[i]));
                 clink->delete_shock(slink->id());
             }
        }

        if ( slink->right_contour_links().size() )
        {
             vcl_vector<dbskfg_composite_link_sptr> right_links=
                 slink->right_contour_links();

             for ( unsigned int i=0; i < right_links.size(); ++i)
             {
                 dbskfg_contour_link* clink= dynamic_cast<dbskfg_contour_link*>
                     (&(*right_links[i]));
                 clink->delete_shock(slink->id());
             }
        }

        if ( slink->get_left_point())
        {
            slink->get_left_point()->delete_shock(slink->id());
        }

        if ( slink->get_right_point())
        {
            slink->get_right_point()->delete_shock(slink->id());
        }

        new_shock_links_.erase(slink->id());
        
    }

    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator nit;

    // Loop over all degree one nodes and delete
    for ( nit = nodes_to_delete.begin() ; nit != nodes_to_delete.end() ; 
        ++nit)
    {
        
        original_graph_->remove_vertex((*nit).second);

    }
}

void dbskfg_transformer::add_rag_nodes()
{
    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator it;
    for ( it = new_shock_links_.begin(); it != new_shock_links_.end() ; ++it)
    {
        dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
            (&(*((*it).second)));

        dbskfg_rag_node_sptr rag_node = new dbskfg_rag_node(
            rag_graph_->next_available_id());
        rag_graph_->add_vertex(rag_node);
                
        rag_node->add_shock_link(slink);
   
        slink->set_rag_node(&(*rag_node));

        new_rag_nodes_.push_back(rag_node->id());
    }
    





}

void dbskfg_transformer::undo_transform()
{

    // ********************** Delete Shock info ***************************
    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator it;
    for ( it = new_shock_links_.begin(); it != new_shock_links_.end() ; ++it)
    {
        for ( unsigned int c=0; c < transform_->contours_affected_.size(); ++c)
        {

            dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
                (&(*transform_->contours_affected_[c]));
            clink->delete_shock((*it).second->id());

            // Look at source and target
            dbskfg_contour_node* source = dynamic_cast<dbskfg_contour_node*>
                (&(*clink->source()));
            source->delete_shock((*it).second->id());
            
            // Look at source and target
            dbskfg_contour_node* target = dynamic_cast<dbskfg_contour_node*>
                (&(*clink->target()));
            target->delete_shock((*it).second->id());


        }

        for ( unsigned int d=0 ; d < transform_->contours_nodes_affected_.size()
                  ; ++d)
        {
            dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>
                (&(*transform_->contours_nodes_affected_[d]));
            cnode->delete_shock((*it).second->id());
        }

    }

    // *************** Remove shock nodes/links *****************************
    for ( it = new_shock_links_.begin(); it != new_shock_links_.end() ; ++it)
    {
       bool flag = original_graph_->remove_edge((*it).second);

    }

    // *************** Remove contour nodes/links ****************************
    for ( it = new_contour_links_.begin(); it != new_contour_links_.end() ; 
          ++it)
    {
        if ( (*it).second->source()->get_composite_degree())
        {
            (*it).second->source()->set_composite_degree(
                (*it).second->source()->get_composite_degree()-1);
        }

        if ( (*it).second->target()->get_composite_degree())
        {
            (*it).second->target()->set_composite_degree(
                (*it).second->target()->get_composite_degree()-1);
        }
       
        bool flag = original_graph_->remove_edge((*it).second);
       
       
    }

    // *************** Remove rag vertices **********************************
    vcl_vector<dbskfg_rag_node_sptr> rag_to_delete;
    vcl_map<unsigned int, vcl_pair<bool,dbskfg_rag_node_sptr> > rag_nodes;

    // Keep track of all shock links
    vcl_vector<vcl_map<unsigned int, dbskfg_shock_link*> > shock_to_rag;

    // Test if shock ray lies in vertex
    for (dbskfg_rag_graph::vertex_iterator vit = 
             rag_graph_->vertices_begin(); 
         vit != rag_graph_->vertices_end(); ++vit)
    {
        bool flag=false;
        for ( it = new_shock_links_.begin(); 
              it != new_shock_links_.end() ; ++it)
        {
            if ( (*vit)->contains_shock((*it).second))
            {
                flag=true;
                (*vit)->delete_shock((*it).second->id());
            }
        }
        
        if ( flag )
        {
            vcl_map<unsigned int, dbskfg_shock_link*>
                old_shock_links = (*vit)->get_shock_links();
            rag_to_delete.push_back(*vit);
            shock_to_rag.push_back(old_shock_links);
        }
    }
   
    for ( unsigned int k=0 ; k < rag_to_delete.size() ; ++k)
    {
      
        rag_graph_->remove_vertex(rag_to_delete[k]);
    }
   
    // ****************** Add back contour nodes/links ************************
    for ( unsigned int j=0; j < transform_->contours_to_remove_.size();
          ++j)
    {
       
        original_graph_->add_vertex(transform_->contours_to_remove_[j]);
        

    }

    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator cit;
    for ( cit = contours_removed_.begin() ; cit != contours_removed_.end() ;
          ++cit)
    {
        dbskfg_composite_link_sptr edge = (*cit).second;
        original_graph_->add_edge(edge);
        edge->source()->add_outgoing_edge(edge);
        edge->target()->add_incoming_edge(edge);
    }

   
 
    // ****************** Add back shock nodes/links **************************
    vcl_vector<dbskfg_composite_node_sptr> snodes
        = transform_->shock_nodes_affected_;

    for ( unsigned int j=0; j < snodes.size() ; ++j )
    {
   
        bool flag =original_graph_->add_vertex(snodes[j]);
    }

    vcl_vector<dbskfg_composite_link_sptr> slinks
        = transform_->shock_links_affected_;

    for ( unsigned int j=0; j < slinks.size() ; ++j )
    {
        original_graph_->add_edge(slinks[j]);
        slinks[j]->source()->add_outgoing_edge(slinks[j]);
        slinks[j]->target()->add_incoming_edge(slinks[j]);

                   
        dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>
        (&(*slinks[j]));

        vcl_vector<dbskfg_composite_link_sptr> left_contour_links
            = shock_link->left_contour_links();
        
        for ( unsigned int l=0; l < left_contour_links.size() ; ++l)
        {
            dbskfg_contour_link* contour_link = 
                dynamic_cast<dbskfg_contour_link*>
                  (&(*left_contour_links[l]));

            contour_link->shock_links_affected(shock_link);
        }
       
        vcl_vector<dbskfg_composite_link_sptr> right_contour_links
            = shock_link->right_contour_links();
        
        for ( unsigned int r=0; r < right_contour_links.size() ; ++r)
        {
            dbskfg_contour_link* contour_link = 
                dynamic_cast<dbskfg_contour_link*>
                  (&(*right_contour_links[r]));

            contour_link->shock_links_affected(shock_link);
        }
     
        if ( shock_link->get_left_point() )
        {
            shock_link->get_left_point()->shock_links_affected(shock_link);
        }
    
        if ( shock_link->get_right_point() )
        {
            shock_link->get_right_point()->shock_links_affected(shock_link);
        }

        if ( rag_nodes.count(shock_link->get_rag_node()->id()))
        {
            dbskfg_rag_node_sptr rag = 
                rag_nodes[shock_link->get_rag_node()->id()].second;
            rag->add_shock_link(shock_link);
        }
        else
        {
            rag_nodes[shock_link->get_rag_node()->id()]
                = vcl_make_pair(true,shock_link->get_rag_node());
            rag_nodes[shock_link->get_rag_node()->id()].second
                ->add_shock_link(shock_link);

        }
        
        

    }

    vcl_map<unsigned int, vcl_pair<bool,dbskfg_rag_node_sptr> >::iterator rit;
    for ( rit = rag_nodes.begin() ; rit != rag_nodes.end(); ++rit)
    {
        if ( (*rit).second.first )
        {
            rag_graph_->add_vertex((*rit).second.second);
        }
    }

    for ( unsigned int i=0; i < shock_to_rag.size() ; ++i)
    {
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator sit;
        vcl_map<unsigned int,dbskfg_shock_link*> shock_to_rag_map =
            shock_to_rag[i];
        for ( sit = shock_to_rag_map.begin() ; sit != shock_to_rag_map.end() ; 
              ++sit)
        {

            dbskfg_rag_node_sptr rag_node = new dbskfg_rag_node(
                rag_graph_->next_available_id());
            rag_graph_->add_vertex(rag_node);
                
            rag_node->add_shock_link((*sit).second);
   
            (*sit).second->set_rag_node(&(*rag_node));

        }

    }
    original_graph_->purge_isolated_vertices();

    classify_nodes();

}

void dbskfg_transformer::remove_extra_shocks()
{
    vcl_vector<dbskfg_composite_node_sptr> queue;

    for ( unsigned int k=0; k < transform_->outer_shock_nodes_.size() ; 
          ++k)
    {
        queue.push_back(transform_->outer_shock_nodes_[k]);
    }

    while(!queue.empty())
    {
        dbskfg_composite_node_sptr node = queue.back();
        queue.pop_back();

        dbskfg_composite_node::edge_iterator srit;
       
        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; 
              srit != node->in_edges_end() 
                  ; ++srit)
        {
           
            // Make sure shock link we havent seen
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->shock_links_affected_,
                     *srit))
            {
                dbskfg_composite_node_sptr opposite_node 
                    = (*srit)->opposite(node);
                
                if ( transform_->poly_.contains(opposite_node->pt().x(),
                                                opposite_node->pt().y()))
                {
                    transform_->shock_links_affected_.push_back(*srit);

                    if ( opposite_node->node_type() == dbskfg_composite_node::
                         SHOCK_NODE )
                    {
                        queue.push_back(opposite_node);
                    }
                }
            }
        }               

        // ************************ Out Edges ***************************
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
           
            // Make sure shock link we havent seen
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->shock_links_affected_,
                     *srit))
            {
                dbskfg_composite_node_sptr opposite_node 
                    = (*srit)->opposite(node);
                
                if ( transform_->poly_.contains(opposite_node->pt().x(),
                                                opposite_node->pt().y()))
                {
  
                    transform_->shock_links_affected_.push_back(*srit);

                    if ( opposite_node->node_type() == dbskfg_composite_node::
                         SHOCK_NODE )
                    {
                        queue.push_back(opposite_node);
                    }

                }
            }
        }               
     
 
    }

}
