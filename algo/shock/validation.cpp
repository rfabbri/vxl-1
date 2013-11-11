//SHOCK AND BOUNDARY STRUCTURE VALIDATION

//#include "msgout.h"

#include "ishock-common.h"
#include "sielements.h"
#include "ishock.h"

bool SIElement::validate (Boundary *bnd)
{
  assert (_id > 0);
  return true;
}

bool SINode::validate (Boundary *bnd)
{
  if (!SIElement::validate(bnd))
    return false;

  assert (_graph_type==NODE);

  //3)Disvcl_tance Validation: The linked boundary elements
  DIST_TYPE dist;
  BElementList::iterator curB = bndList.begin();
   for (; curB!=bndList.end(); ++curB) {
    BElement* belm = *curB;
    switch ((*curB)->type()) {
    case BPOINT:
      dist = ((BPoint*)belm)->distPoint (_origin);
    break;
    case BLINE:
      dist = ((BLine*)belm)->distPoint (_origin);
    break;
    case BARC:
      dist = ((BArc*)belm)->distPoint (_origin);
    break;
    }
    
    //In many cases, RisEq is too strict!
    //if ( !RisEq (_startTime, dist) ) {
    if ( !_isEq (_startTime, dist, R_EPSILON*10) ) {
      vcl_cout<< "SINode linked-boundary distance validation fails. sid=" << _id << ".\n";
      //if (MessageOption>=MSG_NORMAL)
      vcl_cout<< "Error= " << _startTime-dist << ", Epsilon= "<< _epsilon(_startTime, dist, R_EPSILON*10) << vcl_endl;
      return false;
    }
  }

  //4)Disvcl_tance Validation: All other elements
  DIST_TYPE minDist = ISHOCK_DIST_HUGE;
  BElement* belm;
  BElmListIterator i = bnd->BElmList.begin();
   for (; i != bnd->BElmList.end(); i++) {
    belm = (i->second);
    switch ((i->second)->type()) {
    case BPOINT:
    {
      BPoint* bpoint = (BPoint*)(i->second);

      //If (*i) is one of the linked one, ignore
      curB = bndList.begin();
      for(; curB!=bndList.end(); ++curB) 
        if (bpoint == (*curB))
          goto __next_belm;

      dist =  bpoint->distPoint (_origin);
      if (dist < minDist)
        minDist = dist;
    }
    break;
    case BLINE:
    {
      BLine* bline = (BLine*)(i->second);

      //If (*i) is one of the linked one or the twin of the linked one, ignore
      curB = bndList.begin();
      for(; curB!=bndList.end(); ++curB) {
        if (bline == (*curB))
          goto __next_belm;
        if ((*curB)->type() == BLINE)
          if (bline == ((BLine*)(*curB))->twinLine())
            goto __next_belm;
      }
      if (!bline->isPointValidInTPlaneFuzzy(_origin))
        goto __next_belm;

      dist = bline->distPoint (_origin);
      if (dist < minDist)
        minDist = dist;
    }
    break;
    case BARC:
    {
      BArc* barc = (BArc*)(i->second);

      //If (*i) is one of the linked one or the twin of the linked one, ignore
      curB = bndList.begin();
      for(; curB!=bndList.end(); ++curB) {
        if (barc == (*curB))
          goto __next_belm;
        if ((*curB)->type() == BARC)
          if (barc == ((BArc*)(*curB))->twinArc())
            goto __next_belm;
      }
      double a;
      if (!barc->isPointValidInRAngleFuzzy(_origin, a))
        goto __next_belm;

      dist = barc->distPoint (_origin);
      if (dist < minDist)
        minDist = dist;
    }
    break;
    }
__next_belm:
    ;
  }//end for

  //EPSILONISSUE: if we use RisGEq here, we only have 0.05 precision
  //relative error 1E-5, abs error 1E-5*5000=0.05. Too strict for far away shocks!
  //So we use RisG here!
  if (RisG(_startTime, minDist)) { //RisGEq
    vcl_cout<< "SINode other-boundary distance validation fails. sid=" << _id << ".\n";
    //if (MessageOption>=MSG_NORMAL)
    vcl_cout<< " Error= "  << _startTime-minDist << ", Epsilon= "<< _epsilon(_startTime, minDist, R_EPSILON) << vcl_endl;
    return false;
  }

  return true;
}

