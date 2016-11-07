#ifndef _DT_BY_3D_CEDT_H_
#define _DT_BY_3D_CEDT_H_

#include "cedt3d.h"
#include "cedt3d_utils.h"
#include "propagate3d.h"

void DTby3dCEDT(dist_sq_t *pdDtArray, Dimension *pTheDim, int iFlagV, double dDelta,
                offset_t*& fdx, offset_t*& fdy, offset_t*& fdz);

#endif /* DT_BY_3D_CEDT */
