// This is brcv/shp/dbsk2d/algo/dbsk2d_containment_graph.cxx

//:
// \file

#include <dbsk2d/dbsk2d_containment_graph.h>
#include <dbsk2d/dbsk2d_containment_node.h>
#include <dbsk2d/dbsk2d_transform_manager.h>
#include <dbsk2d/algo/dbsk2d_ishock_grouping_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap4_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>

#include <vcl_sstream.h>
#include <vcl_algorithm.h>

#include <vil/vil_image_resource.h>

#include <bsta/bsta_k_medoid.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>

//: constructor
dbsk2d_containment_graph::dbsk2d_containment_graph
(
    dbsk2d_ishock_graph_sptr ishock_graph,
    double path_threshold,
    unsigned int loop_type,
    bool expand_outside
):ishock_graph_(ishock_graph),
  path_threshold_(path_threshold),
  loop_type_(loop_type),
  next_available_id_(0),
  gap_id_(0),
  expand_outside_(expand_outside)
{
}

//: desctructor
dbsk2d_containment_graph::~dbsk2d_containment_graph()
{
    cgraph_nodes_.clear();
}


//:construct graph
void dbsk2d_containment_graph::construct_graph()
{

    dbsk2d_ishock_grouping_transform grouper(ishock_graph_);
    grouper.grow_regions();

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >
        fragments = grouper.get_outer_shock_nodes();
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >
        frag_edges = grouper.get_region_nodes();
    vcl_map<unsigned int, vcl_vector<dbsk2d_ishock_belm*> > 
        frag_belms = grouper.get_region_belms();
    vcl_map<unsigned int,vcl_set<int> >
        region_belms_ids=grouper.get_region_belms_ids();

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
    for ( it = frag_edges.begin() ; it != frag_edges.end() ; ++it)
    {
        // Grab polygon fragment
        vgl_polygon<double> poly;
        grouper.polygon_fragment((*it).first,poly);
        // double con_ratio= grouper.contour_ratio((*it).first,poly);

        bool closed_region=(fragments[(*it).first].size()==0)?
            true:false;

        double con_ratio= grouper.contour_ratio((*it).first);
        if ( con_ratio >= 0.4 &&
             frag_edges[(*it).first].size() > 1  &&
             (expand_outside_ || grouper.region_within_image((*it).first)))
        {
            // Create  a new root node
            dbsk2d_containment_node_sptr root_node = new 
                dbsk2d_containment_node(0,  // Incoming transform
                                        0,  // Depth
                                        next_available_id()); // Id
            cgraph_nodes_[0].push_back(root_node);

            // Create local map
            vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> > local_map;
        
            vcl_vector<dbsk2d_ishock_node*> nodes=fragments[(*it).first];

            for ( unsigned int j=0; j < nodes.size() ; ++j)
            {
                local_map[0].push_back(nodes[j]);
            }
            expand_node(root_node,local_map);

            vcl_map<int,dbsk2d_ishock_bline*> extra_belms;
            vcl_set<int> key;
            dbsk2d_transform_manager::Instance().get_extra_belms(
                frag_belms[(*it).first],key,extra_belms);
            
            if ( !all_region_belms_.count(key))
            {
                
                // dbsk2d_transform_manager::Instance().write_output_polygon(poly);
                // dbsk2d_transform_manager::Instance().write_output_region
                //     (frag_belms[(*it).first]);
                
                    
                vcl_map<int, dbsk2d_ishock_bline* >::iterator oit;
                for (oit = extra_belms.begin() ; 
                     oit != extra_belms.end() ; ++oit)
                {
                    all_region_belms_[key].push_back((*oit).second);
                }
                
                all_region_polys_[key]=poly;


                if ( closed_region )
                {
                    closed_regions_[key]="temp";
                }

            }

            vcl_vector<dbsk2d_ishock_belm*> root_node_belms=
                frag_belms[(*it).first];
            unsigned int outer_shock_nodes_size =
                fragments[(*it).first].size();

            vcl_vector<dbsk2d_containment_node_sptr> children=
                root_node->get_children();
            for ( unsigned int c=0; c < children.size(); ++c)
            {
                children[c]->set_parent_regions(root_node_belms,
                                                outer_shock_nodes_size);
            } 

            // dbsk2d_ishock_transform temp_trans(ishock_graph_,
            //                                    dbsk2d_ishock_transform::GAP);
            // vcl_vector<vgl_polygon<double> > foobar; 
            // foobar.push_back(poly);
            // vcl_stringstream stream;
            // stream<<"Node_"<<root_node->get_id()<<".ps";
            // temp_trans.write_state(stream.str(),foobar);
        }
    }

    bool error_flag=true;
    unsigned int depth=0;
    while ( stack_.size())
    {                                                           
        dbsk2d_containment_node_sptr node = stack_.top();
       
        // See if all children have been visited
        if ( node->get_visited())
        {
            if ( node->get_depth() == 1 )
            {
                vcl_cout<<"Finished with Node id: "<<node->get_id()<<vcl_endl;
            }
            
            if ( error_flag == false )
            {
                node->get_parent_transform()->recompute_full_shock_graph();
                error_flag=true;
            }
            else
            {
                bool flag=node->execute_transform();
                if ( flag == false)
                {
                    node->get_parent_transform()->recompute_full_shock_graph();
                    error_flag=true;
                }
            }
            stack_.pop();
            node->destroy_transform();
            continue;
        }

        // expanding node set visited to true
        node->set_visited(true);

        // 0. Grab original id
        int orig_id=ishock_graph_->getAvailableID();
        // vcl_stringstream stream;
        // stream<<"Node_"<<node->get_id()<<".ps";
        // node->get_parent_transform()->write_boundary(stream.str());

        // 1. First execute incoming transform
        error_flag = node->execute_transform();
        if ( error_flag == false )
        {
            // vcl_stringstream error_stream;
            // error_stream<<"Node_Error_"<<node->get_id()<<"_"<<
            //     node->get_parent_transform()->get_transform_type()<<".ps";
            // vcl_vector<vgl_polygon<double> > temp;
            // node->get_parent_transform()
            // ->write_state(error_stream.str(),temp);

            vcl_cout<<"Error at Node id: "<<node->get_id()<<vcl_endl;
            continue;
        }

        // 2. Perform region growing and grab new regions
        dbsk2d_ishock_grouping_transform grouper(ishock_graph_);
        grouper.grow_transformed_regions(orig_id);

        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >
            parent_regions = node->get_parent_regions();
        vcl_vector<vgl_polygon<double> > polys;

        vcl_set<unsigned int> rag_matched_nodes;
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >::
            iterator pit;
        for ( pit = parent_regions.begin() ; 
              pit != parent_regions.end() ; ++pit)
        {
            vcl_vector<dbsk2d_ishock_belm*> parent_belms = (*pit).second;
           
            vcl_set<int> transform_belms;
            node->get_parent_transform()->get_belms(transform_belms);

            vcl_map<unsigned int,unsigned int> 
                parent_os_nodes=node->get_parent_regions_outer_shock_nodes();
            int compare=parent_os_nodes[(*pit).first];

            bool flag=false;
            vcl_set<int> frag_belms_contour_ids;
            for ( unsigned int r=0; r < parent_belms.size() ; ++r)
            {
                if ( !transform_belms.count(parent_belms[r]->id()))
                {
                    frag_belms_contour_ids.insert(
                        parent_belms[r]->get_contour_id());
                }
                else
                {
                    if ( !flag )
                    {
                        compare--;
                        flag=true;
                    }
                }

            }

            if ( compare == 0 )
            {
                compare=frag_belms_contour_ids.size();
            }
            else if ( compare == parent_os_nodes[(*pit).first] ||
                      compare == frag_belms_contour_ids.size() )
            {
                compare=frag_belms_contour_ids.size();
            }

            grouper.rag_nodes(parent_belms,
                              frag_belms_contour_ids,
                              node->get_parent_transform(),
                              rag_matched_nodes,
                              compare);
        }
 
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >
            region_outer_nodes = grouper.get_outer_shock_nodes();
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >
            frag_edges = grouper.get_region_nodes();
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >
            frag_belms = grouper.get_region_belms();
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >
            frag_extra_belms = grouper.get_region_belms();

        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >::iterator it;
        for ( it = frag_edges.begin() ; it != frag_edges.end() ; ++it)
        {
            // Grab polygon fragment
            vgl_polygon<double> poly;
            grouper.polygon_fragment((*it).first,poly);

            bool closed_region=(region_outer_nodes[(*it).first].size()==0)?
                true:false;

            bool flag=(expand_outside_)?false:
                !grouper.region_within_image((*it).first);

            if ( flag || 
                 !frag_edges[(*it).first].size() || 
                 !rag_matched_nodes.count((*it).first) )
            {
                region_outer_nodes.erase((*it).first);
                frag_belms.erase((*it).first);
            }
            else
            {

                polys.push_back(poly);
            }
            
            // double contour_ratio=grouper.contour_ratio((*it).first,
            //                                            poly);
            double contour_ratio=grouper.contour_ratio((*it).first);

            if ( contour_ratio >= 0.4 &&
                 rag_matched_nodes.count((*it).first) &&
                 frag_edges[(*it).first].size() &&
                 (expand_outside_ || grouper.region_within_image((*it).first)))
            {
                // vcl_stringstream filename;
                // filename<<"Depth_"<<depth<<"_id_"<<
                //     dbsk2d_transform_manager::Instance().
                //     nextAvailableID()<<".png";
                
                // dbsk2d_transform_manager::Instance().save_image_poly(
                //     poly,filename.str());
                vcl_map<int,dbsk2d_ishock_bline*> extra_belms;
                vcl_set<int> key;
                dbsk2d_transform_manager::Instance().get_extra_belms(
                    frag_extra_belms[(*it).first],key,extra_belms);
            
                if ( !all_region_belms_.count(key))
                {
                    // dbsk2d_transform_manager::Instance().
                    //     write_output_polygon(poly);
                    // dbsk2d_transform_manager::Instance().
                    //     write_output_region(frag_belms[(*it).first]);

                    
                    vcl_map<int, dbsk2d_ishock_bline* >::iterator oit;
                    for (oit = extra_belms.begin() ; 
                         oit != extra_belms.end() ; ++oit)
                    {
                        all_region_belms_[key].push_back((*oit).second);
                    }
                    
                    all_region_polys_[key]=poly;

                    if ( closed_region )
                    {
                        closed_regions_[key]="temp";
                    }
                }
            }
            
        }

        // node->get_parent_transform()->write_state(stream.str(),polys);
        
        // 3. expand node
        if ( region_outer_nodes.size()  )
        {
            expand_node(node,region_outer_nodes);
        }

        vcl_vector<dbsk2d_containment_node_sptr> children=
            node->get_children();
        for ( unsigned int c=0; c < children.size(); ++c)
        {

            vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >::iterator 
                fit;
            for ( fit = frag_belms.begin() ; fit != frag_belms.end() ; ++fit)
            {
                unsigned int outer_shock_nodes_size =
                    region_outer_nodes[(*fit).first].size();
                children[c]->set_parent_regions((*fit).second,
                                                outer_shock_nodes_size);
            }
        } 

        depth++;
    }
    
    cluster_centers_=100;
    // if ( all_region_belms_.size() > cluster_centers_ )
    // {
    //     cluster_fragments();
    // }
    // else
    {
        
        vcl_cout<<"Writing out "<<all_region_belms_.size()
                <<" Fragments"<<vcl_endl;
        vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator mit;
        for ( mit = all_region_belms_.begin() ; mit != all_region_belms_.end();
              ++mit)
        {
            dbsk2d_transform_manager::Instance().write_output_region
                ((*mit).second);
            dbsk2d_transform_manager::Instance().write_output_polygon
                (all_region_polys_[(*mit).first]);
        }
        
    }
}

