#include <extrautils/msgout.h>
#include "ishock.h"

#include <vcl_utility.h>
#include <vcl_iostream.h>

// This file contains all the basic functions to traverse 
// the shock graph like a directed or undirected graph

//////////////////////////////////////////////////////////////
// GRAPH OPERATIONS
// Date: sometime in the past
// Amir
//
// These functions have been inspired by LEDA
// and have been designed for quick porting into such a system
// as and when it becomes feasible

// Modifications:
// Nhon Jul 28 2004 revise cyclic_adj_succ, cyclic_adj_pred
//                  added  getLeftBElementFromEdge     
//      
//////////////////////////////////////////////////////////////

// Access operations

int IShock::number_of_nodes()
{
  // returns the number of nodes in IShock. 
  return 0;
}

int IShock::number_of_edges()
{ 
  // returns the number of edges in IShock.
  return 0;
}

int IShock::outdeg(SINode* v, bool doNotincludehidden)
{
  // returns the number of edges adjacent to node v (| adj_edges(v)|).
  return v->outdeg(doNotincludehidden);
}

int IShock::indeg(SINode* v, bool doNotincludehidden)
{
  //returns the number of edges ending at v (| in_edges(v)|)
  return v->indeg(doNotincludehidden);
}

int IShock::degree(SINode* v, bool doNotincludehidden)
{
  //returns outdeg(v) + indeg(v).
  return v->degree(doNotincludehidden);
}

SINode* IShock::source(SILink* e)
{
  //returns the source node of edge e. 
  return e->pSNode();
}

SINode* IShock::dest(SILink* e)
{
  //returns the dest node of edge e. 
  return e->cSNode();
}

SINode* IShock::opposite(SINode* v, SILink* e)
{
  // returns dest(e) if v = source(e) and 
  // source(e) if v = dest(e)
  // if either of these are NULL it returns itself
  // because the assumption is that we are turning around 
  // a semi infinite edge of the graph
  SINode* oppositeNode;

  if (v==e->pSNode())
    oppositeNode = e->cSNode();
  else
    oppositeNode = e->pSNode();

  if (!oppositeNode)
    return v;
  else
    return oppositeNode;
}

SINodesList IShock::all_nodes()
{
  // returns the list V of all nodes of the graph
  SINodesList nodesList;
  return nodesList;
}

SILinksList IShock::all_edges()
{
  // returns the list E of all edges of the graph
  SILinksList linksList;
  return linksList;
}

SILinksList IShock::adj_edges(SINode* v)
{
  // returns adj_edges(v). 
  SILinksList linksList;
  return linksList;
}

SILinksList IShock::out_edges(SINode* v)
{
  // returns adj_edges(v) if IShock is directed and the empty list otherwise.
  SILinksList linksList;
  return linksList;
}

SILinksList IShock::in_edges(SINode* v)
{
  // returns in_edges(v) if IShock is directed and the empty list otherwise.
  SILinksList linksList;
  return linksList;
}

SINodesList IShock::adj_nodes(SINode* v)
{
  // returns the list of all nodes adjacent to v. 
  SINodesList nodesList;
  return nodesList;
}

// graph operations assuming that this is a directed graph

SILink* IShock::first_adj_edge(SINode* v)
{
  // returns the first edge in the adjacency list of v (nil if this list is empty). 
  return NULL;
}

SILink* IShock::last_adj_edge(SINode* v)
{
  // returns the last edge in the adjacency list of v (nil if this list is empty). 
  return NULL;
}

SILink* IShock::adj_succ(SILink* e)
{
  // returns the successor of edge e in the adjacency list of node source(e) (nil if it does not exist). 
  return NULL;
}

SILink* IShock::adj_pred(SILink* e)
{
  // returns the predecessor of edge e in the adjacency list of node source(e) (nil if it does not exist). 
  return NULL;
}

SILink* IShock::cyclic_adj_succ(SILink* e)
{
  // returns the cyclic successor of edge e in the adjacency list of node source(e). 
  return NULL;
}

SILink* IShock::cyclic_adj_pred(SILink* e)
{
  // returns the cyclic predecessor of edge e in the adjacency list of node source(e). 
  return NULL;
}

SILink* IShock::first_in_edge(SINode* v)
{
  // returns the first edge of in_edges(v) (nil if this list is empty). 
  return NULL;
}

SILink* IShock::last_in_edge(SINode* v)
{
  // returns the last edge of in_edges(v) (nil if this list is empty). 
  return NULL;
}

SILink* IShock::in_succ(SILink* e)
{
  // returns the successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  return NULL;
}

SILink* IShock::in_pred(SILink* e)
{
  // returns the predecessor of edge e in in_edges(target(e)) (nil if it does not exist).

  return NULL;
}

SILink* IShock::cyclic_in_succ(SILink* e)
{
  // returns the cyclic successor of edge e in in_edges(target(e)) (nil if it does not exist). 
  return NULL;
}

SILink* IShock::cyclic_in_pred(SILink* e)
{
  // returns the cyclic predecessor of edge e in in_edges(target(e)) (nil if it does not exist). 
  return NULL;
}

////////////////////////////////////////////////////////
// Operations for undirected graphs 
////////////////////////////////////////////////////////

SILink* IShock::adj_succ(SILink* e, SINode* v)
{
  // returns the successor of edge e in the adjacency list of v.
  //Precondition: e is incident to v. 
  return NULL;
}

SILink* IShock::adj_pred(SILink* e, SINode* v)
{
  // returns the predecessor of edge e in the adjacency list of v.
  //Precondition: e is incident to v.
  return NULL;

}
 
SILink* IShock::cyclic_adj_succ(SILink* e, SINode* v, bool include_hidden_SILinks)
{
  // returns the cyclic successor(CW) of edge e in the adjacency list of v.
  // Precondition: e is incident to v. 

  // if this is a leaf edge, it should return itself

  if (v->isASource()){
    //assume that a source node cannot exist with just one child
    if (e == ((SISource*)v)->cSLink())
      return ((SISource*)v)->cSLink2();
    else
      return ((SISource*)v)->cSLink();
  }
  else if (v->isAnA3Source()){
    //an A3 is always a leaf edge
    return e;
  }
  else {
    //junctions and sinks can have multiple parents 
    //start looking for the current edge starting from child
    //any one of the edges incident on these nodes can be hidden

    bool bEdgeFound = false;

    SILink* cSLink = NULL;
    if (v->isAJunction())
      if ( ((SIJunct*)v)->cSLink())
        if ( ((SIJunct*)v)->cSLink()->isNotHidden() || include_hidden_SILinks )
          cSLink = ((SIJunct*)v)->cSLink();

    if (e == cSLink)
      bEdgeFound = true;

    //now go through the list of parents
    SILinksList::iterator curS = v->PSElementList.begin();
    for(; curS!=v->PSElementList.end(); curS++){
      SILink* curSLink = (*curS);
      if (curSLink->isHidden() && (!include_hidden_SILinks)) curSLink = NULL;

      if (bEdgeFound && curSLink)
        return curSLink;

      if (e == curSLink)
        bEdgeFound = true;
    }

    if (bEdgeFound){ //this should always be true by now
      if (cSLink)
        return cSLink;
      else {
        //need to go around the parents one more time
        SILinksList::iterator curS = v->PSElementList.begin();
        for(; curS!=v->PSElementList.end(); ++curS){
          SILink* curSLink = (*curS);
          if (curSLink->isNotHidden() || include_hidden_SILinks) 
            return curSLink;
        }
      }
    }
  }
  
  //it should never get to here
  return NULL;
} 


