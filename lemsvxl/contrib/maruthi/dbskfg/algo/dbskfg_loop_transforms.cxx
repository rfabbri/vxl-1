// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_loop_transforms.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>
// bsol headers
#include <bsol/bsol_algs.h>
#include <bsol/bsol_intrinsic_curve_2d.h>
// vgl headers
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_distance.h>
// vnl headers
#include <vnl/vnl_vector_fixed.h>
// vil headers
#include <vil/vil_bilin_interp.h>
#include <vil/vil_image_resource.h>
// bil headers
#include <bil/algo/bil_color_conversions.h>
// vcl headers
#include <vcl_algorithm.h>


dbskfg_loop_transforms::dbskfg_loop_transforms
( 
    dbskfg_composite_graph_sptr composite_graph,
    vil_image_resource_sptr img_sptr
)
    :composite_graph_(composite_graph),
     longest_contour_(1.0),
     color_gamma_(14.0f),
     alpha_(0.5),
     image_(img_sptr)
{
    if ( image_ )
    {
        vil_image_view<vxl_byte> I = image_->get_view();
     
        if ( I.nplanes() == 3 )
        {
            convert_RGB_to_Lab(I, L_, A_, B_);
        }
    }

}

dbskfg_loop_transforms::~dbskfg_loop_transforms()
{


}

void dbskfg_loop_transforms::detect_loops(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{

    // Reset longest contour
    longest_contour_=1.0;

    vcl_map<unsigned int,vcl_string> visited_links;

    for (dbskfg_composite_graph::edge_iterator eit =
             composite_graph_->edges_begin();
         eit != composite_graph_->edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link = *eit;
        
        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
                (&(*link));

            if ( visited_links.count(link->id()) == 0)
            {
                visited_links[link->id()]="temp";
                expand_link(link,objects,visited_links);
            }
          
        }
    }

    // Normalize all costs
    for ( unsigned int k=0; k < objects.size() ; ++k)
    {
        if ( objects[k]->transform_type_ == dbskfg_transform_descriptor::
             LOOP)
        {
            // Normalize all costs by longest contour
            objects[k]->contour_cost_ = objects[k]->contour_cost_/
                longest_contour_;
            objects[k]->cost_ = (alpha_)*objects[k]->contour_cost_+
                (1.0-alpha_)*objects[k]->app_cost_;
            objects[k]->cost_ = objects[k]->contour_cost_;
      
        }

    }
}

void dbskfg_loop_transforms::detect_loops(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
    vcl_vector<dbskfg_composite_node_sptr>& con_endpoints)
{
   
    // Reset longest contour
    longest_contour_=1.0;

    vcl_map<unsigned int,vcl_string> visited_links;

    for (unsigned int c=0; c <con_endpoints.size() ; ++c)
    {
        dbskfg_composite_node_sptr con_endpoint = con_endpoints[c];

        dbskfg_composite_link_sptr link(0);
        if ( con_endpoint->in_degree() ) 
        {
            link = *(con_endpoint->in_edges_begin());
        }
        else
        {

            link = *(con_endpoint->out_edges_begin());
        }

        dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
            (&(*link));
        
        if ( visited_links.count(link->id()) == 0)
        {
            visited_links[link->id()]="temp";
            expand_link(link,objects,visited_links);
        }
        
    }
  
    // Normalize all costs
    for ( unsigned int k=0; k < objects.size() ; ++k)
    {
        if ( objects[k]->transform_type_ == dbskfg_transform_descriptor::
             LOOP)
        {
            // Normalize all costs by longest contour
            objects[k]->contour_cost_ = objects[k]->contour_cost_/
                longest_contour_;
            objects[k]->cost_ = (alpha_)*objects[k]->contour_cost_+
                (1.0-alpha_)*objects[k]->app_cost_;
            objects[k]->cost_ = objects[k]->contour_cost_;
            
        }

    }

}