bool dbsk2d_containment_graph::
is_rag_node_within_image(vgl_polygon<double>& polygon)
{
    bool flag=true; 

    unsigned int ni=dbsk2d_transform_manager::Instance().get_image()->ni();
    unsigned int nj=dbsk2d_transform_manager::Instance().get_image()->nj();

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

//:construct graph
void dbsk2d_containment_graph::expand_node(
    dbsk2d_containment_node_sptr& node,
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >& outer_shock_nodes)
{

    if ( node->get_prob() <= path_threshold_)
    {
        return;
    }

    // Detect transforms from new regions
    vcl_vector<vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >
        gap_pairs;
    vcl_vector<vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >
        gap4_pairs;

    dbsk2d_ishock_gap_detector detector(ishock_graph_);
    detector.detect_all_gaps(outer_shock_nodes,gap_pairs,gap4_pairs);

    // gap current depth
    unsigned int current_depth = node->get_depth()+1;
    dbsk2d_ishock_transform_sptr transform=
        node->get_parent_transform();
    vcl_set<int> belms_key;

    vcl_map<int,dbsk2d_ishock_bpoint*> gap_endpoints;

    if ( transform )
    {       
        belms_key=node->get_key();
    }

    vcl_map<unsigned int,vcl_vector<double> > gap_costs;
  
    // Add new child nodes for gaps
    for ( unsigned int i=0 ; i < gap_pairs.size() ; ++i)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> gap
            = gap_pairs[i];

        belms_key.insert(gap.first->id());
        belms_key.insert(gap.second->id());

        gap_endpoints[gap.first->id()]=gap.first;
        gap_endpoints[gap.second->id()]=gap.second;

        bool flag=find_node_in_cgraph(current_depth,
                                      node,
                                      belms_key);

        if ( !flag )
        {
            // See if this node already exists
            dbsk2d_ishock_transform_sptr trans = new 
                dbsk2d_ishock_gap_transform(ishock_graph_,
                                            gap,
                                            gap_id());
         
            double prob=trans->likelihood();
            gap_costs[gap.first->id()].push_back(prob);
            gap_costs[gap.second->id()].push_back(prob);

            if ( prob < dbsk2d_transform_manager::
                 Instance().get_threshold() )
            {
                trans=0;
                belms_key.erase(gap.first->id());
                belms_key.erase(gap.second->id());
  
                continue;
            }

            dbsk2d_containment_node_sptr child = 
                new dbsk2d_containment_node(trans,
                                            current_depth,
                                            next_available_id());
            child->set_key(belms_key);
            stack_.push(child);
            node->set_child_node(child);
            child->set_prob(node->get_prob()*prob);
            cgraph_nodes_[child->get_depth()].push_back(child);
        }

        
        belms_key.erase(gap.first->id());
        belms_key.erase(gap.second->id());

    }

    // Add new child nodes for gap4s
    for ( unsigned int i=0 ; i < gap4_pairs.size() ; ++i)
    {
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> gap4
            = gap4_pairs[i];

        dbsk2d_ishock_bpoint* anchor_pt = dbsk2d_transform_manager::Instance()
            .closest_point(gap4_pairs[i]);

        belms_key.insert(gap4.first->id());
        belms_key.insert(anchor_pt->id());

        bool flag=find_node_in_cgraph(current_depth,
                                      node,
                                      belms_key);

        if ( !flag )
        {
            // See if this node already exists
            dbsk2d_ishock_transform_sptr trans = new 
                dbsk2d_ishock_gap4_transform(ishock_graph_,
                                             gap4,
                                             gap_id());
         
            double prob=trans->likelihood();
            gap_costs[gap4.first->id()].push_back(prob);

            if ( !trans->valid_transform() || prob < dbsk2d_transform_manager::
                 Instance().get_threshold() )
            {
                trans=0;
                belms_key.erase(gap4.first->id());
                belms_key.erase(anchor_pt->id());
  
                continue;
            }

            dbsk2d_containment_node_sptr child = 
                new dbsk2d_containment_node(trans,
                                            current_depth,
                                            next_available_id());
            child->set_key(belms_key);
            stack_.push(child);
            node->set_child_node(child);
            child->set_prob(node->get_prob()*prob);
            cgraph_nodes_[child->get_depth()].push_back(child);
        }

        
        belms_key.erase(gap4.first->id());
        belms_key.erase(anchor_pt->id());

    }

    vcl_vector<dbsk2d_ishock_bpoint*> endpoints;
    determine_endpoints(outer_shock_nodes,endpoints);

    for ( unsigned int e=0; e < endpoints.size() ; ++e )
    {
        if ( gap_endpoints.count(endpoints[e]->id()))
        {
            gap_endpoints.erase(endpoints[e]->id());
        } 
    }

    vcl_map<int,dbsk2d_ishock_bpoint*>::iterator mit;
    for ( mit = gap_endpoints.begin() ; mit != gap_endpoints.end() ; ++mit)
    {
        endpoints.push_back((*mit).second);
    }

    vcl_set<vcl_pair<int,int> > loop_pairs;

    for ( unsigned int e=0; e < endpoints.size() ; ++e )
    {
        vcl_set<int> local_copy(belms_key);
        
        dbsk2d_ishock_transform_sptr trans_loop1 = new 
            dbsk2d_ishock_loop_transform(ishock_graph_,
                                         endpoints[e]);
        vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>
            contour_pair=trans_loop1->get_contour_pair();

        vcl_pair<int,int> lp1(contour_pair.first->id(),
                             contour_pair.second->id());

        vcl_pair<int,int> lp2(contour_pair.second->id(),
                             contour_pair.first->id());
        
        if ( loop_pairs.count(lp1) || loop_pairs.count(lp2))
        {
            trans_loop1=0;
            continue;
        }
        
        loop_pairs.insert(lp1);
        loop_pairs.insert(lp2); 
        
        trans_loop1->get_belms(local_copy);
        
        double prob=0.0;
        if ( loop_type_ == 0 )
        {
            if ( gap_costs.count(contour_pair.first->id()) && 
                 !gap_costs.count(contour_pair.second->id()) )
            {
                double c1 = *vcl_min_element(
                    gap_costs[contour_pair.first->id()].begin(),
                    gap_costs[contour_pair.first->id()].end());

                prob = 1.0-c1;
            }
            else if ( gap_costs.count(contour_pair.second->id()) &&
                      !gap_costs.count(contour_pair.first->id()))
            {
                double c1 = *vcl_min_element(
                    gap_costs[contour_pair.second->id()].begin(),
                    gap_costs[contour_pair.second->id()].end());
                    
                prob = 1.0-c1;
            }
            else if ( gap_costs.count(contour_pair.first->id()) &&
                      gap_costs.count(contour_pair.second->id()))
            {
                double c1 = *vcl_min_element(
                    gap_costs[contour_pair.first->id()].begin(),
                    gap_costs[contour_pair.first->id()].end());
                double c2 = *vcl_min_element(
                    gap_costs[contour_pair.second->id()].begin(),
                    gap_costs[contour_pair.second->id()].end());

                     
                prob = 1.0-vcl_min(c1,c2);

            }
            else
            {
                prob = 1;
            }
        }
        else
        {
            prob = trans_loop1->likelihood();
        }

        bool flag=find_node_in_cgraph(current_depth,
                                      node,
                                      local_copy);

        if ( !flag && trans_loop1->valid_transform() && 
             prob > dbsk2d_transform_manager::
             Instance().get_threshold()
            )
        {
            dbsk2d_containment_node_sptr child = 
                new dbsk2d_containment_node(trans_loop1,
                                            current_depth,
                                            next_available_id());
            child->set_key(local_copy);
            stack_.push(child);
            node->set_child_node(child);
            child->set_prob(node->get_prob()*prob);
            cgraph_nodes_[child->get_depth()].push_back(child);
            
            
        }
        else
        {
            trans_loop1=0;
        }
    }       

   
}


