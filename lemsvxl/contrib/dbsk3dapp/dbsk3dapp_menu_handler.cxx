//---------------------------------------------------------------------
// This is lemsvxlsrc/contrib/dbsk3dapp/dbsk3dapp_menu_handler.cxx
//:
// \file
// \brief dbsk3dapp menu handler
//
//
// \author
//  MingChing Chang  May 05, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vgui/vgui.h>
#include <bgui3d/bgui3d_file_io.h>
#include <dbmsh3d/pro/dbmsh3d_cmdpara.h>

#include "dbsk3dapp_manager.h"

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbsk3d/vis/dbsk3d_vis_backpt.h>
#include <dbsk3d/vis/dbsk3d_vis_fs_elm.h>

#include <Inventor/actions/SoSearchAction.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>

#define SELECT_COLOR SbColor(1,0.5,1)

// ###############################################################################
//       File Menu
// ###############################################################################

void dbsk3dapp_manager::file_new()           
{
  //: reset the datastructure
  clear_all_reset();

  //: clean up the drawing
  view_manager_->clear_all_views ();
}

void dbsk3dapp_manager::file_open()           
{
  vgui_dialog params ("File Open");
  vcl_string ext;

  params.file ("Open...", ext, file_opened_cache_);  

  int open_in_view = 1;
  choose_view (params, open_in_view);

  bool b_clear_view = true;
  params.checkbox ("Clear this view prior opening (default true)", b_clear_view);

  bool shape_hints = true;
  params.checkbox ("Use SoShapeHints in drawing mesh (default true)", shape_hints);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  //get the dir_prefix and suffix of the file name
  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));
  vcl_string suffix = dbul_get_suffix (file_opened_cache_);

  SoSeparator* group = NULL;

  // *.IV or *.iv file opening 
  ///if (suffix==".IV" || suffix==".iv") {
    ///group = pv1_->view_iv (file_opened_cache_);
  ///}
  // *.ply2 file opening
  ///else 
  ///if (suffix==".PLY2" || suffix==".ply2") {
    ///group = pv1_->view_ply2 (file_opened_cache_, shape_hints);
  ///}
  // *.ply file opening
  /*if (suffix==".PLY" || suffix==".ply") {
    group = pv1_->view_ply (file_opened_cache_, shape_hints);
  }
  // *.p3d file opening
  else if (suffix==".P3D" || suffix==".p3d") {
    group = pv1_->view_p3d (file_opened_cache_);
  }
  // *.g3d file opening
  else if (suffix==".G3D" || suffix==".g3d") {
    group = pv1_->view_g3d (file_opened_cache_);
  }
  // *.fs file opening
  else if (suffix==".FS" || suffix==".fs") {
    pv1_->load_fs();

    if (open_in_view != 0) {
      // need to turn draw_invalid option on,because all elements now are invalid.
      group = pv1_->vis_fs_mesh (true, true);
    }
  }
  // *.cms file opening
  else if (suffix==".CMS" || suffix==".cms") {
    group = pv1_->view_cms ();
  }
  // *.sg file opening
  else if (suffix==".SG" || suffix==".sg") {
    group = pv1_->view_sg (file_opened_cache_);
  }
  else {
    vgui_dialog error( "Error" );
    error.message( ("*" + suffix + " not supported.").c_str() );
    error.ask();
    return;
  }

  open_in_view--;
  if (open_in_view != -1) {
    if (b_clear_view)
      view_manager_->clear_view (open_in_view);
    view_manager_->add_to_view (open_in_view, group);
    view_manager_->view_all();
  }*/
}

void dbsk3dapp_manager::file_save()           
{
  vgui_dialog params("File->Save...");  
  vcl_string ext;

  if (file_saved_cache_ == "") {
    ext = dbul_get_suffix (file_opened_cache_);
    file_saved_cache_ = dbul_get_dir_prefix (file_opened_cache_) + "-save" + ext;
  }

  params.file ("Save...", ext, file_saved_cache_);

  if (!params.ask())
    return;

  //: detect file type...
  ext = dbul_get_suffix (file_saved_cache_);

  // save *.p3d file
  if (ext==".P3D" || ext==".p3d") {
    pv1_->save_p3d (file_saved_cache_);
  }
  // save *.ply2 file
  else if (ext==".PLY2" || ext==".ply2") {
    pv1_->save_ply2 (file_saved_cache_);
  }
  else {
    vgui_dialog error ("Error");
    error.message ("Please specify a correct file type (p3d, ply2, ply) to save." );
    error.ask();
    return;
  }
}

void dbsk3dapp_manager::file_save_view_as_iv()
{
  vgui_dialog params ("Save View as IV");
  vcl_string ext;

  params.file ("IV to save...", ext, file_saved_cache_);

  int open_in_view = 1;
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back ("View 1 (Top Left)");
  view_choices.push_back ("View 2 (Top Right)");
  view_choices.push_back ("View 3 (Bottom Left)");
  view_choices.push_back ("View 4 (Bottom Right)");
  params.choice ("Select one of the four views", view_choices, open_in_view);

  if (!params.ask())
    return;

  if (file_saved_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a file to save." );
    error.ask();
    return;
  }

  //bgui3d_export_iv (view_manager_->master_root(), file_saved_cache_);

  //: \todo: Alex: not working know why??

}

void dbsk3dapp_manager::fileQuit()           
{
  /*vgui_dialog quit_dl("Quit");
  quit_dl.message("Are you sure you want to quit dbsk3dapp - LEMS 3D Shock Viewer ?");
  if (!quit_dl.ask())
    return;*/

  //view_manager_->master_root()->removeAllChildren();
  vgui::quit();
}


// ###############################################################################
//       Edit Menu
// ###############################################################################

void dbsk3dapp_manager::edit_clear_all_views ()           
{
  //: clean up the drawing
  view_manager_->clear_all_views ();
}

void dbsk3dapp_manager::edit()
{
  assert (0);
}

void dbsk3dapp_manager::search_fs_elm ()
{
  int searchOption = -1;
  unsigned idNumber = 0;
  vgui_dialog params("Search");
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back( "Sheet" );
  view_choices.push_back( "Link" );
  view_choices.push_back( "Node" );
  params.choice( "Search for:", view_choices, searchOption );
  params.field( "Id Number:" , idNumber );
  if( !params.ask() )
    return;

  dbsk3d_fs_mesh* fs = dual_bnd_shock_vis()->fs_mesh();
  
  switch( searchOption ) {
    case 0:
      if( idNumber >= 0 && idNumber < fs->facemap().size() ) {
        fs_face_SoFaceSet* shock_A12_SoFaceSet = (fs_face_SoFaceSet*) fs->facemap(idNumber)->vis_pointer();
        if (shock_A12_SoFaceSet) {
          SoGroup* parent = getParent (shock_A12_SoFaceSet,
                                      dbsk3dapp_manager::instance()->view_manager()->master_root());

          action()->select_highlight (shock_A12_SoFaceSet, parent, SELECT_COLOR);
          view_manager()->view_subgraph (parent);
        }
      }
    break;

    case 1:
      if( idNumber >= 0 && idNumber < fs->edgemap().size() ) {
        fs_edge_SoLineSet* shock_SoLineSet = (fs_edge_SoLineSet*) fs->edgemap(idNumber)->vis_pointer();
        if (shock_SoLineSet) {
          SoGroup* parent = getParent (shock_SoLineSet,
                                      dbsk3dapp_manager::instance()->view_manager()->master_root());
  
          action()->select_highlight (shock_SoLineSet, parent, SELECT_COLOR);
          view_manager()->view_subgraph (parent);
        }
      }
    break;

    case 2:
      if( idNumber >= 0 && idNumber < fs->vertexmap().size() ) {
        fs_vertex_SoCube* shock_SoCube = (fs_vertex_SoCube*) ((dbmsh3d_vertex*)fs->vertexmap(idNumber))->vis_pointer();
        if (shock_SoCube) {
          SoGroup* parent = getParent (shock_SoCube,
                                      dbsk3dapp_manager::instance()->view_manager()->master_root());
  
          action()->select_highlight (shock_SoCube, parent, SELECT_COLOR);
          view_manager()->view_subgraph (parent);
        }
      }
    break;

    default:
      vcl_cout << "\nWARNING: can't found id: " << idNumber << vcl_endl;
    break;
  }

}

