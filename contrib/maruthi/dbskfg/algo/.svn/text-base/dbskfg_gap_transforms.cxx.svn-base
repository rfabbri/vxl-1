// This is brcv/shp/dbskfg/dbskfg_compute_composite_graph.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_gap_transforms.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/algo/dbskfg_expand_local_context.h>
#include <dbskfg/algo/dbskfg_transform_manager.h>
// dbgl headers
#include <dbgl/algo/dbgl_eulerspiral.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
// vgl headers
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_lineseg_test.h>
// vcl headers
#include <vcl_sstream.h>
#include <vcl_algorithm.h>
// vnl headers
#include <vnl/vnl_vector_fixed.h>
// vil headers
#include <vil/vil_bilin_interp.h>
#include <vil/vil_image_resource.h>
// bil headers
#include <bil/algo/bil_color_conversions.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>

dbskfg_gap_transforms::dbskfg_gap_transforms
( 
    dbskfg_composite_graph_sptr composite_graph,
    vil_image_resource_sptr img_sptr
)
    :composite_graph_(composite_graph),
     ess_completion_(0.15),
     curve_power_(2.0f), 
     curve_offset_(0.1f), 
     curve_length_gamma_(4.0f),
     curve_gamma_(40.0f),
     color_gamma_(14.0f),
     alpha_(0.5f),
     largest_gap_(1.0f),
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

dbskfg_gap_transforms::~dbskfg_gap_transforms()
{


}

void dbskfg_gap_transforms::detect_gaps(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{
    
    // Keep track by looking at contour pair
    vcl_map< vcl_pair<unsigned int,unsigned int>, vcl_string> id_map;

    vcl_map<unsigned int,vcl_string> degree_ones;

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
                if ( shock_link->shock_compute_type() == 
                     dbskfg_utilities::PP )
                {

                    if ( ( shock_link->get_left_point()
                           ->get_composite_degree()==1 
                           && 
                           shock_link->get_right_point()
                           ->get_composite_degree()==1 ) 
                         &&
                         !(shock_link->source()->degree() == 1 &&
                           shock_link->target()->degree() == 1 ))
                    {
                        dbskfg_contour_node* lcnode = 
                            shock_link->get_left_point();
                        dbskfg_contour_node* rcnode = 
                            shock_link->get_right_point();
                        
                        if ( lcnode->contour_id() != rcnode->contour_id() )
                        {
                            vcl_pair<unsigned int,unsigned int> cpair = 
                                vcl_make_pair(lcnode->id(),rcnode->id());
                            
                            // Make pair
                            if ( id_map.count(cpair) == 0 )
                            {
                                bool flag = 
                                    detect_gaps_helper(shock_link,objects);

                                if ( flag )
                                {
                                    degree_ones[lcnode->id()]="temp";
                                    degree_ones[rcnode->id()]="temp";
                                }

                                id_map[cpair]="temp";
                                id_map[vcl_make_pair(cpair.second,
                                                     cpair.first)]="temp";
                            }
                         
                        }
                    }

                }
            }
        
        }
    
    }

    // for (dbskfg_composite_graph::edge_iterator eit =
    //          composite_graph_->edges_begin();
    //       eit != composite_graph_->edges_end(); ++eit)
    // {
    //     dbskfg_composite_link_sptr link = *eit;
    //     if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
    //     {
    //         dbskfg_shock_link* shock_link=
    //             dynamic_cast<dbskfg_shock_link*>(&(*link));

    //         if ( shock_link->shock_link_type() == 
    //              dbskfg_shock_link::SHOCK_EDGE )
    //         {
    //             if ( shock_link->shock_compute_type() ==
    //                  dbskfg_utilities::RLLP ||
    //                  shock_link->shock_compute_type() ==
    //                  dbskfg_utilities::LLRP )
    //             {
    //                 dbskfg_contour_node* endpoint(0);
                     
    //                 if ( shock_link->shock_compute_type() ==
    //                      dbskfg_utilities::RLLP )
    //                 {
    //                     endpoint = shock_link->get_left_point();
    //                 }
    //                 else
    //                 {
    //                     endpoint = shock_link->get_right_point();

    //                 }

    //                 if ( endpoint->get_composite_degree() == 1 &&
    //                      degree_ones.count(endpoint->id()) == 0)
    //                 {
    //                     degree_ones[endpoint->id()]="temp";
    //                     detect_gap_4_helper(endpoint,objects);

    //                 }
                    
    //             }

    //         }

    //     }
    // }

}


