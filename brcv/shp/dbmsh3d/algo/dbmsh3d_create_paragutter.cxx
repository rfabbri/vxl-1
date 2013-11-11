/*------------------------------------------------------------*\
 * File: CreateParaGutter_v2.c
 *
 * Functions:
 *   main
 *
 * History:
 *  #0: July 2002, by F.Leymarie
 *   #1: July 2003: Fully closed gutter.
 * #2: March 12, 2004: Migrate to VXL by MingChing Chang
 *
\*------------------------------------------------------------*/

//: Ming: The drand48 subroutine and the erand48 subroutine return 
//  positive double-precision floating-point values uniformly distributed over the interval [0.0, 1.0).

#include <vcl_cmath.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbmsh3d/dbmsh3d_utils.h>


class Pt3dCoord
{
public:
   float    fPosX, fPosY, fPosZ;
   
   Pt3dCoord () {}
   Pt3dCoord (float fx, float fy, float fz) {
      fPosX = fx;
      fPosY = fy;
      fPosZ = fz;
   }
};

#define  PARA_A         0.3      //z=a*y^2
#define  GUTTER_A       (-0.1)   //z=a*x^2, set to 0 for linear parabola gutter. 

#define  MIN_X          -5
#define  MAX_X          5
#define  STEP_X         0.2f
#define  STEP_Y         0.1f
#define  STEP_Z         0.1f

#define  BOTTOM_B       0        //z=bx+c for the bottom (slope) 0.1
#define  BOTTOM_C       3.9      //z=bx+c for the bottom (depth) 4.2 4.0 3.9 3.82 3.6
#define  PLANE_STEP     0.1f

/*
void dbmsh3d_mesh::GenerateParabolaGutter ()
{
   float x, y, z;

   //compute minY maxY using the plane equation.
   double MAX_Y = sqrt ((BOTTOM_B*MAX_X+BOTTOM_C)/PARA_A);
   double MIN_Y = -MAX_Y;

   //1)The surface of the parabolic gutter
   for (x = MIN_X; x<=MAX_X; x+=STEP_X) {

      //ZS: the origin of each parabola.
      //ZS = 0; //for pure parabolic gutter
      double ZS = GUTTER_A * x*x; //for modified parabolic gutter

      double Z_plane = BOTTOM_B * x + BOTTOM_C;

      double shrinkDepth = Z_plane - ZS;
      double r = shrinkDepth / Z_plane;

      for (y = MIN_Y; y<=MAX_Y; y+=STEP_Y) {
         z = r*(PARA_A * y*y) + ZS;

         //threshold
         if (z<Z_plane)
            addPointGene (x, y, z);
      }
   }

   //2)The bottom plane
   for (x = MIN_X; x<=MAX_X; x+=PLANE_STEP) {

      double Z_plane = BOTTOM_B * x + BOTTOM_C;

      for (y = MIN_Y; y<=MAX_Y; y+=PLANE_STEP) {
         z = Z_plane;
         addPointGene (x, y, z);
      }
   }

}
*/


//: Ming: put parameters here
float  fA = 1.0f;
float fB = 0.1f;
float fC = 1.0f;
float fA2 = 0.8f; //: z=-a2.x
float   fP = 1.0f;
float fQ = - 1.0f;
float  fS = 15.0f;
int  iX = 150; //100
int   iY = 150; //100
bool  iFlagRand  = false;
bool iFlagFront = false; //true
bool   iFlagBack  = false; //true


/*
  {"a", _FLOAT, 0, &fA, "Parameter a > 0 of parabola z = a.y^2 (def. = 1.0)"},
  {"b", _FLOAT, 0, &fB, "Parameter b > 0 of roof z = b.x + c (def. = 0.25)"},
  {"c", _FLOAT, 0, &fC, "Parameter c > 0 of roof z = b.x + c (def. = 1.5)"},
  {"p", _FLOAT, 0, &fP, "Parameter p > 0: front end x <= p (def. = 3.0)"},
  {"q", _FLOAT, 0, &fQ, "Parameter q < 0: back end x >= p (def. = -4.0)"},
  {"s", _FLOAT, 0, &fS, "Scaling of coordinates (def. = 20.0)"},
  {"nx", _INT, 0, &iX, "Samples along X axis (def. = 100)"},
  {"ny", _INT, 0, &iY, "Samples along Y axis (def. = 100)"},
  {"rand", _INT, 0, &iFlagRand, "Random positioning: true = 1, false = 0 (def.)."},
  {"front", _INT, 0, &iFlagFront, "Flag: Front end: True = 1 (def.), False = 0"},
  {"back", _INT, 0, &iFlagBack, "Flag: Back end: True = 1 (def.), False = 0"},
*/

