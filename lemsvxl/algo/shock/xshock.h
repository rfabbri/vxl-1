#ifndef _XSHOCK_H
#define _XSHOCK_H

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_cassert.h>

#include <extrautils/common.h>
#include <extrautils/points.h>

class ExSample;
class ExNode;
class ExEdge;
class XShock;

class ExSample
{
public:

  typedef enum  {
    NORMALSAMPLE='N', PRUNEDSAMPLE='P', CONTACTSAMPLE='C'
  } EXSAMPLETYPE;

  typedef enum { 
    BOGUS_SHOCK_LABEL,           
    REGULAR='R', SEMI_DEGENERATE = 'S', DEGENERATE = 'D', CONTACT = 'C' 
  } SHOCKLABEL;

  public:
    int _id;            //sample id
    Point2D<double> _pt;    //extrinsic point
    double _t;          //radius (time)
    int _edge_id;        //edge that it belongs to
                    //Note: At nodes, there are multiple samples corresponding to the edges
    SHOCKLABEL _label;      //type of shock causingit
    EXSAMPLETYPE _type;    //(N)Normal / (P)pruned junction /(C)contact junction
  
    //Dynamics at the sample
    double _theta;
    double _speed;

    //Boundary parameters 
    Point2D<double> _lBPoint; //corresponding left boundary point
    Point2D<double> _rBPoint; //corresponding right boundary point
    double _lBTangent;        //tangent at the left boundary point
    double _rBTangent;        //tangent at the right boundary point

    ExSample(int newid=0): _id(newid){}
    ~ExSample() {}
};

class ExEdge
{
  public:
    int _id;          //Edge ID
    ExNode* _fromNode;  //ExEdge is a directed edge hence the from and to nodes
    ExNode* _toNode;
    int _bIO;        //inside/outside info

    vcl_vector<ExSample* > _EdgeSamples;

    ExEdge(int newid, ExNode* FNode, ExNode* TNode, int newbIO):
      _id(newid), _fromNode(FNode), _toNode(TNode), _bIO(newbIO){}
    ~ExEdge() {}
};

class ExNode 
{ 
public:
  typedef enum { 
    BOGUS_SHOCK_TYPE, SOURCE, SINK, A3SOURCE, JUNCT, TERMNINATED           
  } SHOCKNODETYPE;

   public:
    int _id;          //node ID
      int _bIO;        //inside/outside 
    SHOCKNODETYPE _type; //type of node (see above)

    //the following vectors have information that are sensitive to their indices
    //the same index corresponds to an ExEdge, the sample of the node corresponding to 
    //the edge and the ExNode at the other end of the edge
    vcl_vector<ExEdge*> _edges;        // list of edges it belongs to
    vcl_vector<ExSample*> _samples;    // list of samples of this ExNode
    vcl_vector<ExNode*> _connectedNodes;  // list of nodes it is connected to

      ExNode (int newid, SHOCKNODETYPE newtype, int newbIO): 
      _id(newid), _bIO(newbIO), _type(newtype)
    {
      _edges.clear(); 
      _samples.clear(); 
      _connectedNodes.clear();
    }
      ~ExNode() {}

    void AddNodeAndEdgeToConnectedList(ExNode* newnode, ExEdge* newedge)
    {
      assert(newnode);
      _connectedNodes.push_back(newnode);
      assert(newedge);
      _edges.push_back(newedge);
    }

    ExEdge* FindExEdgeToThisExNode(ExNode* node)
    {
      //find ExEdge corresponding to this Node
      ExEdge* cur_edge= 0;
      for(unsigned int i=0; i<_edges.size(); i++){
        cur_edge = _edges[i];
        if (cur_edge->_toNode == node)
          return cur_edge;
      }
      return NULL;
    }

    ExEdge* FindExEdgeFromThisExNode(ExNode* node)
    {
      //find ExEdge corresponding to this Node
      ExEdge* cur_edge= 0;
      for(unsigned int i=0; i<_edges.size(); i++){
        cur_edge = _edges[i];
        if (cur_edge->_fromNode == node)
          return cur_edge;
      }
      return NULL;
    }

};

typedef vcl_vector <ExNode* >::iterator ExNodeListIterator;
typedef vcl_vector <ExEdge* >::iterator ExEdgeListIterator;
typedef vcl_vector <ExSample* >::iterator ExSampleListIterator;
typedef vcl_vector <ExSample* > ExSampleListVector;

class XShock
{
public:
  vcl_vector <ExNode* >    ExNodeList;  //list of Extrinsic Shock Nodes
  vcl_vector <ExEdge* >    ExEdgeList;  //list of Extrinsic Shock Edges
  vcl_vector <ExSample* >  ExSampleList;//list of Extrinsic Sample Points

  // to keep count of the ids
  int nodeIDcnt;
  int edgeIDcnt;
  int sampleIDcnt;

  int getNextAvailableNodeID(){nodeIDcnt++; return nodeIDcnt;}
  int getNextAvailableEdgeID(){edgeIDcnt++; return edgeIDcnt;}
  int getNextAvailableSampleID(){sampleIDcnt++; return sampleIDcnt;}

  XShock();
  virtual ~XShock(){clear();}

  //clear all the extrinsic elements
  virtual void clear();

  void cropESF(Point2D<double>topLeft, Point2D<double>bottomRight);

  void OutputXShockToFile(const char* filename, vcl_vector<const char *> miscInfo);
  void OutputHeaderToFile(const char* filename, vcl_ofstream & fp, vcl_vector<const char *> miscInfo);
  void OutputNodeSectionToFile(vcl_ofstream & fp);
  void OutputNodeSamplesToFile(vcl_ofstream & fp);
  void OutputEdgeSectionToFile(vcl_ofstream & fp);
  void OutputEdgeSamplesToFile(vcl_ofstream & fp);
  void OutputSample (vcl_ofstream & fp, ExSample* sample);

};

#endif
