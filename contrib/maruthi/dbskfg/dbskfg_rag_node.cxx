// This is brcv/shp/dbskfg/dbskfg_rag_node.cxx

//:
// \file

#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_shock_link_boundary.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <bsol/bsol_algs.h>
#include <vcl_numeric.h>

dbskfg_rag_node::dbskfg_rag_node
( 

    unsigned int id

):dbgrl_vertex<dbskfg_rag_link>(),
  id_(id),
  pt_()
{

}

dbskfg_rag_node::~dbskfg_rag_node()
{
    // Set all pointers to null
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
    {
        (*it).second=0;

    }

    //erase map
    shock_links_.clear();
    
    // Set all points to null
    vcl_map<unsigned int,dbskfg_shock_node*>::iterator sit;
    for ( sit=wavefront_.begin() ; sit != wavefront_.end() ; ++sit)
    {
        (*sit).second=0;
    }
    wavefront_.clear();

    // clear nodes_visited
    nodes_visited_.clear();

    for ( sit=degree_1_nodes_.begin(); sit != degree_1_nodes_.end(); ++sit)
    {
        (*sit).second = 0;
    }

    degree_1_nodes_.clear();

}

void dbskfg_rag_node::add_shock_link(dbskfg_shock_link* shock)
{
    
    // Keep an expanded wavefront
    if ( shock_links_.size() == 0 )
    {
        // Add in shock 
        shock_links_[shock->id()]=shock;

        if ( shock->source()->node_type()== 
             dbskfg_composite_node::SHOCK_NODE &&
             shock->source()->get_composite_degree() > 1)
        {
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->source()));
            wavefront_[snode->id()]=snode;
            nodes_visited_[shock->source()->id()]=1;

        }
        else if ( shock->source()->node_type()== 
             dbskfg_composite_node::SHOCK_NODE &&
             shock->source()->get_composite_degree() == 1 )
        {
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->source()));
            degree_1_nodes_[shock->source()->id()]=snode;
        }
 
        if ( shock->target()->node_type()== 
             dbskfg_composite_node::SHOCK_NODE &&
             shock->target()->get_composite_degree() > 1)
        {
      
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->target()));
            wavefront_[snode->id()]=snode;
            nodes_visited_[shock->target()->id()]=1;
        }
        else if ( shock->target()->node_type()== 
                  dbskfg_composite_node::SHOCK_NODE &&
                  shock->target()->get_composite_degree() == 1 )
        {
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->target()));
            degree_1_nodes_[shock->target()->id()]=snode;
        }

    }
    else
    {

        // Add in shock 
        shock_links_[shock->id()]=shock;

        if ( shock->source()->node_type()== 
             dbskfg_composite_node::SHOCK_NODE &&
             shock->source()->get_composite_degree() > 1)
        {
          
            vcl_map<unsigned int,dbskfg_shock_node*>::iterator sit
                = wavefront_.find(shock->source()->id());
            if ( sit != wavefront_.end())
            {

                nodes_visited_[shock->source()->id()]=
                    nodes_visited_[shock->source()->id()]+1;

                if ( nodes_visited_[shock->source()->id()] ==
                     shock->source()->get_composite_degree() )
                {
                    wavefront_[shock->source()->id()]=0;
                    wavefront_.erase(sit);
                    nodes_visited_.erase(shock->source()->id());
                }


            }
            else
            {
                // Add to wavefront
                dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                    (&(*shock->source()));
                wavefront_[snode->id()]=snode;
                nodes_visited_[shock->source()->id()]=1;
            }
        }
        else if ( shock->source()->node_type()== 
                  dbskfg_composite_node::SHOCK_NODE &&
                  shock->source()->get_composite_degree() == 1 )
        {
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->source()));
            degree_1_nodes_[shock->source()->id()]=snode;
        }

        if ( shock->target()->node_type()== 
             dbskfg_composite_node::SHOCK_NODE &&
             shock->target()->get_composite_degree() > 1)
        {
          
            vcl_map<unsigned int,dbskfg_shock_node*>::iterator sit
                = wavefront_.find(shock->target()->id());
            if ( sit != wavefront_.end())
            {

                nodes_visited_[shock->target()->id()]=
                    nodes_visited_[shock->target()->id()]+1;

                if ( nodes_visited_[shock->target()->id()] ==
                     shock->target()->get_composite_degree() )
                {
                    wavefront_[shock->target()->id()]=0;
                    wavefront_.erase(sit);
                    nodes_visited_.erase(shock->target()->id());
                }
            }
            else
            {
                // Add to wavefront
                dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                    (&(*shock->target()));
                wavefront_[snode->id()]=snode;
                nodes_visited_[shock->target()->id()]=1;
            }
        }
        else if ( shock->target()->node_type()== 
                  dbskfg_composite_node::SHOCK_NODE &&
                  shock->target()->get_composite_degree() == 1 )
        {
            // Add to wavefront
            dbskfg_shock_node* snode = dynamic_cast<dbskfg_shock_node*>
                (&(*shock->target()));
            degree_1_nodes_[shock->target()->id()]=snode;
        }

    }
}

