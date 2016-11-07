// This is a class to represent shock nodes in a composite graph
#ifndef dbskfg_shock_node_h_
#define dbskfg_shock_node_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a shock node in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbsk2d/dbsk2d_shock_node.h>

class dbskfg_shock_node : public dbskfg_composite_node
{

public:

    // Constructor
    dbskfg_shock_node(unsigned int id,
                      vgl_point_2d<double> location,
                      double radius=0.0);

    // Destructor

    /* virtual */ ~dbskfg_shock_node();

    /* virtual */ void print(vcl_ostream& os);

    // Methods    
    dbsk2d_shock_node::shock_node_type 
        shock_node_type() { return shock_node_type_; }

    // Set radius
    void set_radius(double radius){radius_=radius;}
    double get_radius(){return radius_;}

private:

    // Attributes
    
    // Type of shock node
    dbsk2d_shock_node::shock_node_type shock_node_type_;

    // Holds the radius of the maximum inscribed circle
    double radius_;

    // Holds a vector of phis depending on the type of node, this is the 
    // of the angle of the shock tangent with the normal to the contact point
    vcl_vector<double> phi_;

    // Make copy constructor private
    dbskfg_shock_node(const dbskfg_shock_node&);

    // Make assignment operator private
    dbskfg_shock_node& operator=(const dbskfg_shock_node&);
};

#endif // dbskfg_shock_node_h_


