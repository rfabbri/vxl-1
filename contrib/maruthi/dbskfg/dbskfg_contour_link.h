// This is a class to represent contour links in a composite graph
#ifndef dbskfg_contour_link_h_
#define dbskfg_contour_link_h_

//:
// \file
// \brief A class to represent all the geometry/classification needed for
//        a contour link in a composite graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_utilities.h>
#include <vcl_vector.h>

class dbskfg_shock_link;

class dbskfg_contour_link : public dbskfg_composite_link
{

public:

    // Constructor
    dbskfg_contour_link(dbskfg_composite_node_sptr source,
                        dbskfg_composite_node_sptr target,
                        unsigned int id);

    // Destructor

    /* virtual */ ~dbskfg_contour_link();

    /* virtual */ void print(vcl_ostream& os);

    // Methods

    // Add shock links affected
    void shock_links_affected(dbskfg_shock_link* shock_link);

    // Grab all shock links affected
    vcl_vector<dbskfg_shock_link*> shocks_affected()
    {return shocks_affected_;}

    // Add original contour id
    void set_original_contour_id(int id){contour_id_ = id;}
    
    // Get original contour id
    int contour_id(){return contour_id_;}

    void delete_shock(unsigned int id);

private:

    // Attributes
    // Keep original id from previous run
    int contour_id_;

    // Keep a vector of all shock links affected by this node
    vcl_vector<dbskfg_shock_link*> shocks_affected_;

    // Make copy constructor private
    dbskfg_contour_link(const dbskfg_contour_link&);

    // Make assignment operator private
    dbskfg_contour_link& operator=(const dbskfg_contour_link&);

};

#endif // dbskfg_contour_link_h_


