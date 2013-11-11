// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph.cxx

//:
// \file
#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/algo/dbskfg_transformer.h>
#include <dbskfg/dbskfg_utilities.h>
#include <vcl_iostream.h>
#include <vgl/vgl_clip.h>
#include <vcl_algorithm.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_shock_node.h>

dbskfg_detect_transforms::dbskfg_detect_transforms
( 
    dbskfg_composite_graph_sptr composite_graph,
    vil_image_resource_sptr image
)
    :composite_graph_(composite_graph),
     image_(image),
     gap_detector_(composite_graph,image_),
     loop_detector_(composite_graph,image_),
     rag_node_(0)
{
}

dbskfg_detect_transforms::~dbskfg_detect_transforms()
{
    if ( composite_graph_ )
    {
        composite_graph_=0;
    }

    if ( image_ )
    {
        image_ = 0;
    }
    for (unsigned int i=0; i < transforms_.size() ; ++i)
    {
        transforms_[i]=0;
    }

    transforms_.clear();
    transform_neighbors_.clear();
}

unsigned int dbskfg_detect_transforms::transforms_valid(double threshold)
{
    unsigned int numb_transforms(0);

    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {
        
        if ( transforms_[t]->cost_ < threshold && 
             !transforms_[t]->degree_1_nodes() &&
             transform_affects_region(transforms_[t]) )
        {
            numb_transforms++;
        }
      
    }
    
    return numb_transforms;
}

void dbskfg_detect_transforms::detect_transforms(bool detect_gaps,
                                                 bool detect_loops,
                                                 double ess_completion,
                                                 double alpha)
{
    unsigned int gaps_found=0;
    unsigned int loops_found=0;

    if ( detect_gaps)
    {
        vcl_cout<<"************ Detecting Gaps ***************"<<vcl_endl;
        gap_detector_.set_ess_completion(ess_completion);
        gap_detector_.set_alpha(alpha);
        gap_detector_.detect_gaps(transforms_);
        gaps_found = transforms_.size();
        vcl_cout<<"Gaps Found: "<<gaps_found<<vcl_endl;
    }

    if ( detect_loops )
    {
        vcl_cout<<"************ Detecting Loops *****************"<<vcl_endl;
        loop_detector_.set_alpha(alpha);
        loop_detector_.detect_loops(transforms_);
        loops_found=transforms_.size() - gaps_found;
        vcl_cout<<"Loops Found: "<<loops_found<<vcl_endl;
    }
    vcl_cout<<"Total Number of Transforms: "<<transforms_.size()<<vcl_endl;

    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {
        transforms_[t]->trim_transform();
        transforms_[t]->id_ = t;
        transform_neighbors_[t].clear();
        if ( 
            transforms_[t]->transform_type_ == 
            dbskfg_transform_descriptor::GAP)
        {
           transforms_[t]->all_gaps_.push_back(transforms_[t]->gap_);
        }

    }
 
    // Determine all transforms that are intersecting
    for ( unsigned int i=0; i < transforms_.size() ; ++i)
    {

        vgl_polygon<double> polygon = transforms_[i]->poly_;
     
        unsigned int j=i+1;
        for (  ; j < transforms_.size() ; ++j)
        {
            //Keep a flag for status
            int value;

            // Store result
            vgl_polygon<double> intersection;

            //Take union of two polygons
            intersection = vgl_clip(polygon,                 // p1
                                    transforms_[j]->poly_,   // p2
                                    vgl_clip_type_intersect, // p1 U p2
                                    &value);                 // test if success

            assert(value==1);

            if ( intersection.num_sheets() > 0 && 
                 intersection.num_vertices() > 0 )
            {
                transform_neighbors_[i].push_back(j);
                transform_neighbors_[j].push_back(i);
            }
        }
    }

}

void dbskfg_detect_transforms::detect_transforms_simple(bool detect_gaps,
                                                        bool detect_loops,
                                                        double ess_completion,
                                                        double alpha)
{
    unsigned int gaps_found=0;
    unsigned int loops_found=0;

    if ( detect_gaps)
    {
        vcl_cout<<"************ Detecting Gaps ***************"<<vcl_endl;
        gap_detector_.set_ess_completion(ess_completion);
        gap_detector_.set_alpha(0.4);
        gap_detector_.detect_gaps(dbskfg_transform_manager::Instance()
                                  .objects());

        gaps_found = transforms_.size();
        vcl_cout<<"Gaps Found: "<<gaps_found<<vcl_endl;
    }

    if ( detect_loops )
    {
        vcl_cout<<"************ Detecting Loops *****************"<<vcl_endl;
        loop_detector_.set_alpha(1);
        loop_detector_.detect_loops(dbskfg_transform_manager::Instance()
                                    .objects());
        loops_found=transforms_.size() - gaps_found;
        vcl_cout<<"Loops Found: "<<loops_found<<vcl_endl;
    }
    vcl_cout<<"Total Number of Transforms: "<<transforms_.size()<<vcl_endl;

    dbskfg_transform_manager::Instance().update_transforms_conflicts();

}


