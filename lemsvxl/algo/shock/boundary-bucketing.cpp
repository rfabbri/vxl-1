#include <extrautils/msgout.h>
#include "ishock.h"
#include "sielements.h"
#include "boundary-bucketing.h"


Boundary_Bucketing::Boundary_Bucketing (BOUNDARY_LIMIT newbndlimit) 
{
  _Initialize(newbndlimit);
}

Boundary_Bucketing::~Boundary_Bucketing ()
{
  if (bucketing) {
    delete bucketing;
    bucketing = NULL;
  }
}

void Boundary_Bucketing::_Initialize (BOUNDARY_LIMIT newbndlimit)
{
  _type = BUCKETING_BOUNDARY;

  bucketing = NULL;
}

void Boundary_Bucketing::Clear()
{
  Boundary::Clear ();

  //clear the buckets, if any
  if (bucketing) {
    delete bucketing;
    bucketing = NULL;
  }
}

bool Boundary_Bucketing::delGUIElement (BElement* elm)
{
  if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE) {
    //BoundaryLimitHack
    //this hack is duplicated in delBelement for safety
    if (elm->id()<=8)
      return false;
  }

  //insert all the neighboring elements of the current gui element into the 
  //tainted list before any of the shocks are deleted
  BElementList neighboringBElms = getAllNeighboringBElementsFromAGUIElement(elm);
  BElementListIterator curB = neighboringBElms.begin();
  for (; curB!=neighboringBElms.end(); ++curB) {
    taintedBElmList.insert(ID_BElm_pair((*curB)->id(), *curB));
  }
  
   switch (elm->type()) {
      case BPOINT:
    {
      if (((BPoint_Bucketing*)elm)->isFreePoint())
        delBElement (elm);
         break;
    }
      case BLINE:
    {
         BLine_Bucketing* bline;
      if (elm->isGUIElm())
        bline = (BLine_Bucketing*)elm;
      else
        bline = ((BLine_Bucketing*)elm)->twinLine();

         if (!bline->s_pt()->isJunctionPoint())
            delBElement (bline->s_pt());
      else {
        //just remove link from the startPt
        bline->s_pt()->disconnectFrom(bline);
        bline->s_pt()->disconnectFrom(bline->twinLine());

        //Bucketing: remove _EpVisCon by this line from the startPt
        ((BPoint_Bucketing*)bline->s_pt())->delEPVisCon (bline->id());
      }

         if (!bline->e_pt()->isJunctionPoint())
            delBElement (bline->e_pt());
      else{
        //just remove link from the endPt
        bline->e_pt()->disconnectFrom(bline);
        bline->e_pt()->disconnectFrom(bline->twinLine());

        //Bucketing: remove _EpVisCon by this line from the endPt
        ((BPoint_Bucketing*)bline->e_pt())->delEPVisCon (bline->id());
      }

      //having removed the links, now delete the lines
         delBElement (bline->twinLine());
         delBElement (bline);
         break;
    }
      case BARC:
    {
         BArc_Bucketing* barc;
      if (elm->isGUIElm())
        barc = (BArc_Bucketing*)elm;
      else
        barc = ((BArc_Bucketing*)elm)->twinArc();
      
         if (!barc->s_pt()->isJunctionPoint())
            delBElement (barc->s_pt());
      else {
        //just remove link from the point
        barc->s_pt()->disconnectFrom(barc);
        barc->s_pt()->disconnectFrom(barc->twinArc());

        //Bucketing: remove _EpVisCon by this line from the startPt
        ((BPoint_Bucketing*)barc->s_pt())->delEPVisCon (barc->id());
      }

         if (!barc->e_pt()->isJunctionPoint())
            delBElement (barc->e_pt());
      else {
        //just remove link from the point
        barc->e_pt()->disconnectFrom(barc);
        barc->e_pt()->disconnectFrom(barc->twinArc());

        //Bucketing: remove _EpVisCon by this line from the endPt
        ((BPoint_Bucketing*)barc->e_pt())->delEPVisCon (barc->id());
      }

      //having removed the links, now delete the arcs
         delBElement (barc->twinArc());
         delBElement (barc);
         break;
    }
   }

   return true;
}