SILink* IShock::cyclic_adj_pred(SILink* e, SINode* v, bool include_hidden_SILinks)
{
  // returns the cyclic predecessor(CW) of edge e in the adjacency list of v.
  //Precondition: e is incident to v. 

  // if this is a leaf edge, it should return itself

  if (v->isASource()){
    //assume that a source node cannot exist with just one child
    if (e == ((SISource*)v)->cSLink())
      return ((SISource*)v)->cSLink2();
    else
      return ((SISource*)v)->cSLink();
  }
  else if (v->isAnA3Source()){
    //an A3 is always a leaf edge
    return e;
  }
  else {
    //junctions and vcl_sinks can have multiple parents 
    //start looking for the current edge starting from child
    //any one of the edges incident on these nodes can be hidden

    bool bEdgeFound = false;

    SILink* cSLink = NULL;
    if (v->isAJunction())
      if ( ((SIJunct*)v)->cSLink())
        if ( ((SIJunct*)v)->cSLink()->isNotHidden() || include_hidden_SILinks)
          cSLink = ((SIJunct*)v)->cSLink();

    if (e == cSLink)
      bEdgeFound = true;

    //now go through the list of parents
    SILinksList::reverse_iterator curS = v->PSElementList.rbegin();
    for(; curS!=v->PSElementList.rend(); curS++){
      SILink* curSLink = (*curS);
      if (curSLink->isHidden() && (! include_hidden_SILinks)) curSLink = NULL;

      if (bEdgeFound && curSLink)
        return curSLink;

      if (e == curSLink)
        bEdgeFound = true;
    }

    if (bEdgeFound){ //this should always be true by now
      if (cSLink)
        return cSLink;
      else {
        //need to go around the parents one more time
        SILinksList::reverse_iterator curS = v->PSElementList.rbegin();
        for(; curS!=v->PSElementList.rend(); ++curS){
          SILink* curSLink = (*curS);
          if (curSLink->isNotHidden() || include_hidden_SILinks) 
            return curSLink;
        }
      }
    }
  }
  
  //it should never get to here
  return NULL;
}

// ---------------------------------------------------
// unfinished// return the belements in the "back" of v, 
// forward direction is the edge e.

// ---------------------------------------------------

BElementList 
IShock::node_projection_cone(SILink* e, SINode *v){
  BElementList  boundary_cone;

  return boundary_cone;

}





int IShock::countUnprunedParentsOfANode(SINode* node)
{
  return node->countUnprunedParents();
}

SILink* IShock::UnprunedParentOfAJunction (SIJunct* junct)
{
  return junct->UnprunedParent();
}

SILink* IShock::UnprunedParentOfANode (SINode* node)
{
  //note that this function only returns one parent link
  //assuming that the others have been pruned away already

  return node->UnprunedParent();
}

SINode* IShock::traceToNextNode(SINode* cur_node, SILink** Llink)
{
  switch (cur_node->type()) {
    case SIElement::A3SOURCE:
      return traceToNextNode( ((SIA3Source*)cur_node)->cSLink(), Llink );
      break;
    case SIElement::JUNCT:
      return traceToNextNode( ((SIJunct*)cur_node)->cSLink() , Llink);
      break;
    default : return NULL;
  }
}

SINode* IShock::traceToNextNode(SILink* Flink, SILink** Llink)
{
  SILink* cur_link = Flink;
  SILink* last_link;
  SINode* next_node;

  while (cur_link){
    last_link = cur_link;
    cur_link = cur_link->GetChildLink ();
  }
  next_node = last_link->cSNode();  
  *Llink = last_link;
  return next_node;
}

SINode* IShock::traceBackToPreviousNode( SILink* link)
{
  SILink* cur_link = link;
  SILink* last_link;
  SINode* prev_node;

  while (cur_link){
    last_link = cur_link;
    cur_link = cur_link->GetParentLink();
  }
  prev_node = last_link->pSNode();
  return prev_node;
}

BElement* IShock::getRightBElementFromEdge(SILink* curEdge, SINode* curNode)
{
  //return the right belement corresponding to this edge

  //vcl_since this is a directed graph,
  //the right belement it not always the right element
  //it depends on the way we are travervcl_sing the graph

  //also mark the current edge as traversed
  if (curNode == source(curEdge)){
    if (curEdge->getTraversedState() == NOT_TRAVERSED)
      curEdge->setTraversedState(TRAVERSED_FORWARD);
    else if (curEdge->getTraversedState() == TRAVERSED_BACKWARD)
      curEdge->setTraversedState(TRAVERSED_BOTHWAYS);

    return curEdge->rBElement();
  }
  else {
    if (curEdge->getTraversedState() == NOT_TRAVERSED)
      curEdge->setTraversedState(TRAVERSED_BACKWARD);
    else if (curEdge->getTraversedState() == TRAVERSED_FORWARD)
      curEdge->setTraversedState(TRAVERSED_BOTHWAYS);

    return curEdge->lBElement();
  }
}

// Nhon. added Jul 28 2004

BElement* IShock::getLeftBElementFromEdge(SILink* curEdge, SINode* curNode){
  //return the left belement corresponding to this edge

  //vcl_since this is a directed graph,
  //the left belement it not always the left belement in the shock structure
  //it depends on the way we are traversing the graph

  //also mark the current edge as traversed
  if (curNode == source(curEdge)){
    if (curEdge->getTraversedState() == NOT_TRAVERSED)
      curEdge->setTraversedState(TRAVERSED_FORWARD);
    else if (curEdge->getTraversedState() == TRAVERSED_BACKWARD)
      curEdge->setTraversedState(TRAVERSED_BOTHWAYS);

    return curEdge->lBElement();
  }
  else {
    if (curEdge->getTraversedState() == NOT_TRAVERSED)
      curEdge->setTraversedState(TRAVERSED_BACKWARD);
    else if (curEdge->getTraversedState() == TRAVERSED_FORWARD)
      curEdge->setTraversedState(TRAVERSED_BOTHWAYS);

    return curEdge->rBElement();
  }  
}


