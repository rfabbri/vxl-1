// ISHOCK.CPP

#include <extrautils/msgout.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>

//using namespace std;
#include <vcl_string.h>
#include <vcl_cmath.h>
#include <vcl_map.h>

//#include <stdio.h>
#include <time.h>

#include "ishock-common.h"
#include "ishock.h"
#include "bucket.h"

//##########################################################
// Global Variables
//##########################################################

void IShock::moveASILinkToInfinity (SILink* elm)
{
  //1)First remove the shock element
  SIElmList.erase( 
    A_R_IDpair(elm->isActive(), 
      R_IDpair(elm->simTime(),elm->id())) ); 

  //3)update the shock's simulation time
   elm->setSimTime (ISHOCK_DIST_HUGE);

  //6)then reinsert it into the appropriate place
  SIElmList.insert(SIElmPair(A_R_IDpair(elm->isActive(), 
    R_IDpair(elm->simTime(),elm->id())), elm));

  //8)Update current shocks's Drawing
   updateShockDrawing (elm);
}

void IShock::moveASILinkToAJunction (IntrinsicIntersection inter, SILink* elm, DIRECTION dir)
{
  //1) Remove it from the GD list
  if (elm->graph_type()==SIElement::LINK &&
     elm->label() != SIElement::CONTACT)
    GDList.erase(Sal_IDpair(elm->dPnCost(), elm->id()));

  //2) Update it's sim time
  updateASIElementSimTime(elm, inter.R);

  //3) Update Ltau, Rtau, and endVector
  if (dir==LEFT) { //5-1)elm is the Left Shock
    elm->setLeTau (inter.LSLtau);
    elm->setReTau (inter.LSRtau);
    elm->setEndVector (inter.newLtau);
  }
  else { //5-2)elm is the Right Shock
    elm->setLeTau (inter.RSLtau);
    elm->setReTau (inter.RSRtau);
    elm->setEndVector (inter.newRtau);
  }

  //4) recompute its saliency
  computeSymTransSalience(elm);

  //5) reinsert it into the GD list
  if (elm->graph_type()==SIElement::LINK &&
     elm->label() != SIElement::CONTACT)
    GDList.insert(
      Sal_SIElmPair(Sal_IDpair(elm->dPnCost(), elm->id()), elm));

}

void IShock::updateASIElementSimTime(SILink* elm, RADIUS_TYPE R)
{
  //1) First remove the shock element
  SIElmList.erase(A_R_IDpair(elm->isActive(),
    R_IDpair(elm->simTime(),elm->id()) )); //isActive

  //2) update the shock's simulation time and endTime
   elm->setSimTime (R);

  //3) then reinsert it into the appropriate place
  SIElmList.insert(SIElmPair(A_R_IDpair(elm->isActive(), 
    R_IDpair(elm->simTime(),elm->id()) ), elm));

  //3) Update current shocks's Drawing
   updateShockDrawing (elm);
}


//*****************************************************************
//*****************************************************************
// SHOCK COMPUTATION
//*****************************************************************
//*****************************************************************

//return the second count between init. and propagation.
bool IShock::DetectShocks (SHOCK_INIT_TYPE InitType, SHOCK_ALGO_TYPE AlgoType)
{
  bool bSuccess;

  if (InitType == DEFAULT_INIT)
    InitType = _ShockInitType;
  if (AlgoType == DEFAULT_ALGO)
    AlgoType = _ShockAlgoType;

  //first initialize the sources
  if (nSElement()==0 || !_bComputeShockLocally)
    InitializeShocks (InitType);
  else
    InitializeShocks (REGULAR_ADD_PATCH_INIT);

  //now propagate the shocks
  switch (AlgoType) {
  case LAGRANGIAN:
    bSuccess = PropagateShocks ();
    break;
  case DYN_VAL:
    //boundary()->clearAllIVS ();
    bSuccess = PropagateShocks ();
    //_ishockview->UpdateBoundary ();
    break;
  case DYN_VAL_NEW:
    bSuccess = PropagateShocks ();
    break;
  case PROPAGATION:
    break;
  }

  return bSuccess;
}

long IShock::PatchShocksLocally (SHOCK_INIT_TYPE InitType)
{
  if (InitType == DEFAULT_INIT)
    InitType = getShockInitTypeAdding(_ShockInitType);  

  long sec2 = clock();
  if (_bComputeShockLocally) {
    //if (MessageOption==MSG_VERBOSE)
    //  boundary()->DebugPrintTaintedBoundaryList();
    
    InitializeShocks (InitType);
    sec2 = clock();
    PropagateShocks ();
  }
  return sec2;
}

SIElement::SHOCKLABEL IShock::determineShockLabel (BElement* elm1, BElement* elm2)
{
  switch (elm1->type()) {
  case BPOINT:
    switch (elm2->type()) {
    case BPOINT:  return SIElement::DEGENERATE;
    case BLINE:   return SIElement::SEMI_DEGENERATE;
    case BARC:    return SIElement::DEGENERATE;
    }
  case BLINE:
    switch (elm2->type()) {
    case BPOINT:  return SIElement::SEMI_DEGENERATE;
    case BLINE:   return SIElement::REGULAR;
    case BARC:    return SIElement::SEMI_DEGENERATE;
    }
  case BARC:
    switch (elm2->type()) {
    case BPOINT:  return SIElement::DEGENERATE;
    case BLINE:   return SIElement::SEMI_DEGENERATE;
    case BARC:    return SIElement::DEGENERATE;
    }
  }
  return SIElement::BOGUS_SHOCK_LABEL;
}

//*******************************************************
//*******************************************************
//           PROPAGATION
//*******************************************************
//*******************************************************

//Go through the time sorted list of ACTIVE shock elements
//propagate each ACTIVE shock and move on to the NEXT ACTIVE one
//until all elements in the list are 'DEAD' or 'OUT OF THE SCREEN'
//'OUT OF THE SCREEN' means time > MAXRADIUS !

//return true if propagation is successful.
//else return false;
bool IShock::PropagateShocks ()
{
   PROPAGATION_TYPE ret=NO_PROPAGATION;

  //if (MessageOption==MSG_VERBOSE){
  //  vcl_cout<< "\n===== Start Propagating Shocks =====" <<vcl_endl;
  //}

   //MAIN LOOP OF PROPAGATION...
   while (ret!=PROPAGATION_DONE && 
       ret!=PROPAGATION_ERROR_DETECTED) { //INVALID_JUNCT_REMOVED
    ret = PropagateNextShock();
    //if (!ValidateShockList()) {
    //  vcl_cout<< "ValidateShockList() error! " <<vcl_endl;
    //}
  }

  if (ret==PROPAGATION_DONE)
    return true;
  else
    return false;
}

void IShock::PropagateShockJump ()
{
   PROPAGATION_TYPE ret;

   //JUMP PROPAGATION that KEEP PROPAGATING AT ALL CONTACT SHOCKS, A3 CORNERS, AND JUNCTIONS FORMATION
  do {
    ret = PropagateNextShock();
    //if (!ValidateShockList()) {
    //  vcl_cout<< "ValidateShockList() error!"<<vcl_endl;
    //}
  }
   while ((dCurrentTime ==0 && ret!=PROPAGATION_DONE) ||
       ret == NO_PROPAGATION ||
       ret == PROPAGATION_TO_INFINITY ||
       ret == INVALID_CANDIDATE_SOURCE ||
       ret == A3_FORMATION ||
       ret == NEW_SHOCK_FROM_A3 ||
       ret == NEW_BRANCHES_FROM_SOURCE ||
       ret == NEW_SHOCK_FROM_JUNCT ||
       //ret == LEFT_INTERSECTION ||
       //ret == RIGHT_INTERSECTION ||
       ret == BOTH_INTERSECTION);
}

