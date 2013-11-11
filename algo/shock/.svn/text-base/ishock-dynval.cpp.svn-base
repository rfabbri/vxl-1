#include "ishock.h"

void IShock_DynVal::moveASILinkToInfinity (SILink* elm)
{
  //1)First remove the shock element
  SIElmList.erase( 
    A_R_IDpair(elm->isActive(), 
      R_IDpair(elm->simTime(),elm->id())) ); 

  //3)update the shock's simulation time
   elm->setSimTime (ISHOCK_DIST_HUGE);

  if (_ShockAlgoType==DYN_VAL) {
    BPoint_DynVal* bp;
    SIPointPoint* spp;
    bool bSuccess;
    VECTOR_TYPE v;

    switch (elm->type()) {
    case SIElement::POINTPOINT:
      spp = (SIPointPoint*) elm;
      //LEFT BPOINT
      bp = (BPoint_DynVal*)elm->lBElement();
      //lBPoint: Update the _end of IVS which includes u to n
      bSuccess = bp->pointIV()->updateEnd (spp->u(), spp->n(), true);

      //If no update (first propagation), then create one from u to n
      if (!bSuccess)
        bp->pointIV()->addPointIVS (spp->u(), spp->n());

      //RIGHT BPOINT
      bp = (BPoint_DynVal*)elm->rBElement();
      //rBPoint: Update the _start of IVS which includes u+M_PI to n
      v = angle0To2Pi (spp->u()+M_PI);
      bSuccess = bp->pointIV()->updateEnd (v, spp->n(), false);

      //If no update (first propagation), then create one from u to n
      if (!bSuccess)
        bp->pointIV()->addPointIVS (spp->n(), v);

    break;
    }

    //4)Update left & right belement's Drawing
    boundary()->updateBoundaryDrawing (elm->lBElement());
    boundary()->updateBoundaryDrawing (elm->rBElement());  
  }

  //6)then reinsert it into the appropriate place
  SIElmList.insert(SIElmPair(A_R_IDpair(elm->isActive(), 
    R_IDpair(elm->simTime(),elm->id())), elm));

  //8)Update current shocks's Drawing
   updateShockDrawing (elm);
}

void IShock_DynVal::moveSILinksToALeftJunction (SILink* l_neighbor, SILink* current, 
                      IntrinsicIntersection intersection)
{
  //if we ever get to this function, it means that at the current time
  //it's guarrenteed that the current and left neighbor are gonna intersect
  //SPECIAL CASES:: if either shock is a collinear contact
  //intersect with its collinear neighbor first
  if (current->label() == SIElement::CONTACT){
    SIContact* cContact = (SIContact*)current;
    //Only update when there is no SO Source between the 2 ColinearContact
    //(cContact->simTime()==HUGE && cContact->Neighbor()->simTime()==ISHOCK_DIST_HUGE) ||
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

  //8)DYNVAL: update
  if (_ShockAlgoType==DYN_VAL) {
    SIPointPoint *spp, *sppl;
    VECTOR_TYPE v;

    //LEFT BELEMENT
    switch (l_neighbor->type()) {
    case SIElement::POINTPOINT:
      sppl = (SIPointPoint*) l_neighbor;
      ((BPoint_DynVal*)sppl->lBPoint())->pointIV()->updateEnd (sppl->u(), intersection.newLtau, true);
    break;
    }

    //MIDDLE ELEMENT
    switch (current->lBElement()->type()) {
    case BPOINT:
      sppl = (SIPointPoint*) l_neighbor;
      spp = (SIPointPoint*) current;
      v = angle0To2Pi (sppl->u()+M_PI);
      ((BPoint_DynVal*)spp->lBPoint())->pointIV()->updateEnd (angle0To2Pi (sppl->u()+M_PI), spp->u(), false);
        //merge (spp->u(), v);
    break;
    }

    //RIGHT BELEMENT
    switch (current->type()) {
    case SIElement::POINTPOINT:
      spp = (SIPointPoint*) current;
      v = angle0To2Pi (spp->u()+M_PI);
      ((BPoint_DynVal*)spp->rBPoint())->pointIV()->updateEnd (v, intersection.newRtau, false);
    break;
    }
  }
}

void IShock_DynVal::moveSILinksToARightJunction (SILink* current, SILink* r_neighbor, 
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

  //8)DYNVAL: update
  if (_ShockAlgoType==DYN_VAL) {
    SIPointPoint *spp, *sppr;
    VECTOR_TYPE v;

    //LEFT BELEMENT
    switch (current->type()) {
    case SIElement::POINTPOINT:
      spp = (SIPointPoint*) current;
      ((BPoint_DynVal*)spp->lBPoint())->pointIV()->updateEnd (spp->u(), intersection.newLtau, true);
    break;
    }

    //MIDDLE ELEMENT
    switch (current->rBElement()->type()) {
    case BPOINT:
      spp = (SIPointPoint*) current;
      sppr = (SIPointPoint*) r_neighbor;
      v = angle0To2Pi (spp->u()+M_PI);
      ((BPoint_DynVal*)spp->rBPoint())->pointIV()->merge (sppr->u(), v);
    break;
    }

    //RIGHT BELEMENT
    switch (current->type()) {
    case SIElement::POINTPOINT:
      sppr = (SIPointPoint*) r_neighbor;
      v = angle0To2Pi (sppr->u()+M_PI);
      ((BPoint_DynVal*)sppr->rBPoint())->pointIV()->updateEnd (v, intersection.newRtau, false);
    break;
    }
  }

}

bool IShock_DynVal::validateCandidateSource (SISource* source)
{
  BPoint_DynVal *bp1, *bp2;
  Point pt = source->origin ();
  VECTOR_TYPE v;

  switch (source->getBElement1()->type()) {
  case BPOINT:
    bp1 = (BPoint_DynVal*) source->getBElement1();
    v = _vPointPoint (bp1->pt(), pt);
    if (bp1->pointIV()->isInIVS(v))
      return false;

    switch (source->getBElement2()->type()) {
    case BPOINT:
      bp2 = (BPoint_DynVal*) source->getBElement2();
      v = _vPointPoint (bp2->pt(), pt);
      if (bp2->pointIV()->isInIVS(v))
        return false;
    break;
    case BLINE:
    break;
    case BARC:
    break;
    }
  case BLINE:
    break;
  case BARC:
    break;
  }

  return true;
}