// Apply all transforms below threshold
unsigned int dbskfg_detect_transforms::apply_transforms(
    double threshold,
    dbskfg_rag_graph_sptr rag_graph) 
{
    vcl_cout<<"Number of Transforms to Apply: "
            <<transforms_valid(threshold)
            <<vcl_endl;
    // Keep count of number of transforms actually applied
    unsigned int numb_transforms = 0;

    for ( unsigned int k=0; k < transforms_.size() ; ++k)
    {
        if ( !transforms_[k]->processed_ && transforms_[k]->cost_< threshold
             && !transforms_[k]->degree_1_nodes() && 
             transform_affects_region(transforms_[k]) )
        {
            transforms_[k]->processed_=true;
            vcl_vector<unsigned int> neighbors = transform_neighbors_
                [transforms_[k]->id_];

            // Apply the actual transform
            dbskfg_transformer transformer(transforms_[k],
                                           composite_graph_,
                                           rag_graph);

            bool status = transformer.status();
            
            if ( status )
            {
                numb_transforms++;
                for ( unsigned int l=0; l < neighbors.size() ; ++l)
                {
                     transforms_[neighbors[l]]->processed_=true;
                }
            }
          
        }
    }

    vcl_cout<<"Numb transforms: "<<numb_transforms<<vcl_endl;
    // After all transforms have been done lets grow the fragments
    if ( numb_transforms > 0 ) 
    {
        dbskfg_region_growing_transforms region_growing(rag_graph);
        region_growing.grow_regions();
    }

    return numb_transforms;
}


// Apply a specific transform
void dbskfg_detect_transforms::apply_transforms(
    unsigned int transform_id,
    dbskfg_rag_graph_sptr rag_graph) 
{
    unsigned int k= transform_id;

    if ( !transforms_[k]->processed_ )
    {
        transforms_[k]->processed_=true;
        vcl_vector<unsigned int> neighbors = transform_neighbors_
            [transforms_[k]->id_];
        for ( unsigned int l=0; l < neighbors.size() ; ++l)
        {
            transforms_[neighbors[l]]->processed_=true;
        }
        
        // Apply the actual transform
        dbskfg_transformer transformer(transforms_[k],
                                       composite_graph_,
                                       rag_graph);
   
    }

    // After all transforms have been done lets grow the fragments
    dbskfg_region_growing_transforms region_growing(rag_graph);
    region_growing.grow_regions();
    
}

bool dbskfg_detect_transforms::transform_affects_region(
    dbskfg_transform_descriptor_sptr& transform)
{
    if  ( !rag_node_ )
    {
        return true;
    }

    //: Get wavefront
    vcl_map<unsigned int, dbskfg_shock_node*> wavefront = 
        rag_node_->get_wavefront();

    vcl_map<unsigned int,dbskfg_shock_node*>::iterator it;
    for ( it = wavefront.begin() ; it != wavefront.end() ; ++it)
    {
    
        if ( dbskfg_utilities::
             is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                     (*it).second) ||
             dbskfg_utilities::
             is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*it).second) )
        {
            return true;
        }

    }

    return false;
}