BContourList IShock::CCWEulerTourAroundALoop(SILink* e)
{
  BContourList contourList;
  contourList.clear();

  SILink* startEdge = e;
  SINode* startNode = source(e);

  //form contours by travervcl_sing around a loop
  //form an initial contour
  BContour* curCon = new BContour(boundary()->nextAvailableID());
  contourList.push_back(curCon);
  vcl_cout <<"New contour"<<vcl_endl;

  //start the tour from the dest node of this edge
  SINode* curNode = startNode;
  SILink* curEdge = startEdge;
  
  bool bStartOfLoop = true;
  bool bStartOfContour = true;

  //tour until you get back to the startnode and startEdge
  while (!(curEdge==startEdge && curNode==startNode && !bStartOfLoop)){
    
    BElement* lastBElm;
    int semi_degenerate_count;

    if (bStartOfLoop){
      bStartOfLoop = false;
      lastBElm = NULL;
      semi_degenerate_count=0;
    }

    //In this implementation, curNode is always the node before the 
    //curEdge in terms of the direction of traversal

    //***********************************************************
    // This portion contains the logic usingthe Belements
    // derived from an Euler Tour and making them into Contours
    //***********************************************************

    //Allow repititions at the beginning and at the end of contours
    
    BElement* curBElm = getRightBElementFromEdge(curEdge, curNode);
    vcl_cout << curBElm->id() << " ";

    //do not put the element that is already in the list
    if (curBElm == lastBElm){
      // If it is not the beginning of the contour
      //increment semi-degenerate hypothesis count
      if (!bStartOfContour)
        semi_degenerate_count ++;
    }
    else {
      //this element is different than the last element
      //so this is no longer the beginning of the contour
      bStartOfContour = false;

      //if the repetition count was ISHOCK_DIST_HUGE, start a new contour
      if (semi_degenerate_count>12){
        curCon = new BContour(boundary()->nextAvailableID());
        contourList.push_back(curCon);
        bStartOfContour = true;
        vcl_cout << vcl_endl << "New contour: [ID:"<<curCon->id()<<"]"<<vcl_endl;
      }

      //reset the count
      semi_degenerate_count = 0;
      
      //group this element into the current contour
      curCon->elms.push_back(curBElm);
      vcl_cout <<"["<<curBElm->id()<<"] ";
    }

    //special treatment of semi-infinite edges
    if (curEdge->isSemiInfinite()){
      //need to start a new contour
      curCon = new BContour(boundary()->nextAvailableID());
      contourList.push_back(curCon);
      bStartOfContour = true;
      vcl_cout << vcl_endl << "New contour: [ID:"<<curCon->id()<<"]"<<vcl_endl;

      //reset the count
      semi_degenerate_count = 0;

      //is this always true?
      curBElm = curEdge->lBElement();
      vcl_cout << "{"<< curBElm->id() << "} ";

      //add this element to the current contour
      curCon->elms.push_back(curBElm);
      vcl_cout <<"["<<curBElm->id()<<"] ";
    }

    lastBElm = curBElm;

    //***********************************************************

    //go to the next node
    curNode = opposite(curNode, curEdge);

    //go to the next edge around the tour
    curEdge = cyclic_adj_succ(curEdge, curNode);

  }

  vcl_cout << vcl_endl << vcl_endl;

  //put these contours on to the boundary() contour list
  BContourListIterator curConPtr = contourList.begin();
  for (; curConPtr != contourList.end(); curConPtr++)
    boundary()->contourList.push_back(*curConPtr);

  return contourList;
}

BElementList IShock::CCWFollowABranch(SILink* e)
{
  BElementList belmList;

  SILink* startEdge = e;
  SINode* startNode = source(e);

  //start the tour from the dest node of this edge
  SINode* curNode = startNode;
  SILink* curEdge = startEdge;
  
  bool bEndOfBranch = false;

  //tour until you get back to the startnode and startEdge
  while (!bEndOfBranch){
    
    //put the right belement into the list
    //the right belement it not always the right element
    //it depends on the way we are travervcl_sing the graph
    if (curNode == source(curEdge)){
      //don't push an element in twice
      vcl_cout << curEdge->rBElement()->id() << " ";
      if (belmList.size()>1){
        if (belmList.back()->id() != curEdge->rBElement()->id())
          belmList.push_back(curEdge->rBElement());
      }
      else
        belmList.push_back(curEdge->rBElement());
    }
    else {
      vcl_cout << curEdge->lBElement()->id() << " ";

      //don't push an element in twice
      if (belmList.size()>1){
        if (belmList.back()->id() != curEdge->lBElement()->id())
          belmList.push_back(curEdge->lBElement());
      }
      else
        belmList.push_back(curEdge->lBElement());
    }

    //go to the next node
    curNode = opposite(curNode, curEdge);

    //check to see if this is a node of order 2
    if (degree(curNode)==2){
      //go to the adjoining edge
      curEdge = cyclic_adj_succ(curEdge, curNode);
    }
    else
      bEndOfBranch = true; 
  }

  return belmList;
}


// --------------------------------------------------------------------
// Return left boundary tour when tracing CW between two (node,link)'s
// the tour begins at (sn, se) and ends at (dn, de)
// sn is the source of se in the traversal direction
// dn is the source of de in the traversal direction
// sn    se                          de     dn
// x-----------x ... ---> ... x-------------x 

// by Nhon Trinh
// Date: Jul 28 2004
// ------------------------------------------------------------------

BElementList 
IShock::CCWTraceContourBetween(SINode* sn, SILink* se, SINode* dn, SILink* de )
{
  BElementList belm_list;
  
  SINode* start_node = sn;
  SILink* start_link = se;
  SINode* end_node = dn;
  SILink* end_link = de;
  
  //start the tour from the source node of this edge
  SINode* cur_node = start_node;
  SILink* cur_link = start_link;

  //tour until you get back to the end_link and end_node
  // If we get back to start_node and start_link before hitting (end_node, end_link)
  // we terminate the loop and return empty list.

  BElement* last_belm = getRightBElementFromEdge(start_link, start_node);
  belm_list.push_back(last_belm);
  bool is_tour_end = false;
  while (!is_tour_end){
    //go to the next node and next edge
    cur_node = opposite(cur_node, cur_link);
    cur_link = cyclic_adj_succ(cur_link, cur_node, true);

    //put the right belement (traversing direction, not shock's direction)
    //into the list. 
    BElement* cur_belm = getRightBElementFromEdge(cur_link, cur_node);
    if (last_belm != cur_belm){
      belm_list.push_back(cur_belm);
      last_belm = cur_belm;
    }

    // end the loop when we hit the destination node and link
    if ( (cur_node == end_node) && (cur_link == end_link)){
      is_tour_end = true;
      continue;
    }

    // return empty list if we go back to the starting point
    // this happens when the two SILinks (start and end) belong to two disconnected graphs
    if ((cur_node == start_node)&& (cur_link == start_link)){
      belm_list.clear();
      return belm_list;
    }  
  }
  
  // remove duplicating belements between beginning and end of the loop (if there is any)
  BElement* first_belm = belm_list.front();
  for ( BElementList::reverse_iterator belm_rit = belm_list.rbegin();
    belm_rit != belm_list.rend(); belm_rit ++)
  {
    if ((*belm_rit) == first_belm)
      belm_list.pop_back();
    else
      break;
  }
  return belm_list;
}


