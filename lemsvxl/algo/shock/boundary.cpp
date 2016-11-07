//BOUNDARY.CPP
#include <extrautils/msgout.h>

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_set.h>

#include "ishock-common.h"
#include "boundary.h"
#include "ishock.h"
#include "biarc_shock.h"
#include "CEDT.h"

extern Point INFINITY_POINT;

Boundary::Boundary (BOUNDARY_LIMIT newbndlimit) 
{
  _Initialize(newbndlimit);
}

Boundary::~Boundary ()
{
  clear_contours();

  delete _CEDT;
  delete shock;
}

void Boundary::Clear()
{
  clear_contours();
  delBElementList();

  //BoundaryLimitHack
  if (_BoundaryLimit == NO_LIMIT)
    _nextAvailableID = 0;
  else if (_BoundaryLimit == BIG_RECTANGLE) //4 Lines and 4 Points
    _nextAvailableID = 8;
  else if (_BoundaryLimit == BIG_CIRCLE) //4 Arcs and 4 Points
    _nextAvailableID = 8;

  //clear the shocks from the permanent boundary as well
  shock->Clear();
}

void Boundary::clear_contours()
{
  //delete each contour in the vcl_list
  BContourListIterator curCon = contourList.begin();
  for (; curCon != contourList.end(); curCon++)
    delete *curCon;

  //then clear the vcl_list
  contourList.clear();
}

void Boundary::weedOutShortcontours()
{
  //delete vcl_list
  BContourList saveList;

  //delete each contour in the vcl_list
  BContourListIterator curCon = contourList.begin();
  for (; curCon != contourList.end(); curCon++)
    if ( (*curCon)->elms.size() > 10)
      saveList.push_back(*curCon);
    else
      delete *curCon;

  contourList.clear();

  //put these back
  contourList.merge(saveList);
}

void Boundary::_Initialize (BOUNDARY_LIMIT newbndlimit)
{
  _type = NORMAL_BOUNDARY;
  _BoundaryLimit = newbndlimit;
  _nextAvailableID = 0; //because it will be incremented before being returned

  shock = NULL;

  //preprocesvcl_sing is only need to look for intersecting lines
  //we can detect duplicate points during initialization
  _bIsPreprocesvcl_singNeeded = true;
  _bLoadConAsLine = true;
  _bIsBPointsOnly = true;
  _bIsBLinesOnly = true;

  BElmList.clear();
  update_list.clear();
  contourList.clear();
  taintedBElmList.clear();

  if (_BoundaryLimit == BIG_RECTANGLE)
    addBigRecvcl_tangleToLimitOutsideShocks();
  else if (_BoundaryLimit == BIG_CIRCLE)
    addBigCircleToLimitOutsideShocks();

  _CEDT = NULL;
  ////_CEDT = new CEDT(this, Point2D<int>(0,0), 130, 130); //for most .con files
}

void Boundary::updateBoundaryDrawing (BElement* belm)
{
  //Update current boundary's Drawing
  update_list.insert(ID_BElm_pair(belm->id(), belm)); //update_list.push_back(belm);
}

void Boundary::clearBoundaryLimitElements ()
{
  BElmListIterator curB = BElmList.begin();
  for (; curB!=BElmList.end(); curB++) {
    BElement* curBElm = (BElement*)(curB->second);
    delBElementShockList(curBElm);
    delete curBElm;
  }
  BElmList.clear();

  _nextAvailableID = 0;
}

void Boundary::addBigCircleToLimitOutsideShocks()
{

  //Add circular boundary at a big radius to limit outside shocks
  //only put in the inside arcs

  double cx = BND_WORLD_SIZE/2;  //center of the figure
  double cy = BND_WORLD_SIZE/2;
  double R = 0.75*BND_WORLD_SIZE;  //radius of the circle

  Point center(cx,cy);

  BPoint* top_pt = (BPoint*) addNonGUIPoint(cx, cy-R);
  BPoint* bottom_pt = (BPoint*) addNonGUIPoint(cx, cy+R);
  BPoint* left_pt = (BPoint*) addNonGUIPoint(cx-R, cy);
  BPoint* right_pt = (BPoint*) addNonGUIPoint(cx+R, cy);

  BArc* arc1 = new BArc (left_pt, top_pt, nextAvailableID(), false,
    center, R, ARC_NUD_CCW);
  BArc* arc2 = new BArc (top_pt, right_pt,nextAvailableID(), false,
    center, R, ARC_NUD_CCW);
  BArc* arc3 = new BArc (right_pt, bottom_pt,nextAvailableID(), false,
    center, R, ARC_NUD_CCW);
  BArc* arc4 = new BArc (bottom_pt, left_pt, nextAvailableID(), false,
    center, R, ARC_NUD_CCW);

  //add the new lines
  addBElement (arc1);
  addBElement (arc2);
  addBElement (arc3);
  addBElement (arc4);

  //They need to be displayed
  //update_list.insert(ID_BElm_pair(arc1->id(), arc1));
  //update_list.insert(ID_BElm_pair(arc2->id(), arc2));
  //update_list.insert(ID_BElm_pair(arc3->id(), arc3));
  //update_list.insert(ID_BElm_pair(arc4->id(), arc4));
}

