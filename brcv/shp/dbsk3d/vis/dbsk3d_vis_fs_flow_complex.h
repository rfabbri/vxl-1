//: dbsk3d/vis/dbsk3d_vis_fs_flowcomplex.h
//  Visualization of flow complex using our full medial scaffold.
//  May 31, 2007
//  MingChing Chang

#ifndef dbsk3d_vis_fs_flow_complex_h_
#define dbsk3d_vis_fs_flow_complex_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class SbColor;
class SoSeparator;

//###########  Visualizing the flow complex critical points & shock elements  ###########

SoSeparator* vis_fs_flow_complex (dbsk3d_fs_mesh* fs_mesh, 
                                  const int option, const bool draw_shock_elm,
                                  const float cube_size, const float transp = 0.0f,
                                  const bool user_defined_class = true);

SoSeparator* vis_fc_i1_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_dual,
                                     const float pt_size, const SbColor& color);

SoSeparator* vis_fc_i2_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_link,
                                     const float pt_size, const SbColor& color);

SoSeparator* vis_fc_i3_critical_pts (dbsk3d_fs_mesh* fs_mesh, const bool show_link,
                                     const float pt_size, const SbColor& color);

SoSeparator* vis_fs_gabriel_graph (dbsk3d_fs_mesh* fs_mesh, const float width,
                                   const SbColor& color);

SoSeparator* vis_fs_A12_2_pts (dbsk3d_fs_mesh* fs_mesh, const float size,
                               const SbColor& color);

#endif