bool SILink::validate (Boundary *bnd, bool bPropagationCompleted)
{
  bool bValid = true;

  if (!SIElement::validate(bnd))
    return false;

  assert (_edgeID > 0);
  assert (_graph_type==LINK);
  assert (_startTime <= _simTime);
  assert (_lBElement);
  assert (_rBElement);

  if (bPropagationCompleted) {
    //The only one unpropagated ContactShock Problem.
    //We know it's there, but never mind.
    if (!_bPropagated) {
      if (_bActive && _label!=CONTACT) {
        vcl_cout<< "ERROR! SILink not propagated! sid = " << _id <<vcl_endl;
        bValid = false;
      }
    }

    if (_bActive) {
      ////assert (_endTime > MAX_RADIUS);
      //assert (_lNeighbor == NULL);  //!There might be still intersection outside MAX_RADIUS
      //assert (_rNeighbor == NULL);  //!There might be still intersection outside MAX_RADIUS
      assert (_cSNode == NULL);
    }
    else {
      //!! ArcTO.bnd assert (_endTime <= MAX_RADIUS);
      assert ( _lNeighbor || _rNeighbor);
      if (_lNeighbor)
        assert (_lNeighbor->rNeighbor() == this);
      if (_rNeighbor)
        assert (_rNeighbor->lNeighbor() == this);

      //one of the child's pSLink is it self
      assert (_cSNode->type()==JUNCT || _cSNode->type()==SINK || _cSNode->type()==A3SOURCE);
      bool ok = false;

      if (_cSNode->type()==JUNCT) {
        SIJunct* child = (SIJunct*)_cSNode;
        SILinksList::iterator curS = child->PSElementList.begin();
        for(; curS!=child->PSElementList.end(); ++curS)
          if ((*curS) == this)
            ok = true;
      }
      else if (_cSNode->type()==SINK) {
        SISink* child = (SISink*)_cSNode;
        SILinksList::iterator curS = child->PSElementList.begin();
        for(; curS!=child->PSElementList.end(); ++curS)
          if ((*curS) == this)
            ok = true;
      }
      else {
        SIA3Source* child = (SIA3Source*)_cSNode;
        SILinksList::iterator curS = child->PSElementList.begin();
        for(; curS!=child->PSElementList.end(); ++curS)
          if ((*curS) == this)
            ok = true;
      }
      if (!ok)
        bValid = false;
    }
  }

  return bValid;
}

bool SIA3Source::validate (Boundary *bnd)
{
  if (!SINode::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==REGULAR);
   assert (_type==A3SOURCE);
  assert (_startTime == _simTime);
  assert ( nBElement() > 2);
  assert (_bActive==false);
  assert (_bPropagated);

  //1)Structure Validation: All the parent shocks
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    assert ((*curS)->graph_type() == LINK);
    assert ((*curS)->label() == CONTACT);
    SILink *pslink = (SILink*) (*curS);
    //!! ArcTO.bnd assert (pslink->cSNode() == this);
  }

  //2)Structure Validation: The child shock
  assert (_cSLink->graph_type() == LINK);
  assert (_cSLink->pSNode() == this);

  return true;
}

//Validate after propagation, _bActive should always be false
bool SISource::validate (Boundary *bnd, bool bPropagationCompleted)
{
  if (!SINode::validate(bnd))
    return false;

  assert (_order == 2);
   assert (_type==SOURCE);
  assert (_startTime == _simTime);
  assert ( nBElement() == 2);

  //1)Structure Validation: Check propagated sources...
  if ( bPropagationCompleted ) {
    if (_bActive || !_bPropagated) {
      vcl_cout<< "ERROR! Source not propagated! sid = " << _id <<vcl_endl;
      return false;
    }
    assert (_cSLink->pSNode() == this);
    assert (_cSLink->graph_type() == LINK);
    assert (_cSLink2->pSNode() == this);
    assert (_cSLink2->graph_type() == LINK);
  }

  return true;
}

bool SISink::validate (Boundary *bnd)
{
  if (!SINode::validate(bnd))
    return false;

  assert (_order == 4);
  assert (_label==DEGENERATE);
   assert (_type==SINK);
  assert (_startTime == _simTime);
  //THIRDORDER & ARCTHIRDORDER Sink has 2 nBlements
  assert ( nBElement() >= 2);
  assert (_bActive==false);
  assert (_bPropagated);

  //1)Structure Validation: All the parent shocks
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    assert ((*curS)->graph_type() == LINK);
    SILink *pslink = (SILink*) (*curS);
    assert (pslink->cSNode() == this);
  }

  return true;
}

