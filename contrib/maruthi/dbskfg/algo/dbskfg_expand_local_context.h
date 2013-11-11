// This is brcv/shp/dbskfg/algo/dbskfg_expand_local_context.h
#ifndef dbskfg_expand_local_context_h_
#define dbskfg_expand_local_context_h_
//:
// \file
// \brief This class serves as an object to see if local context should
// be expanded
// \author Maruthi Narayanan
// \date 07/11/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/11/2010    Initial version.
//
// \endverbatim 

#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>

class dbskfg_shock_link;
class dbskfg_contour_node;

//: Expands the minimal context polygon of a transform
class dbskfg_expand_local_context
{

public:

    //: Constructor
    dbskfg_expand_local_context
        ( dbskfg_transform_descriptor_sptr transform);

    //: Destructor
    ~dbskfg_expand_local_context();
  
private:

    // Determine Distance to Euler Spiral
    double distance_from_ess(vgl_point_2d<double> test_point);

    // Looks at polygon and keeps iterating until polygon stops growing
    void expand_local_context();

    // Expands the polygon by expanding each node
    void expand_polygon();

    // If a node is expanded lets add contours
    void add_contours(dbskfg_shock_link* shock_link);

    // We need to expand minimal context polygon
    void add_to_polygon(dbskfg_shock_link* shock_link);

    // Hold Transform object
    dbskfg_transform_descriptor_sptr transform_;

    // Keep track of degree one objects
    vcl_map<unsigned int,dbskfg_contour_node*> degree_ones_;

    // Make copy ctor private
    dbskfg_expand_local_context(const dbskfg_expand_local_context&);

    // Make assign operator private
    dbskfg_expand_local_context& operator
        =(const dbskfg_expand_local_context& );

   
};

#endif //dbskfg_expand_local_context_h