void dbskfg_gap_transforms::detect_gaps_knn(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
    unsigned int K)
{
    

    vcl_map< vcl_pair<unsigned int,unsigned int>, vcl_string> id_map;

    // degree one nodes
    vcl_vector<dbskfg_composite_node_sptr> degree_ones;

    for (dbskfg_composite_graph::vertex_iterator vit =
             composite_graph_->vertices_begin();
          vit != composite_graph_->vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = *vit;
        if ( node->node_type() == dbskfg_composite_node::CONTOUR_NODE )
        {
            if ( node->get_composite_degree() == 1 )
            {
                degree_ones.push_back(node);
            }

        }
    }

    for ( unsigned int i=0; i < degree_ones.size() ; ++i)
    {
        dbskfg_composite_node_sptr vertex=degree_ones[i];
        dbskfg_contour_node* con_node1 = dynamic_cast<dbskfg_contour_node*>
            (&(*vertex));

        vcl_map<double,dbskfg_composite_node_sptr> local_distances;
        for ( unsigned int j=0; j < degree_ones.size() ; ++j)
        {

            dbskfg_contour_node* con_node2 = dynamic_cast<dbskfg_contour_node*>
                (&(*degree_ones[j]));

            if ( (j != i) && 
                 (con_node1->contour_id() != con_node2->contour_id()))
            {
                dbskfg_composite_node_sptr vertex2=degree_ones[j];
                double distance = vgl_distance(vertex->pt(),
                                               vertex2->pt());
                local_distances[distance]=vertex2;
            }

        }

        vcl_map<double,dbskfg_composite_node_sptr>::iterator it;
        
        unsigned int count=0;
        for ( it =local_distances.begin() ; 
              it != local_distances.end() ; ++it)
        {
            if ( count == K )
            {
                break;
            }
            
            dbskfg_composite_node_sptr lcnode=vertex;
            dbskfg_composite_node_sptr rcnode=(*it).second;

            vcl_pair<unsigned int,unsigned int> 
                cpair = 
                vcl_make_pair(lcnode->id(),rcnode->id());
                            
            // Make pair
            if ( id_map.count(cpair) == 0 )
            {
                bool flag = 
                    detect_gaps_helper(lcnode,rcnode,objects);
                id_map[cpair]="temp";
                id_map[vcl_make_pair(cpair.second,
                                     cpair.first)]="temp";
            }

            count=count+1;
            
        }

        
    }


    // Keep track by looking at contour pair
    vcl_map< vcl_pair<unsigned int,unsigned int>, vcl_string> id_map_shock;
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
                    if ( shock_link->shock_compute_type() == 
                         dbskfg_utilities::PP )
                    {

                        if ( ( shock_link->get_left_point()
                               ->get_composite_degree()==1 
                               && 
                               shock_link->get_right_point()
                               ->get_composite_degree()==1 ) 
                             &&
                             !(shock_link->source()->degree() == 1 &&
                               shock_link->target()->degree() == 1 ))
                        {
                            dbskfg_contour_node* lcnode = 
                                shock_link->get_left_point();
                            dbskfg_contour_node* rcnode = 
                                shock_link->get_right_point();
                        
                            if ( lcnode->contour_id() != rcnode->contour_id() )
                            {
                                vcl_pair<unsigned int,unsigned int> cpair = 
                                    vcl_make_pair(lcnode->id(),rcnode->id());
                            
                                // Make pair
                                if ( id_map_shock.count(cpair) == 0 )
                                {

                                    id_map_shock[cpair]="temp";
                                    id_map_shock[vcl_make_pair(cpair.second,
                                                               cpair.first)]
                                        ="temp";
                                }
                         
                            }
                        }

                    }
                }
        
            }
    
        }
    }


    for ( unsigned int b=0; b < objects.size() ; ++b)
    {
        dbskfg_composite_node_sptr pt1 = objects[b]->gap_.first;
        dbskfg_composite_node_sptr pt2 = objects[b]->gap_.second;
        
        vcl_pair<unsigned int,unsigned int> pair = 
            vcl_make_pair(pt1->id(),pt2->id());

        if ( id_map_shock.count(pair))
        {
            objects[b]->shock_link_found_ = 1.0;
        }
        else
        {
            objects[b]->shock_link_found_ = 0.0;
        }
    }

}