// node already exists
bool dbsk2d_containment_graph::find_node_in_cgraph(
    unsigned int current_depth,
    dbsk2d_containment_node_sptr& node,
    vcl_set<int>& belms_key)
{

    bool flag=false;

    vcl_vector<dbsk2d_containment_node_sptr> nodes_at_depth;
    if ( cgraph_nodes_.count(current_depth))
    {
        nodes_at_depth = cgraph_nodes_[current_depth];
    }
        
        
    if ( nodes_at_depth.size())
    {
        for ( unsigned int n=0; n < nodes_at_depth.size(); ++n)
        {
            dbsk2d_containment_node_sptr child=
                nodes_at_depth[n];
            if ( child->get_id() != node->get_id() )
            {
                vcl_set<int> prev_key=child->get_key();
                if ( prev_key == belms_key)
                {
                    node->set_child_node(child);
                    flag=true;
                    break;
                }
            }

        }
    }

    return flag;

}


// node already exists
void dbsk2d_containment_graph::determine_endpoints(
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >& region_outer_nodes,
    vcl_vector<dbsk2d_ishock_bpoint*>& endpoints)
{

    vcl_map< int,vcl_string> local_endpoints;
    vcl_map< int,vcl_string> local_contours;

    // Loop thru new regions and determine new transforms
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >::iterator it;

    for ( it = region_outer_nodes.begin() ; it != region_outer_nodes.end(); 
          ++it)
    {
        // Detect transforms
        vcl_vector<dbsk2d_ishock_node*> outer_shock_nodes = (*it).second;
        for ( unsigned int i=0; i < outer_shock_nodes.size() ; ++i)
        {
            ishock_edge_list adj_edges = outer_shock_nodes[i]->adj_edges();
            ishock_edge_list::iterator curS = adj_edges.begin();
            for ( ; curS != adj_edges.end() ; ++curS )
            {
                dbsk2d_ishock_edge* edge = *curS;
                if ( edge->lBElement()->is_a_point() )
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->lBElement();
                   
                    if ( bpoint->is_an_end_point() )
                    {
                        
                        dbsk2d_bnd_vertex* bnd_vertex=bpoint->bnd_vertex();
                        edge_list elist;
                        bnd_vertex->edges(elist);

                        if ( elist.size() == 2 )
                        {
                            elist.erase(elist.begin());
                        }

                        dbsk2d_bnd_edge* edge=(dbsk2d_bnd_edge*)elist[0].ptr();
                        const vcl_list< vtol_topology_object * > * 
                           superiors  = edge->superiors_list();

                        int key1 = bpoint->id();
                        int key2 = (*superiors->begin())->get_id();
                        if ( local_endpoints.count(key1)==0 &&
                             local_contours.count(key2) == 0 )
                        {
                            local_endpoints[key1]="temp";
                            local_contours[key2]="temp";
                            endpoints.push_back(bpoint);
                        }
                        
                         
                    }
                }
                
                if(edge->rBElement()->is_a_point())
                {
                    dbsk2d_ishock_bpoint* bpoint =
                        (dbsk2d_ishock_bpoint*) edge->rBElement();
                    if ( bpoint->is_an_end_point() )
                    {
                        
                        dbsk2d_bnd_vertex* bnd_vertex=bpoint->bnd_vertex();
                        edge_list elist;
                        bnd_vertex->edges(elist);

                        if ( elist.size() == 2 )
                        {
                            elist.erase(elist.begin());
                        }

                        dbsk2d_bnd_edge* edge=(dbsk2d_bnd_edge*)elist[0].ptr();
                        const vcl_list< vtol_topology_object * > * 
                            superiors  = edge->superiors_list();

                        int key1 = bpoint->id();
                        int key2 = (*superiors->begin())->get_id();
                        if ( local_endpoints.count(key1)==0 &&
                             local_contours.count(key2) == 0 )
                        {
                            local_endpoints[key1]="temp";
                            local_contours[key2]="temp";
                            endpoints.push_back(bpoint);
                        }
                         
                    }
                }
            }
        }   
    }

}

