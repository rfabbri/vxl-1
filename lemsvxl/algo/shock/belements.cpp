#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include "belements.h"
#include "ishock.h"

BElement::BElement () : BaseGUIGeometry()
{
  _type = BOGUS_BND; //we don't know which type at this time
  _bGUIElm = false;
  _id = -1;
  _edge_id = -1;
  shockList.clear(); //make sure the shocklist is empty 
}

//return the neighboring Boundary Element linked via this shock
BElement* BElement::getNeighboringBElementFromSILink (SILink* silink)
{
  if (silink->lBElement() == this)
    return silink->rBElement();
  else
    return silink->lBElement();
}
//return the neighboring Boundary Element linked via this shock
BElement* BElement::getNeighboringBElementFromSISource (SISource* sisource)
{
  if (sisource->getBElement1() == this)
    return sisource->getBElement2();
  else
    return sisource->getBElement1();
}
//ONLY CHECK FOR SILink and SISource, FOR ALL THE OTHERS, RETURN NULL.
BElement* BElement::getNeighboringBElementFromSIElement (SIElement* sielm)
{
  if (sielm->graph_type()== SIElement::LINK)
    return getNeighboringBElementFromSILink ((SILink*)sielm);
  if (sielm->type() == SIElement::SOURCE)
    return getNeighboringBElementFromSISource ((SISource*)sielm);
  return NULL;
}

//030910 Special case: see Notes
//Our previous approach, add SILink's BElm works for most cases,
//but fail for degenerate cases of zero length ColinearContact.
//So our decision is to traverse SINodes and add BElms only.
//Because, BElms covered by SINodes should include all that SILinks cover.
BElementList BElement::getAllNeighboringBElements()
{
  BElementList neighboringElms;
  neighboringElms.clear();

  shockListIterator curS = shockList.begin();
   for (; curS!=shockList.end(); ++curS) {
    if ((*curS)->graph_type()==SIElement::NODE) {
      SINode* sinode = (SINode*)(*curS);
      BElementList::iterator curB = sinode->bndList.begin();
      for (; curB!=sinode->bndList.end(); ++curB)
        neighboringElms.push_back (*curB);
    }
  }
   
  return neighboringElms;
}

// get explicit tangent point from A3_node_proj type
point_tangent* 
BElement::get_A3_proj_point_tangent(A3_node_proj* proj){
  point_tangent* p = new point_tangent;
  p->tangent = proj->btangent;
  switch (type()){
    case BPOINT :
      p->pt = ((BPoint*)(this))->pt(); 
      break;
    case BLINE :
      {
        double ratio = (proj->distance) / ((BLine*)(this))->L();
        BLine* this_line = ((BLine*)(this));
        double x = (this_line->end().x - this_line->start().x)*ratio + this_line->start().x;
        double y = (this_line->end().y - this_line->start().y)*ratio + this_line->start().y;
        p->pt = Point(x,y);
      }
      break;
    default :
      return 0;
  }
  return p;
}

double 
BElement::intersect_distance(Point* origin, double line_angle)
{
  vgl_homg_point_2d<double> start_pt(origin->x, origin->y);
  vgl_homg_point_2d<double> end_pt(origin->x + vcl_cos(line_angle), origin->y+vcl_sin(line_angle));
  vgl_homg_line_2d<double> line(start_pt, end_pt); 
  switch (type()) {
      case BPOINT : 
        return 0;
      case BLINE :
        {
          BLine* bsegment = ((BLine*)this);
          vgl_homg_point_2d<double> start_homg_pt ((bsegment->start()).x, (bsegment->start()).y);
          vgl_homg_point_2d<double> end_homg_pt((bsegment->end()).x, (bsegment->end()).y);
          vgl_homg_line_2d<double> boundary_segment(start_homg_pt, end_homg_pt);
          vgl_homg_point_2d<double> intersect_pt(line, boundary_segment);
          return vgl_distance(start_homg_pt, intersect_pt);
        }
      case BARC :
        vcl_cout << "ERROR: belm->type = BARC -> not yet considered" << vcl_endl;
        return 0;
      default :
        vcl_cout << "ERROR: something is wrong. The program should not get here." ;
        return 0;
  }
}


//#######################################################################
//           BPOINT

BPoint::BPoint(COORD_TYPE x, COORD_TYPE y, int newid, bool bGUI, INPUT_TAN_TYPE tangent, double conf) :
  BElement(), _pt(x,y), _dir(tangent), _conf(conf)
{
  _type = BPOINT;
   _id = newid;
   _bGUIElm = bGUI;
}