void dbskfg_gap_transforms::detect_gaps_single(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{
    
    // Keep track by looking at contour pair
    vcl_map< vcl_pair<unsigned int,unsigned int>, vcl_string> id_map;

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
                if ( shock_link->shock_compute_type() == 
                     dbskfg_utilities::PP )
                {
                    
                    if ( (shock_link->get_left_point()
                         ->get_composite_degree()==1 
                          && 
                          shock_link->get_right_point()
                          ->get_composite_degree()==1 )
                         &&
                         !(shock_link->source()->degree() == 1 &&
                           shock_link->target()->degree() == 1 ))
                    {
                        dbskfg_contour_node* lcnode = 
                            shock_link->get_left_point();
                        dbskfg_contour_node* rcnode = 
                            shock_link->get_right_point();
                   
                        vcl_stringstream leftstream;
                        vcl_stringstream rightstream;
                        leftstream<<lcnode->pt();
                        rightstream<<rcnode->pt();
            
                        bool f1 = dbskfg_transform_manager::Instance().
                            endpoint(leftstream.str());
                        bool f2 = dbskfg_transform_manager::Instance().
                            endpoint(rightstream.str());

                        if ( f1 && f2 )
                        {
                   
                            if ( lcnode->contour_id() != 
                                 rcnode->contour_id() )
                            {
                                vcl_pair<unsigned int,unsigned int> 
                                    cpair = 
                                    vcl_make_pair(lcnode->id(),rcnode->id());
                            
                                // Make pair
                                if ( id_map.count(cpair) == 0 )
                                {
                                    bool flag = 
                                        detect_gaps_helper(shock_link,objects);
                                    id_map[cpair]="temp";
                                    id_map[vcl_make_pair(cpair.second,
                                                         cpair.first)]="temp";
                                }
                            }
                        }
                    }
                    

                }
            }
        
        }
    
    }

}


void dbskfg_gap_transforms::detect_gaps_endpoint(
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
    vcl_vector<dbskfg_composite_node_sptr>& endpoints)
{
    // Keep track by looking at contour pair
    vcl_map< vcl_pair<unsigned int,unsigned int>, vcl_string> id_map;

    // Keep degree ones
    vcl_map<unsigned int,vcl_string> degree_ones;

    for ( unsigned int c=0; c < endpoints.size() ; ++c )
    {
        dbskfg_contour_node* con_endpoint = dynamic_cast<dbskfg_contour_node*>
            (&(*endpoints[c]));

        vcl_vector<dbskfg_shock_link*> shocks = con_endpoint->shocks_affected();

        for ( unsigned int s=0; s < shocks.size() ; ++s)
        {
        
            dbskfg_shock_link* shock_link= shocks[s];

            if ( shock_link->shock_compute_type() == 
                 dbskfg_utilities::PP )
            {

                if ( (shock_link->get_left_point()
                      ->get_composite_degree()==1 
                      && 
                      shock_link->get_right_point()
                      ->get_composite_degree()==1 ) 
                     &&
                     !(shock_link->source()->degree() == 1 &&
                       shock_link->target()->degree() == 1 ))
                {

                    dbskfg_contour_node* lcnode = 
                        shock_link->get_left_point();
                    dbskfg_contour_node* rcnode = 
                        shock_link->get_right_point();
                   
                    vcl_stringstream leftstream;
                    vcl_stringstream rightstream;
                    leftstream<<lcnode->pt();
                    rightstream<<rcnode->pt();
                
                    bool f1 = dbskfg_transform_manager::Instance().
                        endpoint(leftstream.str());
                    bool f2 = dbskfg_transform_manager::Instance().
                        endpoint(rightstream.str());
                
                    if ( lcnode->contour_id() != 
                         rcnode->contour_id() )
                    {
                        vcl_pair<unsigned int,unsigned int> 
                            cpair = 
                            vcl_make_pair(lcnode->id(),rcnode->id());
                        
                        // Make pair
                        if ( id_map.count(cpair) == 0 )
                        {

                            bool flag = 
                                detect_gaps_helper(shock_link,objects);

                            if ( flag )
                            {
                                degree_ones[lcnode->id()]="temp";
                                degree_ones[rcnode->id()]="temp";
                            }

                            id_map[cpair]="temp";
                            id_map[vcl_make_pair(cpair.second,
                                                 cpair.first)]="temp";
                        }
                        
                    }
                }

            
            }
        }
    
    }


    // for ( unsigned int c=0; c < endpoints.size() ; ++c )
    // {
    //     dbskfg_contour_node* con_endpoint = 
    //         dynamic_cast<dbskfg_contour_node*>
    //         (&(*endpoints[c]));

    //     if ( degree_ones.count(con_endpoint->id()) == 0 )
    //     {
    //         detect_gap_4_helper(con_endpoint,objects);
    //     }
    // }
}

