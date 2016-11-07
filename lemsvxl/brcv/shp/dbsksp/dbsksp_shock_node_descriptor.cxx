// This is file shp/dbsksp/dbsksp_shock_node_descriptor.cxx

//:
// \file

#include "dbsksp_shock_node_descriptor.h"
#include <vnl/vnl_math.h>

// ============================================================================
//             dbsksp_shock_node_descriptor
// ============================================================================

// ----------------------------------------------------------------------------
//: Default Constructor
dbsksp_shock_node_descriptor::
dbsksp_shock_node_descriptor(): edge (0), rot_angle_to_succ_edge(2*vnl_math::pi),
  shock_flow_dir(1),
  alpha(0),
  phi(vnl_math::pi)
{
}


// -----------------------------------------------------------------------------
//: set shock flow direction from the value of angle phi
void dbsksp_shock_node_descriptor::
set_shock_flow_dir_from_phi()
{
  if (this->phi > vnl_math::pi_over_2)
    this->shock_flow_dir = 1;
  else if (this->phi < vnl_math::pi_over_2)
    this->shock_flow_dir = -1;
  else
    this->shock_flow_dir = 0;
}



