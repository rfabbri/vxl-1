//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_process_vis.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>

SoSeparator* draw_points_cor (vcl_vector<vgl_point_3d<double> >& cor_PS1, 
                              vcl_vector<vgl_point_3d<double> >& cor_PS2,
                              const SbColor& col1, const SbColor& col2, const SbColor& colm)
{
  SoSeparator* root = new SoSeparator;
  assert (cor_PS1.size() == cor_PS2.size());

  float cube_size = 0.1f;

  for (unsigned int i=0; i<cor_PS1.size(); i++) {
    root->addChild (draw_cube (cor_PS1[i], cube_size, col1));
    root->addChild (draw_cube (cor_PS2[i], cube_size, col2));
    root->addChild (draw_line (cor_PS1[i], cor_PS2[i], colm));
  }

  return root;
}