void Boundary::addBigRecvcl_tangleToLimitOutsideShocks()
{
  //Add a recvcl_tangular boundary at a big radius to limit outside shocks
  //only put in the inside lines(i.e., half lines

  double cx = BND_WORLD_SIZE/2;  //center of the figure
  double cy = BND_WORLD_SIZE/2;
  double R =  0.75*BND_WORLD_SIZE;  //half the width of the recvcl_tangle

  BPoint* top_pt = (BPoint*) addNonGUIPoint(cx-R, cy-R);
  BPoint* bottom_pt = (BPoint*) addNonGUIPoint(cx+R, cy+R);
  BPoint* left_pt = (BPoint*) addNonGUIPoint(cx-R, cy+R);
  BPoint* right_pt = (BPoint*) addNonGUIPoint(cx+R, cy-R);

  BLine* line1 = new BLine (left_pt, top_pt, nextAvailableID(), false);
  BLine* line2 = new BLine (top_pt, right_pt, nextAvailableID(), false);
  BLine* line3 = new BLine (right_pt, bottom_pt, nextAvailableID(), false);
  BLine* line4 = new BLine (bottom_pt, left_pt, nextAvailableID(), false);

  //add the new lines
  addBElement (line1);
  addBElement (line2);
  addBElement (line3);
  addBElement (line4);

  //They need to be displayed
  //update_list.insert(ID_BElm_pair(line1->id(), line1));
  //update_list.insert(ID_BElm_pair(line2->id(), line2));
  //update_list.insert(ID_BElm_pair(line3->id(), line3));
  //update_list.insert(ID_BElm_pair(line4->id(), line4));
}

void Boundary::mergeDuplicatePoints(BPoint* pt1, BPoint* pt2)
{
  pt1->mergeWith(pt2);
  //delBElement(pt2);
  //instead of deleting mark it to be deleted
  //pt2->setToBeDeleted();
}

//************************************************************//
//    DELETE OPERATIONS
//************************************************************//

//delete all shocks caused by this boundary element
void Boundary::delBElementShockList (BElement* belm)
{
  assert(belm);

  //go through the ShockList and delete all related shocks
  while (belm->shockList.size()>0){
    BElement::shockListIterator curS = belm->shockList.begin();
    SIElement* selm = (*curS);
    assert(selm->id()>0);
    shock->delASIElement(selm);
  }
}

void Boundary::delBElement (BElement* belm)
{
  if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE) {
    //BoundaryLimitHack
    //do not allow deleting the first eight elements
    //these represent the shock domain boundaries
    if (belm->id()<=8)
      return;
  }

  assert(belm);
  //delete all the shocks caused by this boundary element
  delBElementShockList(belm);
  //Then remove this element from the boundary vcl_list
  BElmList.erase(belm->id());

  //We also need to remove it from the taintedBElmList if it exists there
  taintedBElmList.erase(belm->id());

  //also remove it from the update vcl_list
  update_list.erase(belm->id());

  //and delete the element object
  delete belm; belm=NULL;
}

void Boundary::delBElementList ()
{
  BElmListIterator curB;

  if (_BoundaryLimit == NO_LIMIT) {
    curB = BElmList.begin();
    for (; curB!=BElmList.end(); curB++) {
      BElement* curBElm = (BElement*)(curB->second);
      delBElementShockList(curBElm);
      delete curBElm;
    }
    BElmList.clear();
  }
  else if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE) {
    curB = BElmList.begin();
    for (; curB!=BElmList.end(); curB++) {
      BElement* curBElm = (BElement*)(curB->second);
      if (curBElm->id()>8) {
        delBElementShockList(curBElm);
        delete curBElm;
      }
    }

    //remove the entries of the deleted objects
    //doing it this way increases the speed of deletion
    curB = BElmList.find(8); curB++;
    BElmList.erase(curB, BElmList.end());
  }

  //tainted vcl_list is invalid so clear it too
  update_list.clear();
  taintedBElmList.clear();
}

BElementList Boundary::getAllNeighboringBElementsFromAGUIElement(BElement* GUIElm)
{
  BElementList AllNeighboringBElms;
  BElementList neighboringBElms;

  AllNeighboringBElms.clear();

  switch (GUIElm->type()) {
      case BPOINT:
    {
      neighboringBElms = GUIElm->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();
         break;
    }
      case BLINE:
    {
         BLine* bline = (BLine*)GUIElm;

      neighboringBElms = bline->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();

      if (bline->twinLine()){
        BElementList neighboringBElms = bline->twinLine()->getAllNeighboringBElements();
        AllNeighboringBElms.merge(neighboringBElms);
        neighboringBElms.clear();
      }

      neighboringBElms = bline->s_pt()->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();

      neighboringBElms = bline->e_pt()->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();

         break;
    }
      case BARC:
    {
         BArc* barc = (BArc*)GUIElm;

      neighboringBElms = barc->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();

      if (barc->twinArc()){
        neighboringBElms = barc->twinArc()->getAllNeighboringBElements();
        AllNeighboringBElms.merge(neighboringBElms);
        neighboringBElms.clear();
      }

      neighboringBElms = barc->s_pt()->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();

      neighboringBElms = barc->e_pt()->getAllNeighboringBElements();
      AllNeighboringBElms.merge(neighboringBElms);
      neighboringBElms.clear();
         break;
    }
    default: break;
   }

  return AllNeighboringBElms;
}