bool SIJunct::validate (Boundary *bnd)
{
  if (!SINode::validate(bnd))
    return false;

  assert (_order == 3);
  assert (_label==REGULAR);
   assert (_type==JUNCT);
  assert (_startTime == _simTime);
  assert ( nBElement() > 2);
  assert (_bActive==false);
  assert (_bPropagated);

  //1)Structure Validation: All the parent shocks
  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
    assert ((*curS)->graph_type() == LINK);
    SILink *pslink = (SILink*) (*curS);
    assert (pslink->cSNode() == this);
  }

  //2)Structure Validation: The child shock
  assert (_cSLink->graph_type() == LINK);
  assert (_cSLink->pSNode() == this);

  return true;
}

bool SIPointPoint::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==DEGENERATE);
   assert (_type==POINTPOINT);

  assert (_LsTau >= minLTau() || _LeTau <= maxLTau());
  assert (_RsTau >= minRTau() || _ReTau <= maxRTau());

  assert (_origin == lBPoint()->pt());
  assert (_u == _vPointPoint (lBPoint()->pt(), rBPoint()->pt()));
  assert (_n == angle0To2Pi (_u+M_PI_2));
  assert (_H == _distPointPoint (lBPoint()->pt(), rBPoint()->pt()));

  //1)Disvcl_tance Validation: The startTime vs Starting distances to two boundary elements
  Point  startPt = getStartPt ();
  DIST_TYPE distL = _distPointPoint (startPt, lBPoint()->pt());
  DIST_TYPE distR = _distPointPoint (startPt, rBPoint()->pt());
  if ( !RisEq(_startTime, distL) || !RisEq(_startTime, distR) ) {
    bValid = false;
    //if (MessageOption>=MSG_NORMAL)
    //  vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Starting distances." <<vcl_endl;
  }

  //2)If not Active (has child)
  //  Disvcl_tance Validation: The endTime vs Ending distances to two boundary elements
  if (_cSNode) {
    Point  endPt = getEndPt ();
    distL = _distPointPoint (endPt, lBPoint()->pt());
    distR = _distPointPoint (endPt, rBPoint()->pt());
    if ( !RisEq(_endTime, distL) || !RisEq(_endTime, distR) ) {
      bValid = false;
      //if (MessageOption>=MSG_NORMAL)
      //  vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Ending distances." <<vcl_endl;
    }
  }
  else {
    //!! assert (_endTime > MAX_RADIUS);
  }

  return bValid;
}

bool SIPointLineContact::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==CONTACT);
   assert (_type==POINTLINECONTACT);
  assert (_nu == (_lBElement->type()==BPOINT) ? 0 : 1);
  if (_nu==0) {
    //Reconnect Problem!!
    ///assert (_LsTau== rBLine()->N());
    ///assert (_RsTau==0);
    ///assert (_n == rBLine()->N());
  }
  else {
    ///assert (_LsTau==lBLine()->L());
    ///assert (_RsTau==lBLine()->N());
    ///assert (_n == lBLine()->N());
  }
  assert (_H==0);

  //Disvcl_tance Validation: The startTime vs Starting distances to two boundary elements: SKIP
  //Disvcl_tance Validation: The endTime vs Ending distances to two boundary elements: SKIP

  if (!bValid)
    vcl_cout<< "\n validate SIPointPoint Fail!! sid= "<< _id <<" !!\n";

  return bValid;
}

bool SIPointArcContact::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==CONTACT);

   assert (_type==POINTARCCONTACT);
  assert (_nu == (_lBElement->type()==BPOINT) ? 0 : 1);
  if (_nu==0) { //0:LeftPoint-RightArc
    //Reconnect Problem!!
    //assert (_LsTau==0);
    //assert (_RsTau==0);
    //assert (_n == rBArc()->N());
  }
  else {
    //assert (_LsTau==lBLine()->L());
    //assert (_RsTau==2*M_PI);
    //assert (_n == lArc()->N());
  }
  assert (_H==0);

  //Disvcl_tance Validation: The startTime vs Starting distances to two boundary elements: SKIP
  //Disvcl_tance Validation: The endTime vs Ending distances to two boundary elements: SKIP

  return bValid;
}

