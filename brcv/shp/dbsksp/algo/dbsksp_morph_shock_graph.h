// This is shp/dbsksp/dbsksp_morph_shock_graph.h
#ifndef dbsksp_morph_shock_graph_h_
#define dbsksp_morph_shock_graph_h_

//:
// \file
// \brief A class to morph between two shock graphs
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date April 5, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_map.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <vnl/vnl_vector.h>


// ============================================================================
// dbsksp_morph_shock_graph
// ============================================================================

//: A class to morph between two shock graphs
class dbsksp_morph_shock_graph
{
public:

  // ------------------ CONSTRUCTORS / DESTRUCTORS ------------------
  //: Constructor
  dbsksp_morph_shock_graph(){};
  
  //: Destructor
  virtual ~dbsksp_morph_shock_graph(){};

protected:
  
};



#endif // shp/dbsksp/dbsksp_morph_shock_graph.h