bool dbskfg_rag_node::contains_shock(dbskfg_composite_link_sptr test_link)
{
    return (shock_links_.count(test_link->id()) > 0);
}

void dbskfg_rag_node::delete_shock(unsigned int shock_id)
{


    if ( shock_links_[shock_id]->source()->node_type()== 
         dbskfg_composite_node::SHOCK_NODE &&
         shock_links_[shock_id]->source()->get_composite_degree() > 1)
    {
        if (this->part_of_wavefront(shock_links_[shock_id]->source()->id()))
        {
            wavefront_[shock_links_[shock_id]->source()->id()]=0;
            wavefront_.erase(shock_links_[shock_id]->source()->id());
        }
    }

    if ( shock_links_[shock_id]->target()->node_type()== 
         dbskfg_composite_node::SHOCK_NODE &&
         shock_links_[shock_id]->target()->get_composite_degree() > 1)
    {
        if (this->part_of_wavefront(shock_links_[shock_id]->target()->id()))
        {
            wavefront_[shock_links_[shock_id]->target()->id()]=0;
            wavefront_.erase(shock_links_[shock_id]->target()->id());
        }
    }

    shock_links_[shock_id]=0;
    shock_links_.erase(shock_id);
}

void dbskfg_rag_node::update_outer_shock_nodes()
{
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator sit;

    for  (sit = shock_links_.begin(); sit != shock_links_.end(); ++sit)
    {
        dbskfg_composite_node_sptr source = (*sit).second->source();
        dbskfg_composite_node_sptr target = (*sit).second->target();

        if ( source->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            dbskfg_composite_node::edge_iterator srit;
            unsigned int count(0);

            // Look at in edges of this node
            for ( srit = source->in_edges_begin() ; 
                  srit != source->in_edges_end() 
                      ; ++srit)
            {
                if ( this->contains_shock((*srit)))
                {
                    count++;
                }
            }

            // Look at in edges of this node
            for ( srit = source->out_edges_begin() ; 
                  srit != source->out_edges_end() 
                      ; ++srit)
            {
                if ( this->contains_shock((*srit)))
                {
                    count++;
                }
            }

            if ( count != source->get_composite_degree())
            {
                if ( !wavefront_.count(source->id()))
                {
                    wavefront_[source->id()]=dynamic_cast<dbskfg_shock_node*>
                        (&(*source));
                }
            }
        }
        

        if ( target->node_type() == dbskfg_composite_node::SHOCK_NODE )
        {
            dbskfg_composite_node::edge_iterator srit;
            unsigned int count(0);

            // Look at in edges of this node
            for ( srit = target->in_edges_begin() ; 
                  srit != target->in_edges_end() 
                      ; ++srit)
            {
                if ( this->contains_shock((*srit)))
                {
                    count++;
                }
            }

            // Look at in edges of this node
            for ( srit = target->out_edges_begin() ; 
                  srit != target->out_edges_end() 
                      ; ++srit)
            {
                if ( this->contains_shock((*srit)))
                {
                    count++;
                }
            }

            if ( count != target->get_composite_degree())
            {
                if ( !wavefront_.count(target->id()))
                {
                    wavefront_[target->id()]=dynamic_cast<dbskfg_shock_node*>
                        (&(*target));
                }
            }
        }
    }
   


}