void dbsk2d_containment_graph::write_graph(vcl_string filename)
{

    vcl_ofstream output_ascii_file;
    output_ascii_file.open(filename.c_str(),
                          vcl_ios::out | 
                          vcl_ios::app );

    output_ascii_file<<"digraph G {"<<vcl_endl;
    vcl_map<unsigned int,vcl_vector<dbsk2d_containment_node_sptr> > ::iterator 
        it;
    for ( it = cgraph_nodes_.begin() ; it != cgraph_nodes_.end() ; ++it)
    {
        vcl_vector<dbsk2d_containment_node_sptr> nodes=(*it).second;
        for ( unsigned int i=0; i < nodes.size() ; ++i)
        {
            vcl_vector<dbsk2d_containment_node_sptr> children
                = nodes[i]->get_children();

                
            for ( unsigned int j=0; j < children.size() ; ++j)
            { 
                dbsk2d_ishock_transform_sptr transform=
                    children[j]->get_parent_transform();
                dbsk2d_ishock_transform::TransformType ttype=
                    transform->get_transform_type();
                vcl_string prefix;
                if ( ttype == dbsk2d_ishock_transform::GAP)
                {
                    prefix=" [ label=\"G={";
                }
                else if ( ttype == dbsk2d_ishock_transform::GAP4)
                {
                    prefix=" [ label=\"G4={";
                }
                else
                {
                    prefix=" [ label=\"L={";

                }
                vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>
                    pair=transform->get_contour_pair();
                
                vcl_stringstream body;
                body<<pair.first->id()<<","<<pair.second->id();
            
                output_ascii_file<<"\t"<<nodes[i]->get_id()
                                 <<" -> "
                                 <<children[j]->get_id()
                                 <<prefix
                                 <<body.str()
                                 <<"}\" ];"
                                 <<vcl_endl;

            }
           
        }

    }


    output_ascii_file<<"}"<<vcl_endl;
    output_ascii_file.close();

}


