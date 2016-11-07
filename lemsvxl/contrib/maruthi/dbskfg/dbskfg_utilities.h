// This is the base class for providing a common set of utilities for 
// shock fragment formation
#ifndef dbskfg_utilities_h_
#define dbskfg_utilities_h_

//:
// \file
// \brief A namespace class for representing utilities
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

// dbskfg headers
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
// vgl headers
#include <vgl/vgl_polygon.h>
// vcl headers
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_set.h>
// vsol headers
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vil/vil_image_resource_sptr.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
class dbsk2d_ishock_edge;
class dbskfg_contour_link;
class dbskfg_shock_node;

namespace dbskfg_utilities
{
    //: Enum
    // Defines enumeration of orientation types
    enum Orientation
    {
        LEFT,
        RIGHT
    };

    //: Enum
    // Defines Type of Fragment
    enum Fragment_Type
    {
        DEGENERATE,
        REGULAR
    };

    //: Enum
    // Defines Type of Shock Computation
    enum Shock_Compute_Type
    {
        RLLP, // Right Line Left Point
        LLRP, // Left Line Right Point
        LL,   // Line Line 
        PP    // Point Point
    };


    //: Enum
    // Defines how nodes should be tested for equality
    enum CompareType
    {
        ID,
        POINT
    };

    //: Tell if a shock node is a source
    bool is_a_source(dbskfg_composite_node_sptr node);

    //: Tell if a shock node is a sink
    bool is_a_sink(dbskfg_composite_node_sptr node);

    //: Determine bounding box for composite graph
    void bbox(dbskfg_composite_graph_sptr& cgraph,
              vsol_box_2d& box);

    //: Tell if a shock node is a sink
    //: Methods
    // Search Method to see if a link is part of a set of links
    bool is_link_in_set_of_links(vcl_vector<dbskfg_composite_link_sptr >& set,
                                 dbskfg_composite_link_sptr test_link);

    //: Methods
    // Search Method to see if a node is part of a set of nodes
    bool is_node_in_set_of_nodes(vcl_vector<dbskfg_composite_node_sptr >& set,
                                 dbskfg_composite_node_sptr test_node,
                                 CompareType compare=ID);

    //: Methods
    // Search Method to see if a node is part of a set of nodes
    bool is_node_in_set_of_nodes(vcl_vector<dbskfg_composite_node_sptr >& set,
                                 vcl_string point_location);

    
    //: Methods
    // Search Method to see if a node is part of a set of nodes
    bool is_node_in_set_of_nodes(vcl_vector<dbskfg_composite_node_sptr >& set,
                                 dbskfg_shock_node* snode,
                                 CompareType compare=ID);

    //: Methods
    // Search Method to see if a node is part of a set of nodes
    dbskfg_composite_node_sptr get_node_in_set_of_nodes( 
        vcl_vector<dbskfg_composite_node_sptr >& set,
        dbskfg_composite_node_sptr test_node,
        CompareType compare=ID);


    //: Methods
    // Search Method to see if a node is part of a set of nodes
    dbskfg_composite_node_sptr get_node_in_set_of_nodes( 
        vcl_vector<dbskfg_composite_node_sptr >& set,
        vgl_point_2d<double> point);
  
    //: Methods
    // Search Method to see if a node is part of a set of links
    bool is_node_in_set_of_links(vcl_vector<dbskfg_composite_link_sptr>& set,
                                 dbskfg_composite_node_sptr test_node);

    //: Methods
    // Find gap connected graph
    void find_gap_connected_graph(vcl_map<unsigned int, 
                                  dbskfg_shock_link*>& shock_map,
                                  unsigned int common_edge_id);

    //: Methods
    // From a shock edge determines the original contour id pair
    vcl_pair<int,int> 
        get_contour_id_from_shock_edge(dbsk2d_ishock_edge* edge);

    // Method 
    // Assumes points are degree 1 contour endpoints
    vcl_pair<double,double> get_tangent_pairs(dbskfg_contour_node*
                                              lnode,
                                              dbskfg_contour_node*
                                              rnode,
                                              double& theta1,
                                              double& theta2);

