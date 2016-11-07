#include <extrautils/msgout.h>

#include "sym-transforms.h"
#include "ishock.h"
#include "biarc_shock.h"
#include "boundary.h"
//#include <vcl_cmath.h>


//#########################################################################
//     SHOCK PRUNING FUNCTIONS
//#########################################################################

void IShock::Prune(double thresh)
{    
  SIElmListIterator elmPtr = SIElmList.begin();
  for (; elmPtr != SIElmList.end(); elmPtr++) {
    SIElement* current = elmPtr->second;

    if (current->isAnA3Source()){
      //hide it if its child is below threshold
      SIA3Source* A3 = (SIA3Source*) current;

      if (A3->cSLink()->dPnCost() < thresh) A3->hide();
      else A3->unhide();

      continue;
    }

    if (current->isAJunction()){
      //hide it if its child is below threshold
      SIJunct* junct = (SIJunct*) current;

      if (junct->cSLink()->dPnCost() < thresh) junct->hide();
      else junct->unhide();

      continue;
    }

    if (current->isASource()){
      //hide it if any of the children are below threshold
      SISource* src = (SISource*) current;

      if (src->cSLink()->dPnCost()< thresh ||
         src->cSLink2()->dPnCost()< thresh) {
        //hide source
        src->hide();

        //hide both its children
        src->cSLink()->hide();
        src->cSLink2()->hide();
      }
      else {
        src->unhide();
        //hide both its children
        src->cSLink()->unhide();
        src->cSLink2()->unhide();
      }
      continue;
    }

    if (current->isASink()){
      //hide it if all its parents are gone
      SISink* sink = (SISink*) current;
      bool remove = true;

      SILinksList::iterator curS = sink->PSElementList.begin();
      for(; curS!=sink->PSElementList.end(); ++curS){
        if ((*curS)->dPnCost() >= thresh){
          remove = false;
          break;
        }
      }

      if (remove) sink->hide();
      else sink->unhide();
  
      continue;
    }

    // ignore contacts
    if (current->isAContact()){
      //hide it if not already hidden
      continue;
    }

    //the rest must be links
    if (current->isALink()){
      SILink* curLink = (SILink*) current;

      if (! curLink->pSNode()->isASource()){
        if (curLink->dPnCost() < thresh) current->hide();
        else current->unhide();
      }
    }
  }
}

//#########################################################################
//     SYMMETRY TRANSFORM FUNCTIONS
//#########################################################################

#define SYM_TRANS_DEFAULT_THRESHOLD 10//0.1

void IShock::GD_SymTransToThreshold(double thresh)
{
  double threshold;
  if (thresh<0)
    threshold = SYM_TRANS_DEFAULT_THRESHOLD;
  else
    threshold = thresh;

  while (GD_SymTrans_step(threshold));
}

void IShock::GD_SymTrans(long step)
{
  long limit;
  if (step<0)
    limit = GD_step_count+100;
  else
    limit = step;

  //dummy one
  for (int i=GD_step_count; i<limit;i++){
    if (!GD_SymTrans_step(SYM_TRANS_DEFAULT_THRESHOLD))
      break;
  }
}

bool IShock::GD_SymTrans_step(double threshold)
{
  if (GDList.size()==0) {
    vcl_cout << "Gradient Descent Symmetry Transform Finished."<<vcl_endl;
    return false;
  }

  //pick the first entry in the GDlist
  //the entries in the GDList should all be shock links
  assert(GDList.begin()->second->graph_type() == SIElement::LINK);
  SILink* sElm = (SILink*) GDList.begin()->second;

  if (sElm->dPnCost() >= threshold){
    vcl_cout << "Symmetry Transform Threshold Reached."<<vcl_endl;
    return false;
  }
  
  if (boundary()->GetBoundaryLimit() == BIG_RECTANGLE || 
     boundary()->GetBoundaryLimit() == BIG_CIRCLE) {
    if (sElm->lBElement()->id() <=8 ||
      sElm->rBElement()->id() <=8){
      //this shock comes from the boundary() limit
      //so it's time to stop symmetry transform
      vcl_cout <<"Symmetry Transform cannot proceed further!"<<vcl_endl;
      return false;
    }
  }
  
  //keep count of the number of steps
  GD_step_count++;
  vcl_cout <<"Step: "<< GD_step_count <<" ID: "<<sElm->id()<<" vcl_cost: "<<sElm->dPnCost() <<vcl_endl;

  //we need to pick the right transform to use here
  //My current observation is that the shocks that originate
  //directly from SOs are involved in describing gaps 
  //so that is what we will do!

  if (sElm->pSNode()->type() == SIElement::SOURCE){
    remove_gap(sElm);
    //vcl_cout << "Gap Transform"<<vcl_endl;
  }
  else {
    splice(sElm, 0); //testing option 2
    //vcl_cout << "Splice Transform"<<vcl_endl;
  }

  //patch the shock structure after the symmetry transorm
  PatchShocksLocally(REGULAR_ADD_PATCH_INIT);//boundary()->shock->getShockInitType()

  return true;
}

void IShock::recomputeSaliencies()
{
  SIElmListIterator elmPtr = SIElmList.begin();
  for (; elmPtr != SIElmList.end(); elmPtr++) {
    SIElement* elm = elmPtr->second;

    if (elm->isALink() && !elm->isAContact())
      GDList.erase(Sal_IDpair(elm->dPnCost(), elm->id()));

    //2) recompute its saliency
    computeSymTransSalience(elm);

    //3) reinsert it into the GD list
    if (elm->graph_type()==SIElement::LINK &&
       elm->label() != SIElement::CONTACT)
      GDList.insert(
        Sal_SIElmPair(Sal_IDpair(elm->dPnCost(), elm->id()), elm));
  }
}

int IShock::deleteContourSegmentBetween(BElement* SBElm, BElement* EBElm)
{
  BElementList belmsToDel;
  /*
  //1. make bElement delete list by travervcl_sing from the first element to the last element
  //add to the list all the elements that are not the start and end elms
  BElement* curBElm = SBelm;
  if (curBElm->type() == BPOINT){

  }
  while (curBElm != EBelm){
    curBElm= curBElm->e_pt()->getElmToTheRightOf(curBElm);
    if (curBElm != EBelm)
      belmsToDel.push_back(curBElm);
  }
    
  //2. delete all the elements
  BElementListIterator curBE = belmsToDel.begin();
  for (;curBE != belmsToDel.end(); curBE++){
    BElement* bElmToDel = (*curBE);
    boundary()->delGUIElement(bElmToDel);  
  }
  belmsToDel.clear();*/
  return 1;
}

#define LINE_COMP_DIST 2
//This function computes the saliency of gap transforms
//of different types
double IShock::computeGapTransSalience (SIElement* shock)
{
  BPoint *spt, *ept;

  //a source is an indicator of a gap
  //we need to define this salience in terms
  //of the gap completion vcl_cost

  BElement* lbelm = ((SILink*)shock)->lBElement();
  BElement* rbelm = ((SILink*)shock)->rBElement();

  if (lbelm->type() == BPOINT &&
     rbelm->type() == BPOINT){

    spt = (BPoint*) lbelm;
    ept = (BPoint*) rbelm;

    Point cen = ((SIPointPoint*)shock)->getEndPt();
    double R = ((SIPointPoint*)shock)->endTime();
    double d = ((SIPointPoint*)shock)->H();

    if (d < LINE_COMP_DIST){
      shock->setComp_type(SIElement::LINE_COMP);
      return  vnl_math_max (0.0, (d-1)/R);
    }
    
    //svcl_tandard point-point completion salience
    double newL = PointPointCompletion(spt, ept, R, cen, false);

    //there are three kinds of gap completions possible
    //1. pair of points only
    //2. point and point-tangent
    //3. point tangent pair

    SIElement::COMPLETION_TYPE CompletionType;
    double compL;

    //determine which kind this is
    if (spt->isFreePoint() && ept->isFreePoint()){
      if (spt->hasATangent() && ept->hasATangent()){
        CompletionType = SIElement::PT_PT;
        compL = PointTangentPointTangentCompletion(spt, ept, false);

        /* just testing operator length fav
        SIPointPoint* spp = (SIPointPoint*)shock;

        //favor the tangent information and subtract the operator length
        double sL = 1*vcl_fabs(_dot(spt->tangent(), spp->u()));
        double eL = 1*vcl_fabs(_dot(ept->tangent(), spp->u()));
        
        //only for integer points
        compL = newL - (sL+eL);
        compL = std:: vnl_math_max (compL, 0.0);
        */
      }
      else if (spt->hasATangent() || ept->hasATangent()){
        CompletionType = SIElement::P_PT;
        compL = PointPointTangentCompletion(spt, ept, false);
      }
      else{
        CompletionType = SIElement::PP_ONLY;
        //compL = PointPointCompletion(spt, ept, R, cen, false);
        compL=newL;
      }

      shock->setComp_type(SIElement::LINE_COMP);
      return d/R;
    }
    else if ((spt->isFreePoint() && spt->hasATangent()) || 
          (ept->isFreePoint() && ept->hasATangent()) ){
      CompletionType = SIElement::PT_PT;
      compL = PointTangentPointTangentCompletion(spt, ept, false);
    }
    else if ((spt->isFreePoint() && !spt->hasATangent()) || 
          (ept->isFreePoint() && !ept->hasATangent()) ){
      CompletionType = SIElement::P_PT;
      compL = PointPointTangentCompletion(spt, ept, false);
    }
    else {
      CompletionType = SIElement::PT_PT;
      compL = PointTangentPointTangentCompletion(spt, ept, false);

    }

    if (compL>=ISHOCK_DIST_HUGE){
      CompletionType = SIElement::NO_COMP;
      shock->setComp_type(CompletionType);
      return ISHOCK_DIST_HUGE;
    }
    //else if (compL>newL){
      //vcl_cout << "ShockID: "<<shock->id() <<", Biarc length is greater by: " <<compL-newL <<vcl_endl;
    //else {
    //  CompletionType = SIElement::PP_ONLY;
    //  shock->setComp_type(CompletionType);
    //  return newL/R;
    //}
    else {
      shock->setComp_type(CompletionType);
      return compL/R;
    }
  }
  return ISHOCK_DIST_HUGE;
}

