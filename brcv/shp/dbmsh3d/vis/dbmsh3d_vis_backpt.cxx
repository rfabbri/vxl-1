//: 050223 MingChing Chang

///#include <dbmsh3d/gdtvis/gdtvis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>

// ==========================================================
//    The Homemade vis_pointer OpenInventor Classes
// ==========================================================
SO_NODE_SOURCE (dbmsh3dvis_vertex_SoPointSet);
SO_NODE_SOURCE (dbmsh3dvis_vertex_SoCube);
SO_NODE_SOURCE (dbmsh3dvis_edge_SoLineSet);
SO_NODE_SOURCE (dbmsh3dvis_face_SoFaceSet);


void dbmsh3dvis_init_vispt_OpenInventor_classes ()
{
  dbmsh3dvis_vertex_SoPointSet::initClass();
  dbmsh3dvis_vertex_SoCube::initClass();
  dbmsh3dvis_edge_SoLineSet::initClass();
  dbmsh3dvis_face_SoFaceSet::initClass();
}

// ==========================================================

dbmsh3dvis_vertex_SoPointSet::dbmsh3dvis_vertex_SoPointSet (const dbmsh3d_vertex* point)
{
  SO_NODE_CONSTRUCTOR (dbmsh3dvis_vertex_SoPointSet);
  this->isBuiltIn = true;

  _element = (vispt_elm*) point;
  _element->set_vis_pointer ((void*)this);
}

dbmsh3dvis_vertex_SoCube::dbmsh3dvis_vertex_SoCube (const dbmsh3d_vertex* point)
{
  SO_NODE_CONSTRUCTOR (dbmsh3dvis_vertex_SoCube);
  this->isBuiltIn = true;

  _element = (vispt_elm*) point;
  _element->set_vis_pointer ((void*)this);
}

dbmsh3dvis_edge_SoLineSet::dbmsh3dvis_edge_SoLineSet (const dbmsh3d_edge* edge)
{
  SO_NODE_CONSTRUCTOR (dbmsh3dvis_edge_SoLineSet);
  this->isBuiltIn = true;

  _element = (dbmsh3d_edge*) edge;
  _element->set_vis_pointer ((void*)this);
}

dbmsh3dvis_face_SoFaceSet::dbmsh3dvis_face_SoFaceSet (const dbmsh3d_face* face)
{
  SO_NODE_CONSTRUCTOR (dbmsh3dvis_face_SoFaceSet);
  this->isBuiltIn = true;

  _element = (vispt_elm*) face;
  _element->set_vis_pointer ((void*)this);
}


