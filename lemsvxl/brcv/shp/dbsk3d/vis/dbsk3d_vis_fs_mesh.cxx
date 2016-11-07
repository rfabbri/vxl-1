// MingChing Chang 040226
// VISUALIZATION OF THE fine-scale shocks

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_triangle.h>

#include <dbsk3d/algo/dbsk3d_fs_algo.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

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

const SbColor FS_FINITE_VALID_COLOR   (0.0f, 1.0f, 1.0f); //Cyan 
///const SbColor FS_FINITE_VALID_COLOR   (1.0f, 1.0f, 0.0f); //Yellow
const SbColor FS_FINITE_PRUNED_COLOR  (0.0f, 1.0f, 0.0f); //Green
const SbColor FS_INF_VALID_COLOR      (0.5f, 0.5f, 0.5f); //DarkGray
const SbColor FS_INF_PRUNED_COLOR     (0.0f, 0.0f, 1.0f); //Blue

//: Visualize the valid shock sheets as a single mesh object.
SoSeparator* draw_fs_mesh_valid (dbsk3d_fs_mesh* fs_mesh)
{
  SoSeparator* root = new SoSeparator; 

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //Color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = FS_FINITE_VALID_COLOR;
  root->addChild (basecolor);

  //Draw only valid and finite fs_faces (mark them here).
  unsigned int n_valid_finite = 0;
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;

    if (FF->b_valid() && FF->b_finite()) {
      FF->set_visited (true);
      n_valid_finite++;
    }
    else
      FF->set_visited (false);
  }

  //Draw only the valid faces as a single mesh object.
  draw_M_visited_ifs_geom (root, fs_mesh);

  vul_printf (vcl_cout, "draw_fs_mesh_valid(): %d valid finite fs_faces (total %u).\n",
              n_valid_finite, fs_mesh->facemap().size());
  return root;
}

SoSeparator* draw_fs_mesh (dbsk3d_fs_mesh* fs_mesh, const int colorcode)
{
  return draw_M (fs_mesh, true, 0.0f, colorcode);
}

SoSeparator* draw_fs_mesh (dbsk3d_fs_mesh* fs_mesh, 
                           const int option, const bool draw_valid, 
                           const bool draw_invalid, const bool draw_unb, 
                           const float cube_size, const float transp,
                           const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator; 
  SoSwitch* fs_switch = new SoSwitch;
  root->addChild (fs_switch);
  fs_switch->whichChild = SO_SWITCH_ALL;
  fs_switch->setName( SbName( "Entire,FS" ) );

  //:If shift specified, shift the drawing by that amount.
  SoSeparator* group = new SoSeparator;
  fs_switch->addChild (group);
  
  if (option == 0) { //draw the patch elements
    SoSwitch* patch_switch = new SoSwitch();
    patch_switch->whichChild = SO_SWITCH_ALL;
    patch_switch->setName( SbName( "FS,Sheets,All" ) ); 
    group->addChild( patch_switch );

    patch_switch->addChild (draw_fs_mesh_patches (fs_mesh->facemap(), draw_valid,
      draw_invalid, draw_unb, true, user_defined_class));
  }
  else if (option == 1) { //draw the link elements
    SoSwitch* curveSwitch = new SoSwitch();
    curveSwitch->whichChild = SO_SWITCH_ALL;
    curveSwitch->setName( SbName( "FS,Links,All" ) );
    group->addChild( curveSwitch );
  
    curveSwitch->addChild (draw_fs_mesh_links (fs_mesh->edgemap(), 0, FLT_MAX, 
      draw_invalid, draw_unb, 3, true, user_defined_class));
  }
  else if (option == 2) { //draw the node elements
    SoSwitch* nodeSwitch = new SoSwitch();
    nodeSwitch->whichChild = SO_SWITCH_ALL;
    nodeSwitch->setName( SbName( "FS,Nodes,All" ) );
    group->addChild( nodeSwitch );

    nodeSwitch->addChild (draw_fs_mesh_nodes (fs_mesh->vertexmap(), 
      draw_invalid, SbColor (1.0f, 0.0f, 0.0f), cube_size, user_defined_class));
  }
  return root;
}

