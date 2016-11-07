// This is brcv/shp/dbskfg/dbskfg_utilities.h

//:
// \file

// dbskfg headers
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_shock_node.h>
// dbsk2d headers
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_ishock_belm.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_bnd_edge.h>
#include <dbsk2d/dbsk2d_geometry_utils.h>
// vtol headers
#include <vtol/vtol_topology_object.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polygon_2d.h>
// vgl headers
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_area.h>
// vcl headers
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
// vil headers
#include <vil/vil_plane.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
// bsol headers
#include <bsol/bsol_algs.h>
// vnl headers
#include <vnl/vnl_vector_fixed.h>

#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>

#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/pro/dbskfg_form_composite_graph_process.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>

#include <vgl/vgl_closest_point.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_vector.h>

// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>

bool dbskfg_utilities::is_a_source(dbskfg_composite_node_sptr shock_node)
{
    // Keep all links in a vector
    vcl_vector<dbskfg_composite_link_sptr> shocks;

    // Grab all shock links
    dbskfg_composite_link_sptr link = first_adj_shock(shock_node);
    shocks.push_back(link);

    // Grab all the rest of shock links
    for ( unsigned int i=1 ; i < shock_node->get_composite_degree() ; ++i)
    {
        link=cyclic_adj_shock_succ(link,shock_node);
        shocks.push_back(link);
            
    }

    unsigned int degree=0;

    //Loop over all links
    for ( unsigned int k=0; k < shocks.size() ;  ++k )
    {
        if ( shocks[k]->source()->id() == shock_node->id())
        {
            degree++;
        }
    }

    return (degree == shock_node->get_composite_degree() );

}

bool dbskfg_utilities::is_a_sink(dbskfg_composite_node_sptr shock_node)
{
    // Keep all links in a vector
    vcl_vector<dbskfg_composite_link_sptr> shocks;

    // Grab all shock links
    dbskfg_composite_link_sptr link = first_adj_shock(shock_node);
    shocks.push_back(link);

    // Grab all the rest of shock links
    for ( unsigned int i=1 ; i < shock_node->get_composite_degree() ; ++i)
    {
        link=cyclic_adj_shock_succ(link,shock_node);
        shocks.push_back(link);
            
    }

    unsigned int degree=0;

    //Loop over all links
    for ( unsigned int k=0; k < shocks.size() ;  ++k )
    {
        if ( shocks[k]->target()->id() == shock_node->id())
        {
            degree++;
        }
    }

    return (degree == shock_node->get_composite_degree() );




}

void dbskfg_utilities::bbox(dbskfg_composite_graph_sptr& cgraph,
                            vsol_box_2d& box)
{

    // Take all points and do update boundign box
    for (dbskfg_composite_graph::vertex_iterator vit = 
             cgraph->vertices_begin(); 
         vit != cgraph->vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = *vit;
        box.add_point(node->pt().x(),node->pt().y());
        
    }


}

//: Method
// Search in a set of links for a link
bool dbskfg_utilities::is_link_in_set_of_links(
    vcl_vector<dbskfg_composite_link_sptr>& set,
    dbskfg_composite_link_sptr test_link)
{
    bool flag=false;
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
        if ( test_link->id() == set[i]->id() )
        {
            flag=true;
            break;
        }

    }

    return flag;
}

//: Method
// Search in a set of nodes for a node
bool dbskfg_utilities::is_node_in_set_of_nodes(
    vcl_vector<dbskfg_composite_node_sptr>& set,
    dbskfg_composite_node_sptr test_node,
    CompareType compare)
{
    bool flag=false;
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
        if ( compare == ID)
        {
            if ( test_node->id() == set[i]->id() )
            {
                flag=true;
                break;
            }
        }
        else
        {
            vcl_stringstream test_stream;
            vcl_stringstream set_stream;
            test_stream<<test_node->pt();
            set_stream<<set[i]->pt();

            if ( test_stream.str() == set_stream.str())
            {
                flag=true;
                break;
            }
            
        }

    }

    return flag;
}


//: Method
// Search in a set of nodes for a node
bool dbskfg_utilities::is_node_in_set_of_nodes(
    vcl_vector<dbskfg_composite_node_sptr>& set,
    dbskfg_shock_node* test_node,
    CompareType compare)
{
    bool flag=false;
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
        if ( compare == ID)
        {
            if ( test_node->id() == set[i]->id() )
            {
                flag=true;
                break;
            }
        }
        else
        {
            vcl_stringstream test_stream;
            vcl_stringstream set_stream;
            test_stream<<test_node->pt();
            set_stream<<set[i]->pt();

            if ( test_stream.str() == set_stream.str())
            {
                flag=true;
                break;
            }
            
        }

    }

    return flag;
}


//: Method
// Search in a set of nodes using just a string
bool dbskfg_utilities::is_node_in_set_of_nodes(
    vcl_vector<dbskfg_composite_node_sptr>& set,
    vcl_string point_location)
{
    bool flag=false;
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
       
        vcl_stringstream set_stream;
        set_stream<<set[i]->pt();
        
        if ( point_location == set_stream.str())
        {
            flag=true;
            break;
        }
            
    }

    return flag;
}


//: Method
// Search in a set of nodes for a node
dbskfg_composite_node_sptr dbskfg_utilities::get_node_in_set_of_nodes(
    vcl_vector<dbskfg_composite_node_sptr>& set,
    dbskfg_composite_node_sptr test_node,
    CompareType compare)
{
    vcl_map<double,dbskfg_composite_node_sptr> distance_map;

    dbskfg_composite_node_sptr found_node(0);
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
        if ( compare == ID)
        {
            if ( test_node->id() == set[i]->id() )
            {
                found_node=set[i];
                break;
            }
        }
        else
        {
            vcl_stringstream test_stream;
            vcl_stringstream set_stream;
            test_stream<<test_node->pt();
            set_stream<<set[i]->pt();

            if ( test_stream.str() == set_stream.str() || 
                 vgl_distance(test_node->pt(),set[i]->pt()) < 0.005 )
            {
                distance_map[vgl_distance(test_node->pt(),set[i]->pt())]=
                    set[i];
            }
            
        }

    }

    if ( distance_map.size())
    {
        found_node = (*distance_map.begin()).second;
    }
    return found_node;
}


