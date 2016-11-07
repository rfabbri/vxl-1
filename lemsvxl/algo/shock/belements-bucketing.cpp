#include "belements-bucketing.h"
#include "bucket.h"

//#############################################################
//     BPOINT BUCKETING
//#############################################################

BPoint_Bucketing::BPoint_Bucketing (COORD_TYPE x, COORD_TYPE y, int newid, bool bGUI, INPUT_TAN_TYPE tangent, double conf) :
  BPoint(x, y, newid, bGUI, tangent, conf)
{
  //_PPVisCon.clear ();
  _EPVisCon.clear ();
  __BucketArray = NULL;
}

BPoint_Bucketing::~BPoint_Bucketing ()
{
  //Go through the _PPVisCon and delete all of them...
  PPVisConType::iterator curPPV = _PPVisCon.begin();
   for (; curPPV!=_PPVisCon.end(); ++curPPV) {
      delete (*curPPV);
  }
  _PPVisCon.clear ();

  //Go through the _EPVisCon and delete all of them...
  EPVisConType::iterator curEPV = _EPVisCon.begin();
   for (; curEPV!=_EPVisCon.end(); ++curEPV) {
      delete (*curEPV);
  }
  _EPVisCon.clear ();
}

Bucket* BPoint_Bucketing::getBucket (void)
{
  assert (__BucketArray);
  return &__BucketArray[_bucX][_bucY];
}

//Add Point-Point VisibilityConstraint
void BPoint_Bucketing::addPPVisCon (double vectorx, double vectory)
{
  PPVisCon *vc = new PPVisCon (vectorx,vectory);
  _PPVisCon.push_back (vc);
}

//Add End-Point VisibilityConstraint, for endPoint of a BLine or BArc
void BPoint_Bucketing::addEPVisCon (double vectorx, double vectory, int id)
{
  EPVisCon *vc = new EPVisCon (vectorx, vectory, id);
  _EPVisCon.push_back (vc);
}

//Given the connected BLine or BArc's id, delete the corresponding EPVisCon.
void BPoint_Bucketing::delEPVisCon (int id)
{
  EPVisConType::iterator EPit = _EPVisCon.begin();
  for(; EPit!=_EPVisCon.end(); ++EPit) {
    if ((*EPit)->_id==id) { //found it!
      _EPVisCon.erase (EPit);
      return;
    }
  }
  assert (0);
}

/*void BPoint_Bucketing::mergeWith (BPoint* bpt)
{
  BLine_Bucketing* connectedBLine;
  BArc_Bucketing*  connectedBArc;
  //We need to take the connectivity data from bpt and put it
  //into the linked element list of the current element
  //at the same time we need to update the other elements of
  //their change in connectivity
  BElementList::iterator curB = bpt->LinkedBElmList.begin();
   for(; curB!=bpt->LinkedBElmList.end(); ++curB) {
    if ((*curB)->type()==BLINE) {
      connectedBLine = (BLine_Bucketing*)(*curB);
      connectedBLine->reconnect (bpt, this);
      connectTo (connectedBLine);
    }
    else if ((*curB)->type()==BARC) {
      connectedBArc = (BArc_Bucketing*)(*curB);
      connectedBArc->reconnect (bpt, this);
      connectTo (connectedBArc);
    }
  }

  //choose the one with higher confidence
  if (this->hasATangent() && bpt->hasATangent()){
    if (bpt->conf() > this->conf()){
      this->set_tangent(bpt->tangent());
    }
  }
}*/

//Visibility Constraint for Point-Line:
//treat it as PPVisCon from this Point to footPt.
//For small line segments, we can ignore this,
//because we are not going to earn much by adding this constraint.
void BPoint_Bucketing::addPLVisCon (BLine* bline, double t)
{
  Point footPt = _getFootPt (bline->start(), bline->end(), t);
  PPVisCon *vc = new PPVisCon (footPt.x-_pt.x, footPt.y-_pt.y);
  _PPVisCon.push_back (vc);
}

//The other form of addPLVisCon, similar to addPPVisCon
void BPoint_Bucketing::addPLVisCon (double vectorx, double vectory)
{
  PPVisCon *vc = new PPVisCon (vectorx,vectory);
  _PPVisCon.push_back (vc);
}


//The other form of addPAVisCon, similar to addPPVisCon
void BPoint_Bucketing::addPAVisCon (double vectorx, double vectory)
{
  PPVisCon *vc = new PPVisCon (vectorx,vectory);
  _PPVisCon.push_back (vc);
}

