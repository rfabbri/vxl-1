// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_expand_local_context.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_shock_link.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
// vgl headers
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
// vcl headers
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
// bsol headers
#include <bsol/bsol_algs.h>

dbskfg_expand_local_context::dbskfg_expand_local_context
( 
    dbskfg_transform_descriptor_sptr transform
)
    :transform_(transform)
{
    // Kick off the algorithm
    expand_local_context();

}

dbskfg_expand_local_context::~dbskfg_expand_local_context()
{

}

void dbskfg_expand_local_context::expand_local_context()
{
    unsigned int sheets = transform_->poly_.num_sheets();

    // Keep expanding polygon until it stops expanding
    // Stopping of expansion is indicating by the number of vertices staying
    // constant
    vcl_string prev_string,next_string;
    do
    {
        vcl_stringstream prev_stream,next_stream;

        // Get set of polygon points before expansion
        for ( unsigned int k=0; k < transform_->outer_shock_nodes_.size() ; 
              ++k)
        {
            prev_stream<<transform_->outer_shock_nodes_[k]->id();
        }
        prev_string = prev_stream.str();
         
        // Expand the polygon
        expand_polygon();

        // Get next set of polygon points
        for ( unsigned int k=0; k < transform_->outer_shock_nodes_.size() ; 
              ++k)
        {
            next_stream<<transform_->outer_shock_nodes_[k]->id();
        }
        next_string = next_stream.str();
        
    }while(prev_string != next_string);

    if ( sheets == 1 )
    {
        // Make sure final transform polygon only has one sheet
        // pick largest area sheet
        vgl_polygon<double> start_poly = transform_->poly_;

        transform_->poly_.clear();

        // Keep largest area polygon
        double area=0;
        unsigned int index=0;

        for (unsigned int s = 0; s < start_poly.num_sheets(); ++s)
        { 

            vgl_polygon<double> tempy(start_poly[s]);
            double area_temp = vgl_area(tempy);
            if ( area_temp > area )
            {
                area = area_temp;
                index=s;

            }

        }

        transform_->poly_.push_back(start_poly[index]);
    }



    // Add in contour nodes affected if necessary
    vcl_map<unsigned int,dbskfg_contour_node*>::iterator it;
    for ( it = degree_ones_.begin() ; it != degree_ones_.end() ; ++it)
    {
        
        if ( !dbskfg_utilities::is_node_in_set_of_links(
                 transform_->contours_affected_,
                 (*it).second))
        {

            transform_->contours_nodes_affected_.push_back(
                (*it).second);
        }

    }
}

