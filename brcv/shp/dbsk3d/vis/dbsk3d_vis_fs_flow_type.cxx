// MingChing Chang 040226
// Visualization of fine shock flow type

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_triangle.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <dbsk3d/algo/dbsk3d_fs_algo.h>

#include <dbsk3d/vis/dbsk3d_vis_fs_flow_type.h>
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

//############################################################################
//###########  Visualizing surface in-flow  ###########

SoSeparator* vis_fs_surface_inflow_Ls (dbsk3d_fs_mesh* fs_mesh, const bool user_defined_class)
{
  vul_printf (vcl_cout, "\n  vis_fs_surface_inflow_Ls(): %u fs_edges\n", 
              fs_mesh->edgemap().size());
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  unsigned int n_L_inf = 0;
  unsigned int n_L_drawn = 0;

  unsigned int min_ng = 10000;
  unsigned int max_ng = 0;
  
  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh->edgemap().begin();
  for (; lit != fs_mesh->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;

    if (FE->b_inf()) {
      n_L_inf++;
      continue;
    }

    unsigned int ng = FE->n_asgn_Gs();

    if (ng==0)
      continue;
    else if (ng==1) {
    }
    else if (ng==2) { //Gray
      ///root->addChild (draw_fs_edge (FE, SbColor (0.5, 0.5, 0.5), true, user_defined_class));
    }
    else {
      n_L_drawn++;
      if (ng==3) //Blue
        root->addChild (draw_fs_edge (FE, SbColor (0, 0, 1), true, user_defined_class));
      else if (ng==4) //Light Blue
        root->addChild (draw_fs_edge (FE, SbColor (0, 0.5, 1), true, user_defined_class));
      else if (ng==5) //Cyan
        root->addChild (draw_fs_edge (FE, SbColor (0, 1, 1), true, user_defined_class));
      else if (ng==6) //Cyan-Green
        root->addChild (draw_fs_edge (FE, SbColor (0, 1, 0.5), true, user_defined_class));
      else if (ng==7) //Green
        root->addChild (draw_fs_edge (FE, SbColor (0, 1, 0), true, user_defined_class));
      else if (ng==8) //Green-Yellow
        root->addChild (draw_fs_edge (FE, SbColor (0.5, 1, 0), true, user_defined_class));
      else if (ng==9) //Yellow
        root->addChild (draw_fs_edge (FE, SbColor (1, 1, 0), true, user_defined_class));
      else if (ng==10) //Orange
        root->addChild (draw_fs_edge (FE, SbColor (1, 0.5, 0), true, user_defined_class));
      else if (ng>10) //Red
        root->addChild (draw_fs_edge (FE, SbColor (1, 0, 0), true, user_defined_class));
      else
        assert (0);
    }

    if (ng < min_ng && ng != 0)
      min_ng = ng;
    if (ng > max_ng)
      max_ng = ng;
  }

  vul_printf (vcl_cout, "\tn_L_inf: %u.\n", n_L_inf);
  vul_printf (vcl_cout, "\tn_L_drawn: %u.\n", n_L_drawn);
  
  vul_printf (vcl_cout, "\n\tmin. in-flow gene for fs_edge: %u.\n", min_ng);
  vul_printf (vcl_cout, "\tmax. in-flow gene for fs_edge: %u.\n", max_ng);

  return root;
}

SoSeparator* vis_fs_surface_inflow_Ps (dbsk3d_fs_mesh* fs_mesh, 
                                       const float width, const float cube_size,
                                       const bool user_defined_class)
{
  vul_printf (vcl_cout, "\n  vis_fs_surface_inflow_Ps(): %u fs_faces\n", 
              fs_mesh->facemap().size());
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  unsigned int n_visited = 0;
  unsigned int n_unvisited = 0;
  
  vcl_map<int, dbmsh3d_face*>::iterator lit = fs_mesh->facemap().begin();
  for (; lit != fs_mesh->facemap().end(); lit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*lit).second;

    if (FF->b_visited()) {
      ///root->addChild (draw_fs_face (FF, SbColor (1, 1, 0), 0.0f, user_defined_class));
      root->addChild (vis_P_inflow_gene_prop (FF, width, cube_size));
      n_visited++;
    }
    else {
      ///root->addChild (draw_fs_face (FF, SbColor (0.5, 0.5, 0.5), 0.0f, user_defined_class));
      n_unvisited++;
    }
  }

  vul_printf (vcl_cout, "\tn_visited: %u.\n", n_visited);
  vul_printf (vcl_cout, "\tn_unvisited: %u.\n", n_unvisited);

  return root;
}