//EPSILONISSUE 1: EPVisCon
//EP_EPSILON CAN NOT BE TOO BIG.
//return true for visible
//return false for invisible
bool BPoint_Bucketing::PisPointVisible (Point testPt)
{
  //1)Check _PPVisCon
  //If any of them _dot < 0, invisible
  //'=' is treated as visible here!
  PPVisConType::iterator ppvit = _PPVisCon.begin();
  for (; ppvit!=_PPVisCon.end(); ppvit++) {

    ///030714: This change just save 0.05 to 0.1 sec out of 16 sec for 300KPoints.
    ///double vectorx = (*vit)->_vectorx;
    ///double vectory = (*vit)->_vectory;
    ///if (vectorx*(vectorx+_pt.x-testPt.x)+vectory*(vectory+_pt.y-testPt.y) < 0)

    //the _dot product for _PPVisCon.
    if ( (*ppvit)->_vectorx * ((*ppvit)->_vectorx+_pt.x-testPt.x) + 
        (*ppvit)->_vectory * ((*ppvit)->_vectory+_pt.y-testPt.y) < 0 )
      return false;
  }

  //2)Check _EPVisCon, if this is an endPt of a BLine or BArc
  //'=' is treated as invisible here!
  EPVisConType::iterator epvit = _EPVisCon.begin();
  for (; epvit!=_EPVisCon.end(); epvit++) {

    //the _dot product for _EPVisCon.
    ///Use fuzzy to compare angle here! _isG of EP_EPSILON (A_EPSILON)
    //normalize the dot product to approx theta (work for small theta)
    //if ( (*epvit)->_vectorx * (testPt.x-_pt.x) + 
    //     (*epvit)->_vectory * (testPt.y-_pt.y) > 0 )
    double dot = (*epvit)->_vectorx * (testPt.x-_pt.x) + 
             (*epvit)->_vectory * (testPt.y-_pt.y);
    double theta = dot/hypot(testPt.x-_pt.x, testPt.y-_pt.y);
    if ( _isG(theta, 0, EP_EPSILON) ) //A_EPSILON
      return false;
  }

  //3)If any of the constraints fuzzily valid, call BLine's constraint to test it.

  return true;
}

//For testing a line is visible or not,
//we have to go through the visibility constraints and
//test for both end points at the same time.
bool BPoint_Bucketing::PisLineVisible (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey)
{
  //1)If any of the visibility constraints invalidate both end points at the same time,
  //  then the line is invisible.
  PPVisConType::iterator vit = _PPVisCon.begin();
  for (; vit!=_PPVisCon.end(); vit++) {

    double vectorx = (*vit)->_vectorx;
    double vectory = (*vit)->_vectory;
    double px = _pt.x;
    double py = _pt.y;

    //1-1)if both S and E are invisible, then it is invisible.
    if (vectorx*(vectorx+px-sx)+vectory*(vectory+py-sy) < 0 &&
       vectorx*(vectorx+px-ex)+vectory*(vectory+py-ey) < 0)
      return false;
    //1-2)else, test the next constraint.
  }

  //2)Check _EPVisCon, if this is an endPt of a BLine or BArc
  //'=' is treated as invisible here!
  EPVisConType::iterator epvit = _EPVisCon.begin();
  for (; epvit!=_EPVisCon.end(); epvit++) {

    //the _dot product for _EPVisCon.
    if ( ((*epvit)->_vectorx * (sx-_pt.x) + 
         (*epvit)->_vectory * (sy-_pt.y) > 0) &&
        ((*epvit)->_vectorx * (ex-_pt.x) + 
         (*epvit)->_vectory * (ey-_pt.y) > 0) )
      return false;
  }

  //3)Finally here, we consider the Line to be visible.
  return true;
}

//For testing a bucket is visible or not,
//we have to go through the visibility constraint and
//test for all 4 corner points at the same time.
bool BPoint_Bucketing::PisRectVisible (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B)
{
  //1)If any of the P-P visibility constraints invalidate all 4 corners at the same time,
  //  then the rect is invisible.
  PPVisConType::iterator vit = _PPVisCon.begin();
  for (; vit!=_PPVisCon.end(); vit++) {

    //1-1)if all 4 corners are invisible, then it is invisible.
    if ( ((*vit)->_vectorx*((*vit)->_vectorx+_pt.x-L)+(*vit)->_vectory*((*vit)->_vectory+_pt.y-T) < 0) &&
        ((*vit)->_vectorx*((*vit)->_vectorx+_pt.x-R)+(*vit)->_vectory*((*vit)->_vectory+_pt.y-T) < 0) &&
        ((*vit)->_vectorx*((*vit)->_vectorx+_pt.x-L)+(*vit)->_vectory*((*vit)->_vectory+_pt.y-B) < 0) &&
        ((*vit)->_vectorx*((*vit)->_vectorx+_pt.x-R)+(*vit)->_vectory*((*vit)->_vectory+_pt.y-B) < 0) )
      return false;
    //1-2)else, test the next constraint.
  }

  //2)Check _EPVisCon, if this is an endPt of a BLine or BArc
  //'=' is treated as invisible here!
  //EPSILONISSUE 
  //Should normalize the result here, too.
  EPVisConType::iterator epvit = _EPVisCon.begin();
  for (; epvit!=_EPVisCon.end(); epvit++) {

    //the _dot product for _EPVisCon.
    if ( ((*epvit)->_vectorx * (L-_pt.x) + (*epvit)->_vectory * (T-_pt.y) > 0) &&
        ((*epvit)->_vectorx * (R-_pt.x) + (*epvit)->_vectory * (T-_pt.y) > 0) &&
        ((*epvit)->_vectorx * (L-_pt.x) + (*epvit)->_vectory * (B-_pt.y) > 0) &&
        ((*epvit)->_vectorx * (R-_pt.x) + (*epvit)->_vectory * (B-_pt.y) > 0) )
      return false;
  }
  //3)Finally here, we consider the rect to be visible.
  return true;
}

bool BPoint_Bucketing::PisBArcVisible (BArc_Bucketing* barc)
{
  COORD_TYPE L, T, R, B;
  barc->getBoundingBox (L, T, R, B);
  return PisRectVisible (L, T, R, B);
}