void dbsk2d_containment_graph::write_stats(vcl_ofstream& ofstream)
{

    vcl_map<unsigned int,vcl_vector<dbsk2d_containment_node_sptr> > ::iterator 
        it;
    for ( it = cgraph_nodes_.begin() ; it != cgraph_nodes_.end() ; ++it)
    {
        vcl_vector<dbsk2d_containment_node_sptr> nodes=(*it).second;
        ofstream<<"Depth: "
                <<(*it).first
                <<" Node: "
                <<nodes.size()
                <<vcl_endl;
       
        
    }

    ofstream<<vcl_endl;
    ofstream<<"Number of Regions: "<<all_region_belms_.size()<<vcl_endl;
}

void dbsk2d_containment_graph::cluster_fragments()
{
    bsta_k_medoid cluster(all_region_belms_.size());    

    unsigned int i=0;
    unsigned int j=0;
    vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator mit;
    vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator qit;

    for ( mit = all_region_belms_.begin() ; mit != all_region_belms_.end();
          ++mit)
    {
        
        vcl_set<int> model_set=(*mit).first;
        qit=mit;
        ++qit;

        j=i;
        ++j;

        for ( ; qit != all_region_belms_.end() ; ++qit)
        {
            double jaccard_index=0.0;

            vcl_set<int> query_set=(*qit).first;
            vcl_set<int> intersection;
            vcl_set<int> union_set;
            {
                vcl_insert_iterator<vcl_set<int> > 
                    inserter(intersection,intersection.begin());
                
                vcl_set_intersection(model_set.begin(),
                                     model_set.end(),
                                     query_set.begin(),
                                     query_set.end(),
                                     inserter);
            }
            
            if ( intersection.size())
            {
                vcl_insert_iterator<vcl_set<int> > 
                    inserter(union_set,union_set.begin());

                vcl_set_union(model_set.begin(),
                              model_set.end(),
                              query_set.begin(),
                              query_set.end(),
                              inserter);

                jaccard_index=((double) intersection.size())/
                              ((double) union_set.size());
            }
            
            
            double distance=1.0-jaccard_index;
            cluster.insert_distance(i,j,distance);
            ++j;
        }
        ++i;
    }

    cluster.do_clustering(cluster_centers_);

    for ( unsigned int i=0; i < cluster_centers_ ; ++i)
    {
        unsigned int index=cluster.medoid(i);
        vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator ait
            =all_region_belms_.begin();
        vcl_advance(ait,index);
        dbsk2d_transform_manager::Instance().write_output_region
            ((*ait).second);
        dbsk2d_transform_manager::Instance().write_output_polygon
            (all_region_polys_[(*ait).first]);
        
    }

}


