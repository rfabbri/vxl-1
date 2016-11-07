/**************************************************************/
/*  Name: Alexander Bowman
//  File: dbmsh_item.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MeshFace_h_
#define MeshFace_h_

#include "dbmsh_item.h"

class dbmsh_link;
class dbmsh_node;

class dbmsh_face : public dbmsh_item {

public:
  dbmsh_face();
  virtual ~dbmsh_face();
  void addLink( dbmsh_link* link ) { _links.insert( link ); }
  void addNode( dbmsh_node* node ) { _nodes.insert( node ); }
  void removeLink( dbmsh_link* link ) { _links.erase( link ); }
  void removeNode( dbmsh_node* node ) { _nodes.erase( node ); }
  int numLinks() { return _links.size(); }
  int numNodes() { return _nodes.size(); }
  
  void remove();

private:
  vcl_set<dbmsh_link*> _links;
  vcl_set<dbmsh_node*> _nodes;


};

#endif