BPoint::~BPoint ()
{
}

void BPoint::mergeWith (BPoint* bpt)
{
  BElement* connectedElm;
  //We need to take the connectivity data from bpt and put it
  //into the linked element list of the current element
  //at the same time we need to update the other elements of
  //their change in connectivity
  BElementList::iterator curB = bpt->LinkedBElmList.begin();
   for(; curB!=bpt->LinkedBElmList.end(); ++curB) {
    connectedElm = (*curB);
    connectedElm->reconnect(bpt, this);
    connectTo(connectedElm);
  }

  //choose the one with higher confidence
  if (this->hasATangent() && bpt->hasATangent()){
    if (bpt->conf() > this->conf()){
      this->set_tangent(bpt->tangent());
    }
  }
}

//JunctionPoint: return prev BElement in a Circular List fashion.
BElement* BPoint::_prev (BElementList::iterator it)
{
  BElement* belm;
  if (it!=LinkedBElmList.begin()) {
    it--;
    belm = (*it);
  }
  else { //return the last element of the list
    it = LinkedBElmList.end();
    it--;
    belm = (*it);
  }
  return belm;
}

//1)Search the SIElementList to get the SIContact of these input elements
//2)Delete child shocks of the SIContact
//  the SIContact itself will be set to active automatically.
//3)Retuen the SIContact that need to be re-activated.
SIContact* BPoint::GetReActivatingContactShock (BElement* lelm, BElement* relm)
{
  shockListIterator curS = shockList.begin();
  for (; curS!=shockList.end(); ++curS) {
    SIElement* selm = (*curS);
    if (selm->label()==SIElement::CONTACT) {
      SIContact* sicontact = (SIContact*)(*curS);
      if (sicontact->lBElement()==lelm && sicontact->rBElement()==relm)
        return sicontact;
    }
  }
  return NULL;
}

//connect this to a line or arc type and put it into the appropriate slot
//(i.e. CCW ordered LinkedBElmList ) 
//the rule is:
//      LINE: if this is the endpoint it goes first
//      ARC:  if this is the startpt, the CCW arc goes first
//          if this is the endpt, the CW arc goes first
//Also the issue of ReActivate the neighboring contacts
void BPoint::connectTo(BElement* elm)
{
  VECTOR_TYPE CCWAngle;
  bool  bStartPt;

  //get the tangent angle of the element to be added
  if (elm->type() == BLINE){
    if ( ((BLine*)elm)->s_pt() == this) {
      CCWAngle = ((BLine*)elm)->U();
      bStartPt = true;
    }
    else {
      CCWAngle = ((BLine*)elm)->U() + M_PI;
      bStartPt = false;
    }
  }
  else if (elm->type() == BARC){
    if ( ((BArc*)elm)->s_pt() == this) {
      CCWAngle = ((BArc*)elm)->InTangentAtStartPt();
      bStartPt = true;
    }
    else {
      CCWAngle = ((BArc*)elm)->InTangentAtEndPt();
      bStartPt = false;
    }
  }
  CCWAngle = angle02Pi(CCWAngle);

  //test:: put this angle in as the tangent dir of the point
  _dir = (float)CCWAngle;

  //if there are no elements yet 
  if (LinkedBElmList.size()==0){
    LinkedBElmList.push_back(elm);
    //No ContactShock issue here.
    return;
  }

  VECTOR_TYPE curCCWAngle;
  BElementList::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    BElement* curbElm = (*curB);
    
    if (curbElm->type() == BLINE){
      if ( ((BLine*)curbElm)->s_pt() == this)
        curCCWAngle = ((BLine*)curbElm)->U();
      else
        curCCWAngle = ((BLine*)curbElm)->U() + M_PI;
    }
    else if (curbElm->type() == BARC){
      if ( ((BArc*)curbElm)->s_pt() == this)
        curCCWAngle = ((BArc*)curbElm)->InTangentAtStartPt();
      else
        curCCWAngle = ((BArc*)curbElm)->InTangentAtEndPt();
    }
    curCCWAngle = angle02Pi(curCCWAngle);

    //EPSILONISSUE: HORIZONTAL LINE WILL HAVE 0, 2*M_PI PROBLEM!!
    if (AisEq02Pi(curCCWAngle,CCWAngle)){
      //this portion deals with the twin elements
      if (bStartPt){
        curB++;
        LinkedBElmList.insert(curB, elm);
      //  curbElm = (*curB);
      //  GetReActivatingContactShock (curbElm, this); //case1
      }
      else {
        LinkedBElmList.insert(curB, elm);
      //  if (curB==LinkedBElmList.begin())
      //  GetReActivatingContactShock (this, _prev(curB)); //case2
      }
      return;
    }    
    else if (AisG(curCCWAngle,CCWAngle)){
      //here you have to add it before the current element
      LinkedBElmList.insert(curB, elm);
      //if (bStartPt)
      //  GetReActivatingContactShock (curbElm, this); //case1
      //else
      //  GetReActivatingContactShock (this, _prev(curB)); //case2
      return;
    }
  }

  //if it gets here it means the element has to be added to the end of the list
  LinkedBElmList.push_back(elm);
  //if (bStartPt) {
  //  curB = LinkedBElmList.end();
  //  curB--;
  //  BElement* curbElm = (*curB);
  //  GetReActivatingContactShock (this, curbElm); //case1
  //}
  //else {
  //  curB = LinkedBElmList.begin();
  //  BElement* curbElm = (*curB);
  //  GetReActivatingContactShock (curbElm, this); //case2
  //}
}