bool dbskfg_gap_transforms::detect_gaps_helper(dbskfg_shock_link* shock_link,
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{

    // Create new transform object
    dbskfg_transform_descriptor_sptr transformer = new
        dbskfg_transform_descriptor();

    // Set type of transform
    transformer->transform_type_ = dbskfg_transform_descriptor::GAP;

    // Push back in vector
    objects.push_back(transformer);

    // Set transformer pair
    transformer->gap_.first=shock_link->get_left_point();
    transformer->gap_.second=shock_link->get_right_point();

    //********************* Add contours first *****************************
    // Fill in gap with euler spiral
    dbskfg_contour_node* lpt
        = dynamic_cast<dbskfg_contour_node*>
        (&(*(shock_link->get_left_point())));
            
    dbskfg_contour_node* rpt
        = dynamic_cast<dbskfg_contour_node*>
        (&(*(shock_link->get_right_point())));
                          
    double theta1=0.0;
    double theta2=0.0;

    vcl_pair<double,double> tan_pair=
        dbskfg_utilities::get_tangent_pairs(lpt,rpt,theta1,theta2);

    // Use two percent of gap
    double offset = vgl_distance(lpt->pt(),rpt->pt())*ess_completion_;

    vgl_point_2d<double> start_point(
        lpt->pt().x()+offset*vcl_cos(tan_pair.first),
        lpt->pt().y()+offset*vcl_sin(tan_pair.first));

    vgl_point_2d<double> end_point(
        rpt->pt().x()-offset*vcl_cos(tan_pair.second),
        rpt->pt().y()-offset*vcl_sin(tan_pair.second));

    // Add in contours for front 
    vsol_spatial_object_2d_sptr obj=
        new vsol_line_2d(lpt->pt(),
                         start_point);
    transformer->new_contours_spatial_objects_.push_back(obj);

    dbgl_eulerspiral es(
        start_point,
        tan_pair.first,
        end_point,
        tan_pair.second);

    // Convert euler spiral to vsol object
    vcl_vector<vgl_point_2d<double> > point_samples;
    vcl_vector<vsol_point_2d_sptr> ps;
    es.compute_spiral(point_samples, 0.1);
    {
        for (unsigned j = 0; j < point_samples.size(); j++)
        {
            ps.push_back(new vsol_point_2d(point_samples[j]));
        }

        vsol_spatial_object_2d_sptr poly = new vsol_polyline_2d(ps);
        transformer->new_contours_spatial_objects_.push_back(poly);
    }

    // Add in contours for front 
    obj=
        new vsol_line_2d(end_point,
                         rpt->pt());

    transformer->new_contours_spatial_objects_.push_back(obj);

    //********************** Create dbsol curve for  *************************
    double length_of_gap=vgl_distance(start_point,
                                      end_point);
    double curve_app=0;

    //********************** Determine Local Context *************************

    // Determine influence zone between two end points
    dbskfg_utilities::gap_1_local_context(shock_link->get_left_point(),
                                          shock_link->get_right_point(),
                                          shock_link->id(),
                                          transformer);
                
    //********************** Expand Local Context *************************
    dbskfg_expand_local_context expand(transformer);


    //********************** Add extra contour nodes *********************
    vcl_map<unsigned int,vcl_string> contour_map;
    for ( unsigned int j=0; j < transformer->contours_affected_.size();
          ++j)
    {

        dbskfg_contour_link* contour_link = dynamic_cast<dbskfg_contour_link*>(
            &(*transformer->contours_affected_[j]));
        contour_map[contour_link->source()->id()]="temp";
        contour_map[contour_link->target()->id()]="temp";
    }

    for ( unsigned int k=0; k < transformer->shock_links_affected_.size();
          ++k)
    {

        dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>(
            &(*transformer->shock_links_affected_[k]));
        if ( shock_link->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            if ( contour_map.count(shock_link->get_left_point()->id()) == 0
                 && (shock_link->get_left_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_left_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            if ( contour_map.count(shock_link->get_right_point()->id()) == 0
                 && (shock_link->get_right_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_right_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::PP )
        {
            if ( contour_map.count(shock_link->get_right_point()->id()) == 0
                 && (shock_link->get_right_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_right_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }

            if ( contour_map.count(shock_link->get_left_point()->id()) == 0
                 && (shock_link->get_left_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_left_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

    }

    if ( !valid_euler_spiral(transformer))
    {
        vcl_cout<<"Invalid Euler Spiral Intersects with Original Contours"
                <<vcl_endl;
        objects.pop_back();
        return false;
    }
    
    //********************** Calculate App Cost  *********************
    // First determine polygons to consider
    vgl_polygon<double> poly1(1);
    vgl_polygon<double> poly2(1);

    determine_contrast_polygons(transformer,shock_link,poly1,poly2);
    double app_cost = determine_app_cost(poly1,poly2);

    transformer->cost_=0;

    transformer->app_cost_=curve_app;

    // Set euler spiral parameters
    transformer->gamma_    = es.gamma();
    transformer->k0_       = es.k0();
    transformer->length_   = es.length();
    transformer->d_        = length_of_gap;
    transformer->theta1_   = theta1;
    transformer->theta2_   = theta2;

    transformer->remove_extra_shocks();

    transformer->cost_= 
        dbskfg_transform_manager::Instance().transform_probability(
            es.gamma()* length_of_gap* length_of_gap,
            es.k0()*length_of_gap,
            es.length());

    return true;
}


bool dbskfg_gap_transforms::detect_gaps_helper(
    dbskfg_composite_node_sptr pt1,
    dbskfg_composite_node_sptr pt2,
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{

    // Create new transform object
    dbskfg_transform_descriptor_sptr transformer = new
        dbskfg_transform_descriptor();

    // Set type of transform
    transformer->transform_type_ = dbskfg_transform_descriptor::GAP;

    // Push back in vector
    objects.push_back(transformer);

    // Set transformer pair
    transformer->gap_.first=pt1;
    transformer->gap_.second=pt2;

    //********************* Add contours first *****************************
    // Fill in gap with euler spiral
    dbskfg_contour_node* lpt
        = dynamic_cast<dbskfg_contour_node*>
        (&(*pt1));
            
    dbskfg_contour_node* rpt
        = dynamic_cast<dbskfg_contour_node*>
        (&(*pt2));
                          
    double theta1=0.0;
    double theta2=0.0;

    vcl_pair<double,double> tan_pair=
        dbskfg_utilities::get_tangent_pairs(lpt,rpt,theta1,theta2);

    // Use two percent of gap
    double offset = vgl_distance(lpt->pt(),rpt->pt())*ess_completion_;

    vgl_point_2d<double> start_point(
        lpt->pt().x()+offset*vcl_cos(tan_pair.first),
        lpt->pt().y()+offset*vcl_sin(tan_pair.first));

    vgl_point_2d<double> end_point(
        rpt->pt().x()-offset*vcl_cos(tan_pair.second),
        rpt->pt().y()-offset*vcl_sin(tan_pair.second));

    // Add in contours for front 
    vsol_spatial_object_2d_sptr obj=
        new vsol_line_2d(lpt->pt(),
                         start_point);
    transformer->new_contours_spatial_objects_.push_back(obj);

    dbgl_eulerspiral es(
        start_point,
        tan_pair.first,
        end_point,
        tan_pair.second);

    // Convert euler spiral to vsol object
    vcl_vector<vgl_point_2d<double> > point_samples;
    vcl_vector<vsol_point_2d_sptr> ps;
    es.compute_spiral(point_samples, 0.1);
    {
        for (unsigned j = 0; j < point_samples.size(); j++)
        {
            ps.push_back(new vsol_point_2d(point_samples[j]));
        }

        vsol_spatial_object_2d_sptr poly = new vsol_polyline_2d(ps);
        transformer->new_contours_spatial_objects_.push_back(poly);
    }

    // Add in contours for front 
    obj=
        new vsol_line_2d(end_point,
                         rpt->pt());

    transformer->new_contours_spatial_objects_.push_back(obj);

    //********************** Create dbsol curve for  *************************
    point_samples.insert(point_samples.begin(),lpt->pt());
    point_samples.insert(point_samples.end(),rpt->pt());

    for (dbskfg_composite_graph::edge_iterator eit =
             composite_graph_->edges_begin();
          eit != composite_graph_->edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link = *eit;
        if ( link->link_type() == dbskfg_composite_link::CONTOUR_LINK )
        {
            transformer->contours_affected_.push_back(link);
        }

    }

    double length_of_gap=vgl_distance(start_point,
                                      end_point);
    double curve_app=0;

    if ( !valid_euler_spiral(transformer))
    {
        vcl_cout<<"Invalid Euler Spiral Intersects with Original Contours"
                <<vcl_endl;
        objects.pop_back();
        return false;
    }
    
    transformer->cost_=0;

    transformer->app_cost_=curve_app;

    // Set euler spiral parameters
    transformer->gamma_    = es.gamma();
    transformer->k0_       = es.k0();
    transformer->length_   = es.length();
    transformer->d_        = length_of_gap;
    transformer->theta1_   = theta1;
    transformer->theta2_   = theta2;

    return true;
}

// Helper function to detect gaps
void dbskfg_gap_transforms::detect_gap_4_helper(
    dbskfg_contour_node* endpoint,
    vcl_vector<dbskfg_transform_descriptor_sptr>& objects)
{
    // Create new transform object
    dbskfg_transform_descriptor_sptr transformer = new
        dbskfg_transform_descriptor();

    // Set type of transform
    transformer->transform_type_ = dbskfg_transform_descriptor::GAP;

    // Set bool flag to indicate t type
    transformer->t_type_transform_ = true;

    // Push back in vector
    objects.push_back(transformer);

 
    //********************** Determine Local Context *************************

    vgl_point_2d<double> proj_point;

    // Determine influence zone between two end points
    dbskfg_composite_node_sptr other_point = 
        dbskfg_utilities::gap_4_local_context(endpoint,
                                              transformer,
                                              proj_point);
 
    if ( other_point == 0 )
    {
        objects.pop_back();
        return;
    }
    
    // Set transformer pair
    transformer->gap_.first  = endpoint;
    transformer->gap_.second = other_point;

    // Add in contours for front 
    vsol_spatial_object_2d_sptr obj=
        new vsol_line_2d(endpoint->pt(),
                         proj_point);
    transformer->new_contours_spatial_objects_.push_back(obj);
           
    //********************** Expand Local Context *************************
    dbskfg_expand_local_context expand(transformer);

    //********************** Add extra contour nodes *********************
    vcl_map<unsigned int,vcl_string> contour_map;
    for ( unsigned int j=0; j < transformer->contours_affected_.size();
          ++j)
    {

        dbskfg_contour_link* contour_link = dynamic_cast<dbskfg_contour_link*>(
            &(*transformer->contours_affected_[j]));
        contour_map[contour_link->source()->id()]="temp";
        contour_map[contour_link->target()->id()]="temp";
    }

    for ( unsigned int k=0; k < transformer->shock_links_affected_.size();
          ++k)
    {

        dbskfg_shock_link* shock_link = dynamic_cast<dbskfg_shock_link*>(
            &(*transformer->shock_links_affected_[k]));
        if ( shock_link->shock_compute_type() == dbskfg_utilities::RLLP )
        {
            if ( contour_map.count(shock_link->get_left_point()->id()) == 0
                 && (shock_link->get_left_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_left_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::LLRP )
        {
            if ( contour_map.count(shock_link->get_right_point()->id()) == 0
                 && (shock_link->get_right_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_right_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }
        }

        if ( shock_link->shock_compute_type() == dbskfg_utilities::PP )
        {
            if ( contour_map.count(shock_link->get_right_point()->id()) == 0
                 && (shock_link->get_right_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_right_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_right_point()))
                {

                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_right_point());

                }
            }

            if ( contour_map.count(shock_link->get_left_point()->id()) == 0
                 && (shock_link->get_left_point()->id() != 
                     transformer->gap_.first->id() &&
                     shock_link->get_left_point()->id() != 
                     transformer->gap_.second->id()))
            {
                if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                         transformer->contours_nodes_affected_,
                         shock_link->get_left_point()))
                {
                    transformer->contours_nodes_affected_.
                        push_back(shock_link->get_left_point());
                }

            }
        }

    }


    transformer->cost_=0;
    transformer->app_cost_=0;

    double length_of_gap=vgl_distance(endpoint->pt(),
                                      other_point->pt());

    // Set euler spiral parameters
    transformer->gamma_    = 0;
    transformer->k0_       = 0;
    transformer->length_   = length_of_gap;
    transformer->d_        = length_of_gap;

    transformer->cost_= 
        dbskfg_transform_manager::Instance().transform_probability(
            0,
            0,
            length_of_gap);

    transformer->remove_extra_shocks();

}

bool dbskfg_gap_transforms::valid_euler_spiral(
    dbskfg_transform_descriptor_sptr& transform)
{

    // Create a vector of line segments
    vcl_vector<vgl_line_segment_2d<double> > line_segs;

    // Grab first line
    vsol_line_2d* first_line = transform->new_contours_spatial_objects_[0]
        ->cast_to_curve()->cast_to_line();
    vgl_point_2d<double> p0=vgl_point_2d<double>(first_line->p0()->x(),
                                                 first_line->p0()->y());
    vgl_point_2d<double> p1=vgl_point_2d<double>(first_line->p1()->x(),
                                                 first_line->p1()->y());
    vgl_line_segment_2d<double> first_vgl=
        vgl_line_segment_2d<double>(p0,
                                    p1);

    // Grab second line
    vsol_line_2d* second_line = 
        transform->new_contours_spatial_objects_
        [transform->new_contours_spatial_objects_.size()-1]
        ->cast_to_curve()->cast_to_line();
    p0=vgl_point_2d<double>(second_line->p0()->x(),
                            second_line->p0()->y());
    p1=vgl_point_2d<double>(second_line->p1()->x(),
                            second_line->p1()->y());

    vgl_line_segment_2d<double> second_vgl=
        vgl_line_segment_2d<double>(p0,
                                    p1);

    line_segs.push_back(first_vgl);
    line_segs.push_back(second_vgl);

    // Grab poly line
    vsol_polyline_2d* poly_line = transform->new_contours_spatial_objects_[1]
        ->cast_to_curve()->cast_to_polyline();

    // Grab distance to poly line
    // We have to also account for distances to the individual points
    for ( unsigned int v=0; v < poly_line->size()-1 ; ++v )
    {
        p0=vgl_point_2d<double>(poly_line->vertex(v)->x(),
                                poly_line->vertex(v)->y());
        p1=vgl_point_2d<double>(poly_line->vertex(v+1)->x(),
                                poly_line->vertex(v+1)->y());
        vgl_line_segment_2d<double> line_seg=
            vgl_line_segment_2d<double>(p0,
                                        p1);
        line_segs.push_back(line_seg);
    }


    bool valid=true;
    // Loop over line segs and see if it intersects with anything else
    for ( unsigned int k=0; k < line_segs.size() ; ++k)
    {

        vgl_line_segment_2d<double> line_seg = line_segs[k];

        vgl_point_2d<double> pt1=line_seg.point1();
        vgl_point_2d<double> pt2=line_seg.point2();

        // Loop over contours affected
        vcl_vector<dbskfg_composite_link_sptr>::iterator lit;
        for ( lit = transform->contours_affected_.begin() ; 
              lit != transform->contours_affected_.end() ; ++lit )
        {
      
            if ( ((*lit)->source()->id() != transform->gap_.first->id() &&
                  (*lit)->source()->id() != transform->gap_.second->id() )
                 &&
                 ((*lit)->target()->id() != transform->gap_.first->id() &&
                  (*lit)->target()->id() != transform->gap_.second->id() )
                )
            {
                vgl_line_segment_2d<double> contour = 
                    vgl_line_segment_2d<double>(
                    (*lit)->source()->pt(),
                    (*lit)->target()->pt());
       
                if ( vgl_lineseg_test_lineseg(line_seg,contour))
                {
                    valid=false;
                    break;
                }
            }
        }
        
        if ( !valid)
        {
            break;
        }

    }

    return valid;
}
    
double dbskfg_gap_transforms::determine_app_cost(
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

void dbskfg_gap_transforms::determine_contrast_polygons(
    dbskfg_transform_descriptor_sptr& transform,
    dbskfg_shock_link* slink,
    vgl_polygon<double>& poly1,vgl_polygon<double>& poly2)
{

    // Keep transform poly
    vgl_polygon<double> transform_poly = transform->poly_;
    vgl_point_2d<double> gap1=transform->gap_.first->pt();
    vgl_point_2d<double> gap2=transform->gap_.second->pt();

    // Look at first sheet only
    vcl_stringstream gap1_stream;
    vcl_stringstream gap2_stream;
    gap1_stream<<gap1;
    gap2_stream<<gap2;

    unsigned int gap1_index(0);
    unsigned int gap2_index(0);

    // Find start stop index of gap points
    for (unsigned int p = 0; p < transform_poly[0].size(); ++p) 
    {
        vcl_stringstream temp;
        vgl_point_2d<double> c0(transform_poly[0][p].x(),
                                transform_poly[0][p].y());
        temp<<c0;

        if ( c0 == gap1 || temp.str() == gap1_stream.str())
        {
            gap1_index = p;
        }

        if ( c0 == gap2 || temp.str() == gap2_stream.str())
        {
            gap2_index = p;
        }

        
    }
  
    // We have two cases where the bounding contour is fully contained
    // within the polygon
    // If is one sided then point will be in poly1,otherwise it is two
    // sided
    // Lets get the first polygon from the which will be bounded by
    // the two gaps
    dbskfg_composite_node_sptr wavefront;

    //Keep track of number of outer shock nodes in between 
    // If it is one sided then we will only have a count of one
    unsigned int outer_count(0);
    for (unsigned int o=0; o < transform->outer_shock_nodes_.size() ; ++o)
    {
        vcl_stringstream outer_shock_node;
        vgl_point_2d<double> snode = transform->outer_shock_nodes_[o]->pt();
        outer_shock_node<<snode;
        
        unsigned int start=vcl_min(gap1_index,gap2_index);
        unsigned int stop =vcl_max(gap1_index,gap2_index);

        for ( unsigned int b=start; b <= stop ; ++b)
        {
            vgl_point_2d<double> c0(transform_poly[0][b].x(),
                                    transform_poly[0][b].y());        
            vcl_stringstream temp;
            temp<<c0;
  
            if (c0 == snode || outer_shock_node.str() == temp.str())
            {
                wavefront=transform->outer_shock_nodes_[o];
                outer_count++;
                break;
            }
        }
    }

    if (outer_count == 1)
    {
        // Grab next shock link
        dbskfg_composite_link_sptr link1=dbskfg_utilities::
            first_adj_shock(wavefront);

        // Cast to shock link
        dbskfg_shock_link* slink1 = dynamic_cast<dbskfg_shock_link*>
            (&(*link1));
        poly1.clear();
        slink1->get_rag_node()->fragment_boundary(poly1);

        // Grab next shock link
        dbskfg_composite_link_sptr link2=dbskfg_utilities::
            cyclic_adj_shock_succ(link1,wavefront);
        
        // Cast to shock link
        dbskfg_shock_link* slink2 = dynamic_cast<dbskfg_shock_link*>
            (&(*link2));
        poly2.clear();
        slink2->get_rag_node()->fragment_boundary(poly2);

    }
    else
    {                                              
        unsigned int start=vcl_min(gap1_index,gap2_index);
        unsigned int stop =vcl_max(gap1_index,gap2_index);

        for ( unsigned int b=start; b <= stop ; ++b)
        {
            vgl_point_2d<double> c0(transform_poly[0][b].x(),
                                    transform_poly[0][b].y());        
            poly1.push_back(c0);
        }

        // Now to create the second polygon we have to take the outer sets
        // do in two loops
        for ( unsigned int a =0 ; a <= vcl_min(gap1_index,gap2_index) ; ++a)
        {
            vgl_point_2d<double> c0(transform_poly[0][a].x(),
                                    transform_poly[0][a].y());
            poly2.push_back(c0);

        }

        for ( unsigned int c =vcl_max(gap1_index,gap2_index) ; 
              c < transform_poly[0].size(); ++c)
        {
            vgl_point_2d<double> c0(transform_poly[0][c].x(),
                                    transform_poly[0][c].y());
            poly2.push_back(c0);

        }
    }


}