//#############################################################################
//###########  Visualize the inflow-gene-propagation graph ###########

SoSeparator* vis_P_inflow_gene_prop (dbsk3d_fs_face* FF, 
                                     const float width, const float cube_size)
{
  SoSeparator* root = new SoSeparator;

  SoDrawStyle* drawStyle = new SoDrawStyle;    
  drawStyle->lineWidth = width;
  root->addChild (drawStyle);

  if (FF->flow_type() == FF_FT_I_A12_2) {
    //Type I: draw all lines connecting the A12-2 src to A13-2 or A13-3 pts.
    vgl_point_3d<double> M = FF->mid_pt ();
    root->addChild (draw_cube (M, cube_size, SbColor (0, 0, 0.8f)));

    dbmsh3d_halfedge* HE = FF->halfedge();  
    assert (HE->next());
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      if (FE->flow_type() == FE_FT_I_A13_2_ACUTE) {
        vgl_point_3d<double> C;
        FE->compute_circum_cen (C);
        //Connect MC.
        root->addChild (draw_line (M, C, SbColor (0, 1.0f, 1.0f)));
        //draw the A12-2 src
        root->addChild (draw_cube (C, cube_size, SbColor (0, 1.0f, 0)));
      }
      else if (FE->flow_type() == FE_FT_II_A13_3_OBT) {
        vgl_point_3d<double> C;
        FE->compute_circum_cen (C);
        //Connect MC.
        root->addChild (draw_line (M, C, SbColor (0, 1.0f, 1.0f)));
        //draw the A12-2 src
        root->addChild (draw_cube (C, cube_size, SbColor (0.5f, 0.5f, 0)));
      }    
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }
  else if (FF->flow_type() == FF_FT_II_A13_3) {
    //Type II: draw all lines connecting the incoming A13-3 relay to A13-2 or A13-3 pts. 
    dbsk3d_fs_edge* Li = FF->type_II_find_incoming_FE ();
    vgl_point_3d<double> LiC;
    Li->compute_circum_cen (LiC);
    ///root->addChild (draw_cube (LiC, cube_size, SbColor (0, 0, 0.8f)));

    dbmsh3d_halfedge* HE = FF->halfedge();  
    assert (HE->next());
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      if (FE == Li) { //Skip the incoming A13-3.
        HE = HE->next();
        continue;
      }
      if (FE->flow_type() == FE_FT_I_A13_2_ACUTE) {
        vgl_point_3d<double> C;
        FE->compute_circum_cen (C);
        //Connect MC.
        root->addChild (draw_line (LiC, C, SbColor (0, 0.5f, 0.5f)));
        //draw the A12-2 src
        root->addChild (draw_cube (C, cube_size, SbColor (0, 1.0f, 0)));
      }
      else if (FE->flow_type() == FE_FT_II_A13_3_OBT) {
        vgl_point_3d<double> C;
        FE->compute_circum_cen (C);
        //Connect MC.
        root->addChild (draw_line (LiC, C, SbColor (0, 0.5f, 0.5f)));
        //draw the A12-2 src
        root->addChild (draw_cube (C, cube_size, SbColor (0.5f, 0.5f, 0)));
      }    
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }

  return root;
}

SoSeparator* vis_L_inflow_gene_prop (dbsk3d_fs_edge* FE)
{
  SoSeparator* root = new SoSeparator;

  if (FE->flow_type() == FE_FT_I_A13_2_ACUTE) {
    //Type I: draw 2 line connecting the A12-2 src to 2 A14-4 sinks.

  }
  else if (FE->flow_type() == FE_FT_II_A13_3_OBT) {
    //Type II: draw all lines connecting the incoming A13-3 relay to A13-2 or A13-3 pts. 
  }

  return root;
}

//############################################################################
//###########  Visualizing the flow type of fs_faces  ###########

SoSeparator* vis_fs_patches_flow_type (dbsk3d_fs_mesh* fs_mesh)
{
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //Go through each fs_face and identify their flow type.
  compute_fs_patches_flow_type (fs_mesh);

  //Draw each fs_face
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    SbColor col;
    if (FF->flow_type() == '1')
      col = SbColor (0, 0, 1.0f); //Blue
    else if (FF->flow_type() == '2')
      col = SbColor (0, 1.0f, 0); //Green
    else if (FF->flow_type() == '3')
      col = SbColor (1.0f, 1.0f, 0); //Yellow
    else
      assert (0);

    root->addChild (draw_fs_face (FF, col, 0.0f, true));
  }

  return root;
}