SoSeparator* draw_fs_mesh_patches (vcl_map<int, dbmsh3d_face*>& patches, 
                                   const bool draw_valid,
                                   const bool draw_invalid, const bool draw_unb,
                                   const bool draw_unvisited,
                                   const bool user_defined_class)
{ 
  vul_printf (vcl_cout, "\ndraw_fs_mesh_patches():\n");
  vul_printf (vcl_cout, "\tdraw_invalid: %s, draw_unb: %s, draw_unvisited: %s.\n",
               draw_invalid ? "true" : "false", 
               draw_unb ? "true" : "false",
               draw_unvisited ? "true" : "false");

  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints;
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //Finite and valid, draw in Yellow
  SoSeparator* V_F_Sep = new SoSeparator;
  root->addChild (V_F_Sep);    
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = FS_FINITE_VALID_COLOR;
  V_F_Sep->addChild (basecolor);

  //Finite but pruned, draw in Green
  SoSeparator* IV_F_Sep = new SoSeparator;
  root->addChild (IV_F_Sep);
  basecolor = new SoBaseColor;
  basecolor->rgb = FS_FINITE_PRUNED_COLOR;
  IV_F_Sep->addChild (basecolor);

  //Infinite but valid, draw in DarkGray
  SoSeparator* V_IF_Sep = new SoSeparator;
  root->addChild (V_IF_Sep);
  basecolor = new SoBaseColor;
  basecolor->rgb = FS_INF_VALID_COLOR; 
  V_IF_Sep->addChild (basecolor);

  //Infinite and pruned, draw in blue
  SoSeparator* IV_IF_Sep = new SoSeparator;
  root->addChild (IV_IF_Sep);
  basecolor = new SoBaseColor;
  basecolor->rgb = FS_INF_PRUNED_COLOR; 
  IV_IF_Sep->addChild (basecolor);

  unsigned int n_V_F = 0;
  unsigned int n_IV_F = 0;
  unsigned int n_V_IF = 0;
  unsigned int n_IV_IF = 0;

  vcl_map<int, dbmsh3d_face*>::iterator pit = patches.begin();
  for (; pit != patches.end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;

    if (draw_unvisited==false && FF->b_visited()==false)
      continue; //Skip the unvisited one if specified to skip it.

    /*if (FF->id() != 2304)
      continue;
    vcl_vector<dbmsh3d_vertex*> vertices;
    FF->get_bnd_Vs (vertices);
    for (unsigned int j=0; j<vertices.size(); j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) vertices[j];
      root->addChild (draw_fs_vertex (FV, SbColor(1,0,0), 0.1));
    }*/

    if (FF->b_finite()) {
      if (draw_valid && FF->b_valid()) { //Finite and valid
        draw_fs_face_geom (V_F_Sep, FF, user_defined_class);
        n_V_F++;
      }
      if (draw_invalid && !FF->b_valid()) { //Finite but pruned
        draw_fs_face_geom (IV_F_Sep, FF, user_defined_class);
        n_IV_F++;
      }
    }
    else if (draw_unb) {
      if (draw_valid && FF->b_valid()) { //Infinite but valid (no possible to exist)
        draw_fs_face_geom (V_IF_Sep, FF, user_defined_class);  
        n_V_IF++;
      }
      if (draw_invalid && !FF->b_valid()) { //Infinite and pruned
        draw_fs_face_geom (IV_IF_Sep, FF, user_defined_class);
        n_IV_IF++;
      }
    }
  }

  vul_printf (vcl_cout, "\tAmong %d shock-sheet-elements,\n", patches.size());
  vul_printf (vcl_cout, "\t  %d finite and valid sheet-elms drawn in Yellow.\n", n_V_F);
  vul_printf (vcl_cout, "\t  %d finite but pruned sheet-elms drawn in Green.\n", n_IV_F);
  vul_printf (vcl_cout, "\t  %d infinite but valid sheet-elms drawn in DarkGray.\n", n_V_IF);
  vul_printf (vcl_cout, "\t  %d infinite and pruned sheet-elms drawn in Blue.\n", n_IV_IF);

  return root;
}

