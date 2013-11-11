//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_wf_arc.h
//  MingChing Chang
//  Aug. 09, 2005.

#ifndef gdtvis_welm_h_
#define gdtvis_welm_h_

#include <gdt/gdt_welm.h>
#include <gdt/gdt_mesh.h>

#include <gdt/vis/gdtvis_interval.h>

class SoSeparator;
class SbColor;

SoSeparator* gdt_draw_welms (dbmsh3d_gdt_mesh* gdt_mesh, int nSamples, 
                             float lineWidth=3.0f);

SoSeparator* gdt_draw_W_vispt (gdt_welm* W, SbColor color, int nSamples);
void gdt_draw_W_geometry_vispt (SoSeparator* vis, gdt_welm* W, int nSamples);

SoSeparator* gdt_draw_RF_vispt (gdt_welm* RF, SbColor color, int nSamples);
void gdt_draw_RF_geometry_vispt (SoSeparator* vis, gdt_welm* RF, int nSamples);

#endif


