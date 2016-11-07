// MingChing Chang 040226
// Visualization of the fine-scale shocks

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_cstdlib.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>
#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_sheet.h>
#include <dbsk3d/dbsk3d_fs_sheet_set.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_sheet.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/actions/SoSearchAction.h>

//: Draw the sheet-component with cost lower than threshold in GRAY.
//  Draw all others in random color (G:0 ~ 0.5).
//  The bnd_option is for shock sheets with cost lower than cost_th.
//
// show_inf==0: not draw the INF SHEET.
// show_inf==1: draw the INF SHEET but skip the INF element of it.
// show_inf==2: draw everything.
//
SoSeparator* draw_fs_sheet_set (dbsk3d_fs_sheet_set* fs_ss, const int show_inf,
                                const bool idv, const bool user_defined_class)
{
  vul_printf (vcl_cout, "draw_fs_sheet_set(): %d fs_sheets.\n", fs_ss->sheetmap().size());
  SoSeparator* root = new SoSeparator;
  
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
  root->addChild (hints);

  init_rand_color (DBMSH3D_SHEET_COLOR_SEED); //10); ///

  unsigned int n_vis_bnd = 0;
  unsigned int n_inf_skipped = 0;

  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss->sheetmap().begin();
  for (; it != fs_ss->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* FS = (*it).second;

    SbColor color = get_next_rand_color ();
    FS->compute_splice_cost ();
    if (FS->type() == FS_TYPE_INTERIOR) { //draw interior shocks in darker color
      float h, s, v;
      color.getHSVValue (h, s, v);
      color.setHSVValue (h, s, float(v*0.3f));
      root->addChild (draw_fs_sheet (FS, color, 0.0f, idv, false, user_defined_class));
    }    
    else if (FS->type() == FS_TYPE_UNBOUNDED) {
      if (show_inf != 0) //draw unbounded shocks sheets in GRAY
        root->addChild (draw_fs_sheet (FS, SbColor (0.5f, 0.5f, 0.5f), 0.0f, idv, false, user_defined_class));
      else 
        n_inf_skipped++;
    }
    else if (FS->n_FFs() == 1) { //draw shock sheet with 1 elm in YELLOW
      root->addChild (draw_fs_sheet (FS, SbColor (1.0f, 1.0f, 0.0f), 0.0f, idv, false, user_defined_class));
    }
    else if (FS->n_FFs() == 2) { //draw shock sheet with 2 elms in GREEN
      root->addChild (draw_fs_sheet (FS, SbColor (0.0f, 1.0f, 0.0f), 0.0f, idv, false, user_defined_class));
    }
    else { //Others: draw in random color
      root->addChild (draw_fs_sheet (FS, color, 0.0f, idv, false, user_defined_class));
    }
  }

  vul_printf (vcl_cout, "\t%d inf_sheets skipped.\n", n_inf_skipped);

  return root;
}

