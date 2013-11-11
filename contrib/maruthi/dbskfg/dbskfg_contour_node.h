// This is a class to represent contour nodes in a composite graph
#ifndef dbskfg_contour_node_h_
#define dbskfg_contour_node_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a contour node in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>

class dbskfg_shock_link;

class dbskfg_contour_node : public dbskfg_composite_node
{

public:

    // Enumeration of all contour types
    enum ContourType
    {
        ISOLATED_POINT,
        END_POINT,
        T_JUNCTION,
        Y_JUNCTION,
        L_JUNCTION,
        X_JUNCTION,
        I_JUNCTION
    };

    // Constructor
    dbskfg_contour_node(unsigned int id, 
                        vgl_point_2d<double> location,
                        int contour_id);

    // Destructor

    /* virtual */ ~dbskfg_contour_node();

    /* virtual */void print(vcl_ostream& os);

    // Methods    
    ContourType contour_node_type() { return contour_node_type_; }

    // Return the original contour of this node
    int contour_id(){return contour_id_;}

    // Set the original contour of this node
    void set_contour_id(int contour_id){contour_id_=contour_id;}

    // Set tangent at this node
    void set_tangent(double angle){tangent_ = angle;}

    // Get tangent angle at this node
    double get_tangent(){return tangent_;}

    // Add shock links affected
    void shock_links_affected(dbskfg_shock_link* shock_link);

    // Return shock links affected
    vcl_vector<dbskfg_shock_link*> shocks_affected()
    {return shocks_affected_;}

    void delete_shock(unsigned int id);

    // Determine degenerate shocks
    dbskfg_composite_link_sptr degenerate_shock();

private:

    // Attributes
    int contour_id_;

    // Tangent at this node
    double tangent_;

    // Type of contour node
    ContourType contour_node_type_;

    // Keep a vector of all shock links affected by this node
    vcl_vector<dbskfg_shock_link*> shocks_affected_;

    // Make copy constructor private
    dbskfg_contour_node(const dbskfg_contour_node&);

    // Make assignment operator private
    dbskfg_contour_node& operator=(const dbskfg_contour_node&);
};

#endif // dbskfg_contour_node_h_


