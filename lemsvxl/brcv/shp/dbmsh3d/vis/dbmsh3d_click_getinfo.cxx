#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

//Be careful on the order here!
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_click_getinfo.h>

#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/events/SoKeyboardEvent.h>

//: Output Get Info of Clicked Object.
//
bool dbmsh3d_output_object_info (SoPath* path)
{
  vcl_ostringstream ostrm;
  bool object_found = false;

  //path->getTail() is the frist element got clicked!
  SoNode* hitObject = path->getTail();

  //mesh vertex
  if (hitObject->getTypeId()==dbmsh3dvis_vertex_SoCube::getClassTypeId() ) {
    dbmsh3dvis_vertex_SoCube* mesh_vertex_vis = (dbmsh3dvis_vertex_SoCube*) hitObject;

    dbmsh3d_vertex* V = (dbmsh3d_vertex*) mesh_vertex_vis->element();
    V->getInfo (ostrm);
    vcl_cout<< ostrm.str();

    object_found = true;
  }
  //: mesh edge
  else if (hitObject->getTypeId()==dbmsh3dvis_edge_SoLineSet::getClassTypeId() ) {
    dbmsh3dvis_edge_SoLineSet* mesh_edge_vis = (dbmsh3dvis_edge_SoLineSet*) hitObject;

    dbmsh3d_edge* E = (dbmsh3d_edge*) mesh_edge_vis->element();
    E->getInfo (ostrm);
    vcl_cout<< ostrm.str();

    object_found = true;
  }
  //: mesh face
  else if (hitObject->getTypeId()==dbmsh3dvis_face_SoFaceSet::getClassTypeId() ) {
    dbmsh3dvis_face_SoFaceSet* mesh_face_vis = (dbmsh3dvis_face_SoFaceSet*) hitObject;

    dbmsh3d_face* F = (dbmsh3d_face*) mesh_face_vis->element();
    F->getInfo (ostrm);
    vcl_cout<< ostrm.str();

    object_found = true;
  }
  //: gdt_shock
  /*else if (hitObject->getTypeId()==gdtvis_shock_SoLineSet::getClassTypeId() ) {
    gdtvis_shock_SoLineSet* visS = (gdtvis_shock_SoLineSet*) hitObject;

    if ((gdt_shock*) visS->element()) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                 ", id: "<< ((gdt_shock*)visS->element())->id() << vcl_endl;
      ((gdt_shock*)visS->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();

      object_found = true;
    }
  }
  //: gdt_welm
  else if (hitObject->getTypeId()==gdtvis_welm_SoLineSet::getClassTypeId() ) {
    gdtvis_welm_SoLineSet* visW = (gdtvis_welm_SoLineSet*) hitObject;

    if ((gdt_welm*) visW->element()) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                 ": "<< visW->element() << vcl_endl;
      ((gdt_welm*)visW->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();

      object_found = true;
    }
  }
  //: gdt_interval
  else if (hitObject->getTypeId()==gdtvis_interval_SoLineSet::getClassTypeId() ) {
    gdtvis_interval_SoLineSet* visI = (gdtvis_interval_SoLineSet*) hitObject;

    if ((gdt_interval*) visI->element()) {
      vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() <<
                 ": "<< visI->element() << vcl_endl;
      ((gdt_interval*)visI->element())->getInfo (ostrm);
      vcl_cout<< ostrm.str();

      object_found = true;
    }
  }*/

  if (!object_found)
    vcl_cout<< "\nSelected " << hitObject->getTypeId().getName().getString() << vcl_endl << vcl_endl;
  return object_found;
}


