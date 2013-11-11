// This is file shp/dbsksp/dbsksp_shock_node.cxx

//:
// \file

#include "dbsksp_shock_node.h"
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_node_descriptor.h>

#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>



// ============================================================================
//             dbsksp_shock_node
// ============================================================================


// ----------------------------------------------------------------------------
//: Return descriptor associated with an edge
dbsksp_shock_node_descriptor_sptr dbsksp_shock_node::
descriptor(const dbsksp_shock_edge_sptr& e)
{
  for(vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator it =
    this->descriptor_list_.begin(); it != this->descriptor_list_.end(); ++it)
  {
    assert (*it);
    if ((*it)->edge == e)
    {
      return (*it);
    }
  }
  return 0;
}



// ----------------------------------------------------------------------------
//: These functions should be moved to the graph vertex class

//: add an edge to the end of the list
void dbsksp_shock_node::
add_edge(const E_sptr& e)
{
  dbsksp_shock_node_descriptor_sptr descriptor = new dbsksp_shock_node_descriptor();
  descriptor->edge = e;
  this->add_incoming_edge(e);
  this->descriptor_list_.push_back(descriptor);
}

//: remove an edge, and its descriptor from the edge list
bool dbsksp_shock_node::
remove_shock_edge(const E_sptr& e)
{
  // remove both the edge and its descriptor
  if (this->del_edge(e))
  {
    for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator itr =
      this->descriptor_list_.begin(); 
      itr != this->descriptor_list_.end(); ++itr)
    {
      if ((*itr)->edge == e)
      {
        // edge found
        this->descriptor_list_.erase(itr);
        return true;
      }
    }
    return true;
  }

  return false;
}


//: remove all edges and their descriptors
void dbsksp_shock_node::
remove_all_shock_edges()
{
  this->in_edges_.clear();
  this->descriptor_list_.clear();
}

// ----------------------------------------------------------------------------
//: add an edge right BEFORE a reference edge (in the edge list)
dbsksp_shock_node_descriptor_sptr dbsksp_shock_node::
insert_shock_edge(const E_sptr& new_edge, const E_sptr& ref_edge)
{
  assert(new_edge);

  // insert the edge
  // find location of the ref_edge in the edge list
  vcl_list<E_sptr >::iterator eit = vcl_find(this->in_edges_.begin(),
    this->in_edges_.end(), ref_edge);

  if (eit == this->in_edges_.end())
  {
    vcl_cerr << "ERROR: in dbsksp_shock_node::insert_edge(...)\n" 
      << "reference edge is not incident to this node.\n";
    return 0;
  }
  this->in_edges_.insert(eit, new_edge); 


  // insert the node descriptor
  dbsksp_shock_node_descriptor_sptr descriptor = new dbsksp_shock_node_descriptor();
  descriptor->edge = new_edge;
  
  vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator dit = 
    vcl_find(this->descriptor_list_.begin(), this->descriptor_list_.end(),
    this->descriptor(ref_edge));
  this->descriptor_list_.insert(dit, descriptor);

  // Return
  return descriptor;
}





// ----------------------------------------------------------------------------
//: Check if this node is connected to a terminal edge
bool dbsksp_shock_node::
connected_to_terminal_edge() const
{
  vcl_list<dbsksp_shock_edge_sptr > connected_edges = this->edge_list();
  for (vcl_list<dbsksp_shock_edge_sptr >::iterator itr = connected_edges.begin();
    itr != connected_edges.end(); ++itr)
  {
    dbsksp_shock_edge_sptr e = *itr;
    if (e->is_terminal_edge()) return true;
  }
  return false;
}




// ----------------------------------------------------------------------------
//: Change the phi angle of one edge and uniformly distribute the difference 
// it causes
void dbsksp_shock_node::
change_phi_distribute_difference_uniformly(const dbsksp_shock_edge_sptr& e, 
                                           double new_phi)
{
  assert(this->degree() > 1);
  assert(e->is_vertex(this));

  dbsksp_shock_node_descriptor_sptr d = this->descriptor(e);

  double old_phi = d->phi;
  double diff_per_edge = (new_phi - old_phi) / (this->degree() - 1);
  d->phi = new_phi;

  // iterate thru the descriptors and adjust the phi
  vcl_list<dbsksp_shock_node_descriptor_sptr > dlist = this->descriptor_list();
  for (vcl_list<dbsksp_shock_node_descriptor_sptr >::iterator itr = dlist.begin();
    itr != dlist.end(); ++itr)
  {
    dbsksp_shock_node_descriptor_sptr d0 = *itr;

    // don't adjust phi angle of `e'
    if (d0 == d) continue;
    d0->phi -= diff_per_edge;
  }

  
  return;
}





