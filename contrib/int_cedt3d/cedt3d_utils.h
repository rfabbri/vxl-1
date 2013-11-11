#ifndef _CEDT3D_UTILS_H_
#define _CEDT3D_UTILS_H_

#include "cedt3d.h"
#include "heap.h"

void AllocHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt, int iWSpaceSize);
void DeAllocHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt);
void InitHeapAndCedt(Heap *pTheHeap, Cedt3d *pTheCedt, int iWSpaceSize, dist_sq_t *pdDtArray);
int InitDirDSPM(int idx, int idy, int idz);
void SetMask(Mask *pTheMask);
void SetPlaneOffDiagMasks(dir_t iDiagDir, dir_t* piNew2dMasks);
void SetCubicOffDiagMasks(dir_t iDiagDir, dir_t* piNew3dMasks);
void AdjustDataArray(double *pdDtArray, Dimension *pTheDim);

#endif /* _CEDT3D_UTILS_H_ */