PROPAGATION_TYPE IShock::PropagateNextShock ()
{
  //1)If there are no shocks to propagate, return
  if (nSElement()==0){
  //  if (MessageOption==MSG_VERBOSE)
  //    vcl_cout<< "No shocks exist to propagate!" <<vcl_endl;
    return PROPAGATION_DONE;
  }

  //The shock to be propagated at this time
  SIElement* currentShock=NULL;

  //get the first active shock
  SIElmListIterator FirstActiveElmPtr = 
        SIElmList.lower_bound(A_R_IDpair(true, R_IDpair(0,0)));
  if (FirstActiveElmPtr != SIElmList.end()) {
    //If there's a pre-determined shock to propagate...do it!
    if (_nextShockToPropagate) {
      currentShock = _nextShockToPropagate;
      _nextShockToPropagate = NULL;
    }
    else
      currentShock=FirstActiveElmPtr->second;
  }
  else
    return PROPAGATION_DONE;

  //5)only propagate those within maximum radius
   if (currentShock->simTime() > MAX_RADIUS){
      //vcl_cout<< "*** PROPAGATION_DONE ***" <<vcl_endl;
    DebugPrintOnePropagation (currentShock->id(), PROPAGATION_DONE);
      return PROPAGATION_DONE;
   }

  //6) Propagate from active shock nodes
  dCurrentTime = currentShock->startTime();
  if (currentShock->graph_type()==SIElement::NODE){
    switch (currentShock->type()){
    case SIElement::SOURCE:
      if (_ShockAlgoType==DYN_VAL_NEW){
        if (!validateCandidateSource((SISource*)currentShock)){
          deactivateASIElement(currentShock);
          ((SISource*)currentShock)->setbValid(false);
          DebugPrintOnePropagation (currentShock->id(), INVALID_CANDIDATE_SOURCE);
          
          //add to the shock update_list so that it can be deleted on screen
          update_list.insert(ID_SIElm_pair(currentShock->id(), currentShock));
          return INVALID_CANDIDATE_SOURCE;
        }
      }
      if (_ShockAlgoType==DYN_VAL){
        if (!validateCandidateSource((SISource*)currentShock)){
          deactivateASIElement(currentShock);
          DebugPrintOnePropagation (currentShock->id(), INVALID_CANDIDATE_SOURCE);
          return INVALID_CANDIDATE_SOURCE;
        }
      }
      //source is valid and can be propagated
      InitializeShockBranchesFromASource((SISource*)currentShock);
      currentShock->setPropagated (true);
      DebugPrintOnePropagation (currentShock->id(), NEW_BRANCHES_FROM_SOURCE);
      return NEW_BRANCHES_FROM_SOURCE;
    case SIElement::JUNCT:
    {
      int junct_id = currentShock->id();
      if (InitializeShockFromASIJunct((SIJunct*)currentShock)){
        currentShock->setPropagated (true);
        DebugPrintOnePropagation (currentShock->id(), NEW_SHOCK_FROM_JUNCT);
        return NEW_SHOCK_FROM_JUNCT;
      }
      else {
        DebugPrintOnePropagation (junct_id, INVALID_JUNCT_REMOVED);
        //!!! 030725 Set nextShockToProgagate to the next available shock!!
        //SIElmListIterator FirstActiveElmPtr = SIElmList.lower_bound(A_R_IDpair(true, R_IDpair(0,0)));
        //FirstActiveElmPtr++;
        //_nextShockToPropagate = FirstActiveElmPtr->second;
        return INVALID_JUNCT_REMOVED;
      }
    }
    case SIElement::A3SOURCE:
      InitializeShockFromAnA3Source((SIA3Source*)currentShock);
      currentShock->setPropagated (true);
      DebugPrintOnePropagation (currentShock->id(), NEW_SHOCK_FROM_A3);
      return NEW_SHOCK_FROM_A3;
    default: assert(0);
    }
  }

  //7) Only shockLinks should get this far!
   SILink* currentShockLink = (SILink*)currentShock;

   if (!currentShockLink->isPropagated()) {
      //8)If not propagated, look for intersections...
    //  For DynamicAdding, we should reset all active shocks' _bPropagated to true!!
      PROPAGATION_TYPE result = LookForIntersectionsWithNeighbors (currentShockLink);
    DebugPrintOnePropagation (currentShockLink->id(), result);
      return result;
   }
   else {
    //9)If the current shock has already been propagated, 
    //it is about to form a junciton
      JUNCTION_TYPE junction = currentShockLink->getJunctionType ();

    switch (junction) {
    case SINK_JUNCTION:
      InitializeASink (currentShockLink);
      DebugPrintOnePropagation (currentShockLink->id(), SINK_FORMATION);
      return SINK_FORMATION;

    case DEGENERATE_JUNCTION:
      InitializeAJunction (currentShockLink);
      DebugPrintOnePropagation (currentShockLink->id(), DEGENERATE_JUNCT);
         return DEGENERATE_JUNCT;

    case LEFT_REGULAR_JUNCTION:
    case RIGHT_REGULAR_JUNCTION:
      InitializeAJunction (currentShockLink);
      DebugPrintOnePropagation (currentShockLink->id(), REGULAR_JUNCT);
         return REGULAR_JUNCT;

    case BOGUS_JUNCTION_TYPE:
      vcl_cout<< vcl_endl<<"SHOCK PROPAGATION ERROR DETECTED!!" <<vcl_endl;
      DebugPrintOnePropagation (currentShockLink->id(), REGULAR_JUNCT);
      return PROPAGATION_ERROR_DETECTED;

    default:
      //10)No intersections with any other shock So move it to infinity
      //if (currentShockLink->label() != SIElement::CONTACT)
      //  vcl_cout<< vcl_endl<<"ERROR: SHOCK PROPAGATED BUT NO JUNCTION FORMED !!" <<vcl_endl;
      //assert (0);
      moveASILinkToInfinity (currentShockLink);
      DebugPrintOnePropagation (currentShockLink->id(), PROPAGATION_TO_INFINITY);
      return PROPAGATION_TO_INFINITY;
      }
   }
}

// compute shock branches for 2nd order shocks...           
// 3rd order shock don't have branches (vcl_sink)
void IShock::InitializeShockBranchesFromASource (SISource* source)
{
   BElement* belm1 = source->getBElement1();
   BElement* belm2 = source->getBElement2();

  switch (belm1->type()) {
  case BPOINT:
    switch (belm2->type()) {
    case BPOINT:  PointPointBranches (source, (BPoint*)belm1, (BPoint*)belm2); break;
    case BLINE:   PointLineBranches  (source, (BPoint*)belm1, (BLine*)belm2);  break;
    case BARC:    PointArcBranches   (source, (BPoint*)belm1, (BArc*)belm2);   break;
    } 
   break;
   case BLINE:
    switch (belm2->type()) {
    case BPOINT:  PointLineBranches  (source, (BPoint*)belm2, (BLine*)belm1);  break;
    case BLINE:   return;    //can't happen, (3rd order vcl_sink)
    case BARC:    LineArcBranches    (source, (BLine*)belm1,  (BArc*)belm2);   break;
    }
   break;
   case BARC:
      switch (belm2->type()) {
    case BPOINT:  PointArcBranches   (source, (BPoint*)belm2, (BArc*)belm1);   break;
    case BLINE:   LineArcBranches    (source, (BLine*)belm2,  (BArc*)belm1);   break;
    case BARC:    ArcArcBranches     (source, (BArc*)belm1,   (BArc*)belm2);   break;
    } 
  break;
   }

}

//Shock branches from a 2nd order shock caused by point-point
void IShock::PointPointBranches (SISource* source, BPoint* bp1, BPoint* bp2)
{
   SILink* newShock;

   VECTOR_TYPE u = _vPointPoint (bp1->pt(), bp2->pt());
   newShock = new SIPointPoint ( nextAvailableID(), source->startTime(),
                      bp1, bp2, source,u, u+M_PI);
  addASIElement (newShock);
  source->set_cSLink (newShock);

   //2nd branch
   newShock = new SIPointPoint ( nextAvailableID(), source->startTime(),
                      bp2, bp1, source, u+M_PI, u);
  addASIElement (newShock);
  source->set_cSLink2 (newShock);

   //De-Activate the 2nd order source
  deactivateASIElement(source);
}

//Shock branches from a 2nd order shock caused by point-line
void IShock::PointLineBranches (SISource* source, BPoint* point,
                                BLine* line)
{
   VECTOR_TYPE u = _vPointLine (point->pt(), line->start(), line->end());
  DIST_TYPE delta = _deltaPointLine (point->pt(), line->start(), line->end());

   SILink* newShock; 
   newShock = (SILink*) new SIPointLine (
      nextAvailableID(), source->startTime(),
      point, line, 
      source,
      u, delta);

  addASIElement (newShock);
  source->set_cSLink (newShock);

   //2nd branch
   newShock = (SILink*) new SIPointLine (
      nextAvailableID(), source->startTime(),
      line, point,
    source,
      delta, u);

  addASIElement (newShock);
  source->set_cSLink2 (newShock);
  
   //De-Activate the 2nd order source
  deactivateASIElement(source);
}

//Shock branches from a 2nd order shock caused by point-arc
void IShock::PointArcBranches (SISource* source, BPoint* bp1, BArc* ba2)
{
   SILink* newShock;

  //1)Compute lsVector and rsVector
  //Need to compute s, nu locally here for different cases
  TAU_TYPE lsvector, rsvector;
  VECTOR_TYPE u;
  DIST_TYPE H = _distPointPoint (bp1->pt(), ba2->center());
  DIST_TYPE R1 = 0;
  DIST_TYPE R2 = ba2->R();
   int nu, s;
   if (H>(R1+R2))  s =  1;
   else        s = -1;

  //First Branch, leftPoint, rightArc
  nu = 1;
  u = _vPointPoint (bp1->pt(), ba2->center());
  if (s>0) {
    lsvector = u;
    rsvector = u+M_PI;
  }
  else {
    if (nu>0) {
      lsvector = u+M_PI;
      rsvector = u+M_PI;
    }
    else {
      lsvector = u;
      rsvector = u;
    }
  }
  newShock = (SILink*) new SIPointArc (
        nextAvailableID(), source->startTime(),
      bp1, ba2, source, 
      lsvector, rsvector);
  addASIElement (newShock);
  source->set_cSLink (newShock);

   //2nd branch, leftArc, rightPoint
  nu = -1;
  u = _vPointPoint (ba2->center(), bp1->pt());
  if (s>0) {
    lsvector = u;
    rsvector = u+M_PI;
  }
  else {
    if (nu>0) {
      lsvector = u+M_PI;
      rsvector = u+M_PI;
    }
    else {
      lsvector = u;
      rsvector = u;
    }
  }
   newShock = (SILink*) new SIPointArc (
      nextAvailableID(), source->startTime(),
      ba2, bp1, source,
    lsvector, rsvector);
  
  addASIElement (newShock);
  source->set_cSLink2 (newShock);

   //De-Activate the 2nd order source
  deactivateASIElement(source);
}

