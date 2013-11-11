// BUCKET.CPP
//implementation of 2D bucketing for Shock Initilization
#include <extrautils/msgout.h>

//#include <stdlib.h>
#include <time.h>

#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>  // Include algorithms
//using namespace std;

#include "belements-bucketing.h"
#include "boundary.h"
#include "ishock.h"
#include "bucket.h"

//!!Debug
#ifdef _VIS_DEBUG
int  nP_Bucket = 0;
int  nP_nonNeighboringBucket = 0;
int  nL_Bucket = 0;
int  nL_nonNeighboringBucket = 0;
int  nA_Bucket = 0;
int  nA_nonNeighboringBucket = 0;
#endif

//Preprocessing Here!
//Have to set _bIsPreprocesvcl_singNeeded to FALSE after first Bucketing!
void Bucket::addBElement (BElement* belm)
{
  switch (belm->type()) {
  case BPOINT:
    if (__Bucketing->boundary->_bIsPreprocesvcl_singNeeded) {
      //1)If Preprocessing is needed, if input is a BPoint,
      //  check for duplication inside this Bucket.
      BElmVectorType::iterator belmit = _BElmList.begin();
      for (; belmit!=_BElmList.end(); belmit++) {
        if (_BisEqPoint( ((BPoint_Bucketing*)(*belmit))->pt(), ((BPoint_Bucketing*)belm)->pt() )) {
          //merge bp2 into bp1 and delete bp2
          __Bucketing->boundary->mergeDuplicatePoints ((BPoint_Bucketing*)(*belmit), (BPoint_Bucketing*)belm);

          //delete belm from boundary...
          //Problem: how to delete this duplicated belm??
          //Add to _BElmToBeDeleted and then delete later (afte the loop)!
          __Bucketing->_BElmToBeDeleted.push_back (belm);

          return;
        }
      }
    }

    //INITIALIZE THE __BucketArray FOR EACH ELEMENT.
    ((BPoint_Bucketing*)belm)->__BucketArray = __Bucketing->BucketArray();
  break;
  case BLINE:
    //INITIALIZE THE __BucketArray FOR EACH ELEMENT.
    ((BLine_Bucketing*)belm)->__BucketArray = __Bucketing->BucketArray();
  break;
  case BARC:
    //INITIALIZE THE __BucketArray FOR EACH ELEMENT.
    ((BArc_Bucketing*)belm)->__BucketArray = __Bucketing->BucketArray();
  break;
  default: break;
  }

  //Else, just normal adding.
  _BElmList.push_back (belm);
}

void Bucket::delBElement (BElement* belm)
{
  //  _BElmList.erase (belm->id());
}

bool Bucket::isNeighboringBucket (Bucket* bc)
{
  //if anyone in the _neighborList equals bc, then it's true.
  BucketList::iterator it = _neighborList.begin();
  for (; it!=_neighborList.end(); ++it) {
    if ((*it)==bc)
      return true;
  }
  return false;
}

void Bucket::getInfo (vcl_ostream& ostrm)
{
  char s[1024];

   sprintf (s, "==============================\n"); ostrm<<s;
   sprintf (s, "Bucket [%d][%d]: \n", _index.h, _index.v); ostrm<<s;
  sprintf (s, "id: %d\n\n", _id); ostrm<<s;
   sprintf (s, "Bounding box: (%d, %d)-(%d, %d)\n", _left, _top, _right, _bottom); ostrm<<s;

  sprintf (s, "isNonEmpty: %s\n", _bNonEmpty ? "yes" : "no"); ostrm<<s;
   sprintf (s, "# of BElement(s) in this Bucket: %d\n", nBElement()); ostrm<<s;
  sprintf (s, "BElement(s) in this Bucket: "); ostrm<<s;
  BElmVectorType::iterator belmit = _BElmList.begin();
  for (; belmit!=_BElmList.end(); belmit++) {
    sprintf (s, "%d, ", (*belmit)->id()); ostrm<<s;
  }
  sprintf (s, "\n\nSISource(s) in this Bucket: "); ostrm<<s;
  SIElmSetType::iterator sielmit = _SIElmList.begin();
  for (; sielmit!=_SIElmList.end(); sielmit++) {
    sprintf (s, "%d, ", (*sielmit)->id()); ostrm<<s;
  }

  sprintf (s, "\n\nNeighboring Bucket(s) of this Bucket: "); ostrm<<s;
  BucketList::iterator it = _neighborList.begin();
  for (; it!=_neighborList.end(); ++it) {
     sprintf (s, "[%d,%d], ", (*it)->index().h, (*it)->index().v); ostrm<<s;
  }
  sprintf (s, "\n \n"); ostrm<<s;
}

/////////////////////////////////////////////////////////////////

Bucketing::Bucketing (BElmListType* belmlist, int numPerBucket,
               Boundary_Bucketing* bnd, IShock* shock,
               int world_bound_left, int world_bound_top,
               int world_bound_right, int world_bound_bottom)
{
  _bucketIdCounter = 0;

  _boundaryElmList = belmlist;
  _nElementsPerBucket = numPerBucket;
  boundary = bnd;
  ishock = shock;

  _nBucketsInSlot = NULL;
  _BucketArray = NULL;
  vertMap = NULL;

  hFence = NULL;
  vFence = NULL;

  //Initialize the CurrentSearchID for spiral search...
  _BucketBSearchID = 0;
  _CircleSearchID = 0;

  WORLD_BOUNDING_BOX_LEFT = world_bound_left;
  WORLD_BOUNDING_BOX_TOP = world_bound_top;
  WORLD_BOUNDING_BOX_RIGHT = world_bound_right;
  WORLD_BOUNDING_BOX_BOTTOM = world_bound_bottom;

  freeStructure ();
  allocateStructure ();
}

//Given all the internal parameters, allocate the data structure
//plus the last column and row of buckets!!
void Bucketing::allocateStructure (void)
{
  int i;

  //1)Determine horizontal projection parameters.
  //1-1)Insert all BPoints into the horizontal list (sorted).
  BElmListIterator curB = _boundaryElmList->begin();
  for (; curB!=_boundaryElmList->end(); curB++) {
    if (curB->second->type()==BPOINT) {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) curB->second;
      horizMap.push_back (Pt_BElm_pair(bp->pt(), bp));
    }
  }
  vcl_sort (horizMap.begin(), horizMap.end());

  //1-2)Horizontal parameters.
  _totalBPoints = horizMap.size();
  assert (_nElementsPerBucket>0);
  double totalBuckets = (double)_totalBPoints/_nElementsPerBucket;
  double sqC = vcl_sqrt(totalBuckets);
  _nSlotsMem =  (int)vcl_ceil (sqC) + 2;
  _nElementsPerSlot = (int)vcl_ceil ((double)_totalBPoints/sqC);
  _nBucketsPerSlotMem =  (int)vcl_ceil (sqC)+2;
  _nTotalBuckets = _nSlotsMem*_nBucketsPerSlotMem;

  //1-3)Allocate vertical projection parameters.
  _nBucketsInSlot = new int[_nSlotsMem];  //number of buckets in each slot.
  //Create _nSlotsMem numbers of vertical bucket array (array of vertical maps).
  //we are not usingthe vertMap[0] and vertMap[_nSlotsMem-1]
  vertMap = new VerticalElmType[_nSlotsMem];
  hFence = new int[_nSlotsMem+1]; //array of hFence[]

  //2)Determine horizontal projection parameters.
  //2-1)array of _BucketArray[n][n] and vFence[n+1][n+1]
  _BucketArray = new Bucket*[_nSlotsMem];
  for (i=0; i<_nSlotsMem; i++) {
    _BucketArray[i] = new Bucket[_nBucketsPerSlotMem];
    for (int j=0; j<_nBucketsPerSlotMem; j++) //_nBucketsInSlot[i]
      _BucketArray[i][j].__Bucketing = this;
  }

  //4)double array of vFence[][]
  vFence = new int*[_nSlotsMem+1];
  for (i=0; i<=_nSlotsMem; i++)
    vFence[i] = new int[_nBucketsPerSlotMem+1];
}

void Bucketing::freeStructure (void)
{
  int i;

  if (_nBucketsInSlot)
    delete _nBucketsInSlot;
  _nBucketsInSlot = NULL;

  if (_BucketArray) {
    for (i=0; i<_nSlotsMem; i++)
      delete []_BucketArray[i];
    delete []_BucketArray;
  }
  _BucketArray = NULL;

  horizMap.clear();
  if (vertMap)
    delete []vertMap;
  vertMap = NULL;

  if (hFence)
    delete hFence;
  hFence = NULL;

  if (vFence) {
    for (i=0; i<=_nSlotsMem; i++)
      delete []vFence[i];
    delete []vFence;
  }
  vFence = NULL;
}

//After bucketing, we have to delete all the information of BLine_Bucketing and BArc_Bucketing about
//which bucket they are in.
void Bucketing::delLineArcInBucketInfo (void)
{
  //Go through each Bucket and go through each BLine_Bucketing and BArc_Bucketing
  //clear _lineInBuckets for each BLine_Bucketing
  for (int i=0; i<_nSlots; i++) {
    for (int j=0; j<_nBucketsInSlot[i]; j++) { //For each non-empty BucketA...
      if (_BucketArray[i][j]._bNonEmpty) {
        BElmVectorType::iterator it = _BucketArray[i][j].BElmList()->begin();
        for (; it!=_BucketArray[i][j].BElmList()->end(); ++it) {
          if ((*it)->type()==BLINE) {
            ((BLine_Bucketing*)(*it))->_lineInBuckets.clear();
          }
          if ((*it)->type()==BARC) {
            ((BArc_Bucketing*)(*it))->_arcInBuckets.clear();
          }
        }
      }
    }
  }

}

//FOR INPUT OF LINES, WE HAVE TO ASSIGN BUCKETS IN THE FOLLOWING WAY:
//Ignore BLines, and assign buckets only for points and endPoints.
//After all points are assigned to buckets, we assign buckets to each lines, usinginfo from the endPts.

//Note the special case of 0 boundary elements...
void Bucketing::assignPointsToBuckets (void)
{
  int i, j;

  //1)compute each h[i] and determine _nSlots.
  //  Note that _nSlots may be different from _nSlotsMem in extreme cases.
  //1-1)setup the first fence
  hFence[0] = WORLD_BOUNDING_BOX_LEFT;
  //1-2)setup hFence[1] usingfirst element
  BPoint_Bucketing *bp = (BPoint_Bucketing*) (horizMap.begin()->second);
  hFence[1] =  (int)vcl_floor (bp->pt().x);

  //1-3)setup all hFence[i]
  _nSlots = 2;
  int prevHFence = -1;
  int ii = 2;
  HorizElmTypeIterator curHB = horizMap.begin();
  for (i=0; curHB!=horizMap.end(); i++, curHB++) {
    int imod = i%_nElementsPerSlot;
    BPoint_Bucketing* bp = (BPoint_Bucketing*) (curHB->second);
    if (imod == _nElementsPerSlot-1) { //possible Fence
      float ptx = (float)bp->pt().x;
      if (ptx > prevHFence) { //add hFence[] !=
        hFence[ii] =  (int)vcl_ceil (bp->pt().x);
        _nSlots++;
        ii++;
        prevHFence =  (int)vcl_ceil (bp->pt().x);
      }
    }
  }
  //1-4)if there is leftover...create the last _nSlot
  if (i%_nElementsPerSlot) {
    bp = (BPoint_Bucketing*) (horizMap.rbegin()->second);
    float ptx = (float)bp->pt().x;
    if (ptx > prevHFence) { // only if the leftover !!!
      hFence[ii] =  (int)vcl_ceil (bp->pt().x);
      assert (_nSlots < _nSlotsMem);
      _nSlots++;
    }
  }

  //1-5)setup hFence[_nSlots]
  hFence[_nSlots] = WORLD_BOUNDING_BOX_RIGHT;

  //1-6)Go through horizMap again to add all BPoints in each vertMap
  ii=1;
  curHB = horizMap.begin();
  for (i=0; curHB!=horizMap.end(); i++, curHB++) {
    BPoint_Bucketing* bp = (BPoint_Bucketing*) (curHB->second);
    float ptx = (float)bp->pt().x;
    if (ptx>hFence[ii+1])
      ii++; //increase ii to the next slot
    vertMap[ii].push_back (Y_BElm_pair(bp->pt().y, bp));
  }

  for (i=1; i<_nSlots-1; i++)
    vcl_sort(vertMap[i].begin(), vertMap[i].end());


  //2)Compute each v[i][j] and determine _nBPerSlot[i]
  //

  //2-1)setup the first slot
  _nBucketsInSlot[0] = 1;
  vFence[0][0] = WORLD_BOUNDING_BOX_TOP;
  vFence[0][1] = WORLD_BOUNDING_BOX_BOTTOM;

  //2-2)setup the last slot
  _nBucketsInSlot[_nSlots-1] = 1;
  vFence[_nSlots-1][0] = WORLD_BOUNDING_BOX_TOP;
  vFence[_nSlots-1][1] = WORLD_BOUNDING_BOX_BOTTOM;

  //2-3)
  for (ii=1; ii<_nSlots-1; ii++) {

    //recalculate _nElementsPerBucket here
    double _nElementsThisSlot = vertMap[ii].size();
    int nPerBucket = (int)vcl_ceil ((double)_nElementsThisSlot / (_nBucketsPerSlotMem-2));

    //2-3-0)setup the first fence
    vFence[ii][0] = WORLD_BOUNDING_BOX_TOP;
    //2-3-1)setup vFence[ii][1] usingfirst element
    assert (vertMap[ii].size()>0);
    BPoint_Bucketing *bp = (BPoint_Bucketing*) (vertMap[ii].begin()->second);
    vFence[ii][1] =  (int)vcl_floor (bp->pt().y);

    //2-3-2)setup all vFence[i]
    _nBucketsInSlot[ii] = 2;
    int prevVFence = -1;
    int jj = 2;
    VerticalElmTypeIterator curVB = vertMap[ii].begin();
    for (j=0; curVB!=vertMap[ii].end(); j++, curVB++) {
      int jmod = j%nPerBucket;
      BPoint_Bucketing* bp = (BPoint_Bucketing*) (curVB->second);
      if (jmod == nPerBucket-1) { //possible Fence
        float pty = (float)bp->pt().y;
        if (pty > prevVFence) { //add vFence[] !=
          vFence[ii][jj] =  (int)vcl_ceil (bp->pt().y);
          assert (_nBucketsInSlot[ii]<_nBucketsPerSlotMem);
          _nBucketsInSlot[ii]++;
          jj++;
          prevVFence =  (int)vcl_ceil (bp->pt().y);
        }
      }
    }

    //2-3-3)if there is leftover...create the last _nSlot
    if (j%nPerBucket) {
      bp = (BPoint_Bucketing*) (vertMap[ii].rbegin()->second);
      float pty = (float)bp->pt().y;
      if (pty > prevVFence) {
        vFence[ii][jj] =  (int)vcl_ceil (bp->pt().y);
        assert (_nBucketsInSlot[ii]<_nBucketsPerSlotMem);
        _nBucketsInSlot[ii]++;
      }
    }

    //2-3-4)setup vFence[_nSlots]
    vFence[ii][_nBucketsInSlot[ii]] = WORLD_BOUNDING_BOX_BOTTOM;

    //2-3-5)Go through vertMap[ii] again to add all BPoints in each Bucket[ii][jj]
    jj=1;
    curVB = vertMap[ii].begin();
    for (j=0; curVB!=vertMap[ii].end(); j++, curVB++) {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) (curVB->second);
      float pty = (float)bp->pt().y;
      if (pty>vFence[ii][jj+1]) {
        //increase jj to the next slot
        jj++;
        assert (jj<_nBucketsPerSlotMem);
      }
      //add to the current _BucketArray[ii][jj]
      _BucketArray[ii][jj].addBElement (curVB->second);
      bp->_bucX = ii;
      bp->_bucY = jj;
    }

  }

  //2-3-6)PreProcesvcl_sing: delete the duplicate BElms.
  BElmVectorType::iterator it = _BElmToBeDeleted.begin();
  for (; it!=_BElmToBeDeleted.end(); ++it) {
    boundary->BElmList.erase ((*it)->id());
  }


  //for (i=0; i<_nSlots; i++)
  //  for (j=0; j<_nBucketsInSlot[i]; j++) {
  //    assert (i<_nSlots);
  //    assert (j<_nBucketsInSlot[i]);
  //    assert (j<_nBucketsPerSlotMem);
  //  }

  _bucketIdCounter = 0;
  for (i=0; i<_nSlots; i++)
    for (j=0; j<_nBucketsInSlot[i]; j++) {
      _BucketArray[i][j]._bIsDone = false;
      _BucketArray[i][j].setID (_bucketIdCounter);
      _bucketIdCounter++;

      //3-1)Setup bounding box for each bucket
      _BucketArray[i][j].setRect (hFence[i], vFence[i][j], hFence[i+1], vFence[i][j+1]);

      //3-2)Setup index for each bucket
      _BucketArray[i][j].setIndex (BucketIndex (i, j));
    }

  //4)Free unused memory...
  if (vertMap)
    delete []vertMap;
  vertMap = NULL;
}

