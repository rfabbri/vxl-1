// This is brcv/shp/dbsk2d/dbsk2d_ishock_bcurve.cxx

//:
// \file

#include "dbsk2d_ishock_bcurve.h"


//***********************************************
// Constructors/Destructors/Initializers
//*********************************************
  
//: Constructor
dbsk2d_ishock_bcurve::
dbsk2d_ishock_bcurve(dbsk2d_ishock_bpoint* startpt, 
                     dbsk2d_ishock_bpoint* endpt, 
                     int id, 
                     bool bGUI):
dbsk2d_ishock_belm(id, bGUI, BLINE), 
start_bpoint_(startpt), 
end_bpoint_(endpt), 
twin_bcurve_(0),
bnd_edge_(0)
{
}

//: Destructor
dbsk2d_ishock_bcurve::
~dbsk2d_ishock_bcurve()
{
  this->s_pt()->disconnectFrom(this);
  this->e_pt()->disconnectFrom(this);
  if (twin_bcurve_)
  {
    this->twin_bcurve_->twin_bcurve_ = 0;
  }
  return;
}



//: Return itself if it is a GUI element, else, return its twinelm.
dbsk2d_ishock_belm* dbsk2d_ishock_bcurve::
GUIelm()
{
  if (this->is_a_GUIelm())
    return this;
  else
    return this->twin_bcurve();
}

//: Set the twin dbsk2d_ishock_bcurve of `this'
// Return false if setting fails.
bool dbsk2d_ishock_bcurve::
set_twin_bcurve(dbsk2d_ishock_bcurve* other)
{
  // First perform type and end-point checks.
  // Then set the twin_bcurve_ variables of both `this' and `other'

  // check type
  if (this->is_a() != other->is_a())
    return false;
  // check if end points match
  if ((this->s_pt() != other->e_pt()) || (this->e_pt() != other->s_pt()))
    return false;

  this->twin_bcurve_ = other;
  other->twin_bcurve_ = this;
  return true;
}


//: Return true if `this' and `other' are twin of each other
bool dbsk2d_ishock_bcurve::
is_twin_bcurve_with(const dbsk2d_ishock_bcurve* other) const
{
  return (this->twin_bcurve_ == other) && (other->twin_bcurve_==this);
}

//: left contact shock
dbsk2d_ishock_contact* dbsk2d_ishock_bcurve::lContact()
{ 
  if (shock_map_.size()==0)
    return 0;

  if (shock_map_.begin()->first.type==dbsk2d_ishock_bnd_key::RIGHTCONTACT)
    return (dbsk2d_ishock_contact*) shock_map_.begin()->second;
  else
    return 0;
}

//: right contact shock
dbsk2d_ishock_contact* dbsk2d_ishock_bcurve::rContact()
{
  if (shock_map_.size()==0)
    return 0;

  if (shock_map_.rbegin()->first.type==dbsk2d_ishock_bnd_key::LEFTCONTACT)
    return (dbsk2d_ishock_contact*) shock_map_.rbegin()->second;
  else
    return 0;
}

//: left-most shock
dbsk2d_ishock_edge* dbsk2d_ishock_bcurve::leftmost_shock()
{ 
  if (shock_map_.size()==0)
    return 0;

  if (shock_map_.begin()->first.is_right_type())
    return shock_map_.begin()->second;
  else
    return 0;
}

//: right-most shock
dbsk2d_ishock_edge* dbsk2d_ishock_bcurve::rightmost_shock()
{
  if (shock_map_.size()==0)
    return 0;

  if (shock_map_.rbegin()->first.is_left_type())
    return shock_map_.rbegin()->second;
  else
    return 0;
}

