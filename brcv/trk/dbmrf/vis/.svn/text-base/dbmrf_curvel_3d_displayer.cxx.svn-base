// This is brcv/trk/dbmrf/vis/dbmrf_curvel_3d_displayer.cxx

#include "dbmrf_curvel_3d_displayer.h"
#include <dbmrf/pro/dbmrf_curvel_3d_storage.h>
#include <vgui/vgui_deck_tableau.h>

#include <vcl_set.h>
#include <vcl_list.h>
#include <bmrf/bmrf_curvel_3d.h>
#include <bmrf/bmrf_curve_3d.h>

#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include "SoCurvel3D.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterialBinding.h> 
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

static SoSeparator* make_scene(const dbmrf_curvel_3d_storage_sptr& storage)
{
  // Extract the curves
  vcl_set<bmrf_curve_3d_sptr> curves;
  storage->get_curvel_3d(curves);
  
  if(curves.empty())
    return NULL;
    
  SoSeparator* scene_root = new SoSeparator;

  SoSeparator* group = new SoSeparator;
  scene_root->addChild(group);
  for ( vcl_set<bmrf_curve_3d_sptr>::const_iterator c_itr = curves.begin();
        c_itr != curves.end();  ++c_itr ) {
    SoCoordinate3* line_coords = new SoCoordinate3;
    SoMaterialBinding* material_bind = new SoMaterialBinding;
    SoMaterial* vertex_material = new SoMaterial;
    SoCurvel3D* line = new SoCurvel3D(*c_itr);

    material_bind->value = SoMaterialBinding::PER_VERTEX;
    int coord_num = 0;
    for ( bmrf_curve_3d::const_iterator p_itr = (*c_itr)->begin();
          p_itr != (*c_itr)->end();  ++p_itr, ++coord_num ) {
      line_coords->point.set1Value(coord_num, (*p_itr)->x(), (*p_itr)->y(), (*p_itr)->z());
      vertex_material->diffuseColor.set1Value(coord_num, 1.0 , 1.0 - (*p_itr)->proj_error(), 1.0 - (*p_itr)->proj_error());
    }
    line->numVertices.setValue((*c_itr)->size());
    group->addChild(material_bind);
    group->addChild(line_coords);
    group->addChild(vertex_material);
    group->addChild(line);
  }

  // Make a bounding box
  vnl_double_4x4 xform = storage->bb_xform();
  SoSeparator* bbox = new SoSeparator;
  bbox->setName("Bounding_Box");
  SoTransform* transform = new SoTransform;
  transform->setMatrix(SbMatrix(xform(0,0), xform(1,0), xform(2,0), xform(3,0),
                                xform(0,1), xform(1,1), xform(2,1), xform(3,1),
                                xform(0,2), xform(1,2), xform(2,2), xform(3,2),
                                xform(0,3), xform(1,3), xform(2,3), xform(3,3)));
  bbox->addChild(transform);

  // add unit cube
  static float boxVerts[8][3] = {
    { 0.0f,  0.0f,  1.0f },
    { 1.0f,  0.0f,  1.0f },
    { 0.0f,  1.0f,  1.0f },
    { 1.0f,  1.0f,  1.0f },
    { 0.0f,  0.0f,  0.0f },
    { 1.0f,  0.0f,  0.0f },
    { 0.0f,  1.0f,  0.0f },
    { 1.0f,  1.0f,  0.0f }
  };

  SoVertexProperty* vertex_list = new SoVertexProperty;
  vertex_list->vertex.setValues(0, 8, boxVerts);

  SoIndexedLineSet* box_lines = new SoIndexedLineSet;
  static int32_t cubeIndex[36] = {
    0, 1, -1,              // -1 terminates the edge.
    2, 3, -1,
    0, 2, -1,
    1, 3, -1,
    4, 5, -1,
    6, 7, -1,
    4, 6, -1,
    5, 7, -1,
    0, 4, -1,
    1, 5, -1,
    2, 6, -1,
    3, 7, -1,
  };

  box_lines->coordIndex.setValues(0, 36, cubeIndex);


  SoMaterialBinding* material_bind = new SoMaterialBinding;
  material_bind->value = SoMaterialBinding::OVERALL;
  bbox->addChild(material_bind);
    // Add a red material
  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(0.0f, 0.0f, 1.0f);  // Red
  bbox->addChild(myMaterial);

  bbox->addChild(vertex_list);
  bbox->addChild(box_lines);

  scene_root->addChild(bbox);

  return scene_root;
}


//: Create a tableau if the storage object is of type image
vgui_tableau_sptr
dbmrf_curvel_3d_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into curvel_3d storage object
  dbmrf_curvel_3d_storage_sptr curvel_3d_storage;
  curvel_3d_storage.vertical_cast(storage);

  static bmrf_curve_3d_sptr last_curve = NULL;
  static SoSeparator* scene_root = NULL;
  
  // Extract the curves
  vcl_set<bmrf_curve_3d_sptr> curves;
  curvel_3d_storage->get_curvel_3d(curves);

  // Make the scene if not already made
  if(!last_curve || last_curve != *curves.begin()){
    last_curve = *curves.begin();
    if(scene_root)
      scene_root->unref();
    // Convert the data to a 3D scene
    scene_root = make_scene(curvel_3d_storage);
    if(!scene_root)
      return vgui_deck_tableau_new();
    scene_root->ref();
  }

  // Extract the camera matrix
  vnl_double_3x4 camera = curvel_3d_storage->camera();

  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,scene_root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(scene_root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();

  vgui_deck_tableau_sptr deck = vgui_deck_tableau_new();
  deck->add(exam_tab);
  deck->add(proj_tab);

  scene_root->unref();

  return deck; 
}

