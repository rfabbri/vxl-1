//: 050223 MingChing Chang

#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_sheet.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>

#include <dbsk3d/vis/dbsk3d_vis_backpt.h>

SO_NODE_SOURCE( fs_vertex_SoCube );
SO_NODE_SOURCE( fs_edge_SoLineSet );
SO_NODE_SOURCE( fs_face_SoFaceSet );
SO_NODE_SOURCE( fs_sheet_SoIndexedFaceSet );

SO_NODE_SOURCE( ms_node_SoSphere );
SO_NODE_SOURCE( ms_curve_SoLineSet );
SO_NODE_SOURCE( ms_sheet_SoIndexedFaceSet );


void dbsk3dvis_init_vispt_OpenInventor_classes ()
{
  fs_vertex_SoCube::initClass();
  fs_edge_SoLineSet::initClass();
  fs_face_SoFaceSet::initClass();
  fs_sheet_SoIndexedFaceSet::initClass();

  ms_node_SoSphere::initClass();
  ms_curve_SoLineSet::initClass();
  ms_sheet_SoIndexedFaceSet::initClass();
}

fs_vertex_SoCube::fs_vertex_SoCube (dbsk3d_fs_vertex* node)
{
  SO_NODE_CONSTRUCTOR (fs_vertex_SoCube);
  this->isBuiltIn = true;

  _element = node;
  _element->set_vis_pointer( (void*)this );
}

void fs_vertex_SoCube::update()
{
  vgl_point_3d<double> point = ((dbsk3d_fs_vertex*)_element)->get_pt();
  _translation->translation.setValue( static_cast<float>(point.x()),
                                      static_cast<float>(point.y()),
                                      static_cast<float>(point.z()) );
}

fs_edge_SoLineSet::fs_edge_SoLineSet (dbsk3d_fs_edge* link)
{
  SO_NODE_CONSTRUCTOR (fs_edge_SoLineSet);
  this->isBuiltIn = true;

  _element = link;
  _element->set_vis_pointer( (void*)this );
}

void fs_edge_SoLineSet::update()
{
  const dbsk3d_fs_vertex* nodeS = ((dbsk3d_fs_edge*)_element)->s_FV();
  const dbsk3d_fs_vertex* nodeE = ((dbsk3d_fs_edge*)_element)->e_FV();
  _coordinate->point.set1Value (0, static_cast<float>(nodeS->pt().x()),
                                   static_cast<float>(nodeS->pt().y()),
                                   static_cast<float>(nodeS->pt().z()) );
  _coordinate->point.set1Value (1, static_cast<float>(nodeE->pt().x()),
                                   static_cast<float>(nodeE->pt().y()),
                                   static_cast<float>(nodeE->pt().z()) );
}

fs_face_SoFaceSet::fs_face_SoFaceSet (dbsk3d_fs_face* FF)
{
  SO_NODE_CONSTRUCTOR (fs_face_SoFaceSet);
  this->isBuiltIn = true;

  _element = FF;
  _element->set_vis_pointer( (void*)this );
}

void fs_face_SoFaceSet::update()
{
  dbsk3d_fs_face* patchElm = (dbsk3d_fs_face*) _element;
  patchElm->_ifs_track_ordered_vertices();
  
  for (unsigned int j=0; j<patchElm->vertices().size(); j++) {
    dbmsh3d_vertex* vertex = patchElm->vertices(j);
    _coordinate->point.set1Value(int(j), (float) vertex->pt().x(),
                                 (float) vertex->pt().y(),
                                 (float) vertex->pt().z());
  }
  patchElm->vertices().clear();
}

//: SoIndexedFaceSet for for fs_sheet.
fs_sheet_SoIndexedFaceSet::fs_sheet_SoIndexedFaceSet (dbsk3d_fs_sheet* S)
{
  SO_NODE_CONSTRUCTOR (fs_sheet_SoIndexedFaceSet);
  this->isBuiltIn = true;

  _element = S;
  _element->set_vis_pointer ((void*) this);
}

//: SoSphere for ms_vertex.
ms_node_SoSphere::ms_node_SoSphere (const dbsk3d_ms_node* V)
{
  SO_NODE_CONSTRUCTOR (ms_node_SoSphere);
  this->isBuiltIn = true;

  _element = (dbsk3d_ms_node*) V;
  _element->set_vis_pointer( (void*)this );
}


//: SoLineSet for ms_curve.
ms_curve_SoLineSet::ms_curve_SoLineSet (const dbsk3d_ms_curve* link)
{
  SO_NODE_CONSTRUCTOR (ms_curve_SoLineSet);
  this->isBuiltIn = true;

  _element = (dbsk3d_ms_curve*) link;
  _element->set_vis_pointer( (void*)this );
}

//: SoIndexedFaceSet for for ms_sheet.
ms_sheet_SoIndexedFaceSet::ms_sheet_SoIndexedFaceSet (dbsk3d_ms_sheet* sheet)
{
  SO_NODE_CONSTRUCTOR (ms_sheet_SoIndexedFaceSet);
  this->isBuiltIn = true;

  _element = sheet;
  _element->set_vis_pointer( (void*)this );
}