//: option:
//    2: draw only P_G_asgn.
//    3: draw only L_G_asgn.
//    4: draw only N_G_asgn.
//    5: draw both P_G_asgn. and L_G_asgn.
//    6: draw both P_G_asgn. and N_G_asgn.
//    7: draw both L_G_asgn. and N_G_asgn.
//    8: draw all. 
SoSeparator* draw_fs_sheet_set_bnd_asgn (dbsk3d_fs_sheet_set* fs_ss, const int option,
                                         const float cube_size)
{
  SoSeparator* root = new SoSeparator;
  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > lines;
  
  vcl_set<dbsk3d_fs_edge*> FE_with_G_set; //Set of fs_edges associated with generators.  
  vcl_set<dbsk3d_fs_vertex*> FV_with_G_set; //Set of fs_vertices associated with generators.

  //1) Go through all generators and initialize the G_asso_map_[] to G_ASSO_NONE. 
  vcl_map<int, char>  G_asso_map;
  vcl_map<int, dbmsh3d_vertex*>::iterator git = fs_ss->bnd_mesh()->vertexmap().begin();
  for (; git != fs_ss->bnd_mesh()->vertexmap().end(); git++) {
    dbmsh3d_vertex* G = (*git).second;
    G_asso_map.insert (vcl_pair<int, char> (G->id(), G_ASSO_NONE));
  }  

  //Go through each shock sheet FS
  unsigned int n_valid_P = 0;
  unsigned int n_PG_asgn = 0;
  vcl_map<int, dbsk3d_fs_sheet*>::iterator sit = fs_ss->sheetmap().begin();
  for (; sit != fs_ss->sheetmap().end(); sit++) {
    dbsk3d_fs_sheet* FS = (*sit).second;
    
    //Go through each fs_face FF
    for (dbmsh3d_ptr_node* cur = FS->FF_list(); cur != NULL; cur = cur->next()) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();

      assert (FF->b_valid());
      n_valid_P++;
      vgl_point_3d<double> C = FF->compute_center_pt();
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(0)->pt()));
      mark_G_asso (FF->genes(0), G_ASSO_PATCH, G_asso_map);
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(1)->pt()));
      mark_G_asso (FF->genes(1), G_ASSO_PATCH, G_asso_map);
      n_PG_asgn += 2;

      //Add all boundary shock links with genes. into L_G_Set.
      FF->get_bnd_FE_FV_with_Gs (FE_with_G_set, FV_with_G_set);
    }
  }
  vul_printf (vcl_cout, "  %u fs_sheets, %u valid fs_faces, %u patch-to-gene assignment.\n", 
              fs_ss->sheetmap().size(), n_valid_P, n_PG_asgn);

  //Draw fs_face to gene association in Dark Blue.
  if (option == 2 || option == 5 || option == 6 || option == 8)
    root->addChild (draw_line_set (lines, SbColor (0.3f, 0.3f, 0.5f)));
  lines.clear();

  //2) Go through the FE_with_G_set and draw fs_edge to gene associations.
  unsigned int n_LG_asgn = 0;
  vcl_set<dbsk3d_fs_edge*>::iterator lit = FE_with_G_set.begin();
  for (; lit != FE_with_G_set.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);
    assert (FE->have_asgn_Gs());
    vgl_point_3d<double> C = FE->mid_pt();
    
    for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, G->pt()));
      mark_G_asso (G, G_ASSO_LINK, G_asso_map);
      n_LG_asgn++;
    }
  }
  vul_printf (vcl_cout, "  %u fs_edges has asgn_genes, %u link-to-gene assignment.\n", 
              FE_with_G_set.size(), n_LG_asgn);
  FE_with_G_set.clear();

  //Draw fs_edge to gene association in Dark Green.
  if (option == 3 || option == 5 || option == 7 || option == 8)
    root->addChild (draw_line_set (lines, SbColor (0.3f, 0.5f, 0.3f)));
  lines.clear();
  
  //3) Go through the FV_with_G_set and draw fs_vertex to gene associations.
  unsigned int n_NG_asgn = 0;
  vcl_set<dbsk3d_fs_vertex*>::iterator nit = FV_with_G_set.begin();
  for (; nit != FV_with_G_set.end(); nit++) {
    dbsk3d_fs_vertex* FV = (*nit);
    assert (FV->have_asgn_Gs());

    for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FV->pt(), G->pt()));
      mark_G_asso (G, G_ASSO_NODE, G_asso_map);
      n_NG_asgn++;
    }
  }
  vul_printf (vcl_cout, "  %u fs_vertices has asgn_genes, %u node-to-gene assignment.\n", 
              FV_with_G_set.size(), n_NG_asgn);
  FV_with_G_set.clear();

  //Draw fs_vertex to gene association in Dark Red.
  if (option == 4 || option == 6 || option == 7 || option == 8)
    root->addChild (draw_line_set (lines, SbColor (0.5f, 0.3f, 0.3f)));
  lines.clear();

  //4) Go through all generators and draw the un-associated ones (if any).
  vul_printf (vcl_cout, "  Unassociated generators: ");
  unsigned int n_gene_unasso = 0;
  vcl_map<int, char>::iterator gmit = G_asso_map.begin();
  for (; gmit != G_asso_map.end(); gmit++) {
    char asso = (*gmit).second;
    if (asso == G_ASSO_NONE) {
      int id = (*gmit).first;
      vul_printf (vcl_cout, "%d ", id);
      dbmsh3d_vertex* V = fs_ss->bnd_mesh()->vertexmap(id);
      //Yellow cube for un-asgn generators.
      root->addChild (draw_vertex_vispt_SoCube (V, SbColor (0.5f, 0.0f, 0.0f), cube_size)); 
      n_gene_unasso++;
    }
  }
  if (n_gene_unasso == 0)
    vul_printf (vcl_cout, "NONE.");
  vul_printf (vcl_cout, "\n\n"); 

  ///assert (n_gene_unasso == 0);
  G_asso_map.clear();
  return root;
}