extern Bucket** __BucketArray;
void BPoint_Bucketing::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  BPoint::getInfo (ostrm);

  vcl_sprintf(s, "BUCKETING:\n"); 
    ostrm<<s;
  if (__BucketArray != NULL)
    vcl_sprintf (s, "BPoint in Bucket[%d][%d], BucketId: %d\n\n", 
          _bucX, _bucY, __BucketArray[_bucX][_bucY].id());
  else
    vcl_sprintf (s, "Bucket not initialized yet.\n\n");
  ostrm<<s;

  vcl_sprintf (s, "%d PPVisCon Vector(s):", _PPVisCon.size()); ostrm<<s;
  PPVisConType::iterator curV = _PPVisCon.begin();
  for(; curV!=_PPVisCon.end(); ++curV) {
     vcl_sprintf (s, "(%f, %f), ", (*curV)->_vectorx, (*curV)->_vectory); ostrm<<s;
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;
}

//#############################################################
//     BLINE BUCKETING
//#############################################################

BLine_Bucketing::BLine_Bucketing (BPoint* startpt, BPoint* endpt, int id, bool bGUI) :
  BLine (startpt, endpt, id, bGUI)
{
  //BUCKETING:
  _LPVisCon = NULL;
  __BucketArray = NULL;
}

BLine_Bucketing::~BLine_Bucketing ()
{
  delete _LPVisCon;
  _LPVisCon = NULL;
}

//EPSILONISSUE 20
//Don't forget to update _EPVisCon here, too.
void BLine_Bucketing::reconnect (BPoint* oldPt, BPoint* newPt)
{
  BLine::reconnect (oldPt, newPt);
  
  //2)delete oldPt's _EPVisCon from this BLine (skip),
  //  oldPt will be deleted later on.

  //3)If BLine is a GUILine, add this BLine to newPt's _EPVisCon
  if (_bGUIElm) {
    if(newPt==s_pt())
      ((BPoint_Bucketing*)newPt)->addEPVisCon ((_end.x-_start.x)/_l, (_end.y-_start.y)/_l, _id);
    else
      ((BPoint_Bucketing*)newPt)->addEPVisCon ((_start.x-_end.x)/_l, (_start.y-_end.y)/_l, _id);
  }
}

void BLine_Bucketing::addInBucket (Bucket* curBucket)
{
  curBucket->addBElement (this);
  _lineInBuckets.insert (curBucket);
  //assert (_bucLX==-1); aassert (_bucLY==-1);
}

void BLine_Bucketing::updateLPVisCon (Point pt)
{
  if (_LPVisCon==NULL)
    _LPVisCon = new LPVisCon;
  _LPVisCon->_sp._vectorx = pt.x - _start.x;
  _LPVisCon->_sp._vectory = pt.y - _start.y;
  _LPVisCon->_ep._vectorx = pt.x - _end.x;
  _LPVisCon->_ep._vectory = pt.y - _end.y;
}

//Test for _LPVisCon
//Note that the invisible zone is the intersection of the two constraints.
bool BLine_Bucketing::isPointVisibleUnderCon (Point testPt)
{
  if (_LPVisCon) {
    bool bSinv = false;
    bool bEinv = false;
    //vector: Start->P
    if ( _LPVisCon->_sp._vectorx * (_LPVisCon->_sp._vectorx+_start.x-testPt.x) + 
        _LPVisCon->_sp._vectory * (_LPVisCon->_sp._vectory+_start.y-testPt.y) < 0 )
      bSinv = true;
    //vector: End->P
    if ( _LPVisCon->_ep._vectorx * (_LPVisCon->_ep._vectorx+_end.x-testPt.x) + 
        _LPVisCon->_ep._vectory * (_LPVisCon->_ep._vectory+_end.y-testPt.y) < 0 )
      bEinv = true;

    if (bSinv && bEinv)
      return false;
  }

  //If no _LPVisCon, it's just valid.
  return true;
}

//EPSILONISSUE 6: HAVE TO BE CONSISTENT WITH EPSILONISSUE 1
//SO DON'T USE T
bool BLine_Bucketing::LisPointVisible (Point pt, double& t)
{
  t = isPointValidInTPlaneFuzzy (pt);
  if (t==0)
    return false;

  if (!isPointVisibleUnderCon (pt))
    return false;

  return true;
}