// ###############################################################################
//       View Menu
// ###############################################################################

void dbsk3dapp_manager::choose_view (vgui_dialog &params, int &pick)
{
  int num = view_manager()->get_grid_size();
  
  vcl_vector<vcl_string> choices;
  choices.push_back( "Do Not Display" );

  for( int i=0; i<num; i++ ) {
    char buff[16];
    sprintf( buff, "View %d", i );
    choices.push_back( buff );
  }

  pick = 1;
  params.choice("Display at:", choices, pick );
}

void dbsk3dapp_manager::view_set_num_views (unsigned int num) 
{ 
  view_manager()->choose_nxn_grid (num);
  view_manager()->add_tableaus_to_grid ();
}

void dbsk3dapp_manager::view_show_hide ()
{
  // find all the switches
  SoSearchAction sa;
  sa.setInterest(SoSearchAction::ALL);
  sa.setType( SoSwitch::getClassTypeId() );

  sa.apply( view_manager_->master_root() );

  SoPathList switchPaths = sa.getPaths();

  vgui_dialog params ("Show/Hide visualization...");  
  vcl_vector<SoSwitch*> nodes_with_soswitches;
  vcl_vector<bool> show;

  for (int i=0; i< switchPaths.getLength(); i++) {
    bool isShown;
    SoSwitch* swit = (SoSwitch*) switchPaths[i]->getTail();

    if( swit->whichChild.getValue() == SO_SWITCH_ALL )
      isShown = true;
    else
      isShown = false;
    nodes_with_soswitches.push_back( swit );
    show.push_back (isShown);
  }

  // puts checkboxes in the dialog window for each element
  for (unsigned int i=0; i<nodes_with_soswitches.size(); i++) {
    vcl_string str = nodes_with_soswitches[i]->getName().getString();
    str = remove_commas (str);
    params.checkbox (str.c_str() , (bool&) show[i]);
  }
  
  // error message if there aren't any SoSwitches
  if (nodes_with_soswitches.size() == 0)
    params.message ("Cannot hide anything.");
  if (!params.ask())
    return;
    
  // hide or unhide the child by changing whichChild 
  for (unsigned int i=0; i<nodes_with_soswitches.size(); i++) {
    if (show[i])
      ((SoSwitch*)nodes_with_soswitches[i])->whichChild = SO_SWITCH_ALL;
    else 
      ((SoSwitch*)nodes_with_soswitches[i])->whichChild = SO_SWITCH_NONE;
  }
}

void dbsk3dapp_manager::view_object_boundary_options()
{
  float pointSize = pv1_->pt_size_; 
  float transparency = pv1_->m_transp_;

  vgui_dialog params ("Object Boundary Mesh View Options");
  params.field( "Generatpr Point Size", pointSize ); 
  params.field ("Surface Mesh Transparency (Def: 0)", transparency);
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back ("As Is (Filled)");
  view_choices.push_back ("Lines");
  view_choices.push_back ("Points");
  ///params.choice ("Drawing Style", view_choices, style);

  if (!params.ask())
    return;

  if (pointSize != pv1_->pt_size_) {
    pv1_->pt_size_ = pointSize;
    SoSearchAction sa;
    sa.setInterest(SoSearchAction::ALL);
    sa.setType( SoDrawStyle::getClassTypeId() );
    sa.apply( view_manager_->master_root() );

    SoPathList switchPaths = sa.getPaths();

    for( int i=0; i< switchPaths.getLength(); i++ ) 
    {
      SoDrawStyle* ds = (SoDrawStyle*)switchPaths[i]->getTail();
      ds->pointSize = pointSize;
    }
  }

  // transparency changed
  if (transparency != pv1_->m_transp_) {
    pv1_->m_transp_ = transparency;
    SoSearchAction sa;
    sa.setInterest (SoSearchAction::ALL);
    sa.setName ("boundary_mesh_material");
    sa.apply (view_manager_->master_root());
    SoPathList list = sa.getPaths();
      for (int i=0; i<list.getLength(); i++) {
        SoMaterial* material = (SoMaterial*)list[i]->getTail();
        material->transparency = transparency;
      }
    sa.reset();
  }
  
}

void dbsk3dapp_manager::view_shock_element_options()
{
  // get the values set
  float nodeSize = pv1_->cube_size_;
  float lineWidth = pv1_->m_edge_width_;
  float patchTransparency = pv1_->ssheet_transp_;
 
  vgui_dialog params("Shock Element View Options");
  params.field( "Shock Node Element Cube Size", nodeSize );
  params.field( "Shock Line Element Line Width", lineWidth );
  params.field( "Shock Patch Element Transparency (def 0)", patchTransparency );

  if (!params.ask())
    return;

  // if NodeSize is changed
  if (nodeSize != pv1_->cube_size_) {
    pv1_->cube_size_ = nodeSize;

    SoSearchAction sa;
    sa.setInterest( SoSearchAction::ALL );
    sa.setName( "shock,node,element" );
    sa.apply( view_manager_->master_root() );

    SoPathList list = sa.getPaths();
      for( int i=0; i<list.getLength(); i++ ) {
        fs_vertex_SoCube* cube = (fs_vertex_SoCube*)list[i]->getTail();
        cube->height = nodeSize;
        cube->width = nodeSize;
        cube->depth = nodeSize;
      }
  }
    
  // lineWidth change
  if (lineWidth != pv1_->m_edge_width_) {
    pv1_->m_edge_width_ = lineWidth;

    SoSearchAction sa;
    sa.setInterest( SoSearchAction::ALL );
    sa.setName( "fs_edge_style" );
    sa.apply( view_manager_->master_root() );
    SoPathList list = sa.getPaths();
    ///if( list!=NULL ) {
      for( int i=0; i<list.getLength(); i++ ) {
        SoDrawStyle* ds = (SoDrawStyle*)list[i]->getTail();
        ds->lineWidth.setValue (lineWidth);
      }
    ///}
  }

  // patchTransparency change
  if (patchTransparency != pv1_->ssheet_transp_ ) {
    pv1_->ssheet_transp_ = patchTransparency;

    SoSearchAction sa;
    sa.setInterest( SoSearchAction::ALL );
    sa.setName( "FF_material" );
    sa.apply( view_manager_->master_root() );
    SoPathList list = sa.getPaths();
    ///if( list!=NULL ) {
      for( int i=0; i<list.getLength(); i++ ) {
        SoMaterial* material = (SoMaterial*)list[i]->getTail();
        material->transparency = patchTransparency;
      }
    ///}
  }
}

