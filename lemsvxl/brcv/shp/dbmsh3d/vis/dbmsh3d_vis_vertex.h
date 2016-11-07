//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_vertex.h
//  MingChing Chang
//  May 03, 2005.

#ifndef dbmsh3d_vis_vertex_h_
#define dbmsh3d_vis_vertex_h_

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

class SoSeparator;
class SbColor;

SoSeparator* draw_vertex_geom_SoPoint (const dbmsh3d_vertex* V);
SoSeparator* draw_vertex_geom_vispt_SoPoint (const dbmsh3d_vertex* V);

SoSeparator* draw_vertex_vispt_SoPoint (const dbmsh3d_vertex* V,                                             
                                        const SbColor& color, const float size);

SoSeparator* draw_vertex_geom_SoCube (const dbmsh3d_vertex* V, 
                                      const float size);
SoSeparator* draw_vertex_geom_vispt_SoCube (const dbmsh3d_vertex* V, 
                                            const float size);

SoSeparator* draw_vertex_SoCube (const dbmsh3d_vertex* V, 
                                 const SbColor& color, const float size);
SoSeparator* draw_vertex_vispt_SoCube (const dbmsh3d_vertex* V, 
                                       const SbColor& color, const float size,
                                       const bool show_id = false);

#endif