//For testing a bucket is visible or not,
//we have to go through the visibility constraint and
//test for all 4 corner points at the same time.
bool BLine_Bucketing::LisRectVisible (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B)
{
  //0)Compute tLT, tRT, tLB, tRB.
  double tLT = _getT (Point (L, T), _start, _end);
  double tRT = _getT (Point (R, T), _start, _end);
  double tLB = _getT (Point (L, B), _start, _end);
  double tRB = _getT (Point (R, B), _start, _end);

  //1)For all corner points, test if t<0, if so, it's invalid.
  if (tLT<0 && tRT<0 && tLB<0 && tRB<0)
    return false;

  //2)For all corner points, test if t>1, if so, it's invalid.
  if (tLT>1 && tRT>1 && tLB>1 && tRB>1)
    return false;

  //3)If there is no _LPVisCon constraint, it's valid at this step.
  if (_LPVisCon == NULL)
    return true;

  //4-1)For all corner points, test if dot with _LPVisCon._sp is invalid.
  bool visibSp = true;
  if ( (_LPVisCon->_sp._vectorx * (_LPVisCon->_sp._vectorx+_start.x-L) + 
      _LPVisCon->_sp._vectory * (_LPVisCon->_sp._vectory+_start.y-T) < 0) &&
      (_LPVisCon->_sp._vectorx * (_LPVisCon->_sp._vectorx+_start.x-R) + 
      _LPVisCon->_sp._vectory * (_LPVisCon->_sp._vectory+_start.y-T) < 0) &&      
      (_LPVisCon->_sp._vectorx * (_LPVisCon->_sp._vectorx+_start.x-L) + 
      _LPVisCon->_sp._vectory * (_LPVisCon->_sp._vectory+_start.y-B) < 0) &&
      (_LPVisCon->_sp._vectorx * (_LPVisCon->_sp._vectorx+_start.x-R) + 
      _LPVisCon->_sp._vectory * (_LPVisCon->_sp._vectory+_start.y-B) < 0) )
    visibSp = false;

  //4-2)For all corner points, test if dot with _LPVisCon._ep is invalid.
  bool visibEp = true;
  if ( (_LPVisCon->_ep._vectorx * (_LPVisCon->_ep._vectorx+_start.x-L) + 
      _LPVisCon->_ep._vectory * (_LPVisCon->_ep._vectory+_start.y-T) < 0) &&
      (_LPVisCon->_ep._vectorx * (_LPVisCon->_ep._vectorx+_start.x-R) + 
      _LPVisCon->_ep._vectory * (_LPVisCon->_ep._vectory+_start.y-T) < 0) &&      
      (_LPVisCon->_ep._vectorx * (_LPVisCon->_ep._vectorx+_start.x-L) + 
      _LPVisCon->_ep._vectory * (_LPVisCon->_ep._vectory+_start.y-B) < 0) &&
      (_LPVisCon->_ep._vectorx * (_LPVisCon->_ep._vectorx+_start.x-R) + 
      _LPVisCon->_ep._vectory * (_LPVisCon->_ep._vectory+_start.y-B) < 0) )
    visibEp = false;

  //4-3)the rect is invisible only if invisible to both _sp and _ep.
  if (visibSp==false && visibEp==false)
    return false;

  //5)Finally here, we consider the rect to be visible.
  return true;
}

//Note that the center of the arc test is good for invalidating arcs,
//but it's not a necessary condition.
bool BLine_Bucketing::LisArcVisible (BArc_Bucketing* barc, double& t)
{
  //1)test for t of center of the arc...
  t = isPointValidInTPlaneFuzzy (barc->center());
  if (t==0)
    return false;

  //2)test the bounding box of the arc...
  COORD_TYPE L, T, R, B;
  barc->getBoundingBox (L, T, R, B);
  
  return LisRectVisible (L, T, R, B);
}

void BLine_Bucketing::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  BLine::getInfo (ostrm);

  //BUCKETING
  vcl_sprintf (s, "BUCKETING:\n"); ostrm<<s;

  if (__BucketArray == NULL)
    vcl_sprintf (s, "Bucket not initialized yet.\n\n");
  else {
    if (_bucLX<0) { //print the list of buckets in the list
      vcl_sprintf (s, "BLine in Buckets: "); ostrm<<s;
      BucketList::iterator bit = _lineInBuckets.begin();
      for (; bit != _lineInBuckets.end(); bit++) {
        vcl_sprintf (s, "[%d][%d], ", (*bit)->index().h, (*bit)->index().v); ostrm<<s;
      }
      vcl_sprintf (s, "\n"); ostrm<<s;
  
      //twinLine:
      vcl_sprintf (s, "twinLine in Buckets: "); ostrm<<s;
      bit = twinLine()->_lineInBuckets.begin();
      for (; bit != twinLine()->_lineInBuckets.end(); bit++) {
        vcl_sprintf (s, "[%d][%d], ", (*bit)->index().h, (*bit)->index().v); ostrm<<s;
      }
      vcl_sprintf (s, "\n"); ostrm<<s;
    }
    else {
      vcl_sprintf (s, "BLine in Bucket[%d][%d], BucketId: %d\n", 
            _bucLX, _bucLY, 
            __BucketArray[_bucLX][_bucLY].id()); ostrm<<s;
      vcl_sprintf (s, "twin BLine in Bucket[%d][%d], BucketId: %d\n", 
            twinLine()->_bucLX, twinLine()->_bucLY, 
            __BucketArray[twinLine()->_bucLX][twinLine()->_bucLY].id()); ostrm<<s;
    }
    vcl_sprintf (s, "s_pt() in Bucket[%d][%d], BucketId: %d\n", 
          s_pt()->_bucX, s_pt()->_bucY, 
          __BucketArray[s_pt()->_bucX][s_pt()->_bucY].id()); ostrm<<s;
    vcl_sprintf (s, "e_pt() in Bucket[%d][%d], BucketId: %d\n\n", 
          e_pt()->_bucX, e_pt()->_bucY, 
          __BucketArray[e_pt()->_bucX][e_pt()->_bucY].id()); ostrm<<s;
  }
  //Line-Point Visibility Constraint
  if (_LPVisCon) {
    vcl_sprintf (s, "2 LPVisCon Vectors:\n"); ostrm<<s;
    vcl_sprintf (s, "vStartP: (%f, %f), vEndP (%f, %f)\n\n",
          _LPVisCon->_sp._vectorx, _LPVisCon->_sp._vectory,
          _LPVisCon->_ep._vectorx, _LPVisCon->_ep._vectory); ostrm<<s;
  }
  else {
    vcl_sprintf (s, "No LPVisCon Vector.\n\n"); ostrm<<s;
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;
}

//#############################################################
//     BARC BUCKETING
//#############################################################

BArc_Bucketing::BArc_Bucketing (BPoint* startpt, BPoint* endpt, int id, bool bGUI,
           Point center, double r, ARC_NUD nud) :
  BArc (startpt, endpt, id, bGUI, center, r, nud)
{
  //BUCKETING:
  _APVisCon = NULL;
  __BucketArray = NULL;
}

