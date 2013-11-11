/**************************************************************/
/*  Name: Alexander Bowman
//  File: dbmsh_node.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MeshNode_h_
#define MeshNode_h_

#include "dbmsh_item.h"

#include <vcl_vector.h>


class dbmsh_link;
class dbmsh_face;

class dbmsh_node : public dbmsh_item {

public:
  dbmsh_node();
  virtual ~dbmsh_node();
  void addLink( dbmsh_link* link ) { _links.insert( link ); }
  void addFace( dbmsh_face* face ) { _faces.insert( face ); }
  void removeLink( dbmsh_link* link ) { _links.erase( link ); }
  void removeFace( dbmsh_face* face ) { _faces.erase( face ); }
  vcl_set<dbmsh_face*> getFaces() { return _faces; }
  vcl_set<dbmsh_link*> getLinks() { return _links; }

  int numFaces() { return _faces.size(); }
  int numLinks() { return _links.size(); }
  
  void remove();
  
  dbmsh_link* commonLink( dbmsh_node* node );
  vcl_vector<dbmsh_face*> commonFaces( dbmsh_node* node );

  void setPosition( float x, float y, float z) { _x = x; _y = y; _z = z; }
  void getPosition( float &x, float &y, float &z ) { x = _x; y = _y; z = _z; }


  /*void changePosition( float fromX, float fromY, float fromZ, 
                       float toX, float toY, float toZ );*/


private:
  vcl_set<dbmsh_link*> _links;
  vcl_set<dbmsh_face*> _faces;
  float _x;
  float _y;
  float _z;
  



};



#endif
