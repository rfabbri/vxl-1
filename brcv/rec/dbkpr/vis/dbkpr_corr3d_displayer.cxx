// This is brcv/rec/dbkpr/vis/dbkpr_corr3d_displayer.cxx

#include "dbkpr_corr3d_displayer.h"
#include <dbkpr/pro/dbkpr_corr3d_storage.h>
#include <dbdet/dbdet_keypoint_corr3d.h>
#include <vgui/vgui_deck_tableau.h>

#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterialBinding.h> 
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPointSet.h>

static SoSeparator* make_scene(const dbkpr_corr3d_storage_sptr& storage)
{
  const vcl_vector<dbdet_keypoint_corr3d_sptr>& points = storage->correspondences();
    
  if(points.empty())
    return NULL;
    
  SoSeparator* scene_root = new SoSeparator;

  SoSeparator* group = new SoSeparator;
  scene_root->addChild(group);
  
  SoCoordinate3* point_coords = new SoCoordinate3;
  SoMaterialBinding* material_bind = new SoMaterialBinding;
  SoMaterial* myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0f, 0.0f, 0.0f);  // Red
  SoPointSet* point_set = new SoPointSet;

  material_bind->value = SoMaterialBinding::OVERALL; //PER_VERTEX;
  
  int coord_num = 0;  
  for ( vcl_vector<dbdet_keypoint_corr3d_sptr>::const_iterator p_itr = points.begin();
        p_itr != points.end();  ++p_itr, ++coord_num ) {
    point_coords->point.set1Value(coord_num, (*p_itr)->x(), (*p_itr)->y(), (*p_itr)->z());
    //myMaterial->diffuseColor.set1Value(coord_num, R, G, B);
  }
  point_set->numPoints.setValue(points.size());
  group->addChild(material_bind);
  group->addChild(myMaterial);
  group->addChild(point_coords);
  group->addChild(point_set);
 
  return scene_root;
}


//: Create a tableau if the storage object is of type keypoints_corr3d
vgui_tableau_sptr
dbkpr_corr3d_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into keypoints_corr3d storage object
  dbkpr_corr3d_storage_sptr corr3d_storage;
  corr3d_storage.vertical_cast(storage);

  // cache the scene root so we don't duplicate so much when generating multiple views
  static long int chksum= -1;
  static SoSeparator* scene_root = NULL;
  
  // Extract the points
  const vcl_vector<dbdet_keypoint_corr3d_sptr>& points = corr3d_storage->correspondences();
  long int sum = 0;
  for(unsigned int i=0; i<points.size(); ++i)
    sum += reinterpret_cast<long int>(points[i].ptr());

  // Make the scene if not already made
  if(!scene_root || sum != chksum){
    chksum = sum;
    if(scene_root)
      scene_root->unref();
    // Convert the data to a 3D scene
    scene_root = make_scene(corr3d_storage);
    if(!scene_root)
      return vgui_deck_tableau_new();
    scene_root->ref();
  }

  // Extract the camera matrix
  vnl_double_3x4 camera = corr3d_storage->get_camera()->get_matrix();
  // camera might not be finite (it could be just a proj_camera) - any problems?

  bgui3d_project2d_tableau_sptr proj_tab = bgui3d_project2d_tableau_new(camera,scene_root);
  bgui3d_examiner_tableau_sptr exam_tab = bgui3d_examiner_tableau_new(scene_root);
  exam_tab->set_camera(camera);
  exam_tab->save_home_position();

  vgui_deck_tableau_sptr deck = vgui_deck_tableau_new();
  deck->add(exam_tab);
  deck->add(proj_tab);

  return deck; 
}