//return 1: deletion is successful
//return 0: deletion fails

//we should also make this function call local shock
//detection function to patch up the shock structure
//following the deletion of the elements

bool Boundary::delGUIElement (BElement* elm)
{
  if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE) {
    //BoundaryLimitHack
    //this hack is duplicated in delBelement for safety
    if (elm->id()<=8)
      return false;
  }

  //insert all the neighboring elements of the current gui element into the
  //tainted vcl_list before any of the shocks are deleted
  BElementList neighboringBElms = getAllNeighboringBElementsFromAGUIElement(elm);
  BElementListIterator curB = neighboringBElms.begin();
  for (; curB!=neighboringBElms.end(); ++curB) {
    taintedBElmList.insert(ID_BElm_pair((*curB)->id(), *curB));
  }

   switch (elm->type()) {
      case BPOINT:
    {
      if (((BPoint*)elm)->isFreePoint())
        delBElement (elm);
         break;
    }
      case BLINE:
    {
         BLine* bline;
      if (elm->isGUIElm())
        bline = (BLine*)elm;
      else
        bline = ((BLine*)elm)->twinLine();

         if (!bline->s_pt()->isJunctionPoint())
            delBElement (bline->s_pt());
      else {
        //just remove link from the s_pt()
        bline->s_pt()->disconnectFrom(bline);
        bline->s_pt()->disconnectFrom(bline->twinLine());
      }

         if (!bline->e_pt()->isJunctionPoint())
            delBElement (bline->e_pt());
      else{
        //just remove link from the e_pt()
        bline->e_pt()->disconnectFrom(bline);
        bline->e_pt()->disconnectFrom(bline->twinLine());
      }

      //having removed the links, now delete the lines
         delBElement (bline->twinLine());
         delBElement (bline);
         break;
    }
      case BARC:
    {
         BArc* barc;
      if (elm->isGUIElm())
        barc = (BArc*)elm;
      else
        barc = ((BArc*)elm)->twinArc();

         if (!barc->s_pt()->isJunctionPoint())
            delBElement (barc->s_pt());
      else {
        //just remove link from the point
        barc->s_pt()->disconnectFrom(barc);
        barc->s_pt()->disconnectFrom(barc->twinArc());
      }

         if (!barc->e_pt()->isJunctionPoint())
            delBElement (barc->e_pt());
      else {
        //just remove link from the point
        barc->e_pt()->disconnectFrom(barc);
        barc->e_pt()->disconnectFrom(barc->twinArc());
      }

      //having removed the links, now delete the arcs
         delBElement (barc->twinArc());
         delBElement (barc);
         break;
    }
    default: break;
   }

   return true;
}

void Boundary::moveGUIElement (BElement* belm, COORD_TYPE x, COORD_TYPE y)
{
  switch (belm->type()) {
      case BPOINT:
    {
      ((BPoint*)belm)->moveBPoint (x, y);
         break;
    }
    default: break;
  }
}

//************************************************************//
//    ADD OPERATIONS
//************************************************************//

void Boundary::addBElement (BElement* elm)
{
  BElmList.insert(ID_BElm_pair(elm->id(), elm));
  //also add it to the plagued vcl_list
  taintedBElmList.insert(ID_BElm_pair(elm->id(), elm));
}

BPoint* Boundary::addGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent, double conf)
{

  //1)Test if (x, y) is valid...
  if (!isPointValidInWorld(x,y)) {
    vcl_cout << "BPoint Outside Boundary World! BND_WORLD_SIZE= " << BND_WORLD_SIZE <<vcl_endl;
    return NULL;
  }
  BPoint* newelm;
  newelm = new BPoint (x, y, nextAvailableID(), true, tangent, conf);

   addBElement (newelm);
  update_list.insert(ID_BElm_pair(newelm->id(), newelm));

  //Output points to save for debug...
  //vcl_cout<< "("<<x<<" "<<y<<")\n";
  //MessageOut(2);
   return newelm;
}

BPoint* Boundary::addNonGUIPoint (COORD_TYPE x, COORD_TYPE y, INPUT_TAN_TYPE tangent)
{
   BPoint* newelm = new BPoint (x, y, nextAvailableID(), false, tangent);
   addBElement (newelm);
  return newelm;
}

BElement* Boundary::addGUILine (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey)
{
  //Bad: this will add a GUI BPoint to the end of a BLine.
  //if the line is zero length, just add a point
  if (BisEq(sx, ex) && BisEq(sy, ey)) {
    assert (0);
    vcl_cout << "BLine becomes a BPoint" <<vcl_endl;
    return NULL;
    ///return addGUIPoint(sx, sy);
  }

  //2)Test if (sx, sy)-(ex, ey) is valid...
  if (!isPointValidInWorld(sx,sy) || !isPointValidInWorld(ex,ey)) {
    vcl_cout << "BLine Outside Boundary World! BND_WORLD_SIZE= " << BND_WORLD_SIZE <<vcl_endl;
    return NULL;
  }

  BPoint* spt = new BPoint (sx, sy, nextAvailableID());
  BPoint* ept = new BPoint (ex, ey, nextAvailableID());
  addBElement (spt);
   addBElement (ept);

   return addGUILineBetween(spt, ept);
}

