//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/gdtvis_wf_arc.h
//  MingChing Chang
//  Aug. 09, 2005.

#ifndef _gdtvis_interval_h_
#define _gdtvis_interval_h_

#include <dbmsh3d/gdt/gdt_interval.h>
#include <dbmsh3d/gdt/gdt_mesh.h>

class SoSeparator;

SoSeparator* gdt_draw_I_vispt (const gdt_interval* I, SbColor color, int n_samples);

void gdt_draw_I_geometry_vispt (SoSeparator* vis, const gdt_interval* I, int n_samples);

SoSeparator* gdt_draw_edge_I (dbmsh3d_gdt_edge* edge, int n_test_lines);

SoSeparator* gdt_draw_intervals (dbmsh3d_gdt_mesh* gdt_mesh, int n_test_lines, 
                                 float lineWidth=3);

SoSeparator* gdtf_draw_I_on_front (dbmsh3d_gdt_mesh* gdt_mesh, 
                                   int n_test_lines, 
                                   float lineWidth=3);

SoSeparator* gdt_draw_I_on_front_flag (dbmsh3d_gdt_mesh* gdt_mesh, 
                                       int n_test_lines, 
                                       float lineWidth=3);
#endif


