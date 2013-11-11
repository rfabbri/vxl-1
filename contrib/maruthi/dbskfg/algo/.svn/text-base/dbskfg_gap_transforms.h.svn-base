// This is brcv/shp/dbskfg/algo/dbskfg_gap_transforms.h
#ifndef dbskfg_gap_transforms_h_
#define dbskfg_gap_transforms_h_
//:
// \file
// \brief Algorithm to detect gap transforms 
// \author Maruthi Narayanan
// \date 07/08/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/08/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <vcl_vector.h>

class dbskfg_contour_node;
class dbskfg_shock_link;

//: Form Composite Graph algorithm
class dbskfg_gap_transforms
{

public:

    //: Constructor
    dbskfg_gap_transforms
        ( dbskfg_composite_graph_sptr composite_graph,
          vil_image_resource_sptr img_sptr);
          
    //: Destructor
    ~dbskfg_gap_transforms();
  
    // Called to detect all gaps
    void detect_gaps(vcl_vector<dbskfg_transform_descriptor_sptr>& objects);

    // Called to detect all gaps
    void detect_gaps_single(
        vcl_vector<dbskfg_transform_descriptor_sptr>& objects);

    // Detect gaps with single endpoint
    void detect_gaps_endpoint(
        vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
        vcl_vector<dbskfg_composite_node_sptr>& endpoints);

    // Set ess completion parameter
    void set_ess_completion(double ess){ess_completion_ = ess;}

    // Set alpha for weighting of contour and app cost
    void set_alpha(double alpha){alpha_ = alpha;}

    void detect_gaps_knn(vcl_vector<dbskfg_transform_descriptor_sptr>& objects,
                         unsigned int k);

  
private:

    // Keep a composite graph
    dbskfg_composite_graph_sptr composite_graph_;

    //: store image
    vil_image_resource_sptr image_; 

    // Keep the three views that we convert to
    // Look in lab color space
    vil_image_view<float> L_, A_, B_; 

    // Keep parameters for cost
    double curve_offset_;
    
    double curve_length_gamma_;

    double curve_power_;
    
    double curve_gamma_;

    double color_gamma_;

    // Ess completion
    double ess_completion_;

    // Alpha parameter
    double alpha_;

    // double largest gap will be diagonal
    double largest_gap_;

    // Check valid euler spiral
    bool valid_euler_spiral(dbskfg_transform_descriptor_sptr& transform);

    // Helper function to detect gaps
    bool detect_gaps_helper(dbskfg_shock_link* slink,
                            vcl_vector<dbskfg_transform_descriptor_sptr>
                            & objects);
  
    // Helper function to detect gaps
    bool detect_gaps_helper(dbskfg_composite_node_sptr pt1,
                            dbskfg_composite_node_sptr pt2,
                            vcl_vector<dbskfg_transform_descriptor_sptr>
                            & objects);

    // Helper function to detect gaps
    void detect_gap_4_helper(dbskfg_contour_node* endpoint,
                             vcl_vector<dbskfg_transform_descriptor_sptr>
                             & objects);
    
    // Determine appearance_cost
    double determine_app_cost(vgl_polygon<double>& poly1,
                              vgl_polygon<double>& poly2);

    // Determine constrast polygons
    void determine_contrast_polygons(
        dbskfg_transform_descriptor_sptr& transform,
        dbskfg_shock_link* slink,
        vgl_polygon<double>& poly1,vgl_polygon<double>& poly2);

    // Compute appearance
    
    // Make copy ctor private
    dbskfg_gap_transforms(const dbskfg_gap_transforms&);

    // Make assign operator private
    dbskfg_gap_transforms& operator
        =(const dbskfg_gap_transforms& );

   
};

#endif //dbskfg_gap_transforms_h_