void dbskfg_expand_local_context::expand_polygon()
{

    //Loop over all outer shock nodes and see which ones need to be expanded

    //Mark ids for deletion 
    vcl_vector<unsigned int> outer_shock_nodes_to_delete;
    vcl_vector<dbskfg_composite_node_sptr > outer_shock_nodes_to_add;

    // Keep a countour map
    vcl_map<unsigned int,vcl_string> contour_map;

    for ( unsigned int k=0; k < transform_->outer_shock_nodes_.size() ; 
          ++k)
    {
        double distance=distance_from_ess(transform_->
                                          outer_shock_nodes_[k]->pt());
        dbskfg_composite_node_sptr node = (transform_->outer_shock_nodes_[k]);
        dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>(
            &(*node));

        // If this is true the outer shock node needs to be expanded
        // Lets flag it for deletion and add it to the set of inner shock 
        // nodes later
        // The nodes could be very,very close to each other, so lets make
        // sure that up to four digits they are equal
        vcl_stringstream srad,sdistance;
        srad<<snode->get_radius();
        sdistance<<distance;

        if ( distance < snode->get_radius() && srad.str() != sdistance.str() )
        {
            outer_shock_nodes_to_delete.push_back(node->id());
            transform_->shock_nodes_affected_.push_back(node);
            
            dbskfg_composite_node::edge_iterator srit;

            // ************************ In Edges ***************************
            for ( srit = node->in_edges_begin() ; 
                  srit != node->in_edges_end() 
                      ; ++srit)
            {
                // Test if shock link
                if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
                {
                    dbskfg_shock_link* shock_link=
                        dynamic_cast<dbskfg_shock_link*>(&(*(*srit)));

                       if ( shock_link->shock_link_type() != 
                            dbskfg_shock_link::SHOCK_RAY )
                       {
                           // Make sure shock link we havent seen
                           if ( !dbskfg_utilities::is_link_in_set_of_links(
                                    transform_->shock_links_affected_,
                                    *srit))
                           {
                               // We need to do add contours, expand polygon
                               // and tag this shock link as affected,
                               // we also will keep track of outer shock nodes
                               // to add
                        
                               // 1 Tag shock link
                               transform_->
                                   shock_links_affected_.push_back(*srit);

                               // 2 Add contours
                               add_contours(shock_link);
                        
                               // 3 Expand Polygon
                               add_to_polygon(shock_link);
                               
                               // 4 Keep track of expanding wavefront
                               outer_shock_nodes_to_add.push_back(
                                   (*srit)->opposite(node));
                           }
                       }
                }
            }
            
            // *********************** Out Edges ***************************
            for ( srit = node->out_edges_begin() ; 
                  srit != node->out_edges_end() 
                      ; ++srit)
            {
                // Test if shock link
                if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
                {
                    dbskfg_shock_link* shock_link=
                        dynamic_cast<dbskfg_shock_link*>(&(*(*srit)));

                    if ( shock_link->shock_link_type() != 
                         dbskfg_shock_link::SHOCK_RAY )
                    {
                        // Make sure shock link we havent seen
                        if ( !dbskfg_utilities::is_link_in_set_of_links(
                                 transform_->shock_links_affected_,
                                 *srit))
                        {
                            // We need to do add contours, expand polygon
                            // and tag this shock link as affected,
                            // we also will keep track of outer shock nodes
                            // to add
                        
                            // 1 Tag shock link
                            transform_->
                                shock_links_affected_.push_back(*srit);

                            // 2 Add contours
                            add_contours(shock_link);
                        
                            // 3 Expand Polygon
                            add_to_polygon(shock_link);

                            // 4 Keep track of expanding wavefront
                            outer_shock_nodes_to_add.push_back(
                                (*srit)->opposite(node));

                        }
                    }
                }
            }
 
        }
    }

    // Finally we have to delete outer shock nodes
    // Erase outer shock nodes and add inner shock nodes
    for ( unsigned int d=0; d < outer_shock_nodes_to_delete.size() ; ++d)
    {
        unsigned int id=outer_shock_nodes_to_delete[d];

        vcl_vector<dbskfg_composite_node_sptr>::iterator it;

        for ( it=transform_->outer_shock_nodes_.begin(); 
              it != transform_->outer_shock_nodes_.end() ; 
              ++it)
        {
            if ((*it)->id() == id)
            {
                break;
            }
        }

        transform_->outer_shock_nodes_.erase(it);
    }


    // Now lets add in the new shock nodes to expand wavefront
    for ( unsigned int a=0; a < outer_shock_nodes_to_add.size() ; ++a)
    {
        // Add in only types of shock nodes that are shocks and not contours
        
        if ( outer_shock_nodes_to_add[a]->node_type() == 
             dbskfg_composite_node::SHOCK_NODE )
        { 
            transform_->outer_shock_nodes_.push_back(
                outer_shock_nodes_to_add[a]);
        }
    }
}