BArc_Bucketing::~BArc_Bucketing ()
{
  delete _APVisCon;
  _APVisCon = NULL;
}

//EPSILONISSUE 20
//Don't forget to update _EPVisCon here, too.
void BArc_Bucketing::reconnect (BPoint* oldPt, BPoint* newPt)
{
  BArc::reconnect (oldPt, newPt);
  
  //2)Deletion of oldPt's _EPVisCon from this BArc is SKIPPED,
  //  becase oldPt will be deleted later on.

  //3)If BArc is a GUIArc, add this BArc to newPt's _EPVisCon
  if (_bGUIElm) {
    if (newPt==startPt)
      ((BPoint_Bucketing*)newPt)->addEPVisCon (vcl_cos(_InTangent), vcl_sin(_InTangent), _id);
    else
      ((BPoint_Bucketing*)newPt)->addEPVisCon (vcl_cos(_OutTangent+M_PI), vcl_sin(_OutTangent+M_PI), _id);
  }
}


//################## BARC BUCKETING ##################

void BArc_Bucketing::addInBucket (Bucket* curBucket)
{
  curBucket->addBElement (this);
  _arcInBuckets.insert (curBucket);
  //assert (_bucAX==-1); aassert (_bucAY==-1);
}

bool BArc_Bucketing::isLineIntersecting (COORD_TYPE lsx, COORD_TYPE lsy, COORD_TYPE lex, COORD_TYPE ley)
{
  //Compute the length of Arc center to footPt.
  double CF = _distPointLine (_center, Point(lsx, lsy), Point(lex, ley));
  if (CF>_R) //1)CF>r, line outside circle
    return false;
  else {
    double distLsC = hypot (lsx-_center.x, lsy-_center.y);
    double distLeC = hypot (lex-_center.x, ley-_center.y);
    if (distLsC>_R && distLeC>_R) { //2)both>R, line outside circle
      //2-1)check if t is valid
      double t = _getT (_center, Point(lsx,lsy), Point(lex, ley));
      if (t<0 && t>1)
        return false;
    }
    else if (distLsC<_R && distLeC<_R) //3)both<R, line inside circle
      return false;
    else {//4)must have intersection, solve for I
      //4-1)solve for I. Note that ther may be 2 solutions for I.
      double a = (ley-lsy)/(lex-lsx);
      double b = (lex*lsy-ley*lsx + _center.x*(ley-lsy) - _center.y*(lex-lsx))/(lex-lsx);
      double delta = _R*_R*(1+a*a)-b*b;
      assert (delta>=0);
      double sqdelta = vcl_sqrt (delta);

      double x1 = (-a*b + sqdelta)/(1+a*a);
      double x2 = (-a*b - sqdelta)/(1+a*a);

      //4-2)if lsx<=x1<=lex, x1 is valid
      if (lsx<=x1 && x1<=lex) {
        //test if vCI in (S, E) of BArc or not
        double vCI = _vPointPoint (Point(0, 0), Point(x1, a*x1+b));
        if (isVectorValid(vCI))
          return true;
      }

      //4-3)if lsx<=x2<=lex, x2 is valid
      if (lsx<=x2 && x2<=lex) {
        double vCI = _vPointPoint (Point(0, 0), Point(x2, a*x2+b));
        if (isVectorValid(vCI))
          return true;
      }
    }
  }

  return false;
}

bool BArc_Bucketing::isHLineIntersecting (COORD_TYPE lsx, COORD_TYPE lex, COORD_TYPE ly)
{
  //Compute the length of Arc center to footPt.
  double CF = vcl_fabs (ly-_center.y);
  if (CF>=_R) //1)CF>r, line outside circle
    return false;
  else {
    double distLsC = hypot (lsx-_center.x, ly-_center.y);
    double distLeC = hypot (lex-_center.x, ly-_center.y);
    if (distLsC>_R && distLeC>_R) { //2)both>R, line outside circle
      //2-1)check if cx between lsx and lex
      if (lsx>_center.x || _center.x>lex)
        return false;
    }
    if (distLsC<_R && distLeC<_R) //3)both<R, line inside circle
      return false;
    else {//4)must have intersection, solve for I
      //4-1)solve for I. Note that ther may be 2 solutions for I.
      //    line: y=b
      double b = ly - _center.y;
      double delta = _R*_R-b*b;
      assert (delta>0);
      double sqdelta = vcl_sqrt (delta);

      double x1 = sqdelta + _center.x;
      double x2 = -sqdelta + _center.x;

      //4-2)if lsx<=x1<=lex, x1 is valid
      if (lsx<=x1 && x1<=lex) {
        //test if vCI in (S, E) of BArc or not
        double vCI = _vPointPoint (Point(0, 0), Point(sqdelta, b));
        if (isVectorValid(vCI))
          return true;
      }

      //4-3)if lsx<=x2<=lex, x2 is valid
      if (lsx<=x2 && x2<=lex) {
        double vCI = _vPointPoint (Point(0, 0), Point(-sqdelta, b));
        if (isVectorValid(vCI))
          return true;
      }
    }
  }

  return false;
}

