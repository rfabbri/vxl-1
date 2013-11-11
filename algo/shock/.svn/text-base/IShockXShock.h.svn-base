#ifndef _ISHOCKXSHOCK_H
#define _ISHOCKXSHOCK_H

#include <vcl_utility.h>
#include <vcl_map.h>

#include "xshock.h"
#include "ishock.h"

class IShockExNode;
class IShockExEdge;
class IShockXShock;

typedef vcl_pair<int, ExNode*> SINodeExNodePair;

class IShockExNode: public ExNode
{ 
   public:
    SINode* _iNode;      //link to intrinsic node

      IShockExNode (int newid, SINode* node, SHOCKNODETYPE newtype, int newbIO):
      ExNode(newid, newtype, newbIO), _iNode(node) {}
      ~IShockExNode() {}
};

class IShockExEdge: public ExEdge
{
  public:
    SILink* _first_link;  //first link on the edge

    IShockExEdge(int newid, SILink* f_link, 
      ExNode* FNode, ExNode* TNode, int newbIO):
      ExEdge(newid, FNode, TNode, newbIO), _first_link(f_link){}
    ~IShockExEdge() {}
};

class IShockXShock: public XShock
{
public:
  IShock* _shock;
  vcl_map <int, ExNode*> SINodeToExNodeMapping; //SINode ID to ExNode* mapping

  IShockXShock(IShock* shock){_shock=shock;}
  ~IShockXShock(){}

  IShock* SHK(){return _shock;}
  virtual void clear(){SINodeToExNodeMapping.clear();XShock::clear();}

  void SetupNodes(int option);
  void SetupEdges(int option);
  void SampleAllEdges(int option);
  void UpdateSampleInformationInNodes();

  ExNode* traceBackToPreviousNode( SILink* cur_link);
  ExNode* traceToNextNode(SINode* cur_node);
  ExNode* traceToNextNode(SILink* cur_link);

  void sample_intrinsic_shocks(int option);
  void write_esf_file (const char* filename);

  void sampleShock(SIPointPoint* spp, ExSampleListVector &edgeSampleList);
  void sampleShock(SIPointLine* spl, ExSampleListVector &edgeSampleList);
  void sampleShock(SILineLine* sll, ExSampleListVector &edgeSampleList);
  void sampleShock(SIPointArc* spa, ExSampleListVector &edgeSampleList);
  void sampleShock(SILineArc* sla, ExSampleListVector &edgeSampleList);
  void sampleShock(SIArcArc* saa, ExSampleListVector &edgeSampleList);
  void sampleShock(SIThirdOrder* sto, ExSampleListVector &edgeSampleList);
  void sampleShock(SIArcThirdOrder* sato, ExSampleListVector &edgeSampleList);
};


#endif
