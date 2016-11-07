#ifndef _BUCKET_H
#define _BUCKET_H

#include "ishock-common.h"
#include "belements.h"
#include "sielements.h"

#include <vcl_vector.h>
#include <vcl_queue.h>
#include <vcl_map.h>
#include <vcl_string.h>

class Bucket;
class Bucketing;

class BPoint_Bucketing;
class BLine_Bucketing;
class BArc_Bucketing;

class ValidSource {
public:
  BElement*  _belm1;
  BElement*  _belm2; 
  Point      _startpt;
  RADIUS_TYPE _time;
  Bucket*    _bucket;    //which bucket this source is in.

  ValidSource (BElement* belm1, BElement* belm2, Point startpt, RADIUS_TYPE time, Bucket* bucket) {
    _belm1 = belm1;
    _belm2 = belm2;
    _startpt = startpt;
    _time = time;
    _bucket = bucket;
  }
};

class ValidContactShock {
public:
  BElement*  _lbelm;
  BElement*  _rbelm;

  ValidContactShock (BElement* lbelm, BElement* rbelm) {
    _lbelm = lbelm;
    _rbelm = rbelm;
  }
};

class BucketIndex
{
public:
  int h;
  int v;
public:
  BucketIndex () { h=-1; v=-1; }
  BucketIndex (int newh, int newv) { h=newh; v=newv; }
};

typedef vcl_set<Bucket* > BucketList;
typedef vcl_queue<Bucket* > BucketQueue;

typedef vcl_vector <ValidSource* > ValidSourceType;
typedef vcl_vector <ValidContactShock* > ValidContactShockType;

#ifndef _BOUNDARY_H
typedef vcl_vector<BElement* > BElmVectorType;
typedef vcl_map<int, BElement* > BElmListType;
typedef vcl_pair<int, BElement*> ID_BElm_pair;
#endif

typedef vcl_set<SISource* > SIElmSetType;

class Bucket : public BaseGUIGeometry
{
protected:
  int  _id;

  int  _left, _top, _right, _bottom;
  BucketIndex _index;

  BucketList  _neighborList;
  BElmVectorType    _BElmList;    //list of all BElements in this Bucket
  SIElmSetType    _SIElmList;    //vcl_set of all SISources in this Bucket
                        //will be update in IShock_Bucketing::InitializeShocks()
public:
  bool  _bNonEmpty;
  Bucketing* __Bucketing;

  unsigned long _bVisited, _bAdded; //For spiral search of assignVisibleBucketsNewForABucket()
  unsigned long _bVisitedValidation; //For spiral search of validation
  bool  _bIsDone;

  int id() {
    return _id;
  }
  void setID (int newid) {
    _id = newid;
  }
  int h (); //return the bucket index h from bucket_id
  int v (); //return the bucket index v from bucket_id

  Bucket () : BaseGUIGeometry()
  {
    _id = 0;
    ///_idcounter = 0;
    _left = 0; _top = 0; _right = 0; _bottom = 0;
    ///_bucketing = NULL; init at Bucketing.
    _BElmList.clear();
    _neighborList.clear();

    _bVisited = 0;
    _bAdded = 0;
    _bVisitedValidation = 0;
    _bIsDone = false;
  }
  /*Bucket (int l, int t, int r, int b, Bucketing* buck) : common()
  {
    Bucket (newid);
    _left = l; _top = t; _right = r; _bottom = b;
    _bucketing = buck;
  }*/
  ~Bucket ()
  {
  }

  int nBElement() { return _BElmList.size(); }
  bool isEmpty() { return _BElmList.size()==0; }
  int nNeighborCells() { return _neighborList.size(); }

  BElmVectorType* BElmList() { return &_BElmList; }
  SIElmSetType* SIElmList() { return &_SIElmList; }
  void addBElement (BElement* belm);
  void delBElement (BElement* belm);

  bool isBElmIn (BElement* belm)
  {
    BElmVectorType::iterator i = vcl_find(_BElmList.begin(), _BElmList.end(), belm);
    if (i != _BElmList.end())
      return true;
    else
      return false;
  }

