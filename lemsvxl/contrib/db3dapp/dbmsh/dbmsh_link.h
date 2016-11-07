/**************************************************************/
/*  Name: Alexander Bowman
//  File: dbmsh_link.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MeshLink_h_
#define MeshLink_h_

#include "dbmsh_item.h"

class dbmsh_face;
class dbmsh_node;

class dbmsh_link : public dbmsh_item{

public:
  dbmsh_link();
  virtual ~dbmsh_link();
  void addFace( dbmsh_face* face ) { _faces.insert( face ); }
  void addNode( dbmsh_node* node ) { _nodes.insert( node ); }
  void removeFace( dbmsh_face* face ) { _faces.erase( face ); }
  void removeNode( dbmsh_node* node ) { _nodes.erase( node ); }
  int numFaces() { return _faces.size(); }
  int numNodes() { return _nodes.size(); }

  void remove();

private:
  vcl_set<dbmsh_face*> _faces;
  vcl_set<dbmsh_node*> _nodes;


};

#endif