//: Method
// Search in a set of nodes for a node
dbskfg_composite_node_sptr dbskfg_utilities::get_node_in_set_of_nodes(
    vcl_vector<dbskfg_composite_node_sptr>& set,
    vgl_point_2d<double> test_point)
{

    vcl_map<double,dbskfg_composite_node_sptr> distance_map;

    dbskfg_composite_node_sptr found_node(0);
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
        vcl_stringstream test_stream;
        vcl_stringstream set_stream;
        test_stream<<test_point;
        set_stream<<set[i]->pt();


        if ( test_stream.str() == set_stream.str() || 
             vgl_distance(test_point,set[i]->pt()) < 0.005 )
        {
            distance_map[vgl_distance(test_point,set[i]->pt())]=
                set[i];
        }
        
    }

    if ( distance_map.size())
    {
        found_node = (*distance_map.begin()).second;
    }
    return found_node;
}


//: Method
// Search in a set of links for this node
bool dbskfg_utilities::is_node_in_set_of_links(
    vcl_vector<dbskfg_composite_link_sptr>& set,
    dbskfg_composite_node_sptr test_node)
{
    bool flag=false;
    for ( unsigned int i=0; i < set.size() ; ++i)
    {
     
        if ( test_node->id() == set[i]->source()->id() ||
             test_node->id() == set[i]->target()->id() )
        {   
            flag=true;
            break;
        }

    }

    return flag;
}

 //: Methods
// Find gap connected graph
void dbskfg_utilities::find_gap_connected_graph(
    vcl_map<unsigned int, dbskfg_shock_link*>& shock_map,
    unsigned int common_edge_id)
{
    //Keep a set of visited links
    vcl_map<unsigned int,bool> visited_links;

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {
        visited_links[(*it).first]=false;
    }

    //Use common shock edge between point to build graph
    vcl_vector<unsigned int> stack;
    stack.push_back(common_edge_id);

    while(!stack.empty() )
    {

        // Grap top of stack
        unsigned int id = stack.back();


        // Grab this shock link from map
        dbskfg_composite_link_sptr shock_link=shock_map[id];

        // Erase the element
        stack.pop_back();

        // Search thru map and find all adjancent links to start id
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator sit;
        for ( sit = shock_map.begin() ; sit != shock_map.end() ; ++sit)
        {
            if ( shock_link->id() != (*sit).first)
            {
                dbskfg_composite_node_sptr next_node =
                    shock_link->shared_vertex(*(*sit).second);
                if ( next_node && visited_links[(*sit).first]==false)
                {
                    visited_links[(*sit).first]=true;
                    stack.push_back((*sit).first);

                }
                
            }
        }

        


    }
    
    // Now erase all elements that are not visited
    vcl_map<unsigned int,bool>::iterator vit;
    for ( vit = visited_links.begin() ; vit != visited_links.end() ; ++vit)
    {
        if ( (*vit).second == false)
        {
            shock_map.erase((*vit).first);
        }

    }
}

//: Methods
// From a shock edge determines the original contour id pair
vcl_pair<int,int> 
dbskfg_utilities::get_contour_id_from_shock_edge
(dbsk2d_ishock_edge* elem)
{

    // Define a key
    int leftid  = -1;
    int rightid = -1;

    vcl_pair<int,int> key;

    dbsk2d_ishock_belm *left_belem  = elem->lBElement();
    dbsk2d_ishock_belm *right_belem = elem->rBElement();

    // Check that boundary elements exist
    if ( left_belem==0 || right_belem==0 )
    {
        key.first=leftid;
        key.second=rightid;
        return key;
    }
    
    // --------------------  Process left boundary element

    // Check if it is a line
    if ( left_belem->is_a_line())
    {

        dbsk2d_ishock_bline *line = 
            static_cast<dbsk2d_ishock_bline*>(left_belem);
        const vcl_list<vtol_topology_object*>* contour_list= 
            line->bnd_edge()->superiors_list();
        leftid=contour_list->front()->get_id();

    }
    // If not a line assume it is a point
    else
    {
        // Grab point
        dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>
            (left_belem);

        // Grab container class of point, vertex
        dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

        // Grab vertex superior
        const vcl_list<vtol_topology_object*>* zero_chain_sup_list = 
            vertex->superiors_list();
    
        // Grab zero chain superior
        const vcl_list<vtol_topology_object*>* edge_chain_sup_list = 
            zero_chain_sup_list->front()->superiors_list();

        // Grab edge superior list
        const vcl_list<vtol_topology_object*>* contour_sup_list    = 
            edge_chain_sup_list->front()->superiors_list();

        // Grab id from edge superior, should be contour
        leftid = contour_sup_list->front()->get_id();

       
    }


    // --------------------  Process right boundary element

    // Check if it is a line
    if ( right_belem->is_a_line())
    {
        dbsk2d_ishock_bline *line = static_cast<dbsk2d_ishock_bline*>
            (right_belem);
        const vcl_list<vtol_topology_object*>* contour_list= 
            line->bnd_edge()->superiors_list();
        rightid=contour_list->front()->get_id();

    }
    // If not a line assume it is a point
    else
    {
        // Grab point
        dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>
            (right_belem);

        // Grab container class of point, vertex
        dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

        // Grab vertex superior
        const vcl_list<vtol_topology_object*>* zero_chain_sup_list = 
            vertex->superiors_list();
    
        // Grab zero chain superior
        const vcl_list<vtol_topology_object*>* edge_chain_sup_list = 
            zero_chain_sup_list->front()->superiors_list();

        // Grab edge superior list
        const vcl_list<vtol_topology_object*>* contour_sup_list    = 
            edge_chain_sup_list->front()->superiors_list();

        // Grab id from edge superior, should be contour
        rightid = contour_sup_list->front()->get_id();

    }

    // Create a vector of vcl_pairs
    key.first=leftid;
    key.second=rightid;

    return key;

   
}

