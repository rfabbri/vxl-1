// This is dbrec3d_visitor.h
#ifndef dbrec3d_visitor_h
#define dbrec3d_visitor_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  24-Aug-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_primitive_kernel_part.h"
//#include "dbrec3d_part.h"

class dbrec3d_visitor
{

public:
  dbrec3d_visitor(){}
#if 0
  void visit(dbrec3d_part_sptr part)
  {
    dbrec3d_primitive_kernel_part *pp = dynamic_cast<dbrec3d_primitive_kernel_part *>(part.as_pointer());
    if (pp) visit(pp);
    else {

    }
  }
#endif
  virtual void visit(dbrec3d_part_sptr part) {}
    
};


#endif
