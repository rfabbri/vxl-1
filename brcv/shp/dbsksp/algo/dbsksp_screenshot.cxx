// This is file shp/dbsksp/dbsksp_screenshot.cxx

//:
// \file

#include "dbsksp_screenshot.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_fragment.h>


//------------------------------------------------------------------------------
//: Superimpose a shock graph on top of an image
bool dbsksp_screenshot(const vil_image_view<vxl_byte >& bg_image, 
                       const dbsksp_xshock_graph_sptr& xgraph,
                       vil_image_view<vxl_byte >& screenshot,
                       int contour_radius,
                       int padding_width,
                       vil_rgb<vxl_byte > contour_color,
                       vil_rgb<vxl_byte > padding_color)
{
  screenshot.clear();

  if (bg_image.size()==0) 
    return false;

  // RGB image
  if (bg_image.nplanes() == 3)
  {
    screenshot.deep_copy(bg_image);
  }
  // Gray image
  else if (bg_image.nplanes() == 1)
  {
    // we want the screenshot to be RGB. So we duplicate the gray image
    // in all of the screenshot's 3 planes.
    screenshot.set_size(bg_image.ni(), bg_image.nj(), 3);
    for (unsigned i =0; i < bg_image.ni(); ++i)
    {
      for (unsigned j =0; j < bg_image.nj(); ++j)
      {
        screenshot(i, j, 0) = bg_image(i, j);
        screenshot(i, j, 1) = bg_image(i, j);
        screenshot(i, j, 2) = bg_image(i, j);
      }
    }
  }
  return dbsksp_screenshot_in_place(xgraph, screenshot, contour_radius, padding_width, contour_color, padding_color);
}






//------------------------------------------------------------------------------
//: Draw the shock graph on top of an existing image
bool dbsksp_screenshot_in_place(const dbsksp_xshock_graph_sptr& xgraph,
                       vil_image_view<vxl_byte >& screenshot,
                       int contour_radius,
                       int padding_width,
                       vil_rgb<vxl_byte > contour_color,
                       vil_rgb<vxl_byte > padding_color)
{
  if (!xgraph) return false;
  if (screenshot.size() == 0) return false;


  vxl_byte inner_color[] = {contour_color.R(), contour_color.G(), contour_color.B()}; //{0, 0, 255}; // blue
  vxl_byte outer_color[] = {padding_color.R(), padding_color.G(), padding_color.B()}; //{255, 255, 255}; // white

  
  int inner_radius = contour_radius;
  int outer_radius = inner_radius + padding_width;

  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin(); eit !=
    xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;
    dbsksp_xshock_node_descriptor start = *xe->source()->descriptor(xe);
    dbsksp_xshock_node_descriptor end = xe->target()->descriptor(xe)->opposite_xnode();
    dbsksp_xshock_fragment xfrag(start, end);

    dbgl_biarc bnd[] = {xfrag.bnd_left_as_biarc(), xfrag.bnd_right_as_biarc()};
    for (int i=0; i < 2; ++i)
    {
      dbgl_biarc biarc = bnd[i];
      double len = biarc.len();
      for (double s = 0; s <= len; s = s+1)
      {
        vgl_point_2d<double > p = biarc.point_at(s);
        vgl_vector_2d<double > t = biarc.tangent_at(s);
        vgl_vector_2d<double > n(-t.y(), t.x());

        // outer color
        for (int k = -outer_radius; k <= outer_radius; ++k)
        {
          vgl_point_2d<double > pt = p + k * n;
          vgl_point_2d<int > pi( int(pt.x()), int(pt.y()));

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
          vgl_point_2d<int > pi( int(pt.x()), int(pt.y()));

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
  }
  return true;
}