void dbskfg_rag_node::prune_wavefront_nodes()
{
    
    vcl_map<unsigned int,dbskfg_shock_node*>::iterator srit;
    vcl_vector<unsigned int> nodes_to_delete;
    for ( srit=wavefront_.begin() ; srit != wavefront_.end() ; ++srit)
    {
        dbskfg_shock_node* shock_node = (*srit).second;

        dbskfg_composite_node::edge_iterator srit;
        unsigned int count(0);

        // Look at in edges of this node
        for ( srit = shock_node->in_edges_begin() ; 
              srit != shock_node->in_edges_end() 
                  ; ++srit)
        {
            if ( this->contains_shock((*srit)))
            {
                count++;
            }
        }

        // Look at in edges of this node
        for ( srit = shock_node->out_edges_begin() ; 
              srit != shock_node->out_edges_end() 
                  ; ++srit)
        {
            if ( this->contains_shock((*srit)))
            {
                count++;
            }
        }
             
        if ( count == shock_node->get_composite_degree())
        {
            nodes_to_delete.push_back(shock_node->id());
        }
    }

    for ( unsigned int k=0; k < nodes_to_delete.size() ; ++k)
    {
        wavefront_[nodes_to_delete[k]]=0;
        wavefront_.erase(nodes_to_delete[k]);
        nodes_visited_.erase(nodes_to_delete[k]);
    }
    
   
}

void dbskfg_rag_node::fragment_boundary(vgl_polygon<double>& poly)
{
    //***************** Compute Polygon ************************
    //Loop over all shock links
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator pit;
    pit=shock_links_.begin();
    
    // Start polygon
    vgl_polygon<double> start_poly = (*pit).second->polygon();
    
    // Advance iterator
    ++pit;

    for ( ; pit != shock_links_.end() ; ++pit)
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
    
     poly.push_back(start_poly[index]);

    
}

// Before we add this shock link need to make sure that it is adjacent to 
// this node, in terms of same contour ids
bool dbskfg_rag_node::is_adjacent(dbskfg_shock_link* slink)
{
    // Get contour ids for this node
    bool flag_cpairs=true;

    // Grab a shock link out of the graph of this rag node
    dbskfg_shock_link* this_slink= (*shock_links_.begin()).second;

    if ( flag_cpairs)
    {
        if ( (this_slink->endpoint_spawned()==true  && 
              slink->endpoint_spawned()==false)
             ||
             (this_slink->endpoint_spawned()==false && 
              slink->endpoint_spawned()==true))
        {

            flag_cpairs=false;
        }
    }

    return flag_cpairs;

    
}

// Before we add this shock link need to make sure that it is adjacent to 
// this node, in terms of same contour ids
bool dbskfg_rag_node::is_adjacent_exclude_endpoints(dbskfg_shock_link* slink)
{

    // Get contour ids for this node
    bool flag_cpairs=true;

    // Grab a shock link out of the graph of this rag node
    dbskfg_shock_link* this_slink= (*shock_links_.begin()).second;

    bool pp_type_this=
        (this_slink->shock_compute_type() == dbskfg_utilities::PP )
        ?true:false;
    bool pp_type_slink=
        (slink->shock_compute_type() == dbskfg_utilities::PP )
        ?true:false;

    if ( pp_type_this )
    {
        if ( this_slink->get_left_point()->get_composite_degree() != 1 ||
             this_slink->get_right_point()->get_composite_degree() != 1 )
        {
            pp_type_this=false;
        }
    }

    if ( pp_type_slink )
    {
        if ( slink->get_left_point()->get_composite_degree() != 1 ||
             slink->get_right_point()->get_composite_degree() != 1 )
        {
            pp_type_slink=false;
        }
    }


    if ( flag_cpairs)
    {

        if ( (this_slink->endpoint_spawned()==true  && pp_type_this==true)
             ||
             (slink->endpoint_spawned()==true && pp_type_slink==true) )
        {
            flag_cpairs=false;
        }
            
    }

    return flag_cpairs;
    
}

//: Returns the ratio of real to non-real contour
double dbskfg_rag_node::contour_ratio()
{
    if ( wavefront_.empty() && degree_1_nodes_.empty() )
    {
        return 1.0;
    }

    // Grab the polygon
    vgl_polygon<double> polygon;
    this->fragment_boundary(polygon);

    // Compute total length of this polygon
    double length = 0.0;

    // Take first sheet
    vgl_point_2d<double> p0(polygon[0][0].x(),polygon[0][0].y());
    for (unsigned int p = 1; p < polygon[0].size(); ++p) 
    {
        vgl_point_2d<double> c0(polygon[0][p].x(),polygon[0][p].y());
        length += vgl_distance(p0,c0);
        p0=c0;
    }

    // Add in last distance
    vgl_point_2d<double> last_point(polygon[0][polygon[0].size()-1].x(),
                                    polygon[0][polygon[0].size()-1].y());
    vgl_point_2d<double> first_point(polygon[0][0].x(),
                                     polygon[0][0].y());
    length += vgl_distance(first_point,last_point);

    // Keep a vector all shock rays
    vcl_vector< vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
        shock_rays = determine_shock_rays(polygon);
    
    // Keep a running sum of distances
    double shock_ray_sum(0);
    
    for ( unsigned int k=0; k <shock_rays.size() ; ++k)
    {
        shock_ray_sum+=vgl_distance(shock_rays[k].first,shock_rays[k].second);
    }

    return 1.0-(shock_ray_sum/length);
}

