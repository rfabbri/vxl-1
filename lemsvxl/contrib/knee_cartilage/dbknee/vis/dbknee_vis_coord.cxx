//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_point_set.cxx
// This is dbknee/vis/dbknee_vis_coord.cxx


//:
// \file

#include "dbknee_vis_coord.h"
#include <vnl/vnl_math.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBaseColor.h>


// ----------------------------------------------------------------------------
SoSeparator* dbknee_draw_3_axes(const dbknee_cylinder_based_coord& cs,
                                int x_color, int y_color, int z_color)
{
  SoSeparator* root = new SoSeparator;

  SoSeparator* vis;

  // draw the axes of the new coordinate system
  double axis_length = 1.5 * vnl_math_max(cs.cylinder().length()/2, cs.cylinder().radius());
  
  vis = draw_line(cs.origin(), cs.origin() + axis_length*cs.x_axis(), 
    color_from_code(x_color));
  root->addChild(vis);
  
  vis = draw_line(cs.origin(), cs.origin() + axis_length*cs.y_axis(), 
    color_from_code(y_color));
  root->addChild(vis);
  
  vis = draw_line(cs.origin(), cs.origin() + axis_length*cs.z_axis(), 
    color_from_code(z_color));
  root->addChild(vis);

  return root;
}