void IShock::LineArcBranches (SISource* source, BLine* line, BArc* arc)
{
   VECTOR_TYPE u = _vPointLine (arc->center(), line->start(), line->end());
  DIST_TYPE delta = _deltaPointLine (arc->center(), line->start(), line->end());

   SILink* newShock;

  //First branch nu=1;
   newShock = (SILink*) new SILineArc (
      nextAvailableID(), source->startTime(),
      arc, line,
      source,
      u, delta);

  addASIElement (newShock);
  source->set_cSLink (newShock);

   //2nd branch, nu=-1
   newShock = (SILink*) new SILineArc (
      nextAvailableID(), source->startTime(),
      line, arc, 
      source,
      delta, u);

  addASIElement (newShock);
  source->set_cSLink2 (newShock);

   //De-Activate the 2nd order source
  deactivateASIElement(source);
}

void IShock::ArcArcBranches (SISource* source, BArc* ba1, BArc* ba2)
{
   SILink* newShock;

  //1)Make ba1 small arc and ba2 big arc
  if (ba1->R() > ba2->R()) {
    BArc* temp = ba1;  ba1 = ba2; ba2 = temp;
  }
  DIST_TYPE R1 = ba1->R();
  DIST_TYPE R2 = ba2->R();
  DIST_TYPE H = _distPointPoint (ba1->center(), ba2->center());
   VECTOR_TYPE u;
  TAU_TYPE lsvector, rsvector;
   int nu, s;
   if (H>(R1+R2))  s =  1;
   else        s = -1;

  //First Branch, leftPoint, rightArc
  nu = 1;
  u = _vPointPoint (ba1->center(), ba2->center());
  if (s>0) {
    lsvector = u;
    rsvector = u+M_PI;
  }
  else {
    lsvector = angle0To2Pi(u+M_PI);
    rsvector = angle0To2Pi(u+M_PI);
  }
  newShock = (SILink*) new SIArcArc (
        nextAvailableID(), source->startTime(),
      ba1, ba2, source, 
      lsvector, rsvector);

  addASIElement (newShock);
  source->set_cSLink (newShock);

   //2nd branch, leftArc, rightPoint
  nu = -1;
  u = _vPointPoint (ba2->center(), ba1->center());
  if (s>0) {
    lsvector = u;
    rsvector = angle0To2Pi(u+M_PI);
  }
  else {
    lsvector = u;
    rsvector = u;
  }
   newShock = (SILink*) new SIArcArc (
      nextAvailableID(), source->startTime(),
      ba2, ba1, source,
    lsvector, rsvector);
  addASIElement (newShock);
  source->set_cSLink2 (newShock);

   //De-Activate the 2nd order source
  deactivateASIElement(source);
}

// there is a difference here between local and global shock
// computation. For global we only have to look at active shocks
// for local shock detection, we might need to intersect with 
// a shock that has already propagated. This will mean that all its
// children will be invalid and have to be deleted. But we still have
// to consider possible intersections with them.

PROPAGATION_TYPE IShock::LookForIntersectionsWithNeighbors (SILink* current)
{
  SILink *l_neighbor=0, *r_neighbor=0;
   SILink* candidate;
   BElement* belm;
   IntrinsicIntersection intersection, leftI, rightI;
   TAU_TYPE diff;

   TAU_TYPE iTau=ISHOCK_DIST_HUGE;

   //###################### LEFT SIDE ###################### 
   belm = current->lBElement();   
 
  //go through the list of all shocks caused by the boundary element
  BElement::shockListIterator curS = belm->shockList.begin();
  for (;curS!=belm->shockList.end();++curS) {

    //ignore all the nodes
    if ((*curS)->graph_type()!=SIElement::LINK) continue;

      //go through all Active SILinks
    candidate = (SILink*)(*curS);
      if (candidate->isActive() || _bComputeShockLocally ){ 
      //CRITERIA FOR INTERSECTION (CHECK IT!!)
         if (current->lBElement() == candidate->rBElement()) {

            if (current->type() == SIElement::THIRDORDER){
               if (current->rBElement() == candidate->lBElement()) {
                  //1)THIRD ORDER SINK
                  //CRITERIA FOR formation of a THIRD ORDER
                  //VIA HEAD-ON COLLISION
                  diff = current->LsTau() - candidate->RsTau();
                  if (diff>=0 && LisL(diff, iTau)){
                     iTau = diff; 
                     l_neighbor = candidate;
                  }
               } 
            }
        else if (current->type() == SIElement::ARCTHIRDORDER ){
               if (current->rBElement() == candidate->lBElement()) {
            //2)ARC-THRIDORDER
            diff = current->LsTau() - candidate->RsTau();
                  if (diff>=0 && AisL(diff, iTau)){
                     iTau = diff; 
                     l_neighbor = candidate;
                  }
               } 
            }
            else {
          //3)CIRCLE CENTER SINK...irregular P-P shock
          if (current->type() == SIElement::POINTPOINT ){
             if (current->rBElement() == candidate->lBElement() &&
              current->pSNode() != candidate->pSNode()) {
               l_neighbor = candidate;
              leftI.R = current->startTime();
              leftI.LSLtau = candidate->LsTau();
              leftI.LSRtau = candidate->RsTau();
              leftI.RSLtau = current->LsTau();
              leftI.RSRtau = current->RsTau();
              continue;
            }
          }
          //4)ALL OTHER INTERSECTIONS
               intersection = ComputeIntersection (candidate, current);
               if (intersection.R == ISHOCK_DIST_HUGE) continue; //if no intersection keep going

               //Keep only the one with the minimum time of intersection
               if ( RisGEq (intersection.R, current->startTime()) && 
              RisLEq (intersection.R, current->endTime()) &&
                    RisGEq (intersection.R, candidate->startTime()) && 
              RisLEq (intersection.R, candidate->endTime()) ) {
            if (RisL (intersection.R, leftI.R)) {
              leftI = intersection;
              l_neighbor = candidate;
            }
               }
            }
      }//end CRITERIA FOR INTERSECTION
      }
   } //end for loop shockList

   iTau = ISHOCK_DIST_HUGE;

   //###################### RIGHT SIDE ###################### 
   belm = current->rBElement();

  //go through the list of all shocks caused by the boundary element
  curS = belm->shockList.begin();
  for(;curS!=belm->shockList.end();++curS){

    //ignore all the nodes
    if ((*curS)->graph_type()!=SIElement::LINK) continue;

      //go through all Active SILinks
    candidate = (SILink*)(*curS);
    if (candidate->isActive() || _bComputeShockLocally){ 

      //CRITERIA FOR INTERSECTION (CHECK IT!!)
         if (current->rBElement() == candidate->lBElement()){

            if (current->type() == SIElement::THIRDORDER){
               if (current->lBElement() == candidate->rBElement()) {
                  //1)THIRD ORDER SINK
                  diff =  candidate->LsTau() - current->RsTau();
                  if (diff >=0 && LisL(diff, iTau)){
                     iTau = diff; 
                     r_neighbor = candidate;
                  }
               }
            }
         else if (current->type() == SIElement::ARCTHIRDORDER ){
               if (current->lBElement() == candidate->rBElement()) {
            //2)ARC-THRIDORDER
            diff = candidate->LsTau() - current->RsTau();
                  if (diff>=0 && AisL(diff, iTau)){
                     iTau = diff; 
                     r_neighbor = candidate;
                  }
               }
            }
            else {
          //3)CIRCLE CENTER SINK...
          if (current->type() == SIElement::POINTPOINT ){
             if (current->lBElement() == candidate->rBElement() &&
              current->pSNode() != candidate->pSNode()) {
               r_neighbor = candidate;
              rightI.R = current->startTime();
              rightI.LSLtau = current->LsTau();
              rightI.LSRtau = current->RsTau();
              rightI.RSLtau = candidate->LsTau();
              rightI.RSRtau = candidate->RsTau();
              continue;
            }
          }
          //4)ALL OTHER INTERSECTIONS
               intersection  = ComputeIntersection(current, candidate);

               if (intersection.R == ISHOCK_DIST_HUGE) continue;

               //keep only the one with the minimum time of intersection
               if ( RisGEq (intersection.R, current->startTime()) && 
              RisLEq (intersection.R, current->endTime()) &&
                    RisGEq (intersection.R, candidate->startTime()) && 
              RisLEq (intersection.R, candidate->endTime()) ) {
            if (RisL (intersection.R, rightI.R)) {
              rightI = intersection;
              r_neighbor = candidate;
            }
               }
            }
         }//end CRITERIA FOR INTERSECTION
      }
   } //end for loop for shockList

   //========= RESULTS and UpdateShockwaveTo() =========
   current->setPropagated (true);

   if ((l_neighbor && r_neighbor) && RisEq(leftI.R, rightI.R)) {

      if (current->type() == SIElement::THIRDORDER){

      //June 28, 2002
      //instead of deleting the second branch of the third order
      //we have decided to keep both branches to half their lengths and 
      //introduce a vcl_sink in the middle
      SIThirdOrder* curTO = (SIThirdOrder *)current;
      SIThirdOrder* canTO = (SIThirdOrder *)l_neighbor;
      assert (l_neighbor == r_neighbor);

         //ALL SIThirdOrder properties are updated here
         curTO->setLeTau ( (curTO->LsTau() + canTO->RsTau())/2 );
         curTO->setReTau ( (curTO->RsTau() + canTO->LsTau())/2 );

      canTO->setLeTau ( curTO->ReTau() );
         canTO->setReTau ( curTO->LeTau() );

      updateShockDrawing (current);    //Update current's Drawing
      updateShockDrawing (l_neighbor);  //Update l_neighbor's Drawing

      current->set_rNeighbor (l_neighbor);
      l_neighbor->set_rNeighbor (current);

      current->set_lNeighbor (l_neighbor);
      l_neighbor->set_lNeighbor (current);

      l_neighbor->setPropagated (true);
      InitializeASink (current);
         return THIRD_ORDER_FORMATION;
      }
    else if (current->type() == SIElement::ARCTHIRDORDER) {
         //ALL SIArcThirdOrder properties are updated here
         ((SIArcThirdOrder *)current)->setLeTau (
        ( ((SIArcThirdOrder *)current)->LsTau() + 
          ((SIArcThirdOrder *)l_neighbor)->RsTau() )/2);
         ((SIArcThirdOrder *)current)->setReTau (
        ( ((SIArcThirdOrder *)current)->RsTau() + 
          ((SIArcThirdOrder *)l_neighbor)->LsTau() )/2);

      ((SIArcThirdOrder *)l_neighbor)->setLeTau ( ((SIArcThirdOrder *)current)->ReTau() );
         ((SIArcThirdOrder *)l_neighbor)->setReTau ( ((SIArcThirdOrder *)current)->LeTau() );

      updateShockDrawing (current);    //Update current's Drawing
      updateShockDrawing (l_neighbor);  //Update l_neighbor's Drawing

      current->set_rNeighbor (l_neighbor);
      l_neighbor->set_rNeighbor (current);

      current->set_lNeighbor (l_neighbor);
      l_neighbor->set_lNeighbor (current);

      l_neighbor->setPropagated (true);
      InitializeASink(current);
         return ARC_THIRD_ORDER_FORMATION;
    }
      else {
      moveSILinksToALeftJunction (l_neighbor, current, leftI);
      moveSILinksToARightJunction (current, r_neighbor, rightI);
         return BOTH_INTERSECTION;
      }
   }
   else if (l_neighbor && RisL(leftI.R, rightI.R)) {
      moveSILinksToALeftJunction (l_neighbor, current, leftI);
      return LEFT_INTERSECTION;
   }
   else if (r_neighbor && RisL(rightI.R, leftI.R)) {
      moveSILinksToARightJunction (current, r_neighbor, rightI);
      return RIGHT_INTERSECTION;
   }
   else {
    //special for contacts collinear ones
    //sometimes they can have neighbors so they don't have to go to 
    //infinity
    if (current->lNeighbor()){
      ////////NOT TRUE assert(current->label()==SIElement::CONTACT);
      updateASIElementSimTime(current, current->lNeighbor()->endTime());
      return LEFT_INTERSECTION;
    }
    else if (current->rNeighbor()){
      ////////NOT TRUE assert(current->label()==SIElement::CONTACT);
      updateASIElementSimTime(current, current->rNeighbor()->endTime());
      return RIGHT_INTERSECTION;
    }
    else {
      moveASILinkToInfinity (current);
      return PROPAGATION_TO_INFINITY;
    }
   }
}

