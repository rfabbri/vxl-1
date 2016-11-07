// MingChing Chang 040226
// VISUALIZATION OF THE fine-scale shocks

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_triangle.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <dbsk3d/vis/dbsk3d_vis_fs_flow_complex.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_mesh.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>

#include <Inventor/SbColor.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/actions/SoSearchAction.h>

// option 
//  -1: draw all.
//   1: only index-1 points.
//   2: only index-2 points.
//   3: only index-3 points.
//   4: index-1 + index-2 points.
//   5: index-1 + index-3 points.
//   6: index-2 + index-3 points.
//
SoSeparator* vis_fs_flow_complex (dbsk3d_fs_mesh* fs_mesh, 
                                  const int option, const bool draw_shock_elm,
                                  const float pt_size, const float transp,
                                  const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //Index 1 critical points (A12-2 sources): Blue.    
  if (option == 1 || option == 4 || option == 5 || option == -1)
    root->addChild (vis_fc_i1_critical_pts (fs_mesh, draw_shock_elm, pt_size, SbColor(0.0f, 0.0f, 1.0f)));

  //Index 2 critical points (A13-2 points): Green.    
  if (option == 2 || option == 4 || option == 6 || option == -1)
    root->addChild (vis_fc_i2_critical_pts (fs_mesh, draw_shock_elm, pt_size, SbColor(0.0f, 0.8f, 0.0f)));

  //Index 3 critical points (A14-4 sinks): Red.    
  if (option == 3 || option == 5 || option == 6 || option == -1)
    root->addChild (vis_fc_i3_critical_pts (fs_mesh, draw_shock_elm, pt_size, SbColor(1.0f, 0.0f, 0.0f)));

  return root;
}

SoSeparator* vis_fc_i1_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_dual,
                                     const float pt_size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Index 1 critical points:
  // - our A12-2 sources.
  // - the intersection point of a Delaunay edge and its dual Voronoi facet.
  vcl_vector<vgl_point_3d<double> > i1_critical_pts;

  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  root->addChild (hints);

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = pt_size;
  root->addChild(drawstyle);

  //Go through all shock sheets and visualize all A12-2 sources.
  // - the mid_pt (G0, G1) is inside the sheet.  
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    vcl_vector<dbmsh3d_vertex*> vertices;
    FF->get_bnd_Vs (vertices);

    if (FF->contain_A12_2 (vertices)) {
      i1_critical_pts.push_back (FF->mid_pt());

      //draw FF only if FF is bounded.
      if (show_dual && FF->b_finite())
        draw_fs_face_geom (root, FF);
    }
  }

  //Draw all critical points.
  draw_ptset_geom (root, i1_critical_pts);
  vul_printf (vcl_cout, "# flow complex index-1 critical points: %d.\n", i1_critical_pts.size());
  return root;
}

SoSeparator* vis_fc_i2_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_link,
                                     const float pt_size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Index 2 critical points:
  // - our A13-2 sources.
  // - the intersection point of a Delaunay facet and its dual Voronoi edge.
  vcl_vector<vgl_point_3d<double> > i2_critical_pts;

  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = pt_size;
  root->addChild(drawstyle);

  //Go through all shock links and visualize all A13-2 sources.
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;  
    if (FE->b_inf())
      continue; //skip the 'infinite' shock links.

    vgl_point_3d<double> C;
    FE->compute_circum_cen (C);  
    
    if (FE->detect_flow_type(C) == '1') {
      i2_critical_pts.push_back (C);

      if (show_link)
        draw_fs_edge_geom (root, FE);
    }
  }

  //Draw all critical points.
  draw_ptset_geom (root, i2_critical_pts);
  vul_printf (vcl_cout, "# flow complex index-2 critical points: %d.\n", i2_critical_pts.size());
  return root;
}

SoSeparator* vis_fc_i3_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_node,
                                     const float pt_size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Index 3 critical points:
  // - our A14-4 sources.
  // - the local maximum of distance functions.
  vcl_vector<vgl_point_3d<double> > i3_critical_pts;

  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  root->addChild (hints);

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = pt_size;
  root->addChild(drawstyle);

  //Go through all shock nodes and visualize all A14-4 sinks.
  vcl_map<int, dbmsh3d_vertex*>::iterator it = fs_mesh->vertexmap().begin();
  for (; it != fs_mesh->vertexmap().end(); it++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*it).second;  
    if (FV->b_inf())
      continue; //skip the 'infinite' shock nodes.
    
    ///if (FV->detect_flow_type() == FV_FT_SINK) {
    if (FV->n_out_flow_FEs() == 0) {
      i3_critical_pts.push_back (FV->pt());
      if (show_node)
        draw_fs_vertex_geom (root, FV, 0.1f);
    }
  }

  //Draw all critical points.
  draw_ptset_geom (root, i3_critical_pts);
  vul_printf (vcl_cout, "# flow complex index-3 critical points: %d.\n", i3_critical_pts.size());
  return root;
}

//############################################################################

SoSeparator* vis_fs_gabriel_graph (dbsk3d_fs_mesh* fs_mesh, const float width,
                                   const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Gaberial edges: connecting 2 generators and containing A12-2 points.
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > g_edges;

  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->lineWidth = width;
  root->addChild (drawstyle);

  //Go through all shock sheets and draw all Gabriel edges (with A12-2 sources).
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    vcl_vector<dbmsh3d_vertex*> vertices;
    FF->get_bnd_Vs (vertices);

    if (FF->contain_A12_2 (vertices))
      g_edges.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FF->genes(0)->pt(), FF->genes(1)->pt()));
  }

  //Draw all Gaberial edges.
  root->addChild (draw_line_set (g_edges, color));
  vul_printf (vcl_cout, "# Gabriel edges (correspond. to index-1 critical pts): %d.\n", g_edges.size());
  return root;
}

SoSeparator* vis_fs_A12_2_pts (dbsk3d_fs_mesh* fs_mesh, const float size,
                               const SbColor& color)
{
  SoSeparator* root = new SoSeparator;
  //Go through all shock sheets and draw all A12-2 sources.
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    if (FF->flow_type() == FF_FT_I_A12_2) {
      vgl_point_3d<double> M = FF->mid_pt ();
      root->addChild (draw_cube (M, size, color));
    }
  }
  return root;
}


