// This is dbrec3d_context_part.h
#ifndef dbrec3d_context_part_h
#define dbrec3d_context_part_h

//:
// \file
// \brief Attach a dbrec3d_part to an octree context 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  10-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

template <class context>
class dbrec3d_context_part : public context
{

public:
  
  //: Constructor
  dbrec3d_context_part<context>() : context(), part(){}
  
  //: Constructor - somewhat like a copy contructor
  dbre3d_context_part<context>( const context
protected:
  
};
#endif
