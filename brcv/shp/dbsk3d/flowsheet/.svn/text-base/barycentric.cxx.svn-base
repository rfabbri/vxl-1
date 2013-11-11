/*------------------------------------------------------------*\
 * File: barycentric.c
 *
 * Functions:
 *  GetQuadCoordOfVertex
 *  GetTriCoordOfCurve
 *  GetSigCoordOfCurve
 *
 * History:
 *  #0: Oct. 2001, by F.Leymarie
 *  #1: June 2002: Added GetSigCoordOfCurve()
 *  #2: Aug. 2003: Changed Trilinear to Barycentric coords.
 *
\*------------------------------------------------------------*/

#include "barycentric.h"

/*------------------------------------------------------------*\
 * Function: GetQuadCoordOfVertex
 *
 * Usage: Evaluates the barycentric coordinates of the Shock
 *    Vertex with respect to its associated Tetrahedron,
 *    as the circumcenter of the circumsphere.
 *    Computes the signed volume as well.
 *
\*------------------------------------------------------------*/

int
GetQuadCoordOfVertex(InputData *pInData, ShockVertex *pRed,
         ShockData *pShockData)
{
  char    *pcFName = "GetQuadCoordOfVertex";
  int    iGeneA,iGeneB,iGeneC,iGeneD, iFlag;
  static double  fEpsilon = (float) E_EPSILON;
  double    fAx,fAy,fAz, fBx,fBy,fBz, fCx,fCy,fCz, fDx,fDy,fDz;
  double    fTmpX,fTmpY,fTmpZ, fMuAB,fMuAC,fMuAD,fMuBC,fMuBD,fMuCD;
  double    fTmp1,fTmp2,fTmp3, fAlpha,fBeta,fGamma,fDelta;
  double    fVolume, fSum, fTmp, fABx,fABy,fABz;
  double    fACx,fACy,fACz, fADx,fADy,fADz;
  InputData  *pGeneA, *pGeneB, *pGeneC, *pGeneD;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB, *pGeneCoordC, *pGeneCoordD;

  /* Preliminaries */

  iGeneA = pRed->iGene[0];
  iGeneB = pRed->iGene[1];
  iGeneC = pRed->iGene[2];
  iGeneD = pRed->iGene[3];

  pGeneA = pInData+iGeneA;
  pGeneCoordA  = &(pGeneA->Sample);
  fAx = pGeneCoordA->fPosX;
  fAy = pGeneCoordA->fPosY;
  fAz = pGeneCoordA->fPosZ;

  pGeneB = pInData+iGeneB;
  pGeneCoordB  = &(pGeneB->Sample);
  fBx = pGeneCoordB->fPosX;
  fBy = pGeneCoordB->fPosY;
  fBz = pGeneCoordB->fPosZ;

  pGeneC = pInData+iGeneC;
  pGeneCoordC  = &(pGeneC->Sample);
  fCx = pGeneCoordC->fPosX;
  fCy = pGeneCoordC->fPosY;
  fCz = pGeneCoordC->fPosZ;

  pGeneD = pInData+iGeneD;
  pGeneCoordD  = &(pGeneD->Sample);
  fDx = pGeneCoordD->fPosX;
  fDy = pGeneCoordD->fPosY;
  fDz = pGeneCoordD->fPosZ;


  /* Compute the Mu's : Squares of diameters linking pairs of sources */

  fABx  = fBx - fAx; /* Distance AB */
  fTmpX = fABx * fABx;
  fABy  = fBy - fAy;
  fTmpY = fABy * fABy;
  fABz  = fBz - fAz;
  fTmpZ = fABz * fABz;

  fMuAB = fTmpX + fTmpY + fTmpZ;

  fACx  = fCx - fAx; /* Distance AC */
  fTmpX = fACx * fACx;
  fACy  = fCy - fAy;
  fTmpY = fACy * fACy;
  fACz  = fCz - fAz;
  fTmpZ = fACz * fACz;

  fMuAC = fTmpX + fTmpY + fTmpZ;

  fADx  = fDx - fAx; /* Distance AD */
  fTmpX = fADx * fADx;
  fADy  = fDy - fAy;
  fTmpY = fADy * fADy;
  fADz  = fDz - fAz;
  fTmpZ = fADz * fADz;

  fMuAD = fTmpX + fTmpY + fTmpZ;

  fTmpX = fCx - fBx; /* Distance BC */
  fTmpX *= fTmpX;
  fTmpY = fCy - fBy;
  fTmpY *= fTmpY;
  fTmpZ = fCz - fBz;
  fTmpZ *= fTmpZ;

  fMuBC = fTmpX + fTmpY + fTmpZ;

  fTmpX = fDx - fBx; /* Distance BD */
  fTmpX *= fTmpX;
  fTmpY = fDy - fBy;
  fTmpY *= fTmpY;
  fTmpZ = fDz - fBz;
  fTmpZ *= fTmpZ;

  fMuBD = fTmpX + fTmpY + fTmpZ;

  fTmpX = (fDx - fCx); /* Distance CD */
  fTmpX *= fTmpX;
  fTmpY = (fDy - fCy);
  fTmpY *= fTmpY;
  fTmpZ = (fDz - fCz);
  fTmpZ *= fTmpZ;

  fMuCD = fTmpX + fTmpY + fTmpZ;

  /* Compute the (signed) quadrilinear coordinates */

  fTmp1 = fMuAB * fMuCD;
  fTmp2 = fMuAC * fMuBD;
  fTmp3 = fMuAD * fMuBC;

  fAlpha =  fTmp1 * (fMuBC + fMuBD - fMuCD) +
           fTmp2 * (fMuBC + fMuCD - fMuBD) +
            fTmp3 * (fMuBD + fMuCD - fMuBC) - 
            (2 * fMuBC * fMuBD * fMuCD);

  fBeta  = fTmp1 * (fMuAC + fMuAD - fMuCD) +
     fTmp2 * (fMuAD + fMuCD - fMuAC) +
           fTmp3 * (fMuAC + fMuCD - fMuAD) - (2 * fMuAC * fMuAD * fMuCD);

  fGamma = fTmp1 * (fMuAD + fMuBD - fMuAB) +
     fTmp2 * (fMuAD + fMuAB - fMuBD) +
           fTmp3 * (fMuAB + fMuBD - fMuAD) - (2 * fMuAB * fMuAD * fMuBD);

  fDelta = fTmp1 * (fMuAC + fMuBC - fMuAB) +
     fTmp2 * (fMuAB + fMuBC - fMuAC) +
           fTmp3 * (fMuAB + fMuAC - fMuBC) - (2 * fMuAB * fMuAC * fMuBC);

  fSum = fAlpha + fBeta + fGamma + fDelta;
  fTmp = fSum / 288.0;
  if(fTmp > 0.0) {
    fVolume = (float) sqrt((double) fTmp);
  }
  else {
    fVolume = - (float) sqrt((double) -fTmp);
  }

  if(fabs(fVolume) < fEpsilon) {
    fprintf(stderr, "MESG(%s):\n", pcFName);
    fprintf(stderr, "\tTiny Volume of Tet = %lf\n", fVolume);
    if(fSum < 0.0) {
      fAlpha = - fAlpha;
      fBeta  = - fBeta;
      fGamma = - fGamma;
      fDelta = - fDelta;
    }
  }
  else {
    fAlpha /= fSum;
    fBeta  /= fSum;
    fGamma /= fSum;
    fDelta /= fSum;
  }
  /* fSigma  = fAlpha + fBeta + fGamma + fDelta; */
#if HIGH_MEM
  pRed->fQuadCoord[0] = fAlpha;
  pRed->fQuadCoord[1] = fBeta;
  pRed->fQuadCoord[2] = fGamma;
  pRed->fQuadCoord[3] = fDelta;
  pRed->fTetVolume = fVolume;
#endif
  pShockData->fGeom = fabs(fVolume);
  pShockData->fBaryCoord[0] = fAlpha;
  pShockData->fBaryCoord[1] = fBeta;
  pShockData->fBaryCoord[2] = fGamma;
  pShockData->fBaryCoord[3] = fDelta;

  iFlag = 0;
  if(fAlpha < 0.0) iFlag++;
  if(fBeta  < 0.0) iFlag++;
  if(fGamma < 0.0) iFlag++;
  if(fDelta < 0.0) iFlag++;

  switch(iFlag) {
  case 0:
    pRed->isType = RED_I; /* Circum inside Tet : All flows Inward */
    break;
  case 1:
    pRed->isType = RED_II; /* Circum Outside if one face: 1 flow out */
    break;
  case 2:
  case 3:
  case 4:
    pRed->isType = RED_III; /* Circum Out of 2 faces: 2 flows out */
    break;
  default:
    fprintf(stderr, "WARNING(%s): Quad Coords =\n", pcFName);
    fprintf(stderr, "\t(%lf , %lf , %lf , %lf)\n",
      fAlpha, fBeta, fGamma, fDelta);
    fprintf(stderr, "\tGeneA: %d: (%lf , %lf , %lf)\n", iGeneA, fAx,fAy,fAz);
    fprintf(stderr, "\tGeneB: %d: (%lf , %lf , %lf)\n", iGeneB, fBx,fBy,fBz);
    fprintf(stderr, "\tGeneC: %d: (%lf , %lf , %lf)\n", iGeneC, fCx,fCy,fCz);
    fprintf(stderr, "\tGeneD: %d: (%lf , %lf , %lf)\n", iGeneD, fDx,fDy,fDz);
    fprintf(stderr, "\tVolume = %lf \n", fVolume);
#if HIGH_MEM
    fprintf(stderr, "\tCircumCenter coords = (%lf , %lf , %lf)\n",
      pRed->CircumSphere.fPosX, pRed->CircumSphere.fPosY,
      pRed->CircumSphere.fPosZ);
#endif
    return(FALSE);
    break;
  }
  return(TRUE);
}

