#ifndef _PROPAGATE3D_H_
#define _PROPAGATE3D_H_

#include "cedt3d.h"
#include "cedt3d_utils.h"
#include "heap.h"

void
CedtPropagateDist(Heap *theHeap, Cedt3d *theCedt,
          VectLoc *theVect, Dimension *theDim);
void
InitDiagPlanePropagateDist(Heap *theHeap, Cedt3d *theCedt,
               VectLoc *theVect, Dimension *theDim);
void
InitDiagCubePropagateDist(Heap *theHeap, Cedt3d *theCedt,
              VectLoc *theVect, Dimension *theDim);
void
AddToFrontAndHeap(Heap *theHeap, Cedt3d *theCedt,
          Distance *theDist, VectLoc *theVect, Dimension *theDim);

#endif /* _PROPAGATE3D_H_ */