//: determine all shock rays
vcl_vector< vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
dbskfg_rag_node::determine_shock_rays(vgl_polygon<double>& polygon)
{
    vcl_vector< vcl_pair<vgl_point_2d<double>,vgl_point_2d<double> > >
        shock_rays;

    vcl_map<unsigned int,dbskfg_shock_node*>::iterator srit;
    for ( srit=wavefront_.begin() ; srit != wavefront_.end() ; ++srit)
    {
        bool found=false;
        vgl_point_2d<double> snode = (*srit).second->pt();
        vcl_stringstream outer_shock_node;
        outer_shock_node<<(*srit).second->pt();
        unsigned int index=0;

        for ( unsigned int b=0; b <= polygon[0].size() ; ++b)
        {
            vgl_point_2d<double> c0(polygon[0][b]);        
            vcl_stringstream temp;
            temp<<c0;
  
            if (c0 == snode || outer_shock_node.str() == temp.str())
            {
                index=b;
                found=true;
                break;
            }
        }
        
        if ( found )
        {
            // Take account of two special cases
            if ( index == 0 )
            {
                shock_rays.push_back(vcl_make_pair(polygon[0]
                                                   [polygon[0].size()-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][index+1]));

            }
            else if ( index == polygon[0].size()-1 )
            {
                shock_rays.push_back(vcl_make_pair(polygon[0][index-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][0]));

            }
            else
            {
                shock_rays.push_back(vcl_make_pair(polygon[0][index-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][index+1]));
            }
        }
    }

    for ( srit=degree_1_nodes_.begin() ; srit != degree_1_nodes_.end() ; ++srit)
    {
        bool found=false;
        vgl_point_2d<double> snode = (*srit).second->pt();
        vcl_stringstream outer_shock_node;
        outer_shock_node<<(*srit).second->pt();
        unsigned int index=0;

        for ( unsigned int b=0; b <= polygon[0].size() ; ++b)
        {
            vgl_point_2d<double> c0(polygon[0][b]);        
            vcl_stringstream temp;
            temp<<c0;
  
            if (c0 == snode || outer_shock_node.str() == temp.str())
            {
                index=b;
                found=true;
                break;
            }
        }
        
        if ( found )
        {
            // Take account of two special cases
            if ( index == 0 )
            {
                shock_rays.push_back(vcl_make_pair(polygon[0]
                                                   [polygon[0].size()-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][index+1]));

            }
            else if ( index == polygon[0].size()-1 )
            {
                shock_rays.push_back(vcl_make_pair(polygon[0][index-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][0]));

            }
            else
            {
                shock_rays.push_back(vcl_make_pair(polygon[0][index-1],
                                                   polygon[0][index]));
                shock_rays.push_back(vcl_make_pair(polygon[0][index],
                                                   polygon[0][index+1]));
            }

        }

    }
    
    return shock_rays;

}

bool dbskfg_rag_node::is_rag_node_within_image(int ni,int nj)
{
    bool flag=true;
 
    // Grab the polygon
    vgl_polygon<double> polygon;
    this->fragment_boundary(polygon);

    for (unsigned int s = 0; s < polygon.num_sheets(); ++s)
    { 
        for (unsigned int p = 0; p < polygon[s].size(); ++p)
        { 
            bool xflag = ( polygon[s][p].x() < 0 || polygon[s][p].x() > ni )
                ? false : true;
            bool yflag = ( polygon[s][p].y() < 0 || polygon[s][p].y() > nj )
                ? false : true;

            if (xflag == false || yflag == false )
            {
                flag=false;
                break;
            }
        }
        if (flag==false)
        {
            break;
        }
    }

    return flag;

}