// ----------------------------------------------------------------------------
//: print info of the dbskpoint to an output stream
void dbsksp_shock_node::
print(vcl_ostream & os)
{
  os << "\n<<\n"
    << "type[ " << this->is_a() << " ] id[ " << this->id() << " ]\n"
    << "pt[ (" << this->pt().x() << " , " << this->pt().y() << ") ] \n"
    << "radius[ " << this->radius() << " ]\n"
    << "depth[ " << this->depth() << " ]\n";
  for (dbsksp_shock_node::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    dbsksp_shock_edge_sptr e = *eit;
    dbsksp_shock_node_descriptor_sptr d = this->descriptor(e);
    os << "edge_id[ " << e->id() 
      << " ] phi[ " << d->phi 
      << " ] shock_dir[ " << d->shock_flow_dir 
      << " ] rot_angle_to_succ_edge[ " << d->rot_angle_to_succ_edge 
      << " ]\n";
  }  
  os << ">>\n";
  return;
}







// ============================================================================
//                dbsksp_shock_node_algos
// ============================================================================

//: compute dependent parameters at a A_1^2 node,
// assuming the following free variables
// - edge parameter:                m0
// - edge parameter:                m2
// - rotation between two edges:    theta

// ----------------------------------------------------------------------------
bool dbsksp_shock_node_algos::
compute_params_at_A12_node_given_2_m_and_rot_angle(dbsksp_shock_node_sptr& node)
{
  // Preliminary check
  assert(node->degree() == 2);

  // Collect information from the node and its adjacent edges
    
  dbsksp_shock_node::edge_iterator eit = node->edges_begin();
  dbsksp_shock_edge_sptr e0 = *eit;
  dbsksp_shock_edge_sptr e2 = *(++eit);

  dbsksp_shock_node_descriptor_sptr desc_e0 = node->descriptor(e0);
  dbsksp_shock_node_descriptor_sptr desc_e2 = node->descriptor(e2);

  // parameters to the system of equations
  double m0 = e0->param_m(node);
  double m2 = e2->param_m(node);
  double theta = desc_e0->rot_angle_to_succ_edge;
  int shock_flow_direction = desc_e2->shock_flow_dir;

  // variables of the system of equations
  double alpha0;
  double alpha2;
  double phi0;
  double phi2;
  if (!dbsksp_shock_node_algos::solve_equations_at_A12_node(
    &alpha0,
    &alpha2,
    &phi0,
    &phi2,
    m0,
    m2,
    theta,
    shock_flow_direction)
    ) return false;


  // assign values to the descriptor
  desc_e0->alpha = alpha0;
  desc_e0->phi = phi0;
  desc_e2->alpha = alpha2;
  desc_e2->phi = phi2;

  return true;
}


// ----------------------------------------------------------------------------
//: solve system of equations at A_1^2 node, with
// parameters:
// - m0
// - m2
// - theta
// - shock flow direction: 1 : e0 --> e2, -1 : e2 --> e0
// variables:
// - alpha0
// - alpha2
// - phi0
// - phi2
bool dbsksp_shock_node_algos::
solve_equations_at_A12_node(
  double* alpha0,
  double* alpha2,
  double* phi0,
  double* phi2,
  double m0,
  double m2,
  double theta,
  int shock_flow_direction)
{
  // for now can only handle the cases when m0 != 0 and m2 != 0
  if (m0 == 0 && m2 == 0) return false;

  if (m0 != 0 || m2 != 0)
  {
    *alpha0 = vcl_atan( (m0 * vcl_sin(theta)) / (m2 + m0 * vcl_cos(theta)) );
    *alpha2 = -vcl_atan( (m2 * vcl_sin(theta)) / ( m0 + m2 * vcl_cos(theta)) );
    double sin_phi = vnl_math_abs(vcl_sin(theta)) /
      vcl_sqrt(m2*m2 + m0*m0 + 2*m2*m0*vcl_cos(theta));

    if (vcl_abs(sin_phi) > 1) return false;

    double phi = vcl_asin(sin_phi);
    *phi0 = (shock_flow_direction > 0) ? phi : vnl_math::pi-phi;
    *phi2 = vnl_math::pi - (*phi0);
    return true;
  }
  return false;
}






