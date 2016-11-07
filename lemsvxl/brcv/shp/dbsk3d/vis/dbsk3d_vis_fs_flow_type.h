//: dbsk3d/vis/dbsk3d_vis_fs_flow_type.h
//  Visualization of flow types of each shock element of the medial scaffold.
//  May 31, 2007
//  MingChing Chang

#ifndef dbsk3d_vis_fs_flow_type_h_
#define dbsk3d_vis_fs_flow_type_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>

class SbColor;
class SoSeparator;

//###########  Visualizing surface in-flow  ###########

SoSeparator* vis_fs_surface_inflow_Ls (dbsk3d_fs_mesh* fs_mesh,
                                       const bool user_defined_class = true);

SoSeparator* vis_fs_surface_inflow_Ps (dbsk3d_fs_mesh* fs_mesh,
                                       const float width, const float cube_size,
                                       const bool user_defined_class = true);

//###########  Visualize the inflow-gene-propagation graph ###########
SoSeparator* vis_P_inflow_gene_prop (dbsk3d_fs_face* FF, 
                                     const float width, const float cube_size);

SoSeparator* vis_L_inflow_gene_prop (dbsk3d_fs_edge* FE);

//###########  Visualizing the flow type of fs_faces  ###########

SoSeparator* vis_fs_patches_flow_type (dbsk3d_fs_mesh* fs_mesh);

SoSeparator* vis_fs_links_flow_type (dbsk3d_fs_mesh* fs_mesh);

VIS_COLOR_CODE get_N_flow_type_ccode (const dbsk3d_fs_vertex* FV);
SoSeparator* vis_fs_nodes_flow_type (dbsk3d_fs_mesh* fs_mesh, const float pt_size);
SoSeparator* vis_fs_nodes_flow_type_idv (dbsk3d_fs_mesh* fs_mesh, const float cube_size);

SoSeparator* vis_fs_nodes_iof_type (dbsk3d_fs_mesh* fs_mesh, const float pt_size);
SoSeparator* vis_fs_nodes_iof_type_idv (dbsk3d_fs_mesh* fs_mesh, const float cube_size);

//###########  Visualizing the flow type of fs_edges  ###########

//: Visualize various types of A13 shock curves.
// type == 1: type I, type == 2: type II, type == 3: type III.
SoSeparator* vis_fs_links_type (dbsk3d_fs_mesh* fs_mesh, const int type,
                                const float len);

//: Visualize various types of A13 shock's dual triangles.
// type == 1: type I, type == 2: type II, type == 3: type III.
SoSeparator* vis_fs_links_type_tris (dbsk3d_fs_mesh* fs_mesh, const int type,
                                     const bool view_only_large_tris);

//-v: option. 0: show all, 1: show only type I, 2: show only type II, 
//            3: show only type III, 4: show only type I and II.
SoSeparator* vis_fs_links_color_nG (dbsk3d_fs_mesh* fs_mesh, const float width,
                                    const unsigned int n_genes_th,
                                    const int option,
                                    const bool user_defined_class);

SoSeparator* vis_fs_link_type_2_points (dbsk3d_fs_mesh* fs_mesh, const int option,
                                        const float pt_size, const SbColor& color);

SoSeparator* vis_fs_link_type_3_pts (dbsk3d_fs_mesh* fs_mesh, 
                                     const float pt_size, const SbColor& color);

SoSeparator* vis_fs_link_type_3 (dbsk3d_fs_mesh* fs_mesh, const int option,
                                 const float width, const SbColor& color);

//###########  Visualizing the flow type of fs_vertices  ###########

SoSeparator* vis_fs_nodes_color_nG (dbsk3d_fs_mesh* fs_mesh, const float size,
                                    const unsigned int n_genes_th,
                                    const bool user_defined_class);

#endif