void BPoint::disconnectFrom(BElement* elm)
{
  BElementList::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      LinkedBElmList.erase(curB);
      return;
    }
  }
}

BElement* BPoint::getElmToTheRightOf(BElement* elm)
{
  if (LinkedBElmList.size()==0)
    return NULL;

  if (LinkedBElmList.front() == elm)
    return LinkedBElmList.back();

  BElementList::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      curB--;
      return (*curB);
    }
  }

  return NULL;
}

BElement* BPoint::getElmToTheLeftOf(BElement* elm)
{
  if (LinkedBElmList.size()==0) return NULL;

  if (LinkedBElmList.back() == elm)
    return LinkedBElmList.front();

  BElementList::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB) == elm){
      curB++;
      return (*curB);
    }
  }

  return NULL;
}

//Have to deal with BLine and BArc!
//return true if this BPoint will form a ColinearContact
//also put the two SIContact as cc1 and cc2;
//return false if not.
//Note that we only detect ColinearContact on the wider side!
bool BPoint::isFormingColinearContact (Point src, SIContact **cc1, SIContact **cc2)
{
  if (nLinkedElms()!=4)
    return false;

  //1)Go through the LinkedBElmList, search for the 2 GUIElms
  BElement *belm1 = NULL, *belm2 = NULL;
  BElementList::iterator curB = LinkedBElmList.begin();
  for(; curB!=LinkedBElmList.end(); ++curB) {
    if ((*curB)->isGUIElm()) {
      if (!belm1)
        belm1 = (*curB);
      else
        belm2 = (*curB);
    }
  }
  assert (belm1);
  assert (belm2);

  //2)Assign BLine and BArc 1 & 2...
  BLine *bline1=NULL, *bline2=NULL;
  BArc *barc1=NULL, *barc2=NULL;
  if (belm1->type()==BLINE) {
    bline1 = (BLine*) belm1;
    if (belm2->type()==BLINE) { //I
      bline2 = (BLine*) belm2;
      //if needed, swap bline1 and bline2
      if (bline1->s_pt()==bline2->e_pt()) {
        BLine* temp = bline1;
        bline1 = bline2;
        bline2 = temp;
      }
      //By now, we have clear <l1,l2> defined!
      //2-A)compare their angle
      if (_isEq (bline1->U(), bline2->U(), CONTACT_EPSILON))
        goto DETERMINE_CC1_CC2;
    }//end case I
    else { //II
      barc2 = (BArc*) belm2;
      //if needed, swap bline1 and barc2 to bline2 and barc1
      if (bline1->s_pt()==barc2->e_pt()) {
        barc1 = barc2;
        bline2 = bline1;
        bline1 = NULL;
        barc2 = NULL;
      }
      else
        return false;
      //By now, we have clear <a1,l2> defined!
      //2-B)compare their angle
      if (_isEq (barc1->_OutTangent, bline2->U(), CONTACT_EPSILON))
        goto DETERMINE_CC1_CC2;
    }//end case II
  }
  else {
    barc1 = (BArc*) belm1;
    if (belm2->type()==BLINE) { //III
      bline2 = (BLine*) belm2;
      //if needed, swap bline2 and barc1 to bline1 and barc2
      if (barc1->s_pt()==bline2->e_pt()) {
        bline1 = bline2;
        barc2 = barc1;
        barc1 = NULL;
        bline2 = NULL;
      }
      else
        return false;
      //By now, we have clear <l1,a2> defined!
      //2-C)compare their angle
      if (_isEq (bline1->U(), barc2->_InTangent, CONTACT_EPSILON))
        goto DETERMINE_CC1_CC2;
    }//end case III
    else { //IV
      barc2 = (BArc*) belm2;
      //if needed, swap barc1 and barc2
      if (barc1->s_pt()==barc2->e_pt()) {
        BArc* temp = barc1;
        barc1 = barc2;
        barc2 = temp;
      }
      //By now, we have clear <a1,a2> defined!
      //2-D)compare their angle
      if (_isEq (barc1->_OutTangent, barc2->_InTangent, CONTACT_EPSILON))
        goto DETERMINE_CC1_CC2;
    }//end case IV
  }

  //2-2)Not ColinearContact
  return false;

DETERMINE_CC1_CC2:

  //3)ColinearContact, locate cc1 and cc2...
  //compare n to _pt->src
  VECTOR_TYPE dir = _vPointPoint (_pt, src);
  shockListIterator curS = shockList.begin();
  *cc1 = NULL;
  *cc2 = NULL;
  for (; curS!=shockList.end(); ++curS) {
    if ((*curS)->type()==SIElement::POINTLINECONTACT ||
       (*curS)->type()==SIElement::POINTARCCONTACT) {
      SIContact* sc = (SIContact*)(*curS);
      ///!!!! VERY BAD !!!! SHOULD RE-DESIGN SOMEDAY.
      ///if (dir==0)
      ///  dir=2*M_PI;
      if (_isEqAngle(sc->n(), dir, CONTACT_EPSILON*100)) { //_isEq
        if ((*cc1)==NULL)
          (*cc1) = sc;
        else
          (*cc2) = sc;
      }
    }
  }
  if (*cc1==NULL || *cc2==NULL)
    return false;
  ///assert (*cc1);
  ///assert (*cc2);
  
  if ((*cc1)->lBElement()==this) {
    SIContact* temp = (*cc1);
    (*cc1) = (*cc2);
    (*cc2) = temp;
  }
  return true;
}