//: Vis bnd-shock asso. of all sheet components (camouflage color).
SoSeparator* draw_fs_sheet_set_bnd_mesh (dbsk3d_fs_sheet_set* fs_ss, 
                                         const int option, const float transp)
{
  SoSeparator* root = new SoSeparator;
  int n_tris_drawn = 0;
  init_rand_color (DBMSH3D_SHEET_COLOR_SEED);

  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss->sheetmap().begin();
  for (; it != fs_ss->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* FS = (*it).second;

    SbColor color = get_next_rand_color ();
    int n_tri = draw_fs_sheet_bnd_mesh (root, FS, option, color, transp);
    n_tris_drawn += n_tri;
  }

  return root;
}

//: Vis bnd mesh of the given sheet components.
//  return number of triangles drawn.
int draw_fs_sheet_bnd_mesh (SoSeparator* root, dbsk3d_fs_sheet* FS, 
                            const int option, const SbColor& color, const float transp)
{
  // Gfaces: set of faces that completely sit on Gset (of points).
  // Gfaces2: set of faces that share 2 points in Gset.
  // Gfaces1: set of faces that share only 1 point in Gset.
  vcl_set<dbmsh3d_vertex*> Gset;
  vcl_set<dbmsh3d_face*> Gfaces, Gfaces2, Gfaces1;
  FS->get_bnd_mesh_Fs (Gset, Gfaces, Gfaces2, Gfaces1);

  //For option 2, only draw the Gfaces.
  //option 3: draw Gfaces, Gfaces2.
  //option 4: draw Gfaces, Gfaces2, Gfaces3.
  int n_total = 0;
  root->addChild (draw_ifs (Gset, Gfaces, color, true));
  n_total += Gfaces.size();

  if (option == 3 || option == 4) {
    root->addChild (draw_ifs (Gset, Gfaces2, color, true));
    n_total += Gfaces2.size();
  }
  if (option == 4) {
    root->addChild (draw_ifs (Gset, Gfaces1, color, true));
    n_total += Gfaces1.size();
  }

  return n_total;
}

//#######################################################################

SoSeparator* draw_fs_sheet (dbsk3d_fs_sheet* FS, const SbColor& color, const float transp, 
                            const bool idv, const bool show_id, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue (color/2);
  material->transparency = transp;
  root->addChild (material);

  if (idv) { //draw individual fs_faces.
    for (dbmsh3d_ptr_node* cur = FS->FF_list(); cur != NULL; cur = cur->next()) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
      draw_fs_face_geom (root, FF, user_defined_class);
    }
  }
  else
    draw_fs_sheet_geom (root, FS, user_defined_class);

  return root;
}

void draw_fs_sheet_geom (SoSeparator* root, dbsk3d_fs_sheet* FS,
                         const bool user_defined_class)
{
  vcl_set<dbmsh3d_vertex*> IFS_vset;
  vcl_vector<vgl_point_3d<double> > IFS_pts;
  vcl_vector<vcl_vector<int> > IFS_faces;

  //Draw in IFS to speed up visulization.
  //Go through each shock patch and add all vertices into IFS_vset.
  for (dbmsh3d_ptr_node* cur = FS->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    assert (FF->b_valid());

    //Put all vertices into IFS_vset.
    FF->_ifs_track_ordered_vertices ();
    for (unsigned int i=0; i<FF->vertices().size(); i++) {
      dbmsh3d_vertex* V = FF->vertices(i);
      IFS_vset.insert (V);
    }
  }

  //Go through IFS_vset and reset each vid from 0 (used as index in IFS).
  vcl_set<dbmsh3d_vertex*>::iterator vit = IFS_vset.begin();
  for (unsigned int i=0; vit != IFS_vset.end(); vit++, i++) {
    (*vit)->set_vid (i);
    IFS_pts.push_back ((*vit)->pt());
  }
  IFS_vset.clear();

  //Go through each face again to setup IFS_faces.
  for (dbmsh3d_ptr_node* cur = FS->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();

    vcl_vector<int> vids;
    for (unsigned int i=0; i<FF->vertices().size(); i++) {
      dbmsh3d_vertex* V = FF->vertices(i);
      vids.push_back (V->vid());
    }
    assert (vids.size()>2);
    IFS_faces.push_back (vids);
  }

  if (user_defined_class)
    draw_fs_sheet_mesh_geom (root, FS, IFS_pts, IFS_faces);
  else
    draw_ifs_geom (root, IFS_pts, IFS_faces);

  IFS_pts.clear();
  IFS_faces.clear();
}

