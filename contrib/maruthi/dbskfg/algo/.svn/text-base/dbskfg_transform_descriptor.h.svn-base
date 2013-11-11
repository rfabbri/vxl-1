// This is a class to represent 
#ifndef dbskfg_transform_descriptor_h_
#define dbskfg_transform_descriptor_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a shock graph boundary
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vgl/vgl_polygon.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vcl_string.h>
#include <bxml/bxml_write.h>

class dbskfg_shock_link;

class dbskfg_transform_descriptor: public vbl_ref_count
{

public:

    //: Enum
    enum TransformType
    {
        GAP,
        LOOP
    };

    // Constructor
    dbskfg_transform_descriptor();

    // Destructor
    ~dbskfg_transform_descriptor();

    // Keep an id for this transform
    unsigned int id_;

    // Keep a flag if this id has been processed
    bool processed_;

    // Keep the type of this node
    TransformType transform_type_;

    // Keep cost of this transform
    double cost_;

    // Contour cost
    double contour_cost_;

    // Appearance cost
    double app_cost_;

    // Holds euler spiral parameters
    double gamma_;

    // Holds curvature
    double k0_;

    // Holds length
    double length_;

    // Holds distance of gap
    double d_;

    // Holds theta of angles
    double theta1_;

    // Holds theta of angles
    double theta2_;

    // Has a degree three node for gap/loop
    bool t_type_transform_;

    // Store all contours to remove
    vcl_vector<dbskfg_composite_node_sptr> contours_to_remove_;

    // Store all contours affected
    vcl_vector<dbskfg_composite_link_sptr> contour_links_to_remove_;

    // Store all contours affected
    vcl_vector<dbskfg_composite_link_sptr> contours_affected_;

    // Store all contours affected
    vcl_vector<dbskfg_composite_node_sptr> contours_nodes_affected_;

    // Store all shock nodes affected
    vcl_vector<dbskfg_composite_node_sptr> shock_nodes_affected_;

    // Store all shock links affected
    vcl_vector<dbskfg_composite_link_sptr> shock_links_affected_;

    // Store all contours as vsol objects in this transform
    vcl_vector<vsol_spatial_object_2d_sptr> contours_spatial_objects_;

    // Keep a list of outer shock nodes affected
    vcl_vector<dbskfg_composite_node_sptr> outer_shock_nodes_;
 
    // In this case this will be one polyline 
    vcl_vector<vsol_spatial_object_2d_sptr> new_contours_spatial_objects_;

    // The gap is represented by two endpoints
    vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr> gap_;
    
    // Store a vgl polygon
    vgl_polygon<double> poly_;

    // Helper function to convert contours affected to vsol objects
    void convert_contours_to_vsol();

    // Tell where endpoint is a gap endpoint
    bool endpoint_in_gap(dbskfg_composite_node_sptr node);

    // Determine contour ids affected
    vcl_vector<unsigned int> contour_ids_affected();

    // Get bounding box of all contours
    vsol_box_2d_sptr bounding_box_context();

    // See if this transform involves degree 1 nodes
    bool degree_1_nodes();

    void remove_extra_shocks();

    void trim_transform();

    void trim_transform_helper(dbskfg_shock_link* shock_link);

    void group_transform(dbskfg_transform_descriptor& transform);


    void group_cnodes_affected(dbskfg_transform_descriptor& transform);

    void compute_polygon_string();

    // print information about this transform
    void print(vcl_ostream& os);

    // Return unique gap string
    vcl_pair<vcl_string,vcl_string> gap_string();

    // Return unique gap string
    vcl_pair<vcl_string,vcl_string> gap_endpoints();

    // Determine loop endpoints
    vcl_vector<dbskfg_composite_node_sptr> loop_endpoints_;

    // Keep track of all compounded gaps
    vcl_vector<
        vcl_pair<dbskfg_composite_node_sptr,dbskfg_composite_node_sptr> > 
        all_gaps_;

    // Store a string representation of polygon
    vcl_map<vcl_string,unsigned int> polygon_string_rep_;

    // Store the new contour id associated by transorm manager
    unsigned int gap_id_;

    // Create copy
    void create_copy(dbskfg_composite_graph_sptr& cgraph);
    
    // destroy transform
    void destroy_transform();

    // write transform 
    void write_transform(const bxml_data_sptr& root_xml,
                         vcl_string filename,
                         bool flag);

    // test whether all shock links surrounding node are in shock links affected
    bool node_all_shocks_deleted(dbskfg_composite_node_sptr& node);

    // for viewing purposes
    bool hidden_;

    // shock link found
    double shock_link_found_;
};

#endif // dbskfg_transform_descriptor_h_