void BPoint::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  sprintf (s, "\n==============================\n"); ostrm<<s;
   sprintf (s, "Boundary Point\n \n"); ostrm<<s;
   sprintf (s, "ID: %d\n", _id); ostrm<<s;
   sprintf (s, "Position: (%.10f, %.10f)\n", _pt.x, _pt.y); ostrm<<s;
  sprintf (s, "Tangent: %.5f (= %.5f) \n", _dir, _dir*180/M_PI); ostrm<<s;
  sprintf (s, "Confidence: %.5f \n", _conf); ostrm<<s;

   sprintf (s, "bGUIElm: %s\n\n", _bGUIElm ? "yes" : "no"); ostrm<<s;
  assert (_type==BPOINT);
   sprintf (s, "nSElement: %d\n", nSElement()); ostrm<<s;
   sprintf (s, "SElement(s) Linked to this BPoint: "); ostrm<<s;

   shockListIterator curS = shockList.begin();
   for (; curS!=shockList.end(); ++curS) {
      sprintf (s, "%d, ", (*curS)->id()); ostrm<<s;
  }
   sprintf (s, "\n\n"); ostrm<<s;

   sprintf (s, "nLinkedElms: %d\n", nLinkedElms()); ostrm<<s;
   sprintf (s, "BElement(s) Linked to this BPoint: "); ostrm<<s;
  BElementList::iterator curB = LinkedBElmList.begin();
   for(; curB!=LinkedBElmList.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
   sprintf (s, "\n \n"); ostrm<<s;

  sprintf (s, "Neighboring BElements::\n"); ostrm<<s;
  
   sprintf (s, "Neighboring BElement(s) Linked to this Point (id=%d): ", _id); ostrm<<s;
  BElementList neighboringBElms = getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;
}

//#######################################################################
//           BLINE

