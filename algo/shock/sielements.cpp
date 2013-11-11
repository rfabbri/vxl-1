/////////////////////////////////////////////////////
// INTRINSIC SHOCK ELEMENT FUNCTIONS
/////////////////////////////////////////////////////

#include <vcl_sstream.h>
#include <vcl_cmath.h>

#include <extrautils/msgout.h>
#include "ishock-common.h"
#include "sielements.h"

//###############################################
//   SHOCK CLASS IMPLEMENTATION
//###############################################

SIElement::SIElement (int newid, int neworder, 
  SHOCKLABEL newlabel, RADIUS_TYPE stime): BaseGUIGeometry(), 
  _id(newid), _order(neworder), _label(newlabel), 
  _startTime(stime), _simTime(stime)
{
  _bActive    = true;
  _bPropagated = false;
  _bValid    = true;
  _bHidden    = false;
  _traverseState = NOT_TRAVERSED;

  bIO = true; //inside
  bIOVisited  = false;

  //for symmetry transform
   _dPnCost    = 0;
   _dOC      = 0;
   _dNC      = 0;
}

SINode::SINode (int newid, int neworder, SHOCKLABEL newlabel, RADIUS_TYPE stime) :
   SIElement (newid, neworder, newlabel, stime)
{
  _graph_type = NODE;
  _cSLink = NULL;
  _cSLink2 = NULL;
   bndList.clear();
}

SINode::~SINode ()
{
  //Remove link from the boundary elements to this shock
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      (*curB)->delBnd2ShockLink (this);
  }
   bndList.clear();

  //Now you also need to update connectivity information
  //But this is specific to the kind of node
}

void SINode::clear_pSLink(SILink* pslink)
{
  //remove this SILink from the nodes' parent list
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    SILink* curLink = (*curS);
    if (curLink== pslink){
      PSElementList.remove(curLink);
      break;
    }
  }
}

int SINode::countUnprunedParents (void)
{
  int nNotPruned = 0;

  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS)
    if (!(*curS)->isAContact() && !(*curS)->isHidden())
      nNotPruned++;

  return nNotPruned;
}

SILink* SINode::UnprunedParent (void)
{
  if (countUnprunedParents() == 1) {
    SILinksList::iterator curS = PSElementList.begin();
    for(; curS!=PSElementList.end(); ++curS)
      if (!(*curS)->isAContact() && !(*curS)->isHidden())
        return (*curS);
  }
  assert (0);
  return 0;
}

int SINode::indeg(bool doNotincludehidden)
{
  int nNotHidden = 0;
  int nAll = 0;

  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    if (!(*curS)->isAContact()){
      if (!(*curS)->isHidden())
        nNotHidden++;
      nAll++;
    }
  }

  if (doNotincludehidden)
    return nNotHidden;
  else
    return nAll;
} 

int SINode::outdeg(bool doNotincludehidden)
{
  int deg = 0;

  if (_cSLink){
    if (doNotincludehidden)
      deg += (int)_cSLink->isNotHidden();
    else
      deg++;
  }
  if (_cSLink2){
    if (doNotincludehidden)
      deg += (int)_cSLink2->isNotHidden();
    else
      deg++;
  }
  return deg;
} 


// return true if the node coresponds to curvature extrema on the boundary
bool SINode::is_curvature_xtrema_node(){
  // An curvature extrema node can be either:
  //  - an A3Source
  //  - junction with all pLinks pruned out,
  //  - sink with only one pLink not hidden
  // One way to recognize them is to count the number of NotHidden links connected to it.
  return((degree() == 1)&&(isNotHidden()));
}

SILinksList SINode::neighbor_links(bool include_hidden_links){
  SILinksList links_list;
  links_list.clear();
  if (SILink* cur_link = cSLink()){
    if (cur_link->isNotHidden() || include_hidden_links)
      links_list.push_back(cur_link);
  }
  if (SILink* cur_link = cSLink2()){
    if (cur_link->isNotHidden() || include_hidden_links)
      links_list.push_back(cur_link);
  }

  for (SILinksList::iterator it = PSElementList.begin();
    it != PSElementList.end(); it ++)
  {
    if ((*it)->isNotHidden() || include_hidden_links){
      links_list.push_back((*it));
    }
  }
  return links_list;
}



SILink::SILink (int newid, int neworder, 
           SHOCKLABEL newlabel, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe, 
                SINode* pse) :
   SIElement (newid, neworder, newlabel, stime), _edgeID(newid)
{
  _graph_type = LINK;

  _lBElement = lbe;
  _rBElement = rbe;

  //set up the link from the boundary to the shock
  _lBElement->addBnd2ShockLink (this);
  _rBElement->addBnd2ShockLink (this);

   _lNeighbor = NULL; 
   _rNeighbor = NULL;
   _pSNode = pse;
   _cSNode = NULL;

   _endTime = ISHOCK_DIST_HUGE;
   _H = 0;
  _lsvector = -1;
  _rsvector = -1;
   _LsTau = -1;
   _LeTau = -1;
   _RsTau = -1;
   _ReTau = -1;
   _EndVector = -1;
}

SILink::~SILink ()
{
  //delete the link from the boundary to the shock
  _lBElement->delBnd2ShockLink (this);
  _rBElement->delBnd2ShockLink (this); 

  //updating connectivity information 
  if (_lNeighbor)
    _lNeighbor->clear_rNeighbor();
  if (_rNeighbor)
    _rNeighbor->clear_lNeighbor();
}

//Debug verify every taus
void SILink::correctLTau (TAU_TYPE ltau)
{
  if (_LsTau<_minLTau)
    _LsTau = _minLTau;
  if (_LsTau>_maxLTau)
    _LsTau = _maxLTau;
}

void SILink::correctRTau (TAU_TYPE rtau)
{
  if (_RsTau<_minRTau)
    _RsTau = _minRTau;
  if (_RsTau>_maxRTau)
    _RsTau = _maxRTau;
}

void SILink::setSimTime (RADIUS_TYPE stime) 
{
  //Imporvcl_tant!!
  //In the beginning, a SILink is always first propagated to 
  //  i)  INFINITY (ISHOCK_DIST_HUGE) or
  //  ii) A JUNCTION
  //For all other propagation, simTime is always decrevcl_asing!

  //assert (_startTime<=stime);
  if (stime < _startTime) {
    //if (MessageOption>=MSG_NORMAL) {
    //  vcl_cout<< "Correction! sid= "<< _id << " _endTime less than _startTime, error= " << stime-_startTime <<vcl_endl;
    //  MSGOUT();
    //}
    //Correction: add R_PHI_EPSILONON to _startTime
    stime = _startTime; //+R_PHI_EPSILONON;
  }

  _simTime = stime;
  _endTime = stime;
}

//if the current shockwave is at a junction, return JUNCTION_TYPE
//else return NO_JUNCTION
JUNCTION_TYPE SILink::getJunctionType (void)
{
  //1)Numerical issue: a shock propagates and should have intersection.
  //  but actually no junction found.
   if (_lNeighbor==NULL && _rNeighbor==NULL) {
    vcl_cout<< "Numerical Issue: getJunctionType(): TO_INFINITY sid=" << _id <<vcl_endl;
    return TO_INFINITY; //BOGUS_JUNCTION_TYPE
  }

  //2)
   if (_rNeighbor == _lNeighbor)
      return SINK_JUNCTION;

   SILink *rshock = this;
   while (rshock->rNeighbor() !=NULL && rshock->rNeighbor() != this)
      rshock = rshock->rNeighbor();

   if (rshock->rNeighbor() == this)
      return SINK_JUNCTION;

   SILink *lshock = this;
   while (lshock->lNeighbor() !=NULL && lshock->lNeighbor() != this)
      lshock = lshock->lNeighbor();

   if (lshock->lNeighbor() == this)
      return SINK_JUNCTION;

  //!!!!! Special case of SINK: shouldn't happen,
  //But it does happen when numerical epsilon of R is introduced.
  //( see 030216-RemoveSpecialCaseOfSink-3.bnd)
   if (lshock->lBElement() == rshock->rBElement()) {
    //vcl_cout<< vcl_endl<<"WARNING: Numerical Issue--Special case of SINK." <<vcl_endl;
      lshock->set_lNeighbor (rshock);
      rshock->set_rNeighbor (lshock);
      return SINK_JUNCTION;
   }

   if (rshock != this && lshock != this)
      return DEGENERATE_JUNCTION;

   if (rshock != this && rshock != _rNeighbor)
      return DEGENERATE_JUNCTION;

   if (lshock != this && lshock != _lNeighbor)
      return DEGENERATE_JUNCTION;

   if (lshock == this && rshock == _rNeighbor)
      return RIGHT_REGULAR_JUNCTION;

   if (rshock == this && lshock == _lNeighbor) 
      return LEFT_REGULAR_JUNCTION;

  return BOGUS_JUNCTION_TYPE;
}

/*bool SILink::isCornerShock ()
{
   if (type()==LINELINE) {
    SILineLine* silineline = (SILineLine*) this;
    if (_BisEqPoint( silineline->lBLine()->end(), silineline->rBLine()->start() ))
      return true;
    else 
      return false;
  }
  else if (type()==LINEARC) {
    SILineArc* silinearc = (SILineArc*) this;
    //if (_RisEqPoint( lBLine->end(), rBLine()->start() ))
    //  return true;
    //else 
    //  return false;
  }
  else if (type()==ARCARC) {
    SIArcArc* siarcarc = (SIArcArc*) this;
    //if (_RisEqPoint( lBLine->end(), rBLine()->start() ))
    //  return true;
    //else 
    //  return false;
  }
  else
    return false;
}*/

//A Pruned Junction is a junction that contains only a parent shock and a child shock.
//It's a 'Link Junction' that connects two shocks after pruning.
SILink* SILink::GetChildLink ()
{
  if (!cSNode()) //1)Some edges might go to infinity
    return NULL;

  if (cSNode()->type() != JUNCT) //2)Child is a Sink
    return NULL;

  SIJunct* junctNode = (SIJunct*) cSNode();
  //3)Return child link only if it is a PrunedJunction
  if (junctNode->countUnprunedParents()==1)
    return junctNode->cSLink();

  return NULL;
}

SILink* SILink::GetParentLink ()
{
  if (!pSNode()) {
    return NULL;
  }

  // Return parent link only if it is a PrunedJunction
  if (pSNode()->type() != JUNCT)
    return NULL;
  
  SIJunct* junctnode = (SIJunct*) pSNode();
  if (junctnode->countUnprunedParents()==1)
    return junctnode->UnprunedParent ();

  return NULL;
}

//overwite this function because clear neighbor is a special
//function for contacts. Once you clear a contact as a neighbor,
//it can go intersect with anything again i.e., endtime is not limited
//to the last intersection, so we'll take it to infinity
void SIContact::clear_rNeighbor ()
{ 
  _rNeighbor = NULL;
  //if (_ColinearContactType == RIGHT_COLINEAR_CONTACT)
  //  _endTime=ISHOCK_DIST_HUGE;
}

void SIContact::clear_lNeighbor ()
{ 
  _lNeighbor = NULL; 
  //if (_ColinearContactType == LEFT_COLINEAR_CONTACT)
  //  _endTime=ISHOCK_DIST_HUGE;
}

SIA3Source::SIA3Source (int newid, RADIUS_TYPE stime, Point Or,
  BElementList blist, SILinksList pselist) :
   SINode (newid, 1, REGULAR, stime)
{
   _type = A3SOURCE;
  _origin = Or;
  bndList = blist;
   PSElementList = pselist;
   _cSLink = NULL;

  //add link from the boundary elements to this shock
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      (*curB)->addBnd2ShockLink (this);
  }    

  //add link from parents to this node
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS)
    (*curS)->set_cSNode(this);
}

SIA3Source::~SIA3Source(){}

SISource::SISource (int newid, SHOCKLABEL newlabel, 
  RADIUS_TYPE stime, Point Or, BElement* belm1, BElement* belm2) :
   SINode (newid, 2, newlabel, stime)
{
   _type = SOURCE;
  _origin = Or;
  _bValid = true;

  //add boundary to shock link
  belm1->addBnd2ShockLink (this);
  belm2->addBnd2ShockLink (this);

  //put the boundary elements into the list
  bndList.push_back(belm1);
   bndList.push_back(belm2);

}

SISource::~SISource()
{}

//equal operator: checks to see if they are from the same boundary
//int isSameSource(SISource* A, SISource* B)
//{
//  return (A->getBElement1() == B->getBElement1() &&
//        A->getBElement2() == B->getBElement2())   ||
//       (A->getBElement1() == B->getBElement2() &&
//        A->getBElement2() == B->getBElement1());
//}

SISink::SISink (int newid, RADIUS_TYPE stime, Point Or,
  BElementList blist, SILinksList pselist ) :
   SINode (newid, 4, DEGENERATE, stime)
{
  
   _type = SINK;
  _origin = Or;
  _bPropagated = true;
  _bActive = false; //vcl_sinks are always inactive
  _bPropagated = true; //vcl_sinks don't need to be propagated!

  bndList = blist;
   PSElementList = pselist;

  //add link from the boundary elements to this shock
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
     (*curB)->addBnd2ShockLink (this);
  }

  //add link from parents to this node
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS)
    (*curS)->set_cSNode(this);
}