void draw_fs_sheet_mesh_geom (SoGroup* root, dbsk3d_fs_sheet* FS,
                              const vcl_vector<vgl_point_3d<double> >& pts,
                              const vcl_vector<vcl_vector<int> >& faces)
{
  //Assign vertices
  int nVertices = pts.size();
  float (*xyz)[3] = new float[nVertices][3];
  for (unsigned int i=0; i<pts.size(); i++) {
    xyz[i][0] = (float) pts[i].x();
    xyz[i][1] = (float) pts[i].y();
    xyz[i][2] = (float) pts[i].z();
  }
 
  //Assign faces
  SoVertexProperty* vp = new SoVertexProperty;
  unsigned int nFacesIndices = _count_faces_indices (faces);
  int* ind = new int [nFacesIndices];
  unsigned int k = 0;  
  for (unsigned int i=0; i<faces.size(); i++) {
    assert (faces[i].size() > 2);
    for (unsigned int j=0; j<faces[i].size(); j++) {
      ind[k] = faces[i][j];
      k++;
    }    
    ind[k] = -1; //Add the final '-1'
    k++;
  }
  assert (k == nFacesIndices);
  vp->vertex.setValues (0, nVertices, xyz);
  delete []xyz;

  fs_sheet_SoIndexedFaceSet* indexedFaceSet = new fs_sheet_SoIndexedFaceSet (FS);
  indexedFaceSet->vertexProperty = vp;
  indexedFaceSet->coordIndex.setValues (0, nFacesIndices, ind);
  delete []ind;
  root->addChild (indexedFaceSet);
}

SoSeparator* draw_fs_sheet_A13_A3_bnd_curves (dbsk3d_fs_sheet* FS, const float width)
{
  SoSeparator* root = new SoSeparator;

  ///vcl_ostringstream ostrm;
  ///FS->getInfo (ostrm);
  ///vcl_cout << ostrm.str();
  
  //Line width
  SoDrawStyle*  drawStyle = new SoDrawStyle ();
  drawStyle->lineWidth = 3;
  root->addChild (drawStyle);

  //Visualize the A3_bnd_links and A13_bnd_links.
  vcl_set<dbsk3d_fs_edge*> A3_bnd_links;
  vcl_set<dbsk3d_fs_edge*> A13_bnd_links;
  FS->get_bnd_FEs (A3_bnd_links, A13_bnd_links);

  //Visualize the A3_bnd_links in Blue.
  vcl_set<dbsk3d_fs_edge*>::iterator lit = A3_bnd_links.begin();
  for (; lit != A3_bnd_links.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);
    root->addChild (draw_fs_edge (FE, color_from_code(COLOR_BLUE), width, false));
  }

  //Visualize the A13_bnd_links in Red.
  lit = A13_bnd_links.begin();
  for (; lit != A13_bnd_links.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);
    root->addChild (draw_fs_edge (FE, color_from_code(COLOR_RED), width, false));
  }

  return root;
}

