// This is file shp/dbsksp/dbsksp_skbranch.cxx

//:
// \file

#include "dbsk2d_bnd_vertex.h"


#include <vtol/vtol_edge.h>
#include <dbsk2d/dbsk2d_geometry_utils.h>


//***********************************************************
// Constructors/Destructors/Initializers
//***********************************************************


//----------------------------------------------------------
//: Constructor 
dbsk2d_bnd_vertex::
dbsk2d_bnd_vertex(dbsk2d_ishock_bpoint* bpoint, int id):
vtol_vertex()
{
  dbsk2d_assert (bpoint);
  this->set_id(id);
  this->bpoint_ = bpoint;
  this->bpoint_->set_bnd_vertex(this); 
}


//----------------------------------------------------------
//: Destructor
dbsk2d_bnd_vertex::~dbsk2d_bnd_vertex()
{
  delete this->bpoint_;
}


//***********************************************************
// Data access
//***********************************************************


//----------------------------------------------------------
//: Clone `this': creation of a new object and initialization
// inherited.
vsol_spatial_object_2d* dbsk2d_bnd_vertex::
clone() const
{
  dbsk2d_ishock_bpoint* bpoint = new
    dbsk2d_ishock_bpoint(this->point().x(), this->point().y(), -1);
  return new dbsk2d_bnd_vertex(bpoint);
}

//***********************************************************
// Relation with other vtol objects
//***********************************************************



//***********************************************************
// Operators
//***********************************************************

//----------------------------------------------------------
//: Is `this' vertex the same as `other' ?
// Shallow comparison. Exac geometry comparison
// inherited.
bool dbsk2d_bnd_vertex::
operator==(const vtol_vertex &other) const
{
  // only compare `other' is of same type as `this'
  if (! other.is_class("dbsk2d_bnd_vertex"))
    return false;

  // exact comparison of geometry
  return this->point() == static_cast<const dbsk2d_bnd_vertex* >(&other)->point();
}



//----------------------------------------------------------
//: copy the geometry
// inherited.
void dbsk2d_bnd_vertex::
copy_geometry(const vtol_vertex &other)
{
  // only copy if `other' is of same type as `this'
  if (! other.is_class("dbsk2d_bnd_vertex"))
    return;
  vgl_point_2d<double > pt = 
    static_cast<const dbsk2d_bnd_vertex* >(&other)->point();

  this->bpoint_->set_pt(pt.x(), pt.y());
}


//----------------------------------------------------------
//: compare the geometry with fuzzy boolean
// inherited. 
bool dbsk2d_bnd_vertex::
compare_geometry(const vtol_vertex &other) const
{
  // only compare `other' is of same type as `this'
  if (! other.is_class("dbsk2d_bnd_vertex"))
    return false;

  // compare the geometry of internal bpoints
  return _BisEqPoint(this->point(), static_cast<const dbsk2d_bnd_vertex* >(&other)->point());
}


//***********************************************************
// Utilities
//***********************************************************


//-------------------------------------------------------------------
//: Merge superior linkages of `this' with 'other' vertex
// All superior linkage in `other' will be transfered to 'this'
void dbsk2d_bnd_vertex::
merge_superiors_with( dbsk2d_bnd_vertex_sptr other )
{ 
  if (other == this) return;

  // go through the list of superiors of `other'
  while (!other->superiors_list()->empty())
  {
    vtol_topology_object* sup = other->superiors_list()->back();
    sup->unlink_inferior(other.ptr());
    sup->link_inferior(this);
  }
}


//----------------------------------------------------------------------------
//: Merge `this' vertex with `other' vertex
// usually because they are too close to each other
// all linkages in `other' will be transfered to `this'
void dbsk2d_bnd_vertex::
merge_with(dbsk2d_bnd_vertex_sptr other)
{
  // merge the internal bpoint
  this->bpoint_->mergeWith(other->bpoint_);

  // reconnect the edges connected to `other'
  edge_list elist;
  other->edges(elist);
  for (unsigned int i=0; i<elist.size(); ++i)
  {
    if (elist[i]->is_endpoint1(other.ptr()))
      elist[i]->set_v1(this);

    if (elist[i]->is_endpoint2(other.ptr()))
      elist[i]->set_v2(this);
  }
    
  // by now all zero_chains associated with edges have been reconnected
  // need to handle zero_chain that are not associated with edges
  this->merge_superiors_with(other);
}

//-------------------------------------------------------------------
//: print
// inherited. Need rewrite.
void dbsk2d_bnd_vertex::
print(vcl_ostream & os) const
{
  os << "<" << this->is_a() << " " << (void const *)this <<
    "> with id " << this->get_id() << vcl_endl;
  os << "- bpoint (" << this->bpoint()->is_a() <<
    "  " << (void const*)(this->bpoint()) << 
    ") with id "<< this->bpoint()->id() << '\n';
}

//----------------------------------------------------------
//: Compute bounding box of the vertex. A local implementation
// inherited.
void dbsk2d_bnd_vertex::
compute_bounding_box() const
{
  this->set_bounding_box(this->point().x(), this->point().y());
}




//------------------------------------------------------------------------
//: touch `this' vertex and edges and contours containing `this' vertex
void dbsk2d_bnd_vertex::
touch_and_propagate()
{
  // edges
  this->touch();
  edge_list edges;
  this->edges(edges);
  for (unsigned int i = 0; i< edges.size(); ++i)
  {
    edges[i]->touch();
  }

  // contours
  one_chain_list contours;
  this->one_chains(contours);
  for (unsigned int i=0; i<contours.size(); ++i)
  {
    contours[i]->touch();
  }
}











//****************************************************************
// Depreciated functions
//****************************************************************
//: Create a line edge from `this' and `other' only if this edge does not exist.
//  Otherwise it just returns the existing edge
//  REQUIRE: other!=*this
// inherited. Depreciated. Simply return 0.
vtol_edge_sptr dbsk2d_bnd_vertex::
new_edge(vtol_vertex_sptr const& other)
{
  return 0; 
}