bool BArc_Bucketing::isVLineIntersecting (COORD_TYPE lsy, COORD_TYPE ley, COORD_TYPE lx)
{
  //Compute the length of Arc center to footPt.
  double CF = vcl_fabs (lx-_center.x);
  if (CF>=_R) //1)CF>r, line outside circle
    return false;
  else {
    double distLsC = hypot (lx-_center.x, lsy-_center.y);
    double distLeC = hypot (lx-_center.x, ley-_center.y);
    if (distLsC>_R && distLeC>_R) { //2)both>R, line outside circle
      //2-1)check if cy between lsy and ley
      if (lsy>_center.y || _center.y>ley)
        return false;
    }
    if (distLsC<_R && distLeC<_R) //3)both<R, line inside circle
      return false;
    else {//4)must have intersection, solve for I
      //4-1)solve for I. Note that ther may be 2 solutions for I.
      //    lineL x=c
      double c = lx-_center.x;
      double delta = _R*_R-CF*CF;
      assert (delta>0);
      double sqdelta = vcl_sqrt (delta);

      double y1 = sqdelta + _center.y;
      double y2 = -sqdelta + _center.y;

      //4-2)if lsy<=y1<=ley, y1 is valid
      if (lsy<=y1 && y1<=ley) {
        //test if vCI in (S, E) of BArc or not
        double vCI = _vPointPoint (Point(0, 0), Point(c, sqdelta));
        if (isVectorValid(vCI))
          return true;
      }

      //4-3)if lsy<=y2<=ley, y2 is valid
      if (lsy<=y2 && y2<=ley) {
        double vCI = _vPointPoint (Point(0, 0), Point(c, -sqdelta));
        if (isVectorValid(vCI))
          return true;
      }
    }
  }

  return false;
}

bool BArc_Bucketing::isRectIntersecting (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B)
{
  if (isHLineIntersecting(L, R, T))
    return true;
  if (isHLineIntersecting(L, R, B))
    return true;
  if (isVLineIntersecting(T, B, L))
    return true;
  if (isVLineIntersecting(T, B, R))
    return true;

  return false;
}