void IShock::CCWEulerTourToGroupIntoContours()

{

  //**********************************************
  //Perform a complete Euler Tour of the shock graph
  //and form contours

  ClearTraversedStates();
  IdentifyEndPoints();
  
  //Go through the shock list and tour around each loop
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* current = curS->second;

    //look for edges that haven't been traversed or have been traversed only
    //in one direction

    if (!current->isALink())
      continue;

    if (current->getTraversedState() == TRAVERSED_BOTHWAYS ||
       current->isHidden())
      continue;

    //form contours by travervcl_sing around a loop
    //first form the contour as a belementlist only
    //later chop it up into contours
    SILink* startEdge = (SILink*)current;
    SINode* startNode;

    if (startEdge->getTraversedState() == NOT_TRAVERSED ||
       startEdge->getTraversedState() == TRAVERSED_BACKWARD){
      startNode = source(startEdge);
    }
    else {
      startNode = dest(startEdge);
    }

    //start the tour from the dest node of this edge
    SINode* curNode = startNode;
    SILink* curEdge = startEdge;
    
    BElementList belmlist;
    belmlist.clear();

    BElement* lastBElm = 0;

    //tour until you get back to the startnode and startEdge
    while (1){
      //In this implementation, curNode is always the node before the 
      //curEdge in terms of the direction of traversal

      //***********************************************************
      // This portion contains the logic usingthe Belements
      // derived from an Euler Tour and making them into Contours
      //***********************************************************

      BElement* curBElm = getRightBElementFromEdge(curEdge, curNode);
      //vcl_cout << curBElm->id() << " ";

      //do not put the element that is already in the list
      if (curBElm != lastBElm){
        //group this element into the current contour
        belmlist.push_back(curBElm);
        //vcl_cout <<"["<<curBElm->id()<<"] ";
      }

      //special treatment of semi-infinite edges
      if (curEdge->isSemiInfinite()){
        curBElm = curEdge->lBElement();
        belmlist.push_back(curBElm);
        //set flag to traversed_bothways
        curEdge->setTraversedState(TRAVERSED_BOTHWAYS);
      }

      lastBElm = curBElm;

      //***********************************************************

      //go to the next node
      curNode = opposite(curNode, curEdge);

      //check to see if we are the starting point around the loop
      if (curNode == startNode)
        break;

      //also check to see if this is an pruned node
      //if it is then we have to do a virtual tree traversal over the 
      //hidden branches of the shocks
      //if (degree(curNode) != ){
        //follow around a hidden tree
      //}

      //go to the next edge around the tour
      curEdge = cyclic_adj_succ(curEdge, curNode);

    }

    //Now that the entire shock loop has been traversed
    //we need to chop it up into contours
    BContour* curCon = NULL;

    bool bStartIsNotStart = ((BPoint*)(*(belmlist.begin())))->_bSomething;
    bool contourFound = false;

    //determine if this is a closed contour
    //if it is we don;t need to break it up
    //if it isn't a closed contour, we need to break it up at the endpoints

    BElementListIterator curB = belmlist.begin();
    for (; curB != belmlist.end(); curB++){
      BElement* curbelm = (*curB);

      //find the endpoints to start the contour
      if ( !((BPoint*)curbelm)->_bSomething){
        contourFound = true;
        //if a contour already exists attach it to the end of it
        if (curCon)
          curCon->elms.push_back(curbelm);

        curCon = new BContour(boundary()->nextAvailableID());
        boundary()->contourList.push_back(curCon);
        //vcl_cout << vcl_endl << "New contour: [ID:"<<curCon->id()<<"]"<<vcl_endl;

        //add this element to the contour
        curCon->elms.push_back(curbelm);
      }
      else {
        if (curCon){
          //add this element to the contour
          curCon->elms.push_back(curbelm);
        }
      }
    }

    if (bStartIsNotStart){
      //add the beginning portion of the contour to the current contour
      for (curB = belmlist.begin(); curB != belmlist.end(); curB++){
        //find the first endpoint to start the contour
        if ( !((BPoint*)*curB)->_bSomething){
          curCon->elms.push_back(*curB);
          break;
        }
      }
    }

    if (!contourFound){
      //its a simple closed contour
      curCon = new BContour(boundary()->nextAvailableID());
      boundary()->contourList.push_back(curCon);
      //vcl_cout << vcl_endl << "New contour: [ID:"<<curCon->id()<<"]"<<vcl_endl;

      for (curB = belmlist.begin(); curB != belmlist.end(); curB++)
        curCon->elms.push_back(*curB);
    }

    //vcl_cout << vcl_endl << vcl_endl;  
  }

  BContourList BConsToDel;

  BContourListIterator curC = boundary()->contourList.begin();
  for (; curC != boundary()->contourList.end(); curC++){
    BContour* curCon = (*curC);
    if (curCon->elms.size()==2){
      if ( ((BPoint*)(curCon->elms.front()))->_bSomething == false &&
          ((BPoint*)(curCon->elms.back()))->_bSomething == false)
          BConsToDel.push_back(curCon);
    }
  }

  //delete them
  for (curC = BConsToDel.begin(); curC != BConsToDel.end(); curC++)
    boundary()->contourList.remove(*curC);

}

/*
void IShock::FormShockGroups()
{
  SIElement* current;
  SILink* first_link;

  // *************************************************************
  //clear traversed states in all the elements first
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    current = curS->second;
    current->setTraversedState(NOT_TRAVERSED);
  }
  // *************************************************************

  curS = SIElmList.begin();
  for (; curS != SIElmList.end(); curS++) {
    current = (SIElement*)(curS->second);

    if (current->isANode()) continue;
    if (current->isHidden()) continue;
    if (current->getTraversedState() == TRAVERSED_FORWARD)
      continue;

    //only the remaining untraversed edges should get to here
    first_link = (SILink*)current;
    FormDirectedShockGroups(first_link);
  }
}
*/

