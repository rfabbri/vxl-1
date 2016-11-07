//: This is lemsvxlsrc/brcv/shp/dbsk3d/pro/dbsk3d_process_vis.cxx
//  Creation: Dec 24, 2005   Ming-Ching Chang

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <vgl/vgl_vector_3d.h>


#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_point_set.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>

#include <dbsk3d/vis/dbsk3d_process_vis.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>


//###############################################################
//  Visualization Functions in dbmsh3d_vis_base 
//###############################################################

SoSeparator* dbsk3d_pro_vis::vis_ptset (const int colorcode, const bool draw_idv)
{
  SoSeparator* root = new SoSeparator;
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("bnd,pt"));
  root->addChild (soswitch);

  int ccode = (colorcode == COLOR_NOT_SPECIFIED) ? m_vertex_ccode_ : colorcode;

  if (pro_data_ == PD_PTS)
    return draw_ptset (pts_, colorcode, pt_size_);
  else if (pro_data_ == PD_IDPTS)
    assert (0);
  else if (pro_data_ == PD_ORIPTS)
    assert (0); 
  else if (pro_data_ == PD_SG3PI)
    assert (0); 
  else if (pro_data_ == PD_MESH) {
    if (draw_idv) //-idv 1: 
      soswitch->addChild (draw_ptset_idv (mesh_, ccode, cube_size_, user_defined_class_));
    else
      soswitch->addChild (draw_ptset (mesh_, ccode, pt_size_));
  }
  else
    assert (0); 

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ptset_idv ()
{
  SoSeparator* root = new SoSeparator;
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("bnd,pt"));
  root->addChild (soswitch);

  soswitch->addChild (draw_ptset_idv (mesh_, m_vertex_ccode_, 
                                      cube_size_, user_defined_class_));
  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ptset_color ()
{
  return draw_pt_set_color (mesh_, mesh_vertex_color_set_, pt_size_, user_defined_class_);
}

