// IShockXShock

// This file contains the code to sample the shocks
// and save as an extrinsic shock file (.ESF)
//
// This class is an intermediate between completely intrinsic shock 
// class and a completely extrinsic shock class

#include <vcl_cmath.h>
#include <vcl_vector.h>
//#include <stdio.h>

#include "ishock.h"
#include "IShockXShock.h"

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  Sample the shocks and output as .esf file
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

ExNode* IShockXShock::traceToNextNode(SINode* cur_node)
{
  switch (cur_node->type()) {
    case SIElement::A3SOURCE:
      return traceToNextNode( ((SIA3Source*)cur_node)->cSLink() );
      break;
    case SIElement::JUNCT:
      return traceToNextNode( ((SIJunct*)cur_node)->cSLink() );
      break;
    default:
      return NULL;
  }
}

ExNode* IShockXShock::traceToNextNode(SILink* link)
{
  SILink* cur_link = link;
  SILink* last_link;
  SINode* next_node;

  while (cur_link){
    last_link = cur_link;
    cur_link = cur_link->GetChildLink ();
  }
  next_node = last_link->cSNode();
  //now find ExNode corresponding to this _iNode
  return (SINodeToExNodeMapping.find(next_node->id()))->second;
}

ExNode* IShockXShock::traceBackToPreviousNode( SILink* link)
{
  SILink* cur_link = link;
  SILink* last_link;
  SINode* prev_node;

  while (cur_link){
    last_link = cur_link;
    cur_link = cur_link->GetParentLink();
  }
  prev_node = last_link->pSNode();
  //now find ExNode corresponding to this _iNode
  return (SINodeToExNodeMapping.find(prev_node->id()))->second;
}

void IShockXShock::sample_intrinsic_shocks(int option)
{
  int actual_option;

  //if the ESF export option is selection, then you need to output
  //both sides in the selection window
  if (option==ESF_SELECTION)
    actual_option = BOTHSIDE;
  else 
    actual_option = option;

   // 1. Mark inside/outside 
  SHK()->MarkInsideOutside();

  // 2. make a list of all the nodes and all the edges
  // 3. Setup the node graph structure
  SetupNodes(actual_option);
  SetupEdges(actual_option);

  // 4. sample all the edges
  SampleAllEdges(actual_option);
  UpdateSampleInformationInNodes();
}

void IShockXShock::write_esf_file (const char* filename)
{
  vcl_vector<const char *> miscInfo;
  miscInfo.push_back("Pruning Scale: 1.00");
  miscInfo.push_back("Other parameters used: Default");

  //save to file
  OutputXShockToFile(filename, miscInfo);
}

