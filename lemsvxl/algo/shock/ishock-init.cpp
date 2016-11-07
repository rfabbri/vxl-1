// ISHOCK-INIT.CPP
//Intrinsic Shock Initialization

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_cmath.h>

//MSGOUT
//#include "msgout.h"

//#include <stdio.h>
#include <time.h>

#include "ishock-common.h"
#include "ishock.h"
#include "bucket.h"

extern Point INFINITY_POINT; //common.cpp

//*****************************************************************
//*****************************************************************
// SHOCK INITILIZATION PART
//*****************************************************************
//****************************************************************

void IShock::InitializeShocks (SHOCK_INIT_TYPE InitType)
{
  BPoint *bp1, *bp2;
  BLine  *bl1, *bl2;
  BArc   *ba1, *ba2;

  //1)Initialization options. Call initShocksBucketing() for bucketing.
  if (InitType == DEFAULT_INIT) {
    InitType = _ShockInitType;
  }

  //2)Pre-Process the Boundary.
  if (boundary()->_bIsPreprocesvcl_singNeeded)
    boundary()->PreProcessBoundary ();

  //3)Set up lists of BElements.
  //  Depending on the InitType, the boundaries involved is picked.
  BElmListType *ListA; //List of all new elements
                //1)The first initialization: all BElements
                //2)Dynamic adding: all new elements
  BElmListType *ListB; //List of all possible paired elements
                //1)Brute-Force: all BElements
  BElmListType *ListC; //List of all possible elements for invalidation
                //1)Brute-Force: all BElements

  if (InitType == LAGRANGIAN_INIT) { //O(n^3) Brute-Force Initilization
    ListA = &(boundary()->BElmList);
    ListB = &(boundary()->BElmList);
    ListC = &(boundary()->BElmList);
  }
  else if (InitType == BRUTE_FORCE_ADD_PATCH_INIT) { //O(n^2)*num_new
    ListA = &(boundary()->taintedBElmList);
    ListB = &(boundary()->BElmList);
    ListC = &(boundary()->BElmList);
  }
  else if (InitType == REGULAR_ADD_PATCH_INIT) {
    ListA = &(boundary()->taintedBElmList);
    ListB = &(boundary()->taintedBElmList);
    ListC = &(boundary()->taintedBElmList);
  }
  else if (InitType == DELETE_PATCH_INIT) {
    ListA = &(boundary()->taintedBElmList);
    ListB = &(boundary()->taintedBElmList);
    ListC = &(boundary()->taintedBElmList);
  }

  //4)Setup array of BElements for loop speedup.
  int i;
  BElement** BElmArray = new  BElement*[boundary()->BElmList.size()];
  BElmListIterator bit = boundary()->BElmList.begin();
  for (i=0; bit != boundary()->BElmList.end(); bit++, i++)
    BElmArray[i] = bit->second;

  BElement** taintedBElmArray;
  if (InitType == LAGRANGIAN_INIT)
    taintedBElmArray = NULL;
  else {
    taintedBElmArray = new BElement*[boundary()->taintedBElmList.size()];
    bit = boundary()->taintedBElmList.begin();
    for (i=0; bit != boundary()->taintedBElmList.end(); bit++, i++)
      taintedBElmArray[i] = bit->second;
  }

  //5)Setup ArrayA, ArrayB, ArrayC for loop speedup.
  int sizeArrayA, sizeArrayB, sizeArrayC;
  BElement **ArrayA, **ArrayB, **ArrayC;
  if (InitType == LAGRANGIAN_INIT) { //O(n^3) Brute-Force Initilization
    ArrayA = BElmArray;
    ArrayB = BElmArray;
    ArrayC = BElmArray;
    sizeArrayA = sizeArrayB = sizeArrayC = boundary()->BElmList.size();
  }
  else if (InitType == BRUTE_FORCE_ADD_PATCH_INIT) { //O(n^2)*num_new
    ArrayA = taintedBElmArray;
    ArrayB = BElmArray;
    ArrayC = BElmArray;
    sizeArrayA = boundary()->taintedBElmList.size();
    sizeArrayB = sizeArrayC = boundary()->BElmList.size();
  }
  else if (InitType == REGULAR_ADD_PATCH_INIT) {
    ArrayA = taintedBElmArray;
    ArrayB = taintedBElmArray;
    ArrayC = taintedBElmArray;
    sizeArrayA = sizeArrayB = sizeArrayC = boundary()->taintedBElmList.size();
  }
  else if (InitType == DELETE_PATCH_INIT) {
    ArrayA = taintedBElmArray;
    ArrayB = taintedBElmArray;
    ArrayC = taintedBElmArray;
    sizeArrayA = sizeArrayB = sizeArrayC = boundary()->taintedBElmList.size();
  }

  Point  midPoint;

  //6)For adding new elements, first we have to validate each existing sources, O(#_of_sources*n).
  if (InitType == BRUTE_FORCE_ADD_PATCH_INIT) {
    vcl_vector<SIElement*> elmsToDel;
    SIElmListIterator curS = SIElmList.begin();
    for (; curS!=SIElmList.end(); curS++){
      SIElement* curSElm = curS->second;
      //Only check 2nd-order sources...
      if (!curSElm->isASource()) continue;

      //compute distance from the shock element to the newBElm from ListA
      DIST_TYPE dist;
      for (i=0; i<sizeArrayA;i++) {
        switch (ArrayA[i]->type()) {
        case BPOINT:
          bp1 = (BPoint*)ArrayA[i];
          dist = bp1->distPoint (curSElm->origin());
          break;
        case BLINE:
          bl1 = (BLine*)ArrayA[i];
          dist = bl1->validDistPoint (curSElm->origin());
          if (dist==ISHOCK_DIST_HUGE)
            continue;
          break;
        case BARC:
          ba1 = (BArc*)ArrayA[i];
          dist = ba1->validDistPoint (curSElm->origin());
          if (dist==ISHOCK_DIST_HUGE)
            continue;
          break;
        }
        //delete the invalid source (and all children).
        if (!(_isGEq(dist, curSElm->startTime(), DIST_BOUND))) { //isG
          elmsToDel.push_back(curSElm);
          break; //if any BElm invalid the source, jump-out!
        }
      }//end for aa
    }//end for SIElmList

    for (int i=0; i<elmsToDel.size() ; i++)
      delASIElement(elmsToDel[i]);
  }

  //4)C(N,2), Compute midpoints for candidate SOs
  //  Or      Initialize ContactShock
  //  O(N),   Validate the midpoints
  int count=0;
   for (int a=sizeArrayA-1; a>=0; a--, count++) {
    int b, c;
    double t;
    VECTOR_TYPE aa;
    DIST_TYPE midDistSq;
    Point footPt, arcPt;

    if (InitType == BRUTE_FORCE_ADD_PATCH_INIT) {
      b=sizeArrayB-1;
      b -= (count+1);
    }
    else { //only need to look at C(N,2) combinations
      b=a-1;
    }

      for (; b>=0; b--) {
      bp1=NULL; bl1=NULL; ba1=NULL;
         switch (ArrayA[a]->type()) {
      case BPOINT:
        bp1 = (BPoint*)ArrayA[a];
        bp2=NULL; bl2=NULL; ba2=NULL;
        switch (ArrayB[b]->type()) {
        case BPOINT:
          bp2 = (BPoint*)ArrayB[b];
          assert (! _BisEqPoint(bp1->pt(), bp2->pt()) );

          midDistSq = _distSqPointPoint (bp1->pt(), bp2->pt())/4;
          midPoint = _midPointPoint (bp1->pt(), bp2->pt());
          break;
        case BLINE:
          bl2 = (BLine*)ArrayB[b];
          if (bl2->s_pt() == bp1) {
            initializeAContactShock(bp1, bl2, InitType);
            continue;
          }
          else if (bl2->e_pt() == bp1) {
            initializeAContactShock(bl2, bp1, InitType);
            continue;
          }
          else {
            t = bl2->isPointValidInTPlaneFuzzy (bp1->pt());
            if (t==0)
              continue;
            footPt = _getFootPt (bl2->start(), bl2->end(), t);
            midDistSq = _distSqPointLine (bp1->pt(), bl2->start(), bl2->end())/4;
            midPoint = _midPointPoint (bp1->pt(), footPt);
          }
          break;
        case BARC:
          ba2 = (BArc*)ArrayB[b];
          if (ba2->s_pt() == bp1) {
            initializeAContactShock(bp1, ba2, InitType);
            continue;
          }
          else if (ba2->e_pt() == bp1) {
            initializeAContactShock(ba2, bp1, InitType);
            continue;
          }
          else {
            if (!ba2->isPointValidInRAngleFuzzy (bp1->pt(), aa))
              continue;
            arcPt = _vectorPoint (ba2->center(), aa, ba2->R());
            midDistSq = _distSqPointPoint (bp1->pt(), arcPt)/4;
            midPoint = _midPointPoint (bp1->pt(), arcPt);
          }
          break;
        default:
          continue;
        }
        break;
      case BLINE:
        bl1 = (BLine*)ArrayA[a];
        bp2=NULL; bl2=NULL; ba2=NULL;
        switch (ArrayB[b]->type()) {
        case BPOINT:
          bp2 = (BPoint*)ArrayB[b];
          if (bl1->s_pt() == bp2) {
            initializeAContactShock(bp2, bl1, InitType);
            continue;
          }
          else if (bl1->e_pt() == bp2) {
            initializeAContactShock(bl1, bp2, InitType);
            continue;
          }
          else {
            t = bl1->isPointValidInTPlaneFuzzy (bp2->pt());
            if (t==0)
              continue;
            footPt = _getFootPt (bl1->start(), bl1->end(), t);
            midDistSq = _distSqPointLine (bp2->pt(), bl1->start(), bl1->end())/4;
            midPoint = _midPointPoint (bp2->pt(), footPt);
          }
          break;
        case BLINE: //BY DEFINITION, NO LINE-LINE SOURCE
          continue;
          break;
        case BARC:
          ba2 = (BArc*)ArrayB[b];

          //For Arc-Line, no source if arc is CCW.
          if (ba2->nud()==ARC_NUD_CCW)
            continue;

          t = bl1->isPointValidInTPlaneFuzzy (ba2->center());
          if (t==0)
            continue;
          footPt = _getFootPt (bl1->start(), bl1->end(), t);
          if (!ba2->isPointValidInRAngleFuzzy (footPt, aa))
            continue;
          arcPt = _vectorPoint (ba2->center(), aa, ba2->R());
          midDistSq = _distSqPointLine (arcPt, bl1->start(), bl1->end())/4;
          midPoint = _midPointPoint (arcPt, footPt);
          break;
        }
        break;
      case BARC:
        ba1 = (BArc*)ArrayA[a];
        bp2=NULL; bl2=NULL; ba2=NULL;
        switch (ArrayB[b]->type()) {
        case BPOINT:
          bp2 = (BPoint*)ArrayB[b];
          if (ba1->s_pt() == bp2) {
            initializeAContactShock(bp2, ba1, InitType);
            continue;
          }
          else if (ba1->e_pt() == bp2) {
            initializeAContactShock(ba1, bp2, InitType);
            continue;
          }
          else {
            if (!ba1->isPointValidInRAngleFuzzy (bp2->pt(), aa))
              continue;
            arcPt = _vectorPoint (ba1->center(), aa, ba1->R());
            midDistSq = _distSqPointPoint (bp2->pt(), arcPt)/4;
            midPoint = _midPointPoint (bp2->pt(), arcPt);
          }
          break;
          case BLINE:  
          {
            //For Arc-Line, no source if arc is CCW.
            if (ba1->nud()==ARC_NUD_CCW)
              continue;

            bl2 = (BLine*)ArrayB[b];
            t = bl2->isPointValidInTPlaneFuzzy (ba1->center());
            if (t==0)
              continue;
            footPt = _getFootPt (bl2->start(), bl2->end(), t);
            if (!ba1->isPointValidInRAngleFuzzy (footPt, aa))
              continue;
            arcPt = _vectorPoint (ba1->center(), aa, ba1->R());
            midDistSq = _distSqPointLine (arcPt, bl2->start(), bl2->end())/4;
            midPoint = _midPointPoint (arcPt, footPt);
          }
          break;
          case BARC:
          {
            ba2 = (BArc*)ArrayB[b];
            if (ba1->twinArc() == ba2)
              continue;

            //For Arc-Arc, no source if both are CCW.
            if (ba1->nud()==ARC_NUD_CCW && ba2->nud()==ARC_NUD_CCW)
              continue;

            //For Arc-Arc, 3 cases.
            DIST_TYPE H = _distPointPoint (ba1->center(), ba2->center());
            if (H > ba1->R()+ba2->R()) { //1)two arcs are far away, test center.

              if (ba1->nud()!=ARC_NUD_CW || ba2->nud()!=ARC_NUD_CW)
                continue;

              VECTOR_TYPE a1;
              if (!ba1->isPointValidInRAngleFuzzy (ba2->center(), a1))
                continue;
              Point arcPt1 = _vectorPoint (ba1->center(), a1, ba1->R());
              VECTOR_TYPE a2;
              if (!ba2->isPointValidInRAngleFuzzy (ba1->center(), a2))
                continue;
              Point arcPt2 = _vectorPoint (ba2->center(), a2, ba2->R());
              midDistSq = _distSqPointPoint (arcPt1, arcPt2)/4;
              midPoint = _midPointPoint (arcPt1, arcPt2);
            }
            else if (H<vcl_fabs(ba1->R()-ba2->R())) { //2)detect small and big arc.
              BArc* bigArc;
              BArc* smallArc;
              if (ba1->R()>ba2->R()) {
                bigArc = ba1; smallArc = ba2;
              }
              else {
                bigArc = ba2; smallArc = ba1;
              }

              if (bigArc->nud()!=ARC_NUD_CCW || smallArc->nud()!=ARC_NUD_CW)
                continue;

              VECTOR_TYPE big_a;
              if (!bigArc->isPointValidInRAngleFuzzy (smallArc->center(), big_a))
                continue;
              Point bigArcPt = _vectorPoint (bigArc->center(), big_a, bigArc->R());

              //For smallArc, the footPt is on the other side.
              Point footPt;
              VECTOR_TYPE v = _vPointPoint (bigArc->center(), smallArc->center());
              footPt = _vectorPoint (bigArc->center(), v, bigArc->R());

              if (!smallArc->isVectorFuzzilyValid (v, EP_EPSILON))
                continue;

              Point smallArcPt = _vectorPoint (smallArc->center(), v, smallArc->R());
              midDistSq = _distSqPointPoint (bigArcPt, smallArcPt)/4;
              midPoint = _midPointPoint (bigArcPt, smallArcPt);
            }
            else { //3)arcs intersecting, no possible source.
              continue;
            }
          }
          break;
        }
        break;
      }//end switch (i->second)

      if (_ShockAlgoType==LAGRANGIAN) {
        //Validate MidPoint as valid sources
        //if the distance of this midPoint to any other
        //element is smaller or equal to its distance to 
        //current elements it is not a valid source !!!
        //This is what makes it O(N^3) ...
        //We  have to validate against the entire list
        bool bValid = true;
        for (c=sizeArrayC-1; c>=0; c--) {
          if (doesBElementInvalidateSource (ArrayC[c], ArrayA[a], ArrayB[b], 
                                 midPoint, midDistSq)) {
            bValid = false;
            break;
          }
        }//end for c

        if (bValid) { //if it is a valid source, create a shock source
          //For DELETE_PATCH_INIT and REGULAR_ADD_PATCH_INIT, need to validate against both elms.
          if (InitType==REGULAR_ADD_PATCH_INIT || InitType==DELETE_PATCH_INIT)
            if (!ValidateCandidateSource_TwoBElmNeighbors (ArrayA[a], ArrayB[b], midPoint, midDistSq))
              continue;
          initializeASource (ArrayA[a], ArrayB[b], midPoint, vcl_sqrt(midDistSq));
        }
      }//end if LAGRANGIAN
    }//end for b
   }//end for a

  //clear the taintedBElementList now that shocks have been reinitialized from them
  boundary()->taintedBElmList.clear();

  ////vcl_cout<<"InitializeShocks(): # of sources initialized: "<< nSourceElement() <<vcl_endl;
  //if (MessageOption==MSG_VERBOSE){
  //  DebugPrintShockList (true);
  //}

  //DYNVAL: Clear all IVSes
  //boundary()->clearAllIVS ();

  delete []BElmArray;
  delete []taintedBElmArray;
}

