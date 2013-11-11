//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_face.h
//  MingChing Chang
//  May 03, 2005.

#ifndef dbmsh3d_vis_face_h_
#define dbmsh3d_vis_face_h_

#include <vgl/vgl_point_3d.h>
#include <dbmsh3d/dbmsh3d_face.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>
#include <Inventor/SbColor.h>

class dbmsh3d_mesh;
class SoGroup;
class SoSeparator;
class SoFaceSet;
class SbColor;
class SoBaseColor;
class SoMaterial;
class SoCoordinate3;

//: return the SoCoordinate3 to enable online alternation.
SoCoordinate3* _draw_F_ifs_geom (SoGroup* group, const dbmsh3d_face* F);
SoCoordinate3* _draw_F_mhe_geom (SoGroup* group, const dbmsh3d_face* F);

//: Check if the vertex is too close to any previous vertex.
//  return true if no duplicate.
bool _check_duplicate (SbVec3f* verts, unsigned int count, const vgl_point_3d<double>& pt);

void draw_F_geom (SoGroup* group, const dbmsh3d_face* F);

void draw_F_geom_vispt (SoGroup* group, const dbmsh3d_face* F);

SoSeparator* draw_F (const dbmsh3d_face* F, const SbColor& color, 
                     const float transp = 0.0f,
                     const bool user_defined_class = true);

SoSeparator* draw_F (const dbmsh3d_face* F,
                     const SoMaterial* material,
                     const bool user_defined_class = true);

SoSeparator* draw_F_with_id (const dbmsh3d_face* F,
                             const SbColor& color,
                             const float transp = 0.0f,
                             const bool user_defined_class = true);

SoSeparator* draw_F_textured (const dbmsh3d_textured_face_mc* F,
                              const bool user_defined_class = true);
#endif