SoSeparator* vis_fs_links_flow_type (dbsk3d_fs_mesh* fs_mesh)
{
  SoSeparator* root = new SoSeparator;

  //Go through each fs_edge and identify their flow type.
  compute_fs_links_flow_type (fs_mesh);

  //Draw each fs_edge
  vcl_map<int, dbmsh3d_edge*>::iterator eit = fs_mesh->edgemap().begin();
  for (; eit != fs_mesh->edgemap().end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    SbColor col;

    if (FE->flow_type() == 'I') {
      //col = SbColor (0.5f, 0.5f, 0.5f); //Gray
      continue; //Skip drawing the infinity fs_edges.
    }
    else {
      if (FE->flow_type() == '1') 
        col = SbColor (0, 0, 1.0f); //Blue
      else if (FE->flow_type() == '2')
        col = SbColor (0, 1.0f, 0); //Green
      else if (FE->flow_type() == '3')
        col = SbColor (1.0f, 0, 1.0f); //Pink
      else {
        assert (0);        
      }
      root->addChild (draw_fs_edge (FE, col, false, true));
    }
  }

  return root;
}

SoSeparator* vis_fs_nodes_iof_type (dbsk3d_fs_mesh* fs_mesh, const float pt_size)
{
  SoSeparator* root = new SoSeparator;

  //Draw each type of fs_vertices in a group.
  vcl_vector<vgl_point_3d<double> > N_type_4, N_type_3, N_type_2;

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    if (FV->is_inf_from_FE())
      continue;

    int n_out = FV->n_out_flow_FEs();
    if (n_out == 0)
      N_type_4.push_back (FV->pt());
    else if (n_out == 1)
      N_type_3.push_back (FV->pt());
    else if (n_out == 2)
      N_type_2.push_back (FV->pt());
  }

  root->addChild (draw_ptset (N_type_4, COLOR_RED, pt_size*2));
  root->addChild (draw_ptset (N_type_3, COLOR_YELLOW, pt_size));
  root->addChild (draw_ptset (N_type_2, COLOR_GREEN, pt_size));

  return root;
}

SoSeparator* vis_fs_nodes_iof_type_idv (dbsk3d_fs_mesh* fs_mesh, const float size)
{
  SoSeparator* root = new SoSeparator;

  //Draw each fs_vertex
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    if (FV->is_inf_from_FE())
      continue;

    SbColor col;
    int n_out = FV->n_out_flow_FEs();    
    if (n_out == 0)
      col = SbColor (1.0f, 0, 0); //Red
    else if (n_out == 1)
      col = SbColor (1.0f, 1.0f, 0); //Yellow
    else if (n_out == 2)
      col = SbColor (0, 1.0f, 0); //Green

    root->addChild (draw_fs_vertex (FV, col, size, true));
  }

  return root;
}

VIS_COLOR_CODE get_N_flow_type_ccode (const dbsk3d_fs_vertex* FV)
{
  assert (FV->flow_type() != FV_FT_INF);

  switch (FV->flow_type()) {
  case FV_FT_4641: //'1'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4641.\n", FV->id());
    #endif
    return COLOR_RED;
  break;
  case FV_FT_4631: //'2'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4631.\n", FV->id());
    #endif
    return COLOR_RED;
  break;
  case FV_FT_4531: //'3'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4531.\n", FV->id());
    #endif
    return COLOR_ORANGE;
  break;
  case FV_FT_4520: //'4'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4520.\n", FV->id());
    #endif
    return COLOR_ORANGE;
  break;
  case FV_FT_4421_O: //'5'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4421_O.\n", FV->id());
    #endif
    return COLOR_PURPLE;
  break;
  case FV_FT_4410_O: //'6'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4410_O.\n", FV->id());
    #endif
    return COLOR_GREEN;
  break;
  case FV_FT_4410_P: //'7'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4410_P.\n", FV->id());
    #endif
    return COLOR_BLUE;
  break;
  case FV_FT_4300_T: //'8'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4300_T.\n", FV->id());
    #endif
    return COLOR_CYAN;
  break;
  case FV_FT_4300_L: //'9'
    #if DBMSH3D_DEBUG > 4
    vul_printf (vcl_cout, "FV %d flow_type FV_FT_4300_L.\n", FV->id());
    #endif
    return COLOR_PINK;
  break;
  case FV_FT_UNCLASSIFIED: //'U' Gray
    ///vul_printf (vcl_cout, "FV %d flow_type FV_FT_UNCLASSIFIED.\n", FV->id());
    return COLOR_GRAY;
  break;
  default:
    assert (0);
  break;
  }
  return COLOR_BLACK;
}