//return true: curBElm invalidates the source.
//return false: curBElm does not invalidate the source.
bool IShock::doesBElementInvalidateSource (BElement* curBElm, BElement* elm1, BElement* elm2, 
                             Point midPoint, double midDistSq)
{
  double curDistSq;

  if (curBElm==elm1 || curBElm==elm2)
    return false;

  switch (curBElm->type()) {
  case BPOINT:
    curDistSq = _distSqPointPoint (((BPoint*)curBElm)->pt(), midPoint);
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  break;
  case BLINE:
  {
    BLine* bline = (BLine*)curBElm;

    //Only need for BLIne. Have to do this first, because it's a special case 
    //for a line to invalidate source from its 2 endPts.
    if ((bline->s_pt()==elm1 && bline->e_pt()==elm2) ||
       (bline->e_pt()==elm1 && bline->s_pt()==elm2)) {
      return true;
    }

    curDistSq = bline->validDistSqPoint (midPoint);
    
    if (bline->s_pt()==elm1 || bline->s_pt()==elm2 ||
       bline->e_pt()==elm1 || bline->e_pt()==elm2) {
      if (_isL(curDistSq, midDistSq, DIST_BOUND)) //ConnectedGUI
        return true;
    }
    else {
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return true;
    }
  }
  break;
  case BARC:
  {
    BArc* barc = (BArc*)curBElm;

    curDistSq = barc->validDistSqPoint (midPoint);
  
    if (barc->s_pt()==elm1 || barc->s_pt()==elm2 ||
       barc->e_pt()==elm1 || barc->e_pt()==elm2) {
      if (_isL(curDistSq, midDistSq, DIST_BOUND)) //ConnectedGUI
        return true;
    }
    else {
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return true;
    }
  }
  break;
  }//end switch

  return false;
}

