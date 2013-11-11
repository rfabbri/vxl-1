//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_fmm.cxx
//  MingChing Chang
//  Aug. 09, 2005.

#include <dbgdt3d/dbgdt3d_manager.h>
#include <dbgdt3d/dbgdt3d_mesh.h>
#include <dbgdt3d/dbgdt3d_path.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>

#include <dbgdt3d/vis/dbgdt3d_vis_mesh.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoDrawStyle.h>

SoSeparator* gdt_draw_faces_vispt (dbmsh3d_gdt_mesh* mesh, 
                                   bool b_shape_hints, float trans)
{
  SoSeparator* group = new SoSeparator;

  //: put the SoShapeHints,
  //  see http://doc.coin3d.org/Coin/classSoShapeHints.html
  if (b_shape_hints) {
    SoShapeHints* hints = new SoShapeHints();
    hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
    group->addChild (hints);
  }

  //: Assign Style for search and change interactively...
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("boundary_mesh_style") );
  group->addChild (ds);

  vcl_map<int, dbmsh3d_face*>::iterator it = mesh->facemap().begin();
  for (; it != mesh->facemap().end(); it++) {
    dbmsh3d_face* face = (*it).second;
    
    draw_F_geom_vispt (group, face);
  }

  return group;
}

SoSeparator* gdt_draw_edges_vispt (dbmsh3d_gdt_mesh* gdt_mesh, float lineWidth)
{
  SoSeparator* group = new SoSeparator;

  //line width
  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName (SbName("gdt_mesh_edge_style"));
  ds->lineWidth.setValue (lineWidth);
  group->addChild(ds);

  vcl_map<int, dbmsh3d_edge*>::iterator it = gdt_mesh->edgemap().begin();
  for (; it != gdt_mesh->edgemap().end(); it++) {
    dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) (*it).second;

    //: draw edges in colors.
    SbColor color;
    SoSeparator* vis;
 
#if GDT_ALGO_F

    if (cur_edge->is_visited(gdt_mesh->i_traverse_flag())) {         
      if (cur_edge->one_dege_I()) //DEGENERATE FRONT
        color = SbColor (0.0f, 0.3f, 0.0f);
      else //ACTIVE FRONT
        color = SbColor (0.0f, 1.0f, 0.0f);

      vis = draw_edge (cur_edge, color);
      group->addChild (vis);
    }

#elif GDT_ALGO_WS

    if (cur_edge->is_propagated()) {
      if (cur_edge->one_dege_I()) //DEGENERATE
        color = SbColor (0.0f, 0.0f, 0.5f);
      else //REGULAR PROPAGATED EDGES
        color = SbColor (0.0f, 0.0f, 1.0f);

      vis = draw_edge (cur_edge, color);
      group->addChild (vis);
    }

#endif

  }

  return group;
}

SoSeparator* gdt_draw_vertices_vispt (dbmsh3d_gdt_mesh* gdt_mesh, 
                                      const float size)
{
  SoSeparator* vis = new SoSeparator;

  vcl_map<int, dbmsh3d_vertex*>::iterator it = gdt_mesh->vertexmap().begin();
  for (; it != gdt_mesh->vertexmap().end(); it++) {
    dbmsh3d_gdt_vertex_3d* vertex = (dbmsh3d_gdt_vertex_3d*) (*it).second;

    if (!vertex->is_interior()) //draw boundary vertex in light blue
      vis->addChild (draw_vertex_vispt_SoCube (vertex, SbColor(0.0f, 0.5f, 1.0f), size));
    else if (vertex->is_hyperbolic()) //draw in yellow
      vis->addChild (draw_vertex_vispt_SoCube (vertex, SbColor(1.0f, 1.0f, 0.0f), size));
    else if (vertex->is_elliptic()) //draw in green
      vis->addChild (draw_vertex_vispt_SoCube (vertex, SbColor(0.0f, 0.75f, 0.0f), size));
    else //draw planar vertex in gray
      vis->addChild (draw_vertex_vispt_SoCube (vertex, SbColor(0.5f, 0.5f, 0.5f), size));
  }

  return vis;
}