//: draw_option == -1: draw both the shock link and the surface triangle
//  draw option == 1: draw only the shock link
//  draw option == 2: draw only the surface triangle
//  
SoSeparator* draw_fs_mesh_links (vcl_map<int, dbmsh3d_edge*>& links, 
                                 const float th1, const float th2,
                                 const bool draw_invalid, const bool draw_unb,   
                                 const int draw_option,
                                 const bool draw_unvisited,
                                 const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  //color
  SoBaseColor* acuteColor = new SoBaseColor;  
  acuteColor->rgb.setValue (SbColor(0.0f, 1.0f, 0.0f)); //Green
  SoBaseColor* obtuseColor = new SoBaseColor;
  obtuseColor->rgb.setValue (SbColor(0.0f, 0.0f, 1.0f)); //Blue
  SoBaseColor* nona132Color = new SoBaseColor;
  nona132Color->rgb.setValue (SbColor(1.0f, 0.0f, 0.0f)); //Red
  
  //line width
  SoDrawStyle* drawStyle = new SoDrawStyle ();
  drawStyle->lineWidth.setValue (1); //3
  root->addChild (drawStyle);
  
  //finite links
  SoSwitch* finiteSwitch = new SoSwitch;
  finiteSwitch->whichChild = SO_SWITCH_ALL;
  finiteSwitch->setName (SbName ("FS,Links,Finite"));
  root->addChild (finiteSwitch);

  SoSeparator* finiteSep = new SoSeparator;
  finiteSwitch->addChild (finiteSep);

  //unbounded links
  SoSwitch* unbSwitch = new SoSwitch;
  unbSwitch->setName (SbName ("FS,Links,Infinite"));
  root->addChild (unbSwitch);

  SoSeparator* unbSep = new SoSeparator;
  unbSwitch->addChild (unbSep);

  //surface triangles
  SoSeparator* bndFaceSep = new SoSeparator;
  root->addChild (bndFaceSep);  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  bndFaceSep->addChild(hints);

  unsigned int n_link_drawn = 0;

  vcl_map<int, dbmsh3d_edge*>::iterator lit = links.begin();
  for (; lit != links.end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;

    if (draw_invalid==false && FE->is_valid_via_F()==false)
      continue;
    if (FE->cost() < th1)
      continue;
    if (FE->cost() > th2)
      continue;
    if (FE->b_inf())
      continue;
    bool link_finite = FE->is_finite_via_F ();
    if (draw_unb == false && link_finite == false)
      continue;
    if (draw_unvisited==false && FE->b_visited()==false)
      continue; //Skip the unvisited one if specified to skip it.

    n_link_drawn++;

    if (draw_option == -1 || draw_option == 1) {
      SoSeparator* vis;      
      dbmsh3d_vertex *G0, *G1, *G2;
      FE->get_3_Gs_via_FF (&G0, &G1, &G2);

      //distinguish the color of shock link.
      //  - non-A13-2
      //  - acute
      //  - obtuse
      ///if (FE->contain_A13_2_src() == false)
      if (FE->is_flow_bidirect () > 0)
        vis = draw_fs_edge (FE, nona132Color, false, user_defined_class);
      else if (dbgl_is_tri_non_acute (G0->pt(), G1->pt(), G2->pt()))
        vis = draw_fs_edge (FE, obtuseColor, false, user_defined_class);
      else
        vis = draw_fs_edge (FE, acuteColor, false, user_defined_class);

      if (link_finite)
        finiteSep->addChild (vis);
      else
        unbSep->addChild (vis);
    }

    if (draw_option == -1 || draw_option == 2) {      
      vcl_vector<dbmsh3d_vertex*> bnd_pts;
      bool result = FE->get_ordered_Gs_via_FF (bnd_pts);      
      assert (result);

      SbVec3f* vertices = new SbVec3f[bnd_pts.size()];
      for (unsigned int i=0; i<bnd_pts.size(); i++) {
        dbmsh3d_vertex* G = bnd_pts[i];
        vertices[i].setValue (G->pt().x(), G->pt().y(), G->pt().z());
      }
      
      bndFaceSep->addChild (draw_filled_polygon (vertices, bnd_pts.size(), 
                                                 SbColor(0.5f, 0.5f, 0.5f), 0.0f)); //Gray
      delete vertices;
    }
  }

  vul_printf (vcl_cout, "draw_fs_mesh_links(): %u shock links drawn.\n", n_link_drawn);
  return root;
}

