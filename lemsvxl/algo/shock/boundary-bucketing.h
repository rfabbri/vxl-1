#ifndef  _BOUNDARY_BUCKETING_H
#define  _BOUNDARY_BUCKETING_H

#include "boundary.h"
#include "belements-bucketing.h"
#include "bucket.h"

class Boundary_Bucketing : public Boundary
{
public:
  Bucketing*  bucketing;

  Boundary_Bucketing (BOUNDARY_LIMIT newbndlimit=NO_LIMIT);
  virtual ~Boundary_Bucketing ();

  virtual void _Initialize (BOUNDARY_LIMIT newbndlimit=NO_LIMIT);  //initialize the boundary
  virtual void Clear();
  virtual bool delGUIElement (BElement* elm);
  //virtual void mergeDuplicatePoints (BPoint* pt1, BPoint* pt2);

  virtual BPoint* addGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent=-100.0, double conf=0.0);
  virtual BPoint* addNonGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent=-100.0);

  virtual BElement* addGUILine (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey);
  virtual BElement* addGUILineBetween (BPoint* spt, BPoint* ept);
  virtual BElement* addGUILineBetween (BPoint* spt, Point end_pt, BPoint** newEndPt);

  virtual BElement* addGUIArcBetween (BPoint* spt, BPoint* ept, Point center, 
                          double r, ARC_NUD nud, ARC_NUS nus);
  virtual BElement* addGUIArc (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey,
                      COORD_TYPE centerx, COORD_TYPE centery, double r, ARC_NUD nud, ARC_NUS nus);
  virtual BElement* addGUIArcBetween (BPoint* bspt, Point ept, Point center, 
                          double r, ARC_NUD nud, ARC_NUS nus);
  void clearVisibilityConstraints (void);
};

#endif