//For REGULAR_ADD_PATCH_INIT and DELETE_PATCH_INIT
//use initDistSqPointGUI() instead of validDistSqPoint()
bool IShock::doesBElementInvalidateSource2 (BElement* curBElm, BElement* elm1, BElement* elm2, 
                              Point midPoint, double midDistSq)
{
  double curDistSq;

  if (curBElm==elm1 || curBElm==elm2)
    return false;

  switch (curBElm->type()) {
  case BPOINT:
    curDistSq = _distSqPointPoint (((BPoint*)curBElm)->pt(), midPoint);
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  break;
  case BLINE:
  {
    BLine* bline = (BLine*)curBElm;

    //Only need for BLIne. Have to do this first, because it's a special case 
    //for a line to invalidate source from its 2 endPts.
    if ((bline->s_pt()==elm1 && bline->e_pt()==elm2) ||
       (bline->e_pt()==elm1 && bline->s_pt()==elm2)) {
      return true;
    }

    curDistSq = bline->initDistSqPointGUI (midPoint); //validDistSqPoint()
    
    if (bline->s_pt()==elm1 || bline->s_pt()==elm2 ||
       bline->e_pt()==elm1 || bline->e_pt()==elm2) {
      if (_isL(curDistSq, midDistSq, DIST_BOUND)) //ConnectedGUI
        return true;
    }
    else {
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return true;
    }
  }
  break;
  case BARC:
  {
    BArc* barc = (BArc*)curBElm;

    curDistSq = barc->initDistSqPointGUI (midPoint); //validDistSqPoint()
  
    if (barc->s_pt()==elm1 || barc->s_pt()==elm2 ||
       barc->e_pt()==elm1 || barc->e_pt()==elm2) {
      if (_isL(curDistSq, midDistSq, DIST_BOUND)) //ConnectedGUI
        return true;
    }
    else {
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return true;
    }
  }
  break;
  }//end switch

  return false;
}