//: Method
// Get a set of tangent pairs from contour links
vcl_pair<double,double> dbskfg_utilities::get_tangent_pairs(
    dbskfg_contour_node* lnode,
    dbskfg_contour_node* rnode,
    double& theta1,
    double& theta2)
{

    dbskfg_composite_link_sptr left_contour_link;
    dbskfg_composite_link_sptr right_contour_link;
    
    bool lflag=false;
    bool rflag=false;

    dbskfg_composite_node::edge_iterator srit;

    // Move to left in and out
    for ( srit = lnode->in_edges_begin() ; srit != lnode->in_edges_end() 
              ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            left_contour_link = *srit;
            lflag=true;
            break;
        }  
    }

    if (!lflag)
    {
        for ( srit = lnode->out_edges_begin() ; srit != lnode->out_edges_end() 
                  ; ++srit)
        {
            if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
            {
                left_contour_link = *srit;
                lflag=true;
                break;
            }  
        }
    }

    // Move to right in and out
    for ( srit = rnode->in_edges_begin() ; srit != rnode->in_edges_end() 
              ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            right_contour_link = *srit;
            rflag=true;
            break;
        }  
    }

    if (!rflag)
    {
        for ( srit = rnode->out_edges_begin() ; srit != rnode->out_edges_end() 
                  ; ++srit)
        {
            if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
            {
                right_contour_link = *srit;
                rflag=true;
                break;
            }  
        }
    }

    double ltan,rtan;

    vnl_vector_fixed<double,2> line_seg(rnode->pt().x()-lnode->pt().x(),
                                        rnode->pt().y()-lnode->pt().y());

    line_seg.normalize();


    if ( lnode->id() == left_contour_link->target()->id())
    {
        vgl_line_2d<double> ref_line(left_contour_link->target()->pt(), 
                                     left_contour_link->source()->pt());
        ltan=ref_line.slope_radians();

    }
    else
    {
         vgl_line_2d<double> ref_line(left_contour_link->source()->pt(), 
                                      left_contour_link->target()->pt());
         ltan=ref_line.slope_radians();

    }

    if ( rnode->id() == right_contour_link->target()->id())
    {
        vgl_line_2d<double> ref_line(right_contour_link->source()->pt(), 
                                     right_contour_link->target()->pt());
        rtan=ref_line.slope_radians();

    }
    else
    {
        vgl_line_2d<double> ref_line(right_contour_link->target()->pt(), 
                                      right_contour_link->source()->pt());
        rtan=ref_line.slope_radians();

    }

    vnl_vector_fixed<double,2>  left_seg(cos(ltan),sin(ltan));
    vnl_vector_fixed<double,2>  right_seg(cos(rtan),sin(rtan));

    double cross_product1 = vnl_cross_2d(line_seg,left_seg);
    double cross_product2 = vnl_cross_2d(line_seg,right_seg);
    double dot_product1   = dot_product(line_seg,left_seg);
    double dot_product2   = dot_product(line_seg,right_seg);

    double theta_bar1 = atan2(cross_product1,dot_product1);
    double theta_bar2 = atan2(cross_product2,dot_product2);
    
    theta1=theta_bar1;
    theta2=theta_bar2;

    return vcl_make_pair(ltan,rtan);
}



//Method:
// Used to determine the local context of everything involved in gap type 1
void dbskfg_utilities::gap_1_local_context
(
    dbskfg_contour_node* p1,
    dbskfg_contour_node* p2,
    unsigned int common_edge_id,
    dbskfg_transform_descriptor_sptr transform
)
{

    //Lets make a map and hold all shocks affected and all contours affected
    vcl_map<unsigned int,dbskfg_shock_link*> shock_map;
    vcl_map<unsigned int, vcl_string> contour_map;

    // Loop over both nodes and find all unique shocks affected
    vcl_vector<dbskfg_shock_link*> p1_shocks=p1->shocks_affected();
    vcl_vector<dbskfg_shock_link*> p2_shocks=p2->shocks_affected();

    //***************** Determine Shock Links Affected ***********************

    for ( unsigned int i=0; i < p1_shocks.size() ; ++i)
    {
        
        shock_map[p1_shocks[i]->id()]=p1_shocks[i];
    }

    // Loop over both nodes and find all unique shocks affected
    for ( unsigned int j=0; j < p2_shocks.size() ; ++j)
    {

        shock_map[p2_shocks[j]->id()]=p2_shocks[j];
    }

    // prune out disconnected shocks from graph spawned by two endpoints
    if ( shock_map.size() > 1 )
    {
        find_gap_connected_graph(shock_map,common_edge_id);
    }

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {   
        transform->shock_links_affected_.push_back((*it).second);
    }

    //***************** Determine Contours Affected **************************
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {

        dbskfg_shock_link* shock_link = (*it).second;
        
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::RLLP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 )
                {
                    transform->contours_affected_.push_back(clinks[i]);    
                    contour_map[clinks[i]->id()]="temp";
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
                if ( contour_map.count(clinks[i]->id())==0 )
                {
                    transform->contours_affected_.push_back(clinks[i]);   
                    contour_map[clinks[i]->id()]="temp";
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
        if ( !is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*snit1).second->source()) && 
             !is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                      (*snit1).second->source() ))

        {
            transform->outer_shock_nodes_.push_back((*snit1).second->source());
        }

        if ( !is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*snit1).second->target()) &&
             !is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                      (*snit1).second->target() ))
        {

            transform->outer_shock_nodes_.push_back((*snit1).second->target());
        }

    }

    //***************** Compute Polygon ************************
    //Loop over all shocks affected and compose polygon
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
 
        assert(value==1);
    


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
    
    transform->poly_.push_back(start_poly[index]);

    assert(transform->poly_.num_sheets() == 1 );
    
}

