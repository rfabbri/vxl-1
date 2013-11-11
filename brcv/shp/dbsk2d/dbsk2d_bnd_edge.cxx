// This is file dbsk2d/dbsk2d_bnd_edge.cxx

//:
// \file

#include "dbsk2d_bnd_edge.h"

#include <vcl_algorithm.h>

#include <vgl/vgl_intersection.h>

#include <dbsk2d/dbsk2d_bnd_vertex.h>
#include <dbsk2d/dbsk2d_bnd_cell.h>
#include <dbsk2d/dbsk2d_bnd_contour.h>
#include <dbsk2d/dbsk2d_bnd_contour_sptr.h>





//**************************************************
// Constructors/Destructors/Initializers
//**************************************************

//---------------------------------------------------------
///: Constructor - a real edge
dbsk2d_bnd_edge::
dbsk2d_bnd_edge( dbsk2d_bnd_vertex_sptr v1,
                   dbsk2d_bnd_vertex_sptr v2,
                   dbsk2d_ishock_bcurve* left_bcurve, 
                   dbsk2d_ishock_bcurve* right_bcurve,
                   int id):
vtol_edge()
{
  this->set_id(id);
  this->set_v1(v1->cast_to_vertex());
  this->set_v2(v2->cast_to_vertex());
  this->left_bcurve_ = 0;
  this->right_bcurve_ = 0;
  bool success = this->set_bcurves(left_bcurve, right_bcurve);
  // require
  dbsk2d_assert(success);
}


//---------------------------------------------------------
//: Constructor - a degenerate edge, i.e. a point
dbsk2d_bnd_edge::
dbsk2d_bnd_edge( dbsk2d_bnd_vertex_sptr v, int id):
vtol_edge(), left_bcurve_(0), right_bcurve_(0)
{
  this->set_id(id);
  this->set_v1(v->cast_to_vertex());
  this->set_v2(v->cast_to_vertex());
}

// Destructor
dbsk2d_bnd_edge::
~dbsk2d_bnd_edge()
{
  if (this->left_bcurve_)
    delete this->left_bcurve_;
  if (this->right_bcurve_)
    delete this->right_bcurve_;
}


//**************************************************
// Data access
//**************************************************

//---------------------------------------------------------
//: Set the two half curves dbsk2d_ishock_bcurve
// Only apply for a real edge, not for a point
// First check if `left' and `right' are twin. Then set.
// Return false if `left' and `right' are not twins.
bool dbsk2d_bnd_edge::
set_bcurves( dbsk2d_ishock_bcurve* left_bcurve, dbsk2d_ishock_bcurve* right_bcurve)
{
  if (this->is_a_point()) return false;
  if (!left_bcurve || !right_bcurve) return false;
  
  // "twin" test - check if `left_bcurve' and `right_bcurve' are twin of each other
  if (! (left_bcurve->is_twin_bcurve_with(right_bcurve)))
  {
    return false;
  }

  // "end-point" test - make sure left_bcurve and right_bcurve share the same 
  // end points as the end-vertices of `this' edge
  if (!this->v1() || !this->v2()) 
    return false;

  //// relax this condition a bit to work with cell shock computation scheme
  //if ((left_bcurve->s_pt() != this->bnd_v1()->bpoint()) ||
  //  (left_bcurve->e_pt() != this->bnd_v2()->bpoint()))
  //  return false;

  if ((this->bnd_v1() != left_bcurve->s_pt()->bnd_vertex() ) ||
    ( this->bnd_v2() != left_bcurve->e_pt()->bnd_vertex()))
    return false;


  // finally, assign new values to member variables
  this->left_bcurve_ = left_bcurve;
  this->left_bcurve_->set_bnd_edge(this);
  this->right_bcurve_ = right_bcurve;
  this->right_bcurve_->set_bnd_edge(this);
  return true;
}


//---------------------------------------------------------
//: Delete the bcurve objects of `this'
void dbsk2d_bnd_edge::
clear_bcurves()
{
  if (this->left_bcurve_)
  {
    delete this->left_bcurve_;
    this->left_bcurve_ = 0;
    delete this->right_bcurve_;
    this->right_bcurve_ = 0;
  }
}
  


//---------------------------------------------------------
//: return length of the edge
double dbsk2d_bnd_edge::len() const
{ 
  if (this->is_a_point())
    return 0; 
  return this->left_bcurve()->len();
}


//**************************************************
// OPERATORS
//**************************************************