/*------------------------------------------------------------*\
 * Function: GetTriCoordOfCurve
 *
 * Usage: Evaluates the trilinear coordinates of the shock
 *    curve source with respect to its associated triplet
 *    of generators.
 *
\*------------------------------------------------------------*/

int
GetTriCoordOfCurve(InputData *pInData, ShockCurve *pGreen,
       ShockData *pShockData)
{
  char    *pcFName = "GetTriCoordOfCurve";
  int    iGeneA,iGeneB,iGeneC, iFlag, iGreen;
  static double  dEpsilon = (double) E_EPSILON;
  double  dAx,dAy,dAz, dBx,dBy,dBz, dCx,dCy,dCz, dSum;
  double  dTmpX,dTmpY,dTmpZ, dMuAB,dMuAC,dMuBC, dTmp;
  double  dAlpha,dBeta,dGamma, dAreaSq;
  InputData  *pGeneA, *pGeneB, *pGeneC;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB, *pGeneCoordC;

  /* Preliminaries */

  iGeneA = pGreen->iGene[0];
  iGeneB = pGreen->iGene[1];
  iGeneC = pGreen->iGene[2];

  pGeneA = pInData+iGeneA;
  pGeneCoordA  = &(pGeneA->Sample);
  dAx = (double) pGeneCoordA->fPosX;
  dAy = (double) pGeneCoordA->fPosY;
  dAz = (double) pGeneCoordA->fPosZ;

  pGeneB = pInData+iGeneB;
  pGeneCoordB  = &(pGeneB->Sample);
  dBx = (double) pGeneCoordB->fPosX;
  dBy = (double) pGeneCoordB->fPosY;
  dBz = (double) pGeneCoordB->fPosZ;

  pGeneC = pInData+iGeneC;
  pGeneCoordC  = &(pGeneC->Sample);
  dCx = (double) pGeneCoordC->fPosX;
  dCy = (double) pGeneCoordC->fPosY;
  dCz = (double) pGeneCoordC->fPosZ;

  /* Compute the Mu's : Squares of diameters linking pairs of sources */

  dTmpX = dBx - dAx; /* Distance AB */
  dTmpX *= dTmpX;
  dTmpY = dBy - dAy;
  dTmpY *= dTmpY;
  dTmpZ = dBz - dAz;
  dTmpZ *= dTmpZ;

  dMuAB = dTmpX + dTmpY + dTmpZ;
  /* dAB = sqrt(dMuAB); */

  dTmpX = dCx - dAx; /* Distance AC */
  dTmpX *= dTmpX;
  dTmpY = dCy - dAy;
  dTmpY *= dTmpY;
  dTmpZ = dCz - dAz;
  dTmpZ *= dTmpZ;

  dMuAC = dTmpX + dTmpY + dTmpZ;
  /* dAC = sqrt(dMuAC); */

  dTmpX = dCx - dBx; /* Distance BC */
  dTmpX *= dTmpX;
  dTmpY = dCy - dBy;
  dTmpY *= dTmpY;
  dTmpZ = dCz - dBz;
  dTmpZ *= dTmpZ;

  dMuBC = dTmpX + dTmpY + dTmpZ;
  /* dBC = sqrt(dMuBC); */

  /* Find Longest side */
  if(dMuAB > dMuAC) {
    if(dMuAB > dMuBC) {
      pGreen->fMaxSideLengthSq = (float) dMuAB;
      if(dMuAC < dMuBC)
  pGreen->fMinSideLengthSq = (float) dMuAC;
      else
  pGreen->fMinSideLengthSq = (float) dMuBC;
    }
    else {
      pGreen->fMaxSideLengthSq = (float) dMuBC;
      pGreen->fMinSideLengthSq = (float) dMuAC;
    }
  }
  else if(dMuAC > dMuBC) {
    pGreen->fMaxSideLengthSq = (float) dMuAC;
    if(dMuAB < dMuBC)
      pGreen->fMinSideLengthSq = (float) dMuAB;
    else
      pGreen->fMinSideLengthSq = (float) dMuBC;
  }
  else {
    pGreen->fMaxSideLengthSq = (float) dMuBC;
    pGreen->fMinSideLengthSq = (float) dMuAB;
  }

  pGreen->fPerimeter = (float) (sqrt(dMuAB) + sqrt(dMuAC) + sqrt(dMuBC));


  /* Compute the (signed) trilinear coordinates */

  dAlpha = dMuAC + dMuAB - dMuBC;
  dBeta  = dMuBC + dMuAB - dMuAC;
  dGamma = dMuBC + dMuAC - dMuAB;

  dTmp = fabs(dMuBC * dAlpha + dMuAC * dBeta + dMuAB * dGamma);
  if(dTmp < dEpsilon) {
    pGreen->fTriArea = 0.0;
    dAlpha *= dMuBC;
    dBeta  *= dMuAC;
    dGamma *= dMuAB;
    /* Sum of coord. should be equal to one */
    dSum = fabs(dAlpha + dBeta + dGamma - 1.0);
#if FALSE
    fprintf(stderr, "WARNING(%s): 16 AreaSq = %lf for Shock curve %d\n",
      pcFName, (float) dTmp, iGreen);
    if(dSum > 0.01) {
      fprintf(stderr, "\tTri Coords = (%lf , %lf , %lf), Sum = %lf .\n",
        dAlpha, dBeta, dGamma, (dAlpha+dBeta+dGamma));
    }
#endif
  }
  else {
    dAreaSq = dTmp / 16.0;
    pGreen->fTriArea = (float) sqrt(dAreaSq);
    dAlpha *= (dMuBC / dTmp);
    dBeta  *= (dMuAC / dTmp);
    dGamma *= (dMuAB / dTmp);
    /* Sum of coord. should be equal to one */
    dSum = fabs(dAlpha + dBeta + dGamma - 1.0);
    if(dSum > 0.01) {
      /* If sum is near 0: likely that genes are co-linear */
      fprintf(stderr, "WARNING(%s): Shock curve with Tri Coords =\n",
        pcFName);
      fprintf(stderr, "\t(%lf , %lf , %lf) , Sum = %lf .\n",
        dAlpha, dBeta, dGamma,  (dAlpha+dBeta+dGamma));
    }
  }

  pGreen->fTriCoord[0] = (float) dAlpha;
  pGreen->fTriCoord[1] = (float) dBeta;
  pGreen->fTriCoord[2] = (float) dGamma;

  pShockData->fGeom = (float) sqrt(dAreaSq);
  pShockData->fBaryCoord[0] = (float) dAlpha;
  pShockData->fBaryCoord[1] = (float) dBeta;
  pShockData->fBaryCoord[2] = (float) dGamma;

  iFlag = 0;
  if(dAlpha < 0.0) iFlag++;
  if(dBeta  < 0.0) iFlag++;
  if(dGamma < 0.0) iFlag++;

  switch(iFlag) {
  case 0:
    pGreen->isType = GREEN_I;
    break;
  case 1:
    pGreen->isType = GREEN_II;
    break;
  default:
    fprintf(stderr, "ERROR(%s): Too many neg. Tri Coords:\n", pcFName);
    fprintf(stderr, "\t(%lf , %lf , %lf)\n",
      dAlpha, dBeta, dGamma);
    return(FALSE);
    break;
  }

  return(TRUE);
}

