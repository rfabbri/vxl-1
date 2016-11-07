#include "boundary-dynval.h"

Boundary_DynVal::Boundary_DynVal (BOUNDARY_LIMIT newbndlimit) 
{
  ///_bIsRealBoundary = true;
  _Initialize(newbndlimit);  
}

Boundary_DynVal::~Boundary_DynVal ()
{
}

void Boundary_DynVal::clearAllIVS (void)
{
  BElement*  current;
  BPoint_DynVal*    bpoint;
  BLine_DynVal*    bline;
  BArc_DynVal*      barc;

  BElmListIterator elmPtr = BElmList.begin();
  for (; elmPtr != BElmList.end(); elmPtr++) {
    current = elmPtr->second;

    switch (current->type()) {
    case BPOINT:
      bpoint = (BPoint_DynVal*) current;//dynamic_cast<BPoint*>(elmPtr->second);
      bpoint->pointIV()->clear();
    break;
    case BLINE:
      bline = (BLine_DynVal*) current;//dynamic_cast<BLine*>(elmPtr->second);
      bline->lineIV()->clear();
    break;
    case BARC:
      barc = (BArc_DynVal*) current;//dynamic_cast<BArc*>(elmPtr->second);
      barc->arcIV()->clear();
    break;
    default: break;
    }
  }
}