//get the boundingBox of this BAtc...
void BArc_Bucketing::getBoundingBox (COORD_TYPE& L, COORD_TYPE& T, COORD_TYPE& R, COORD_TYPE& B)
{
  bool isEin = false; bool isNin = false; bool isWin = false; bool isSin = false;
  int  nNin = 0;
  if (isVectorValid(0))      { isEin = true; nNin++; }
  if (isVectorValid(M_PI_2*3))  { isNin = true; nNin++; } //flip
  if (isVectorValid(M_PI))    { isWin = true; nNin++; }
  if (isVectorValid(M_PI_2))  { isSin = true; nNin++; }

  switch (nNin) {
  case 0: //Just like BLine, get minX, maxX, minY, maxY directly...
    L = vnl_math_min (_start.x, _end.x);
    T = vnl_math_min (_start.y, _end.y);
    R = vnl_math_max (_start.x, _end.x);
    B = vnl_math_max (_start.y, _end.y);
    return;
  break;
  case 1: //contains only one extrema...
    if (isEin) {//R
      L = vnl_math_min (_start.x, _end.x);
      T = vnl_math_min (_start.y, _end.y);
      R = _center.x + _R;
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
    else if (isNin) {//T
      L = vnl_math_min (_start.x, _end.x);
      T = _center.y - _R;
      R = vnl_math_max (_start.x, _end.x);
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
    else if (isWin) {//L
      L = _center.x - _R;
      T = vnl_math_min (_start.y, _end.y);
      R = vnl_math_max (_start.x, _end.x);
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
    else { //isSin //B
      L = vnl_math_min (_start.x, _end.x);
      T = vnl_math_min (_start.y, _end.y);
      R = vnl_math_max (_start.x, _end.x);
      B = _center.y + _R;
      return;
    }
  break;
  case 2: //contains two extrema...
    if (isEin && isNin) { //T&R
      L = vnl_math_min (_start.x, _end.x);
      T = _center.y - _R;
      R = _center.x + _R;
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
    else if (isNin && isWin) { //L&T
      L = _center.x - _R;
      T = _center.y - _R;
      R = vnl_math_max (_start.x, _end.x);
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
    else if (isWin && isSin) { //L&B
      L = _center.x - _R;
      T = vnl_math_min (_start.y, _end.y);
      R = vnl_math_max (_start.x, _end.x);
      B = _center.y + _R;
      return;
    }
    else { //isSin && isEin //R&B
      L = vnl_math_min (_start.x, _end.x);
      T = vnl_math_min (_start.y, _end.y);
      R = _center.x + _R;
      B = _center.y + _R;
      return;
    }
  break;
  case 3: //contains three extrema...
    if (!isEin) { //LTB
      L = _center.x - _R;
      T = _center.y - _R;
      R = vnl_math_max (_start.x, _end.x);
      B = _center.y + _R;
      return;
    }
    else if (!isNin) { //LRB
      L = _center.x - _R;
      T = vnl_math_min (_start.y, _end.y);
      R = _center.x + _R;
      B = _center.y + _R;
      return;
    }
    else if (!isWin) { //TRB
      L = vnl_math_min (_start.x, _end.x);
      T = _center.y - _R;
      R = _center.x + _R;
      B = _center.y + _R;
      return;
    }
    else { //if (!isSin) //LTR
      L = _center.x - _R;
      T = _center.y - _R;
      R = _center.x + _R;
      B = vnl_math_max (_start.y, _end.y);
      return;
    }
  break;
  case 4: //all 4 extrama are included.
      L = _center.x - _R;
      T = _center.y - _R;
      R = _center.x + _R;
      B = _center.y + _R;
      return;
  break;
  }
  assert (0);
}

//Similar to the Line case. 
//Have to deal with the tangent issue for CW BArcs.
void BArc_Bucketing::updateAPVisCon (Point pt)
{
  if (_APVisCon==NULL)
    _APVisCon = new APVisCon;

  //1)For CCW BArcs, the same as BLine case.
  if (_nud==ARC_NUD_CCW) {
    ///assert (CP<_R);
    _APVisCon->_sPoint = _start;
    _APVisCon->_ePoint = _end;
  }
  else { //2)For CW BArcs, we need to consider the tangent issue.
    ///assert (CP>_R);
    bool bLeftTangent, bRightTangent;
    Point LeftTangentPoint, RightTangentPoint;
    bLeftTangent = _getLeftTangentPoint (pt, LeftTangentPoint);
    bRightTangent = _getRightTangentPoint (pt, RightTangentPoint);

    if (bLeftTangent)
      _APVisCon->_sPoint = LeftTangentPoint;
    else
      _APVisCon->_sPoint = _start;

    if (bRightTangent)
      _APVisCon->_ePoint = RightTangentPoint;
    else
      _APVisCon->_ePoint = _end;
  }

  //3)Create the VisCon...
  _APVisCon->_sp._vectorx = pt.x - _APVisCon->_sPoint.x;
  _APVisCon->_sp._vectory = pt.y - _APVisCon->_sPoint.y;
  _APVisCon->_ep._vectorx = pt.x - _APVisCon->_ePoint.x;
  _APVisCon->_ep._vectory = pt.y - _APVisCon->_ePoint.y;
}

//return true if the LeftTangentPoint is valid
//else, retur false
//Put the LeftTangentPoint in argument, too.
bool BArc_Bucketing::_getLeftTangentPoint (Point pt, Point& LeftTangentPoint)
{
  VECTOR_TYPE vCP = _vPointPoint (_center, pt);
  DIST_TYPE CP  = _distPointPoint (_center, pt);
  VECTOR_TYPE vCTl = angle02Pi (vCP + vcl_acos(_R/CP));
  if (isVectorValid(vCTl)) {
    LeftTangentPoint = _vectorPoint (_center, vCTl, _R);
    return true;
  }
  return false;
}

//return true if the RightTangentPoint is valid
//else, retur false
//Put the RightTangentPoint in argument, too.
bool BArc_Bucketing::_getRightTangentPoint (Point pt, Point& RightTangentPoint)
{
  VECTOR_TYPE vCP = _vPointPoint (_center, pt);
  DIST_TYPE CP  = _distPointPoint (_center, pt);
  VECTOR_TYPE vCTr = angle02Pi (vCP - vcl_acos(_R/CP));
  if (isVectorValid(vCTr)) {
    RightTangentPoint = _vectorPoint (_center, vCTr, _R);
    return true;
  }
  return false;
}

//Test for _APVisCon
//Note that the invisible zone is the intersection of the two constraints.
bool BArc_Bucketing::isPointVisibleUnderCon (Point testPt)
{
  if (_APVisCon) {
    //vector: Start->P
    //vector: End->P
    if ( (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-testPt.x) + 
         _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-testPt.y) < 0) &&
        (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-testPt.x) + 
         _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-testPt.y) < 0) )
      return false;
  }

  //If no _APVisCon, it's just valid.
  return true;
}

//Test for isPointValidInADist and _APVisCon
bool BArc_Bucketing::AisPointVisible (Point pt, VECTOR_TYPE& a)
{
  if (!isPointValidInRAngleFuzzy(pt, a))
    return false;

  if (!isPointVisibleUnderCon (pt))
    return false;

  return true;
}

//For testing a line is visible or not,
//we have to go through the visibility constraint and
//test for both end points at the same time.
bool BArc_Bucketing::AisLineVisible (COORD_TYPE sx, COORD_TYPE sy, COORD_TYPE ex, COORD_TYPE ey)
{
  //0)Compute aLT, aRT, aLB, aRB.
  VECTOR_TYPE aS = _vPointPoint (_center, Point (sx, sy));
  VECTOR_TYPE aE = _vPointPoint (_center, Point (ex, ey));

  //1)For all corner points, test if a<start, if so, it's invalid.
  if (isVectorLessThanStart(aS) && isVectorLessThanStart(aE))
    return false;

  //2)For all corner points, test if a>end, if so, it's invalid.
  if (isVectorGreaterThanEnd(aS) && isVectorGreaterThanEnd(aE))
    return false;

  //3)If there is no _APVisCon constraint, it's valid at this step.
  if (_APVisCon == NULL)
    return true;

  //4-1)For all corner points, test if dot with _APVisCon._sp is invalid.
  bool visibSp = true;
  if ( (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-sx) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-sy) < 0) &&
      (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-ex) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-ey) < 0) )
    visibSp = false;

  //4-2)For all corner points, test if dot with _APVisCon._ep is invalid.
  bool visibEp = true;
  if ( (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-sx) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-sy) < 0) &&
      (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-ex) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-ey) < 0) )
    visibEp = false;

  //4-3)the rect is invisible only if invisible to both _sp and _ep.
  if (visibSp==false && visibEp==false)
    return false;

  //check if footPt is valid on line, will do in other place.
  //check if footPt is valid on arc, will do in other place.

  //5)Finally here, we consider the rect to be visible.
  return true;
}

