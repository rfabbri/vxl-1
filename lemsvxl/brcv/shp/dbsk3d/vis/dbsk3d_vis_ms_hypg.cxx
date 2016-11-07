//: MingChing Chang 
//  Nov 30, 2004

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <dbmsh3d/dbmsh3d_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

#include <dbmsh3d/algo/dbmsh3d_ray_intersect.h>
#include <dbsk3d/algo/dbsk3d_ms_recon.h>
#include <dbsk3d/algo/dbsk3d_ms_hypg_trans.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_fmm.h>

#include <dbsk3d/vis/dbsk3d_vis_ms_hypg.h>
#include <dbsk3d/vis/dbsk3d_vis_ms_elm.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoMaterial.h>

void generate_shock_color_table (unsigned long seed, unsigned int size, 
                                 vcl_vector<SbColor>& color_table)
{
  color_table.clear();

  //initialize the random color by given seed
  init_rand_color (seed);

  for (unsigned int i=0; i<size; i++) {
    //generate a random color, skipping color close to blue or red!
    SbColor color = get_rand_color_no_red_blue ();
    color_table.push_back (color);
  }
}

//######################################################################
//: VISUALIZATION OF THE SHOCK SCAFFOLD
SoSeparator* draw_ms_graph (dbmsh3d_graph* ms_graph,
                            const SbColor& A1A3_color, const SbColor& A14_color,
                            const SbColor& Dege_A1A3_color, const SbColor& Dege_A14_color, 
                            const SbColor& LN_color,
                            const float vertex_radius, const SbColor& A3_rib_color, 
                            const SbColor& A13_axial_color, const SbColor& Dege_axial_color, 
                            const SbColor& A3_loop_color, const SbColor& A13_loop_color, 
                            const SbColor& Vc_color,
                            const float width, const bool idv,
                            const bool show_vid, const bool show_cid,
                            const bool draw_only_selected,
                            const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  if (ms_graph == NULL)
    return root;

  //vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator SV_it = ms_graph->vertexmap().begin();
  for (; SV_it != ms_graph->vertexmap().end(); SV_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*SV_it).second;
    if (draw_only_selected && MN->selected() == false)
      continue;
    switch (MN->n_type()) {
    case N_TYPE_RIB_END:
      root->addChild (draw_ms_vertex (MN, A1A3_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_AXIAL_END:
      root->addChild (draw_ms_vertex (MN, A14_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_DEGE_RIB_END:
      root->addChild (draw_ms_vertex (MN, Dege_A1A3_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_DEGE_AXIAL_END:
      root->addChild (draw_ms_vertex (MN, Dege_A14_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_LOOP_END:
      root->addChild (draw_ms_vertex (MN, LN_color, vertex_radius, show_vid, user_defined_class));
    break;
    default:
      assert (0);
    break;
    }
  }

  //curves
  SoDrawStyle* ds = new SoDrawStyle;
  ds->lineWidth.setValue (width);
  root->addChild( ds );

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_graph->edgemap().begin();
  for (; cit != ms_graph->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (draw_only_selected && MC->selected() == false)
      continue;
    switch (MC->c_type()) {
    case C_TYPE_RIB:
      if (MC->is_self_loop())
        root->addChild (draw_ms_loop (MC, A3_loop_color, width+2, idv, show_vid, user_defined_class));
      else
        root->addChild (draw_ms_curve (MC, A3_rib_color, width+2, idv, show_vid, user_defined_class));
    break;
    case C_TYPE_AXIAL:
      if (MC->is_self_loop())
        root->addChild (draw_ms_curve (MC, A13_loop_color, width+4, idv, show_vid, user_defined_class));
      else
        root->addChild (draw_ms_curve (MC, A13_axial_color, width+4, idv, show_vid, user_defined_class));
    break;
    case C_TYPE_DEGE_AXIAL:
      root->addChild (draw_ms_curve (MC, Dege_axial_color, width+4, idv, show_vid, user_defined_class));
    break;
    case C_TYPE_VIRTUAL:
      root->addChild (draw_ms_curve (MC, Vc_color, width, idv, show_vid, user_defined_class));
    break;
    default:
      assert (0);
    break;
    }
  }

  return root;
}


SoSeparator* draw_ms_graph_flow (dbmsh3d_graph* ms_graph,
                            const SbColor& A1A3_color, const SbColor& A14_color,
                            const SbColor& Dege_A1A3_color, const SbColor& Dege_A14_color, 
                            const SbColor& LN_color,
                            const float vertex_radius, const SbColor& A3_rib_color, 
                            const SbColor& A13_axial_color, const SbColor& A13_axial2_color, 
                            const SbColor& A13_axial3_color, const SbColor& A13_axial4_color, 
                            const SbColor& Dege_axial_color, 
                            const SbColor& A3_loop_color, const SbColor& A13_loop_color,
                            const SbColor& Vc_color,
                            const float curve_width, const bool idv,
                            const bool show_vid, const bool show_cid,
                            const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  if (ms_graph == NULL)
    return root;

  //vertices
  vcl_map<int, dbmsh3d_vertex*>::iterator SV_it = ms_graph->vertexmap().begin();
  for (; SV_it != ms_graph->vertexmap().end(); SV_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*SV_it).second;
    switch (MN->n_type()) {
    case N_TYPE_RIB_END:
      root->addChild (draw_ms_vertex (MN, A1A3_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_AXIAL_END:
      root->addChild (draw_ms_vertex (MN, A14_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_DEGE_RIB_END:
      root->addChild (draw_ms_vertex (MN, Dege_A1A3_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_DEGE_AXIAL_END:
      root->addChild (draw_ms_vertex (MN, Dege_A14_color, vertex_radius, show_vid, user_defined_class));
    break;
    case N_TYPE_LOOP_END:
      root->addChild (draw_ms_vertex (MN, LN_color, vertex_radius, show_vid, user_defined_class));
    break;
    default:
      assert (0);
    break;
    }
  }

  //curves
  SoDrawStyle* ds = new SoDrawStyle;
  ds->lineWidth.setValue (curve_width);
  root->addChild( ds );

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_graph->edgemap().begin();
  for (; cit != ms_graph->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    switch (MC->c_type()) {
    case C_TYPE_RIB:
      if (MC->is_self_loop())
        root->addChild (draw_ms_loop (MC, A3_loop_color, curve_width+2, idv, show_vid, user_defined_class));
      else
        root->addChild (draw_ms_curve (MC, A3_rib_color, curve_width+2, idv, show_vid, user_defined_class));
    break;
    case C_TYPE_AXIAL:
      if (MC->is_self_loop())
        root->addChild (draw_ms_curve (MC, A13_loop_color, curve_width+4, idv, show_vid, user_defined_class));
      else {
        //Distinguish the two types of A13-2 and A13-3.
        //Drawing each A13 curve element.
        //root->addChild (draw_ms_curve (MC, A13_axial_color, curve_width+4, idv, show_vid, user_defined_class));
        for (unsigned int i=0; i<MC->E_vec().size(); i++) {
          dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(i);
          FE_FLOW_TYPE type = FE->detect_flow_type ();
          SbColor col;
          switch (type) {
          case FE_FT_I_A13_2_ACUTE:
            col = A13_axial_color;
          break;
          case FE_FT_II_A13_3_OBT:
            col = A13_axial2_color;
          break;
          case FE_FT_III_A14_ACUTE:
            col = A13_axial3_color;
          break;
          case FE_FT_IV_A14_OBT:
            col = A13_axial4_color;
          }

          root->addChild (draw_fs_edge (FE, col, curve_width+4, false, user_defined_class));
        }
      }
    break;
    case C_TYPE_DEGE_AXIAL: //*2
      root->addChild (draw_ms_curve (MC, Dege_axial_color, curve_width+4, idv, show_vid, user_defined_class));
    break;
    case C_TYPE_VIRTUAL:
      root->addChild (draw_ms_curve (MC, Vc_color, curve_width, idv, show_vid, user_defined_class));
    break;
    default:
      assert (0);
    break;
    }
  }

  return root;
}

SoSeparator* draw_ms_vertices_ctable (dbmsh3d_graph* G, 
                                      const vcl_vector<SbColor>& ctable, 
                                      const float vertex_radius,
                                      const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  if (G == NULL)
    return root;

  vcl_map<int, dbmsh3d_vertex*>::iterator SV_it = G->vertexmap().begin();
  for (; SV_it != G->vertexmap().end(); SV_it++) {
    dbsk3d_ms_node* MN = (dbsk3d_ms_node*) (*SV_it).second;
    int id = MN->id();
    SbColor color = ctable[id];
    root->addChild (draw_ms_vertex (MN, color, vertex_radius, user_defined_class));
  }

  return root;
}

SoSeparator* draw_ms_curves_ctable (dbmsh3d_graph* ms_graph, 
                                    const vcl_vector<SbColor>& ctable, const float width,
                                    const bool idv, const bool show_id,
                                    const bool skip_non_match_vc, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;

  if (ms_graph == NULL)
    return root;

  SoDrawStyle* ds = new SoDrawStyle;
  ds->lineWidth.setValue (width);
  root->addChild( ds );

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_graph->edgemap().begin();
  for (; cit != ms_graph->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;    
    int id = MC->id();
    assert (id < ctable.size());
    SbColor color = ctable[id];

    if (skip_non_match_vc && MC->c_type() == C_TYPE_VIRTUAL)
      if (color == UNMATCHED_COLOR)
        continue;

    root->addChild (draw_ms_curve (MC, color, width, idv, show_id, user_defined_class));
  }

  return root;
}

SoSeparator* draw_ms_sheets_ctable (dbsk3d_ms_hypg* ms_hypg, 
                                    const vcl_vector<SbColor>& ctable,
                                    const float transp, const bool idv, const bool show_id,
                                    const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  assert (ms_hypg);

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild( hints );

  vcl_map<int, dbmsh3d_sheet*>::iterator it = ms_hypg->sheetmap().begin();
  for (; it != ms_hypg->sheetmap().end(); it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it).second;
    int id = MS->id();
    SbColor color = ctable[id];
    root->addChild (draw_ms_sheet (MS, color, transp, idv, show_id, user_defined_class));
  }

  return root;
}

SoSeparator* draw_ms_sheets (dbsk3d_ms_hypg* ms_hypg, 
                             const SbColor& color,
                             const float transp, const bool idv, const bool show_id,
                             const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  assert (ms_hypg);

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild( hints );

  vcl_map<int, dbmsh3d_sheet*>::iterator it = ms_hypg->sheetmap().begin();
  for (; it != ms_hypg->sheetmap().end(); it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it).second;
    root->addChild (draw_ms_sheet (MS, color, transp, idv, show_id, user_defined_class));
  }

  return root;
}

SoSeparator* draw_ms_sheets_bnd_mesh_ctable (dbsk3d_ms_hypg* ms_hypg, const int option,
                                             const vcl_vector<SbColor>& ctable, 
                                             const float transp, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  assert (ctable.size() != 0);

  //Generate IFS on the bnd_mesh.
  ms_hypg->bnd_mesh()->build_IFS_mesh();
  
  vcl_map<int, dbmsh3d_sheet*>::iterator it = ms_hypg->sheetmap().begin();
  for (; it != ms_hypg->sheetmap().end(); it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it).second;
    int id = MS->id();
    SbColor color = ctable[id];

    // Gfaces: set of faces that completely sit on Gset (of points).
    // Gfaces2: set of faces that share 2 points in Gset.
    // Gfaces1: set of faces that share only 1 point in Gset.
    vcl_set<dbmsh3d_vertex*> Gset;
    vcl_set<dbmsh3d_face*> Gfaces, Gfaces2, Gfaces1;
    MS->get_bnd_mesh_Fs (Gset, Gfaces, Gfaces2, Gfaces1);

    root->addChild (draw_ifs (Gset, Gfaces, color, true));
    
    if (option == 3 || option == 4)
      root->addChild (draw_ifs (Gset, Gfaces2, color, true));
    if (option == 4)
      root->addChild (draw_ifs (Gset, Gfaces1, color, true));
  }
  
  //Draw non-visited triangles in gray.

  //Clear the bnd_mesh's IFS mesh.
  ms_hypg->bnd_mesh()->clean_IFS_mesh();

  return root;
}

//:
//  option 5: pseudo-color by ranking order by cost.
//  option 6: pseudo-color by cost/max_cost.
SoSeparator* draw_ms_sheets_cost (dbsk3d_ms_hypg* ms_hypg, const int option, 
                                  const float transp, const bool idv, 
                                  const bool show_id, const bool user_defined_class)
{
  SoSeparator* root = new SoSeparator;
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild(hints);

  //Compute cost for each ms_sheet as # of associated generators.
  vcl_multimap<float, dbmsh3d_sheet*> MS_mmap;
  float min_cost = FLT_MAX;
  float max_cost = FLT_MIN;

  vcl_map<int, dbmsh3d_sheet*>::iterator it = ms_hypg->sheetmap().begin();
  for (; it != ms_hypg->sheetmap().end(); it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it).second;
    float cost = (float) MS->get_cost();
    MS_mmap.insert (vcl_pair<float, dbmsh3d_sheet*>(cost, MS));
    if (cost < min_cost)
      min_cost = cost;
    if (cost > max_cost)
      max_cost = cost;
  }

  float range = max_cost - min_cost;
  vul_printf (vcl_cout, "\tdraw_ms_sheets_cost(): %u ms_sheets, min = %.2f , max = %.2f , range = %.2f\n",
              ms_hypg->sheetmap().size(), min_cost, max_cost, range);

  vcl_multimap<float, dbmsh3d_sheet*>::iterator mit = MS_mmap.begin();
  for (int i=0; mit != MS_mmap.end(); mit++, i++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*mit).second;    
    //Assign pseudo color using the cost of this MS.
    float cvalue;
    if (option==5) 
      cvalue = float(i) / (MS_mmap.size()-1);
    else
      cvalue = (MS->cost() - min_cost) / range;
    SbColor color = get_color_tone (cvalue);
    root->addChild (draw_ms_sheet (MS, color, transp, idv, show_id, user_defined_class));
  }  

  return root;
}

SoSeparator* draw_ms_sheets_gdt (vcl_map <int, dbmsh3d_fmm_mesh*>& sheet_fmm_mesh_map, 
                                 const float transp)
{
  SoSeparator* root = new SoSeparator;
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild (hints);

  vcl_map<int, dbmsh3d_fmm_mesh*>::iterator it = sheet_fmm_mesh_map.begin();
  for (; it != sheet_fmm_mesh_map.end(); it++) {
    dbmsh3d_fmm_mesh* fmm_mesh = (*it).second;
    root->addChild (dbmsh3d_draw_fmm_mesh (fmm_mesh));
  }

  return root;
}

//######################################################################
//: VISUALIZATION OF THE RECONSTRUCTION

//Go through all A3 Rib curves and draw the shock-bnd line.
SoSeparator* draw_A3rib_bnd_link (dbmsh3d_graph* ms_graph, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_graph->edgemap().begin();
  for (; cit != ms_graph->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;
    
    //Go through all V_vec of this A3Rib.
    for (unsigned int j=0; j<MC->V_vec().size(); j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
      vcl_set<dbmsh3d_vertex*> Genes = FV->get_Gs_from_FFs ();

      //draw line from this FV to each gene.
      vcl_set<dbmsh3d_vertex*>::iterator git = Genes.begin();
      while (git != Genes.end()) {
        const dbmsh3d_vertex* G = (*git);
        SoDrawStyle* drawStyle = new SoDrawStyle ();
        root->addChild (draw_line (FV->pt(), G->pt(), color, drawStyle));
        git++;
      }
    }//end for j
  }//end for i

  return root;
}

//Go through all A13 Axial curves and draw the shock-bnd line.
SoSeparator* draw_A13axial_bnd_link (dbmsh3d_graph* ms_graph, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_graph->edgemap().begin();
  for (; cit != ms_graph->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_AXIAL) 
      continue;

    //Go through all V_vec of this A13Axial
    for (unsigned int j=0; j<MC->V_vec().size(); j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
      vcl_set<dbmsh3d_vertex*> Genes = FV->get_Gs_from_FFs ();

      //draw line from this FV to each gene.
      vcl_set<dbmsh3d_vertex*>::iterator git = Genes.begin();
      while (git != Genes.end()) {
        const dbmsh3d_vertex* G = (*git);
        SoDrawStyle* drawStyle = new SoDrawStyle ();
        ///drawStyle->lineWidth.setValue (1);
        root->addChild (draw_line (FV->pt(), G->pt(), color, drawStyle));
        git++;
      }
    }//end for j
  }//end for i

  return root;
}

//######################################################################
//: Visualization of sectional triangles along A3 ribs.
SoSeparator* draw_A3_sect_tris (dbsk3d_ms_hypg* ms_hypg, const SbColor& A3_sect_tris_color)
{
  SoSeparator* root = new SoSeparator;

  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  root->addChild (hints);

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg->edgemap().begin();
  for (; cit != ms_hypg->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;
    
    //Go through all linkElms of this A3Rib.    
    for (unsigned int j=0; j<MC->E_vec().size(); j++) {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(j);
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();

      //Determine centroid C of FF
      vgl_point_3d<double> C = FF->compute_center_pt();
      
      //Determine Ga and Gb
      vgl_point_3d<double> Ga = FF->genes(0)->pt();
      vgl_point_3d<double> Gb = FF->genes(1)->pt();

      //draw triangle GaGbC
      //color
      SoBaseColor* basecolor = new SoBaseColor;
      basecolor->rgb = A3_sect_tris_color;
      root->addChild (basecolor);

      draw_triangle_geom (root, C, Ga, Gb);
    }//end for j
  }//end for i

  return root;
}


//: Visualization of ridge region bnd curves along A3 ribs.
SoSeparator* draw_A3_ridgebnd_curves (dbsk3d_ms_hypg* ms_hypg, const float& width,
                                      const SbColor& A3_ridgebnd_color_a,
                                      const SbColor& A3_ridgebnd_color_b)
{  
  vul_printf (vcl_cout, "draw_A3_ridgebnd_curves().\n");
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg->edgemap().begin();
  for (int i=0; cit != ms_hypg->edgemap().end(); cit++, i++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;

    //For each A3Rib, determine two ridge boundary curves.
    vcl_vector<vgl_point_3d<double> > ridgebnd_curve_a, ridgebnd_curve_b;
    get_ridge_bnd_curves (MC, ridgebnd_curve_a, ridgebnd_curve_b);
    
    //For each A3-rib, draw the two ridgebnd curves.
    root->addChild (draw_polyline (ridgebnd_curve_a, width, A3_ridgebnd_color_a));
    root->addChild (draw_polyline (ridgebnd_curve_b, width, A3_ridgebnd_color_b));
  }

  vul_printf (vcl_cout, "done.\n");
  return root;
}

//: Visualization of shock-tab surface region bnd curves along A13 axials.
SoSeparator* draw_A13_surfbnd_curves (dbsk3d_ms_hypg* ms_hypg, const float& width, 
                                      const SbColor& A13_surfbnd_color_a,
                                      const SbColor& A13_surfbnd_color_b)
{
  vul_printf (vcl_cout, "draw_A13_surfbnd_curves().\n");
  SoSeparator* root = new SoSeparator;

  vcl_map<int, dbmsh3d_sheet*>::iterator it = ms_hypg->sheetmap().begin();
  for (; it != ms_hypg->sheetmap().end(); it++) {
    dbsk3d_ms_sheet* MS = (dbsk3d_ms_sheet*) (*it).second;

    //Only visualize the shock tab case
    if (MS->has_incident_A3rib() == false)
      continue;

    //Loop through each A13Axial of this MS    
    dbmsh3d_halfedge* HE = MS->halfedge();
    do {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) HE->edge();
      if (MC->c_type() == C_TYPE_AXIAL) {

        //For each A13Axial, determine two cut-off patch boundary curves.
        vcl_vector<vgl_point_3d<double> > surfbnd_curve_a, surfbnd_curve_b;
        get_curoff_patch_bnd_curves (MC, MS, surfbnd_curve_a, surfbnd_curve_b);
        
        //For each A3-rib, draw the two ridgebnd curves.
        root->addChild (draw_polyline (surfbnd_curve_a, width, A13_surfbnd_color_a));
        root->addChild (draw_polyline (surfbnd_curve_b, width, A13_surfbnd_color_b));

      }
      HE = HE->next();
    }
    while (HE != MS->halfedge() && HE != NULL);    
  }

  vul_printf (vcl_cout, "done.\n");
  return root;
}

//: Visualization of ridge vectors along A3 ribs.
SoSeparator* draw_A3_ridge_vectors (dbsk3d_ms_hypg* ms_hypg, 
                                   const SbColor& color, const float& vector_len)
{
  vul_printf (vcl_cout, "draw_A3_ridge_vectors()\n");

  SoSeparator* root = new SoSeparator;
  //assign color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);    

  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (3);

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg->edgemap().begin();
  for (; cit != ms_hypg->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;

    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(0);
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    vgl_point_3d<double> C = FF->compute_center_pt();
    vgl_vector_3d<double> ridgeV = get_init_ridge_vector (FE, FF, C);

    dbsk3d_fs_edge* prevL = FE;
    
    //Go through all linkElms of this A3Rib.    
    for (unsigned int j=1; j<MC->E_vec().size(); j++) {
      FE = (dbsk3d_fs_edge*) MC->E_vec(j);
      FF = (dbsk3d_fs_face*) FE->halfedge()->face();
      
      //compute the center point of the FF
      C = FF->compute_center_pt();
      //compute the ridge vector
      ridgeV = get_ridge_vector (FE, FF, C, prevL);

      //Draw the ridge vector  
      vgl_vector_3d<double> ridge_Vn = ridgeV / ridgeV.length() * vector_len;
      root->addChild (draw_line (C, C + ridge_Vn, color, drawStyle));

      prevL = FE;
    }//end for j
    
    vul_printf (vcl_cout, "shock curve %d \n", MC->id());
  }//end for i

  vul_printf (vcl_cout, "done.\n");
  return root;
}


//: Visualization of ridge curves along A3 ribs.
SoSeparator* draw_A3_ridge_pts (dbsk3d_ms_hypg* ms_hypg, 
                                const float& radius, const SbColor& color)
{  
  vul_printf (vcl_cout, "draw_A3_ridge_curves(): totally %d shock curves.\n", ms_hypg->edgemap().size());

  SoSeparator* root = new SoSeparator;
  //assign color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);    

  SoDrawStyle* drawStyle = new SoDrawStyle;
  ///drawStyle->lineWidth.setValue (width);

  vcl_vector< vgl_point_3d<double> > ridge_curve;

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg->edgemap().begin();
  for (; cit != ms_hypg->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;

    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(0);
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    vgl_point_3d<double> C = FF->compute_center_pt();
    vgl_vector_3d<double> ridgeV = get_init_ridge_vector (FE, FF, C);

    dbsk3d_fs_edge* prevL = FE;
    ridge_curve.clear();
    
    //Go through all linkElms of this A3Rib.    
    for (unsigned int j=1; j<MC->E_vec().size(); j++) {
      FE = (dbsk3d_fs_edge*) MC->E_vec(j);
      FF = (dbsk3d_fs_face*) FE->halfedge()->face();
      
      //compute the center point of the FF
      C = FF->compute_center_pt();
      //compute the ridge vector
      ridgeV = get_ridge_vector (FE, FF, C, prevL);

      //Intersect this ridgeV to a surface triangle
      vgl_point_3d<double> ridgePt;
      dbmsh3d_face* tri = intersect_ray_mesh (C, ridgeV, ms_hypg->fs_mesh()->bnd_mesh(), ridgePt);
      ///assert (tri);
      if (tri) {
        //draw the ridge point
        root->addChild (draw_sphere_geom (ridgePt, radius));
        ridge_curve.push_back (ridgePt);
      }
      else {
        vul_printf (vcl_cout, "\n Error! ridgeV intersects no surface triangle!\n"); 
        vul_printf (vcl_cout, "MC %d, A3RibElm %d, FF %d",
                     MC->id(), FE->id(), FF->id());
      }

      prevL = FE;
    }//end for j
    
    vul_printf (vcl_cout, "shock curve %d \n", MC->id());
    
    //visualize the ridge curve    
    ///root->addChild (draw_polyline (ridge_curve, width, color));

  }//end for i


  vul_printf (vcl_cout, "done.\n");
  return root;
}

//: Visualization of ridge curves along A3 ribs.
SoSeparator* draw_A3_ridge_curves (dbsk3d_ms_hypg* ms_hypg, 
                                   const float& radius, const SbColor& color,
                                   const float& width)
{  
  vul_printf (vcl_cout, "draw_A3_ridge_curves(): totally %d shock curves.\n", ms_hypg->edgemap().size());

  SoSeparator* root = new SoSeparator;
  //assign color
  SoBaseColor* basecolor = new SoBaseColor;
  basecolor->rgb = color;
  root->addChild (basecolor);    

  SoDrawStyle* drawStyle = new SoDrawStyle;
  drawStyle->lineWidth.setValue (width);

  vcl_vector< vgl_point_3d<double> > ridge_curve;

  vcl_map<int, dbmsh3d_edge*>::iterator cit = ms_hypg->edgemap().begin();
  for (; cit != ms_hypg->edgemap().end(); cit++) {
    dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*cit).second;
    if (MC->c_type() != C_TYPE_RIB) 
      continue;

    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) MC->E_vec(0);
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    vgl_point_3d<double> C = FF->compute_center_pt();
    vgl_vector_3d<double> ridgeV = get_init_ridge_vector (FE, FF, C);

    dbsk3d_fs_edge* prevL = FE;
    ridge_curve.clear();
    
    //Go through all linkElms of this A3Rib.    
    for (unsigned int j=1; j<MC->E_vec().size(); j++) {
      FE = (dbsk3d_fs_edge*) MC->E_vec(j);
      FF = (dbsk3d_fs_face*) FE->halfedge()->face();
      
      //compute the center point of the FF
      C = FF->compute_center_pt();
      //compute the ridge vector
      ridgeV = get_ridge_vector (FE, FF, C, prevL);

      //Intersect this ridgeV to a surface triangle
      vgl_point_3d<double> ridgePt;
      dbmsh3d_face* tri = intersect_ray_mesh (C, ridgeV, ms_hypg->fs_mesh()->bnd_mesh(), ridgePt);
      ///assert (tri);
      if (tri) {
        //draw the ridge point
        root->addChild (draw_cube_geom (ridgePt, radius));
        ridge_curve.push_back (ridgePt);
      }
      else {
        vul_printf (vcl_cout, "\n Error! ridgeV intersects no surface triangle!\n"); 
        vul_printf (vcl_cout, "MC %d, A3RibElm %d, FF %d",
                     MC->id(), FE->id(), FF->id());
      }

      prevL = FE;
    }//end for j
    
    vul_printf (vcl_cout, "shock curve %d \n", MC->id());
    
    //visualize the ridge curve    
    root->addChild (draw_polyline (ridge_curve, width, color));

  }//end for i


  vul_printf (vcl_cout, "done.\n");
  return root;
}
