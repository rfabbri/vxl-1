/**************************************************************/
/*  Name: Alexander Bowman
//  File: dbmsh_item.h
//  Asgn:
//  Date:
***************************************************************/

#ifndef MeshItem_h_
#define MeshItem_h_

#include <vcl_set.h>
#include <vcl_cstdio.h>

class dbmsh_item {

public:
  dbmsh_item();
  virtual ~dbmsh_item();
  void setDrawnItem( void* pointer ) { _pointer = pointer; }
  void* getDrawnItem() { return _pointer; }

protected:
  void* _pointer;

};



#endif
