//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_fmm.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbmsh3d/vis/dbmsh3d_vis_fmm.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoDrawStyle.h>

SoSeparator* dbmsh3d_draw_fmm_mesh (dbmsh3d_fmm_mesh* fmm_mesh)
{
  //: compute the farthest distance value to decide color
  float max_distance = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fmm_mesh->vertexmap().begin();
  for (; vit != fmm_mesh->vertexmap().end(); vit++) {
    dbmsh3d_fmm_vertex_3d* vertex = (dbmsh3d_fmm_vertex_3d*) (*vit).second;
    // DEBUG: print out the geodesic value for each vertex:
    // vul_printf (vcl_cout, "v[%d]=%f \n", vertex->id(), vertex->distance_);
    if (vertex->dist() > max_distance)
      max_distance = vertex->dist();
  }

  SoSeparator* group = new SoSeparator;

  //: if the color is non-black, assign it
  ///int pv = color.getPackedValue();
  ///if (pv != 255)
  ///material->transparency = fSurfTrans;
  
  //: Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  group->addChild (ds);

  //: draw each triangle in color w.r.t distance
  for (unsigned int i=0; i<fmm_mesh->facemap().size(); i++) {
    SoSeparator* triGroup = new SoSeparator;
    dbmsh3d_face* triangle = fmm_mesh->facemap (i);
    dbmsh3d_halfedge* he = triangle->halfedge();
    dbmsh3d_fmm_vertex_3d* v1 = (dbmsh3d_fmm_vertex_3d*) he->edge()->sV();
    dbmsh3d_fmm_vertex_3d* v2 = (dbmsh3d_fmm_vertex_3d*) he->edge()->eV();
    he = triangle->halfedge()->next();
    dbmsh3d_fmm_vertex_3d* v3 = (dbmsh3d_fmm_vertex_3d*) he->edge()->other_V (v2);

    double avg_distance = (v1->dist() + v2->dist() + v3->dist()) / 3;
    float cvalue = (float) avg_distance / max_distance; ///1.0f - 
    
    //: put the SoShapeHints,
    //  see http://doc.coin3d.org/Coin/classSoShapeHints.html
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    triGroup->addChild (hints);

    SoBaseColor* basecolor = new SoBaseColor;
    SbColor color = get_color_tone (cvalue); 
    basecolor->rgb = color;
    triGroup->addChild (basecolor);

    draw_triangle_geom (triGroup, v1->pt(), v2->pt(), v3->pt());
    group->addChild (triGroup);
  }

  return group;

    /*vcl_map<int, dbmsh3d_vertex*>::iterator vit = fmm_mesh->vertexmap().begin();
    for (; vit != fmm_mesh->vertexmap().end(); vit++) {
      dbmsh3d_fmm_vertex_3d* vertex = (dbmsh3d_fmm_vertex_3d*) (*vit).second;
      float distance = vertex->distance_;
      float c = 1.0f - distance/max_distance;
      SbColor col = SbColor (0.0f,c,c);

      _root->addChild (draw_cube (vertex->pt().x(), vertex->pt().y(), vertex->pt().z(), 
                       CMDLINE_VERTEX_RADIUS/3, col));
    }*/
}