BLine::BLine (BPoint* startpt, BPoint* endpt, int id, bool bGUI) :
  BElement ()
{
  _id    = id;
   _type    = BLINE;
  _bGUIElm  = bGUI;

  _start  = startpt->pt();
   _end    = endpt->pt();
  
  startPt  = startpt;
  endPt    = endpt;
   _twinLine = NULL;

   _u      = _vPointPoint (_start, _end);
   _n      = angle02Pi (_u + M_PI_2);
   _l      = _distPointPoint (_start, _end);

  //now you need to link the BPoints to the line
  s_pt()->connectTo(this);
  e_pt()->connectTo(this);
}

//EPSILONISSUE 20
//Remember to recompute _u, _n, and _l for this BLine
//Don't forget to refresh _lineIV, too.
void BLine::reconnect(BPoint* oldPt, BPoint* newPt)
{
  if(oldPt==s_pt()) {
    startPt = newPt;
    _start  = newPt->pt();
  }
  else {
    assert (oldPt==e_pt());
    endPt = newPt;
    _end  = newPt->pt();
  }
   _u      = _vPointPoint (_start, _end);
   _n      = angle02Pi (_u + M_PI_2);
   _l      = _distPointPoint (_start, _end);
}

DIST_TYPE BLine::validDistPoint (Point pt)
{
  double t = isPointValidInTPlaneFuzzy (pt);
  if (t==0)
    return ISHOCK_DIST_HUGE;
  else
    return _distPointLine (pt, _start, _end);
}

DIST_TYPE BLine::validDistSqPoint (Point pt)
{
  double t = isPointValidInTPlaneFuzzy (pt);
  if (t==0)
    return ISHOCK_DIST_HUGE;
  else
    return _distSqPointLine (pt, _start, _end);
}

//This function is needed for Local Shock Computation.
DIST_TYPE BLine::initDistSqPointGUI (Point pt)
{
  double t = _isPointValidInT (pt);
  if (t!=0) 
    return _distSqPointLine (pt, _start, _end);
  else {
    if (t<=0)
      return _distSqPointPoint (pt, _start);
    else 
      return _distSqPointPoint (pt, _end);
  }
}

//  Even this function is not design to tolerate fuzzy, 
//  we still need a TINY_EPSILON here!
//1)First check for T constraint,
//2)then check for Plane constraint,
//if both valid, return t
//else return 0
//double BLine::isPointValidInTPlane (Point pt)
//{
//  double t = _isPointValidInTFuzzy (pt, TINY_EPSILON); //_isPointValidInT(pt);
//  if (t!=0)
//    if(_isPointValidInPlane(pt))
//      return t;
//  return 0;
//}

//EPSILONISSUE 1
double BLine::isPointValidInTPlaneFuzzy (Point pt) 
{
  double t = _isPointValidInTFuzzy(pt, EP_EPSILON);
  if (t!=0)
    if(_isPointValidInPlane(pt))
      return t;
  return 0;
}