//Go through belm1's list of SIContacts
//to see if there exists a source formed with belm2
SIContact* IShock::contactExists(BElement* belm1, BElement* belm2)
{
  BElement::shockListIterator curS = belm1->shockList.begin();
   for (; curS!=belm1->shockList.end(); ++curS) {
      if ((*curS)->label() == SIElement::CONTACT) {
      if (((SIContact*)(*curS))->lBElement() == belm2 ||
         ((SIContact*)(*curS))->rBElement() == belm2)
         return (SIContact*)(*curS);
    }
  }

  return NULL;
}

//Go through belm1's list of sources
//to see if there exists a source formed with belm2
SISource* IShock::sourceExists (BElement* belm1, BElement* belm2)
{
  BElement::shockListIterator curS = belm1->shockList.begin();
   for (; curS!=belm1->shockList.end(); ++curS) {
      if ((*curS)->type() == SIElement::SOURCE) {
      if (((SISource*)(*curS))->getBElement1() == belm2 ||
         ((SISource*)(*curS))->getBElement2() == belm2)
         return (SISource*)(*curS);
    }
  }

  return NULL;
}

SIContact* IShock::initializeAContactShock (BElement* elm1, BElement* elm2, int InitType)
{
   SIContact *newShock;
  BPoint  *bp1, *bp2;
  BLine    *bl1, *bl2;
  BArc    *ba1, *ba2;

  //For DELETE_PATCH_INIT and REGULAR_ADD_PATCH_INIT
  //need to do an existence check before initializing the shock because it may already exist.
  if ((InitType==REGULAR_ADD_PATCH_INIT || InitType==DELETE_PATCH_INIT) && 
     contactExists(elm1, elm2))
    return NULL;

   if (elm1->type()==BPOINT) {
      if (elm2->type()==BLINE) {
      bp1 = (BPoint*)elm1; bl2 = (BLine*)elm2;
      newShock = new SIPointLineContact (nextAvailableID(), bp1, bl2);
    }
      else if (elm2->type()==BARC) { 
      bp1 = (BPoint*)elm1; ba2 = (BArc*)elm2;
      newShock = new SIPointArcContact (nextAvailableID(), bp1, ba2);
    }
   }
   else {
      if (elm1->type()==BLINE) {
      bl1 = (BLine*)elm1; bp2 = (BPoint*)elm2;
      newShock = new SIPointLineContact (nextAvailableID(), bl1, bp2);
      }   
      else if (elm1->type()==BARC) {
      ba1 = (BArc*)elm1; bp2 = (BPoint*)elm2;
      newShock = new SIPointArcContact (nextAvailableID(), ba1, bp2);
    }
   }
  addASIElement (newShock);
  return newShock;
}