SoSeparator* vis_fs_nodes_flow_type (dbsk3d_fs_mesh* fs_mesh, const float pt_size)
{
  SoSeparator* root = new SoSeparator;
  compute_fs_nodes_flow_type (fs_mesh);

  unsigned int n_ft_inf = 0;

  //Visualize each fs_vertex in color.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    if (FV->flow_type() == FV_FT_INF) { //'I' For shock vertex at infinity. Skip
      n_ft_inf++;
      continue;
    }

    VIS_COLOR_CODE ccode = get_N_flow_type_ccode (FV);
    SbColor col = color_from_code (ccode);
    
    //draw_fs_vertex (FV, col, const float size,
                            //const bool user_defined_class = true);
  }

  return root;
}

SoSeparator* vis_fs_nodes_flow_type_idv (dbsk3d_fs_mesh* fs_mesh, const float size)
{
  SoSeparator* root = new SoSeparator;
  compute_fs_nodes_flow_type (fs_mesh);

  unsigned int n_ft_inf = 0;
  unsigned int n_ft_4641 = 0;
  unsigned int n_ft_4631 = 0;
  unsigned int n_ft_4531 = 0;
  unsigned int n_ft_4520 = 0;
  unsigned int n_ft_4421_O = 0;
  unsigned int n_ft_4410_O = 0;
  unsigned int n_ft_4410_P = 0;
  unsigned int n_ft_4300_T = 0;
  unsigned int n_ft_4300_L = 0;
  unsigned int n_ft_unclassified = 0;
  
  //Visualize each fs_vertex in color.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    if (FV->flow_type() == FV_FT_INF) { //'I' For shock vertex at infinity. Skip
      n_ft_inf++;
      continue;
    }

    switch (FV->flow_type()) {
    case FV_FT_4641:    n_ft_4641++; break;
    case FV_FT_4631:    n_ft_4631++; break;
    case FV_FT_4531:    n_ft_4531++; break;
    case FV_FT_4520:    n_ft_4520++; break;
    case FV_FT_4421_O:  n_ft_4421_O++; break;
    case FV_FT_4410_O:  n_ft_4410_O++; break;
    case FV_FT_4410_P:  n_ft_4410_P++; break;
    case FV_FT_4300_T:  n_ft_4300_T++; break;
    case FV_FT_4300_L:  n_ft_4300_L++; break;
    case FV_FT_UNCLASSIFIED: n_ft_unclassified++; break;
    default:
      assert (0);
    break;
    }

    VIS_COLOR_CODE ccode = get_N_flow_type_ccode (FV);
    SbColor col = color_from_code (ccode);    
    root->addChild (draw_fs_vertex (FV, col, size));
  }

  vul_printf (vcl_cout, "\nvis_fs_nodes_flow_type_idv(): %d total fs_vertices:\n", 
              fs_mesh->vertexmap().size());

  vul_printf (vcl_cout, "  n_ft_inf    (hidden): %d\n", n_ft_inf);
  vul_printf (vcl_cout, "  n_ft_4641   (1, red): %d\n", n_ft_4641);
  vul_printf (vcl_cout, "  n_ft_4631   (2, red): %d\n", n_ft_4631);
  vul_printf (vcl_cout, "  n_ft_4531   (3, orange): %d\n", n_ft_4531);
  vul_printf (vcl_cout, "  n_ft_4520   (4, orange): %d\n", n_ft_4520);
  vul_printf (vcl_cout, "  n_ft_4421_O (5, purple): %d\n", n_ft_4421_O);
  vul_printf (vcl_cout, "  n_ft_4410_O (6, green): %d\n", n_ft_4410_O);
  vul_printf (vcl_cout, "  n_ft_4410_P (7, blue): %d\n", n_ft_4410_P);
  vul_printf (vcl_cout, "  n_ft_4300_T (8, cyan): %d\n", n_ft_4300_T);
  vul_printf (vcl_cout, "  n_ft_4300_L (9, pink): %d\n", n_ft_4300_L);
  vul_printf (vcl_cout, "  n_ft_unclassified (gray): %d\n", n_ft_unclassified);

  return root;
}

