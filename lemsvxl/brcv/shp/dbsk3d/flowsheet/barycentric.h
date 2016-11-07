#ifndef _BARYCENTER_H_
#define _BARYCENTER_H_

#include "sheet_flow_mbuck_mres.h"
/* #include "predicates.h" */

#define TEST_TETRA_DIST

int
GetQuadCoordOfVertex(InputData *pInData, ShockVertex *pRed,
         ShockData *pShockData);

int
GetTriCoordOfCurve(InputData *pInData, ShockCurve *pGreen,
       ShockData *pShockData);

int
GetSigCoordOfCurve(InputData *pInData, ShockCurve *pGreen);


#endif /* _BARYCENTER_H_ */