//: Methods 
// determine influence zone between point and line or point and point
// Return point to consider
dbskfg_composite_node_sptr dbskfg_utilities::gap_4_local_context(
    dbskfg_contour_node* p1,
    dbskfg_transform_descriptor_sptr transform,
    vgl_point_2d<double>& other_point)
{

    dbskfg_composite_link_sptr contour_link;
    bool lflag=false;

    dbskfg_composite_node::edge_iterator srit;
    // Move to left in and out
    for ( srit = p1->in_edges_begin() ; srit != p1->in_edges_end() ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            contour_link = *srit;
            lflag=true;
            break;
        }  
    }

    if (!lflag)
    {
        for ( srit = p1->out_edges_begin() ; srit != p1->out_edges_end() 
                  ; ++srit)
        {
            if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
            {
                contour_link = *srit;
                lflag=true;
                break;
            }  
        }
    }

    double angle2=0.0;
  
    double xcoord=0.0;
    double ycoord=0.0;


    // Compute tangent
    if ( p1->id() == contour_link->target()->id())
    {
        xcoord=p1->pt().x()-contour_link->source()->pt().x();
        ycoord=p1->pt().y()-contour_link->source()->pt().y();
        vgl_line_2d<double> ref_line(contour_link->target()->pt(), 
                                     contour_link->source()->pt());
        angle2=ref_line.slope_radians();

    }
    else
    {
        xcoord=p1->pt().x()-contour_link->target()->pt().x();
        ycoord=p1->pt().y()-contour_link->target()->pt().y();
     
        vgl_line_2d<double> ref_line(contour_link->source()->pt(), 
                                     contour_link->target()->pt());
        angle2=ref_line.slope_radians();

    }


    vnl_vector_fixed<double,2> line_seg(xcoord,ycoord);
    line_seg.normalize();

    transform->theta1_ = angle2;
    transform->theta2_ = angle2;

    // Look at shock links affected for contour point

    //Lets make a map and hold all shocks affected and all contours affected
    vcl_map<unsigned int,dbskfg_shock_link*> shock_map;
    vcl_map<unsigned int, vcl_string> contour_map;

    // Loop over both nodes and find all unique shocks affected
    vcl_vector<dbskfg_shock_link*> p1_shocks=p1->shocks_affected();

    // Degree one nodes in local context
    vcl_vector<dbskfg_composite_node_sptr> degree_ones_local_context;

    //***************** Determine Shock Links Affected ***********************

    for ( unsigned int i=0; i < p1_shocks.size() ; ++i)
    {
        
        shock_map[p1_shocks[i]->id()]=p1_shocks[i];
    }

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {   
        transform->shock_links_affected_.push_back((*it).second);
    }

    //***************** Determine Contours Affected **************************
    for ( it = shock_map.begin() ; it != shock_map.end() ; ++it)
    {

        dbskfg_shock_link* shock_link = (*it).second;
        
        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::RLLP )
        {

            vcl_vector<dbskfg_composite_link_sptr>
                clinks = shock_link->right_contour_links();
                        
            for ( unsigned int i=0; i<clinks.size() ; ++i)
            {
                if ( contour_map.count(clinks[i]->id())==0 )
                {
                    transform->contours_affected_.push_back(clinks[i]);    
                    contour_map[clinks[i]->id()]="temp";
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
                if ( contour_map.count(clinks[i]->id())==0 )
                {
                    transform->contours_affected_.push_back(clinks[i]);   
                    contour_map[clinks[i]->id()]="temp";
                }         
            }                                

        }

        if ( shock_link->shock_compute_type() == 
             dbskfg_utilities::PP )
        {

            if ( shock_link->get_left_point()->get_composite_degree()== 1 &&
                 shock_link->get_left_point()->id() != p1->id() )
            {
                // We have found a degree one node that we interact with
                degree_ones_local_context.push_back(shock_link
                                                    ->get_left_point());
            }

            if ( shock_link->get_right_point()->get_composite_degree()== 1 &&
                 shock_link->get_right_point()->id() != p1->id() )
            {
                // We have found a degree one node that we interact with
                degree_ones_local_context.push_back(shock_link
                                                    ->get_right_point());
            }

        }

    }


    vcl_map<double,dbskfg_composite_node_sptr> distances;
    vcl_map<double,vgl_point_2d<double> > angle_distances;

    // Find closes point distance wise
    for ( unsigned int t=0; t < transform->contours_affected_.size() ; ++t)
    {

        if ( transform->contours_affected_[t]->id() != contour_link->id())
        {
            dbskfg_contour_node* source = dynamic_cast<dbskfg_contour_node*>
                (&(*transform->contours_affected_[t]->source()));
            dbskfg_contour_node* target = dynamic_cast<dbskfg_contour_node*>
                (&(*transform->contours_affected_[t]->target()));

            vgl_point_2d<double> source_pt = source->pt();
            vgl_point_2d<double> target_pt = target->pt();

            double xcoord,ycoord;
            vgl_closest_point_to_linesegment
                (xcoord,ycoord,
                 source_pt.x(),source_pt.y(),
                 target_pt.x(),target_pt.y(),
                 p1->pt().x(),p1->pt().y());
        
            vgl_point_2d<double> foot_pt(xcoord,ycoord);
       
            vnl_vector_fixed<double,2> line_seg1(source_pt.x()-p1->pt().x(),
                                                 source_pt.y()-p1->pt().y());
            line_seg1.normalize();
            vnl_vector_fixed<double,2> line_seg2(target_pt.x()-p1->pt().x(),
                                                 target_pt.y()-p1->pt().y());
            line_seg2.normalize();
            vnl_vector_fixed<double,2> line_seg3(foot_pt.x()-p1->pt().x(),
                                                 foot_pt.y()-p1->pt().y());
            line_seg3.normalize();

            double angle_a = angle(line_seg,line_seg1);
            double angle_b = angle(line_seg,line_seg2);
           
            //double angle_c = angle(line_seg,line_seg3);

            angle_distances[angle_a]=source_pt;
            angle_distances[angle_b]=target_pt;

            // angle_distances[angle_c]=foot_pt;
            
            distances[angle_a]=source;
            distances[angle_b]=target;

        }

    }

    if ( degree_ones_local_context.size())
    {
        dbskfg_contour_node* source = dynamic_cast<dbskfg_contour_node*>
            (&(*degree_ones_local_context[0]));
        vgl_point_2d<double> source_pt = source->pt();
        vnl_vector_fixed<double,2> line_seg1(source_pt.x()-p1->pt().x(),
                                             source_pt.y()-p1->pt().y());

        double angle_a = angle(line_seg,line_seg1);
        angle_distances[angle_a]=source_pt;
    }

    other_point = (*angle_distances.begin()).second;

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
                }
                else
                {

                    inner_shock_nodes[next_node->id()].first++;
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
        if ( !is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*snit1).second->source()) && 
             !is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                      (*snit1).second->source() ))

        {
            transform->outer_shock_nodes_.push_back((*snit1).second->source());
        }

        if ( !is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*snit1).second->target()) &&
             !is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                      (*snit1).second->target() ))
        {

            transform->outer_shock_nodes_.push_back((*snit1).second->target());
        }

    }

    //***************** Compute Polygon ************************
    //Loop over all shocks affected and compose polygon
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
 
        assert(value==1);
    


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
    
    transform->poly_.push_back(start_poly[index]);

    assert(transform->poly_.num_sheets() == 1 );
 
    return (*distances.begin()).second;
}

