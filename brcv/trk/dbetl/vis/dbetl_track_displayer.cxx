// This is brcv/trk/dbetl/vis/dbetl_track_displayer.cxx

#include "dbetl_track_displayer.h"
#include <dbetl/pro/dbetl_track_storage.h>
#include <vgui/vgui_deck_tableau.h>

#include <dbetl/dbetl_point_track.h>
#include <dbetl/dbetl_camera.h>

#include <bgui3d/bgui3d_project2d_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>

//: Create a tableau if the storage object is of type dbetl
vgui_tableau_sptr
dbetl_track_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into dbetl storage object
  dbetl_track_storage_sptr dbetl_storage;
  dbetl_storage.vertical_cast(storage);
 
  // Extract the tracks
  vcl_vector<vcl_vector<dbetl_point_track_sptr> > tracks = dbetl_storage->tracks();

  if(tracks.empty())
    return vgui_deck_tableau_new();   

  // Extract the camera matrix
  vnl_double_3x4 camera(*dbetl_storage->camera());

  SoSeparator* scene_root = new SoSeparator;
  scene_root->ref();

  SoSeparator* group = new SoSeparator;
  scene_root->addChild(group);

  SoMaterial *myMaterial = new SoMaterial;
  myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0); // Red

  SoDrawStyle* myDrawStyle = new SoDrawStyle;
  myDrawStyle->pointSize = 2.0f;

  SoCoordinate3* point_coords = new SoCoordinate3;
  SoPointSet* points = new SoPointSet;
  int coord_num = 0;
  for (unsigned int i=0; i<tracks.size(); ++i){
    for ( vcl_vector<dbetl_point_track_sptr>::const_iterator p_itr = tracks[i].begin();
          p_itr != tracks[i].end();  ++p_itr) {
      if((*p_itr)->num_points()<5)
        continue;
      vgl_point_3d<double> pt = (*p_itr)->mean_3d();
      point_coords->point.set1Value(coord_num++, pt.x(), pt.y(), pt.z());
    } 
  }

  points->numPoints.setValue(coord_num);
  group->addChild(myMaterial);
  group->addChild(myDrawStyle);
  group->addChild(point_coords);
  group->addChild(points);

  // Draw the curves

  SoMaterial *curve_material = new SoMaterial;
  curve_material->diffuseColor.setValue(0.0, 0.0, 1.0); // Blue

  SoDrawStyle* curve_drawstyle = new SoDrawStyle;
  curve_drawstyle->lineWidth = 1.0f;

  SoSeparator* curve_grp = new SoSeparator;
  group->addChild(curve_material);
  group->addChild(curve_drawstyle);
  group->addChild(curve_grp);

  for (unsigned int i=0; i<tracks.size(); ++i){
    for ( vcl_vector<dbetl_point_track_sptr>::const_iterator p_itr = tracks[i].begin();
          p_itr != tracks[i].end();  ++p_itr) {
      if((*p_itr)->num_points()<5)
        continue;

      SoCoordinate3* curve_coords = new SoCoordinate3;
      vcl_vector<vgl_point_3d<double> > pts = (*p_itr)->curve_points();
      for( unsigned int j=0; j<pts.size(); ++j)
        curve_coords->point.set1Value(j, pts[j].x(), pts[j].y(), pts[j].z());
  
      SoLineSet* curve = new SoLineSet;
      curve->numVertices.setValue(pts.size());

      curve_grp->addChild(curve_coords);
      curve_grp->addChild(curve);
    }
  }




 
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