void IShock::moveSILinksToALeftJunction (SILink* l_neighbor, SILink* current, 
                      IntrinsicIntersection intersection)
{
  //if we ever get to this function, it means that at the current time
  //it's guarrenteed that the current and left neighbor are gonna intersect
  //SPECIAL CASES:: if either shock is a collinear contact
  //intersect with its collinear neighbor first
  if (current->label() == SIElement::CONTACT){
    SIContact* cContact = (SIContact*)current;
    //Only update when there is no SO Source between the 2 ColinearContact
    //(cContact->simTime()==ISHOCK_DIST_HUGE && cContact->Neighbor()->simTime()==ISHOCK_DIST_HUGE) ||
    //    (!RisEq(lContact->Neighbor()->simTime(), current->simTime())) ) {
    if (cContact->ColinearContactType() == LEFT_COLINEAR_CONTACT) {
      if (cContact->Neighbor()->lNeighbor() && 
         cContact->Neighbor()->lNeighbor()->pSNode() &&
         cContact->rNeighbor() &&
         cContact->rNeighbor()->pSNode() &&
         cContact->Neighbor()->lNeighbor()->pSNode() == cContact->rNeighbor()->pSNode())
         ;
      else {
        IntrinsicIntersection inter;
        inter.R = intersection.R;
        inter.LSLtau = cContact->LsTau();
        inter.LSRtau = cContact->RsTau();
        inter.RSLtau = cContact->Neighbor()->LsTau();
        inter.RSRtau = cContact->Neighbor()->RsTau();
        moveSILinksToARightJunction(cContact, cContact->Neighbor(), inter);
      }
    }
  }
  if (l_neighbor->label() == SIElement::CONTACT){
    SIContact* lContact = (SIContact*)l_neighbor;
    if (lContact->ColinearContactType() == RIGHT_COLINEAR_CONTACT)
      if (lContact->Neighbor()->rNeighbor() &&
         lContact->Neighbor()->rNeighbor()->pSNode() &&
         lContact->lNeighbor() &&
         lContact->lNeighbor()->pSNode() &&
         lContact->Neighbor()->rNeighbor()->pSNode() == lContact->lNeighbor()->pSNode())
         ;
      else {
        IntrinsicIntersection inter;
        inter.R = intersection.R;
        inter.LSLtau = lContact->Neighbor()->LsTau();
        inter.LSRtau = lContact->Neighbor()->RsTau();
        inter.RSLtau = lContact->LsTau();
        inter.RSRtau = lContact->RsTau();
        moveSILinksToALeftJunction(lContact->Neighbor(), lContact, inter);
      }
  }

  //1)If it's NOT the first time of intersecting with LeftNeighbor, return.
  //  The current shock will be updated by it's neighbor.
  if (current->lNeighbor()){
    if (current->lNeighbor()==l_neighbor){
      if (RisGEq(intersection.R, l_neighbor->endTime())){
        moveASILinkToAJunction (intersection, current, RIGHT);
        return;
      }
    }
    else {//else happens in 4Junct.bnd
      //Not true for init shocks from source in the beginning.
      //assert(current->label()==SIElement::CONTACT ||
      //      current->lNeighbor()->label()==SIElement::CONTACT);
      current->lNeighbor()->clear_rNeighbor();
      current->clear_lNeighbor();
    }
  }

  //2)If intersection time is earlier than current's endTime and
  //  If there is a VALID neighbor on the right side, delete ties with
  //  the r_neighbor, because its now going to have a new neighbor 
  //  at some other propagation.
  // also allows for a degnerate junction
  if (current->rNeighbor()) {
    if (current->rNeighbor()->label() == SIElement::CONTACT){
      SIContact* rcontact = (SIContact*)current->rNeighbor();
      if (rcontact->ColinearContactType() == RIGHT_COLINEAR_CONTACT){
        //move the neighboring contact down if needed
        if (RisL(intersection.R, rcontact->endTime())){
          //do nothign for now
        }
      }
      else {
        if (RisL(intersection.R, current->rNeighbor()->endTime())){
          current->rNeighbor()->clear_lNeighbor();
          current->clear_rNeighbor();
        }
      }
    }
    else {
      if (RisL(intersection.R, current->rNeighbor()->endTime())){
        current->rNeighbor()->clear_lNeighbor();
        current->clear_rNeighbor();
      }
    }
  }

  // 2.5) If this is local propagation we need to 
  //      kill all the neighbor's children too
  if (_bComputeShockLocally && l_neighbor->cSNode())
    delASIElement(l_neighbor->cSNode());

  //3)Update current's intrinsic parameters to this intersection
  current->set_lNeighbor (l_neighbor);
  moveASILinkToAJunction (intersection, current, RIGHT);

  //4)Update neighbor's right neighbor
  //the left neighbor can't have any other neighbor than the current
  if (l_neighbor->rNeighbor()) { 
    if (l_neighbor->rNeighbor() != current){
      l_neighbor->rNeighbor()->clear_lNeighbor();
      l_neighbor->clear_rNeighbor();
    }
  }

  //5)Update l_neighbor's intrinsic parameters to this intersection
  //  Imporvcl_tant: We can pull neighbor's endTime there,
  //             but we CANNOT pull neighbor's simTime there!
  //  Special case: move neighboring contact shock when A3 shock is forming
  //!!!!!Eq for special case of zero-length, see 030318-LineLineSpecial.bnd

  l_neighbor->set_rNeighbor (current);
  if ( RisLEq(intersection.R, l_neighbor->simTime()) ||
      intersection.R==0.0 )
    moveASILinkToAJunction (intersection, l_neighbor, LEFT);
  else {
    l_neighbor->setEndTime(intersection.R);
    updateShockDrawing(l_neighbor);
  }

  //6)Update neighbor's left neighbor :: allow for degenerate junctions
  if (l_neighbor->lNeighbor()) {
    if (RisL(intersection.R, l_neighbor->lNeighbor()->endTime())) { 
      l_neighbor->lNeighbor()->clear_rNeighbor();
      l_neighbor->clear_lNeighbor();
    }
  }
}

