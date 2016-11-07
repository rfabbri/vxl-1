#include <extrautils/msgout.h>

#include <vcl_sstream.h>
#include <vcl_cmath.h>

//#include <stdio.h>

#include "ishock-common.h"
#include "sielements.h"

extern MESSAGE_OPTION MessageOption;

void SIA3Source::getInfo (vcl_ostream& ostrm)
{/*
  int i;
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("A3 Source\n\n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==A3SOURCE);
  assert (_graph_type==NODE);

  s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16f\n", _simTime); buf+=s;
  s.Printf ("Origin (Position): (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("nBElement: %d\n", nBElement()); buf+=s;
   s.Printf ("BElements Linked to this Shock: "); buf+=s;
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      s.sprintf ("%d, ", (*curB)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("nPSElement: %d\n", nPSElement()); buf+=s;
   s.Printf ("PSElements Linked to this Shock: "); buf+=s;

  SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
      s.Printf ("%d, ", (*curS)->id()); buf+=s;
  }
   s.sprintf ("\n \n"); buf+=s;

   s.Printf ("cSElement id: %d\n\n", IDifExists(_cSLink)); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SISource::getInfo (vcl_ostream& ostrm)
{/*
  int i;
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("2nd Order Shock Source\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==SOURCE);
  assert (_graph_type==NODE);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16f\n", _simTime); buf+=s;
   s.Printf ("Origin (Position): (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("nBElement: %d\n", nBElement()); buf+=s;
   s.Printf ("BElements Linked to this Shock: "); buf+=s;
  BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      s.sprintf ("%d, ", (*curB)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("cSElement id: %d\n", IDifExists(_cSLink)); buf+=s;
   s.Printf ("cSElement2 id: %d\n\n", IDifExists(_cSLink2)); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SISink::getInfo (vcl_ostream& ostrm)
{/*  
  int i;
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("4rd Order Shock Sink\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
   s.Printf ("Order: %d\n", order()); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==SINK);
  assert (_graph_type==NODE);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16f\n", _simTime); buf+=s;
   s.Printf ("Origin (Position): (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("nBElement: %d\n", nBElement()); buf+=s;
   s.Printf ("BElements Linked to this Shock: "); buf+=s;
   BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      s.sprintf ("%d, ", (*curB)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("nPSElement: %d\n", nPSElement()); buf+=s;
   s.Printf ("PSElements Linked to this Shock: "); buf+=s;

   SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
      s.Printf ("%d, ", (*curS)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SIJunct::getInfo (vcl_ostream& ostrm)
{/*
  int i;
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Shock Junction\n \n"); buf+=s;
   s.Printf ("ID: %d\n", _id); buf+=s;
   s.Printf ("Order: %d\n", order()); buf+=s;
  s.Printf ("_bActive: %s\n", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   assert (_type==JUNCT);
  assert (_graph_type==NODE);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16f\n", _simTime); buf+=s;
   s.Printf ("Origin (Position): (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("nBElement: %d\n", nBElement()); buf+=s;
   s.Printf ("BElements Linked to this Shock: "); buf+=s;
   BElementList::iterator curB = bndList.begin();
   for(; curB!=bndList.end(); ++curB) {
      s.sprintf ("%d, ", (*curB)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("nPSElement: %d\n", nPSElement()); buf+=s;
   s.Printf ("PSElements Linked to this Shock: "); buf+=s;
   SILinksList::iterator curS = PSElementList.begin();
  for(; curS!=PSElementList.end(); ++curS){
      s.Printf ("%d, ", (*curS)->id()); buf+=s;
  }
   s.Printf ("\n \n"); buf+=s;

   s.Printf ("cSElement id: %d\n \n", IDifExists(_cSLink)); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SIPointPoint::getInfo (vcl_ostream& ostrm)
{/*
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
   else {
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Point-Point Shock (LINE)\n \n"); buf+=s;
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

   assert (_type==POINTPOINT);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %s, ", simtime); buf+=s;
   s.Printf ("endTime: %s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

  Point lpt, rpt;
  if (lBElement) lpt = lBPoint()->pt();
  if (rBElement) rpt = rBPoint()->pt();
   s.Printf ("lBPoint id: %d (%f, %f)\n", IDifExists(_lBElement), lpt.x, lpt.y); buf+=s;
   s.Printf ("rBPoint id: %d (%f, %f)\n\n", IDifExists(_rBElement), rpt.x, rpt.y); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n", _rsvector); buf+=s;
   s.Printf ("LsTau: %f (minLtau: %f maxLtau: %f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f (minRtau: %f maxRtau: %f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("n: %f\n \n", _n); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SIPointLineContact::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Point-Line Contact (LINE)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;
   s.Printf ("Type: "); buf+=s;
   switch (_label) {
   case REGULAR: s.Printf ("Regular"); break;
   case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
   case DEGENERATE: s.Printf ("Degenerate"); break;
   case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;

   s.Printf ("Colinear Type: "); buf+=s;
   switch (_ColinearContactType) {
   case ISOLATED_CONTACT: s.Printf ("ISOLATED_CONTACT"); break;
   case LEFT_COLINEAR_CONTACT: s.Printf ("LEFT_COLINEAR_CONTACT"); break;
   case RIGHT_COLINEAR_CONTACT: s.Printf ("RIGHT_COLINEAR_CONTACT"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;

  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;

   assert (_type==POINTLINECONTACT);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d, ", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("LsTau: %f, ", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f, ", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f, ", _H); buf+=s;
   s.Printf ("n: %.15f\n", _n); buf+=s;
  if (_nu==0)
    s.Printf ("nu: %d (LeftPoint-RightLine)\n", _nu);
  else
    s.Printf ("nu: %d (LeftLine-RightPoint)\n", _nu); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SIPointArcContact::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Point-Arc Contact (LINE)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;

   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;

   s.Printf ("Colinear Type: "); buf+=s;
   switch (_ColinearContactType) {
   case ISOLATED_CONTACT: s.Printf ("ISOLATED_CONTACT"); break;
   case LEFT_COLINEAR_CONTACT: s.Printf ("LEFT_COLINEAR_CONTACT"); break;
   case RIGHT_COLINEAR_CONTACT: s.Printf ("RIGHT_COLINEAR_CONTACT"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;

  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s; 

   assert (_type==POINTARCCONTACT);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d, ", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("LsTau: %f, ", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f, ", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f, ", _H); buf+=s;
   s.Printf ("n: %.15f\n", _n); buf+=s;
  if (_nu==0)
    s.Printf ("nu: %d (LeftPoint-RightArc), ", _nu);
  else
    s.Printf ("nu: %d (LeftArc-RightPoint), ", _nu); buf+=s;
  if (_nud==-1)
    s.Printf ("nud: %d (To Center)\n", _nud);
  else
    s.Printf ("nud: %d (Away from Center)\n", _nud); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SIPointLine::getInfo (vcl_ostream& ostrm)
{/*
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
   else {
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Point-Line Shock (PARABOLA)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;
  s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;

  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;    

   assert (_type==POINTLINE);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

  Point pt;

  s.Printf ("nu: %d\n", _nu); buf+=s;
  if (_nu>0) {
    if (lBElement) pt = lBPoint()->pt();
    if (rBElement) start = rBLine()->start();
    if (rBElement) end = rBLine()->end();
    s.Printf ("lBPoint id: %d (%f, %f)\n", IDifExists(_lBElement), pt.x, pt.y); buf+=s;
    s.Printf ("rBLine id: %d (%f, %f)-(%f, %f)\n\n", IDifExists(_rBElement), start.x, start.y, end.x, end.y); buf+=s;
  }
  else {
    if (rBElement) pt = rBPoint()->pt();
    if (lBElement) start = lBLine()->start();
    if (lBElement) end = lBLine()->end();
    s.Printf ("lBLine id: %d (%f, %f)-(%f, %f)\n", IDifExists(_lBElement), start.x, start.y, end.x, end.y); buf+=s;
    s.Printf ("rBPoint id: %d (%f, %f)\n\n", IDifExists(_rBElement), pt.x, pt.y); buf+=s;
  }

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n", _rsvector); buf+=s;
   s.Printf ("LsTau: %f (minLtau: %f maxLtau: %f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f (minRtau: %f maxRtau: %f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("n: %f\n", _n); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("delta: %f\n", _delta); buf+=s;
   s.Printf ("l: %f\n \n", _l); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SIPointArc::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   if (_s>0)
      s.Printf ("Point-Arc Shock (HYPERBOLA)\n \n");
   else
      s.Printf ("Point-Arc Shock (ELLIPSE)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d\n", _order); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("Case: %d of 12\n\n", _case); buf+=s;
  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n\n", 
              start.x, start.y, endx, endy); buf+=s;
   assert (_type==POINTARC);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

  Point pt;
  s.Printf ("nu: %d\n", _nu); buf+=s;
  if (_nu>0) {
    if (lBElement) pt = lBPoint()->pt();
    if (rBElement) start = rBArc()->start();
    if (rBElement) end = rBArc()->end();
    s.Printf ("lBPoint id: %d (%f, %f)\n", IDifExists(_lBElement), pt.x, pt.y); buf+=s;
    s.Printf ("rBArc id: %d (%f, %f)-(%f, %f)\n\n", IDifExists(_rBElement), start.x, start.y, end.x, end.y); buf+=s;
  }
  else {
    if (rBElement) pt = rBPoint()->pt();
    if (lBElement) start = lBArc()->start();
    if (lBElement) end = lBArc()->end();
    s.Printf ("lBArc id: %d (%f, %f)-(%f, %f)\n", IDifExists(_lBElement), start.x, start.y, end.x, end.y); buf+=s;
    s.Printf ("rBPoint id: %d (%f, %f)\n\n", IDifExists(_rBElement), pt.x, pt.y); buf+=s;
  }

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n\n", _rsvector); buf+=s;

   s.Printf ("LsTau: %.16f (minLtau: %.16f maxLtau: %.16f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %.16f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %.16f (minRtau: %.16f maxRtau: %.16f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %.16f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
  s.Printf ("nu: %d\n", _nu); buf+=s;
   s.Printf ("sigma: %d\n \n", _s); buf+=s;

  if (_nu==1)
    s.Printf ("nu: %d (LeftPoint-RightArc)\n", _nu);
  else
    s.Printf ("nu: %d (LeftArc-RightPoint)\n", _nu); buf+=s;
  if (_s==1)
    s.Printf ("sigma: %d (+1:H>R)\n", _s);
  else
    s.Printf ("sigma: %d (-1:R>H)\n", _s); buf+=s;

   s.Printf ("_nudl: %d\n", _nudl); buf+=s;
   s.Printf ("_nudr: %d\n", _nudr); buf+=s;

   s.Printf ("_a: %.16f\n", _a); buf+=s;
   s.Printf ("_b2: %.16f\n", _b2); buf+=s;
   s.Printf ("_c: %.16f\n", _c); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SILineLine::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Line-Line Shock (LINE)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s; 
   assert (_type==LINELINE);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("simTime: %.16s\n", simtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d, ", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock Node id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock Node id: %d\n\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n", _rsvector); buf+=s;
   s.Printf ("LsTau: %f (minLtau: %f maxLtau: %f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f (minRtau: %f maxRtau: %f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
  s.Printf ("(Al)-(Bl): (%f,%f)-(%f,%f)\n", _Al.x, _Al.y,  _Bl.x, _Bl.y); 
  s.Printf ("(Ar)-(Br): (%f,%f)-(%f,%f)\n", _Ar.x, _Ar.y,  _Br.x, _Br.y); 
   s.Printf ("_leftL: %f, ", _lL); buf+=s;
   s.Printf ("_rightL: %f\n", _lR); buf+=s;
   s.Printf ("nl: %f (%.2f deg) CW from X-axis\n", _nl, _nl*180/M_PI); buf+=s;
   s.Printf ("ul: %f (%.2f deg) CW from X-axis\n", _ul, _ul*180/M_PI); buf+=s;
   s.Printf ("nr: %f (%.2f deg) CW from X-axis\n", _nr, _nr*180/M_PI); buf+=s;
   s.Printf ("ur: %f (%.2f deg) CW from X-axis\n", _ur, _ur*180/M_PI); buf+=s;
   s.Printf ("sigma (nl dot nr): %f\n\n", _sigma); buf+=s;

   s.Printf ("thetaL: %f, ", _thetaL); buf+=s;
   s.Printf ("thetaR: %f\n", _thetaR); buf+=s;
   s.Printf ("phi: %f\n", _phi); buf+=s;
   s.Printf ("deltaL: %f dist (Origin, AL), ", _deltaL); buf+=s;
   s.Printf ("deltaR: %f dist (Origin, AR)\n", _deltaR); buf+=s;
   s.Printf ("_N1L (slope): %f, ", _N1L); buf+=s;
  s.Printf ("_N1R (slope): %f\n", _N1R); buf+=s;
  s.Printf ("_N2L: %f, ", _N2L); buf+=s;
  s.Printf ("_N2R: %f\n", _N2R); buf+=s;
  s.Printf ("v (1/vcl_cos(M_PI/4 - _thetaL/2)): %f\n\n", v(_LeTau)); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SILineArc::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Line-Arc Shock (PARABOLA)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("Case: %d of 12\n", _case); buf+=s;
  assert (_case>=1 && _case<=12);

   assert (_type==LINEARC);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("endTime: %.16s, ", endtime); buf+=s;
   s.Printf ("simTime: %.16s\n", simtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("nu: %d\n", _nu); buf+=s;
   s.Printf ("lBElement id: %d\n", _lBElement->id()); buf+=s;
   s.Printf ("rBElement id: %d\n\n", _rBElement->id()); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n\n", _rsvector); buf+=s;

   s.Printf ("LsTau: %f (minLtau: %f maxLtau: %f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f (minRtau: %f maxRtau: %f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n \n", _H); buf+=s;
   s.Printf ("n: %f\n", _n); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("s: %d =dot(uo, ul) (1:same side)\n", _s); buf+=s;
   s.Printf ("nud: %d =barc->nud (1:CW -1:CCW)\n", _nud); buf+=s;
   s.Printf ("delta: %f\n", _delta); buf+=s;
   s.Printf ("l: %f\n", _l); buf+=s;
   s.Printf ("R: %f\n\n", _R); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/
}

void SIArcArc::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Arc-Arc Shock \n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Order: %d, ", _order); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
  buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("Case: %d of 12\n\n", _case); buf+=s;
  assert (_case>=1 && _case<=12);
   assert (_type==ARCARC);
  assert (_graph_type==LINK);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("Origin: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n\n", _rsvector); buf+=s;

   s.Printf ("LsTau: %f (minLtau: %f maxLtau: %f)\n", _LsTau, minLTau(), maxLTau()); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f (minRtau: %f maxRtau: %f)\n", _RsTau, minRTau(), maxRTau()); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("u: %f\n", _u); buf+=s;
   s.Printf ("_nudl: %d\n", _nudl); buf+=s;
   s.Printf ("_nudr: %d\n\n", _nudr); buf+=s;

   s.Printf ("_MU: %d\n", _MU); buf+=s;
  if (_s==1)
    s.Printf ("sigma: %d (+1:H>R)\n", _s);
  else
    s.Printf ("sigma: %d (-1:R>H)\n", _s); buf+=s;

  Point lstart, lend, rstart, rend;
  s.Printf ("nu: %d\n", _nu); buf+=s;
  s.Printf ("lR: %.5f\n", _Rl); buf+=s;
  s.Printf ("rR: %.5f\n", _Rr); buf+=s;
  if (lBElement) lstart = lBArc()->start();
  if (lBElement) lend = lBArc()->end();
  if (rBElement) rstart = rBArc()->start();
  if (rBElement) rend = rBArc()->end();
  if (_nu>0) {
    s.Printf ("Small lBArc id: %d (%f, %f)-(%f, %f)\n",
          IDifExists(_lBElement), lstart.x, lstart.y, lend.x, lend.y); buf+=s;
    s.Printf ("Big rBArc id: %d (%f, %f)-(%f, %f)\n",
          IDifExists(_rBElement), rstart.x, rstart.y, rend.x, rend.y); buf+=s;
  }
  else {
    s.Printf ("Big lBArc id: %d (%f, %f)-(%f, %f)\n",
          IDifExists(_lBElement), lstart.x, lstart.y, lend.x, lend.y); buf+=s;
    s.Printf ("Small rBArc id: %d (%f, %f)\n",
          IDifExists(_rBElement), rstart.x, rstart.y, rend.x, rend.y); buf+=s;
  }

   s.Printf ("_hmu: %d\n", _hmu); buf+=s;
   s.Printf ("_emu: %d\n\n", _emu); buf+=s;

   s.Printf ("_a: %f\n", _a); buf+=s;
   s.Printf ("_b2: %f\n", _b2); buf+=s;
   s.Printf ("_c: %f\n", _c); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SIThirdOrder::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

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
   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("lBElement id: %d\n", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d\n", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d\n", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n", IDifExists(_cSNode)); buf+=s;
   s.Printf ("bPropagated: %s\n\n", _bPropagated ? "yes" : "no"); buf+=s;

   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n", _rsvector); buf+=s;
   s.Printf ("LsTau: %f, ", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f, ", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

   s.Printf ("H: %f\n", _H); buf+=s;
   s.Printf ("nl: %f (%.2f deg) CW from X-axis\n", _nl, _nl*180/M_PI); buf+=s;
   s.Printf ("ul: %f (%.2f deg) CW from X-axis\n", _ul, _ul*180/M_PI); buf+=s;
   s.Printf ("lL: %f\n", _lL); buf+=s;
   s.Printf ("lR: %f\n \n", _lR); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void SIArcThirdOrder::getInfo (vcl_ostream& ostrm)
{/*
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
      endtime.Printf ("%.16f", _endTime);
      endx.Printf ("%f", end.x);
      endy.Printf ("%f", end.y);
   }

   if (_simTime==ISHOCK_DIST_HUGE)
      simtime.Printf ("INFINITY");
   else if (_simTime > MAX_RADIUS) 
      simtime.Printf ("OFF SCREEN");
   else
      simtime.Printf ("%.16f", _simTime);

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Arc-Arc Third Order Shock (ARC)\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n", _edgeID); buf+=s;
  s.Printf ("_bActive: %s, ", _bActive ? "yes" : "no"); buf+=s;
  s.Printf ("_bPropagated: %s\n", _bPropagated ? "yes" : "no"); buf+=s;
   s.Printf ("From-To: (%f, %f)-(%s, %s)\n", 
              start.x, start.y, endx, endy); buf+=s;
   s.Printf ("Order: %d, ", order()); buf+=s;
   s.Printf ("Type : "); buf+=s;
   switch (_label) {
      case REGULAR: s.Printf ("Regular"); break;
      case SEMI_DEGENERATE: s.Printf ("Semi-Degenerate"); break;
      case DEGENERATE: s.Printf ("Degenerate"); break;
      case CONTACT: s.Printf ("Contact"); break;
   }
   buf+=s; s.Printf ("\n"); buf+=s;
   s.Printf ("Case: %d of 4\n", _case); buf+=s;
  assert (_case>=1 && _case<=4);

   s.Printf ("startTime: %.16f\n", _startTime); buf+=s;
   s.Printf ("simTime: %.16s, ", simtime); buf+=s;
   s.Printf ("endTime: %.16s\n", endtime); buf+=s;
   s.Printf ("vStart: (%f, %f)\n \n", _origin.x, _origin.y); buf+=s;

   s.Printf ("Order: %d\n", _order); buf+=s;
   s.Printf ("lBElement id: %d, ", IDifExists(_lBElement)); buf+=s;
   s.Printf ("rBElement id: %d\n", IDifExists(_rBElement)); buf+=s;

   s.Printf ("Left Intersecting Shock id: %d, ", IDifExists(_lNeighbor)); buf+=s;
   s.Printf ("Right Intersecting Shock id: %d\n", IDifExists(_rNeighbor)); buf+=s;
   s.Printf ("Parent Shock id: %d, ", IDifExists(_pSNode)); buf+=s;
   s.Printf ("Child Shock id: %d\n\n", IDifExists(_cSNode)); buf+=s;

   s.Printf ("_ul: %.12f, ", _ul); buf+=s;
   s.Printf ("_ur: %.12f\n", _ur); buf+=s;
   s.Printf ("_lsvector: %.12f, ", _lsvector); buf+=s;
   s.Printf ("_rsvector: %.12f\n", _rsvector); buf+=s;
   s.Printf ("LsTau: %f, ", _LsTau); buf+=s;
   s.Printf ("LeTau: %f\n", _LeTau); buf+=s;
   s.Printf ("RsTau: %f, ", _RsTau); buf+=s;
   s.Printf ("ReTau: %f\n", _ReTau); buf+=s;
   s.Printf ("EndVector: %f\n\n", _EndVector); buf+=s;

  if (MessageOption >= MSG_TERSE) {
    s.Printf ("bIO: %s\n", bIO ? "Inside" : "Outside"); buf+=s;
    s.Printf ("bIOVisited: %s\n", bIOVisited ? "yes" : "no"); buf+=s;
    s.Printf ("IOLabel: %d\n", IOLabel); buf+=s;
    s.Printf ("bHidden: %s\n \n", _bHidden ? "yes" : "no"); buf+=s;

    s.Printf ("PruneCost: %.3f\n", _dPnCost);buf+=s;
    s.Printf ("dOC: %.3f\n", _dOC);buf+=s;
    s.Printf ("dNC: %.3f\n", _dNC);buf+=s;
  }

  ostrm << buf;
*/}

void VisFrag::getInfo (vcl_ostream& ostrm)
{/*
  wxString s, buf;

   s.Printf ("\n==============================\n"); buf+=s;
   s.Printf ("Visual Fragment\n \n"); buf+=s;
   s.Printf ("ID: %d, ", _id); buf+=s;
  s.Printf ("EDGE ID: %d\n\n", _shock_edge->id()); buf+=s;

  s.Printf ("average_intensity: %.2f, ", _avg_intensity); buf+=s;
  s.Printf ("Svcl_tandard Deviation: %.2f, ", _std_dev); buf+=s;

  ostrm << buf;
*/}