void dbskfg_loop_transforms::detect_loops(
    dbskfg_transform_descriptor_sptr& transform)
{
    // Clean out transform before we use it
    transform->contours_affected_.clear();
    transform->contours_nodes_affected_.clear();
    transform->shock_nodes_affected_.clear();
    transform->shock_links_affected_.clear();
    transform->contours_spatial_objects_.clear();
    transform->outer_shock_nodes_.clear();
    transform->new_contours_spatial_objects_.clear();
    transform->poly_.clear();
    transform->all_gaps_.clear();
    transform->polygon_string_rep_.clear();


    // Create two maps to hold everything
    // Holds all shocks affected
    vcl_map<unsigned int, dbskfg_shock_link*> shock_map;
    vcl_map<unsigned int, vcl_string> visited_nodes;
    vcl_map<unsigned int, vcl_string> contour_map;
    vcl_map<unsigned int, vcl_string> contour_node_map;
    vcl_map<unsigned int,dbskfg_composite_link_sptr> loop_segment_map;


    // Need to move over to original set

    // Define node/link iteraros for printing
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;

    vcl_map<unsigned int, unsigned int> local_map;


    // loop endpoints
    for ( unsigned int k=0; k < transform->loop_endpoints_.size() ; ++k )
    {
        dbskfg_composite_node_sptr new_node = composite_graph_
            ->find_node(transform->loop_endpoints_[k]->id());

        if ( new_node->get_composite_degree() == 1 || 
             new_node->get_composite_degree() >= 3)
        {
            transform->loop_endpoints_[k]=new_node;
        }
        else
        {
            transform->loop_endpoints_[k]=new_node;
            local_map[new_node->id()]=k;
        }
        
    }

    vcl_map<unsigned int, unsigned int>::iterator sit;
    for ( sit = local_map.begin() ; sit != local_map.end() ; ++sit)
    {
        transform->loop_endpoints_.erase(transform->loop_endpoints_.begin()+
                                         (*sit).second);
    }

    // loop endpoints
    for ( unsigned int k=0; k < transform->contours_to_remove_.size() ; ++k )
    {
        dbskfg_composite_node_sptr new_node = composite_graph_
            ->find_node(transform->contours_to_remove_[k]->id());

        if ( local_map.count(new_node->id()) == 0 )
        {
            transform->contours_to_remove_[k]=new_node;
        }
        else
        {
            transform->contours_to_remove_[k]=new_node;
            local_map[new_node->id()]=k;
        }
        
    }

    for ( sit = local_map.begin() ; sit != local_map.end() ; ++sit)
    {
        transform->contours_to_remove_.erase(
            transform->contours_to_remove_.begin()+
            (*sit).second);
    }

    // contour links to remove
    for ( lit = transform->contour_links_to_remove_.begin() ; 
          lit != transform->contour_links_to_remove_.end() ; ++lit )
    {
        dbskfg_composite_link_sptr new_link = composite_graph_
            ->find_link((*lit)->id());
        (*lit) = new_link;
    }

    for ( unsigned int i=0; i < transform->contour_links_to_remove_.size()
              ; ++i)
    {
        dbskfg_composite_node_sptr source = 
            transform->contour_links_to_remove_[i]->source();
        dbskfg_composite_node_sptr target = 
            transform->contour_links_to_remove_[i]->target();
        dbskfg_composite_link_sptr link =
            transform->contour_links_to_remove_[i];

        // Grab all shocks affected for this node
        dbskfg_contour_node* source_cnode = 
            dynamic_cast<dbskfg_contour_node*>(&(*(source)));

        // Grab all shocks affected for this node
        dbskfg_contour_node* target_cnode = 
            dynamic_cast<dbskfg_contour_node*>(&(*(target)));

        // Grab all shocks affected for this node
        dbskfg_contour_link* clink = 
            dynamic_cast<dbskfg_contour_link*>(&(*link));

        if ( source_cnode->get_composite_degree() < 3 )
        {
            
            vcl_vector<dbskfg_shock_link*> shocks_n = 
                source_cnode->shocks_affected();
            for ( unsigned int i(0); i < shocks_n.size() ; ++i)
            {
                
                shock_map[shocks_n[i]->id()]=shocks_n[i];
            }
            
        }

        if ( target_cnode->get_composite_degree() < 3 )
        {
            
            vcl_vector<dbskfg_shock_link*> shocks_n = 
                target_cnode->shocks_affected();
            for ( unsigned int i(0); i < shocks_n.size() ; ++i)
            {
                
                shock_map[shocks_n[i]->id()]=shocks_n[i];
            }
            
        }


        vcl_vector<dbskfg_shock_link*> shocks_l = clink->shocks_affected();
        for ( unsigned int i(0); i < shocks_l.size() ; ++i)
        {
            
            shock_map[shocks_l[i]->id()]=shocks_l[i];
        }

        loop_segment_map[link->id()]=link;
        visited_nodes[source_cnode->id()]="temp";
        visited_nodes[target_cnode->id()]="temp";
        contour_node_map[source_cnode->id()]="temp";
        contour_node_map[target_cnode->id()]="temp";
    
    }


    // Populate shock links affected
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator kit;
    for ( kit = shock_map.begin() ; kit != shock_map.end() ; ++kit)
    {   
        transform->shock_links_affected_.push_back((*kit).second);
    }

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;

    //***************** Determine Contours Affected **************************
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {

        dbskfg_shock_link* shock_link = (*it).second;
        
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::LL )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                lclinks = shock_link->left_contour_links();
            vcl_vector<dbskfg_composite_link_sptr>
                rclinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<lclinks.size() ; ++i)
            {
                if ( contour_map.count(lclinks[i]->id())==0 &&
                     !loop_segment_map.count(lclinks[i]->id()))
                {
                    
                    transform->contours_affected_.push_back(lclinks[i]);    
                    contour_map[lclinks[i]->id()]="temp";
                    contour_node_map[lclinks[i]->source()->id()]="temp";
                    contour_node_map[lclinks[i]->target()->id()]="temp";
                }

            }                                

            for ( unsigned int i=0; i<rclinks.size() ; ++i)
            {
                if ( contour_map.count(rclinks[i]->id())==0 &&
                     !loop_segment_map.count(rclinks[i]->id()))
                {
                    transform->contours_affected_.push_back(rclinks[i]);    
                    contour_map[rclinks[i]->id()]="temp";
                    contour_node_map[rclinks[i]->source()->id()]="temp";
                    contour_node_map[rclinks[i]->target()->id()]="temp";
           
                }
            }
                
        }

        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::RLLP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 &&
                     !loop_segment_map.count(clinks[i]->id()))
                {
                    transform->contours_affected_.push_back(clinks[i]);    
                    contour_map[clinks[i]->id()]="temp";
                    contour_node_map[clinks[i]->source()->id()]="temp";
                    contour_node_map[clinks[i]->target()->id()]="temp";
           
                }
            }                                

        }
    
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::LLRP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->left_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 &&
                     visited_nodes.count(clinks[i]->source()->id()) == 0)
                {
                    transform->contours_affected_.push_back(clinks[i]);   
                    contour_map[clinks[i]->id()]="temp";
                    contour_node_map[clinks[i]->source()->id()]="temp";
                    contour_node_map[clinks[i]->target()->id()]="temp";
           
                }         
            }                                

        }

    }

    //***************** Determine Shock Nodes Affected ************************

    // Now lets figure out shocks nodes affected
    // There will be two types of shock nodes affected, those within
    // the local context and those on the border of the local context
    // Those on the outside will not have both source and target within
    // the merged local context
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        inner_shock_nodes;
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit1;

    for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; ++snit1)
    {
        // Grab current shock link
        dbskfg_shock_link* shock_link = (*snit1).second;

        vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit2;
        vcl_map<unsigned int,unsigned int> local_map;

        for ( snit2 = shock_map.begin() ; snit2 != shock_map.end() ; ++snit2)
        {
            dbskfg_composite_node_sptr next_node=
                shock_link->shared_vertex((*(*snit2).second));

            if ( next_node && ((*snit2).second->id() != shock_link->id()) )
            {

                if ( inner_shock_nodes.count(next_node->id())==0 )
                {
                    // Add in shock node
                    inner_shock_nodes[next_node->id()].first=1;
                    inner_shock_nodes[next_node->id()].second=next_node;
                    local_map[next_node->id()]=1;
                }
                else
                {
                    if ( local_map.count(next_node->id())==0)
                    {
                        inner_shock_nodes[next_node->id()].first++;
                    }
                    local_map[next_node->id()]=1;
                }
            }
            
        }

    }

    // Now we need to filter out shock nodes that do not equal composite 
    // degree
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        ::iterator bit;
    
    for ( bit = inner_shock_nodes.begin() ; bit != inner_shock_nodes.end() 
              ; ++bit)
    {
        if ( (*bit).second.first >= 
             (*bit).second.second->get_composite_degree() )
        {

            transform->shock_nodes_affected_.push_back((*bit).second.second);
        }

    }

    // Outer shock nodes will those that are not in the set of inner shock 
    // nodes
    for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; ++snit1)
    {
        if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->shock_nodes_affected_,
                 (*snit1).second->source()) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->outer_shock_nodes_,
                 (*snit1).second->source()))
        {
            if ( (*snit1).second->source()->node_type() == 
                 dbskfg_composite_node::SHOCK_NODE )
            {
                transform->outer_shock_nodes_.push_back(
                    (*snit1).second->source());
            }
        }

        if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->shock_nodes_affected_,
                 (*snit1).second->target()) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->outer_shock_nodes_,
                 (*snit1).second->target()))
        {
            if ( (*snit1).second->target()->node_type() == 
                 dbskfg_composite_node::SHOCK_NODE )
            {
                transform->outer_shock_nodes_.push_back(
                    (*snit1).second->target());
            }
        }

    }

    //********************** Add extra contour nodes *********************
    for ( unsigned int k=0; k < transform->shock_links_affected_.size();
          ++k)
    {

        dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>(
            &(*transform->shock_links_affected_[k]));
        if ( shock_link->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            if ( contour_node_map.count(shock_link->get_left_point()->id()) 
                 == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            if ( contour_node_map.count(shock_link->get_right_point()->id()) 
                 == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::PP )
        {
            if ( contour_node_map.count(
                     shock_link->get_right_point()->id()) == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }

            if ( contour_node_map.count(
                     shock_link->get_left_point()->id()) == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }
    }

    //***************** Compute Polygon ************************
    //Loop over all shocks affected and compose polygon
    if ( shock_map.size() )
    {
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator pit;
        pit=shock_map.begin();

        // Lets start with the first polygon for filling up local context
        vgl_polygon<double> start_poly = (*pit).second->polygon();

        // Advance iterator
        ++pit;

        for ( ; pit != shock_map.end() ; ++pit)
        {
            //Take temp
            vgl_polygon<double> temp(1);
            temp  = (*pit).second->polygon();

            //Keep a flag for status
            int value;

            //Take union of two polygons
            start_poly = vgl_clip(start_poly,             // p1
                                  temp,                   // p2
                                  vgl_clip_type_union,    // p1 U p2
                                  &value);                // test if success

            if ( value == 0 )
            {
                return;
            }
        

    
        
        }

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
    
        // Test if in largest area
        bool flag=false;
        for ( unsigned int k=0; k < transform->contours_to_remove_.size() ; ++k)
        {
            vgl_polygon<double> tempy(start_poly[index]);
            if ( !tempy.contains(transform->contours_to_remove_[k]
                                 ->pt().x(),
                                 transform->contours_to_remove_[k]
                                 ->pt().y()))
            {
                flag=true;
                break;
            }
        }

        if ( flag)
        {
            for (unsigned int s = 0; s < start_poly.num_sheets(); ++s)
            { 
                transform->poly_.push_back(start_poly[s]);
            }
       
        }
        else
        {
            transform->poly_.push_back(start_poly[index]);
        }
    }
   
}

void dbskfg_loop_transforms::
expand_link(dbskfg_composite_link_sptr link,
            vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
            vcl_map<unsigned int,vcl_string>& visited_links)
{


    // Create new transform object
    dbskfg_transform_descriptor_sptr transform = new
        dbskfg_transform_descriptor();

    // Set type of transform
    transform->transform_type_ = dbskfg_transform_descriptor::LOOP;

    // Create two maps to hold everything
    // Holds all shocks affected
    vcl_map<unsigned int, dbskfg_shock_link*> shock_map;
    vcl_map<unsigned int, vcl_string> visited_nodes;
    vcl_map<unsigned int, vcl_string> contour_map;
    vcl_map<unsigned int, vcl_string> contour_node_map;

    // Push back source and target to start of stack
    // Keep a stack for everything
    vcl_vector<dbskfg_composite_node_sptr> stack;

    stack.push_back(link->source());
    stack.push_back(link->target());

    if ( link->source()->get_composite_degree() == 1)
    {

        transform->loop_endpoints_.push_back(link->source());
    }

    if ( link->target()->get_composite_degree() == 1)
    {

        transform->loop_endpoints_.push_back(link->target());
    }


    visited_nodes[link->source()->id()]="temp";
    visited_nodes[link->target()->id()]="temp";

    while(!stack.empty())
    {

        // Pop of stack
        dbskfg_composite_node_sptr node = stack.back();

        if ( node->get_composite_degree() == 1 || 
             node->get_composite_degree() >= 3 )
        {
            if ( transform->loop_endpoints_.size() < 2 )
            {
                if ( transform->loop_endpoints_.size() == 0 )
                {
                    transform->loop_endpoints_.push_back(node);
                }
                else
                {
                    if ( transform->loop_endpoints_[0]->id() != node->id())
                    {
                        transform->loop_endpoints_.push_back(node);
                    }
                }

            }
        }

        // Add this node to transform list
        transform->contours_to_remove_.push_back(node);
        if ( node->get_composite_degree() >= 3)
        {
            transform->t_type_transform_=true;
        }

        contour_node_map[node->id()]="temp";

        // pop of stack
        stack.pop_back();
        
        // Expand this node
        if ( node->get_composite_degree() < 3 )
        {
            expand_node(node,stack,visited_nodes,visited_links,shock_map);
        }
       
    }

    // See if loop endpoints equals size one, can allow this to happen
    if ( transform->loop_endpoints_.size() == 1 )
    {
        transform->loop_endpoints_.clear();
    }

    // Populate shock links affected
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {   
        transform->shock_links_affected_.push_back((*it).second);
    }

    //***************** Determine Contour Links to Remove *********************
    vcl_map<unsigned int,dbskfg_composite_link_sptr> loop_segment_map;

    for ( unsigned int d=0; d < transform->
              contours_to_remove_.size(); ++d)
        
    {
        dbskfg_composite_node_sptr node =
            transform->contours_to_remove_[d];
        
        if ( node->get_composite_degree() < 3 )
        {

            dbskfg_composite_node::edge_iterator in;
            dbskfg_composite_node::edge_iterator out;
        
            // Look at in edges first
            for ( in = node->in_edges_begin() ; 
                  in != node->in_edges_end() 
                      ; ++in)
            {
                dbskfg_composite_link_sptr link = *in;

                if ( link->link_type() == 
                     dbskfg_composite_link::CONTOUR_LINK)
                {
                    loop_segment_map[link->id()]=link;
                }
            }

            // Look at out edges first
            for ( out = node->out_edges_begin() ; 
                  out != node->out_edges_end() 
                      ; ++out)
            {
                dbskfg_composite_link_sptr link = *out;

                if ( link->link_type() == 
                     dbskfg_composite_link::CONTOUR_LINK)
                {
                    loop_segment_map[link->id()]=link;
                }
            }
        }
    }

    // This means that it is a one link of just degree three
    if ( loop_segment_map.size() == 0 && 
         transform->contours_to_remove_.size() 
         == 2 )
    {
        dbskfg_composite_node_sptr node =
            transform->contours_to_remove_[0];
        
        dbskfg_composite_node::edge_iterator in;
        dbskfg_composite_node::edge_iterator out;
        
        // Look at in edges first
        for ( in = node->in_edges_begin() ; 
              in != node->in_edges_end() 
                  ; ++in)
        {
            dbskfg_composite_link_sptr link = *in;

            if ( link->link_type() == 
                 dbskfg_composite_link::CONTOUR_LINK )
            {
                dbskfg_composite_node_sptr opposite =
                    link->opposite(node);
                if ( opposite->id() == transform->contours_to_remove_[1]->id()
                    )
                {
                    loop_segment_map[link->id()]=link;
                }
            }
        }

        // Look at out edges first
        for ( out = node->out_edges_begin() ; 
              out != node->out_edges_end() 
                  ; ++out)
        {
            dbskfg_composite_link_sptr link = *out;

            if ( link->link_type() == 
                 dbskfg_composite_link::CONTOUR_LINK )
            {
                dbskfg_composite_node_sptr opposite =
                    link->opposite(node);
                if ( opposite->id() == transform->contours_to_remove_[1]->id()
                    )
                {
                    loop_segment_map[link->id()]=link;
                }
            }
        }
        
        

    }

    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator sit;
    for ( sit = loop_segment_map.begin() ; sit != loop_segment_map.end() ; 
          ++sit)
    {
        transform->contour_links_to_remove_.push_back((*sit).second);
        
    }

    //***************** Determine Contours Affected **************************
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {

        dbskfg_shock_link* shock_link = (*it).second;
        
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::LL )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                lclinks = shock_link->left_contour_links();
            vcl_vector<dbskfg_composite_link_sptr>
                rclinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<lclinks.size() ; ++i)
            {
                if ( contour_map.count(lclinks[i]->id())==0 &&
                     !loop_segment_map.count(lclinks[i]->id()))
                {
                    
                    transform->contours_affected_.push_back(lclinks[i]);    
                    contour_map[lclinks[i]->id()]="temp";
                    contour_node_map[lclinks[i]->source()->id()]="temp";
                    contour_node_map[lclinks[i]->target()->id()]="temp";
                }

            }                                

            for ( unsigned int i=0; i<rclinks.size() ; ++i)
            {
                if ( contour_map.count(rclinks[i]->id())==0 &&
                     !loop_segment_map.count(rclinks[i]->id()))
                {
                    transform->contours_affected_.push_back(rclinks[i]);    
                    contour_map[rclinks[i]->id()]="temp";
                    contour_node_map[rclinks[i]->source()->id()]="temp";
                    contour_node_map[rclinks[i]->target()->id()]="temp";
           
                }
            }
                
        }

        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::RLLP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 &&
                     !loop_segment_map.count(clinks[i]->id()))
                {
                    transform->contours_affected_.push_back(clinks[i]);    
                    contour_map[clinks[i]->id()]="temp";
                    contour_node_map[clinks[i]->source()->id()]="temp";
                    contour_node_map[clinks[i]->target()->id()]="temp";
           
                }
            }                                

        }
    
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::LLRP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->left_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 &&
                     visited_nodes.count(clinks[i]->source()->id()) == 0)
                {
                    transform->contours_affected_.push_back(clinks[i]);   
                    contour_map[clinks[i]->id()]="temp";
                    contour_node_map[clinks[i]->source()->id()]="temp";
                    contour_node_map[clinks[i]->target()->id()]="temp";
           
                }         
            }                                

        }

    }

    //***************** Determine Shock Nodes Affected ************************

    // Now lets figure out shocks nodes affected
    // There will be two types of shock nodes affected, those within
    // the local context and those on the border of the local context
    // Those on the outside will not have both source and target within
    // the merged local context
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        inner_shock_nodes;
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit1;

    for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; ++snit1)
    {
        // Grab current shock link
        dbskfg_shock_link* shock_link = (*snit1).second;

        vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit2;
        vcl_map<unsigned int,unsigned int> local_map;

        for ( snit2 = shock_map.begin() ; snit2 != shock_map.end() ; ++snit2)
        {
            dbskfg_composite_node_sptr next_node=
                shock_link->shared_vertex((*(*snit2).second));

            if ( next_node && ((*snit2).second->id() != shock_link->id()) )
            {

                if ( inner_shock_nodes.count(next_node->id())==0 )
                {
                    // Add in shock node
                    inner_shock_nodes[next_node->id()].first=1;
                    inner_shock_nodes[next_node->id()].second=next_node;
                    local_map[next_node->id()]=1;
                }
                else
                {
                    if ( local_map.count(next_node->id())==0)
                    {
                        inner_shock_nodes[next_node->id()].first++;
                    }
                    local_map[next_node->id()]=1;
                }
            }
            
        }

    }

    // Now we need to filter out shock nodes that do not equal composite 
    // degree
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        ::iterator bit;
    
    for ( bit = inner_shock_nodes.begin() ; bit != inner_shock_nodes.end() 
              ; ++bit)
    {
        if ( (*bit).second.first >= 
             (*bit).second.second->get_composite_degree() )
        {

            transform->shock_nodes_affected_.push_back((*bit).second.second);
        }

    }

    // Outer shock nodes will those that are not in the set of inner shock 
    // nodes
    for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; ++snit1)
    {
        if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->shock_nodes_affected_,
                 (*snit1).second->source()) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->outer_shock_nodes_,
                 (*snit1).second->source()))
        {
            if ( (*snit1).second->source()->node_type() == 
                 dbskfg_composite_node::SHOCK_NODE )
            {
                transform->outer_shock_nodes_.push_back(
                    (*snit1).second->source());
            }
        }

        if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->shock_nodes_affected_,
                 (*snit1).second->target()) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 transform->outer_shock_nodes_,
                 (*snit1).second->target()))
        {
            if ( (*snit1).second->target()->node_type() == 
                 dbskfg_composite_node::SHOCK_NODE )
            {
                transform->outer_shock_nodes_.push_back(
                    (*snit1).second->target());
            }
        }

    }

    //********************** Add extra contour nodes *********************
    for ( unsigned int k=0; k < transform->shock_links_affected_.size();
          ++k)
    {

        dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>(
            &(*transform->shock_links_affected_[k]));
        if ( shock_link->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            if ( contour_node_map.count(shock_link->get_left_point()->id()) 
                 == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            if ( contour_node_map.count(shock_link->get_right_point()->id()) 
                 == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::PP )
        {
            if ( contour_node_map.count(
                     shock_link->get_right_point()->id()) == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }

            if ( contour_node_map.count(
                     shock_link->get_left_point()->id()) == 0 )
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transform->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transform->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }
    }

    //***************** Compute Polygon ************************
    //Loop over all shocks affected and compose polygon
    if ( shock_map.size() )
    {
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator pit;
        pit=shock_map.begin();

        // Lets start with the first polygon for filling up local context
        vgl_polygon<double> start_poly = (*pit).second->polygon();

        // Advance iterator
        ++pit;

        for ( ; pit != shock_map.end() ; ++pit)
        {
            //Take temp
            vgl_polygon<double> temp(1);
            temp  = (*pit).second->polygon();

            //Keep a flag for status
            int value;

            //Take union of two polygons
            start_poly = vgl_clip(start_poly,             // p1
                                  temp,                   // p2
                                  vgl_clip_type_union,    // p1 U p2
                                  &value);                // test if success

            if ( value == 0 )
            {
                return;
            }
        

    
        
        }

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
    
        // Test if in largest area
        bool flag=false;
        for ( unsigned int k=0; k < transform->contours_to_remove_.size() ; ++k)
        {
            vgl_polygon<double> tempy(start_poly[index]);
            if ( !tempy.contains(transform->contours_to_remove_[k]
                                ->pt().x(),
                                transform->contours_to_remove_[k]
                                ->pt().y()))
            {
                flag=true;
                break;
            }
        }

        if ( flag)
        {
             for (unsigned int s = 0; s < start_poly.num_sheets(); ++s)
             { 
                 transform->poly_.push_back(start_poly[s]);
             }
       
        }
        else
        {
            transform->poly_.push_back(start_poly[index]);
        }
    }
   
    transform->remove_extra_shocks();
   
    vgl_polygon<double> poly1(1);
    vgl_polygon<double> poly2(1);
    bool flag = determine_contour_cost(transform,poly1,poly2);

    // if ( flag )
    // {
    //     double app_cost = determine_app_cost(poly1,poly2);
    //     transform->app_cost_=app_cost;
    // }
    // else
    // {
    //     transform->app_cost_=0.0;
    // }
    objects.push_back(transform);




}

bool dbskfg_loop_transforms::determine_contour_cost
(dbskfg_transform_descriptor_sptr& transform,
 vgl_polygon<double>& poly1,vgl_polygon<double>& poly2)
{
    // Grab all contours to remove
    
    // Find degree one node
    unsigned int i=0;
    bool flag=false;
    for ( ; i < transform->contours_to_remove_.size() ; ++i)
    {
        if ( transform->contours_to_remove_[i]->get_composite_degree() == 1 )
        {
            flag=true;
            break;
        }
    }


    // keep a bsol curve
    bsol_intrinsic_curve_2d contour;
    vcl_vector<vgl_point_2d<double> > contour_points;
    double contour_length(0.0);

    // Keep a vector of points
    if ( flag)
    {
        dbskfg_composite_node_sptr endpoint = 
            transform->contours_to_remove_[i];
        dbskfg_composite_link_sptr link = dbskfg_utilities::
            first_adj_contour(endpoint);
        dbskfg_composite_node_sptr node = link->opposite(endpoint); 
        contour.add_vertex(endpoint->pt().x(),endpoint->pt().y());
        contour_points.push_back(endpoint->pt());
        contour.add_vertex(node->pt().x(),node->pt().y());
        contour_points.push_back(node->pt());

        while( node->get_composite_degree() != 1 )
        {
            link = dbskfg_utilities::cyclic_adj_contour_succ(link,
                                                             node);
            node = link->opposite(node);
            contour.add_vertex(node->pt().x(),node->pt().y());
            contour_points.push_back(node->pt());
        }
        
        
        // Determine contrast polygons
        contour.computeProperties();

        contour_length = contour.length();
    }
    else
    {
        // We have a closed curve

        dbskfg_composite_node_sptr node = 
            transform->contours_to_remove_[0];
        dbskfg_composite_link_sptr link = dbskfg_utilities::
            first_adj_contour(node);

        unsigned int start_id = node->id();

        do
        {
            contour.add_vertex(node->pt().x(),node->pt().y());
            contour_points.push_back(node->pt());
            
            link = dbskfg_utilities::cyclic_adj_contour_succ(link,
                                                             node);
         
            vgl_point_2d<double> prev_point=node->pt();
            node = link->opposite(node);
            vgl_point_2d<double> next_point=node->pt();
            
            contour_length += vgl_distance(prev_point,next_point);

        }while(start_id != node->id());
      
    }

    // Keep track of longest contour
    transform->contour_cost_ = contour_length;
    
    if ( contour_length > longest_contour_ )
    {
        longest_contour_=contour_length;
    }

    return flag;

    if ( flag==false)
    {
        // We have a closed curve
        return flag;
    }

    vgl_point_2d<double> ep1(contour.vertex(0)->x(),contour.vertex(0)->y());
    vgl_point_2d<double> ep2(contour.vertex(contour.size()-1)->x(),
                             contour.vertex(contour.size()-1)->y());
    
    // Find closest points to polygon
    vgl_point_2d<double> p1 = vgl_closest_point(transform->poly_,
                                                ep1);
    vcl_stringstream p1_stream;
    p1_stream<<p1;

    // Find closest points to polygon
    vgl_point_2d<double> p2 = vgl_closest_point(transform->poly_,
                                                ep2);
    vcl_stringstream p2_stream;
    p2_stream<<p2;

    // Determine index for p1 and p2
    unsigned index_p1,index_p2;

    for ( unsigned int b=0; b <= transform->poly_[0].size() ; ++b)
    {
        vgl_point_2d<double> c0(transform->poly_[0][b].x(),
                                transform->poly_[0][b].y());        
        vcl_stringstream temp;
        temp<<c0;
        
        if ( c0 == p1 || p1_stream.str() == temp.str())
        {
            index_p1=b;
        }

        if ( c0 == p2 || p2_stream.str() == temp.str())
        {
            index_p2=b;
        }

    }

    unsigned int start=vcl_min(index_p1,index_p2);
    unsigned int stop =vcl_max(index_p1,index_p2);

    // Means the start of the curve is closer 
    if ( index_p1 < index_p2 )
    {
        vcl_vector<vgl_point_2d<double> >::reverse_iterator it;
        for ( it = contour_points.rbegin() ; 
              it != contour_points.rend() ; ++it)
        {
            poly1.push_back(*it);

        }

        for ( unsigned int b=start; b <= stop ; ++b)
        {
            vgl_point_2d<double> c0(transform->poly_[0][b].x(),
                                    transform->poly_[0][b].y());        
            poly1.push_back(c0);
        }

    }
    else
    {
        vcl_vector<vgl_point_2d<double> >::iterator it;
        for ( it = contour_points.begin() ; 
              it != contour_points.end() ; ++it)
        {
            poly1.push_back(*it);

        }

        for ( unsigned int b=start; b <= stop ; ++b)
        {
            vgl_point_2d<double> c0(transform->poly_[0][b].x(),
                                    transform->poly_[0][b].y());        
            poly1.push_back(c0);
        }

    }


    // Means the start of the curve is closer 
    if ( index_p1 < index_p2 )
    {
        vcl_vector<vgl_point_2d<double> >::reverse_iterator it;
        for ( it = contour_points.rbegin() ; 
              it != contour_points.rend() ; ++it)
        {
            poly2.push_back(*it);

        }

        
        for ( int b=start; b != stop-1 ; --b)
        {
            
            if ( b == -1 )
            {
                b = transform->poly_[0].size()-1;
            }
            vgl_point_2d<double> c0(transform->poly_[0][b].x(),
                                    transform->poly_[0][b].y());        
            poly2.push_back(c0);

            if ( b == stop-1 )
            {
                break;
            }
    
        }


    }
    else
    {
        vcl_vector<vgl_point_2d<double> >::iterator it;
        for ( it = contour_points.begin() ; 
              it != contour_points.end() ; ++it)
        {
            poly2.push_back(*it);

        }
        
        for ( int b=start; b != stop-1 ; --b)
        {
           
            if ( b == -1 )
            {
                b = transform->poly_[0].size()-1;
            }
        
            vgl_point_2d<double> c0(transform->poly_[0][b].x(),
                                    transform->poly_[0][b].y());        
            poly2.push_back(c0);
           
            if ( b == stop-1 )
            {
                break;
            }
        }

    }

    return flag;
}


double dbskfg_loop_transforms::determine_app_cost(
    vgl_polygon<double>& poly1,
    vgl_polygon<double>& poly2)
{

    // Create a vector of all the points in the scanline
    vcl_vector<vgl_point_2d<double> > poly1_points;
    vcl_vector<vgl_point_2d<double> > poly2_points;

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(poly1, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            vgl_point_2d<double> c0(x,y);
            poly1_points.push_back(c0);
        }

    }

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi2(poly2, false);  
    for (psi2.reset(); psi2.next(); ) 
    {
        int y = psi2.scany();
        for (int x = psi2.startx(); x <= psi2.endx(); ++x) 
        {
            vgl_point_2d<double> c0(x,y);
            poly2_points.push_back(c0);
     
        }
     
    }

    if ( image_ )
    {
        vil_image_view<vxl_byte> I = image_->get_view();
     
        if ( I.nplanes() == 3 )
        {
            
            //create two histograms using bilinearly interpolated image values
            vnl_vector_fixed<double, 3> plus_mean((double)0.0f), 
                minus_mean((double)0.0f);

            for ( unsigned int v = 0 ; v < poly1_points.size() ; ++v)
            {
                vnl_vector_fixed<double, 3> v1;
                v1[0] = vil_bilin_interp_safe(
                    L_, poly1_points[v].x(), poly1_points[v].y());
                v1[1] = vil_bilin_interp_safe(
                    A_, poly1_points[v].x(), poly1_points[v].y());
                v1[2] = vil_bilin_interp_safe(
                    B_, poly1_points[v].x(), poly1_points[v].y());
                plus_mean += v1;

            }
  
            for ( unsigned int v = 0 ; v < poly2_points.size() ; ++v)
            {
                vnl_vector_fixed<double, 3> v1;
                v1[0] = vil_bilin_interp_safe(
                    L_, poly2_points[v].x(), poly2_points[v].y());
                v1[1] = vil_bilin_interp_safe(
                    A_, poly2_points[v].x(), poly2_points[v].y());
                v1[2] = vil_bilin_interp_safe(
                    B_, poly2_points[v].x(), poly2_points[v].y());
                minus_mean += v1;

            }
    
            plus_mean  /= poly1_points.size();
            minus_mean /= poly2_points.size();

            return distance_LAB(plus_mean,minus_mean,color_gamma_);


        }
        else
        {
            // gray scale image
            double plus_mean = 0.0, minus_mean = 0.0;

            for(unsigned i = 0; i < poly1_points.size(); ++i)
            {
                double v = vil_bilin_interp_safe(I, 
                                                 poly1_points[i].x(), 
                                                 poly1_points[i].y());
                plus_mean += v;
            }
            for(unsigned i = 0; i < poly2_points.size(); ++i)
            {
                double v = vil_bilin_interp_safe(I, 
                                                 poly2_points[i].x(), 
                                                 poly2_points[i].y());
                minus_mean += v;
            }
            plus_mean /= poly1_points.size();
            minus_mean /= poly2_points.size();
 
            return distance_intensity(plus_mean, minus_mean, color_gamma_);
        }                     
    }

    return 0.0;

}

