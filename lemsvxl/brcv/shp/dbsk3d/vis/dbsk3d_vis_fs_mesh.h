//: dbsk3d/vis/dbsk3d_vis_fs_mesh.h
//  Visualization of the fine-scale shock mesh
//  Feb 26, 2004
//  MingChing Chang

#ifndef dbsk3d_vis_fs_mesh_h_
#define dbsk3d_vis_fs_mesh_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class SbColor;
class SoSeparator;

SoSeparator* draw_fs_mesh_valid (dbsk3d_fs_mesh* fs_mesh);

SoSeparator* draw_fs_mesh (dbsk3d_fs_mesh* fs_mesh, const int colorcode);

SoSeparator* draw_fs_mesh (dbsk3d_fs_mesh* fs_mesh, 
                   const int option, const bool draw_valid, 
                   const bool draw_invalid, const bool draw_unbounded, 
                   const float cube_size, const float transp = 0.0f,
                   const bool user_defined_class = true);

SoSeparator* draw_fs_mesh_patches (vcl_map<int, dbmsh3d_face*>& patches, 
                                   const bool draw_valid,
                                   const bool draw_invalid, const bool draw_unbounded,
                                   const bool draw_unvisited = true,
                                   const bool user_defined_class = true);

SoSeparator* draw_fs_mesh_links (vcl_map<int, dbmsh3d_edge*>& links, 
                                 const float th1, const float th2,
                                 const bool draw_invalid, const bool draw_unbounded,
                                 const int draw_option,
                                 const bool draw_unvisited = true,
                                 const bool user_defined_class = true);

SoSeparator* draw_fs_mesh_nodes (vcl_map<int, dbmsh3d_vertex*>& nodes, 
                                 const bool draw_invalid, 
                                 const SbColor color, const float size,
                                 const bool user_defined_class = true);

SoSeparator* draw_fs_patch_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb);

SoSeparator* draw_fs_link_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb);

SoSeparator* draw_fs_node_to_gene (dbsk3d_fs_mesh* fs_mesh, const bool draw_unb);

//: Draw valid shocks of non-1-ring vertices on the object boundary.
//  This function marks related shock patch->i_value_ to 2.
SoSeparator* draw_Ps_non1ring_Gs (dbsk3d_fs_mesh* fs_mesh, const int option,
                                  const bool user_defined_class = true);

SoSeparator* draw_Ls_non1ring_Gs (dbsk3d_fs_mesh* fs_mesh, 
                                  const bool user_defined_class = true);

//############################################################################

SoSeparator* draw_fs_mesh_color_by_radius (dbsk3d_fs_mesh* fs_mesh, const float transp = 0.0f);

SoSeparator* draw_fs_mesh_comp_links (dbsk3d_fs_mesh* fs_mesh, 
                                      const int compid, const float width);

#endif