void IShock::moveSILinksToARightJunction (SILink* current, SILink* r_neighbor, 
                       IntrinsicIntersection intersection)
{
  if (current->label() == SIElement::CONTACT) {
    SIContact* cContact = (SIContact*)current;
    if (cContact->ColinearContactType() == RIGHT_COLINEAR_CONTACT) {
      if (cContact->Neighbor()->rNeighbor() &&
         cContact->Neighbor()->rNeighbor()->pSNode() &&
         cContact->lNeighbor() &&
         cContact->lNeighbor()->pSNode() &&
         cContact->Neighbor()->rNeighbor()->pSNode() == cContact->lNeighbor()->pSNode())
        ;
      else {
        IntrinsicIntersection inter;
        inter.R = intersection.R;
        inter.LSLtau = cContact->Neighbor()->LsTau();
        inter.LSRtau = cContact->Neighbor()->RsTau();
        inter.RSLtau = cContact->LsTau();
        inter.RSRtau = cContact->RsTau();
        moveSILinksToALeftJunction(cContact->Neighbor(), cContact, inter);
      }
    }
  }
  if (r_neighbor->label() == SIElement::CONTACT){
    SIContact* rContact = (SIContact*)r_neighbor;
    if (rContact->ColinearContactType() == LEFT_COLINEAR_CONTACT) {
      if (rContact->Neighbor()->lNeighbor() &&
         rContact->Neighbor()->lNeighbor()->pSNode() &&
         rContact->rNeighbor() &&
         rContact->rNeighbor()->pSNode() &&
         rContact->Neighbor()->lNeighbor()->pSNode() == rContact->rNeighbor()->pSNode())
        ;
      else {
        IntrinsicIntersection inter;
        inter.R = intersection.R;
        inter.LSLtau = rContact->LsTau();
        inter.LSRtau = rContact->RsTau();
        inter.RSLtau = rContact->Neighbor()->LsTau();
        inter.RSRtau = rContact->Neighbor()->RsTau();
        moveSILinksToARightJunction(rContact, rContact->Neighbor(), inter);
      }
    }
  }

  //1)If it's NOT the first time of intersecting with RightNeighbor, return.
  //  The current shock will be updated by it's neighbor.
  if (current->rNeighbor()) {
    //chnage this so that it will only move if it is not there yet
    //Instead of return, we have to move the current shock
    if (current->rNeighbor()==r_neighbor){
      if (RisGEq(intersection.R, r_neighbor->endTime())){
        moveASILinkToAJunction (intersection, current, LEFT);
        return;
      }
    }
    else {
      //Not true for init shocks from source in the beginning.
      //assert(current->label()==SIElement::CONTACT ||
      //      current->rNeighbor()->label()==SIElement::CONTACT);
      current->rNeighbor()->clear_lNeighbor();
      current->clear_rNeighbor();
    }
  }

  //2)If intersection time is earlier than current's endTime and
  //  If there is a VALID neighbor on the left side, delete ties with
  //  the l_neighbor, because its now going to have a new neighbor 
  //  at some other propagation.

  if (current->lNeighbor()){
    if (RisL(intersection.R, current->lNeighbor()->endTime())){
      current->lNeighbor()->clear_rNeighbor();
      current->clear_lNeighbor();
    }
  }

  // 2.5) If this is local propagation we need to 
  //      kill all the neighbor's children too
  if (_bComputeShockLocally && r_neighbor->cSNode())
    delASIElement(r_neighbor->cSNode());

  //3)Update current's intrinsic parameters to this intersection
  current->set_rNeighbor (r_neighbor);
  moveASILinkToAJunction (intersection, current, LEFT);

  //4)Update neighbor's left neighbor
  if (r_neighbor->lNeighbor()) { 
    if (r_neighbor->lNeighbor() != current){
      r_neighbor->lNeighbor()->clear_rNeighbor();
      r_neighbor->clear_lNeighbor();
    }
  }

  //5)Update r_neighbor's intrinsic parameters to this intersection
  //  Imporvcl_tant: We can pull neighbor's endTime there,
  //             but we CANNOT pull neighbor's simTime there!
  //  Special case: move neighboring contact shock when A3 shock is forming
  //is the contact case really necessary???
  //!!!!!Eq for special case of zero-length see 030318-LineLineSpecial.bnd

  r_neighbor->set_lNeighbor (current);
  if ( RisLEq (intersection.R, r_neighbor->simTime()) || 
      intersection.R==0.0 )
    moveASILinkToAJunction (intersection, r_neighbor, RIGHT);
  else {
    r_neighbor->setEndTime(intersection.R);//just move the end time
    updateShockDrawing(r_neighbor);
  }

  //6)Update neighbor's right neighbor
  if (r_neighbor->rNeighbor()) {
    if ( RisL(intersection.R, r_neighbor->rNeighbor()->endTime()) ) { 
      r_neighbor->rNeighbor()->clear_lNeighbor();
      r_neighbor->clear_rNeighbor();
    }
  }
}

void IShock::InitializeAJunction(SILink* current)
{
   SINode* newJunction;

  //If all the shocks involved are CONTACTS then it
  //will form an A3Source instead
  bool bA3=true;

   //1)Get lmostshock and rmostshock of the junction
   SILink *rshock = current;
   while (rshock->rNeighbor() != NULL){
    if (rshock->label() != SIElement::CONTACT) bA3=false;
      rshock = rshock->rNeighbor();
  }
   //rshock is now the RIGHTMOST shock in the junction

   SILink *lshock = current;
   while (lshock->lNeighbor() != NULL){
    if (lshock->label() != SIElement::CONTACT) bA3=false;
      lshock = lshock->lNeighbor();
  }
   //lshock is now the LEFTMOST shock in the junction

  //2)Make the BElementList and the PSElementList for this Junction
  //  By travervcl_sing from the leftmost to the rightmost shock
  BElementList belist;
  SILinksList pselist;
    
  SILink* cur = lshock;
   while (cur) {
    belist.push_back(cur->lBElement());
    pselist.push_back(cur);

    //Ming: degenerate cases!
    //if one of the intersected shock is not propagated,
    //we should propagate it next time!
    //The junction will form later!
    if (!cur->isPropagated()) {
      _nextShockToPropagate = cur;
      ///vcl_cout<< "_nextShockToPropagate id= "<<_nextShockToPropagate->id()<<vcl_endl;
      return;
    }

    deactivateASIElement(cur);
      cur = cur->rNeighbor();
   }
  //rightmost element
  belist.push_back(rshock->rBElement());

   //2)Initialize either a new Junction or a new A3Source
   if (lshock->label() == SIElement::CONTACT && 
     rshock->label() == SIElement::CONTACT && bA3) {
    //2-2)ColinearContact never forms a A3!!
    //For corner, use current->startTime(), current->getStartPt(), 
    newJunction = new SIA3Source (nextAvailableID(), 
      current->endTime(), current->getEndPt(),
      belist, pselist);
  }
   else {
    //IF THIS IS A DEGENERATE JUNCTION, WE NEED TO AVERAGE THE 
    //INTRISIC PARAMETERS SO THAT THEY ARE ALL CONSISTENT

      newJunction = new SIJunct (nextAvailableID(), 
      current->endTime(), current->getEndPt(),
         belist, pselist);
  }

  //add this junction on to the shock list
   addASIElement (newJunction);
} 

void IShock::InitializeShockFromAnA3Source(SIA3Source* current)
{
   //Determine the leftmost and the rightmost boundary elementsof the junction.
  //because of the way it was added to the list
   BElement* lbelm = current->bndList.front();
   BElement* rbelm = current->bndList.back();
  //Also need to find the leftmost and the rightmost parent
  SILink* lshock = current->PSElementList.front();
  SILink* rshock = current->PSElementList.back();
  
  SILink* newShock = InitializeShockFromAJunction(
    current, lbelm, rbelm, lshock, rshock);

  addASIElement(newShock);
  current->set_cSLink (newShock);
  deactivateASIElement(current);
}