void dbsk3dapp_manager::view_scaffold_hypergraph_options()
{
  float sheetTransparency = pv1_->ms_vis_param_.sheet_transp_;

  SoSearchAction sa;
  // search for the root of the shock
  sa.setInterest(SoSearchAction::FIRST);
  sa.setName ("ms,hypg");
  sa.apply( view_manager_->master_root() );
  
  // if that node exists
  if ( !sa.getPath() )
    return;

  SoSwitch* shockRoot = (SoSwitch*)sa.getPath()->getTail();
  // find all the SoDrawStyles
  SoSearchAction sa2;
  sa2.setInterest(SoSearchAction::ALL);
  sa2.setType( SoDrawStyle::getClassTypeId() );
  sa2.apply( shockRoot );
    
  SoPathList dsPaths = sa2.getPaths(); 

  SoDrawStyle* ds = (SoDrawStyle*)dsPaths[0]->getTail();
    
  // get the values of its members ( all in the list should be the same? )
  float lineWidth = ds->lineWidth.getValue();
#if 0
//this variable is not used in the code.  PLEASE FIX!  -MM
  int style = ds->style.getValue();
#endif

  // find all the SoSpheres   
  sa2.setType( SoSphere::getClassTypeId() );
  sa2.apply( (SoSwitch*)sa.getPath()->getTail() );
  SoPathList spherePaths = sa2.getPaths();
  SoSphere* sphere = (SoSphere*)spherePaths[0]->getTail();
  float radius = sphere->radius.getValue();

  vgui_dialog params("Shock Scaffold Hypergraph View Options");

    params.field( "Shock Scaffold Vertex Radius (Def: 0.15)", radius );
    params.field( "Shock Scaffold Link Width (Def: 3)", lineWidth );
    params.field( "Shock Scaffold Sheet Transparency (def 0)", sheetTransparency );

    if( !params.ask() )
      return;

    // change all the cubes if the size isn't the same
    if( radius != sphere->radius.getValue() ) {
      for( int i =0; i< spherePaths.getLength(); i++ ) {
        ((SoSphere*)spherePaths[i]->getTail())->radius = radius;       
      }
    }

    // change the value etc if not the same
    if( lineWidth != ds->lineWidth.getValue()) {  
      for( int i =0; i< dsPaths.getLength(); i++ ) {
        SoDrawStyle* drawStyle = (SoDrawStyle*)dsPaths[i]->getTail();
        drawStyle->lineWidth.setValue (lineWidth);
      }
    }

    // sheetTransparency change
    if (sheetTransparency != pv1_->ms_vis_param_.sheet_transp_) {
      pv1_->ms_vis_param_.sheet_transp_ = sheetTransparency;

      //SoSearchAction sa;
      sa.setInterest( SoSearchAction::ALL );
      sa.setName( "ms_sheet_material" );
      sa.apply( view_manager_->master_root() );
      SoPathList list = sa.getPaths();
      ///if( list!=NULL ) {
        for( int i=0; i<list.getLength(); i++ ) {
          SoMaterial* material = (SoMaterial*)list[i]->getTail();
          material->transparency = sheetTransparency;
        }
      ///}
      sa.reset();
    }
}

void dbsk3dapp_manager::view_background_color()
{
  vcl_string t;
  vgui_dialog params("Choose Background Color");
  params.color("Background Color", t);
  if(!params.ask())
    return;
}

#define CLOSEST_COLOR_1 SbColor (0.05f, 0.05f, 0.05f)
#define CLOSEST_COLOR_2 SbColor (0.075f, 0.075f, 0.075f)

void dbsk3dapp_manager::search_A13_A13()
{
  /*//: the closest A13_A13
  dbsk3d_ms_curve* A13_curve_1;
  dbsk3d_ms_curve* A13_curve_2;
  dbsk3d_fs_vertex* FV1;
  dbsk3d_fs_vertex* FV2;

  dual_bnd_shock_vis()->ms_hypg()->search_A13_A13(
    &A13_curve_1, &A13_curve_2, &FV1, &FV2);

  //: if found, draw them in different color!
  if (A13_curve_1) {
    fs_vertex_SoCube* shape1 = (fs_vertex_SoCube*) FV1->vis_pointer();
    SoGroup* parent = getParent (shape1, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape1, parent, CLOSEST_COLOR_1);

    view_manager()->view_subgraph (parent);

    dbmsh3dvis_gui_elm* shape2 = (fs_vertex_SoCube*)FV2->vis_pointer();
    parent = getParent (shape1, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape2, parent, CLOSEST_COLOR_2);

  }*/
}

void dbsk3dapp_manager::search_A13_A3()
{
  /*//: the closest A13_A13
  dbsk3d_ms_curve* A13_curve;
  dbsk3d_ms_curve* A3_curve;
  dbsk3d_fs_vertex* A13_node_elm;
  dbsk3d_fs_vertex* A3_node_elm;

  dual_bnd_shock_vis()->ms_hypg()->search_A13_A3(
    &A13_curve, &A3_curve, &A13_node_elm, &A3_node_elm);

  //: if found, draw them in different color!
  if (A13_curve) {
    fs_vertex_SoCube* shape1 = (fs_vertex_SoCube*)A13_node_elm->vis_pointer();
    SoGroup* parent = getParent (shape1, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape1, parent, CLOSEST_COLOR_1);

    view_manager()->view_subgraph (parent);

    fs_vertex_SoCube* shape2 = (fs_vertex_SoCube*)A3_node_elm->vis_pointer();
    parent = getParent (shape2, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape2, parent, CLOSEST_COLOR_2);
  }*/
}

void dbsk3dapp_manager::search_A1A3_A13()
{
  /*//: the closest A13_A13
  dbsk3d_ms_node* A1A3_vertex;
  dbsk3d_ms_curve* A13_curve;
  dbsk3d_fs_vertex* A13_node_elm;

  dual_bnd_shock_vis()->ms_hypg()->search_A1A3_A13(
    &A1A3_vertex, &A13_curve, &A13_node_elm);

  //: if found, draw them in different color!
  if (A1A3_vertex) {
    fs_vertex_SoCube* shape1 = (fs_vertex_SoCube*)A1A3_vertex->fs_vertex()->vis_pointer();
    SoGroup* parent = getParent (shape1, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape1, parent, CLOSEST_COLOR_1);

    view_manager()->view_subgraph (parent);

    fs_vertex_SoCube* shape2 = (fs_vertex_SoCube*)A13_node_elm->vis_pointer();
    parent = getParent (shape2, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape2, parent, CLOSEST_COLOR_2);    
  }*/
}

void dbsk3dapp_manager::search_A1A3_A1A3()
{
  /*//: the closest A13_A13
  dbsk3d_ms_node* A1A3_vertex_1;
  dbsk3d_ms_node* A1A3_vertex_2;

  dual_bnd_shock_vis()->ms_hypg()->search_A1A3_A1A3 (&A1A3_vertex_1, &A1A3_vertex_2);

  //: if found, draw them in different color!
  if (A1A3_vertex_1) {
    fs_vertex_SoCube* shape1 = (fs_vertex_SoCube*)A1A3_vertex_1->fs_vertex()->vis_pointer();
    SoGroup* parent = getParent (shape1, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape1, parent, CLOSEST_COLOR_1);

    view_manager()->view_subgraph (parent);

    fs_vertex_SoCube* shape2 = (fs_vertex_SoCube*)A1A3_vertex_2->fs_vertex()->vis_pointer();
    parent = getParent (shape2, dbsk3dapp_manager::instance()->view_manager()->master_root());
    action()->select_highlight (shape2, parent, CLOSEST_COLOR_2);
  }*/
}