void dbskfg_utilities::save_image_poly(vgl_polygon<double>& vgl_poly,
                                       vil_image_resource_sptr img_sptr,
                                       vcl_string filename)
{
    
    vil_image_resource_sptr img_r = vil_plane(img_sptr, 0);
    vil_image_resource_sptr img_g = vil_plane(img_sptr, 1);
    vil_image_resource_sptr img_b = vil_plane(img_sptr, 2);

    vsol_polygon_2d_sptr vsol_poly = bsol_algs::poly_from_vgl(vgl_poly);
    
    vsol_poly->compute_bounding_box();
    vsol_box_2d_sptr bbox = vsol_poly->get_bounding_box();
    double minx = bbox->get_min_x()-5 < 0 ? 0 : bbox->get_min_x()-5;
    double miny = bbox->get_min_y()-5 < 0 ? 0 : bbox->get_min_y()-5;

    vil_image_view<vil_rgb<vxl_byte> > 
        temp((int)vcl_ceil(bbox->width() + 10), 
             (int)vcl_ceil(bbox->height() + 10), 1); 
    vil_rgb<vxl_byte> bg_col(255, 255, 255);
    temp.fill(bg_col);

    vil_image_view<vxl_byte> img_rv = img_r->get_view();
    vil_image_view<vxl_byte> img_gv = img_g->get_view();
    vil_image_view<vxl_byte> img_bv = img_b->get_view();

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(vgl_poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            if (x < 0 || y < 0)
            {
                continue;
            }
            if (x >= int(img_r->ni()) || y >= int(img_r->nj()))
            { 
                continue;
            }
            int xx = (int)vcl_floor(x - minx + 0.5); 
            int yy = (int)vcl_floor(y - miny + 0.5);
            if (xx < 0 || yy < 0)
            {
                continue;
            }
            if (double(xx) > bbox->width() || double(yy) > bbox->height())
            {
                continue;
            }
            temp(xx,yy) = 
                vil_rgb<vxl_byte>(img_rv(x,y), img_gv(x,y), img_bv(x,y));
        }
    }

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);
    vil_save_image_resource(out_img, 
                            filename.c_str()); 

}


void dbskfg_utilities::save_image_mask(vgl_polygon<double>& vgl_poly,
                                       vil_image_resource_sptr img_sptr,
                                       vcl_string filename,
                                       bool actual)
{
    
    vil_image_resource_sptr img_r = vil_plane(img_sptr, 0);
    vil_image_resource_sptr img_g = vil_plane(img_sptr, 1);
    vil_image_resource_sptr img_b = vil_plane(img_sptr, 2);

    vil_image_view<vil_rgb<vxl_byte> > temp(img_sptr->ni(),img_sptr->nj());
    vil_rgb<vxl_byte> bg_col(255, 255, 255);
    temp.fill(bg_col);

    vil_image_view<vxl_byte> img_rv = img_r->get_view();
    vil_image_view<vxl_byte> img_gv = img_g->get_view();
    vil_image_view<vxl_byte> img_bv = img_b->get_view();

    // do not include boundary
    vgl_polygon_scan_iterator<double> psi(vgl_poly, false);  
    for (psi.reset(); psi.next(); ) 
    {
        int y = psi.scany();
        for (int x = psi.startx(); x <= psi.endx(); ++x) 
        {
            if (x < 0 || y < 0)
            {
                continue;
            }
            if (x >= int(temp.ni()) || y >= int(temp.nj()))
            { 
                continue;
            }
            if ( actual )
            {
                temp(int(x),int(y)) = 
                    vil_rgb<vxl_byte>(img_rv(x,y), img_gv(x,y), img_bv(x,y));
            }
            else
            {
                temp(int(x),int(y)) = vil_rgb<vxl_byte>(0,0,0);
            }
        }
    }

    vil_image_resource_sptr out_img = vil_new_image_resource_of_view(temp);
    vil_save_image_resource(out_img, 
                            filename.c_str()); 

}

bool dbskfg_utilities::comparison( dbskfg_transform_descriptor_sptr transform1,
                                   dbskfg_transform_descriptor_sptr transform2)
{
    return transform1->cost_ < transform2->cost_;
}

