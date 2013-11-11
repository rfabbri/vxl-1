// This is brcv/shp/dbskfg/algo/dbskfg_transformer.h
#ifndef dbskfg_transformer_h_
#define dbskfg_transformer_h_
//:
// \file
// \brief Algorithm to apply transforms to composite graph
// \author Maruthi Narayanan
// \date 07/29/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/29/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>

class dbskfg_shock_link;
class dbsk2d_ishock_edge;

//: Takes a transform object and applies it to graph
class dbskfg_transformer
{

public:

    //: Constructor
    dbskfg_transformer
        (   dbskfg_transform_descriptor_sptr transform, 
            dbskfg_composite_graph_sptr composite_graph,
            dbskfg_rag_graph_sptr rag_graph);

    //: Destructor
    ~dbskfg_transformer();

    //: Return status of transformation
    bool status(){return status_flag_;}

    //: Undo transform 
    void undo_transform();

    //: Return new rag nodes
    vcl_vector<unsigned int> new_rag_nodes(){return new_rag_nodes_;}

    //: Tell me if rag node has been deleted
    bool rag_deleted(unsigned int id) { return 
            rag_nodes_deleted_.count(id) > 0 ;}
private:

    // Hold composite graph
    dbskfg_composite_graph_sptr original_graph_;
    
    // Holds a gap detector object
    dbskfg_transform_descriptor_sptr transform_;

    // Holds local_graph
    dbskfg_composite_graph_sptr local_graph_;

    // Holds shock storage
    dbsk2d_shock_storage_sptr shock_storage_;

    // Holds all contour/shock nodes
    vcl_map<vcl_string, dbskfg_composite_node_sptr> all_nodes_;

    // Holds all new contour nodes
    vcl_set<vcl_string> new_contour_nodes_;

    // Holds all existing contour links
    vcl_map<unsigned int, dbskfg_composite_link_sptr> orig_contour_links_;

    // Holds all contours deleted
    vcl_map<unsigned int, dbskfg_composite_link_sptr> contours_removed_;

    // Holds new contour links
    vcl_map<unsigned int, dbskfg_composite_link_sptr> new_contour_links_;

    // Holds all new contour links by endpoint, and start point
    vcl_map<vcl_string,dbskfg_composite_link_sptr> new_contour_links_points_;

    // Holds all new shock links
    vcl_map<unsigned int, dbskfg_composite_link_sptr> new_shock_links_;

    // Keep track that all outer shock nodes are visited
    vcl_map<unsigned int, vcl_pair<dbskfg_composite_node_sptr,bool> > 
        wavefront_;

    // Keep a map of all rag nodes deleted
    vcl_map<unsigned int,vcl_string> rag_nodes_deleted_;

    // Keep a vector of all new rag nodes added
    vcl_vector<unsigned int> new_rag_nodes_;

    // Holds rag graph
    dbskfg_rag_graph_sptr rag_graph_;
    
    // Keeps track of status
    bool status_flag_;

    //: Methods
    
    // Deletes out of original composite graph
    void remove_segs_original_graph();

    // Computes the new piece to add
    bool compute_local_graph();

    // Refits the piece to the original graph
    void local_surgery();

    // Determine whether shock link should be added
    void shock_link_add(dbskfg_composite_link_sptr link,
                        dbskfg_composite_node_sptr& source_node,
                        dbskfg_composite_node_sptr& target_node);

    // Create shock link
    void create_shock_link(dbskfg_composite_node_sptr source,
                           dbskfg_composite_node_sptr target,
                           dbskfg_shock_link* shock_link);

    // Test whether further local surgery needs to be done
    void wavefront_complete(dbsk2d_ishock_edge* edge);

    // Test whether further local surgery needs to be done
    void wavefront_complete(dbskfg_composite_node_sptr& node);

    //: Helper Methods
    void add_new_contours();

    //: Classify Nodes
    void classify_nodes();

    //: add rag nodes
    void add_rag_nodes();

    //: verify surgery
    void verify_surgery();

    //: test for extra shock links that need to be removed
    void remove_extra_shocks();

    //: test for extra shock links that need to be removed
    void remove_extra_shocks_helper( 
        vcl_vector<dbskfg_composite_node_sptr>& queue);

    // Make copy ctor private
    dbskfg_transformer(const dbskfg_transformer&);

    // Make assign operator private
    dbskfg_transformer& operator
        =(const dbskfg_transformer& );

   
};

#endif //dbskfg_transformer_h_