// ###############################################################################
//       Process Menu
// ###############################################################################

void dbsk3dapp_manager::add_bndsphere()
{
  if (!pv1_->has_bnd_mesh()) {
    vgui_dialog params("Error");
    params.message("No generator to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Add Bounding Sphere");

  bool b_output_file = false;
  float radius_ratio, nsample_ratio;
  params.checkbox ("Save output file (prefix-sphere.p3d)", b_output_file);
  params.field ("bounding sphere radius ratio (Def.=5)", radius_ratio);
  params.field ("bounding shpere num sample ratio (Def.=1)", nsample_ratio);

  int open_in_view = 1;
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back ("View 0 (Top Left)");
  view_choices.push_back ("View 1 (Top Right)");
  view_choices.push_back ("View 2 (Bottom Left)");
  view_choices.push_back ("View 3 (Bottom Right)");
  params.choice ("To one of the four views", view_choices, open_in_view);

  if (!params.ask())
    return;
 
  pv1_->add_bndsphere (radius_ratio, nsample_ratio);
  SoSeparator* group = pv1_->vis_ptset();

  if (b_output_file)
    pv1_->save_p3d_with_sphere ();

  view_manager_->clear_view (open_in_view);
  view_manager_->add_to_view (open_in_view, group);
  view_manager_->view_all();
}

void dbsk3dapp_manager::compute_voronoi_via_qhull()
{
  //show error message
  vgui_dialog params ("Compute Voronoi via QHull");
  params.message ("Not Implemented! Use the qvoronoi command-line to do it.");
  params.ask();
  return;
}

void dbsk3dapp_manager::surface_meshing()
{
  if (!pv1_->has_fs_mesh()) {
    vgui_dialog params("Error");
    params.message("No fullshock (*.fs) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Surface Meshing & Shock Pruning");
  vcl_string ext;

  float max_sample_ratio = 5.0f;
  params.field ("Maximum sampling ratio (Def=5)", max_sample_ratio);

  bool b_show_reconstructed_surface = true;
  params.checkbox ("Show Reconstructed Surface", b_show_reconstructed_surface);
  bool b_show_color_code_surface = true;
  params.checkbox ("Show Surface in Reconstruction Color Code", b_show_color_code_surface);
  bool b_show_surface_animation = true;
  params.checkbox ("Show Surface Reconstruction Animation", b_show_surface_animation);
  float timer_interval = 0.2f;
  params.field ("Surface Animation Interval (Def: 0.2)", timer_interval);

  if (!params.ask())
    return;

  pv1_->fs_segre()->run_surface_segre (max_sample_ratio, 0, 1.0f, false);

  // Show the point cloud in the TopLeft view
  view_manager_->clear_view (0);
  SoSeparator* vis = pv1_->vis_ptset ();
  view_manager_->add_to_view (0, vis);

  // Show the shiny surface in the TopRight view
  if (b_show_reconstructed_surface) {
    view_manager_->clear_view (1);
    vis = pv1_->vis_mesh_options (0);
    view_manager_->add_to_view (1, vis);
  }

  // Show the color-code surface in the BottomLeft view
  if (b_show_color_code_surface) {
    view_manager_->clear_view (2);
    vis = pv1_->vis_mesh_options (2);
    view_manager_->add_to_view (2, vis);
  }

  // Show the propagation animation of surface reconstruction in the BottomLeft view
  if (b_show_surface_animation) {
    view_manager_->clear_view (3);
    vis = pv1_->vis_mesh_options (3);
    view_manager_->add_to_view (3, vis);
  }

  view_manager_->view_all();
}


void dbsk3dapp_manager::rmin_shock_prune()
{
  if (!pv1_->has_fs_mesh()) {
    vgui_dialog params("Error");
    params.message("No fullshock (*.fs) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Surface Smoothing / Shock Pruning");

  ///params.field ("Close to Surface Pruning Ratio (Def.=0.1)", 
  ///              pv1_->proc_param_.prune_box_ratio_);

  int i_original_view = 0;
  int open_in_view = 1;
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back ("View 0 (Top Left)");
  view_choices.push_back ("View 1 (Top Right)");
  view_choices.push_back ("View 2 (Bottom Left)");
  view_choices.push_back ("View 3 (Bottom Right)");
  params.choice ("Draw original fullshock before pruning in view", view_choices, i_original_view);
  params.choice ("Draw fullshock after pruning in view", view_choices, open_in_view);

  if (!params.ask())
    return;

  // Show the original in the TopLeft view
  view_manager_->clear_view (i_original_view);
  SoSeparator* group = pv1_->vis_fs_mesh (true, true);
  view_manager_->add_to_view (i_original_view, group);

  ///pv1_->shock_regularization (pv1_->proc_param_.rmin_);

  // Show the pruned fullshock in the TopRight view
  view_manager_->clear_view (open_in_view);
  group = pv1_->vis_fs_mesh (true, false);
  view_manager_->add_to_view (open_in_view, group);
  view_manager_->view_all();
}

void dbsk3dapp_manager::build_ms_hypg()
{
  vgui_dialog params ("Build Shock Scaffold Hypergraph from Components");

  bool b_output_fs = false;
  params.checkbox ("Save updated fullshock file (prefix.fs)", b_output_fs);
  bool b_output_cms = false;
  params.checkbox ("Save output scaffold hypergraph file (prefix.cms)", b_output_cms);

  if (!params.ask())
    return;

  if (pv1_->build_ms_hypg(0)) {

    SoSeparator* scaffold_element_group = pv1_->vis_fs_mesh_color_by_radius ();
    SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
    SoSeparator* scaffold_graph_group = pv1_->vis_ms_graph ();
    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

    ///view_manager_->clear_all_views ();
    view_manager_->clear_view (0);
    view_manager_->add_to_view (0, scaffold_hypergraph_group);
    view_manager_->add_to_view (0, bnd_gene_group);
    view_manager_->clear_view (1);
    view_manager_->add_to_view (1, scaffold_graph_group);
    view_manager_->add_to_view (1, bnd_gene_group);
    view_manager_->clear_view (2);
    view_manager_->add_to_view (2, scaffold_element_group);
    view_manager_->view_all();

    if (b_output_fs)
      pv1_->save_fs ();
    if (b_output_cms)
      pv1_->save_cms ();
  }
  else {
    vgui_dialog params("Fail when building scaffold hypergraph");
    params.message("Make sure you have run Surface Smoothing/Shock Pruning first.");
    params.ask();
    return;
  }
}

void dbsk3dapp_manager::build_sg_sa_from_cms ()
{
  if (!pv1_->has_ms_hypg()) {
    vgui_dialog params("Error");
    params.message("No scaffold hypergraph (*.cms) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Build Scaffold Graph (*.sg) from Hypergraph (*.cms)");
  bool b_output_sg = false;
  params.checkbox ("Save output stand-alone scaffold graph file (prefix.sg)", b_output_sg);

  if (!params.ask())
    return;

  pv1_->build_ms_graph_sa_from_cms ();
  
  if (b_output_sg)
    pv1_->save_sg (pv1_->dir_prefix() + ".sg");

  view_manager_->clear_all_views ();

  SoSeparator* sg_sa_group = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (0, sg_sa_group);
  SoSeparator* bnd_gene_group = pv1_->vis_ptset ();
  view_manager_->add_to_view (0, bnd_gene_group);
  ///setTopLeftText ("Scaffold Graph (stand alone)");

  SoSeparator* shg_group = pv1_->vis_ms_hypg ();
  view_manager_->add_to_view (1, shg_group);
  view_manager_->add_to_view (1, bnd_gene_group);
  ///setTopRightText ("Scaffold Hypergraph");

  SoSeparator* sg_group = pv1_->vis_ms_graph ();
  view_manager_->add_to_view (2, sg_group);
  view_manager_->add_to_view (2, bnd_gene_group);
  ///setBottomLeftText ("Scaffold Graph");
}

void dbsk3dapp_manager::remove_cms_transitions()
{
  if (!pv1_->has_ms_hypg()) {
    vgui_dialog params("Error");
    params.message("No scaffold hypergraph (*.cms) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Remove Scaffold Hypergraph Transitions");

  ///params.field ("Shock Tab Threshold (Def.=5)", 
  ///              pv1_->proc_param_.trans_tab_th_);
  bool b_output_ht_cms = false;
  params.checkbox ("Save output scaffold hypergraph file (prefix-ht.cms)", b_output_ht_cms);

  if (!params.ask())
    return;
  
  //: Clean up the visualizations.
  view_manager_->clear_all_views ();

  SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

  //: Display the Scaffold Hypergraph and Graph before
  SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
  view_manager_->add_to_view (0, scaffold_hypergraph_group);
  view_manager_->add_to_view (0, bnd_gene_group);

  SoSeparator* scaffold_graph_group = pv1_->vis_ms_graph ();
  view_manager_->add_to_view (1, scaffold_graph_group);
  view_manager_->add_to_view (1, bnd_gene_group);

  ///pv1_->perform_trans_tabs (pv1_->proc_param_.trans_tab_th_);

  //: Display the Scaffold Hypergraph and Graph after transitions
  scaffold_hypergraph_group = pv1_->vis_ms_hypg_ctable ();
  view_manager_->add_to_view (2, scaffold_hypergraph_group);
  view_manager_->add_to_view (2, bnd_gene_group);


  scaffold_graph_group = pv1_->vis_ms_graph ();
  view_manager_->add_to_view (3, scaffold_graph_group);
  view_manager_->add_to_view (3, bnd_gene_group);

  if (b_output_ht_cms)
    pv1_->save_cms (pv1_->dir_prefix() + "-ht.cms");
}

void dbsk3dapp_manager::build_sg_sa_from_cms_after_trans ()
{
  if (!pv1_->has_ms_hypg()) {
    vgui_dialog params("Error");
    params.message("No scaffold hypergraph (*.cms) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Build Scaffold Graph (*-ht.sg) from Hypergraph (*-ht.cms)");
  bool b_output_sg = false;
  params.checkbox ("Save output stand-alone scaffold graph file (prefix-ht.sg)", b_output_sg);

  if (!params.ask())
    return;

  pv1_->build_ms_graph_sa_from_cms ();
  
  if (b_output_sg)
    pv1_->save_sg (pv1_->dir_prefix() + "-ht.sg");

  view_manager_->clear_all_views ();

  SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

  SoSeparator* sg_sa_group = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (0, sg_sa_group);
  view_manager_->add_to_view (0, bnd_gene_group);
  ///setTopLeftText ("Scaffold Graph (stand alone)");

  SoSeparator* shg_group = pv1_->vis_ms_hypg ();
  view_manager_->add_to_view (1, shg_group);
  view_manager_->add_to_view (1, bnd_gene_group);
  ///setTopRightText ("Scaffold Hypergraph");

  SoSeparator* sg_group = pv1_->vis_ms_graph ();
  view_manager_->add_to_view (2, sg_group);
  view_manager_->add_to_view (2, bnd_gene_group);
  ///setBottomLeftText ("Scaffold Graph");
}

void dbsk3dapp_manager::remove_sg_transitions()
{
  if (!pv1_->has_sg_sa()) {
    vgui_dialog params("Error");
    params.message("No stand alone scaffold graph (*.sg) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Remove Scaffold Graph Transitions");
  params.field ("A5 Threshold (Def.=2.0f)", pv1_->proc_param_.trans_A5_th_);
  bool b_output_sg = false;
  params.checkbox ("Save output stand-alone scaffold graph file (prefix-ht-gt.sg)", b_output_sg);

  if (!params.ask())
    return;

  //: Clean up the visualizations.
  view_manager_->clear_all_views ();

  //: Display the Scaffold Graph before
  SoSeparator* vis = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (0, vis);

  pv1_->graph_trans_A5 ();

  ///pv1_->graph_trans_A15 ();

  //: Display the Scaffold Graph after transitions
  vis = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (1, vis);

  if (b_output_sg)
    pv1_->save_sg (pv1_->dir_prefix() + "-ht-gt.sg");
}

void dbsk3dapp_manager::smooth_scaffold_graph ()
{
  if (!pv1_->has_sg_sa()) {
    vgui_dialog params("Error");
    params.message("No stand alone scaffold graph (*.sg) to begin with.");
    params.ask();
    return;
  }

  vgui_dialog params ("Smooth Scaffold Graph");
  params.field ("Number of smoothing operations (Def.=5)", 
                pv1_->proc_param_.n_smooth_);
  bool b_output_sg = false;
  params.checkbox ("Save output stand-alone scaffold graph file (prefix-ht-gt-sm?.sg)", b_output_sg);

  if (!params.ask())
    return;
  
  //: Clean up the visualizations.
  view_manager_->clear_all_views ();

  //: Display the Scaffold Graph before
  SoSeparator* vis = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (0, vis);

  pv1_->ms_graph_sa_smooth ();

  //: Display the Scaffold Graph after transitions
  vis = pv1_->vis_ms_graph_sa ();
  view_manager_->add_to_view (1, vis);

  if (b_output_sg) {
    char namebuf[128];
    sprintf (namebuf, "-ht-gt-sm%d.sg", pv1_->proc_param_.n_smooth_);
    pv1_->save_sg (pv1_->dir_prefix() + namebuf);
  }
}


void dbsk3dapp_manager::show_flow()
{
  if (!pv1_->has_fs_mesh()) {
    vgui_dialog params("Error");
    params.message("No fullshock (*.fs) to begin with.");
    params.ask();
    return;
  }

  SoSeparator* sep = pv1_->display_sheet_flow();
  
  view_manager_->add_to_view( 1, sep );

}

void dbsk3dapp_manager::reconstruct()
{
  if (!pv1_->has_fs_mesh()) {
    vgui_dialog params("Error");
    params.message("No fullshock (*.fs) to begin with.");
    params.ask();
    return;
  }

  SoSeparator* sep = pv1_->reconstruct();
  
  view_manager_->add_to_view( 0, sep );

}


// ###############################################################################
//       Task Manu
// ###############################################################################

void dbsk3dapp_manager::shock_recovery ()
{
  vgui_dialog params ("Shock Recovery");

  pv1_->reset_mesh ();
  ///pv1_->reset_point_gene_set_with_shpere ();
  pv1_->reset_fs_mesh ();

  vcl_string ext;
  params.message("3 files required: prefix.p3d, prefix-sphere.p3d, prefix-sphere.vor .");
  params.file ("File prefix (Select the original .p3d file)", ext, file_opened_cache_);

  int open_in_view = 0;
  vcl_vector<vcl_string> view_choices;
  view_choices.push_back ("View 0 (Top Left)");
  view_choices.push_back ("View 1 (Top Right)");
  view_choices.push_back ("View 2 (Bottom Left)");
  view_choices.push_back ("View 3 (Bottom Right)");
  params.choice ("Output to view", view_choices, open_in_view);

  if (!params.ask())
    return;

  //: reset the datastructure
  clear_all_reset();  

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  vcl_string dir_prefix = dbul_get_dir_prefix (file_opened_cache_);
  if (file_opened_cache_ == "")
    return;

  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));

  bool success = pv1_->fs_recover_qhull ();

  if (success) {
    SoSeparator* group = pv1_->vis_ptset ();
    
    view_manager_->clear_view (open_in_view);
    view_manager_->add_to_view (open_in_view, group);
    view_manager_->view_all();
  }
  else {
    vgui_dialog params ("Shock Recovery Error");
    params.ask();
    return;
  }
}

void dbsk3dapp_manager::compute_fullshock()
{
  vgui_dialog params ("Shock Recovery from QHull/Voronoi with bounding sphere"); 
  vcl_string ext;
  params.file ("Original Point Cloud file (.p3d) to get 'prefix'", ext, file_opened_cache_);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  //: reset the datastructure
  clear_all_reset();  

  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));

  bool success = pv1_->fs_recover_qhull ();
  if (success) {
    SoSeparator* group = pv1_->vis_fs_mesh (true, true);
    view_manager_->add_to_view (1, group);
    view_manager_->view_all();
  }
}

void dbsk3dapp_manager::scaffold_hypergraph_from_fs()
{
  vgui_dialog params ("FullShock to Shock Scaffold Hypergraph");
  vcl_string ext;
  params.file ("Full Shock File (.fs)", ext, file_opened_cache_);

  ///params.field ("Close to Surface Pruning Ratio (Def.=0.1)", 
  ///              pv1_->proc_param_.prune_box_ratio_);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  //: reset the datastructure
  clear_all_reset();  

  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));

  pv1_->load_fs ();

  float max_sample_ratio = 5.0f;
  pv1_->fs_segre()->run_surface_segre (max_sample_ratio, 0, 1.0f, false);
  SoSeparator* vis = pv1_->vis_mesh_options (0);

  view_manager_->clear_view (0);
  view_manager_->add_to_view (0, vis);
  view_manager_->view_all();

  ///pv1_->shock_regularization (pv1_->proc_param_.rmin_);

  if (pv1_->build_ms_hypg(0)) {

    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

    SoSeparator* scaffold_element_group = pv1_->vis_fs_mesh_color_by_radius ();
    SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
    SoSeparator* scaffold_graph_group = pv1_->vis_ms_graph ();

    view_manager_->clear_view (1);
    view_manager_->add_to_view (1, scaffold_hypergraph_group);
    view_manager_->add_to_view (1, bnd_gene_group);
    view_manager_->clear_view (2);
    view_manager_->add_to_view (2, scaffold_graph_group);
    view_manager_->add_to_view (2, bnd_gene_group);
    view_manager_->clear_view (3);
    view_manager_->add_to_view (3, scaffold_element_group);
    view_manager_->add_to_view (3, bnd_gene_group);
    view_manager_->view_all();

    pv1_->build_ms_graph_sa_from_cms ();
  }
}

void dbsk3dapp_manager::compute_scaffold_hypergraph ()
{
  vgui_dialog params ("Compute Shock Scaffold Hypergraph (all in one)");
  vcl_string ext;
  params.file ("Original Point Cloud file (.p3d) to get 'prefix'", ext, file_opened_cache_);

  ///params.field ("Close to Surface Pruning Ratio (Def.=0.1)", 
  ///              pv1_->proc_param_.prune_box_ratio_);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  //: reset the datastructure
  clear_all_reset();  

  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));

  pv1_->fs_recover_qhull ();

  float max_sample_ratio = 5.0f;
  pv1_->fs_segre()->run_surface_segre (max_sample_ratio, 0, 1.0f, false);
  SoSeparator* vis = pv1_->vis_mesh_options (0);

  view_manager_->clear_view (0);
  view_manager_->add_to_view (0, vis);
  view_manager_->view_all();

  ///pv1_->shock_regularization (pv1_->proc_param_.rmin_);

  //: If the scafoold hypergraph is successfully extracted, draw views.
  if (pv1_->build_ms_hypg(0)) {
    SoSeparator* scaffold_element_group = pv1_->vis_fs_mesh_color_by_radius ();
    SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
    SoSeparator* scaffold_graph_group = pv1_->vis_ms_graph ();
    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

    view_manager_->clear_view (1);
    view_manager_->add_to_view (1, scaffold_hypergraph_group);
    view_manager_->add_to_view (1, bnd_gene_group);
    view_manager_->clear_view (2);
    view_manager_->add_to_view (2, scaffold_graph_group);
    view_manager_->add_to_view (2, bnd_gene_group);
    view_manager_->clear_view (3);
    view_manager_->add_to_view (3, scaffold_element_group);
    view_manager_->add_to_view (3, bnd_gene_group);
    view_manager_->view_all();

    pv1_->build_ms_graph_sa_from_cms ();

  }
}