void dbsk2d_containment_graph::merge_closed_regions()
{

    vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator mit;
    for ( mit = all_region_belms_.begin() ; mit != all_region_belms_.end();
          ++mit)
    {
        vcl_set<int> closed_region_key;
        vcl_set<int> closed_region_key_orig;
        vgl_polygon<double> poly;

        if ( closed_regions_.count((*mit).first))
        {   

            vcl_vector<dbsk2d_ishock_belm*> belms=(*mit).second;
            closed_region_key_orig=(*mit).first;
            for ( unsigned int i=0; i < belms.size() ; ++i)
            {
                dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belms[i];
                closed_region_key.insert(bline->twinLine()->id());
            }
            poly=all_region_polys_[(*mit).first];
        }
        else
        {
            continue;
        }
        

        bool write_out=false;
        vcl_map<vcl_set<int>,vcl_vector<dbsk2d_ishock_belm*> >::iterator nit;
        for ( nit = all_region_belms_.begin() ; nit != all_region_belms_.end();
              ++nit)
        {

            if ( closed_regions_.count((*nit).first))
            {   
                vcl_set<int> test_region_key=(*nit).first;
                vcl_map<int,int> mapping_twinline;

                vcl_vector<dbsk2d_ishock_belm*> belms=(*nit).second;
                for ( unsigned int i=0; i < belms.size() ; ++i)
                {
                    dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belms[i];
                    mapping_twinline[bline->id()]=bline->twinLine()->id();
                }

                vgl_polygon<double> poly_test=all_region_polys_[(*nit).first];
                if ( test_region_key != closed_region_key_orig)
                {
                    vcl_set<int> intersection;
                    vcl_insert_iterator<vcl_set<int> > 
                        inserter(intersection,intersection.begin());
                    
                    vcl_set_intersection(closed_region_key.begin(),
                                         closed_region_key.end(),
                                         test_region_key.begin(),
                                         test_region_key.end(),
                                         inserter);
                    
                    if ( intersection.size())
                    {
                        //Keep a flag for status
                        int value;
                        
                        //Take union of two polygons
                        poly = vgl_clip(poly,                // p1
                                        poly_test,           // p2
                                        vgl_clip_type_union, // p1 U p2
                                        &value);             // test if success
                        
                        write_out=true;

                        
                        vcl_set<int> difference;
                        vcl_insert_iterator<vcl_set<int> > 
                            insert_diff(difference,difference.begin());
                    
                        vcl_set_difference(test_region_key.begin(),
                                           test_region_key.end(),
                                           closed_region_key.begin(),
                                           closed_region_key.end(),
                                           insert_diff);
                        
                        vcl_set<int>::iterator sit;
                        for ( sit=difference.begin() ; sit != difference.end()
                                  ;++sit)
                        {
                            closed_region_key.insert(mapping_twinline[*sit]);
                        }
                        
                        
                    }
                    

                }
            }

        }
        
        if ( write_out )
        {
            dbsk2d_transform_manager::Instance().write_output_polygon(poly);
        }

    }
    







}
