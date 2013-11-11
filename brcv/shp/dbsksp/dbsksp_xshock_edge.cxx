// This is file shp/dbsksp/dbsksp_xshock_edge.cxx

//:
// \file

#include "dbsksp_xshock_edge.h"
#include <vnl/vnl_math.h>

#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>

//: constructor with two nodes
dbsksp_xshock_edge::
dbsksp_xshock_edge(const V_sptr& source, const V_sptr& target, unsigned id):
  dbgrl_edge<dbsksp_xshock_node>(source, target), id_(id)
{
}

// -----------------------------------------------------------------------------
//: Return true if `this' is a terminal edge (A_^1-A_\infty)
bool dbsksp_xshock_edge::
is_terminal_edge() const
{
  return this->source()->degree()==1 || this->target()->degree()==1;
}


// GEOMETRY=====================================================================

//: Return a fragment (influence zone) associated with this edge
dbsksp_xshock_fragment_sptr dbsksp_xshock_edge::
fragment()
{
  dbsksp_xshock_node_descriptor start = *(this->source()->descriptor(this));
  dbsksp_xshock_node_descriptor end = this->target()->descriptor(this)->opposite_xnode();
  return new dbsksp_xshock_fragment(start, end);
}




// -----------------------------------------------------------------------------
//: Return parent node
dbsksp_xshock_edge::V_sptr dbsksp_xshock_edge::
parent_node() const
{
  if (this->source()->depth() == this->target()->depth()) return 0;
  return (this->source()->depth() < this->target()->depth()) ? 
    this->source() : this->target();
}


// -----------------------------------------------------------------------------
//: Return child node
dbsksp_xshock_edge::V_sptr dbsksp_xshock_edge::
child_node() const
{
  if (this->source()->depth() == this->target()->depth()) return 0;
  return (this->source()->depth() < this->target()->depth()) ?
    this->target() : this->source();
}


  
// UTILITY FUNCTIONS ---------------------------------------------------------


//: write info of the dbskbranch to an output stream
vcl_ostream& dbsksp_xshock_edge::
print(vcl_ostream& os)
{
  dbsksp_xshock_node_descriptor* xd_source = this->source()->descriptor(this);
  dbsksp_xshock_node_descriptor* xd_target = this->target()->descriptor(this);
  if (!xd_source || !xd_target)
  {
    os << "\nERROR: node descriptor at either source or target of edge (id = " 
      << this->id() << ")" << " is not available.\n";
    return os;
  }
  vgl_vector_2d<double > chord = xd_target->pt() - xd_source->pt();
  double chord_length = chord.length();
  vgl_vector_2d<double > chord_dir = (chord_length > 1e-5)? chord : 
    xd_source->shock_tangent();

  double source_alpha = signed_angle(chord_dir, xd_source->shock_tangent());
  double target_alpha = signed_angle(-chord_dir, xd_target->shock_tangent());

  os << "\n<<\n"
    << "type[ " << this->is_a() << " ] id[ " << this->id() << " ]\n"
    << "  source_id[ " << this->source()->id() << " ]"
    << "  target_id[ " << this->target()->id()<< " ]\n"
    << "  source_alpha[ " << source_alpha << " ]"
    << "  target_alpha[ " << target_alpha << " ]\n" 
    << "  chord_length[ " << chord_length << " ]"
    << ">>\n";
  return os;
}



