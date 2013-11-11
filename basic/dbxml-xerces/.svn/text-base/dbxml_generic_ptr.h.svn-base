// This is brl/bbas/bxml/bxml_generic_ptr.h
#ifndef bxml_generic_ptr_h_
#define bxml_generic_ptr_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief A generic pointer mechanism s.t all class hierarchies of interest can be represented without requiring a common base class
// \author
//   J.L. Mundy
//
// \verbatim
//  Modifications:
//   J.L. Mundy December 07, 2002    Initial version.
//   Based on the original TargetJr design by R. Hoffman and J. Liu
// \endverbatim
//--------------------------------------------------------------------------------
#include <vcl_ostream.h>
#include <vcl_vector.h>

//forward declarations
class vsol_spatial_object_2d;

class dbxml_generic_ptr
{
 public:
  ~dbxml_generic_ptr(){}
  //:type enumeration
  enum ptr_type{NO_TYPE=0, VECT, VSOL_SO}; 
  ptr_type type() const{return type_;}

  //:Constructing generic pointers from specific class hierarchies
  dbxml_generic_ptr():type_(NO_TYPE){ptrs_.anyobject_ = 0;}
  dbxml_generic_ptr(void *p):type_(NO_TYPE){ptrs_.anyobject_ = p;}

  //:Accessing specific classes from a generic pointer
  inline void* get_anyobject() const {return ptrs_.anyobject_;}

  //:stream print
  inline friend vcl_ostream& operator<<(vcl_ostream&, const dbxml_generic_ptr&);
 private:
  union type_union
  {
    void*  anyobject_;
    vcl_vector<void*>* vec_ptr_;
    vsol_spatial_object_2d* vso_ptr_;
  };
  //:utility functions

  //:members
  ptr_type type_;
  type_union ptrs_;
};

//: Ostream inline function implementation
inline vcl_ostream& operator<<(vcl_ostream& os, const dbxml_generic_ptr& ptr) 
{
  os << " dbxml_generic_ptr type  " << int(ptr.type()) << vcl_endl;
  return os;
}

#endif // bxml_generic_ptr_h_