BElement* Boundary::addGUILine (Point start_pt, Point end_pt)
{
  return addGUILine(start_pt.x, start_pt.y, end_pt.x, end_pt.y);
}

BElement* Boundary::addGUILineBetween (BPoint* spt, Point end_pt, BPoint** newEndBPt)
{
  //1)if the line is zero length, return NULL
  if (_BisEqPoint(spt->pt(), end_pt)) {
    *newEndBPt = NULL;
    return NULL;
  }

  BPoint* ept = new BPoint (end_pt.x, end_pt.y, nextAvailableID());
  addBElement (ept);

  *newEndBPt = ept;

  return addGUILineBetween(spt, ept);
}

BElement* Boundary::addGUILineBetween (Point start_pt, BPoint* ept, BPoint** newStartBPt)
{
  //1)if the line is zero length, return
  if (_BisEqPoint(start_pt, ept->pt())) {
    *newStartBPt = NULL;
    return NULL;
  }

  BPoint* spt = new BPoint (start_pt.x, start_pt.y, nextAvailableID());
  addBElement (spt);

  *newStartBPt = spt;

  return addGUILineBetween(spt, ept);
}

BElement* Boundary::addGUILineBetween (BPoint* spt, BPoint* ept)
{
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

   BLine* newelm = new BLine (fin_spt, fin_ept, nextAvailableID(), true); //GUI one
   BLine* twinelm = new BLine (fin_ept, fin_spt, nextAvailableID(), false); //Non-GUI

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

   return (BElement*)newelm;
}

void Boundary::addPolyLineBetween (BPoint* spt, BPoint* ept, vcl_vector<Point> pts, int id)
{
  BPoint* prev_bpoint = spt;
  BPoint* next_bpoint = NULL;

  for (int i=0; i<pts.size(); i++)
  {
    BElement* bl = addGUILineBetween(prev_bpoint, Point(pts[i].x, pts[i].y), &next_bpoint);
    bl->set_edge_id(id);

    //Note that the new next_bpoint might be NULL, due to tiny sample point that's less than epsilon.
    if (next_bpoint!=NULL)
      prev_bpoint = next_bpoint;
  }

  //final line segment
  BElement* bl = addGUILineBetween(prev_bpoint, ept);
  bl->set_edge_id(id);

}

BElement* Boundary::addGUIArc(COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey,
                    COORD_TYPE cx, COORD_TYPE cy,
                    DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //if the arc is zero length, just add a point
  if (BisEq(sx, ex) && BisEq(sy, ey)) {
    assert (0);
    vcl_cout << "BArc becomes a BPoint" <<vcl_endl;
    return NULL;
    //return addGUIPoint(sx, sy);
  }

  if (r==0)
    return NULL;

  BPoint* spt = new BPoint (sx, sy, nextAvailableID());
  BPoint* ept = new BPoint (ex, ey, nextAvailableID());
  addBElement (spt);
   addBElement (ept);

  BElement* newelm = addGUIArcBetween (spt, ept, Point(cx,cy), r, nud, nus);

   return newelm;
}

BElement* Boundary::addGUIArcBetween (BPoint* spt, BPoint* ept, Point center,
                          DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //if the two boundary points are close, delete one point
  if (_BisEqPoint(spt->pt(), ept->pt())){
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
    BPoint* mpt = new BPoint (mpoint.x, mpoint.y, nextAvailableID());
    addBElement (mpt);

    addGUIArcBetween (mpt, ept, center, r, nud, ARC_NUS_SMALL);
    return addGUIArcBetween (spt, mpt, center, r, nud, ARC_NUS_SMALL);
  }
  else { //2)Small arc case:
    BArc* newelm = new BArc (spt, ept, nextAvailableID(), true, center, r, nud);
    BArc* twinelm = new BArc (ept, spt, nextAvailableID(), false, center, r, (ARC_NUD)(-(int)nud));

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

    return newelm;
  }
}

BElement* Boundary::addGUIArcBetween (Point spt, BPoint* bept, Point center,
                       DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //1)if the arc is zero length, return
  if (_BisEqPoint(spt, bept->pt())) {
    return NULL;
  }

  BPoint* bspt = new BPoint (spt.x, spt.y, nextAvailableID());
  addBElement (bspt);

  return addGUIArcBetween (bspt, bept, center, r, nud, nus);
}

BElement* Boundary::addGUIArcBetween (BPoint* bspt, Point ept, Point center,
                       DIST_TYPE r, ARC_NUD nud, ARC_NUS nus)
{
  //1)if the line is zero length, return
  if (_BisEqPoint(bspt->pt(), ept)) {
    return NULL;
  }

  BPoint* bept = new BPoint (ept.x, ept.y, nextAvailableID());
  addBElement (bept);

  return addGUIArcBetween (bspt, bept, center, r, nud, nus);
}

