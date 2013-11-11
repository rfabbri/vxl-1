//: MingChing Chang 040405
//  This is the visualization of the shock elements.

#ifndef dbsk3d_vis_fs_elm_h_
#define dbsk3d_vis_fs_elm_h_

#include <dbsk3d/dbsk3d_fs_vertex.h>
#include <dbsk3d/dbsk3d_fs_edge.h>
#include <dbsk3d/dbsk3d_fs_face.h>

class SbColor;
class SoSeparator;
class SoBaseColor;
class SoMaterial;

SoSeparator* draw_fs_face (dbsk3d_fs_face* FF, 
                           const SbColor& color, const float transp = 0.0f,
                           const bool user_defined_class = true);

void draw_fs_face_geom (SoSeparator* root, dbsk3d_fs_face* FF,
                        const bool user_defined_class = true);


SoSeparator* draw_fs_edge (dbsk3d_fs_edge* FE, 
                           const SbColor& color, const float width,
                           const bool draw_dual = true,
                           const bool user_defined_class = true);

SoSeparator* draw_fs_edge (dbsk3d_fs_edge* FE, const SoBaseColor* basecolor, 
                           const bool draw_dual = true,
                           const bool user_defined_class = true);

void draw_fs_edge_geom (SoSeparator* root, dbsk3d_fs_edge* FE,
                        const bool user_defined_class = true);

SoSeparator* draw_fs_vertex (dbsk3d_fs_vertex* FV, 
                             const SbColor& color, const float size,
                             const bool user_defined_class = true);

void draw_fs_vertex_geom (SoSeparator* root, dbsk3d_fs_vertex* FV, const float size,
                          const bool user_defined_class = true);

#endif
