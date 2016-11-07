// This is the base class for representing all nodes in the graph
#ifndef dbskfg_composite_node_h_
#define dbskfg_composite_node_h_

//:
// \file
// \brief A base class to represent shock nodes/contour nodes in the graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbgrl/dbgrl_vertex.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_ostream.h>

class dbskfg_composite_link;

class dbskfg_composite_node : public dbgrl_vertex<dbskfg_composite_link>
{

public:

    // Enumeration of all types of nodes
    enum NodeType
    {
        SHOCK_NODE,
        CONTOUR_NODE
    };

    // Constructor
    dbskfg_composite_node(vgl_point_2d<double> location, 
                          NodeType node_type,
                          unsigned int id,
                          bool virtual_flag=false);

    // Destructor
    //: inline function
    virtual ~dbskfg_composite_node(){};

    // Print out information about the node
    virtual void print(vcl_ostream& os);

    //: Return node type method
    //: inline function
    NodeType node_type(){return node_type_;}

    //: Return point location method
    //: inline function
    vgl_point_2d<double> pt(){ return pt_; }

    //: Return id of this node
    //: inline function
    unsigned int id() const{ return id_; }

    //Get the composite degree of a node
    unsigned int get_composite_degree() const {return composite_degree_;}

    //: Get the composite degree of the node
    void set_composite_degree(unsigned int degree){composite_degree_=degree;}

    //: Set whether this node is virtual
    void set_virtual(bool flag){virtual_node_ = flag;}

    //: Is this node virtual
    bool virtual_node(){return virtual_node_;}

    //: Set pt
    void set_pt(vgl_point_2d<double> pt){pt_=pt;}

protected:

    // Attributes
    vgl_point_2d<double> pt_;

private:

    // Attributes

    // Type of node this is
    NodeType node_type_;

    // Id of this node
    unsigned int id_;

    // Unsigned int composite degree of node
    // relative to contour or shock links
    unsigned int composite_degree_;

    // Stores a flag to indicate if node is virtual
    bool virtual_node_;

    // Make default ctor private
    dbskfg_composite_node();

    // Make copy constructor private
    dbskfg_composite_node(const dbskfg_composite_node&);

    // Make assignment operator private
    dbskfg_composite_node& operator=(const dbskfg_composite_node&);
};

#endif // dbskfg_composite_node_h_


