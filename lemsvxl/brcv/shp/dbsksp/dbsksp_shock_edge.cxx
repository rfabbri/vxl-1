// This is file shp/dbsksp/dbsksp_shock_edge.cxx

//:
// \file

#include "dbsksp_shock_edge.h"
#include <dbsksp/dbsksp_shock_node.h>
#include <dbsksp/dbsksp_shock_node_descriptor.h>
#include <dbsksp/dbsksp_shock_fragment.h>

#include <vnl/vnl_math.h>



//: constructor
dbsksp_shock_edge::
dbsksp_shock_edge(const dbsksp_shock_node_sptr& source, 
                  const dbsksp_shock_node_sptr& target,
                  unsigned int id) : 
dbgrl_edge<dbsksp_shock_node>(source, target), i_visited_(0), id_(id)
{
}





//: Return and set the parameter m
double dbsksp_shock_edge::
param_m(const dbsksp_shock_node_sptr& starting_node) const
{
  if (!starting_node || starting_node == this->source())
    return this->param_m_;
  else
    return -this->param_m_;
}

void dbsksp_shock_edge::
set_param_m(double m, const dbsksp_shock_node_sptr& starting_node)
{
  if (!starting_node || starting_node == this->source())
    this->param_m_ = m;
  else
    this->param_m_ = -m;
}









// ----------------------------------------------------------------------------
//: Return and set direction of the shock chord
vgl_vector_2d<double > dbsksp_shock_edge::
chord_dir(const dbsksp_shock_node_sptr& starting_node) const
{
  if (!starting_node || starting_node == this->source())
    return this->chord_dir_;
  else
    return -this->chord_dir_;
}



// ----------------------------------------------------------------------------

//: Set direction of the shock chord
// Default is starting from source of the edge
void dbsksp_shock_edge::
set_chord_dir(const vgl_vector_2d<double >& direction, 
                    const dbsksp_shock_node_sptr& starting_node )
{
  if (!starting_node || starting_node == this->source())
    this->chord_dir_ = normalized(direction);
  else
    this->chord_dir_ = -normalized(direction);
}


// ----------------------------------------------------------------------------
//: Return true if `this' is a terminal edge (A_^1-A_\infty)
bool dbsksp_shock_edge::
is_terminal_edge() const
{
  return this->source()->degree()==1 || this->target()->degree()==1;
}


// \NOTE may need to removed since the function has been implemented in dbgrl
//: Determine if ``this'' edge shares a vertex with other edge
// Return the shared node if so, otherwise return 0
dbsksp_shock_node_sptr dbsksp_shock_edge::
shared_vertex(const dbsksp_shock_edge& other) const
{
  if (this->source() == other.source() || this->source() == other.target())
    return this->source();
  if (this->target() == other.source() || this->target() == other.target())
    return this->target();
  return 0;
}


// ROOTED TREE functions ----------------------------------------------------

// -----------------------------------------------------------------------------
//: Return the ``child'' wrt of this edge
dbsksp_shock_node_sptr dbsksp_shock_edge::
child_node() const
{
  return (this->source()->parent_edge() == this) ? 
    this->source() : this->target();
}



// -----------------------------------------------------------------------------
//: Return ``parent'' node wrt this edge
dbsksp_shock_node_sptr dbsksp_shock_edge::
parent_node() const
{
  return (this->source()->parent_edge() == this) ? 
    this->target() : this->source();
}





// UTILITY FUNCTIONS ===========================================================

//: Set values of delta_r from info of the source and target nodes
void dbsksp_shock_edge::
compute_radius_increment_from_nodes()
{
  dbsksp_shock_node_descriptor_sptr d_source = 
    this->source()->descriptor(this);
  dbsksp_shock_node_descriptor_sptr d_target = 
    this->target()->descriptor(this);

  double a0 = d_source->alpha + d_source->phi;
  double a2 = -d_target->alpha + d_target->phi;

  //// replace the following formula by its equavalence but avoiding dividing 0
  //double dr_over_len = (vcl_sin(a0) - vcl_sin(a2)) / 
  //  vcl_sin(vnl_math::pi - a0 - a2);

  double dr_over_len = vcl_sin((a0-a2)/2) / vcl_sin((a0+a2)/2);
  this->set_radius_increment(dr_over_len * this->chord_length());
}



//: Form a shock fragment for this edge
void dbsksp_shock_edge::
form_fragment()
{
  if (!this->fragment())
    this->set_fragment(new dbsksp_shock_fragment());
  this->fragment()->build_from(this);
  
  return;
}



  // =============== MISCELLANEOUS =====================================

//: write info of the dbskbranch to an output stream
void dbsksp_shock_edge::
print(vcl_ostream & os)
{
  os << "\n<<\n"
    << "type[ " << this->is_a() << " ] id[ " << this->id() << " ]\n"
    << "source_id[ " << this->source()->id() 
    << " ] target_id[ " << this->target()->id()<< " ]\n"
    << "param_m[ " << this->param_m() << " ]\n"
    << "chord_length[ " << this->chord_length() << " ]\n>>\n";
};