bool SIPointLine::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==SEMI_DEGENERATE);
   assert (_type==POINTLINE);
  
  assert (_LsTau >= minLTau() || _LeTau <= maxLTau());
  ///!!! assert (_RsTau >= minRTau() || _ReTau <= maxRTau());

  assert (_nu == (_lBElement->type()==BPOINT) ? 1 : -1);
  if (_nu>0) {
    assert (_origin == lBPoint()->pt());
    assert (_n == rBLine()->U());
    assert (_H == rBLine()->distPoint (lBPoint()->pt()));
    assert (_delta == _deltaPointLine (lBPoint()->pt(), rBLine()->start(), rBLine()->end()));
    assert (_l = rBLine()->L());
  }
  else {
    assert (_origin == rBPoint()->pt());
    assert (_n == lBLine()->U());
    assert (_H == lBLine()->distPoint (rBPoint()->pt()));
    assert (_delta == _deltaPointLine (rBPoint()->pt(), lBLine()->start(), lBLine()->end()));
    assert (_l = lBLine()->L());
  }
  assert (_u == angle0To2Pi (_n - M_PI_2));

  //1)Disvcl_tance Validation: The startTime vs Starting distances to two boundary elements
  Point  startPt = getStartPt ();
  DIST_TYPE distL = (_nu>0) ? _distPointPoint (startPt, lBPoint()->pt())
                  : _distPointLine (startPt, lBLine()->start(), lBLine()->end());
  DIST_TYPE distR = (_nu>0) ? _distPointLine (startPt, rBLine()->start(), rBLine()->end())
                  : _distPointPoint (startPt, rBPoint()->pt());
  if ( !RisEq(_startTime, distL) || !RisEq(_startTime, distR) ) {
    bValid = false;
    //if (MessageOption>=MSG_NORMAL)
    //  vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Starting distances." <<vcl_endl;
  }

  //2)If not Active (has child)
  //  Disvcl_tance Validation: The endTime vs Ending distances to two boundary elements
  if (_cSNode) {
    Point  endPt = getEndPt ();
    distL = (_nu>0) ? _distPointPoint (endPt, lBPoint()->pt())
               : _distPointLine (endPt, lBLine()->start(), lBLine()->end());
    distR = (_nu>0) ? _distPointLine (endPt, rBLine()->start(), rBLine()->end())
               : _distPointPoint (endPt, rBPoint()->pt());
    if ( !RisEq(_endTime, distL) || !RisEq(_endTime, distR) ) {
      bValid = false;
      vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Ending distances." <<vcl_endl;
    }
  }
  else {
    assert (_endTime > MAX_RADIUS);
  }

  return bValid;
}

bool SIPointArc::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  assert (_order == 1);
  assert (_label==DEGENERATE);
   assert (_type==POINTARC);
  
  assert (_LsTau >= minLTau() || _LeTau <= maxLTau());
  assert (_RsTau >= minRTau() || _ReTau <= maxRTau());

  assert (_nu == (_lBElement->type()==BPOINT) ? 1 : -1);
