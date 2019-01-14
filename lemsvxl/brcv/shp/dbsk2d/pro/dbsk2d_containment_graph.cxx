// This is brcv/shp/dbsk2d/pro/dbsk2d_containment_graph.cxx

//:
// \file

#include <dbsk2d/pro/dbsk2d_containment_graph.h>
#include <dbsk2d/pro/dbsk2d_containment_node.h>
#include <dbsk2d/pro/dbsk2d_transform_manager.h>
#include <dbsk2d/pro/dbsk2d_ishock_grouping_transform.h>
#include <dbsk2d/pro/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/pro/dbsk2d_ishock_gap4_transform.h>
#include <dbsk2d/pro/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>

#include <sstream>
#include <algorithm>

#include <vil/vil_image_resource.h>

#include <bsta/bsta_k_medoid.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

//: constructor
dbsk2d_containment_graph::dbsk2d_containment_graph
(
    dbsk2d_ishock_graph_sptr ishock_graph,
    double path_threshold,
    unsigned int loop_type,
    bool expand_outside,
    bool train,
    bool debug,
    bool show_shock,
    int quad
):ishock_graph_(ishock_graph),
  path_threshold_(path_threshold),
  loop_type_(loop_type),
  next_available_id_(0),
  gap_id_(0),
  expand_outside_(expand_outside),
  train_(train),
  debug_(debug),
  show_shock_(show_shock),
  quad_(quad)
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

    std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >
        fragments = grouper.get_outer_shock_nodes();
    std::map<unsigned int,std::vector<dbsk2d_ishock_edge*> >
        frag_edges = grouper.get_region_nodes();
    std::map<unsigned int, std::vector<dbsk2d_ishock_belm*> > 
        frag_belms = grouper.get_region_belms();
    std::map<unsigned int,std::set<int> >
        region_belms_ids=grouper.get_region_belms_ids();
    std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
        degree_three_nodes = grouper.get_degree_three_nodes();

    std::map<unsigned int,std::vector<dbsk2d_ishock_edge*> >::iterator it;
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
             grouper.region_within_image((*it).first,quad_))
        {
            // Create  a new root node
            dbsk2d_containment_node_sptr root_node = new 
                dbsk2d_containment_node(0,  // Incoming transform
                                        0,  // Depth
                                        next_available_id()); // Id
            cgraph_nodes_[0].push_back(root_node);

            // Create local map
            std::map<unsigned int,std::vector<dbsk2d_ishock_node*> > local_map;
        
            std::vector<dbsk2d_ishock_node*> nodes=fragments[(*it).first];

            for ( unsigned int j=0; j < nodes.size() ; ++j)
            {
                local_map[0].push_back(nodes[j]);
            }

            std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
                degree_three_links;

            // Find links to consider for degree three
            {
                std::vector<dbsk2d_ishock_belm*> d3_nodes=
                    degree_three_nodes[(*it).first];
                std::set<int> id_set=region_belms_ids[(*it).first];

                for ( unsigned int d=0; d < d3_nodes.size() ; ++d)
                {
                    dbsk2d_ishock_bpoint* node = 
                        dynamic_cast<dbsk2d_ishock_bpoint*>(d3_nodes[d]);

                    belm_list::iterator curB = node->LinkedBElmList.begin();
                    for(; curB!=node->LinkedBElmList.end(); ++curB) 
                    {
                        int id=(*curB)->id();
                        if ( id_set.count(id))
                        {
                            degree_three_links[(*it).first].push_back(*curB);
                        }
                        
                    }
                }
            }

            expand_node(root_node,local_map,degree_three_nodes,
                        degree_three_links);

            std::vector<dbsk2d_ishock_belm*> root_node_belms=
                frag_belms[(*it).first];
            unsigned int outer_shock_nodes_size =
                fragments[(*it).first].size();
            
            std::vector<dbsk2d_containment_node_sptr> children=
                root_node->get_children();
            for ( unsigned int c=0; c < children.size(); ++c)
            {
                children[c]->set_parent_regions(root_node_belms,
                                                outer_shock_nodes_size);
            }


	    if ( debug_ )
	    {
		
		dbsk2d_ishock_transform temp_trans
		    (
		     ishock_graph_,
		     dbsk2d_ishock_transform::GAP);
		
		std::vector<vgl_polygon<double> > foobar; 
		foobar.push_back(poly);
		std::stringstream stream;
		stream<<"Node_"<<root_node->get_id()<<".ps";
		temp_trans.write_state(stream.str(),foobar,
				       show_shock_);
	    }
	    
        }

        if ( con_ratio >= 0.4 &&
             frag_edges[(*it).first].size() > 1  &&
             (expand_outside_
	      || grouper.region_within_image((*it).first,quad_)))  
        {
            std::map<int,dbsk2d_ishock_bline*> extra_belms;
            std::set<int> key;
            std::set<int> closed_region_key;
            dbsk2d_transform_manager::Instance().get_extra_belms(
                frag_belms[(*it).first],key,closed_region_key,extra_belms);
            
            if ( !all_region_belms_.count(key))
            {
                
                // dbsk2d_transform_manager::Instance().write_output_polygon(poly);
                // dbsk2d_transform_manager::Instance().write_output_region
                //     (frag_belms[(*it).first]);
                
                
                if ( train_ )
                {
                    std::vector<dbsk2d_ishock_belm*> belms=
                        frag_belms[(*it).first];
                    for ( unsigned int b=0; b < belms.size() ; ++b)
                    {
                        all_region_belms_[key].push_back(belms[b]);
                    }

                }
                else
                {
                    std::map<int, dbsk2d_ishock_bline* >::iterator oit;
                    for (oit = extra_belms.begin() ; 
                         oit != extra_belms.end() ; ++oit)
                    {
                        all_region_belms_[key].push_back((*oit).second);
                    }
                }
                all_region_polys_[key]=poly;


                if ( closed_region && grouper.region_within_image
                     ((*it).first,quad_))
                {
                    std::vector<dbsk2d_ishock_belm*> belms=
                        frag_belms[(*it).first];
                    for ( unsigned int b=0; b < belms.size() ; ++b)
                    {
                        closed_regions_[closed_region_key].push_back(belms[b]);
                    }
                }

                if ( train_ )
                {
                    // containment graph stats
                    region_stats_[key].push_back(0.0);       //depth
                    region_stats_[key].push_back(1.0);       //path prob
                    region_stats_[key].push_back(1.0);       //region gap cost

                    // get polygon stats
                    std::vector<double> region_stats;
                    grouper.get_region_stats((*it).first,
                                             poly,region_stats);

                    for ( unsigned int p=0; p < region_stats.size() ; ++p)
                    {
                        region_stats_[key].push_back(region_stats[p]);
                    }

                    std::vector<double> app_stats;
                    
                    dbsk2d_transform_manager::Instance().get_appearance_stats
                        (frag_edges[(*it).first],
                         frag_belms[(*it).first],
                         region_stats[0],
                         app_stats);

                    for ( unsigned int a=0; a < app_stats.size() ; ++a)
                    {
                        region_stats_[key].push_back(app_stats[a]);
                    }

                    

                }
                
                

            }
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
                std::cout<<"Finished with Node id: "<<node->get_id()<<std::endl;
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

	    if ( !debug_ )
	    {
		node->destroy_transform();
	    }
	    
            continue;
        }

        // expanding node set visited to true
        node->set_visited(true);

        // 0. Grab original id
        int orig_id=ishock_graph_->getAvailableID();

        // node->get_parent_transform()->write_boundary(stream.str());

        // 1. First execute incoming transform
        error_flag = node->execute_transform();
        if ( error_flag == false )
        {
            // std::stringstream error_stream;
            // error_stream<<"Node_Error_"<<node->get_id()<<"_"<<
            //     node->get_parent_transform()->get_transform_type()<<".ps";
            // std::vector<vgl_polygon<double> > temp;
            // node->get_parent_transform()
            // ->write_state(error_stream.str(),temp);

            std::cout<<"Error at Node id: "<<node->get_id()<<std::endl;
            continue;
        }

        // 2. Perform region growing and grab new regions
        dbsk2d_ishock_grouping_transform grouper(ishock_graph_);
        grouper.grow_transformed_regions(orig_id);

        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
            parent_regions = node->get_parent_regions();
        std::vector<vgl_polygon<double> > polys;

        std::set<unsigned int> rag_matched_nodes;
        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >::
            iterator pit;
        for ( pit = parent_regions.begin() ; 
              pit != parent_regions.end() ; ++pit)
        {
            std::vector<dbsk2d_ishock_belm*> parent_belms = (*pit).second;
           
            std::set<int> transform_belms;
            node->get_parent_transform()->get_belms(transform_belms);

            std::map<unsigned int,unsigned int> 
                parent_os_nodes=node->get_parent_regions_outer_shock_nodes();
            int compare=parent_os_nodes[(*pit).first];

            bool flag=false;
            std::set<int> frag_belms_contour_ids;
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

        std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >
            region_outer_nodes = grouper.get_outer_shock_nodes();
        std::map<unsigned int,std::vector<dbsk2d_ishock_edge*> >
            frag_edges = grouper.get_region_nodes();
        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
            frag_belms = grouper.get_region_belms();
        std::map<unsigned int,std::set<int> >
            frag_belms_ids=grouper.get_region_belms_ids();
        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
            frag_extra_belms = grouper.get_region_belms();
        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
            degree_three_nodes = grouper.get_degree_three_nodes();
        std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >
            degree_three_links;
       
        std::map<unsigned int,std::vector<dbsk2d_ishock_edge*> >::iterator it;
        for ( it = frag_edges.begin() ; it != frag_edges.end() ; ++it)
        {
            // Grab polygon fragment
            vgl_polygon<double> poly;
            grouper.polygon_fragment((*it).first,poly);

            bool closed_region=(region_outer_nodes[(*it).first].size()==0)?
                true:false;

            if ( !grouper.region_within_image((*it).first,quad_) || 
                 !frag_edges[(*it).first].size() || 
                 !rag_matched_nodes.count((*it).first) )
            {
                region_outer_nodes.erase((*it).first);
                frag_belms.erase((*it).first);
                degree_three_nodes.erase((*it).first);
            }
            else
            {
                polys.push_back(poly);

                
                // Find links to consider for degree three
                std::vector<dbsk2d_ishock_belm*> d3_nodes=
                    degree_three_nodes[(*it).first];
                std::set<int> id_set=frag_belms_ids[(*it).first];

                for ( unsigned int d=0; d < d3_nodes.size() ; ++d)
                {
                    dbsk2d_ishock_bpoint* node = 
                        dynamic_cast<dbsk2d_ishock_bpoint*>(d3_nodes[d]);
                    
                    belm_list::iterator curB = node->LinkedBElmList.begin();
                    for(; curB!=node->LinkedBElmList.end(); ++curB) 
                    {
                        int id=(*curB)->id();
                        if ( id_set.count(id))
                        {
                            degree_three_links[(*it).first].push_back(*curB);
                        }
                        
                    }
                }
            }
            
            // double contour_ratio=grouper.contour_ratio((*it).first,
            //                                            poly);
            double contour_ratio=grouper.contour_ratio((*it).first);

            if ( contour_ratio >= 0.4 &&
                 rag_matched_nodes.count((*it).first) &&
                 frag_edges[(*it).first].size() &&
                 (expand_outside_ || grouper.region_within_image((*it).first
                                                                 ,quad_)))
            {
                // std::stringstream filename;
                // filename<<"Depth_"<<depth<<"_id_"<<
                //     dbsk2d_transform_manager::Instance().
                //     nextAvailableID()<<".png";
                
                // dbsk2d_transform_manager::Instance().save_image_poly(
                //     poly,filename.str());
                std::map<int,dbsk2d_ishock_bline*> extra_belms;
                std::set<int> key;
                std::set<int> closed_region_key;
                dbsk2d_transform_manager::Instance().get_extra_belms(
                    frag_extra_belms[(*it).first],key,closed_region_key,
                    extra_belms);
            
                if ( !all_region_belms_.count(key))
                {
                    // dbsk2d_transform_manager::Instance().
                    //     write_output_polygon(poly);
                    // dbsk2d_transform_manager::Instance().
                    //     write_output_region(frag_belms[(*it).first]);

                    
                    if ( train_ )
                    {
                        std::vector<dbsk2d_ishock_belm*> belms=
                            frag_belms[(*it).first];
                        for ( unsigned int b=0; b < belms.size() ; ++b)
                        {
                            all_region_belms_[key].push_back(belms[b]);
                        }

                    }
                    else
                    {
                        std::map<int, dbsk2d_ishock_bline* >::iterator oit;
                        for (oit = extra_belms.begin() ; 
                             oit != extra_belms.end() ; ++oit)
                        {
                            all_region_belms_[key].push_back((*oit).second);
                        }
                    }
   
                    all_region_polys_[key]=poly;

                    if ( closed_region && grouper.region_within_image
                         ((*it).first,quad_))
                    {

                        std::vector<dbsk2d_ishock_belm*> belms=
                            frag_belms[(*it).first];
                        for ( unsigned int b=0; b < belms.size() ; ++b)
                        {
                            closed_regions_[closed_region_key].push_back(
                                belms[b]);
                        }
                    }

                    if ( train_ )
                    {

                        // stats
                        region_stats_[key].push_back(node->get_depth());
                        region_stats_[key].push_back(node->get_prob()); 
                        region_stats_[key].push_back(node->get_gap_prob());
                        
                        // get polygon stats
                        std::vector<double> region_stats;
                        grouper.get_region_stats((*it).first,
                                                 poly,region_stats);
                        
                        for ( unsigned int p=0; p < region_stats.size() ; ++p)
                        {
                            region_stats_[key].push_back(region_stats[p]);
                        }
                        
                        std::vector<double> app_stats;
                    
                        dbsk2d_transform_manager::Instance().
                            get_appearance_stats
                            (frag_edges[(*it).first],
                             frag_belms[(*it).first],
                             region_stats[0],
                             app_stats);

                        for ( unsigned int a=0; a < app_stats.size() ; ++a)
                        {
                            region_stats_[key].push_back(app_stats[a]);
                        }

                    }

                }
            }
            
        }

	if ( debug_ )
	{
	    std::stringstream stream;
	    stream<<"Node_"<<node->get_id()<<".ps";
	    node->get_parent_transform()->write_state(stream.str(),polys,
						      show_shock_);
        }

        // 3. expand node
        if ( region_outer_nodes.size()  )
        {
            expand_node(node,region_outer_nodes,degree_three_nodes,
                degree_three_links);
        }

        std::vector<dbsk2d_containment_node_sptr> children=
            node->get_children();

        for ( unsigned int c=0; c < children.size(); ++c)
        {

            std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >::iterator 
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
        
        std::cout<<"Writing out "<<all_region_belms_.size()
                <<" Fragments"<<std::endl;

        if ( train_ )
        {
            std::cout<<"Writing out training data"<<std::endl;
        }

        std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator mit;
        for ( mit = all_region_belms_.begin() ; mit != all_region_belms_.end();
              ++mit)
        {
            dbsk2d_transform_manager::Instance().write_output_region
                ((*mit).second);
            dbsk2d_transform_manager::Instance().write_output_polygon
                (all_region_polys_[(*mit).first]);

            if ( train_)
            {
                dbsk2d_transform_manager::Instance().write_output_region_stats
                    (region_stats_[(*mit).first]);

            }
        }

        //merge_closed_regions();
    }
}