SoSeparator* draw_fs_mesh_nodes (vcl_map<int, dbmsh3d_vertex*>& nodes, 
                                 const bool draw_invalid, 
                                 const SbColor color, const float size,
                                 const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoBaseColor *baseColor = new SoBaseColor;
  baseColor->rgb.setValue( color );
  root->addChild( baseColor );    

  vcl_map<int, dbmsh3d_vertex*>::iterator nit = nodes.begin();
  for (; nit != nodes.end(); nit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*nit).second;

    if (FV->b_inf())
      continue;
    if (draw_invalid==false && FV->is_valid_via_FF()==false)
      continue;

    SoSeparator* cubeGroup = new SoSeparator;
    draw_fs_vertex_geom (cubeGroup, FV, size, user_defined_class);
    root->addChild (cubeGroup);
  }

  return root;
}

SoSeparator* draw_fs_patch_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb)
{
  vul_printf (vcl_cout, "\ndraw_fs_patch_to_gene():\n");
  vul_printf (vcl_cout, "  Blue: Valid and finite FF-G.\n");
  if (draw_unb)
    vul_printf (vcl_cout, "  DarkGray: Valid and infinite FF-G.\n");
  else
    vul_printf (vcl_cout, "  Unbounded fs_faces are not shown.\n");
  SoSeparator* root = new SoSeparator;

  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_fin_PG;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_unb_PG;

  unsigned int n_valid_finite_sheets = 0;
  unsigned int n_valid_unbounded_sheets = 0;
  vcl_map<int, dbmsh3d_face*>::iterator it = fs_mesh->facemap().begin();
  for (; it != fs_mesh->facemap().end(); it++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*it).second;
    if (FF->b_valid() == false)
      continue;

    //draw two lines from FF's center C to G0 and G1.
    vgl_point_3d<double> C = FF->compute_center_pt();

    if (FF->b_finite()) {
      n_valid_finite_sheets++;    
      valid_fin_PG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(0)->pt()));
      valid_fin_PG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(1)->pt()));
    }
    else {
      n_valid_unbounded_sheets++;    
      if (draw_unb) {        
        valid_unb_PG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(0)->pt()));
        valid_unb_PG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(1)->pt()));
      }
    }
  }

  root->addChild (draw_line_set (valid_fin_PG, SbColor(0.0f, 0.0f, 1.0f))); //Blue
  if (draw_unb)
    root->addChild (draw_line_set (valid_fin_PG, SbColor(0.5f, 0.5f, 0.5f))); //DarkGray

  vul_printf (vcl_cout, "  Among %u valid FF's (total %u), %u finite, %u unbounded,\n",
              n_valid_finite_sheets + n_valid_unbounded_sheets, fs_mesh->facemap().size(),
              n_valid_finite_sheets, n_valid_unbounded_sheets);
  vul_printf (vcl_cout, "\t%u FF-G asgn (%u finite FF-G asgn.).\n",
              (n_valid_finite_sheets + n_valid_unbounded_sheets)*2, n_valid_finite_sheets*2);
  return root;
}

SoSeparator* draw_fs_link_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb)
{
  vul_printf (vcl_cout, "\ndraw_fs_link_to_gene():\n");
  vul_printf (vcl_cout, "  Green: Valid and finite FE-G.\n");
  if (draw_unb)
    vul_printf (vcl_cout, "  Orange: Valid and infinite FE-G.\n");
  else
    vul_printf (vcl_cout, "  Unbounded fs_edges are not shown.\n");
  SoSeparator* root = new SoSeparator;
  
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_fin_LG;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_unb_LG;

  unsigned int n_valid_finite_links = 0;
  unsigned int n_valid_unbounded_links = 0;
  unsigned int n_finite_GL_asgn = 0;
  unsigned int n_unbounded_GL_asgn = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator it = fs_mesh->edgemap().begin();
  for (; it != fs_mesh->edgemap().end(); it++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*it).second;
    if (FE->is_valid_via_F() == false)
      continue;

    //draw a line from the FE to each generator.
    vgl_point_3d<double> C = FE->mid_pt();

    if (FE->is_finite_via_F()) {
      n_valid_finite_links++;        
      for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        ///dbmsh3d_vertex* G = (dbmsh3d_vertex*)(*vit);
        valid_fin_LG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, G->pt()));
        n_finite_GL_asgn++;
      }
    }
    else {
      n_valid_unbounded_links++;
      for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        if (draw_unb)
          valid_unb_LG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, G->pt()));
        n_unbounded_GL_asgn++;
      }
    }
  }

  root->addChild (draw_line_set (valid_fin_LG, SbColor(0.0f, 1.0f, 0.0f))); //Green
  if (draw_unb)
    root->addChild (draw_line_set (valid_unb_LG, SbColor(1.0f, 1.0f, 0.0f))); //Orange

  vul_printf (vcl_cout, "  Among %u valid FE's (total %u), %u finite, %u unbounded,\n",
              n_valid_finite_links + n_valid_unbounded_links, fs_mesh->edgemap().size(),
              n_valid_finite_links, n_valid_unbounded_links);
  vul_printf (vcl_cout, "\t%u FE-G asgn (%u finite FE-G asgn.).\n",               
              n_finite_GL_asgn + n_unbounded_GL_asgn, n_finite_GL_asgn);
  return root;
}