SoSeparator* draw_fs_sheet_bnd_asgn (dbsk3d_fs_sheet* FS, const bool draw_idv, const float cube_size)
{
  SoSeparator* root = new SoSeparator;

  //Line width
  //SoDrawStyle*  drawStyle = new SoDrawStyle ();
  //drawStyle->lineWidth = 1;
  //root->addChild (drawStyle);

  SbColor PG_color (0.0f, 0.0f, 1.0f); //Blue
  SbColor LG_color (0.0f, 1.0f, 0.0f); //Green    
  SbColor NG_color (1.0f, 0.0f, 0.0f); //Red
  SoBaseColor* basecolor = new SoBaseColor;

  SoSeparator* root_PG = new SoSeparator;
  root->addChild (root_PG);
  basecolor->rgb = PG_color;
  root_PG->addChild (basecolor);

  SoSeparator* root_LG = new SoSeparator;
  root->addChild (root_LG);
  basecolor->rgb = LG_color;
  root_LG->addChild (basecolor);

  SoSeparator* root_NG = new SoSeparator;
  root->addChild (root_NG);
  basecolor->rgb = NG_color;
  root_NG->addChild (basecolor);

  vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > > lines;
  vcl_set<dbsk3d_fs_edge*> FE_with_G_set; //Set of fs_edges associated with generators.  
  vcl_set<dbsk3d_fs_vertex*> FV_with_G_set; //Set of fs_vertices associated with generators.

  //Draw the shock-to-bnd assignment.
  for (dbmsh3d_ptr_node* cur = FS->FF_list(); cur != NULL; cur = cur->next()) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
    assert (FF->b_valid());

    //Visualize the 2 generators of each fs_face FF.
    //Draw two lines from FF's center C to Ga and Gb.    
    //Also draw the Ga and Gb
    vgl_point_3d<double> C = FF->compute_center_pt();

    for (unsigned int j=0; j<2; j++) {
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (C, FF->genes(j)->pt()));
      if (draw_idv)
        root_PG->addChild (draw_vertex_geom_vispt_SoCube (FF->genes(j), cube_size));
    }

    //Add all boundary shock links with genes. into L_G_Set.
    FF->get_bnd_FE_FV_with_Gs (FE_with_G_set, FV_with_G_set);
  }
  root->addChild (draw_line_set (lines, PG_color));
  lines.clear();

  //Draw all LG_asgn.
  vcl_set<dbsk3d_fs_edge*>::iterator lit = FE_with_G_set.begin();
  for (; lit != FE_with_G_set.end(); lit++) {
    dbsk3d_fs_edge* FE = (*lit);
    assert (FE->have_asgn_Gs());
    vgl_point_3d<double> LC = FE->mid_pt();
    
    for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (LC, G->pt()));
      if (draw_idv)
        root_LG->addChild (draw_vertex_vispt_SoCube (G, LG_color, cube_size));
    }
  }
  root->addChild (draw_line_set (lines, LG_color));
  lines.clear();

  //Draw all NG_asgn.
  vcl_set<dbsk3d_fs_vertex*>::iterator nit = FV_with_G_set.begin();
  for (; nit != FV_with_G_set.end(); nit++) {
    dbsk3d_fs_vertex* FV = (*nit);
    assert (FV->have_asgn_Gs());

    for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
      dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
      lines.push_back (vcl_pair<vgl_point_3d<double>, vgl_point_3d<double> > (FV->pt(), G->pt()));
      if (draw_idv)
        root_NG->addChild (draw_vertex_vispt_SoCube (G, NG_color, cube_size));
    }
  }
  root->addChild (draw_line_set (lines, NG_color));
  lines.clear();

  return root;
}

// option:
//   1: only shows Gfaces
//   2: shows Gfaces and Gfaces2
//   3: shows all 3 (Gfaces, Gfaces2, Gfaces2)
//
SoSeparator* draw_fs_sheet_bnd_mesh (dbsk3d_fs_sheet* FS, const int option, 
                                     const int draw_idv, const float transp)
{
  SoSeparator* root = new SoSeparator;

  // Gfaces: set of faces that completely sit on Gset (of points).
  // Gfaces2: set of faces that share 2 points in Gset.
  // Gfaces1: set of faces that share only 1 point in Gset.
  vcl_set<dbmsh3d_vertex*> Gset;
  vcl_set<dbmsh3d_face*> Gfaces, Gfaces2, Gfaces1;
  FS->get_bnd_mesh_Fs (Gset, Gfaces, Gfaces2, Gfaces1);

  vcl_set<dbmsh3d_vertex*> Gset2;
  get_ifs_faces_pts (Gfaces2, Gset2);
  vcl_set<dbmsh3d_vertex*> Gset1;
  get_ifs_faces_pts (Gfaces1, Gset1);

  if (draw_idv) { //Visualizing each individual associated bnd mesh faces.
    vcl_set<dbmsh3d_face*>::iterator it = Gfaces.begin();
    for (; it != Gfaces.end(); it++) {
      dbmsh3d_face* F = *it;
      root->addChild (draw_F (F, color_from_code (COLOR_YELLOW)));
    }
  }
  else { //Visualizing using indexed-face-set.
    root->addChild (draw_ifs (Gset, Gfaces, COLOR_YELLOW, true));
    if (option == 2 || option == 3)
      root->addChild (draw_ifs (Gset2, Gfaces2, COLOR_LIGHTGREEN, true));
    if (option == 3)
      root->addChild (draw_ifs (Gset1, Gfaces1, COLOR_ORANGE, true));
  }

  return root;
}


// #####################################################################

SoSeparator* draw_tabs_bnd_curve (dbsk3d_fs_sheet_set* fs_ss, const float cost_th)
{
  SoSeparator* root = new SoSeparator;

  //Go through each tab and draw the boundary curves.
  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss->sheetmap().begin();
  for (; it != fs_ss->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* FS = (*it).second;

    ///if (FS->type() == FS_TYPE_TAB && FS->cost() <= cost_th)
      ///root->addChild (draw_tab_P_to_G (FS));
  }

  return root;
}

SoSeparator* draw_tab_P_to_G (dbsk3d_fs_sheet* FS)
{
  SoSeparator* root = new SoSeparator;
  return root;
}


