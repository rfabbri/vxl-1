// XSHOCK.CPP

// This file contains code to maintain and ouput an
// Extrisic Shock Data Structure

#include <vcl_cmath.h>
#include <vcl_iostream.h>

#include <extrautils/common.h>
#include "xshock.h"

/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  Extrinsic Shocks
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

XShock::XShock(void)
{
  nodeIDcnt=0;
  edgeIDcnt=0;
  sampleIDcnt=0;

  ExNodeList.clear();
  ExEdgeList.clear();
  ExSampleList.clear();
}

void XShock::clear()
{
  int i;

  if (ExNodeList.size()>0){
    for(i=0; i<ExNodeList.size(); i++)
      delete ExNodeList[i];
    ExNodeList.clear();
  }

  if (ExEdgeList.size()>0){
    for(i=0; i<ExEdgeList.size(); i++)
      delete ExEdgeList[i];
    ExEdgeList.clear();
  }

  if (ExSampleList.size()>0){
    for(i=0; i<ExSampleList.size(); i++)
      delete ExSampleList[i];
    ExSampleList.clear();
  }
}

void XShock::OutputXShockToFile(const char* filename, vcl_vector<const char*> miscInfo)
{
  // 1. open the esf file
   if (!filename){
      //COUT << " No esg shock filename given"<<vcl_endl;
      return;
   }
   vcl_ofstream fp(filename);
   if (!fp){
      //cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

  fp.precision(5);

  //2. write out the info to file
  OutputHeaderToFile(filename, fp, miscInfo);
  OutputNodeSectionToFile(fp);
  OutputNodeSamplesToFile(fp);
  OutputEdgeSectionToFile(fp);
  OutputEdgeSamplesToFile(fp);

   //3. close file
   fp.close();

}

void XShock::OutputHeaderToFile(const char* filename, vcl_ofstream & fp, vcl_vector<const char*> miscInfo)
{
  //fp <<"Extrinsic Shock File v2.0"<<vcl_endl<<vcl_endl;
   fp <<"Extrinsic Shock File v1.0"<<vcl_endl<<vcl_endl;

  fp <<"Begin [GENERAL INFO]"<<vcl_endl<<vcl_endl;

  fp <<"Input Contour File: "<<filename<<vcl_endl;
  for (int i=0; i<miscInfo.size(); i++)
    fp << miscInfo[i] <<vcl_endl;

  fp <<"Date of Creation: 12/26/2002"<<vcl_endl<<vcl_endl;

  fp <<"Number of Nodes: "<<nodeIDcnt<<vcl_endl;
  fp <<"Number of Edges: "<<edgeIDcnt<<vcl_endl;
  fp <<"Number of Sample Points: "<<sampleIDcnt<<vcl_endl<<vcl_endl;

  fp <<"End [GENERAL INFO]"<<vcl_endl<<vcl_endl;

}

void XShock::OutputNodeSectionToFile(vcl_ofstream & fp)
{
  int j;

   fp <<"Begin [NODE DESCRIPTION]"<<vcl_endl;
   fp <<"# node_ID  node_type Inside/Outside [CW linked node_IDs] [corresponding sample points]"<<vcl_endl;

  for (int i=0; i<ExNodeList.size(); i++){
    ExNode* cur_node = ExNodeList[i];

    //hack
    if (cur_node->_id == -1) continue;

    //node_ID
    fp <<cur_node->_id<<" ";
    //node_type
    switch (cur_node->_type){
      case ExNode::A3SOURCE:  fp <<"A "; break;
      case ExNode::SOURCE:    fp <<"S "; break;
      case ExNode::SINK:    fp <<"F "; break;
      case ExNode::JUNCT:    fp <<"J "; break;
      case ExNode::TERMNINATED:  fp <<"T "; break;
    }

    //inside/outside
    if (cur_node->_bIO == INSIDE)      fp <<"I ";
    else if (cur_node->_bIO == OUTSIDE)  fp <<"O ";
    else                      fp <<"U ";
    
    //linked nodes
    fp <<"[";
    for (j=0; j<cur_node->_connectedNodes.size(); j++){
      ExNode* con_node = cur_node->_connectedNodes[j];
      fp << con_node->_id <<" ";
    }
    fp <<"] ";

    //samples
    fp <<"[";
    for (j=0; j<cur_node->_samples.size(); j++){
      ExSample* cur_sample = cur_node->_samples[j];
      fp << cur_sample->_id <<" ";
    }
    fp <<"] ";
    fp <<vcl_endl;
  }
   fp <<"End [NODE DESCRIPTION]"<<vcl_endl<<vcl_endl;
}

void XShock::OutputSample (vcl_ofstream & fp, ExSample* sample)
{
   fp <<"Begin SAMPLE"<<vcl_endl;
   fp <<"sample_id "<< sample->_id<<vcl_endl;
   fp <<"(x, y, t) ("<< sample->_pt.getX() <<", " << sample->_pt.getY() <<", " << sample->_t <<")"<<vcl_endl;
   fp <<"edge_id "<< sample->_edge_id <<vcl_endl;

  if (sample->_label==ExSample::REGULAR)            fp <<"label regular";
  else if (sample->_label==ExSample::SEMI_DEGENERATE)  fp <<"label regular";
  else if (sample->_label==ExSample::DEGENERATE)      fp <<"label degenerate";
  fp <<vcl_endl;

  if (sample->_type == ExSample::NORMALSAMPLE)      fp <<"type "<< "N"<<vcl_endl;
  else if (sample->_type == ExSample::PRUNEDSAMPLE)  fp <<"type "<< "P"<<vcl_endl;
  else                                fp <<"type "<< "C"<<vcl_endl;

   fp <<"theta "<< sample->_theta<<vcl_endl;
   fp <<"speed "<< sample->_speed<<vcl_endl;
   fp <<"boundaryPoints [(" << sample->_lBPoint.getX() <<", "<< sample->_lBPoint.getY() <<")";
  fp <<", (" << sample->_rBPoint.getX() <<", " << sample->_rBPoint.getY() <<")]"<<vcl_endl;
   fp <<"boundaryTangents ["<< sample->_lBTangent <<", "<< sample->_rBTangent <<"]"<<vcl_endl;
   fp <<"End SAMPLE"<<vcl_endl<<vcl_endl;
}

void XShock::OutputNodeSamplesToFile(vcl_ofstream & fp)
{
   fp <<"Begin [NODE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;

  for (int i=0; i<ExNodeList.size(); i++){
    ExNode* cur_node = ExNodeList[i];

    //hack
    if (cur_node->_id == -1) continue;

    for (int j=0; j<cur_node->_samples.size(); j++){
      ExSample* cur_sample = cur_node->_samples[j];
      OutputSample (fp, cur_sample);
    }
  }
   fp <<"End [NODE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;
}

void XShock::OutputEdgeSectionToFile(vcl_ofstream & fp)
{
   fp <<"Begin [EDGE DESCRIPTION]"<<vcl_endl;
   fp <<"# Edge_ID Inside/Outside [From_NODE To_NODE] [IDs of the sample points]"<<vcl_endl;

  for (int i=0; i<ExEdgeList.size(); i++){
    ExEdge* cur_edge = ExEdgeList[i];

    //marked for removal !!! hack
    if (cur_edge->_fromNode == NULL && cur_edge->_toNode == NULL)
       continue;

    // edge id
    fp <<cur_edge->_id<<" ";

    //inside /outside
    if (cur_edge->_bIO==INSIDE)      fp <<"I ";
    else if (cur_edge->_bIO == OUTSIDE)  fp <<"O ";
    else                      fp <<"U ";

    //from node - to node
    fp <<"[";
    fp << cur_edge->_fromNode->_id<<" ";
    fp << cur_edge->_toNode->_id;
    fp <<"] ";

    //samples including node samples
    fp <<"[";
    for (int j=0; j<cur_edge->_EdgeSamples.size(); j++){
      ExSample* cur_sample = cur_edge->_EdgeSamples[j];
      //hack
      if (cur_sample->_edge_id == -1) continue;
      fp << cur_sample->_id <<" ";
    }
    fp <<"]"<<vcl_endl;
  }

  fp <<"End [EDGE DESCRIPTION]"<<vcl_endl<<vcl_endl;
}

void XShock::OutputEdgeSamplesToFile(vcl_ofstream & fp)
{
   fp <<"Begin [EDGE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;

  for (int i=0; i<ExEdgeList.size(); i++){
    ExEdge* cur_edge = ExEdgeList[i];

    //marked for removal !!! hack
    if (cur_edge->_fromNode == NULL && cur_edge->_toNode == NULL)
       continue;

    //No need to write first and last sample of each edge...
    for (int j=1; j<cur_edge->_EdgeSamples.size()-1; j++){
      ExSample* cur_sample = cur_edge->_EdgeSamples[j];
      
      //hack
      if (cur_sample->_edge_id == -1) continue;

      OutputSample (fp, cur_sample);
    }
  }
   fp <<"End [EDGE SAMPLE POINTS]"<<vcl_endl<<vcl_endl;
}

void XShock::cropESF(Point2D<double>topLeft, Point2D<double>bottomRight)
{
  //this will delete all the extrinsic shocks that are outside the window

  vcl_vector<ExEdge*>::iterator curEdge = ExEdgeList.begin();
  for (;curEdge!=ExEdgeList.end();curEdge++){
    ExEdge* cEdge = (*curEdge);
    Point2D<double> edge_start = cEdge->_fromNode->_samples.front()->_pt;
    Point2D<double> edge_end = cEdge->_toNode->_samples.front()->_pt;

    bool start_of_edge_is_outside_limits = false;
    bool end_of_edge_is_outside_limits = false;

    if( edge_start.getX() < topLeft.getX() || 
       edge_start.getY() > topLeft.getY() ||
       edge_start.getX() > bottomRight.getX() ||
       edge_start.getY() < bottomRight.getY() )
    {
      //start node is outside the boundary
      start_of_edge_is_outside_limits =true;
    }

    if( edge_end.getX() < topLeft.getX() || 
       edge_end.getY() > topLeft.getY() ||
       edge_end.getX() > bottomRight.getX() ||
       edge_end.getY() < bottomRight.getY() )
    {
      //start node is outside the boundary
      end_of_edge_is_outside_limits =true;
    }

    //if both the nodes are outside the limits
    //mark for delete

    if (start_of_edge_is_outside_limits &&
      end_of_edge_is_outside_limits){
      
      //mark the old node for delete
      cEdge->_fromNode->_id = -1; //hack
      //mark the old node for delete
      cEdge->_toNode->_id = -1; //hack

      cEdge->_fromNode = NULL; //hack
      cEdge->_toNode = NULL;
    }
    //if only one of the nodes are outside the limits
    //create a new node at the intersection of the edge with
    // the limiting box
    else if (start_of_edge_is_outside_limits){
      assert(0);
    }
    else if (end_of_edge_is_outside_limits){
      //traverse the list of the samples in this edge and find the sample 
      //that is just inside the box
      ExSample* endSample=NULL;
      vcl_vector<ExSample*>::iterator curSample = cEdge->_EdgeSamples.begin();
      for (;curSample!=cEdge->_EdgeSamples.end();curSample++){
        ExSample* curS = (*curSample);

        if (endSample){
          //mark for deletion
          curS->_edge_id = -1; //hack
          //can probably remove it from the list
        }

        if((curS->_pt.getX() < topLeft.getX() ||
           curS->_pt.getY() > topLeft.getY() ||
           curS->_pt.getX() > bottomRight.getX() ||
           curS->_pt.getY() < bottomRight.getY()) && !endSample )
        {
          endSample = curS;
        }
      }

      //form a new termination Node usingthis sample point
      long id_to_use = 100000 + getNextAvailableNodeID();
      //use the ids above shock id

      ExNode* new_end_node = new ExNode(id_to_use, 
        ExNode::A3SOURCE, OUTSIDE);  //ExNode::TERMNINATED
      ExNodeList.push_back(new_end_node);

      new_end_node->AddNodeAndEdgeToConnectedList(cEdge->_fromNode, cEdge);
      new_end_node->_samples.push_back(endSample);

      //change the from node information to point to the to node
      ExNode* from_node = cEdge->_fromNode;
      //go through the connected node list of the from node
      for (int j=0; j<from_node->_connectedNodes.size(); j++){
        ExNode* con_node = from_node->_connectedNodes[j];
        if (con_node==cEdge->_toNode){
          from_node->_connectedNodes[j] = new_end_node;
          break;
        }
      }

      //mark the old node for delete
      cEdge->_toNode->_id = -1; //hack

      //connect this edge to new node
      cEdge->_toNode = new_end_node;
      
    }

  }

}

