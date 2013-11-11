//: dbsk3d/vis/dbsk3d_vis_fs_sheet.h
//  Visualization of the fine-scale shock sheet component
//  Dec 026 2006
//  MingChing Chang

#ifndef dbsk3d_vis_fs_sheet_h_
#define dbsk3d_vis_fs_sheet_h_

#include <dbsk3d/dbsk3d_fs_sheet_set.h>

class SbColor;
class SoSeparator;

//##########################################################################

SoSeparator* draw_fs_sheet_set (dbsk3d_fs_sheet_set* fs_ss, const int show_inf, 
                                const bool idv = false, const bool user_defined_class = true);

SoSeparator* draw_fs_sheet_set_bnd_asgn (dbsk3d_fs_sheet_set* fs_ss, const int option,
                                         const float cube_size);

//: Vis bnd-shock asso. of all sheet components (camouflage color).
SoSeparator* draw_fs_sheet_set_bnd_mesh (dbsk3d_fs_sheet_set* fs_ss, 
                                         const int option, const float transp = 0.0f);

//: Vis bnd mesh of the given sheet components.
//  return number of triangles drawn.
int draw_fs_sheet_bnd_mesh (SoSeparator* root, dbsk3d_fs_sheet* S, 
                            const int option, const SbColor& color, const float transp = 0.0f);

//##########################################################################

SoSeparator* draw_fs_sheet (dbsk3d_fs_sheet* S, const SbColor& color, const float transp = 0.0f,
                            const bool idv = false, const bool show_id = false, 
                            const bool user_defined_class = true);

void draw_fs_sheet_geom (SoSeparator* root, dbsk3d_fs_sheet* S,
                         const bool user_defined_class);

void draw_fs_sheet_mesh_geom (SoGroup* root, dbsk3d_fs_sheet* S,
                              const vcl_vector<vgl_point_3d<double> >& pts,
                              const vcl_vector<vcl_vector<int> >& faces);

SoSeparator* draw_fs_sheet_A13_A3_bnd_curves (dbsk3d_fs_sheet* S, const float width);

SoSeparator* draw_fs_sheet_bnd_asgn (dbsk3d_fs_sheet* S, const bool draw_idv, const float cube_size);

SoSeparator* draw_fs_sheet_bnd_mesh (dbsk3d_fs_sheet* S, const int option, 
                                     const int draw_idv, const float transp = 0.0f);

SoSeparator* draw_tabs_bnd_curve (dbsk3d_fs_sheet_set* fs_ss, const float cost_th);

SoSeparator* draw_S_bnd_curve (dbsk3d_fs_sheet* S);

#endif
