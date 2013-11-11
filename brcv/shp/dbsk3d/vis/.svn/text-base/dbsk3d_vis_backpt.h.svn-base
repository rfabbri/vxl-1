//: 040309 Ming: geometry_backward_pointer.h
//: Derived class of standard OpenInventor object to put backward pointer 
//: to our datastructure.

#ifndef _dbsk3d_vis_backpt_h_
#define _dbsk3d_vis_backpt_h_

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>

class vispt_elm;

class dbsk3d_fs_face;
class dbsk3d_fs_edge;
class dbsk3d_fs_vertex;

class dbsk3d_ms_node;
class dbsk3d_ms_curve;
class dbsk3d_ms_sheet;

#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoIndexedLineSet.h> 
#include <Inventor/nodes/SoIndexedFaceSet.h> 
#include <Inventor/nodes/SoPointSet.h> 
#include <Inventor/nodes/SoMarkerSet.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h> 
#include <Inventor/nodes/SoFaceSet.h>

void dbsk3dvis_init_vispt_OpenInventor_classes();


//: SoCube for fs_vertex.
class fs_vertex_SoCube : public SoCube, public dbmsh3dvis_gui_elm 
{
  SO_NODE_HEADER(fs_vertex_SoCube);

protected:
  //: for online alternation
  SoTranslation* _translation;

public:
  fs_vertex_SoCube (dbsk3d_fs_vertex* node=NULL );
  virtual ~fs_vertex_SoCube() {}

  static void initClass(void) { 
    SO_NODE_INIT_CLASS(fs_vertex_SoCube, SoCube, "SoCube"); 
  }
  virtual const char * getFileFormatName(void) const { 
    return "fs_vertex_SoCube"; 
  }

  //: for online alternation
  void setTranslation (SoTranslation* translation) { 
    _translation = translation; 
  }
  SoTranslation* translation() { 
    return _translation; 
  }
  void update();
};

//: SoLineSet for fs_edge.
class fs_edge_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER(fs_edge_SoLineSet);

protected:
  //: for online alternation
  SoCoordinate3* _coordinate;

public:
  fs_edge_SoLineSet (dbsk3d_fs_edge* link=NULL );
  virtual ~fs_edge_SoLineSet() {}
  static void initClass(void) {SO_NODE_INIT_CLASS(fs_edge_SoLineSet, SoLineSet, "SoLineSet"); }
  virtual const char * getFileFormatName(void) const { 
    return "fs_edge_SoLineSet"; 
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

//: SoFaceSet for fs_face.
class fs_face_SoFaceSet : public SoFaceSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (fs_face_SoFaceSet);

private:
  SoCoordinate3* _coordinate;

public:
  fs_face_SoFaceSet (dbsk3d_fs_face* patch=NULL );
  virtual ~fs_face_SoFaceSet() {}
  static void initClass (void) { 
    SO_NODE_INIT_CLASS (fs_face_SoFaceSet, SoFaceSet, "SoFaceSet"); 
  }
  virtual const char * getFileFormatName(void) const { 
    return "fs_face_SoFaceSet"; 
  }
  void setCoordinate3 (SoCoordinate3* coordinate) { 
    _coordinate = coordinate; 
  }
  SoCoordinate3* coordinate3() { 
    return _coordinate; 
  }
  void update();
};

class dbsk3d_fs_sheet;

//: SoIndexedFaceSet for for fs_sheet.
class fs_sheet_SoIndexedFaceSet : public SoIndexedFaceSet, public dbmsh3dvis_gui_elm 
{
  SO_NODE_HEADER(fs_sheet_SoIndexedFaceSet);

public:
  fs_sheet_SoIndexedFaceSet (dbsk3d_fs_sheet* sheet=NULL );
  virtual ~fs_sheet_SoIndexedFaceSet() {}
  static void initClass(void) { SO_NODE_INIT_CLASS(fs_sheet_SoIndexedFaceSet, SoIndexedFaceSet, "SoIndexedFaceSet"); }
  virtual const char * getFileFormatName(void) const { 
    return "fs_sheet_SoIndexedFaceSet"; 
  }
};

//###########################################################

class dbsk3d_ms_node;
class dbsk3d_ms_curve;
class dbsk3d_ms_sheet;

//: SoSphere for ms_vertex.
class ms_node_SoSphere : public SoSphere, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (ms_node_SoSphere);

public:
  ms_node_SoSphere (const dbsk3d_ms_node* SVertex=NULL);
  virtual ~ms_node_SoSphere() {}
  static void initClass (void) { SO_NODE_INIT_CLASS(ms_node_SoSphere, SoSphere, "SoSphere"); }
  virtual const char* getFileFormatName(void) const { 
    return "ms_node_SoSphere"; 
  }
};

//: SoLineSet for ms_curve.
class ms_curve_SoLineSet : public SoLineSet, public dbmsh3dvis_gui_elm
{
  SO_NODE_HEADER (ms_curve_SoLineSet);

public:
  ms_curve_SoLineSet (const dbsk3d_ms_curve* scurve=NULL);
  virtual ~ms_curve_SoLineSet() {}
  static void initClass (void) { SO_NODE_INIT_CLASS(ms_curve_SoLineSet, SoLineSet, "SoLineSet"); }
  virtual const char* getFileFormatName(void) const { 
    return "ms_curve_SoLineSet"; 
  }
};

//: SoIndexedFaceSet for for ms_sheet.
class ms_sheet_SoIndexedFaceSet : public SoIndexedFaceSet, public dbmsh3dvis_gui_elm 
{
  SO_NODE_HEADER(ms_sheet_SoIndexedFaceSet);

public:
  ms_sheet_SoIndexedFaceSet ( dbsk3d_ms_sheet* sheet=NULL );
  virtual ~ms_sheet_SoIndexedFaceSet() {}
  static void initClass(void) { SO_NODE_INIT_CLASS(ms_sheet_SoIndexedFaceSet, SoIndexedFaceSet, "SoIndexedFaceSet"); }
  virtual const char * getFileFormatName(void) const { 
    return "ms_sheet_SoIndexedFaceSet"; 
  }
};

#endif