SoSeparator* draw_fs_node_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb)
{
  vul_printf (vcl_cout, "\ndraw_fs_node_to_gene():\n");
  vul_printf (vcl_cout, "  Red: Valid and finite FV-G.\n");
  if (draw_unb)
    vul_printf (vcl_cout, "  DarkRed: Valid and only belongs to unbounded objects.\n");
  else
    vul_printf (vcl_cout, "  fs_vertices only belongs to unbounded objects are not shown.\n");
  SoSeparator* root = new SoSeparator;
  
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_fin_NG;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > valid_unb_NG;

  unsigned int n_valid_finite_nodes = 0;
  unsigned int n_valid_unbounded_nodes = 0;
  unsigned int n_finite_GN_asgn = 0;
  unsigned int n_unbounded_GN_asgn = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator it = fs_mesh->vertexmap().begin();
  for (; it != fs_mesh->vertexmap().end(); it++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*it).second;
    if (FV->is_valid_via_FF() == false)
      continue;

    if (FV->is_valid_finite_via_FF()) { 
      //fs_vertex that belongs to finite (bounded) fs_face
      n_valid_finite_nodes++;
      for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        valid_fin_NG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FV->pt(), G->pt()));
        n_finite_GN_asgn++;
      }
    }
    else { //fs_vertex that only belongs to unbounded fs_face
      n_valid_unbounded_nodes++;
      for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        if (draw_unb)
          valid_unb_NG.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FV->pt(), G->pt()));
        n_unbounded_GN_asgn++;
      }
    }
  }

  root->addChild (draw_line_set (valid_fin_NG, SbColor(1.0f, 0.0f, 0.0f))); //Red
  if (draw_unb)
    root->addChild (draw_line_set (valid_unb_NG, SbColor(0.5f, 0.0f, 0.0f))); //DarkRed

  vul_printf (vcl_cout, "  Among %u valid and FV's (total %u), %u finite, %u unbounded,\n",
              n_valid_finite_nodes + n_valid_unbounded_nodes, fs_mesh->vertexmap().size(),
              n_valid_finite_nodes, n_valid_unbounded_nodes);
  vul_printf (vcl_cout, "\t%u FV-G asgn (%u finite FV-G asgn.).\n", 
              n_finite_GN_asgn + n_unbounded_GN_asgn, n_finite_GN_asgn);
  return root;
}

