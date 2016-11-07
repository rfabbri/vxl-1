// This is shp/dbsksp/dbsksp_shock_node_descriptor.h
#ifndef dbsksp_shock_node_descriptor_h_
#define dbsksp_shock_node_descriptor_h_

//:
// \file
// \brief A class for a shock node descriptor
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Dec 2, 2008
// \verbatim
//  Modifications
// \endverbatim

#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_vector_2d.h>


//: This class describes property of a node, associated with each edge it is incident to
class dbsksp_shock_node_descriptor : public vbl_ref_count
{
public:

  //: Default Constructor
  dbsksp_shock_node_descriptor();

  //: Destructor
  ~dbsksp_shock_node_descriptor() {}

  //: set shock flow direction from the value of angle phi
  void set_shock_flow_dir_from_phi();

public:

  //: The shock edge corresponding to this parameter
  dbsksp_shock_edge_sptr edge; 
  
  //: rotation angle to go from chord direction of this `edge'
  // to chord direction of successor edge (CCW)
  double rot_angle_to_succ_edge;

  //: shock flow direction of `edge' at `this' node
  // -1: incoming, 1: outgoing
  int shock_flow_dir;

  //: angle between the chord and the shock
  double alpha;

  //: shock attack angle (between shock direction and normal vectors to the boundary
  double phi; 

  //: shock tangent - added for symmetric shock snake, 
  // never computed as dependent parameters
  vgl_vector_2d<double > tangent;
};

#endif // shp/dbsksp/dbsksp_shock_node_descriptor.h
