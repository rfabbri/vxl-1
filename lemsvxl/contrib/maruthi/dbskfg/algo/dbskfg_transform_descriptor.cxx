// This is brcv/shp/dbskfg/algo/dbskfg_transform_descriptor.h

//:
// \file

// dbskfg headers
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_shock_link.h>
// vsol headers
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
// bsol headers
#include <bsol/bsol_algs.h>
// vgl headers
#include <vgl/vgl_clip.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_distance.h>
// dbxml headers
#include <dbxml/dbxml_algos.h>

dbskfg_transform_descriptor::dbskfg_transform_descriptor()
 :vbl_ref_count(),
  id_(0),
  processed_(false),
  transform_type_(dbskfg_transform_descriptor::LOOP),
  cost_(0),
  contour_cost_(0),
  app_cost_(0),
  gamma_(0),
  k0_(0),
  length_(0),
  d_(0),
  theta1_(0),
  theta2_(0),
  t_type_transform_(false),
  poly_(),
  gap_(0,0),
  gap_id_(0),
  hidden_(false),
  shock_link_found_(true)
{
    


}

dbskfg_transform_descriptor::~dbskfg_transform_descriptor()
{

    contours_to_remove_.clear();
    contours_affected_.clear();
    contours_nodes_affected_.clear();
    shock_nodes_affected_.clear();
    shock_links_affected_.clear();
    contours_spatial_objects_.clear();
    outer_shock_nodes_.clear();
    new_contours_spatial_objects_.clear();
    poly_.clear();
    loop_endpoints_.clear();
    all_gaps_.clear();
    polygon_string_rep_.clear();

}

void dbskfg_transform_descriptor::destroy_transform()
{

    contours_to_remove_.clear();
    contours_affected_.clear();
    contours_nodes_affected_.clear();
    shock_nodes_affected_.clear();
    shock_links_affected_.clear();
    contours_spatial_objects_.clear();
    outer_shock_nodes_.clear();
    new_contours_spatial_objects_.clear();
    poly_.clear();
    loop_endpoints_.clear();
    all_gaps_.clear();    
    polygon_string_rep_.clear();
}

void dbskfg_transform_descriptor::convert_contours_to_vsol()
{

    // Convert all contours to vsol objects for easy rendering
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;

    for ( lit = contours_affected_.begin() ; 
          lit != contours_affected_.end() ; ++lit )
    {
      
        vsol_spatial_object_2d_sptr obj=
            new vsol_line_2d((*lit)->source()->pt(),
                             (*lit)->target()->pt());
        obj->set_id((*lit)->id());
        contours_spatial_objects_.push_back(obj);


    }

    // Convert all contours to vsol objects for easy rendering
    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

    for ( nit = contours_nodes_affected_.begin() ; 
          nit != contours_nodes_affected_.end() ; ++nit )
    {
      
        vsol_spatial_object_2d_sptr obj=
            new vsol_point_2d((*nit)->pt().x(),
                              (*nit)->pt().y());
        obj->set_id((*nit)->id());
        contours_spatial_objects_.push_back(obj);


    }

}

// void dbskfg_transform_descriptor::convert_contours_to_vsol()
// {
   
//     // Convert all contours to vsol objects for easy rendering
//     vcl_vector<dbskfg_composite_link_sptr>::iterator lit;

//     for ( lit = contours_affected_.begin() ; 
//           lit != contours_affected_.end() ; ++lit )
//     {
      
//         vsol_spatial_object_2d_sptr obj=
//             new vsol_line_2d((*lit)->source()->pt(),
//                              (*lit)->target()->pt());
//         dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
//             (&(*(*lit)));
//         obj->set_id(clink->contour_id());//(*lit)->id());
//         contours_spatial_objects_.push_back(obj);


//     }

//     // Convert all contours to vsol objects for easy rendering
//     vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

//     for ( nit = contours_nodes_affected_.begin() ; 
//           nit != contours_nodes_affected_.end() ; ++nit )
//     {
//         if ( (*nit)->get_composite_degree() == 1 )
//         {
//             // Find incoming outgoing/link
//             dbskfg_composite_link_sptr link;
//             bool flag=false;

//             if ( (*nit)->in_degree() )
//             {
//                 link = *((*nit)->in_edges_begin());
//             }
//             else
//             {
//                 link = *((*nit)->out_edges_begin());                    
//             }

//             vsol_spatial_object_2d_sptr obj=
//                 new vsol_line_2d(link->source()->pt(),
//                                  link->target()->pt());
//             dbskfg_contour_link* clink = dynamic_cast<dbskfg_contour_link*>
//                 (&(*link));
//             obj->set_id(clink->contour_id());
//             contours_spatial_objects_.push_back(obj);

//         }
//         else
//         {
//             vsol_spatial_object_2d_sptr obj=
//                 new vsol_point_2d((*nit)->pt().x(),
//                                   (*nit)->pt().y());
//             obj->set_id((*nit)->id());
//             contours_spatial_objects_.push_back(obj);


//         }

//     }
 

bool dbskfg_transform_descriptor::degree_1_nodes()
{

    bool flag=false;

    // See whether outer shock nodes has a degree 1 
    // Recheck this later
    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

    for ( nit = outer_shock_nodes_.begin() ; 
          nit != outer_shock_nodes_.end() ; ++nit )
    {
        if ( (*nit)->get_composite_degree() == 1 )
        {
            flag=true;
            break;
        }
    }
 
    return flag;
}

vsol_box_2d_sptr dbskfg_transform_descriptor::bounding_box_context()
{
    vsol_box_2d_sptr bbox(0);
    for (unsigned int s = 0; s < poly_.num_sheets(); ++s)
    { 
        vgl_polygon<double> tempy(poly_[s]);
        vsol_polygon_2d_sptr vsol_poly = bsol_algs::poly_from_vgl(tempy);
        if ( !bbox )
        {
            bbox = vsol_poly->get_bounding_box();
        }
        else
        {
            bbox->grow_minmax_bounds(vsol_poly->get_bounding_box());
        }
    }

    return bbox;
}