//---------------------------------------------------------
//: Equality operator
// inherited from vtol_edge.
bool dbsk2d_bnd_edge::
operator==(const vtol_edge &other) const
{
  if (other.is_a() != this->is_a())
    return false;
  return (*this) == *static_cast<const dbsk2d_bnd_edge*>(&other);
}


//---------------------------------------------------------
//: Equality operator
bool dbsk2d_bnd_edge::
operator==(const dbsk2d_bnd_edge& other) const
{
  if (this==&other) return true;

  // case: point and real curve
  if ((this->is_a_point() && !other.is_a_point()) ||
    (! this->is_a_point() && other.is_a_point()))
    return false;

  if (!(*(this->v1_)==*(other.v1_)))
    return false;
    
  // case: point - point
  if (this->is_a_point() && other.is_a_point())
    return true;
    
  if (!(*(this->v2_)==*(other.v2_)))
    return false;

  // case: both `this' and `other' are both real curves
  if (!(this->left_bcurve_) || !(other.left_bcurve_))
    return false;
  
  // only compare the left_bcurves, result from right_bcurves
  // should be the same
  // \TODO - Comparing pointers is a hack.
  // should write functions to compare geometry.
  return (this->left_bcurve_ == other.left_bcurve_);
}


//**************************************************
// BASIC FUNCTIONS
//**************************************************



//: Merge `this' edge with `other' edge
// all linkages in `other' will be transfered to `this'
void dbsk2d_bnd_edge::
merge_with(dbsk2d_bnd_edge & other)
{

  // check if the line edges are exactly the same
  signed char dir = 1;
  if (this->v1()==other.v1() && this->v2()==other.v2())
  {
    dir = 1;
  }
  else if (this->v1()==other.v2() && this->v2()==other.v1())
  {
    dir = -1;
  }
  else
  {
    dbsk2d_assert(false);
  }
  
  // replace `other' by `this', topologically
  while (other.numsup() > 0)
  {
    vtol_topology_object* t = other.superiors_list()->front();
    dbsk2d_bnd_contour_sptr contour = static_cast<dbsk2d_bnd_contour* >(t);

    vcl_vector<signed char > directions(1, dir);
    vcl_vector<dbsk2d_bnd_edge_sptr > new_edges(1, this);
    contour->replace_edges(new_edges, directions, &other);
  }

  // replace `other' by `this', geographically
  vcl_list<dbsk2d_bnd_cell* > cells = other.cells();
  other.unlink_from_cells();
  for (vcl_list<dbsk2d_bnd_cell* >::iterator cit = cells.begin();
    cit != cells.end(); ++cit)
  {
    (*cit)->add_bnd_edge(this);
  }
  other.unlink();
}




//---------------------------------------------------------
//: comparison of geometry
// inherited. Need rewrite.
bool dbsk2d_bnd_edge::
compare_geometry(const vtol_edge &other) const
{
  // "same-class" test
  if (other.is_a() != this->is_a())
    return false;
  
  // compare geometry of curves inside the edge
  //const dbsk2d_bnd_edge* other_bnd = static_cast<const dbsk2d_bnd_edge*>(&other);
  return false;
}


//---------------------------------------------------------
//: reverse the edge direction, i.e. exchange starting and ending vertices
bool dbsk2d_bnd_edge::
reverse_direction()
{
  if (this->is_a_point()) return true;
  if (this->zero_chain()->numinf() != 2) return false;
  
  // reverse end-vertex order by reversing order of internal zero chain
  vtol_zero_chain_sptr internal_zch = this->zero_chain();
  internal_zch->unlink_all_inferiors();
  internal_zch->link_inferior(this->v2());
  internal_zch->link_inferior(this->v1());
  this->set_vertices_from_zero_chains();

  //reverse left and right bcurve
  dbsk2d_ishock_bcurve* temp = this->left_bcurve_;
  this->left_bcurve_ = this->right_bcurve_;
  this->right_bcurve_ = temp;
  return true;

}








// == Cell-related functions ======================


//------------------------------------------------------------------------
//: Add a cell to `this' edge's cell list
void dbsk2d_bnd_edge::
add_cell(const dbsk2d_bnd_cell_sptr& cell)
{
  this->cells_.push_back(cell.ptr());
}


//------------------------------------------------------------------------
//: Remove a cell from `this' edge's cell list
void dbsk2d_bnd_edge::
remove_cell(const dbsk2d_bnd_cell_sptr& cell)
{
  this->cells_.remove(cell.ptr());
}