//############################################################################
//###########  Visualizing the flow type of fs_edges  ###########

//: Visualize various types of A13 shock curves.
// type == 1: type I, type == 2: type II, type == 3: type III.
SoSeparator* vis_fs_links_type (dbsk3d_fs_mesh* fs_mesh, const int type,
                                const float len)
{
  SoSeparator* root = new SoSeparator;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > links;
  
  char ctype;
  if (type == 1)
    ctype = '1';
  else if (type == 2)
    ctype = '2';
  else if (type == 3)
    ctype = '3';
  else
    assert (0);

  //Color code:
  SbColor col;
  if (type == 1)
    col = SbColor (0, 1, 0); //Green
  else if (type == 2)
    col = SbColor (0.5f, 0.5f, 0); //Yellow
  else if (type == 3)
    col = SbColor (1, 0.5f, 0); //Orange
  else
    assert (0);

  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;      
    if (FE->detect_flow_type() == ctype) {
      vgl_point_3d<double> C;
      FE->compute_circum_cen (C);
      vgl_vector_3d<double> sv = FE->s_FV()->pt() - C;
      sv /= sv.length ();
      vgl_vector_3d<double> ev = FE->e_FV()->pt() - C;
      ev /= ev.length ();
      vgl_point_3d<double> s_FV = C + sv * len;
      vgl_point_3d<double> e_FV = C + ev * len;
      links.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (s_FV, e_FV));
    }
  }
  root->addChild (draw_line_set (links, col));

  return root;
}

//: Visualize various types of A13 shock's dual triangles.
// type == 1: type I, type == 2: type II, type == 3: type III.
SoSeparator* vis_fs_links_type_tris (dbsk3d_fs_mesh* fs_mesh, 
                                     const int type,
                                     const bool view_only_large_tris)
{
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  root->addChild (hints);

  char ctype;
  if (type == 1)
    ctype = '1';
  else if (type == 2)
    ctype = '2';
  else if (type == 3)
    ctype = '3';
  else
    assert (0);

  //Color code:
  SbColor col;
  if (type == 1)
    col = SbColor (0, 1, 0); //Green
  else if (type == 2)
    col = SbColor (0.5f, 0.5f, 0); //Yellow
  else if (type == 3)
    col = SbColor (1, 0.5f, 0); //Orange
  else
    assert (0);

  //Put all points into a vector.
  vcl_vector<vgl_point_3d<double> > ifs_pts;
  vcl_vector<vcl_vector<int> > ifs_faces;

  //Go through all generators and assign vid to be [0 to n-1].  
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->bnd_mesh()->vertexmap().begin();
  for (unsigned int i=0; vit != fs_mesh->bnd_mesh()->vertexmap().end(); vit++, i++) {
    dbmsh3d_vertex* G = (*vit).second;
    G->set_vid (i);
    ifs_pts.push_back (G->pt());
  }

  double tri_size_th;
  if (view_only_large_tris) {
    fs_mesh->compute_median_A122_dist();
    tri_size_th = fs_mesh->median_A122_dist() * 5; //msr;
  }

  //Go through all shock links and visualize all A13-2 sources.
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;  
    if (FE->b_inf())
      continue; //skip the 'infinite' shock links.

    if (FE->detect_flow_type() == ctype) {
      vcl_vector<dbmsh3d_vertex*> genes;
      bool result = FE->get_ordered_Gs_via_FF (genes);
      assert (genes.size() > 2);
      assert (result);
      vcl_vector<int> face_vids;
      for (unsigned int i=0; i<genes.size(); i++)
        face_vids.push_back (genes[i]->vid());

      //add to IFS faces
      if (view_only_large_tris) {
        //option: only show the large triangles.
        if (vgl_distance (genes[0]->pt(), genes[1]->pt()) > tri_size_th &&
            vgl_distance (genes[1]->pt(), genes[2]->pt()) > tri_size_th &&
            vgl_distance (genes[2]->pt(), genes[0]->pt()) > tri_size_th) 
          ifs_faces.push_back (face_vids);
      }
      else
        ifs_faces.push_back (face_vids);
    }
  }

  //Draw all corresponding triangles.
  root->addChild (draw_ifs (ifs_pts, ifs_faces, col, true));
  vul_printf (vcl_cout, "# A13 triangles %d.\n", ifs_faces.size());
  ifs_pts.clear();
  ifs_faces.clear();
  return root;
}