void IShock::ClearShockGroups()
{
  //delete each shock edge in the list
  SIEdgeListIterator curEdge = SIEdgesList.begin();
  for (; curEdge != SIEdgesList.end(); curEdge++)
    delete curEdge->second;

  //then clear the list
  SIEdgesList.clear();
}

void IShock::IdentifyEndPoints()
{
  // go through the list of boundary() elements and 
  // check for connectivity information
  // mark all the vcl_singly connected elements: these are the
  // end points of the contours

  BElmListIterator curB = boundary()->BElmList.begin();
  for (; curB!=boundary()->BElmList.end(); curB++) {
    BElement* curBElm = (BElement*)(curB->second);

    //connected elements are defined to be the ones with 
    //pruned 2nd order shocks between them

    int connectivity = 0;
    BElement::shockListIterator curSS = curBElm->shockList.begin();
    for (; curSS != curBElm->shockList.end(); ++curSS) {
      SIElement* curSElm = (*curSS);
      if (curSElm->isASource() && curSElm->isHidden())
        connectivity++;
    }

    //assume that all the belements are points for now
    if (connectivity < 2){
      ((BPoint*)curBElm)->_bSomething = false;
      boundary()->update_list.insert(ID_BElm_pair(curBElm->id(), curBElm));
    }
    else {
      if (((BPoint*)curBElm)->_bSomething == false){
        ((BPoint*)curBElm)->_bSomething = true;
        boundary()->update_list.insert(ID_BElm_pair(curBElm->id(), curBElm));
      }
    }
  }
}

void IShock::ClearTraversedStates()
{
  //*************************************************************
  //clear traversed states in all the elements first
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    SIElement* current = curS->second;
    current->setTraversedState(NOT_TRAVERSED);
  }
  //*************************************************************
}  

void IShock::FormShockGroups(bool DTbased)
{
  SIElement* current;
  SINode* cur_node;

  //First identify the end points of the contours to mark them
  //This is only needed for point data
  //IdentifyEndPoints();

  //Clear the traversed states of the shocks
  ClearTraversedStates();

  //*************************************************************
  //make a list of all "valid" nodes
  //"valid" defined as degree 3 or higher nodes
  // And some Source nodes with "the special property"
  vcl_cout << "Valid Nodes" << vcl_endl;
  SINodeList validNodeList;
  SIElmListIterator curS = SIElmList.begin();
  for (; curS!=SIElmList.end(); curS++){
    current = curS->second;
    if (!current->isANode()) continue;
    //if (current->isASink()) continue; //only the nodes that have children
    if (current->isHidden()) continue;

    //only unhidden nodes remain now
    //if a source only keep the special ones
    if (current->isASource()){
      SISource* curSrc = (SISource*)current;
      validNodeList.push_back((SINode*)current);
      //vcl_cout << current->id() << " ";

      /*
      //look into this login for points to rewrite it in a general fashion
      if (current->dPnCost() != ISHOCK_DIST_HUGE || 
         !((BPoint*)curSrc->getBElement1())->_bSomething ||
         !((BPoint*)curSrc->getBElement2())->_bSomething){
        validNodeList.push_back((SINode*)current);
        vcl_cout << current->id() << " ";
      }
      */
    }
    else {
      //we don't want the degree 2 nodes
      //keep the degree 1 nodes, these are the leaf nodes
      //but not all of them are guarenteed to end at a vcl_sink
      int degree = ((SINode*)current)->degree();

      if ( degree != 2){
        validNodeList.push_back((SINode*)current);
        vcl_cout << current->id() << " ";
      }
    }
  }
  vcl_cout << vcl_endl;
  //*************************************************************
  
  //follow all the links from these nodes to next valid nodes
  //to form the shock groups and contours
  SINodeListIterator curNode = validNodeList.begin();
  for (; curNode != validNodeList.end(); curNode++) {
    cur_node = *curNode;

    SILink* first_link;
    //get the first link in this group
    if (cur_node->isASource()){
      //first group
      first_link = ((SISource*)cur_node)->cSLink();
      if (first_link->getTraversedState() == NOT_TRAVERSED)
        FormUnDirectedShockGroups(cur_node, first_link);
      //second group
      first_link = ((SISource*)cur_node)->cSLink2();
      if (first_link->getTraversedState() == NOT_TRAVERSED)
        FormUnDirectedShockGroups(cur_node, first_link);
    }

    if (cur_node->isAnA3Source()){
      first_link = ((SIA3Source*)cur_node)->cSLink();
      if (first_link->getTraversedState() == NOT_TRAVERSED)
        FormUnDirectedShockGroups(cur_node, first_link);
    }

    if (cur_node->isAJunction()){
      first_link = ((SIJunct*)cur_node)->cSLink();
      if (first_link->getTraversedState() == NOT_TRAVERSED)
        FormUnDirectedShockGroups(cur_node, first_link);

      //parents
      int nparents = ((SIJunct*)cur_node)->countUnprunedParents();
      SILinksList::iterator curL = ((SIJunct*)cur_node)->PSElementList.begin();
      for(; curL!=((SIJunct*)cur_node)->PSElementList.end(); ++curL) {
        SILink* parentLink = (*curL);
        if (parentLink->isHidden() || parentLink->isAContact()) continue;
        if (parentLink->getTraversedState() == NOT_TRAVERSED)
          FormUnDirectedShockGroups(cur_node, parentLink);
      }
    }  

    if (cur_node->isASink()){
      //parents
      int nparents = ((SIJunct*)cur_node)->countUnprunedParents();
      SILinksList::iterator curL = ((SIJunct*)cur_node)->PSElementList.begin();
      for(; curL!=((SIJunct*)cur_node)->PSElementList.end(); ++curL) {
        SILink* parentLink = (*curL);
        if (parentLink->isHidden() || parentLink->isAContact()) continue;
        if (parentLink->getTraversedState() == NOT_TRAVERSED)
          FormUnDirectedShockGroups(cur_node, parentLink);
      }
    }

    if (DTbased && cur_node->degree()>2){
      //for DT faces we need to make faces from these nodes as well

      BContour* lCon = new BContour(boundary()->nextAvailableID());
      boundary()->contourList.push_back(lCon);
      BContour* rCon = new BContour(boundary()->nextAvailableID());
      boundary()->contourList.push_back(rCon);

      BElementListIterator curB = cur_node->bndList.begin();
      for (;curB!=cur_node->bndList.end(); curB++)
        lCon->elms.push_back(*curB);

      SILinksList shockLinks;
      shockLinks.push_back((SILink*)cur_node);

      SIEdge* curEdge = new SIEdge(nextAvailableID(), SIElement::REGULAR,
        lCon, rCon, NULL, NULL, shockLinks);

      //for a shock Edge for this shock group
      SIEdgesList.insert(vcl_pair<int, SIEdge*>(curEdge->id(), curEdge));
    }
  }
}