/*  if (_nu>0) {
    assert (_origin == lBPoint()->pt());
    assert (_H == rBLine()->distPoint (lBPoint()->pt()));
    assert (_delta == _deltaPointLine (lBPoint()->pt(), rBLine()->start(), rBLine()->end()));
    assert (_l = rBLine()->L());
  }
  else {
    assert (_origin == rBPoint()->pt());
    assert (_H == lBLine()->distPoint (rBPoint()->pt()));
    assert (_delta == _deltaPointLine (rBPoint()->pt(), lBLine()->start(), lBLine()->end()));
    assert (_l = lBLine()->L());
  }
  assert (_u == angle0To2Pi (_n - M_PI_2));

  //1)Disvcl_tance Validation: The startTime vs Starting distances to two boundary elements
  Point  startPt = getStartPt ();
  DIST_TYPE distL = (_nu>0) ? _distPointPoint (startPt, lBPoint()->pt())
                  : _distPointLine (startPt, lBLine()->start(), lBLine()->end());
  DIST_TYPE distR = (_nu>0) ? _distPointLine (startPt, rBLine()->start(), rBLine()->end())
                  : _distPointPoint (startPt, rBPoint()->pt());
  if ( !RisEq(_startTime, distL) || !RisEq(_startTime, distR) ) {
    bValid = false;
    vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Starting distances." <<vcl_endl;
  }

  //2)If not Active (has child)
  //  Disvcl_tance Validation: The endTime vs Ending distances to two boundary elements
  if (_cSNode) {
    Point  endPt = getEndPt ();
    distL = (_nu>0) ? _distPointPoint (endPt, lBPoint()->pt())
               : _distPointLine (endPt, lBLine()->start(), lBLine()->end());
    distR = (_nu>0) ? _distPointLine (endPt, rBLine()->start(), rBLine()->end())
               : _distPointPoint (endPt, rBPoint()->pt());
    if ( !RisEq(_endTime, distL) || !RisEq(_endTime, distR) ) {
      bValid = false;
      vcl_cout<< "Disvcl_tance validation fails! SIPointPoint Ending distances." <<vcl_endl;
    }
  }
  else {
    assert (_endTime > MAX_RADIUS);
  }
*/
  return bValid;
}
/*
void SIPointArc::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   if (_s>0)
      s.Printf ("Point-Arc Shock (HYPERBOLA)\n \n");
   else
      s.Printf ("Point-Arc Shock (ELLIPSE)\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==POINTARC);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %f\n", _startTime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
  if (_nu==1)
    s.Printf ("nu: %d (LeftPoint-RightArc)\n", _nu);
  else
    s.Printf ("nu: %d (LeftArc-RightPoint)\n", _nu); buf+=s;
  if (_s==1)
    s.Printf ("sigma: %d (+1:H>R)\n", _nu);
  else
    s.Printf ("sigma: %d (-1:R>H)\n", _nu); buf+=s;
   s.Printf ("_nudl: %d\n", _nudl); buf+=s;
   s.Printf ("_nudr: %d\n", _nudr); buf+=s;
   s.Printf ("_Rl: %f\n", _Rl); buf+=s;
   s.Printf ("_Rr: %f\n", _Rr); buf+=s;
   s.Printf ("_R: %f\n\n", _R); buf+=s;


   s.Printf ("_a: %f\n", _a); buf+=s;
   s.Printf ("_b2: %f\n", _b2); buf+=s;
   s.Printf ("_c: %f\n", _c); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bPruned: %s\n \n", _bPruned ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/

bool SILineLine::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  return bValid;
}
/*
void SILineLine::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Line-Line Shock (LINE)\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s; 
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==LINELINE);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %f\n", _startTime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
  s.Printf ("(Al)-(Bl): (%f,%f)-(%f,%f)\n", _Al.x, _Al.y,  _Bl.x, _Bl.y); 
  s.Printf ("(Ar)-(Br): (%f,%f)-(%f,%f)\n", _Ar.x, _Ar.y,  _Br.x, _Br.y); 
   s.Printf ("_leftL: %f\n", _lL); buf+=s;
   s.Printf ("_rightL: %f\n\n", _lR); buf+=s;

   s.Printf ("nl: %f (%.2f deg) CW from X-axis\n", _nl, _nl*180/M_PI); buf+=s;
   s.Printf ("ul: %f (%.2f deg) CW from X-axis\n", _ul, _ul*180/M_PI); buf+=s;
   s.Printf ("nr: %f (%.2f deg) CW from X-axis\n", _nr, _nr*180/M_PI); buf+=s;
   s.Printf ("ur: %f (%.2f deg) CW from X-axis\n", _ur, _ur*180/M_PI); buf+=s;
   s.Printf ("sigma (nl dot nr): %f\n\n", _sigma); buf+=s;

   s.Printf ("thetaL: %f\n", _thetaL); buf+=s;
   s.Printf ("thetaR: %f\n", _thetaR); buf+=s;
   s.Printf ("phi: %f\n", _phi); buf+=s;
   s.Printf ("deltaL: %f dist (Origin, AL)\n", _deltaL); buf+=s;
   s.Printf ("deltaR: %f dist (Origin, AR)\n\n", _deltaR); buf+=s;

   s.Printf ("_N1L (slope): %f\n", _N1L); buf+=s;
  s.Printf ("_N1R (slope): %f\n", _N1R); buf+=s;
  s.Printf ("_N2L: %f\n", _N2L); buf+=s;
  s.Printf ("_N2R: %f\n\n", _N2R); buf+=s;

  s.Printf ("v (1/vcl_cos(M_PI/4 - _thetaL/2)): %f\n\n", v(_LeTau)); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bPruned: %s\n \n", _bPruned ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/

bool SILineArc::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  return bValid;
}
/*
void SILineArc::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Line-Arc Shock (PARABOLA)\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s; 
   s.Printf ("Order: %d\n", order()); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("bActive: %s\n", isActive() ? "yes" : "no"); buf+=s;
   s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("startTime: %f\n", startTime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", _lBElement->id()); buf+=s;
   s.Printf ("rBElement id: %d\n", _rBElement->id()); buf+=s;
   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;

   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n", _EndVector); buf+=s;
   s.Printf ("H: %f\n \n", _H); buf+=s;

   s.Printf ("n: %f\n", _n); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("nu: %d\n", _nu); buf+=s;
   s.Printf ("s: %d =dot(uo, ul) (1:same side)\n", _s); buf+=s;
   s.Printf ("nud: %d =barc->nud (1:CW -1:CCW)\n", _nud); buf+=s;
   s.Printf ("delta: %f\n", _delta); buf+=s;
   s.Printf ("l: %f\n", _l); buf+=s;
   s.Printf ("R: %f\n\n", _R); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bShow: %s\n \n", bShow ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/
bool SIArcArc::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  return bValid;
}
/*
void SIArcArc::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Arc-Arc Shock \n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Order: %d\n", order()); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("bActive: %s\n", isActive() ? "yes" : "no"); buf+=s;
   s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("startTime: %f\n", startTime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("bPropagated: %s\n\n", _bPropagated ? "yes" : "no"); buf+=s;

   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("sigma: %d\n \n", _s); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bShow: %s\n \n", bShow ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/

bool SIThirdOrder::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  return bValid;
}
/*
void SIThirdOrder::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Third Order Shock (BLINE)\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s; 
   s.Printf ("Order: %d\n", order()); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("bActive: %s\n", isActive() ? "yes" : "no"); buf+=s;
   s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("startTime: %f\n", startTime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("bPropagated: %s\n\n", _bPropagated ? "yes" : "no"); buf+=s;

   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("nl: %f (%.2f deg) CW from X-axis\n", _nl, _nl*180/M_PI); buf+=s;
   s.Printf ("ul: %f (%.2f deg) CW from X-axis\n", _ul, _ul*180/M_PI); buf+=s;
   s.Printf ("lL: %f\n", _lL); buf+=s;
   s.Printf ("lR: %f\n \n", _lR); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bShow: %s\n \n", bShow ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/

bool SIArcThirdOrder::validate (Boundary *bnd)
{
  bool bValid = true;

  if (!SILink::validate(bnd))
    return false;

  return bValid;
}
/*
void SIArcThirdOrder::getInfo (ostream& ostrm)
{
  wxString s, buf;
   wxString endtime, simtime;
   wxString endx, endy;
   Point start = getStartPt ();
   Point end = getEndPt ();

   if (_endTime==ISHOCK_DIST_HUGE) {
      endtime.Printf ("INFINITY");
      endx.Printf ("INFINITY");
      endy.Printf ("INFINITY");
   }
   else if (_endTime > MAX_RADIUS) {
      endtime.Printf ("OFF SCREEN");
      endx.Printf ("OFF SCREEN");
      endy.Printf ("OFF SCREEN");
   }
   else {
      endtime.Printf ("%f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Arc-Arc Third Order Shock (ARC)\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Order: %d\n", order()); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("bActive: %s\n", isActive() ? "yes" : "no"); buf+=s;
   s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("startTime: %f\n", startTime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("simTime: %s\n", simtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("bPropagated: %s\n\n", _bPropagated ? "yes" : "no"); buf+=s;

   s.Printf ("LsTau: %f\n", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f\n", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

  s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
  s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
   s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
   s.Printf ("bShow: %s\n \n", bShow ? "yes" : "no"); buf+=s;

   s.Printf ("PruneScore: %.3f\n", _dPnScore);buf+=s;
  s.Printf ("Pruned? %s\n", _bPruned ? "yes" : "no");buf+=s;
  s.Printf ("dA: %.3f\n", _dA);buf+=s;
  s.Printf ("cumuA: %.3f\n", _dcumuA);buf+=s;
  s.Printf ("wholeA: %.3f\n\n", _wholeA);buf+=s;

  ostrm << buf;
}
*/