bool IShock::InitializeShockFromASIJunct(SIJunct* current)
{
   //Determine the leftmost and the rightmost boundary elementsof the junction.
  //because of the way it was added to the list
   BElement* lbelm = current->bndList.front();
   BElement* rbelm = current->bndList.back();
  assert (lbelm != rbelm);

  //Also need to find the leftmost and the rightmost parent
  SILink* lshock = current->PSElementList.front();
  SILink* rshock = current->PSElementList.back();
  assert (lshock != rshock);

  SILink* newShock = InitializeShockFromAJunction(
    current, lbelm, rbelm, lshock, rshock);

  //EPSILONISSUE 5
  //!!!!!Dynamic Error Recovery during Shock Propagation!!!!!
  if (!newShock->isValid()){
    //1)this shock should not have formed!!!
    vcl_cout <<"Numerical Recovery: Junction Removed! Jid: "<< current->id() << vcl_endl;
    //it is not yet in the shock list so we can just delete it
    delete newShock;

    //2)!!! 030725 Set nextShockToProgagate to the next available shock!!
    SIElmListIterator FirstActiveElmPtr = SIElmList.lower_bound(A_R_IDpair(true, R_IDpair(0,0)));
    FirstActiveElmPtr++;
    _nextShockToPropagate = FirstActiveElmPtr->second;

    //3)The junction is invalid as well. Reactivate the SILinks...
    //  so the correct intersection will happen later...
    delASIElement(current);

    //NOW WHAT ???
    //well, the first order of buvcl_siness is to consider 
    //a misvcl_sing source between the elements
    //initializeASource (lbelm, rbelm, REGULAR_ADD_PATCH_INIT);
    return false; //bad junction, junction deleted
  }
  else {
    addASIElement(newShock);
    current->set_cSLink (newShock);
    deactivateASIElement(current);  
  }
  //successful initialization of a new shockfrom this junciton
  return true;
}