void dbskfg_detect_transforms::transform_affects_region(
    dbskfg_rag_node_sptr& rag_node,
    vcl_set<unsigned int>& rag_con_ids)
{
 
    vcl_vector<dbskfg_composite_node_sptr> con_endpoints;
    rag_node->determine_contour_points(con_endpoints);

    gap_detector_.set_ess_completion(0.25);
    gap_detector_.set_alpha(0.4);

    // Run on all gaps first
    gap_detector_.detect_gaps_endpoint(transforms_,
                 con_endpoints);


    vcl_map<unsigned int, dbskfg_composite_node_sptr> local_map;

    // Insert all con_nodes into local map
    for ( unsigned int c=0; c < con_endpoints.size() ; ++c )
    {
        dbskfg_contour_node* con_node = dynamic_cast<dbskfg_contour_node*>
            (&(*(con_endpoints[c])));

        if ( !rag_con_ids.count(con_node->contour_id()))
        {
            local_map[con_endpoints[c]->id()]=con_endpoints[c];
        }
    }

    // Now loop through transforms and find all those to consider
    for ( unsigned int i=0; i < transforms_.size() ; ++i )
    {
        
        dbskfg_contour_node* con_node = dynamic_cast<dbskfg_contour_node*>
            (&(*(transforms_[i]->gap_.first)));
        dbskfg_contour_node* con_node2 = dynamic_cast<dbskfg_contour_node*>
            (&(*(transforms_[i]->gap_.second)));

        if ( !rag_con_ids.count(con_node->contour_id()) )
        {
            local_map[con_node->id()] = transforms_[i]->gap_.first;
        }
        
        if ( !rag_con_ids.count(con_node2->contour_id() ))
        {
            local_map[con_node2->id()] = transforms_[i]->gap_.second;
        }
    }    

    // Create loop endpoints
    vcl_vector<dbskfg_composite_node_sptr> loop_endpoints;
    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator it;
    for ( it = local_map.begin() ; it != local_map.end() ; ++it)
    {
        loop_endpoints.push_back((*it).second);
    }

    loop_detector_.detect_loops(transforms_,
                               loop_endpoints);

    vcl_map<vcl_string,vcl_vector<double> > gap_costs;

    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {

        transforms_[t]->id_ = t;
        transform_neighbors_[t].clear();
        if ( 
            transforms_[t]->transform_type_ == 
            dbskfg_transform_descriptor::GAP)
        {
            transforms_[t]->all_gaps_.push_back(transforms_[t]->gap_);

            vcl_pair<vcl_string,vcl_string> gap_endpoints = 
                transforms_[t]->gap_endpoints();
          
            gap_costs[gap_endpoints.first].push_back(transforms_[t]->cost_);
            gap_costs[gap_endpoints.second].push_back(transforms_[t]->cost_);

        }

    }


    // Reupdate the costs for all loops
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {

        if ( transforms_[t]->transform_type_ == dbskfg_transform_descriptor::
             LOOP)
        {
            // Make sure loop is not in set of rag node points
            vcl_vector<dbskfg_composite_node_sptr> loop_endpoints
                = transforms_[t]->loop_endpoints_;
            
            bool invalid_transform=false;

            vcl_vector<unsigned int> contour_ids_affected = transforms_[t]
                ->contour_ids_affected();

            // Print out nodes
            vcl_set<unsigned int>::iterator it;
            
            for ( it = rag_con_ids.begin() ; it != rag_con_ids.end() ; ++it)
            {
            
                if ( vcl_find(contour_ids_affected.begin(),
                              contour_ids_affected.end(),
                              *it) != contour_ids_affected.end())
                {
                    transforms_[t]->cost_ = 0;
                    invalid_transform=true;
                    break;
                }
            }

            if ( invalid_transform )
            {
                continue;
            }

            if ( transforms_[t]->loop_endpoints_.size() )
            {
                vcl_stringstream sstream1;
                sstream1<<transforms_[t]->loop_endpoints_[0]->pt();
        
                vcl_stringstream sstream2;
                sstream2<<transforms_[t]->loop_endpoints_[1]->pt();
                
                if ( gap_costs.count(sstream1.str()) && 
                     !gap_costs.count(sstream2.str()) )
                {
                    double c1 = *vcl_min_element(
                        gap_costs[sstream1.str()].begin(),
                        gap_costs[sstream1.str()].end());

                    transforms_[t]->cost_ = 1.0-c1;
                }
                else if ( gap_costs.count(sstream2.str()) &&
                          !gap_costs.count(sstream1.str()))
                {
                    double c1 = *vcl_min_element(
                        gap_costs[sstream2.str()].begin(),
                        gap_costs[sstream2.str()].end());
                    
                    transforms_[t]->cost_ = 1.0-c1;
                }
                else if ( gap_costs.count(sstream2.str()) &&
                          gap_costs.count(sstream1.str()))
                {
                     double c1 = *vcl_min_element(
                        gap_costs[sstream1.str()].begin(),
                        gap_costs[sstream1.str()].end());
                     double c2 = *vcl_min_element(
                        gap_costs[sstream2.str()].begin(),
                        gap_costs[sstream2.str()].end());

                     
                     transforms_[t]->cost_ = 1.0-vcl_min(c1,c2);

                }
                else
                {
                    transforms_[t]->cost_ = 1;
                }
            }
            else
            {
                transforms_[t]->cost_ = 1;
            }
        }
    }
    
    vcl_sort(transforms_.begin(),transforms_.end(),
             dbskfg_utilities::comparison);
    vcl_reverse(transforms_.begin(),transforms_.end());

    for ( unsigned int v=0; v < transforms_.size() ; ++v)
    {

        if ( transforms_[v]->cost_ <  
             dbskfg_transform_manager::Instance().get_threshold())
        {
            transforms_.erase(transforms_.begin()+v,
                              transforms_.end());
            break;
        }
    }

    
    
}

