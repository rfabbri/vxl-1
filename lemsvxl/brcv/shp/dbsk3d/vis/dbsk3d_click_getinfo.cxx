#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

//Be careful on the order here!
#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_sheet.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_click_getinfo.h>

#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/events/SoKeyboardEvent.h>

//: Output Get Info of Clicked Object.
//
bool dbsk3d_output_object_info (SoPath* path)
{
  vcl_ostringstream ostrm;
  bool object_found = false;

  //path->getTail() is the frist element got clicked!
  SoNode* hitObject = path->getTail();

  //fs_vertex_SoCube 
  if (hitObject->getTypeId()==fs_vertex_SoCube::getClassTypeId() ) {
    fs_vertex_SoCube* FV_vis = (fs_vertex_SoCube*) hitObject;
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*)FV_vis->element();
    FV->getInfo (ostrm);
    vcl_cout<< ostrm.str();
    object_found = true;
  }
  //fs_edge_SoLineSet
  else if (hitObject->getTypeId()==fs_edge_SoLineSet::getClassTypeId() ) {
    fs_edge_SoLineSet* FE_vis = (fs_edge_SoLineSet*) hitObject;
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*)FE_vis->element();
    FE->getInfo (ostrm);
    vcl_cout<< ostrm.str();
    object_found = true;
  }
  //fs_face_SoFaceSet
  else if (hitObject->getTypeId()==fs_face_SoFaceSet::getClassTypeId() ) {
    fs_face_SoFaceSet* FF_vis = (fs_face_SoFaceSet*) hitObject;
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FF_vis->element();
    FF->getInfo (ostrm);
    vcl_cout<< ostrm.str();
    object_found = true;
  }  
  //fs_sheet_SoIndexedFaceSet
  else if (hitObject->getTypeId()==fs_sheet_SoIndexedFaceSet::getClassTypeId()) {
    fs_sheet_SoIndexedFaceSet* FS_vis = (fs_sheet_SoIndexedFaceSet*) hitObject;
    dbsk3d_fs_sheet* FS = (dbsk3d_fs_sheet*) FS_vis->element();
    FS->getInfo (ostrm);
    vcl_cout<< ostrm.str();
    object_found = true;
  }
  //ms_node_SoSphere
  else if (hitObject->getTypeId()==ms_node_SoSphere::getClassTypeId() ) {
    ms_node_SoSphere* MN_vis = (ms_node_SoSphere*) hitObject;
    
    //if the object is from an IV file, this field will be NULL.
    if ((dbsk3d_ms_node*)MN_vis->element()) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                ", id: "<< ((dbsk3d_ms_node*)MN_vis->element())->id() << vcl_endl;
      ((dbsk3d_ms_node*)MN_vis->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();
      object_found = true;
    }
  }
  //ms_curve_SoLineSet
  else if (hitObject->getTypeId()==ms_curve_SoLineSet::getClassTypeId() ) {
    ms_curve_SoLineSet* MC_vis = (ms_curve_SoLineSet*) hitObject;

    //if the object is from an IV file, this field will be NULL.
    if ( (dbsk3d_ms_curve*)MC_vis->element() ) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                 ", id: "<< ((dbsk3d_ms_curve*)MC_vis->element())->id() << vcl_endl;
      ((dbsk3d_ms_curve*)MC_vis->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();
      object_found = true;
    }
  }
  //ms_sheet_SoIndexedFaceSet
  else if (hitObject->getTypeId()==ms_sheet_SoIndexedFaceSet::getClassTypeId()) {
    ms_sheet_SoIndexedFaceSet* MS_vis = (ms_sheet_SoIndexedFaceSet*) hitObject;

    if ( (dbsk3d_ms_sheet*) MS_vis->element() ) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                 ", id: "<< ((dbsk3d_ms_sheet*)MS_vis->element())->id() << vcl_endl;
      ((dbsk3d_ms_sheet*)MS_vis->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();
      object_found = true;
    }
  }

  //if (!object_found)
    //vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() << vcl_endl << vcl_endl;
  return object_found;
}