//void Boundary_Bucketing::mergeDuplicatePoints (BPoint* pt1, BPoint* pt2)
//{
//  ((BPoint_Bucketing*)pt1)->mergeWith(pt2);
//}

BPoint* Boundary_Bucketing::addGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent, double conf)
{
  //1)Test if (x, y) is valid...
  if (!isPointValidInWorld(x,y)) {
    //vcl_cout << "BPoint Outside Boundary World! BND_WORLD_SIZE= " << BND_WORLD_SIZE <<vcl_endl;
    //MSGOUT(0);
    return NULL;
  }
  BPoint* newelm;
  newelm = new BPoint_Bucketing (x, y, nextAvailableID(), true, tangent, conf);

   addBElement (newelm);
  update_list.insert(ID_BElm_pair(newelm->id(), newelm));

  //Output points to save for debug...
  //vcl_cout<< "("<<x<<" "<<y<<")\n";
  //MessageOut(2);
   return newelm;
}


BPoint* Boundary_Bucketing::addNonGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent)
{
   BPoint* newelm = new BPoint_Bucketing (x, y, nextAvailableID(), false, tangent);
   addBElement (newelm);
  return newelm;
}

BElement* Boundary_Bucketing::addGUILine (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey)
{
  //if the line is zero length, just add a point
  if (BisEq(sx, ex) && BisEq(sy, ey))
    return addGUIPoint (sx, sy);

  //2)Test if (sx, sy)-(ex, ey) is valid...
  if (!isPointValidInWorld(sx,sy) || !isPointValidInWorld(ex,ey)) {
    vcl_cout << "BLine Outside Boundary World! BND_WORLD_SIZE= " << BND_WORLD_SIZE <<vcl_endl;
    return NULL;
  }

  BPoint_Bucketing* spt = new BPoint_Bucketing (sx, sy, nextAvailableID());
  BPoint_Bucketing* ept = new BPoint_Bucketing (ex, ey, nextAvailableID());
  addBElement (spt);
   addBElement (ept);

   return addGUILineBetween (spt, ept);
}

