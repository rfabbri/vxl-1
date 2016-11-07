// This is file shp/dbsksp/dbsksp_xshock_node.cxx

//:
// \file

#include "dbsksp_xshock_node.h"
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include <vcl_set.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbnl/dbnl_angle.h>


// =============================================================================
// dbsksp_xshock_node
// =============================================================================



// DATA ACCESS -----------------------------------------------------------------


// -----------------------------------------------------------------------------
//: Return descriptor associated with an edge
dbsksp_xshock_node_descriptor* dbsksp_xshock_node::
descriptor(const E_sptr& e)
{
  vcl_map<E_sptr, dbsksp_xshock_node_descriptor >::iterator it = 
    this->descriptor_map().find(e);
  if (it == this->descriptor_map().end())
    return 0;
  else
    return &it->second;
}


// -----------------------------------------------------------------------------
//: Return descriptor associated with an edge - const
const dbsksp_xshock_node_descriptor* dbsksp_xshock_node::
descriptor(const E_sptr& e) const
{
  vcl_map<E_sptr, dbsksp_xshock_node_descriptor >::const_iterator it = 
    this->descriptor_map().find(e);
  if (it == this->descriptor_map().end())
    return 0;
  else
    return &it->second;
}

// -----------------------------------------------------------------------------
//: Return extrinsic coordinates of the node
vgl_point_2d<double > dbsksp_xshock_node::
pt() const
{
  return this->pt_;
}


// -----------------------------------------------------------------------------
//: Set coordinates of the node
void dbsksp_xshock_node:: 
set_pt(const vgl_point_2d<double >& pt)
{
  this->pt_ = pt;
  
  // update the point in all the descriptors as well
  for (vcl_map<E_sptr, dbsksp_xshock_node_descriptor >::iterator it = 
    this->descriptor_map().begin(); it != this->descriptor_map().end(); ++it)
  {
    it->second.pt_ = pt;
  }
  return;
}



// -----------------------------------------------------------------------------
//: Return and set radius at the node
double dbsksp_xshock_node::
radius() const
{
  return this->radius_;
}


// -----------------------------------------------------------------------------
//: Set radius of the node
void dbsksp_xshock_node::
set_radius(double radius)
{
  this->radius_ = radius;
  // update the point in all the descriptors as well
  for (vcl_map<E_sptr, dbsksp_xshock_node_descriptor >::iterator it = 
    this->descriptor_map().begin(); it != this->descriptor_map().end(); ++it)
  {
    it->second.radius_ = radius;
  }
  return;
}
  
  
// -----------------------------------------------------------------------------
//: remove an edge, and its descriptor from the edge list
// Return false if the edge is not found in the list
bool dbsksp_xshock_node::
remove_shock_edge(const E_sptr& e)
{
  // delete the "topological" edge
  if (this->del_edge(e))
  {
    // delelete the "descriptor" of this edge `e'
    int count = this->descriptor_map_.erase(e);
    assert(count == 1);
    if (count != 1)
    {
      vcl_cerr << "ERROR: Inconsistency - More than one descriptor for one edge.";
      return false;
    }
    return true;
  }
  else
  {
    // the edge `e' is not connected to `this' vertex.
    return false;
  }
  return false;
}


// -----------------------------------------------------------------------------
//: Replace an edge with a different edge. Return the descriptor of the new edge
// If new_xe == 0 then this is equivalent to remove an edge
dbsksp_xshock_node_descriptor* dbsksp_xshock_node::
replace_shock_edge(const E_sptr& old_xe, const E_sptr& new_xe)
{
  // preliminary check
  if (!new_xe)
  {
    this->remove_shock_edge(old_xe);
    return 0;
  }

  // locate the edge
  dbsksp_xshock_node::edge_iterator eit = this->edges_begin();
  for (; eit != this->edges_end() && (*eit) != old_xe; ++eit);
  if (eit == this->edges_end()) 
    return 0;

  // replace the edge, topologically
  *eit = new_xe;

  // replace the edge descriptor

  // i. remove the old descriptor
  dbsksp_xshock_node_descriptor temp_xdesc = *this->descriptor(old_xe);
  int count = this->descriptor_map_.erase(old_xe);
  assert(count == 1);

  // ii. add the new descriptor
  vcl_map<dbsksp_xshock_edge_sptr, dbsksp_xshock_node_descriptor >::iterator mit = 
    this->descriptor_map_.insert(vcl_make_pair(new_xe, temp_xdesc)).first;
  mit->second = temp_xdesc;

  return &mit->second;
}


// -----------------------------------------------------------------------------
//: remove all edges and their descriptors
void dbsksp_xshock_node::
remove_all_shock_edges()
{
  this->in_edges_.clear();
  this->descriptor_map_.clear();
  return;
}