//return saliency value for each shock segment...
void IShock::computeSymTransSalience (SIElement* shock)
{
  switch (shock->type()){
    case SIElement::A3SOURCE:
    {
      ((SIA3Source*)shock)->computeSalience();
      break;
    }
    case SIElement::SOURCE:
    {
      ((SISource*)shock)->computeSalience();
      break;
    }
    case SIElement::JUNCT:
    {
      ((SIJunct*)shock)->computeSalience();
      break;
    }
    case SIElement::SINK:
    {
      ((SISink*)shock)->computeSalience();
      break;
    }
    case SIElement::POINTPOINT:
    {
      SIPointPoint* spp = (SIPointPoint*)shock;
      
      if (spp->pSNode()->type()==SIElement::SOURCE){

        //temp just as a test
        bool test = false;
        if (  ((BPoint*)spp->lBElement())->hasATangent() &&
          ((BPoint*)spp->rBElement())->hasATangent() ){
          double dot1 = vcl_fabs(_angle_vector_dot( ((BPoint*)spp->lBElement())->tangent(), spp->u()));
          double dot2 = vcl_fabs(_angle_vector_dot( ((BPoint*)spp->rBElement())->tangent(), spp->u()));

          test = ((dot1 < 0.707) && (dot2 < 0.707) && 
                 (spp->H()>1)); //some random threshold 

              //  && 
              //  ((BPoint*)spp->lBElement())->_bSomething &&
              //  ((BPoint*)spp->rBElement())->_bSomething
        }

        if (1){//test
          //set the source vcl_cost as ISHOCK_DIST_HUGE too
          //will be used later to differentiate the sources
          spp->pSNode()->set_dPnCost(ISHOCK_DIST_HUGE);
          spp->set_dPnCost(ISHOCK_DIST_HUGE);//this one cannot be deleted
          break;
        }

        //this requires a gap transform 
        //Pholosophy 1:
        //the original contour is at best the operator half length
        double dOC = 0;// vnl_math_min (1.0, spp->H())
    
        //philosophy 2:
        spp->set_dPnCost(computeGapTransSalience(spp));
        //assume at least that the gap was closed with a straight line
        spp->set_dNC(spp->H()); 

        //philosophy 3:
        //now in the sense of information available, there should be no
        //direct vcl_cost in performing this replacement because that is the best 
        //we can do here. The vcl_cost should come from another metric of disparity
        //between the actual points and the new curve
        //spp->set_dPnCost(0);

        break;
      }

      spp->computeSalience();
      break;
    }
    case SIElement::POINTLINE:
    {
      SIPointLine* spl = (SIPointLine*)shock;
      if (spl->pSNode()->type()==SIElement::SOURCE){
        //this requires a gap transform
        //for now
        spl->set_dNC(ISHOCK_DIST_HUGE);
        spl->set_dPnCost(ISHOCK_DIST_HUGE);
        break;
      }
      spl->computeSalience();
      break;
    }
    case SIElement::LINELINE:
    {
      SILineLine* sll = (SILineLine*)shock;
      sll->computeSalience();
      break;
    }
    case SIElement::POINTARC:
    {
      SIPointArc* spa = (SIPointArc*)shock;
      if (spa->pSNode()->type()==SIElement::SOURCE){
        //this requires a gap transform
        //for now
        spa->set_dNC(ISHOCK_DIST_HUGE);
        spa->set_dPnCost(ISHOCK_DIST_HUGE);
        break;
      }
      spa->computeSalience();
      break;
    }
    case SIElement::LINEARC:
    {
      SILineArc* sla = (SILineArc*)shock;
      if (sla->pSNode()->type()==SIElement::SOURCE){
        //this requires a gap transform
        //for now
        sla->set_dNC(ISHOCK_DIST_HUGE);
        sla->set_dPnCost(ISHOCK_DIST_HUGE);
        break;
      }
      sla->computeSalience();
      break;
    }
    case SIElement::ARCARC:
    {
      SIArcArc* saa = (SIArcArc*)shock;
      if (saa->pSNode()->type()==SIElement::SOURCE){
        //this requires a gap transform
        //for now
        saa->set_dNC(ISHOCK_DIST_HUGE);
        saa->set_dPnCost(ISHOCK_DIST_HUGE);
        break;
      }
      saa->computeSalience();
      break;
    }
    case SIElement::THIRDORDER:
    {
      SIThirdOrder* sto = (SIThirdOrder*)shock;
      sto->computeSalience();
    }
    case SIElement::ARCTHIRDORDER:
    {
      SIArcThirdOrder* sato = (SIArcThirdOrder*)shock;
      sato->computeSalience();
    }
  }
}

//THIS FUNCTION IS INCOMPLETE
//I decided to abandon it because it caused a lot of other problems
//it is easiest to ignore zero length shocks

//ZERO LENGTH SHOCK TRANSFORM
//need to move all the information from the child node to
//the parent node of the zero length shock
int IShock::zeroLengthShockTransform(SILink* current)
{
  //get the parent and the child nodes
  SINode* pNode = current->pSNode();
  SINode* cNode = current->cSNode();

  //first move the information from the BElementlist of the cNode
  //first the elements after the zero_length shock

  BElementList::iterator cur_cNode_B = cNode->bndList.begin();
  for(; cur_cNode_B!=cNode->bndList.end(); ++cur_cNode_B) {
    if ((*cur_cNode_B) == *(pNode->bndList.begin())) break;
  }
  cur_cNode_B;

  //now the elements before the zero-length shock
  pNode->bndList.splice(pNode->bndList.begin(), cNode->bndList, 
                 cNode->bndList.begin(), cur_cNode_B);

  cur_cNode_B++;cur_cNode_B++;

  pNode->bndList.splice(pNode->bndList.end(), cNode->bndList, 
                 cur_cNode_B, cNode->bndList.end()); 

  return 1;
}

int IShock::delAllBElementsOfParentsToA3(SILink* current)
{
  //we need to clean up all the boundary() elements corresponding to 
  //all the shocks before this shock all the way to the A3
  //SO make a list of all the boundary() elements first

  BElmListType delBList;
  delBList.clear();

  bool A3SourceFound = false;

  SILink* curS = current;
  SINode* curNode;

  while (!A3SourceFound && curS){
    delBList.insert(ID_BElm_pair(curS->lBElement()->id(), curS->lBElement()));
    delBList.insert(ID_BElm_pair(curS->rBElement()->id(), curS->rBElement()));

    curNode = curS->pSNode();

    if (curNode->type() == SIElement::SOURCE)
      return 0; //we are looking for an A3
    else {
      BElementList::iterator curB = curNode->bndList.begin();
      for(; curB!=curNode->bndList.end(); ++curB) {
        delBList.insert(ID_BElm_pair((*curB)->id(), (*curB)));
      }
      if (curNode->type() == SIElement::A3SOURCE){
        A3SourceFound = true;
        break;
      }
    }

    curS = curS->GetParentLink();
    if (curS == NULL){
      //branching structure found::Cannot perform this symmetry transform
      return 0;
    }
  }

  //now that we have found the A3, we can start deleting the bElements

  //before deleting the elements put the belements they are linked to into
  //the tainted list

  if (_ShockInitType==REGULAR_ADD_PATCH_INIT || _ShockInitType==LAGRANGIAN_INIT){
    BElmListIterator curbElm = delBList.begin();
    for (;curbElm!=delBList.end(); curbElm++){
      BElement* belm = curbElm->second;
      BElementList neighboringBElms = belm->getAllNeighboringBElements();
      BElementListIterator curB = neighboringBElms.begin();
      for (; curB!=neighboringBElms.end(); ++curB) {
        boundary()->taintedBElmList.insert(ID_BElm_pair((*curB)->id(), *curB));
      }
    }
  }

  //when boundary() elements start to be deleted, the current shock 
  // could also be deleted with it so save the left and right boundary() elements
  //for the logic
  BElement *lBElm = current->lBElement();
  BElement *rBElm = current->rBElement();

  //delete all the elements
  while (delBList.size()>0){
    BElement* bElmToDel = delBList.begin()->second;
    delBList.erase(delBList.begin());
    //quick hack
    //do not delete the elements that belong to the current shock
    if (bElmToDel != lBElm && bElmToDel != rBElm)
      boundary()->delGUIElement(bElmToDel);  
  }

  return 1; // operation successful
}

#define SYM_TRANS_EPSILON 0.1