vcl_vector<unsigned int> dbskfg_transform_descriptor::contour_ids_affected()
{

    vcl_vector<unsigned int> contours_affected;

    if ( transform_type_ == LOOP )
    {
        // See whether outer shock nodes has a degree 1 
        // Recheck this later
        vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

        vcl_map<unsigned int,vcl_string> contours;
        for ( nit = contours_to_remove_.begin() ; 
              nit != contours_to_remove_.end() ; ++nit )
        {
            dbskfg_contour_node* cnode = dynamic_cast
                <dbskfg_contour_node*>(&(*(*nit)));
            if ( cnode->get_composite_degree() < 3 )
            {
                contours[cnode->contour_id()]="temp";
            }
        }

        vcl_map<unsigned int,vcl_string>::iterator it;
        for ( it = contours.begin() ; it != contours.end() ; ++it)
        {
            contours_affected.push_back((*it).first);
            
        }
    }
    else
    {
        dbskfg_contour_node* node1= dynamic_cast<dbskfg_contour_node*>
            (&(*gap_.first));
        dbskfg_contour_node* node2= dynamic_cast<dbskfg_contour_node*>
            (&(*gap_.second));
        contours_affected.push_back(node1->contour_id());
        contours_affected.push_back(node2->contour_id());


    }

    return contours_affected;

}

void dbskfg_transform_descriptor::remove_extra_shocks()
{
    vcl_vector< vgl_polygon<double> > polygons_to_add;
    vcl_vector<unsigned int> nodes_to_delete;

    vcl_map<vcl_pair<unsigned int,unsigned int>,
        vcl_vector< dbskfg_composite_link_sptr > > shared_links;
    vcl_vector<dbskfg_composite_node_sptr> queue;

    for ( unsigned int k=0; k < this->outer_shock_nodes_.size() ; 
          ++k)
    {
        queue.push_back(this->outer_shock_nodes_[k]);
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
                     this->shock_links_affected_,
                     *srit))
            {
                dbskfg_composite_node_sptr opposite_node 
                    = (*srit)->opposite(node);
                
                if ( (this->poly_.contains(opposite_node->pt().x(),
                                           opposite_node->pt().y()))
                     ||
                     dbskfg_utilities::is_node_in_set_of_nodes(
                         this->outer_shock_nodes_,
                         opposite_node))
                {

                    this->shock_links_affected_.push_back(*srit);
                    nodes_to_delete.push_back(node->id());     
                    
                    if ( opposite_node->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    {

                        if (  dbskfg_utilities::is_node_in_set_of_nodes(
                                  this->outer_shock_nodes_,
                                  opposite_node) &&
                              this->poly_.num_sheets() > 1)
                        {
                            nodes_to_delete.push_back(opposite_node->id());     

                        }
                        else
                        {
                           
                            if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                                     this->outer_shock_nodes_,
                                     opposite_node))
                            {
                                this->outer_shock_nodes_.push_back(
                                    opposite_node);
                                queue.push_back(opposite_node);
                            }
                        }
                    }
         
                    dbskfg_shock_link* slink =
                        dynamic_cast<dbskfg_shock_link*>
                        (&(*(*srit)));
                        
                    polygons_to_add.push_back(slink->polygon());

                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::LL )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            lclinks = slink->left_contour_links();
                        vcl_vector<dbskfg_composite_link_sptr>
                            rclinks = slink->right_contour_links();
                        
                        for ( unsigned int i=0; i<lclinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     lclinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    lclinks[i]);
                            }
                        }                                

                        for ( unsigned int i=0; i<rclinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     rclinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    rclinks[i]);
                            }
                        }
                
                    }

                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::RLLP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = slink->right_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                             if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     clinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    clinks[i]);
                            }
                        }                                

                    }
    
                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::LLRP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = slink->left_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     clinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    clinks[i]);
                            }
                        }                                

                    }

                }
                else
                {
                    if ( opposite_node->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    {

                        vcl_pair<unsigned int,unsigned int> pair
                            =vcl_make_pair(
                                opposite_node->id(),
                                opposite_node->get_composite_degree());
                        shared_links[pair]
                            .push_back(*srit);

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
                     this->shock_links_affected_,
                     *srit))
            {
                dbskfg_composite_node_sptr opposite_node 
                    = (*srit)->opposite(node);
                               
                if ( (this->poly_.contains(opposite_node->pt().x(),
                                           opposite_node->pt().y()))
                     ||
                     dbskfg_utilities::is_node_in_set_of_nodes(
                         this->outer_shock_nodes_,
                         opposite_node))
                {

                    this->shock_links_affected_.push_back(*srit);
                    nodes_to_delete.push_back(node->id());     

                    if ( opposite_node->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    {
                        if (  dbskfg_utilities::is_node_in_set_of_nodes(
                                  this->outer_shock_nodes_,
                                  opposite_node) &&
                              this->poly_.num_sheets() > 1)
                        {
                            nodes_to_delete.push_back(opposite_node->id());

                        }
                        else
                        {
                            if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                                     this->outer_shock_nodes_,
                                     opposite_node))
                            {
                                this->outer_shock_nodes_.push_back(
                                    opposite_node);
                                queue.push_back(opposite_node);
                            }
                        }
                    }
        
                    dbskfg_shock_link* slink =
                        dynamic_cast<dbskfg_shock_link*>
                        (&(*(*srit)));
                        
                    polygons_to_add.push_back(slink->polygon());

                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::LL )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            lclinks = slink->left_contour_links();
                        vcl_vector<dbskfg_composite_link_sptr>
                            rclinks = slink->right_contour_links();
                        
                        for ( unsigned int i=0; i<lclinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     lclinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    lclinks[i]);
                            }
                        }                                

                        for ( unsigned int i=0; i<rclinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     rclinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    rclinks[i]);
                            }
                        }
                
                    }

                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::RLLP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = slink->right_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     clinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    clinks[i]);
                            }
                        }                                

                    }
    
                    if ( slink->shock_compute_type() == 
                         dbskfg_utilities::LLRP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = slink->left_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                            if ( !dbskfg_utilities::is_link_in_set_of_links(
                                     this->contours_affected_,
                                     clinks[i]))
                            {
                                this->contours_affected_.push_back(
                                    clinks[i]);
                            }
                        }                                

                    }

                }
                else
                {
                    if ( opposite_node->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    {

                        vcl_pair<unsigned int,unsigned int> pair
                            =vcl_make_pair(
                                opposite_node->id(),
                                opposite_node->get_composite_degree());
                        shared_links[pair]
                            .push_back(*srit);
                    }
                }
                
            }
        }               
     
        
    }

    vcl_map< vcl_pair<unsigned int,unsigned int>, 
          vcl_vector<dbskfg_composite_link_sptr> >::iterator sit;

    for ( sit = shared_links.begin() ; sit != shared_links.end() ; ++sit)
    {

        vcl_vector<dbskfg_composite_link_sptr> links=(*sit).second;
        if ( links.size() != (*sit).first.second )
        {

            continue;
           
        }


        for ( unsigned int c=0; c < links.size() ; ++c)
        {

            this->shock_links_affected_.push_back(links[c]);
            
            dbskfg_composite_node_sptr shared_node(0);
            if ( links[c]->source()->id() != (*sit).first.first )
            {
                nodes_to_delete.push_back(links[c]->source()->id());
                shared_node=links[c]->target();
            }
            else
            {
                nodes_to_delete.push_back(links[c]->target()->id());
                shared_node=links[c]->source();
            }
         
            if ( !dbskfg_utilities::is_node_in_set_of_nodes(
                             this->shock_nodes_affected_,
                             shared_node))
            {
                this->shock_nodes_affected_.push_back(shared_node);
            }

            dbskfg_shock_link* slink =
                dynamic_cast<dbskfg_shock_link*>
                (&(*(links[c])));
            
            polygons_to_add.push_back(slink->polygon());
            
            if ( slink->shock_compute_type() == 
                 dbskfg_utilities::LL )
            {
                
                vcl_vector<dbskfg_composite_link_sptr>
                    lclinks = slink->left_contour_links();
                vcl_vector<dbskfg_composite_link_sptr>
                    rclinks = slink->right_contour_links();
                
                for ( unsigned int i=0; i<lclinks.size() ; ++i)
                {
                    if ( !dbskfg_utilities::is_link_in_set_of_links(
                             this->contours_affected_,
                             lclinks[i]))
                    {
                        this->contours_affected_.push_back(
                            lclinks[i]);
                    }
                }                                
                
                for ( unsigned int i=0; i<rclinks.size() ; ++i)
                {
                    if ( !dbskfg_utilities::is_link_in_set_of_links(
                             this->contours_affected_,
                             rclinks[i]))
                    {
                        this->contours_affected_.push_back(
                            rclinks[i]);
                    }
                }
            }
            
            if ( slink->shock_compute_type() == 
                 dbskfg_utilities::RLLP )
            {
                
                vcl_vector<dbskfg_composite_link_sptr>
                    clinks = slink->right_contour_links();
                
                for ( unsigned int i=0; i<clinks.size() ; ++i)
                {
                    if ( !dbskfg_utilities::is_link_in_set_of_links(
                             this->contours_affected_,
                             clinks[i]))
                    {
                        this->contours_affected_.push_back(
                            clinks[i]);
                    }
                }                                
                
            }
            
            if ( slink->shock_compute_type() == 
                 dbskfg_utilities::LLRP )
            {
                
                vcl_vector<dbskfg_composite_link_sptr>
                    clinks = slink->left_contour_links();
                
                for ( unsigned int i=0; i<clinks.size() ; ++i)
                {
                    if ( !dbskfg_utilities::is_link_in_set_of_links(
                             this->contours_affected_,
                             clinks[i]))
                    {
                        this->contours_affected_.push_back(
                            clinks[i]);
                    }
                }                                
                
            }

        }
    
    }

    for ( unsigned int i=0; i < polygons_to_add.size() ; ++i)
    {
        int value;

        //Take union of two polygons
        this->poly_ = vgl_clip(
            this->poly_,            // p1
            polygons_to_add[i],     // p2
            vgl_clip_type_union,    // p1 U p2
            &value);                // test if success

            
    }

    for ( unsigned int n=0; n < nodes_to_delete.size(); ++n)
    {

        bool flag=false;
        vcl_vector<dbskfg_composite_node_sptr>::iterator oit;
        for ( oit = this->outer_shock_nodes_.begin() ; oit !=
                  this->outer_shock_nodes_.end() ; ++oit)
        {
            if ( (*oit)->id() == nodes_to_delete[n])
            {

                flag=true;   
                break;
                   
            }

        }

        if ( flag && node_all_shocks_deleted((*oit))) 
        {
            this->shock_nodes_affected_.push_back((*oit));
            this->outer_shock_nodes_.erase(oit);
               
        }
    }

    vcl_vector<unsigned int> cons_to_delete;
    for ( unsigned int c=0; c < this->contours_nodes_affected_.size() ; ++c)
    {


        if ( dbskfg_utilities::is_node_in_set_of_links(
                 this->contours_affected_,
                 this->contours_nodes_affected_[c]))
        {
            cons_to_delete.push_back(c);
        }

    }

    vcl_vector<unsigned int>::reverse_iterator rit;
    for ( rit = cons_to_delete.rbegin() ; rit != cons_to_delete.rend(); ++rit)
    {
        this->contours_nodes_affected_.erase(
            this->contours_nodes_affected_.begin() + *rit);
    }
}