SISink::~SISink(){}

SIJunct::SIJunct (int newid, RADIUS_TYPE stime, Point Or,
  BElementList blist, SILinksList pselist) :
   SINode (newid, 3, REGULAR, stime)
{
   _type = JUNCT;
  _origin = Or;
  bndList = blist;
   PSElementList = pselist;
   _cSLink = NULL;

  //add link from the boundary elements to this shock
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      (*curB)->addBnd2ShockLink (this);
  }

  //add link from parents to this node
  SILinksList::iterator curS = PSElementList.begin();
   for(; curS!=PSElementList.end(); ++curS) {
      (*curS)->set_cSNode(this);
  }
}

SIJunct::~SIJunct(){}

SIPointPoint::SIPointPoint (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1,  DEGENERATE, stime, lbe, rbe, pse)
{
   _type = POINTPOINT;
  _H = _distPointPoint (lBPoint()->pt(), rBPoint()->pt());
  assert (_H!=0);
   _u = _vPointPoint (lBPoint()->pt(), rBPoint()->pt());
   _n = angle0To2Pi (_u+M_PI_2); //ALWAYS, BY DEFINITION

  _ctype = PP_ONLY; //default completion type
  _origin = ((BPoint*)lbe)->pt(); 

  _lsvector = lsvector;
  _rsvector = rsvector;
  _LsTau = CCW(_u, lsvector);
  if (AisEq(_LsTau, 2*M_PI))  //Correct LsTau
    _LsTau = 0;

  _RsTau = CCW(_u+M_PI, rsvector);
  if (AisEq(_RsTau, 0))    //Correct RsTau
    _RsTau = 2*M_PI;

  //avoid inconsistencies in the left and right taus
  if (AisG(_LsTau, LTau(_RsTau))) 
    _RsTau = RTau(_LsTau);
  if (AisL(_RsTau, RTau(_LsTau))) 
    _LsTau = LTau(_RsTau);

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  //catch bigger bugs
  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();

   _LeTau = _maxLTau;
   _ReTau = _minRTau;

  //Extreme case of P-P in ArcThirdOrder cases:
  //ignore the Tau check...
  if (_LsTau >= M_PI_2) {
    _LeTau = _LsTau;
    return;
  }

  assert (_minLTau<=_LsTau && _LsTau<=_maxLTau);
  assert (_minRTau<=_RsTau && _RsTau<=_maxRTau);
}

// Always use left point as the _origin
Point SIPointPoint::getPtFromLTau (TAU_TYPE ltau)
{
   Point pt;
  DIST_TYPE d = _H/(2*vcl_cos(ltau));
  pt = _origin + rotateCCW(d*vcl_cos(ltau), d*vcl_sin(ltau), _u);
  
   return pt;
}

SIPointLineContact::SIPointLineContact (int newid, BElement* lbe, BElement* rbe) :
   SIContact (newid, lbe, rbe)
{
   _type = POINTLINECONTACT;
  _H = 0;
  _nu = (lbe->type()==BPOINT) ? 0 : 1;
  if (_nu==0) { //0:LeftPoint-RightLine
    _n = rBLine()->N();
    _LsTau = _n; //0 before
    _RsTau = 0;
    _origin = lBPoint()->pt();
  }
  else { //1:LeftLineRightPoint
    _n = lBLine()->N();
    _LsTau = lBLine()->L();
    _RsTau = _n; //2*M_PI before
    _origin = rBPoint()->pt();
  }

   _LeTau=_LsTau; //for contact shock, tau is consvcl_tant
   _ReTau=_RsTau; //for contact shock, tau is consvcl_tant

}

SIPointArcContact::SIPointArcContact(int newid, BElement* lbe, BElement* rbe) :
   SIContact (newid, lbe, rbe)
{
   _type = POINTARCCONTACT;
  _H = 0;
  _nu = (lbe->type()==BPOINT) ? 0 : 1;

   if (_nu==0) { //0:LeftPoint-RightArc
    _nud = rBArc()->nud();
    _n = _vPointPoint (rBArc()->center(), lBPoint()->pt());
      _R = rBArc()->R();

    _RsTau = _n; //1)one definition for both nud<1 >1 definition
    _origin = lBPoint()->pt();

    //2)Make _n always goes outward.
    if (_nud==-1)
      _n = angle0To2Pi (_n+M_PI);
    if (_n==2*M_PI) //special case
      _n = 0;
    _LsTau = _n;
   }
   else { //1:LeftArcRightPoint
    //_s = lBArc()->nud;
    //_n = _vPointPoint (lBArc()->center, rBPoint()->pt());
    //if (_s==-1)
    //  _n = angle0To2Pi (_n+M_PI);
    _nud = lBArc()->nud();
    _n = _vPointPoint (lBArc()->center(), rBPoint()->pt());
      _R = lBArc()->R();

    _LsTau = _n; //1)one definition for both nud<1 >1 definition
    _origin = rBPoint()->pt();

    //2)Make _n always goes outward.
    if (_nud==-1)
      _n = angle0To2Pi (_n+M_PI);
    if (_n==2*M_PI) //special case
      _n = 0;
    _RsTau = _n;
   }

   _LeTau=_LsTau; //for contact shock, tau is consvcl_tant
   _ReTau=_RsTau; //for contact shock, tau is consvcl_tant

}

SIPointLine::SIPointLine (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe,
                SINode* pse,
                LTAU_TYPE lsvector, LTAU_TYPE rsvector) :
   SILink (newid, 1,  SEMI_DEGENERATE, stime, lbe, rbe, pse)
{
  BPoint* bpoint;
  BLine*  bline;

   _type = POINTLINE;
  _lsvector = lsvector;
  _rsvector = rsvector;

   _nu = (lbe->type()==BPOINT) ? 1 : -1;
   if (_nu>0) {
    bpoint = lBPoint();
    bline =  rBLine();
    _n = bline->U();
    _u = angle0To2Pi (_n - M_PI_2);
    _l = bline->L();
    _delta = _deltaPointLine (bpoint->pt(), bline->start(), bline->end());

    _LsTau = CCW (_u, lsvector);
    if (AisEq(_LsTau, 2*M_PI))    //Correct LsTau
      _LsTau = 0;

    _RsTau = rsvector - _delta;  //Correct later
  }
  else {
      bpoint = rBPoint();
      bline =  lBLine();
    _n = bline->U();
    _u = angle0To2Pi (_n - M_PI_2);
    _l = bline->L();
    _delta = _deltaPointLine (bpoint->pt(), bline->start(), bline->end());

    _LsTau = _delta - lsvector;  //Correct later

    _RsTau = CCW (_u, rsvector);
    if (AisEq(_RsTau, 0))      //Correct RsTau
      _RsTau = 2*M_PI;
  }

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _H = bline->distPoint (bpoint->pt());
  assert (_H!=0);
  _origin = bpoint->pt();

  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();

   if (_nu>0) {
    correctRTau (_RsTau);

    _LeTau = _maxLTau;
    _ReTau = _maxRTau;
  }
  else {
    correctLTau (_LsTau);

    _LeTau = _maxLTau;
    _ReTau = _minRTau;
  }
}

LTAU_TYPE SIPointLine::RTau(LTAU_TYPE LTau)
{
  TAU_TYPE rtau;
  DIST_TYPE d;
  DIST_TYPE c = _H/2;

  if (_nu==1) {
    d = _H/(1+vcl_cos(LTau));  //d = 2*c/(1+vcl_cos(LTau));
    return vcl_fabs(d*vcl_sin(LTau));
  }
  else {
    //in this case LTau is expected to be negative by definition
    //but our definition uses all positive taus
    rtau = angle0To2Pi(vcl_acos(-LTau/vcl_sqrt(LTau*LTau + 4*c*c)) 
                      + vcl_atan(-2*c/LTau));
    return 2*M_PI - rtau;
  }
}

LTAU_TYPE SIPointLine::LTau(LTAU_TYPE RTau)
{
  TAU_TYPE ltau;
  DIST_TYPE d;
  DIST_TYPE c = _H/2;

  if (_nu==-1){
    d = _H/(1+vcl_cos(RTau));
    return vcl_fabs(d*vcl_sin(RTau));
  }
  else {
    //in this case RTau is expected to be positive
    ltau = angle0To2Pi(vcl_acos(-RTau/vcl_sqrt(RTau*RTau + 4*c*c)) 
                      + vcl_atan(-2*c/RTau));
    return ltau;
  }
}

bool SIPointLine::isTauValid_MinMax (LTAU_TYPE letau, LTAU_TYPE retau)
{
  if (_nu==1)
    return AisLEq(_minLTau,letau) && AisLEq(letau,_maxLTau) &&
         LisLEq(_minRTau,retau) && LisLEq(retau,_maxRTau);
  else
    return LisLEq(_minLTau,letau) && LisLEq(letau,_maxLTau) &&
         AisLEq(_minRTau,retau) && AisLEq(retau,_maxRTau);
}

//for Point-Line, always use Point tau
Point SIPointLine::getPtFromPointTau (TAU_TYPE ptau)
{
   Point pt;
  DIST_TYPE d = _H/(1+vcl_cos(ptau));
  pt = _origin + rotateCCW(d*vcl_cos(ptau), d*vcl_sin(ptau), _u);
  
   return pt;
}

Point SIPointLine::getLFootPt (TAU_TYPE ptau)
{
   if (_nu==1)
      return ((BPoint*)_lBElement)->pt();
   else
      return _getFootPt (getPtFromTau(ptau), ((BLine*)_lBElement)->start(), ((BLine*)_lBElement)->end());
}

Point SIPointLine::getRFootPt (TAU_TYPE ptau)
{
   if (_nu==-1)
      return ((BPoint*)_rBElement)->pt();
   else 
    return _getFootPt (getPtFromTau(ptau), ((BLine*)_rBElement)->start(), ((BLine*)_rBElement)->end());
}

SIPointArc::SIPointArc (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe, SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1,  DEGENERATE, stime, lbe, rbe, pse)
{
  BPoint* bpoint;
  BArc*   barc;

   _type = POINTARC;
  _lsvector = lsvector;
  _rsvector = rsvector;
   _nu = (lbe->type()==BPOINT) ? 1 : -1;
   if (_nu==1) {
    bpoint = lBPoint();
    barc   =  rBArc();
    _u = _vPointPoint (bpoint->pt(), barc->center());
    _nudl = 1;
    _nudr = barc->nud();
    _origin = bpoint->pt();

    _Sr = barc->_CCWStartVector;
    _Er = barc->_CCWEndVector;
    _Sl = 0; //Not used
    _El = 0;
  
    _As = barc->_StartVector;
    _Ae = barc->_EndVector;
  }
  else {
      bpoint = rBPoint();
      barc   =  lBArc();
    _u = _vPointPoint (barc->center(), bpoint->pt());
    _nudl = barc->nud();
    _nudr = 1;
    _origin = barc->center();

    _Sl = barc->_CCWStartVector;
    _El = barc->_CCWEndVector;
    _Sr = 0; //Not used
    _Er = 0;

    _As = barc->_StartVector;
    _Ae = barc->_EndVector;
  }

  _H = _distPointPoint(bpoint->pt(), barc->center());
  assert (_H!=0);
   if (_H>(Rl()+Rr()))  _s =  1;
   else            _s = -1;

  if (_s==1) {
    if (_nu==1)
      _case=1;
    else
      _case=2;
  }
  else {
    if (_nu==1)
      _case=3;
    else
      _case=4;
  }

   if (_s==1){
      _a = (Rl()-Rr())/2; //Could be negative!
      _c = _H/2;
      _b2 = _c*_c-_a*_a;
    _b = vcl_sqrt(_b2);

    _Asym = (_nu==1) ? vcl_atan2(_b,-vcl_fabs(_a)) : vcl_atan2(_b,_a);
    _Asym = angle0To2Pi (_Asym);

    if (_nu==1)
      _LAsym = _Asym;
    else
      _RAsym = _Asym+M_PI;
   }
   else {
      _a = (Rl()+Rr())/2;
      _c = _H/2;
      _b2 = _a*_a-_c*_c;
    _b = vcl_sqrt(_b2);
   }

  _LsTau = CCW (_u, lsvector);
  _RsTau = CCW (_u+M_PI, rsvector);

  if (_s==1) {
    if (AisEq(_LsTau, 2*M_PI)) //Correct LsTau
      _LsTau = 0;
    if (AisEq(_RsTau, 0)) //Correct RsTau
      _RsTau = 2*M_PI;
  }
  else {
    if (_nu==1) {
      if (AisEq(_LsTau, 2*M_PI)) //Correct LsTau
        _LsTau = 0;
      if (AisEq(_RsTau, 2*M_PI)) //Correct RsTau
        _RsTau = 0;
    }
    else {
      if (AisEq(_LsTau, 0)) //Correct LsTau
        _LsTau = 2*M_PI;
      if (AisEq(_RsTau, 0)) //Correct RsTau
        _RsTau = 2*M_PI;
    }
  }

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  assert (_minLTau <= _maxLTau); //'=' very degenerate case

  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();
  //EPSILONISSUE 26: unlikely, but possible that both are almost M_PI!
  if (_minRTau>_maxRTau) {
    double temp = _minRTau;
    _minRTau = _maxRTau;
    _maxRTau = temp;
  }
  assert (_minRTau <= _maxRTau);

  if (_s==1) {
    _LeTau = _maxLTau;
    _ReTau = _minRTau;
  }
  else {
    if (_nu==1) {
      _LeTau = _maxLTau;
      _ReTau = _maxRTau;
    }
    else {
      _LeTau = _minLTau;
      _ReTau = _minRTau;
    }
  }

  assert (_minLTau<=_LsTau && _LsTau<=_maxLTau);
  assert (_minRTau<=_RsTau && _RsTau<=_maxRTau);
}