SISource* IShock::initializeASource (BElement* belm1, BElement* belm2, 
                          Point startpt, RADIUS_TYPE time)
{
  //1)Initialize the newSource
   SIElement::SHOCKLABEL newLabel = determineShockLabel (belm1, belm2);
  SISource* newSource = new SISource (
    nextAvailableID(), newLabel, time, startpt, belm1, belm2);

  //2)Add it into shockList
  addASIElement (newSource);
  _nSourceElement++; //# of sources counter++

  //3)Propagated two child shocks
  InitializeShockBranchesFromASource(newSource);
  newSource->setPropagated (true);
  deactivateASIElement(newSource);
  
  //EPSILONISSUE 7
  //4)For sources forms with ColinearContact, deal with it here!
  //if not from BPoint source, ignore this part.
  //P-P: belm1 is BPoint, belm2 is BPoint
  //P-L: belm1 is BPoint, belm2 is BLine
  //P-A: belm1 is BPoint, belm2 is BArc
  if (belm1->type()==BPOINT)
    checkAndIntersectColinearContacts (newSource, belm1, belm2);

  return newSource;
}

//For DELETE_PATCH_INIT and REGULAR_ADD_PATCH_INIT, need to validate against both elms.
bool IShock::ValidateCandidateSource_TwoBElmNeighbors (BElement* belm1, BElement* belm2, 
                                     Point midPoint, double midDistSq)
{
  //1)If there already exists a source, ignore and return.
  if (sourceExists (belm1, belm2))
    return false;

  //2)Check belm1's neighbors
  bool bValid = true;
  BElement::shockListIterator curS = belm1->shockList.begin();
  for (; curS!=belm1->shockList.end(); ++curS){
    //2-1)get curBElm from the neighbor of belm1
    BElement* curBElm = belm1->getNeighboringBElementFromSIElement (*curS);
    if (curBElm==NULL)
      continue;

    //2-2)
    if (doesBElementInvalidateSource2 (curBElm, belm1, belm2, midPoint, midDistSq)) {
      bValid = false;
      break;
    }
  }

  //3)
  return bValid;
}


