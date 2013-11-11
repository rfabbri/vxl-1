// This is a class to represent containment links in a composite graph
#ifndef dbskfg_containment_link_h_
#define dbskfg_containment_link_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a containment link in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_edge.h>
#include <vcl_ostream.h>
#include <dbskfg/algo/dbskfg_transform_descriptor.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <con_graph/dbskfg_containment_node_sptr.h>

class dbskfg_containment_link : public dbgrl_edge<dbskfg_containment_node>
{

public:

    // Constructor
    dbskfg_containment_link(
        dbskfg_containment_node_sptr source,
        dbskfg_containment_node_sptr target,
        dbskfg_transform_descriptor_sptr transform=0);

    // Destructor
    ~dbskfg_containment_link();

    // Returns the type of transform
    dbskfg_transform_descriptor::TransformType transform_type()
    {return transform_type_;}

    // Set contours affected
    void set_contours_affected(vcl_vector<unsigned int> contours);

    // Set distance
    void set_distance(double distance){distance_ = distance;}

    // get contours affected
    vcl_vector<unsigned int> get_contours_affected(){
        return contours_affected_;}

    // Returns the transform object associated with this link
    dbskfg_transform_descriptor_sptr transform(){return transform_;}

    // Returns the grouped transform
    dbskfg_transform_descriptor_sptr grouped_transform(){
        return grouped_transform_;}

    // Determine grouped or not transforms
    void determine_group_transform();

    // Grab the gap id
    unsigned int get_new_contour_id(){return new_contour_id_;}

    // Grab the distance
    double get_distance(){return distance_;}

    // Return the cost of this link ( transform )
    double cost(){return cost_;}

    // Prints out information about this node
    void print(vcl_ostream& os);

    // Destroy transform
    void destroy_transform()
    {
        if ( transform_ )
        {
            if ( transform_->get_references() == 1 )
            {

                transform_->destroy_transform();
            }
            transform_=0;
        }

        if ( grouped_transform_)
        {
            if ( grouped_transform_->get_references() == 1 )
            {

                grouped_transform_->destroy_transform();
            }
            grouped_transform_=0;
        }
        link_size_=0;
    }

    double link_size(){return link_size_;}

private:

    // Return transform
    dbskfg_transform_descriptor_sptr transform_;

    // Return transform
    dbskfg_transform_descriptor_sptr grouped_transform_;

    // Store contours affected
    vcl_vector<unsigned int> contours_affected_;

    // Store the cost of this node 
    double cost_;

    // Store the transform type
    dbskfg_transform_descriptor::TransformType transform_type_;

    // Stores the new contour added , makes sense only for gaps
    unsigned int new_contour_id_;
   
    double link_size_;

    double distance_;

    // Make copy constructor private
    dbskfg_containment_link(const dbskfg_containment_link&);

    // Make assignment operator private
    dbskfg_containment_link& operator=(const dbskfg_containment_link&);
};

#endif // dbskfg_containment_link_h_


