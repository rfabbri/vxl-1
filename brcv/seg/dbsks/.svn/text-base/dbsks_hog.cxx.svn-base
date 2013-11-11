// This is file seg/dbsks/dbsks_hog.cxx

//:
// \file

#include "dbsks_hog.h"
#include <dbgl/algo/dbgl_circ_arc.h>
#include "dbdet_arc_patch.h"


//: Compute feacture vector of histogram of orientation gradient (HOG) along the
// boundary of an extrinsic fragment
bool dbsks_hog_boundary(vnl_vector<double >& hog,
                        const vil_image_view<double >& Gx,
                        const vil_image_view<double >& Gy,
                        const dbsksp_xshock_fragment& xfrag, 
                        int num_segments, 
                        double patch_width, 
                        int num_orientation_bins)
{
  // Preliminary checks
  if (num_segments <= 0) return false;
  if (patch_width < 1) return false;
  if (num_orientation_bins < 2) return false;

  
  // List of patches to compute HOG of
  vcl_vector<dbdet_arc_patch > patch_list(2*num_segments);

  // Divide each boundary contour into equal-length segments, approximate each
  // with a circular arc, then compute histogram for each segment
  dbgl_biarc bnd_left = xfrag.bnd_left_as_biarc();
  double len_left = bnd_left.len();
  for (int i =0; i < num_segments; ++i)
  {
    vgl_point_2d<double > start = bnd_left.point_at( i*len_left / num_segments );
    vgl_vector_2d<double > start_tangent = bnd_left.tangent_at(i*len_left / num_segments );
    vgl_point_2d<double > end = bnd_left.point_at((i+1)*len_left / num_segments);
    vgl_vector_2d<double > end_tangent = bnd_left.tangent_at((i+1)*len_left / num_segments);

    vgl_point_2d<double > midpt = bnd_left.point_at( (i+0.5)*len_left / num_segments );
    
    // estimate this curve segment with a circular arc
    dbgl_circ_arc arc;
    if (!arc.set_from(start, midpt, end))
    {
      hog.clear();
      return false;
    }
    
    // contruct the arc patch
    dbdet_arc_patch patch(arc, patch_width);

    // save
    patch_list[i] = patch;
  }


  dbgl_biarc bnd_right = xfrag.bnd_right_as_biarc();
  double len_right = bnd_right.len();
  for (int i =0; i < num_segments; ++i)
  {
    vgl_point_2d<double > start = bnd_right.point_at( i*len_right / num_segments );
    vgl_vector_2d<double > start_tangent = bnd_right.tangent_at(i*len_right / num_segments );
    vgl_point_2d<double > end = bnd_right.point_at((i+1)*len_right / num_segments);
    vgl_vector_2d<double > end_tangent = bnd_right.tangent_at((i+1)*len_right / num_segments);

    vgl_point_2d<double > midpt = bnd_right.point_at( (i+0.5)*len_right / num_segments );
    
    // estimate this curve segment with a circular arc
    dbgl_circ_arc arc;
    if (!arc.set_from(start, midpt, end))
    {
      hog.clear();
      return false;
    }
    
    // contruct the arc patch
    dbdet_arc_patch patch(arc, patch_width);

    // save
    patch_list[i+num_segments] = patch;
  }

  // Reserve space for the returned feature vector
  hog.set_size(2*num_segments*num_orientation_bins*4);

  // compute HOG for each patch
  for (unsigned i =0; i < patch_list.size(); ++i)
  {
    dbdet_arc_patch patch = patch_list[i];
    dbgl_circ_arc arc = patch.axis();


    // compute HOG for the patch
    vnl_vector<double > hog0 = dbdet_compute_HOG(Gx, Gy, patch, num_orientation_bins);

    // Normalize the HOG by the mean gradient magnitude in the area
    double mean_gradient_magnitude[4];
    {
      // approximate the patch with a rectangle
      vgl_line_segment_2d<double > center_line = patch.center_line();
      vgl_vector_2d<double > v = center_line.point2() - center_line.point1();
      vgl_vector_2d<double > n = center_line.normal();


      // create centerline for 4 big patches surrounding this patch
      vgl_point_2d<double > top_left_start = center_line.point1() + patch_width * n;
      vgl_point_2d<double > top_left_end = top_left_start + 2 * v;
      
      vgl_point_2d<double > top_right_start = center_line.point1() - patch_width * n;
      vgl_point_2d<double > top_right_end = top_right_start + 2*v;

      vgl_point_2d<double > bot_left_end = center_line.point2() + patch_width * n;
      vgl_point_2d<double > bot_left_start = bot_left_end - 2 * v;

      vgl_point_2d<double > bot_right_end = center_line.point2() - patch_width * n;
      vgl_point_2d<double > bot_right_start = bot_right_end - 2 * v;

      // construct the patches
      dbdet_arc_patch top_left(dbgl_circ_arc(top_left_start, top_left_end, 0), 2*patch_width);
      dbdet_arc_patch top_right(dbgl_circ_arc(top_right_start, top_right_end, 0), 2*patch_width);
      dbdet_arc_patch bot_left(dbgl_circ_arc(bot_left_start, bot_left_end, 0), 2*patch_width);
      dbdet_arc_patch bot_right(dbgl_circ_arc(bot_right_start, bot_right_end, 0), 2*patch_width);

      
      mean_gradient_magnitude[0] = dbdet_compute_mean_gradient_magnitude(Gx, Gy, top_left, 2, 2);
      mean_gradient_magnitude[1] = dbdet_compute_mean_gradient_magnitude(Gx, Gy, top_right, 2, 2);
      mean_gradient_magnitude[2] = dbdet_compute_mean_gradient_magnitude(Gx, Gy, bot_left, 2, 2);
      mean_gradient_magnitude[3] = dbdet_compute_mean_gradient_magnitude(Gx, Gy, bot_right, 2, 2);      
    }
    
    //// compute mean gradient magnitude in the neighborhood
    //dbgl_circ_arc big_arc(arc.point_at(-0.5), arc.point_at(1.5), arc.k());
    //dbdet_arc_patch big_patch(big_arc, 2*patch_width);
    //double mean_gradient_mag = dbdet_compute_mean_gradient_magnitude(Gx, Gy,
    //  big_patch, 2, 2);

    //// normalize !
    
    //hog0 /= (mean_gradient_mag + 1e-8); // to avoid divide by 0
    for (int k =0; k < 4; ++k)
    {
      // record
      hog.update(hog0 / (mean_gradient_magnitude[k]+1e-8), (i*4+k) *num_orientation_bins); 
    }

    
  }

  return true;
}

