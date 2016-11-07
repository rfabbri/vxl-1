#include "belements-dynval.h"

//#############################################################
//     BPOINT BUCKETING
//#############################################################

BPoint_DynVal::BPoint_DynVal (COORD_TYPE x, COORD_TYPE y, int newid, bool bGUI, INPUT_TAN_TYPE tangent, double conf) :
  BPoint(x, y, newid, bGUI, tangent, conf)
{

}

BPoint_DynVal::~BPoint_DynVal ()
{

}

void BPoint_DynVal::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  BPoint::getInfo (ostrm);

  //DYNVAL
  vcl_sprintf (s, "Dynamic Validation\n"); ostrm<<s;
   vcl_sprintf (s, "bInvalid: %s\n", pointIV()->isIVRegionCompleted() ? "Dead" : "Alive"); ostrm<<s;
   vcl_sprintf (s, "nIVS: %d\n", pointIV()->ivsList()->size()); ostrm<<s;
  vcl_sprintf (s, "IVS(es) of this BPoint: "); ostrm<<s;
  IVRegion::ivsListIterator cur = pointIV()->ivsList()->begin();
   for(; cur!=pointIV()->ivsList()->end(); ++cur) {
      vcl_sprintf (s, "(%f - %f) ", (*cur)->start(), (*cur)->end()); ostrm<<s;
  }
   vcl_sprintf (s, "\n \n"); ostrm<<s;
}

//#############################################################
//     BLINE BUCKETING
//#############################################################

BLine_DynVal::BLine_DynVal (BPoint* startpt, BPoint* endpt, int id, bool bGUI) :
  BLine (startpt, endpt, id, bGUI), _lineIV (_distPointPoint(startpt->pt(), endpt->pt()))
{

}

BLine_DynVal::~BLine_DynVal ()
{

}

void BLine_DynVal::reconnect(BPoint* oldPt, BPoint* newPt)
{
  BLine::reconnect(oldPt, newPt);
  _lineIV.setRegionEnd (_l);
}

void BLine_DynVal::getInfo (vcl_ostream& ostrm)
{
  char s[1024];
  BLine::getInfo (ostrm);

  //DYNVAL
  vcl_sprintf (s, "Dynamic Validation\n"); ostrm<<s;
   vcl_sprintf (s, "bInvalid: %s\n", lineIV()->isIVRegionCompleted() ? "Dead" : "Alive"); ostrm<<s;
  vcl_sprintf (s, "IVRegion (0 - length): (%f - %f)\n", lineIV()->regionStart(), lineIV()->regionEnd()); ostrm<<s;
   vcl_sprintf (s, "nIVS: %d\n", lineIV()->ivsList()->size()); ostrm<<s;
  vcl_sprintf (s, "IVS(es) of this BLine: "); ostrm<<s;
  IVRegion::ivsListIterator cur = lineIV()->ivsList()->begin();
   for(; cur!=lineIV()->ivsList()->end(); ++cur) {
      vcl_sprintf (s, "(%f - %f) ", (*cur)->start(), (*cur)->end()); ostrm<<s;
  }
}

//#############################################################
//     BARC BUCKETING
//#############################################################

BArc_DynVal::BArc_DynVal (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
           Point center, double r, ARC_NUD nud) :
  BArc (startpt, endpt, id, bGUI, center, r, nud),
  _arcIV ( (nud==-1) ? _vPointPoint(center, startpt->pt()) : _vPointPoint(center, endpt->pt()),
        (nud==-1) ? _vPointPoint(center, endpt->pt()) : _vPointPoint(center, startpt->pt()) )
{

}

BArc_DynVal::~BArc_DynVal ()
{

}

//EPSILONISSUE
//Don't forget to refresh _arcIV, too.
void BArc_DynVal::reconnect(BPoint* oldPt, BPoint* newPt)
{

  BArc::reconnect(oldPt, newPt);

  _arcIV.setArcRegionStart ( (_nud==-1) ? _vPointPoint(_center, _start) : _vPointPoint(_center, _end) );
  _arcIV.setArcRegionEnd ( (_nud==-1) ? _vPointPoint(_center, _end) : _vPointPoint(_center, _start) );

}

//Fuzzily valid.
bool BArc_DynVal::isPointValid (Point pt)
{
  VECTOR_TYPE v = _vPointPoint (_center, pt);
  if ( !_arcIV.isVectorFuzzilyValid(v, 1E-13) ) //isVectorValid
    return false;

  DIST_TYPE d = _distPointPoint (_center, pt);
  
  if (_nud==ARC_NUD_CCW && d>_R) return false;
  if (_nud==ARC_NUD_CW  && d<_R) return false;
  if (d==_R) return false; //assert (d != _R); !!!

  return true; //All the others.
}