//1: splice OK
//-1: operation undefined
//-2: meaningless operation
int IShock::splice(SIElement* current, int option)
{
  Point lfoot, rfoot;
  BPoint *lpoint, *rpoint;
  Point start, end;
  Point center;
  double R;
  int Final_option;

  if (current->label() == SIElement::CONTACT ||
     current->graph_type() == SIElement::NODE)
    return -2;

  switch (current->type()) {
    case SIElement::POINTPOINT:
    {
      SIPointPoint* spp = (SIPointPoint *)current;

      lpoint = (BPoint*)spp->lBElement();
      rpoint = (BPoint*)spp->rBElement();
      center = spp->getEndPt ();
      R = spp->endTime();   //radius of the arc

      BPoint* start_pt = NULL;
      BPoint* end_pt = NULL;

      if (lpoint->isJunctionPoint())  start_pt = lpoint; 
      else                    start = lpoint->pt();

      if (rpoint->isJunctionPoint())  end_pt = rpoint; 
      else                    end = rpoint->pt();

      //delete all the boundary() elements involved all the way to an A3 Source
      if (!delAllBElementsOfParentsToA3(spp))
        return -2;

      if (start_pt == NULL)  start_pt = boundary()->addNonGUIPoint(start.x, start.y);
      if (end_pt == NULL)    end_pt = boundary()->addNonGUIPoint(end.x, end.y);

      //now to add an arc between the points
      boundary()->addGUIArcBetween (start_pt, end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);

      return 1;
      break;
    }
    case SIElement::POINTLINE:
    {
      SIPointLine* spl = (SIPointLine*)current;

      BElement* lBElm = spl->lBElement();
      BElement* rBElm = spl->rBElement();

      //temp points
      Point lstart = lBElm->s_pt()->pt();
      Point lend;
      Point rstart;
      Point rend = rBElm->e_pt()->pt();

      //whether any portion of the left or right elements are remaining
      bool bLBElmRemaining = false;
      bool bRBElmRemaining = false;

      //for biarc completion
      double SAngle, EAngle;

      //First determine if the current option is feasible
      //Some transforms cannot be done because of numerical instability
      //A different option needs to be forced in such situations
      
      Final_option = option;
      if (option==0){
        double LDELTA, RDELTA, Ldelta, Rdelta;

        if (spl->nu()==1){
          //arc on the left
          LDELTA = ISHOCK_DIST_HUGE;//don't care
          RDELTA = spl->ReTau() - spl->RsTau();
          Ldelta = 0;
          Rdelta = spl->l() - (spl->delta() + spl->ReTau());
        }
        else {
          //arc on the right
          LDELTA = spl->LeTau() - spl->LsTau();
          RDELTA = ISHOCK_DIST_HUGE;//don't care
          Ldelta = (spl->delta() - spl->LeTau());
          Rdelta = 0;
        }

        bLBElmRemaining = BisG(Ldelta, 0);//may need to use LisG for consistency
        bRBElmRemaining = BisG(Rdelta, 0);

        //if either the segment to be transformed is small or if a transform
        //leaves behind a small piece of the contour, force it to use the 
        //boundary() neighborhood
        if ((LDELTA < SYM_TRANS_EPSILON) ||
           (RDELTA < SYM_TRANS_EPSILON) ||
           (bLBElmRemaining && Ldelta < SYM_TRANS_EPSILON) ||
           (bRBElmRemaining && Rdelta < SYM_TRANS_EPSILON))
           Final_option = 2; //We need to use boundary() neighborhood here  
      }  

      //having decided on the best transform option, proceed with the specifics 
      if (Final_option==0){
        //svcl_tandard splice
        lfoot = spl->getLFootPt (spl->eTau());
        rfoot = spl->getRFootPt (spl->eTau());

        //the line segments will be cut off at the foot points
        lend = lfoot;
        rstart = rfoot;

        center = spl->getEndPt(); //center of the arc
        R = spl->endTime(); //radius of the arc to be added
      }
      else if (Final_option==1){
        //alternate completion:
        //DELTA neighborhood on the boundary() model
        //
        const double DELTA = 0.5;

        //go DELTA Away from the foot or only up to the end point
        //double lfootTau =  vnl_math_max (spl->eTau() - DELTA, 0);
        //double rfootTau = spl->

        //lfoot  = spl->getLFootPt(spl->eTau());
        //rfoot  = spl->getRFootPt(spl->eTau());

        //lfoot = _vectorPoint(lfoot, lBElm->U() + M_PI, DELTA);
        //rfoot = _vectorPoint(rfoot, rBElm->U(), DELTA);

        //the center and radius if the arc has to be recalculated
        //double thetaE = CCW (spl->ul(),spl->ur());
        //double dR = DELTA/vcl_tan(thetaE/2);
        //double dR = 1;

        //R = spl->endTime() + dR; //radius of the arc to be added
        //center = _vectorPoint(spl->getEndPt(), spl->tangent(spl->eTau()), dR); //center of the arc
      }
      else if (Final_option==2){
        //alternate completion:
        //Boundary Neighborhood splice

        //we can't do a biarc completion for point-line shocks
        //so we have to do a arc completion from the end of the line

        double theta, dir;
        double d = _distPointPoint(lstart, rend);
        if (spl->nu()==1){
          dir = rBElm->TangentAgainstCurveAtPoint(rend);
          theta = CCW(_vPointPoint( rend, lstart), dir);
          R = d/(2*vcl_sin(theta));
          center = _vectorPoint(rend, dir-M_PI/2, R);
        }
        else {
          dir = lBElm->TangentAlongCurveAtPoint(lstart);
          theta = CCW(_vPointPoint( lstart, rend), dir);                  
          R = d/(2*vcl_sin(theta));
          center = _vectorPoint(lstart, dir-M_PI/2, R);
        }

        //change Final option to arc completion
        //but make sure that the element is completely chopped off
        Final_option = 0;
        bLBElmRemaining = false;
        bRBElmRemaining = false;
      }
      else {
        //alternate completion
        //use the tangents of the neighboring elements
        
        //determine the right start and end boundary() tangents
        if (lBElm->s_pt()->isEndPoint())
          SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        else 
          SAngle = lBElm->s_pt()->getElmToTheLeftOf(lBElm)->TangentAlongCurveAtPoint(lstart);
        
        if (rBElm->e_pt()->isEndPoint())
          EAngle = rBElm->TangentAlongCurveAtPoint(rend);
        else 
          EAngle = rBElm->e_pt()->getElmToTheRightOf(rBElm)->TangentAlongCurveAtPoint(rend);
      }


      // Assumption: 
      // We know exactly what kind of modification to be made to 
      // the boundary().

      //First, we delete all the boundary() elements involved all the way to an A3 Source
      //Instead of doing this we could do the same thing as we did for the 2-4 transform
      //follow the contour and delete it instead..
      if (!delAllBElementsOfParentsToA3(spl))
        return -2;

      BPoint *arc_start_pt, *arc_end_pt;  //final modification points

      if (Final_option==0){ //default option: svcl_tandard splice

        //chop the left and right elements to correct lengths
        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, !bLBElmRemaining);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, !bRBElmRemaining);

        //now to add an arc between the points
        boundary()->addGUIArcBetween (arc_start_pt, arc_end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      }
      else if (Final_option==2 || Final_option==3){  
        //Boundary Neighborhood splice

        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, true);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, true);

        //Compute the biarcs:
        //now that we have the points and the appropriate tangents 
        //we can call the biarc completion code
        boundary()->addGUIBiArcBetween(arc_start_pt, SAngle, arc_end_pt, EAngle);
      }

      return 1;
      break;
    }
    case SIElement::LINELINE:
    {
      SILineLine* sll = (SILineLine *)current;

      BLine* lBElm = (BLine*)sll->lBElement();
      BLine* rBElm = (BLine*)sll->rBElement();

      //temp points
      Point lstart = lBElm->s_pt()->pt();
      Point lend;
      Point rstart;
      Point rend = rBElm->e_pt()->pt();

      //whether any portion of the left or right elements are remaining
      bool bLBElmRemaining = false;
      bool bRBElmRemaining = false;

      //for biarc completion
      double SAngle, EAngle;

      //First determine if the current option is feasible
      //Some transforms cannot be done because of numerical instability
      //A different option needs to be forced in such situations
      
      Final_option = option;
      if (option==0){
        double LDELTA = sll->ReTau() - sll->RsTau();
        double RDELATA = LDELTA;
        double Ldelta = sll->LeTau();
        double Rdelta = sll->lR() - sll->ReTau();

        bLBElmRemaining = BisG(Ldelta, 0);//may need to use LisG for consistency
        bRBElmRemaining = BisG(Rdelta, 0);

        //if either the segment to be transformed is small or if a transform
        //leaves behind a small piece of the contour, force it to use the 
        //boundary() neighborhood
        if ((LDELTA < SYM_TRANS_EPSILON) ||
           (bLBElmRemaining && Ldelta < SYM_TRANS_EPSILON) ||
           (bRBElmRemaining && Rdelta < SYM_TRANS_EPSILON))
           Final_option = 2; //We need to use boundary() neighborhood here  
      }  

      //having decided on the best transform option, proceed with the specifics 
      if (Final_option==0){
        //svcl_tandard splice
        lfoot = sll->getLFootPt (sll->eTau());
        rfoot = sll->getRFootPt (sll->eTau());

        //the line segments will be cut off at the foot points
        lend = lfoot;
        rstart = rfoot;

        center = sll->getEndPt(); //center of the arc
        R = sll->endTime(); //radius of the arc to be added
      }
      else if (Final_option==1){
        //alternate completion:
        //DELTA neighborhood on the boundary() model
        //
        const double DELTA = 0.5;

        //go DELTA Away from the foot or only up to the end point
        //double lfootTau =  vnl_math_max (sll->eTau() - DELTA, 0);
        //double rfootTau = sll->

        lfoot  = sll->getLFootPt(sll->eTau());
        rfoot  = sll->getRFootPt(sll->eTau());

        lfoot = _vectorPoint(lfoot, lBElm->U() + M_PI, DELTA);
        rfoot = _vectorPoint(rfoot, rBElm->U(), DELTA);

        //the center and radius if the arc has to be recalculated
        double thetaE = CCW (sll->ul(),sll->ur());
        double dR = DELTA/vcl_tan(thetaE/2);
        //double dR = 1;

        R = sll->endTime() + dR; //radius of the arc to be added
        center = _vectorPoint(sll->getEndPt(), sll->tangent(sll->eTau()), dR); //center of the arc
      }
      else if (Final_option==2){
        //alternate completion:
        //Boundary Neighborhood splice

        //determine the right start and end boundary() tangents
        //for biarc completion
        SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        EAngle = rBElm->TangentAlongCurveAtPoint(rend);
      }
      else {
        //alternate completion
        //use the tangents of the neighboring elements
        
        //determine the right start and end boundary() tangents
        if (lBElm->s_pt()->isEndPoint())
          SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        else 
          SAngle = lBElm->s_pt()->getElmToTheLeftOf(lBElm)->TangentAlongCurveAtPoint(lstart);
        
        if (rBElm->e_pt()->isEndPoint())
          EAngle = rBElm->TangentAlongCurveAtPoint(rend);
        else 
          EAngle = rBElm->e_pt()->getElmToTheRightOf(rBElm)->TangentAlongCurveAtPoint(rend);
      }


      // Assumption: 
      // We know exactly what kind of modification to be made to 
      // the boundary().

      //First, we delete all the boundary() elements involved all the way to an A3 Source
      //Instead of doing this we could do the same thing as we did for the 2-4 transform
      //follow the contour and delete it instead..
      if (!delAllBElementsOfParentsToA3(sll))
        return -2;

      BPoint *arc_start_pt, *arc_end_pt;  //final modification points

      if (Final_option==0){ //default option: svcl_tandard splice

        //chop the left and right elements to correct lengths
        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, !bLBElmRemaining);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, !bRBElmRemaining);

        //now to add an arc between the points
        boundary()->addGUIArcBetween (arc_start_pt, arc_end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      }
      else if (Final_option==2 || Final_option==3){  
        //Boundary Neighborhood splice

        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, true);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, true);

        //Compute the biarcs:
        //now that we have the points and the appropriate tangents 
        //we can call the biarc completion code
        boundary()->addGUIBiArcBetween(arc_start_pt, SAngle, arc_end_pt, EAngle);
      }

      return 1;
      break;
    }
    case SIElement::POINTARC:
    {
      SIPointArc* spa = (SIPointArc*)current;

      BElement* lBElm = spa->lBElement();
      BElement* rBElm = spa->rBElement();

      //temp points
      Point lstart = lBElm->s_pt()->pt();
      Point lend;
      Point rstart;
      Point rend = rBElm->e_pt()->pt();

      //whether any portion of the left or right elements are remaining
      bool bLBElmRemaining = false;
      bool bRBElmRemaining = false;

      //for biarc completion
      double SAngle, EAngle;

      //First determine if the current option is feasible
      //Some transforms cannot be done because of numerical instability
      //A different option needs to be forced in such situations
      
      Final_option = option;
      if (option==0){
        double LDELTA, RDELTA, Ldelta, Rdelta;

        if (spa->nu()==1){
          //arc on the left
          LDELTA = ISHOCK_DIST_HUGE;//don't care
          Ldelta = 0;
          RDELTA = spa->nudr()*spa->Rr()*(spa->RsTau() - spa->ReTau());
          if (spa->nudl() == 1)
            Rdelta = spa->Rr()*CCW(spa->Ae(), spa->u()+M_PI+spa->ReTau());
          else
            Rdelta = spa->Rr()*CCW(spa->u()+M_PI+spa->ReTau(), spa->Ae());
        }
        else {
          //arc on the right
          RDELTA = ISHOCK_DIST_HUGE;//don't care
          Rdelta = 0;
          LDELTA = spa->nudl()*spa->Rl()*(spa->LeTau() - spa->LsTau());
          if (spa->nudl() == 1)
            Ldelta = spa->Rl()*CCW(spa->u()+spa->LeTau(), spa->As());
          else
            Ldelta = spa->Rl()*CCW(spa->As(), spa->u()+spa->LeTau());
        }

        bLBElmRemaining = BisG(Ldelta, 0);//may need to use LisG for consistency
        bRBElmRemaining = BisG(Rdelta, 0);

        //if either the segment to be transformed is small or if a transform
        //leaves behind a small piece of the contour, force it to use the 
        //boundary() neighborhood
        if ((LDELTA < SYM_TRANS_EPSILON) ||
           (RDELTA < SYM_TRANS_EPSILON) ||
           (bLBElmRemaining && Ldelta < SYM_TRANS_EPSILON) ||
           (bRBElmRemaining && Rdelta < SYM_TRANS_EPSILON))
           Final_option = 2; //We need to use boundary() neighborhood here  
      }  

      //having decided on the best transform option, proceed with the specifics 
      if (Final_option==0){
        //svcl_tandard splice
        lfoot = spa->getLFootPt (spa->eTau());
        rfoot = spa->getRFootPt (spa->eTau());

        //the line segments will be cut off at the foot points
        lend = lfoot;
        rstart = rfoot;

        center = spa->getEndPt(); //center of the arc
        R = spa->endTime(); //radius of the arc to be added
      }
      else if (Final_option==1){
        //alternate completion:
        //DELTA neighborhood on the boundary() model
        //
        const double DELTA = 0.5;

        //go DELTA Away from the foot or only up to the end point
        //double lfootTau =  vnl_math_max (spa->eTau() - DELTA, 0);
        //double rfootTau = spa->

        //lfoot  = spa->getLFootPt(spa->eTau());
        //rfoot  = spa->getRFootPt(spa->eTau());

        //lfoot = _vectorPoint(lfoot, lBElm->U() + M_PI, DELTA);
        //rfoot = _vectorPoint(rfoot, rBElm->U(), DELTA);

        //the center and radius if the arc has to be recalculated
        //double thetaE = CCW (spa->ul(),spa->ur());
        //double dR = DELTA/vcl_tan(thetaE/2);
        //double dR = 1;

        //R = spa->endTime() + dR; //radius of the arc to be added
        //center = _vectorPoint(spa->getEndPt(), spa->tangent(spa->eTau()), dR); //center of the arc
      }
      else if (Final_option==2){
        //alternate completion:
        //Boundary Neighborhood spaice

        //we can't do a biarc completion for point-line shocks
        //so we have to do a arc completion from the end of the line

        double theta, dir;
        double d = _distPointPoint(lstart, rend);
        if (spa->nu()==1){
          dir = rBElm->TangentAgainstCurveAtPoint(rend);
          theta = CCW(_vPointPoint( rend, lstart), dir);
          R = d/(2*vcl_sin(theta));
          center = _vectorPoint(rend, dir-M_PI/2, R);
        }
        else {
          dir = lBElm->TangentAlongCurveAtPoint(lstart);
          theta = CCW(_vPointPoint( lstart, rend), dir);                  
          R = d/(2*vcl_sin(theta));
          center = _vectorPoint(lstart, dir-M_PI/2, R);
        }

        //change Final option to arc completion
        //but make sure that the element is completely chopped off
        Final_option = 0;
        bLBElmRemaining = false;
        bRBElmRemaining = false;
      }
      else {
        //alternate completion
        //use the tangents of the neighboring elements
        
        //determine the right start and end boundary() tangents
        if (lBElm->s_pt()->isEndPoint())
          SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        else 
          SAngle = lBElm->s_pt()->getElmToTheLeftOf(lBElm)->TangentAlongCurveAtPoint(lstart);
        
        if (rBElm->e_pt()->isEndPoint())
          EAngle = rBElm->TangentAlongCurveAtPoint(rend);
        else 
          EAngle = rBElm->e_pt()->getElmToTheRightOf(rBElm)->TangentAlongCurveAtPoint(rend);
      }


      // Assumption: 
      // We know exactly what kind of modification to be made to 
      // the boundary().

      //First, we delete all the boundary() elements involved all the way to an A3 Source
      //Instead of doing this we could do the same thing as we did for the 2-4 transform
      //follow the contour and delete it instead..
      if (!delAllBElementsOfParentsToA3(spa))
        return -2;

      BPoint *arc_start_pt, *arc_end_pt;  //final modification points

      if (Final_option==0){ //default option: svcl_tandard splice

        //chop the left and right elements to correct lengths
        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, !bLBElmRemaining);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, !bRBElmRemaining);

        //now to add an arc between the points
        boundary()->addGUIArcBetween (arc_start_pt, arc_end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      }
      else if (Final_option==2 || Final_option==3){  
        //Boundary Neighborhood splice

        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, true);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, true);

        //Compute the biarcs:
        //now that we have the points and the appropriate tangents 
        //we can call the biarc completion code
        boundary()->addGUIBiArcBetween(arc_start_pt, SAngle, arc_end_pt, EAngle);
      }

      return 1;
      break;
    }
    case SIElement::LINEARC:
    {  
      SILineArc* sla = (SILineArc*)current;

      BElement* lBElm = sla->lBElement();
      BElement* rBElm = sla->rBElement();

      //temp points
      Point lstart = lBElm->s_pt()->pt();
      Point lend;
      Point rstart;
      Point rend = rBElm->e_pt()->pt();

      //whether any portion of the left or right elements are remaining
      bool bLBElmRemaining = false;
      bool bRBElmRemaining = false;

      //for biarc completion
      double SAngle, EAngle;

      //First determine if the current option is feasible
      //Some transforms cannot be done because of numerical instability
      //A different option needs to be forced in such situations
      
      Final_option = option;
      if (option==0){
        double LDELTA, RDELTA, Ldelta, Rdelta;

        if (sla->nu()==1){
          //arc on the left
          LDELTA = sla->nud()*sla->R() * (sla->LeTau() - sla->LsTau());
          RDELTA = sla->nud()*(sla->ReTau() - sla->RsTau());

          if (sla->nud()==1){
            Ldelta = sla->R()*CCW(sla->u()+sla->LeTau(), sla->As());
            Rdelta = sla->l() - (sla->delta() + sla->ReTau());
          }
          else {
            Ldelta = sla->R()*CCW(sla->As(), sla->u()+sla->LeTau());
            Rdelta = sla->l() - (sla->delta() - sla->ReTau());
          }
        }
        else {
          //arc on the right
          LDELTA = sla->nud()*(sla->LeTau() - sla->LsTau());
          RDELTA = sla->nud()*sla->R() * (sla->RsTau() - sla->ReTau());

          if (sla->nud()==1){
            Ldelta = (sla->delta() - sla->LeTau());
            Rdelta = sla->R()*CCW(sla->Ae(), sla->u()+sla->ReTau());
          }
          else {
            Ldelta = (sla->delta() + sla->LeTau());
            Rdelta = sla->R()*CCW(sla->u()+sla->ReTau(), sla->Ae());
          }
        }

        bLBElmRemaining = BisG(Ldelta, 0);//may need to use LisG for consistency
        bRBElmRemaining = BisG(Rdelta, 0);

        //if either the segment to be transformed is small or if a transform
        //leaves behind a small piece of the contour, force it to use the 
        //boundary() neighborhood
        if ((LDELTA < SYM_TRANS_EPSILON) ||
           (RDELTA < SYM_TRANS_EPSILON) ||
           (bLBElmRemaining && Ldelta < SYM_TRANS_EPSILON) ||
           (bRBElmRemaining && Rdelta < SYM_TRANS_EPSILON))
           Final_option = 2; //We need to use boundary() neighborhood here  
      }  

      //having decided on the best transform option, proceed with the specifics 
      if (Final_option==0){
        //svcl_tandard splice
        lfoot = sla->getLFootPt (sla->eTau());
        rfoot = sla->getRFootPt (sla->eTau());

        //the line segments will be cut off at the foot points
        lend = lfoot;
        rstart = rfoot;

        center = sla->getEndPt(); //center of the arc
        R = sla->endTime(); //radius of the arc to be added
      }
      else if (Final_option==1){
        //alternate completion:
        //DELTA neighborhood on the boundary() model
        //
        const double DELTA = 0.5;

        //go DELTA Away from the foot or only up to the end point
        //double lfootTau =  vnl_math_max (sla->eTau() - DELTA, 0);
        //double rfootTau = sla->

        //lfoot  = sla->getLFootPt(sla->eTau());
        //rfoot  = sla->getRFootPt(sla->eTau());

        //lfoot = _vectorPoint(lfoot, lBElm->U() + M_PI, DELTA);
        //rfoot = _vectorPoint(rfoot, rBElm->U(), DELTA);

        //the center and radius if the arc has to be recalculated
        //double thetaE = CCW (sla->ul(),sla->ur());
        //double dR = DELTA/vcl_tan(thetaE/2);
        //double dR = 1;

        //R = sla->endTime() + dR; //radius of the arc to be added
        //center = _vectorPoint(sla->getEndPt(), sla->tangent(sla->eTau()), dR); //center of the arc
      }
      else if (Final_option==2){
        //alternate completion:
        //Boundary Neighborhood splice

        //determine the right start and end boundary() tangents
        //for biarc completion
        SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        EAngle = rBElm->TangentAlongCurveAtPoint(rend);
      }
      else {
        //alternate completion
        //use the tangents of the neighboring elements
        
        //determine the right start and end boundary() tangents
        if (lBElm->s_pt()->isEndPoint())
          SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        else 
          SAngle = lBElm->s_pt()->getElmToTheLeftOf(lBElm)->TangentAlongCurveAtPoint(lstart);
        
        if (rBElm->e_pt()->isEndPoint())
          EAngle = rBElm->TangentAlongCurveAtPoint(rend);
        else 
          EAngle = rBElm->e_pt()->getElmToTheRightOf(rBElm)->TangentAlongCurveAtPoint(rend);
      }


      // Assumption: 
      // We know exactly what kind of modification to be made to 
      // the boundary().

      //First, we delete all the boundary() elements involved all the way to an A3 Source
      //Instead of doing this we could do the same thing as we did for the 2-4 transform
      //follow the contour and delete it instead..
      if (!delAllBElementsOfParentsToA3(sla))
        return -2;

      BPoint *arc_start_pt, *arc_end_pt;  //final modification points

      if (Final_option==0){ //default option: svcl_tandard splice

        //chop the left and right elements to correct lengths
        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, !bLBElmRemaining);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, !bRBElmRemaining);

        //now to add an arc between the points
        boundary()->addGUIArcBetween (arc_start_pt, arc_end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      }
      else if (Final_option==2 || Final_option==3){  
        //Boundary Neighborhood splice

        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, true);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, true);

        //Compute the biarcs:
        //now that we have the points and the appropriate tangents 
        //we can call the biarc completion code
        boundary()->addGUIBiArcBetween(arc_start_pt, SAngle, arc_end_pt, EAngle);
      }

      return 1;
      break;
    }
    case SIElement::ARCARC:
    {
      SIArcArc* saa = (SIArcArc*)current;

      BElement* lBElm = saa->lBElement();
      BElement* rBElm = saa->rBElement();

      //temp points
      Point lstart = lBElm->s_pt()->pt();
      Point lend;
      Point rstart;
      Point rend = rBElm->e_pt()->pt();

      //whether any portion of the left or right elements are remaining
      bool bLBElmRemaining = false;
      bool bRBElmRemaining = false;

      //for biarc completion
      double SAngle, EAngle;

      //First determine if the current option is feasible
      //Some transforms cannot be done because of numerical instability
      //A different option needs to be forced in such situations
      
      Final_option = option;
      if (option==0){
        double LDELTA, RDELTA, Ldelta, Rdelta;

        if (saa->Case()==1 || saa->Case()==2 ||
           saa->Case()==5 || saa->Case()==7)
        {
          LDELTA = saa->Rl()*(saa->LeTau() - saa->LsTau());
          Ldelta = saa->Rl()*CCW(saa->u()+saa->LeTau(), saa->AsL());
          RDELTA = saa->Rr()*(saa->RsTau() - saa->ReTau());
          Rdelta = saa->Rr()*CCW(saa->AeR(), saa->u() + M_PI + saa->ReTau());
        }
        else if (saa->Case()==3 ||
              saa->Case()==9 || saa->Case()==12)
        {
          LDELTA = saa->Rl()*(saa->LeTau() - saa->LsTau());
          Ldelta = saa->Rl()*CCW(saa->u()+saa->LeTau(), saa->AsL());
          RDELTA = saa->Rr()*(saa->ReTau() - saa->RsTau());
          Rdelta = saa->Rr()*CCW(saa->u() + M_PI + saa->ReTau(), saa->AeR());
        }
        else if (saa->Case()==4 ||
              saa->Case()==10 || saa->Case()==11)
        {
          LDELTA = saa->Rl()*(saa->LsTau() - saa->LeTau());
          Ldelta = saa->Rl()*CCW(saa->AsL(), saa->u()+saa->LeTau());
          RDELTA = saa->Rr()*(saa->RsTau() - saa->ReTau());
          Rdelta = saa->Rr()*CCW(saa->AeR(), saa->u() + M_PI + saa->ReTau());
        }
        else if (saa->Case()==6)
        {
          LDELTA = saa->Rl()*(saa->LsTau() - saa->LeTau());
          Ldelta = saa->Rl()*CCW(saa->AsL(), saa->u()+saa->LeTau());
          RDELTA = saa->Rr()*(saa->ReTau() - saa->RsTau());
          Rdelta = saa->Rr()*CCW(saa->u() + M_PI + saa->ReTau(),saa->AeR());
        }
        else if (saa->Case()==8 )
        {
          LDELTA = saa->Rl()*(saa->LsTau() - saa->LeTau());
          Ldelta = saa->Rl()*CCW(saa->AeL(), saa->u()+saa->LeTau());
          RDELTA = saa->Rr()*(saa->ReTau() - saa->RsTau());
          Rdelta = saa->Rr()*CCW(saa->u() + M_PI + saa->ReTau(),saa->AsR());
        }

        bLBElmRemaining = BisG(Ldelta, 0);//may need to use LisG for consistency
        bRBElmRemaining = BisG(Rdelta, 0);

        //if either the segment to be transformed is small or if a transform
        //leaves behind a small piece of the contour, force it to use the 
        //boundary() neighborhood
        if ((LDELTA < SYM_TRANS_EPSILON) ||
           (RDELTA < SYM_TRANS_EPSILON) ||
           (bLBElmRemaining && Ldelta < SYM_TRANS_EPSILON) ||
           (bRBElmRemaining && Rdelta < SYM_TRANS_EPSILON))
           Final_option = 2; //We need to use boundary() neighborhood here  
      }  

      //having decided on the best transform option, proceed with the specifics 
      if (Final_option==0){
        //svcl_tandard splice
        lfoot = saa->getLFootPt (saa->eTau());
        rfoot = saa->getRFootPt (saa->eTau());

        //the line segments will be cut off at the foot points
        lend = lfoot;
        rstart = rfoot;

        center = saa->getEndPt(); //center of the arc
        R = saa->endTime(); //radius of the arc to be added
      }
      else if (Final_option==1){
        //alternate completion:
        //DELTA neighborhood on the boundary() model
        //
        const double DELTA = 0.5;

        //go DELTA Away from the foot or only up to the end point
        //double lfootTau =  vnl_math_max (saa->eTau() - DELTA, 0);
        //double rfootTau = saa->

        //lfoot  = saa->getLFootPt(saa->eTau());
        //rfoot  = saa->getRFootPt(saa->eTau());

        //lfoot = _vectorPoint(lfoot, lBElm->U() + M_PI, DELTA);
        //rfoot = _vectorPoint(rfoot, rBElm->U(), DELTA);

        //the center and radius if the arc has to be recalculated
        //double thetaE = CCW (saa->ul(),saa->ur());
        //double dR = DELTA/vcl_tan(thetaE/2);
        //double dR = 1;

        //R = saa->endTime() + dR; //radius of the arc to be added
        //center = _vectorPoint(saa->getEndPt(), saa->tangent(saa->eTau()), dR); //center of the arc
      }
      else if (Final_option==2){
        //alternate completion:
        //Boundary Neighborhood splice

        //determine the right start and end boundary() tangents
        //for biarc completion
        SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        EAngle = rBElm->TangentAlongCurveAtPoint(rend);
      }
      else {
        //alternate completion
        //use the tangents of the neighboring elements
        
        //determine the right start and end boundary() tangents
        if (lBElm->s_pt()->isEndPoint())
          SAngle = lBElm->TangentAlongCurveAtPoint(lstart);
        else 
          SAngle = lBElm->s_pt()->getElmToTheLeftOf(lBElm)->TangentAlongCurveAtPoint(lstart);
        
        if (rBElm->e_pt()->isEndPoint())
          EAngle = rBElm->TangentAlongCurveAtPoint(rend);
        else 
          EAngle = rBElm->e_pt()->getElmToTheRightOf(rBElm)->TangentAlongCurveAtPoint(rend);
      }


      // Assumption: 
      // We know exactly what kind of modification to be made to 
      // the boundary().

      //First, we delete all the boundary() elements involved all the way to an A3 Source
      //Instead of doing this we could do the same thing as we did for the 2-4 transform
      //follow the contour and delete it instead..
      if (!delAllBElementsOfParentsToA3(saa))
        return -2;

      BPoint *arc_start_pt, *arc_end_pt;  //final modification points

      if (Final_option==0){ //default option: svcl_tandard splice

        //chop the left and right elements to correct lengths
        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, !bLBElmRemaining);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, !bRBElmRemaining);

        //now to add an arc between the points
        boundary()->addGUIArcBetween (arc_start_pt, arc_end_pt, center, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      }
      else if (Final_option==2 || Final_option==3){  
        //Boundary Neighborhood splice

        arc_start_pt = boundary()->chopBElement (lBElm, lBElm->s_pt(), lend, true);
        arc_end_pt = boundary()->chopBElement (rBElm, rBElm->e_pt(), rstart, true);

        //Compute the biarcs:
        //now that we have the points and the appropriate tangents 
        //we can call the biarc completion code
        boundary()->addGUIBiArcBetween(arc_start_pt, SAngle, arc_end_pt, EAngle);
      }

      return 1;
      break;
    }
    default:
      return -1;
  }
  return 0;
}