// bool dbsk2d_containment_graph::
// is_rag_node_within_image(vgl_polygon<double>& polygon)
// {
//     bool flag=true; 

//     unsigned int ni=dbsk2d_transform_manager::Instance().get_image()->ni();
//     unsigned int nj=dbsk2d_transform_manager::Instance().get_image()->nj();

//     for (unsigned int s = 0; s < polygon.num_sheets(); ++s)
//     { 
//         for (unsigned int p = 0; p < polygon[s].size(); ++p)
//         { 
//             bool xflag = ( polygon[s][p].x() < 0 || polygon[s][p].x() > ni )
//                 ? false : true;
//             bool yflag = ( polygon[s][p].y() < 0 || polygon[s][p].y() > nj )
//                 ? false : true;

//             if (xflag == false || yflag == false )
//             {
//                 flag=false;
//                 break;
//             }
//         }
//         if (flag==false)
//         {
//             break;
//         }
//     }

//     return flag;

// }

//:construct graph
void dbsk2d_containment_graph::expand_node(
    dbsk2d_containment_node_sptr& node,
    std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >& outer_shock_nodes,
    std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >& degree_three_nodes,
    std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >& degree_three_links
)
{

    if ( node->get_prob() <= path_threshold_)
    {
        return;
    }

    // Detect transforms from new regions
    std::vector<std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >
        gap_pairs;
    std::vector<std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >
        gap4_pairs;

    dbsk2d_ishock_gap_detector detector(ishock_graph_);
    detector.detect_all_gaps(outer_shock_nodes,gap_pairs,gap4_pairs);

    // gap current depth
    unsigned int current_depth = node->get_depth()+1;
    dbsk2d_ishock_transform_sptr transform=
        node->get_parent_transform();
    std::set<int> belms_key;

    std::map<int,dbsk2d_ishock_bpoint*> gap_endpoints;

    if ( transform )
    {       
        belms_key=node->get_key();
    }

    std::map<unsigned int,std::vector<double> > gap_costs;
  
    // Add new child nodes for gaps
    for ( unsigned int i=0 ; i < gap_pairs.size() ; ++i)
    {
        std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> gap
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
            child->set_gap_prob(node->get_gap_prob()*prob);
            cgraph_nodes_[child->get_depth()].push_back(child);
        }

        
        belms_key.erase(gap.first->id());
        belms_key.erase(gap.second->id());

    }

    // Add new child nodes for gap4s
    for ( unsigned int i=0 ; i < gap4_pairs.size() ; ++i)
    {
        std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> gap4
            = gap4_pairs[i];

        dbsk2d_ishock_bpoint* anchor_pt = dbsk2d_transform_manager::Instance()
            .get_anchor_pt(gap4_pairs[i]);

        if ( anchor_pt->is_an_end_point())
        {
            continue;
        }

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
                                             anchor_pt,
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
            child->set_gap_prob(node->get_gap_prob()*prob);
            cgraph_nodes_[child->get_depth()].push_back(child);
        }

        
        belms_key.erase(gap4.first->id());
        belms_key.erase(anchor_pt->id());

    }

    std::vector<dbsk2d_ishock_bpoint*> endpoints;
    determine_endpoints(outer_shock_nodes,endpoints);

    for ( unsigned int e=0; e < endpoints.size() ; ++e )
    {
        if ( gap_endpoints.count(endpoints[e]->id()))
        {
            gap_endpoints.erase(endpoints[e]->id());
        } 
    }

    std::map<int,dbsk2d_ishock_bpoint*>::iterator mit;
    for ( mit = gap_endpoints.begin() ; mit != gap_endpoints.end() ; ++mit)
    {
        endpoints.push_back((*mit).second);
    }

    std::set<std::pair<int,int> > loop_pairs;

    for ( unsigned int e=0; e < endpoints.size() ; ++e )
    {
        std::set<int> local_copy(belms_key);
        
        dbsk2d_ishock_transform_sptr trans_loop1 = new 
            dbsk2d_ishock_loop_transform(ishock_graph_,
                                         endpoints[e]);
        std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>
            contour_pair=trans_loop1->get_contour_pair();

        std::pair<int,int> lp1(contour_pair.first->id(),
                             contour_pair.second->id());

        std::pair<int,int> lp2(contour_pair.second->id(),
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
                double c1 = *std::min_element(
                    gap_costs[contour_pair.first->id()].begin(),
                    gap_costs[contour_pair.first->id()].end());

                prob = 1.0-c1;
            }
            else if ( gap_costs.count(contour_pair.second->id()) &&
                      !gap_costs.count(contour_pair.first->id()))
            {
                double c1 = *std::min_element(
                    gap_costs[contour_pair.second->id()].begin(),
                    gap_costs[contour_pair.second->id()].end());
                    
                prob = 1.0-c1;
            }
            else if ( gap_costs.count(contour_pair.first->id()) &&
                      gap_costs.count(contour_pair.second->id()))
            {
                double c1 = *std::min_element(
                    gap_costs[contour_pair.first->id()].begin(),
                    gap_costs[contour_pair.first->id()].end());
                double c2 = *std::min_element(
                    gap_costs[contour_pair.second->id()].begin(),
                    gap_costs[contour_pair.second->id()].end());

                     
                prob = 1.0-std::min(c1,c2);

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

    std::map<std::set<int>,int> deg_three_loops;

    std::map<unsigned int,std::vector<dbsk2d_ishock_belm*> >::iterator dit;
    for ( dit = degree_three_nodes.begin() ; dit != degree_three_nodes.end();
          ++dit)
    {
        std::vector<dbsk2d_ishock_belm*> d3_nodes=(*dit).second;

        for ( unsigned int d=0; d < d3_nodes.size() ; ++d)
        {
            dbsk2d_ishock_bpoint* junction_node = 
                dynamic_cast<dbsk2d_ishock_bpoint*>(d3_nodes[d]);

            if ( degree_three_links.count((*dit).first))
            {
                std::vector<dbsk2d_ishock_belm*> links=
                    degree_three_links[(*dit).first];

                for ( unsigned int l=0; l < links.size() ; ++l)
                {
                    dbsk2d_ishock_belm* test = junction_node
                        ->getElmToTheRightOf(links[l]);
                    
                    if ( !test )
                    {
                        continue;
                    }
                    
                    std::set<int> local_copy(belms_key);
                    
                    dbsk2d_ishock_transform_sptr trans_loop1 = new 
                        dbsk2d_ishock_loop_transform(ishock_graph_,
                                                     junction_node,
                                                     links[l]);
           
                    trans_loop1->get_belms(local_copy);

                    if ( deg_three_loops.count(local_copy)  || 
                         !trans_loop1->valid_transform() )
                    {
                        trans_loop1=0;
                        continue;
                    }
                    
                    deg_three_loops[local_copy]=1;
                    
                    bool flag=find_node_in_cgraph(current_depth,
                                                  node,
                                                  local_copy);

                    double prob = trans_loop1->likelihood();

                    if ( !flag && trans_loop1->valid_transform() && 
                         prob > dbsk2d_transform_manager::
                         Instance().get_threshold() )
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
        }
        
    }
   
}


// node already exists
bool dbsk2d_containment_graph::find_node_in_cgraph(
    unsigned int current_depth,
    dbsk2d_containment_node_sptr& node,
    std::set<int>& belms_key)
{

    bool flag=false;

    std::vector<dbsk2d_containment_node_sptr> nodes_at_depth;
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
                std::set<int> prev_key=child->get_key();
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
    std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >& region_outer_nodes,
    std::vector<dbsk2d_ishock_bpoint*>& endpoints)
{

    std::map< int,std::string> local_endpoints;
    std::map< int,std::string> local_contours;

    // Loop thru new regions and determine new transforms
    std::map<unsigned int,std::vector<dbsk2d_ishock_node*> >::iterator it;

    for ( it = region_outer_nodes.begin() ; it != region_outer_nodes.end(); 
          ++it)
    {
        // Detect transforms
        std::vector<dbsk2d_ishock_node*> outer_shock_nodes = (*it).second;
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
                            if ( elist[1]->superiors_list()->size() )
                            {
                                elist.erase(elist.begin());
                            }
                        }

                        dbsk2d_bnd_edge* edge=(dbsk2d_bnd_edge*)elist[0].ptr();
                        const std::list< vtol_topology_object * > * 
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
                            if ( elist[1]->superiors_list()->size() )
                            {
                                elist.erase(elist.begin());
                            }
                        }

                        dbsk2d_bnd_edge* edge=(dbsk2d_bnd_edge*)elist[0].ptr();
                        const std::list< vtol_topology_object * > * 
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

void dbsk2d_containment_graph::write_graph(std::string filename)
{

    std::ofstream output_ascii_file;
    output_ascii_file.open(filename.c_str(),
                          std::ios::out | 
                          std::ios::app );

    output_ascii_file<<"digraph G {"<<std::endl;
    std::map<unsigned int,std::vector<dbsk2d_containment_node_sptr> > ::iterator 
        it;
    for ( it = cgraph_nodes_.begin() ; it != cgraph_nodes_.end() ; ++it)
    {
        std::vector<dbsk2d_containment_node_sptr> nodes=(*it).second;
        for ( unsigned int i=0; i < nodes.size() ; ++i)
        {
            std::vector<dbsk2d_containment_node_sptr> children
                = nodes[i]->get_children();

                
            for ( unsigned int j=0; j < children.size() ; ++j)
            { 
                dbsk2d_ishock_transform_sptr transform=
                    children[j]->get_parent_transform();
                dbsk2d_ishock_transform::TransformType ttype=
                    transform->get_transform_type();
                std::string prefix;
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
                std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>
                    pair=transform->get_contour_pair();
                
                std::stringstream body;
                body<<pair.first->id()<<","<<pair.second->id();
            
                output_ascii_file<<"\t"<<nodes[i]->get_id()
                                 <<" -> "
                                 <<children[j]->get_id()
                                 <<prefix
                                 <<body.str()
                                 <<"}\" ];"
                                 <<std::endl;

            }
           
        }

    }


    output_ascii_file<<"}"<<std::endl;
    output_ascii_file.close();

}


void dbsk2d_containment_graph::write_stats(std::ofstream& ofstream)
{

    std::map<unsigned int,std::vector<dbsk2d_containment_node_sptr> > ::iterator 
        it;
    for ( it = cgraph_nodes_.begin() ; it != cgraph_nodes_.end() ; ++it)
    {
        std::vector<dbsk2d_containment_node_sptr> nodes=(*it).second;
        ofstream<<"Depth: "
                <<(*it).first
                <<" Node: "
                <<nodes.size()
                <<std::endl;
       
        
    }

    ofstream<<std::endl;
    ofstream<<"Number of Regions: "<<all_region_belms_.size()<<std::endl;
}

void dbsk2d_containment_graph::cluster_fragments()
{
    bsta_k_medoid cluster(all_region_belms_.size());    

    unsigned int i=0;
    unsigned int j=0;
    std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator mit;
    std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator qit;

    for ( mit = all_region_belms_.begin() ; mit != all_region_belms_.end();
          ++mit)
    {
        
        std::set<int> model_set=(*mit).first;
        qit=mit;
        ++qit;

        j=i;
        ++j;

        for ( ; qit != all_region_belms_.end() ; ++qit)
        {
            double jaccard_index=0.0;

            std::set<int> query_set=(*qit).first;
            std::set<int> intersection;
            std::set<int> union_set;
            {
                std::insert_iterator<std::set<int> > 
                    inserter(intersection,intersection.begin());
                
                std::set_intersection(model_set.begin(),
                                     model_set.end(),
                                     query_set.begin(),
                                     query_set.end(),
                                     inserter);
            }
            
            if ( intersection.size())
            {
                std::insert_iterator<std::set<int> > 
                    inserter(union_set,union_set.begin());

                std::set_union(model_set.begin(),
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
        std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator ait
            =all_region_belms_.begin();
        std::advance(ait,index);
        dbsk2d_transform_manager::Instance().write_output_region
            ((*ait).second);
        dbsk2d_transform_manager::Instance().write_output_polygon
            (all_region_polys_[(*ait).first]);
        
    }

}


void dbsk2d_containment_graph::merge_closed_regions()
{
    std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> > final_merged_belms;
    std::map<std::set<int>,vgl_polygon<double> > final_merged_polys;

    std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator mit;
    for ( mit = closed_regions_.begin() ; mit != closed_regions_.end();
          ++mit)
    {
        std::set<int> closed_region_key;
        std::set<int> closed_region_key_orig;
        vgl_polygon<double> poly;

        std::vector<dbsk2d_ishock_belm*> belms=(*mit).second;
        closed_region_key_orig=(*mit).first;
        for ( unsigned int i=0; i < belms.size() ; ++i)
        {
            dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belms[i];
            closed_region_key.insert(bline->twinLine()->id());
        }
        poly=all_region_polys_[(*mit).first];
        
        
        std::map<int,dbsk2d_ishock_bline*> lines;
        std::set<int> final_key;
        bool write_out=false;
        std::map<std::set<int>,std::vector<dbsk2d_ishock_belm*> >::iterator nit;
        for ( nit = closed_regions_.begin() ; nit != closed_regions_.end();
              ++nit)
        {

            std::set<int> test_region_key=(*nit).first;
            std::map<int,int> mapping_twinline;
            
            std::vector<dbsk2d_ishock_belm*> belms=(*nit).second;
            for ( unsigned int i=0; i < belms.size() ; ++i)
            {
                dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)belms[i];
                mapping_twinline[bline->id()]=bline->twinLine()->id();
            }
            
            vgl_polygon<double> poly_test=all_region_polys_[(*nit).first];
            if ( test_region_key != closed_region_key_orig)
            {
                std::set<int> intersection;
                std::insert_iterator<std::set<int> > 
                    inserter(intersection,intersection.begin());
                
                std::set_intersection(closed_region_key.begin(),
                                     closed_region_key.end(),
                                     test_region_key.begin(),
                                     test_region_key.end(),
                                     inserter);
                
                if ( intersection.size() &&
                     intersection != closed_region_key &&
                     intersection != test_region_key )
                {
                    //Keep a flag for status
                    int value;
                        
                    //Take union of two polygons
                    poly = vgl_clip(poly,                // p1
                                    poly_test,           // p2
                                    vgl_clip_type_union, // p1 U p2
                                    &value);             // test if success
                    
                    write_out=true;
                    
                    
                    std::set<int> difference;
                    std::insert_iterator<std::set<int> > 
                        insert_diff(difference,difference.begin());
                    
                    std::set_difference(test_region_key.begin(),
                                       test_region_key.end(),
                                       closed_region_key.begin(),
                                       closed_region_key.end(),
                                       insert_diff);
                    
                    std::set<int>::iterator sit;
                    for ( sit=difference.begin() ; sit != difference.end()
                              ;++sit)
                    {
                        closed_region_key.insert(mapping_twinline[*sit]);
                    }
                    
                    if ( lines.size() == 0 )
                    {
                        
                        std::vector<dbsk2d_ishock_belm*> closed_region_belms
                            = (*mit).second;
                        
                        for ( unsigned int c=0; 
                              c < closed_region_belms.size() ; 
                              ++c)
                        {
                            dbsk2d_ishock_bline* bline= 
                                (dbsk2d_ishock_bline*)
                                    (closed_region_belms[c]);
                            if ( !intersection.count(bline->twinLine()->id()))
                            {
                                lines[bline->twinLine()->id()]=bline;
                            }
                            else
                            {
                                if ( lines.count(bline->twinLine()->id()))
                                {
                                    lines.erase(bline->twinLine()->id());
                                }
                            }
                            
                        }
                        
                        
                    }
                    else
                    {
                        std::vector<dbsk2d_ishock_belm*> closed_region_belms
                            = (*mit).second;

                        for ( unsigned int c=0; 
                              c < closed_region_belms.size() ; 
                              ++c)
                        {
                            dbsk2d_ishock_bline* bline= 
                                (dbsk2d_ishock_bline*)
                                (closed_region_belms[c]);

                            if ( intersection.count(bline->twinLine()->id()))
                            {
                                if ( lines.count(bline->twinLine()->id()))
                                {
                                    lines.erase(bline->twinLine()->id());
                                }
                            }
                            
                        }
                     
                    }
                    
                    {
                        
                        std::vector<dbsk2d_ishock_belm*> test_region_belms
                            = (*nit).second;
                        
                        for ( unsigned int b=0; 
                              b < test_region_belms.size() ; 
                              ++b)
                        {
                            dbsk2d_ishock_bline* bline= 
                                (dbsk2d_ishock_bline*)
                                (test_region_belms[b]);
                            if ( !intersection.count(bline->id()))
                            {
                                lines[bline->id()]=bline;
                            }
                            else
                            {
                                if ( lines.count(bline->twinLine()->id()))
                                {
                                    lines.erase(bline->twinLine()->id());
                                }
                            }
                        }
                        
                    }
                    
                        
                }
                    

            }
        }


        if ( write_out )
        {
           
            std::map<int,dbsk2d_ishock_bline*>::iterator lit;
            for ( lit=lines.begin() ; lit != lines.end() ; ++lit)
            {
                final_key.insert((*lit).second->s_pt()->id());
                final_key.insert((*lit).second->e_pt()->id());
            }
            
            final_merged_polys[final_key]=poly;
            std::map<int,dbsk2d_ishock_bline*>::iterator kit;
            for ( kit=lines.begin() ; kit != lines.end() ; ++kit)
            {
                final_merged_belms[final_key].push_back((*kit).second);
            }
        }
    }

    std::cout<<"Final merged size: "<<final_merged_polys.size()<<std::endl;
    std::map<std::set<int>,vgl_polygon<double> >::iterator bit;
    for ( bit = final_merged_polys.begin() ; bit != final_merged_polys.end();
          ++bit)
    {
        dbsk2d_transform_manager::Instance().write_output_polygon(
            (*bit).second);
        dbsk2d_transform_manager::Instance().write_output_region(
            final_merged_belms[(*bit).first]);

        if ( train_ )
        {
            dbsk2d_transform_manager::Instance().write_stats_closed(
                final_merged_belms[(*bit).first]);

        }
    }
    







}
