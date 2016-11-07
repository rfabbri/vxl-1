#ifndef  _BELEMENTS_DYNVAL_H
#define  _BELEMENTS_DYNVAL_H

#include "belements.h"
#include "dyn-val.h"
#include <vcl_iostream.h>
#include <vcl_cstdio.h>

//#############################################################
//     DYN_VAL
//#############################################################

class BPoint_DynVal : public BPoint
{
protected:
  PointIVRegion _pointIV;  //DYNVAL

public:
  BPoint_DynVal (COORD_TYPE x, COORD_TYPE y, int id, bool bGUI=false, 
              INPUT_TAN_TYPE tangent=TANGENT_UNDEFINED, double conf=0.0);
  virtual ~BPoint_DynVal ();

  PointIVRegion* pointIV() { return &_pointIV;  }
  virtual void getInfo (vcl_ostream& ostrm);
};


class BLine_DynVal : public BLine
{
protected:
  LineIVRegion _lineIV;  //DYNVAL
public:
  BLine_DynVal (BPoint* startpt, BPoint* endpt, int id=-1, bool bGUI=false);
  ~BLine_DynVal ();

  LineIVRegion* lineIV() { return &_lineIV; }

  virtual BPoint_DynVal* s_pt(char dummy='0') { return (BPoint_DynVal*) startPt; }
  virtual BPoint_DynVal* e_pt(char dummy='0') { return (BPoint_DynVal*) endPt; }
  virtual BLine_DynVal* twinLine(char dummy='0') { return (BLine_DynVal*) _twinLine; }

  virtual void reconnect(BPoint* oldPt, BPoint* newPt);
  virtual void getInfo (vcl_ostream& ostrm);
};


class BArc_DynVal : public BArc
{
protected:
    ArcIVRegion _arcIV;
public:
  BArc_DynVal (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
             Point center, double r, ARC_NUD nud);
  virtual ~BArc_DynVal ();

  
  virtual BPoint_DynVal* s_pt(char dummy='0') { return (BPoint_DynVal*) startPt; }
  virtual BPoint_DynVal* e_pt(char dummy='0') { return (BPoint_DynVal*) endPt; }
  virtual BArc_DynVal*  twinArc(char dummy='0') { return (BArc_DynVal*) _twinArc; }

  ArcIVRegion* arcIV() { return &_arcIV; }
  virtual bool isPointValid (Point pt);

  virtual void reconnect(BPoint* oldPt, BPoint* newPt);
};

#endif
