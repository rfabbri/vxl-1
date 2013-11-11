//: MingChing Chang 041130
//  This is the visualization of the medial scaffold elements.

#ifndef dbsk3d_vis_ms_elm_h_
#define dbsk3d_vis_ms_elm_h_

#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_ms_sheet.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoDrawStyle.h>

//: If user_defined_class is false, interrogation capability will be disabled,
//  but better for outputing the visualization to a standard *.iv file.

SoSeparator* draw_ms_vertex (const dbsk3d_ms_node* SVertex, 
                             const SbColor color, const float size,
                             const bool show_id = false,
                             const bool user_defined_class = true);

void draw_ms_vertex_geom (SoSeparator* root, const dbsk3d_ms_node* SVertex, 
                          const float vertex_radius, const bool user_defined_class = true);

SoSeparator* draw_ms_curve (const dbsk3d_ms_curve* SCurve, 
                            const SbColor color, const float width,
                            const bool idv = false, const bool show_id = false,
                            const bool user_defined_class = true);

void draw_ms_curve_geom (SoSeparator* root, const dbsk3d_ms_curve* SCurve,
                         const bool use_coin3d_class = true);

SoSeparator* draw_ms_loop (dbsk3d_ms_curve* sLoop, 
                           const SbColor color, const float width,
                           const bool idv = false, const bool show_id = false,
                           const bool user_defined_class = true);

void draw_ms_loop_geom (SoSeparator* root, const dbsk3d_ms_curve* SLoop,
                        const bool user_defined_class = 0);

SoSeparator* draw_ms_sheet (dbsk3d_ms_sheet* MS, 
                            const SbColor color, const float transp,
                            const bool idv = false, const bool show_id = false,
                            const bool user_defined_class = true);

void draw_ms_sheet_geom (SoSeparator* root, dbsk3d_ms_sheet* MS,
                         const bool user_defined_class = true);

void draw_SS_faces_geom (SoGroup* root, dbsk3d_ms_sheet* MS,
                         vcl_map<int, dbmsh3d_vertex*>& V_map,
                         vcl_map<int, dbmsh3d_face*>& F_map);

void draw_SS_ifs_geom (SoGroup* root, dbsk3d_ms_sheet* MS,
                       const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces);

#endif
