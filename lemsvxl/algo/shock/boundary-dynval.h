#ifndef  _BOUNDARY_DYNVAL_H
#define  _BOUNDARY_DYNVAL_H

#include "boundary.h"
#include "belements-dynval.h"

class Boundary_DynVal : public Boundary
{
public:
  Boundary_DynVal (BOUNDARY_LIMIT newbndlimit=NO_LIMIT);
  virtual ~Boundary_DynVal ();

  void clearAllIVS (void);
};

#endif
