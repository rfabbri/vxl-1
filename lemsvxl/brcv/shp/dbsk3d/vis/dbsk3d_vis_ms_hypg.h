//: This is dbsk3d/vis/dbsk3d_vis_ms_hypg.h
//  Visualization of the medial scaffold
//  Nov 30, 2004
//  MingChing Chang

#ifndef dbsk3d_vis_ms_hypg_h_
#define dbsk3d_vis_ms_hypg_h_

#include <dbmsh3d/dbmsh3d_graph.h>
#include <dbmsh3d/algo/dbmsh3d_fmm.h>
#include <dbsk3d/dbsk3d_ms_hypg.h>
#include <dbsk3d/dbsk3d_sg_sa.h>

class SbColor;
class SoSeparator;

void generate_shock_color_table (unsigned long seed, unsigned int size, 
                                 vcl_vector<SbColor>& color_table);

SoSeparator* draw_ms_graph (dbmsh3d_graph* ms_graph,
                            const SbColor& A1A3_color, const SbColor& A14_color,
                            const SbColor& Dege_A1A3_color, const SbColor& Dege_A14_color, 
                            const SbColor& LN_color,
                            const float vertex_radius, const SbColor& A3_rib_color, 
                            const SbColor& A13_axial_color, const SbColor& Dege_axial_color, 
                            const SbColor& A3_loop_color, const SbColor& A13_loop_color,
                            const SbColor& Vc_color,
                            const float curve_width, const bool idv = false,
                            const bool show_vid = false, const bool show_cid = false,
                            const bool draw_only_selected = false,
                            const bool user_defined_class = true);

SoSeparator* draw_ms_graph_flow (dbmsh3d_graph* ms_graph,
                            const SbColor& A1A3_color, const SbColor& A14_color,
                            const SbColor& Dege_A1A3_color, const SbColor& Dege_A14_color, 
                            const SbColor& LN_color,
                            const float vertex_radius, const SbColor& A3_rib_color, 
                            const SbColor& A13_axial_color, const SbColor& A13_axial2_color, 
                            const SbColor& A13_axial3_color, const SbColor& A13_axial4_color, 
                            const SbColor& Dege_axial_color, 
                            const SbColor& Vc_color,
                            const SbColor& A3_loop_color, const SbColor& A13_loop_color,
                            const float curve_width, const bool idv = false,
                            const bool show_vid = false, const bool show_cid = false,
                            const bool user_defined_class = true);

SoSeparator* draw_ms_vertices_ctable (dbmsh3d_graph* ms_graph, 
                                      const vcl_vector<SbColor>& ctable, 
                                      const float size,
                                      const bool user_defined_class_ = true);

SoSeparator* draw_ms_curves_ctable (dbmsh3d_graph* ms_graph, 
                                    const vcl_vector<SbColor>& ctable, 
                                    const float curve_width,
                                    const bool idv = false, const bool show_id = false,
                                    const bool skip_non_match_vc = true,
                                    const bool user_defined_class = true);

SoSeparator* draw_ms_sheets_ctable (dbsk3d_ms_hypg* ms_hypg, 
                                    const vcl_vector<SbColor>& ctable, 
                                    const float transp = 0.0f,
                                    const bool idv = false, const bool show_id = false,
                                    const bool user_defined_class = true);

SoSeparator* draw_ms_sheets (dbsk3d_ms_hypg* ms_hypg, 
                             const SbColor& color,
                             const float transp, const bool idv, const bool show_id,
                             const bool user_defined_class);

SoSeparator* draw_ms_sheets_bnd_mesh_ctable (dbsk3d_ms_hypg* ms_hypg, const int option,
                                             const vcl_vector<SbColor>& ctable, 
                                             const float transp = 0.0f,
                                             const bool user_defined_class_ = true);

SoSeparator* draw_ms_sheets_cost (dbsk3d_ms_hypg* ms_hypg, const int option, 
                                  const float transp = 0.0f, const bool idv = false, 
                                  const bool show_id = false,
                                  const bool user_defined_class = true);

SoSeparator* draw_ms_sheets_gdt (vcl_map <int, dbmsh3d_fmm_mesh*>& sheet_fmm_mesh_map,
                                 const float transp = 0.0f);

//: Visualization of sectional triangles along A3 ribs.
SoSeparator* draw_A3_sect_tris (dbsk3d_ms_hypg* ms_hypg, const SbColor& A3_sect_tris_color);

//: Visualization of ridge region bnd curves along A3 ribs.
SoSeparator* draw_A3_ridgebnd_curves (dbsk3d_ms_hypg* ms_hypg, const float& width, 
                                      const SbColor& A3_ridgebnd_color_a,
                                      const SbColor& A3_ridgebnd_color_b);

//: Visualization of shock-tab surface region bnd curves along A13 axials.
SoSeparator* draw_A13_surfbnd_curves (dbsk3d_ms_hypg* ms_hypg, const float& width, 
                                      const SbColor& A13_surfbnd_color_a,
                                      const SbColor& A13_surfbnd_color_b);


SoSeparator* draw_A3_ridge_vectors (dbsk3d_ms_hypg* ms_hypg, 
                                   const SbColor& color, const float& vector_len);

//: Visualization of ridge curves along A3 ribs.
SoSeparator* draw_A3_ridge_pts (dbsk3d_ms_hypg* ms_hypg, 
                                const float& radius, const SbColor& color);

SoSeparator* draw_A3_ridge_curves (dbsk3d_ms_hypg* ms_hypg, 
                                   const float& radius, const SbColor& color,
                                   const float& width);

SoSeparator* draw_A3rib_bnd_link (dbmsh3d_graph* ms_graph, const SbColor& color);
SoSeparator* draw_A13axial_bnd_link (dbmsh3d_graph* ms_graph, const SbColor& color);

#endif
