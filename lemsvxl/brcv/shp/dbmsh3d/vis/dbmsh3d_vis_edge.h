//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_mesh.h
//  MingChing Chang
//  May 03, 2005.

#ifndef _dbmsh3d_vis_edge_h_
#define _dbmsh3d_vis_edge_h_

#include <Inventor/SbColor.h>
class SoSeparator;

void draw_edge_geom (SoSeparator* root, const dbmsh3d_edge *E,
                     const bool user_defined_class = true);

SoSeparator* draw_edge (dbmsh3d_edge* E, const SbColor& color, const float width = 1.0f,
                        const bool user_defined_class = true);


#endif


