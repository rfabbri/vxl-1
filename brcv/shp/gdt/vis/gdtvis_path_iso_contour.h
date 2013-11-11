//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_gdt.h
//  MingChing Chang
//  Aug. 09, 2005.

#ifndef gdtvis_path_iso_contour_h_
#define gdtvis_path_iso_contour_h_

#include <gdt/gdt_manager.h>
#include <gdt/gdt_path.h>

class SoSeparator;
class SbColor;
class SoDrawStyle;

SoSeparator* gdt_draw_result (gdt_manager* gdt_manager, 
                              int n_query_contours, 
                              int n_verbose,
                              float lineWidth=3);

void gdt_draw_iso_contours (SoSeparator* group,
                            dbmsh3d_mesh* gdt_mesh, 
                            int n_query_contours, 
                            float total_length,
                            float step_dist,
                            float lineWidth=3);

void gdt_draw_isocontour_I_face (SoSeparator* group, 
                                 gdt_interval* input_I, 
                                 dbmsh3d_face* cur_face, 
                                 double gdt_dist, float lineWidth);

void gdt_draw_gdt_path (SoSeparator* group, gdt_path* gdt_path,
                        SbColor color, float lineWidth);

#endif