    //: Methods
    // compares two transforms
    bool comparison( dbskfg_transform_descriptor_sptr transform1,
                     dbskfg_transform_descriptor_sptr transform2);

    //: Methods
    // Determine clockwise orientation
    vcl_vector<dbskfg_shock_link*> clockwise(dbskfg_composite_node_sptr node);

    //: Methods 
    // determine influence zone between two gap points filling out 
    // all information for a transform
    void gap_1_local_context(dbskfg_contour_node* p1,
                             dbskfg_contour_node* p2,
                             unsigned int common_edge_id,
                             dbskfg_transform_descriptor_sptr transform);  

    //: Methods 
    // determine influence zone between point and line or point and point
    // Return point to consider
    dbskfg_composite_node_sptr gap_4_local_context(
        dbskfg_contour_node* p1,
        dbskfg_transform_descriptor_sptr transform,
        vgl_point_2d<double>& point);  

    //: Methods
    // calculate if point in polygon and returns sheet number
    bool point_in_polygon(vgl_polygon<double>& poly,double x,double y,
                          unsigned int& sheet);

    //: Method
    // Saves image polygon
    void save_image_poly(vgl_polygon<double>& vgl_poly,
                         vil_image_resource_sptr img_sptr,
                         vcl_string filename);

    //: Method
    // Saves image polygon
    void save_image_mask(vgl_polygon<double>& vgl_poly,
                         vil_image_resource_sptr img_sptr,
                         vcl_string filename,
                         bool actual=false);


    //: Return first adjacent shock edge
    dbskfg_composite_link_sptr 
        first_adj_shock(dbskfg_composite_node_sptr shock_node);
  
    //: Return first adjacent shock edge
    dbskfg_composite_link_sptr 
        first_adj_contour(dbskfg_composite_node_sptr contour_node);
    
    //: Return cyclic succesor
    dbskfg_composite_link_sptr 
        cyclic_adj_succ(dbskfg_composite_link_sptr edge,
                        dbskfg_composite_node_sptr node);

    //: Return cyclic succesor
    dbskfg_composite_link_sptr 
        cyclic_adj_pred(dbskfg_composite_link_sptr edge,
                        dbskfg_composite_node_sptr node);

    //: Return cyclic shock succesor
    dbskfg_composite_link_sptr 
        cyclic_adj_shock_succ(dbskfg_composite_link_sptr shock_edge,
                              dbskfg_composite_node_sptr shock_node );

    //: Return cyclic shock succesor
    dbskfg_composite_link_sptr 
        cyclic_adj_contour_succ(dbskfg_composite_link_sptr contour_edge,
                                dbskfg_composite_node_sptr contour_node );

    //: Return cyclic shock predicate
    dbskfg_composite_link_sptr 
        cyclic_adj_shock_pred(dbskfg_composite_link_sptr shock_edge,
                              dbskfg_composite_node_sptr shock_node );


    //: Detect transforms
    void detect_transforms(
        vidpro1_vsol2D_storage_sptr& contour_storage,
        vcl_vector<dbskfg_transform_descriptor_sptr>& results,
        bool detect_gaps = true,
        bool detect_loops = true);

    // Return set of points for euler_spiral
    void ess_points(dbskfg_transform_descriptor_sptr transform,
                    vcl_vector<vgl_point_2d<double> >& points);


    // Return 1-jacard index
    double jacard_distance(vgl_polygon<double>& poly1,
                           vgl_polygon<double>& poly2);

    // Reclassify nodes
    void classify_nodes(dbskfg_composite_graph_sptr composite_graph_);


    // Read file
    void read_binary_file(
        vcl_string input_file, 
        vcl_map<unsigned int,
        vcl_vector< vsol_spatial_object_2d_sptr > >& geoms,
        vcl_map<unsigned int,vcl_set<unsigned int> >& con_ids,
        vcl_pair<unsigned int,unsigned int>& image_size);
}
#endif // dbskfg_utilities_h_