/*------------------------------------------------------------*\
 * Function: main
 *
 * Usage: Loads user inputs and creates samples of:
 *      (1) a parabolic gutter
 *      (2) a plane roof
 *   Outputs simple g3d ASCII format: 1 Dim, 2 number of points
 *      3 floating pt. coord. and RGB float values
 *
\*------------------------------------------------------------*/

int CreateParaGutter_main (char* pcFileNameOut)
{
  char      *pcFName = "main(CreateParaGutter_v2)";
  char      *pcOutFile;
  char    cOutFile[128]="";
  FILE    *fp1;
  register int  i,j,k,l;
  //unsigned short usaSeed[3];
  int    iMax, iTot, iTmp;
  float     fTmp, fRangeX,fDeltaX, fRadical, fYright,fYleft;
  float     fRangeY,fDeltaY, fZroof, fX,fY,fZ, fColR,fColG,fColB;
  float      fRangeZ,fDeltaZ, fXtmp,fYtmp;
  float      fXmin,fXmax, fYmin,fYmax, fZmin,fZmax;
  Pt3dCoord  *p3dPts, *pPt;
  Pt3dCoord  *p3dRGB, *pCol;

  ///opt_parse_args(argc, argv, optab);        
  if(pcFileNameOut == NULL){
    vcl_fprintf(stderr,
     "ERROR(%s): Missing output (prefix) filename\n", pcFName);
    vcl_fprintf(stderr, "\n\t use -h for more help.\n");
    exit(-1); }

  if(iX < 1 || iX > 100000) {
    vcl_fprintf(stderr, "ERROR(%s): Incorrect sampling along the X axis = %d\n",
     pcFName, iX);
    exit(-1); }
  if(iY < 1 || iY > 100000) {
    vcl_fprintf(stderr, "ERROR(%s): Incorrect sampling along the Y axis = %d\n",
       pcFName, iY);
    exit(-1); }
  if(fA <= 0.0) {
    vcl_fprintf(stderr,
       "ERROR(%s): Parabola parameter a cannot be non-positive: a = %lf\n",
     pcFName, fA);
    exit(-1); }
  if(fB <= 0.0) {
    vcl_fprintf(stderr,
       "ERROR(%s): Roof parameter b cannot be non-positive: b = %lf\n",
      pcFName, fB);
    exit(-1); }
  if(fC <= 0.0) {
    vcl_fprintf(stderr,
       "ERROR(%s): Roof parameter c cannot be non-positive: c = %lf\n",
      pcFName, fC);
    exit(-1); }
  if(fQ > 0.0) {
    vcl_fprintf(stderr,
     "WARNING(%s): Back end parameter q should not be positive: q = %lf\n",
      pcFName, fQ);
  }
  if(fP <= fQ) {
    vcl_fprintf(stderr, "ERROR(%s): P <= Q\n", pcFName);
    exit(-1); }

  fTmp = fB * fQ + fC;
  if(fTmp < 0.0) {
    vcl_fprintf(stderr, "ERROR(%s):\n",pcFName);
    vcl_fprintf(stderr,
     "\tBack end q too far w/r paramater a: b.q+c = %lf < 0 = %lf\n",
       fTmp, fA);
    exit(-1); }

  //iMax = 4 * iY * iX; //Ming: modify
  iMax = 8 * iY * iX; //Ming: modify


  p3dPts = NULL;
  if((p3dPts = (Pt3dCoord *) calloc(iMax, sizeof(Pt3dCoord))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): CALLOC failed on p3dPts[%d].\n",
       pcFName, iMax);
    return(-2); }
  p3dRGB = NULL;
  if((p3dRGB = (Pt3dCoord *) calloc(iMax, sizeof(Pt3dCoord))) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): CALLOC failed on p3dRGB[%d].\n",
       pcFName, iMax);
    return(-2); }
  vcl_fprintf(stderr, "\tThere will be at most %d points generated...\n",
    iMax);
  
  fZmin = fYmin = fXmin =  100000.0;
  fZmax = fYmax = fXmax = -100000.0;

  fRangeX = fP - fQ;
  fDeltaX = fRangeX / (float) iX;
  pPt = p3dPts-1;
  pCol = p3dRGB-1;
  k = 0;

  if (!iFlagRand) { /* Uniform samplings */
    /* ----- 1st : Parabolic gutter with roof ----- */
    fX = fP; /* Start at Front end */

      for(i = 0; i < iX; i++) { /* Slices along X axis */
         fRadical = (fB * fX + fC) / fA;
        fYright = (float) vcl_sqrt ((double) fRadical);
       fYleft = - fYright;
        fRangeY = fYright - fYleft;
         fDeltaY = fRangeY / (float) iY;

         //: Compute new startz
         float fZs = fA2*fX*fX;

         /* Roof height for this X slice */
        fZroof = fB * fX + fC;

         fY = fYleft; /* Start on far left (negative side) */

        for (j = 0; j < iY; j++) { /* Slices along Y axis */
           /* Parabola locus */
          fZ = fY * fY * fA - fZs;

            pPt++;
            pPt->fPosX = fX;
           pPt->fPosY = fY;
           pPt->fPosZ = fZ;
           if(fX < fXmin) fXmin = fX;
          if(fX > fXmax) fXmax = fX;
          if(fY < fYmin) fYmin = fY;
          if(fY > fYmax) fYmax = fY;
          if(fZ < fZmin) fZmin = fZ;
          if(fZ > fZmax) fZmax = fZ;
          k++;
           pCol++;
           pCol->fPosX = 1.0;  /* RED */
          pCol->fPosY = 0.0;
            pCol->fPosZ = 0.0;

            /* Roof locus */
           pPt++;
            pPt->fPosX = fX;
           pPt->fPosY = fY;
           pPt->fPosZ = fZroof; //- fZs;
          if(fX < fXmin) fXmin = fX;
          if(fX > fXmax) fXmax = fX;
          if(fY < fYmin) fYmin = fY;
          if(fY > fYmax) fYmax = fY;
          if(fZ < fZmin) fZmin = fZ;
          if(fZ > fZmax) fZmax = fZ;
          k++;
           pCol++;
           pCol->fPosX = 0.0;  /* GREEN */
           pCol->fPosY = 0.5;
            pCol->fPosZ = 0.0;
         
            /* Next Y step */
          fY += fDeltaY;

       } /* End of For(j) : Slices along Y axis : j++ */

        fX -= fDeltaX;

    } /* End of For(i) : Slices along X axis : i++ */

     vul_printf (vcl_cout,
             "\t%d uniformly sampled points for parabolic gutter and roof.\n", k);

      /* ----- 2nd : front end ----- */
      l = 0;
      if (iFlagFront) {
       fX = fP;
    
         fRangeZ = fB * fX + fC; /* Roof top : Bottom = 0 */
         fDeltaZ = fRangeZ / (float) iX;
     
         fRadical = (fB * fX + fC) / fA;
        fYright = (float) sqrt((double) fRadical);
         fYleft = - fYright;
        fRangeY = fYright - fYleft;
         fDeltaY = fRangeY / (float) iY;
     
         fY = fYleft; /* Start on far left (negative side) */
        for (j = 0; j < iY; j++) { /* Slices along Y axis */
     
            fZ = fA * fY * fY; /* Parabola == Bottom */
            float fZs = fA2*fX*fX;
            fZ -= fZs;

           while(fZ < fRangeZ) { /* while we have not hit the roof */

              pPt++;
               pPt->fPosX = fX;
              pPt->fPosY = fY;
              pPt->fPosZ = fZ;
              if(fX < fXmin) fXmin = fX;
             if(fX > fXmax) fXmax = fX;
             if(fY < fYmin) fYmin = fY;
             if(fY > fYmax) fYmax = fY;
             if(fZ < fZmin) fZmin = fZ;
             if(fZ > fZmax) fZmax = fZ;
             l++;
              pCol++;
              pCol->fPosX = 0.0;  /* BLUE */
               pCol->fPosY = 0.0;
               pCol->fPosZ = 255.0;
 
               fZ += fDeltaZ;
          }

           /* Next Y step */
          fY += fDeltaY;
       } /* End of For(j) : Slices along Y axis : j++ */

        vul_printf (vcl_cout,
                "\t%d uniformly sampled points added for front end,\n", l);
    } /* End of If(iFlagFront) */
    
      if (iFlagBack) {
        iTmp = l;
         fX = fQ;
       
         fRangeZ = fB * fX + fC; /* Roof top : Bottom = 0 */
         fDeltaZ = fRangeZ / (float) iX;
        
         fRadical = (fB * fX + fC) / fA;
        fYright = (float) sqrt((double) fRadical);
         fYleft = - fYright;
        fRangeY = fYright - fYleft;
         fDeltaY = fRangeY / (float) iY;
        
         fY = fYleft; /* Start on far left (negative side) */
        for (j = 0; j < iY; j++) { /* Slices along Y axis */
           fZ = fA * fY * fY; /* Parabola == Bottom */
            float fZs = fA2*fX*fX;
            fZ -= fZs;

            while (fZ < fRangeZ) { /* while we have not hit the roof */
             pPt++;
               pPt->fPosX = fX;
              pPt->fPosY = fY;
              pPt->fPosZ = fZ;
              if(fX < fXmin) fXmin = fX;
             if(fX > fXmax) fXmax = fX;
             if(fY < fYmin) fYmin = fY;
             if(fY > fYmax) fYmax = fY;
             if(fZ < fZmin) fZmin = fZ;
             if(fZ > fZmax) fZmax = fZ;
             l++;
              pCol++;
              pCol->fPosX = 0.0;  /* BLUE */
               pCol->fPosY = 0.0;
               pCol->fPosZ = 1.0;
               
               fZ += fDeltaZ;
          }

           /* Next Y step */
          fY += fDeltaY;
       } /* End of For(j) : Slices along Y axis : j++ */
        vul_printf (vcl_cout,
                "\t%d uniformly sampled points added for Back end,\n",
                  (l-iTmp));
     } /* End of If(iFlagEnd) */

      iTot = k + l;
     vcl_fprintf(stderr, "\tfor a total of %d samples.\n", iTot);
   } /* End of Uniform Sampling case */
   else { /* if(iFlagRand) : Random samplings */

    ///usaSeed[0] = (unsigned short) iX;
    ///usaSeed[1] = (unsigned short) iY;
    ///usaSeed[2] = (unsigned short) iMax;
    ///seed48(usaSeed);
    vcl_srand ( (unsigned int) (vcl_time(NULL)) );


    /* ----- 1st : Parabolic gutter with roof ----- */

    fX = fP; /* Start at Front end */

    for(i = 0; i < iX; i++) { /* Slices along X axis */

      for(j = 0; j < iY; j++) { /* Slices along Y axis */

 fX = fP + i * fDeltaX;
  /* We add to Xcoord some random fluctuation of max size DeltaX */
 fXtmp = fX + (fDeltaX * (float) (float)rand() / (float)RAND_MAX); /* 0.0 <= drand48 < 1.0 */

 fRadical = (fB * fXtmp + fC) / fA;
  fYright = (float) sqrt((double) fRadical);
   fYleft = - fYright;
  fRangeY = fYright - fYleft;
   fDeltaY = fRangeY / (float) iY;

  /* Start from far left (negative side) */
 fY = fYleft + j * fDeltaY;
 /* We add to Ycoord some random fluctuation of max size DeltaY */
 fYtmp = fY + (fDeltaY * (float) (float)rand() / (float)RAND_MAX); /* 0.0 <= drand48 < 1.0 */

 /* Parabola locus */
 fZ = fYtmp * fYtmp * fA;
   pPt++;
   pPt->fPosX = fXtmp;
  pPt->fPosY = fYtmp;
  pPt->fPosZ = fZ;
  if(fX < fXmin) fXmin = fX;
 if(fX > fXmax) fXmax = fX;
 if(fY < fYmin) fYmin = fY;
 if(fY > fYmax) fYmax = fY;
 if(fZ < fZmin) fZmin = fZ;
 if(fZ > fZmax) fZmax = fZ;
 k++;
  pCol++;
  pCol->fPosX = 1.0;  /* RED */
 pCol->fPosY = 0.0;
   pCol->fPosZ = 0.0;

   /* Roof locus */
  /* We add to Xcoord some random fluctuation of max size DeltaX */
 fXtmp = fX + (fDeltaX * (float) (float)rand() / (float)RAND_MAX); /* 0.0 <= drand48 < 1.0 */
 /* Roof height for this X locus */
  fZroof = fB * fXtmp + fC;
  /* We add to Ycoord some random fluctuation of max size DeltaY */
 fYtmp = fY + (fDeltaY * (float) (float)rand() / (float)RAND_MAX); /* 0.0 <= drand48 < 1.0 */
 pPt++;
   pPt->fPosX = fXtmp;
  pPt->fPosY = fYtmp;
  pPt->fPosZ = fZroof;
 if(fX < fXmin) fXmin = fX;
 if(fX > fXmax) fXmax = fX;
 if(fY < fYmin) fYmin = fY;
 if(fY > fYmax) fYmax = fY;
 if(fZ < fZmin) fZmin = fZ;
 if(fZ > fZmax) fZmax = fZ;
 k++;
  pCol++;
  pCol->fPosX = 0.0;  /* GREEN */
  pCol->fPosY = 128.0;
 pCol->fPosZ = 0.0;

   /* Next Y step */
 fY += fDeltaY;

      } /* End of For(j) : Slices along Y axis : j++ */

      fX -= fDeltaX;

    } /* End of For(i) : Slices along X axis : i++ */

    vcl_fprintf(stderr,
       "\t%d randomly sampled points for parabolic gutter and roof.\n", k);

    /* ----- 2nd : front end ----- */


    l = 0;
    if(iFlagFront) {
      fX = fP;

      fRangeZ = fB * fX + fC; /* Roof top : Bottom = 0 */
      fDeltaZ = fRangeZ / (float) iX;

      fRadical = (fB * fX + fC) / fA;
      fYright = (float) sqrt((double) fRadical);
      fYleft = - fYright;
      fRangeY = fYright - fYleft;
      fDeltaY = fRangeY / (float) iY;

      fY = fYleft; /* Start on far left (negative side) */
      for(j = 0; j < iY; j++) { /* Slices along Y axis */

 fZ = fA * fY * fY; /* Parabola == Bottom */

  while(fZ < fRangeZ) { /* while we have not hit the roof */

    pPt++;
   pPt->fPosX = fX;
     pPt->fPosY = fY;
     pPt->fPosZ = fZ;
     if(fX < fXmin) fXmin = fX;
    if(fX > fXmax) fXmax = fX;
    if(fY < fYmin) fYmin = fY;
    if(fY > fYmax) fYmax = fY;
    if(fZ < fZmin) fZmin = fZ;
    if(fZ > fZmax) fZmax = fZ;
    l++;
     pCol++;
     pCol->fPosX = 0.0;  /* BLUE */
   pCol->fPosY = 0.0;
   pCol->fPosZ = 1.0;
 
     fZ += fDeltaZ;
  }

  /* Next Y step */
 fY += fDeltaY;

      } /* End of For(j) : Slices along Y axis : j++ */

      vcl_fprintf(stderr,
        "\t%d uniformly sampled points added for front end,\n", l);

    } /* End of If(iFlagFront) */
    
    if(iFlagBack) {
      iTmp = l;
      fX = fQ;

      fRangeZ = fB * fX + fC; /* Roof top : Bottom = 0 */
      fDeltaZ = fRangeZ / (float) iX;

      fRadical = (fB * fX + fC) / fA;
      fYright = (float) sqrt((double) fRadical);
      fYleft = - fYright;
      fRangeY = fYright - fYleft;
      fDeltaY = fRangeY / (float) iY;

      fY = fYleft; /* Start on far left (negative side) */
      for(j = 0; j < iY; j++) { /* Slices along Y axis */

 fZ = fA * fY * fY; /* Parabola == Bottom */

  while(fZ < fRangeZ) { /* while we have not hit the roof */

    pPt++;
   pPt->fPosX = fX;
     pPt->fPosY = fY;
     pPt->fPosZ = fZ;
     if(fX < fXmin) fXmin = fX;
    if(fX > fXmax) fXmax = fX;
    if(fY < fYmin) fYmin = fY;
    if(fY > fYmax) fYmax = fY;
    if(fZ < fZmin) fZmin = fZ;
    if(fZ > fZmax) fZmax = fZ;
    l++;
     pCol++;
     pCol->fPosX = 0.0;  /* BLUE */
   pCol->fPosY = 0.0;
   pCol->fPosZ = 1.0;
 
     fZ += fDeltaZ;
  }

  /* Next Y step */
 fY += fDeltaY;

      } /* End of For(j) : Slices along Y axis : j++ */

      vcl_fprintf(stderr,
        "\t%d uniformly sampled points added for Back end,\n",
         (l-iTmp));

    } /* End of If(iFlagEnd) */

    iTot = k + l;
    vcl_fprintf(stderr, "\tfor a total of %d samples.\n", iTot);

  } /* End of Random Sampling case */

  /* #endif */

  if(iTot > iMax) {
    vcl_fprintf(stderr, "ERROR(%s): Overflow in samples > %d\n", pcFName, iMax);
    exit(-2);
  }

  /* --- Color samples --- */