void dbskfg_transform_descriptor::trim_transform()
{

    // Define node/link iteraros for printing
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;
    vcl_map<unsigned int,dbskfg_composite_node_sptr> nodes_to_expand;
    vcl_vector< vgl_polygon<double> > polygons_to_add;
    vcl_vector<unsigned int> outer_shock_nodes_to_delete;

    bool flag=true;

    for ( lit = shock_links_affected_.begin() ; 
          lit != shock_links_affected_.end() ; ++lit )
    {
        dbskfg_composite_node_sptr source = (*lit)->source();
        dbskfg_composite_node_sptr target = (*lit)->target();
        
        if ( dbskfg_utilities::is_node_in_set_of_nodes(
                 this->outer_shock_nodes_,source)&&
             dbskfg_utilities::is_node_in_set_of_nodes(
                 this->outer_shock_nodes_,target)&&
             vgl_distance(source->pt(),target->pt())<0.005)
        {

            nodes_to_expand[source->id()]=source;
            nodes_to_expand[target->id()]=target;

            this->shock_nodes_affected_.push_back(source);
            this->shock_nodes_affected_.push_back(target);

            outer_shock_nodes_to_delete.push_back(source->id());
            outer_shock_nodes_to_delete.push_back(target->id());

        }
    }
  
    if ( nodes_to_expand.size() == 0 )
    {
        return;
    }

    // Finally we have to delete outer shock nodes
    // Erase outer shock nodes and add inner shock nodes
    for ( unsigned int d=0; d < outer_shock_nodes_to_delete.size() ; ++d)
    {
        unsigned int id=outer_shock_nodes_to_delete[d];

        vcl_vector<dbskfg_composite_node_sptr>::iterator it;

        for ( it=this->outer_shock_nodes_.begin(); 
              it !=this->outer_shock_nodes_.end() ; 
              ++it)
        {
            if ((*it)->id() == id)
            {
                break;
            }
        }

        this->outer_shock_nodes_.erase(it);
    }

    bool flag2=true;

    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator it;
    for ( it=nodes_to_expand.begin() ; it != nodes_to_expand.end() ; ++it)
    {
        dbskfg_composite_node_sptr node = (*it).second;
        dbskfg_composite_node::edge_iterator srit;

        // ************************ In Edges ***************************
        for ( srit = node->in_edges_begin() ; srit != node->in_edges_end() 
                  ; ++srit)
        {
            // Test if shock link
            if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
            {
                // Make sure shock link we havent seen
                if ( !dbskfg_utilities::is_link_in_set_of_links(
                         this->shock_links_affected_,
                         *srit))
                {
                    this->shock_links_affected_.push_back(*srit);
                    
                    dbskfg_shock_link* slink = dynamic_cast
                        <dbskfg_shock_link*>(&(*(*srit)));

                    trim_transform_helper(slink);

                    polygons_to_add.push_back(slink->polygon());

                    if ( (*srit)->opposite(node)->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    { 
                        this->outer_shock_nodes_.push_back(
                            (*srit)->opposite(node));
                    }

                    flag2=false;
                    break;
                }

                
            }
        }

        if ( flag2 )
        {
            for ( srit = node->out_edges_begin() ; 
                  srit != node->out_edges_end() ; ++srit)
            {
                // Test if shock link
                if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
                {
                    // Make sure shock link we havent seen
                    if ( !dbskfg_utilities::is_link_in_set_of_links(
                             this->shock_links_affected_,
                             *srit))
                    {
                        this->shock_links_affected_.push_back(*srit);

                    }

                    dbskfg_shock_link* slink = dynamic_cast
                        <dbskfg_shock_link*>(&(*(*srit)));

                    trim_transform_helper(slink);

                    polygons_to_add.push_back(slink->polygon());


                    if ( (*srit)->opposite(node)->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE )
                    { 
                        this->outer_shock_nodes_.push_back(
                            (*srit)->opposite(node));
                    }
                }

            }
        }

    }

    for ( unsigned int i=0; i < polygons_to_add.size() ; ++i)
    {
        int value;

        //Take union of two polygons
        this->poly_ = vgl_clip(
            this->poly_,            // p1
            polygons_to_add[i],     // p2
            vgl_clip_type_union,    // p1 U p2
            &value);                // test if success

            
    }

    
}

void dbskfg_transform_descriptor::trim_transform_helper(
    dbskfg_shock_link* shock_link)
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
                     this->contours_affected_,
                     clinks[i]))
            {
                this->contours_affected_.push_back(clinks[i]);
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
                     this->contours_affected_,
                     clinks[i]))
            {
                this->contours_affected_.push_back(clinks[i]);
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
                     this->contours_affected_,
                     clinks[i]))
            {
                this->contours_affected_.push_back(clinks[i]);
            }
            
        }                                
        
        // Right line
        vcl_vector<dbskfg_composite_link_sptr>
            rclinks = shock_link->right_contour_links();
                     
        for ( unsigned int i=0; i<rclinks.size() ; ++i)
        {
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     this->contours_affected_,
                     rclinks[i]))
            {
                this->contours_affected_.push_back(rclinks[i]);
            }

        }


    }

    // Grab contours left line and right point
    if ( shock_link->shock_compute_type() == 
         dbskfg_utilities::PP )
    {
        dbskfg_contour_node* cnode1 = shock_link->get_left_point();
        dbskfg_contour_node* cnode2 = shock_link->get_right_point();


        if ( !dbskfg_utilities::is_node_in_set_of_links(
                 this->contours_affected_,cnode1) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 this->contours_nodes_affected_,cnode1))
        {
            this->contours_nodes_affected_.push_back(cnode1);
        }

        if ( !dbskfg_utilities::is_node_in_set_of_links(
                 this->contours_affected_,cnode2) &&
             !dbskfg_utilities::is_node_in_set_of_nodes(
                 this->contours_nodes_affected_,cnode2))
        {
            this->contours_nodes_affected_.push_back(cnode2);
        }


    }
}

