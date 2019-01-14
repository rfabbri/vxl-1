//this is /contrib/bm/dvrml/dvrml_indexed_lineset.h
#ifndef dvrml_indexed_lineset_h_
#define dvrml_indexed_lineset_h_
//:
// \file
// \date August 18, 2010
// \author Brandon A. Mayer
//
// Class to draw out indexed linesets. The point list is a list of vgl points to be include in the lineset. The connection
// list is a list of unsigned ints that signify which points are to be connected by line segments. The point_list must have size >=2.
//
// \verbatim
//  Modifications
// \endverbatim

#include"dvrml_shape.h"

class dvrml_indexed_lineset: public dvrml_shape
{
public:
    dvrml_indexed_lineset(){}

    typedef std::vector<vgl_point_3d<double> > point_list_type;
    typedef std::vector<std::vector<unsigned> > connection_list_type;
    typedef std::vector<dvrml_appearance_sptr> appearance_list_type;

    dvrml_indexed_lineset( point_list_type point_list,
                           connection_list_type connection_list,
                           appearance_list_type appearance_list ): 
        lineset_point_list(point_list), lineset_connection_list(connection_list), lineset_appearance_list(appearance_list){}

    dvrml_indexed_lineset( point_list_type point_list, connection_list_type connection_list): 
        lineset_point_list(point_list), lineset_connection_list(connection_list){}

    ~dvrml_indexed_lineset(){}

    std::vector<vgl_point_3d<double> >lineset_point_list;
    std::vector<std::vector<unsigned> >lineset_connection_list;
    std::vector<dvrml_appearance_sptr> lineset_appearance_list;
  
    virtual void draw( std::ostream& os );

    virtual std::string shape(){ return "indexed lineset";}
};

#endif //dvrml_indexed_lineset_h_