void BLine::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

   sprintf (s, "\n==============================\n"); ostrm<<s;
   sprintf (s, "Boundary Line\n \n"); ostrm<<s;
   sprintf (s, "ID: %d\n", _id); ostrm<<s;
   sprintf (s, "From-To: (%.10f, %.10f)-(%.10f, %.10f)\n \n", 
              _start.x, _start.y, _end.x, _end.y); ostrm<<s;

   sprintf (s, "u: %.16f\n", _u); ostrm<<s;
   sprintf (s, "n: %.16f (u+1.57)\n", _n); ostrm<<s;
   sprintf (s, "length: %f\n", _l); ostrm<<s;

   sprintf (s, "bGUIElm: %s\n", _bGUIElm ? "yes" : "no"); ostrm<<s;
  assert (_type==BLINE);
   sprintf (s, "nSElement: %d\n", nSElement()); ostrm<<s;
   sprintf (s, "SElement(s) Linked to this BLine (id=%d): ", _id); ostrm<<s;
  shockListIterator curS = shockList.begin();
   for (; curS!=shockList.end(); ++curS) {
      sprintf (s, "%d, ", (*curS)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;

  sprintf (s, "Linked twinLine id: %d (%f, %f)-(%f, %f)\n", 
          twinLine()->id(), twinLine()->start().x, twinLine()->start().y,
          twinLine()->end().x, twinLine()->end().y); ostrm<<s;
  sprintf (s, "Linked s_pt() id: %d (%f, %f)\n", 
          s_pt()->id(), s_pt()->pt().x, s_pt()->pt().y); ostrm<<s;
  sprintf (s, "Linked e_pt() id: %d (%f, %f)\n\n", 
          e_pt()->id(), e_pt()->pt().x, e_pt()->pt().y); ostrm<<s;

  sprintf (s, "Linked twinLine's nSElement: %d\n", twinLine()->nSElement()); ostrm<<s;
  sprintf (s, "SElement(s) Linked to the twin BLine (id=%d): ", twinLine()->id()); ostrm<<s;
  curS = twinLine()->shockList.begin();
  for(; curS!=twinLine()->shockList.end(); ++curS) {
    sprintf (s, "%d, ", (*curS)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;

   sprintf (s, "Neighboring BElement(s) Linked to this BLine (id=%d): ", _id); ostrm<<s;
  BElementList neighboringBElms = getAllNeighboringBElements();
  BElementListIterator curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n"); ostrm<<s;

  if (twinLine()){
    sprintf (s, "Neighboring BElement(s) Linked to the twinLine (id=%d): ", twinLine()->id()); ostrm<<s;
    neighboringBElms.clear();
    neighboringBElms = twinLine()->getAllNeighboringBElements();
    curB = neighboringBElms.begin();
    for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
    }
    sprintf (s, "\n"); ostrm<<s;
  }

   sprintf (s, "Neighboring BElement(s) Linked to the s_pt() (id=%d): ", s_pt()->id()); ostrm<<s;
  neighboringBElms.clear();
  neighboringBElms = s_pt()->getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n"); ostrm<<s;
  
   sprintf (s, "Neighboring BElement(s) Linked to the e_pt() (id=%d): ", e_pt()->id()); ostrm<<s;
  neighboringBElms.clear();
  neighboringBElms = e_pt()->getAllNeighboringBElements();
  curB = neighboringBElms.begin();
   for (; curB!=neighboringBElms.end(); ++curB) {
      sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }
  sprintf (s, "\n\n"); ostrm<<s;
}

//#######################################################################
//           BARC

BArc::BArc (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
            Point center, DIST_TYPE r, ARC_NUD nud)
   : BElement ()
{
  _id = id;
   _type = BARC;
   _bGUIElm = bGUI;

  _start  = startpt->pt();
   _end    = endpt->pt();
  _center  = center;
  _R      = r;

  _StartVector = angle02Pi(_vPointPoint(_center, _start));
  _EndVector = angle02Pi(_vPointPoint(_center, _end));

   _nud = nud;
  if (_nud==ARC_NUD_CCW) {
    _CCWStartVector  = _StartVector;
    _CCWEndVector    = _EndVector;
    _InTangent = angle02Pi (_StartVector + M_PI/2);
    _OutTangent = angle02Pi (_EndVector + M_PI/2);
  }
  else {
    _CCWStartVector  = _EndVector;
    _CCWEndVector    = _StartVector;
    _InTangent = angle02Pi (_StartVector - M_PI/2);
    _OutTangent = angle02Pi (_EndVector - M_PI/2);
  }
  _bAcross = (_CCWStartVector>_CCWEndVector);

  startPt  = startpt;
  endPt    = endpt;
   _twinArc = NULL;

  //now you need to link the BPoints to the line
  s_pt()->connectTo(this);
  e_pt()->connectTo(this);
}

//functions for computing boundary tangents
double BArc::TangentAlongCurveAtPoint (Point pt)
{
  if (_nud == ARC_NUD_CCW)
    return angle02Pi(_vPointPoint(_center, pt) + M_PI/2);
  else
    return angle02Pi(_vPointPoint(_center, pt) - M_PI/2);
}

double BArc::TangentAgainstCurveAtPoint (Point pt)
{
  if (_nud == ARC_NUD_CCW)
    return angle02Pi(_vPointPoint(_center, pt) - M_PI/2);
  else
    return angle02Pi(_vPointPoint(_center, pt) + M_PI/2);
}

//EPSILONISSUE 21
//Remember to recompute _center and _R for this BArc
//_StartVectors, _EndVectors, too
//Here we fix _R, recompute the new _center
void BArc::reconnect(BPoint* oldPt, BPoint* newPt)
{
  if(oldPt==s_pt()){
    startPt = newPt;
    _start = newPt->pt();
    _StartVector = _vPointPoint(_center, _start);
  }
  else {
    assert (oldPt==e_pt());
    endPt = newPt;
    _end = newPt->pt();
    _EndVector = _vPointPoint(_center, _end);
  }
  _center = getCenterOfArc (_start.x, _start.y, _end.x, _end.y, _R, _nud, ARC_NUS_SMALL);

  if (_nud==ARC_NUD_CCW) {
    _CCWStartVector  = _StartVector;
    _CCWEndVector    = _EndVector;
    _InTangent = angle02Pi (_StartVector + M_PI/2);
    _OutTangent = angle02Pi (_EndVector + M_PI/2);
  }
  else {
    _CCWStartVector  = _EndVector;
    _CCWEndVector    = _StartVector;
    _InTangent = angle02Pi (_StartVector - M_PI/2);
    _OutTangent = angle02Pi (_EndVector - M_PI/2);
  }
  _bAcross = (_CCWStartVector>_CCWEndVector);
}

bool BArc::isVectorFuzzilyValid(VECTOR_TYPE v, double epsilon)
{
  //Fuzzily valid near boundary.
  if (_isEq(v, _CCWStartVector, epsilon) ||
     _isEq(v, _CCWEndVector, epsilon))
    return false;

  return isVectorValid (v);
}

bool BArc::isVectorValid (VECTOR_TYPE v)
{
  if (_bAcross)
    return (0 <= v && v <= _CCWEndVector ||
          _CCWStartVector <= v && v <= M_PI*2);
  else
    return (_CCWStartVector <= v && v <= _CCWEndVector);
}

bool BArc::isVectorLessThanStart (VECTOR_TYPE v)
{
  if (_bAcross)
    return v<_CCWStartVector && v>_CCWStartVector-M_PI;
  else
    return (v < _CCWStartVector);
}

bool BArc::isVectorGreaterThanEnd (VECTOR_TYPE v)
{
  if (_bAcross)
    return v>_CCWEndVector && v<_CCWEndVector+M_PI;
  else
    return (v > _CCWEndVector);
}

//  Even this function is not design to tolerate fuzzy, 
//  we still need a TINY_EPSILON here!
//1)First check for angle constraint,
//2)then check for R constraint,
//if both valid, return true, also put angle as a return in reference.
//else return false
//bool BArc::isPointValidInRAngle (Point pt, VECTOR_TYPE& angle)
//{  
//  angle = _vPointPoint (_center, pt);
//  if (!isVectorFuzzilyValid(angle, TINY_EPSILON))
//    return false;
//
//  DIST_TYPE r = _distPointPoint (_center, pt);
//  if (_nud==ARC_NUD_CCW) {
//    if (r>=_R)
//      return false;
//  }
//  else { //ARC_NUD_CW
//    if (r<=_R)
//      return false;
//  }
//  
//  return true;
//}

//EPSILONISSUE 1
bool BArc::isPointValidInRAngleFuzzy (Point pt, VECTOR_TYPE& angle)
{  
  angle = _vPointPoint (_center, pt);
  if (!isVectorFuzzilyValid (angle, EP_EPSILON))
    return false;

  DIST_TYPE r = _distPointPoint (_center, pt);
  if (_nud==ARC_NUD_CCW) {
    if (r>=_R)
      return false;
  }
  else { //ARC_NUD_CW
    if (r<=_R)
      return false;
  }
  
  return true;
}

//Check validation or not, if not visible, return ISHOCK_DIST_HUGE.
DIST_TYPE BArc::validDistPoint (Point pt)
{
  double a;
  if (isPointValidInRAngleFuzzy (pt, a)) {
    double d = _distPointPoint (_center, pt);
    return vcl_fabs (d-_R);
  }
  else
    return ISHOCK_DIST_HUGE;
}

//There is no good way to compute this without footPt!
//We have to computer footPt here!
DIST_TYPE BArc::validDistSqPoint (Point pt)
{
  double a;
  if (isPointValidInRAngleFuzzy (pt, a)) {
    double d = _distSqPointPoint (_center, pt);

    Point footPt = _vectorPoint (_center, a, _R);
    return _distSqPointPoint (pt, footPt);
  }
  else
    return ISHOCK_DIST_HUGE;
}

//This function is needed for Local Shock Computation.
DIST_TYPE BArc::initDistSqPointGUI (Point pt)
{
  double a = _vPointPoint (_center, pt);
  if (isVectorValid (a)) {
    double d = _distPointPoint(pt, _center);
    return vcl_fabs(d-_R)*vcl_fabs(d-_R);    //kind of redundant!!
  }
  else { //2)Slow: Choose the closer one from startPt and endPt
    double dists = _distSqPointPoint (_start, pt);
    double diste = _distSqPointPoint (_end, pt);
    return vnl_math_min (dists, diste);
  }
}

void BArc::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

   sprintf (s, "\n==============================\n"); ostrm<<s;
   sprintf (s, "Boundary Arc\n \n"); ostrm<<s;
   sprintf (s, "ID: %d\n", _id); ostrm<<s;
   sprintf (s, "From-To: (%f, %f)-(%f, %f)\n", 
              _start.x, _start.y, _end.x, _end.y); ostrm<<s;
   sprintf (s, "Angle(From-To): (%f)-(%f) Degree: (%f)-(%f)\n",
          _StartVector, _EndVector, _StartVector*180/M_PI, _EndVector*180/M_PI); ostrm<<s;
  sprintf (s, "Radius: %10f\n", _R); ostrm<<s;
  sprintf (s, "Curvature: %10f\n", 1/_R); ostrm<<s;
  sprintf (s, "Length: %10f\n", L()); ostrm<<s;
   sprintf (s, "Center: (%10f, %10f)\n", _center.x, _center.y); ostrm<<s;
   sprintf (s, "InTangent: %.16f\n", _InTangent); ostrm<<s;
   sprintf (s, "OutTangent: %.16f\n\n", _OutTangent); ostrm<<s;
   sprintf (s, "nud: %s, ", (_nud==ARC_NUD_CW) ? "1 (CW)" : "-1 (CCW)"); ostrm<<s;

   sprintf (s, "bGUIElm: %s\n", _bGUIElm ? "yes" : "no"); ostrm<<s;
   sprintf (s, "nSElement: %d\n", nSElement()); ostrm<<s;
   sprintf (s, "SElement(s) Linked to this BArc (id=%d): ", _id); ostrm<<s;

  shockListIterator curS = shockList.begin();
   for(; curS!=shockList.end(); ++curS) {
      sprintf (s, "%d, ", (*curS)->id()); ostrm<<s;
  }
   sprintf (s, "\n\n"); ostrm<<s;

   sprintf (s, "Linked twinArc id: %d (%f, %f)-(%f, %f)\n", 
              twinArc()->id(), twinArc()->start().x, twinArc()->start().y,
              twinArc()->end().x, twinArc()->end().y); ostrm<<s;
   sprintf (s, "Linked s_pt() id: %d (%f, %f)\n", 
              s_pt()->id(), s_pt()->pt().x, s_pt()->pt().y); ostrm<<s;
   sprintf (s, "Linked e_pt() id: %d (%f, %f)\n\n", 
              e_pt()->id(), e_pt()->pt().x, e_pt()->pt().y); ostrm<<s;

   sprintf (s, "twin BArc's nSElement: %d\n", twinArc()->nSElement()); ostrm<<s;
   sprintf (s, "SElement(s) Linked to the twin BArc (id=%d): ", twinArc()->id()); ostrm<<s;
  curS = twinArc()->shockList.begin();
   for(; curS!=twinArc()->shockList.end(); ++curS) {
      sprintf (s, "%d, ", (*curS)->id()); ostrm<<s;
  }
   sprintf (s, "\n\n"); ostrm<<s;

}

//#############################################################
//     CONTOUR
//#############################################################

BContour::BContour(int id):
  BElement ()
{
  _id = id;
   _type = BCONTOUR;

  elms.clear();
}

void BContour::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

   sprintf (s, "Boundary Contour\n \n"); ostrm<<s;
  sprintf (s, "ID: %d\n", _id); ostrm<<s;
  sprintf (s, "This Contour Consists of the following elements: \n"); ostrm<<s;
  
  BElementList::iterator curB = elms.begin();
   for(; curB!=elms.end(); ++curB) {
    sprintf (s, "%d, ", (*curB)->id()); ostrm<<s;
  }

  sprintf (s, "\n \n");ostrm<<s;
  //.CON file output
  sprintf (s, "CONTOUR\nOPEN\n%d\n", elms.size()); ostrm<<s;
  curB = elms.begin();
   for(; curB!=elms.end(); ++curB) {
    if ((*curB)->type()==BPOINT) {
      BPoint* bpoint = (BPoint*)(*curB);
      sprintf (s, "%f %f\n", bpoint->pt().x, bpoint->pt().y); ostrm<<s;
    }
  }
  
  sprintf (s, "\n \n");ostrm<<s;
}
