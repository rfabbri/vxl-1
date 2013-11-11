// This is the base class for representing all links in the graph
#ifndef dbskfg_composite_link_h_
#define dbskfg_composite_link_h_

//:
// \file
// \brief A base class to represent shock links/contour links in the graph
//  
// \author Maruthi Narayanan
// \date  06/27/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_edge.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <vcl_ostream.h>

class dbskfg_composite_link : public dbgrl_edge< dbskfg_composite_node >
{

public:

    // Enumeration of all types of links
    enum LinkType
    {
        SHOCK_LINK,
        CONTOUR_LINK
    };


    //: Constructor with source and target
    dbskfg_composite_link(dbskfg_composite_node_sptr source,
                          dbskfg_composite_node_sptr target,
                          LinkType type=SHOCK_LINK,
                          unsigned int id=0);

    // Destructor
    virtual ~dbskfg_composite_link(){}

    // Print out information about this node
    virtual void print(vcl_ostream &os);

    //: Return type of link
    //: inline function
    LinkType link_type(){ return link_type_;}

    //: Return the id type
    //: inline function
    unsigned int id() const { return id_; }

protected:

    // Attributes
    unsigned int id_;

private:
    
    // Typ of link that this is
    LinkType link_type_;

    // Make private default constructor
    dbskfg_composite_link();

    // Make copy constructor private
    dbskfg_composite_link(const dbskfg_composite_link&);

    // Make assignment operator private
    dbskfg_composite_link& operator=(const dbskfg_composite_link&);
};

#endif // dbskfg_composite_link_h_