  int Left() { return _left; }
  int Top() { return _top; }
  int Right() { return _right; }
  int Bottom() { return _bottom; }
  void setLeft (int l) { _left = l; }
  void setTop (int t) { _top = t; }
  void setRight (int r) { _right = r; }
  void setBottom (int b) { _bottom = b; }
  void setRect (int l, int t, int r, int b) {
    _left = l; _top = t; _right = r; _bottom = b;
  }
  Point Lt() { return Point(_left, _top); }
  Point Rt() { return Point(_right, _top); }
  Point Lb() { return Point(_left, _bottom); }
  Point Rb() { return Point(_right, _bottom); }  

  ///bool ifExistUpperNeighbor() {
  ///  return _index.v>0;
  ///}
  ///bool ifExistLowerNeighbor();
  ///Bucket* UpperNeighrbor();
  ///Bucket* LowerNeighrbor();

  ///Bucketing* bucketing() { return _bucketing; }
  ///void setBucketing (Bucketing* buck) { _bucketing = buck; }
  BucketIndex index() { return _index; }
  void setIndex (BucketIndex ind) { _index = ind; }

  BucketList* neighborList() { return &_neighborList; }
  void addNeighboringBuckets (Bucket* bc) {
    _neighborList.insert (bc);
  }
  bool isNeighboringBucket (Bucket* bc);

  virtual void getInfo (vcl_ostream& ostrm);

  //Given a value, test if it is within the Y range of the bucket Top and Bottom
  //Note that the last bucket BucketArray[i][_nBucketsPerSlotMem-1] includes the closed bottom boundary
  bool isValueInBucketY (double value)
  {
    if (_index.v==0) {
      //////if (_index.h==0): unfinished!
      if (value >= _top && value < _bottom)
        return true;
      else
        return false;
    }
    if (_index.v==1) {
      if (value >= _top && value <= _bottom)
        return true;
      else
        return false;
    }
    else { //normal cases
      if (value > _top && value <= _bottom)
        return true;
      else
        return false;
    }
  }
  bool isPointInside (Point pt) {
    if (_index.h==0) {
      if (pt.x >= _left && pt.x < _right)
        if (isValueInBucketY (pt.y))
          return true;
    }
    if (_index.h==1) { //_isFirstHorizBucket
      if (pt.x >= _left && pt.x <= _right)
        if (isValueInBucketY (pt.y))
          return true;
    }
    else {
      if (pt.x > _left && pt.x <= _right)
        if (isValueInBucketY (pt.y))
          return true;
    }
    return false;
  }
  //The fuzzy misvcl_sing case of isPointInside()
  bool isPointInside2 (Point pt) {
    //The index.h==0 and index.v==0 case!!
    if (_index.v!=1) {
      if (pt.y==_top && _left<=pt.x && pt.x<_right)
        return true;
    }
    if (_index.h!=1) {
      if (pt.x==_left && _top<=pt.y && pt.y<_bottom)
        return true;
    }
    return false;
  }

  bool isIntervalContainsBucketY (double v_top, double v_bottom)
  {
    if (_index.v==0) {
      if (v_top < _top && v_bottom > _bottom)
        return true;
      else
        return false;
    }
    else { //normal cases
      if (v_top <= _top && v_bottom > _bottom)
        return true;
      else
        return false;
    }
  }