#if 0
  pCol = p3dRGB-1;
  iHalf = k / 2;
  j = 0;
  for(i = 0; i < iHalf; i++) {
    pCol++;
    pCol->fPosX = 1.0; /* Parabola sample painted Red */
    pCol->fPosY = 0.0;
    pCol->fPosZ = 0.0;
    j++;
    pCol++;
    pCol->fPosX = 0.0; /* Roof sample painted Blue */
    pCol->fPosY = 0.0;
    pCol->fPosZ = 1.0;
    j++;
  }
  if(j != k) {
    vcl_fprintf(stderr, "ERROR(%s):\n", pcFName);
    vcl_fprintf(stderr, "\tPainted %d samples instead of total of %d\n", j, k);
    exit(-2);
  }
  for(i = 0; i < l; i++) {
    pCol++;
    pCol->fPosX = 0.0; /* Front end sample painted green */
    pCol->fPosY = 1.0;
    pCol->fPosZ = 0.0;
  }
#endif

  vcl_fprintf(stderr, "\tRange of coordinates values:\n");
  vcl_fprintf(stderr, "\t\t%f < X < %f\n", fXmin, fXmax);
  vcl_fprintf(stderr, "\t\t%f < Y < %f\n", fYmin, fYmax);
  vcl_fprintf(stderr, "\t\t%f < Z < %f\n", fZmin, fZmax);

  /* ---- Generate g3d file ---- */

  vcl_strcpy(cOutFile, pcFileNameOut);
  if(iFlagRand)
    vcl_strcat(cOutFile, "Rand.g3d");
  else
    vcl_strcat(cOutFile, "Unif.g3d");
  pcOutFile = &cOutFile[0];

  fp1 = NULL;
  if((fp1 = fopen(pcOutFile, "w")) == NULL) {
    vcl_fprintf(stderr, "ERROR(%s): Can't open output file %s\n", pcFName,
      pcOutFile);
    exit(-1); }
  
  vcl_fprintf(fp1, "3\n");  /* ID (dimensions) */
  vcl_fprintf(fp1, "%d\n", iTot); /* Number of points */

  pCol = p3dRGB-1;
  pPt  = p3dPts-1;
  for(i = 0; i < iTot; i++) {
    pPt++;
    fX = pPt->fPosX * fS;
    fY = pPt->fPosY * fS;
    fZ = pPt->fPosZ * fS;
    pCol++;
    fColR = pCol->fPosX;
    fColG = pCol->fPosY;
    fColB = pCol->fPosZ;
    vcl_fprintf(fp1, "%f %f %f %.1f %.1f %.1f\n",
     fX, fY, fZ, fColR, fColG, fColB);
  }

  vcl_fprintf(stderr, "\tScaling factor: s = %lf\n", fS);
  vcl_fprintf(stderr, "\tScaled Range of coordinates values:\n");
  vcl_fprintf(stderr, "\t\t%lf < X < %f\n", (fXmin*fS), (fXmax*fS));
  vcl_fprintf(stderr, "\t\t%lf < Y < %f\n", (fYmin*fS), (fYmax*fS));
  vcl_fprintf(stderr, "\t\t%lf < Z < %f\n", (fZmin*fS), (fZmax*fS));

  fclose(fp1);
  free(p3dPts); p3dPts = NULL;
  free(p3dRGB); p3dRGB = NULL;

  vcl_fprintf(stderr,
    "MESG(%s): This is it!\n\tThe Parabolic gutter was saved in file %s\n",
    pcFName, pcOutFile);

  return(true);
}

/* ---- EoF ---- */