int IShock::experimental_splice(SIElement* current)
{
  return 1;
}

double IShock::PointPointCompletion (BPoint* spt, BPoint* ept, double R, Point cen, bool complete)
{
  if (complete){
    //if (R>MAX_RADIUS){
      //put in a line instead
      boundary()->addGUILineBetween(spt, ept);
    //}
    //else {
    //  boundary()->addGUIArcBetween(spt, ept, cen, R, ARC_NUD_CCW, ARC_NUS_SMALL);
    //}
    return 0;
  }
  else {
    //only compute the salience
    //if (R>MAX_RADIUS){
      return _distPointPoint(spt->pt(), ept->pt());
    //}
    //else {
    //  double theta = CCW(_vPointPoint(cen, spt->pt()), _vPointPoint(cen, ept->pt()));
    //  return theta*R;
    //}
  }
}

double IShock::PointPointTangentCompletion (BPoint* startpt, BPoint* endpt, bool complete)
{
  //get the tangents at one of the points for the arc completion
  double dir, theta, d, R;
  Point cen;

  //local start and end pts for the arc
  BPoint *spt, *ept;

  //these must be end points or junction points
  //DON"T KNOW WHAT TO DO WITH JUNCTION POINTS YET!
  if (startpt->isJunctionPoint() || endpt->isJunctionPoint())
    return ISHOCK_DIST_HUGE;

  if (startpt->isFreePoint() && endpt->isFreePoint()){
    if (startpt->hasATangent()){
      spt = startpt;
      ept = endpt;
    }
    else {
      spt = endpt;
      ept = startpt;
    }
  }
  else {
    if (startpt->isFreePoint()){
      spt = endpt;
      ept = startpt;
    }
    else {
      spt = startpt;
      ept = endpt;
    }
  }

  //compute reference dir for determining correct tangents
  double ref_dir = _vPointPoint(spt->pt(), ept->pt());

  //compute radius and center of arc from spt
  if (spt->hasATangent()){
    if (_angle_vector_dot(ref_dir, spt->tangent())>=0)
      dir = spt->tangent();
    else
      dir = spt->tangent()+M_PI;
  }
  else {
    //get the direction from the connected element
    if (spt->LinkedBElmList.front()->type() == BLINE){
      if ( ((BLine*)spt->LinkedBElmList.front())->s_pt() == spt)
        dir = ((BLine*)spt->LinkedBElmList.front())->U() + M_PI;
      else
        dir = ((BLine*)spt->LinkedBElmList.front())->U();
    }
    else if (spt->LinkedBElmList.front()->type() == BARC){
      if ( ((BArc*)spt->LinkedBElmList.front())->s_pt() == spt)
        dir = ((BArc*)spt->LinkedBElmList.front())->OutTangentAtStartPt();
      else
        dir = ((BArc*)spt->LinkedBElmList.front())->OutTangentAtEndPt();
    }
  }

  d = _distPointPoint(ept->pt(), spt->pt());
  theta = CCW(_vPointPoint( spt->pt(), ept->pt()), dir);
  R = d/(2*vcl_sin(theta));

  cen = _vectorPoint(spt->pt(), dir-M_PI/2, R);

  if (complete){
    if (vcl_fabs(R)>MAX_RADIUS){
      boundary()->addGUILineBetween(spt, ept);
    }
    else {
      //now draw the completing arc
      if (R>0)
        boundary()->addGUIArcBetween(ept, spt, cen, R, ARC_NUD_CCW, ARC_NUS_SMALL);
      else
        boundary()->addGUIArcBetween(spt, ept, cen, -1*R, ARC_NUD_CCW, ARC_NUS_SMALL);
    }
  }
  else {
    if (vcl_fabs(R)>MAX_RADIUS){
      return d;
    }
    else {
      if (R>0){
        double theta = CCW(_vPointPoint(cen, ept->pt()), _vPointPoint(cen, spt->pt()));
        return theta*R;
      }
      else {
        double theta = CCW(_vPointPoint(cen, spt->pt()), _vPointPoint(cen, ept->pt()));
        return vcl_fabs(theta*R);
      }
    }
  }
  return 0;
}

