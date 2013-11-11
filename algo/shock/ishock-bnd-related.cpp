#include "ishock-common.h"
#include "ishock.h"

IShock::IShock (Boundary* bnd, SHOCK_ALGO_TYPE newAlgoType, bool bIsReal)
{
  _bIsRealIShock = bIsReal;
   _nextAvailableID = 0;
   _boundary = bnd;
  bnd->shock = this;
  _ShockAlgoType = newAlgoType;
  _ShockInitType = BOGUS_INIT;
  _bComputeShockLocally = false;
  dCurrentTime = 0;
  _nSourceElement = 0;

  DELTA_SAMPLE = 1;//0.5
  GD_step_count=0;
  
  SIElmList.clear();
  update_list.clear ();
  GDList.clear();

  VisFragsList.clear();
  SIEdgesList.clear();

  _nextShockToPropagate = NULL;
}

IShock::~IShock ()
{
  ClearVisualFragments();
  ClearShockGroups();

  if (_bIsRealIShock) {
    delTheSIElementList ();
  }

   if (_boundary !=NULL)
      _boundary->shock =NULL;

  //currently to avoid memory leak we need to delete the
  //boundary when the shock is deleted because only the shock
  //class has been declared with a smart pointer
  //In the future smarter things have to be done to make the contour-axis
  //class as one

  delete _boundary;
}

void IShock::Clear ()
{
  ClearVisualFragments();
  ClearShockGroups();

  delTheSIElementList();
  _nextAvailableID=0;
  _nSourceElement=0;
  //COUT<< "Ishock::clear(): Existing shocks cleared!" <<endl;
  //MSGOUT (0);
}

void IShock::delTheSIElementList ()
{
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* curSElm = (SIElement*)(curS->second);
    delete curSElm;
  }
  //clean out the list which is by now just pointers to the 
  //deleted items on the list
  SIElmList.clear();

  //also clear all the other Lists
  update_list.clear();
  GDList.clear();
}

//insert new element into the time ordered shock list
void IShock::addASIElement (SIElement* elm)
{
  // add element to the shock list
  SIElmList.insert(
    SIElmPair( A_R_IDpair( elm->isActive(), 
      R_IDpair(elm->simTime(), elm->id()) ), elm ));

  computeSymTransSalience(elm);

  //add element to the GD List if it is a non contact shock link
  if (elm->graph_type()==SIElement::LINK &&
     elm->label() != SIElement::CONTACT)
    GDList.insert(
      Sal_SIElmPair(Sal_IDpair(elm->dPnCost(), elm->id()), elm));
  
  //add to the shock update_list
  update_list.insert(ID_SIElm_pair(elm->id(), elm));
}

void IShock::delASIElement (SIElement* elm)
{
  //Now you also need to update connectivity information
  //You can't do it from the destructors of the elements
  //because the shock list needs to be updated too

  if (!elm) {
    //This happens when we delete a A3, the child is NULL. assert (0);
    return;
  }
  assert (elm->id()>0);

  if (elm->graph_type() == SIElement::LINK){
    //just get rid of them, there're a nuisance
    //get rid of all the children too
    //BUT THIS IS GONNA MAKE THIS FUNCTION RECURSIVE!!!!
    SILink* curLink = (SILink*)elm; 
    if ( curLink->cSNode())
      delASIElement(curLink->cSNode());
  }
  else {
    //go through the list of parents of this node
    //by this time only the shocks that belong to neighboring
    //boundaries are remaining

    //what about the children?
    //the children can't exist without these nodes so get rid of them too!

    SILinksList::iterator curS;
    switch(elm->type()){
      case SIElement::A3SOURCE:
        curS = ((SIA3Source*)elm)->PSElementList.begin();
        for(; curS!=((SIA3Source*)elm)->PSElementList.end(); ++curS)
          reactivateASIElement (*curS); //very imporvcl_tant!

        //remove the child
        delASIElement(((SIA3Source*)elm)->cSLink());
        break;
      case SIElement::SOURCE:
        //remove the children
        delASIElement(((SISource*)elm)->cSLink());
        delASIElement(((SISource*)elm)->cSLink2());
        break;
      case SIElement::JUNCT:
        curS = ((SIJunct*)elm)->PSElementList.begin();
        for(; curS!=((SIJunct*)elm)->PSElementList.end(); ++curS)
          reactivateASIElement (*curS); //very imporvcl_tant!

        //remove the child
        delASIElement(((SIJunct*)elm)->cSLink());
        break;
      case SIElement::SINK:
        curS = ((SISink*)elm)->PSElementList.begin();
        for(; curS!=((SISink*)elm)->PSElementList.end(); ++curS)
          reactivateASIElement(*curS); //very imporvcl_tant!
    }
  }

  //Now remove it from the shock list and delete it
  //remove it from the GD list first if it is a SILink
  if (elm->graph_type()==SIElement::LINK &&
     elm->label() != SIElement::CONTACT)
    GDList.erase(Sal_IDpair(elm->dPnCost(), elm->id()));

  //remove it from the update list
  update_list.erase(elm->id());
  
  SIElmList.erase(A_R_IDpair(elm->isActive(), 
                    R_IDpair(elm->simTime(),elm->id()) ));

  delete elm; //delete the actual object
}

void IShock::deactivateASIElement (SIElement* elm)
{
  //First remove the shock element
  SIElmList.erase(A_R_IDpair(elm->isActive(), 
    R_IDpair(elm->simTime(),elm->id())) ); 

  //update the shock's bActive status
  elm->setActive (false);

  //then reinsert it into the appropriate place
  SIElmList.insert(SIElmPair(A_R_IDpair(elm->isActive(), 
                    R_IDpair(elm->simTime(),elm->id())), elm));
}

void IShock::reactivateASIElement (SIElement* elm)
{
  //reactivate this element for propagation
  //should involve moving the shock element to its start time
  //and resetting all the other variables

  //only the shock links will be reactivated
  assert(elm->graph_type()==SIElement::LINK);
  SILink* curLink = (SILink*)elm;

  //First remove the shock element
  SIElmList.erase(A_R_IDpair(curLink->isActive(), 
    R_IDpair(curLink->simTime(),curLink->id())) ); 

  //update the shock's bActive status
  curLink->setActive (true);
  curLink->set_cSNode(NULL);
  curLink->clear_lNeighbor();
  curLink->clear_rNeighbor();
  curLink->setPropagated(false);
  curLink->setSimTime(curLink->startTime());
  curLink->setEndTime(ISHOCK_DIST_HUGE);

  //then reinsert it into the appropriate place
  SIElmList.insert(SIElmPair(A_R_IDpair(curLink->isActive(), 
                    R_IDpair(curLink->simTime(),curLink->id())), curLink));

}
