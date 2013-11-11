#include "splr_pizza_slice_symmetry_representatives.h"

#include <vnl/vnl_math.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_algorithm.h>

double distance_from_origin_to_box_2d(vgl_box_2d<double> box){
  //if box contains origin then zero
  if (box.contains(vgl_point_2d<double>(0.,0.))){
    return 0.0;
  }
  //{(min_x, min_y), (max_x, min_y), (max_x, max_y), (min_x, max_y)}
  double px[4] = {box.min_x(), box.max_x(), box.max_x(), box.min_x()};
  double py[4] = {box.min_y(), box.min_y(), box.max_y(), box.max_y()};
  return vgl_distance_to_closed_polygon(px, py, 4, 0.0, 0.0);
}

double distance_from_origin_to_farthest_point_in_box_2d(vgl_box_2d<double> box){
  double xsq = vcl_max(box.min_x() * box.min_x(), box.max_x() * box.max_x());
  double ysq = vcl_max(box.min_y() * box.min_y(), box.max_y() * box.max_y());
  return sqrt(xsq + ysq);
}

/* Iterate through values of z.  For each, 
   iterate through values of r ranging from the 
   origin-to-bounding-box distance to the 
   origin-to-farthest-point-on-bounding-box distance.
   For each value of r, find the two extremal angles theta1 and 
   theta2 for which the bounding box has points at (r, theta1) 
   and (r, theta2)
*/

/* This code is written specifically for a box as input,
   but a similar algorithm will work for a general convex region
*/
void splr_pizza_slice_symmetry_representatives(biob_explicit_worldpt_roster_sptr representatives, unsigned int num_slices, biob_worldpt_box box, double spacing){
  double pizza_slice_angle = 2. * vnl_math::pi / num_slices;
  vgl_point_2d<double> origin_2d(0., 0.);
  //get projection of box into the plane defined by a fixed value of z
  //(For a general convex region, this depends on z
  vgl_box_2d<double> box_2d(vgl_point_2d<double>(box.min_x(), box.min_y()),
                            vgl_point_2d<double>(box.max_x(), box.max_y()));
  //  vgl_point_2d<double> closest_point = closest_point_on_box_2d(box_2d);
  //  double closest_dist = vgl_distance(closest_point, origin_2d);
  //  double farthest_dist = vgl_distance(farthest_point, origin_2d);
  //  vgl_point_2d<double> farthest_point = farthest_point_on_box_2d(box_2d);
  for (double r = distance_from_origin_to_box_2d(box_2d);
       r <= distance_from_origin_to_farthest_point_in_box_2d(box_2d);
       r += spacing){
    for (double theta = 0; theta <= pizza_slice_angle; theta += spacing/r){
      for (double z = box.min_z(); z <= box.max_z(); z += spacing){
         worldpt pt(r * cos(theta), r * sin(theta), z);
         representatives->add_point(pt);
      }
    }
  }

}