//######################## INIT DT POINTS ##########################

void IShock::init_source_from_delaunay_edge (edge_ref e)
{
  site_struct *op = (site_struct*)ORG(e);
  site_struct *dp = (site_struct*)DEST(e);

  //one side
  site_struct *tp = (site_struct*)DEST(ONEXT(e));
  //other side
  site_struct *bp = (site_struct*)DEST(ONEXT(SYM(e)));

  double d2_od = (op->x - dp->x)*(op->x - dp->x) + (op->y - dp->y)*(op->y - dp->y);
  double d2_ot = (op->x - tp->x)*(op->x - tp->x) + (op->y - tp->y)*(op->y - tp->y);
  double d2_dt = (tp->x - dp->x)*(tp->x - dp->x) + (tp->y - dp->y)*(tp->y - dp->y);
  double d2_ob = (op->x - bp->x)*(op->x - bp->x) + (op->y - bp->y)*(op->y - bp->y);
  double d2_db = (bp->x - dp->x)*(bp->x - dp->x) + (bp->y - dp->y)*(bp->y - dp->y);

  bool test1 = (d2_ot+d2_dt > d2_od);
  bool test2 = (d2_ob+d2_db > d2_od);

  /*
  //this test is for the existence of a voronoi edge
  double t1 = ccw((int)op, (int)dp, (int)tp);
  double t2 = ccw((int)dp, (int)op, (int)bp);

  if (t1>0.0 && t2>0.0){
    //add voronoi edge here
  }
  */

  if (test1 && test2){
    //passed both tests
    //init a source from this edge

    Point midPoint = Point((op->x + dp->x)/2,(op->y + dp->y)/2);
    double midDist = vcl_sqrt(d2_od)/2;
    BElement* lbp = boundary()->BElmList[op->id];
    BElement* rbp = boundary()->BElmList[dp->id];
    initializeASource(lbp, rbp, midPoint, midDist);

  }

  //just a test for DT visualization
  //boundary()->addGUILine(op->x, op->y, dp->x, dp->y);

  //edge_ref erp = SYM(e);
  //op = (site_struct*)ORG(erp);
  //dp = (site_struct*)DEST(erp);
  
}

