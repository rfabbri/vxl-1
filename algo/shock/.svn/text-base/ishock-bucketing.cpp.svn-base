// ISHOCK.CPP

#include <extrautils/msgout.h>
#include "ishock-common.h"
#include "ishock.h"
#include "bucket.h"

IShock_Bucketing::IShock_Bucketing (Boundary* bnd, SHOCK_ALGO_TYPE newAlgoType, bool isReal) :
  IShock (bnd, newAlgoType, isReal)
{
}

IShock_Bucketing::~IShock_Bucketing ()
{
}

void IShock_Bucketing::InitializeShocks (SHOCK_INIT_TYPE InitType)
{
  if (InitType == DEFAULT_INIT)
    InitType = _ShockInitType;

  //A)BUCKETING_INIT: Re-initialize the bucketing structure.
  //  Recompute the visibilityConstraints.
  //  Recompute all sources.
  if (InitType==BUCKETING_INIT) {
    if (boundary()->nBElement()==0)
      return;

    //1)
    boundary()->clearVisibilityConstraints ();
    if (boundary()->bucketing)
      delete boundary()->bucketing;

    vcl_cout<< "Bucketing:\n";
    vcl_cout<< "Total BElements= " << boundary()->nBElement() <<vcl_endl;
    vcl_cout<< "nElmsPerBucket= " << 12 <<vcl_endl;

    //2)
    boundary()->bucketing = new Bucketing (&boundary()->BElmList, 12, boundary(), this,
                              0, 0, BND_WORLD_SIZE, BND_WORLD_SIZE);
    boundary()->bucketing->initShocksBucketing ();
    //Do preprocesvcl_sing inside Bucketing.
    boundary()->_bIsPreprocesvcl_singNeeded = false;

    //3)Go through contactShockList and initialize them here:
    ValidContactShockType::iterator vcit = boundary()->bucketing->_validContactShockList.begin();
    for (; vcit!=boundary()->bucketing->_validContactShockList.end(); vcit++) {
      initializeAContactShock ((*vcit)->_lbelm, (*vcit)->_rbelm, InitType);
    }
    boundary()->bucketing->_validContactShockList.clear();

    //4)Go through validShourceList and initialize sources here:
    ValidSourceType::iterator vit = boundary()->bucketing->_validSourceList.begin();
    for (; vit!=boundary()->bucketing->_validSourceList.end(); vit++) {
      SISource* newSource = initializeASource ((*vit)->_belm1, (*vit)->_belm2, 
                                  (*vit)->_startpt, (*vit)->_time);
      //Add newSource to this bucket's source element list.
      (*vit)->_bucket->SIElmList()->insert (newSource);
    }
    boundary()->bucketing->_validSourceList.clear();
  }
  else if (InitType==BUCKETING_ADD_INIT) {
    BElmListType* ListA = &(boundary()->taintedBElmList);
    //1-1)Add elements in ListA to buckets, including necessary preprocesvcl_sing.
    //1-2)Init sources, add visibility constraints
    //1-3)validate existing sources
    assert (boundary()->bucketing);
    boundary()->bucketing->initShocksNewElmsBucketing (ListA);
    boundary()->_bIsPreprocesvcl_singNeeded = false;
  
    //3)Go through contactShockList and initialize them here:
    //  Avoid duplication is done in LPShockInit() and APShockInit() part!
    ValidContactShockType::iterator vcit = boundary()->bucketing->_validContactShockList.begin();
    for (; vcit!=boundary()->bucketing->_validContactShockList.end(); vcit++) {
      initializeAContactShock ((*vcit)->_lbelm, (*vcit)->_rbelm, InitType);
    }
    boundary()->bucketing->_validContactShockList.clear();

    //4)Go through validSourceList and initialize sources here:
    ValidSourceType::iterator vit = boundary()->bucketing->_validSourceList.begin();
    for (; vit!=boundary()->bucketing->_validSourceList.end(); vit++) {
      //Avoid duplication!!
      //For P-P, check if belm1->_id <= belm2->_id, ignore the new source.
      if ((*vit)->_belm1->type()==BPOINT && (*vit)->_belm2->type()==BPOINT)
        if ((*vit)->_belm1->id() <= (*vit)->_belm2->id())
          continue;

      SISource* newSource = initializeASource ((*vit)->_belm1, (*vit)->_belm2, 
                                  (*vit)->_startpt, (*vit)->_time);
      //Add newSource to this bucket's source element list.
      (*vit)->_bucket->SIElmList()->insert (newSource);
    }
    boundary()->bucketing->_validSourceList.clear();

    //5)Go through invalidSourceList and delete sources here:
    //  !!!Note that a source is possibly invalidated by many belms!!!
    //  _invalidSourceList is a set.
    SIElmSetType::iterator ivit = boundary()->bucketing->_invalidSourceList.begin();
    for (; ivit!=boundary()->bucketing->_invalidSourceList.end(); ivit++) {
      //SIElement* sielm = (SIElement*)(*ivit);
      //assert (sielm->id()>0);
      delASIElement (*ivit);
    }
    boundary()->bucketing->_invalidSourceList.clear();

  }
  else if (InitType==DELETE_PATCH_INIT)  {
    //ASSUME NO CHANGE AT BUCKET STRUCTURE.
    //After this, some bucket might be empty!!
    IShock::InitializeShocks (InitType);
  }

  //clear the taintedBElementList now that shocks have been reinitialized from them
  boundary()->taintedBElmList.clear();
}

bool IShock_Bucketing::DetectShocks (SHOCK_INIT_TYPE InitType, SHOCK_ALGO_TYPE AlgoType)
{
  bool bSuccess;

  if (InitType == DEFAULT_INIT)
    InitType = _ShockInitType;
  if (AlgoType == DEFAULT_ALGO)
    AlgoType = _ShockAlgoType;

  //first initialize the sources
  if (nSElement()==0 || !_bComputeShockLocally)
    InitializeShocks (InitType);
  else
    InitializeShocks (BUCKETING_ADD_INIT);

  //now propagate the shocks
  switch (AlgoType) {
  case LAGRANGIAN:
    bSuccess = PropagateShocks ();
    break;
  case DYN_VAL:
    //boundary()->clearAllIVS ();
    bSuccess = PropagateShocks ();
    //_ishockview->UpdateBoundary ();
    break;
  case DYN_VAL_NEW:
    bSuccess = PropagateShocks ();
    break;
  case PROPAGATION:
    break;
  }

  return bSuccess;
}