  bool isRectInside (COORD_TYPE L, COORD_TYPE T, COORD_TYPE R, COORD_TYPE B) {

    if (_index.h==0) {
      if (L >= _left && R < _right) {
        if (_index.v==0) {
          if (T >= _top && B < _bottom)
            return true;
          else
            return false;
        }
        if (_index.v==1) {
          if (T >= _top && B <= _bottom)
            return true;
          else
            return false;
        }
        else { //normal cases
          if (T > _top && B <= _bottom)
            return true;
          else
            return false;
        }
      }
    }
    if (_index.h==1) { //_isFirstHorizBucket
      if (L >= _left && R <= _right) {
        if (_index.v==0) {
          if (T >= _top && B < _bottom)
            return true;
          else
            return false;
        }
        if (_index.v==1) {
          if (T >= _top && B <= _bottom)
            return true;
          else
            return false;
        }
        else { //normal cases
          if (T > _top && B <= _bottom)
            return true;
          else
            return false;
        }
      }
    }
    else {
      if (L > _left && R <= _right) {
        if (_index.v==0) {
          if (T >= _top && B < _bottom)
            return true;
          else
            return false;
        }
        if (_index.v==1) {
          if (T >= _top && B <= _bottom)
            return true;
          else
            return false;
        }
        else { //normal cases
          if (T > _top && B <= _bottom)
            return true;
          else
            return false;
        }
      }
    }
    return false;

  }
};

class Boundary_Bucketing;
class IShock;

class Bucketing
{
protected:
  int  _bucketIdCounter;

  int  _totalBPoints;
  int  _nElementsPerBucket;

  int _nSlotsMem, _nBucketsPerSlotMem;  //mem alloc

  int _nSlots;                //real one
  int *_nBucketsInSlot;

  int _nElementsPerSlot;
  int _nTotalBuckets;
  BElmListType* _boundaryElmList;
  Bucket** _BucketArray;
  IShock*    ishock;

  typedef vcl_pair<Point, BElement*> Pt_BElm_pair;
  typedef vcl_vector<Pt_BElm_pair> HorizElmType;
  typedef vcl_vector<Pt_BElm_pair>::iterator HorizElmTypeIterator;
  HorizElmType horizMap;

  typedef vcl_pair<COORD_TYPE, BElement*> Y_BElm_pair;
  typedef vcl_vector<Y_BElm_pair> VerticalElmType;
  typedef vcl_vector<Y_BElm_pair>::iterator VerticalElmTypeIterator;
  VerticalElmType* vertMap;

  int* hFence;  //array of hFence[]
  int** vFence;  //double array of vFence[][]

  int WORLD_BOUNDING_BOX_LEFT;
  int WORLD_BOUNDING_BOX_TOP;
  int WORLD_BOUNDING_BOX_RIGHT;
  int WORLD_BOUNDING_BOX_BOTTOM;

  BucketIndex _findBucketOfAPoint (Point pt);
  int _findBucketIndexYOfAPoint (Point pt, int i);

public:

  Boundary_Bucketing*  boundary;
  BElmVectorType _BElmToBeDeleted; //for Preprocessing

  int nElmsPerBucket() { return _nElementsPerBucket; }
  int nBucketsPerSlot() { return _nBucketsPerSlotMem; }
  int nSlots() { return _nSlots; }
  int nBucketsInSlot (int i) { return _nBucketsInSlot[i]; }
  Bucket** BucketArray() { return _BucketArray; }
  Bucket*  GetBucket(int h, int v) {
    assert (h>-1);
    assert (v>-1);
    return &_BucketArray[h][v];
  }

  //SearchIDs: To avoid initialization of flag for each spiral search!
  unsigned long _BucketBSearchID;
  //before each spiral search for BucketB, call this function.
  unsigned long getNewBucketBSearchID() {
    _BucketBSearchID++;
    return _BucketBSearchID;
  }
  unsigned long _CircleSearchID;
  //before each spiral search for within-circle-validation, call this function.
  unsigned long getNewCircleSearchID() {
    _CircleSearchID++;
    return _CircleSearchID;
  }

  void allocateStructure (void);
  void freeStructure (void);
  void delLineArcInBucketInfo (void);
  void assignPointsToBuckets (void);
  void assignNeighboringBuckets (void);
    void assignNeighboringBucketsForABucket (Bucket& bucket);
  void assignLinesArcsToBuckets (void);
    void assignLineToBuckets (BLine_Bucketing* bl);
    void assignArcToBuckets (BArc_Bucketing* ba);

  void BucketStatistics (void);
  Bucket* findBucketOfAPoint (Point pt);
  Bucket* findBucketOfAPoint2 (Point pt);