//For testing a bucket is visible or not,
//we have to go through the visibility constraint and
//test for all 4 corner points at the same time.
bool BArc_Bucketing::AisRectVisible (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B)
{
  //0)Compute aLT, aRT, aLB, aRB.
  VECTOR_TYPE aLT = _vPointPoint (_center, Point (L, T));
  VECTOR_TYPE aRT = _vPointPoint (_center, Point (R, T));
  VECTOR_TYPE aLB = _vPointPoint (_center, Point (L, B));
  VECTOR_TYPE aRB = _vPointPoint (_center, Point (R, B));

  //1)For all corner points, test if a<start, if so, it's invalid.
  if (isVectorLessThanStart(aLT) && 
     isVectorLessThanStart(aRT) &&
     isVectorLessThanStart(aLB) &&
     isVectorLessThanStart(aRB))
    return false;

  //2)For all corner points, test if a>end, if so, it's invalid.
  if (isVectorGreaterThanEnd(aLT) &&
     isVectorGreaterThanEnd(aRT) &&
     isVectorGreaterThanEnd(aLB) &&
     isVectorGreaterThanEnd(aRB))
    return false;

  //3)If there is no _APVisCon constraint, it's valid at this step.
  if (_APVisCon == NULL)
    return true;

  //4-1)For all corner points, test if dot with _APVisCon._sp is invalid.
  bool visibSp = true;
  if ( (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-L) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-T) < 0) &&
      (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-R) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-T) < 0) &&      
      (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-L) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-B) < 0) &&
      (_APVisCon->_sp._vectorx * (_APVisCon->_sp._vectorx+_APVisCon->_sPoint.x-R) + 
      _APVisCon->_sp._vectory * (_APVisCon->_sp._vectory+_APVisCon->_sPoint.y-B) < 0) )
    visibSp = false;

  //4-2)For all corner points, test if dot with _APVisCon._ep is invalid.
  bool visibEp = true;
  if ( (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-L) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-T) < 0) &&
      (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-R) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-T) < 0) &&      
      (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-L) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-B) < 0) &&
      (_APVisCon->_ep._vectorx * (_APVisCon->_ep._vectorx+_APVisCon->_ePoint.x-R) + 
      _APVisCon->_ep._vectory * (_APVisCon->_ep._vectory+_APVisCon->_ePoint.y-B) < 0) )
    visibEp = false;

  //4-3)the rect is invisible only if invisible to both _sp and _ep.
  if (visibSp==false && visibEp==false)
    return false;

  //5)Finally here, we consider the rect to be visible.
  return true;
}

void BArc_Bucketing::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

  BArc::getInfo (ostrm);

  vcl_sprintf (s, "BUCKETING:\n"); ostrm<<s;
  if (__BucketArray == NULL)
    vcl_sprintf (s, "Bucket not initialized yet.\n\n");
  else {
    if (_bucAX<0) { //print the list of buckets in the list
      vcl_sprintf (s, "BArc in Buckets: "); ostrm<<s;
      BucketList::iterator bit = _arcInBuckets.begin();
      for (; bit != _arcInBuckets.end(); bit++) {
        vcl_sprintf (s, "[%d][%d], ", (*bit)->index().h, (*bit)->index().v); ostrm<<s;
      }
      vcl_sprintf (s, "\n"); ostrm<<s;
  
      //twinArc:
      vcl_sprintf (s, "twinArc in Buckets: "); ostrm<<s;
      bit = twinArc()->_arcInBuckets.begin();
      for (; bit != twinArc()->_arcInBuckets.end(); bit++) {
        vcl_sprintf (s, "[%d][%d], ", (*bit)->index().h, (*bit)->index().v); ostrm<<s;
      }
      vcl_sprintf (s, "\n"); ostrm<<s;
    }
    else {
      vcl_sprintf (s, "BArc in Bucket[%d][%d], BucketId: %d\n", 
            _bucAX, _bucAY, 
            __BucketArray[_bucAX][_bucAY].id()); ostrm<<s;
      vcl_sprintf (s, "twin BArc in Bucket[%d][%d], BucketId: %d\n", 
            twinArc()->_bucAX, twinArc()->_bucAY, 
            __BucketArray[twinArc()->_bucAX][twinArc()->_bucAY].id()); ostrm<<s;
    }
    vcl_sprintf (s, "s_pt() in Bucket[%d][%d], BucketId: %d\n", 
          s_pt()->_bucX, s_pt()->_bucY, 
          __BucketArray[s_pt()->_bucX][s_pt()->_bucY].id()); ostrm<<s;
    vcl_sprintf (s, "e_pt() in Bucket[%d][%d], BucketId: %d\n\n", 
          e_pt()->_bucX, e_pt()->_bucY, 
          __BucketArray[e_pt()->_bucX][e_pt()->_bucY].id()); ostrm<<s;
  }

  //Arc-Point Visibility Constraint
  if (_APVisCon) {
    vcl_sprintf (s, "2 APVisCon Vectors:\n"); ostrm<<s;
    vcl_sprintf (s, "sPoint: (%f, %f), vStartP: (%f, %f), \n",
          _APVisCon->_sPoint.x, _APVisCon->_sPoint.y, 
          _APVisCon->_sp._vectorx, _APVisCon->_sp._vectory); ostrm<<s;
    vcl_sprintf (s, "ePoint: (%f, %f), vEndP (%f, %f). \n\n",
          _APVisCon->_ePoint.x, _APVisCon->_ePoint.y, 
          _APVisCon->_ep._vectorx, _APVisCon->_ep._vectory); ostrm<<s;
  }
  else {
    vcl_sprintf (s, "No APVisCon Vector.\n\n"); ostrm<<s;
  }
  vcl_sprintf (s, "\n\n"); ostrm<<s;
}