void IShockXShock::SetupNodes(int option)
{
  SIElement* current;
  SINode* cur_SINode;
  ExNode* cur_node;

  //first make a list of all valid nodes

  SIElmListIterator curSPtr = SHK()->SIElmList.begin();
  for (; curSPtr != SHK()->SIElmList.end(); curSPtr++) {
    current = (SIElement*)(curSPtr->second);

    if (!((option == (int)current->bIO) || (option == BOTHSIDE))) continue;
    if (!current->isANode()) continue;

    cur_SINode = (SINode*)current;

    //In order to accomodate the system where pruning without symmetry tranforms 
    //is allowed and also for future versions where shock nodes are not subclassed
    //but differ only by their connectivity, we shall use the connectivity information
    //to decide on the node properties

    if (cur_SINode->indeg()==0){
      //A3 or Source
      if (cur_SINode->outdeg()==1){
        //A3
        
        //use SINode->id() instead of getNextAvailableNodeID() for debug
        getNextAvailableNodeID();
        cur_node = new IShockExNode(cur_SINode->id(), 
          cur_SINode, ExNode::A3SOURCE, cur_SINode->bIO);
        ExNodeList.push_back(cur_node);
        SINodeToExNodeMapping.insert(SINodeExNodePair(cur_SINode->id(), cur_node));
        continue;
      }
      if (cur_SINode->outdeg()==2){
        //Source
        
        //use SINode->id() instead of getNextAvailableNodeID() for debug
        getNextAvailableNodeID();
        cur_node = new IShockExNode(cur_SINode->id(), 
          cur_SINode, ExNode::SOURCE, cur_SINode->bIO);
        ExNodeList.push_back(cur_node);
        SINodeToExNodeMapping.insert(SINodeExNodePair(cur_SINode->id(), cur_node));
        continue;
      }
    }
    else {
      //Junct or Sink
      
      //is it a pruned junction??? Ignore a pruned junction
      if (cur_SINode->indeg()>1 &&
         cur_SINode->outdeg()==1){
        //junction

        //use SINode->id() instead of getNextAvailableNodeID() for debug
        getNextAvailableNodeID();
        cur_node = new IShockExNode(cur_SINode->id(), 
          cur_SINode, ExNode::JUNCT, cur_SINode->bIO);
        ExNodeList.push_back(cur_node);
        SINodeToExNodeMapping.insert(SINodeExNodePair(cur_SINode->id(), cur_node));

        continue;
      }
      if (cur_SINode->outdeg()==0){
        //vcl_sink
        
        //use SINode->id() instead of getNextAvailableNodeID() for debug
        getNextAvailableNodeID();
        cur_node = new IShockExNode(cur_SINode->id(), 
          cur_SINode, ExNode::SINK, cur_SINode->bIO);
        ExNodeList.push_back(cur_node);
        SINodeToExNodeMapping.insert(SINodeExNodePair(cur_SINode->id(), cur_node));
        continue;
      }
    }
  }
}

void IShockXShock::SetupEdges(int option)
{
  // now put in the connectivity information on the nodes

  ExNode* end_node;
  ExEdge* cur_edge;
  SILink* first_link;
  int nparents;

  ExNodeListIterator curN = ExNodeList.begin();
  for (; curN!=ExNodeList.end(); curN++){
    IShockExNode* cur_node = (IShockExNode*)(*curN);
    switch (cur_node->_type) {
      case ExNode::A3SOURCE:
      {
        //first child
        end_node = traceToNextNode(cur_node->_iNode);
        first_link = cur_node->_iNode->cSLink();
        //use edgeID instead
        getNextAvailableEdgeID();
        cur_edge = new IShockExEdge(first_link->edgeID(), first_link, cur_node, end_node, cur_node->_bIO);

        //Add node connectivity and edge connectivity information to node list
        cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        ExEdgeList.push_back(cur_edge);

        break;
      }
      case ExNode::SOURCE:
      {
        //first child
        end_node = traceToNextNode(((SISource*)cur_node->_iNode)->cSLink());
        first_link = cur_node->_iNode->cSLink();
        //use edgeID instead
        getNextAvailableEdgeID();
        cur_edge = new IShockExEdge(first_link->edgeID(), first_link, cur_node, end_node, cur_node->_bIO);

        //Add node connectivity and edge connectivity information to node list
        cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        ExEdgeList.push_back(cur_edge);

        //second child
        end_node = traceToNextNode(((SISource*)cur_node->_iNode)->cSLink2());
        first_link = cur_node->_iNode->cSLink2();
        //use edgeID instead
        getNextAvailableEdgeID();
        cur_edge = new IShockExEdge(first_link->edgeID(), first_link, cur_node, end_node, cur_node->_bIO);

        //Add node connectivity and edge connectivity information to node list
        cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        ExEdgeList.push_back(cur_edge);

        break;
      }
      case ExNode::JUNCT:
      {
        //1)add child node first
        end_node = traceToNextNode(cur_node->_iNode);
        first_link = cur_node->_iNode->cSLink();
        //use edgeID instead
        getNextAvailableEdgeID();
        cur_edge = new IShockExEdge(first_link->edgeID(), first_link, cur_node, end_node, cur_node->_bIO);
        
        //Add node connectivity and edge connectivity information to node list
        cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        ExEdgeList.push_back(cur_edge);
        
        //2) Then add parent nodes
        nparents = cur_node->_iNode->countUnprunedParents();

        SILinksList::iterator curS = ((SIJunct*)cur_node->_iNode)->PSElementList.begin();
        for(; curS!=cur_node->_iNode->PSElementList.end(); ++curS) {
          SILink* parentLink = (*curS);
          if (parentLink->isAContact() || parentLink->isHidden()) 
            continue;

          end_node = traceBackToPreviousNode(parentLink);
          //now lookfor this edge in the parent nodes' list
          cur_edge = end_node->FindExEdgeToThisExNode(cur_node);
          cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        }
        break;
      }
      case ExNode::SINK:
      {
        // add all parent nodes
        nparents = cur_node->_iNode->countUnprunedParents();

        SILinksList::iterator curS = cur_node->_iNode->PSElementList.begin();
        for(; curS!=cur_node->_iNode->PSElementList.end(); ++curS) {
          SILink* parentLink = (*curS);
          if (parentLink->isAContact() || parentLink->isHidden()) 
            continue;

          end_node = traceBackToPreviousNode(parentLink);
          //now lookfor this edge in the parent nodes' list
          cur_edge = end_node->FindExEdgeToThisExNode(cur_node);
          cur_node->AddNodeAndEdgeToConnectedList(end_node, cur_edge);
        }
        break;
      }
    }
  }
}