  bool is_B1_P_B2_Visible (Bucket* B1, Point P, Bucket* B2);
  bool is_P1_P2_B_Visible (Point P1, Point P2, Bucket* B);
  bool doesBElmInvalidateSAB (BElement* belm, Point midPoint, double midDistSq, 
                     int GAid, int GBid, bool bIsConnectedGUIElm);

  ValidSourceType _validSourceList;
  void addValidSource (BPoint_Bucketing* bp1, BPoint_Bucketing* bp2, 
                Point startpt, RADIUS_TYPE time, Bucket* bucket);
  void addValidSource (BPoint_Bucketing* bp1, BLine_Bucketing* bl2, 
                Point footPt, Point startpt, RADIUS_TYPE time, Bucket* bucket);
  void addValidSource (BPoint_Bucketing* bp1, BArc_Bucketing* ba2, 
                Point footPt, Point startpt, RADIUS_TYPE time, Bucket* bucket);
  void addValidSource (BLine_Bucketing* bl1, BArc_Bucketing* ba2, 
                Point footPt, Point arcPt, Point startpt, RADIUS_TYPE time, Bucket* bucket);
  void addValidSource (BArc_Bucketing* ba1, BArc_Bucketing* ba2, 
                Point arcPt1, Point arcPt2, Point startpt, RADIUS_TYPE time, Bucket* bucket);

  ValidContactShockType _validContactShockList;
  void addValidContactShock (BElement* lbelm, BElement* rbelm);

  void localShockInit (ValidSourceType& _validSourceList);
    void localShockInitForEachBucket (Bucket* bucket, ValidSourceType& _validSourceList);

  //Counters for Spiral Search
  long _nABIteration, _nListCIteration;

  void initShocksNewElmsBucketing (BElmListType* ListA);
    void assignNewElmsToBuckets (BElmListType* ListA);
    void localShockInitNewElms (BElmListType* ListA, ValidSourceType& _validSourceList);
    void validateExistingSourcesForNewElms (BElmListType* ListA);
      void validateExistingSources_BPoint_Bucket (BPoint_Bucketing* bp1, Bucket* BucketA, Bucket* curBucket,
                                    SIElmSetType& _invalidSourceList);
      void validateExistingSources_BLine_Bucket (BLine_Bucketing* bl1, Bucket* BucketA, Bucket* curBucket,
                                   SIElmSetType& _invalidSourceList);
      void validateExistingSources_BArc_Bucket (BArc_Bucketing* ba1, Bucket* BucketA, Bucket* curBucket,
                                  SIElmSetType& _invalidSourceList);
      void validateExistingSources_BPoint_SpiralSearch (BPoint_Bucketing* bp1, Bucket* BucketA,
                                        SIElmSetType& _invalidSourceList);
      void validateExistingSources_BLine_SpiralSearch (BLine_Bucketing* bl1, Bucket* BucketA,
                                       SIElmSetType& _invalidSourceList);
      void validateExistingSources_BArc_SpiralSearch (BArc_Bucketing* ba1, Bucket* BucketA,
                                      SIElmSetType& _invalidSourceList);
    
  SIElmSetType _invalidSourceList;

  void initShocksBucketing (void);

    void PPShockInit (BPoint_Bucketing* bp1, BPoint_Bucketing* bp2, bool bWithInBucket);
    void PLShockInit (BPoint_Bucketing* bp1, BLine_Bucketing* bl2, Bucket* BucketB, bool bWithInBucket);
    void PAShockInit (BPoint_Bucketing* bp1, BArc_Bucketing* ba2, Bucket* BucketB, bool bWithInBucket);
    void LPShockInit (BLine_Bucketing* bl1, Bucket* BucketA, BPoint_Bucketing* bp2, bool bWithinBucket);
    void LAShockInit (BLine_Bucketing* bl1, Bucket* BucketA, BArc_Bucketing* ba2, Bucket* BucketB, bool bWithinBucket);
    void APShockInit (BArc_Bucketing* ba1, Bucket* BucketA, BPoint_Bucketing* bp2, bool bWithinBucket);
    void ALShockInit (BArc_Bucketing* ba1, Bucket* BucketA, BLine_Bucketing* bl2, Bucket* BucketB, bool bWithinBucket);
    void AAShockInit (BArc_Bucketing* ba1, Bucket* BucketA, BArc_Bucketing* ba2, Bucket* BucketB, bool bWithinBucket);

