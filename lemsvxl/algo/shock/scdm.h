// SCDM.H

#ifndef _SCDM_H
#define _SCDM_H

#include "boundary.h"
#include "ishock-common.h"

typedef enum {
  BOGUS_SCDM_SEED_TYPE,
  SCDM_POINTS=1,
  SCDM_LINES=2,
  SCDM_BIARCS=3
} SCDM_SEED_TYPE;

//1D link list of points, 
// a ordered point-and-tangent description of a seed
class SeedPoint {
  public:
    Point pt;
    BPoint* bpoint;
    double tangent; //estimated tangent at this point

    SeedPoint (double newx, double newy, double newtangent=0) 
    { pt.x = newx; pt.y = newy; bpoint = NULL; tangent = newtangent; next = NULL;}

    SeedPoint* next;
};

//ASSUMPTION:
// 1) Ordered Point List
// 2) No two points at the same position
class SCDMSeed {
  public:
    int id;
    int nSamplePoints;
    SeedPoint* pointlist;
    SeedPoint* lastpoint;

    SCDMSeed (int newid) 
    { id = newid; nSamplePoints = 0;
      pointlist = NULL; lastpoint = NULL; next = NULL; }
    ~SCDMSeed () { delAllSeedPoints (); }

    void addPoint (double x, double y, BPoint* bpoint); //add to the last point
    //addPointAfterPoint (SeedPoint* point); //add after the point
    void modifyPoint (double x, double y, double newx, double newy, double newtangent);
    //delPoint (SeedPoint* point);
    void delAllSeedPoints (void);

    void FitBiArc (Boundary* boundary);

    SCDMSeed* next;
};

class SCDM
{
  public:
    SCDM () { nNumberOfSeeds=0; idcounter=0; 
           SeedHead=NULL; SeedCurrent=NULL; }
    ~SCDM () { delAllSCDMSeeds (); }

    int idcounter;
    int nNumberOfSeeds;
    SCDMSeed* SeedHead;
    SCDMSeed* SeedCurrent;

    SCDMSeed* addSCDMSeed ();
    //SCDMSeed* mergeSCDMSeed (SCDMSeed* seed1, SCDMSeed* seed2);
    void delSCDMSeed (SCDMSeed* seed);
    //bool delSCDMSeed (int seedid); //return false if fail

    void delAllSCDMSeeds ();
};


#endif