extern unsigned next_mark;

void IShock::TraverseDelaunay(edge_ref e)
{
  unsigned mark = next_mark;
  next_mark++;
  if (next_mark == 0) next_mark = 1;
  quad_do_enum(e, mark); 
}

void IShock::quad_do_enum (edge_ref e, unsigned mark)
{
  while (MARK(e) != mark)
  {
    init_source_from_delaunay_edge(e);
    MARK(e) = mark;
    quad_do_enum (ONEXT(SYM(e)), mark);
    e = ONEXT(e);
  }
}

void IShock::InitializeShocksDTPoints (void)
{
  int num_of_points;

  //quick check 
  if (boundary()->GetBoundaryLimit()==BIG_RECTANGLE || boundary()->GetBoundaryLimit()==BIG_CIRCLE)
    assert(0);
  else
    num_of_points = boundary()->nBElement();

  long sec1 = clock();

  //allocate memory for DT
  site_struct *sites = new site_struct[num_of_points];

  int x=0;
  BElmListIterator i= boundary()->BElmList.begin();
   for (; i!=boundary()->BElmList.end(); i++) {
    BPoint* bp = (BPoint*)(i->second);
    sites[x].x = (float)bp->pt().x;
    sites[x].y = (float)bp->pt().y;
    sites[x].id = bp->id();
    x++;
  }

  edge_ref e;
  e = delaunay_build (sites, num_of_points);

  long sec2 = clock();

  vcl_cout << "Time before traversal of the delaunay: " << sec2-sec1 <<" msecs."<<vcl_endl;

  //traverse the quad edge data structure to extract the valid sources
  TraverseDelaunay(e);

  //quad_enum(e, init_source_from_delaunay_edge, NULL);

  //delete the DT structure
  delete []sites;

  //clear the taintedBElementList now that shocks have been reinitialized from them
  boundary()->taintedBElmList.clear();

  //vcl_cout<<"# of shocks initialized: "<<nSElement() <<vcl_endl;
  //DebugPrintShockList ();
}



