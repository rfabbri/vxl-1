// This is a class to represent shock links in a composite graph
#ifndef dbskfg_shock_link_h_
#define dbskfg_shock_link_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a shock link in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_utilities.h>
#include <dbskfg/dbskfg_shock_link_boundary.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_vector.h>
#include <vcl_utility.h>
#include <vcl_set.h>

#include <dbsk2d/dbsk2d_xshock_sample.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>

class dbskfg_contour_node;
class dbskfg_contour_link;

class dbskfg_shock_link : public dbskfg_composite_link
{

public:

    enum ShockType{
        CONTACT_SHOCK,
        SHOCK_RAY,
        SHOCK_EDGE
    };

    // Constructor
    dbskfg_shock_link(dbskfg_composite_node_sptr source,
                      dbskfg_composite_node_sptr target,
                      unsigned int id,
                      ShockType shock_type,
                      dbskfg_utilities::Fragment_Type type=
                      dbskfg_utilities::REGULAR,
                      int original_ishock_id=-1);

    // Destructor

    /* virtual */ ~dbskfg_shock_link();

    

    // Methods

    //----Information about this Link ---------------------------------------

    //: Prints information about this node
    /* virtual */ void print(vcl_ostream& os);

    //: Returns whether this shock link is line/line, point/line, etc
    dbskfg_utilities::Shock_Compute_Type shock_compute_type();

    //: Return the type of link this is method shock ray,shock edge , etc
    ShockType shock_link_type() { return shock_link_type_; }

    //---- Contour Methods ---------------------------------------------------
    // This section contains methods that update the contours for this link
    // and retreives the contours

    //: Add to left link
    void add_to_left_contour(dbskfg_composite_link_sptr link)
    {if(!dbskfg_utilities::is_link_in_set_of_links(left_contour_links_,link))
        {left_contour_links_.push_back(link);}}
    
    //: Get left contours
    vcl_vector<dbskfg_composite_link_sptr>& left_contour_links()
    {return left_contour_links_;}

    //: Add to right contour
    void add_to_right_contour(dbskfg_composite_link_sptr link)
    {if(!dbskfg_utilities::is_link_in_set_of_links(right_contour_links_,link))
        {right_contour_links_.push_back(link);}}
   
    //: Get right contours
    vcl_vector<dbskfg_composite_link_sptr>& right_contour_links()
    {return right_contour_links_;}

    //: Add left pont
    void add_left_point(dbskfg_contour_node* node)
    {left_point_=node;}

    //: Get left point
    dbskfg_contour_node* get_left_point(){return left_point_;}

    //: Add right point
    void add_right_point(dbskfg_contour_node* node)
    {right_point_=node;}
    
    //: Get right point
    dbskfg_contour_node* get_right_point(){return right_point_;}

    //: form shock fragment
    void form_shock_fragment();

    //: get polygon
    vgl_polygon<double> polygon(){ return polygon_boundary_; }

    //: return the extrinsic points for rendering this geometry
    vcl_vector<vgl_point_2d<double> >& ex_pts() { return ex_pts_; }

    //: set polygon
    void set_polygon(vgl_polygon<double> poly);

    //: set extrinsinc points
    void set_sampled_shock_points(
        vcl_vector<dbsk2d_xshock_sample_sptr>& samples){samples_=samples;}

    //: set extrinsinc points
    vcl_vector<dbsk2d_xshock_sample_sptr>& get_sampled_shock_points()
    {return samples_;}

    // get original shock id
    int get_original_shock_id(){return original_ishock_id_;}

    //: Function tell whether this contour id spawned this shock
    //: from either left or right side
    bool contour_spawned_shock(dbskfg_composite_link_sptr clink,
                               dbskfg_utilities::Orientation& dir);

    //: Grab fragment type
    dbskfg_utilities::Fragment_Type fragment_type()
    {return fragment_type_;}

    //: Endpoint spawned
    bool endpoint_spawned();

    //: Get contour pair
    vcl_set<unsigned int> get_contour_pair();

    //: Figure out shock extrinsinc points for this link
    void construct_locus();

    //: Figure out shock
    void compute_shock(dbsk2d_ishock_edge** edge,double scale_ratio=1.0);

    //: Add rag node apart of 
    void set_rag_node(dbskfg_rag_node_sptr rag_node)
    {rag_node_ = rag_node;}
    
    dbskfg_rag_node_sptr get_rag_node(){return rag_node_;}

    void set_left_boundary(dbskfg_shock_link_boundary left_boundary)
    {left_boundary_ = left_boundary; }

    void set_right_boundary(dbskfg_shock_link_boundary right_boundary)
    {right_boundary_ = right_boundary; }


    dbskfg_shock_link_boundary get_left_boundary()
    {return left_boundary_;}

    dbskfg_shock_link_boundary get_right_boundary()
    {return right_boundary_;}

    void splice_cost(vcl_vector<double>& costs);

    dbsk2d_ishock_edge* ishock_edge(){return elm_;}

private:

    // Attributes

    //: Type of shock link
    ShockType shock_link_type_;

    // Type of fragment either regular or degenarte
    dbskfg_utilities::Fragment_Type fragment_type_;

    //: Represent this as two boundary objects
    dbskfg_shock_link_boundary left_boundary_;
    
    //: Represent right boundary
    dbskfg_shock_link_boundary right_boundary_;

    // If a point line combination have a left point
    dbskfg_contour_node* left_point_;

    // If we have a point line combination we have a right point
    dbskfg_contour_node* right_point_;

    // Keeps a list of all contour links for the left element
    vcl_vector<dbskfg_composite_link_sptr> left_contour_links_;

    // Keeps a list of all contour links for the right element
    vcl_vector<dbskfg_composite_link_sptr> right_contour_links_;

    // Keeps a unified list of all elements for degenerate fragment
    vcl_vector<dbskfg_composite_link_sptr> degenerate_links_;

    //: extrinsic points for drawing purposes
    vcl_vector<vgl_point_2d<double> > ex_pts_;

    // Keeps the whole fragment in a polygon
    vgl_polygon<double> polygon_boundary_;

    // The rag node that this shock link is a part of
    dbskfg_rag_node_sptr rag_node_;

    // Keep an ishock elm
    dbsk2d_ishock_edge* elm_;

    vcl_vector<dbsk2d_ishock_belm*> belm_;

    vcl_vector<dbsk2d_xshock_sample_sptr> samples_;

    // store original ishock id
    int original_ishock_id_;

    // Private methods

    // Find regular fragment
    void form_regular_fragment();
   
    // Form degenerate fragment
    void form_degenerate_fragment();

    // Finding start pair for polygon
    vcl_pair< vgl_point_2d<double>,vgl_point_2d<double> >
        find_start_pair_of_polygon(dbskfg_composite_link_sptr clink,
                                   unsigned int& target_id,
                                   dbskfg_utilities::Orientation dir);

    // Make copy constructor private
    dbskfg_shock_link(const dbskfg_shock_link&);

    // Make assignment operator private
    dbskfg_shock_link& operator=(const dbskfg_shock_link&);

};

#endif // dbskfg_shock_link_h_