SoSeparator* dbsk3d_pro_vis::vis_mesh (const bool draw_idv, 
                                       const bool b_shape_hints,
                                       const int colorcode,
                                       const bool b_draw_isolated_pts)
{
  vul_printf (vcl_cerr, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());

  SoSeparator* root = new SoSeparator;

  //Add switch
  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("boundary,mesh"));
  root->addChild (soswitch); 

  mesh_->mark_unmeshed_pts ();
  
  if (draw_idv) //Draw each individual F,E,V as separate object to allow getInfo.
    soswitch->addChild (draw_M_faces_idv (mesh_, true, m_transp_, colorcode, user_defined_class_));
  else //Draw the whole mesh as a single object
    soswitch->addChild (draw_M (mesh_, b_shape_hints,  m_transp_, colorcode));

  //Draw the unmeshed vertices as point cloud.
  if (b_draw_isolated_pts)
    root->addChild (draw_ptset (mesh_, m_vertex_ccode_, pt_size_, true));

  unsigned n_unmeshed_pts = mesh_->count_unmeshed_pts ();
  vul_printf (vcl_cerr, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_mesh_edges (const int colorcode, const float width,
                                             const bool draw_idv)
{
  if (draw_idv == false)
    return draw_M_edges (mesh_, color_from_code (colorcode), width);
  else
    return draw_M_edges_idv (mesh_, color_from_code (colorcode), width);
}

SoSeparator* dbsk3d_pro_vis::vis_mesh_pts_edges (const int colorcode, const bool draw_idv,
                                                 const float pt_size)
{
  SoSeparator* root = new SoSeparator;  
  int ccode = (colorcode == COLOR_NOT_SPECIFIED) ? m_vertex_ccode_ : colorcode;
  root->addChild (draw_ptset (mesh_, ccode, pt_size));
  root->addChild (vis_mesh_edges (COLOR_GRAY, draw_idv));
  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_mesh_bndcurve (const int colorcode, 
                                                const float width)
{
  return draw_M_bndcurve (mesh_, colorcode, width);
}

SoSeparator* dbsk3d_pro_vis::vis_mesh_color ()
{
  return draw_M_color (mesh_, true, m_transp_, mesh_face_color_set_, user_defined_class_);
}

//: Visualize the reconstructed surface mesh according to the input option.
//  Note that this surface is the 
SoSeparator* dbsk3d_pro_vis::vis_mesh_options (int option, const bool draw_idv, const bool showid)
{
  vul_printf (vcl_cerr, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());
  SoSeparator* root = new SoSeparator;
  if (mesh_->b_watertight())
    option = 1; //draw gold color for watertight surface

  mesh_->mark_unmeshed_pts ();

  switch (option) {
  case 1: //-v 1: visualize the surface mesh as one single object (fastest).
    if (mesh_->b_watertight())
      root->addChild (draw_M (mesh_, true, m_transp_, COLOR_GOLD));
    else
      root->addChild (draw_M (mesh_, true, m_transp_));
  break;
  case 2: //-v 2: distinguish acute vs obtuse triangles.
    root->addChild (draw_M_bnd_faces_cost_col (mesh_, draw_idv, showid, m_transp_));
  break;
  case 0:
  case 3: //-v 3: distinguish triangle topology.
    root->addChild (draw_M_bnd_faces_topo_col (mesh_, draw_idv, showid, m_transp_,
                                                 user_defined_class_));
    root->addChild (draw_M_topo_vertices (mesh_, 1, cube_size_, user_defined_class_));
  break;
  }

  //Draw the unmeshed vertices as point cloud.
  root->addChild (draw_ptset (mesh_, m_vertex_ccode_, pt_size_, true));

  unsigned n_unmeshed_pts = mesh_->count_unmeshed_pts ();
  vul_printf (vcl_cerr, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

//: Visualize the reconstructed surface mesh in animation
SoSeparator* dbsk3d_pro_vis::vis_mesh_anim (const int nF_batch)
{
  vul_printf (vcl_cerr, "vis_mesh_anim(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());
  SoSeparator* root = new SoSeparator;
  root->addChild (draw_M_bnd_faces_anim (mesh_));
  return root;
}

// ######################################################################

bool dbsk3d_pro_vis::compute_dx_dy_dz (const float ratio)
{
  //Ignore if any of dx, dy, dz is a specified value (other than -1).
  if (view_dx_ != -1 || view_dy_ != -1 || view_dz_ != -1)
    return false;

  //Compute view shift from the enlarged bounding box.
  vgl_box_3d<double> bbox;
  bool r = detect_bounding_box (mesh_, bbox);

  view_dx_ = (float) (bbox.width() * ratio);
  view_dy_ = (float) (bbox.height() * ratio);
  view_dz_ = (float) (bbox.depth() * ratio);

  vul_printf (vcl_cerr, "    compute_dx_dy_dz(): \n");
  vul_printf (vcl_cerr, "\tBounding box width %f, height %f, depth %f.\n",
               bbox.width(), bbox.height(), bbox.depth());
  vul_printf (vcl_cerr, "\tSet view dx %f, dy %f, dz %f.\n",
               view_dx_, view_dy_, view_dz_);
  return r;
}

//: Compute the cube and ball size for visualization of graph/hypergraph.
//  If the fs_mesh_ is available, use fs_mesh_->compute_median_A122_dist().
//  If the sg_sa_ is available, use sg_sa_->compute_medialn_link_len();
void dbsk3d_pro_vis::compute_vertex_node_size ()
{
  vul_printf (vcl_cerr, "    compute_vertex_node_size(): ");

  if (fs_mesh_->facemap().size() != 0) {
    fs_mesh_->compute_median_A122_dist();
    cube_size_ = (float) (fs_mesh_->median_A122_dist() / 10);
    ms_vis_param_.ball_size_ = (float) (fs_mesh_->median_A122_dist() / 6); 
    
    vul_printf (vcl_cerr, "cube %f, ball %f.\n", cube_size_, ms_vis_param_.ball_size_);
    return;
  }

  if (sg_sa_->edgemap().size() != 0) {
    sg_sa_->compute_median_edge_len();
    cube_size_ = (float) (sg_sa_->median_edge_len() / 2); // 10, 5
    ms_vis_param_.ball_size_ = (float) (sg_sa_->median_edge_len()/1.5); //6, 3
    
    vul_printf (vcl_cerr, "cube %f, ball %f.\n", cube_size_, ms_vis_param_.ball_size_);
    return;
  }
  
  //Can't estimate the avg_a12_dist. Use default value.
  cube_size_ = dbmsh3d_cmd_rc();
  ms_vis_param_.ball_size_ = dbmsh3d_cmd_r();
  vul_printf (vcl_cerr, "default: cube %f, ball %f.\n", cube_size_, ms_vis_param_.ball_size_);
}


//###############################################################

SoSeparator* dbsk3d_pro_vis::vis_unasgn_genes (const float size)
{
  SoSeparator* root = new SoSeparator;

  //color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = color_from_code (COLOR_DARKCYAN);
  root->addChild (basecolor);

  vcl_vector<dbmsh3d_vertex*> unasgn_genes;
  fs_mesh_->check_all_G_asgn (unasgn_genes);

  for (unsigned int i=0; i< unasgn_genes.size(); i++) {
    dbmsh3d_vertex* G = unasgn_genes[i];    
    root->addChild (draw_vertex_geom_vispt_SoCube (G, size));
  }

  return root;
}

// ###################################################################

// Draw only non-1-ring genes individually.
SoSeparator* dbsk3d_pro_vis::vis_vertices_of_holes ()
{
  return draw_M_topo_vertices (mesh_, 2, cube_size_, user_defined_class_);
}

//: Visualize the leftover shocks around holes.
// -n 1: draw only related shock sheet elements with any generator at hole.
// -n 2: draw only related shock sheet elements with both generators at hole.
// -n 3: draw only related shock link elements.
SoSeparator* dbsk3d_pro_vis::vis_shocks_of_holes (int option)
{
  SoSeparator* root = new SoSeparator;

  if (option==1 || option==2) {
    //Draw valid shocks of non-1-ring object vertices.
    //related shock FF->i_value_ is set to 2.
    root->addChild (draw_Ps_non1ring_Gs (fs_mesh_, option, user_defined_class_));
  }
  else if (option==3) {
    //Go through all patches with i_value_ == 2 and identify shock links of holes.
    root->addChild (draw_Ls_non1ring_Gs (fs_mesh_, user_defined_class_));
  }

  return root;
}

// ######################################################################

SoSeparator* dbsk3d_pro_vis::vis_fs_mesh_valid (const bool draw_idv)
{
  if (draw_idv == false)
    return draw_fs_mesh_valid (fs_mesh_);
  else
    return draw_fs_mesh_patches (fs_mesh_->facemap(), 
                                 true, //draw_valid,
                                 false, //draw_invalid, 
                                 true, //draw_unbounded,
                                 true, //draw_unvisited,
                                 user_defined_class_);
}

SoSeparator* dbsk3d_pro_vis::vis_fs_mesh (const int colorcode)
{
  return draw_fs_mesh (fs_mesh_, colorcode);
}

SoSeparator* dbsk3d_pro_vis::vis_fs_mesh (const bool draw_valid,
                                          const bool draw_invalid, 
                                          const bool draw_unbounded)
{
  return draw_fs_mesh (fs_mesh_, 0, draw_valid, draw_invalid, draw_unbounded, 
                       cube_size_, 0.0f, user_defined_class_);
}

#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/SbRotation.h>

SoSeparator* dbsk3d_pro_vis::display_sheet_flow ()
{
  SoSeparator* flowRoot = new SoSeparator;

  SoSwitch* flowSwitch = new SoSwitch;
  flowSwitch->whichChild = SO_SWITCH_ALL;
  flowSwitch->setName( SbName( "Flow" ) );  

  flowRoot->addChild( flowSwitch );

  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;
    
    if (FF->b_valid())  {
      vgl_point_3d<double> center_pt = FF->compute_center_pt();
      SbVec3f srcVec(center_pt.x(), center_pt.y(), center_pt.z() );

      vgl_vector_3d<double> p = FF->get_shock_flow( center_pt );
      SbVec3f flowVec( p.x(), p.y(), p.z() );

      SoSeparator* flowGroup = new SoSeparator;
  
      SoSeparator* cylGroup = new SoSeparator;
      flowGroup->addChild( cylGroup );
  
      flowVec/=10.0f;

      // make rendering faster for complex shapes
      SoComplexity* complexity = new SoComplexity;
      complexity->type = SoComplexity::SCREEN_SPACE;
      complexity->value = 0.1f;
      flowGroup->addChild( complexity );

      // CYLINDER AS ARROW
      SoCylinder* cyl = new SoCylinder;
      cyl->radius = flowVec.length()/10.0f;
      cyl->height = flowVec.length();
  
      SoTransform* cylTrans = new SoTransform;
      cylTrans->translation = srcVec+flowVec/2;
      cylTrans->rotation = SbRotation( SbVec3f(0,1,0), flowVec );
  
      cylGroup->addChild( cylTrans );
      cylGroup->addChild( cyl );

      SoSeparator* sep = new SoSeparator;
      flowGroup->addChild( sep );
  
      SoTransform* trans = new SoTransform;
      trans->translation = srcVec+flowVec;
      float length = flowVec.length()/5.0f;
      trans->scaleFactor = SbVec3f(length,length,length);
      trans->rotation = SbRotation( SbVec3f(0,1,0), flowVec );
      sep->addChild( trans );

      SoCone* cone = new SoCone;
      sep->addChild( cone );
      
      if( length < 1 )
        flowSwitch->addChild( flowGroup );
    }
  }

  return flowRoot;
}

SoSeparator* dbsk3d_pro_vis::reconstruct()
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* reconSwitch = new SoSwitch;
  reconSwitch->whichChild = SO_SWITCH_ALL;
  reconSwitch->setName( SbName( "Reconstruction" ) ); 
  root->addChild( reconSwitch );

  SoSeparator* sep = new SoSeparator;
  reconSwitch->addChild(sep);

  SoCoordinate3* coord = new SoCoordinate3;
  coord->point.deleteValues(0);
  SoPointSet* ps = new SoPointSet;

  sep->addChild( coord );
  sep->addChild( ps );

  vcl_map<int, dbmsh3d_face*>::iterator pit = fs_mesh_->facemap().begin();
  for (; pit != fs_mesh_->facemap().end(); pit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*pit).second;

    if (FF->b_valid()) {      
      vgl_point_3d<double> plus, minus;
      
      vgl_point_3d<double> center_pt = FF->compute_center_pt();
      FF->reconstruct_two_gene (center_pt, plus, minus );
      coord->point.set1Value( coord->point.getNum(), plus.x(), plus.y(), plus.z() );
      coord->point.set1Value( coord->point.getNum(), minus.x(), minus.y(), minus.z() );
    }    
  }

  return root;
}

// ######################################################################

SoSeparator* dbsk3d_pro_vis::vis_fs_mesh_color_by_radius ()
{
  return draw_fs_mesh_color_by_radius (fs_mesh_, ssheet_transp_);
}

SoSeparator* dbsk3d_pro_vis::vis_ms_graph (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,graph" ) );
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (ms_hypg_, 
                         ms_vis_param_.A1A3_vertex_color_, ms_vis_param_.A14_vertex_color_,
                         ms_vis_param_.Dege_vertex_color_, ms_vis_param_.Loop_vertex_color_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_, ms_vis_param_.A13_axial_color_,
                         ms_vis_param_.Dege_axial_color_, ms_vis_param_.A3_loop_color_,
                         ms_vis_param_.curve_width_, idv,
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_sg_sa (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,graph" ) );
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (sg_sa_,
                         ms_vis_param_.A1A3_vertex_color_, ms_vis_param_.A14_vertex_color_,
                         ms_vis_param_.Dege_vertex_color_, ms_vis_param_.Loop_vertex_color_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_, ms_vis_param_.A13_axial_color_,
                         ms_vis_param_.Dege_axial_color_, ms_vis_param_.A3_loop_color_,
                         ms_vis_param_.curve_width_, idv,
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ms_graph_2 (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,graph" ) );
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (sg_sa_, ///ms_hypg_, 
                         ms_vis_param_.A1A3_vertex_color_2_, ms_vis_param_.A14_vertex_color_2_,
                         ms_vis_param_.Dege_vertex_color_2_, ms_vis_param_.Loop_vertex_color_2_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_2_, ms_vis_param_.A13_axial_color_2_,
                         ms_vis_param_.Dege_axial_color_2_, ms_vis_param_.A3_loop_color_2_,
                         ms_vis_param_.curve_width_, idv, 
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_sg_sa_2 (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,graph" ) );
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (sg_sa_, ///ms_hypg_, 
                         ms_vis_param_.A1A3_vertex_color_2_, ms_vis_param_.A14_vertex_color_2_,
                         ms_vis_param_.Dege_vertex_color_2_, ms_vis_param_.Loop_vertex_color_2_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_2_, ms_vis_param_.A13_axial_color_2_,
                         ms_vis_param_.Dege_axial_color_2_, ms_vis_param_.A3_loop_color_2_,
                         ms_vis_param_.curve_width_, idv, 
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));

  return root;
}

//: Visualize the ms_hypg according to given option.
//  0: draw the medial scaffold graph and randomly color each sheet but avoiding color close to red or blue.
//  1: draw the medial scaffold graph and draw the whole ms_sheets in a single color.
//  2: only draw the (medial scaffold) graph and skip drawing all sheets.
//  3: only draw the ms_sheets in random colors and skip drawing the (medial scaffold) graph.
//  4: only draw the ms_sheets in a single color and skip drawing the (medial scaffold) graph.
//  5: color each ms_sheet by cost (in pseudo color).
//   6: color each ms_sheet by cost (in pseudo color).
//   7: color each ms_sheet by geodesic distance.
//   8: not drawing anything.
//
SoSeparator* dbsk3d_pro_vis::vis_ms_hypg (const int option, const int color_seed, const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,hypg" ) );
  root->addChild (soswitch);

  if (option==0 || option==3) {
    //Generate the sheet color table using the id counter instead of size()
    generate_shock_color_table (color_seed, ms_hypg_->sheet_id_counter(), 
                                ms_vis_param_.sheets_ctable_);

    //Visualize each ms_sheets using the color table.
    soswitch->addChild (draw_ms_sheets_ctable (ms_hypg_, ms_vis_param_.sheets_ctable_, 
                            ms_vis_param_.sheet_transp_, idv,
                            ms_vis_param_.sheet_show_id_, user_defined_class_));
  }
  else if (option==1 || option==4) {
    //Use the color_seed as COLOR_CODE
    SbColor color = color_from_code (color_seed);
    soswitch->addChild (draw_ms_sheets (ms_hypg_, color, ms_vis_param_.sheet_transp_, idv,
                                        ms_vis_param_.sheet_show_id_, user_defined_class_));
  }

  if (option==0 || option==1 || option==2) 
    soswitch->addChild (draw_ms_graph (ms_hypg_,     
                            ms_vis_param_.A1A3_vertex_color_, ms_vis_param_.A14_vertex_color_,
                            ms_vis_param_.Dege_vertex_color_, ms_vis_param_.Loop_vertex_color_,
                            ms_vis_param_.ball_size_, 
                            ms_vis_param_.A3_rib_color_, ms_vis_param_.A13_axial_color_,
                            ms_vis_param_.Dege_axial_color_, ms_vis_param_.A3_loop_color_,
                            ms_vis_param_.curve_width_, idv,
                            ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                            user_defined_class_));  

  if (option==5 || option==6)
    soswitch->addChild (draw_ms_sheets_cost (ms_hypg_, option, ms_vis_param_.sheet_transp_, idv,
                            ms_vis_param_.sheet_show_id_, user_defined_class_));

  if (option==7) {
    if (ms_hypg_fmm_->sheet_fmm_mesh_map().size() == 0) {      
      ms_hypg_fmm_->build_sheets_fmm_mesh ();
      ms_hypg_fmm_->run_fmm_on_sheet_meshes ();
    }
    soswitch->addChild (draw_ms_sheets_gdt (ms_hypg_fmm_->sheet_fmm_mesh_map(), ms_vis_param_.sheet_transp_));
  }

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ms_hypg_bnd_mesh (const int option)
{
  return draw_ms_sheets_bnd_mesh_ctable (ms_hypg_, option, ms_vis_param_.sheets_ctable_, m_transp_, user_defined_class_);
}

SoSeparator* dbsk3d_pro_vis::vis_ms_hypg_ctable (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,hypg" ) );
  root->addChild (soswitch);

  soswitch->addChild (draw_ms_sheets_ctable (ms_hypg_, 
                         ms_vis_param_.sheets_ctable_, 
                         ms_vis_param_.sheet_transp_, idv, ms_vis_param_.sheet_show_id_,
                         user_defined_class_));

  soswitch->addChild (draw_ms_graph (ms_hypg_, 
                         ms_vis_param_.A1A3_vertex_color_, ms_vis_param_.A14_vertex_color_,
                         ms_vis_param_.Dege_vertex_color_, ms_vis_param_.Loop_vertex_color_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_, ms_vis_param_.A13_axial_color_,
                         ms_vis_param_.Dege_axial_color_, ms_vis_param_.A3_loop_color_,
                         ms_vis_param_.curve_width_, idv,
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_)); 
  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ms_hypg_2_ctable (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName( SbName( "ms,hypg" ) );
  root->addChild (soswitch);

  soswitch->addChild (draw_ms_sheets_ctable (ms_hypg_, 
                         ms_vis_param_.sheets_ctable_, 
                         ms_vis_param_.sheet_transp_, idv, ms_vis_param_.sheet_show_id_,
                         user_defined_class_));

  soswitch->addChild (draw_ms_graph (ms_hypg_, 
                         ms_vis_param_.A1A3_vertex_color_2_, ms_vis_param_.A14_vertex_color_2_,
                         ms_vis_param_.Dege_vertex_color_2_, ms_vis_param_.Loop_vertex_color_2_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_2_, ms_vis_param_.A13_axial_color_2_,
                         ms_vis_param_.Dege_axial_color_2_, ms_vis_param_.A3_loop_color_2_,
                         ms_vis_param_.curve_width_, idv, 
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));

  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ms_graph_sa (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("ms,graph")); 
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (sg_sa_, 
                         ms_vis_param_.A1A3_vertex_color_, ms_vis_param_.A14_vertex_color_,
                         ms_vis_param_.Dege_vertex_color_, ms_vis_param_.Loop_vertex_color_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_, ms_vis_param_.A13_axial_color_,
                         ms_vis_param_.Dege_axial_color_, ms_vis_param_.A3_loop_color_,
                         ms_vis_param_.curve_width_, idv, 
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));
  return root;
}

SoSeparator* dbsk3d_pro_vis::vis_ms_graph_sa_2 (const bool idv)
{
  SoSeparator* root = new SoSeparator;

  SoSwitch* soswitch = new SoSwitch;
  soswitch->whichChild = SO_SWITCH_ALL;
  soswitch->setName (SbName("ms,graph")); 
  root->addChild( soswitch );

  soswitch->addChild (draw_ms_graph (sg_sa_, 
                         ms_vis_param_.A1A3_vertex_color_2_, ms_vis_param_.A14_vertex_color_2_,
                         ms_vis_param_.Dege_vertex_color_2_, ms_vis_param_.Loop_vertex_color_2_,
                         ms_vis_param_.ball_size_, 
                         ms_vis_param_.A3_rib_color_2_, ms_vis_param_.A13_axial_color_2_,
                         ms_vis_param_.Dege_axial_color_2_, ms_vis_param_.A3_loop_color_2_,
                         ms_vis_param_.curve_width_, idv, 
                         ms_vis_param_.vertex_show_id_, ms_vis_param_.curve_show_id_,
                         user_defined_class_));
  return root;
}

//############################## Visualization of Files ##############################


SoSeparator* dbsk3d_pro_vis::view_vor ()
{
  SoSeparator* group = new SoSeparator;
  return group;
}

SoSeparator* dbsk3d_pro_vis::view_cms (vcl_string prefix)
{
  if (load_cms(prefix))
    return vis_ms_hypg ();
  else 
    return NULL;
}

SoSeparator* dbsk3d_pro_vis::view_sg (vcl_string prefix)
{
  if (load_sg(prefix))
    return vis_ms_graph ();
  else 
    return NULL;
}



// ###################################################################
//: Read the list file and view each file.
//  Format of list file:
//    One line per dataset: (data_file) [data_alignment_hmatrix]
//    The alignment is optional.
//
//    - option ==0 : nothing.
//             ==1 : save points into pts_[].
//             ==2 : save points into pts_[] and faces into ifs_faces[][].
//    - vis_option ==0 : show each file in random color.
//                 ==1 : show all files in a single color (silver).
//
SoSeparator* dbsk3d_pro_vis::vis_list_file (vcl_string filename, 
                                            const int option, const int vis_option)
{
  SoSeparator* root = new SoSeparator;
  
  if (dbul_get_suffix (filename) == "")
    filename += ".txt";

  vul_printf (vcl_cerr, "dbsk3dr_pro_vis::vis_list_file(): %s.\n", filename.c_str());

  vcl_ifstream  in;
  vcl_string    linestr;
  in.open (filename.c_str());
  if (in == false) {
    vul_printf (vcl_cerr, "Can't open listfile %s\n", filename.c_str());
    return root; 
  }

  //Visualization: either in random colors or a single color.
  VIS_COLOR_CODE ccode = COLOR_RANDOM;
  if (vis_option == 1)
    ccode = COLOR_SILVER;
  init_rand_color (DBMSH3D_MESH_COLOR_SEED);

  //Read in each line of dataset in the list file.
  int file_count = 0;
  int af_count = 0;
  int n_total_pts = 0;
  int n_total_faces = 0;
  while (in) {
    linestr.clear();
    vcl_getline (in, linestr);

    if (linestr.length() == 0 || vcl_strncmp (linestr.c_str(), "#", 1) == 0)
      continue; //skip empty line and comments.

    char file[256] = "", file_af[256] = "";
    vcl_sscanf (linestr.c_str(), "%s %s", file, file_af);
    DBMSH3D_FILE_TYPE type;
    
    //1) Read in the data file
    if (vcl_strcmp (file, "") != 0) {
      //clean up the datastructure before loading.
      reset_data (); 
      pro_data_ = PD_MESH;
      reset_shock_data ();
      
      vcl_string suffix = dbul_get_suffix (file);
      if (suffix == ".cms") {
        type = DBMSH3D_FILE_CMS;
        load_cms (file);
      }
      else if (suffix == ".sg") {
        type = DBMSH3D_FILE_SG;
        load_sg (file); 
      }
      else {
        file_count--;
      }
      file_count++;
    }

    //2) Read in the alignment file (if any)
    if (vcl_strcmp (file_af, "") != 0) {      
      load_hmatrix (file_af); //read in the alignment file.
      apply_xform_hmatrix (); //xform the dataset.
      af_count++;
    }

    //3) Display the data file in color.
    //-v 0: regular, 
    if (vis_option==0) {
      if (ccode == COLOR_RANDOM) {
        VIS_COLOR_CODE c = get_next_rand_ccode ();
        root->addChild (vis_fs_mesh (c)); //the medial scaffold mesh.
        root->addChild (vis_ptset (c)); //the boundary point cloud.
      }
    }
    //-v 1: draw in single color
    else if (vis_option==1) {
      root->addChild (vis_fs_mesh (COLOR_SILVER)); //the medial scaffold mesh.
      root->addChild (vis_ptset ());
    }
    //-v 2: color each sheet.
    else if (vis_option==2) {
      root->addChild (vis_ms_hypg());
      root->addChild (vis_ptset ());
    }
    //-v 3: draw only the graph
    else if (vis_option==3) {
      root->addChild (vis_ms_graph());
      root->addChild (vis_ptset ());
    }
    //-v 4: draw the standalone graph sg_sa.
    else if (vis_option==4) {
      root->addChild (vis_sg_sa());
      root->addChild (vis_ptset ());
    }


    //4) If option==1, save points into pts_[].
    /*if (option==1 || option==2)
      _copy_mesh_to_pts ();
    else if (option==2)
      _copy_mesh_to_ifs_faces ();*/
    
    if (type == DBMSH3D_FILE_3PI)
      n_total_pts += pts_.size();
    else {
      n_total_pts += mesh_->vertexmap().size();
      n_total_faces += mesh_->facemap().size();
    }

    vul_printf (vcl_cerr, "\t Currently %u points %u faces loaded.\n", 
                n_total_pts, n_total_faces);
  }

  in.close();
  if (option==1)
    assert (pts_.size() == n_total_pts);

  vul_printf (vcl_cerr, "\n====================================================\n");
  vul_printf (vcl_cerr, "  vis_list_file(): %d files (%d align. files) shown from %s.\n", 
              file_count, af_count, filename.c_str());
  vul_printf (vcl_cerr, "\tTotally %u points, %u faces.\n", 
              n_total_pts, n_total_faces);
  return root;
}

void dbsk3d_pro_vis::parse_run_file (const char* prefix)
{

  //1)If file open fails, return.
  vcl_ifstream fp (prefix,vcl_ios::in);
  if (!fp){
    vcl_cout<<"\n\t Unable to Open "<<prefix<<vcl_endl;
    return;
  }

  //: parse the run file to get those values
  vcl_string s = "init";
  while (s != "") {
    fp >> s;
    if (s == "-prefix") {
      vcl_string  prefix;
      fp >> prefix;

      set_dir_prefix (prefix);
    }
    else if (s == "-tab") {
      int tab_th;
      fp >> tab_th;
      ///proc_param_.trans_tab_th_ = tab_th;
    }
    else if (s == "-a5") {
      float a5_th;
      fp >> a5_th;
      proc_param_.trans_A5_th_ = a5_th;
    }
    else if (s == "-a15") {
      float a15_th;
      fp >> a15_th;
    }
    else if (s == "-smooth") {
      int n_smooth;
      fp >> n_smooth;
      proc_param_.n_smooth_ = n_smooth;
    }
    else if (s == "-r") {
      float ballr;
      fp >> ballr;
      ms_vis_param_.ball_size_ = ballr;
    }
    else if (s == "-rc") {
      float cuber;
      fp >> cuber;
      cube_size_ = cuber;
    }
  }

  fp.close();
}




