//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_mesh.h
//  MingChing Chang
//  May 03, 2005.

#ifndef dbmsh3d_vis_mesh_h_
#define dbmsh3d_vis_mesh_h_

#include <vcl_set.h>
#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/algo/dbmsh3d_mesh_algos.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/engines/SoTimeCounter.h>

class SoGroup;
class SoSeparator;
class SoVertexProperty;

//###############################################################
int _count_faces_indices (const vcl_vector<vcl_vector<int> >& faces);

void draw_ifs_geom (SoGroup* root, 
                    const vcl_vector<vgl_point_3d<double> >& pts,
                    const vcl_vector<vcl_vector<int> >& faces);

void draw_ifs_geom (SoGroup* root, vcl_set<dbmsh3d_vertex*>& V_set,
                    vcl_set<dbmsh3d_face*>& F_set);

void draw_faces_geom (SoGroup* root, vcl_map<int, dbmsh3d_vertex*>& V_map,
                      vcl_map<int, dbmsh3d_face*>& F_map);

SoSeparator* draw_ifs (const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces,
                       const int colorcode = COLOR_SILVER,
                       const bool b_shape_hints = false, 
                       const float transp = 0.0f);

SoSeparator* draw_ifs (const vcl_vector<vgl_point_3d<double> >& pts,
                       const vcl_vector<vcl_vector<int> >& faces,
                       const SbColor& color,
                       const bool b_shape_hints = false, 
                       const float transp = 0.0f);

SoSeparator* draw_ifs (vcl_set<dbmsh3d_vertex*>& pts,
                       vcl_set<dbmsh3d_face*>& faces,
                       const int colorcode = COLOR_SILVER,
                       const bool b_shape_hints = false, 
                       const float transp = 0.0f);

SoSeparator* draw_ifs (vcl_set<dbmsh3d_vertex*>& pts,
                       vcl_set<dbmsh3d_face*>& faces,
                       const SbColor& color,
                       const bool b_shape_hints = false, 
                       const float transp = 0.0f);

//###############################################################
// input: dbmsh3d_mesh* M
//  return:
//     SoVertexProperty* vp,
//     int *ind
void _draw_M_ifs_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind);
void _draw_M_mhe_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int n_ind, int* ind);

void draw_M_ifs_geom (SoGroup* root, dbmsh3d_mesh* M);
void draw_M_mhe_geom (SoGroup* root, dbmsh3d_mesh* M);

//: default color code is COLOR_SILVER.
//  return the SoSeparator created for this mesh.
SoSeparator* draw_M (dbmsh3d_mesh* M, const bool b_shape_hints = false,
                     const float transp = 0.0f, const int colorcode = COLOR_SILVER);

//: Similar to the above but only draw the marked face.
void _draw_M_visited_ifs_geom (dbmsh3d_mesh* M, SoVertexProperty* vp, int* ind);
void draw_M_visited_ifs_geom (SoGroup* root, dbmsh3d_mesh* M);

SoSeparator* draw_M_ifs_visited (dbmsh3d_mesh* M, const int colorcode = 0,
                                const bool b_shape_hints = false,
                                const float transp = 0.0f);

//: option 1: draw non-manifold-1-ring ones in RED.
//  option 2: draw non-1-ring vertices in BLUE. 
//  option 3: draw both. 
SoSeparator* draw_M_topo_vertices (dbmsh3d_mesh* M, const int option,
                                   const float size, const bool user_defined_class = true);

SoSeparator* draw_M_edges_idv (dbmsh3d_mesh* M, const SbColor& color, 
                               const float width, const bool user_defined_class = true);

SoSeparator* draw_M_edges (dbmsh3d_mesh* M, const SbColor& color, const float width);

void draw_M_mhe_edges_geom (SoSeparator* root, dbmsh3d_mesh* M);
void draw_M_ifs_edges_geom (SoSeparator* root, dbmsh3d_mesh* M);

SoSeparator* draw_M_bndcurve (dbmsh3d_mesh* M, const int colorcode, const float width);

SoSeparator* draw_M_faces_idv (dbmsh3d_mesh* M, const bool b_shape_hints = false, 
                               const float transp = 0.0f, const int colorcode = 0,
                               const bool user_defined_class = true);

SoSeparator* draw_M_color (dbmsh3d_mesh* M, 
                           const bool b_shape_hints, const float trans,
                           const vcl_vector<SbColor>& color_set,
                           const bool user_defined_class = true);

SoSeparator* draw_mesh_vertices (dbmsh3d_mesh* M, const float size);

SoSeparator* draw_M_textured (dbmsh3d_textured_mesh_mc* M);

//############ Visualize Mesh Geometry & Topology Info ############

SoSeparator* draw_M_bnd_faces_cost_col (dbmsh3d_mesh* M, const bool draw_idv,
                                        const bool showid = false,
                                        const float transp = 0.0f);

SoSeparator* draw_M_bnd_faces_topo_col (dbmsh3d_mesh* M, const bool draw_idv,
                                        const bool showid = false,
                                        const float transp = 0.0f,
                                        const bool user_defined_class = true);
VIS_COLOR_CODE get_M_face_topo_color (const TRIFACE_TYPE type);

SoSeparator* draw_M_bnd_faces_anim (dbmsh3d_mesh* M, const int nF_batch = 30);

void draw_faces_in_switch (SoSeparator* root, 
                           const vcl_vector<dbmsh3d_face*>& Faces);

#endif