vcl_vector<dbskfg_shock_link*> dbskfg_utilities::clockwise(
    dbskfg_composite_node_sptr node)
{

    vcl_vector<dbskfg_shock_link*> clockwise_links;

    dbskfg_composite_node::edge_iterator srit;
    vcl_map<unsigned int, vcl_string> in_edges,out_edges;
    vcl_vector<vcl_pair<dbskfg_composite_node_sptr,dbskfg_shock_link*> > 
        traversal_points;

    // Look at in edges first
    for ( srit  = node->in_edges_begin() ; 
          srit != node->in_edges_end()   ; ++srit)
    {
        dbskfg_composite_link_sptr link = *srit;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* shock_link=
                dynamic_cast<dbskfg_shock_link*>(&(*link));

            if ( shock_link->shock_link_type() == 
                 dbskfg_shock_link::SHOCK_EDGE )
            {
                dbskfg_composite_node_sptr opposite =
                    shock_link->opposite(node);
                traversal_points.push_back(vcl_make_pair(opposite,
                                                         shock_link));
                in_edges[shock_link->id()]="temp";
            }
        }

    }

    // Look at out edges next
    for ( srit  = node->out_edges_begin() ; 
          srit != node->out_edges_end()   ; ++srit)
    {

        dbskfg_composite_link_sptr link = *srit;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* shock_link=
                dynamic_cast<dbskfg_shock_link*>(&(*link));
            
            if ( shock_link->shock_link_type() == 
                 dbskfg_shock_link::SHOCK_EDGE )
            {

                dbskfg_composite_node_sptr opposite =
                    shock_link->opposite(node);
                traversal_points.push_back(vcl_make_pair(opposite,
                                                         shock_link));
                out_edges[shock_link->id()]="temp";
            }
        }

    }

    vcl_vector<vnl_vector_fixed<double,2> > vecs;

    // Look at ordering 
    for ( unsigned int d=1; d < traversal_points.size(); ++d)
    {
        dbskfg_composite_node_sptr v1 =
            traversal_points[d-1].first;
        dbskfg_composite_node_sptr v2 =
            traversal_points[d].first;
        
        vnl_vector_fixed<double,2> dir(v2->pt().x() - 
                                       v1->pt().x(),
                                       v2->pt().y() -
                                       v1->pt().y());

        vecs.push_back(dir);
    }

    bool ccw=false;
    for ( unsigned int k=1 ; k < vecs.size() ; ++k)
    {
        double cross_product = vnl_cross_2d(vecs[1],vecs[0]);

        if ( cross_product > 0  )
        {
            ccw=true;
            break;
        }
        else
        {
         
            return clockwise_links;
        }
    }

    
    if ( ccw )
    {

        vcl_vector<vcl_pair<dbskfg_composite_node_sptr,dbskfg_shock_link*> > 
            ::reverse_iterator rit;

        // Look at ordering 
        rit = traversal_points.rbegin();
        ++rit;

        for ( ; rit != traversal_points.rend(); ++rit)
        {
            clockwise_links.push_back((*rit).second);
        }
        
        clockwise_links.push_back((traversal_points.back()).second);

    }
   
    return clockwise_links;
    
}

bool dbskfg_utilities::point_in_polygon(vgl_polygon<double>& poly,
                                        double x,double y,
                                        unsigned int& sheet)
{
  bool c = false;
  bool set=false;
  for (unsigned int s=0; s < poly.num_sheets(); ++s)
  {
    vgl_polygon<double>::sheet_t const& pgon = poly[s];
    int n = pgon.size();
    for (int i = 0, j = n-1; i < n; j = i++)
    {
      const vgl_point_2d<double> &p_i = pgon[i];
      const vgl_point_2d<double> &p_j = pgon[j];

      // by definition, corner points and edge points are inside the polygon:
      if ((p_j.x() - x) * (p_i.y() - y) == (p_i.x() - x) * (p_j.y() - y) &&
          (((p_i.x()<=x) && (x<=p_j.x())) || ((p_j.x()<=x) && (x<=p_i.x()))) &&
          (((p_i.y()<=y) && (y<=p_j.y())) || ((p_j.y()<=y) && (y<=p_i.y()))))
      {
          sheet = s;
        return true;
      }

      // invert c for each edge crossing:
      if ((((p_i.y()<=y) && (y<p_j.y())) || ((p_j.y()<=y) && (y<p_i.y()))) &&
          (x < (p_j.x() - p_i.x()) * (y - p_i.y()) / (p_j.y() - p_i.y()) + p_i.x()))
      {
        c = !c;
      }
    }
    if ( c == true)
    {
        if ( set == false)
        {
            sheet = s;
            set = true;
        }
    }
  }
  return c;





}

dbskfg_composite_link_sptr 
dbskfg_utilities::first_adj_shock(dbskfg_composite_node_sptr shock_node)
{
    dbskfg_composite_link_sptr adj_edge=0;
    dbskfg_composite_node::edge_iterator srit;

    // Look at in edges first
    for ( srit  = shock_node->in_edges_begin() ; 
          srit != shock_node->in_edges_end()   ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
                (&(*(*srit)));
            if ( slink->shock_link_type() ==  dbskfg_shock_link::SHOCK_EDGE )
            {
                adj_edge=(*srit);
                break;
            }
        }  
    }

    if ( !adj_edge)
    {
        for ( srit  = shock_node->out_edges_begin() ; 
              srit != shock_node->out_edges_end()   ; ++srit)
        {
            if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
            {
                dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
                    (&(*(*srit)));
                if ( slink->shock_link_type() ==  
                     dbskfg_shock_link::SHOCK_EDGE )
                {
                    adj_edge=(*srit);
                    break;
                }
            }  
        }



    }

    return adj_edge;
}


