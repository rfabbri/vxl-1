#ifndef  _BELEMENTS_BUCKETING_H
#define  _BELEMENTS_BUCKETING_H

//#############################################################
//     BUCKETING
//#############################################################

#include "belements.h"
#include <vcl_set.h>

class Bucket;
#ifndef _BUCKET_H
typedef vcl_set<Bucket* > BucketList;
#endif


#include <vcl_iostream.h>
#include <vcl_cstdio.h>

class BPoint_Bucketing;
class BLine_Bucketing;
class BArc_Bucketing;

//Point-Point VisibilityConstraint
class PPVisCon {
public:
  double _vectorx, _vectory;
  PPVisCon () {
    //_vectorx = -1;
    //_vectory = -1;
  }
  PPVisCon (double vx, double vy) {
    _vectorx = vx;
    _vectory = vy;
  }
};

class EPVisCon {
public:
  int  _id; //keep track of where this constraint is from!
  double _vectorx, _vectory;
  EPVisCon () {}
  EPVisCon (double vx, double vy, int id) {
    _id = id;
    _vectorx = vx;
    _vectory = vy;
  }
};

//Line-Point VisibilityConstraint
class LPVisCon {
public:
  PPVisCon _sp;
  PPVisCon _ep;

  LPVisCon () {}
};

//Arc-Point VisibilityConstraint
class APVisCon {
public:
  Point    _sPoint;  //spPoint is either startPt or the startTangetPt
  Point    _ePoint;  //epPoint is either endPt or the endTangetPt
  PPVisCon _sp;
  PPVisCon _ep;

  APVisCon () {}
};

typedef vcl_vector <PPVisCon*> PPVisConType;
typedef vcl_vector <EPVisCon*> EPVisConType;

class BArc_Bucketing;

class BPoint_Bucketing : public BPoint
{
public:
  
  Bucket** __BucketArray;
  int    _bucX, _bucY;

  BPoint_Bucketing (COORD_TYPE x, COORD_TYPE y, int id, bool bGUI=false, 
              INPUT_TAN_TYPE tangent=TANGENT_UNDEFINED, double conf=0.0);
  virtual ~BPoint_Bucketing ();

  Bucket* getBucket (void);

  //The original Visibility Constraint for a pair of points.
  PPVisConType _PPVisCon;
  void addPPVisCon (double vectorx, double vectory);

  //Visibility Constraint for a endPt
  EPVisConType _EPVisCon;
  void addEPVisCon (double vectorx, double vectory, int id);
  void delEPVisCon (int id);

  //virtual void mergeWith (BPoint* bpt);

  //Visibility Constraint for Point-Line:
  //treat it as PPVisCon from this Point to footPt.
  void addPLVisCon (BLine* bline, double t);
  void addPLVisCon (double vectorx, double vectory);
  void addPAVisCon (double vectorx, double vectory);

  bool PisPointVisible (Point testPt);
  bool PisLineVisible (COORD_TYPE sx, COORD_TYPE sy, 
                COORD_TYPE ex, COORD_TYPE ey);
  bool PisRectVisible (COORD_TYPE L, COORD_TYPE T, 
                COORD_TYPE R, COORD_TYPE B);
  bool PisBArcVisible (BArc_Bucketing* barc);

  virtual void getInfo (vcl_ostream& ostrm);
};


class BLine_Bucketing : public BLine
{
public:
  Bucket** __BucketArray;
  //If the line is inside one bucket, index it using[_bucLX][_bucLY]
  //Else, use _lineInBuckets, in this case, _bucLX=_bucLY=-1.
  int _bucLX, _bucLY;

  //Put the BLine into curBucket, and put a record of curBucket to the BLine
  BucketList _lineInBuckets;

  virtual BPoint_Bucketing* s_pt(char dummy='0') {
    return (BPoint_Bucketing*) startPt;
  }
  virtual BPoint_Bucketing* e_pt(char dummy='0') {
    return (BPoint_Bucketing*) endPt;
  }
  virtual BLine_Bucketing* twinLine(char dummy='0') {
    return (BLine_Bucketing*) _twinLine;
  }

  BLine_Bucketing (BPoint* startpt, BPoint* endpt, int id=-1, bool bGUI=false);
  ~BLine_Bucketing ();

  void addInBucket (Bucket* curBucket);

  LPVisCon* _LPVisCon; //NULL: no visibility constraint.
  void updateLPVisCon (Point pt);
  bool isPointVisibleUnderCon (Point pt);

  //Test for isPointValidInTPlaneFuzzy and _LPVisCon
  //if visible, also get t in return.
  bool LisPointVisible (Point pt, double& t);
  bool LisRectVisible (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B);
  bool LisArcVisible (BArc_Bucketing* barc, double& t);

  virtual void reconnect (BPoint* oldPt, BPoint* newPt);

  virtual void getInfo (vcl_ostream& ostrm);
};


class BArc_Bucketing : public BArc
{
public:

  Bucket** __BucketArray;
  //If the arc is inside one bucket, index it using[_bucAX][_bucAY]
  //Else, use vcl_set _arcInBuckets, in this case, _bucAX=_bucAY=-1.
  int _bucAX, _bucAY;

  //Put the BArc into curBucket, and put a record of curBucket to the BArc
  BucketList _arcInBuckets;

  virtual BPoint_Bucketing* s_pt(char dummy='0') { return (BPoint_Bucketing*) startPt; }
  virtual BPoint_Bucketing* e_pt(char dummy='0') { return (BPoint_Bucketing*) endPt; }
  virtual BArc_Bucketing*  twinArc(char dummy='0') { return (BArc_Bucketing*) _twinArc; }

  BArc_Bucketing (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
             Point center, double r, ARC_NUD nud);
  virtual ~BArc_Bucketing ();

  void addInBucket (Bucket* curBucket);

  void getBoundingBox (COORD_TYPE& L, COORD_TYPE& T, COORD_TYPE& R, COORD_TYPE& B);
  bool isLineIntersecting (COORD_TYPE lsx, COORD_TYPE lsy, COORD_TYPE lex, COORD_TYPE ley);
  bool isHLineIntersecting (COORD_TYPE lsx, COORD_TYPE lex, COORD_TYPE ly);
  bool isVLineIntersecting (COORD_TYPE lsy, COORD_TYPE ley, COORD_TYPE lx);
  bool isRectIntersecting (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B);

  APVisCon* _APVisCon; //NULL: no visibility constraint.
  void updateAPVisCon (Point pt);
    bool _getLeftTangentPoint (Point pt, Point& LeftTangentPoint);
    bool _getRightTangentPoint (Point pt, Point& RightTangentPoint);
  bool isPointVisibleUnderCon (Point pt);

  //Test for isPointValidInADist and _APVisCon
  //if visible, get a in return.
  bool AisPointVisible (Point pt, VECTOR_TYPE& a);
  bool AisLineVisible (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey);
  bool AisRectVisible (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B);

  virtual void reconnect (BPoint* oldPt, BPoint* newPt);

  virtual void getInfo (vcl_ostream& ostrm);
};



#endif