void dbsk3dapp_manager::scaffold_hypergraph_plus_transitions()
{
  vgui_dialog params ("Compute Shock Scaffold Hypergraph (all in one)");
  vcl_string ext;
  params.file ("Original Point Cloud file (.p3d) to get 'prefix'", ext, file_opened_cache_);

  ///params.field ("Close to Surface Pruning Ratio (Def.=0.1)", 
  ///              pv1_->proc_param_.prune_box_ratio_);

  //Transitions
  ///params.field ("Shock Tab Threshold (Def.=5)", 
                ///pv1_->proc_param_.trans_tab_th_);
  params.field ("A5 Threshold (Def.=2.0)", 
                pv1_->proc_param_.trans_A5_th_);

  params.field ("Number of smoothing operations (Def.=5)", 
                pv1_->proc_param_.n_smooth_);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a input file (prefix)." );
    error.ask();
    return;
  }

  //: reset the datastructure
  clear_all_reset();  

  pv1_->set_dir_file (dbul_get_dir_file (file_opened_cache_));

  pv1_->fs_recover_qhull ();
  float max_sample_ratio = 5.0f;
  pv1_->fs_segre()->run_surface_segre (max_sample_ratio, 0, 1.0f, false);
  SoSeparator* vis = pv1_->vis_mesh_options (0);

  view_manager_->clear_view (0);
  view_manager_->add_to_view (0, vis);
  view_manager_->view_all();

  ///pv1_->shock_regularization (pv1_->proc_param_.rmin_);

  //: If the scafoold hypergraph is successfully extracted, draw views.
  if (pv1_->build_ms_hypg(0)) {

    SoSeparator* scaffold_element_group = pv1_->vis_fs_mesh_color_by_radius ();
    view_manager_->clear_view (1);
    view_manager_->add_to_view (1, scaffold_element_group);
    
    ///pv1_->perform_trans_tabs (pv1_->proc_param_.trans_tab_th_);

    pv1_->build_ms_graph_sa_from_cms ();    

    pv1_->graph_trans_A5 ();

    ///pv1_->graph_trans_A15 ();

    pv1_->ms_graph_sa_smooth ();

    SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
    SoSeparator* ms_graph_sa_group = pv1_->vis_ms_graph_sa ();
    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();

    view_manager_->clear_view (2);
    view_manager_->add_to_view (2, scaffold_hypergraph_group);
    view_manager_->add_to_view (2, bnd_gene_group);
    view_manager_->clear_view (3);
    view_manager_->add_to_view (3, ms_graph_sa_group);
    view_manager_->add_to_view (3, bnd_gene_group);
    view_manager_->view_all();
  }
}