dbskfg_composite_link_sptr 
dbskfg_utilities::first_adj_contour(dbskfg_composite_node_sptr contour_node)
{
    dbskfg_composite_link_sptr adj_edge=0;
    dbskfg_composite_node::edge_iterator srit;

    // Look at in edges first
    for ( srit  = contour_node->in_edges_begin() ; 
          srit != contour_node->in_edges_end()   ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        { 
            adj_edge=(*srit);
            break;
         
        }  
    }

    if ( !adj_edge)
    {
        for ( srit  = contour_node->out_edges_begin() ; 
              srit != contour_node->out_edges_end()   ; ++srit)
        {
            if ( (*srit)->link_type() == dbskfg_composite_link::CONTOUR_LINK )
            {
                adj_edge=(*srit);
                break;
             
            }  
        }


    }

    return adj_edge;
}

dbskfg_composite_link_sptr 
dbskfg_utilities::cyclic_adj_succ(dbskfg_composite_link_sptr edge,
                                  dbskfg_composite_node_sptr node)
{
    dbskfg_composite_link_sptr adj_edge=0;
    dbskfg_composite_node::edge_iterator srit;

    // Look at in edges first
    for ( srit  = node->in_edges_begin() ; 
          srit != node->in_edges_end()   ; ++srit)
    {
        if ((*srit) == edge ) // edge found
        {
            srit++;
            if ( srit != node->in_edges_end())
            {
                adj_edge=(*srit);
            }
            else
            {
                if ( node->out_edges().size() > 0 )
                {
                    adj_edge = node->out_edges().front(); 
                }
                else
                {
                    adj_edge = node->in_edges().front();
                }
            }
            break;

        }
    }

    if ( !adj_edge)
    {
        // Look at out edges first
        for ( srit  = node->out_edges_begin() ; 
              srit != node->out_edges_end()   ; ++srit)
        {
            if ((*srit) == edge ) // edge found
            {
                srit++;
                if ( srit != node->out_edges_end())
                {
                    adj_edge=(*srit);
                }
                else
                {
                    if ( node->in_edges().size() > 0 )
                    {
                        adj_edge = node->in_edges().front(); 
                    }
                    else
                    {
                        adj_edge = node->out_edges().front();
                    }
                }
                break;

            }
        }



    }

    return adj_edge;
}

dbskfg_composite_link_sptr 
dbskfg_utilities::cyclic_adj_pred(dbskfg_composite_link_sptr edge,
                                  dbskfg_composite_node_sptr node)
{
    dbskfg_composite_link_sptr adj_edge=0;
    dbskfg_composite_node::edge_iterator srit;

    // Look at in edges first
    for ( srit  = node->in_edges_begin() ; 
          srit != node->in_edges_end()   ; ++srit)
    {
        if ((*srit) == edge ) // edge found
        {
            if (srit == node->in_edges_begin()) 
            {
                //first in_edge, look in out_edges
                if (node->out_edges().size()>0)
                {
                    adj_edge = node->out_edges().back();
                }
                else //no out_edges
                {
                    adj_edge = node->in_edges().back();
                }
            }
            else {
                srit--;
                adj_edge = (*srit);
            }  
            break;
    
        }
    }

    if ( !adj_edge)
    {
        // Look at out edges first
        for ( srit  = node->out_edges_begin() ; 
              srit != node->out_edges_end()   ; ++srit)
        {
            if ((*srit) == edge ) // edge found
            {
                if (srit == node->out_edges_begin())
                {
                    //first out_edge, check in_edges
                    if (node->in_edges().size()>0)
                    {
                        adj_edge = node->in_edges().back();
                    }
                    else //no in_edges
                    {
                        adj_edge = node->out_edges().back();
                    }
                }
                else {
                    srit--;
                    adj_edge = (*srit);
                }   
                break;
            }
        }



    }

    return adj_edge;
}

dbskfg_composite_link_sptr
dbskfg_utilities::cyclic_adj_shock_succ(dbskfg_composite_link_sptr shock_edge,
                                        dbskfg_composite_node_sptr shock_node)
{
    dbskfg_composite_link_sptr adj_edge = 
        cyclic_adj_succ(shock_edge,shock_node);

    while(true)
    {
        if ( adj_edge->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
                (&(*adj_edge));

            if ( slink->shock_link_type() ==  dbskfg_shock_link::SHOCK_EDGE )
            {
                break;
            }
        }
        adj_edge = cyclic_adj_succ(adj_edge,shock_node);
    }

    return adj_edge;


}


dbskfg_composite_link_sptr
dbskfg_utilities::cyclic_adj_contour_succ(
    dbskfg_composite_link_sptr contour_edge,
    dbskfg_composite_node_sptr contour_node)
{
    dbskfg_composite_link_sptr adj_edge = 
        cyclic_adj_succ(contour_edge,contour_node);

    while(true)
    {
        if ( adj_edge->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            break;
        }
        adj_edge = cyclic_adj_succ(adj_edge,contour_node);
    }

    return adj_edge;


}

dbskfg_composite_link_sptr
dbskfg_utilities::cyclic_adj_shock_pred(dbskfg_composite_link_sptr shock_edge,
                                        dbskfg_composite_node_sptr shock_node)
{
    dbskfg_composite_link_sptr adj_edge = 
        cyclic_adj_pred(shock_edge,shock_node);

    while(true)
    {
        if ( adj_edge->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* slink = dynamic_cast<dbskfg_shock_link*>
                (&(*adj_edge));

            if ( slink->shock_link_type() ==  dbskfg_shock_link::SHOCK_EDGE )
            {
                break;
            }
        }
        adj_edge = cyclic_adj_pred(adj_edge,shock_node);
    }

    return adj_edge;


}