void IShockXShock::SampleAllEdges(int option)
{
  ExSampleListVector *edgeSamples;

  ExEdgeListIterator curE = ExEdgeList.begin();
  for (; curE!=ExEdgeList.end(); curE++){
    IShockExEdge* cur_edge = (IShockExEdge*)(*curE);

    //get the edge sample vector to fill in the information
    edgeSamples = &cur_edge->_EdgeSamples;
    edgeSamples->clear();

    int sampleToUpdate = -1;
    //sample the edge here
    //need to traverse the edge components and sample them
    SILink* cur_link = cur_edge->_first_link;
    while (cur_link){
      switch (cur_link->type()){
        case SIElement::POINTPOINT:
          sampleShock((SIPointPoint*)cur_link, *edgeSamples);
          break;
        case SIElement::POINTLINE:
          sampleShock((SIPointLine*)cur_link, *edgeSamples);
          break;
        case SIElement::LINELINE:
          sampleShock((SILineLine*)cur_link, *edgeSamples);
          break;
        case SIElement::POINTARC:
          sampleShock((SIPointArc*)cur_link, *edgeSamples);
          break;
        case SIElement::LINEARC:
          sampleShock((SILineArc*)cur_link, *edgeSamples);
          break;
        case SIElement::ARCARC:
          sampleShock((SIArcArc*)cur_link, *edgeSamples);
          break;
        case SIElement::THIRDORDER:
          sampleShock((SIThirdOrder*)cur_link, *edgeSamples);
          break;
        case SIElement::ARCTHIRDORDER:
          sampleShock((SIArcThirdOrder*)cur_link, *edgeSamples);
          break;
      }

      //if this link followed a previos link mark the first sample as
      //a pruned junction sample
      if (sampleToUpdate>0){
        ExSample* first_sample = (* edgeSamples)[sampleToUpdate];
        first_sample->_type = ExSample::PRUNEDSAMPLE;
        sampleToUpdate = -1;
      }

      int prev_edge_id = cur_link->edgeID();
      cur_link = cur_link->GetChildLink();

      //if this is the same shock group, you need to 
      //label the sample as a pruned junction sample
      if (cur_link){
        cur_link->setEdgeID(prev_edge_id);
        edgeSamples->back()->_type = ExSample::PRUNEDSAMPLE;
        sampleToUpdate = edgeSamples->size();
      }
    }

  }
}

void IShockXShock::UpdateSampleInformationInNodes()
{
  // Go through the ExNode list again to add the 
  // END_SAMPLEs on each EDGE to ExNode's list.

  for (int i=0; i<ExNodeList.size(); i++){
    ExNode* cur_node = ExNodeList[i];

    //for each node upate the samples from the edges
    for (int j=0; j<cur_node->_connectedNodes.size(); j++){
      ExEdge* cur_edge = cur_node->_edges[j];
      
      if (cur_node == cur_edge->_fromNode)
        cur_node->_samples.push_back(cur_edge->_EdgeSamples.front());
      else {
        assert(cur_node == cur_edge->_toNode);
        cur_node->_samples.push_back(cur_edge->_EdgeSamples.back());
      }
    }
  }
}