void dbskfg_detect_transforms::transform_affects_region(
    dbskfg_rag_node_sptr& rag_node,
    double ess,
    double alpha)
{
 
    vcl_vector<dbskfg_composite_node_sptr> con_endpoints;
    rag_node->determine_contour_points(con_endpoints);

    gap_detector_.set_ess_completion(ess);
    gap_detector_.set_alpha(alpha);

    // Run on all gaps first
    gap_detector_.detect_gaps_endpoint(transforms_,
                 con_endpoints);

    vcl_map<unsigned int, dbskfg_composite_node_sptr> local_map;

    // Insert all con_nodes into local map
    for ( unsigned int c=0; c < con_endpoints.size() ; ++c )
    {
        dbskfg_contour_node* con_node = dynamic_cast<dbskfg_contour_node*>
            (&(*(con_endpoints[c])));

        local_map[con_endpoints[c]->id()]=con_endpoints[c];
        
    }

    // Now loop through transforms and find all those to consider
    for ( unsigned int i=0; i < transforms_.size() ; ++i )
    {
        
        dbskfg_contour_node* con_node = dynamic_cast<dbskfg_contour_node*>
            (&(*(transforms_[i]->gap_.first)));
        dbskfg_contour_node* con_node2 = dynamic_cast<dbskfg_contour_node*>
            (&(*(transforms_[i]->gap_.second)));

        if ( con_node->get_composite_degree() == 1 )
        {
            local_map[con_node->id()] = transforms_[i]->gap_.first;
        }

        if ( con_node2->get_composite_degree() == 1 )
        {
            local_map[con_node2->id()] = transforms_[i]->gap_.second;
        }
    }    

    // Create loop endpoints
    vcl_vector<dbskfg_composite_node_sptr> loop_endpoints;
    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator it;
    for ( it = local_map.begin() ; it != local_map.end() ; ++it)
    {
        loop_endpoints.push_back((*it).second);
    }

    loop_detector_.detect_loops(transforms_,
                               loop_endpoints);

    vcl_map<vcl_string,vcl_vector<double> > gap_costs;

    // Assign id to all transforms
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {
        transforms_[t]->trim_transform();
        transforms_[t]->id_ = t;
        transform_neighbors_[t].clear();
        if ( 
            transforms_[t]->transform_type_ == 
            dbskfg_transform_descriptor::GAP)
        {
            transforms_[t]->all_gaps_.push_back(transforms_[t]->gap_);

            vcl_pair<vcl_string,vcl_string> gap_endpoints = 
                transforms_[t]->gap_endpoints();
          
            gap_costs[gap_endpoints.first].push_back(transforms_[t]->cost_);
            gap_costs[gap_endpoints.second].push_back(transforms_[t]->cost_);

        }

    }


    // Reupdate the costs for all loops
    for ( unsigned int t=0; t < transforms_.size() ; ++t)
    {

        if ( transforms_[t]->transform_type_ == dbskfg_transform_descriptor::
             LOOP)
        {

            if ( transforms_[t]->loop_endpoints_.size() )
            {
                vcl_stringstream sstream1;
                sstream1<<transforms_[t]->loop_endpoints_[0]->pt();
        
                vcl_stringstream sstream2;
                sstream2<<transforms_[t]->loop_endpoints_[1]->pt();
                
                if ( gap_costs.count(sstream1.str()) && 
                     !gap_costs.count(sstream2.str()) )
                {
                    double c1 = *vcl_min_element(
                        gap_costs[sstream1.str()].begin(),
                        gap_costs[sstream1.str()].end());

                    transforms_[t]->cost_ = 1.0-c1;
                }
                else if ( gap_costs.count(sstream2.str()) &&
                          !gap_costs.count(sstream1.str()))
                {
                    double c1 = *vcl_min_element(
                        gap_costs[sstream2.str()].begin(),
                        gap_costs[sstream2.str()].end());
                    
                    transforms_[t]->cost_ = 1.0-c1;
                }
                else if ( gap_costs.count(sstream2.str()) &&
                          gap_costs.count(sstream1.str()))
                {
                     double c1 = *vcl_min_element(
                        gap_costs[sstream1.str()].begin(),
                        gap_costs[sstream1.str()].end());
                     double c2 = *vcl_min_element(
                        gap_costs[sstream2.str()].begin(),
                        gap_costs[sstream2.str()].end());

                     
                     transforms_[t]->cost_ = 1.0-vcl_min(c1,c2);

                }
                else
                {
                    transforms_[t]->cost_ = 1;
                }
            }
            else
            {
                transforms_[t]->cost_ = 1;
            }
        }
    }
    
    vcl_sort(transforms_.begin(),transforms_.end(),
             dbskfg_utilities::comparison);
    vcl_reverse(transforms_.begin(),transforms_.end());

    for ( unsigned int v=0; v < transforms_.size() ; ++v)
    {
   
        if ( transforms_[v]->cost_ <  
             dbskfg_transform_manager::Instance().get_threshold())
        {
            transforms_.erase(transforms_.begin()+v,
                              transforms_.end());
            break;
        }
    }

    
    
}