//: Draw valid shocks of non-1-ring vertices on the object boundary.
//  This function marks related shock patch->i_value_ to 2.
//  option == 1: draw FF if any gene is at hole.
//  option == 2: draw FF if both genes are at hole.
SoSeparator* draw_Ps_non1ring_Gs (dbsk3d_fs_mesh* fs_mesh, const int option,
                                  const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //draw in Yellow
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = FS_FINITE_VALID_COLOR;
  root->addChild (basecolor);

  //Go through each shock patch element and draw it if it's valid and 
  //any of its G[2] is not on the 1-ring-topology, i.e., on mesh boundary.
  fs_mesh->reset_traverse_f ();
  vgl_point_3d<double> C;

  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->b_valid() == false)
      continue; //skip the invalid sheet elements.
    if (FF->b_visited())
      continue; //skip the already drawn one.

    //Check if the two generators of FF is of 1-ring topology or not.
    FF->set_visited (true);
    bool G0_not_1ring = FF->genes(0)->detect_vtopo_type() != VTOPO_2_MANIFOLD_1RING;
    bool G1_not_1ring = FF->genes(1)->detect_vtopo_type() != VTOPO_2_MANIFOLD_1RING;

    if (option == 1) { //Draw FF if any gene is at hole.
      //Draw a line connecting FF to G on the boundary.
      if (G0_not_1ring || G1_not_1ring) {
        FF->set_i_visited (2); //Mark the related FF with value 2.
        draw_fs_face_geom (root, FF, user_defined_class);
        C = FF->compute_center_pt();
        if (G0_not_1ring)
          draw_line_geom (root, C, FF->genes(0)->pt());
        if (G1_not_1ring)
          draw_line_geom (root, C, FF->genes(1)->pt());
      }
    }
    else if (option == 2) { //Draw FF if both genes are at hole.
      if (G0_not_1ring && G1_not_1ring) {
        FF->set_i_visited (2); //Mark the related FF with value 2.
        draw_fs_face_geom (root, FF, user_defined_class);

        C = FF->compute_center_pt();        
        draw_line_geom (root, C, FF->genes(0)->pt());        
        draw_line_geom (root, C, FF->genes(1)->pt());
      }
    }
  }

  return root;
}

//: Go through all patches with i_value_ == 2 and identify shock links of holes.
//  shock link of holes: 3 assigned genes are non-1-ring vertices.
SoSeparator* draw_Ls_non1ring_Gs (dbsk3d_fs_mesh* fs_mesh, 
                                  const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  assert (0);
  return root;
}

//######################################################################

//: draw shock sheets of the specified medial scaffold component 
SoSeparator* draw_fs_mesh_color_by_radius (dbsk3d_fs_mesh* fs_mesh, const float transp)
{
  SoSeparator* root = new SoSeparator;
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //Color of fs_face as function of formation time of each mid_point.
  float min_time = FLT_MAX;
  float max_time = FLT_MIN;
  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    float min_t, max_t;
    ///FF->get_min_max_V_time (min_t, max_t);
    get_FF_min_max_time (FF, min_t, max_t);
    if (min_t < min_time)
      min_time = min_t;
    if (max_t > max_time)
      max_time = max_t;
  }

  float range = max_time - min_time;
  vul_printf (vcl_cout, "\tdraw_fs_mesh_color_by_radius(): min = %.2f , max = %.2f , range = %.2f\n",
              min_time, max_time, range);

  pit = fs_mesh->facemap().begin();
  for (; pit != fs_mesh->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    //Assign pseudo color using the radius (time) of this FF.
    double center_pt_time = FF->compute_center_pt_time();
    float cvalue = (float) (center_pt_time - min_time) / range;
    SbColor color = get_color_tone (cvalue);
    root->addChild (draw_fs_face (FF, color, transp));
  }

  return root;
}

//: draw shock links of the specified medial scaffold component 
SoSeparator* draw_fs_mesh_comp_links (dbsk3d_fs_mesh* fs_mesh, 
                                      const int compid, const float lineWidth)
{
  SoSeparator* root = new SoSeparator;

  SoDrawStyle* ds = new SoDrawStyle;
  ds->setName( SbName("fs_edge_style") );
  ds->lineWidth.setValue (lineWidth);
  root->addChild(ds);

  vcl_map<int, dbmsh3d_edge*>::iterator lit = fs_mesh->edgemap().begin();
  for (; lit != fs_mesh->edgemap().end(); lit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*lit).second;
    if (!FE->is_valid_via_F())
      continue;
  
    if (FE->e_type() == E_TYPE_MANIFOLD) //YELLOW
      root->addChild (draw_fs_edge (FE, SbColor (0.8f, 0.8f, 0), false));
    else if (FE->e_type() == E_TYPE_AXIAL) //RED
      root->addChild (draw_fs_edge (FE, SbColor (1, 0, 0), false));
    else if (FE->e_type() == E_TYPE_RIB) //BLUE
      root->addChild (draw_fs_edge (FE, SbColor (0, 0, 1), false));
  }

  return root;
}


