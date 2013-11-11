// This is brcv/shp/dbskfg/algo/dbskfg_loop_transforms.h
#ifndef dbskfg_loop_transforms_h_
#define dbskfg_loop_transforms_h_
//:
// \file
// \brief Algorithm to detect loop transforms 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>

class dbskfg_shock_link;

//: Form Composite Graph algorithm
class dbskfg_loop_transforms
{

public:

    //: Constructor
    dbskfg_loop_transforms
        ( dbskfg_composite_graph_sptr composite_graph,
          vil_image_resource_sptr img_sptr);
          
    //: Destructor
    ~dbskfg_loop_transforms();

    // Set alpha for weighting of contour and app cost
    void set_alpha(double alpha){alpha_ = alpha;}

    // Called to detect all loops
    void detect_loops(vcl_vector<dbskfg_transform_descriptor_sptr>& objects);

    // Detect loops with a contour id endpoint
    void detect_loops(
        vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
        vcl_vector<dbskfg_composite_node_sptr>& con_endpoints);


    // Detect loops if given start and stop endpoint
    void detect_loops(dbskfg_transform_descriptor_sptr& transform);

private:

    // Attributes
    double longest_contour_;

    //: store image
    vil_image_resource_sptr image_; 

    // Keep the three views that we convert to
    // Look in lab color space
    vil_image_view<float> L_, A_, B_; 

    // color gamma for distance
    double color_gamma_;

    // alph for weighting contour and app cost
    double alpha_;

    // Determine appearance_cost
    double determine_app_cost(vgl_polygon<double>& poly1,
                              vgl_polygon<double>& poly2);

    // Call to expand node
    void expand_node(dbskfg_composite_node_sptr node,
                     vcl_vector<dbskfg_composite_node_sptr>& stack,
                     vcl_map<unsigned int,vcl_string>& visited_nodes,
                     vcl_map<unsigned int,vcl_string>& visited_links,
                     vcl_map<unsigned int,dbskfg_shock_link*>& shock_map);

    // Call to expand link
    void expand_link(dbskfg_composite_link_sptr link,
                     vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
                     vcl_map<unsigned int,vcl_string>& visited_links);

    // Determine contour cost
    bool determine_contour_cost(dbskfg_transform_descriptor_sptr& transform,
                                vgl_polygon<double>& poly1,
                                vgl_polygon<double>& poly2);

    // Keep a composite graph
    dbskfg_composite_graph_sptr composite_graph_;

    // Make copy ctor private
    dbskfg_loop_transforms(const dbskfg_loop_transforms&);

    // Make assign operator private
    dbskfg_loop_transforms& operator
        =(const dbskfg_loop_transforms& );


};

#endif //dbskfg_loop_transforms_h_
