// This is brcv/shp/dbskfg/algo/dbskfg_compute_composite_graph_streamlined.h
#ifndef dbskfg_compute_composite_graph_streamlined_h_
#define dbskfg_compute_composite_graph_streamlined_h_
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
#include <dbsk2d/dbsk2d_boundary_sptr.h>
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
class dbskfg_compute_composite_graph_streamlined
{

public:

    //: Constructor
    dbskfg_compute_composite_graph_streamlined
        ( dbsk2d_ishock_graph_sptr ishock_graph, 
          dbskfg_composite_graph_sptr composite_graph,
          dbskfg_rag_graph_sptr rag_graph,
          bool adjacency_flag=true);

    //: Destructor
    ~dbskfg_compute_composite_graph_streamlined();
  
    //: return the composite graph
    dbskfg_composite_graph_sptr composite_graph();
    
    // Compute Graph
    bool compile_composite_graph(dbsk2d_boundary_sptr boundary);

private:

    // compile all conotur nodes and links
    // and insert them into the graph
    void compile_contour_elements(dbsk2d_boundary_sptr boundary);

    // compile all shock nodes and links
    // and insert them into the graph
    void compile_shock_elements();

    // compile all shock fragments
    void compile_shock_fragments();

    // Reclassify nodes
    void classify_nodes();

    // Composite Graph that we are going to populate
    dbskfg_composite_graph_sptr composite_graph_;

    // Intrinsinc shock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Region adjancency graph
    dbskfg_rag_graph_sptr rag_graph_;

    // Bool adjaceny flag
    bool adjacency_flag_;

    // Keep an original map of boundary elements to new contour links
    vcl_map<int,dbskfg_composite_link_sptr> boundary_link_mapping_;

    // Keep an original map of boundary elements to new contour nodes
    vcl_map<int,dbskfg_composite_node_sptr> boundary_node_mapping_;

    // Make copy ctor private
    dbskfg_compute_composite_graph_streamlined
        (const dbskfg_compute_composite_graph_streamlined&);

    // Make assign operator private
    dbskfg_compute_composite_graph_streamlined& operator
        =(const dbskfg_compute_composite_graph_streamlined& );

};

#endif //dbsk2d_ishock_prune_h_