//-v: option. 0: show all, 1: show only type I, 2: show only type II, 
//            3: show only type III, 4: show only type I and II.
//
SoSeparator* vis_fs_links_color_nG (dbsk3d_fs_mesh* fs_mesh, const float width,
                                    const unsigned int n_genes_th,
                                    const int option,
                                    const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
  root->addChild (hints);

  //Count the max # genes of FV.
  unsigned int max_n_genes = 0;
  unsigned int min_n_genes = 1000;
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;  
    if (FE->flow_type() != '3')
      continue;

    vcl_set<dbmsh3d_vertex*> pass_genes;
    unsigned int n_genes = FE->get_pass_Gs (pass_genes);

    if (n_genes > max_n_genes)
      max_n_genes = n_genes;
    if (n_genes != 0 && n_genes < min_n_genes)
      min_n_genes =n_genes;
  }

  vul_printf (vcl_cout, "vis_fs_links_color_nG(): %u fs_edges.\n", 
              fs_mesh->edgemap().size());

  //For # genes > threshold nG_th, draw in dart red color
  vul_printf (vcl_cout, "\tmax_n_genes: %u, min_n_genes: %u.\n", max_n_genes, min_n_genes);
  vul_printf (vcl_cout, "\tn_genes_th: %u\n", n_genes_th);

  it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;
    SbColor col;

    if (FE->flow_type() == '1') {
      if (option == 0 || option == 1 || option == 4) {
        //Color the type I fs_edge in dark blue.
        col = SbColor (0, 0, 0.5f);           
        root->addChild (draw_fs_edge (FE, col, true, user_defined_class));
      }
    }
    else if (FE->flow_type() == '2') {
      if (option == 0 || option == 2 || option == 4) {
        //Color the type II fs_edge in dark green.
        col = SbColor (0, 0.5f, 0);
        root->addChild (draw_fs_edge (FE, col, true, user_defined_class));
      }
    }
    else {
      if (option == 0 || option == 3) {
        //Color the type III fs_edge by # genes with threshold.
        vcl_set<dbmsh3d_vertex*> pass_genes;
        unsigned int n_genes = FE->get_pass_Gs (pass_genes);

        if (n_genes > n_genes_th)
          col = SbColor (0.5f, 0, 0);
        if (n_genes == 0)
          col = SbColor (0, 0, 0);
        else {
          double cvalue = ((double) n_genes - min_n_genes) / n_genes_th;
          col = get_color_tone (cvalue);
        }
        root->addChild (draw_fs_edge (FE, col, true, user_defined_class));
      }
    }

  }

  return root;

}

//#############################################################################

SoSeparator* vis_fs_link_type_2_points (dbsk3d_fs_mesh* fs_mesh, const int option,
                                        const float pt_size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Index 2 critical points:
  // - our A13-2 sources.
  // - the intersection point of a Delaunay facet and its dual Voronoi edge.
  vcl_vector<vgl_point_3d<double> > slink_type_2_pts;

  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = pt_size;
  root->addChild(drawstyle);
  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  root->addChild (hints);

  //Put all points into a vector.
  vcl_vector<vgl_point_3d<double> > ifs_pts;
  vcl_vector<vcl_vector<int> > ifs_faces;

  //Go through all generators and assign vid to be [0 to n-1].  
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->bnd_mesh()->vertexmap().begin();
  for (unsigned int i=0; vit != fs_mesh->bnd_mesh()->vertexmap().end(); vit++, i++) {
    dbmsh3d_vertex* G = (*vit).second;
    G->set_vid (i);
    ifs_pts.push_back (G->pt());
  }

  //Go through all shock links and visualize all A13-2 sources.
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;  
    if (FE->b_inf())
      continue; //skip the 'infinite' shock links.

    vgl_point_3d<double> C;
    FE->compute_circum_cen (C);      
    if (FE->detect_flow_type(C) == '2') {
      vcl_vector<dbmsh3d_vertex*> genes;
      bool result = FE->get_ordered_Gs_via_FF (genes);
      assert (result);

      vcl_vector<int> face_vids;
      for (unsigned int i=0; i<genes.size(); i++)
        face_vids.push_back (genes[i]->vid());
      //add to IFS faces.
      ifs_faces.push_back (face_vids);

      slink_type_2_pts.push_back (C);
    }
  }

  //Draw all critical points.
  if (option == 1 || option == 3)
    draw_ptset_geom (root, slink_type_2_pts);
  if (option == 2 || option == 3) //Draw all A13-2 acute triangles.
    root->addChild (draw_ifs (ifs_pts, ifs_faces, COLOR_DARKYELLOW, true));

  vul_printf (vcl_cout, "# type 2 shock curves (A13-2 not in tri.): %d.\n", slink_type_2_pts.size());
  ifs_pts.clear();
  ifs_faces.clear();
  return root;
}