void Bucketing::assignNeighboringBuckets (void)
{
  //The travervcl_sing sequence is _BucketArray[0~_nSlots],
  //and _BucketArray[i][0~_nBucketsInSlot[i]]
  //For each bucket, push all neighboring Bucket into _neighborList
  //Avoid duplicate elements in the list.
  int i, j;

  for (i=0; i<_nSlots; i++)
    for (j=0; j<_nBucketsInSlot[i]; j++) {
      assignNeighboringBucketsForABucket (_BucketArray[i][j]);
    }
}

//Brute-Force check for neighboring buckets...
void Bucketing::assignNeighboringBucketsForABucket (Bucket& bucket)
{
  int i, j;

  //1)Check the bucket above...
  if (bucket.index().v > 0)
    bucket.addNeighboringBuckets (&_BucketArray[bucket.index().h][bucket.index().v-1]);

  //2)Check the bucket below...
  if (bucket.index().v < _nBucketsInSlot[bucket.index().h]-1)
    bucket.addNeighboringBuckets (&_BucketArray[bucket.index().h][bucket.index().v+1]);

  //3)Check the buckets of left column (if any)
  i = bucket.index().h-1;
  if (i >= 0) {
    for (j=0; j<_nBucketsInSlot[i]; j++) {
      if (bucket.isValueInBucketY (_BucketArray[i][j].Top()) ||
         bucket.isValueInBucketY (_BucketArray[i][j].Bottom()) ||
         bucket.isIntervalContainsBucketY (_BucketArray[i][j].Top(), _BucketArray[i][j].Bottom()))
        bucket.addNeighboringBuckets (&_BucketArray[i][j]);
    }
  }

  //4)Check the buckets of right column (if any)
  i = bucket.index().h+1;
  if (i < _nSlots) {
    for (j=0; j<_nBucketsInSlot[i]; j++) {
      if (bucket.isValueInBucketY (_BucketArray[i][j].Top()) ||
         bucket.isValueInBucketY (_BucketArray[i][j].Bottom()) ||
         bucket.isIntervalContainsBucketY (_BucketArray[i][j].Top(), _BucketArray[i][j].Bottom()))
        bucket.addNeighboringBuckets (&_BucketArray[i][j]);
    }
  }
}

void Bucketing::assignLinesArcsToBuckets (void)
{
  //1)Assign BLines and BArcs to bucket(s)...
  //  Go through each BLine_Bucketing and check it's endPts
  //  Check GUIElement to avoid duplication.
  //  Assign both the GUI and non-GUI one at the same time.
  BElmListIterator curB = _boundaryElmList->begin();
  for (; curB!=_boundaryElmList->end(); curB++) {
    if (curB->second->type()==BLINE && curB->second->isGUIElm())
      assignLineToBuckets ((BLine_Bucketing*) curB->second);

    if (curB->second->type()==BARC && curB->second->isGUIElm())
      assignArcToBuckets ((BArc_Bucketing*) curB->second);
  }

  //2)Set _bNonEmpty for each bucket
  for (int i=0; i<_nSlots; i++)
    for (int j=0; j<_nBucketsInSlot[i]; j++) {
      if (_BucketArray[i][j].isEmpty())
        _BucketArray[i][j]._bNonEmpty = false;
      else
        _BucketArray[i][j]._bNonEmpty = true;
    }
}

//Assign the input bl to intersecting buckets.
void Bucketing::assignLineToBuckets (BLine_Bucketing* bl)
{
  //1)For the two endPts of bl, select left point as S
  //2)Get the start and end buckets.
  Point S, E;
  Bucket *sBucket, *eBucket;
  if (bl->start().x < bl->end().x) {
    S = bl->start();
    E = bl->end();
    sBucket = &_BucketArray[bl->s_pt()->_bucX][bl->s_pt()->_bucY];
    eBucket = &_BucketArray[bl->e_pt()->_bucX][bl->e_pt()->_bucY];
  }
  else {
    S = bl->end();
    E = bl->start();
    sBucket = &_BucketArray[bl->e_pt()->_bucX][bl->e_pt()->_bucY];
    eBucket = &_BucketArray[bl->s_pt()->_bucX][bl->s_pt()->_bucY];
  }

  //3)If BLine_Bucketing inside one bucket.
  if (sBucket==eBucket) {
    //1-1)Line bounded in one bucket...
    bl->_bucLX = bl->s_pt()->_bucX;
    bl->_bucLY = bl->s_pt()->_bucY;
    bl->addInBucket (sBucket);
    bl->twinLine()->_bucLX = bl->s_pt()->_bucX;
    bl->twinLine()->_bucLY = bl->s_pt()->_bucY;
    bl->twinLine()->addInBucket (sBucket);
    return;
  }

  //Line bounded in two or more buckets...
  //Note that searching through neighbors is not suitable here!
  //Even s_pt() and e_pt() are in adjacent bucket,
  //the line could still cross over other buckets,
  //although it's very unlikely to happen in edge input.
  bl->_bucLX = -1;
  bl->_bucLY = -1;
  bl->twinLine()->_bucLX = -1;
  bl->twinLine()->_bucLY = -1;
  float minSyEy = (float) vnl_math_min (S.y, E.y);
  float maxSyEy = (float) vnl_math_max (S.y, E.y);
  int J=-1;

  //4)If BLine_Bucketing inside one _Slot, easier, just do a vertical search.
  if (sBucket->index().h == eBucket->index().h) {

    //4-1)First jump to the starting of comparison.
    //    Search for the first vFence that is possible of intersecting with BLine_Bucketing.
    int i = sBucket->index().h;
    for (int j=1; j<_nBucketsInSlot[i]; j++) {
      if ((vFence[i][j] >= minSyEy)) {
        J = j;
        break;
      }
    }
    #ifdef _VIS_DEBUG
    assert (J>0);
    #endif

    //4-2)A vertical search from vFence[i][V] to the end.
    for (; J<_nBucketsInSlot[i]; J++) {
      Point vs (hFence[i], vFence[i][J]);
      Point ve (hFence[i+1], vFence[i][J]);
      if (_isLineLineIntersecting (S, E, vs, ve)) {
        //4-2-1)If intersecting, add Bucket[i][J-1] to it.
        bl->addInBucket (&_BucketArray[i][J-1]);
        bl->twinLine()->addInBucket (&_BucketArray[i][J-1]);
      }
      else //4-2-2)jump out. We know that intersection has to happen at least once.
        break;
    }

    //4-3)Add the last bucket in.
    //    Note that: if maxSyEy==vFence[i][J-1], don't add the last bucket in.
    if (maxSyEy!=vFence[i][J-1]) {
      bl->addInBucket (&_BucketArray[i][J-1]);
      bl->twinLine()->addInBucket (&_BucketArray[i][J-1]);
    }

    //4-4)If e_pt()'s or s_pt()'s is not in, add into it.
    if (!bl->s_pt()->getBucket()->isBElmIn(bl)) {
      ///!!!DynAdding assert (0);
      bl->addInBucket (bl->s_pt()->getBucket());
      bl->twinLine()->addInBucket (bl->s_pt()->getBucket());
    }
    if (!bl->e_pt()->getBucket()->isBElmIn(bl)) {
      ///!!!DynAdding assert (0);
      bl->addInBucket (bl->e_pt()->getBucket());
      bl->twinLine()->addInBucket (bl->e_pt()->getBucket());
    }
    return;
  }//endif 4)

  //5)For all the other general cases, look for intersection of SE with vertical Fences.

  //5-1)Determine size of the interY[] array.
  int sz = eBucket->index().h - sBucket->index().h;
  assert (sz>0);
  Point* interP = new Point [sz];

  //5-2)Compute each interY
  //    ignore all intersection with horizontal boundary cases.
  double m = (E.y-S.y)/(E.x-S.x);
  int spi = sBucket->index().h; //index on interP[i-spi]

  for (int i=sBucket->index().h; i<eBucket->index().h; i++) {
    int ii = i+1;
    interP[i-spi].x = hFence[ii];
    interP[i-spi].y = m*(hFence[ii]-S.x) + S.y;
  }

  //5-3)For S -> interY[0], Slot[sBucket->index().h]
  int i = sBucket->index().h;
  //determine which bucket interP[0] is in (on the left).
  int indexV = _findBucketIndexYOfAPoint (interP[0], i);

  //Add BucketArray[i][J] to BucketArray[i][sBucket->_index.v] in
  int vstart = vnl_math_min (sBucket->index().v, indexV);
  int vend = vnl_math_max (sBucket->index().v, indexV);

  for (int j=vstart; j<=vend; j++) {
    bl->addInBucket (&_BucketArray[i][j]);
    bl->twinLine()->addInBucket (&_BucketArray[i][j]);
  }

  //5-4)For interY[0] -> interY[sz-1], Slot[i]
  for (i=sBucket->index().h+1; i<eBucket->index().h; i++) {

    //determine which bucket interP[i-spi] is in (on the right).
    vstart = _findBucketIndexYOfAPoint (interP[i-spi-1], i);
    //determine which bucket interP[i-spi] is in (on the right).
    vend = _findBucketIndexYOfAPoint (interP[i-spi], i);

    for (int j=vstart; j<=vend; j++) {
      bl->addInBucket (&_BucketArray[i][j]);
      bl->twinLine()->addInBucket (&_BucketArray[i][j]);
    }

  }

  //5-5)For interY[sz-1] -> E, Slot[eBucket->_index.h]
  i = eBucket->index().h;
  //determine which bucket interP[sz-1] is in (on the right).
  indexV = _findBucketIndexYOfAPoint (interP[sz-1], i);

  //Add BucketArray[i][J] to BucketArray[i][sBucket->_index.v] in
  vstart = vnl_math_min (eBucket->index().v, indexV);
  vend = vnl_math_max (eBucket->index().v, indexV);

  for (int j=vstart; j<=vend; j++) {
    bl->addInBucket (&_BucketArray[i][j]);
    bl->twinLine()->addInBucket (&_BucketArray[i][j]);
  }

  delete []interP;

  //6)If e_pt()'s or s_pt()'s is not in, add into it.
  if (!bl->s_pt()->getBucket()->isBElmIn(bl)) {
    assert (0);
    bl->addInBucket (bl->s_pt()->getBucket());
    bl->twinLine()->addInBucket (bl->s_pt()->getBucket());
  }
  if (!bl->e_pt()->getBucket()->isBElmIn(bl)) {
    assert (0);
    bl->addInBucket (bl->e_pt()->getBucket());
    bl->twinLine()->addInBucket (bl->e_pt()->getBucket());
  }
}

