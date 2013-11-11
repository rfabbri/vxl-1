#ifndef _INTERSECTIONS_H
#define _INTERSECTIONS_H

#include "ishock-common.h"
#include "boundary.h"
#include "ishock.h"

IntrinsicIntersection ComputeIntersection 
  (SIPointPoint* shL, SIPointPoint* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointPoint* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointPoint* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointPoint* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointPoint* shL, SIPointArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SIPointPoint* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SILineLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLineContact* shL, SILineArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIPointPoint* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SILineArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArcContact* shL, SIArcArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SIPointPoint* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SILineLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointLine* shL, SILineArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIPointPoint* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SILineArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIPointArc* shL, SIArcArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SILineLine* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineLine* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineLine* shL, SILineLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineLine* shL, SILineArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SIPointLineContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SIPointLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SILineLine* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SILineArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SILineArc* shL, SIArcArc* shR);

IntrinsicIntersection ComputeIntersection 
  (SIArcArc* shL, SIPointArcContact* shR);
IntrinsicIntersection ComputeIntersection 
  (SIArcArc* shL, SIPointArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIArcArc* shL, SILineArc* shR);
IntrinsicIntersection ComputeIntersection 
  (SIArcArc* shL, SIArcArc* shR);

void set_AAA_PAA_APA_PAP_AAP_ABCs (int s, int st, double theta,
      double a, double b2, double c,
      double at, double bt2, double ct,
      double &A, double &B, double &C);

#endif
