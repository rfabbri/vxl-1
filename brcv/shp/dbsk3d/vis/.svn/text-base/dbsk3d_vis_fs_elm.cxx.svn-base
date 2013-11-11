//MingChing Chang 040405
//  VISUALIZATION OF EACH SHOCK ELEMENT

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoTranslation.h>

SoSeparator* draw_fs_face (dbsk3d_fs_face* FF, 
                           const SbColor& color, const float transp,
                           const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoMaterial *material = new SoMaterial;
  material->setName (SbName("FF_material"));
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue(color/2);  //color, color/2
  material->transparency = transp;
  root->addChild (material);
  
  draw_fs_face_geom (root, FF, user_defined_class);
  return root;
}

void draw_fs_face_geom (SoSeparator* root, dbsk3d_fs_face* FF,
                        const bool user_defined_class)
{
  ///FF->_ifs_track_ordered_vertices ();
  SoCoordinate3* coords = _draw_F_mhe_geom (root, FF);

  if (user_defined_class) {
    fs_face_SoFaceSet* faceSet = new fs_face_SoFaceSet (FF);
    faceSet->setCoordinate3 (coords);
    root->addChild (faceSet);
  }
  else {
    SoFaceSet* faceSet = new SoFaceSet;
    root->addChild (faceSet);
  }

  ///FF->vertices().clear();
}

SoSeparator* draw_fs_edge (dbsk3d_fs_edge* FE, 
                           const SbColor& color, const float width,
                           const bool draw_dual, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;    

  //Color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  //Line width
  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);
  root->addChild (drawStyle);

  draw_fs_edge_geom (root, FE, user_defined_class);

  if (draw_dual) { //Draw the corresponding dual triangle (polygon).
    vcl_vector<dbmsh3d_vertex*> genes;
    FE->get_ordered_Gs_via_FF (genes);
    draw_filled_polygon_geom (root, genes);
  }

  return root;
}

SoSeparator* draw_fs_edge (dbsk3d_fs_edge* FE, 
                           const SoBaseColor* basecolor, const bool draw_dual,
                           const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  root->addChild ((SoBaseColor*) basecolor);

  draw_fs_edge_geom (root, FE, user_defined_class);
  if (draw_dual) { //Draw the corresponding dual triangle (polygon).
    vcl_vector<dbmsh3d_vertex*> genes;
    FE->get_ordered_Gs_via_FF (genes);
    draw_filled_polygon_geom (root, genes);
  }

  return root;
}

void draw_fs_edge_geom (SoSeparator* root, dbsk3d_fs_edge* FE,
                        const bool user_defined_class)
{
  SoCoordinate3* coords = new SoCoordinate3;
  coords->point.set1Value (0, (float) FE->s_FV()->pt().x(), (float) FE->s_FV()->pt().y(), (float) FE->s_FV()->pt().z() );
  coords->point.set1Value (1, (float) FE->e_FV()->pt().x(), (float) FE->e_FV()->pt().y(), (float) FE->e_FV()->pt().z() );
  root->addChild (coords);

  if (user_defined_class) {
    fs_edge_SoLineSet* lineSet = new fs_edge_SoLineSet (FE);
    lineSet->setCoordinate3 (coords);
    lineSet->numVertices.set1Value (0, 2);
    root->addChild (lineSet);
  }
  else {
    SoLineSet* lineSet = new SoLineSet;
    lineSet->numVertices.set1Value (0, 2);
    root->addChild (lineSet);
  }
}

SoSeparator* draw_fs_vertex (dbsk3d_fs_vertex* FV, 
                             const SbColor& color, const float size,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);

  draw_fs_vertex_geom (root, FV, size, user_defined_class);
  return root;
}

void draw_fs_vertex_geom (SoSeparator* root, dbsk3d_fs_vertex *FV, 
                          const float size, const bool user_defined_class)
{
  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f((float) FV->pt().x(), 
                                       (float) FV->pt().y(), 
                                       (float) FV->pt().z()));
  root->addChild (trans);

  if (user_defined_class) {
    fs_vertex_SoCube* cube = new fs_vertex_SoCube (FV);
    cube->setName (SbName("shock,node,element"));
    cube->setTranslation (trans);
    cube->width = size;
    cube->height = size;
    cube->depth = size;
    root->addChild (cube); 
  }
  else {
    SoCube* cube = new SoCube;
    cube->setName (SbName("shock,node,element"));
    cube->width = size;
    cube->height = size;
    cube->depth = size;
    root->addChild (cube); 
  }
}