void dbskfg_loop_transforms::expand_node(
    dbskfg_composite_node_sptr node,
    vcl_vector<dbskfg_composite_node_sptr>& stack,
    vcl_map<unsigned int,vcl_string>& visited_nodes,
    vcl_map<unsigned int,vcl_string>& visited_links,
    vcl_map<unsigned int,dbskfg_shock_link*>& shock_map)
{

    // Grab all shocks affected for this node
    dbskfg_contour_node* cnode = dynamic_cast<dbskfg_contour_node*>(&(*node));

    vcl_vector<dbskfg_shock_link*> shocks = cnode->shocks_affected();
    for ( unsigned int i(0); i < shocks.size() ; ++i)
    {

        shock_map[shocks[i]->id()]=shocks[i];
    }

    dbskfg_composite_node::edge_iterator in;
    dbskfg_composite_node::edge_iterator out;

    // Look at in edges first
    for ( in = node->in_edges_begin() ; in != node->in_edges_end() 
              ; ++in)
    {
        dbskfg_composite_link_sptr link = *in;

        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK)
        {
            // Grab all shocks affected for this node
            dbskfg_contour_node* cnode = 
                dynamic_cast<dbskfg_contour_node*>(&(*(link->opposite(node))));
           
            if ( cnode->get_composite_degree() < 3 )
            {
                vcl_vector<dbskfg_shock_link*> shocks_n = 
                    cnode->shocks_affected();
                for ( unsigned int i(0); i < shocks_n.size() ; ++i)
                {

                    shock_map[shocks_n[i]->id()]=shocks_n[i];
                }

            }

            if ( visited_nodes.count(cnode->id()) == 0 )
            {
                visited_nodes[cnode->id()]="temp";
                stack.push_back(cnode);
            }

            // Grab all shocks affected for this node
            dbskfg_contour_link* clink = 
                dynamic_cast<dbskfg_contour_link*>(&(*link));

            vcl_vector<dbskfg_shock_link*> shocks_l = clink->shocks_affected();
            for ( unsigned int i(0); i < shocks_l.size() ; ++i)
            {

                shock_map[shocks_l[i]->id()]=shocks_l[i];
            }

            visited_links[link->id()]="temp";
        }


    }

    // Look at out edges first
    for ( out = node->out_edges_begin() ; out != node->out_edges_end() 
              ; ++out)
    {

        dbskfg_composite_link_sptr link = *out;

        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK)
        {
            // Grab all shocks affected for this node
            dbskfg_contour_node* cnode = 
                dynamic_cast<dbskfg_contour_node*>(&(*(link->opposite(node))));
            
            if ( cnode->get_composite_degree() < 3 )
            {

                vcl_vector<dbskfg_shock_link*> shocks_n = 
                    cnode->shocks_affected();
                for ( unsigned int i(0); i < shocks_n.size() ; ++i)
                {

                    shock_map[shocks_n[i]->id()]=shocks_n[i];
                }
             
            }

            if ( visited_nodes.count(cnode->id()) == 0 )
            {
                visited_nodes[cnode->id()]="temp";
                stack.push_back(cnode);
            }

            // Grab all shocks affected for this node
            dbskfg_contour_link* clink = 
                dynamic_cast<dbskfg_contour_link*>(&(*link));

            vcl_vector<dbskfg_shock_link*> shocks_l = clink->shocks_affected();
            for ( unsigned int i(0); i < shocks_l.size() ; ++i)
            {

                shock_map[shocks_l[i]->id()]=shocks_l[i];
            }

            visited_links[link->id()]="temp";
        }



    }

}