#if FALSE

/*------------------------------------------------------------*\
 * Function: GetSigCoordOfCurve
 *
 * Usage: Evaluates the sign of trilinear coordinates of the shock
 *    curve with respect to its associated triangle.
 *
\*------------------------------------------------------------*/

int
GetSigCoordOfCurve(InputData *pInData, ShockCurve *pGreen)
{
  char    *pcFName = "GetSigCoordOfCurve";
  int    iGeneA,iGeneB,iGeneC, iFlag;
  static float  fEpsilon = (float) E_EPSILON; /* 0.000001 */
  float    fTmp;
  double  dAx,dAy,dAz, dBx,dBy,dBz, dCx,dCy,dCz;
  double  dTmpX,dTmpY,dTmpZ, dMuAB,dMuAC,dMuBC;
  double  dAlpha,dBeta,dGamma, dAreaSq;
  InputData  *pGeneA, *pGeneB, *pGeneC;
  Pt3dCoord  *pGeneCoordA, *pGeneCoordB, *pGeneCoordC;

  /* Preliminaries */

  iGeneA = pGreen->iGene[0];
  iGeneB = pGreen->iGene[1];
  iGeneC = pGreen->iGene[2];

  pGeneA = pInData+iGeneA;
  pGeneCoordA  = &(pGeneA->Sample);
  dAx = (double) pGeneCoordA->fPosX;
  dAy = (double) pGeneCoordA->fPosY;
  dAz = (double) pGeneCoordA->fPosZ;

  pGeneB = pInData+iGeneB;
  pGeneCoordB  = &(pGeneB->Sample);
  dBx = (double) pGeneCoordB->fPosX;
  dBy = (double) pGeneCoordB->fPosY;
  dBz = (double) pGeneCoordB->fPosZ;

  pGeneC = pInData+iGeneC;
  pGeneCoordC  = &(pGeneC->Sample);
  dCx = (double) pGeneCoordC->fPosX;
  dCy = (double) pGeneCoordC->fPosY;
  dCz = (double) pGeneCoordC->fPosZ;

  /* Compute the Mu's : Squares of diameters linking pairs of sources */

  dTmpX = dBx - dAx; /* Distance AB */
  dTmpX *= dTmpX;
  dTmpY = dBy - dAy;
  dTmpY *= dTmpY;
  dTmpZ = dBz - dAz;
  dTmpZ *= dTmpZ;

  dMuAB = dTmpX + dTmpY + dTmpZ;

  dTmpX = dCx - dAx; /* Distance AC */
  dTmpX *= dTmpX;
  dTmpY = dCy - dAy;
  dTmpY *= dTmpY;
  dTmpZ = dCz - dAz;
  dTmpZ *= dTmpZ;

  dMuAC = dTmpX + dTmpY + dTmpZ;

  dTmpX = dCx - dBx; /* Distance BC */
  dTmpX *= dTmpX;
  dTmpY = dCy - dBy;
  dTmpY *= dTmpY;
  dTmpZ = dCz - dBz;
  dTmpZ *= dTmpZ;

  dMuBC = dTmpX + dTmpY + dTmpZ;

  /* Compute the (signed) trilinear coordinates *\
  \* use only the numerator for signe test      */

  dAlpha = dMuAC + dMuAB - dMuBC;
  dBeta  = dMuBC + dMuAB - dMuAC;
  dGamma = dMuBC + dMuAC - dMuAB;

  dAreaSq = dMuBC * dAlpha + dMuAC * dBeta + dMuAB * dGamma;
  dAreaSq /= 16.0;
  fTmp = pGreen->fTriArea = (float) sqrt(dAreaSq);
  if(fTmp < fEpsilon) {
    fprintf(stderr, "WARNING(%s): Triangle Area = %lf\n",
      pcFName, fTmp);
    fprintf(stderr, "\tfor triplet (%d , %d , %d)\n",
      iGeneA, iGeneB, iGeneC);
    fprintf(stderr, "\tSide lengths^2 = (%lf , %lf , %lf)\n",
      dMuAC, dMuAB, dMuBC);
    return(FALSE);
  }

  pGreen->fTriCoord[0] = (float) dAlpha;
  pGreen->fTriCoord[1] = (float) dBeta;
  pGreen->fTriCoord[2] = (float) dGamma;

  iFlag = 0;
  if(dAlpha < 0.0) iFlag++;
  if(dBeta  < 0.0) iFlag++;
  if(dGamma < 0.0) iFlag++;

  switch(iFlag) {
  case 0:
    pGreen->isType = GREEN_I;
    break;
  case 1:
    pGreen->isType = GREEN_II;
    break;
  default:
    fprintf(stderr, "ERROR(%s):\n", pcFName);
    fprintf(stderr, "\tTrilinear Coords = (%lf , %lf , %lf)\n",
      dAlpha, dBeta, dGamma);
    fprintf(stderr, "\tfor triplet (%d , %d , %d)\n",
      iGeneA, iGeneB, iGeneC);
    fprintf(stderr, "\tSide lengths^2 = (%lf , %lf , %lf)\n",
      dMuAC, dMuAB, dMuBC);
    return(FALSE);
    break;
  }

  return(TRUE);
}

#endif

/* -- EoF -- */
