// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_grouping_transform.h
#ifndef dbsk2d_ishock_grouping_transform_h_
#define dbsk2d_ishock_grouping_transform_h_
//:
// \file
// \brief Base class for all shock grouping_transforms
// \author Maruthi Narayanan
// \date 07/15/11
// 
// This file contains base methods which are common to gap or loop grouping_transforms

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/15/12    Initial version.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform_sptr.h>

#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <vcl_iterator.h>

class dbsk2d_lagrangian_ishock_detector;
class dbsk2d_ishock_belm;
class dbsk2d_ishock_edge;
  
//: Loop Grouping_Transform Remvol algorithm
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_grouping_transform
{
    
public: 
    //: Constructor
    dbsk2d_ishock_grouping_transform(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph);

    //: Destructor
    ~dbsk2d_ishock_grouping_transform(){};

    //: grow regions
    void grow_regions();

    //: grow individual regions
    void grow_transformed_regions(int id);

    //: extract polygons
    void extract_polygon(dbsk2d_ishock_edge* edge,vgl_polygon<double>& poly);

    //: contour_ratio
    double contour_ratio(unsigned int index,vgl_polygon<double>& polygon);

    //: contour_ratio
    double contour_ratio(unsigned int index);

    //: real contour ratio 
    void real_contour_ratio(unsigned int index,
                            double& gt_con_ratio,
                            double& gap_ratio);

    // convex area
    double convex_area(unsigned int index);

    // convex area
    double convex_area(vgl_polygon<double>& poly);


    // get statistics of polygon
    void get_region_stats(
        unsigned int index,
        vgl_polygon<double>& poly,
        vcl_vector<double>& region_stats);

    //: contour_ratio
    double real_contour_length(unsigned int index);

    // See if region is within class
    bool region_within_image(unsigned int index);

    //: extract poly for fragment
    void polygon_fragment(unsigned int,vgl_polygon<double>& poly);

    //: write out output polygon
    void write_out_polygons(vcl_string filename);

    //: write out output polygon
    void write_out_polygons(vcl_string filename,
                            dbsk2d_ishock_transform& transform);

    // sort dbsk2d ishock edges
    bool static inline sort_edges(dbsk2d_ishock_edge* e1,
                           dbsk2d_ishock_edge* e2)
    { return e1->id() < e2->id();}


    // get region nodes
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> >&
        get_region_nodes(){return region_nodes_;}

    // get outer_shock nodes
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >&
        get_outer_shock_nodes(){return outer_shock_nodes_;}

    // get region belms ids
    vcl_map<unsigned int,vcl_set<int> >&
        get_region_belms_ids(){return region_belms_ids_;}

    // get region belm contour ids
    vcl_map<unsigned int,vcl_set<int> >&
        get_region_belms_contour_ids(){return region_belms_contour_ids_;}

    // get region belms
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >&
        get_region_belms(){return region_belms_;}


    // get region belm contour ids
    vcl_map<unsigned int,vcl_set<int> >&
        get_degree_three_node_ids(){return degree_three_node_ids_;}

    // get region belms
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >&
        get_degree_three_nodes(){return degree_three_nodes_;}
   
    // See if region has matching with rag graph
    void inline rag_nodes(vcl_vector<dbsk2d_ishock_belm*> region_elements,
                          vcl_set<int> region_contour_ids,
                          dbsk2d_ishock_transform_sptr transform,
                          vcl_set<unsigned int>& rag_nodes_matched,
                          int compare)
    {
        /* if ( region_contour_ids.size() == 1 ) */
        /* { */
        /*     vcl_vector<unsigned int> matching_regions; */
        /*     vcl_map<unsigned int,vcl_set<int> >::iterator it; */
        /*     for ( it = region_belms_contour_ids_.begin() ;  */
        /*           it != region_belms_contour_ids_.end(); */
        /*           ++it) */
        /*     { */
        /*         bool flag=transform->region_affected((*it).second); */
                
        /*         if ( flag ) */
        /*         { */
        /*             vcl_cout<<"Found Region affected by transform"<<vcl_endl; */
        /*             vcl_set<int> contour_ids=(*it).second; */
                    
        /*             bool flag2 = vcl_includes(contour_ids.begin(), */
        /*                                       contour_ids.end(), */
        /*                                       region_contour_ids.begin(), */
        /*                                       region_contour_ids.end()); */
        /*             if ( flag2 == true) */
        /*             { */
        /*                 rag_nodes_matched.insert((*it).first); */
        /*             vcl_cout<<"Found Region has this contour"<<vcl_endl; */
        /*             }    */
        /*         } */
        /*     } */


        /* } */
        /* else if ( region_contour_ids.size() > 1 ) */
        /* { */

        vcl_set<int> test_set;
        for ( unsigned int i=0; i < region_elements.size() ; ++i)
        {
            test_set.insert(region_elements[i]->id());
        }

        vcl_map<unsigned int,vcl_set<int> >::iterator it;
        for ( it = region_belms_ids_.begin() ; 
              it != region_belms_ids_.end();
              ++it)
        {

            vcl_set<int> intersection;
            vcl_insert_iterator<vcl_set<int> > 
                inserter(intersection,intersection.begin());
            
            vcl_set_intersection(test_set.begin(),
                                 test_set.end(),
                                 (*it).second.begin(),
                                 (*it).second.end(),
                                 inserter);
            
            if ( intersection.size())
            {

                if ( region_contour_ids.size() == 1 )
                {
                    if (transform->get_transform_type()
                        ==dbsk2d_ishock_transform::GAP )
                    {
                        if ( intersection.size() != test_set.size() )
                        {
                            continue;
                        }
                    }
                    else
                    {
                        
                        if ( !transform->region_in_min_local_context
                             ((*it).second))
                        {
                            continue;
                        }
                                     
                    }
                }

                vcl_set<int> contour_ids=
                    region_belms_contour_ids_[(*it).first];
                
                vcl_set<int> cont;
                vcl_insert_iterator<vcl_set<int> > 
                    con_insert(cont,cont.begin());
                
                vcl_set_intersection(contour_ids.begin(),
                                     contour_ids.end(),
                                     region_contour_ids.begin(),
                                     region_contour_ids.end(),
                                     con_insert);
                
                int cont_size=cont.size();
                if ( cont_size >= compare )
                {
                    rag_nodes_matched.insert((*it).first);
                }
                
            }
            
            
        }
    
    }

private:

    // Method to tell if shock formed from end point
    bool inline shock_from_endpoint(dbsk2d_ishock_edge* selm)
    {


        dbsk2d_ishock_bpoint* bpoint_left = (selm->lBElement()->is_a_point())
            ?(dbsk2d_ishock_bpoint*)selm->lBElement():0;
        dbsk2d_ishock_bpoint* bpoint_right = (selm->rBElement()->is_a_point())
            ?(dbsk2d_ishock_bpoint*)selm->rBElement():0;

        bool lflag=(bpoint_left)?bpoint_left->is_an_end_point():false;
        bool rflag=(bpoint_right)?bpoint_right->is_an_end_point():false;

        return lflag|rflag;
    }

    // Method test if node is still expandable
    void inline node_expandable(dbsk2d_ishock_node* node,
                                vcl_vector<dbsk2d_ishock_edge*>& edges,
                                bool& insert)
    {

        ishock_edge_list adj_edges=node->adj_edges();

        ishock_edge_list::iterator curS = adj_edges.begin();
        for(; curS!= adj_edges.end(); ++curS)
        {
            dbsk2d_ishock_edge* selm = *curS;
            
            if ( selm->is_a_contact())
            {
                continue;
            }

            bool endpoint_spawned=shock_from_endpoint(selm);
            bool visited=visited_edges_.count(selm->id());

            if ( endpoint_spawned )
            {
                insert=true;
            }

            if (!(endpoint_spawned|visited))
            {
                edges.push_back(selm);
            }
            
        }    


    }


    // Expand wavefront in a Recursive manner
    void expand_wavefront(dbsk2d_ishock_node* node,unsigned int map_key);
 
    // Private intrinsinc shock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Private boundary class
    dbsk2d_boundary_sptr boundary_;

    // Store visited edges
    vcl_map<unsigned int,vcl_string> visited_edges_;

    // Store rag nodes
    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_edge*> > region_nodes_;

    // Store outer shock nodes
    vcl_map<unsigned int, vcl_vector<dbsk2d_ishock_node*> > outer_shock_nodes_;

    // Store belms
    vcl_map<unsigned int, vcl_set<int> > region_belms_ids_;
    
    // Store belms contour ids
    vcl_map<unsigned int, vcl_set<int> > region_belms_contour_ids_;

    // Store belms of all regions
    vcl_map<unsigned int, vcl_vector<dbsk2d_ishock_belm*> > 
        region_belms_;

    // Store alll degree three nodes
    vcl_map<unsigned int, vcl_vector<dbsk2d_ishock_belm*> >
        degree_three_nodes_;

    vcl_map<unsigned int,vcl_set<int> > degree_three_node_ids_;

    // Make copy ctor private
    dbsk2d_ishock_grouping_transform(const dbsk2d_ishock_grouping_transform&);

    // Make assign operator private
    dbsk2d_ishock_grouping_transform& operator
        =(const dbsk2d_ishock_grouping_transform& );
};

#endif //dbsk2d_ishock_grouping_transform_h_
