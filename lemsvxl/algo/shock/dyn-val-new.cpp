// DYN-VAL-NEW.CPP
//Dynamic Validation for Shock Detection usingdistance measures only

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>

//#include <stdio.h>
#include <stdlib.h>
//#include <time.h>


#include "ishock.h"

//##########################################################
//SHOCK COMPUTATION
//##########################################################

//validate the current source
bool IShock::validateCandidateSource (SISource* candSource)
{
  BElement *lBElm, *rBElm, *candbElm;
  BElement::shockListIterator curS;
  double curDist;
  bool bConnectedGUILine;
  BLine* bl;
  BArc* ba;

  bool bValid = true;
  double candDist = candSource->startTime();
  Point candPt = candSource->origin();

  // go through all the boundary elements linked to the left
  // and right boundary elements of this candidate source
  // do the distance test on them
  
  lBElm = candSource->getBElement1();
  rBElm = candSource->getBElement2();

  Point footPt;

  //left side
  curS = lBElm->shockList.begin();

  for(;curS!=lBElm->shockList.end();++curS){
    if (!bValid) break;

    //get the other boundary element of this shock
    if ((*curS)->graph_type() == SIElement::LINK){
      if ( ((SILink*)(*curS))->lBElement() == lBElm) {
        candbElm = ((SILink*)(*curS))->rBElement();
        SILink* temp = ((SILink*)(*curS));
      }
      else {
        candbElm = ((SILink*)(*curS))->lBElement();
      }
    } 
    else if ((*curS)->type() == SIElement::SOURCE &&
          (*curS) != candSource){
      if ( ((SISource*)(*curS))->getBElement1() == lBElm)
        candbElm = ((SISource*)(*curS))->getBElement2();
      else 
        candbElm = ((SISource*)(*curS))->getBElement1();
    }
    else continue;

    //Ming:
    //if (candbElm==lBElm || candbElm==rBElm)
    //  continue;

    bConnectedGUILine=false;
    //compute distance from candidate source to the candidate BElement
    switch (candbElm->type()) {
      case BPOINT:
        curDist = _distPointPoint (candPt, ((BPoint*)candbElm)->pt());
        break;
      case BLINE:
        bl = (BLine*)candbElm;
        if (bl->s_pt()==lBElm || bl->s_pt()==rBElm ||
           bl->e_pt()==lBElm || bl->e_pt()==rBElm)
          bConnectedGUILine = true;
        if (bl->twinLine()==lBElm || bl->twinLine()==rBElm)
          continue;

        //curDist = bl->validDistPoint (candPt);
        //if (curDist==ISHOCK_DIST_HUGE)
        //  continue;
        curDist = bl->initDistSqPointGUI (candPt);
        curDist = vcl_sqrt (curDist);
        break;
      case BARC:
        ba = (BArc*)candbElm;
        if (ba->s_pt()==lBElm || ba->s_pt()==rBElm   ||
           ba->e_pt()==lBElm || ba->e_pt()==rBElm)
          bConnectedGUILine = true;
        if (ba->twinArc()==lBElm || ba->twinArc()==rBElm)
          continue;
        //curDist = ba->validDistPoint (candPt);
        //if (curDist==ISHOCK_DIST_HUGE)
        //  continue;
        curDist = ba->initDistSqPointGUI (candPt);
        curDist = vcl_sqrt (curDist);
        break;
        default: break;
    }

    //1)For ConnectedGUILine, test for _is, For the others, test for _isLEq
    if (bConnectedGUILine && _isL(curDist, candDist, DIST_BOUND))
        bValid = false;
    if (!bConnectedGUILine && _isLEq(curDist, candDist, DIST_BOUND))
        bValid = false;
  }

  //right side
  curS = rBElm->shockList.begin();

  for(;curS!=rBElm->shockList.end();++curS){
    if (!bValid) break;

    if ((*curS)->graph_type() == SIElement::LINK){
      //get the boundary element of this shock
      if ( ((SILink*)(*curS))->lBElement() == rBElm)
        candbElm = ((SILink*)(*curS))->rBElement();
      else 
        candbElm = ((SILink*)(*curS))->lBElement();
    }
    else if ((*curS)->type() == SIElement::SOURCE &&
          (*curS) != candSource){
      if ( ((SISource*)(*curS))->getBElement1() == rBElm)
        candbElm = ((SISource*)(*curS))->getBElement2();
      else 
        candbElm = ((SISource*)(*curS))->getBElement1();
    }
    else continue;

    //Ming:
    //if (candbElm==lBElm || candbElm==rBElm)
    //  continue;

    bConnectedGUILine=false;
    //compute distance from candidate source to the candidate BElement
    switch (candbElm->type()) {
      case BPOINT:
        curDist = _distPointPoint (candPt, ((BPoint*)candbElm)->pt());
        break;
      case BLINE:
        bl = (BLine*)candbElm;
        if (bl->s_pt()==lBElm || bl->s_pt()==rBElm   ||
           bl->e_pt()==lBElm || bl->e_pt()==rBElm)
          bConnectedGUILine = true;
        if (bl->twinLine()==lBElm || bl->twinLine()==rBElm)
          continue;

        //curDist = bl->validDistPoint (candPt);
        //if (curDist==ISHOCK_DIST_HUGE)
        //  continue;
        curDist = bl->initDistSqPointGUI (candPt);
        curDist = vcl_sqrt (curDist);
        break;
      case BARC:
        ba = (BArc*)candbElm;
        if (ba->s_pt()==lBElm || ba->s_pt()==rBElm   ||
           ba->e_pt()==lBElm || ba->e_pt()==rBElm)
          bConnectedGUILine = true;
        if (ba->twinArc()==lBElm || ba->twinArc()==rBElm)
          continue;
        //curDist = ba->validDistPoint (candPt);
        //if (curDist==ISHOCK_DIST_HUGE)
        //  continue;
        curDist = ba->initDistSqPointGUI (candPt);
        curDist = vcl_sqrt (curDist);
        break;
        default: break;
    }

    //1)For ConnectedGUILine, test for _is, For the others, test for _isLEq
    if (bConnectedGUILine && _isL(curDist, candDist, DIST_BOUND))
        bValid = false;
    if (!bConnectedGUILine && _isLEq(curDist, candDist, DIST_BOUND))
        bValid = false;
  }


  return bValid;
}