  void initShocks_BPoint_SpiralSearch (BPoint_Bucketing* bp1, Bucket* BucketA, 
                           ValidSourceType& validSourceList);
    void initShocks_BPoint_Bucket (BPoint_Bucketing* bp1, Bucket* BucketA, Bucket* curBucket, 
                         ValidSourceType& validSourceList, bool bWithinBucket=false);
  void initShocks_BLine_SpiralSearch (BLine_Bucketing* bl1, Bucket* BucketA, 
                          ValidSourceType& validSourceList);
    void initShocks_BLine_Bucket (BLine_Bucketing* bl1, Bucket* BucketA, Bucket* curBucket, 
                        ValidSourceType& validSourceList, bool bWithinBucket=false);
  void initShocks_BArc_SpiralSearch (BArc_Bucketing* ba1, Bucket* BucketA, 
                          ValidSourceType& validSourceList);
    void initShocks_BArc_Bucket (BArc_Bucketing* ba1, Bucket* BucketA, Bucket* curBucket, 
                        ValidSourceType& validSourceList, bool bWithinBucket=false);

  bool PPwithinCircleValidation (BPoint_Bucketing* GPA, BPoint_Bucketing* GPB, 
                       Point midPoint, double midDistSq, Bucket* SABBucket);
    bool doesBLineInvalidatePPSource (BLine_Bucketing* bline, BPoint_Bucketing* GPA, BPoint_Bucketing* GPB, 
                           Point midPoint, double midDistSq);
    bool doesBArcInvalidatePPSource (BArc_Bucketing* barc, BPoint_Bucketing* GPA, BPoint_Bucketing* GPB, 
                           Point midPoint, double midDistSq);
  bool PLwithinCircleValidation (BPoint_Bucketing* GPA, BLine_Bucketing* GLB, 
                       Point footPt, Point midPoint, double midDistSq, Bucket* SABBucket);
    bool doesBLineInvalidatePXSource (BLine_Bucketing* bline, BPoint_Bucketing* GPA, Point footFt, 
                           Point midPoint, double midDistSq);
    bool doesBArcInvalidatePXSource (BArc_Bucketing* barc, BPoint_Bucketing* GPA, Point footFt, 
                           Point midPoint, double midDistSq);
  bool PAwithinCircleValidation (BPoint_Bucketing* GPA, BArc_Bucketing* GAB, 
                       Point footPt, Point midPoint, double midDistSq, Bucket* SABBucket);
  bool LAwithinCircleValidation (BLine_Bucketing* GLA, BArc_Bucketing* GAB, Point footPt, Point arcPt, 
                       Point midPoint, double midDistSq, Bucket* SABBucket);
    bool doesBLineInvalidateAXSource (BLine_Bucketing* bline, Point midPoint, double midDistSq);
    bool doesBArcInvalidateAXSource (BArc_Bucketing* barc, Point midPoint, double midDistSq);
  bool AAwithinCircleValidation (BArc_Bucketing* GAA, BArc_Bucketing* GAB, Point arcPtA, Point arcPtB,
                       Point midPoint, double midDistSq, Bucket* SABBucket);

  Bucketing (BElmListType* belmlist, int numPerBucket,
        Boundary_Bucketing* bnd, IShock* shock,
        int world_bound_left = 0,
        int world_bound_top = 0,        
        int world_bound_right = 1000,
        int world_bound_bottom = 1000);

  ~Bucketing ()
  {
    ////////delLineArcInBucketInfo ();
    freeStructure ();
  }
};



#endif
