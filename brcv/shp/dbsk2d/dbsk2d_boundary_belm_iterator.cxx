// This is brcv/shp/dbsk2d/dbsk2d_boundary_belm_iterator.cxx

//:
// \file


#include "dbsk2d_boundary.h"


//**********************************************
// DBSK2D_BELM_ITERATOR
//**********************************************

//--------------------------------------------
//: Constructor
dbsk2d_boundary::belm_iterator::
belm_iterator( dbsk2d_boundary* boundary, bool is_begin )
: boundary_(boundary),
belm_iter_(boundary->belm_list_.begin())
{
  if (! is_begin)
    this->belm_iter_ = boundary->belm_list_.end();
}
  

//--------------------------------------------
//: Pre-Increment
dbsk2d_boundary::belm_iterator& dbsk2d_boundary::belm_iterator::operator++ ()
{
  ++(this->belm_iter_);
  return (*this);
}


//--------------------------------------------
    //: Post-Increment
dbsk2d_boundary::belm_iterator dbsk2d_boundary::belm_iterator::operator++ (int)
{

  belm_iterator old(*this);
  ++(*this);
  return old;
}



//-----------------------------------------------
//: Dereference
dbsk2d_ishock_belm* dbsk2d_boundary::belm_iterator::
operator -> () const
{
  return *(*this);
}


//-----------------------------------------------
//: Dereference
dbsk2d_ishock_belm* dbsk2d_boundary::belm_iterator::
operator * () const
{
  if (this->belm_iter_ == this->boundary_->belm_list_.end())
    return 0;
  else
    return *(this->belm_iter_);
}



//-----------------------------------------------
//: Equality comparison
bool dbsk2d_boundary::belm_iterator::
operator == (const belm_iterator& rhs) const
{
  return *rhs == *(*this);
}


//-----------------------------------------------
//: Inequality comparison
bool dbsk2d_boundary::belm_iterator::
operator != (const belm_iterator& rhs) const
{
  return !(rhs == *this);
}


