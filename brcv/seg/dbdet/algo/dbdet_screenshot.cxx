// This is file shp/dbdet/dbdet_screenshot.cxx

//:
// \file

#include "dbdet_screenshot.h"
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>

//: Draw a contour fragment map on top of an existing image
bool dbdet_screenshot(dbdet_curve_fragment_graph& cfg, 
                       vil_image_view<vxl_byte >& screenshot)
{
  // preliminary checks
  if (screenshot.size() == 0) return true;

  vxl_byte white[] = {255, 255, 255}; // white
  vxl_byte blue[] = {0, 0, 255}; // blue

  int outer_radius = 1;
  int inner_radius = 1;

  bool randomized_color = true;

  vnl_random random;

  // draw each edgel chain with a different color
  dbdet_edgel_chain_list_iter f_it = cfg.frags.begin();
  for (; f_it != cfg.frags.end(); f_it++)
  {
    dbdet_edgel_chain* chain = (*f_it);

    vxl_byte* outer_color = white;;
    vxl_byte inner_color[3];
    if (randomized_color)
    {
      inner_color[0] = (vxl_byte)random(255);
      inner_color[1] = (vxl_byte)random(255);
      inner_color[2] = (vxl_byte)random(255);
    }
    else
    {
      inner_color[0] = blue[0];
      inner_color[1] = blue[1];
      inner_color[2] = blue[2];
    }

    for (unsigned j=0; j<chain->edgels.size(); j++)
    {
      vgl_point_2d<double > p = chain->edgels[j]->pt;
      double angle = chain->edgels[j]->tangent;
      vgl_vector_2d<double > t(vcl_cos(angle), vcl_sin(angle));
      vgl_vector_2d<double > n(-t.y(), t.x());

      // outer color
      for (int k = -outer_radius; k <= outer_radius; ++k)
      {
        vgl_point_2d<double > pt = p + k * n;
        vgl_point_2d<int > pi( vnl_math::rnd(pt.x()), vnl_math::rnd(pt.y()));

        if (screenshot.in_range(pi.x(), pi.y()))
        {
          for (unsigned plane =0; plane < screenshot.nplanes(); ++plane)
          {
            screenshot(pi.x(), pi.y(), plane) = outer_color[plane];
          }
        }
      } // for

      // inner color
      for (int k = -inner_radius; k <= inner_radius; ++k)
      {
        vgl_point_2d<double > pt = p + k * n;
        vgl_point_2d<int > pi( vnl_math::rnd(pt.x()), vnl_math::rnd(pt.y()));

        if (screenshot.in_range(pi.x(), pi.y()))
        {
          for (unsigned plane =0; plane < screenshot.nplanes(); ++plane)
          {
            screenshot(pi.x(), pi.y(), plane) = inner_color[plane];
          }
        }
      } // for
    }
  }
  return true;
}







