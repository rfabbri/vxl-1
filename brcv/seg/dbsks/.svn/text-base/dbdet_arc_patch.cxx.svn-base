// This is brcv/seg/dbdet/dbdet_arc_patch.cxx
//:
// \file

#include "dbdet_arc_patch.h"
#include <vil/vil_bilin_interp.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_line_segment_2d.h>

// =============================================================================
// dbdet_arc_patch
// =============================================================================


// -----------------------------------------------------------------------------
//: Compute coordinate and orientation of a grid of points
bool dbdet_arc_patch::
compute_grid(double dx, double dy, vnl_matrix<double >& x, vnl_matrix<double >& y, 
             vnl_matrix<double >& fx, vnl_matrix<double >& fy) const
{
  double len_over_2 = this->axis_.len() / 2;
  double w_over_2 = this->width_ / 2;
  int half_ni = (int) (len_over_2 / dx);
  int half_nj = (int) (w_over_2 / dy);

  // initialize the matrix
  x.set_size(2*half_ni+1, 2*half_nj+1);
  y.set_size(2*half_ni+1, 2*half_nj+1);
  fx.set_size(2*half_ni+1, 2*half_nj+1);
  fy.set_size(2*half_ni+1, 2*half_nj+1);


  // transform grid (i, j) --> local (u,v) --> global (x,y)
  for (int i = -half_ni; i <= half_ni; ++i)
  {
    double u = i*dx + len_over_2;
    vgl_point_2d<double > pt0 = this->axis_.point_at_length(u);
    vgl_vector_2d<double >  t0 = this->axis_.tangent_at_length(u);
    vgl_vector_2d<double > n0(-t0.y(), t0.x());
    for (int j = -half_nj; j <= half_nj; ++j)
    {
      double v = j * dy;
      vgl_point_2d<double > pt = pt0 + v * n0;
      vgl_vector_2d<double > t = t0;

      // set values to grid
      x(half_ni + i, half_nj+j) = pt.x();
      y(half_ni + i, half_nj+j) = pt.y();
      fx(half_ni + i, half_nj+j) = t.x();
      fy(half_ni + i, half_nj+j) = t.y();
    }
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Approximate center line of this patch
vgl_line_segment_2d<double > dbdet_arc_patch::
center_line() const
{
  double height = this->axis_.height();
  vgl_vector_2d<double > chord = this->axis_.point2() - this->axis_.point1();
  vgl_point_2d<double > top_pt = this->axis_.point_at(0.5);
  vgl_point_2d<double > line_center = centre(top_pt, this->axis_.chord_midpoint());
  return vgl_line_segment_2d<double >(line_center - chord/2, line_center + chord/2);
}


// =============================================================================
// =============================================================================
// =============================================================================
// =============================================================================
// =============================================================================

// -----------------------------------------------------------------------------
//: Compute histogram of orientated gradient (HOG) in an arc patch, taking the 
// central axis of the patch as the reference direction
vnl_vector<double > dbdet_compute_HOG(const vil_image_view<double >& Gx, 
                                     const vil_image_view<double >& Gy,
                                     const dbdet_arc_patch& patch, 
                                     int num_bins)
{
  // heuristic choice of number of bins
  //int num_bins = 9; // Number bins in histogram
  double angle_per_bin = vnl_math::pi / num_bins;
  
  // unit vectors used to compute gradient orientation
  vnl_vector<double > uu(num_bins, 0);
  vnl_vector<double > vv(num_bins, 0);
  for (int i =0; i < num_bins; ++i)
  {
    uu(i) = vcl_cos((i+0.5)*angle_per_bin);
    vv(i) = vcl_sin((i+0.5)*angle_per_bin);
  }
  
  // place holder for the histogram
  vnl_vector<double > hog(num_bins, 0);


  // compute world-coordinate and reference vector of grid points
  vnl_matrix<double > X, Y, Fx, Fy;
  double dx = 1; // spacing along the central axis
  double dy = 1; // spacing orthorgonal to the central axis
  patch.compute_grid(dx, dy, X, Y, Fx, Fy);

  // Now compute the gradient at each point by linear interpolation and 
  // and put them to corresponding bins
  unsigned ni = X.rows();
  unsigned nj = Y.cols();

  // \TODO there is room for improvement here
  // Instead of linear interpolation at each point, we can linearly interpolate 
  // the image at some scale, say 4, then use nearest neighbor to compute values
  // for the selected points
  for (unsigned i =0; i < ni; ++i)
  {
    for (unsigned j =0; j < nj; ++j)
    {
      // gradient in world-coordinate (x, y)
      double gx = vil_bilin_interp(Gx, X(i, j), Y(i, j));
      double gy = vil_bilin_interp(Gy, X(i, j), Y(i, j));
      
      // reference axis
      double fx = Fx(i, j);
      double fy = Fy(i, j);

      // gradient in local-coordinate system (u, v)
      double gu = gx*fx + gy*fy;    // cos
      double gv = gx*(-fy) + gy*fx; // sin

      // find out the corresponding bin of this gradient vector by computing the magnitude 
      // of dot product with the direction vector of each bin
      double max_dot = 0;
      int max_dot_bin = 0;
      for (int bin =0; bin < num_bins; ++bin)
      {
        double dot = vnl_math_abs(gu * uu[bin] + gv * vv[bin]);
        if (max_dot < dot)
        {
          max_dot_bin = bin; 
          max_dot = dot;
        }
      }

      // use magnitude of gradient vector as its weights in the histogram
      hog[max_dot_bin] += vnl_math_hypot(gu, gv);
    }
  }

  // normalize by the number of points computed
  hog /= (ni *nj);

  return hog;
}







//: Compute mean gradient magnitude in a patch
double dbdet_compute_mean_gradient_magnitude(const vil_image_view<double >& Gx, 
                                     const vil_image_view<double >& Gy,
                                     const dbdet_arc_patch& patch,
                                     double dx,
                                     double dy)
{
  // compute a grid of points for this big patch, from which we can compute
  // average gradient magnitude
  vnl_matrix<double > X, Y, Fx, Fy;
  patch.compute_grid(dx, dy, X, Y, Fx, Fy);

  // Now compute the gradient at each point by linear interpolation and 
  // and put them to corresponding bins
  unsigned ni = X.rows();
  unsigned nj = Y.cols();

  double sum_mag2 = 0;
  for (unsigned i =0; i < ni; ++i)
  {
    for (unsigned j =0; j < nj; ++j)
    {
      // gradient in world-coordinate (x, y)
      double gx = vil_bilin_interp_safe_extend(Gx, X(i, j), Y(i, j));
      double gy = vil_bilin_interp_safe_extend(Gy, X(i, j), Y(i, j));
      sum_mag2 += gx*gx + gy*gy;
    }
  }

  return vcl_sqrt(sum_mag2/(ni*nj));
}








