// This is brcv/shp/dbsk2d/algo/dbsk2d_containment_graph.h
#ifndef dbsk2d_containment_graph_h_
#define dbsk2d_containment_graph_h_
//:
// \file
// \brief This class holds the full containment graph 
// \author Maruthi Narayanan
// \date 09/12/12
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 09/09/12    Initial version.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_containment_node_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <vgl/vgl_polygon.h>

#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_stack.h>
#include <vcl_string.h>
#include <vcl_set.h>

class dbsk2d_ishock_node;
class dbsk2d_ishock_bpoint;
class dbsk2d_ishock_belm;

//: Represents a graph structure
class dbsk2d_containment_graph
{
    
public: 
    //: Constructor
    dbsk2d_containment_graph(dbsk2d_ishock_graph_sptr ishock_graph,
                             double path_threshold,
                             unsigned int loop_type,
                             bool expand_outside=false);

    //:Destructor
    ~dbsk2d_containment_graph();

    //: construct graph
    void construct_graph();

    //: write graph
    void write_graph(vcl_string filename);

    //: write stats
    void write_stats(vcl_ofstream& ofstream);

    void set_cluster_centers(unsigned int cluster_centers)
    {cluster_centers_=cluster_centers;}

private: 
    
    // store ishock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Store all nodes in a map by depth
    vcl_map<unsigned int,vcl_vector<dbsk2d_containment_node_sptr> > 
        cgraph_nodes_;

    // store a stack for traversal
    vcl_stack<dbsk2d_containment_node_sptr> stack_;

    // Keep track of regions
    vcl_map<vcl_set<int>, vcl_vector<dbsk2d_ishock_belm*> > all_region_belms_;

    // Keep track of polygons
    vcl_map<vcl_set<int>, vgl_polygon<double> > all_region_polys_;

    // Keep track of polygons
    vcl_map<vcl_set<int>, vcl_string > closed_regions_;

    // store path threshold
    double path_threshold_;

    // loop cost type
    unsigned int loop_type_;

    // Number of cluster centers if doing clustering
    unsigned int cluster_centers_;

    // store id for nodes
    unsigned int next_available_id_;

    // store whether expanding outside
    bool expand_outside_;

    //: get next available id 
    unsigned int next_available_id()
    {
        next_available_id_++;
        return next_available_id_;
    }

    // store id for nodes
    int gap_id_;

    //: get next available id 
    int gap_id()
    {
        gap_id_--;
        return gap_id_;
    }

    // test whether fragment is within image
    bool is_rag_node_within_image(vgl_polygon<double>& polygon);

    // expand root node
    void expand_node(
        dbsk2d_containment_node_sptr& node,
        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_node*> >
                     & outer_shock_nodes);

    // find node at this depth in graph
    bool find_node_in_cgraph(unsigned int current_depth,
                             dbsk2d_containment_node_sptr& node,
                             vcl_set<int>& belms_key);

    // Return a set of endpoints
    void determine_endpoints( vcl_map<unsigned int,
                              vcl_vector<dbsk2d_ishock_node*> >& 
                              outer_shock_nodes,
                              vcl_vector<dbsk2d_ishock_bpoint*>& endpoints);

    // cluster patches and keep mediods
    void cluster_fragments();

    // merge closed regions
    void merge_closed_regions();

    // Make copy ctor private
    dbsk2d_containment_graph(const dbsk2d_containment_graph&);

    // Make assign operator private
    dbsk2d_containment_graph& operator
        =(const dbsk2d_containment_graph& );
};

#endif //dbsk2d_containment_graph_h_