SILink* IShock::InitializeShockFromAJunction(SINode* current,
  BElement* lbelm, BElement* rbelm, SILink* lshock, SILink* rshock)
{
   SILink* newShock;
  RADIUS_TYPE startTime = current->startTime();

   switch (lbelm->type()) {
   case BPOINT:
    switch (rbelm->type()) {
    case BPOINT:
      newShock = new SIPointPoint (
                  nextAvailableID(), startTime,
                  lbelm, rbelm, current,
                  lshock->EndVector(), rshock->EndVector());
    break;
    case BLINE:
      newShock =  new SIPointLine (
                  nextAvailableID(), startTime,
                  lbelm, rbelm, current,
                  lshock->EndVector(), rshock->EndVector());
    break;
    case BARC:
      if (_BisEqPoint (((BPoint*)lbelm)->pt(), ((BArc*)rbelm)->center()))
        newShock =  new SIArcThirdOrder (
              nextAvailableID(), startTime,
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
      else
        newShock =  new SIPointArc (
                    nextAvailableID(), startTime,
                    lbelm, rbelm, current,
                    lshock->EndVector(), rshock->EndVector());
    break;
    }
  break;
  case BLINE:
    switch (rbelm->type()) {
    case BPOINT:
      newShock =  new SIPointLine (
                  nextAvailableID(), startTime,
                  lbelm, rbelm, current, 
                  lshock->EndVector(), rshock->EndVector());
    break;
    case BLINE: {
      BLine* lbline = (BLine*) lbelm;
      BLine* rbline = (BLine*) rbelm;
      //EPSILONISSUE 2
      //DIFFERENCE BETWEEN THIRDORDER AND LINELINE: MAKE DIFF SMALL!
      if (_isEq(lbline->N(),rbline->N()+M_PI, TO_EPSILON) ||
         _isEq(lbline->N()+M_PI,rbline->N(), TO_EPSILON)) {
        //ThirdOrder
        newShock = new SIThirdOrder (
              nextAvailableID(), startTime,
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
      }
      else {
        newShock = new SILineLine (
              nextAvailableID(), startTime,
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
      }
      break; }
    case BARC:
      newShock = new SILineArc (
                    nextAvailableID(), startTime,
                    lbelm, rbelm, current,
                    lshock->EndVector(), rshock->EndVector());
    break;
    }
  break;
  case BARC:
    switch (rbelm->type()) {
    case BPOINT:
      if (_BisEqPoint (((BArc*)lbelm)->center(), ((BPoint*)rbelm)->pt()))
        newShock = new SIArcThirdOrder (
              nextAvailableID(), startTime, 
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
      else
        newShock = new SIPointArc (
                    nextAvailableID(), startTime,
                    lbelm, rbelm, current,
                    lshock->EndVector(), rshock->EndVector());
    break; 
    case BLINE:
      newShock = new SILineArc (
                    nextAvailableID(), startTime,
                    lbelm, rbelm, current,
                    lshock->EndVector(), rshock->EndVector());
    break;
    case BARC:
      if (_BisEqPoint (((BArc*)lbelm)->center(), ((BArc*)rbelm)->center()))
        newShock = new SIArcThirdOrder (
              nextAvailableID(), startTime,
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
      else
        newShock = new SIArcArc (
              nextAvailableID(), startTime,
              lbelm, rbelm, current,
              lshock->EndVector(), rshock->EndVector());
    break;
    }
  break;
  } //end switch
  return newShock;
}

void IShock::InitializeASink(SILink* current)
{
  //Make the BElementList and the PSElementList for this Sink
  //by travervcl_sing around the junction
  BElementList belist;
  SILinksList pselist;

  RADIUS_TYPE sumTime=0;
  COORD_TYPE sumCoordX=0, sumCoordY=0;
  int   n=0;

  SILink* cur = current;
   while (cur != current->lNeighbor()) {
    belist.push_back(cur->lBElement());
    pselist.push_back(cur);
    deactivateASIElement(cur);

    sumTime += cur->endTime();
    Point endpt = cur->getEndPt();
    sumCoordX += endpt.x;
    sumCoordY += endpt.y;
    n++;

      cur = cur->rNeighbor();
   }
  //rightmost shock element
  belist.push_back(cur->lBElement());
  pselist.push_back(cur);

   //Create a new Sink
  //1)Uvcl_sing current's endTime and EndPt
   //SISink* newSink = new SISink (nextAvailableID(), 
  //  current->endTime(), current->d_endTime(), current->getEndPt(),
  //  belist, pselist);

  //2)Uvcl_sing average of all shock's endTime and EndPt;
  sumTime /= n;
  sumCoordX /= n;
  sumCoordY /= n;
   SISink* newSink = new SISink (nextAvailableID(), 
    sumTime, Point(sumCoordX, sumCoordY),
    belist, pselist);
   
  //add it to the shock list
   addASIElement (newSink);
  deactivateASIElement(cur);
}

void IShock::updateShockDrawing (SIElement* sielm)
{
  //Update current shock's Drawing
  update_list.insert(ID_SIElm_pair(sielm->id(), sielm));
}

void IShock::UpdateShocks()
{
  //go through the update_list and draw each element
  vcl_map<int, SIElement*>::iterator elmPtr = update_list.begin();
  for (; elmPtr != update_list.end(); elmPtr++){
    SIElement* curShock = elmPtr->second;
    curShock->compute_extrinsic_locus();
  }

  //clear list after it is drawn
  update_list.clear();
}

//////////////////////////////////////////////////////////////

void IShock::setIOLabel (int srclabel, int newlabel)
{
  //1)Traverse the whole shock list and assign label...
  SIElmListIterator curSPtr = SIElmList.begin();
  for (; curSPtr != SIElmList.end(); curSPtr++) {
    SIElement* current = (SIElement*)(curSPtr->second);
    if (current->IOLabel == srclabel) 
      current->IOLabel = newlabel;
   }
}

void IShock::setIO (int srclabel, int inout)
{
  //1)Traverse the whole shock list and assign label...
  SIElmListIterator curSPtr = SIElmList.begin();
  for (; curSPtr != SIElmList.end(); curSPtr++) {
    SIElement* current = (SIElement*)(curSPtr->second);
    if (current->IOLabel == srclabel) 
      current->bIO = (inout==1);
   }
}

extern bool CMDLINE;

//Mark the flag SIElement::bIO= 1:Inside, 0:Outside
void IShock::MarkInsideOutside ()
{
  SIElement* current;
  SILinksList::iterator curS;

  //1)Initialize and assign label...

  SIElmListIterator curSPtr = SIElmList.begin();
  for (; curSPtr != SIElmList.end(); curSPtr++) {
    current = (SIElement*)(curSPtr->second);
    current->IOLabel = current->id();
    current->bIOVisited = false;
    current->bIO = INSIDE;
   }

  //2)Connect Components to Mark Inside/Outside
  SISink* vcl_sink;
  SIJunct* junct;

  curSPtr = SIElmList.begin();
  for (; curSPtr != SIElmList.end(); curSPtr++) {
    current = (SIElement*)(curSPtr->second);
    if (current->bIOVisited==false) {
      //visit the shock and child/parent shocks
      if (current->graph_type()==SIElement::NODE) {
        switch (current->type()) {
        case SIElement::A3SOURCE:
          groupIOLabel ( ((SIA3Source*)current)->cSLink(), current);
          break;
        case SIElement::SOURCE:
          groupIOLabel ( ((SISource*)current)->cSLink(), current);
          groupIOLabel ( ((SISource*)current)->cSLink2(), current);
          break;
        case SIElement::SINK:
          vcl_sink = (SISink*)current;
          curS = vcl_sink->PSElementList.begin();
          for(; curS!=vcl_sink->PSElementList.end(); ++curS)
            groupIOLabel ((*curS), current);
          break;
        case SIElement::JUNCT:
          groupIOLabel ( ((SIJunct*)current)->cSLink(), current);
          junct = (SIJunct*)current;
          curS = junct->PSElementList.begin();
          for(; curS!=junct->PSElementList.end(); ++curS)
            groupIOLabel ((*curS), current);
          break;
        }
      }
      else if (current->graph_type()==SIElement::LINK) {
        //BoundaryLimitHack
        //label the  shocks that are caused by the eight permanent boundary elements
        //their IDs are 1-8
        if (boundary()->GetBoundaryLimit() == BIG_RECTANGLE || 
           boundary()->GetBoundaryLimit() == BIG_CIRCLE)
        {
          if ( ((SILink*)current)->lBElement()->id() <=8 ||
              ((SILink*)current)->rBElement()->id() <=8 )
            setIO (current->IOLabel, OUTSIDE);
        }
        else {
          if ( ((SILink*)current)->cSNode()==NULL)
            setIO (current->IOLabel, OUTSIDE);
        }

        if (current->label() != SIElement::CONTACT)
          groupIOLabel ( ((SILink*)current)->pSNode(), current);

        if (((SILink*)current)->cSNode())
          groupIOLabel ( ((SILink*)current)->cSNode(), current);
      }
    }
   }
}

void IShock::groupIOLabel (SIElement* dest, SIElement* cur)
{
  cur->bIOVisited=true;

  if (dest->IOLabel > cur->IOLabel) 
    setIOLabel (dest->IOLabel, cur->IOLabel);
  else 
    setIOLabel (cur->IOLabel, dest->IOLabel);
}


void IShock::write_shock_file (const char* filename)
{

}

void IShock::DebugPrintOnePropagation (int id, PROPAGATION_TYPE action)
{/*
  if (MessageOption==MSG_NONE || MessageOption==MSG_TERSE)
    return;

  vcl_cout<< "---> Shock "<< id;
  switch (action) {
  case BOGUS_PROPAGATION_TYPE: assert (0); break;
  case NO_PROPAGATION: vcl_cout<< " NO_PROPAGATION"; break;
  case PROPAGATION_DONE: vcl_cout<< " PROPAGATION_DONE"; break;
  case PROPAGATION_TO_INFINITY: vcl_cout<< " PROPAGATION_TO_INFINITY"; break;
  case INVALID_CANDIDATE_SOURCE: vcl_cout<< " INVALID_CANDIDATE_SOURCE"; break;
  case A3_FORMATION: vcl_cout<< " A3_FORMATION"; break;
  case REGULAR_JUNCT: vcl_cout<< " REGULAR_JUNCT"; break;
  case DEGENERATE_JUNCT: vcl_cout<< " DEGENERATE_JUNCT"; break;
  case SINK_FORMATION: vcl_cout<< " SINK_FORMATION"; break;
  case NEW_SHOCK_FROM_A3: vcl_cout<< " NEW_SHOCK_FROM_A3"; break;
  case NEW_BRANCHES_FROM_SOURCE: vcl_cout<< " NEW_BRANCHES_FROM_SOURCE"; break;
  case NEW_SHOCK_FROM_JUNCT: vcl_cout<< " NEW_SHOCK_FROM_JUNCT"; break;
  case INVALID_JUNCT_REMOVED: vcl_cout<< "INVALID JUNCTION, JUNCTION REMOVED"; break;
  case LEFT_INTERSECTION: vcl_cout<< " LEFT_INTERSECTION"; break;
  case RIGHT_INTERSECTION: vcl_cout<< " RIGHT_INTERSECTION"; break;
  case BOTH_INTERSECTION: vcl_cout<< " BOTH_INTERSECTION"; break;
  case THIRD_ORDER_FORMATION: vcl_cout<< " THIRD_ORDER_FORMATION"; break;
  case ARC_THIRD_ORDER_FORMATION: vcl_cout<< " ARC_THIRD_ORDER_FORMATION"; break;  
  }
  vcl_cout<< vcl_endl;

  if (MessageOption==MSG_VERBOSE)
    DebugPrintShockList (true);
*/
}

void IShock::DebugPrintShockList (bool bPrintAll)
{
  if (bPrintAll!=true)
    return;

  vcl_cout<< "ShockList: " <<vcl_endl;
  SIElmListIterator elmPtr = SIElmList.begin();
  for (; elmPtr != SIElmList.end(); elmPtr++) {
    SIElement* current = elmPtr->second;

    switch (current->type()) {
    case SIElement::A3SOURCE:      vcl_cout<< "A3Source"; break;
    case SIElement::SOURCE:        vcl_cout<< "Source"; break;
    case SIElement::SINK:        vcl_cout<< "Sink"; break;
    case SIElement::JUNCT:        vcl_cout<< "Junct"; break;
    case SIElement::POINTPOINT:    vcl_cout<< "P-P"; break;
    case SIElement::POINTLINE:      vcl_cout<< "P-L"; break;
    case SIElement::POINTARC:      vcl_cout<< "P-A"; break;
    case SIElement::LINELINE:      vcl_cout<< "L-L"; break;
    case SIElement::LINEARC:      vcl_cout<< "L-A"; break;
    case SIElement::ARCARC:        vcl_cout<< "A-A"; break;
    case SIElement::POINTLINECONTACT:vcl_cout<< "PLC"; break;
    case SIElement::POINTARCCONTACT:  vcl_cout<< "PAC"; break;
    case SIElement::THIRDORDER:    vcl_cout<< "TO"; break;
    case SIElement::ARCTHIRDORDER:  vcl_cout<< "ATO"; break;
    }
    vcl_cout<< ", Sid: "<< current->id();
    vcl_cout<< ", simTime: "<< current->simTime();
    if (current->isActive()) {
      if (current->simTime() < MAX_RADIUS)
        vcl_cout<< ", Active.";
      else
        vcl_cout<< ", OutOfRange.";
    }
    else
      vcl_cout<< ", Dead.";
    if (current->graph_type() == SIElement::LINK)
      if (current->isPropagated()) 
        vcl_cout<< " Propagated.";
      else
        vcl_cout<< " Unpropagated.";

    vcl_cout<< vcl_endl;
  }

  vcl_cout<< "------------------" <<vcl_endl;
}

void IShock::DebugPrintGDList()
{
  vcl_cout<< "Shocks Saliency List: " <<vcl_endl;

  vcl_cout.precision(8);

  GDListIterator elmPtr = GDList.begin();
  for (; elmPtr != GDList.end(); elmPtr++) {
    SIElement* current = elmPtr->second;

    switch (current->type()) {
    case SIElement::A3SOURCE:      vcl_cout<< "A3Source"; break;
    case SIElement::SOURCE:        vcl_cout<< "Source"; break;
    case SIElement::SINK:        vcl_cout<< "Sink"; break;
    case SIElement::JUNCT:        vcl_cout<< "Junct"; break;
    case SIElement::POINTPOINT:    vcl_cout<< "P-P"; break;
    case SIElement::POINTLINE:      vcl_cout<< "P-L"; break;
    case SIElement::POINTARC:      vcl_cout<< "P-A"; break;
    case SIElement::LINELINE:      vcl_cout<< "L-L"; break;
    case SIElement::LINEARC:      vcl_cout<< "L-A"; break;
    case SIElement::ARCARC:        vcl_cout<< "A-A"; break;
    case SIElement::POINTLINECONTACT:vcl_cout<< "PLC"; break;
    case SIElement::POINTARCCONTACT:  vcl_cout<< "PAC"; break;
    case SIElement::THIRDORDER:    vcl_cout<< "TO"; break;
    case SIElement::ARCTHIRDORDER:  vcl_cout<< "ATO"; break;
    }
    vcl_cout<< ", Sid: "<< current->id();
    vcl_cout<< ", saliency: "<< current->dPnCost() <<vcl_endl;
  }

  vcl_cout<< "------------------" <<vcl_endl;
  if (GDList.begin()!=GDList.end())
    vcl_cout<< "Least salient shock: "<< GDList.begin()->second->id() << vcl_endl;

}

void IShock::MessageOutDetectionResults (int wndid)
{
   int nTotalShocks=0, nTotalContacts=0;
  int nTotalSNodes=0, nTotalSLinks=0;
  int nA3=0, nSO=0, nSJunct=0, nSink=0;
  int nPP=0, nPL=0, nPA=0;
  int nLL=0, nLA=0, nAA=0;
  int nPLC=0, nPAC=0, nLLC=0, nLAC=0, nAAC=0;
  int nTO=0, nATO=0;

  int nActiveShocks=0, nDeadShocks=0;

  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* current = dynamic_cast<SIElement*>(curS->second);
    if (current->isActive())
      nActiveShocks++;

    switch (current->type()) {
      case SIElement::A3SOURCE:        nA3++;    break;
      case SIElement::SOURCE:          nSO++;    break;
      case SIElement::JUNCT:          nSJunct++;  break;
      case SIElement::SINK:          nSink++;    break;
      case SIElement::POINTPOINT:      nPP++;    break;
      case SIElement::POINTLINE:        nPL++;    break;
      case SIElement::POINTARC:        nPA++;    break;
      case SIElement::LINELINE:        nLL++;    break;
      case SIElement::LINEARC:        nLA++;    break;
      case SIElement::ARCARC:          nAA++;    break;
      case SIElement::POINTLINECONTACT:  nPLC++;    break;
      case SIElement::POINTARCCONTACT:    nPAC++;    break;
      case SIElement::THIRDORDER:      nTO++;    break;
      case SIElement::ARCTHIRDORDER:    nATO++;    break;
    }
  }

  nTotalShocks = nSElement();
  nTotalSNodes = nA3 + nSO + nSJunct + nSink;
  nTotalSLinks = nPP + nPL + nPA + nLL + nLA + nAA + nTO + nATO;
  nTotalContacts = nPLC + nPAC + nLLC + nLAC + nAAC;
  nDeadShocks = nTotalShocks - nActiveShocks;

  assert (nTotalShocks == nTotalSNodes + nTotalSLinks + nTotalContacts);

  
   vcl_cout<< "\n===== Shock Detection Results ====="<<vcl_endl;
   if (_ShockAlgoType==LAGRANGIAN)
      vcl_cout<<"ShockAlgorithm: Lagrangian: "<<vcl_endl;
   else if (_ShockAlgoType==DYN_VAL)
      vcl_cout<<"ShockAlgorithm: Dynamic Validation: "<<vcl_endl;

   vcl_cout<<"Bnd Elements: "<< boundary()->nBElement()<<vcl_endl;
   vcl_cout<<"Total Shock Elements: "<< nTotalShocks <<vcl_endl<<vcl_endl;

  /*if (MessageOption == MSG_VERBOSE) {
    vcl_cout <<"SNodes: "<< nTotalSNodes <<vcl_endl; 
    vcl_cout <<"SLinks: "<< nTotalSLinks <<vcl_endl;
    vcl_cout <<"Contacts: "<< nTotalContacts <<vcl_endl;
    vcl_cout <<"A3s: "<< nA3  <<vcl_endl;
    vcl_cout <<"SO Source: "<< nSO <<vcl_endl;
    vcl_cout <<"Junctions: "<< nSJunct <<vcl_endl;
    vcl_cout <<"Sinks: " << nSink <<vcl_endl;
    vcl_cout <<"Point-Point: "<< nPP  <<vcl_endl;
    vcl_cout <<"Point-Line: "<< nPL <<vcl_endl;
    vcl_cout <<"Point-Arc: "<< nPA <<vcl_endl;
    vcl_cout <<"Line-Line: "<< nLL  <<vcl_endl;
    vcl_cout <<"Line-Arc: "<< nLA <<vcl_endl;
    vcl_cout <<"Arc-Arc: "<< nAA <<vcl_endl;
    vcl_cout <<"PointLineContact: "<< nPLC <<vcl_endl;
    vcl_cout <<"PointArcContact: "<< nPAC<<vcl_endl;
    vcl_cout <<"LLC: "<< nLLC <<vcl_endl;
    vcl_cout <<"LAC: "<< nLAC <<vcl_endl;
    vcl_cout <<"AAC: "<< nAAC <<vcl_endl;
    vcl_cout <<"ThirdOrder: "<< nTO  <<vcl_endl;
    vcl_cout <<"ArcThirdOrder: "<< nATO <<vcl_endl;
  }*/

}

//################################################
//      SHOCK VALIDATION 
bool IShock::ValidateBoundaryShockStructure (void)
{
  bool bValid = true;

  //1)Validate Each Shock Element
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* current = dynamic_cast<SIElement*>(curS->second);

    if (!validateSElement (current, true))
      bValid = false;
  }

  //2)Validate each BElement

  //3)Validate the Shock List
  if (!ValidateShockList())
    bValid = false;

  if (!bValid)
    vcl_cout<< "ValidateBoundaryShockStructure() Fails!!\n";
  else 
    //vcl_cout<< "ValidateBoundaryShockStructure() Successfully Completed !!\n";
    vcl_cout<< ".";

  return bValid;
}

bool IShock::ValidateShockInitilization (void)
{
  //for other algorithms the shocks are not necessarily valid at initialization
  if (GetShockAlgorithm() != LAGRANGIAN)
    return false;

  bool bValid = true;

  vcl_cout<< vcl_endl << "===== Shock Source Validation =====" <<vcl_endl;
  vcl_cout<< "InitAlgorithm: ";
  switch (_ShockInitType) {
  case BUCKETING_INIT: vcl_cout<<"Bucketing"; break;
  case LAGRANGIAN_INIT: vcl_cout<<"Brute Force"; break;
  case DT_INIT_POINTS: vcl_cout<<"D.T. Points"; break;
  }
  vcl_cout<<vcl_endl;
  //vcl_cout<< "# of sources: "<< nSourceElement() <<vcl_endl;

  //1)Validate Each Shock Element
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* current = dynamic_cast<SIElement*>(curS->second);

    if (current->type()==SIElement::SOURCE)
      if (!validateSElement (current, false))
        bValid = false;
  }

  //2)Validate each BElement

  //3)Validate the Shock List
  if (!ValidateShockList())
    bValid = false;

  if (!bValid)
    vcl_cout<< "ValidateShockInitilization() Fail!!\n\n";
  else 
    vcl_cout<< "ValidateShockInitilization() Successfully Completed !!\n\n";
  return bValid;
}

//Validate the shock list 'SIElmList' structure...
//The Shocklist is sorted according to:
//1)Active/Dead
//2)simTime: strict weak ordering
bool IShock::ValidateShockList (void)
{
  bool bValid = true;

  SIElement *currentShock, *prevShock;
  SIElmListIterator current = SIElmList.begin();

  //1) Validate time order
  current = SIElmList.begin();
  SIElmListIterator prev = current;

  //Empty Shock List, return true
  if (current == SIElmList.end())
    return true;
  current++;

  while (current != SIElmList.end()) {
    currentShock = current->second;
    prevShock = prev->second;

    if (currentShock->simTime() < MAX_RADIUS &&
      RisG(prevShock->simTime(),currentShock->simTime()) &&
      (prevShock->isActive() == currentShock->isActive())) {
      vcl_cout<< "ValidateShockList(): Order time list Failed!! sid=" << currentShock->id() <<vcl_endl;
      return false;
    }

    prev = current;
    current++;
  }

  return true;
}

bool IShock::validateBElement (BElement* belm)
{
  return true;
}

bool IShock::validateSElement (SIElement* selm, bool bPropagationCompleted)
{  
  bool bValid = false;

  switch (selm->type()) {
  case SIElement::A3SOURCE:
    bValid = ((SIA3Source*)selm)->validate (boundary());
  break;
  case SIElement::SOURCE:
    bValid = ((SISource*)selm)->validate (boundary(), bPropagationCompleted);
  break;
  case SIElement::JUNCT:
    bValid = ((SIJunct*)selm)->validate (boundary());
  break;
  case SIElement::SINK:
    bValid = ((SISink*)selm)->validate (boundary());
  break;

  case SIElement::POINTPOINT:
    bValid = ((SIPointPoint*)selm)->validate (boundary());
  break;
  case SIElement::POINTLINECONTACT:
    bValid = ((SIPointLineContact*)selm)->validate (boundary());
  break;
  case SIElement::POINTLINE:
    bValid = ((SIPointLine*)selm)->validate (boundary());
  break;
  case SIElement::LINELINE:
    bValid = ((SILineLine*)selm)->validate (boundary());
  break;
  case SIElement::THIRDORDER:
    bValid = ((SIThirdOrder*)selm)->validate (boundary());
  break;

  case SIElement::POINTARCCONTACT:
    bValid = ((SIPointArcContact*)selm)->validate (boundary());
  break;
  case SIElement::POINTARC:
    bValid = ((SIPointArc*)selm)->validate (boundary());
  break;
  case SIElement::LINEARC:
    bValid = ((SILineArc*)selm)->validate (boundary());
  break;
  case SIElement::ARCARC:
    bValid = ((SIArcArc*)selm)->validate (boundary());
  break;  
  case SIElement::ARCTHIRDORDER:
    bValid = ((SIArcThirdOrder*)selm)->validate (boundary());
  break;

  default:
    vcl_cout<< "No validation function. sid= "<< selm->id() <<"!!\n";
  break;
  }

  return bValid;
}

bool IShock::validateBElement (int id)
{
  return true;
}

bool IShock::validateSElement (int id)
{
  return true;
}

//vcl_cout: basePluginWnd->Message
void IShock::DebugPrintShockInfoFromID(int id)
{
  SIElmListIterator elmPtr = SIElmList.begin();
  for (; elmPtr != SIElmList.end(); elmPtr++) {
    SIElement* current = elmPtr->second;

    if (current->id() == id){
      //display info
      current->getInfo(vcl_cout);
      return;
    }      
  }
  vcl_cout <<"INVALID SHOCK ID: "<<id<<vcl_endl;
}
