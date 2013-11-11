// SCDM.CPP

#include "biarc_shock.h"
#include "boundary.h"
#include "scdm.h"


SCDM_SEED_TYPE SCDM_seed_type = SCDM_BIARCS;

//Add a sample point to the end of the list, SeedPoint* pointlist
void SCDMSeed::addPoint (double x, double y, BPoint* bpoint)
{
  SeedPoint* newpoint = new SeedPoint (x, y);
  newpoint->bpoint = bpoint;
  nSamplePoints++;

  //if the first one...
  if (pointlist==NULL) {
    pointlist = newpoint;
    lastpoint = newpoint;
  }
  else {
    lastpoint->next = newpoint;
    lastpoint = newpoint;
  }
}

//Traverse through the list, SeedPoint* pointlist
//to find the point (x,y) and modify it
void SCDMSeed::modifyPoint (double x, double y, 
                double newx, double newy, double newtangent)
{
  SeedPoint* current = pointlist;

  while (current != NULL) {
    if (current->pt.x == x && current->pt.y == y)
      break;
    current = current->next;
  }

  //now current is the one...
  current->tangent = newtangent;
}

void SCDMSeed::delAllSeedPoints (void)
{
  SeedPoint* current = pointlist;
  SeedPoint* temp_cur;

  while (current != NULL) {
    temp_cur = current;
    current = current->next;

    delete temp_cur;
  }
  pointlist = NULL;
  lastpoint = NULL;
  nSamplePoints=0;
}

// Operate on the scdm->SeedCurrent->pointlist data structure
//1)Compute tangent information
//2)Fit BiArc from two pair of point-tangents
//3)Add the BiArc
#include <vcl_cassert.h>
void SCDMSeed::FitBiArc (Boundary* boundary)
{
  ///////////////////////////////
  //1)Compute tangent information 

  //Special Cases
  if (nSamplePoints==0 || nSamplePoints==1)
    return;
  if (nSamplePoints==2) { //add a line
    return;  
  }
  if (nSamplePoints==3) { //add three lines
    return;
  }

  // go through pointlist,
  // call getTangentFromThreePoints() for the 123, 234, 345, 456, ...
  // keep 12 in temp for n12.
  SeedPoint* first_point = pointlist;
  SeedPoint* second_point = pointlist->next;
  SeedPoint* i = pointlist;
  SeedPoint *ii, *iii;

  while (i != NULL) {
    ii = i->next;
    if (ii==NULL) ii = first_point;
    iii = ii->next;
    if (iii==NULL) iii = first_point;
    ii->tangent = getTangentFromThreePoints (i->pt.x, i->pt.y, iii->pt.x, iii->pt.y, 
                ii->pt.x, ii->pt.y);
    assert(ii->tangent<=2*M_PI && ii->tangent>=0);
    /*CatchBugIfNot (ii->tangent<=2*M_PI && ii->tangent>=0, "ii->tangent<=2*M_PI && ii->tangent>=0", 
            "SCDMSeed::FitBiArc()");*/
    i = i->next;
  }  

  /////////////////////////////////////////////
  //2)Fit BiArc from two pair of point-tangents
  // go through the pointlist,
  // call BiArc::compute_biarc_params() to get BiArc
  BiArcShock biarc;
  Point mid, center1, center2;
  double R1, R2;
  int dir1;
  int dir2;
  first_point = pointlist;
  i = pointlist;
  while (i != NULL) {
    ii = i->next;
    if (ii==NULL) ii = first_point;
    biarc.set_start_params (i->pt, i->tangent);
    biarc.set_end_params (ii->pt, ii->tangent);
    biarc.compute_biarc_params();
    biarc.compute_other_stuff();
    mid = biarc.bi_arc_params.end1;
    center1 = biarc.bi_arc_params.center1;
    center2 = biarc.bi_arc_params.center2;
    R1 = vcl_fabs(biarc.bi_arc_params.radius1);
    R2 = vcl_fabs(biarc.bi_arc_params.radius2);
    dir1 = biarc.bi_arc_params.dir1;
    dir2 = biarc.bi_arc_params.dir2;
    BPoint* mid_pt = boundary->addGUIPoint (mid.x, mid.y);
    //catch bigger bugs
    /*ASSERT(mid_pt!=0);
    CatchBugIfNot (mid_pt!=0, "mid_pt!=0", 
            "SCDMSeed::FitBiArc()");*/

    //now to add an arc between the points
    //use a line instead if the curvature is very small
    if (R1 < MAX_BIARC_RADIUS){
      BArc* barc1 = (BArc*) boundary->addGUIArcBetween (i->bpoint, mid_pt, 
              center1, R1, (ARC_NUD) dir1, ARC_NUS_LARGE);
    }
    else {
      BLine* bline1 = (BLine*) boundary->addGUILineBetween(i->bpoint, mid_pt);
    }

    if (R2 < MAX_BIARC_RADIUS){
      BArc* barc2 = (BArc*) boundary->addGUIArcBetween (mid_pt, ii->bpoint, 
              center2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
    }
    else {
      BLine* bline2 = (BLine*) boundary->addGUILineBetween(mid_pt, ii->bpoint);
    }

    i = i->next;
  }
}

///////////////////////////////////////////////////////////

SCDMSeed* SCDM::addSCDMSeed ()
{
  idcounter++;
  SCDMSeed* newseed = new SCDMSeed (idcounter);

  //if the first one...
  if (SeedHead == NULL) {
    SeedCurrent = newseed;
    SeedHead = newseed;
  }
  else {
    SeedCurrent->next = newseed;
    SeedCurrent = newseed;
  }

  return SeedCurrent;
}

//Traverse through the list, SCDMSeed* SeedHead
//to find the seed 'seed' and remove it from list
void SCDM::delSCDMSeed (SCDMSeed* seed)
{
  SCDMSeed* current = SeedHead;
  SCDMSeed* previous = NULL;

  //first one
  if (SeedHead == seed) {
    SCDMSeed* temp = SeedHead->next;
    delete SeedHead;
    SeedHead = temp;
    return;
  }

  while (current != NULL) {
    if (current == seed) {
      //found it. This is the one. Delete it.
      //previous is always not NULL
      previous->next = current->next;
      delete current;
      return;
    }
    previous = current;
    current = current->next;
  }
}

void SCDM::delAllSCDMSeeds ()
{
  SCDMSeed* current = SeedHead;
  SCDMSeed* temp_cur;

  while (current != NULL) {
    temp_cur = current;
    current = current->next;

    delete temp_cur;
  }
  SeedHead = NULL;
  SeedCurrent = NULL;
}

////////////////////////////////////////////////////////