void dbskfg_transform_descriptor::group_transform(dbskfg_transform_descriptor&
                                                  transform2)
{
    // First determine union union of two polygons
    if ( this->poly_.num_sheets() )
    {
        int value;

        //Take union of two polygons
        vgl_polygon<double> union_poly = vgl_clip(
            poly_,                  // p1
            transform2.poly_,       // p2
            vgl_clip_type_union,    // p1 U p2
            &value);                // test if success


        // Keep largest area polygon
        double area=0;
        unsigned int index=0;

        // for (unsigned int s = 0; s < union_poly.num_sheets(); ++s)
        // { 

        //     vgl_polygon<double> tempy(union_poly[s]);
        //     double area_temp = vgl_area(tempy);
        //     if ( area_temp > area )
        //     {
        //         area = area_temp;
        //         index=s;

        //     }

        // }
    
        poly_.clear();
        poly_ = union_poly;
    }
    else
    {
    
        this->poly_ = transform2.poly_;
    }

    // Keep a map of all nodes similar
    vcl_map<unsigned int,dbskfg_composite_node_sptr> common_nodes;
    vcl_map<unsigned int,dbskfg_composite_link_sptr> common_links;
    vcl_map<unsigned int,dbskfg_composite_node_sptr> common_nodes_to_remove;
    vcl_map<unsigned int,dbskfg_composite_node_sptr> common_inner_shock_nodes;
    vcl_map<unsigned int,dbskfg_composite_node_sptr> common_outer_shock_nodes;
    vcl_map<unsigned int,dbskfg_composite_link_sptr> common_shock_links;

    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator nit;
    vcl_map<unsigned int,dbskfg_composite_link_sptr>::iterator lit;
    vcl_vector< vgl_polygon<double> > polygons_to_add;

    //************************* Contours Affected *****************************
    for ( unsigned int i=0; i < contours_affected_.size(); ++i)
    {
        common_links[contours_affected_[i]->id()]=
            contours_affected_[i];
           
    }

    for ( unsigned int i=0; i < transform2.contours_affected_.size(); ++i)
    {
        common_links[transform2.contours_affected_[i]->id()]=
            transform2.contours_affected_[i];
           
    }
    
    //************************* Contour Nodes *****************************
    for ( unsigned int i=0; i < contours_nodes_affected_.size(); 
          ++i)
    {
        common_nodes[contours_nodes_affected_[i]->id()]=
            contours_nodes_affected_[i];
           
    }

    for ( unsigned int i=0; i < transform2.contours_nodes_affected_.size(); 
          ++i)
    {
        common_nodes[transform2.contours_nodes_affected_[i]->id()]=
            transform2.contours_nodes_affected_[i];
           
    }

    //************************* Contour Removes *****************************
    for ( unsigned int i=0; i < contours_to_remove_.size(); 
          ++i)
    {
        common_nodes_to_remove[contours_to_remove_[i]->id()]=
            contours_to_remove_[i];
           
    }

    for ( unsigned int i=0; i < transform2.contours_to_remove_.size(); 
          ++i)
    {
        common_nodes_to_remove[transform2.contours_to_remove_[i]->id()]=
            transform2.contours_to_remove_[i];
           
    }

    //************************* Inner Shock nodes **************************
    for ( unsigned int i=0; i < shock_nodes_affected_.size(); ++i)
    {
        common_inner_shock_nodes[shock_nodes_affected_[i]->id()]=
            shock_nodes_affected_[i];
           
    }

    for ( unsigned int i=0; i < transform2.shock_nodes_affected_.size(); ++i)
    {
        common_inner_shock_nodes[transform2.shock_nodes_affected_[i]->id()]=
            transform2.shock_nodes_affected_[i];
           
    }


    //************************* Outer Shock nodes **************************
    for ( unsigned int i=0; i < outer_shock_nodes_.size(); ++i)
    {
        common_outer_shock_nodes[outer_shock_nodes_[i]->id()]=
            outer_shock_nodes_[i];
           
    }

    for ( unsigned int i=0; i < transform2.outer_shock_nodes_.size(); ++i)
    {
        common_outer_shock_nodes[transform2.outer_shock_nodes_[i]->id()]=
            transform2.outer_shock_nodes_[i];
           
    }
    
    //************************* Shock links *****************************
    for ( unsigned int i=0; i < shock_links_affected_.size(); ++i)
    {
        common_shock_links[shock_links_affected_[i]->id()]=
            shock_links_affected_[i];
           
    }

    for ( unsigned int i=0; i < transform2.shock_links_affected_.size(); ++i)
    {
        common_shock_links[transform2.shock_links_affected_[i]->id()]=
            transform2.shock_links_affected_[i];
           
    }

    // Deal with special case of degree 3 or higher counter node that is to be
    // removed
    for ( nit = common_nodes_to_remove.begin() ; 
          nit != common_nodes_to_remove.end() ; ++nit)
    {

        if ( (*nit).second->get_composite_degree() >= 3 )
        {
            
            dbskfg_composite_node_sptr test_node = (*nit).second;
            
            dbskfg_composite_node::edge_iterator eit;
            
            bool flag_delete=true;

            for (eit = test_node->out_edges_begin(); 
                 eit != test_node->out_edges_end() ; ++eit)
            {
                if ( (*eit)->link_type() == 
                     dbskfg_composite_link::CONTOUR_LINK )
                {
                    dbskfg_composite_node_sptr cnode=
                        (*eit)->opposite(test_node);

                    if (  common_nodes_to_remove.count(cnode->id()))
                    {
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

                    dbskfg_composite_node_sptr cnode=
                        (*eit)->opposite(test_node);

                    if (  common_nodes_to_remove.count(cnode->id()))
                    {
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
                // Grab all shocks affected for this node
                dbskfg_contour_node* cnode = 
                    dynamic_cast<dbskfg_contour_node*>(&(*(test_node)));
                
                vcl_map<unsigned int, dbskfg_shock_link*> shock_map;

                // We need to grap everything associated with this node
                vcl_vector<dbskfg_shock_link*> shocks_n = 
                    cnode->shocks_affected();
                for ( unsigned int i(0); i < shocks_n.size() ; ++i)
                {

                    if ( !common_shock_links.count(shocks_n[i]->id()))
                    {
                        shock_map[shocks_n[i]->id()]=shocks_n[i];
                        polygons_to_add.push_back(shocks_n[i]->polygon());
                    }
                    common_shock_links[shocks_n[i]->id()]=shocks_n[i];
                    
                    
                    dbskfg_shock_link* shock_link =
                        shocks_n[i];

                    if ( shock_link->shock_compute_type() == 
                         dbskfg_utilities::RLLP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = shock_link->right_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                            common_links[clinks[i]->id()]=clinks[i];
                        }

                    }
    
                    if ( shock_link->shock_compute_type() == 
                         dbskfg_utilities::LLRP )
                    {

                        vcl_vector<dbskfg_composite_link_sptr>
                            clinks = shock_link->left_contour_links();
                        
                        for ( unsigned int i=0; i<clinks.size() ; ++i)
                        {
                            common_links[clinks[i]->id()]=clinks[i];
                        }                                

                    }
                }

                //***************** Determine Shock Nodes Affected ***********

                // Now lets figure out shocks nodes affected
                // There will be two types of shock nodes affected, those within
                // the local context and those on the border of the 
                // local contex Those on the outside will not have both 
                // source and target within the merged local context
                vcl_map<unsigned int,
                    vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
                    inner_shock_nodes;
                vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit1;

                for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; 
                      ++snit1)
                {
                    // Grab current shock link
                    dbskfg_shock_link* shock_link = (*snit1).second;

                    vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit2;
                    vcl_map<unsigned int,unsigned int> local_map;

                    for ( snit2 = shock_map.begin() ; 
                          snit2 != shock_map.end() ; ++snit2)
                    {
                        dbskfg_composite_node_sptr next_node=
                            shock_link->shared_vertex((*(*snit2).second));

                        if ( next_node && ((*snit2).second->id() 
                                           != shock_link->id()) )
                        {

                            if ( inner_shock_nodes.count(next_node->id())==0 )
                            {
                                // Add in shock node
                                inner_shock_nodes[next_node->id()].first=1;
                                inner_shock_nodes[next_node->id()].second
                                    =next_node;
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

                // Now we need to filter out shock nodes that do not 
                // equal composite degree
                vcl_map<unsigned int,vcl_pair<unsigned int,
                    dbskfg_composite_node_sptr> >
                    ::iterator bit;
    
                for ( bit = inner_shock_nodes.begin() ; 
                      bit != inner_shock_nodes.end() 
                          ; ++bit)
                {
                    if ( (*bit).second.first >= 
                         (*bit).second.second->get_composite_degree() )
                    {
                        common_inner_shock_nodes[(*bit).second.second->id()]
                            = (*bit).second.second;
                    }

                }

                // Outer shock nodes will those that are not in the set of 
                // inner shock nodes
                for ( snit1 = shock_map.begin() ; snit1 != shock_map.end() ; 
                      ++snit1)
                {

                    if ( !common_inner_shock_nodes.count(
                             (*snit1).second->source()->id()) && 
                        (*snit1).second->source()->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE)
                    {
                        common_outer_shock_nodes[
                            (*snit1).second->source()->id()]
                            = (*snit1).second->source();
                    }

                    if ( !common_inner_shock_nodes.count(
                             (*snit1).second->target()->id()) && 
                        (*snit1).second->target()->node_type() == 
                         dbskfg_composite_node::SHOCK_NODE)
                    {
                        common_outer_shock_nodes[
                            (*snit1).second->target()->id()]
                            = (*snit1).second->target();
                    }
                }

            }
        }
    }


    for ( unsigned int i=0; i < polygons_to_add.size() ; ++i)
    {
        int value;

        //Take union of two polygons
        this->poly_ = vgl_clip(
            this->poly_,            // p1
            polygons_to_add[i],     // p2
            vgl_clip_type_union,    // p1 U p2
            &value);                // test if success

            
    }

    // clear out this transform
    contours_affected_.clear();
    contours_nodes_affected_.clear();
    contours_spatial_objects_.clear();
    contours_to_remove_.clear();
    shock_links_affected_.clear();
    shock_nodes_affected_.clear();
    outer_shock_nodes_.clear();
 
    for ( unsigned int k=0; k < transform2.all_gaps_.size() ; ++k)
    {
        all_gaps_.push_back(transform2.all_gaps_[k]);

    }

    for ( lit = common_links.begin() ; lit != common_links.end() ; ++lit)
    {
        // For each link must make sure not in common nodes to remove
        dbskfg_composite_node_sptr source_node = (*lit).second->source();
        dbskfg_composite_node_sptr target_node = (*lit).second->target();
        if (!( common_nodes_to_remove.count(source_node->id()) ||
               common_nodes_to_remove.count(target_node->id())))
        {
            contours_affected_.push_back((*lit).second);
        }
        else
        {
            bool flag1 = common_nodes_to_remove.count(source_node->id());
            bool flag2 = common_nodes_to_remove.count(target_node->id());
            
            if ( source_node->get_composite_degree() > 2 ||
                 target_node->get_composite_degree() > 2 )
            {
                if (!(flag1 == flag2))
                {
                    contours_affected_.push_back((*lit).second);
                }
            }

        }
    }

    for ( nit = common_nodes.begin() ; nit != common_nodes.end() ; ++nit)
    {
        if ( !(common_nodes_to_remove.count((*nit).first)))
        {
            if ( !dbskfg_utilities::is_node_in_set_of_links(
                     this->contours_affected_,(*nit).second))
            {
                bool flag=true;
                for ( unsigned int k=0; k < this->all_gaps_.size() ; ++k)
                {
             
                    if ( (this->all_gaps_[k].first->id() ==
                          (*nit).second->id()) ||
                         (this->all_gaps_[k].second->id() ==
                          (*nit).second->id()) )
                    {
                        flag=false;
                        break;
                    }
                }
                
                if ( flag)
                {
                    contours_nodes_affected_.push_back((*nit).second);
                }
            }
        }
    }

    for ( nit = common_nodes_to_remove.begin() ; 
          nit != common_nodes_to_remove.end() ; ++nit)
    {

        contours_to_remove_.push_back((*nit).second);
    }


    for ( lit = common_shock_links.begin() ; lit != common_shock_links.end() ; 
          ++lit)
    {

        shock_links_affected_.push_back((*lit).second);

    }

    for ( unsigned int t=0; 
          t < transform2.new_contours_spatial_objects_.size(); ++t)
    {

        new_contours_spatial_objects_.push_back(
            transform2.new_contours_spatial_objects_[t]);
    }

    for ( nit = common_outer_shock_nodes.begin(); nit !=
              common_outer_shock_nodes.end(); ++nit )
    {
        if ( !(common_inner_shock_nodes.count((*nit).second->id())))
        {
            // See if all links are in shock links affected
            dbskfg_composite_node::edge_iterator eit;
            unsigned int local_degree(0);
            for ( eit = (*nit).second->in_edges_begin(); 
                  eit != (*nit).second->in_edges_end();
                  ++eit)
            {
                if (common_shock_links.count((*eit)->id()))
                {
                    local_degree++;
                }
            }
           
            for ( eit = (*nit).second->out_edges_begin(); 
                  eit != (*nit).second->out_edges_end();
                  ++eit)
            {
                if (common_shock_links.count((*eit)->id()))
                {
                    local_degree++;
                }
            }

            if ( local_degree != (*nit).second->get_composite_degree())
            {
                outer_shock_nodes_.push_back((*nit).second);
            }
            else
            {
                common_inner_shock_nodes[(*nit).second->id()]=
                    (*nit).second;
            }
        }
    }


    for ( nit = common_inner_shock_nodes.begin(); nit !=
              common_inner_shock_nodes.end(); ++nit )
    {
        shock_nodes_affected_.push_back((*nit).second);

    }

}


void dbskfg_transform_descriptor::group_cnodes_affected(
    dbskfg_transform_descriptor& transform2)
{

    for ( unsigned int k=0; k < transform2.all_gaps_.size() ; ++k)
    {
        this->all_gaps_.push_back(transform2.all_gaps_[k]);

    }

    // Keep a map of all nodes similar
    vcl_map<unsigned int,dbskfg_composite_node_sptr> common_nodes;
    
    //************************* Contour Nodes *****************************
    for ( unsigned int i=0; i < contours_nodes_affected_.size(); 
          ++i)
    {
        common_nodes[contours_nodes_affected_[i]->id()]=
            contours_nodes_affected_[i];
           
    }

    for ( unsigned int i=0; i < transform2.contours_nodes_affected_.size(); 
          ++i)
    {
        common_nodes[transform2.contours_nodes_affected_[i]->id()]=
            transform2.contours_nodes_affected_[i];
           
    }

 
    // clear out this transform
    contours_nodes_affected_.clear();
    vcl_map<unsigned int,dbskfg_composite_node_sptr>::iterator nit;

    for ( nit = common_nodes.begin() ; nit != common_nodes.end() ; ++nit)
    {
        if ( !(dbskfg_utilities::is_node_in_set_of_nodes(
                   this->contours_to_remove_,(*nit).second)))
        {
            if ( !dbskfg_utilities::is_node_in_set_of_links(
                     this->contours_affected_,(*nit).second))
            {

                bool flag=true;
                for ( unsigned int k=0; k < this->all_gaps_.size() ; ++k)
                {
                    if ( (all_gaps_[k].first->id() ==
                          (*nit).second->id()) ||
                         (all_gaps_[k].second->id() ==
                          (*nit).second->id()) )
                    {
                        flag=false;
                        break;
                    }
                }
                
                if ( flag)
                {
                    contours_nodes_affected_.push_back((*nit).second);
                }
            }
        }
    }
  
}

vcl_pair<vcl_string,vcl_string> dbskfg_transform_descriptor::gap_string()
{
    vcl_stringstream sstream1;
    sstream1<<gap_.first->pt()<<gap_.second->pt();
    
    vcl_stringstream sstream2;
    sstream2<<gap_.second->pt()<<gap_.first->pt();

    vcl_pair<vcl_string,vcl_string> points = vcl_make_pair
        (sstream1.str(),sstream2.str());
    return points;


}

vcl_pair<vcl_string,vcl_string> dbskfg_transform_descriptor::gap_endpoints()
{
    vcl_stringstream sstream1;
    sstream1<<gap_.first->pt();
    
    vcl_stringstream sstream2;
    sstream2<<gap_.second->pt();

    vcl_pair<vcl_string,vcl_string> points = vcl_make_pair
        (sstream1.str(),sstream2.str());
    return points;


}


void dbskfg_transform_descriptor::print(vcl_ostream& os)
{

    // Define node/link iteraros for printing
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;
    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

    os<<"Transform id: "<<id_<<vcl_endl;
    os<<"Transform cost: "<<cost_<<vcl_endl;
    os<<"Transform contour cost: "<<contour_cost_<<vcl_endl;
    os<<"Transform app cost: "<<app_cost_<<vcl_endl;

    os<<"Contours Affected: ";
    for ( lit = contours_affected_.begin() ; 
          lit != contours_affected_.end() ; ++lit )
    {
        os<<(*lit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Contour Nodes Affected: ";
    for ( nit = contours_nodes_affected_.begin() ; 
          nit != contours_nodes_affected_.end() ; ++nit )
    {
        os<<(*nit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Shocks Affected: ";
    for ( lit = shock_links_affected_.begin() ; 
          lit != shock_links_affected_.end() ; ++lit )
    {
        os<<(*lit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Shock Nodes (Inner) Affected: ";
    for ( nit = shock_nodes_affected_.begin() ; 
          nit != shock_nodes_affected_.end() ; ++nit )
    {
        os<<(*nit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Shock Nodes (Outer) Affected: ";
    for ( nit = outer_shock_nodes_.begin() ; 
          nit != outer_shock_nodes_.end() ; ++nit )
    {
        os<<(*nit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Contours to Remove: ";
    unsigned int degree=0;

    for ( nit = contours_to_remove_.begin() ; 
          nit != contours_to_remove_.end() ; ++nit )
    {
        if ( (*nit)->get_composite_degree() > 2 )
        {
            degree=(*nit)->get_composite_degree();
        }
        os<<(*nit)->id()<<" ";
    }
    os<<vcl_endl;

    os<<"Contours Links to Remove: ";

    for ( lit = contour_links_to_remove_.begin() ; 
          lit != contour_links_to_remove_.end() ; ++lit )
    {       
        os<<(*lit)->id()<<" ";
    }
    os<<vcl_endl;

    if ( degree )
    {
        os <<"Transform is a loop with T junction"<<vcl_endl;
    }

    os<<"Transform Type: "<<transform_type_<<vcl_endl;

    vcl_vector<unsigned int> cids = contour_ids_affected();
    os<<"Contour Ids Affected: ";
    for ( unsigned int c=0; c < cids.size() ; ++c)
    {
        os<<cids[c]<<" ";
    }
    os<<vcl_endl;

    os<<"Contour Spatial Ojbects Ids: ";
    for ( unsigned int k=0; k < contours_spatial_objects_.size(); ++k)
    {

        os<<contours_spatial_objects_[k]->get_id()<<" ";
    }
    os<<vcl_endl;

    os<<"All Gaps: "<<vcl_endl;
    for ( unsigned int k=0; k < all_gaps_.size() ; ++k)
    {
        os<<"Gap "<<k<<" : "<<all_gaps_[k].first->pt()
          <<all_gaps_[k].second->pt()<<vcl_endl;

    }
    
    poly_.print(os);
    os<<vcl_endl;
}


void dbskfg_transform_descriptor::create_copy(
dbskfg_composite_graph_sptr& cgraph)
{

    // Define node/link iteraros for printing
    vcl_vector<dbskfg_composite_link_sptr>::iterator lit;
    vcl_vector<dbskfg_composite_node_sptr>::iterator nit;

    // shock links affected
    for ( lit = shock_links_affected_.begin() ; 
          lit != shock_links_affected_.end() ; ++lit )
    {
        dbskfg_composite_link_sptr new_link = cgraph->find_link((*lit)->id());
        (*lit) = new_link;
    }

    // shock nodes affected
    for ( nit = shock_nodes_affected_.begin() ; 
          nit != shock_nodes_affected_.end() ; ++nit )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node((*nit)->id());
        (*nit) = new_node;

    }
    
    // contours affected
    for ( lit = contours_affected_.begin() ; 
          lit != contours_affected_.end() ; ++lit )
    {
        dbskfg_composite_link_sptr new_link = cgraph->find_link((*lit)->id());
        (*lit) = new_link;

    }
   
    // contour nodes affected
    for ( nit = contours_nodes_affected_.begin() ; 
          nit != contours_nodes_affected_.end() ; ++nit )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node((*nit)->id());
        (*nit) = new_node;
    }

    // contours to remove
    for ( nit = contours_to_remove_.begin() ; 
          nit != contours_to_remove_.end() ; ++nit )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node((*nit)->id());
        (*nit) = new_node;
    }


    // outer shock nodes
    for ( nit = outer_shock_nodes_.begin() ; 
          nit != outer_shock_nodes_.end() ; ++nit )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node((*nit)->id());
        (*nit) = new_node;
    }

    // copy gaps if 
    
    if ( gap_.first )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node(
            gap_.first->id());
        gap_.first = new_node;
    }

    if ( gap_.second )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node(
            gap_.second->id());
        gap_.second = new_node;

    }

    // loop endpoints
    for ( nit = loop_endpoints_.begin() ; 
          nit != loop_endpoints_.end() ; ++nit )
    {
        dbskfg_composite_node_sptr new_node = cgraph->find_node((*nit)->id());
        (*nit)=new_node;
    }
    
    // Keep track of all compounded gaps
    vcl_vector< vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr> 
        >::iterator sit;
    for ( sit = all_gaps_.begin(); sit != all_gaps_.end(); ++sit)
    {
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr>& gap
            = (*sit);

        dbskfg_composite_node_sptr new_node = cgraph->find_node(
            gap.first->id());
        gap.first = new_node;
     
        dbskfg_composite_node_sptr new_node2 = cgraph->find_node(
            gap.second->id());
        gap.second = new_node2;
    
    }
}

void dbskfg_transform_descriptor::write_transform(
    const bxml_data_sptr& root_xml,
    vcl_string filename,
    bool flag)
{
    // Create root element
    bxml_document  doc;
    bxml_element*  root_elm = dbxml_algos::
        cast_to_element(root_xml,"transform_training");
    doc.set_root_element(root_elm);
 
    if ( root_elm->num_data() == 0 )
    {
        root_elm->append_text("\n   ");
    }
    else
    {
        root_elm->append_text("   ");  
    }
    // Create transform set for basic transform information
    bxml_data_sptr transform     = new bxml_element("transform");
    bxml_element*  transform_elm = dbxml_algos::cast_to_element(transform,
                                                                "transform");
    root_elm->append_data(transform);
    root_elm->append_text("\n");
    transform_elm->append_text("\n      ");

    transform_elm->set_attribute("id",id_);

    if ( this->transform_type_ == 0 )
    {
        transform_elm->set_attribute("type","gap");
    }
    else
    {
        transform_elm->set_attribute("type","loop");
    }

    if ( flag )
    {
        transform_elm->set_attribute("class","1");
    }
    else
    {
        transform_elm->set_attribute("class","0");
    }

    // Create basic euler spiral information
    bxml_data_sptr euler_spiral     = new bxml_element("Euler_Spiral_Features");
    bxml_element*  euler_spiral_elm = dbxml_algos::cast_to_element(
        euler_spiral,
        "Euler_Spiral_Features");
    
    // Add points and lines
    transform_elm->append_data(euler_spiral);  
    transform_elm->append_text("\n      ");

    vcl_stringstream gamma_stream,k0_stream,length_stream,d_stream;
    vcl_stringstream theta1_stream,theta2_stream;
    gamma_stream<<vcl_fixed; k0_stream<<vcl_fixed; length_stream<<vcl_fixed;
    d_stream<<vcl_fixed; theta1_stream<<vcl_fixed; theta2_stream<<vcl_fixed;
    gamma_stream.precision(10); k0_stream.precision(10);
    length_stream.precision(10);d_stream.precision(10);
    theta1_stream.precision(10); theta2_stream.precision(10);
    
    gamma_stream  << this->gamma_;
    k0_stream     << this->k0_;
    length_stream << this->length_;
    d_stream      << this->d_;
    theta1_stream << this->theta1_;
    theta2_stream << this->theta2_;

    euler_spiral_elm->set_attribute("k0",k0_stream.str());
    euler_spiral_elm->set_attribute("gamma",gamma_stream.str());
    euler_spiral_elm->set_attribute("length",length_stream.str());
    euler_spiral_elm->set_attribute("d",d_stream.str());
    euler_spiral_elm->set_attribute("theta1",theta1_stream.str());
    euler_spiral_elm->set_attribute("theta2",theta2_stream.str());


    // Get euler spiral points
    vcl_vector<vgl_point_2d<double> > points;
    dbskfg_utilities::ess_points(this,points);

    // Create basic appearance information
    bxml_data_sptr curve     = new bxml_element("Euler_Spiral_Curve");
    bxml_element*  curve_elm = dbxml_algos::cast_to_element(
        curve,
        "Euler_Spiral_Curve");


    // Add points and lines
    transform_elm->append_data(curve);
    transform_elm->append_text("\n      ");


    unsigned int size=0;
    for (  unsigned int p=0; p < points.size(); ++p)
    {
        bxml_data_sptr cnode = new bxml_element("contour_node");
        bxml_element *cnode_elm = dbxml_algos::cast_to_element(
            cnode,"contour_node");
        curve_elm->append_data(cnode_elm);

        vgl_point_2d<double> point = points[p];

        // Grad x,y points
        vcl_stringstream xstream,ystream;
        xstream<<vcl_fixed; ystream<<vcl_fixed;
        xstream.precision(10); ystream.precision(10);
        xstream<<point.x();  ystream<<point.y();

        cnode_elm->set_attribute("x",xstream.str());
        cnode_elm->set_attribute("y",ystream.str());

        if ( size == points.size() - 1 )
        {
            curve_elm->append_text("\n      ");
        }
        else
        {
            curve_elm->append_text("\n        ");
            
        }
        size++;
    }

    // Create basic appearance information
    bxml_data_sptr app_features     = new bxml_element("Image_Features");
    bxml_element*  app_features_elm = dbxml_algos::cast_to_element(
        app_features,
        "Image_Features");
    
    // Add points and lines
    transform_elm->append_data(app_features);  
    transform_elm->append_text("\n   ");

    vcl_stringstream contrast_stream;
    contrast_stream<<vcl_fixed; 
    contrast_stream.precision(10); 
    contrast_stream<<this->app_cost_;

    app_features_elm->set_attribute("contrast",contrast_stream.str());
    app_features_elm->set_attribute("continuity","2.0");

    bxml_write(filename,doc);
}

bool dbskfg_transform_descriptor::node_all_shocks_deleted(
    dbskfg_composite_node_sptr& node)
{
    dbskfg_composite_node::edge_iterator srit;
       
    bool flag=true;

    // ************************ In Edges ***************************
    for ( srit = node->in_edges_begin() ; 
          srit != node->in_edges_end() 
              ; ++srit)
    {
           
        // Make sure shock link we havent seen
        if ( !dbskfg_utilities::is_link_in_set_of_links(
                 this->shock_links_affected_,
                 *srit))
        {
            flag=false;
            break;
        }
    }

    if ( flag )
    {
        for ( srit = node->out_edges_begin() ; 
              srit != node->out_edges_end() 
                  ; ++srit)
        {
           
            // Make sure shock link we havent seen
            if ( !dbskfg_utilities::is_link_in_set_of_links(
                     this->shock_links_affected_,
                     *srit))
            {
                flag=false;
                break;
            }
        }

    }

    return flag;

}

bool dbskfg_transform_descriptor::endpoint_in_gap(
    dbskfg_composite_node_sptr node)
{

    bool flag=false;
    for ( unsigned int k=0; k < this->all_gaps_.size() ; ++k)
    {
        
        if ( (this->all_gaps_[k].first->id() ==
              node->id()) ||
             (this->all_gaps_[k].second->id() ==
              node->id()) )
        {
            flag=true;
            break;
        }
    }
    

    return flag;




}

void dbskfg_transform_descriptor::compute_polygon_string()
{
    for (unsigned int s = 0; s < poly_.num_sheets(); ++s)
    { 
        for (unsigned int p = 0; p < poly_[s].size(); ++p)
        {
            vcl_stringstream streamer;
            streamer<<poly_[s][p];
            polygon_string_rep_[streamer.str()]=0;
        }
    }
}