BElement* Boundary_Bucketing::addGUILineBetween (BPoint* spt, BPoint* ept)
{
  //1)Copy from Boundary::addGUILineBetween
  //if the two boundary points are close, delete one point
  if (_BisEqPoint(spt->pt(), ept->pt())){
    mergeDuplicatePoints(spt, ept);
    delBElement(ept);
    return NULL;
  }

  bool delLBElm=false;
  bool delRBElm=false;

  BPoint* fin_spt = spt;
  BPoint* fin_ept = ept;

  double dtheta;
  double u = _vPointPoint (spt->pt(), ept->pt());
  double lLength = _distPointPoint(spt->pt(), ept->pt());

  BLine *lLine, *rLine;

  double wL = ISHOCK_DIST_HUGE;
  double wR = ISHOCK_DIST_HUGE;

  //check for colinearity here
  if (spt->isEndPoint()){
    if (spt->LinkedBElmList.front()->type() == BLINE){
      lLine = (BLine*) spt->LinkedBElmList.front();
      
      double l, k;
      //if spt is the starting point of this line
      if (lLine->s_pt() == spt){
        dtheta = vcl_fabs(u- angle02Pi(lLine->U()+M_PI));
        l = _distPointPoint(spt->pt(),lLine->e_pt()->pt());
        k = 1/getArcRadiusFromThreePoints(lLine->e_pt()->pt(), spt->pt(), ept->pt());
      }
      else {
        dtheta = vcl_fabs(u- lLine->U());
        l = _distPointPoint(spt->pt(),lLine->s_pt()->pt());
        k = 1/getArcRadiusFromThreePoints(lLine->s_pt()->pt(), spt->pt(), ept->pt());
      }

      //Trying to fit the lines as arc in a box smaller than the accuracy of the input
      wL = l*l*k/8;
    }
  }

  //also check for colinearity on the other side
  if (ept->isEndPoint()){
    if (ept->LinkedBElmList.front()->type() == BLINE){
      rLine = (BLine*)ept->LinkedBElmList.front();
      
      double l, k;
      //if ept is the starting point of this line
      if (rLine->s_pt() == ept){
        dtheta = vcl_fabs(u- rLine->U());
        l = _distPointPoint(ept->pt(),rLine->e_pt()->pt());        
        k = 1/getArcRadiusFromThreePoints(rLine->e_pt()->pt(), ept->pt(), spt->pt());
      }
      else {
        dtheta = vcl_fabs(u- angle02Pi(rLine->U()+M_PI));
        l = _distPointPoint(ept->pt(),rLine->s_pt()->pt());
        k = 1/getArcRadiusFromThreePoints(rLine->s_pt()->pt(), ept->pt(), spt->pt());
      }

      //Trying to fit the lines as arc in a box smaller than the accuracy of the input
      double wR = l*l*k/8;
    }
  }

  //use the smaller scale one for now
  if (wL < W_THRESHOLD && wL <= wR){ //boundary estimation accuracy
    //merge them
    if (lLine->s_pt() == spt){
        fin_spt = lLine->e_pt();
    }
    else {
        fin_spt = lLine->s_pt();
    }

    //delete the connected line
    //delGUIElement(lLine);
    //only mark for deletion
    delLBElm = true;
  }

  if (wR < W_THRESHOLD && wR<wL){ //boundary estimation accuracy
    //merge them
    if (rLine->s_pt() == ept){
        fin_ept = rLine->e_pt();
    }
    else {
        fin_ept = rLine->s_pt();
    }

    //delete the connected line
    //delGUIElement(rLine);
    //only mark for deletion
    delRBElm = true;
  }
  
  //Re-activate existing SIContact to removes the chance of zero length shocks
  if (!fin_spt->isFreePoint()){
    BElement::shockListIterator curS = fin_spt->shockList.begin();
    for (; curS!=fin_spt->shockList.end(); ++curS) {
      if ((*curS)->label() == SIElement::CONTACT) {
        SIContact* contact = (SIContact*)(*curS);
        shock->delASIElement (contact->cSNode());
      }
    }
  }

  if (!fin_ept->isFreePoint()){
    //check for colinearity here
    BElement::shockListIterator curS = fin_ept->shockList.begin();
    for (; curS!=fin_ept->shockList.end(); ++curS) {
      if ((*curS)->label() == SIElement::CONTACT) {
        SIContact* contact = (SIContact*)(*curS);
        shock->delASIElement (contact->cSNode());
      }
    }
  }

  //now put in the new line

   BLine* newelm = new BLine_Bucketing (fin_spt, fin_ept, nextAvailableID(), true); //GUI one
   BLine* twinelm = new BLine_Bucketing (fin_ept, fin_spt, nextAvailableID(), false); //Non-GUI

  //link to one another (best to do this here)
  newelm->_twinLine = twinelm;
  twinelm->_twinLine = newelm;

   addBElement (newelm);
   addBElement (twinelm);

  //only the GUIElement need to be displayed
  update_list.insert(ID_BElm_pair(newelm->id(), newelm));

  //but the points might need to be updated
  if (fin_spt->isGUIElm())
    update_list.insert(ID_BElm_pair(fin_spt->id(), fin_spt));
  if (fin_ept->isGUIElm())
    update_list.insert(ID_BElm_pair(fin_ept->id(), fin_ept));

  //the points are no longer gui elements
  fin_spt->setGUIElm (false);
  fin_ept->setGUIElm (false);

  // delete the old lines now if necessary
  if (delRBElm)    delGUIElement(rLine);
  if (delLBElm)    delGUIElement(lLine);


  //Bucketing: addPPVisCon to the two endPts...
  //Note to add unit vector to it!!

  double l = ((BLine*)newelm)->L();
  ((BPoint_Bucketing*)fin_spt)->addEPVisCon ((fin_ept->pt().x-fin_spt->pt().x)/l, (fin_ept->pt().y-fin_spt->pt().y)/l, newelm->id());
  ((BPoint_Bucketing*)fin_ept)->addEPVisCon ((fin_spt->pt().x-fin_ept->pt().x)/l, (fin_spt->pt().y-fin_ept->pt().y)/l, newelm->id());

   return (BElement*)newelm;

}

