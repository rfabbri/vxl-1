// This is the base class for all links in the region adjancency graph
#ifndef dbskfg_rag_link_h_
#define dbskfg_rag_link_h_

//:
// \file
// \brief A base class to represent links in the rag graph
//  
// \author Maruthi Narayanan
// \date  07/12/2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbgrl/dbgrl_edge.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>

class dbskfg_rag_link : public dbgrl_edge< dbskfg_rag_node >
{

public:

    //: Constructor without node correspondences
    dbskfg_rag_link(unsigned int id);

    //: Constructor with source and target
    dbskfg_rag_link(dbskfg_rag_node_sptr source,
                    dbskfg_rag_node_sptr target,
                    unsigned int id=0);

    // Destructor
    virtual ~dbskfg_rag_link(){}

    //: Return the id type
    //: inline function
    unsigned int id(){ return id_; }
    
protected:

    // Attributes
    unsigned int id_;

private:
    
    // Make copy constructor private
    dbskfg_rag_link(const dbskfg_rag_link&);

    // Make assignment operator private
    dbskfg_rag_link& operator=(const dbskfg_rag_link&);
};

#endif // dbskfg_rag_link_h_


