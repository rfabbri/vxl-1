//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_shock.h
//  MingChing Chang
//  May 03, 2005.

#ifndef gdtvis_shock_h_
#define gdtvis_shock_h_

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbgdt3d/dbgdt3d_manager.h>

#include <Inventor/SbColor.h>

class SoGroup;
class SoSeparator;
class SoVertexProperty;

SoSeparator* gdt_draw_shocks (gdt_ws_manager* ws_manager, const float lineWidth);

SoSeparator* draw_shock (gdt_shock* S, SbColor color, const float lineWidth);

void gdt_draw_shock_geometry_vispt (SoSeparator* vis, const gdt_shock* S, 
                                    const vcl_vector<vgl_point_3d<double> >& path_vertices);

#endif