void IShock::FormDirectedShockGroups(SILink* first_link)
{
  //now follow this link all the way to the next "real" node
  //form contours in the process

  BContour* lCon = new BContour(boundary()->nextAvailableID());
  boundary()->contourList.push_back(lCon);
  BContour* rCon = new BContour(boundary()->nextAvailableID());
  boundary()->contourList.push_back(rCon);

  //keep a ordered list of the shock links in the group
  SILinksList shockLinks;

  SILink* cur_link = first_link;
  SILink* last_link;
  while (cur_link){
    //add the current shock link to the link list for this edge
    shockLinks.push_back(cur_link);

    BElement* lBElm = cur_link->lBElement();
    BElement* rBElm = cur_link->rBElement();

    //add these elements to the current contours
    if (lCon->elms.size()>0){
      if (lCon->elms.back()->id() != lBElm->id()){
        lCon->elms.push_back(lBElm);
        //vcl_cout <<"["<<lBElm->id()<<"] ";
      }
    }
    else {
      lCon->elms.push_back(lBElm);
      //vcl_cout <<"["<<lBElm->id()<<"] ";
    }

    if (rCon->elms.size()>0){
      if (rCon->elms.front()->id() != rBElm->id()){
        rCon->elms.push_front(rBElm);
        //vcl_cout <<"["<<rBElm->id()<<"] ";
      }
    }
    else {
      rCon->elms.push_front(rBElm);
      //vcl_cout <<"["<<rBElm->id()<<"] ";
    }

    //set flag to traversed_forward
    cur_link->setTraversedState(TRAVERSED_FORWARD);

    last_link = cur_link;
    cur_link = cur_link->GetChildLink();
  }

  SIEdge* curEdge = new SIEdge(nextAvailableID(), SIElement::REGULAR,
      lCon, rCon, first_link->pSNode(), last_link->cSNode(), shockLinks);

  //for a shock Edge for this shock group
  SIEdgesList.insert(vcl_pair<int, SIEdge*>(curEdge->id(), curEdge));

  //vcl_cout << vcl_endl << vcl_endl;

}

void IShock::FormUnDirectedShockGroups(SINode* first_node, SILink* first_link)
{
  //get the ID for this shock group
  int EdgeID = nextAvailableID();

  BContour* lCon = new BContour(boundary()->nextAvailableID());
  boundary()->contourList.push_back(lCon);
  BContour* rCon = new BContour(boundary()->nextAvailableID());
  boundary()->contourList.push_back(rCon);

  //vcl_cout <<"New Contour Fragment: " << lCon->id() <<vcl_endl;

  //keep a ordered list of the shock links in the group
  SILinksList shockLinks;

  //start the tour from the dest node of this edge
  SINode* curNode = first_node;
  SILink* curLink = first_link;
  SILink* last_link = NULL;
  SINode* last_node = NULL;
  
  bool bSemiInfinite = false;

  //now follow this link all the way to the next "real" node
  //form contours in the process
  while (1){
    last_link = curLink;

    //add the current shock link to the link list for this edge
    if (!curLink->isSemiInfinite())
      shockLinks.push_back(curLink);

    BElement* lBElm = curLink->lBElement();
    BElement* rBElm = curLink->rBElement();

    //add these elements to the current contours
    //the right belement it not always the right element
    //it depends on the way we are travervcl_sing the graph
    if (curNode == source(curLink)){

      if (lCon->elms.size()>0){
        if (lCon->elms.back()->id() != lBElm->id()){
          lCon->elms.push_back(lBElm);
        }
      }
      else {
        lCon->elms.push_back(lBElm);
      }

      if (rCon->elms.size()>0){
        if (rCon->elms.front()->id() != rBElm->id()){
          rCon->elms.push_front(rBElm);
        }
      }
      else {
        rCon->elms.push_front(rBElm);
      }
    }
    else {

      if (lCon->elms.size()>0){
        if (lCon->elms.back()->id() != rBElm->id()){
          lCon->elms.push_back(rBElm);
        }
      }
      else {
        lCon->elms.push_back(rBElm);
      }

      if (rCon->elms.size()>0){
        if (rCon->elms.front()->id() != lBElm->id()){
          rCon->elms.push_front(lBElm);
        }
      }
      else {
        rCon->elms.push_front(lBElm);
      }
    }

    //set flag to traversed_forward
    curLink->setTraversedState(TRAVERSED_FORWARD);
    curLink->setEdgeID(EdgeID);

    if (curLink->isSemiInfinite()){
      bSemiInfinite = true;
      break;
    }
      
    //go to the next node
    curNode = opposite(curNode, curLink);

    last_node = curNode;

    //check to see if this is a node of order 2
    if (degree(curNode)==2){
      //if a special source end the traversal
      if (curNode->isASource()){
        SISource* curSrc = (SISource*)curNode;

        /* redo this login for the general case this is trying to catch
        if (curSrc->dPnCost() != ISHOCK_DIST_HUGE ||
           !((BPoint*)curSrc->getBElement1())->_bSomething ||
           !((BPoint*)curSrc->getBElement2())->_bSomething
          )
          break;
        else
        //go to the adjoining edge
        curLink = cyclic_adj_succ(curLink, curNode);
        */
        break;
      }
      else
        //go to the adjoining edge
        curLink = cyclic_adj_succ(curLink, curNode);
    }
    else
      break; 

    if (!curLink) break;
  }

  
  SIEdge* curEdge = new SIEdge(EdgeID, SIElement::REGULAR,
      lCon, rCon, first_node, last_node, shockLinks);

  //for a shock Edge for this shock group
  SIEdgesList.insert(vcl_pair<int, SIEdge*>(curEdge->id(), curEdge));
  
}

inline Point P2D2P(Point2D<double> pt)
{
  return Point(pt.x(), pt.y());
}