//Bucketing:
//Add computed shock structure from each bucket to the main IShock...
void IShock::MergeFromIShock (IShock* ishock)
{
  //Go through the Shock list and add them into the main list...
  //No duplicate of SIElements.
  SIElmListIterator curS = ishock->SIElmList.begin();
  for (; curS!=ishock->SIElmList.end(); curS++) {
    SIElement* curSElm = curS->second;

    //change this guy's id
    _nextAvailableID++;
    curSElm->setId (_nextAvailableID);
    addASIElement (curSElm);
  }
}


//######################## SPEED PROFILE ##########################
//######################## MAP ##########################

void IShock::TestSTLProfiling (BElmListType* ListA, BElmListType* ListB, BElmListType* ListC)
{

  long sec1 = clock();
  int totalid=0;
  RBElmListIterator aa, bb, cc;
  int n=0, counter = 0;
   for (aa=ListA->rbegin(); aa != ListA->rend(); aa++, n++) {
    for (bb=ListB->rbegin(); bb != ListB->rend(); bb++) {
      for (cc=ListC->rbegin(); cc != ListC->rend(); cc++) {
        counter++;
        int id1 = (aa->second)->id();
        int id2 = (bb->second)->id();
        int id3 = (cc->second)->id();
        totalid += (id1*id2*id3);
      }
    }
  }
  long sec2 = clock();
  vcl_cout<< "\n\nNumber of elements= " << n <<vcl_endl<<vcl_endl;
  vcl_cout<< "STL map counter= " << counter <<vcl_endl;
  vcl_cout<< "STL map totalid= " << totalid <<vcl_endl;
  vcl_cout << "Time on trivial loop on STL map: "<<sec2-sec1<<" msec."<<vcl_endl<<vcl_endl;

  //######################## VECTOR ##########################
  vcl_vector<BElement* > belmVector;
  for (aa=ListA->rbegin(); aa != ListA->rend(); aa++) {
    belmVector.push_back (aa->second);
  }
  vcl_vector<BElement* > *VectorA = &belmVector;
  vcl_vector<BElement* > *VectorB = &belmVector;
  vcl_vector<BElement* > *VectorC = &belmVector;
  typedef vcl_vector<BElement* >::reverse_iterator belmVectorIterator;

  sec1 = clock();
  counter = 0;
  totalid = 0;
  belmVectorIterator iii, jjj, kkk;
  for (iii=belmVector.rbegin(); iii!=belmVector.rend(); iii++) {
    for (jjj=belmVector.rbegin(); jjj!=belmVector.rend(); jjj++) {
      for (kkk=belmVector.rbegin(); kkk!=belmVector.rend(); kkk++) {
        counter++;
        int id1 = (*iii)->id();
        int id2 = (*jjj)->id();
        int id3 = (*kkk)->id();

        totalid += (id1*id2*id3);
      }
    }
  }
  sec2 = clock();
  vcl_cout<< "STL vcl_vector counter= " << counter <<vcl_endl;
  vcl_cout<< "STL vcl_vector totalid= " << totalid <<vcl_endl;
  vcl_cout << "Time on trivial loop on STL Vector: "<<sec2-sec1<<" msec."<<vcl_endl<<vcl_endl;

  //######################## ARRAY ##########################
  BElement** belmArray = new  BElement*[n];
  int _i;
  for (_i=0, aa=ListA->rbegin(); aa != ListA->rend(); aa++, _i++) {
    belmArray[_i] = aa->second;
  }

  sec1 = clock();
  counter = 0;
  totalid = 0;
  for (int ii=0; ii<n; ii++) {
    for (int jj=0; jj<n; jj++) {
      for (int kk=0; kk<n; kk++) {
        counter++;
        int id1 = belmArray[ii]->id();
        int id2 = belmArray[jj]->id();
        int id3 = belmArray[kk]->id();

        totalid += (id1*id2*id3);
      }
    }
  }
  sec2 = clock();
  vcl_cout<< "C++ array counter= " << counter <<vcl_endl;
  vcl_cout<< "C++ array totalid= " << totalid <<vcl_endl;
  vcl_cout << "Time on trivial loop on C++ Array: "<<sec2-sec1<<" msec."<<vcl_endl<<vcl_endl;
  delete []belmArray;

  return;
}