void dbsk3dapp_manager::run_file_scaffold_hypergraph_plus_transitions()
{
  vgui_dialog params ("Run File: Compute Shock Scaffold Hypergraph (all in one)");
  vcl_string ext;
  params.file ("Specify Run File (run_xxx.bat)", ext, file_opened_cache_);
  
  bool b_show_bnd_surface = true;
  params.checkbox ("Show Reconstructed Boundary Mesh", b_show_bnd_surface);
  bool b_show_cms = true;
  params.checkbox ("Show Shock Scaffold Hypergraph", b_show_cms);
  bool b_show_fs_elm = false;
  params.checkbox ("Show Shock Element Hypergraph", b_show_fs_elm);
  bool b_show_sg = false;
  params.checkbox ("Show Shock Scaffold Graph", b_show_sg);

  if (!params.ask())
    return;

  if (file_opened_cache_ == "") {
    vgui_dialog error ("Error");
    error.message ("Please specify a run file (run_xxx.bat)." );
    error.ask();
    return;
  }

  //: reset the datastructure
  clear_all_reset();  

  ///pv1_->oper_param_.dir_ = dbul_get_dir (file_opened_cache_);
  pv1_->parse_run_file (file_opened_cache_.c_str());

  pv1_->fs_recover_qhull ();
  float max_sample_ratio = 5.0f;
  pv1_->fs_segre()->run_surface_segre (max_sample_ratio, 0, 1.0f, false);
  
  if (b_show_bnd_surface) {
    SoSeparator* vis = pv1_->vis_mesh_options (0);
    view_manager_->clear_view (0);
    view_manager_->add_to_view (0, vis);
  }

  ///pv1_->shock_regularization (pv1_->proc_param_.rmin_);

  //: If the scafoold hypergraph is successfully extracted, draw views.
  if (!pv1_->build_ms_hypg(0))
    return;

  if (b_show_fs_elm) {
    SoSeparator* scaffold_element_group = pv1_->vis_fs_mesh_color_by_radius ();
    view_manager_->clear_view (2);
    view_manager_->add_to_view (2, scaffold_element_group);
  }
    
  ///pv1_->perform_trans_tabs (pv1_->proc_param_.trans_tab_th_);

  if (b_show_cms) {
    view_manager_->clear_view (1);
    SoSeparator* scaffold_hypergraph_group = pv1_->vis_ms_hypg ();
    view_manager_->add_to_view (1, scaffold_hypergraph_group);
    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();
    view_manager_->add_to_view (1, bnd_gene_group);
  }

  pv1_->build_ms_graph_sa_from_cms ();    

  pv1_->graph_trans_A5 ();
  ///pv1_->graph_trans_A15 ();

  pv1_->ms_graph_sa_smooth ();

  if (b_show_sg) {
    SoSeparator* ms_graph_sa_group = pv1_->vis_ms_graph_sa ();

    view_manager_->clear_view (3);
    view_manager_->add_to_view (3, ms_graph_sa_group);
    SoSeparator* bnd_gene_group = pv1_->vis_ptset ();
    view_manager_->add_to_view (3, bnd_gene_group);
  }

  //: change to 4 views
  if (b_show_fs_elm || b_show_sg) {
    view_manager()->resize_grid (2, 2);
    view_manager()->add_tableaus_to_grid (); 
  }

  view_manager_->view_all();
}