BElement* Boundary_Bucketing::addGUILineBetween (BPoint* spt, Point end_pt, BPoint** newEndBPt)
{
  //1)if the line is zero length, return NULL
  if (_BisEqPoint(spt->pt(), end_pt)) {
    *newEndBPt = NULL;
    return NULL;
  }

  BPoint_Bucketing* ept = new BPoint_Bucketing (end_pt.x, end_pt.y, nextAvailableID());
  addBElement (ept);

  *newEndBPt = ept;

  return addGUILineBetween (spt, ept);
}

BElement* Boundary_Bucketing::addGUIArc (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey,
                            COORD_TYPE cx, COORD_TYPE cy, 
                            DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //if the arc is zero length, just add a point
  if (BisEq(sx, ex) && BisEq(sy, ey))
    return addGUIPoint (sx, sy);

  if (r==0)
    return NULL;

  BPoint_Bucketing* spt = new BPoint_Bucketing (sx, sy, nextAvailableID());
  BPoint_Bucketing* ept = new BPoint_Bucketing (ex, ey, nextAvailableID());
  addBElement (spt);
   addBElement (ept);

  BElement* newelm = addGUIArcBetween (spt, ept, Point(cx,cy), r, nud, nus);

   return newelm;
}

BElement* Boundary_Bucketing::addGUIArcBetween (BPoint* spt, BPoint* ept, Point center, 
                                DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{  //if the two boundary points are close, delete one point
  if (_BisEqPoint(spt->pt(), ept->pt())) {
    mergeDuplicatePoints(spt, ept);
    delBElement(ept);
    return NULL;
  }

  if (r> MAX_RADIUS){
    return addGUILineBetween(spt, ept);
  }

  if (nud==ARC_NUD_CW) { //this is to always make the GUI arc the CCW one
    BPoint* temp = ept;
    ept = spt;
    spt = temp;
    nud = ARC_NUD_CCW;
  }

  //Determine if this arc is better suited to be a line
  double v1 = _vPointPoint(center, spt->pt());
  double v2 = _vPointPoint(center, ept->pt());
  double theta = CCW(v1, v2);//because should always be counterclockwise

  //Trying to fit the arc in a box smaller than the accuracy of the input
  theta = theta/2;
  double l = 2*r*vcl_sin(theta);
  double w = l*l/r/8;

  //if (w < W_THRESHOLD){ //boundary estimation accuracy
  //  return addGUILineBetween(spt, ept);
  //}

  //030321 If the arc is a big arc, we break it into two.
  //This will solve a lot of annoying cases later on.
  if (nus==ARC_NUS_LARGE) { //1)Big arc case:
    VECTOR_TYPE svector = _vPointPoint (center, spt->pt());
    VECTOR_TYPE evector = _vPointPoint (center, ept->pt());
    ANGLE_TYPE angle;
    if (nud==ARC_NUD_CCW)
      angle = CCW (svector, evector);
    else
      angle = CCW (evector, svector);
    VECTOR_TYPE mvector;
    if (nud==ARC_NUD_CCW)
      mvector = svector + angle/2;
    else
      mvector = evector + angle/2;
    Point mpoint = _vectorPoint (center, mvector, r);

    //add the midPoint
    BPoint_Bucketing* mpt = new BPoint_Bucketing (mpoint.x, mpoint.y, nextAvailableID());
    addBElement (mpt);

    addGUIArcBetween (mpt, ept, center, r, nud, ARC_NUS_SMALL);
    return addGUIArcBetween (spt, mpt, center, r, nud, ARC_NUS_SMALL);
  }
  else { //2)Small arc case:
    BArc_Bucketing* newelm = new BArc_Bucketing (spt, ept, nextAvailableID(), true, center, r, nud);
    BArc_Bucketing* twinelm = new BArc_Bucketing (ept, spt, nextAvailableID(), false, center, r, (ARC_NUD)(-(int)nud));
  
    //link to one another (best to do this here)
    newelm->_twinArc = twinelm;
    twinelm->_twinArc = newelm;

    addBElement (newelm);
    addBElement (twinelm);

    //only the GUIElement need to be displayed
    update_list.insert(ID_BElm_pair(newelm->id(), newelm));

    //but the points might need to be updated
    if (spt->isGUIElm())
      update_list.insert(ID_BElm_pair(spt->id(), spt));
    if (ept->isGUIElm())
      update_list.insert(ID_BElm_pair(ept->id(), ept));
  
    //the points are no longer gui elements
    spt->setGUIElm (false);
    ept->setGUIElm (false);

    //Bucketing: addPPVisCon to the two endPts...
    //Note that nud is always CCW here!
    //Should use a faster way to avoid vcl_cos and vcl_sin!
    ((BPoint_Bucketing*)spt)->addEPVisCon (vcl_cos(newelm->_InTangent), vcl_sin(newelm->_InTangent), newelm->id());
    ((BPoint_Bucketing*)ept)->addEPVisCon (vcl_cos(newelm->_OutTangent+M_PI), vcl_sin(newelm->_OutTangent+M_PI), newelm->id());
  
    return newelm;
  }
}

BElement* Boundary_Bucketing::addGUIArcBetween (BPoint* bspt, Point ept, Point center, 
                          DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //1)if the line is zero length, return
  if (_BisEqPoint(bspt->pt(), ept)) {
    return NULL;
  }

  BPoint_Bucketing* bept = new BPoint_Bucketing (ept.x, ept.y, nextAvailableID());
  addBElement (bept);

  return addGUIArcBetween (bspt, bept, center, r, nud, nus);
}

void Boundary_Bucketing::clearVisibilityConstraints (void)
{
  BElmListIterator curB = BElmList.begin();
  for (; curB!=BElmList.end(); curB++) {
    BElement* curBElm = (BElement*)(curB->second);
    switch (curBElm->type()) {
    case BPOINT:
    {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) curBElm;
      PPVisConType::iterator ppit = bp->_PPVisCon.begin();
      for (; ppit != bp->_PPVisCon.end(); ppit++)
        delete (*ppit);
      bp->_PPVisCon.clear ();
      ///EPVisConType::iterator epit = bp->_EPVisCon.begin();
      ///for (; epit != bp->_EPVisCon.end(); epit++)
      ///  delete (*epit);
      ///bp->_EPVisCon.clear ();
    }
    break;
    case BLINE:
    {
      BLine_Bucketing* bl = (BLine_Bucketing*) curBElm;
      if (bl->_LPVisCon) {
        delete bl->_LPVisCon;
        bl->_LPVisCon = NULL;
      }
    }
    break;
    case BARC:
    {
      BArc_Bucketing* ba = (BArc_Bucketing*) curBElm;
      if (ba->_APVisCon) {
        delete ba->_APVisCon;
        ba->_APVisCon = NULL;
      }
    }
    break;
    }//end switch
  }//end for
}