//-------------------------------------------------------
//          SAMPLE SHOCKS
//========================================================

//1) Explicitly include the first and last points
//   usingbegin and end tau for every shock link
//2) Sample each point into ExSample class at this stage

void IShockXShock::sampleShock(SIPointPoint* spp, ExSampleListVector &edgeSampleList)
{
   double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spp->tauDir();
   tau = stau = spp->sTau();
   etau = spp->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt = P2P2D(spp->getPtFromLTau(tau));
    cur_sample->_t = spp->r(tau);
    cur_sample->_edge_id = spp->edgeID();
    cur_sample->_label = ExSample::DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    double angle = angle0To2Pi (spp->tangent(tau));
    cur_sample->_theta = angle*180/M_PI;
    cur_sample->_speed = spp->v(tau);
    cur_sample->_lBPoint = P2P2D(spp->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(spp->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx
    
    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample); 

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    next_tau = vcl_atan(2*ds/spp->H() + vcl_tan(tau));

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());
  cur_sample->_pt = P2P2D(spp->getPtFromLTau(etau));
  cur_sample->_t = spp->r(etau);
  cur_sample->_edge_id = spp->edgeID();
  cur_sample->_label = ExSample::DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = spp->tangent(etau)*180/M_PI;
  cur_sample->_speed = spp->v(etau);
  cur_sample->_lBPoint = P2P2D(spp->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(spp->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx
  
  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);
}

void IShockXShock::sampleShock(SIPointLine* spl, ExSampleListVector &edgeSampleList)
{
   double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spl->tauDir();
   tau = stau = spl->sTau();
   etau = spl->eTau();

  while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt =  P2P2D(spl->getPtFromTau(tau));
    cur_sample->_t = spl->r(tau);
    cur_sample->_edge_id = spl->edgeID();
    cur_sample->_label = ExSample::SEMI_DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    double angle = angle0To2Pi (spl->tangent(tau));
    cur_sample->_theta = angle*180/M_PI;
    cur_sample->_speed = spl->v(tau);
    cur_sample->_lBPoint = P2P2D(spl->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(spl->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx

    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    if (spl->nu()==1)
      next_tau = tau + ds/spl->d(tau);
    else
      next_tau = tau - ds/spl->d(tau);

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

  cur_sample->_pt =  P2P2D(spl->getPtFromTau(etau));
  cur_sample->_t = spl->r(etau);
  cur_sample->_edge_id = spl->edgeID();
  cur_sample->_label = ExSample::SEMI_DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = spl->tangent(etau)*180/M_PI;
  cur_sample->_speed = spl->v(etau);
  cur_sample->_lBPoint = P2P2D(spl->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(spl->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx  

  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);
}

void IShockXShock::sampleShock(SILineLine* sll, ExSampleListVector &edgeSampleList)
{
   double tau, stau, etau;
   Point pt;
  ExSample* first_sample = NULL;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sll->tauDir();
   tau = stau = sll->sTau();
   etau = sll->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());
    
    cur_sample->_pt =  P2P2D(sll->getPtFromLTau(tau));
    cur_sample->_t = sll->r(tau);
    cur_sample->_edge_id = sll->edgeID();
    cur_sample->_label = ExSample::REGULAR;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    double angle = angle0To2Pi (sll->tangent(tau));
    cur_sample->_theta = angle*180/M_PI;
    cur_sample->_speed = sll->v(tau);
    cur_sample->_lBPoint = P2P2D(sll->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(sll->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx
      
    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
      double next_tau = tau - ds*vcl_fabs(vcl_cos(M_PI/4 + sll->thetaR()/2));

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());
  
  cur_sample->_pt =  P2P2D(sll->getPtFromLTau(etau));
  cur_sample->_t = sll->r(etau);
  cur_sample->_edge_id = sll->edgeID();
  cur_sample->_label = ExSample::REGULAR;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = sll->tangent(etau)*180/M_PI;
  cur_sample->_speed = sll->v(etau);
  cur_sample->_lBPoint = P2P2D(sll->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(sll->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx
      
  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);
}

void IShockXShock::sampleShock(SIThirdOrder* sto, ExSampleListVector &edgeSampleList)
{
   double tau, stau, etau;
   Point pt;
  ExSample* first_sample = NULL;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sto->tauDir();
   tau = stau = sto->sTau();
   etau = sto->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());
    
    cur_sample->_pt =  P2P2D(sto->getPtFromLTau(tau));
    cur_sample->_t = sto->r(tau);
    cur_sample->_edge_id = sto->edgeID();
    cur_sample->_label = ExSample::REGULAR;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    cur_sample->_theta = sto->tangent(tau)*180/M_PI;
    cur_sample->_speed = sto->v(tau);
    cur_sample->_lBPoint = P2P2D(sto->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(sto->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx
      
    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
      double next_tau = tau - ds;

      //use this next_tau
      tau = next_tau;
   } 

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());
  
  cur_sample->_pt =  P2P2D(sto->getPtFromLTau(etau));
  cur_sample->_t = sto->r(etau);
  cur_sample->_edge_id = sto->edgeID();
  cur_sample->_label = ExSample::REGULAR;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = sto->tangent(etau)*180/M_PI;
  cur_sample->_speed = sto->v(etau);
  cur_sample->_lBPoint = P2P2D(sto->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(sto->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx
      
  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);

}

void IShockXShock::sampleShock(SIPointArc* spa, ExSampleListVector &edgeSampleList)
{
  double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = spa->tauDir();
   tau = stau = spa->sTau();
   etau = spa->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt =  P2P2D(spa->getPtFromTau(tau));
    cur_sample->_t = spa->r(tau);
    cur_sample->_edge_id = spa->edgeID();
    cur_sample->_label = ExSample::DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    cur_sample->_theta = spa->tangent(tau)*180/M_PI;
    cur_sample->_speed = spa->v(tau);
    cur_sample->_lBPoint = P2P2D(spa->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(spa->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx

    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    if (tau_direction==TAU_INCREASING)
      next_tau = tau + ds/spa->d(tau);
    else
      next_tau = tau - ds/spa->d(tau);

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

  cur_sample->_pt =  P2P2D(spa->getPtFromTau(etau));
  cur_sample->_t = spa->r(etau);
  cur_sample->_edge_id = spa->edgeID();
  cur_sample->_label = ExSample::DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = spa->tangent(etau)*180/M_PI;
  cur_sample->_speed = spa->v(etau);//incomplete
  cur_sample->_lBPoint = P2P2D(spa->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(spa->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx  

  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);

}

void IShockXShock::sampleShock(SILineArc* sla, ExSampleListVector &edgeSampleList)
{
  double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sla->tauDir();
   tau = stau = sla->sTau();
   etau = sla->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt =  P2P2D(sla->getPtFromTau(tau));
    cur_sample->_t = sla->r(tau);
    cur_sample->_edge_id = sla->edgeID();
    cur_sample->_label = ExSample::DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    cur_sample->_theta = sla->tangent(tau)*180/M_PI;
    cur_sample->_speed = sla->v(tau);
    cur_sample->_lBPoint = P2P2D(sla->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(sla->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx

    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    if (tau_direction==TAU_INCREASING)
      next_tau = tau + ds/sla->d(tau);
    else
      next_tau = tau - ds/sla->d(tau);

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

  cur_sample->_pt =  P2P2D(sla->getPtFromTau(etau));
  cur_sample->_t = sla->r(etau);
  cur_sample->_edge_id = sla->edgeID();
  cur_sample->_label = ExSample::DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = sla->tangent(etau)*180/M_PI;
  cur_sample->_speed = sla->v(etau);
  cur_sample->_lBPoint = P2P2D(sla->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(sla->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx  

  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);

}

void IShockXShock::sampleShock(SIArcArc* saa, ExSampleListVector &edgeSampleList)
{
  double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = saa->tauDir();
   tau = stau = saa->sTau();
   etau = saa->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt =  P2P2D(saa->getPtFromTau(tau));
    cur_sample->_t = saa->r(tau);
    cur_sample->_edge_id = saa->edgeID();
    cur_sample->_label = ExSample::DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    cur_sample->_theta = saa->tangent(tau)*180/M_PI;
    cur_sample->_speed = saa->v(tau);
    cur_sample->_lBPoint = P2P2D(saa->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(saa->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx

    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    if (tau_direction==TAU_INCREASING)
      next_tau = tau + ds/saa->dFromLTau(tau);
    else
      next_tau = tau - ds/saa->dFromLTau(tau);

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

  cur_sample->_pt =  P2P2D(saa->getPtFromTau(etau));
  cur_sample->_t = saa->r(etau);
  cur_sample->_edge_id = saa->edgeID();
  cur_sample->_label = ExSample::DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = saa->tangent(etau)*180/M_PI;
  cur_sample->_speed = saa->v(etau);
  cur_sample->_lBPoint = P2P2D(saa->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(saa->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx  

  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);

}

void IShockXShock::sampleShock(SIArcThirdOrder* sato, ExSampleListVector &edgeSampleList)
{
  double tau, stau, etau, next_tau;
   Point pt;
  TAU_DIRECTION_TYPE tau_direction;

  tau_direction = sato->tauDir();
   tau = stau = sato->sTau();
   etau = sato->eTau();

   while ((tau<etau && tau_direction==TAU_INCREASING) ||
        (tau>etau && tau_direction==TAU_DECREASING)) {

    ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

    cur_sample->_pt =  P2P2D(sato->getPtFromTau(tau));
    cur_sample->_t = sato->r(tau);
    cur_sample->_edge_id = sato->edgeID();
    cur_sample->_label = ExSample::DEGENERATE;
    cur_sample->_type = ExSample::NORMALSAMPLE;
    cur_sample->_theta = sato->tangent(tau)*180/M_PI;
    cur_sample->_speed = sato->v(tau);
    cur_sample->_lBPoint = P2P2D(sato->getLFootPt(tau));
    cur_sample->_rBPoint = P2P2D(sato->getRFootPt(tau));
    cur_sample->_lBTangent = 0; //xx
    cur_sample->_rBTangent = 0; //xx

    //add to ExSampleList
    ExSampleList.push_back(cur_sample);
    //add to referenced list to store with the edge
    edgeSampleList.push_back(cur_sample);

      double ds = SHK()->DELTA_SAMPLE;
      //increment tau to unit arclength on shock
    if (tau_direction==TAU_INCREASING)
      next_tau = tau + ds/sato->d();
    else
      next_tau = tau - ds/sato->d();

      //use this next_tau
      tau = next_tau;
   }

  ExSample* cur_sample = new ExSample (getNextAvailableSampleID());

  cur_sample->_pt =  P2P2D(sato->getPtFromTau(etau));
  cur_sample->_t = sato->r(etau);
  cur_sample->_edge_id = sato->edgeID();
  cur_sample->_label = ExSample::DEGENERATE;
  cur_sample->_type = ExSample::NORMALSAMPLE;
  cur_sample->_theta = sato->tangent(etau)*180/M_PI;
  cur_sample->_speed = sato->v(etau);
  cur_sample->_lBPoint = P2P2D(sato->getLFootPt(etau));
  cur_sample->_rBPoint = P2P2D(sato->getRFootPt(etau));
  cur_sample->_lBTangent = 0; //xx
  cur_sample->_rBTangent = 0; //xx  

  //add to ExSampleList
  ExSampleList.push_back(cur_sample);
  //add to referenced list to store with the edge
  edgeSampleList.push_back(cur_sample);
}

// write .esf file 
void IShock::write_esf_file (const char* filename, int option)
{
}