void Bucketing::assignArcToBuckets (BArc_Bucketing* ba)
{
  //1)Detect bounding box of this BArc_Bucketing...
  COORD_TYPE L, T, R, B;
  ba->getBoundingBox (L, T, R, B);

  //2)Start from the bucket that contains _start
  //  Test if ba is completely inside one bucket...
  Bucket* sBucket = findBucketOfAPoint (ba->start());

  //2-1)ba bounded in one bucket...
  //    Note that e_pt() of this ba can still goes into other buckets!!
  if ( sBucket->isRectInside (L, T, R, B) ) {
    ba->_bucAX = ba->s_pt()->_bucX;
    ba->_bucAY = ba->s_pt()->_bucY;
    ba->addInBucket (sBucket);
    ba->twinArc()->_bucAX = ba->s_pt()->_bucX;
    ba->twinArc()->_bucAY = ba->s_pt()->_bucY;
    ba->twinArc()->addInBucket (sBucket);
    return;
  }
  else { //2-2) assign sBucket and do a spiral search
    ba->addInBucket (sBucket);
    ba->twinArc()->addInBucket (sBucket);
  }

  //3)A spiral search to detect intersection of two bounding recvcl_tangles.
  //  Use _bVisitedValidation, not conflict with the other usage in withinCircleValidation.
  getNewCircleSearchID();

  BucketQueue bucketQueue;
  sBucket->_bVisitedValidation = _CircleSearchID; //sBucket itself is visited now.

  BucketList::iterator snit = sBucket->neighborList()->begin();
  for (; snit!=sBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.

    //test if ba intersects with recvcl_tangle or not.
    //if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
    //                    (*snit)->Right(), (*snit)->Bottom()) ) {
    if (ba->isRectIntersecting (curBucket->Left(), curBucket->Top(),
                       curBucket->Right(), curBucket->Bottom()) ) {
      //3-1)assign curBucket for ba
      ba->addInBucket (curBucket);
      ba->twinArc()->addInBucket (curBucket);

      //3-2)If valid, has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID)
          bucketQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //4)Test on the validationQueue in a spiral way... run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!bucketQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = bucketQueue.front();
    bucketQueue.pop();

    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if ba intersects with recvcl_tangle or not.
    //if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
    //                    curBucket->Right(), curBucket->Bottom()) ) {
    if (ba->isRectIntersecting (curBucket->Left(), curBucket->Top(),
                       curBucket->Right(), curBucket->Bottom()) ) {
      //4-2)assign curBucket for ba
      ba->addInBucket (curBucket);
      ba->twinArc()->addInBucket (curBucket);

      //4-3)If valid, has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID)
          bucketQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while bucketQueue

  //5)If e_pt()'s or s_pt()'s is not in, add into it.
  if (!ba->s_pt()->getBucket()->isBElmIn(ba)) {
    ba->addInBucket (ba->s_pt()->getBucket());
    ba->twinArc()->addInBucket (ba->s_pt()->getBucket());
  }
  if (!ba->e_pt()->getBucket()->isBElmIn(ba)) {
    ba->addInBucket (ba->e_pt()->getBucket());
    ba->twinArc()->addInBucket (ba->e_pt()->getBucket());
  }
}

void Bucketing::BucketStatistics (void)
{
  int nTotalNumBuckets = 0;
  int totalNonEmptyBuckets = 0;
  int total_NeighboringBuckets = 0;

  for (int i=0; i<_nSlots; i++)
    for (int j=0; j<_nBucketsInSlot[i]; j++) {
      nTotalNumBuckets++;
      if (_BucketArray[i][j]._bNonEmpty) {
        totalNonEmptyBuckets ++;
        total_NeighboringBuckets += _BucketArray[i][j].neighborList()->size();
      }
    }

  int nTotalEmptyBuckets = nTotalNumBuckets-totalNonEmptyBuckets;
  double dAverage_NeighboringBuckets = (double)total_NeighboringBuckets/totalNonEmptyBuckets;

  vcl_cout<< "Total number of Buckets= " << nTotalNumBuckets <<vcl_endl;
  vcl_cout<< "Total number of Empty Buckets= " << nTotalEmptyBuckets <<vcl_endl;
  vcl_cout<< "Average number of Neighboring Buckets per Bucket= " << dAverage_NeighboringBuckets <<vcl_endl;
}

//protected function, If pt not inside any bucket, just return;
BucketIndex Bucketing::_findBucketOfAPoint (Point pt)
{
  int i, j;
  BucketIndex index;
  float ptx = (float) pt.x;
  float pty = (float) pt.y;

  //1)First search for hFence[h]
  //  Consider the arc case, have to search in empty buckets!!!
  for (i=0; i<_nSlots; i++) { //_nSlots-1
    if (i==0) {
      if (hFence[i] <= ptx && ptx < hFence[i+1]) {
        index.h = i;
        break;
      }
    }
    else if (i==1) {
      if (hFence[i] <= ptx && ptx <= hFence[i+1]) {
        index.h = i;
        break;
      }
    }
    else {
      if (hFence[i] < ptx && ptx <= hFence[i+1]) {
        index.h = i;
        break;
      }
    }
  }
  //assert (index.h != -1);
  if (index.h == -1)
    return index;

  //2)Within hFence[h], search for vFence[v]
  i = index.h;
  for (j=0; j<_nBucketsInSlot[i]; j++) {
    if (j==0) {
      if (vFence[i][j] <= pty && pty < vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
    else if (j==1) {
      if (vFence[i][j] <= pty && pty <= vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
    else {
      if (vFence[i][j] < pty && pty <= vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
  }
  //assert (index.v != -1);
  return index;
}

//return which bucket pt is in, via search in _Slot[i] only
int Bucketing::_findBucketIndexYOfAPoint (Point pt, int i)
{
  float pty = (float) pt.y;
  int j, v=-1;
  for (j=0; j<_nBucketsInSlot[i]; j++) {
    if (j==0) {
      if (vFence[i][j] <= pty && pty < vFence[i][j+1]) {
        v = j;
        break;
      }
    }
    else if (j==1) {
      if (vFence[i][j] <= pty && pty <= vFence[i][j+1]) {
        v = j;
        break;
      }
    }
    else {
      if (vFence[i][j] < pty && pty <= vFence[i][j+1]) {
        v = j;
        break;
      }
    }
  }
  assert (v != -1);
  return v;
}

//A public function, Either return the Bucket or NULL
Bucket* Bucketing::findBucketOfAPoint (Point pt)
{
  BucketIndex idx = _findBucketOfAPoint (pt);
  if (idx.h==-1 || idx.v==-1)
    return NULL;

  #ifdef _VIS_DEBUG
  assert (_BucketArray);
  #endif
  return &_BucketArray[idx.h][idx.v];
}

//Similar to findBucketOfAPoint(), but return the closest bucket if the point is ouside the limit.
Bucket* Bucketing::findBucketOfAPoint2 (Point pt)
{
  int i, j;
  BucketIndex index;
  float ptx = (float) pt.x;
  float pty = (float) pt.y;

  //1)First search for hFence[h]
  //  Consider the arc case, have to search in empty buckets!!!
  for (i=0; i<_nSlots; i++) { //_nSlots-1
    if (i==0) {
      if (hFence[i] <= ptx && ptx < hFence[i+1]) {
        index.h = i;
        break;
      }
    }
    else if (i==1) {
      if (hFence[i] <= ptx && ptx <= hFence[i+1]) {
        index.h = i;
        break;
      }
    }
    else {
      if (hFence[i] < ptx && ptx <= hFence[i+1]) {
        index.h = i;
        break;
      }
    }
  }

  //1-2)
  if (index.h==-1) {
    if (pt.x < WORLD_BOUNDING_BOX_LEFT)
      index.h = 0;
    else
      index.h = _nSlots-1;
    return &_BucketArray[index.h][0];
  }

  //2)Within hFence[h], search for vFence[v]
  i = index.h;
  for (j=0; j<_nBucketsInSlot[i]; j++) {
    if (j==0) {
      if (vFence[i][j] <= pty && pty < vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
    else if (j==1) {
      if (vFence[i][j] <= pty && pty <= vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
    else {
      if (vFence[i][j] < pty && pty <= vFence[i][j+1]) {
        index.v = j;
        break;
      }
    }
  }

  //2-2)
  if (index.v==-1) {
    if (pt.y < WORLD_BOUNDING_BOX_TOP)
      index.v = 0;
    else
      index.v = _nBucketsInSlot[i]-1;
  }

  //assert (_BucketArray);
  //assert (index.h!=-1);
  //assert (index.v!=-1);
  return &_BucketArray[index.h][index.v];
}

//return true if visible
//return false if invisible
bool Bucketing::is_B1_P_B2_Visible (Bucket* B1, Point P, Bucket* B2)
{
  //1)Given point P, compute the 4 invalidVectors:
  Vector PLt = Vector (P, B1->Lt());
  Vector PRt = Vector (P, B1->Rt());
  Vector PLb = Vector (P, B1->Lb());
  Vector PRb = Vector (P, B1->Rb());

  //2)First check if the point is already computed.
  //  If no, compute the 4 invalidVectors and set the invisibility.
  //!!Note the '=' condition!! Here we treat '=' as possible visible.
  //Early jump-out here!!
  //If one of the test if FALSE, return FALSE as the result.
  //In other words, if one of the 4 corners says visible,
  //the two buckets are considered to be visible to each other.

  //2-1)LT, upperNieghbor's LB
  Vector PLtTest = Vector (P, B2->Lt());
  if (_dot(PLt, PLtTest) < 0 && _dot(PRt, PLtTest) < 0 &&
     _dot(PLb, PLtTest) < 0 && _dot(PRb, PLtTest) < 0)
    ;//bLT = true;
  else
    return true;

  //2-2)RT, upperNieghbor's RB
  Vector PRtTest = Vector (P, B2->Rt());
  if (_dot(PLt, PRtTest) < 0 && _dot(PRt, PRtTest) < 0 &&
     _dot(PLb, PRtTest) < 0 && _dot(PRb, PRtTest) < 0)
    ;//bRT = true;
  else
    return true;

  //2-3)LB, lowerNieghbor's LT
  Vector PLbTest = Vector (P, B2->Lb());
  if (_dot(PLt, PLbTest) < 0 && _dot(PRt, PLbTest) < 0 &&
     _dot(PLb, PLbTest) < 0 && _dot(PRb, PLbTest) < 0)
    ;//bLB = true;
  else
    return true;

  //2-4)RB, lowerNieghbor's RT
  Vector PRbTest = Vector (P, B2->Rb());
  if (_dot(PLt, PRbTest) < 0 && _dot(PRt, PRbTest) < 0 &&
     _dot(PLb, PRbTest) < 0 && _dot(PRb, PRbTest) < 0)
    ;//bRB = true;
  else
    return true;

  //2-5)Return invisible as the result
  return false;
}

//return true if visible
//return false if invisible
bool Bucketing::is_P1_P2_B_Visible (Point P1, Point P2, Bucket* B)
{
  //1)Given point P, compute the 4 invalidVectors:
  //2)Test the 4 dot products.
  //!!Note the '=' condition!! Here we treat '=' as visible.
  Vector PTest = Vector (P2, P1);
  Vector TestLt = Vector(P2, B->Lt());
  Vector TestRt = Vector(P2, B->Rt());
  Vector TestLb = Vector(P2, B->Lb());
  Vector TestRb = Vector(P2, B->Rb());
  if (_dot(TestLt, PTest) < 0 && _dot(TestRt, PTest) < 0 &&
      _dot(TestLb, PTest) < 0 && _dot(TestRb, PTest) < 0)
    return false;
  else
    return true;
}

//####################################################################

void Bucketing::addValidSource (BPoint_Bucketing* bp1, BPoint_Bucketing* bp2,
                      Point startpt, DIST_TYPE time, Bucket* bucket)
{
  ValidSource* newsrc = new ValidSource (bp1, bp2, startpt, time, bucket);
  _validSourceList.push_back (newsrc);

  //Add Visibility Constraints.
  bp1->addPPVisCon (bp2->pt().x - bp1->pt().x, bp2->pt().y - bp1->pt().y);
  bp2->addPPVisCon (bp1->pt().x - bp2->pt().x, bp1->pt().y - bp2->pt().y);
}

void Bucketing::addValidSource (BPoint_Bucketing* bp1, BLine_Bucketing* bl2,
                      Point footPt, Point startpt, DIST_TYPE time, Bucket* bucket)
{
  ValidSource* newsrc = new ValidSource (bp1, bl2, startpt, time, bucket);
  _validSourceList.push_back (newsrc);

  //Add Visibility Constraints.
  bp1->addPLVisCon (footPt.x - bp1->pt().x, footPt.y - bp1->pt().y);
  bl2->updateLPVisCon (bp1->pt());
  //Here we assume each line only forms one L-P source.
}

void Bucketing::addValidSource (BPoint_Bucketing* bp1, BArc_Bucketing* ba2,
                      Point footPt, Point startpt, DIST_TYPE time, Bucket* bucket)
{
  ValidSource* newsrc = new ValidSource (bp1, ba2, startpt, time, bucket);
  _validSourceList.push_back (newsrc);

  //Add Visibility Constraints.
  bp1->addPAVisCon (footPt.x - bp1->pt().x, footPt.y - bp1->pt().y);
  ba2->updateAPVisCon (bp1->pt());
  //Here we assume each arc only forms one A-P source.
}

void Bucketing::addValidSource (BLine_Bucketing* bl1, BArc_Bucketing* ba2,
                      Point footPt, Point arcPt, Point startpt, DIST_TYPE time, Bucket* bucket)
{
  ValidSource* newsrc = new ValidSource (bl1, ba2, startpt, time, bucket);
  _validSourceList.push_back (newsrc);

  //Add Visibility Constraints.
  bl1->updateLPVisCon (arcPt);
  ba2->updateAPVisCon (footPt);
  //Here we assume each arc only forms one L-A source.
}

void Bucketing::addValidSource (BArc_Bucketing* ba1, BArc_Bucketing* ba2,
                      Point arcPt1, Point arcPt2, Point startpt, DIST_TYPE time, Bucket* bucket)
{
  ValidSource* newsrc = new ValidSource (ba1, ba2, startpt, time, bucket);
  _validSourceList.push_back (newsrc);

  //Add Visibility Constraints.
  ba1->updateAPVisCon (arcPt2);
  ba2->updateAPVisCon (arcPt1);
  //Here we assume each arc only forms one L-A source.
}

//No need to add visibility constraint, because it's already known.
void Bucketing::addValidContactShock (BElement* lbelm, BElement* rbelm)
{
  ValidContactShock* newcontact = new ValidContactShock (lbelm, rbelm);
  _validContactShockList.push_back (newcontact);
}

void Bucketing::localShockInit (ValidSourceType& _validSourceList)
{
  for (int i=0; i<_nSlots; i++)
    for (int j=0; j<_nBucketsInSlot[i]; j++) {
      //For nonempty buckets, do a local shock detection inside each bucket...
      if (_BucketArray[i][j]._bNonEmpty)
        localShockInitForEachBucket (&_BucketArray[i][j], _validSourceList);
    }
}

//To build initial visitility constraint for each points.
void Bucketing::localShockInitForEachBucket (Bucket* bucket, ValidSourceType& _validSourceList)
{
  //Initialize Sources within this Bucket
  //1)Setup array of BElements for loop speedup.
  BElement** BElmArray = new  BElement*[bucket->BElmList()->size()];
  BElmVectorType::iterator bit = bucket->BElmList()->begin();
  for (int i=0; bit != bucket->BElmList()->end(); bit++, i++)
    BElmArray[i] = (*bit);

  //2)Setup ArrayA, ArrayB, ArrayC for loop speedup.
  int sizeBElmArray = bucket->BElmList()->size();

  //3)C(N,2), Compute midpoints for candidate SOs
  int a, b;
   for (a=sizeBElmArray-1; a>=0; a--) {
    switch (BElmArray[a]->type()) {
    case BPOINT:
      //Avoid duplicate sources: done by index a and b!
      for (b=a-1; b>=0; b--) {
        switch (BElmArray[b]->type()) {
        case BPOINT:
          PPShockInit ((BPoint_Bucketing*)BElmArray[a], (BPoint_Bucketing*)BElmArray[b], true);
        break;
        case BLINE:
          PLShockInit ((BPoint_Bucketing*)BElmArray[a], (BLine_Bucketing*)BElmArray[b], bucket, true);
        break;
        case BARC:
          PAShockInit ((BPoint_Bucketing*)BElmArray[a], (BArc_Bucketing*)BElmArray[b], bucket, true);
        break;
        default: break;
        }
      }
    break;

    case BLINE:
      //Avoid duplicate sources: done by index a and b!
      for (b=a-1; b>=0; b--) {
        switch (BElmArray[b]->type()) {
        case BPOINT:
          LPShockInit ((BLine_Bucketing*)BElmArray[a], bucket, (BPoint_Bucketing*)BElmArray[b], true);
        break;
        case BLINE:
          //IGNORE THE LINE-LINE CASE.
          //!!!!!Should add VisCon for this L-L
        break;
        case BARC:
          LAShockInit ((BLine_Bucketing*)BElmArray[a], bucket, (BArc_Bucketing*)BElmArray[b], bucket, true);
        break;
        default: break;
        }
      }
    break;

    case BARC:
      for (b=a-1; b>=0; b--) {
        switch (BElmArray[b]->type()) {
        case BPOINT:
          APShockInit ((BArc_Bucketing*)BElmArray[a], bucket, (BPoint_Bucketing*)BElmArray[b], true);
        break;
        case BLINE:
          ALShockInit ((BArc_Bucketing*)BElmArray[a], bucket, (BLine_Bucketing*)BElmArray[b], bucket, true);
        break;
        case BARC:
          AAShockInit ((BArc_Bucketing*)BElmArray[a], bucket, (BArc_Bucketing*)BElmArray[b], bucket, true);
        break;
        default: break;
        }
      }
    break;
    default: break;
    }//end switch(a)
  }//end for a

  //4)release the memory
  delete []BElmArray;
}

//Function to form a possible source for Point-Point.
//First test visibility constraints of the two points.
//If invalid, try to early jump out.
//If valid, try to form sources and add visibility constraints.
void Bucketing::PPShockInit (BPoint_Bucketing* bp1,
                    BPoint_Bucketing* bp2, bool bWithinBucket)
{
  //assert (! _BisEqPoint(bp1->pt(), bp2->pt()) );
  if (bp2->PisPointVisible (bp1->pt())) {
    if (bp1->PisPointVisible (bp2->pt())) {
      //1)Both visible to each other, compute midPoint
      Point  midPoint = _midPointPoint (bp1->pt(), bp2->pt());
      double midDistSq = _distSqPointPoint (bp1->pt(), bp2->pt())/4;

      //1-2)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = PPwithinCircleValidation (bp1, bp2, midPoint, midDistSq, SABBucket);
      //1-3)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bp1, bp2, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      //2)For within bucket case,
      //  If bp1 visible to bp2, but bp2 not visible to bp1...
      //  add bp1 into bp2's Visibility Constraint
      //  For between bucket case, simply ignore, because we never need this VisibConstraint.
      //  (We never consider bp1 later on).
      if (bWithinBucket) {
        bp2->addPPVisCon (bp1->pt().x - bp2->pt().x, bp1->pt().y - bp2->pt().y);
      }
    }
  }
  else {
    //3)For bp2 visible to bp1, but bp1 not visible to bp2...
    //   add bp2 into bp1's Visibility Constraint
    if (bp1->PisPointVisible (bp2->pt()))
      bp1->addPPVisCon (bp2->pt().x - bp1->pt().x, bp2->pt().y - bp1->pt().y);
  }
}

//Function to form a possible source for Point-Line.
//Also form a contact shock if it's the case.
//1)First use the line n constraint
//2)Then use the line plane constraint
//3)Then use the point Visibility Constraint.

//PLShockInit(): if the footPt is inside BucketB, early-jump out!
//to avoid duplication with the one formed in InitShockWithinBucket().
void Bucketing::PLShockInit (BPoint_Bucketing* bp1, BLine_Bucketing* bl2,
                    Bucket* BucketB, bool bWithinBucket)
{
  double t;
  if (bl2->LisPointVisible (bp1->pt(), t)) {
    if (bp1->PisLineVisible (bl2->start().x, bl2->start().y, bl2->end().x, bl2->end().y)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute t again.
      Point footPt = _getFootPt (bl2->start(), bl2->end(), t);

      //1-1)If footPt is not inside BucketB, early jump-out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketB)
        return;

      //double midDistSq = _distSqPointPoint (bp1->pt(), footPt)/4;
      double midDistSq = _distSqPointLine (bp1->pt(), bl2->start(), bl2->end())/4;
      Point midPoint = _midPointPoint (bp1->pt(), footPt);

      //1-2)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = PLwithinCircleValidation (bp1, bl2, footPt, midPoint, midDistSq, SABBucket);

      //1-3)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bp1, bl2, footPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      if (bWithinBucket) {
        //2)add bp1 to bl2's VC
        bl2->updateLPVisCon (bp1->pt());
      }
    }
  }
  else {
    //For bp1 is not visible to bl2, bl2 is visible to bp1
    if (bp1->PisLineVisible (bl2->start().x, bl2->start().y, bl2->end().x, bl2->end().y)) {
      if (bWithinBucket && bp1->id()>bl2->id()) { //Check bp1->id()>bl2->id() for DynAdding
        //3)Test if bp1 is one of the e_pt() of bl2.
        if (bl2->s_pt() == bp1) {
          addValidContactShock (bp1, bl2);
          return;
        }
        else if (bl2->e_pt() == bp1) {
          addValidContactShock (bl2, bp1);
          return;
        }
      }

      //4)add bl2 to bp1's VC
      //  Problematic!! this will add twinLine() into VC that duplicates the line itself.
      //  We have to check if bp2 also visible to bl1 and then add the constraint.
      //////////bp1->addPLVisCon (bl2, t);
    }
  }
}

//Copy the rule for VisCon from Point-Line.
void Bucketing::PAShockInit (BPoint_Bucketing* bp1, BArc_Bucketing* ba2,
                    Bucket* BucketB, bool bWithinBucket)
{
  double a;
  if (ba2->AisPointVisible (bp1->pt(), a)) {
    if (bp1->PisBArcVisible (ba2)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute a again.
      Point footPt = _vectorPoint (ba2->center(), a, ba2->R());

      //1-1)If footPt is not inside BucketB, early jump-out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketB)
        return;

      double midDistSq = _distSqPointPoint (bp1->pt(), footPt)/4;
      Point midPoint = _midPointPoint (bp1->pt(), footPt);

      //1-2)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = PAwithinCircleValidation (bp1, ba2, footPt, midPoint, midDistSq, SABBucket);

      //1-3)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bp1, ba2, footPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      if (bWithinBucket) {
        //2)add bp1 to bl2's VC
        ba2->updateAPVisCon (bp1->pt());
      }
    }
  }
  else {
    //For bp1 is not visible to bl2, bl2 is visible to bp1
    if (bp1->PisBArcVisible (ba2)) {
      if (bWithinBucket && bp1->id()>ba2->id()) { //Check bp1->id()>ba2->id() for DynAdding
        //3)Test if bp1 is one of the e_pt() of bl2.
        if (ba2->s_pt() == bp1) {
          addValidContactShock (bp1, ba2);
          return;
        }
        else if (ba2->e_pt() == bp1) {
          addValidContactShock (ba2, bp1);
          return;
        }
      }

      //4)add bl2 to bp1's VC
      //  Problematic!! this will add twinLine() into VC that duplicates the line itself.
      //  We have to check if bp2 also visible to bl1 and then add the constraint.
      //////////bp1->addPLVisCon (bl2, t);
    }
  }
}

//Note that LPShockInit() and PLShockInit() is different!
//1)The visibilityConstraint update is different.
//2)For LPShockInit(), first check if the footPt is inside BucketA or not,
//  if footPt not inside BucketA, can early jump out.
void Bucketing::LPShockInit (BLine_Bucketing* bl1, Bucket* BucketA,
                    BPoint_Bucketing* bp2, bool bWithinBucket)
{
  double t;
  if (bl1->LisPointVisible (bp2->pt(), t)) {
    if (bp2->PisLineVisible (bl1->start().x, bl1->start().y, bl1->end().x, bl1->end().y)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute t again.
      Point footPt = _getFootPt (bl1->start(), bl1->end(), t);

      //1-1)If footPt is not inside BucketA, early jump out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketA)
        return;

      //double midDistSq = _distSqPointPoint (bp2->pt(), footPt)/4;
      double midDistSq = _distSqPointLine (bp2->pt(), bl1->start(), bl1->end())/4;
      Point midPoint = _midPointPoint (bp2->pt(), footPt);

      //1-2)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = PLwithinCircleValidation (bp2, bl1, footPt, midPoint, midDistSq, SABBucket);

      //1-3)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bp2, bl1, footPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      //2)For bp2 is not visible to bl1.
      //add bp2 to bl1's VC
      bl1->updateLPVisCon (bp2->pt());
    }
  }
  else {
    if (bp2->PisLineVisible (bl1->start().x, bl1->start().y, bl1->end().x, bl1->end().y)) {
      if (bWithinBucket) {
        //3)Test if bp1 is one of the e_pt() of bl2.
        if (bl1->s_pt() == bp2) {
          addValidContactShock (bp2, bl1);
          return;
        }
        else if (bl1->e_pt() == bp2) {
          addValidContactShock (bl1, bp2);
          return;
        }
      }

      //4)add bl1 to bp2's VC
      //  Problematic!! this will add twinLine() into VC that duplicates the line itself.
      //  We have to check if bp2 also visible to bl1 and then add the constraint.
      ///////bp2->addPLVisCon (bl1, t);
    }
  }
}

void Bucketing::APShockInit (BArc_Bucketing* ba1, Bucket* BucketA,
                    BPoint_Bucketing* bp2, bool bWithinBucket)
{
  double a;
  if (ba1->AisPointVisible (bp2->pt(), a)) {
    if (bp2->PisBArcVisible (ba1)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute a again.
      Point footPt = _vectorPoint (ba1->center(), a, ba1->R());

      //1-1)If footPt is not inside BucketA, early jump out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketA)
        return;

      double midDistSq = _distSqPointPoint (bp2->pt(), footPt)/4;
      Point midPoint = _midPointPoint (bp2->pt(), footPt);

      //1-2)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = PAwithinCircleValidation (bp2, ba1, footPt, midPoint, midDistSq, SABBucket);

      //1-3)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bp2, ba1, footPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      //2)For bp2 is not visible to ba1.
      //add bp2 to bl1's VC
      ba1->updateAPVisCon (bp2->pt());
    }
  }
  else {
    if (bp2->PisBArcVisible (ba1)) {
      if (bWithinBucket) {
        //3)Test if bp1 is one of the e_pt() of bl2.
        if (ba1->s_pt() == bp2) {
          addValidContactShock (bp2, ba1);
          return;
        }
        else if (ba1->e_pt() == bp2) {
          addValidContactShock (ba1, bp2);
          return;
        }
      }

      //4)add bl1 to bp2's VC
      //  Problematic!! this will add twinLine() into VC that duplicates the line itself.
      //  We have to check if bp2 also visible to bl1 and then add the constraint.
      ///////bp2->addPLVisCon (bl1, t);
    }
  }
}

void Bucketing::LAShockInit (BLine_Bucketing* bl1, Bucket* BucketA,
                    BArc_Bucketing* ba2, Bucket* BucketB, bool bWithinBucket)
{
  double t;
  //Note that BLine_Bucketing and CCW Arc never form a valid source.
  //The e_pt() of the CCW Arc will add VisCon to bl1.
  if (ba2->nud()==ARC_NUD_CCW)
    return;

  if (bl1->LisArcVisible (ba2, t)) {
    if (ba2->AisLineVisible (bl1->start().x, bl1->start().y, bl1->end().x, bl1->end().y)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute t again.
      Point footPt = _getFootPt (bl1->start(), bl1->end(), t);

      //1-1)If footPt is not inside BucketA, early jump out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketA)
        return;

      //1-2)Check if footPt is valid in ba2
      double a;
      if (!ba2->isPointValidInRAngleFuzzy (footPt, a))
        return;

      //1-2)If arcPt is not inside BucketB, early jump out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      Point arcPt = _vectorPoint (ba2->center(), a, ba2->R());
      if (findBucketOfAPoint(arcPt) != BucketB)
        return;

      //double midDistSq = _distSqPointPoint (arcPt, footPt)/4;
      double midDistSq = _distSqPointLine (arcPt, bl1->start(), bl1->end())/4;
      Point midPoint = _midPointPoint (arcPt, footPt);

      //1-3)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = LAwithinCircleValidation (bl1, ba2, footPt, arcPt, midPoint, midDistSq, SABBucket);

      //1-4)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bl1, ba2, footPt, arcPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      //2)arc is visible line, but line is not visible to arc.
      //  Don't update VisCon. Let the e_pt() do it.
      ///add ba2 to bl1's VC
      ///double a = _vPointPoint (ba2->center(), footPt);
      ///Point arcPt = _vectorPoint (ba2->center(), a, ba2->R());
      ///bl1->updateLPVisCon (arcPt);
    }
  }
  else {
    ///if (ba2->AisLineVisible (bl1->start().x, bl1->start().y, bl1->end().x, bl1->end().y)) {
      //3)add bl1 to ba2's VC
      /////ba2->updateALVisCon (bl1, t);
    ///}
  }
}

void Bucketing::ALShockInit (BArc_Bucketing* ba1, Bucket* BucketA,
                    BLine_Bucketing* bl2, Bucket* BucketB, bool bWithinBucket)
{
  //Note that CCW Arc and BLine_Bucketing never form a valid source.
  //The e_pt() of the bl2 will add VisCon to ba1.
  if (ba1->nud()==ARC_NUD_CCW)
    return;

  double t;
  if (bl2->LisArcVisible (ba1, t)) {
    if (ba1->AisLineVisible (bl2->start().x, bl2->start().y, bl2->end().x, bl2->end().y)) {

      //1)Both visible to each other, compute midPoint
      //  Don't compute t again.
      Point footPt = _getFootPt (bl2->start(), bl2->end(), t);

      //1-1)If footPt is not inside BucketB, early jump-out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      if (findBucketOfAPoint(footPt) != BucketB)
        return;

      //1-2)Check if footPt is valid in ba1
      double a;
      if (!ba1->isPointValidInRAngleFuzzy (footPt, a))
        return;

      //1-3)If arcPt is not inside BucketA, early jump out.
      //    Also skip adding the visibilityConstraint to avoid duplication!
      Point arcPt = _vectorPoint (ba1->center(), a, ba1->R());
      if (findBucketOfAPoint(arcPt) != BucketA)
        return;

      //double midDistSq = _distSqPointPoint (arcPt, footPt)/4;
      double midDistSq = _distSqPointLine (arcPt, bl2->start(), bl2->end())/4;
      Point midPoint = _midPointPoint (arcPt, footPt);

      //1-4)do a within-circle-validation for the midPoint
      Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
      bool bValid = LAwithinCircleValidation (bl2, ba1, footPt, arcPt, midPoint, midDistSq, SABBucket);

      //1-5)If valid, add sources into BucketIShock
      if (bValid) {
        addValidSource (bl2, ba1, footPt, arcPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
      }
    }
    else {
      //2)add ba1 to bl2's VC
      ////bl2->updateLAVisCon (ba1->pt());
    }
  }
  else {
    //For bp1 is not visible to bl2, bl2 is visible to bp1
    ///if (ba1->AisLineVisible (bl2->start().x, bl2->start().y, bl2->end().x, bl2->end().y)) {

      //3)add bl2 to ba1's VC
      //////////ba1->updateALVisCon (bl2, t);
    ///}
  }
}

//Note that AA has 4 cases of NUD!
//I)Normal case, similar to the point one.
//II)
//IV, no possible source.

void Bucketing::AAShockInit (BArc_Bucketing* ba1, Bucket* BucketA,
                    BArc_Bucketing* ba2, Bucket* BucketB, bool bWithinBucket)
{
  VECTOR_TYPE a12, a21;

  if (ba1->twinArc() == ba2)
    return;

  //For Arc-Arc, no source if both are CCW.
  if (ba1->nud()==ARC_NUD_CCW && ba2->nud()==ARC_NUD_CCW)
    return;

  //3 cases: H>r1+r2, H<vcl_fabs(r1-r2), and between.
  DIST_TYPE H = _distPointPoint (ba1->center(), ba2->center());
  if (H > ba1->R()+ba2->R()) { //1)two arcs are far away, test center.

    if (ba1->nud()!=ARC_NUD_CW || ba2->nud()!=ARC_NUD_CW)
      return;

    if (!ba1->isPointValidInRAngleFuzzy (ba2->center(), a12))
      return;

    if (!ba2->isPointValidInRAngleFuzzy (ba1->center(), a21))
      return;

    //1-1)If arcPt1 is not inside BucketA, early jump out.
    Point arcPt1 = _vectorPoint (ba1->center(), a12, ba1->R());
    if (findBucketOfAPoint(arcPt1) != BucketA)
      return;

    //1-2)If arcPt2 is not inside BucketB, early jump out.
    Point arcPt2 = _vectorPoint (ba2->center(), a21, ba2->R());
    if (findBucketOfAPoint(arcPt2) != BucketB)
      return;

    //1-3)TOO MUCH TIME. SHOULDN'T DO IT.
    //    MOST OF THE CASE HERE ARE VISIBLE. ONLY TEST THE APVisCon here!!!
    //    test the bounding box of the two arcs...
    //ba2->getBoundingBox (L2, T2, R2, B2);
    //if (!ba1->AisRectVisible(L2, T2, R2, B2))
    //  return;
    //ba1->getBoundingBox (L1, T1, R1, B1);
    //if (!ba2->AisRectVisible(L1, T1, R1, B1))
    //  return;

    double midDistSq = _distSqPointPoint (arcPt1, arcPt2)/4;
    Point midPoint = _midPointPoint (arcPt1, arcPt2);

    //1-5)do a within-circle-validation for the midPoint
    Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
    bool bValid = AAwithinCircleValidation (ba1, ba2, arcPt1, arcPt2, midPoint, midDistSq, SABBucket);

    //1-6)If valid, add sources into BucketIShock
    if (bValid) {
      addValidSource (ba1, ba2, arcPt1, arcPt2, midPoint, vcl_sqrt(midDistSq), SABBucket);
    }
  }
  else if (H<vcl_fabs(ba1->R()-ba2->R())) { //2)detect small and big arc.
    BArc_Bucketing* bigArc;
    BArc_Bucketing* smallArc;
    Bucket* BucketBigArc;
    Bucket* BucketSmallArc;
    if (ba1->R()>ba2->R()) {
      bigArc = ba1; smallArc = ba2;
      BucketBigArc = BucketA; BucketSmallArc = BucketB;
    }
    else {
      bigArc = ba2; smallArc = ba1;
      BucketBigArc = BucketB; BucketSmallArc = BucketA;
    }

    if (bigArc->nud()!=ARC_NUD_CCW || smallArc->nud()!=ARC_NUD_CW)
      return;

    VECTOR_TYPE big_a; //Test big arc first!
    if (!bigArc->isPointValidInRAngleFuzzy (smallArc->center(), big_a))
      return;

    //For smallArc, the footPt is on the other side.
    Point footPt;
    footPt.x = smallArc->center().x*2 - bigArc->center().x;
    footPt.y = smallArc->center().y*2 - bigArc->center().y;
    VECTOR_TYPE small_a;
    if (!smallArc->isPointValidInRAngleFuzzy (footPt, small_a))
      return;

    //1-1)If bigArcPt is not inside BucketBigArc, early jump out.
    Point bigArcPt = _vectorPoint (bigArc->center(), big_a, bigArc->R());
    if (findBucketOfAPoint(bigArcPt) != BucketBigArc)
      return;

    //1-2)If smallArcPt is not inside BucketSmallArc, early jump out.
    Point smallArcPt = _vectorPoint (smallArc->center(), small_a, smallArc->R());
    if (findBucketOfAPoint(smallArcPt) != BucketSmallArc)
      return;

    //1-4)test the bounding box of ba1
    //ba1->getBoundingBox (L1, T1, R1, B1);
    //if (!ba2->AisRectVisible(L1, T1, R1, B1))
    //  return;

    double midDistSq = _distSqPointPoint (bigArcPt, smallArcPt)/4;
    Point midPoint = _midPointPoint (bigArcPt, smallArcPt);

    //1-5)do a within-circle-validation for the midPoint
    Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
    bool bValid = AAwithinCircleValidation (bigArc, smallArc, bigArcPt, smallArcPt, midPoint, midDistSq, SABBucket);

    //1-6)If valid, add sources into BucketIShock
    if (bValid) {
      addValidSource (bigArc, smallArc, bigArcPt, smallArcPt, midPoint, vcl_sqrt(midDistSq), SABBucket);
    }
  }
  else { //3)arcs intersecting, no possible source.
    return;
  }
}

/*void Bucketing::AAShockInit (BArc_Bucketing* ba1, Bucket* BucketA, BArc_Bucketing* ba2, Bucket* BucketB, bool bWithinBucket)
{
  double a12, a21;
  COORD_TYPE L1, T1, R1, B1, L2, T2, R2, B2;

  if (ba1->nud()==ARC_NUD_CW) {
    if (ba2->nud()==ARC_NUD_CW) {
      //I)Normal case, similar to the point one.
      if (ba1->isPointValidInRAngleFuzzy(ba2->center(), a12)) { //AisPointVisible
        if (ba2->isPointValidInRAngleFuzzy (ba1->center(), a21)) {

          //1-1)If arcPt1 is not inside BucketA, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          Point arcPt1 = _vectorPoint (ba1->center(), a12, ba1->R());
          if (findBucketOfAPoint(arcPt1) != BucketA)
            return;

          //1-2)If arcPt2 is not inside BucketB, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          Point arcPt2 = _vectorPoint (ba2->center(), a21, ba2->R());
          if (findBucketOfAPoint(arcPt2) != BucketB)
            return;

          //1-3)Check d>r1+r2 in this case.
          double d = _distPointPoint (ba1->center(), ba2->center());
          if (d < ba1->R()+ba2->R())
            return;

          //1-4)test the bounding box of the two arcs...
          ba2->getBoundingBox (L2, T2, R2, B2);
          if (!ba1->AisRectVisible(L2, T2, R2, B2))
            return;
          ba1->getBoundingBox (L1, T1, R1, B1);
          if (!ba2->AisRectVisible(L1, T1, R1, B1))
            return;

          double midDistSq = _distSqPointPoint (arcPt1, arcPt2)/4;
          Point midPoint = _midPointPoint (arcPt1, arcPt2);

          //1-5)do a within-circle-validation for the midPoint
          Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
          bool bValid = AAwithinCircleValidation (ba1, ba2, arcPt1, arcPt2, midPoint, midDistSq, SABBucket);

          //1-6)If valid, add sources into BucketIShock
          if (bValid) {
            addValidSource (ba1, ba2, arcPt1, arcPt2, midPoint, vcl_sqrt(midDistSq), SABBucket);
          }
        }
      }
    }//end Case I
    else {//II)
      if (ba1->twinArc()==ba2) //twinElements can not form a source!
        return;
      if (ba2->isPointValidInRAngleFuzzy (ba1->center(), a21)) {
        //1-0)If arcPt2 is not visible to ba1, no valid source.
        Point arcPt2 = _vectorPoint (ba2->center(), a21, ba2->R());
        if (ba1->isPointValidInRAngleFuzzy (arcPt2, a12)) {

          //1-1)If arcPt1 is not inside BucketA, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          Point arcPt1 = _vectorPoint (ba1->center(), a12, ba1->R());
          if (findBucketOfAPoint(arcPt1) != BucketA)
            return;

          //1-2)If arcPt2 is not inside BucketB, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          if (findBucketOfAPoint(arcPt2) != BucketB)
            return;

          //1-3)Check r2>r1 in this case.
          if (ba2->R()<ba1->R())
            return;

          //1-4)test the bounding box of ba1
          ba1->getBoundingBox (L1, T1, R1, B1);
          if (!ba2->AisRectVisible(L1, T1, R1, B1))
            return;

          double midDistSq = _distSqPointPoint (arcPt1, arcPt2)/4;
          Point midPoint = _midPointPoint (arcPt1, arcPt2);

          //1-5)do a within-circle-validation for the midPoint
          Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
          bool bValid = AAwithinCircleValidation (ba1, ba2, arcPt1, arcPt2, midPoint, midDistSq, SABBucket);

          //1-6)If valid, add sources into BucketIShock
          if (bValid) {
            addValidSource (ba1, ba2, arcPt1, arcPt2, midPoint, vcl_sqrt(midDistSq), SABBucket);
          }
        }
      }
    }//end case II
  }
  else {//ba1 is CCW
    if (ba2->nud()==ARC_NUD_CW) {//III)
      if (ba1->twinArc()==ba2) //twinElements can not form a source!
        return;
      if (ba1->isPointValidInRAngleFuzzy (ba2->center(), a12)) {
        //1-0)If arcPt1 is not visible to ba2, no valid source.
        Point arcPt1 = _vectorPoint (ba1->center(), a12, ba1->R());
        if (ba2->isPointValidInRAngleFuzzy (arcPt1, a21)) {

          //1-1)If arcPt1 is not inside BucketA, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          if (findBucketOfAPoint(arcPt1) != BucketA)
            return;

          //1-2)If arcPt2 is not inside BucketB, early jump out.
          //    Also skip adding the visibilityConstraint to avoid duplication!
          Point arcPt2 = _vectorPoint (ba2->center(), a21, ba2->R());
          if (findBucketOfAPoint(arcPt2) != BucketB)
            return;

          //1-3)Check r1>r2 in this case.
          if (ba1->R()<ba2->R())
            return;

          //1-4)test the bounding box of ba2
          ba2->getBoundingBox (L2, T2, R2, B2);
          if (!ba1->AisRectVisible(L2, T2, R2, B2))
            return;

          double midDistSq = _distSqPointPoint (arcPt1, arcPt2)/4;
          Point midPoint = _midPointPoint (arcPt1, arcPt2);

          //1-5)do a within-circle-validation for the midPoint
          Bucket* SABBucket = findBucketOfAPoint2 (midPoint);
          bool bValid = AAwithinCircleValidation (ba1, ba2, arcPt1, arcPt2, midPoint, midDistSq, SABBucket);

          //1-6)If valid, add sources into BucketIShock
          if (bValid) {
            addValidSource (ba1, ba2, arcPt1, arcPt2, midPoint, vcl_sqrt(midDistSq), SABBucket);
          }
        }
      }
    }//end case III
    else {
      //IV) No possible source!
      return;
    }
  }
}*/

// #################################################################################
//         BUCKETING SOURCE INITIALIZATION FOR DYNAMIC ADDING

void Bucketing::initShocksNewElmsBucketing (BElmListType* ListA)
{
  //1)Add elements in ListA to buckets, including necessary preprocesvcl_sing.
  assignNewElmsToBuckets (ListA);

  //2)Init sources, add visibility constraints
  //  Local Shock Detection for new elements with all other elements.
  localShockInitNewElms (ListA, _validSourceList);

  //3)validate existing sources
  validateExistingSourcesForNewElms (ListA);
}

//For dynamic adding elements.
//Here we don't change the bucket structure. Just add things into existing buckets.
void Bucketing::assignNewElmsToBuckets (BElmListType* ListA)
{
  //1)First assign BPoints, including all endPts...
  BElmListType::iterator curB = ListA->begin();
  for (; curB!=ListA->end(); curB++) {
    if (curB->second->type()==BPOINT) {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) curB->second;
      BucketIndex idx = _findBucketOfAPoint (bp->pt());
      assert (idx.h!=-1);
      Bucket* curBucket = &_BucketArray[idx.h][idx.v];
      curBucket->addBElement (bp);
      bp->_bucX = idx.h;
      bp->_bucY = idx.v;
    }
  }

  //2)Secondly, assign BLines and BArcs.
  curB = ListA->begin();
  for (; curB!=ListA->end(); curB++) {
    //Only add GUIElements, when adding a GUIElement, also add it's endPt and twinElms.
    if (!curB->second->isGUIElm())
      continue;

    if (curB->second->type()==BLINE)
      assignLineToBuckets ((BLine_Bucketing*)curB->second);
    if (curB->second->type()==BARC)
      assignArcToBuckets ((BArc_Bucketing*)curB->second);
  }

  //3)Update the emptyBucket infomation.
  //  Also set the non-empty bucket's _bIsDone to false again.
  //  Note that in Dynamic Adding, we don't set _bIsDone to true.
  for (int i=0; i<_nSlots; i++)
    for (int j=0; j<_nBucketsInSlot[i]; j++) {
      if (_BucketArray[i][j].isEmpty())
        _BucketArray[i][j]._bNonEmpty = false;
      else {
        _BucketArray[i][j]._bNonEmpty = true;
        _BucketArray[i][j]._bIsDone = false;
      }
    }
}

//Assume sparse input.
//Consider that we already have a lot of VisCons for existing elements.
//For each new elm, try to form sources in a spiral way, and
//add VisCon as the old rules.
//Note to avoid duplicate sources between new elements.
void Bucketing::localShockInitNewElms (BElmListType* ListA, ValidSourceType& _validSourceList)
{
  BElmListType::iterator curB = ListA->begin();
  for (; curB!=ListA->end(); curB++) {
    switch (curB->second->type()) {
    case BPOINT:
    {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) curB->second;
      Bucket* BucketA = &_BucketArray[bp->_bucX][bp->_bucY];
      //Form sources with-in BucketA, BucketB=BucketA
      initShocks_BPoint_Bucket (bp, BucketA, BucketA, _validSourceList, true);
      initShocks_BPoint_SpiralSearch (bp, BucketA, _validSourceList);
    }
    break;
    case BLINE:
    {
      BLine_Bucketing* bl = (BLine_Bucketing*) curB->second;
      //For BLINE, we have to do it for each segment in different BucketA.
      if (bl->_bucLX<0) { //1) bl inside multiple buckets
        BucketList::iterator bit = bl->_lineInBuckets.begin();
        for (; bit != bl->_lineInBuckets.end(); bit++) {
          Bucket* BucketA = &_BucketArray[(*bit)->index().h][(*bit)->index().v];
          initShocks_BLine_Bucket (bl, BucketA, BucketA, _validSourceList, true);
          initShocks_BLine_SpiralSearch (bl, BucketA, _validSourceList);
        }
      }
      else { //2) bl inside one bucket only
        Bucket* BucketA = &_BucketArray[bl->_bucLX][bl->_bucLY];
        initShocks_BLine_Bucket (bl, BucketA, BucketA, _validSourceList, true);
        initShocks_BLine_SpiralSearch (bl, BucketA, _validSourceList);
      }
    }
    break;
    case BARC:
    {
      BArc_Bucketing* ba = (BArc_Bucketing*) curB->second;
      //For BARC, we have to do it for each segment in different BucketA.
      if (ba->_bucAX<0) { //1) ba inside multiple buckets
        BucketList::iterator bit = ba->_arcInBuckets.begin();
        for (; bit != ba->_arcInBuckets.end(); bit++) {
          Bucket* BucketA = &_BucketArray[(*bit)->index().h][(*bit)->index().v];
          initShocks_BArc_Bucket (ba, BucketA, BucketA, _validSourceList, true);
          initShocks_BArc_SpiralSearch (ba, BucketA, _validSourceList);
        }
      }
      else { //2) ba inside one bucket only
        Bucket* BucketA = &_BucketArray[ba->_bucAX][ba->_bucAY];
        initShocks_BArc_Bucket (ba, BucketA, BucketA, _validSourceList, true);
        initShocks_BArc_SpiralSearch (ba, BucketA, _validSourceList);
      }
    }
    break;
    default: break;
    }
  }
}

//A brute-force approach should go through all sources again all newElms.
//Here, with bucketing, for each newElm, run a SpiralSearch to test sources in each visible bucket.
void Bucketing::validateExistingSourcesForNewElms (BElmListType* ListA)
{
  BElmListType::iterator curB = ListA->begin();
  for (; curB!=ListA->end(); curB++) {
    switch (curB->second->type()) {
    case BPOINT:
    {
      BPoint_Bucketing* bp = (BPoint_Bucketing*) curB->second;
      Bucket* BucketA = &_BucketArray[bp->_bucX][bp->_bucY];
      //Form sources with-in BucketA, BucketB=BucketA
      validateExistingSources_BPoint_Bucket (bp, BucketA, BucketA, _invalidSourceList);
      validateExistingSources_BPoint_SpiralSearch (bp, BucketA, _invalidSourceList);
    }
    break;
    case BLINE:
    {
      BLine_Bucketing* bl = (BLine_Bucketing*) curB->second;
      //For BLINE, we have to do it for each segment in different BucketA.
      if (bl->_bucLX<0) { //1) bl inside multiple buckets
        BucketList::iterator bit = bl->_lineInBuckets.begin();
        for (; bit != bl->_lineInBuckets.end(); bit++) {
          Bucket* BucketA = &_BucketArray[(*bit)->index().h][(*bit)->index().v];
          validateExistingSources_BLine_Bucket (bl, BucketA, BucketA, _invalidSourceList);
          validateExistingSources_BLine_SpiralSearch (bl, BucketA, _invalidSourceList);
        }
      }
      else { //2) bl inside one bucket only
        Bucket* BucketA = &_BucketArray[bl->_bucLX][bl->_bucLY];
        validateExistingSources_BLine_Bucket (bl, BucketA, BucketA, _invalidSourceList);
        validateExistingSources_BLine_SpiralSearch (bl, BucketA, _invalidSourceList);
      }
    }
    break;
    case BARC:
    {
      BArc_Bucketing* ba = (BArc_Bucketing*) curB->second;
      //For BARC, we have to do it for each segment in different BucketA.
      if (ba->_bucAX<0) { //1) ba inside multiple buckets
        BucketList::iterator bit = ba->_arcInBuckets.begin();
        for (; bit != ba->_arcInBuckets.end(); bit++) {
          Bucket* BucketA = &_BucketArray[(*bit)->index().h][(*bit)->index().v];
          validateExistingSources_BArc_Bucket (ba, BucketA, BucketA, _invalidSourceList);
          validateExistingSources_BArc_SpiralSearch (ba, BucketA, _invalidSourceList);
        }
      }
      else { //2) bl inside one bucket only
        Bucket* BucketA = &_BucketArray[ba->_bucAX][ba->_bucAY];
        validateExistingSources_BArc_Bucket (ba, BucketA, BucketA, _invalidSourceList);
        validateExistingSources_BArc_SpiralSearch (ba, BucketA, _invalidSourceList);
      }
    }
    break;
    default: break;
    }
  }
}

//Here, we know that curBucket is visible to bp1.
//Note that we don't test any VisCon for Points, because go through each VisCon
// may consume more time than just a hypot to get distance!
//Check validality (distance) of each source of curBucket w.r.t bp1.
//Note that a source is possibly invalidated by many belms.
//So if found, remove it from the SIElmList.
void Bucketing::validateExistingSources_BPoint_Bucket (BPoint_Bucketing* bp1, Bucket* BucketA, Bucket* curBucket,
                                     SIElmSetType& _invalidSourceList)
{
  vcl_vector<SISource*> SourcesToDel;

  SIElmSetType::iterator sit = curBucket->SIElmList()->begin();
  for (; sit!=curBucket->SIElmList()->end(); sit++) {
    //check if current source (*sit) is valid or not.
    DIST_TYPE dist = bp1->distPoint ((*sit)->origin());
    if (!(_isG(dist, (*sit)->startTime(), DIST_BOUND))) {
      _invalidSourceList.insert (*sit);
      SourcesToDel.push_back (*sit);
    }
  }

  //Go through SourcesToDel list again to delete them...
  for (unsigned int i=0; i<SourcesToDel.size() ; i++) {
    //Remember to remove the invalid source from this bucket.
    SIElmSetType::iterator it = vcl_find (curBucket->SIElmList()->begin(),
                           curBucket->SIElmList()->end(), SourcesToDel[i]);
    if (it != curBucket->SIElmList()->end()) //if found
      curBucket->SIElmList()->erase (it);
    else
      assert (0);
  }
}

//Note that we should at least test EPVisCon and plane for the line
void Bucketing::validateExistingSources_BLine_Bucket (BLine_Bucketing* bl1, Bucket* BucketA, Bucket* curBucket,
                                    SIElmSetType& _invalidSourceList)
{
  vcl_vector<SISource*> SourcesToDel;

  SIElmSetType::iterator sit = curBucket->SIElmList()->begin();
  for (; sit!=curBucket->SIElmList()->end(); sit++) {
    //check if the shock source is visible to the BLine.
    double t = bl1->isPointValidInTPlaneFuzzy ((*sit)->origin());
    if (t==0)
      continue;

    DIST_TYPE dist = bl1->distPoint ((*sit)->origin());
    //check if current source (*sit) is valid or not.
    if (!(_isG(dist, (*sit)->startTime(), DIST_BOUND))) {
      _invalidSourceList.insert (*sit);
      SourcesToDel.push_back (*sit);
    }
  }

  //Go through SourcesToDel list again to delete them...
  for (unsigned int i=0; i<SourcesToDel.size() ; i++) {
    //Remember to remove the invalid source from this bucket.
    SIElmSetType::iterator it = vcl_find (curBucket->SIElmList()->begin(),
                           curBucket->SIElmList()->end(), SourcesToDel[i]);
    if (it != curBucket->SIElmList()->end()) //if found
      curBucket->SIElmList()->erase (it);
    else
      assert (0);
  }
}

void Bucketing::validateExistingSources_BArc_Bucket (BArc_Bucketing* ba1, Bucket* BucketA, Bucket* curBucket,
                                    SIElmSetType& _invalidSourceList)
{
  vcl_vector<SISource*> SourcesToDel;

  SIElmSetType::iterator sit = curBucket->SIElmList()->begin();
  for (; sit!=curBucket->SIElmList()->end(); sit++) {
    //check if the shock source is visible to the BArc.
    VECTOR_TYPE a;
    if (!ba1->isPointValidInRAngleFuzzy ((*sit)->origin(), a))
      continue;

    DIST_TYPE dist = ba1->distPoint ((*sit)->origin());
    //check if current source (*sit) is valid or not.
    if (!(_isG(dist, (*sit)->startTime(), DIST_BOUND))) {
      _invalidSourceList.insert (*sit);
      SourcesToDel.push_back (*sit);
    }
  }

  //Go through SourcesToDel list again to delete them...
  for (unsigned int i=0; i<SourcesToDel.size() ; i++) {
    //Remember to remove the invalid source from this bucket.
    SIElmSetType::iterator it = vcl_find (curBucket->SIElmList()->begin(),
                           curBucket->SIElmList()->end(), SourcesToDel[i]);
    if (it != curBucket->SIElmList()->end()) //if found
      curBucket->SIElmList()->erase (it);
    else
      assert (0);
  }
}

void Bucketing::validateExistingSources_BPoint_SpiralSearch (BPoint_Bucketing* bp1, Bucket* BucketA,
                                         SIElmSetType& _invalidSourceList)
{
  //1)Initialization: get a new SearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.

    //if the curBucket is visible...
    if (bp1->PisRectVisible (BucketB->Left(), BucketB->Top(),
                     BucketB->Right(), BucketB->Bottom())) {
      //2-1)If BucketB not-done, Form possible shocks with generators in BucketB
      if (BucketB->_bIsDone==false)
        validateExistingSources_BPoint_Bucket (bp1, BucketA, BucketB, _invalidSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           bp1->PisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    //    Note that is possible we have some buckets here that's already done.
    if (curBucket->_bVisited==_BucketBSearchID) //>=
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty)...
    if (bp1->PisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        validateExistingSources_BPoint_Bucket (bp1, BucketA, curBucket, _invalidSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           bp1->PisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue
}


void Bucketing::validateExistingSources_BLine_SpiralSearch (BLine_Bucketing* bl1, Bucket* BucketA,
                                        SIElmSetType& _invalidSourceList)
{
  //1)Initialization: get a new SearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.

    //if the curBucket is visible...
    if (bl1->LisRectVisible (BucketB->Left(), BucketB->Top(),
                     BucketB->Right(), BucketB->Bottom())) {
      //2-1)If BucketB not-done, Form possible shocks with generators in BucketB
      if (BucketB->_bIsDone==false)
        validateExistingSources_BLine_Bucket (bl1, BucketA, BucketB, _invalidSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           bl1->LisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    //    Note that is possible we have some buckets here that's already done.
    if (curBucket->_bVisited==_BucketBSearchID) //>=
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty)...
    if (bl1->LisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        validateExistingSources_BLine_Bucket (bl1, BucketA, curBucket, _invalidSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           bl1->LisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue
}


void Bucketing::validateExistingSources_BArc_SpiralSearch (BArc_Bucketing* ba1, Bucket* BucketA,
                                        SIElmSetType& _invalidSourceList)
{
  //1)Initialization: get a new SearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.

    //if the curBucket is visible...
    if (ba1->AisRectVisible (BucketB->Left(), BucketB->Top(),
                     BucketB->Right(), BucketB->Bottom())) {
      //2-1)If BucketB not-done, Form possible shocks with generators in BucketB
      if (BucketB->_bIsDone==false)
        validateExistingSources_BArc_Bucket (ba1, BucketA, BucketB, _invalidSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           ba1->AisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    //    Note that is possible we have some buckets here that's already done.
    if (curBucket->_bVisited==_BucketBSearchID) //>=
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty)...
    if (ba1->AisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        validateExistingSources_BArc_Bucket (ba1, BucketA, curBucket, _invalidSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           ba1->AisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue
}

// #################################################################################
//         BUCKETING SOURCE INITIALIZATION

void Bucketing::initShocksBucketing ()
{
  Point  midPoint;

  //!!Debug
  #ifdef _VIS_DEBUG
  nP_Bucket = 0;
  nP_nonNeighboringBucket = 0;
  nL_Bucket = 0;
  nL_nonNeighboringBucket = 0;
  #endif

  //=============== MAIN BUCKETING CODE =================
  long sec_start_bucket = clock();
  //1)Build Buckets: put points into buckets.
  assignPointsToBuckets ();

  //2)For each bucket, determine the neighboring buckets.
  assignNeighboringBuckets ();

  //3)For Line and Arcs, assign Lines and Arcs to buckets.
  assignLinesArcsToBuckets ();

  //Multi-resolution: Skipped! assignVisibleBuckets_LayeredSearch ();

  long sec_after_build_bucket = clock ();
  long time_build_bucket = sec_after_build_bucket - sec_start_bucket;
  vcl_cout<< "Time for Building Buckets: "<< time_build_bucket <<" msec."<<vcl_endl;

  //4)Local Shock Detection
  localShockInit (_validSourceList);
  long sec_after_shockinit_bucket = clock();
  long time_local_shock_detect = sec_after_shockinit_bucket - sec_after_build_bucket;
  vcl_cout<< "Time for Local Shock Detection within Buckets: "<< time_local_shock_detect <<" msec."<<vcl_endl;

  //5)For each generator A, and for each point B in visible buckets of BucketA
  //  Try to form every possible source S_ab in a spiral way.
  //  Early Jump out and visibility constraint
  for (int i=0; i<_nSlots; i++) {
    for (int j=0; j<_nBucketsInSlot[i]; j++) { //For each non-empty BucketA...
      if (_BucketArray[i][j]._bNonEmpty) {
        BElmVectorType::iterator it = _BucketArray[i][j].BElmList()->begin();
        for (; it!=_BucketArray[i][j].BElmList()->end(); ++it) {
          switch ((*it)->type()) {
          case BPOINT:
            //5-A)Do a sipral search for this BPoint_Bucketing
            initShocks_BPoint_SpiralSearch ((BPoint_Bucketing*)(*it), &_BucketArray[i][j], _validSourceList);
          break;
          case BLINE:
            //5-B)Do a sipral search for this BLine_Bucketing
            //    Note that if a BLine_Bucketing is covered in multiple buckets,
            //    This function will be executed in each bucket.
            //    Each call computes souces within that bucket only!
            initShocks_BLine_SpiralSearch ((BLine_Bucketing*)(*it), &_BucketArray[i][j], _validSourceList);
          break;
          case BARC:
            //5-C)Do a sipral search for this BArc_Bucketing
            //    Note that if a BArc_Bucketing is covered in multiple buckets,
            //    This function will be executed in each bucket.
            //    Each call computes souces within that bucket only!
            initShocks_BArc_SpiralSearch ((BArc_Bucketing*)(*it), &_BucketArray[i][j], _validSourceList);
          break;
          default: break;
          }
        }//end for each (*it) in each bucket
      }//end if empty bucket

      //6)After it, BucketA is done. Never need to look at it again.
      _BucketArray[i][j]._bIsDone = true;

    }//end j
  }//end i

  //7)Output results.
  long sec_after_bucketing = clock();
  long time_forming_source = sec_after_bucketing - sec_after_shockinit_bucket;
  vcl_cout<< "Time for Forming Sources between Buckets: "<< time_forming_source <<" msec."<<vcl_endl;
  long total_time = time_build_bucket + time_local_shock_detect + time_forming_source;
  vcl_cout<< "-->Total Time for Bucketing: "<< total_time <<" msec."<<vcl_endl;
  vcl_cout<< "Bucketing: # of Sources= " << _validSourceList.size() <<vcl_endl;

  //!!Debug
  #ifdef _VIS_DEBUG
  vcl_cout<< "---Debug---"<<vcl_endl;
  vcl_cout<< "nP_Bucket: "<<nP_Bucket<<vcl_endl;
  vcl_cout<< "nP_nonNeighboringBucket: "<<nP_nonNeighboringBucket<<vcl_endl;
  vcl_cout<< "nP_Bucket/nBElement: "<<(double)nP_Bucket/boundary->nBElement()*2<<vcl_endl;
  vcl_cout<< "nL_Bucket: "<<nL_Bucket<<vcl_endl;
  vcl_cout<< "nL_nonNeighboringBucket: "<<nL_nonNeighboringBucket<<vcl_endl;
  vcl_cout<< "nL_Bucket/nBElement: "<<(double)nL_Bucket/boundary->nBElement()*2<<vcl_endl;
  #endif
}



//MAIN SPIRAL SEARCH SHOCK INIT. FOR EACH BPOINT
//Note that for already done bucket, we still have to test if it is visible or visited.
//Because it's neighbor might form a valid source with bp1.
void Bucketing::initShocks_BPoint_SpiralSearch (BPoint_Bucketing* bp1, Bucket* BucketA,
                                ValidSourceType& validSourceList)
{
  //1)Initialization: get a new SearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.

    //if the curBucket is visible...
    if (bp1->PisRectVisible (BucketB->Left(), BucketB->Top(),
                     BucketB->Right(), BucketB->Bottom())) {
      //2-1)If BucketB not-done, Form possible shocks with generators in BucketB
      if (BucketB->_bIsDone==false)
        initShocks_BPoint_Bucket (bp1, BucketA, BucketB, _validSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           bp1->PisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    //    Note that is possible we have some buckets here that's already done.
    if (curBucket->_bVisited==_BucketBSearchID) //>=
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty)...
    if (bp1->PisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        initShocks_BPoint_Bucket (bp1, BucketA, curBucket, _validSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           bp1->PisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue
}

//MAIN SPIRAL SEARCH SHOCK INIT. FOR EACH BLINE
void Bucketing::initShocks_BLine_SpiralSearch (BLine_Bucketing* bl1, Bucket* BucketA, ValidSourceType& validSourceList)
{
  //1)Initialization: get a new SearchID: _BucketBSearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.
    //if the curBucket is not-done & visible...
    if (bl1->LisRectVisible (BucketB->Left(), BucketB->Top(),
                    BucketB->Right(), BucketB->Bottom())) {
      //2-1)Form possible shocks with generators in curBucket
      if (BucketB->_bIsDone==false)
        initShocks_BLine_Bucket (bl1, BucketA, BucketB, _validSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           bl1->LisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    if (curBucket->_bVisited==_BucketBSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty) and not done...
    if (bl1->LisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        initShocks_BLine_Bucket (bl1, BucketA, curBucket, _validSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           bl1->LisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue
}

//MAIN SPIRAL SEARCH SHOCK INIT. FOR EACH BARC
void Bucketing::initShocks_BArc_SpiralSearch (BArc_Bucketing* ba1, Bucket* BucketA, ValidSourceType& validSourceList)
{
  //1)Initialization: get a new SearchID: _BucketBSearchID.
  getNewBucketBSearchID ();

  BucketA->_bVisited = _BucketBSearchID; //of course, BucketA itself is already visited.
  BucketQueue visibleBucketQueue;      //queue for the spiral search

  //2)Try to form shocks with visible neighboring buckets
  //  also try to rule them out usingthe visibility constraints.
  //  Note that it is possible that empty bucket's neighbor contains valid source.
  //  So we can't skip empty bucket in the spiral search!!
  BucketList::iterator it = BucketA->neighborList()->begin();
  for (; it!=BucketA->neighborList()->end(); ++it) {
    Bucket* BucketB = (*it);
    BucketB->_bVisited = _BucketBSearchID; //BucketB is now visited.
    //if the curBucket is not-done & visible...
    if (ba1->AisRectVisible (BucketB->Left(), BucketB->Top(),
                    BucketB->Right(), BucketB->Bottom())) {
      //2-1)Form possible shocks with generators in curBucket
      if (BucketB->_bIsDone==false)
        initShocks_BArc_Bucket (ba1, BucketA, BucketB, _validSourceList);

      //2-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = BucketB->neighborList()->begin();
      for (; nit!=BucketB->neighborList()->end(); ++nit) {
        Bucket* neiBucketB = (*nit);
        if (neiBucketB->_bVisited<_BucketBSearchID &&
           ba1->AisRectVisible (neiBucketB->Left(), neiBucketB->Top(),
                         neiBucketB->Right(), neiBucketB->Bottom()))
          visibleBucketQueue.push (neiBucketB);
      }
    }
  }

  //3)The spiral search through all buckets in the queue.
  while (!visibleBucketQueue.empty()) {
    //3-1)Deque the current and set it to visited.
    Bucket* curBucket = visibleBucketQueue.front();
    visibleBucketQueue.pop();

    //3-2)If already visited, ignore.
    //    It's needed because it's possible that curBucket is not visited when it's added,
    //    but later on itself is visited in the neighbor list.
    if (curBucket->_bVisited==_BucketBSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisited<_BucketBSearchID);
    #endif
    curBucket->_bVisited = _BucketBSearchID; //curBucket is now visited.

    //3-3)if the curBucket is visible (it is non-empty) and not done...
    if (ba1->AisRectVisible (curBucket->Left(), curBucket->Top(),
                    curBucket->Right(), curBucket->Bottom())) {
      //3-3-1)Form possible shocks with generators in curBucket
      if (curBucket->_bIsDone==false)
        initShocks_BArc_Bucket (ba1, BucketA, curBucket, _validSourceList);

      //3-3-2)Add the curBucket's non-visited & visible neighbor bucket to the queue
      BucketList::iterator nit = curBucket->neighborList()->begin();
      for (; nit!=curBucket->neighborList()->end(); ++nit) {
        Bucket* neiBucket = (*nit);
        if (neiBucket->_bVisited<_BucketBSearchID &&
           ba1->AisRectVisible (neiBucket->Left(), neiBucket->Top(),
                         neiBucket->Right(), neiBucket->Bottom()))
          visibleBucketQueue.push (neiBucket);
      }
    }
  }//end while queue*/
}

//For the BPoint_Bucketing bp1 and the curBucket
//Wee need bWithinBucket here for DynAdding
void Bucketing::initShocks_BPoint_Bucket (BPoint_Bucketing* bp1, Bucket* BucketA, Bucket* curBucket,
                            ValidSourceType& validSourceList, bool bWithinBucket)
{
  //0)For empty buckets, return
  if (curBucket->_bNonEmpty==false)
    return;

  //1)First test the visibility constraint of bp1.
  //  Note that the visibility constraint now is more and
  //  might be sufficient to rule the bucket out.
  if (!bp1->PisRectVisible (curBucket->Left(), curBucket->Top(),
                   curBucket->Right(), curBucket->Bottom()))
    return;

  //!!Debug
  #ifdef _VIS_DEBUG
  ///assert (bp1->getBucket()!=curBucket); //if BucketA!=bp1->getBucket(), alarm!

  //if forming source with non-neighboring bucket, output.
  if (!BucketA->isNeighboringBucket(curBucket)) {
    ///vcl_cout<< "bp1 P-P Visibility Constraint: "<< bp1->_PPVisCon.size() <<vcl_endl;
    ///vcl_cout<< "bp1: "<<bp1->id()<<"-BucketB["<<curBucket->index().h<<"]["<<curBucket->index().v<<"]"<<vcl_endl;
    nP_nonNeighboringBucket++;
  }
  nP_Bucket++;
  #endif

  //3)Form possible shocks with generators in curBucket
  //  Note that the order of forming source between buckets is maintained.
  //  So no need to check for avoiding duplicate sources
  BElmVectorType::iterator bit = curBucket->BElmList()->begin();
  for (; bit!=curBucket->BElmList()->end(); ++bit) {
    switch ((*bit)->type()) {
    case BPOINT:
      PPShockInit (bp1, (BPoint_Bucketing*)(*bit), bWithinBucket);
    break;
    case BLINE:
      PLShockInit (bp1, (BLine_Bucketing*)(*bit), curBucket, bWithinBucket);
    break;
    case BARC:
      PAShockInit (bp1, (BArc_Bucketing*)(*bit), curBucket, bWithinBucket);
    break;
    default: break;
    }//end switch
  }//for each element in curBucket
}

//For the bl1 (in BucketA) and the curBucket
void Bucketing::initShocks_BLine_Bucket (BLine_Bucketing* bl1, Bucket* BucketA, Bucket* curBucket,
                            ValidSourceType& validSourceList, bool bWithinBucket)
{
  //0)For empty buckets, return
  if (!curBucket->_bNonEmpty)
    return;

  //1)First test the visibility constraint of bl1.
  //  Note that the visibility constraint now is more and
  //  might be sufficient to rule the bucket out.
  if (!bl1->LisRectVisible (curBucket->Left(), curBucket->Top(),
                   curBucket->Right(), curBucket->Bottom()))
    return;

  //!!Debug
  #ifdef _VIS_DEBUG
  //if forming source with non-neighboring bucket, output.
  if (!BucketA->isNeighboringBucket(curBucket)) {
    ///vcl_cout<< "bl1 _LPVisCon: " <<vcl_endl;
    ///vcl_cout<< "bl1: "<<bl1->id()<<"-BucketB["<<curBucket->index().h<<"]["<<curBucket->index().v<<"]"<<vcl_endl;
    nL_nonNeighboringBucket++;
  }
  nL_Bucket++;
  #endif

  //3)Form possible shocks with generators in curBucket
  //  Note that the order of forming source between buckets is maintained.
  //  So no need to check for avoiding duplicate sources
  BElmVectorType::iterator bit = curBucket->BElmList()->begin();
  for (; bit!=curBucket->BElmList()->end(); ++bit) {
    switch ((*bit)->type()) {
    case BPOINT:
      //Note that LPShockInit() and PLShockInit() is different!
      LPShockInit (bl1, BucketA, (BPoint_Bucketing*)(*bit), bWithinBucket);
    break;
    case BLINE:
      //Line-Line never forms a source!
    break;
    case BARC:
      LAShockInit (bl1, BucketA, (BArc_Bucketing*)(*bit), curBucket, bWithinBucket);
    break;
    default: break;
    }//end switch
  }//for each element in curBucket
}

//For the bl1 (in BucketA) and the curBucket
void Bucketing::initShocks_BArc_Bucket (BArc_Bucketing* ba1, Bucket* BucketA, Bucket* curBucket,
                           ValidSourceType& validSourceList, bool bWithinBucket)
{
  //0)For empty buckets, return
  if (!curBucket->_bNonEmpty)
    return;

  //1)First test the visibility constraint of ba1.
  //  Note that the visibility constraint now is more and
  //  might be sufficient to rule the bucket out.
  if (!ba1->AisRectVisible (curBucket->Left(), curBucket->Top(),
                   curBucket->Right(), curBucket->Bottom()))
    return;

  //!!Debug
  #ifdef _VIS_DEBUG
  //if forming source with non-neighboring bucket, output.
  if (!BucketA->isNeighboringBucket(curBucket)) {
    ///vcl_cout<< "bl1 _LPVisCon: " <<vcl_endl;
    ///vcl_cout<< "bl1: "<<bl1->id()<<"-BucketB["<<curBucket->index().h<<"]["<<curBucket->index().v<<"]"<<vcl_endl;
    nA_nonNeighboringBucket++;
  }
  nA_Bucket++;
  #endif

  //3)Form possible shocks with generators in curBucket
  //  Note that the order of forming source between buckets is maintained.
  //  So no need to check for avoiding duplicate sources
  BElmVectorType::iterator bit = curBucket->BElmList()->begin();
  for (; bit!=curBucket->BElmList()->end(); ++bit) {
    switch ((*bit)->type()) {
    case BPOINT:
      //Note that LPShockInit() and PLShockInit() is different!
      APShockInit (ba1, BucketA, (BPoint_Bucketing*)(*bit), bWithinBucket);
    break;
    case BLINE:
      ALShockInit (ba1, BucketA, (BLine_Bucketing*)(*bit), curBucket, bWithinBucket);
    break;
    case BARC:
      AAShockInit (ba1, BucketA, (BArc_Bucketing*)(*bit), curBucket, bWithinBucket);
    break;
    default: break;
    }//end switch
  }//for each element in curBucket
}

//EPSILONISSUE
//Deal with the connectedGUI and fuzzy distance comparison issue for difference type of GA and GB.
//Only do validation within bucket. This is the first step of original withinCircleValidation().
inline bool Bucketing::doesBLineInvalidatePPSource (BLine_Bucketing* bline, BPoint_Bucketing* GPA, BPoint_Bucketing* GPB,
                                   Point midPoint, double midDistSq)
{
  bool bIsConnectedGUIA = false;
  bool bIsConnectedGUIB = false;
  bool bIsConnectedGUI = false;
  //Test if (*sbit) is the conncectedGUI for ePtA or ePtB.
  if (bline->s_pt()==GPA || bline->e_pt()==GPA)
    bIsConnectedGUIA = true;
  if (bline->s_pt()==GPB || bline->e_pt()==GPB)
    bIsConnectedGUIB = true;
  //If bline is the line that connect ePtA and ePtB, SAB is not valid!
  if (bIsConnectedGUIA && bIsConnectedGUIB)
    return true;
  //If bline is one of the connectedGUI...
  if (bIsConnectedGUIA || bIsConnectedGUIB)
    bIsConnectedGUI = true;

  //1-1)If t<=0 or t>=1, bline does not invalidate it.
  ///double tSAB_to_Line = _getT (midPoint, bline->start(), bline->end());
  //if (tSAB_to_Line<=0 || tSAB_to_Line>=1)
  ///if (_isLEq(tSAB_to_Line,0,L_EPSILON) || _isGEq(tSAB_to_Line,1,L_EPSILON))
  ///  return false;

  //1-2)If SAB not in the left plane of bline, bline does not invalidate it.
  //if (!bline->_isPointValidInPlane(midPoint))
  //  return false;

  //1-3)Compute footPoint
  //Point footPoint = _getValidFootPt (midPoint, bline->start(), bline->end(), tSAB_to_Line);
  //double curDistSq = _distSqPointPoint (footPoint, midPoint);
  double curDistSq = bline->validDistSqPoint (midPoint);
  if (curDistSq==ISHOCK_DIST_HUGE)
    return false;

  //2-1)For nonConnectedGUI, test for _isLEq
  if (!bIsConnectedGUI) {
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  }
  //EPSILONISSUE 9:
  else { //2-2)ForConnedtedGUI, both t and plane is ok here, e_pt() can't form source!!
    //if (_isL(curDistSq, midDistSq, DIST_BOUND))
    //between ANGLE_EPSILON and  DIST_EPSILON, generate source
    if (_isGEq(curDistSq, midDistSq, DIST_BOUND))
      return false;
    else
      return true;
  }

  return false;
}

//Deal with the connectedGUI and fuzzy distance comparison issue for difference type of GA and GB.
//Only do validation within bucket. This is the first step of original withinCircleValidation().
inline bool Bucketing::doesBArcInvalidatePPSource (BArc_Bucketing* barc, BPoint_Bucketing* GPA, BPoint_Bucketing* GPB,
                                  Point midPoint, double midDistSq)
{
  bool bIsConnectedGUIA = false;
  bool bIsConnectedGUIB = false;
  bool bIsConnectedGUI = false;
  //Test if (*sbit) is the conncectedGUI for ePtA or ePtB.
  if (barc->s_pt()==GPA || barc->e_pt()==GPA)
    bIsConnectedGUIA = true;
  if (barc->s_pt()==GPB || barc->e_pt()==GPB)
    bIsConnectedGUIB = true;
  //If barc is the arc that connect ePtA and ePtB, SAB is not valid!
  if (bIsConnectedGUIA && bIsConnectedGUIB)
    return true;
  //If barc is one of the connectedGUI...
  if (bIsConnectedGUIA || bIsConnectedGUIB)
    bIsConnectedGUI = true;

  //1)If midPoint can not see from barc, barc does not invalidate it.
  ///double a_SAB_to_Arc;
  ///if (!barc->isPointValidInRAngleFuzzy (midPoint, a_SAB_to_Arc))
  ///  return false;

  //1-2)Compute footPoint
  ///Point footPoint = _vectorPoint (barc->center(), a_SAB_to_Arc, barc->R());
  ///double curDistSq = _distSqPointPoint (footPoint, midPoint);
  double curDistSq = barc->validDistSqPoint (midPoint);
  if (curDistSq==ISHOCK_DIST_HUGE)
    return false;

  //2-1)For nonConnectedGUI, test for _isLEq
  if (!bIsConnectedGUI) {
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  }
  //EPSILONISSUE 9:
  else { //2-2)ForConnedtedGUI, both t and plane is ok here, e_pt() can't form source!!
    //if (_isL(curDistSq, midDistSq, DIST_BOUND))
    //between ANGLE_EPSILON and  DIST_EPSILON, generate source
    if (_isGEq(curDistSq, midDistSq, DIST_BOUND))
      return false;
    else
      return true;
  }

  return false;
}

inline bool Bucketing::doesBLineInvalidatePXSource (BLine_Bucketing* bline, BPoint_Bucketing* GPA, Point footPt,
                                    Point midPoint, double midDistSq)
{
  bool bIsConnectedGUI = false;
  if (bline->s_pt()==GPA || bline->e_pt()==GPA)
    bIsConnectedGUI = true;

  //2-2-1)If t not valid, bline does not invalidate it.
  double tSAB_to_Line = bline->isPointValidInTPlaneFuzzy (midPoint);
  if (tSAB_to_Line==0)
    return false;
  //2-2-3)Compute footPoint
  Point footPoint = _getValidFootPt (midPoint, bline->start(), bline->end(), tSAB_to_Line);
  double curDistSq = _distSqPointPoint (footPoint, midPoint);

  //2-1)For nonConnectedGUI, test for _isLEq
  if (!bIsConnectedGUI) {
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  }
  //EPSILONISSUE 10
  else { //2-2)ForConnedtedGUI, both t and plane is ok here, e_pt() can't form source!!
    //if (_isL(curDistSq, midDistSq, DIST_BOUND))
    //compute vcl_vector of midPoint->footPoint
    double vMF = _vPointPoint (midPoint, footPoint);
    double vMA = _vPointPoint (midPoint, GPA->pt());
    if (!_isEq(vMF, vMA, TO_EPSILON))
      return true;
  }

  return false;
}

inline bool Bucketing::doesBArcInvalidatePXSource (BArc_Bucketing* barc, BPoint_Bucketing* GPA, Point footPt,
                                  Point midPoint, double midDistSq)
{
  bool bIsConnectedGUI = false;
  if (barc->s_pt()==GPA || barc->e_pt()==GPA)
    bIsConnectedGUI = true;

  //1)If midPoint can not see from barc, barc does not invalidate it.
  double a_SAB_to_Arc;
  if (!barc->isPointValidInRAngleFuzzy (midPoint, a_SAB_to_Arc))
    return false;

  //1-2)Compute footPoint
  Point footPoint = _vectorPoint (barc->center(), a_SAB_to_Arc, barc->R());
  double curDistSq = _distSqPointPoint (footPoint, midPoint);

  //2-1)For nonConnectedGUI, test for _isLEq
  if (!bIsConnectedGUI) {
    if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
      return true;
  }
  //EPSILONISSUE 10
  else { //2-2)ForConnedtedGUI, both t and plane is ok here, e_pt() can't form source!!
    //if (_isL(curDistSq, midDistSq, DIST_BOUND))
    //compute vcl_vector of midPoint->footPoint
    double vMF = _vPointPoint (midPoint, footPoint);
    double vMA = _vPointPoint (midPoint, GPA->pt());
    if (!_isEq(vMF, vMA, TO_EPSILON))
      return true;
  }

  return false;
}


bool Bucketing::doesBLineInvalidateAXSource (BLine_Bucketing* bline, Point midPoint, double midDistSq)
{
  double curDistSq = bline->validDistSqPoint (midPoint);
  if (curDistSq==ISHOCK_DIST_HUGE)
    return false;

  if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
    return true;

  return false;
}

bool Bucketing::doesBArcInvalidateAXSource (BArc_Bucketing* barc, Point midPoint, double midDistSq)
{
  double curDistSq = barc->validDistSqPoint (midPoint);
  if (curDistSq==ISHOCK_DIST_HUGE)
    return false;

  if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
    return true;

  return false;
}

//return true if the midPoint is valid, false else...
bool Bucketing::PPwithinCircleValidation (BPoint_Bucketing* GPA, BPoint_Bucketing* GPB,
                             Point midPoint, double midDistSq, Bucket* SABBucket)
{
  Point GA = GPA->pt();
  Point GB = GPB->pt();
  double curDistSq;

  //1)First try the points in the bucket that contains midPoint
  //  If anyone of them invalidate SAB, SAB is not valid.
  BElmVectorType::iterator sbit = SABBucket->BElmList()->begin();
  for (; sbit!=SABBucket->BElmList()->end(); ++sbit) {
    //1)GA or GB can not invalidate their own shock...
    if (GPA==(*sbit) || GPB==(*sbit))
      continue;
    switch ((*sbit)->type()) {
    case BPOINT:
      //1-1)POINT, test for isLEq
      curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return false;
    break;
    case BLINE:
      if (doesBLineInvalidatePPSource ((BLine_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
        return false;
    break;
    case BARC:
      if (doesBArcInvalidatePPSource ((BArc_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
        return false;
    break;
    default: break;
    }
  }

  //2)Test if the circle is completely inside the bucket.
  //  If yes, done. SAB is valid.
  //Determine bounding box of the circle: L, T, R, B
  Point PM = midPoint;
  double r = hypot (GB.y-GA.y, GB.x-GA.x);
  double L = PM.x - r;
  double T = PM.y - r;
  double R = PM.x + r;
  double B = PM.y + r;
  Point LT (L, T);
  Point RT (R, T);
  Point LB (L, B);
  Point RB (R, B);
  //test if 4 corner points are all inside
  if (SABBucket->isPointInside (LT) &&
     SABBucket->isPointInside (RT) &&
     SABBucket->isPointInside (LB) &&
     SABBucket->isPointInside (RB)) {
    //SAB is valid
    return true;
  }

  //3)Test on first-layer neighbors.

  //3-1)Initialization: Setup the validationQueue for a spiral search
  //    Get a new SearchID (_nCurrentSearchID). No initialization of flags needed.
  //    the smaller id means it's never been searched.
  getNewCircleSearchID();

  BucketQueue validationQueue;
  SABBucket->_bVisitedValidation = _CircleSearchID; //SABBucket itself is visited now.

  BucketList::iterator snit = SABBucket->neighborList()->begin();
  for (; snit!=SABBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.
    //3-1)Ignore the empty neighbor...WRONG
    ///if (!curBucket->_bNonEmpty)
    ///  continue;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
                        (*snit)->Right(), (*snit)->Bottom()) ) {
      //3-2)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = (*snit)->BElmList()->begin();
      for (; sbit!=(*snit)->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GPB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePPSource ((BLine_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
          return false;
        break;
        case BARC:
          if (doesBArcInvalidatePPSource ((BArc_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //3-3)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //**vcl_cout<<"GAid: "<<GAid<<", GBid: "<<GBid<<", validationQueue size: "<<validationQueue.size()<<vcl_endl;

  //4)Test on the validationQueue in a spiral way...
  //  If anyone invalidates it, done. Else, run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!validationQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = validationQueue.front();
    validationQueue.pop();

    //4-2)If already visited, ignore.
    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
                        curBucket->Right(), curBucket->Bottom()) ) {
      //4-3)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = curBucket->BElmList()->begin();
      for (; sbit!=curBucket->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GPB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePPSource ((BLine_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidatePPSource ((BArc_Bucketing*)(*sbit), GPA, GPB, midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //4-4)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while validationQueue

  //4-5)Pass all invalidation, the source SAB is valid
  return true;
}

bool Bucketing::PLwithinCircleValidation (BPoint_Bucketing* GPA, BLine_Bucketing* GLB,
                            Point footPt, Point midPoint, double midDistSq, Bucket* SABBucket)
{
  Point GA = GPA->pt();
  Point GB = footPt;
  //int GAid = GPA->id();
  //int GBid = GLB->id();
  double curDistSq;

  //1)First try the points in the bucket that contains midPoint
  //  If anyone of them invalidate SAB, SAB is not valid.
  BElmVectorType::iterator sbit = SABBucket->BElmList()->begin();
  for (; sbit!=SABBucket->BElmList()->end(); ++sbit) {
    //1)GA or GB can not invalidate their own shock...
    if (GPA==(*sbit) || GLB==(*sbit))
      continue;
    switch ((*sbit)->type()) {
    case BPOINT:
      curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return false;
    break;
    case BLINE:
      if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
        return false;
    break;
    case BARC:
      if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
        return false;
    break;
    default: break;
    }
  }

  //2)Test if the circle is completely inside the bucket.
  //  If yes, done. SAB is valid.
  //Determine bounding box of the circle: L, T, R, B
  Point PM = midPoint;
  double r = hypot (GB.y-GA.y, GB.x-GA.x);
  double L = PM.x - r;
  double T = PM.y - r;
  double R = PM.x + r;
  double B = PM.y + r;
  Point LT (L, T);
  Point RT (R, T);
  Point LB (L, B);
  Point RB (R, B);
  //test if 4 corner points are all inside
  if (SABBucket->isPointInside (LT) &&
     SABBucket->isPointInside (RT) &&
     SABBucket->isPointInside (LB) &&
     SABBucket->isPointInside (RB)) {
    //SAB is valid
    return true;
  }

  //3)Test on first-layer neighbors.

  //3-1)Initialization: Setup the validationQueue for a spiral search
  //    Get a new SearchID (_nCurrentSearchID). No initialization of flags needed.
  //    the smaller id means it's never been searched.
  getNewCircleSearchID();

  BucketQueue validationQueue;
  SABBucket->_bVisitedValidation = _CircleSearchID; //SABBucket itself is visited now.

  BucketList::iterator snit = SABBucket->neighborList()->begin();
  for (; snit!=SABBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.
    //3-1)Ignore the empty neighbor...WRONG
    ///if (!curBucket->_bNonEmpty)
    ///  continue;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
                        (*snit)->Right(), (*snit)->Bottom()) ) {
      //3-2)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = (*snit)->BElmList()->begin();
      for (; sbit!=(*snit)->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GLB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //3-3)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //**vcl_cout<<"GAid: "<<GAid<<", GBid: "<<GBid<<", validationQueue size: "<<validationQueue.size()<<vcl_endl;

  //4)Test on the validationQueue in a spiral way...
  //  If anyone invalidates it, done. Else, run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!validationQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = validationQueue.front();
    validationQueue.pop();

    //4-2)If already visited, ignore.
    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
                        curBucket->Right(), curBucket->Bottom()) ) {
      //4-3)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = curBucket->BElmList()->begin();
      for (; sbit!=curBucket->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GLB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //4-4)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while validationQueue

  //4-5)Pass all invalidation, the source SAB is valid
  return true;
}


bool Bucketing::PAwithinCircleValidation (BPoint_Bucketing* GPA, BArc_Bucketing* GAB,
                            Point footPt, Point midPoint, double midDistSq, Bucket* SABBucket)
{
  Point GA = GPA->pt();
  Point GB = footPt;
  double curDistSq;

  //1)First try the points in the bucket that contains midPoint
  //  If anyone of them invalidate SAB, SAB is not valid.
  BElmVectorType::iterator sbit = SABBucket->BElmList()->begin();
  for (; sbit!=SABBucket->BElmList()->end(); ++sbit) {
    //1)GA or GB can not invalidate their own shock...
    if (GPA==(*sbit) || GAB==(*sbit))
      continue;
    switch ((*sbit)->type()) {
    case BPOINT:
      curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return false;
    break;
    case BLINE:
      if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
        return false;
    break;
    case BARC:
      if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
        return false;
    break;
    default: break;
    }
  }

  //2)Test if the circle is completely inside the bucket.
  //  If yes, done. SAB is valid.
  //Determine bounding box of the circle: L, T, R, B
  Point PM = midPoint;
  double r = hypot (GB.y-GA.y, GB.x-GA.x);
  double L = PM.x - r;
  double T = PM.y - r;
  double R = PM.x + r;
  double B = PM.y + r;
  Point LT (L, T);
  Point RT (R, T);
  Point LB (L, B);
  Point RB (R, B);
  //test if 4 corner points are all inside
  if (SABBucket->isPointInside (LT) &&
     SABBucket->isPointInside (RT) &&
     SABBucket->isPointInside (LB) &&
     SABBucket->isPointInside (RB)) {
    //SAB is valid
    return true;
  }

  //3)Test on first-layer neighbors.

  //3-1)Initialization: Setup the validationQueue for a spiral search
  //    Get a new SearchID (_nCurrentSearchID). No initialization of flags needed.
  //    the smaller id means it's never been searched.
  getNewCircleSearchID();

  BucketQueue validationQueue;
  SABBucket->_bVisitedValidation = _CircleSearchID; //SABBucket itself is visited now.

  BucketList::iterator snit = SABBucket->neighborList()->begin();
  for (; snit!=SABBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.
    //3-1)Ignore the empty neighbor...WRONG
    ///if (!curBucket->_bNonEmpty)
    ///  continue;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
                        (*snit)->Right(), (*snit)->Bottom()) ) {
      //3-2)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = (*snit)->BElmList()->begin();
      for (; sbit!=(*snit)->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
          return false;
        break;
        case BARC:
          if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
          return false;
        break;
        default: break;
        }//end switch
      }//end for

      //3-3)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //**vcl_cout<<"GAid: "<<GAid<<", GBid: "<<GBid<<", validationQueue size: "<<validationQueue.size()<<vcl_endl;

  //4)Test on the validationQueue in a spiral way...
  //  If anyone invalidates it, done. Else, run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!validationQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = validationQueue.front();
    validationQueue.pop();

    //4-2)If already visited, ignore.
    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
                        curBucket->Right(), curBucket->Bottom()) ) {
      //4-3)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = curBucket->BElmList()->begin();
      for (; sbit!=curBucket->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GPA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidatePXSource ((BLine_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidatePXSource ((BArc_Bucketing*)(*sbit), GPA, footPt, midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //4-4)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while validationQueue

  //4-5)Pass all invalidation, the source SAB is valid
  return true;
}

bool Bucketing::LAwithinCircleValidation (BLine_Bucketing* GLA, BArc_Bucketing* GAB, Point footPt, Point arcPt,
                            Point midPoint, double midDistSq, Bucket* SABBucket)
{
  Point GA = footPt;
  Point GB = arcPt;
  double curDistSq;

  //1)First try the points in the bucket that contains midPoint
  //  If anyone of them invalidate SAB, SAB is not valid.
  BElmVectorType::iterator sbit = SABBucket->BElmList()->begin();
  for (; sbit!=SABBucket->BElmList()->end(); ++sbit) {
    //1)GA or GB can not invalidate their own shock...
    if (GLA==(*sbit) || GAB==(*sbit))
      continue;
    switch ((*sbit)->type()) {
    case BPOINT:
      curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return false;
    break;
    case BLINE:
      if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
        return false;
    break;
    case BARC:
      if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
        return false;
    break;
    default: break;
    }
  }

  //2)Test if the circle is completely inside the bucket.
  //  If yes, done. SAB is valid.
  //Determine bounding box of the circle: L, T, R, B
  Point PM = midPoint;
  double r = hypot (GB.y-GA.y, GB.x-GA.x);
  double L = PM.x - r;
  double T = PM.y - r;
  double R = PM.x + r;
  double B = PM.y + r;
  Point LT (L, T);
  Point RT (R, T);
  Point LB (L, B);
  Point RB (R, B);
  //test if 4 corner points are all inside
  if (SABBucket->isPointInside (LT) &&
     SABBucket->isPointInside (RT) &&
     SABBucket->isPointInside (LB) &&
     SABBucket->isPointInside (RB)) {
    //SAB is valid
    return true;
  }

  //3)Test on first-layer neighbors.

  //3-1)Initialization: Setup the validationQueue for a spiral search
  //    Get a new SearchID (_nCurrentSearchID). No initialization of flags needed.
  //    the smaller id means it's never been searched.
  getNewCircleSearchID();

  BucketQueue validationQueue;
  SABBucket->_bVisitedValidation = _CircleSearchID; //SABBucket itself is visited now.

  BucketList::iterator snit = SABBucket->neighborList()->begin();
  for (; snit!=SABBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.
    //3-1)Ignore the empty neighbor...WRONG
    ///if (!curBucket->_bNonEmpty)
    ///  continue;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
                        (*snit)->Right(), (*snit)->Bottom()) ) {
      //3-2)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = (*snit)->BElmList()->begin();
      for (; sbit!=(*snit)->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GLA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
          return false;
        break;
        case BARC:
          if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
          return false;
        break;
        default: break;
        }//end switch
      }//end for

      //3-3)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //**vcl_cout<<"GAid: "<<GAid<<", GBid: "<<GBid<<", validationQueue size: "<<validationQueue.size()<<vcl_endl;

  //4)Test on the validationQueue in a spiral way...
  //  If anyone invalidates it, done. Else, run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!validationQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = validationQueue.front();
    validationQueue.pop();

    //4-2)If already visited, ignore.
    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
                        curBucket->Right(), curBucket->Bottom()) ) {
      //4-3)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = curBucket->BElmList()->begin();
      for (; sbit!=curBucket->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GLA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //4-4)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while validationQueue

  //4-5)Pass all invalidation, the source SAB is valid
  return true;
}

bool Bucketing::AAwithinCircleValidation (BArc_Bucketing* GAA, BArc_Bucketing* GAB, Point arcPtA, Point arcPtB,
                            Point midPoint, double midDistSq, Bucket* SABBucket)
{
  Point GA = arcPtA;
  Point GB = arcPtB;
  double curDistSq;

  //1)First try the points in the bucket that contains midPoint
  //  If anyone of them invalidate SAB, SAB is not valid.
  BElmVectorType::iterator sbit = SABBucket->BElmList()->begin();
  for (; sbit!=SABBucket->BElmList()->end(); ++sbit) {
    //1)GA or GB can not invalidate their own shock...
    if (GAA==(*sbit) || GAB==(*sbit))
      continue;
    switch ((*sbit)->type()) {
    case BPOINT:
      curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
      if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
        return false;
    break;
    case BLINE:
      if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
        return false;
    break;
    case BARC:
      if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
        return false;
    break;
    default: break;
    }
  }

  //2)Test if the circle is completely inside the bucket.
  //  If yes, done. SAB is valid.
  //Determine bounding box of the circle: L, T, R, B
  Point PM = midPoint;
  double r = hypot (GB.y-GA.y, GB.x-GA.x);
  double L = PM.x - r;
  double T = PM.y - r;
  double R = PM.x + r;
  double B = PM.y + r;
  Point LT (L, T);
  Point RT (R, T);
  Point LB (L, B);
  Point RB (R, B);
  //test if 4 corner points are all inside
  if (SABBucket->isPointInside (LT) &&
     SABBucket->isPointInside (RT) &&
     SABBucket->isPointInside (LB) &&
     SABBucket->isPointInside (RB)) {
    //SAB is valid
    return true;
  }

  //3)Test on first-layer neighbors.

  //3-1)Initialization: Setup the validationQueue for a spiral search
  //    Get a new SearchID (_nCurrentSearchID). No initialization of flags needed.
  //    the smaller id means it's never been searched.
  getNewCircleSearchID();

  BucketQueue validationQueue;
  SABBucket->_bVisitedValidation = _CircleSearchID; //SABBucket itself is visited now.

  BucketList::iterator snit = SABBucket->neighborList()->begin();
  for (; snit!=SABBucket->neighborList()->end(); ++snit) {
    Bucket* curBucket = *snit;
    curBucket->_bVisitedValidation = _CircleSearchID; //(*snit) is visited now.
    //3-1)Ignore the empty neighbor...WRONG
    ///if (!curBucket->_bNonEmpty)
    ///  continue;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, (*snit)->Left(), (*snit)->Top(),
                        (*snit)->Right(), (*snit)->Bottom()) ) {
      //3-2)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = (*snit)->BElmList()->begin();
      for (; sbit!=(*snit)->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GAA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          //1-1)POINT, test for isLEq
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
          return false;
        break;
        case BARC:
          if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
          return false;
        break;
        default: break;
        }//end switch
      }//end for

      //3-3)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add (*snit)'s neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = (*snit)->neighborList()->begin();
      for (; snnit!=(*snit)->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end for (*snit)

  //**vcl_cout<<"GAid: "<<GAid<<", GBid: "<<GBid<<", validationQueue size: "<<validationQueue.size()<<vcl_endl;

  //4)Test on the validationQueue in a spiral way...
  //  If anyone invalidates it, done. Else, run until the queue is empty.
  //  The buckets in the validationQueue should be unvisited 2nd-layer and higher-layer neighbors.
  while (!validationQueue.empty()) {
    //4-1)Deque the current and set it to visited.
    Bucket* curBucket = validationQueue.front();
    validationQueue.pop();

    //4-2)If already visited, ignore.
    if (curBucket->_bVisitedValidation == _CircleSearchID)
      continue;
     #ifdef _VIS_DEBUG
    assert (curBucket->_bVisitedValidation<_CircleSearchID);
    #endif
    curBucket->_bVisitedValidation = _CircleSearchID;

    //test if the two recvcl_tangle intersects or not.
    if ( _isTwoRectsIntersecting (L, T, R, B, curBucket->Left(), curBucket->Top(),
                        curBucket->Right(), curBucket->Bottom()) ) {
      //4-3)Run the same doesBElmInvalidateSAB() test for each elm, if anyone invalidates it, done.
      BElmVectorType::iterator sbit = curBucket->BElmList()->begin();
      for (; sbit!=curBucket->BElmList()->end(); ++sbit) {
        //1)GA or GB can not invalidate their own shock...
        if (GAA==(*sbit) || GAB==(*sbit))
          continue;
        switch ((*sbit)->type()) {
        case BPOINT:
          curDistSq = _distSqPointPoint (((BPoint_Bucketing*)(*sbit))->pt(), midPoint);
          if (_isLEq(curDistSq, midDistSq, DIST_BOUND))
            return false;
        break;
        case BLINE:
          if (doesBLineInvalidateAXSource ((BLine_Bucketing*)(*sbit), midPoint, midDistSq))
            return false;
        break;
        case BARC:
          if (doesBArcInvalidateAXSource ((BArc_Bucketing*)(*sbit), midPoint, midDistSq))
            return false;
        break;
        default: break;
        }//end switch
      }//end for

      //4-4)If no one invalidates it, no conclusion yet. Has to go to other buckets...
      //    Add curBucket's neighbor to the queue, again, for a spiral search
      //    Only add unvisited one into the queue.
      BucketList::iterator snnit = curBucket->neighborList()->begin();
      for (; snnit!=curBucket->neighborList()->end(); ++snnit) {
        if ((*snnit)->_bVisitedValidation<_CircleSearchID) //&& (*snnit)->_bNonEmpty
          validationQueue.push (*snnit);
      }
    }
    //if no intersecting, the iteration stop, go to the next one...
  }//end while validationQueue

  //4-5)Pass all invalidation, the source SAB is valid
  return true;
}