// ----------------------------------------------------------------------------
bool dbsksp_shock_node_algos::
compute_params_at_A12_node_given_2_m_and_1_phi(
  const dbsksp_shock_node_sptr& node)
{
  // Preliminary check
  assert(node->degree() == 2);

  // Collect information from the node and its adjacent edges
    
  dbsksp_shock_node::edge_iterator eit = node->edges_begin();
  dbsksp_shock_edge_sptr e0 = *eit;
  dbsksp_shock_edge_sptr e2 = *(++eit);

  dbsksp_shock_node_descriptor_sptr desc_e0 = node->descriptor(e0);
  dbsksp_shock_node_descriptor_sptr desc_e2 = node->descriptor(e2);

  // parameters to the system of equations
  double m0 = e0->param_m(node);
  double m2 = e2->param_m(node);

  // make sure the two phi's are consistent
  double phi0 = desc_e0->phi;
  desc_e2->phi = vnl_math::pi - phi0;

  // solve the constraints
  double sin_alpha0 = m0 * vcl_sin(phi0);
  double sin_alpha2 = m2 * vcl_sin(phi0);


  // check validity condition
  if (vcl_abs(sin_alpha0) > 1 || vcl_abs(sin_alpha2) > 1)
    return false;


  desc_e0->alpha = vcl_asin(sin_alpha0);
  desc_e2->alpha = vcl_asin(sin_alpha2);

  desc_e0->rot_angle_to_succ_edge = desc_e0->alpha + vnl_math::pi - desc_e2->alpha;
  desc_e2->rot_angle_to_succ_edge = desc_e2->alpha + vnl_math::pi - desc_e0->alpha;

  //// Shock flow dir is now a constraint rather than a dependent variable
  // temporary - try not constraining it
  desc_e0->shock_flow_dir = vnl_math_sgn(desc_e0->phi > vnl_math::pi_over_2);
  desc_e2->shock_flow_dir = -desc_e0->shock_flow_dir;
  return true;
}



// ----------------------------------------------------------------------------
bool dbsksp_shock_node_algos::
compute_params_at_A13_node_given_3_m_and_2_phi(const dbsksp_shock_node_sptr& node)
{
  // Preliminary check
  assert(node->degree() == 3);
  int degree = node->degree();
  
  // Collect information from the node and its adjacent edges
  vcl_vector<dbsksp_shock_edge_sptr > edges;
  for (dbsksp_shock_node::edge_iterator eit = node->edges_begin(); 
    eit != node->edges_end(); ++eit)
  {
    edges.push_back(*eit);
  }

  vcl_vector<dbsksp_shock_node_descriptor_sptr > descriptors;
  for (vcl_list<dbsksp_shock_node_descriptor_sptr >::const_iterator it = 
    node->descriptor_list().begin(); it != node->descriptor_list().end(); ++it)
  {
    descriptors.push_back(*it);
  }

  // make sure sum of the phi's is pi
  double sum = 0;
  for (int i=0; i<degree-1; ++i)
  {
    sum += descriptors[i]->phi;
  }
  descriptors[degree-1]->phi = vnl_math::pi - sum;

  // compute alpha's from m and phi
  for (int i=0; i<degree; ++i)
  {
    double sin_alpha = edges[i]->param_m(node) * vcl_sin(descriptors[i]->phi);
    descriptors[i]->alpha = vcl_asin(sin_alpha);
  }

  // compute angle between two adjacent shock chords
  for (int i=0; i<degree; ++i)
  {
    int ip1 = (i+1) % degree;
    descriptors[i]->rot_angle_to_succ_edge = 
      descriptors[i]->alpha + descriptors[i]->phi +
      descriptors[ip1]->phi - descriptors[ip1]->alpha;
  }

  // Shock flow dir is now a constraint rather than a dependent variable
  return true;
}


// ----------------------------------------------------------------------------
bool dbsksp_shock_node_algos::
compute_params_at_A1nGT3_node_given_n_m_and_n_minus_1_phi(
  const dbsksp_shock_node_sptr& node)
{
  // Preliminary check
  assert(node->degree() > 3);
  int degree = node->degree();
  
  // Collect information from the node and its adjacent edges
  vcl_vector<dbsksp_shock_edge_sptr > edges;
  for (dbsksp_shock_node::edge_iterator eit = node->edges_begin(); 
    eit != node->edges_end(); ++eit)
  {
    edges.push_back(*eit);
  }

  vcl_vector<dbsksp_shock_node_descriptor_sptr > descriptors;
  for (vcl_list<dbsksp_shock_node_descriptor_sptr >::const_iterator it = 
    node->descriptor_list().begin(); it != node->descriptor_list().end(); ++it)
  {
    descriptors.push_back(*it);
  }

  // make sure sum of the phi's is pi
  double sum = 0;
  for (int i=0; i<degree-1; ++i)
  {
    sum += descriptors[i]->phi;
  }
  descriptors[degree-1]->phi = vnl_math::pi - sum;

  // compute alpha's from m and phi
  for (int i=0; i<degree; ++i)
  {
    double sin_alpha = edges[i]->param_m(node) * vcl_sin(descriptors[i]->phi);
    descriptors[i]->alpha = vcl_asin(sin_alpha);
  }

  // compute angle between two adjacent shock chords
  for (int i=0; i<degree; ++i)
  {
    int ip1 = (i+1) % degree;
    descriptors[i]->rot_angle_to_succ_edge = 
      descriptors[i]->alpha + descriptors[i]->phi +
      descriptors[ip1]->phi - descriptors[ip1]->alpha;
  }

  // Shock flow dir is now a constraint rather than a dependent variable
  return true;
}