DIST_TYPE SIPointArc::dFromLTau (TAU_TYPE Ltau)
{
  DIST_TYPE d;
  double denom;

  if (_s>0)
    denom = _c*vcl_cos(Ltau)-_a;
  else
    denom = _a-_c*vcl_cos(Ltau);

  if (_isEq(denom,0,1E-13))//(denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

DIST_TYPE SIPointArc::dFromRTau (TAU_TYPE Rtau)
{
  DIST_TYPE d;
  double denom;

  if (_s>0)
    denom = _a+_c*vcl_cos(Rtau);
  else
    denom = _a-_c*vcl_cos(Rtau);

  if (denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

//Input ltau, return rtau
TAU_TYPE SIPointArc::RTau (TAU_TYPE Ltau)
{  
  //1)EXTREMECASE: (Point inside Arc to form a Sink!)
  if (_s>0) {
    if (AisEq(Ltau, 0))
      return 2*M_PI;
    if (_nu==1)
      if (AisEq(Ltau, _LAsym))
        return _Asym+M_PI;
  }
  else {
    if (AisEq(Ltau, 2*M_PI))
      return M_PI;
    if (AisEq(Ltau, M_PI))
      return 0;
  }

  //2)Normal Case
  DIST_TYPE d = dFromLTau (Ltau);
  double m = ( d*vcl_sin(Ltau) )/( d*vcl_cos(Ltau)-_H );

  //For (_s>0) case, RTau is between (M_PI to 2*M_PI)
  //For (_s<0) case, RTau is between (0 to M_PI)
  int si = (_s>0) ? (m>0) : (m<0);

  TAU_TYPE rtau = angle0To2Pi (vcl_atan(m)+si*M_PI);

  return rtau;
}


TAU_TYPE SIPointArc::LTau (TAU_TYPE Rtau)
{  
  //1)EXTREMECASE: (Point inside Arc to form a Sink!)
  if (_s>0) {
    if (AisEq(Rtau, 2*M_PI))
      return 0;
    if (_nu==-1)
      if (AisEq(Rtau, _RAsym)) 
        return _Asym;
  }
  else {
    if (AisEq(Rtau, 0)) 
      return M_PI;
    if (AisEq(Rtau, M_PI)) 
      return 2*M_PI;
  }

  //2)Normal Case
  DIST_TYPE d = dFromRTau (Rtau);
  double m = ( d*vcl_sin(Rtau) )/( d*vcl_cos(Rtau)-_H );

  //For (_s>0) case, LTau is between (0 to M_PI)
  //For (_s<0) case, LTau is between (M_PI to 2*M_PI)
  int si = (_s>0) ? (m<0) : (m>0);

  TAU_TYPE ltau = angle0To2Pi (vcl_atan(m)+si*M_PI);

  return ltau;
}

bool SIPointArc::isLSTauValid ()
{
  bool result = true;
  if (_s==1) {
    if (_nu==1) { //Case 1:
      result = AisGEq(_LsTau,0) && AisLEq(_LsTau,_LAsym);
    }
    else { //Case 2:
      VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
      result = _validStartEnd0To2PiEPIncld (v, _Sl, _El);
    }
  }
  else {
    if (_nu==1) { //Case 3:
      result = AisGEq(_LsTau,M_PI) && AisLEq(_LsTau,2*M_PI);
    }
    else { //Case 4:
      VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
      result = _validStartEnd0To2PiEPIncld (v, _Sl, _El);
    }
  }
  //assert (result);
  return result;
}

bool SIPointArc::isRSTauValid ()
{
  bool result = true;
  if (_s==1) {
    if (_nu==1) { //Case 1:
      VECTOR_TYPE v = angle0To2Pi (_u+M_PI+_RsTau);
      //assert (_validStartEnd0To2PiEPIncld (v, _Sr, _Er));
      result = _validStartEnd0To2PiEPIncld (v, _Sr, _Er);
    }
    else { //Case 2:
      //assert (AisGEq(_RsTau,_RAsym) && AisLEq(_RsTau,2*M_PI));
      result = AisGEq(_RsTau,_RAsym) && AisLEq(_RsTau,2*M_PI);
    }
  }
  else {
    if (_nu==1) { //Case 3:
      VECTOR_TYPE v = angle0To2Pi (_u+M_PI+_RsTau);
      //assert (_validStartEnd0To2PiEPIncld (v, _Sr, _Er));
      result = _validStartEnd0To2PiEPIncld (v, _Sr, _Er);
    }
    else { //Case 4:
      //assert (AisGEq(_RsTau,0) && AisLEq(_RsTau,M_PI));
      result = AisGEq(_RsTau,0) && AisLEq(_RsTau,M_PI);
    }
  }
  //assert (result);
  return result;
}

TAU_TYPE SIPointArc::computeMinLTau ()
{
  if (_s==1) {
    if (_nu==1) //Case 1:
      return _LsTau;
    else //Case 2:
      return _LsTau;
  }
  else {
    if (_nu==1) //Case 3:
      return _LsTau;
    else { //Case 4:
      //See notes: Special issue on minLTau
      TAU_TYPE minltau = CCW(_u, _Sl);
      VECTOR_TYPE _u_plus_PI = angle0To2Pi(_u+M_PI);
      if (_validStartEnd0To2Pi(_u, _Sl, _El)) {
        if (_validStartEnd0To2Pi(_u_plus_PI, _Sl, _El)) {
          if (_distPointPoint(rBPoint()->pt(),lBArc()->end()) <
             _distPointPoint(rBPoint()->pt(),lBArc()->start())) //case 5
            return M_PI;
          else //case 1
            return (_LsTau>=minltau) ? minltau : M_PI;
        }
        else
          return minltau;
      }
      else {
        if (_validStartEnd0To2Pi(_u_plus_PI, _Sl, _El))
          return M_PI;
        else
          return minltau;
      }
    }
  }
}

TAU_TYPE SIPointArc::computeMaxLTau ()
{
  if (_s==1) {
    if (_nu==1) { //Case 1:
      //double _LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
      bool bIgnoreRightLimit = false;
      VECTOR_TYPE rvector = angle0To2Pi( RTau(_LAsym) + _u+M_PI );
      //If the converted LAsym is inside the right arc, ignore right limit.
      if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
        bIgnoreRightLimit = true;

      if (bIgnoreRightLimit)
        return _LAsym;
      else
        return LTau(CCW(_u+M_PI, _Sr));
    }
    else { //Case 2:
      //double _RAsym = vcl_atan2 (_b, _a) +M_PI;
      bool bIgnoreLeftLimit = false;
      VECTOR_TYPE lvector = angle0To2Pi( LTau(_RAsym) + _u );
      //If the converted RAsym is inside the left arc, ignore left limit.
      if (_validStartEnd0To2Pi(lvector, _Sl, _El))
        bIgnoreLeftLimit = true;

      if (bIgnoreLeftLimit)
        return LTau(_RAsym);
      else
        return CCW(_u, _El); //_Er:wrong
    }
  }
  else {
    if (_nu==1) //Case 3:
      return LTau(computeMaxRTau());
    else //Case 4:
      return _LsTau;
  }
}

TAU_TYPE SIPointArc::computeMinRTau ()
{  
  if (_s==1) {
    if (_nu==1) { //Case 1:
      //double LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
      bool bIgnoreRightLimit = false;
      VECTOR_TYPE rvector = angle0To2Pi( RTau(_LAsym) + _u+M_PI );
      //If the converted LAsym is inside the right arc, ignore right limit.
      if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
        bIgnoreRightLimit = true;

      if (bIgnoreRightLimit)
        return RTau(_LAsym);
      else
        return CCW(_u+M_PI, _Sr);
    }
    else { //Case 2:
      //double RAsym = vcl_atan2(_b, _a) +M_PI;
      bool bIgnoreLeftLimit = false;
      VECTOR_TYPE lvector = angle0To2Pi( LTau(_RAsym) + _u );
      // the converted RAsym is inside the left arc, ignore left limit.
      if (_validStartEnd0To2Pi(lvector, _Sl, _El))
        bIgnoreLeftLimit = true;

      if (bIgnoreLeftLimit)
        return _RAsym;
      else
        return RTau(CCW(_u, _El));
    }
  }
  else {
    if (_nu==1) //Case 3:
      return _RsTau;
    else //Case 4:
      return RTau(computeMinLTau());
  }
}

TAU_TYPE SIPointArc::computeMaxRTau ()
{
  if (_s==1) {
    if (_nu==1) //Case 1:
      return _RsTau;
    else //Case 2:
      return _RsTau;
  }
  else {
    if (_nu==1) { //Case 3:
      //See notes: Special issue on maxrRTau
      ANGLE_TYPE maxrtau = CCW(_u+M_PI, _Er);
      VECTOR_TYPE _u_plus_PI = angle0To2Pi(_u+M_PI);
      if (_validStartEnd0To2Pi(_u, _Sr, _Er)) {
        if (_validStartEnd0To2Pi(_u_plus_PI, _Sr, _Er)) {
          if (_distPointPoint(lBPoint()->pt(),rBArc()->start()) <
             _distPointPoint(lBPoint()->pt(),rBArc()->end())) //case 5
            return M_PI;
          else //case 1
            return (_RsTau<=maxrtau) ? maxrtau : M_PI;
        }
        else
          return M_PI;
      }
      else {
        if (_validStartEnd0To2Pi(_u_plus_PI, _Sr, _Er))
          return maxrtau;
        else
          return maxrtau;
      }
    }
    else //Case 4:
      return _RsTau;
  }
}

TAU_TYPE SIPointArc::getLTauFromTime (RADIUS_TYPE time)
{
  DIST_TYPE d;
  TAU_TYPE ltau;

  if (_s>0) {
    d = time + Rl();
    ltau = vcl_acos ( (_a*d+_b2)/d/_c );
  }
  else {
    d = Rr() - time;
    ltau = vcl_acos ( (_a*d-_b2)/d/_c );
  }

  return ltau;
}

Point SIPointArc::getPtFromLTau (TAU_TYPE tau)
{
   //assuming tau is always left tau
   Point pt;

  assert (tau>=0);
   DIST_TYPE d = dFromLTau (tau);

  pt = _origin + rotateCCW(d*vcl_cos(tau), d*vcl_sin(tau), _u);

   return pt;
}

//always use left tau
Point SIPointArc::getLFootPt (TAU_TYPE ltau)
{
   if (_nu==1)
      return ((BPoint*)_lBElement)->pt();
   else
      return _vectorPoint (((BArc*)_lBElement)->center(), angle0To2Pi(_u+ltau), ((BArc*)_lBElement)->R());
}

//always use the svcl_tandard tau
Point SIPointArc::getRFootPt (TAU_TYPE tau)
{
  //always use the right tau
  double rtau = RTau(tau);

   if (_nu==-1)
      return ((BPoint*)_rBElement)->pt();
   else 
    return _vectorPoint (((BArc*)_rBElement)->center(), angle0To2Pi(_u+M_PI+rtau), ((BArc*)_rBElement)->R());
}

SILineLine::SILineLine (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1, REGULAR, stime, lbe, rbe, pse)
{
   _type = LINELINE;
  _lsvector = lsvector;
  _rsvector = rsvector;

   _Al = ((BLine *)lbe)->start();
   _Bl = ((BLine *)lbe)->end();
   _Ar = ((BLine *)rbe)->start();
   _Br = ((BLine *)rbe)->end();

   _lL = _distPointPoint (_Al, _Bl);
   _lR = _distPointPoint (_Ar, _Br);

   _nl = angle0To2Pi (vcl_atan2(_Bl.y - _Al.y, _Bl.x - _Al.x));
   _ul = angle0To2Pi (_nl - M_PI_2);
   _nr = angle0To2Pi (vcl_atan2(_Br.y - _Ar.y, _Br.x - _Ar.x));
   _ur = angle0To2Pi (_nr - M_PI_2);
  //_sigma = (_Bl.x - _Al.x)*(_Br.x - _Ar.x) + (_Bl.y - _Al.y)*(_Br.y - _Ar.y);
   _sigma = vcl_cos(_nl)*vcl_cos(_nr) + vcl_sin(_nl)*vcl_sin(_nr); // nl dot nr

  _thetaL = CCW (_ul, _nr);
  _thetaR = CCW (_ur, _nl);

  //Need new formula for this special case:
   _phi = CCW (_nr, _nl+M_PI)/2;
  if (_phi>M_PI_2) { //Extreme case!
    _phiSpecialCase = true;
    vcl_cout<< "phi special case for SILineLine. error= " <<_phi-M_PI_2<<vcl_endl;
    _phi = M_PI_2; //A_EPSILON
     _H = _distPointPoint (_Al, _Ar);
    assert (_H!=0);

    ANGLE_TYPE alphaL = CCW (_ul, vcl_atan2(_Ar.y - _Al.y , _Ar.x - _Al.x));
    ANGLE_TYPE alphaR = CCW (_ur, vcl_atan2(_Al.y - _Ar.y , _Al.x - _Ar.x));
    _deltaL = _H*vcl_sin(alphaL - _thetaL)/vcl_cos(_thetaL);
    if (_deltaL < _lL)
      _deltaL = _lL;
    _deltaR = _H*vcl_sin(alphaR - _thetaR)/vcl_cos(_thetaR);
    if (_deltaR < _lR)
      _deltaR = _lR;

    _N1L = -vcl_tan(_phi);  //-ISHOCK_DIST_HUGE
    _N1R = vcl_tan(_phi);    //+ISHOCK_DIST_HUGE
    
    _N2L = -_H*vcl_cos(alphaL+2*_phi)/(2*vcl_cos(_phi)*vcl_cos(_phi));  //ISHOCK_DIST_HUGE
    _N2R = -_H*vcl_cos(2*_phi - alphaR)/(2*vcl_cos(_phi)*vcl_cos(_phi));  //ISHOCK_DIST_HUGE
  }
  else { //Normal case.
    _phiSpecialCase = false;

     _H = _distPointPoint (_Al, _Ar);
    assert (_H!=0);

    ANGLE_TYPE alphaL = CCW (_ul, vcl_atan2(_Ar.y - _Al.y , _Ar.x - _Al.x));
    ANGLE_TYPE alphaR = CCW (_ur, vcl_atan2(_Al.y - _Ar.y , _Al.x - _Ar.x));
  
    _deltaL = _H*vcl_sin(alphaL - _thetaL)/vcl_cos(_thetaL);
    _deltaR = _H*vcl_sin(alphaR - _thetaR)/vcl_cos(_thetaR);
  
    _N1L = -vcl_tan(_phi);
    _N1R = vcl_tan(_phi);
    
    _N2L = -_H*vcl_cos(alphaL+2*_phi)/(2*vcl_cos(_phi)*vcl_cos(_phi));
    _N2R = -_H*vcl_cos(2*_phi - alphaR)/(2*vcl_cos(_phi)*vcl_cos(_phi));
  }

  //_origin is the S (intersection of two line?)
  _origin.x = _Al.x - 0.5*_H*vcl_cos(_ul);
  _origin.y = _Al.y - 0.5*_H*vcl_sin(_ul);

  _LsTau = lsvector;
  _RsTau = rsvector;

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();



  correctLTau (_LsTau);
  correctRTau (_RsTau);

   _LeTau = _minLTau;
   _ReTau = _maxRTau;
}

DIST_TYPE SILineLine::d (TAU_TYPE ltau)
{
  return _N1L*ltau + _N2L;
}

TAU_TYPE SILineLine::RTau(TAU_TYPE Ltau)
{
  if (_phiSpecialCase) {
    return Ltau;
  }
  TAU_TYPE ltau = (_N1L*Ltau + _N2L - _N2R)/_N1R;
  if (LisEq(ltau, 0))
    ltau = 0;
  if (LisEq(ltau, _lL))
    ltau = _lL;
  return ltau;
}

bool SILineLine::isTauValid_MinMax (TAU_TYPE letau, TAU_TYPE retau)
{
  return LisLEq(_minLTau,letau) && LisLEq(letau,_maxLTau) &&
       LisLEq(_minRTau,retau) && LisLEq(retau,_maxRTau);
}

TAU_TYPE SILineLine::LTau(TAU_TYPE Rtau)
{
  if (_phiSpecialCase) {
    return Rtau;
  }
  TAU_TYPE rtau = (_N1R*Rtau + _N2R - _N2L)/_N1L;
  if (LisEq(rtau, 0))
    rtau = 0;
  if (LisEq(rtau, _lR))
    rtau = _lR;
  return rtau;
}

Point SILineLine::getPtFromLTau (TAU_TYPE tau)
{
   Point pt;

  //Here we compute the point directly, without reference to _origin
  RADIUS_TYPE radius = r(tau);
  pt.x = _Al.x + tau*vcl_cos(_nl) - radius*vcl_cos(_ul);
  pt.y = _Al.y + tau*vcl_sin(_nl) - radius*vcl_sin(_ul);
   return pt;
}

SILineArc::SILineArc (int newid, RADIUS_TYPE stime,
                BElement* lbelm, BElement* rbelm,
                SINode* pse,
                LTAU_TYPE lsvector, LTAU_TYPE rsvector) :
   SILink (newid, 1,  REGULAR, stime, lbelm, rbelm, pse)
{
   BLine* bline;
   BArc* barc;

   _type = LINEARC;
  _lsvector = lsvector;
  _rsvector = rsvector;

   if (lbelm->type()==BARC) {
      bline = (BLine*)rbelm;
      barc = (BArc*)lbelm;
    _nu = 1;
   }
   else {
      bline = (BLine*)lbelm;
      barc = (BArc*)rbelm;
    _nu = -1;
   }

  _CCWAs = barc->_CCWStartVector;
  _CCWAe = barc->_CCWEndVector;

  _As = barc->_StartVector;
  _Ae = barc->_EndVector;

  _origin = barc->center();
  _foot = _getFootPt (barc->center(), bline->start(), bline->end());
  _H = _distPointPoint(_foot, barc->center());
  if (_H!=0)
    _u = _vPointLine (barc->center(), bline->start(), bline->end());
  else //Special cases:
    _u = bline->N();
  _nud = barc->nud();
   _delta = _deltaPointLine (barc->center(), bline->start(), bline->end());
   _l = bline->L();
   _R = barc->R();

  //!!!New 4 special cases: assert (_H!=0);
   if (_angle_vector_dot(_u, bline->N())<0) _s = 1;
   else                 _s = -1;

  //For special cases: we still keep _s=-1 (not 0)
  if (_s*_nud <0) _n = angle0To2Pi (_u - M_PI_2);
  else            _n = angle0To2Pi (_u + M_PI_2);

  //EPSILONISSUE 17
  //For cotangent Line-Arc that (_s*_nud==-1),
  //_R > _H and _c>0
  //R is more accurate than H, set H less than R
  if (_s*_nud <0 && _R<_H)
    _H = _R-A_EPSILON;

  //this still applies.
  _c = (_R + (_s*_nud)*_H)/2;

  assert (_c>0);

  _delta = _deltaPointLine (barc->center(), bline->start(), bline->end());

  if (_nud==1) {
    if (_s==1) {
      if (_nu==1) {
        _case = 1;
        _LsTau = CCW (_u, lsvector);
        if (AisEq(_LsTau, 2*M_PI)) //Correct LsTau
          _LsTau = 0;
        _RsTau = rsvector - _delta;
      }
      else {
        _case = 2;
        _LsTau = _delta - lsvector;
        _RsTau = CCW (_u, rsvector);
        if (AisEq(_RsTau, 0)) //Correct RsTau
          _RsTau = 2*M_PI;
      }
    }
    else if (_H!=0) {
      if (_nu==1) {
        _case = 3;
        _LsTau = CCW (_u, lsvector);
        if (AisEq(_LsTau, 0)) //Correct LsTau
          _LsTau = 2*M_PI;
        _RsTau = rsvector - _delta;
      }
      else {
        _case = 4;
        _LsTau = _delta - lsvector;
        _RsTau = CCW (_u, rsvector);
        if (AisEq(_RsTau, 2*M_PI)) //Correct RsTau
          _RsTau = 0;
      }
    }
    else { //_H==0
      if (_nu==1) {
        _case = 9;
        _LsTau = CCW (_u, lsvector);
        _RsTau = rsvector - _delta;
      }
      else {
        _case = 10;
        _LsTau = _delta - lsvector;
        _RsTau = CCW (_u, rsvector);
      }
    }
  }
  else {
    if (_s==1) {
      if (_nu==1) {
        _case = 5;
        _LsTau = CCW (_u, lsvector);
        _RsTau = _delta - rsvector;
      }
      else {
        _case = 6;
        _LsTau = lsvector - _delta;
        _RsTau = CCW (_u, rsvector);
      }
    }
    else {
      if (_nu==1) { //Case 7, 11
        if (_H!=0)  _case = 7;
        else      _case = 11;
        _LsTau = CCW (_u, lsvector);
        _RsTau = _delta - rsvector;
      }
      else { //Case 8, 12
        if (_H!=0)  _case = 8;
        else      _case = 12;
        _LsTau = lsvector - _delta;
        _RsTau = CCW (_u, rsvector);
      }
    }
  }

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  assert (_minLTau <= _maxLTau);
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();
  assert (_minRTau <= _maxRTau);

  //Setup EndTaus.....
   if (_nud==1) {
    if (_s==1) {
         if (_nu==1) { //Case 1:
            _LeTau = _maxLTau;
            _ReTau = _maxRTau;
      }
      else { //Case 2:
            _LeTau = _maxLTau;
            _ReTau = _minRTau;
      }
      }
    else {
         if (_nu==1) { //Case 3, 9:
            _LeTau = _maxLTau;
            _ReTau = _maxRTau;
      } 
      else { //Case 4, 10:
            _LeTau = _maxLTau;
            _ReTau = _minRTau;
      }
    }
   }
   else { //nud==-1
      if (_s==1) {
      if (_nu==1) { //Case 5:
            _LeTau = _minLTau;
            _ReTau = _minRTau;
         }
      else { //Case 6:
            _LeTau = _minLTau;
            _ReTau = _maxRTau;
         }
      }
      else {
      if (_nu==1) { //Case 7, 11:
            _LeTau = _minLTau;
            _ReTau = _minRTau;
         }
      else { //Case 8, 12:
            _LeTau = _minLTau;
            _ReTau = _maxRTau;
         }
      }
   }
}

DIST_TYPE SILineArc::d (TAU_TYPE ptau) 
{
  return (_R+(_nud*_s)*_H)/(1+(_nud*_s)*vcl_cos(ptau));
}

LTAU_TYPE SILineArc::RTau (LTAU_TYPE LTau)
{
  LTAU_TYPE rtau;
  DIST_TYPE d;
   double alpha;

  if (_nu==1){
    //!!!!bug if (vcl_cos(LTau)==1)
    d = 2*_c/(1+(_s*_nud)*vcl_cos(LTau));
    assert (d>=0);
    return vcl_fabs(d*vcl_sin(LTau));
  }
  else {
    if (LTau==0) {
      return 2*M_PI; //M_PI;
    }
    else {
      if (LTau>0) alpha = vcl_atan(-2*_c/LTau);
        else        alpha = vcl_atan(-2*_c/LTau) + M_PI;
  
      rtau = angle0To2Pi(vcl_acos(-LTau/vcl_sqrt(LTau*LTau + 4*_c*_c)) 
                        + alpha);
      if (_s*_nud==1)
         return 2*M_PI - rtau;
      else
        return M_PI - rtau;
    }
  }
}

LTAU_TYPE SILineArc::LTau (LTAU_TYPE RTau)
{
  LTAU_TYPE ltau;
  DIST_TYPE d;
   ANGLE_TYPE alpha;

  if (_nu==-1){
    d = 2*_c/(1+(_s*_nud)*vcl_cos(RTau));
    return vcl_fabs(d*vcl_sin(RTau));
  }
  else {
    if (RTau==0) {
      return 0;
    }
    else {
      if (RTau>0) alpha = vcl_atan(-2*_c/RTau);
        else        alpha = vcl_atan(-2*_c/RTau) + M_PI;
  
      ltau = angle0To2Pi(vcl_acos(-RTau/vcl_sqrt(RTau*RTau + 4*_c*_c)) 
                        + alpha);
      if (_s*_nud==1)
         return ltau;
      else
        return ltau+M_PI;
    }
  }
}

bool SILineArc::isLSTauValid ()
{
  bool result=true;

  //1)Limit check on angle taus only
  switch (_case) {
  case 1: if(_LsTau>M_PI) result=false; break;
  case 3: if(_LsTau<M_PI_2) result=false; break;
  case 5: if(_LsTau<M_PI) result=false; break;
  case 7: if(_LsTau>M_PI_2) result=false; break;
  }
  if (result==false) return result;

  //2)Boundary Check
   if (_nud==1) {
      if (_nu==1) { //Case 1, 3:
      VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
      result = _validStartEnd0To2PiEPIncld (v, _CCWAs, _CCWAe);
    }
    else { //Case 2, 4:
      result = LisGEq(_delta-_LsTau,0) && LisLEq(_delta-_LsTau,_l);
    }
  }
  else {
      if (_nu==1) { //Case 5, 7:
      VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
      result = _validStartEnd0To2PiEPIncld (v, _CCWAs, _CCWAe);  
    }
    else { //Case 6, 8:
      result = LisGEq(_delta+_LsTau,0) && LisLEq(_delta+_LsTau,_l);
    }
  }
  //assert (result);
  return result;
}

bool SILineArc::isRSTauValid ()
{
  bool result=true;

  //1)Limit check on angle taus only
  switch (_case) {
  case 2: if(_RsTau<M_PI) result=false; break;
  case 4: if(_RsTau>M_PI_2) result=false; break;
  case 6: if(_RsTau>M_PI) result=false; break;
  case 8: if(_RsTau<3*M_PI_2) result=false; break;
  }
  if (result==false) return result;

  //2)Boundary Check
   if (_nud==1) {
      if (_nu==1) { //Case 1, 3:
      result = LisGEq(_RsTau+_delta,0) && LisLEq(_RsTau+_delta,_l);
    }
    else { //Case 2, 4:
      VECTOR_TYPE v = angle0To2Pi (_u+_RsTau);
      result = _validStartEnd0To2PiEPIncld (v, _CCWAs, _CCWAe);
    }
  }
  else {
      if (_nu==1) { //Case 5, 7:
      result = LisGEq(_delta-_RsTau,0) && LisLEq(_delta-_RsTau,_l);
    }
    else { //Case 6, 8:
      VECTOR_TYPE v = angle0To2Pi (_u+_RsTau);
      result = _validStartEnd0To2PiEPIncld (v, _CCWAs, _CCWAe);
    }
  }
  //assert (result);
  return result;
}

LTAU_TYPE SILineArc::computeMinLTau ()
{
  if (_nud==1) {
    if (_s==1) {
      if (_nu==1) //Case 1:
        return _LsTau;
      else //Case 2:
        return _LsTau;
    }
    else {
      if (_nu==1) //Case 3, 9:
        return _LsTau;
      else //Case 4, 10:
        return _LsTau;
    }
  }
  else {
    if (_s==1) {
      if (_nu==1) { //Case 5:
        double min_on_arc, min_on_line;
        if (CCW(_u, _CCWAs) < _LsTau) //5-1:
          min_on_arc =   vnl_math_max ( M_PI, CCW(_u, _CCWAs) );
        else { //5-2: 
          if (CCW(_u, _CCWAs)>M_PI && CCW(_u, _CCWAe)>CCW(_u, _CCWAs))
            min_on_arc =  vnl_math_max ( M_PI, CCW(_u, _CCWAs) );
          else
            min_on_arc = M_PI;
        }
        if (_delta-_l<0)
          min_on_line = 0;
        else
          min_on_line = LTau( vnl_math_max (_delta-_l, 0.0));
        return vnl_math_max (min_on_arc, min_on_line);
      }
      else { //Case 6:
        double min_on_arc;
        if (CCW(_u, _CCWAe) > _RsTau) //6-1:
          min_on_arc = LTau(  vnl_math_min (M_PI, CCW(_u, _CCWAe)) );
        else { //6-2:
          if (CCW(_u, _CCWAe)<M_PI && CCW(_u, _CCWAe)>CCW(_u, _CCWAs))
            min_on_arc = LTau(  vnl_math_min (M_PI, CCW(_u, _CCWAe)) );
          else
            min_on_arc = 0;
        }
        double min_on_line =  vnl_math_max (-_delta, 0.0);
        return vnl_math_max (min_on_arc, min_on_line);
      }
    }
    else {
      if (_nu==1) { //Case 7, 11:
        double min_on_arc;
        if (CCW(_u, _CCWAe) < _LsTau) //7-1:
          min_on_arc = CCW(_u, _CCWAs);
        else { //7-2:
          if (CCW(_u, _CCWAs) < CCW(_u, _CCWAe))
            min_on_arc = CCW(_u, _CCWAs);
          else
            min_on_arc = 0;
        }
        double min_on_line =  vnl_math_max (_delta-_l, 0.0);
        return vnl_math_max (min_on_arc, LTau(min_on_line));
      }
      else { //Case 8, 12:
        double min_on_arc;
        if (CCW(_u, _CCWAe) > _RsTau) //8-1:
          min_on_arc = LTau (CCW(_u, _CCWAe));
        else { //8-2:
          if (CCW(_u, _CCWAs) < CCW(_u, _CCWAe))
            min_on_arc = LTau (CCW(_u, _CCWAe));
          else
            min_on_arc = 0;
        }
        double min_on_line =  vnl_math_max (-_delta, 0.0);
        return vnl_math_max (min_on_arc, min_on_line);
      }
    }
  }
}

LTAU_TYPE SILineArc::computeMaxLTau ()
{
  if (_nud==1) {
    if (_s==1) {
      if (_nu==1) { //Case 1:
        if (_validStartEnd0To2PiEPIncld(angle0To2Pi(_u+M_PI), angle0To2Pi(_u+_LsTau), _CCWAe)) //1-1: no left limit
          return LTau (_l-_delta);
        else //1-2:
          return  vnl_math_min ( CCW(_u, _CCWAe), LTau(_l-_delta) );
      }
      else { //Case 2:
        if (_validStartEnd0To2PiEPIncld(angle0To2Pi(_u+M_PI), _CCWAs, angle0To2Pi(_u+_RsTau))) //2-1: no right limit
          return _delta;
        else //2-2:
          return vnl_math_min ( _delta, LTau(CCW(_u, _CCWAs)) );
      }
    }
    else {
      if (_nu==1) { //Case 3, 9: nud=1,s=-1,nu=1
        if (_validStartEnd0To2PiEPIncld(_u, angle0To2Pi(_u+_LsTau), _CCWAe)) //3-1: no left limit
          return LTau (_l-_delta);
        else //3-2:
          return  vnl_math_min ( LTau(_l-_delta), CCW(_u, _CCWAe) );
      } //Case 4, 10:
      else { //nud=1,s=-1,nu=-1
        if (_validStartEnd0To2PiEPIncld(_u, _CCWAs, angle0To2Pi(_u+_RsTau))) //4-1: no right limit
          return _delta;
        else //4-2:
          return  vnl_math_min ( _delta, LTau(CCW(_u, _CCWAs)) );
      }
    }
  }
  else {
    if (_s==1) {
      if (_nu==1) //Case 5:
        return _LsTau;
      else //Case 6:
        return _LsTau;
    }
    else {
      if (_nu==1) //Case 7, 11:
        return _LsTau;
      else //Case 8, 12:
        return _LsTau;
    }
  }
}

LTAU_TYPE  SILineArc::computeMinRTau ()
{
  if (_nud==1) {
    if (_s==1) {
      if (_nu==1) //Case 1:
        return _RsTau;
      else { //Case 2:
        if (_validStartEnd0To2PiEPIncld(angle0To2Pi(_u+M_PI), _CCWAs, angle0To2Pi(_u+_RsTau))) //2-1: no right limit
          return RTau(_delta);
        else //2-2:
          return  vnl_math_max ( RTau(_delta), CCW(_u, _CCWAs) );
      }
    }
    else {
      if (_nu==1) //Case 3, 9:
        return _RsTau;
      else { //Case 4, 10:
        if (_validStartEnd0To2PiEPIncld(_u, _CCWAs, angle0To2Pi(_u+_RsTau))) //4-1: no left limit
          return RTau(_delta);
        else //4-2:
          return  vnl_math_max ( RTau(_delta), CCW(_u,_CCWAs) );
      }
    }
  }
  else {
    if (_s==1) {
      if (_nu==1) { //Case 5:
        double min_on_arc;
        if (CCW(_u, _CCWAs) < _LsTau) //5-1: 
          min_on_arc = RTau(  vnl_math_max (M_PI, CCW(_u, _CCWAs)) );
        else { //5-2:
          if (CCW(_u, _CCWAs)>M_PI && CCW(_u, _CCWAe)>CCW(_u, _CCWAs))
            min_on_arc = RTau(  vnl_math_max (M_PI, CCW(_u, _CCWAs)) );
          else
            min_on_arc = 0;
        }
        double min_on_line =  vnl_math_max (_delta-_l, 0.0);
        return vnl_math_max (min_on_arc, min_on_line);
      }
      else //Case 6:
        return _RsTau;
    }
    else {
      if (_nu==1) { //Case 7, 11:
        double min_on_arc;
        if (CCW(_u, _CCWAe) < _LsTau) //7-1:
          min_on_arc = RTau (CCW(_u, _CCWAs));
        else { //7-2:
          if (CCW(_u, _CCWAs) < CCW(_u, _CCWAe))
            min_on_arc = RTau( CCW(_u, _CCWAs) );
          else
            min_on_arc = 0;
        }
        double min_on_line =  vnl_math_max (_delta-_l, 0.0);
        return vnl_math_max (min_on_arc, min_on_line);
      }
      else //Case 8, 12:
        return _RsTau;
    }
  }
}

LTAU_TYPE  SILineArc::computeMaxRTau ()
{
  if (_nud==1) {
    if (_s==1) {
      if (_nu==1) { //Case 1:
        if (_validStartEnd0To2PiEPIncld(angle0To2Pi(_u+M_PI), angle0To2Pi(_u+_LsTau), _CCWAe)) //1-1: no left limit
          return _l-_delta;
        else //1-2:
          return vnl_math_min (RTau(CCW(_u, _CCWAe)), _l-_delta);
      }
      else //Case 2:
        return _RsTau;
    }
    else {
      if (_nu==1) {  //Case 3, 9:
        if (_validStartEnd0To2PiEPIncld(_u, angle0To2Pi(_u+_LsTau), _CCWAe)) //3-1: no left limit
          return _l-_delta;
        else //3-2:
          return  vnl_math_min ( _l-_delta, RTau(CCW(_u, _CCWAe)) );
      }
      else //Case 4, 10:
        return _RsTau;
    }
  }
  else {
    if (_s==1) {
      if (_nu==1) //Case 5:
        return _RsTau;
      else { //Case 6:
        double max_on_arc;
        if (CCW(_u, _CCWAe) > _RsTau) //6-1: 
          max_on_arc = vnl_math_min (M_PI, CCW(_u, _CCWAe));
        else { //6-2:
          if (CCW(_u, _CCWAe)<M_PI && CCW(_u, _CCWAe)>CCW(_u, _CCWAs))
            max_on_arc = vnl_math_min (M_PI, CCW(_u, _CCWAe));
          else
            max_on_arc = M_PI;
        }
        double min_on_line =  vnl_math_max (-_delta, 0.0);
        return vnl_math_min (max_on_arc, RTau(min_on_line));
      }
    }
    else {
      if (_nu==1) //Case 7, 11:
        return _RsTau;
      else { //Case 8, 12:
        double max_on_arc, max_on_line;
        if (CCW(_u, _CCWAe) > _RsTau) //8-1:
          max_on_arc = CCW(_u, _CCWAe);
        else { //8-2:
          if (CCW(_u, _CCWAs) < CCW(_u, _CCWAe))
            max_on_arc = CCW(_u, _CCWAe);
          else
            max_on_arc = 2*M_PI;
        }
        if (_delta>0)
          max_on_line = 2*M_PI;
        else
          max_on_line = RTau( vnl_math_max (-_delta, 0.0));
        return vnl_math_min (max_on_arc, max_on_line);
      }
    }
  }
}

bool SILineArc::isTauValid_LA (LTAU_TYPE letau, LTAU_TYPE retau)
{
  bool result_minmax = isTauValid_MinMax (letau, retau);
  /*
  bool result=true;
  VECTOR_TYPE arc_start_vector, arc_end_vector;
  VECTOR_TYPE start_vector , end_vector;

  /////////////////////////////////////
   //1)Check the Leftside

   //if point return with 1
   if (_nu==1){
    if (_nud==-1){
      arc_start_vector = ((BArc*)_lBElement)->_StartVector;
      arc_end_vector = ((BArc*)_lBElement)->_EndVector;
    }
    else {
      arc_start_vector = ((BArc*)_lBElement)->_EndVector;
      arc_end_vector = ((BArc*)_lBElement)->_StartVector;
    }

    if (_nu*_nud>0){
      start_vector = angle0To2Pi(_LsTau + _u);
      end_vector = angle0To2Pi(letau + _u);
    }
    else {
      start_vector = angle0To2Pi(letau + _u);
      end_vector = angle0To2Pi(_LsTau + _u);
    }

    //adjust vectors for comparision
    if (AisG(arc_start_vector, arc_end_vector)) arc_start_vector -= 2*M_PI;
    if (AisG(start_vector, arc_end_vector))     start_vector -= 2*M_PI;
    if (AisG(end_vector, arc_end_vector))      end_vector -= 2*M_PI;

    if (!(AisLEq(arc_start_vector, start_vector) &&
         AisLEq(start_vector, end_vector) &&
        AisLEq(end_vector, arc_end_vector))
      )
      result = false;
   }
  else {
    //check left line
    double lA = letau*_nu*_nud + _delta;
    
    if (LisL(lA,0) || LisG(lA, _l))
         result = false;
  }

  //now check the right side

   if (_nu==-1){
    if (_nud==-1){
      arc_start_vector = ((BArc*)_rBElement)->_StartVector;
      arc_end_vector = ((BArc*)_rBElement)->_EndVector;
    }
    else {
      arc_start_vector = ((BArc*)_rBElement)->_EndVector;
      arc_end_vector = ((BArc*)_rBElement)->_StartVector;
    }
    
    if (_nu*_nud>0){
      start_vector = angle0To2Pi(_RsTau + _u);
      end_vector = angle0To2Pi(retau + _u);
    }
    else {
      start_vector = angle0To2Pi(retau + _u);
      end_vector = angle0To2Pi(_RsTau + _u);
    }

    //adjust vectors for comparision
    if (AisG(arc_start_vector, arc_end_vector)) arc_start_vector -= 2*M_PI;
    if (AisG(start_vector, arc_end_vector))     start_vector -= 2*M_PI;
    if (AisG(end_vector, arc_end_vector))      end_vector -= 2*M_PI;

    if (!(AisLEq(arc_start_vector, start_vector) &&
         AisLEq(start_vector, end_vector) &&
        AisLEq(end_vector, arc_end_vector))
      )
      result = false;
   }

  else { 
    //check right line
    double lA = retau*_nu*_nud + _delta;
    
    if (LisL(lA,0) || LisG(lA, _l))
         result = false;
  }
  //EPSILONISSUE 21
  ////////!!!!!!assert (result_minmax == result);
  */
   return result_minmax; //result;result_minmax
}

TAU_TYPE SILineArc::getPointTauFromTime (RADIUS_TYPE time)
{
  TAU_TYPE ptau;
  //ptau = vcl_acos ( (2*_c-time)*(_s*_nud)/time );
  ptau = vcl_acos (-time/(time+2*_c));
  return (_nu==1) ? ptau : 2*M_PI-ptau;
}

bool SILineArc::isTauValid_MinMax (LTAU_TYPE letau, LTAU_TYPE retau)
{
  if (_nu==1)
    return AisLEq(_minLTau,letau) && AisLEq(letau,_maxLTau) &&
         LisLEq(_minRTau,retau) && LisLEq(retau,_maxRTau);
  else
    return LisLEq(_minLTau,letau) && LisLEq(letau,_maxLTau) &&
         AisLEq(_minRTau,retau) && AisLEq(retau,_maxRTau);
}

//for Line-Arc, always use arc tau
Point SILineArc::getPtFromPointTau (TAU_TYPE ptau)
{
   DIST_TYPE c2 = _c*2;
   DIST_TYPE d = c2/(1+(_s*_nud)*vcl_cos(ptau));
   Point pt = _origin + rotateCCW(d*vcl_cos(ptau),d*vcl_sin(ptau), _u);

   return pt;
}

Point SILineArc::getLFootPt (TAU_TYPE ptau)
{
   if (_nu==1)
      return _vectorPoint (lBArc()->center(), angle0To2Pi(_u+ptau), lBArc()->R());
   else {
      //return _getFootPt (getPtFromTau(ptau), lBLine()->start(), lBLine()->end());
    return _vectorPoint (_foot, _n+M_PI, LTau(ptau));
  }
}

Point SILineArc::getRFootPt (TAU_TYPE ptau)
{
   if (_nu==-1)
      return _vectorPoint (rBArc()->center(), angle0To2Pi(_u+ptau), rBArc()->R());
   else {
    //return _getFootPt (getPtFromTau(ptau), rBLine()->start(), rBLine()->end());
    return _vectorPoint (_foot, _n, RTau(ptau));
  }
}

// this constructor is for initializing the arc-arc shock from 
// SO sources
SIArcArc::SIArcArc (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe, SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1,  REGULAR, stime, lbe, rbe, pse)
{
   _type = ARCARC;
  _lsvector = lsvector;
  _rsvector = rsvector;

   _Rl = lBArc()->R();
   _Rr = rBArc()->R();
  _origin = lBArc()->center();
  _H = _distPointPoint(lBArc()->center(), rBArc()->center());
  assert (_H!=0);

  _nu = (_Rl<_Rr) ? 1 : -1; //+1: LeftSmallerR, -1: RightSmallerR
   _nudl = lBArc()->nud();
   _nudr = rBArc()->nud();

  _Sl = lBArc()->_CCWStartVector;
  _El = lBArc()->_CCWEndVector;
  _Sr = rBArc()->_CCWStartVector;
  _Er = rBArc()->_CCWEndVector;

  _AsL = lBArc()->_StartVector;
  _AeL = lBArc()->_EndVector;
  _AsR = rBArc()->_StartVector;
  _AeR = rBArc()->_EndVector;

   if (_H>(_Rl+_Rr)) {
    _MU = 1;  //Normal outward cases
    _s =  1; //Hyperbola
  }
   else if (_H<vcl_fabs(_Rl-_Rr)) {
    _MU = 1;  //Normal outward cases
    _s = -1; //Ellipse
  }
  else { //2)Inner hyperbola & ellipse.
    _MU = -1;
    if (_nudl*_nudr==1)
      _s = 1;  //hyperbola by intersecting circles//+2; 
    else
      _s = -1; //Ellipse inside intersecting circles//-2; 
  }
   _u = _vPointPoint (lBArc()->center(), rBArc()->center());

   if (_s==1) {
      _a = (_Rl-_Rr)/2;
      _c = _H/2;
      _b2 = _c*_c-_a*_a;
    _b = vcl_sqrt(_b2);

    _Asym = (_nu==1) ? vcl_atan2(_b,-vcl_fabs(_a)) : vcl_atan2(_b,_a);
    _Asym = angle0To2Pi (_Asym);

    if (_nu==1)
      _LAsym = _Asym;
    else
      _RAsym = _Asym+M_PI;
   }
   else {
      _a = (_Rl+_Rr)/2;
      _c = _H/2;
      _b2 = _a*_a-_c*_c;
    _b = vcl_sqrt(_b2);
   }

  _LsTau = CCW (_u, lsvector);
  _RsTau = CCW (_u+M_PI, rsvector);

  //The tangent epsilon issue!! We use AisEq (1E-9) here!
  if (_s>0) {
    if (AisEq(_LsTau, 2*M_PI)) //Correct LsTau
      _LsTau = 0;
    if (AisEq(_RsTau, 0)) //Correct RsTau
      _RsTau = 2*M_PI;
  }
  else {
    if (_nu==1) {
      if (AisEq(_LsTau, 2*M_PI)) //Correct LsTau
        _LsTau = 0;
      if (AisEq(_RsTau, 2*M_PI)) //Correct RsTau
        _RsTau = 0;
    }
    else {
      if (AisEq(_LsTau, 0)) //Correct LsTau
        _LsTau = 2*M_PI;
      if (AisEq(_RsTau, 0)) //Correct RsTau
        _RsTau = 2*M_PI;
    }
  }

  //DETERMINE _hmu for _s==1, _emu for s==-1
  if (_MU==-1) {
    if (_s==1) { //Hyperbola by intersecting circles
      if (lBArc()->nud() == ARC_NUD_CCW &&
         rBArc()->nud() == ARC_NUD_CCW)
        _hmu = -1;
      else
        _hmu = 1;
    }
    else { //Ellipse inside intersecting circles
      if (lBArc()->nud() == ARC_NUD_CCW &&
         rBArc()->nud() == ARC_NUD_CW)
        _emu = -1;
      else
        _emu = 1;

      if (_nu==-1)
        _emu = -_emu;
    }
  }

  //Determine cases:
  if (_MU==1) {
    if (_s==1) {
      if (_nu==1)
        _case = 1;
      else
        _case = 2;
    }
    else {
      if (_nu==1)
        _case = 3;
      else
        _case = 4;
    }
  }
  else {
    if (_s==1) {
      if (_nu==1)
        if (_hmu==1)
          _case = 5;
        else
          _case = 6;
      else
        if (_hmu==1)
          _case = 7;
        else
          _case = 8;
    }
    else { //_s==-1
      if (_nu==1)
        if (_emu==1)
          _case = 9;
        else
          _case = 10;
      else
        if (_emu==1)
          _case = 11;
        else
          _case = 12;
    }
  }

  //dynamic validation usingthe domain of the intrinsic paramters
  if (!isLSTauValid() || !isRSTauValid()){
    //vcl_since this test failed, it means that this shock should not exist
    //set the valid flag to false
    _bValid = false;
    return; //there is no need to do the rest 
  }

  _minLTau = computeMinLTau ();
  _maxLTau = computeMaxLTau ();
  assert (_minLTau <= _maxLTau);
  _minRTau = computeMinRTau ();
  _maxRTau = computeMaxRTau ();
  assert (_minRTau <= _maxRTau);

  //INITILIZE END TAU...
  if (_MU==1) { //1)The same as Point-Arc case
    if (_s==1) {
      _LeTau = _maxLTau;
      _ReTau = _minRTau;
    }
    else {
      if (_nu==1) {
        _LeTau = _maxLTau;
        _ReTau = _maxRTau;
      }
      else {
        _LeTau = _minLTau;
        _ReTau = _minRTau;
      }
    }
  }
  else { //2)New here!
    if (_s==1) { //2-1): Hyperbola
      if (_hmu==1) { //outward hyperbola
        _LeTau = _maxLTau;
        _ReTau = _minRTau;
      }
      else { //inward hyperbola
        _LeTau = _minLTau;
        _ReTau = _maxRTau;
      }
    }
    else { //2-2): Ellipse
      if (_nu*_emu==1) { //two of the ellipses
        _LeTau = _maxLTau;
        _ReTau = _maxRTau;
      }
      else { //the other two of the ellipses
        _LeTau = _minLTau;
        _ReTau = _minRTau;
      }
    }
  }

  assert (_minLTau<=_LsTau && _LsTau<=_maxLTau);
  assert (_minRTau<=_RsTau && _RsTau<=_maxRTau);
}

bool SIArcArc::isTauValid_AA (TAU_TYPE letau, TAU_TYPE retau)
{
  bool result_minmax = isTauValid_MinMax (letau, retau);
  bool result=true;

  //if (_case==1 || _case==2 || _case==5 || _case==7) { //Case 1, 2, 5, 7: normal case.
  //  return result_minmax;
  //}
  //else { //Special case for Case 3, 4, 9, 10, 11, 12 (Ellipse Cases)
    //1)Check Leftside
    VECTOR_TYPE l_start_vector, l_end_vector;
    if (_case==1 || _case==2 || _case==5 || _case==7 ||
       _case==3 || _case==9 || _case==12) { //Case 3, 9, 12: _nu*_emu==1
      if (AisL(letau, _LsTau)) //ltau increvcl_asing
        result=false;
      l_start_vector = angle0To2Pi(_u+_LsTau);
      l_end_vector = angle0To2Pi(_u+letau);
    }
    else { //Case 4, 10, 11; Case 6 & 8
      if (AisG(letau, _LsTau)) //ltau decrevcl_asing
        result=false;
      l_end_vector = angle0To2Pi(_u+_LsTau);
      l_start_vector = angle0To2Pi(_u+letau);
    }
    if (!_validStartEnd0To2PiEPIncld (l_start_vector, _Sl, _El) ||
       !_validStartEnd0To2PiEPIncld (l_end_vector, _Sl, _El))
      result = false;

    //2)Check Rightside
    VECTOR_TYPE r_start_vector, r_end_vector;
    //Case 3, 9, 12: _nu*_emu==1; Case 6 & 8
    if (_case==3 || _case==9 || _case==12 || _case==6 || _case==8) {  
      if (AisL(retau, _RsTau)) //rtau increvcl_asing
        result=false;
      r_start_vector = angle0To2Pi(_u+M_PI+_RsTau);
      r_end_vector = angle0To2Pi(_u+M_PI+retau);
    }
    else { //Case 4, 10, 11
      if (AisG(retau, _RsTau)) //rtau decrevcl_asing
        result=false;
      r_end_vector = angle0To2Pi(_u+M_PI+_RsTau);
      r_start_vector = angle0To2Pi(_u+M_PI+retau);
    }
    if (!_validStartEnd0To2PiEPIncld (r_start_vector, _Sr, _Er) ||
       !_validStartEnd0To2PiEPIncld (r_end_vector, _Sr, _Er))
      result = false;

    //3)If all passed, return true.
    return result & result_minmax;
  //}
}

DIST_TYPE SIArcArc::dFromLTau (TAU_TYPE Ltau)
{
  DIST_TYPE d;
  double denom;

  if (_s>0)
    denom = _c*vcl_cos(Ltau)-_a;
  else
    denom = _a-_c*vcl_cos(Ltau);

  if (AisEq(denom,0)) //if (denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

DIST_TYPE SIArcArc::dFromRTau (TAU_TYPE Rtau)
{
  DIST_TYPE d;
  double denom;

  if (_s>0)
    denom = _a+_c*vcl_cos(Rtau);
  else
    denom = _a-_c*vcl_cos(Rtau);

  if (AisEq(denom,0)) //if (denom==0)
    d = ISHOCK_DIST_HUGE;
  else
    d = _b2/denom;

  //If tau are not in valid range, d will be < 0
  return d;
}

TAU_TYPE SIArcArc::RTau (TAU_TYPE Ltau)
{
  //1)EXTREME CASE: SEE SIPointArc::RTau()
  if (_s==1) {
    if (AisEq(Ltau, 0))
      return 2*M_PI;
    if (_nu==1)
      if (AisEq(Ltau, _Asym)) 
        return _Asym+M_PI;
  }
  else {
    if (AisEq(Ltau, 2*M_PI)) 
      return M_PI;
    if (AisEq(Ltau, M_PI)) 
      return 0;
  }

  //2)Normal case
  DIST_TYPE d = dFromLTau (Ltau);
  double m = ( d*vcl_sin(Ltau) )/( d*vcl_cos(Ltau)-_H );

  //For (_s>0) case, RTau is between (M_PI to 2*M_PI)
  //For (_s<0) case, RTau is between (0 to M_PI)
  int si = (_s>0) ? (m>0) : (m<0);

  VECTOR_TYPE rtau = angle0To2Pi (vcl_atan(m)+si*M_PI);

  return rtau;
}

TAU_TYPE SIArcArc::LTau (TAU_TYPE Rtau)
{  
  //1)EXTREME CASE: SEE SIPointArc::LTau()
  if (_s>0) {
    if (AisEq(Rtau, 2*M_PI))
      return 0;
    if (_nu==-1)
      if (AisEq(Rtau, _Asym+M_PI))
        return _Asym;
  }
  else {
    if (AisEq(Rtau, 0)) 
      return M_PI;
    if (AisEq(Rtau, M_PI)) 
      return 2*M_PI;
  }

  DIST_TYPE d = dFromRTau (Rtau);
  double m = ( d*vcl_sin(Rtau) )/( d*vcl_cos(Rtau)-_H );

  //For (_s>0) case, LTau is between (0 to M_PI)
  //For (_s<0) case, LTau is between (M_PI to 2*M_PI)
  int si = (_s>0) ? (m<0) : (m>0);

  VECTOR_TYPE ltau = angle0To2Pi (vcl_atan(m)+si*M_PI);

  return ltau;
}

bool SIArcArc::isLSTauValid ()
{
  bool result=true;

  //General check on left:
  VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
  if (!_validStartEnd0To2PiEPIncld (v, _Sl, _El))
    result=false;
  //General check on right:
  v = angle0To2Pi (_u+M_PI+_RsTau);
  if (!_validStartEnd0To2PiEPIncld (v, _Sr, _Er))
    result=false;

  if (_case==1 || _case==5) { //Check LAsym
    if (!AisGEq(_LsTau,0) || !AisLEq(_LsTau,_LAsym))
      result=false;
  }
  else if (_case==2 || _case==7) { //Check RAsym
    if (!AisGEq(_LsTau,0) || !AisLEq(_LsTau,_RAsym))
      result=false;
  }
  else  if (_case==6 || _case==8) {
    if (!AisGEq(_LsTau,0))
      result=false;
  }
  else { //Case 3, 4, 9, 10, 11, 12: Check M_PI or 2*M_PI
    if (!AisGEq(_LsTau,M_PI) || !AisLEq(_LsTau,2*M_PI))
      result=false;
  }

  //assert (result);
  return result;
}

bool SIArcArc::isRSTauValid ()
{
  bool result=true;

  //General check on left:
  VECTOR_TYPE v = angle0To2Pi (_u+_LsTau);
  if (!_validStartEnd0To2PiEPIncld (v, _Sl, _El))
    result=false;
  //General check on right:
  v = angle0To2Pi (_u+M_PI+_RsTau);
  if (!_validStartEnd0To2PiEPIncld (v, _Sr, _Er))
    result=false;

  if (_case==1 || _case==5) { //Check LAsym
    if (!AisGEq(_RsTau,2*M_PI-_LAsym) || !AisLEq(_RsTau,2*M_PI))
      result=false;
  }
  else if (_case==2 || _case==7) { //Check RAsym
    if (!AisGEq(_RsTau,_RAsym) || !AisLEq(_RsTau,2*M_PI))
      result=false;
  }
  else if (_case==6 || _case==8) {
    if (!AisLEq(_RsTau,2*M_PI))
      result=false;
  }
  else { //Case 3, 4, 9, 10, 11, 12: Check M_PI or 2*M_PI
    if (!AisGEq(_RsTau,0) || !AisLEq(_RsTau,M_PI))
      result=false;
  }

  //assert (result);
  return result;
}

TAU_TYPE SIArcArc::computeMinLTau ()
{
  if (_MU==1) { //SIMILAR TO Point-Arc
    if (_s==1) {
      if (_nu==1) //Case 1:
        return _LsTau;
      else //Case 2:
        return _LsTau;
    }
    else { //Case 3:
      if (_nu==1)
        return _LsTau;
      else { //Case 4:
        //See notes: Special issue on minLTau
        ANGLE_TYPE minltau = CCW(_u, _Sl);
        VECTOR_TYPE _u_plus_PI = angle0To2Pi(_u+M_PI);
        if (_validStartEnd0To2Pi(_u, _Sl, _El)) {
          if (_validStartEnd0To2Pi(_u_plus_PI, _Sl, _El)) {
            if (_distPointPoint(rBArc()->center(),lBArc()->end()) <
              _distPointPoint(rBArc()->center(),lBArc()->start())) //case 5
              return M_PI;
            else //case 1
              return (_LsTau>=minltau) ? minltau : M_PI;
          }
          else
            return minltau;
        }
        else {
          if (_validStartEnd0To2Pi(_u_plus_PI, _Sl, _El))
            return M_PI;
          else
            return minltau;
        }
      }//end _nu==-1, s==-1
    }
  }
  else { //_MU==-1
    if (_s==1) { //Inner Hyperbola, _nu==+1/-1 are the same
      if (_hmu==1) //Case 5, Case 7:
        return _LsTau;
      else //Case 6, Case 8:
        return 0;
    }
    else { //Inner Ellipse, 
      if (_nu==1) {
        if (_emu==1) //Case 9:
          return _LsTau;
        else //Case 10:
          return M_PI;
      }
      else {
        if (_emu==1) //Case 11:
          return M_PI;
        else //Case 12:
          return _LsTau;
      }
    }
  }
}

TAU_TYPE SIArcArc::computeMaxLTau ()
{
  if (_MU==1) { //SIMILAR TO Point-Arc
    if (_s==1) {
      if (_nu==1) { //Case 1:
        //double LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
        bool bIgnoreRightLimit = false;
        VECTOR_TYPE rvector = angle0To2Pi( RTau(_LAsym) + _u+M_PI );
        //If the converted LAsym is inside the right arc, ignore right limit.
        if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
          bIgnoreRightLimit = true;

        bool bIgnoreLeftLimit = false;
        VECTOR_TYPE lvector = angle0To2Pi( _LAsym + _u );
        // the  LAsym is inside the left arc, ignore left limit.
        if (_validStartEnd0To2Pi(lvector, _Sl, _El))
          bIgnoreLeftLimit = true;

        if (bIgnoreRightLimit) {
          if (bIgnoreLeftLimit)
            return _LAsym;
          else
            return CCW(_u, _El);
        }
        else {
          if (bIgnoreLeftLimit)
            return  vnl_math_min ( LTau(CCW(_u+M_PI, _Sr)), _LAsym );
          else
            return  vnl_math_min ( LTau(CCW(_u+M_PI, _Sr)), CCW(_u, _El) );
        }
      }
      else { //Case 2:
        //double RAsym = vcl_atan2 (_b, _a) +M_PI;
        bool bIgnoreLeftLimit = false;
        VECTOR_TYPE lvector = angle0To2Pi( LTau(_RAsym) + _u );
        // the converted RAsym is inside the left arc, ignore left limit.
        if (_validStartEnd0To2Pi(lvector, _Sl, _El))
          bIgnoreLeftLimit = true;

        bool bIgnoreRightLimit = false;
        VECTOR_TYPE rvector = angle0To2Pi( _RAsym + _u+M_PI );
        // the RAsym is inside the right arc, ignore right limit.
        if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
          bIgnoreRightLimit = true;

        if (bIgnoreLeftLimit) {
          if (bIgnoreRightLimit)
            return LTau(_RAsym);
          else
            return LTau( CCW(_u+M_PI, _Sr) );
        }
        else {
          if (bIgnoreRightLimit)
            return  vnl_math_min ( CCW(_u, _El), LTau(_RAsym) );
          else
            return  vnl_math_min ( CCW(_u, _El), LTau(CCW(_u+M_PI, _Sr)) );
        }
      }
    }
    else { //s==-1
      if (_nu==1) //Case 3:
        return LTau(computeMaxRTau());
      else //Case 4:
        return _LsTau;
    }
  }
  else { //_MU==-1
    if (_s==1) { //Inner Hyperbola
      if (_hmu==1) {
        //JUST LIKE THE POINT-ARC ONE, Asymptote, leftLimit, rightLimit!!!!!
        ANGLE_TYPE limitL;
        if (CCW(_u, El()) < CCW(_u, Sl()))
          limitL = ISHOCK_DIST_HUGE; //See note: no limit in this big arc case
        else
          limitL = CCW(_u, El());
        ANGLE_TYPE minRTauNoAsym = CCW(_u+M_PI, Sr());
        if (minRTauNoAsym>M_PI)
          minRTauNoAsym = M_PI;
        if (_nu==1) { //Case 5:
          //Asymptote on left.
          ANGLE_TYPE limit_LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
          ANGLE_TYPE limitR = LTau(  vnl_math_max (minRTauNoAsym, limit_LAsym+M_PI) );
          ANGLE_TYPE min_LR = vnl_math_min (limitL, limitR);
          return vnl_math_min (min_LR, limit_LAsym);
        }
        else { //Case 7:
          //Asymptote on right.
          ANGLE_TYPE limit_RAsym = vcl_atan2 (_b, _a);
          ANGLE_TYPE limitR = LTau(  vnl_math_max (minRTauNoAsym, limit_RAsym+M_PI) );
          ANGLE_TYPE min_LR = vnl_math_min (limitL, limitR);
          return vnl_math_min (min_LR, limit_RAsym);
        }
      }//END JUST LIKE THE POINT-ARC ONE
      else //Case 6, Case 8:
        return _LsTau;
    }
    else { //Inner Ellipse
      if (_nu==1) {
        if (_emu==1) //Case 9:
          return 2*M_PI;
        else //Case 10:
          return _LsTau;
      }
      else {
        if (_emu==1) //Case 11:
          return _LsTau;
        else //Case 12:
          return 2*M_PI;
      }
    }
  }
}

TAU_TYPE SIArcArc::computeMinRTau ()
{
  if (_MU==1) { //SIMILAR TO Point-Arc
    if (_s==1) {
      if (_nu==1) { //Case 1:
        //double LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
        bool bIgnoreRightLimit = false;
        VECTOR_TYPE rvector = angle0To2Pi( RTau(_LAsym) + _u+M_PI );
        //If the converted LAsym is inside the right arc, ignore right limit.
        if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
          bIgnoreRightLimit = true;

        bool bIgnoreLeftLimit = false;
        VECTOR_TYPE lvector = angle0To2Pi( _LAsym + _u );
        // the  LAsym is inside the left arc, ignore left limit.
        if (_validStartEnd0To2Pi(lvector, _Sl, _El))
          bIgnoreLeftLimit = true;

        if (bIgnoreRightLimit) {
          if (bIgnoreLeftLimit)
            return RTau(_LAsym);
          else
            return RTau(CCW(_u, _El));
        }
        else {
          if (bIgnoreLeftLimit)
            return  vnl_math_max ( CCW(_u+M_PI, _Sr), RTau(_LAsym) );
          else
            return  vnl_math_max ( CCW(_u+M_PI, _Sr), RTau(CCW(_u, _El)) );
        }
      }
      else { //Case 2: nu==-1
        //double RAsym = vcl_atan2 (_b, _a) +M_PI;
        bool bIgnoreLeftLimit = false;
        VECTOR_TYPE lvector = angle0To2Pi( LTau(_RAsym) + _u );
        // the converted RAsym is inside the left arc, ignore left limit.
        if (_validStartEnd0To2Pi(lvector, _Sl, _El))
          bIgnoreLeftLimit = true;

        bool bIgnoreRightLimit = false;
        VECTOR_TYPE rvector = angle0To2Pi( _RAsym + _u+M_PI );
        // the RAsym is inside the right arc, ignore right limit.
        if (_validStartEnd0To2Pi(rvector, _Sr, _Er))
          bIgnoreRightLimit = true;        

        if (bIgnoreLeftLimit) {
          if (bIgnoreRightLimit)
            return _RAsym;
          else
            return CCW(_u+M_PI, _Sr);
        }
        else {
          if (bIgnoreRightLimit)
            return  vnl_math_max ( RTau(CCW(_u, _El)), _RAsym );
          else
            return  vnl_math_max ( RTau(CCW(_u, _El)), CCW(_u+M_PI, _Sr) );
        }
      }
    }//end s==1
    else { //s==-1
      if (_nu==1) //Case 3:
        return _RsTau;
      else //Case 4:
        return RTau(computeMinLTau());
    }
  }
  else { //_MU==-1
    if (_s==1) { //Inner Hyperbola
      if (_hmu==1) {
        //JUST LIKE THE POINT-ARC ONE, Asymptote, leftLimit, rightLimit!!!!!
        ANGLE_TYPE limitR = CCW(_u+M_PI, _Sr);
        ANGLE_TYPE maxLTauNoAsym = CCW(_u, _El);
        if (maxLTauNoAsym > M_PI)
          maxLTauNoAsym = M_PI;
        if (_nu==1) { //Case 5:
          //Asymptote on left.
          VECTOR_TYPE limit_LAsym = angle0To2Pi( vcl_atan2(_b, -vcl_fabs(_a)) );
          VECTOR_TYPE limitL = RTau (  vnl_math_min (maxLTauNoAsym, limit_LAsym) );
          VECTOR_TYPE min_LR = vnl_math_max (limitL, limitR);
          return vnl_math_min (min_LR, limit_LAsym+M_PI);
        }
        else { //Case 7:
          //Asymptote on right.
          VECTOR_TYPE limit_RAsym = vcl_atan2 (_b, _a);
          VECTOR_TYPE limitL = RTau (  vnl_math_min (maxLTauNoAsym, limit_RAsym) );
          VECTOR_TYPE min_LR = vnl_math_max (limitL, limitR);
          return vnl_math_min (min_LR, limit_RAsym+M_PI);
        }
      }//END JUST LIKE THE POINT-ARC ONE
      else //Case 6, Case 8:
        return _RsTau;
    }
    else { //Inner Ellipse
      if (_nu==1) {
        if (_emu==1) //Case 9:
          return _RsTau;
        else //Case 10:
          return 0;
      }
      else {
        if (_emu==1) //Case 11:
          return 0;
        else //Case 12:
          return _RsTau;
      }
    }
  }
}

TAU_TYPE SIArcArc::computeMaxRTau ()
{
  if (_MU==1) {
    if (_s==1) {
      if (_nu==1) //Case 1:
        return _RsTau;
      else //Case 2:
        return _RsTau;
    }
    else {
      if (_nu==1) { //Case 3:
        //See notes: Special issue on maxrRTau
        ANGLE_TYPE maxrtau = CCW(_u+M_PI, _Er);
        VECTOR_TYPE _u_plus_PI = angle0To2Pi(_u+M_PI);
        if (_validStartEnd0To2Pi(_u, _Sr, _Er)) {
          if (_validStartEnd0To2Pi(_u_plus_PI, _Sr, _Er)) {
            if (_distPointPoint(lBArc()->center(),rBArc()->start()) <
               _distPointPoint(lBArc()->center(),rBArc()->end())) //case 5
              return M_PI;
            else //case 1
              return (_RsTau<=maxrtau) ? maxrtau : M_PI;
          }
          else
            return M_PI;
        }
        else {
          if (_validStartEnd0To2Pi(_u_plus_PI, _Sr, _Er))
            return maxrtau;
          else
            return maxrtau;
        }
      }
      else //Case 4:
        return _RsTau;
    }
  }
  else { //_MU==-1
    if (_s==1) { //Inner Hyperbola
      if (_hmu==1) //Case 5, Case 7:
        return _RsTau;
      else //Case 6, Case 8:
        return 2*M_PI;
    }
    else { //Inner Ellipse
      if (_nu==1) {
        if (_emu==1) //Case 9:
          return M_PI;
        else //Case 10:
          return _RsTau;
      }
      else {
        if (_emu==1) //Case 11:
          return _RsTau;
        else //Case 12:
          return M_PI;
      }
    }
  }
}

/*
bool SIArcArc::isIntersectionValid(double letau, double retau)
{
   double arc_start_vector, arc_end_vector;
  int arc_direction;
  double start_vector , end_vector;

   //check the LEFT side first
  arc_start_vector = _Sl;
  arc_end_vector = _El;

  if (arc_direction>0){
    start_vector = angle0To2Pi(LsTau + _u);
    end_vector = angle0To2Pi(letau + _u);
  }
  else {
    start_vector = angle0To2Pi(letau + _u);
    end_vector = angle0To2Pi(LsTau + _u);
  }

  //adjust vectors for comparision
  if (AisG(arc_start_vector, arc_end_vector)) arc_start_vector -= 2*M_PI;
  if (AisG(start_vector, arc_end_vector))     start_vector -= 2*M_PI;
  if (AisG(end_vector, arc_end_vector))      end_vector -= 2*M_PI;

  if (!(AisLEq(arc_start_vector, start_vector) &&
       AisLEq(start_vector, end_vector) &&
      AisLEq(end_vector, arc_end_vector))
    )
    return 0;
   
  //now check the right side

   arc_direction = ((BArc*)rBElement)->nud;
  if (arc_direction==-1){
    arc_start_vector = ((BArc*)rBElement)->start_angle;
    arc_end_vector = ((BArc*)rBElement)->end_angle;
  }
  else {
    arc_start_vector = ((BArc*)rBElement)->end_angle;
    arc_end_vector = ((BArc*)rBElement)->start_angle;
  }
  
  if (arc_direction>0){
    start_vector = angle0To2Pi(retau + u + M_PI);
    end_vector = angle0To2Pi(RsTau + u + M_PI);
  }
  else {
    start_vector = angle0To2Pi(RsTau + u + M_PI);
    end_vector = angle0To2Pi(retau + u + M_PI);
  }

  //adjust vectors for comparision
  if (AisG(arc_start_vector, arc_end_vector)) arc_start_vector -= 2*M_PI;
  if (AisG(start_vector, arc_end_vector))     start_vector -= 2*M_PI;
  if (AisG(end_vector, arc_end_vector))      end_vector -= 2*M_PI;

  if (!(AisLEq(arc_start_vector, start_vector) &&
       AisLEq(start_vector, end_vector) &&
      AisLEq(end_vector, arc_end_vector))
    )
    return 0;
   
   return 1;
}*/

TAU_TYPE SIArcArc::getLTauFromTime (RADIUS_TYPE time)
{
  DIST_TYPE d;
  TAU_TYPE ltau;

  if (_s>0) {
    d = time + _Rl;
    ltau = vcl_acos ( (_a*d+_b2)/d/_c );
  }
  else {
    d = _Rr - time;
    ltau = vcl_acos ( (_a*d-_b2)/d/_c );
  }

  return ltau;
}

Point SIArcArc::getPtFromLTau (TAU_TYPE tau)
{
   //assuming tau is always left tau
   Point pt;
  assert (tau>=0);
   DIST_TYPE d = dFromLTau (tau);

  pt = rotateCCW(d*vcl_cos(tau), d*vcl_sin(tau), _u);
   pt.x = _origin.x + pt.x;
   pt.y = _origin.y + pt.y;

   return pt;
}

//always use left tau
Point SIArcArc::getLFootPt (TAU_TYPE tau)
{
  return _vectorPoint ( lBArc()->center(), angle0To2Pi(_u+tau), lBArc()->R());
}

//always use the svcl_tandard tau
Point SIArcArc::getRFootPt (TAU_TYPE tau)
{
  //always use the right tau
  double rtau = RTau(tau);
  return _vectorPoint ( rBArc()->center(), angle0To2Pi(_u+M_PI+rtau), rBArc()->R());
}

SIThirdOrder::SIThirdOrder (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe,
                SINode* pse,
                TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1, REGULAR, stime, lbe, rbe, pse) 
{
   _type = THIRDORDER;
   _endTime = _startTime;
  _lsvector = lsvector;
  _rsvector = rsvector;

   _Al = ((BLine *)lbe)->start();
   _Bl = ((BLine *)lbe)->end();
   _Ar = ((BLine *)rbe)->start();
   _Br = ((BLine *)rbe)->end();

   _lL = _distPointPoint(_Al, _Bl);
   _lR = _distPointPoint(_Ar, _Br);

   _nl = vcl_atan2(_Bl.y - _Al.y, _Bl.x - _Al.x);
   _ul = angle0To2Pi (_nl- M_PI_2);

   //distance between the lines
   _H = vcl_fabs((_Ar.x - _Al.x)*vcl_cos(_ul) + (_Ar.y - _Al.y)*vcl_sin(_ul));

   _origin.x = _Al.x - 0.5*_H*vcl_cos(_ul);
   _origin.y = _Al.y - 0.5*_H*vcl_sin(_ul);

  //nu=1: shock_dir=nr, nu=-1: shock_dir=nl

  _LsTau = lsvector;
  _RsTau = rsvector;

   _LeTau = 0;
   _ReTau = _lR;
}

Point SIThirdOrder::getPtFromLTau (TAU_TYPE ltau) 
{ 
  Point lstartpt = _origin;
  return _vectorPoint (lstartpt, _nl, ltau); 
}

SIArcThirdOrder::SIArcThirdOrder (int newid, RADIUS_TYPE stime,
                BElement* lbe, BElement* rbe,
                SINode* pse,
           TAU_TYPE lsvector, TAU_TYPE rsvector) :
   SILink (newid, 1, REGULAR, stime, lbe, rbe, pse)
{
  BPoint *lpoint, *rpoint;
  BArc   *larc, *rarc;
   _type = ARCTHIRDORDER;
   _endTime = _startTime;
  _lsvector = lsvector;
  _rsvector = rsvector;
  ANGLE_TYPE arclen;

  _H = 0; //_distPointPoint(lpoint->pt(), rarc->center());

  if (lbe->type() == BPOINT) { //Case 1:
    _case = 1;
    _nu = 1;
    _Rl = 0;
    _Rr = ((BArc*)rbe)->R();
    _nudl = ARC_NUD_CW;
    _nudr = ((BArc*)rbe)->nud();
    lpoint = (BPoint*)lbe;
    rarc = (BArc*)rbe;
    _origin = lpoint->pt();

    _ul = rarc->_CCWEndVector;
    _ur = rarc->_CCWStartVector;

    _RsTau = CCW (_ur, rsvector);
    arclen = CCW (rarc->_CCWStartVector, rarc->_CCWEndVector);
    _LsTau = arclen - _RsTau;
  }
  else if (rbe->type() == BPOINT) { //Case 2:
    _case = 2;
    _nu = 2;
    _Rl = ((BArc*)lbe)->R();
    _Rr = 0;
    _nudl = ((BArc*)lbe)->nud();
    _nudr = ARC_NUD_CW;
    larc = (BArc*)lbe;
    rpoint = (BPoint*)rbe;
    _origin = rpoint->pt();

    _ul = larc->_CCWStartVector;
    _ur = larc->_CCWEndVector;

    _LsTau = CCW (_ul, lsvector);
    arclen = CCW (larc->_CCWStartVector, larc->_CCWEndVector);
    _RsTau = arclen - _LsTau;
  }
  else { //Case 3 & 4:
    _nu = 3;
    _Rl = ((BArc*)lbe)->R();
    _Rr = ((BArc*)rbe)->R();
    _nudl = ((BArc*)lbe)->nud();
    _nudr = ((BArc*)rbe)->nud();
    larc = (BArc*)lbe;
    rarc = (BArc*)rbe;
    _origin = larc->center();

    if (larc->nud()==ARC_NUD_CW) { //Case 3:
      _case = 3;
      _ul = larc->_CCWEndVector;
      _ur = rarc->_CCWStartVector;

      _LsTau = CCW (lsvector, _ul);
      _RsTau = CCW (_ur, rsvector);
    }
    else { //Case 4:
      _case = 4;
      _ul = larc->_CCWStartVector;
      _ur = rarc->_CCWEndVector;

      _LsTau = CCW (_ul, lsvector);
      _RsTau = CCW (rsvector, _ur);
      //special case to deal with, if angle0To2Pi allow 2Pi for output
      if (_RsTau == 2*M_PI) 
        _RsTau = 0;
    }
  }

   if (_nudr==ARC_NUD_CCW) { //CCW: case 1 and 3
      _LeTau = _LsTau - M_PI/8;
      _ReTau = _RsTau + M_PI/8;
   }
   else { //CW: case 2 and 4
      _LeTau = _LsTau - M_PI/8;
      _ReTau = _RsTau + M_PI/8;
   }
  assert (_LeTau >=0);
  assert (_ReTau >=0);
}

Point SIArcThirdOrder::getPtFromLTau (TAU_TYPE ltau)
{
   //assuming tau is always left tau
   Point pt;
   DIST_TYPE a, b2, c, d;

  //_H is always 0, so c=0, a=b
  a = (_Rl+_Rr)/2;
  c = 0; //_H/2;
  b2 = a*a;

  //For CCWATO: ltau goes clockwise
  TAU_TYPE tau = (_nudr==ARC_NUD_CCW) ? -ltau : ltau;
   d = b2/(a - c*vcl_cos(tau));

   pt = rotateCCW(d*vcl_cos(tau),d*vcl_sin(tau), _ul);
   pt.x = _origin.x + pt.x;
   pt.y = _origin.y + pt.y;

   return pt;
}

Point SIArcThirdOrder::getLFootPt (TAU_TYPE ltau)
{
  if (_case==1 || _case==3)
    return _vectorPoint ( lBArc()->center(), angle0To2Pi(_ul-ltau), lBArc()->R());
  else
    return _vectorPoint ( lBArc()->center(), angle0To2Pi(_ul+ltau), lBArc()->R());
}

//always use the svcl_tandard tau
Point SIArcThirdOrder::getRFootPt (TAU_TYPE tau)
{
  //always use the right tau
  double rtau = RTau(tau);

  if (_case==1 || _case==3)
    return _vectorPoint ( rBArc()->center(), angle0To2Pi(_ur+rtau), rBArc()->R());
  else
    return _vectorPoint ( rBArc()->center(), angle0To2Pi(_ur-rtau), rBArc()->R());
}

// SIEDGE for keeping shock groups after a pruning process

SIEdge::SIEdge (int newid, SHOCKLABEL newlabel,
           BContour* lcon, BContour* rcon, 
        SINode* psnode, SINode* csnode, SILinksList slinks):
  SIElement(newid, 1 , newlabel, 0)
{
  _lContour = lcon;
  _rContour = rcon;

  _links = slinks;

  _pSNode = psnode;
  _cSNode = csnode;

  if (psnode)
    _startTime = psnode->startTime();

  if (csnode)
    _endTime = csnode->startTime();
}

SIEdge::~SIEdge ()
{
}
