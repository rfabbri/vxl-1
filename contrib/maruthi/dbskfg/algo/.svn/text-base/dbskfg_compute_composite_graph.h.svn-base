// This is brcv/shp/dbskfg/algo/dbskfg_compute_composite_graph.h
#ifndef dbskfg_compute_composite_graph_h_
#define dbskfg_compute_composite_graph_h_
//:
// \file
// \brief Algorithm to compute composite graph representation
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbskfg/dbskfg_utilities.h>
#include <vcl_map.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_utility.h>
#include <vgl/vgl_line_segment_2d.h>

class dbsk2d_ishock_node;
class dbsk2d_ishock_edge;
class dbskfg_contour_node;
class dbskfg_contour_link;
class dbskfg_shock_link;

//: Form Composite Graph algorithm
class dbskfg_compute_composite_graph
{

public:

    //: Constructor
    dbskfg_compute_composite_graph
        ( dbsk2d_ishock_graph_sptr ishock_graph, 
          dbsk2d_shock_graph_sptr coarse_shock_graph, 
          dbskfg_composite_graph_sptr composite_graph,
          dbskfg_rag_graph_sptr rag_graph);

    //: Destructor
    ~dbskfg_compute_composite_graph();
  
    //: return the composite graph
    dbskfg_composite_graph_sptr composite_graph();

    
    // Compute Graph
    bool compile_composite_graph();

private:

    // compile all shock/contour nodes and shock rays
    void compile_nodes_and_shock_rays(bool& status_flag);

    // compile all contour links and atomic fragments
    void compile_contour_and_shock_links(bool& status_flag);

    // compile the bounding polygon of all fragments
    void compile_shock_fragments();

    // compile all shock/contour nodes and shock rays helper
    void compile_nodes_and_shock_rays_helper(dbsk2d_ishock_node* node,
                                             bool& status_flag);

    // compile all contour links and atomic fragments
    void compile_contour_and_shock_links_helper
        (dbsk2d_ishock_edge* edge,
         dbskfg_shock_link* shock_link,
         vcl_map< vcl_pair<unsigned int,unsigned int>, 
         dbskfg_composite_link_sptr>& 
         edge_map, dbskfg_utilities::Orientation type,bool& status_flag);

  
    //: Methods
    // return a vector of points that lie in between lines
    vcl_vector< dbskfg_contour_node* > 
        find_points_on_line(vgl_line_segment_2d<double>& line, 
                            int s_contour_id,
                            int e_contour_id);

    // Reclassify nodes
    void classify_nodes();

    // Composite Graph that we are going to populate
    dbskfg_composite_graph_sptr composite_graph_;

    // Coarse shock graph that we use to figure out what to populate
    dbsk2d_shock_graph_sptr shock_graph_;

    // Intrinsinc shock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Region adjancency graph
    dbskfg_rag_graph_sptr rag_graph_;

    // Keeps a unique string id attached to each contour node
    vcl_map<vcl_string,dbskfg_contour_node*> contour_node_map_;

    // Keep a mapping of instrinsinc shock ids to composite shock nodes
    vcl_map<int,dbskfg_composite_node*> isnodes_to_composite_snodes_;

    // Keep a map of all contour nodes associated with an original contour id
    vcl_map<int, vcl_vector<dbskfg_contour_node*> > nodes_contour_;

    // Make copy ctor private
    dbskfg_compute_composite_graph(const dbskfg_compute_composite_graph&);

    // Make assign operator private
    dbskfg_compute_composite_graph& operator
        =(const dbskfg_compute_composite_graph& );

};

#endif //dbsk2d_ishock_prune_h_