bool dbskfg_rag_node::endpoint_spawned_node()
{
    bool flag=true;
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
    {
        if ( !(*it).second->endpoint_spawned())
        {
            flag=false;
            break;
        }

        
    }

    return flag;

}


bool dbskfg_rag_node::endpoint_spawned_node_exclude_endpoints()
{

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
    {
        dbskfg_shock_link* this_slink=(*it).second;

        bool pp_type_this=
            (this_slink->shock_compute_type() == dbskfg_utilities::PP )
            ?true:false;

        if ( pp_type_this )
        {
            if ( this_slink->get_left_point()->get_composite_degree() == 1 &&
                 this_slink->get_right_point()->get_composite_degree() == 1 )
            {
                return true;
            }
        }

        
    }

    return false;

}

void dbskfg_rag_node::print(vcl_ostream& os)
{
    os<<"Rag Id: "<<id_<<vcl_endl;
    os<<"Shock Links: ";
    // Set all pointers to null
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
    {
        os<<(*it).second->id()<<" ";
    }
    os<<vcl_endl;
    os<<"Wavefront: ";

    vcl_map<unsigned int,dbskfg_shock_node*>::iterator sit;
    for ( sit=wavefront_.begin() ; sit != wavefront_.end() ; ++sit)
    {
        os<<(*sit).second->id()<<" ";

    }


    os<<vcl_endl;

    os<<"Degree 1 nodes: ";


    for ( sit=degree_1_nodes_.begin() ; sit != degree_1_nodes_.end() ; ++sit)
    {
        os<<(*sit).second->id()<<" ";

    }


    os<<vcl_endl;
 
    os<<"Polygon: "<<vcl_endl;
    vgl_polygon<double> temp;
    fragment_boundary(temp);
    temp.print(os);
    os<<"Contour Ratio: "<<contour_ratio()<<vcl_endl;
    os<<vcl_endl;

}

bool dbskfg_rag_node::intersection_bbox(vgl_box_2d<double>& bbox)
{
    vgl_polygon<double> poly;
    fragment_boundary(poly);
    return vgl_intersection(bbox,poly);

}

bool dbskfg_rag_node::node_within_rag(unsigned int id)
{

    bool flag=false;

    //***************** Determine Shock Nodes Affected ************************

    // Now lets figure out shocks nodes affected
    // There will be two types of shock nodes affected, those within
    // the local context and those on the border of the local context
    // Those on the outside will not have both source and target within
    // the merged local context
    vcl_map<unsigned int,vcl_pair<unsigned int,dbskfg_composite_node_sptr> >
        inner_shock_nodes;
    vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit1;

    for ( snit1 = shock_links_.begin() ; 
          snit1 != shock_links_.end() ; ++snit1)
    {
        // Grab current shock link
        dbskfg_shock_link* shock_link = (*snit1).second;

        vcl_map<unsigned int,dbskfg_shock_link*>::iterator snit2;
        for ( snit2 = shock_links_.begin() ; 
              snit2 != shock_links_.end() ; ++snit2)
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
            if ( (*bit).second.second->id() == id )
            {
                flag=true;
            }

        }

    }


    return flag;


}

