//: 040309 Ming: dbmsh3d_vis_backpt.h
//: Derived class of standard OpenInventor object to put backward pointer 
//: to our datastructure.

#ifndef dbmsh3d_vis_backpt_h_
#define dbmsh3d_vis_backpt_h_

#include <dbmsh3d/dbmsh3d_utils.h>

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_face.h>

#include <Inventor/nodes/SoPointSet.h> 
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoCoordinate3.h>

void dbmsh3dvis_init_vispt_OpenInventor_classes ();

//: the GUI Element has a backward pointer to the element in
//  the internal data structure.
class dbmsh3dvis_gui_elm
{
protected:
  vispt_elm* _element;

public:
  dbmsh3dvis_gui_elm() {}
  virtual ~dbmsh3dvis_gui_elm() {}

  vispt_elm* element() { 
    return _element; 
  }
};

//----------------- MESH Vertex Sphere -----------------------------
class dbmsh3dvis_vertex_SoPointSet : public SoPointSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER( dbmsh3dvis_vertex_SoPointSet );

public:
  dbmsh3dvis_vertex_SoPointSet (const dbmsh3d_vertex* point=NULL );
  virtual ~dbmsh3dvis_vertex_SoPointSet() {}

  static void initClass(void) { 
    SO_NODE_INIT_CLASS( dbmsh3dvis_vertex_SoPointSet, SoPointSet, "SoPointSet"); 
  }
  virtual const char * getFileFormatName(void) const { 
    return "dbmsh3dvis_vertex_SoPointSet"; 
  }
};

//----------------- MESH Vertex Cube -------------------------------------
class dbmsh3dvis_vertex_SoCube : public SoCube, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (dbmsh3dvis_vertex_SoCube);

public:
  dbmsh3dvis_vertex_SoCube (const dbmsh3d_vertex* point=NULL);
  virtual ~dbmsh3dvis_vertex_SoCube() {}

  static void initClass(void) { 
    SO_NODE_INIT_CLASS( dbmsh3dvis_vertex_SoCube, SoCube, "SoCube"); 
  }
  virtual const char * getFileFormatName(void) const { 
    return "dbmsh3dvis_vertex_SoCube"; 
  }
};

//----------------- MESH EDGE -------------------------------------
class dbmsh3dvis_edge_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (dbmsh3dvis_edge_SoLineSet);

protected:
  //: for online alternation
  SoCoordinate3* _coordinate;

public:
  dbmsh3dvis_edge_SoLineSet (const dbmsh3d_edge* edge=NULL);
  virtual ~dbmsh3dvis_edge_SoLineSet() {}

  static void initClass() { 
    SO_NODE_INIT_CLASS (dbmsh3dvis_edge_SoLineSet, SoLineSet, "SoLineSet"); 
  }
  virtual const char * getFileFormatName() const { 
    return "dbmsh3dvis_edge_SoLineSet"; 
  }

  //: for online alternation
  void setCoordinate3 (SoCoordinate3* coordinate) { 
    _coordinate = coordinate; 
  }
  SoCoordinate3* coordinate3() { 
    return _coordinate; 
  }
  void update();
};

//----------------- MESH FACE -------------------------------------
class dbmsh3dvis_face_SoFaceSet : public SoFaceSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (dbmsh3dvis_face_SoFaceSet);

public:
  dbmsh3dvis_face_SoFaceSet (const dbmsh3d_face* face=NULL);
  virtual ~dbmsh3dvis_face_SoFaceSet() {}

  static void initClass() { 
    SO_NODE_INIT_CLASS (dbmsh3dvis_face_SoFaceSet, SoFaceSet, "SoFaceSet"); 
  }
  virtual const char * getFileFormatName() const { 
    return "dbmsh3dvis_face_SoFaceSet"; 
  }
};

#endif