// ###############################################################################
//       Match Manu
// ###############################################################################

void dbsk3dapp_manager::match_shock_matching()           
{
  vgui_dialog params("Shock Matching");
  vcl_string ext;
  vcl_string sg_file_1 = file_opened_cache_;
  vcl_string sg_file_2 = file_opened_cache_;
  params.file ("Shock Scaffold Graph File 1 (with more vertices)", ext, sg_file_1);
  params.file ("Shock Scaffold Graph File 2 (with fewer vertices)", ext, sg_file_2);

  bool b_rigid_trans = true;
  params.checkbox ("After matching, find optimal rigid transformation", b_rigid_trans);
  bool b_icp = false;
  params.checkbox ("After optimal rigid transformation, run ICP", b_icp);

  if (!params.ask())
    return;

  vcl_string strDirFile1 = dbul_get_dir_file (sg_file_1);
  ///spvr_->p1()->oper_param_.strDirFile_ = strDirFile1;
  vcl_string strDirFile2 = dbul_get_dir_file (sg_file_2);
  ///spvr_->p2()->oper_param_.strDirFile_ = strDirFile2;

  ///spvr_->shock_match (b_rigid_trans);

  ///if (b_icp)
    ///spvr_->run_pp_icp_regstr (n_iter, dbmsh3dr_icpcv(), );

  //: Display matching results.
  ///spvr_->generate_match_color_tables ();

  //Display matching result of graph 1
  ///SoSeparator* vis1 = spvr_->vis_msg_1 ();
  /*view_manager_->clear_view (0);
  view_manager_->add_to_view (0, vis1);
  
  //Display boundary of graph 1
  ///spvr_->p1()->load_p3d();
  ///if (b_rigid_trans)
  ///  spvr_->shock_match()->transform_point_G_1_to_2 (spvr_->p1()->bnd_set());
  ///SoSeparator* b1_vis = spvr_->vis_bnd_1 ();
  view_manager_->add_to_view (0, b1_vis);

  //Display matching result of graph 2
  SoSeparator* vis2 = spvr_->vis_msg_2 ();
  view_manager_->clear_view (1);
  view_manager_->add_to_view (1, vis2);

  //Display boundary of graph 2
  spvr_->p2()->load_p3d();
  SoSeparator* b2_vis = spvr_->vis_bnd_2 ();
  view_manager_->add_to_view (1, b2_vis);

  //Display one-view matching results.
  SoSeparator* vis = spvr_->vis_msg_match (0);
  view_manager_->clear_view (2);
  view_manager_->add_to_view (2, vis);
  view_manager_->view_all();

  //Display boundary 1 and boundary 2 in the 3rd view
  view_manager_->add_to_view (2, b1_vis);
  view_manager_->add_to_view (2, b2_vis);*/
}

/*************************************************************************
 * Function Name: dbsk3dapp_manager::matchingRegistration
 * Parameters:
 * Effects:
 *************************************************************************/
void dbsk3dapp_manager::match_registration()
{
  /*vgui_dialog params("Registration");
  params.file( "Registration filename", ext, registrationData.string1 );
  if (!params.ask())
    return;
//  view_manager_->registration( registrationData );*/
  vgui_dialog params("Registration");  
  params.message("Not Implemented Yet");
  if (!params.ask())
    return;
}

