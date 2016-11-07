//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_vis.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

#ifndef dbmsh3dr_vis_h_
#define dbmsh3dr_vis_h_

#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>

class SoSeparator;

SoSeparator* draw_points_cor (vcl_vector<vgl_point_3d<double> >& cor_PS1, 
                              vcl_vector<vgl_point_3d<double> >& cor_PS2,
                              const SbColor& col1, const SbColor& col2, const SbColor& colm);

#endif
