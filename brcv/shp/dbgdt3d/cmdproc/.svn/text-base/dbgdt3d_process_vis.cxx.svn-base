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

#include <dbgdt3d/cmdproc/dbgdt3d_process_vis.h>

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>


//###############################################################
//  Visualization Functions in dbmsh3d_vis_base 
//###############################################################

SoSeparator* dbgdt3d_pro_vis::vis_ptset (const int colorcode, const bool draw_idv)
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

SoSeparator* dbgdt3d_pro_vis::vis_ptset_idv ()
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

SoSeparator* dbgdt3d_pro_vis::vis_ptset_color ()
{
  return draw_pt_set_color (mesh_, mesh_vertex_color_set_, pt_size_, user_defined_class_);
}

SoSeparator* dbgdt3d_pro_vis::vis_mesh (const bool draw_idv, 
                                       const bool b_shape_hints,
                                       const int colorcode,
                                       const bool b_draw_isolated_pts)
{
  vul_printf (vcl_cout, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
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
  vul_printf (vcl_cout, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

SoSeparator* dbgdt3d_pro_vis::vis_mesh_edges (const int colorcode, const float width,
                                             const bool draw_idv)
{
  if (draw_idv == false)
    return draw_M_edges (mesh_, color_from_code (colorcode), width);
  else
    return draw_M_edges_idv (mesh_, color_from_code (colorcode), width);
}

SoSeparator* dbgdt3d_pro_vis::vis_mesh_pts_edges (const int colorcode, const bool draw_idv,
                                                 const float pt_size)
{
  SoSeparator* root = new SoSeparator;  
  int ccode = (colorcode == COLOR_NOT_SPECIFIED) ? m_vertex_ccode_ : colorcode;
  root->addChild (draw_ptset (mesh_, ccode, pt_size));
  root->addChild (vis_mesh_edges (COLOR_GRAY, draw_idv));
  return root;
}

SoSeparator* dbgdt3d_pro_vis::vis_mesh_bndcurve (const int colorcode, 
                                                const float width)
{
  return draw_M_bndcurve (mesh_, colorcode, width);
}

SoSeparator* dbgdt3d_pro_vis::vis_mesh_color ()
{
  return draw_M_color (mesh_, true, m_transp_, mesh_face_color_set_, user_defined_class_);
}

//: Visualize the reconstructed surface mesh according to the input option.
//  Note that this surface is the 
SoSeparator* dbgdt3d_pro_vis::vis_mesh_options (int option, const bool draw_idv, const bool showid)
{
  vul_printf (vcl_cout, "vis_bnd_mesh(): V: %d, E: %d, F: %d\n", 
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
  vul_printf (vcl_cout, "  # unmeshed pts: %d\n", n_unmeshed_pts);

  return root;
}

//: Visualize the reconstructed surface mesh in animation
SoSeparator* dbgdt3d_pro_vis::vis_mesh_anim (const int nF_batch)
{
  vul_printf (vcl_cout, "vis_mesh_anim(): V: %d, E: %d, F: %d\n", 
              mesh_->vertexmap().size(), mesh_->edgemap().size(),
              mesh_->facemap().size());
  SoSeparator* root = new SoSeparator;
  root->addChild (draw_M_bnd_faces_anim (mesh_));
  return root;
}