void Boundary::addGUIBiArcBetween (BPoint* arc_start_pt, double SAngle,
                        BPoint* arc_end_pt, double EAngle)
{
  //Compute the biarcs:
  //now that we have the points and the appropriate tangents
  //we can call the biarc completion code
  BiArcShock new_biArc(arc_start_pt->pt(), SAngle, arc_end_pt->pt(), EAngle);
  new_biArc.compute_biarc_params();
  new_biArc.compute_other_stuff();

  Point mid = new_biArc.bi_arc_params.end1;
  double R1 = vcl_fabs(new_biArc.bi_arc_params.radius1);
  double R2 = vcl_fabs(new_biArc.bi_arc_params.radius2);
  int dir1 = new_biArc.bi_arc_params.dir1;
  int dir2 = new_biArc.bi_arc_params.dir2;
  double l1 = new_biArc.bi_arc_params.Length1;
  double l2 = new_biArc.bi_arc_params.Length2;

  BPoint* mid_pt = (BPoint*) addNonGUIPoint(mid.x, mid.y);
  Point cen1 = getCenterOfArc (arc_start_pt->pt().x, arc_start_pt->pt().y, mid_pt->pt().x, mid_pt->pt().y, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
  Point cen2 = getCenterOfArc (mid_pt->pt().x, mid_pt->pt().y, arc_end_pt->pt().x, arc_end_pt->pt().y, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
  addGUIArcBetween (arc_start_pt, mid_pt, cen1, R1, (ARC_NUD) dir1, ARC_NUS_SMALL);
  addGUIArcBetween (mid_pt, arc_end_pt, cen2, R2, (ARC_NUD) dir2, ARC_NUS_SMALL);
}

BPoint* Boundary::chopBElement(BElement* belm, BPoint* StartPt, Point EndPt, bool chopCompletely)
{
  BPoint *start_pt, *end_pt;

  switch (belm->type()){
    case BPOINT:
    {
      return (BPoint*)belm;
      break;
    }
    case BLINE:
    {
      BLine* bline = (BLine*) belm;

      //is it linked to other elements?
      if (StartPt->isJunctionPoint())
        start_pt = StartPt;
      else
        start_pt = addNonGUIPoint(StartPt->pt().x, StartPt->pt().y);

      //delete the original line
      delGUIElement(bline);

      if (chopCompletely){
        end_pt = start_pt;
      }
      else {
        //add the chopped piece
        end_pt = addNonGUIPoint(EndPt.x, EndPt.y);
        addGUILineBetween(start_pt, end_pt);
      }
      break;
    }
    case BARC:
    {
      BArc* barc = (BArc*) belm;
      bool bchopEnd = (StartPt==barc->s_pt());

      //is it linked to other elements?
      if (StartPt->isJunctionPoint())
        start_pt = StartPt;
      else
        start_pt = addNonGUIPoint(StartPt->pt().x, StartPt->pt().y);

      //save the other parameters of the arc
      Point cen = barc->center();
      double R = barc->R();
      ARC_NUD nud = barc->nud();

      //delete the original line
      delGUIElement(barc);

      if (chopCompletely){
        end_pt = start_pt;
      }
      else {
        //add the chopped piece
        end_pt = addNonGUIPoint(EndPt.x, EndPt.y);

        if (bchopEnd)
          addGUIArcBetween(start_pt, end_pt, cen, R, nud, ARC_NUS_SMALL);
        else
          addGUIArcBetween(end_pt, start_pt, cen, R, nud, ARC_NUS_SMALL);
      }
      break;
    }
    default: break;
  }

  //it is wise to remove all the contact shocks that this pair of points
  //cause as well. This give the shock propagation a fresh problem
  //and removes the chance that any zero length shocks form
  SIElementList ContactsToDel;
  BElement::shockListIterator curS = start_pt->shockList.begin();
  for (; curS!=start_pt->shockList.end(); ++curS) {
    if ((*curS)->label() == SIElement::CONTACT)
      ContactsToDel.push_back(*curS);
  }

  //finally delete them
  for (curS = ContactsToDel.begin(); curS!=ContactsToDel.end(); ++curS)
    shock->delASIElement(*curS);

  return end_pt;
}

BPoint* Boundary::doesBPointExist (BPoint* bpoint)
{
  BPoint *bp1, *bp2;
  BElmListIterator i, temp;
  BPoint *returnPoint = NULL;

   //1)C(N,2): test if e_pt() in the same position
   for (i=BElmList.begin(); i!=BElmList.end(); i++) {
    if ((i->second)->type()==BPOINT) {
      bp1 = (BPoint*)(i->second);
      bp2 = (BPoint*)bpoint;
      if (_BisEqPoint(bp1->pt(), bp2->pt()))
        returnPoint = bp1;
    }
   }
  return returnPoint;
}

void Boundary::PreProcessBoundary (void)
{
  BPoint *bp1, *bp2;

  vcl_cout<< "Boundary Preprocessing\n";

  BElmListIterator i, j, temp;

  BElmVectorType BElmToBeDeleted;

   //1)C(N,2): test if e_pt() in the same position
  //  The iterator temp is required for immediate deletion!
   for (i=BElmList.begin(); i!=BElmList.end(); i++) {
    j=i; j++;
    switch ((i->second)->type()) {
    case BPOINT:
      bp1 = (BPoint*)(i->second);
      for (;j!=BElmList.end(); j++) {
        switch ((j->second)->type()) {
        case BPOINT:
          bp2 = (BPoint*)(j->second);
          if (_BisEqPoint(bp1->pt(), bp2->pt())) {
            //merge bp2 into bp1 and delete bp2
            mergeDuplicatePoints(bp1, bp2);
            temp=j; temp++;
            //add this point to the deleted point vcl_list
            BElmToBeDeleted.push_back (bp2);
            delBElement(bp2);
            temp--; j=temp;
          }
        default: break;
        }//end switch
      }//end for j
      default: break;
      }//end switch
   }//end for i

  //2)Go through the vcl_list again to remove them from BElmList
  BElmVectorType::iterator it = BElmToBeDeleted.begin();
  for (; it!=BElmToBeDeleted.end(); ++it) {
    BElmList.erase ((*it)->id());
  }

  //3)
  _bIsPreprocesvcl_singNeeded = false;
}

void Boundary::PreProcessBPoint (BPoint* bp)
{
  BPoint *bp1;
  BElmListIterator i, temp;

   //1)C(N,2): test if e_pt() in the same position
   for (i=BElmList.begin(); i!=BElmList.end(); i++) {
      switch ((i->second)->type()) {
    case BPOINT:
      bp1 = (BPoint*)(i->second);
      if (bp1->id()==bp->id())
        continue;
      //======================================
      //Merge the EndPts of a Corner
      if (_BisEqPoint(bp->pt(), bp1->pt())) {
        //merge bp2 into bp1 and delete bp2
        mergeDuplicatePoints (bp, bp1);
        i++;
        delBElement(bp1);
        i--;
      }
      default: break;
    }//end switch
   }//end for i
}

void Boundary::PreProcessGUIElement (BElement* GUIElm)
{
  switch (GUIElm->type()) {
  case BPOINT:
    PreProcessBPoint ((BPoint*)GUIElm);
  break;
  case BLINE:
    PreProcessBPoint (((BLine*)GUIElm)->s_pt());
    PreProcessBPoint (((BLine*)GUIElm)->e_pt());
  break;
  case BARC:
    PreProcessBPoint (((BArc*)GUIElm)->s_pt());
    PreProcessBPoint (((BArc*)GUIElm)->e_pt());
  break;
  default: break;
  }
}

void Boundary::PreProcessBoundaryForEdgeInput(double position_accuracy, double angle_accuracy,
    double operator_width, double operator_length)
{
  //this function is trying to weed out redundant edges that subpixel edge detectors like
  //L/L often output. These edges unnecessarily increase the complexity of the grouping
  //process and often times crashes the DT code

  //0. Remove edge of the image responses.
  //1. if two edges are really close, then replace them with an edge that is in between them
  //2. Definition of really close:
  //      a. if tangents are within angle_accuracy, and
  //        i. if they are seperated in the normal direction by less than the
  //          position accuracy
  //        ii. if they are seperated in the tangential direction by less than
  //          the position accuracy
  //3. If an edge is close to two edges that are not close to one another then remove it.
  //4. Definition of close:
  //      a. If the tangents agree within the angle inaccuracy and the edge in the
  //        middle is less than half the operator length away from both the edges.

  //To make it a less than O(N^2) operation, let's bin the points first

  vcl_cout << "**********************************************" <<vcl_endl;
  vcl_cout << "Edge Results Preprocessing for DT" << vcl_endl;
  vcl_cout << "**********************************************" <<vcl_endl;

  //long sec1 = clock();

  vcl_list<int> ** ImgBins;
  ImgBins = new vcl_list<int> *[1001];
  for (int j=0; j<1001;j++)
    ImgBins[j] = new vcl_list<int>[1001];

  //long sec2 = clock();

  //vcl_cout << "init Bins Time: "<<sec2-sec1<<" msec."<<vcl_endl;
  vcl_cout << "Binning the Edge Points" << vcl_endl;

  //keep the elements to delete in this vcl_set
  vcl_set<int> elmsToDel;

  BElmListIterator i=BElmList.begin();
   for (; i!=BElmList.end(); i++) {
    BPoint* bp = (BPoint*)(i->second);
    int x =  (int)vcl_floor (bp->pt().x);
    int y =  (int)vcl_floor (bp->pt().y);

    if (x>=0 && y>=0){
      ImgBins[x][y].push_back(bp->id());

      //if the point is close to another grid location by
      //less than the position accuracy, include it in the
      //neighboring grids too

      if ((bp->pt().x - x)< position_accuracy && (x-1)>=0){
        ImgBins[x-1][y].push_back(bp->id());
      }

      if (x+1 - (bp->pt().x )< position_accuracy && (x+1)<1001){
        ImgBins[x+1][y].push_back(bp->id());
      }

      if ((bp->pt().y - y)< position_accuracy && (y-1)>=0){
        ImgBins[x][y-1].push_back(bp->id());
      }

      if ((y+1 - bp->pt().y)< position_accuracy && (y+1)<1001){
        ImgBins[x][y+1].push_back(bp->id());
      }
    }
    else
      elmsToDel.insert(bp->id());
  }
  //long sec3 = clock();

  vcl_cout << "Done Binning." << vcl_endl;
  //vcl_cout << "Fill Bins Time: "<<sec3-sec2<<" msec."<<vcl_endl;

  //Now the search space should be limited to a neighborhood of 9 bins only
  //let's do each bin seperately first

  for (int x=0; x<1000; x++){
    for (int y=0; y<1000; y++){
      if (ImgBins[x][y].size()>1){
        vcl_list<int>::iterator m, n;
        for (m = ImgBins[x][y].begin(); m!=ImgBins[x][y].end(); m++){
          n=m; n++;
          for (;n!=ImgBins[x][y].end(); n++){
            BPoint* bp1 = (BPoint*)BElmList[(*m)];
            BPoint* bp2 = (BPoint*)BElmList[(*n)];

            double a1 = bp1->tangent();
            double a2 = bp2->tangent();

            //do the angles agree?
            if (_angle_vector_dot(a1,a2) > vcl_cos(2*angle_accuracy)){
              //are they really close?
              double dist = _distPointPoint(bp1->pt(), bp2->pt());
              if (dist <= position_accuracy){//dist <operator_length/2
                //hide the second point
                //they should both be replaced by another point really
                elmsToDel.insert(bp2->id());
              }
            }
          }
        }
      }
    }
  }

  //long sec4 = clock();
  //vcl_cout << "Deciding duplicates Time: "<<sec4-sec3<<" msec."<<vcl_endl;

  //delete all the marked elements
  vcl_set<int>::iterator k=elmsToDel.begin();
   for (; k!=elmsToDel.end(); k++) {
    int ID = *k;
    BPoint* curBElm = (BPoint*)BElmList[ID];

    //curBElm->_bSomething = false; //for visualizing
    //update_list.insert(ID_BElm_pair(curBElm->id(), curBElm));

    delBElement(BElmList[ID]);
  }

  //long sec5 = clock();
  //vcl_cout << "Deleting duplicates Time: "<<sec5-sec4<<" msec."<<vcl_endl;

  //delete the bins
  for (int x=0; x<1001;x++){
    for (int y=0;y<1001;y++){
      ImgBins[x][y].clear();
    }
    delete []ImgBins[x];
  }
  delete []ImgBins;

  //long sec6 = clock();
  vcl_cout << "Done Cleaning." << vcl_endl;
  //vcl_cout << "Deleting Bins Time: "<<sec6-sec5<<" msec."<<vcl_endl;

  //long sec7 = clock();

  /*
  //allocate memory for DT
  int num_of_points = BElmList.size();
  site_struct *sites = new site_struct[num_of_points];

  x=0;
  i=BElmList.begin();
   for (; i!=BElmList.end(); i++) {
    BPoint* bp = (BPoint*)(i->second);
    sites[x].x = bp->pt().x;
    sites[x].y = bp->pt().y;
    x++;
  }

  long sec8 = clock();
  vcl_cout << "Setting up DT structures Time: "<<sec8-sec7<<" msec."<<vcl_endl;

  edge_ref e;
  e = delaunay_build (sites, num_of_points);
  long sec9 = clock();

  //quad_enum(e, NULL, NULL);

  long sec10 = clock();
  vcl_cout << "Initilization Time: "<<sec9-sec8<<" msec."<<vcl_endl;
  vcl_cout << "Traversal Time: "<<sec10-sec9<<" msec."<<vcl_endl;

  //delete the DT structure
  delete []sites;
*/

}

//Set Range of Influence of Shocks caused by the Boundary element...
void Boundary::setRangeOfInfluence (BElement* elm)
{
/*
   SLink* slist = elm->SElementList;

   //go through the vcl_list and vcl_set bShow to GUI_RANGE_OF_INFLUENCE...
   while (slist != 0) {
      slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
      //if (elm->type()!=BPOINT)
      //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
      slist = slist->next;
   }

   //TWIN GUY...
   switch (elm->type()) {
      case BPOINT:
           break;
      case BLINE:

           //twinLine
           slist = ((BLine*)elm)->twinLine->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           //endPoint 1
           slist = ((BLine*)elm)->e_pt()[0]->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           //endPoint 2
           slist = ((BLine*)elm)->e_pt()[1]->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           break;
      case BARC:
           //twinArc
           slist = ((BArc*)elm)->twinArc->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           //endPoint 1
           slist = ((BArc*)elm)->e_pt()[0]->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           //endPoint 2
           slist = ((BArc*)elm)->e_pt()[1]->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
              //slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           break;
      case BCIRCLE:
           //twinCircle
           slist = ((BCircle*)elm)->twinCircle->SElementList;
           while (slist != 0) {
              slist->se->bShow=GUI_RANGE_OF_INFLUENCE;
           //   slist->se->setRangeOfInfluenceForChildShock (slist->se);
              slist = slist->next;
           }
           break;
   }*/
}


//**************************************************************//
//             DEBUG FUNCTIONS
//**************************************************************//

void Boundary::MessageOutBoundarySummaries (int wndid)
{
   int nTotalBElms=0;
  int nTotalGUIElms=0;
  int nTotalNonGUIElms=0;
  int nBP=0, nBL=0, nBA=0;
  int nGUIBP=0, nGUIBL=0, nGUIBA=0;
  int nNonGUIBP=0, nNonGUIBL=0, nNonGUIBA=0;

  BElmListIterator curB = BElmList.begin();
  for (; curB!=BElmList.end(); curB++){
    BElement* current = (curB->second);

    //BoundaryLimitHack
    if (_BoundaryLimit == BIG_RECTANGLE || _BoundaryLimit == BIG_CIRCLE)
      if (current->id() <=8) continue;

    if (!current->isGUIElm())  nTotalNonGUIElms++;
    else              nTotalGUIElms++;

    switch (current->type()) {
    case BPOINT:
      if (!current->isGUIElm()) nNonGUIBP++;
      else               nGUIBP++;
    break;
    case BLINE:
      if (!current->isGUIElm()) nNonGUIBL++;
      else               nGUIBL++;
    break;
    case BARC:
      if (!current->isGUIElm()) nNonGUIBA++;
      else               nGUIBA++;
    break;
    default: break;
    }
  }

  nTotalBElms  = nBElement();
  nBP = nGUIBP + nNonGUIBP;
  nBL = nGUIBL + nNonGUIBL;
  nBA = nGUIBA + nNonGUIBA;

  nTotalNonGUIElms = nNonGUIBP + nNonGUIBL + nNonGUIBA;
  //assert (nTotalBElms == nBP + nBL + nBA + 8 );
  //assert (nTotalBElms == nBP + nBL + nBA);

   vcl_cout<<"Boundary Summaries" <<vcl_endl;
   vcl_cout<<"# of Total Boundary Elements: "<< nTotalBElms <<vcl_endl;
   vcl_cout<<"# of Total GUI BElements: "<< nTotalGUIElms
     <<", total Non-GUI: "<< nTotalNonGUIElms <<vcl_endl;
   vcl_cout<<"# of BPoints: " << nBP << " (GUI: " << nGUIBP
     << ", Non-GUI: " << nNonGUIBP << ")" <<vcl_endl;
   vcl_cout<<"# of BLines: "  << nBL << " (GUI: " << nGUIBL
     << ", Non-GUI: " << nNonGUIBL << ")" <<vcl_endl;
   vcl_cout<<"# of BArcs: "   << nBA << " (GUI: " << nGUIBA
     << ", Non-GUI: " << nNonGUIBA << ")" <<vcl_endl;

}

void Boundary::DebugPrintBoundaryList()
{
  vcl_cout <<vcl_endl;
  vcl_cout << " ==== BOUNDARY LIST ====" <<vcl_endl;
  vcl_cout<< "BoundaryList: " <<vcl_endl;
  BElmListIterator elmPtr = BElmList.begin();
  for (; elmPtr != BElmList.end(); elmPtr++) {
    BElement* current = (elmPtr->second);

    switch (current->type()) {
    case BPOINT:  vcl_cout<< "BPoint"; break;
    case BLINE:    vcl_cout<< "BLine"; break;
    case BARC:    vcl_cout<< "BArc"; break;
    default: break;
    }
    vcl_cout<< ", Bid: "<< current->id()<<vcl_endl;
  }

  vcl_cout <<" ========================" <<vcl_endl;

}

void Boundary::DebugPrintTaintedBoundaryList()
{
  vcl_cout <<vcl_endl;
  vcl_cout << " ==== TAINTED BOUNDARY LIST ====" <<vcl_endl;
  vcl_cout<< "BoundaryList: " <<vcl_endl;
  BElmListIterator elmPtr = taintedBElmList.begin();
  for (; elmPtr != taintedBElmList.end(); elmPtr++) {
    BElement* current = (elmPtr->second);

    switch (current->type()) {
    case BPOINT:  vcl_cout<< "BPoint"; break;
    case BLINE:    vcl_cout<< "BLine"; break;
    case BARC:    vcl_cout<< "BArc"; break;
    default: break;
    }
    vcl_cout<< ", Bid: "<< current->id()<<vcl_endl;
  }

  vcl_cout <<" ========================" <<vcl_endl;

}

void Boundary::DebugPrintBElementInfoFromID(int id)
{
  BElmListIterator elmPtr = BElmList.find(id);

  if (elmPtr != BElmList.end()){
    BElement* current = elmPtr->second;

#ifndef _VISUALIZER_CMDLINE_
    //display info
    current->getInfo(vcl_cout);
#endif
  }
  else
    vcl_cout <<"INVALID BOUNDARY ID: "<<id<<vcl_endl;
}

void Boundary::UpdateBoundary()
{
  //recompute the extrinsic locus of all the elements that were changed
  //the update vcl_list is maintained so that minimal recomputation of the
  //actual extrinsic locus is needed during computation
  //TO Debate: Maintenance of this vcl_list might be too much time consuming

  BElmListIterator curB = update_list.begin();
  for (; curB!=update_list.end(); curB++) {
    BElement* curBElm = (BElement*)(curB->second);
    curBElm->compute_extrinsic_locus();
  }

  //clear vcl_list after it is drawn
  update_list.clear();
}