SoSeparator* vis_fs_link_type_3_pts (dbsk3d_fs_mesh* fs_mesh, const float pt_size, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  vcl_vector<vgl_point_3d<double> > slink_type_3_pts;

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
    if (FE->detect_flow_type(C) == '3') {
      slink_type_3_pts.push_back (C);
    }
  }

  //Draw all critical points.
  draw_ptset_geom (root, slink_type_3_pts);

  vul_printf (vcl_cout, "# type 3 shock curves (A13-2 not in tri.): %d.\n", slink_type_3_pts.size());
  return root;
}


SoSeparator* vis_fs_link_type_3 (dbsk3d_fs_mesh* fs_mesh, const int option,
                                 const float width, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  vcl_vector<vgl_point_3d<double> > slink_type_3_pts;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > slink_type_3_lines;

  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);
  
  //point size
  SoDrawStyle * drawstyle = new SoDrawStyle;
  drawstyle->pointSize = 2; //pt_size;
  root->addChild(drawstyle);

  //Go through all shock links and visualize all A13-2 sources.
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;  
    if (FE->b_inf())
      continue; //skip the 'infinite' shock links.

    vgl_point_3d<double> C;
    FE->compute_circum_cen (C);      
    if (FE->detect_flow_type(C) == '3') {
      slink_type_3_pts.push_back (C);
      slink_type_3_lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FE->s_FV()->pt(), FE->e_FV()->pt()));
    }
  }

  //Draw all critical points.
  if (option == 1 || option == 3)
    draw_ptset_geom (root, slink_type_3_pts);
  if (option == 2 || option == 3) //Draw all A13-2 acute triangles.
    root->addChild (draw_line_set (slink_type_3_lines, color));

  vul_printf (vcl_cout, "# type 3 shock curves (A13-2 not in tri.): %d.\n", slink_type_3_pts.size());
  return root;
}

//#############################################################################
//###########  Visualizing the flow type of fs_vertices  ###########

SoSeparator* vis_fs_nodes_color_nG (dbsk3d_fs_mesh* fs_mesh, const float size, 
                                    const unsigned int n_genes_th,
                                    const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //Count the max # genes of FV.
  unsigned int max_n_genes = 0;
  unsigned int min_n_genes = 1000;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = fs_mesh->vertexmap().begin();
  for (; it != fs_mesh->vertexmap().end(); it++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*it).second;  
    unsigned int nG = FV->n_asgn_Gs();
    if (nG > max_n_genes)
      max_n_genes = nG;
    if (nG != 0 && nG < min_n_genes)
      min_n_genes = nG;
  }

  vul_printf (vcl_cout, "vis_fs_nodes_color_nG(): %u fs_vertices.\n", 
              fs_mesh->vertexmap().size());

  //For # genes > threshold nG_th, draw in dart red color
  vul_printf (vcl_cout, "\tmax_n_genes: %u, min_n_genes: %u.\n", max_n_genes, min_n_genes);
  vul_printf (vcl_cout, "\tn_genes_th: %u\n", n_genes_th);

  it = fs_mesh->vertexmap().begin();
  for (; it != fs_mesh->vertexmap().end(); it++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*it).second;
    unsigned int nG = FV->n_asgn_Gs();
    SbColor col;

    //Color the fs_vertex by # genes with threshold.
    if (nG > n_genes_th)
      col = SbColor (0.5f, 0, 0);
    if (nG == 0)
      col = SbColor (0, 0, 0);
    else {
      double cvalue = ((double) nG - min_n_genes) / n_genes_th;
      col = get_color_tone (cvalue);
    }

    root->addChild (draw_fs_vertex (FV, col, size, user_defined_class));
  }

  return root;
}