//: determine contour endpoints
void dbskfg_rag_node::determine_contour_points(
    vcl_vector<dbskfg_composite_node_sptr>&
    con_endpoints)
{

    vcl_map<unsigned int,dbskfg_shock_node*>::iterator mit;
    for ( mit = wavefront_.begin() ; mit != wavefront_.end() ; ++mit)
    {
        dbskfg_shock_node* shock_node = (*mit).second;

        if ( shock_node->get_composite_degree() > 1 )
        {
            dbskfg_composite_node::edge_iterator srit;
            bool flag=false;

            // Look at in edges of this node
            for ( srit = shock_node->in_edges_begin() ; 
                  srit != shock_node->in_edges_end() 
                      ; ++srit)
            {
                dbskfg_shock_link* shock_link =
                    dynamic_cast<dbskfg_shock_link*>
                    (&(*(*srit)));

                if ( shock_link->endpoint_spawned() ) 
                {
                   
                    dbskfg_contour_node* cnode(0);

                    if ( shock_link->get_left_point() )
                    {
                        if ( 
                            shock_link->get_left_point()
                            ->get_composite_degree() == 1 )
                        {
                            
                            dbskfg_contour_node* cnode =
                                shock_link->get_left_point();
                            con_endpoints.push_back(cnode);
                            flag=true;
                          
                        }
                    }

                    if ( shock_link->get_right_point() )
                    {
                        if ( 
                            shock_link->get_right_point()
                            ->get_composite_degree() == 1 )
                        {
                                                        
                            dbskfg_contour_node* cnode =
                                shock_link->get_right_point();
                            con_endpoints.push_back(cnode);
                            flag=true;
                            
       
                        }
                    }
               
                    break;
                }
               
            }
                
            if ( !flag )
            {
        
                for ( srit = shock_node->out_edges_begin() ; 
                      srit != shock_node->out_edges_end() 
                          ; ++srit)
                {
                    
                    dbskfg_shock_link* shock_link =
                        dynamic_cast<dbskfg_shock_link*>
                        (&(*(*srit)));

                    if ( shock_link->endpoint_spawned() ) 
                    {
                   
                        dbskfg_contour_node* cnode(0);

                        if ( shock_link->get_left_point() )
                        {
                            if ( 
                                shock_link->get_left_point()
                                ->get_composite_degree() == 1 )
                            {
                            
                                dbskfg_contour_node* cnode =
                                    shock_link->get_left_point();
                                con_endpoints.push_back(cnode);
                                flag=true;
                          
                            }
                        }

                        if ( shock_link->get_right_point() )
                        {
                            if ( 
                                shock_link->get_right_point()
                                ->get_composite_degree() == 1 )
                            {
                                                        
                                dbskfg_contour_node* cnode =
                                    shock_link->get_right_point();
                                con_endpoints.push_back(cnode);
                                flag=true;
                            
       
                            }
                        }
               
                     
                    }
                }
                
                
            }
           
        }

    }

}


void dbskfg_rag_node::rag_contour_ids(vcl_set<unsigned int>& con_id)
{

    vcl_map<unsigned int,bool> contour_ids;

    vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
    for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
    {
        // For each shock link grab its contours
        // Get contour pair
        vcl_set<unsigned int> link_ids =
            (*it).second->get_contour_pair();

        vcl_set<unsigned int>::iterator sit;
        for ( sit = link_ids.begin() ; sit != link_ids.end() ; ++sit)
        {
            contour_ids[*sit]=true;
        }
    }

    vcl_map<unsigned int,bool>::iterator mit;
    for ( mit = contour_ids.begin() ; mit != contour_ids.end() ; ++mit)
    {
        con_id.insert((*mit).first);

    }


}

bool dbskfg_rag_node::wavefront_exists( 
const vcl_map<unsigned int, vgl_point_2d<double> >& wavefront)
{
    
    vcl_map<unsigned int, vgl_point_2d<double> >::const_iterator wit;
    for (wit = wavefront.begin() ; wit != wavefront.end() ; ++wit)
    {
    
        bool flag=false;
        vcl_map<unsigned int,dbskfg_shock_link*>::iterator it;
        for ( it=shock_links_.begin() ; it != shock_links_.end() ; ++it)
        {
            if ( (*it).second->source()->id() == (*wit).first ||
                 (*it).second->target()->id() == (*wit).first )
            {

                flag = true;
                break;
            }
            
        }

        if ( flag == false)
        {
            return false;
        }
    } 

    return true;
  




}

// Transform affects regions
bool dbskfg_rag_node::transform_local_context(
    dbskfg_transform_descriptor_sptr transform)
{

    vcl_map<unsigned int,dbskfg_shock_node*>::iterator it;
    for ( it = wavefront_.begin() ; it != wavefront_.end() ; ++it)
    {
    
        if ( dbskfg_utilities::
             is_node_in_set_of_nodes(transform->outer_shock_nodes_,
                                     (*it).second,
                                     dbskfg_utilities::POINT) ||
             dbskfg_utilities::
             is_node_in_set_of_nodes(transform->shock_nodes_affected_,
                                     (*it).second,
                                     dbskfg_utilities::POINT) )
        {
            return true;
        }

    }

    return false;

}

// Return a string representing wavefront based on contour end points
void dbskfg_rag_node::wavefront_string(vcl_set<vcl_string>& wavefront)
{

    vcl_vector<dbskfg_composite_node_sptr> con_points;
    this->determine_contour_points(con_points);

    for ( unsigned int i=0; i < con_points.size(); ++i)
    {
        vgl_point_2d<double> point = con_points[i]->pt();
        vcl_stringstream streamer;
        streamer<<point;
        wavefront.insert(streamer.str());
    }

}