// -----------------------------------------------------------------------------
//: add an edge right BEFORE a reference edge (in the edge list)
// if ref_edge = 0 then add to the end of list
dbsksp_xshock_node_descriptor* dbsksp_xshock_node::
insert_shock_edge(const E_sptr& new_edge, const E_sptr& ref_edge)
{
  if (!new_edge)
    return 0;


  // find location of the ref_edge in the edge list
  if (ref_edge)
  {
    vcl_list<E_sptr >::iterator eit = vcl_find(this->in_edges_.begin(),
      this->in_edges_.end(), ref_edge);
    if (eit == this->in_edges_.end())
    {
      vcl_cerr << "ERROR: in dbsksp_shock_node::insert_edge(...)\n" 
        << "reference edge is not incident to this node.\n";
      return 0;
    }
    this->in_edges_.insert(eit, new_edge);
  }
  else
  {
    this->in_edges_.push_back(new_edge);
  }
    
  // insert the node descriptor
  dbsksp_xshock_node_descriptor descriptor;
  vcl_map<E_sptr, dbsksp_xshock_node_descriptor >::iterator it =
    this->descriptor_map_.insert(vcl_make_pair(new_edge, descriptor)).first;
  return &it->second;
}






// -----------------------------------------------------------------------------
//: Return the parent edge (connecting to the parent node)
dbsksp_xshock_edge_sptr dbsksp_xshock_node::
parent_edge()
{
  dbsksp_xshock_edge_sptr xe = 0;
  for (dbsksp_xshock_node::edge_iterator eit = this->edges_begin(); eit != 
    this->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr cur_xe = *eit;
    if (cur_xe->opposite(this)->depth() < this->depth())
    {
      xe = cur_xe;
      break;
    }
  }
  return xe;
}


// UTILITY ---------------------------------------------------------------------


// -----------------------------------------------------------------------------
//: Rotate the tangent of one branch. Update descriptors of other branches 
// accordingly
bool dbsksp_xshock_node::
rotate_tangent(const E_sptr& xe, double ds)
{
  dbsksp_xshock_node_descriptor* xdesc = this->descriptor(xe);
  if (!xe) return false;

  // modify tangent angle
  xdesc->psi_ += ds;

  // if this is a degree-2 node, we need to modify the tangent of the other edge
  // and keep phi's intact
  if (this->degree() == 2)
  {
    // determine the ``other'' edge
    dbsksp_xshock_edge_sptr xe2 = 0;
    if (*(this->edges_begin()) == xe)
    {
      xe2 = *(++this->edges_begin());
    }
    else
    {
      xe2 = *(this->edges_begin());
    }
    dbsksp_xshock_node_descriptor* xdesc2 = this->descriptor(xe2);
    xdesc2->psi_ = xdesc->psi_ + vnl_math::pi; 
  }
  // if degree > 2, keep the tangents intact and modify the phi's
  else if (this->degree() == 3)
  {
    // recompute phi's according to the tangents
    dbsksp_xshock_node_descriptor* xdesc[3];
    int count = 0;
    for (dbsksp_xshock_node::edge_iterator eit = this->edges_begin();
      eit != this->edges_end(); ++eit, ++count)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      xdesc[count] = this->descriptor(xe);
    }

    xdesc[0]->phi_ = vnl_math::pi - angle(xdesc[1]->shock_tangent(), xdesc[2]->shock_tangent());
    xdesc[1]->phi_ = vnl_math::pi - angle(xdesc[2]->shock_tangent(), xdesc[0]->shock_tangent());
    xdesc[2]->phi_ = vnl_math::pi - xdesc[0]->phi_ - xdesc[1]->phi_;
  }
  else
  {
    vcl_cerr << "ERROR: rotate_tangent can't be applied to node degree "
      << this->degree() << ".\n";
    return false;
  }

  return true;
}








//------------------------------------------------------------------------------
//: Order list of edges according to shock tangent
void dbsksp_xshock_node::
order_edge_list_by_shock_tangent()
{
  // collect list of shock tangents
  vcl_multimap<double, dbsksp_xshock_edge_sptr > map_angle_to_edge;
  for (dbsksp_xshock_node::edge_iterator eit = this->edges_begin(); eit !=
    this->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    dbsksp_xshock_node_descriptor* xdesc = this->descriptor(xe);

    // tangent angle
    double angle = dbnl_angle_0to2pi(xdesc->shock_tangent_angle());
    map_angle_to_edge.insert(vcl_make_pair(angle, xe));
  }

  // put the angle back in
  this->edge_list().clear();
  for (vcl_multimap<double, dbsksp_xshock_edge_sptr >::iterator iter =
    map_angle_to_edge.begin(); iter != map_angle_to_edge.end(); ++iter)
  {
    this->edge_list().push_back(iter->second);
  }
  
  return;
}





// -----------------------------------------------------------------------------
//: print info of the dbskpoint to an output stream
vcl_ostream& dbsksp_xshock_node::
print(vcl_ostream & os)
{
  os << "\n<<\n"
    << "type[ " << this->is_a() << " ] id[ " << this->id() << " ]\n"
    << "  pt[ (" << this->pt().x() << " , " << this->pt().y() << ") ]"
    << "  radius[ " << this->radius() << " ]\n";
  for (dbsksp_xshock_node::edge_iterator eit = this->edges_begin();
    eit != this->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    dbsksp_xshock_node_descriptor* xdesc = this->descriptor(xe);
    os << "  edge_id[ " << xe->id() 
      << " ] phi[ " << xdesc->phi_
      << " ]\n";
  }  
  os << ">>\n";
  return os;
}