void dbskfg_utilities::detect_transforms( 
    vidpro1_vsol2D_storage_sptr& contour_storage,
    vcl_vector<dbskfg_transform_descriptor_sptr>& results,
    bool detect_gaps,
    bool detect_loops)
{

    // // Take input contour and compute shock

    // // 1) Get image storage class
    // vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

    // /*********************** Shock Compute **********************************/
    // // 3) Create shock pro process and assign inputs 
    // dbsk2d_compute_ishock_process shock_pro;

    // shock_pro.clear_input();
    // shock_pro.clear_output();

    // shock_pro.add_input(image_storage);
    // shock_pro.add_input(contour_storage);

    // // Set params
    // shock_pro.parameters()->set_value("-exist_ids",true);

    // bool status_flag=true;
    // status_flag = shock_pro.execute();

    // shock_pro.finish();

    // assert(status_flag == true);

    // // Grab output from shock computation
    // vcl_vector<bpro1_storage_sptr> shock_results;

    // shock_results = shock_pro.get_output();

    // // Clean up after ourselves
    // shock_pro.clear_input();
    // shock_pro.clear_output();

    // // Lets vertical cast to shock stroge
    // // Holds shock storage
    // dbsk2d_shock_storage_sptr shock_storage;
    // shock_storage.vertical_cast(shock_results[0]);

    // /*********************** Compute Composite Graph ************************/
    // dbskfg_form_composite_graph_process cg_pro;

    // cg_pro.clear_input();
    // cg_pro.clear_output();

    // cg_pro.add_input(shock_storage);
    // cg_pro.add_input(image_storage);

    // status_flag = cg_pro.execute();
    // cg_pro.finish();

    // assert(status_flag == true);

    // vcl_vector<bpro1_storage_sptr> cg_results;
    // cg_results = cg_pro.get_output();

    // cg_pro.clear_input();
    // cg_pro.clear_output();

    // dbskfg_composite_graph_storage_sptr cg_storage;
    // cg_storage.vertical_cast(cg_results[0]);


    // // Holds local_graph
    // dbskfg_composite_graph_sptr local_graph;
    // local_graph = cg_storage->get_composite_graph();

    // // Run the dbskfg detect transforms 
    // dbskfg_detect_transforms transformer(local_graph,
    //                                      cg_storage->get_image());

    // // Kick of detecting transformers
    // transformer.detect_transforms_simple(detect_gaps,
    //                                      detect_loops,
    //                                      0.25,
    //                                      0.5);

    // results=transformer.objects();




}


// Return set of points for euler_spiral
void dbskfg_utilities::ess_points(dbskfg_transform_descriptor_sptr transform,
                                  vcl_vector<vgl_point_2d<double> >& points)
{

    for ( unsigned int t=0; 
          t < transform->new_contours_spatial_objects_.size() ; ++t)
    {
        if ( transform->new_contours_spatial_objects_[t]->cast_to_curve()
             ->cast_to_line())
        {
            // Grab line
            vsol_line_2d* line = 
                transform->new_contours_spatial_objects_[t]
                ->cast_to_curve()->cast_to_line();

            vgl_point_2d<double> p0=vgl_point_2d<double>(line->p0()->x(),
                                                         line->p0()->y());
            vgl_point_2d<double> p1=vgl_point_2d<double>(line->p1()->x(),
                                                         line->p1()->y());
          
            points.push_back(p0);
            points.push_back(p1);

        }
        else
        {
            // Grab poly line
            vsol_polyline_2d* poly_line = 
                transform->new_contours_spatial_objects_[t]
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

                if ( !((v + 1) == poly_line->size()-1) )
                { 
                    points.push_back(p1);
                }

      
            }

        }

    }

}

// Return 1-jacard index
double dbskfg_utilities::jacard_distance(vgl_polygon<double>& poly1,
                                         vgl_polygon<double>& poly2)
{

    //Take union of two polygons
    int value;

    double jacard_index=0;

    vgl_polygon<double> intersect_poly = vgl_clip(
        poly1,                   // p1
        poly2,                   // p2
        vgl_clip_type_intersect, // p1 U p2
        &value);                 // test if success

    if ( intersect_poly.num_sheets() )
    {
        vgl_polygon<double> union_poly = vgl_clip(
            poly1,                  // p1
            poly2,                  // p2
            vgl_clip_type_union,    // p1 U p2
            &value);                // test if success

        double union_poly_area = vgl_area(union_poly);
        double intersect_poly_area = vgl_area(intersect_poly);

        jacard_index = intersect_poly_area/union_poly_area;

    }
    else
    {
        jacard_index = 0;
    }

    return 1.0-jacard_index;
}

void dbskfg_utilities::classify_nodes(dbskfg_composite_graph_sptr composite_graph_)
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

void dbskfg_utilities::read_binary_file
(vcl_string input_file,
 vcl_map<unsigned int,
 vcl_vector< vsol_spatial_object_2d_sptr > >& geoms,
 vcl_map<unsigned int,vcl_set<unsigned int> >& con_ids,
 vcl_pair<unsigned int,unsigned int>& image_size)
{
    
    vcl_ifstream file (input_file.c_str(), 
                       vcl_ios::in|vcl_ios::binary|vcl_ios::ate);

    double* memblock(0);
    unsigned int size_cons=0;
    if (file.is_open())
    {
        vcl_ifstream::pos_type size = file.tellg();
        size_cons=size/sizeof(double);
        memblock = new double[size_cons];
        file.seekg (0, vcl_ios::beg);
        file.read ((char *) memblock, size);
        file.close();

    }
    
    unsigned int image_ni = memblock[0];
    unsigned int image_nj = memblock[1];
    
    vcl_cout<<"Image size: "<<image_ni<<" by "<<image_nj<<vcl_endl;
    vcl_cout<<"Read in size_cons data: "<<size_cons<<vcl_endl;
    unsigned int c=2;
    unsigned int id=0;
    while ( c < size_cons)
    {
        unsigned int numb_contours=memblock[c];
        c++;
        unsigned int stop=numb_contours+c;
        while ( c < stop )
        {
            vgl_point_2d<double> p1(memblock[c],memblock[c+1]);
            vgl_point_2d<double> p2(memblock[c+2],memblock[c+3]);
            vsol_spatial_object_2d_sptr line=new vsol_line_2d(p1,p2);
            line->set_id(memblock[c+4]);
            geoms[id].push_back(line);
            con_ids[id].insert(memblock[c+4]);
            c=c+5;
        }
        id++;

    }

    image_size.first  = image_ni;
    image_size.second = image_nj;

}