void IShock::FormVisualFragments()
{
  //go through the list of SIEdges and form visual fragments
  //from them. Currently these are just polygons and the circular sectors 
  //at the leaf nodes are misvcl_sing
  //display each visual Fragment in the list
  //int n=0;

  SIEdgeListIterator curEdge = SIEdgesList.begin();
  for (; curEdge != SIEdgesList.end(); curEdge++){
    SIEdge* cur_edge =  curEdge->second;
  
    //temp: DO NOT SHOW ANY FRAGMENTS THAT GO TO INFINITY
    if (!cur_edge->pSNode() || !cur_edge->cSNode())
      continue;

    /*
    //insvcl_tantiate two new Visual Fragment from this edge
    VisFrag* curFrag = new VisFrag(nextAvailableID(), cur_edge);
    VisFragsList.push_back(curFrag);

    //vcl_cout <<"CurEdgeID: " << cur_edge->id()<< " VISFRAG L" <<vcl_endl;

    Point fpt, lpt;
    
    //for test only some psnodes might not exists (e.g. for deg 3 nodes)
    if (cur_edge->pSNode())
      fpt = cur_edge->pSNode()->origin();

    //csNode might not always exists
    if (cur_edge->cSNode())
      lpt = cur_edge->cSNode()->origin();
    else
      lpt = cur_edge->lastLink()->getStartPt();//this should always work!!

    //vcl_cout << "first point: (" << fpt.x <<", " << fpt.y <<")"<<vcl_endl; 
    //vcl_cout << "last point: (" << lpt.x <<", " << lpt.y <<")"<<vcl_endl;

    //last point
    curFrag->push_back( Point (fpt.x, fpt.y) );
    //vcl_cout << "spt: (" << fpt.x <<", " <<fpt.y <<") "<< vcl_endl;

    //go along left contour
    BElementListIterator curB =  cur_edge->lContour()->elms.begin();
    for (; curB != cur_edge->lContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "cpt: (" << bp->pt().x <<", " <<bp->pt().y <<")"<<vcl_endl;
    }

    //last point
    curFrag->push_back( Point (lpt.x, lpt.y) );
    //vcl_cout << "spt: (" << lpt.x <<", " <<lpt.y <<") "<< vcl_endl;

    SINode* curNode= cur_edge->cSNode();
    SILink* curLink = cur_edge->lastLink();

    while (1){
      //go to the adjoining edge
      curNode = opposite(curNode, curLink);
      //stupid loop here for the sake of stupid polygon forming code
      if (curNode == cur_edge->pSNode()) break;
      curFrag->push_back( Point ( curNode->origin().x, curNode->origin().y));
      //vcl_cout << "spt: (" << curNode->origin().x <<", " <<curNode->origin().y <<") "<< vcl_endl;
      curLink = cyclic_adj_succ(curLink, curNode);
    }

    //Now define a second visual fragment for the right side of the shock
    curFrag = new VisFrag(nextAvailableID(), cur_edge);
    VisFragsList.push_back(curFrag);

    //vcl_cout <<"CurEdgeID: " << cur_edge->id()<< " VISFRAG R" <<vcl_endl;

    //last point
    curFrag->push_back( Point (lpt.x, lpt.y) );
    //vcl_cout << "spt: (" << lpt.x <<", " <<lpt.y <<") "<< vcl_endl;

    //go along right contour
    curB =  cur_edge->rContour()->elms.begin();
    for (; curB != cur_edge->rContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "cpt: (" << bp->pt().x <<", " <<bp->pt().y <<")"<<vcl_endl;
    }

    //first point
    curFrag->push_back( Point (fpt.x, fpt.y) );
    //vcl_cout << "spt: (" << fpt.x <<", " <<fpt.y <<") "<< vcl_endl;

    //now traverse along the shock edge for the final side of the visual fragment
    curNode= cur_edge->pSNode();
    curLink = cur_edge->startLink();

    while (1){
      //go to the adjoining edge
      curNode = opposite(curNode, curLink);
      //stupid loop here for the sake of stupid polygon forming code
      if (curNode == cur_edge->cSNode()) break;
      curFrag->push_back( Point ( curNode->origin().x, curNode->origin().y));
      //vcl_cout << "spt: (" << curNode->origin().x <<", " <<curNode->origin().y <<") "<< vcl_endl;
      curLink = cyclic_adj_succ(curLink, curNode);
    }
    */
    
    //insvcl_tantiate a new Visual Fragment from this edge
    VisFrag* curFrag = new VisFrag(nextAvailableID(), cur_edge);
    VisFragsList.push_back(curFrag);

    Point fpt, lpt;
    
    //for test only some psnodes might not exists (e.g. for deg 3 nodes)
    if (cur_edge->pSNode())
      fpt = cur_edge->pSNode()->origin();

    //csNode might not always exists
    if (cur_edge->cSNode())
      lpt = cur_edge->cSNode()->origin();
    else
      lpt = cur_edge->lastLink()->getStartPt();//this should always work!!

    //vcl_cout << "first point: (" << fpt.x <<", " << fpt.y <<")"<<vcl_endl; 
    //vcl_cout << "last point: (" << lpt.x <<", " << lpt.y <<")"<<vcl_endl;

    //first point

    //if the first point is a pruned junction(Now an A3)
    if (cur_edge->pSNode()->degree() == 1 &&
      !cur_edge->pSNode()->isAnA3Source()){

      //traverse around the bump to make a bigger fragment

      //project to compute starting point
      Point cur_pt = cur_edge->startLink()->getRFootPt(cur_edge->startLink()->sTau());
      curFrag->push_back( cur_pt);

      double R = cur_edge->startLink()->startTime();
      double startAngle = vcl_atan2(cur_pt.y - fpt.y, cur_pt.x - fpt.x);

      cur_pt = cur_edge->startLink()->getLFootPt(cur_edge->startLink()->sTau());
      double endAngle = vcl_atan2(cur_pt.y - fpt.y, cur_pt.x - fpt.x);

      double dA = CCW(endAngle, startAngle);
      //four pieces
      for (int i=1; i<5; i++){
        cur_pt = _vectorPoint(fpt, startAngle - i*dA/4, R);
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
            curFrag->push_back( cur_pt);
      }
/*
       this is difficult to do because if the rBELement is a point it is ambiguous
      BElement* curBElm =  cur_edge->startLink()->rBElement();
      if (curBElm->type() == BPOINT)
        curBElm = 
      while (1){
        cur_pt = curBElm->s_pt()->pt();
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
          curFrag->push_back( cur_pt);
        
        if (curBElm->s_pt() == cur_edge->startLink()->lBElement())
          break;

        curBElm = curBElm->s_pt()->getElmToTheLeftOf(curBElm);

        if (curBElm == cur_edge->startLink()->lBElement())
          break;
      }
*/      
      
      
    }
    else {
      curFrag->push_back( Point (fpt.x, fpt.y) );
      //vcl_cout << "fpt: (" << fpt.x <<", " <<fpt.y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }
    
    //go along left contour
    //get the projections of the endpoints of the shocks
    SINode* curNode = cur_edge->pSNode();
    SILinksList::iterator curS = cur_edge->links()->begin();
    for(; curS!=cur_edge->links()->end(); ++curS){
      Point cur_pt;
      if (curNode == source(*curS)){
        //going forward now
        cur_pt = (*curS)->getLFootPt((*curS)->sTau());
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
          curFrag->push_back( cur_pt);

        curNode = dest(*curS);

        if ((*curS) == cur_edge->lastLink()){
          //include the endpoint projection too
          cur_pt = (*curS)->getLFootPt((*curS)->eTau());
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
            curFrag->push_back( cur_pt);
        }
      }
      else {
        //going backward now
        cur_pt = (*curS)->getRFootPt((*curS)->eTau());
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
          curFrag->push_back( cur_pt);
        curNode = source(*curS);

        if ((*curS) == cur_edge->lastLink()){
          //include the endpoint projection too
          cur_pt = (*curS)->getRFootPt((*curS)->sTau());
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
            curFrag->push_back( cur_pt);
        }
      }
    }
/*
    //this will only work for points
    //for others we need to project the shock points to the boundary
    BElementListIterator curB =  cur_edge->lContour()->elms.begin();
    for (; curB != cur_edge->lContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "lcpt: (" << bp->pt().x <<", " <<bp->pt().y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }
*/
    //last point
    //if the first point is a pruned junction(Now an A3)
    if (cur_edge->cSNode()->degree() == 1 ){//&& cur_edge->cSNode()->isASink()

      //interpolate around the pruned chunk

      if (cur_edge->cSNode() == dest(cur_edge->lastLink())){
        //project to compute starting point
        Point cur_pt = cur_edge->lastLink()->getLFootPt(cur_edge->lastLink()->eTau());
        curFrag->push_back( cur_pt);

        double R = cur_edge->lastLink()->endTime();
        double startAngle = vcl_atan2(cur_pt.y - lpt.y, cur_pt.x - lpt.x);

        cur_pt = cur_edge->lastLink()->getRFootPt(cur_edge->lastLink()->eTau());
        double endAngle = vcl_atan2(cur_pt.y - lpt.y, cur_pt.x - lpt.x);

        double dA = CCW(endAngle, startAngle);
        //four pieces
        for (int i=1; i<5; i++){
          cur_pt = _vectorPoint(lpt, startAngle - i*dA/4, R);
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
              curFrag->push_back( cur_pt);
        }
      }
      else {
        //project to compute starting point
        Point cur_pt = cur_edge->lastLink()->getRFootPt(cur_edge->lastLink()->sTau());
        curFrag->push_back( cur_pt);

        double R = cur_edge->lastLink()->startTime();
        double startAngle = vcl_atan2(cur_pt.y - lpt.y, cur_pt.x - lpt.x);

        cur_pt = cur_edge->lastLink()->getLFootPt(cur_edge->lastLink()->sTau());
        double endAngle = vcl_atan2(cur_pt.y - lpt.y, cur_pt.x - lpt.x);

        double dA = CCW(endAngle, startAngle);
        //four pieces
        for (int i=1; i<5; i++){
          cur_pt = _vectorPoint(lpt, startAngle - i*dA/4, R);
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
              curFrag->push_back( cur_pt);
        }
      }
    }
    else {
      if (!_BisEqPoint(lpt, curFrag->poly()->back()))
        curFrag->push_back( Point (lpt.x, lpt.y) );
      //vcl_cout << "lpt: (" << lpt.x <<", " <<lpt.y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }

    //go along right contour
    //get the projections of the endpoints of the shocks
    curNode = cur_edge->cSNode();
    SILinksList::reverse_iterator curRS = cur_edge->links()->rbegin();
    for(; curRS!=cur_edge->links()->rend(); ++curRS){
      Point cur_pt;
      if (curNode == source(*curRS)){
        //going forward now
        cur_pt = (*curRS)->getLFootPt((*curRS)->sTau());
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
          curFrag->push_back( cur_pt);
        curNode = dest(*curRS);

        if ((*curRS) == cur_edge->startLink()){
          //include the endpoint projection too
          cur_pt = (*curRS)->getLFootPt((*curRS)->eTau());
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
            curFrag->push_back( cur_pt);
        }
      }
      else {
        //going backward now
        cur_pt = (*curRS)->getRFootPt((*curRS)->eTau());
        if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
          curFrag->push_back( cur_pt);
        curNode = source(*curRS);

        if ((*curRS) == cur_edge->startLink()){
          //include the endpoint projection too
          cur_pt = (*curRS)->getRFootPt((*curRS)->sTau());
          if (!_BisEqPoint(cur_pt, curFrag->poly()->back()))
            curFrag->push_back( cur_pt);
        }
      }
    }

    //check the very last point with the very first point to see if they are close
    if (_BisEqPoint(curFrag->poly()->front(), curFrag->poly()->back()))
      curFrag->poly()->pop_back();

/*
    //this will only work for points
    //for others we need to project the shock points to the boundary
    curB =  cur_edge->rContour()->elms.begin();
    for (; curB != cur_edge->rContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "lcpt: (" << bp->pt().x <<", " <<bp->pt().y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }
*/
  }
}

void IShock::FormDTFragments()
{
  //go through the list of SIEdges and form visual fragments
  //form them. Currently these are just polygons and the circular sectors 
  //at the leaf nodes are misvcl_sing
  //display each visual Fragment in the list
  
  SIEdgeListIterator curEdge = SIEdgesList.begin();
  for (; curEdge != SIEdgesList.end(); curEdge++){
    SIEdge* cur_edge =  curEdge->second;
  
    //insvcl_tantiate a new Visual Fragment from this edge
    VisFrag* curFrag = new VisFrag(nextAvailableID(), cur_edge);
    VisFragsList.push_back(curFrag);

    //go along left contour
    BElementListIterator curB =  cur_edge->lContour()->elms.begin();
    for (; curB != cur_edge->lContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "lcpt: (" << bp->pt().x <<", " <<bp->pt().y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }

    //go along right contour
    curB =  cur_edge->rContour()->elms.begin();
    for (; curB != cur_edge->rContour()->elms.end(); curB++){
      BPoint* bp = (BPoint*)(*curB);
      curFrag->push_back( Point ( bp->pt().x, bp->pt().y));
      //vcl_cout << "lcpt: (" << bp->pt().x <<", " <<bp->pt().y <<") Edge ID: "<< edge->id() <<vcl_endl;
    }
  }
}

void IShock::ClearVisualFragments()
{
  //delete each polygon in the list
  VisFragListIterator curFrag = VisFragsList.begin();
  for (; curFrag != VisFragsList.end(); curFrag++)
    delete *curFrag;

  //then clear the list
  VisFragsList.clear();
}
