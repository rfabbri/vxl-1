//: dbsk3d/vis/dbsk3d_vis_fs_segre.h
//  Visualization of the shock segregation elements.
//  Dec 19, 2006
//  MingChing Chang

#ifndef dbsk3d_vis_fs_segre_h_
#define dbsk3d_vis_fs_segre_h_

#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/algo/dbsk3d_fs_segre.h>

class SoSeparator;

SoSeparator* draw_segre_Q1_L_F (dbsk3d_fs_segre* fs_segre, 
                                const float th1, const float th2,
                                const int option,
                                const bool user_defined_class);

SoSeparator* draw_segre_Q1_batch (dbsk3d_fs_segre* fs_segre, 
                                  const float th1, const float th2,
                                  const bool type_I_only, const float len);

SoSeparator* draw_segre_Q2_L_F (dbsk3d_fs_segre* fs_segre, 
                                const float th1, const float th2,
                                const int option,
                                const bool user_defined_class);

SoSeparator* draw_segre_oversize_L_F (dbsk3d_fs_mesh* fs_mesh,
                                      const float size_th,
                                      const bool user_defined_class);

#endif