//----------------------------------------------------------------------
//: Return false if `this' edge is guaranteed not to intersect `cell'
bool dbsk2d_bnd_edge::
intersect_cell(const dbsk2d_bnd_cell_sptr& cell, double tol)
{
  vsol_box_2d_sptr ebox = this->get_bounding_box();
  vgl_box_2d<double > extended_box(ebox->get_min_x()-tol, 
    ebox->get_max_x()+tol, ebox->get_min_y()-tol, ebox->get_max_y()+tol);
  vgl_box_2d<double > intersection = 
    vgl_intersection(cell->box(), extended_box);
  return !intersection.is_empty();
}




//----------------------------------------------------------------------
//: Disconnect `this' from all the cells it belongs to
void dbsk2d_bnd_edge::
unlink_from_cells()
{
  while (this->cells_.size()>0)
  {
    this->cells_.front()->remove_bnd_edge(this);
  }
}



//**************************************************
// UTILITIES
//**************************************************


//----------------------------------------------------------------------
//: print
// inherited. Need rewrite.
void dbsk2d_bnd_edge::
print(vcl_ostream &os) const
{
  os << "\n<" << this->is_a()<< " " <<(void const *)this << "> with id "<<
    this->get_id() << '\n';
  vcl_string str = (this->is_a_point())? "Yes" : "No";
  os << " * Is a point ? " << str << vcl_endl;
  // two end vertices
  os << " * v1 = ";
  this->v1()->print(os);
  os << " * v2 = ";
  this->v2()->print(os);

  // two internal bcurves
  //left
  os << " * left_bcurve: ( " << this->left_bcurve()->is_a() <<
    "  " << (void const*)(this->left_bcurve()) << 
    ") with id "<< this->left_bcurve()->id() << '\n';
  // starting bpoint
  os << "     + s_pt: ( " << this->left_bcurve()->s_pt()->is_a() <<
    "  " << (void const*)(this->left_bcurve()->s_pt()) << 
    ") with id "<< this->left_bcurve()->s_pt()->id() << '\n';

  // ending bpoint
  os << "     + e_pt: ( " << this->left_bcurve()->e_pt()->is_a() <<
    "  " << (void const*)(this->left_bcurve()->e_pt()) << 
    ") with id "<< this->left_bcurve()->e_pt()->id() << '\n';


  //right
  os << " * right_bcurve: ( " << this->right_bcurve()->is_a() <<
    "  " << (void const*)(this->right_bcurve()) << 
    ") with id "<< this->right_bcurve()->id() << '\n';
  //starting bpoint
  os << "     + s_pt: ( " << this->right_bcurve()->s_pt()->is_a() <<
    "  " << (void const*)(this->right_bcurve()->s_pt()) << 
    ") with id "<< this->right_bcurve()->s_pt()->id() << '\n';

  os << "     + e_pt: ( " << this->right_bcurve()->e_pt()->is_a() <<
    "  " << (void const*)(this->right_bcurve()->e_pt()) << 
    ") with id "<< this->right_bcurve()->e_pt()->id() << '\n';
}


// -----------------------------------------------------------------------
void dbsk2d_bnd_edge::
print_cell_info(vcl_ostream &os) const
{
  os << "Bnd cells: " << this->cells().size();
  for (vcl_list<dbsk2d_bnd_cell* >::const_iterator cit = 
    this->cells().begin(); cit != this->cells().end(); ++cit)
  {
    dbsk2d_bnd_cell_sptr cell = *cit;
    os << " (" << cell->index().row << "," << cell->index().col << ") ";
  }
  os << "\n";
}




//----------------------------------------------------------------------
//: Compute bounding box.
// Inherited from vsol_spatial object_2d
void dbsk2d_bnd_edge::
compute_bounding_box() const
{
  // need to first empty the old box
  this->empty_bounding_box();

  //if this is a singular point, bounding box is simply a point
  if (this->is_a_point())
  {
    vgl_point_2d<double > pt = this->bnd_v1()->point();
    this->set_bounding_box(pt.x(), pt.y()); 
    return;
  }
  else
  {
    // for regular curve, obtain bounding box info from the internal bcurve
    vbl_bounding_box<double, 2 > box;
    this->left_bcurve()->compute_bounding_box(box);
    this->add_to_bounding_box(box.xmin(), box.ymin());
    this->add_to_bounding_box(box.xmax(), box.ymax());
  }
}