void dbsk3dapp_manager::match_icp()           
{
  /*vgui_dialog params("Iterative Closest Point (ICP)");
  params.file( "Iterative Closest Point G3D file 1 (*.g3d)", ext, icpData.string1 );
  params.file( "                            file 2 (*.g3d)", ext, icpData.string2 );
  if (!params.ask())
    return;
  //view_manager_->icp( icpData );*/
  vgui_dialog params("ICP");  
  params.message("Not Implemented Yet");
  if (!params.ask())
    return;
}

void dbsk3dapp_manager::matching_groundtruth()
{
  vgui_dialog params("Groundtruth");  
  params.message("Not Implemented Yet");
  if (!params.ask())
    return;
}

// ###############################################################################
//       Option Menu
// ###############################################################################

void dbsk3dapp_manager::option_do_validations()
{
  vgui_dialog params ("Option: Do Validations");
  bool do_valid = false;
  params.checkbox ("Do Validations", do_valid);

  if (params.ask() == true) {
    dbmsh3d_cmd_valid.value_ = (int) do_valid;
  }
}

void dbsk3dapp_manager::draw_object_boundary_options()
{ 
  vgui_dialog params ("Object Boundary Mesh Drawing Options");
  vcl_vector<vcl_string> color_scheme_choices;
  color_scheme_choices.push_back ("Default Color with Lighting");
  color_scheme_choices.push_back ("Color by Propagation Color Code");
  color_scheme_choices.push_back ("Color by Group");
  color_scheme_choices.push_back ("Animated Propagation");
  ///params.choice ("Reconstructed Surface Mesh Drawing Scheme", color_scheme_choices, scheme);
  float timer_interval = 0.2f;
  params.field ("Surface Animation Interval (Def: 0.2)", timer_interval);

  if (!params.ask())
    return;

  SoSearchAction sa;
  sa.setInterest( SoSearchAction::ALL );
  sa.setName ("Reconstructed,Surface,Mesh");
  sa.apply( view_manager_->master_root() );
  SoPathList list = sa.getPaths();
  ///if (list==NULL)
  if (list.getLength() == 0)
    return;  
  
  for (int i=0; i<list.getLength(); i++) {
    SoGroup* parent = (SoGroup*)list[i]->getNodeFromTail(2);
    parent->removeChild (list[i]->getNodeFromTail(1));
    ///parent->addChild (dual_bnd_shock_vis()->vis_mesh_options(scheme));
  }
}

void dbsk3dapp_manager::draw_shock_element_options()
{ 
}

void dbsk3dapp_manager::draw_scaffold_hypergraph_options()
{
}

// ###############################################################################
//       Help Menu
// ###############################################################################

void dbsk3dapp_manager::helpInteraction()
{
  vgui_dialog params("Interaction");
  params.message("Dragging:");
  params.message("  Left: Spin");
  params.message("  Middle: Pan");
  params.message("  CTRL+Middle: Zoom");
  params.message("  CTRL+Left: Scenegraph click");
  params.message("Press s, then left-click to seek");
  params.message("Press v to view the entire screen structure");
  params.message("To select a tableau, left click, to deselect, middle click");

  if (!params.ask())
    return;
}

void dbsk3dapp_manager::help_vxl_object_size()
{
  char s[1024];

  vgui_dialog params("VXL Object Memory Size");
  
  vcl_sprintf (s, "sizeof (vgl_point_3d<double>): %d", (int)sizeof (vgl_point_3d<double>));
  params.message (s);
  vcl_sprintf (s, "sizeof (vcl_vector<int>): %d", (int)sizeof (vcl_vector<int>));
  params.message (s);
  vcl_sprintf (s, "sizeof (vcl_map<int, double>): %d", (int)sizeof (vcl_map<int, double>));
  params.message (s);

  vcl_sprintf (s, "See commandline output for details.");
  params.message (s);

  dbmsh3d_hypg_print_object_size ();  

  if (!params.ask())
    return;
}

void dbsk3dapp_manager::help_object_size()
{
  char s[1024];

  vgui_dialog params("Object Memory Size");
  
  vcl_sprintf (s, "sizeof (dbmsh3d_vertex): %d", (int)sizeof (dbmsh3d_vertex));
  params.message (s);
  
  vcl_sprintf (s, "sizeof (dbsk3d_fs_vertex): %d", (int)sizeof (dbsk3d_fs_vertex));
  params.message (s);
  vcl_sprintf (s, "sizeof (dbsk3d_fs_edge): %d", (int)sizeof (dbsk3d_fs_edge));
  params.message (s);
  vcl_sprintf (s, "sizeof (dbsk3d_fs_face): %d", (int)sizeof (dbsk3d_fs_face));
  params.message (s);
  vcl_sprintf (s, "sizeof (dbsk3d_ms_node): %d", (int)sizeof (dbsk3d_ms_node));
  params.message (s);
  vcl_sprintf (s, "sizeof (dbsk3d_ms_curve): %d", (int)sizeof (dbsk3d_ms_curve));
  params.message (s);
  vcl_sprintf (s, "sizeof (dbsk3d_ms_sheet): %d", (int)sizeof (dbsk3d_ms_sheet));
  params.message (s);

  vcl_sprintf (s, "See commandline output for details.");
  params.message (s);

  pv1_->print_mem_usage();

  if (!params.ask())
    return;
}


void dbsk3dapp_manager::help_test_object_size()
{
  char s[1024];

  class test_class_empty {
  };
  class test_class_void_pointer {
    void* pointer;
  };
  class test_class_bool {
    bool b;
  };
  class test_class_int {
    int i;
  };
  class test_class_float {
    float f;
  };
  class test_class_double {
    double d;
  };
  class test_class_vector {
    vcl_vector<void*> v;
  };
  class test_class_map {
    vcl_map<int,void*> m;
  };

  class test_class_virtual_fun {
   public:
    virtual void f (int i, int j) {
      j=2;
    }
    virtual void f2 (double i, double j) {
    }
    virtual ~test_class_virtual_fun () {
    }
  };

  class test_class_virtual_destruct {
   public:
    virtual ~test_class_virtual_destruct () {
    }
  };

  class test_class {
  };

  vgui_dialog params("Test Object Memory Size");
  
  vcl_sprintf (s, "sizeof (test_class_empty): %d", (int)sizeof (test_class_empty));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_void_pointer): %d", (int)sizeof (test_class_void_pointer));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_bool): %d", (int)sizeof (test_class_bool));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_int): %d", (int)sizeof (test_class_int));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_float): %d", (int)sizeof (test_class_float));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_double): %d", (int)sizeof (test_class_double));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_vector): %d", (int)sizeof (test_class_vector));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_map): %d", (int)sizeof (test_class_map));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_virtual_fun): %d", (int)sizeof (test_class_virtual_fun));
  params.message (s);
  vcl_sprintf (s, "sizeof (test_class_virtual_destruct): %d", (int)sizeof (test_class_virtual_destruct));
  params.message (s);  
  vcl_sprintf (s, "sizeof (test_class): %d", (int)sizeof (test_class));
  params.message (s);

  if (!params.ask())
    return;
}

void dbsk3dapp_manager::helpAbout()
{
  vgui_dialog params("About");
  params.message("dbsk3dapp - LEMS 3D Shock Application");
  params.message("Brown University");
  params.message("Ming-Ching Chang and Alexander Keating Bowman 2005");
  if (!params.ask())
    return;
}