void dbskfg_expand_local_context::add_contours(dbskfg_shock_link* shock_link)
{

    // Grab contours right line, left point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::RLLP )
    {
        vcl_vector<dbskfg_composite_link_sptr>
            clinks = shock_link->right_contour_links();
                        
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->contours_affected_,
                     clinks[i]))
            {
                transform_->contours_affected_.push_back(clinks[i]);
            }    
        }                                

    }
    
    // Grab contours left line right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::LLRP )
    {

        vcl_vector<dbskfg_composite_link_sptr>
            clinks = shock_link->left_contour_links();
         
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->contours_affected_,
                     clinks[i]))
            {
                transform_->contours_affected_.push_back(clinks[i]);
            }
        }                                

    }

    // Grab contours left line and right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::LL )
    {

        // Left Line
        vcl_vector<dbskfg_composite_link_sptr>
            clinks = shock_link->left_contour_links();
                        
        for ( unsigned int i=0; i<clinks.size() ; ++i)
        {
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->contours_affected_,
                     clinks[i]))
            {
                transform_->contours_affected_.push_back(clinks[i]);
            }
            
        }                                
        
        // Right line
        vcl_vector<dbskfg_composite_link_sptr>
            rclinks = shock_link->right_contour_links();
                     
        for ( unsigned int i=0; i<rclinks.size() ; ++i)
        {
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     transform_->contours_affected_,
                     rclinks[i]))
            {
                transform_->contours_affected_.push_back(rclinks[i]);
            }

        }


    }

    // Grab contours left line and right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::PP )
    {
        dbskfg_contour_node* cnode1 = shock_link->get_left_point();
        dbskfg_contour_node* cnode2 = shock_link->get_right_point();

        
        if ( cnode1->get_composite_degree()==1 && 
             transform_->endpoint_in_gap(cnode1) == false )
        {
            degree_ones_[cnode1->id()]=cnode1;
        }

        if ( cnode2->get_composite_degree()==1 && 
             transform_->endpoint_in_gap(cnode2) == false )
        {
            degree_ones_[cnode2->id()]=cnode2;
        }

    }
}

void dbskfg_expand_local_context::add_to_polygon(dbskfg_shock_link* shock_link)
{

    //Take temp
    vgl_polygon<double> temp(1);
    temp  = shock_link->polygon();
    
    //Keep a flag for status
    int value;

    //Take union of two polygons
    transform_->poly_ = vgl_clip(transform_->poly_, 
                                temp,                 
                                vgl_clip_type_union, 
                                &value);

    // Make sure operation worked
    assert(value == 1);


}

double dbskfg_expand_local_context::
distance_from_ess(vgl_point_2d<double> test_point)
{
    vcl_vector<double> distances;

    // Loop over all parts of new contours spatial objects and record 
    // distances
    for ( unsigned int t=0; 
          t < transform_->new_contours_spatial_objects_.size() ; ++t)
    {
        if ( transform_->new_contours_spatial_objects_[t]->cast_to_curve()
             ->cast_to_line())
        {
            // Grab line
            vsol_line_2d* line = 
                transform_->new_contours_spatial_objects_[t]
                ->cast_to_curve()->cast_to_line();

            vgl_point_2d<double> p0=vgl_point_2d<double>(line->p0()->x(),
                                                         line->p0()->y());
            vgl_point_2d<double> p1=vgl_point_2d<double>(line->p1()->x(),
                                                         line->p1()->y());
            vgl_line_segment_2d<double> line_seg =
                vgl_line_segment_2d<double>(p0,
                                            p1);
             
            // Push back line distance
            distances.push_back(vgl_distance(line_seg,test_point));

        }
        else
        {
            // Grab poly line
            vsol_polyline_2d* poly_line = 
                transform_->new_contours_spatial_objects_[t]
                ->cast_to_curve()->cast_to_polyline();
    

            // Grab distance to poly line
            // We have to also account for distances to the individual points
            for ( unsigned int v=0; v < poly_line->size()-1 ; ++v )
            {
                vgl_point_2d<double> p0=
                    vgl_point_2d<double>(poly_line->vertex(v)->x(),
                                         poly_line->vertex(v)->y());
                vgl_point_2d<double> p1=
                    vgl_point_2d<double>(poly_line->vertex(v+1)->x(),
                                         poly_line->vertex(v+1)->y());
                vgl_line_segment_2d<double> line_seg=
                    vgl_line_segment_2d<double>(p0,
                                                p1);
                distances.push_back(vgl_distance(line_seg,test_point));
                distances.push_back(vgl_distance(p0,test_point));
                distances.push_back(vgl_distance(p1,test_point));

      
            }

        }

    }

    return *vcl_min_element(distances.begin(),distances.end());

}