double IShock::PointTangentPointTangentCompletion (BPoint* spt, BPoint* ept, bool complete)
{
  //compute the tangents at the points to compute the BiArc
  double dir_s,dir_e;

  //these must be end points or junction points
  //DON"T KNOW WHAT TO DO WITH JUNCTION POINTS YET!
  if (spt->isJunctionPoint() || ept->isJunctionPoint())
    return HUGE ;

  //compute reference dir for determining correct tangents
  double ref_dir = _vPointPoint(spt->pt(), ept->pt());

  //at the starting point
  if (spt->isFreePoint()){
    if (_angle_vector_dot(ref_dir, spt->tangent())>=0)
      dir_s = angle0To2Pi(spt->tangent());
    else
      dir_s = angle0To2Pi(spt->tangent()+M_PI);
  } 
  else if (spt->LinkedBElmList.front()->type() == BLINE){
    if ( ((BLine*)spt->LinkedBElmList.front())->s_pt() == spt)
      dir_s = angle0To2Pi(((BLine*)spt->LinkedBElmList.front())->U() + M_PI);
    else
      dir_s = angle0To2Pi(((BLine*)spt->LinkedBElmList.front())->U());
  }
  else if (spt->LinkedBElmList.front()->type() == BARC){
    if ( ((BArc*)spt->LinkedBElmList.front())->s_pt() == spt)
      dir_s = angle0To2Pi(((BArc*)spt->LinkedBElmList.front())->OutTangentAtStartPt());
    else
      dir_s = angle0To2Pi(((BArc*)spt->LinkedBElmList.front())->OutTangentAtEndPt());
  }

  //at the end point
  if (ept->isFreePoint()){
    if (_angle_vector_dot(ref_dir, ept->tangent())>=0)
      dir_e = angle0To2Pi(ept->tangent());
    else
      dir_e = angle0To2Pi(ept->tangent()+M_PI);
  }
  else if (ept->LinkedBElmList.front()->type() == BLINE){
    if ( ((BLine*)ept->LinkedBElmList.front())->s_pt() == ept)
      dir_e = angle0To2Pi(((BLine*)ept->LinkedBElmList.front())->U());
    else
      dir_e = angle0To2Pi(((BLine*)ept->LinkedBElmList.front())->U() + M_PI);
  }
  else if (ept->LinkedBElmList.front()->type() == BARC){
    if ( ((BArc*)ept->LinkedBElmList.front())->s_pt() == ept)
      dir_e = angle0To2Pi(((BArc*)ept->LinkedBElmList.front())->InTangentAtStartPt());
    else
      dir_e = angle0To2Pi(((BArc*)ept->LinkedBElmList.front())->InTangentAtEndPt());
  }

  //now that we have the points and the appropriate tangents we can call the biarc completion code
  BiArcShock new_biArc(spt->pt(), dir_s, ept->pt(), dir_e);

  new_biArc.compute_biarc_params();
  new_biArc.compute_other_stuff();

  Point mid, center1, center2;
  double R1, R2;
  int dir1, dir2;
  double l1, l2;

  mid = new_biArc.bi_arc_params.end1;
  center1 = new_biArc.bi_arc_params.center1;
  center2 = new_biArc.bi_arc_params.center2;

  R1 = fabs(new_biArc.bi_arc_params.radius1);
  R2 = fabs(new_biArc.bi_arc_params.radius2);

  dir1 = new_biArc.bi_arc_params.dir1;
  dir2 = new_biArc.bi_arc_params.dir2;

  l1 = new_biArc.bi_arc_params.Length1;
  l2 = new_biArc.bi_arc_params.Length2;

  if (complete){
    if (R1 >= MAX_RADIUS && R2 >= MAX_RADIUS &&
       l1 > 0 && l1 < MAX_RADIUS &&
       l2 > 0 && l2 < MAX_RADIUS ){
      //line completion is better
      boundary()->addGUILineBetween(spt, ept);
    }
    else {
      BPoint* mid_pt = (BPoint*) boundary()->addNonGUIPoint(mid.x, mid.y);

      if (R1 >= MAX_RADIUS &&
         l1 > 0 && l1 < MAX_RADIUS){
        //complete with a line and an an arc
        boundary()->addGUILineBetween (spt, mid_pt);
        Point cen = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, ept->pt().x, ept->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, ept, cen, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
      else if (R2 >= MAX_RADIUS &&
            l2 > 0 && l2 < MAX_RADIUS ){
        //complete with a line and an an arc
        Point cen = getCenterOfArc (spt->pt().x, spt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (spt, mid_pt, cen, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUILineBetween (mid_pt, ept);
      }
      else {
        //now to add an arc between the points
        Point cen1 = getCenterOfArc (spt->pt().x, spt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        Point cen2 = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, ept->pt().x, ept->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (spt, mid_pt, cen1, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, ept, cen2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
    }
  }
  else {
    //simplest possible salience using length only

    if (R1 >= MAX_RADIUS && R2 >= MAX_RADIUS &&
       l1 > 0 && l1 < MAX_RADIUS &&
       l2 > 0 && l2 < MAX_RADIUS ){
      //line completion is better
      return _distPointPoint(spt->pt(), ept->pt());
    }
    else {
      if (R1 >= MAX_RADIUS &&
         l1 > 0 && l1 < MAX_RADIUS){
        return _distPointPoint(spt->pt(), mid) + l2;
      }
      else if (R2 >= MAX_RADIUS &&
            l2 > 0 && l2 < MAX_RADIUS ){
        return _distPointPoint(mid, ept->pt()) + l1;
      }
      else {
        
        double L = (new_biArc.bi_arc_params.Length1 + new_biArc.bi_arc_params.Length2);

        double K_diff = (new_biArc.bi_arc_params.K1 - new_biArc.bi_arc_params.K2)/L;
        double K_avg = (new_biArc.bi_arc_params.K1 + new_biArc.bi_arc_params.K2)/2;

        double K_0 = new_biArc.bi_arc_params.K1;
        double K_L = new_biArc.bi_arc_params.K2;

        double E;
        E = K_diff*K_diff*L*L*L + K_avg*K_avg*L*L*L + L; 

        //E = (K_diff*K_diff*L*L*L/3 + fabs(K_0*K_L*L)); 
        //E = K_diff*K_diff*L;

        //return L;
        //return (1 - exp(-E));

        if (E<0) //some kind of an error usually -INF
          return HUGE;
        else
          return E;
      }
    }
  }

  return 0;
}

//this is called from the gap transform if the points are too close
void IShock::PointPointRegulatoryDelete(BPoint*spt, BPoint*ept, Point start_pt)
{
  double vcl_tan_dir = TANGENT_UNDEFINED;
  //determine which kind this is
  if (spt->isFreePoint() && ept->isFreePoint()){
    if (spt->hasATangent() && ept->hasATangent()){
      //average the tangents
      vcl_tan_dir = (spt->tangent() + ept->tangent())/2;
    }
    else if (spt->hasATangent()) {
      vcl_tan_dir = spt->tangent();
    }
    else if (ept->hasATangent()){
      vcl_tan_dir = ept->tangent();
    }
    boundary()->delGUIElement(spt);
    boundary()->delGUIElement(ept);

    boundary()->addGUIPoint(start_pt.x, start_pt.y, (float)vcl_tan_dir);
  }
  else if (spt->isFreePoint()){
    boundary()->delGUIElement(spt);
  }
  else if (ept->isFreePoint()){
    boundary()->delGUIElement(ept);
  }
  else {
    //can't delete any of them
    //so just connect them with a straight line
    boundary()->addGUILineBetween(spt, ept);
  }
}

#define TOO_CLOSE_DIST 0.1

//1: gap removal OK
//-1: operation undefined
//-2: meaningless operation
int IShock::remove_gap(SIElement* current)
{
  BPoint *spt, *ept;
  BElement *belm1, *belm2;

  if (current->type() == SIElement::SOURCE){
    SISource* gap_source = (SISource*) current;

    belm1 = gap_source->getBElement1();
    belm2 = gap_source->getBElement2();
    
  }
  else if (current->type() == SIElement::POINTPOINT){
    if ( ((SIPointPoint*)current)->pSNode()->isASource()){
      belm1 = ((SIPointPoint*)current)->lBElement();
      belm2 = ((SIPointPoint*)current)->rBElement();
    }
    else
      return -1; //can't handle the others yet
  }
  else
    return -1; //don't know how to handle the other kinds

  //handle degenerate gaps
  if (belm1->type() == BPOINT &&
     belm2->type() == BPOINT){
    
    spt = (BPoint*) belm1;
    ept = (BPoint*) belm2;
    Point cen = ((SIPointPoint*)current)->getEndPt();
    double R = ((SIPointPoint*)current)->endTime();

    //if the completion flag is set to no completion, return
    if (current->comp_type() == SIElement::NO_COMP)
      return 1;

    //due to local computation, some of the saliencies might not
    //be up-to-date especially about junction information
    //so we need to check again for this
    if (spt->isJunctionPoint() || ept->isJunctionPoint())
      return -1;

    //if the points are too close get rid of one of them 
    //and assimilate the information at the two points
    if (spt->isFreePoint() && ept->isFreePoint() &&
       ((SIPointPoint*)current)->H()<TOO_CLOSE_DIST )
    {
      PointPointRegulatoryDelete(spt, ept, ((SIPointPoint*)current)->getStartPt());
      return 1;
    }

    if (_ShockInitType==REGULAR_ADD_PATCH_INIT || BRUTE_FORCE_ADD_PATCH_INIT){
      //we need to add the boundary() elements linked to the two points into
      //the tainted list
      BElementList neighboringBElms = spt->getAllNeighboringBElements();
      BElementList neighborsToMerge = ept->getAllNeighboringBElements();
      neighboringBElms.merge(neighborsToMerge);

      //BElementList::iterator curB = neighboringBElms.begin();
      //for (; curB!=neighboringBElms.end(); ++curB){
      //  BElement* nBElm = (*curB);
      //  boundary()->taintedBElmList.insert(ID_BElm_pair(nBElm->id(), nBElm));
      //}

      //let's also get neighbors of neighbors
      BElementList neighborOfNeighbors;
      //now highlight each of them
      BElementListIterator curB = neighboringBElms.begin();
      for (; curB!=neighboringBElms.end(); ++curB) {
        BElementList neighborsToMerge = (*curB)->getAllNeighboringBElements();
        neighborOfNeighbors.merge(neighborsToMerge);
      }

      //add them all to the tainted list
      BElementList::iterator curNB = neighborOfNeighbors.begin();
      for (; curNB!=neighborOfNeighbors.end(); ++curNB){
        BElement* nBElm = (*curNB);
        boundary()->taintedBElmList.insert(ID_BElm_pair(nBElm->id(), nBElm));
      }
    }

    //it is wise to remove all the contact shocks that this pair of points
    //cause as well. This give the shock propagation a fresh problem
    //and removes the chance that any zero length shocks form
    SIElementList ContactsToDel;
    if (!spt->isFreePoint()){
      BElement::shockListIterator curS = spt->shockList.begin();
      for (; curS!=spt->shockList.end(); ++curS) {
        if ((*curS)->label() == SIElement::CONTACT)
          ContactsToDel.push_back(*curS);
      }
    }
    if (!ept->isFreePoint()){
      BElement::shockListIterator curS = ept->shockList.begin();
      for (; curS!=ept->shockList.end(); ++curS) {
        if ((*curS)->label() == SIElement::CONTACT)
          ContactsToDel.push_back(*curS);
      }
    }
    //finally delete them
    BElement::shockListIterator curS = ContactsToDel.begin();
    for (; curS!=ContactsToDel.end(); ++curS)
      delASIElement(*curS);

    //there are three kinds of gap completions possible
    //1. pair of points only
    //2. point and point-tangent
    //3. point tangent pair

    if (0){
      //purely experimental
      //force pointpoint completion
      PointPointCompletion(spt, ept, R, cen);
    }
    else if (current->comp_type() == SIElement::LINE_COMP){
      //for small gaps in the beginning
      //complete with just a line
      boundary()->addGUILineBetween(spt, ept);
    }
    else if (current->comp_type() == SIElement::PP_ONLY)
      PointPointCompletion(spt, ept, R, cen);
    else if (current->comp_type() == SIElement::P_PT)
      PointPointTangentCompletion(spt, ept);
    else if (current->comp_type() == SIElement::PT_PT)
      PointTangentPointTangentCompletion(spt, ept);
    else {
      //this should only be called from a forced operation usingthe gap completion tool
      PointTangentPointTangentCompletion(spt, ept);//hack
    }

  }
/*
  else if (belm1->type() == BLINE ||
        belm2->type() == BLINE){

    //assuming that this is caused by two lines
    BLine *sline, *oline;

    if (belm1->type() == BPOINT){
      spt = (BPoint*) belm1;
      sline = (BLine*)spt->LinkElmList->be;
      oline = (BLine*) belm2;
    }
    else {
      spt = (BPoint*) belm2;
      sline = (BLine*)spt->LinkElmList->be;
      oline = (BLine*) belm1;
    }

    //compute intersection point of the two lines
    BPoint* ipoint;
          
    //check to see if the intersection point is on the line
    double ttt = _getT(ipoint->pt(), oline->s_pt()->pt() , oline->e_pt()->pt());

    if (ttt>0 && ttt<1 ){
      // if on the line need to break up the line
      BLine* bline1 = (BLine*) boundary()->addGUILineBetween(ipoint, spt);
      if (bline1)
        bline1->setNew (true);
    }
    else {
      //  if not add two line segments
      BPoint* mid_pt = (BPoint*) boundary()->addGUIPoint (ipoint->pt().x, ipoint->pt().y);
      mid_pt->setNew (true);

      BLine* bline1 = (BLine*) boundary()->addGUILineBetween(mid_pt, spt);
      if (bline1)
        bline1->setNew (true);

      BLine* bline2;

      double d1 = _distPointPoint(mid_pt->pt(), oline->s_pt()->pt());
      double d2 = _distPointPoint(mid_pt->pt(), oline->e_pt()->pt());
          
      if (d1>d2)
        bline2 = (BLine*) boundary()->addGUILineBetween(mid_pt, oline->e_pt());
      else
        bline2 = (BLine*) boundary()->addGUILineBetween(mid_pt, oline->s_pt());

      if (bline2)
        bline2->setNew (true);
    }      
  }
  */
  return 1;
}


//old code from gap transform salience
/*
      //compute the tangents at the points to compute the BiArc
      double dir_s,dir_e;
  
      //at the starting point
      if (spt->nLinkedElms()>0 && spt->LinkElmList->be->type() == BLINE){
        if ( ((BLine*)spt->LinkElmList->be)->s_pt() == spt)
          dir_s = ((BLine*)spt->LinkElmList->be)->n + M_PI;
        else
          dir_s = ((BLine*)spt->LinkElmList->be)->n;
      }
      else if (spt->nLinkElm>0 && spt->LinkElmList->be->type() == BARC){
        if ( ((BArc*)spt->LinkElmList->be)->s_pt() == spt){
          if ( ((BArc*)spt->LinkElmList->be)->nud == +1)
            dir_s = ((BArc*)spt->LinkElmList->be)->start_angle + M_PI/2;
          else
            dir_s = ((BArc*)spt->LinkElmList->be)->start_angle - M_PI/2;
        }
        else {
          if ( ((BArc*)spt->LinkElmList->be)->nud == +1)
            dir_s = ((BArc*)spt->LinkElmList->be)->end_angle - M_PI/2;
          else
            dir_s = ((BArc*)spt->LinkElmList->be)->end_angle + M_PI/2;
        }
      }

      //at the end point
      if (ept->nLinkElm>0 && ept->LinkElmList->be->type() == BLINE){
        if ( ((BLine*)ept->LinkElmList->be)->s_pt() == ept)
          dir_e = ((BLine*)ept->LinkElmList->be)->n;
        else
          dir_e = ((BLine*)ept->LinkElmList->be)->n+M_PI;
      }
      else if (ept->nLinkElm>0 && ept->LinkElmList->be->type() == BARC){
        if ( ((BArc*)ept->LinkElmList->be)->s_pt() == ept){
          if ( ((BArc*)ept->LinkElmList->be)->nud == +1)
            dir_e = ((BArc*)ept->LinkElmList->be)->start_angle - M_PI/2;
          else
            dir_e = ((BArc*)ept->LinkElmList->be)->start_angle + M_PI/2;
        }
        else {
          if ( ((BArc*)ept->LinkElmList->be)->nud == +1)
            dir_e = ((BArc*)ept->LinkElmList->be)->end_angle + M_PI/2;
          else
            dir_e = ((BArc*)ept->LinkElmList->be)->end_angle - M_PI/2;
        }
      }

      dir_s = angle0To2Pi(dir_s);
      dir_e = angle0To2Pi(dir_e);

      //for lines and arcs compute biarc
      if (spt->nLinkElm>0  && ept->nLinkElm>0){ 
        BiArcShock new_biArc(spt->pt, dir_s, ept->pt, dir_e);
        new_biArc.compute_biarc_params();

        double L = (new_biArc.bi_arc_params.Length1 + new_biArc.bi_arc_params.Length2);

        double lambda = (new_biArc.bi_arc_params.K1 - new_biArc.bi_arc_params.K2)/L;
        double K_avg = (new_biArc.bi_arc_params.K1 + new_biArc.bi_arc_params.K2)/2;

        double K_0 = new_biArc.bi_arc_params.K1;
        double K_L = new_biArc.bi_arc_params.K2;

        double E;
        E = lambda*lambda*L*L*L + K_avg*K_avg*L*L*L + L; 
        //E = (lambda*lambda*L*L*L/3 + vcl_fabs(K_0*K_L*L)); 
        //E = lambda*lambda*L;

        //shock->dPnScore = 1 - exp(-E);
        shock->dPnScore = E;

        //qDebug("lambda:%.5f K_avg:%.5f L:%.2f E:%.2f",lambda*lambda*L*L*L/3, 
        //  K_0*K_L*L, L, E);

        //shock->dPnScore = (new_biArc.bi_arc_params.Length1 + new_biArc.bi_arc_params.Length2)/2;
        //shock->dPnScore = shock->startTime;
      }
      else //for points
        shock->dPnScore = shock->startTime();
    //}
    //else
    //  shock->dPnScore = ISHOCK_DIST_HUGE;  //for higher order junctions
  }

  else if (belm1->type() == BLINE ||
        belm2->type() == BLINE){

    //assuming that this is caused by two lines

      //compute prune score usingcorner/junction approximation
      BPoint* spt;
      BLine *sline, *oline;

      if (belm1->type() == BPOINT){
        spt = (BPoint*) belm1;
        sline = (BLine*)spt->LinkElmList->be;
        oline = (BLine*) belm2;
      }
      else {
        spt = (BPoint*) belm2;
        sline = (BLine*)spt->LinkElmList->be;
        oline = (BLine*) belm1;
      }

      BPoint* ipoint = boundary()->VmidLineLine(sline, oline);
      if (ipoint->pt == INFINITY_POINT)
        shock->dPnScore = ISHOCK_DIST_HUGE;
      else {
        shock->dPnScore = distPointPoint(spt->pt, ipoint->pt);
      }
  }
  */

int IShock::A4_A2Transform(SISource* src, SISink* vcl_sink, SILink* first_link, SILink* last_link)
{
  //find the shock branch before the source node
  SILink* before_first_link = boundary()->shock->cyclic_adj_succ(first_link, src);

  //find the shock branch on the other side
  SILink* past_last_link = boundary()->shock->cyclic_adj_succ(last_link, vcl_sink);

  //determine the common boundary() element here
  BElement* commonBElm = NULL;
  if (past_last_link->rBElement() == first_link->lBElement())
    commonBElm = first_link->lBElement();
  else if (past_last_link->lBElement() == first_link->rBElement())
    commonBElm = first_link->rBElement();
  else
    commonBElm = NULL; //nothing common

  Point Lspt, Lept, Rspt, Rept;
  double LSAngle, LEAngle, RSAngle, REAngle;
  bool UpdateLeftSide = false;
  bool UpdateRightSide = false;
  BElement *LSBelm, *LEBelm, *RSBelm, *REBelm;
  BElementList belmsToDel;

  //display the possible transform result
  if (first_link->lBElement() == commonBElm || !commonBElm){
    //1. find the right start and end foot points
    //Rspt = first_link->getRFootPt(first_link->sTau());
    //Rept = past_last_link->getLFootPt(past_last_link->eTau());

    Rspt = before_first_link->getLFootPt(before_first_link->eTau());
    Rept = past_last_link->getLFootPt(past_last_link->sTau());

    //2. determine the right start and end boundary() tangents
    RSAngle = first_link->rBElement()->TangentAlongCurveAtPoint(Rspt);
    REAngle = past_last_link->lBElement()->TangentAlongCurveAtPoint(Rept);

    //record the start and end belms that are to be chopped
    RSBelm = first_link->rBElement();
    REBelm = past_last_link->lBElement();

    //make boundary() element delete list by travervcl_sing from the first element to the last element
    //add to the list all the elements that are not the start and end elms

    BElement* curBElm = RSBelm;
    while (curBElm != REBelm){
      curBElm= curBElm->e_pt()->getElmToTheRightOf(curBElm);
      if (curBElm != REBelm)
        belmsToDel.push_back(curBElm);
    }
      
    UpdateRightSide = true;
  }

  if (first_link->rBElement() == commonBElm || !commonBElm) {
    //1. find the left start and end foot points
    //Lspt = first_link->getLFootPt(first_link->sTau());
    //Lept = past_last_link->getRFootPt(past_last_link->eTau());

    Lspt = before_first_link->getRFootPt(before_first_link->eTau());
    Lept = past_last_link->getRFootPt(past_last_link->sTau());

    //2. determine the left start and end boundary() tangents
    LSAngle = first_link->lBElement()->TangentAgainstCurveAtPoint(Lspt);
    LEAngle = past_last_link->rBElement()->TangentAgainstCurveAtPoint(Lept);

    //record the start and end belms that are to be chopped
    LSBelm = first_link->lBElement();
    LEBelm = past_last_link->rBElement();

    //make boundary() element delete list by travervcl_sing from the src to the vcl_sink
    //add to the list all the elements that are not the start and end elms

    BElement* curBElm = LSBelm;
    while (curBElm != LEBelm){
      curBElm= curBElm->s_pt()->getElmToTheLeftOf(curBElm);
      if (curBElm != LEBelm)
        belmsToDel.push_back(curBElm);
    }
    UpdateLeftSide = true;
  }

  //2. delete all the elements
  BElementListIterator curBE = belmsToDel.begin();
  for (;curBE != belmsToDel.end(); curBE++){
    BElement* bElmToDel = (*curBE);
    boundary()->delGUIElement(bElmToDel);  
  }
  belmsToDel.clear();

  //////////////////////////////////////////////////////////////

  //make all the rest of the biarc updates between these points
  if (UpdateRightSide){
    //chop the start and end elements to correct lengths
    BPoint* Rs_pt = boundary()->chopBElement (RSBelm, RSBelm->s_pt(), Rspt);
    BPoint* Re_pt = boundary()->chopBElement (REBelm, REBelm->e_pt(), Rept);

    //3. Compute the two pairs of biarcs
    //now that we have the points and the appropriate tangents we can call the biarc completion code
    BiArcShock new_biArc(Rspt, RSAngle, Rept, REAngle);

    new_biArc.compute_biarc_params();
    new_biArc.compute_other_stuff();

    Point mid, center1, center2;
    double R1, R2;
    int dir1, dir2;
    double l1, l2;

    mid = new_biArc.bi_arc_params.end1;
    center1 = new_biArc.bi_arc_params.center1;
    center2 = new_biArc.bi_arc_params.center2;

    R1 = vcl_fabs(new_biArc.bi_arc_params.radius1);
    R2 = vcl_fabs(new_biArc.bi_arc_params.radius2);

    dir1 = new_biArc.bi_arc_params.dir1;
    dir2 = new_biArc.bi_arc_params.dir2;

    l1 = new_biArc.bi_arc_params.Length1;
    l2 = new_biArc.bi_arc_params.Length2;

    //4. apply the transform

    if (R1 >= MAX_RADIUS && R2 >= MAX_RADIUS){
      //line completion is better
      boundary()->addGUILineBetween(Rs_pt, Re_pt);
    }
    else {
      BPoint* mid_pt = (BPoint*) boundary()->addNonGUIPoint(mid.x, mid.y);

      if (R1 >= MAX_RADIUS){
        //complete with a line and an an arc
        boundary()->addGUILineBetween (Rs_pt, mid_pt);
        Point cen = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, Re_pt->pt().x, Re_pt->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, Re_pt, cen, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
      else if (R2 >= MAX_RADIUS){
        //complete with a line and an an arc
        Point cen = getCenterOfArc (Rs_pt->pt().x, Rs_pt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (Rs_pt, mid_pt, cen, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUILineBetween (mid_pt, Re_pt);
      }
      else {
        //now to add an arc between the points
        Point cen1 = getCenterOfArc (Rs_pt->pt().x, Rs_pt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        Point cen2 = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, Re_pt->pt().x, Re_pt->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (Rs_pt, mid_pt, cen1, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, Re_pt, cen2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
    }
  }
  
  if (UpdateLeftSide){
    //chop the start and end elements to correct lengths
    BPoint* Ls_pt = boundary()->chopBElement (LSBelm, LSBelm->e_pt(), Lspt);
    BPoint* Le_pt = boundary()->chopBElement (LEBelm, LEBelm->s_pt(), Lept);

    //3. Compute the two pairs of biarcs
    //now that we have the points and the appropriate tangents we can call the biarc completion code
    BiArcShock new_biArc(Lspt, LSAngle, Lept, LEAngle);

    new_biArc.compute_biarc_params();
    new_biArc.compute_other_stuff();

    Point mid, center1, center2;
    double R1, R2;
    int dir1, dir2;
    double l1, l2;

    mid = new_biArc.bi_arc_params.end1;
    center1 = new_biArc.bi_arc_params.center1;
    center2 = new_biArc.bi_arc_params.center2;

    R1 = vcl_fabs(new_biArc.bi_arc_params.radius1);
    R2 = vcl_fabs(new_biArc.bi_arc_params.radius2);

    dir1 = new_biArc.bi_arc_params.dir1;
    dir2 = new_biArc.bi_arc_params.dir2;

    l1 = new_biArc.bi_arc_params.Length1;
    l2 = new_biArc.bi_arc_params.Length2;

    //4. apply the transform

    if (R1 >= MAX_RADIUS && R2 >= MAX_RADIUS){
      //line completion is better
      boundary()->addGUILineBetween(Ls_pt, Le_pt);
    }
    else {
      BPoint* mid_pt = (BPoint*) boundary()->addNonGUIPoint(mid.x, mid.y);

      if (R1 >= MAX_RADIUS){
        //complete with a line and an an arc
        boundary()->addGUILineBetween (Ls_pt, mid_pt);
        Point cen = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, Le_pt->pt().x, Le_pt->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, Le_pt, cen, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
      else if (R2 >= MAX_RADIUS){
        //complete with a line and an an arc
        Point cen = getCenterOfArc (Ls_pt->pt().x, Ls_pt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (Ls_pt, mid_pt, cen, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUILineBetween (mid_pt, Le_pt);
      }
      else {
        //now to add an arc between the points
        Point cen1 = getCenterOfArc (Ls_pt->pt().x, Ls_pt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        Point cen2 = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, Le_pt->pt().x, Le_pt->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (Ls_pt, mid_pt, cen1, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
        boundary()->addGUIArcBetween (mid_pt, Le_pt, cen2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
      }
    }
  }

  return 1;
}
  
void IShock::FindAllContourFragments()
{
}

void IShock::FormLoopGraph()
{
}

void IShock::LabelGreenGraph()
{
}


