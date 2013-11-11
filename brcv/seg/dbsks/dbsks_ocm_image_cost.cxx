// This is file seg/dbsks/dbsks_ocm_image_cost.cxx

//:
// \file

#include "dbsks_ocm_image_cost.h"

#include <dbsks/dbsks_utils.h>
#include <dbsksp/dbsksp_shapelet.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_gaussian_kernel.h>

#include <bil/algo/bil_edt.h>
#include <vnl/vnl_math.h>

// ============================================================================
//  dbsks_ocm_image_cost
// ============================================================================



// ----------------------------------------------------------------------------
//: Set the member variables
void dbsks_ocm_image_cost::
set_edgemap(const vil_image_view<float > edgemap, float edge_threshold)
{
  this->edgemap_ = edgemap;
  this->edge_threshold_ = edge_threshold;

  // Compute DT and gradient of DT of edgemap

  //: Compute distance transform of the edge map 
  this->compute_dt(this->edgemap_, this->edge_threshold_, this->dt_);
  
  // Compute gradient maps of the DT
  this->compute_gradient(this->dt_, 
    this->dt_grad_x_, this->dt_grad_y_, this->dt_grad_mag_);
}



// -----------------------------------------------------------------------------
// Maximum value for ocm cost (depends on ocm_type)
float dbsks_ocm_image_cost::
max_cost() const
{
  switch (this->ocm_type_in_use())
  {
  case L2_DISTANCE_ANGLE:
    return 9.0f;
  case SHOTTON_OCM:
    return 3.0f;
  default:
    return vnl_numeric_traits<float >::maxval;
  }
}



// -----------------------------------------------------------------------------
//: (Normalized) Oriented Chamfer Matching of a set of (point-tangent)s
float dbsks_ocm_image_cost::
f(const vcl_vector<vgl_point_2d<double > >& pts,
  const vcl_vector<vgl_vector_2d<double > >& tangents) const
{
  switch (this->ocm_type_in_use())
  {
  case L2_DISTANCE_ANGLE:
    return this->f_L2(pts, tangents);
  case SHOTTON_OCM:
    return this->f_shotton_ocm(pts, tangents);
  default:
    vcl_cout << "ERROR: Unknown OCM (oriented chamfer matching type).\n";
    assert(false);
    return vnl_numeric_traits<float >::maxval;
  }
}


// ----------------------------------------------------------------------------
//: Compute L^2 Oriented Chamfer Matching cost for a grid of circular arcs
void dbsks_ocm_image_cost::
f(const dbsks_circ_arc_grid& grid, float ds,
  vbl_array_1d<vnl_matrix<float > >& image_cost)
{
  // Clip the cost
  float default_value = this->max_cost();
  /////////////////////////////////////////////

  // allocate memory for the cost grid
  // we organize the result as an array of a 2D matrix
  // each 2D matrix saves the cost for one circular arc translated around the image
  int num_planes = grid.size() / (grid.num_x_*grid.num_y_);
  image_cost.clear();
  image_cost.reserve(num_planes);
  for (int i=0; i< num_planes; ++i)
  {
    image_cost.push_back(vnl_matrix<float >());
  }
 
  for (unsigned i=0; i<image_cost.size(); ++i)
  {
    image_cost[i].set_size(grid.num_x_, grid.num_y_);
    image_cost[i].fill(default_value);
  }

  vcl_cout << "i_chord = ";
  for (int i_chord=0; i_chord < grid.num_chord_; ++i_chord)
  {
    vcl_cout << " " << i_chord;
    double chord = grid.chord_[i_chord];
    for (int i_height=0; i_height < grid.num_height_; ++i_height)
    {
      double height = grid.height_[i_height];

      // all reference to 
      double x0 = grid.x_[0];
      double y0 = grid.y_[0];
      double theta0 = grid.theta_[0];

      vgl_point_2d<double > pt0(x0, y0);
      vgl_vector_2d<double > t0(vcl_cos(theta0), vcl_sin(theta0));

      if (vnl_math_abs(height) > chord/3)
      {
        continue;
      }

      dbgl_circ_arc arc;
      if (! arc.set_from(chord, height, pt0, t0))
      {
        continue;
      }
     
      // compute the point set that will used to compute energy
      vcl_vector<vgl_point_2d<double > > pts_ref;
      vcl_vector<vgl_vector_2d<double > > tangents_ref;
      arc.compute_samples(ds, pts_ref, tangents_ref);
      
      // Rotated points
      vcl_vector<vgl_point_2d<double > > pts_rot = pts_ref;
      vcl_vector<vgl_vector_2d<double > > tangents_rot = tangents_ref;

      // Rotated-then-translated points
      vcl_vector<vgl_point_2d<double > > pts = pts_ref;
      vcl_vector<vgl_vector_2d<double > > tangents = tangents_ref;

      // iterate thru rotation and translation
      // we only have to compute for half of the rotation angles
      // the other half can be mirrored from the computed ones
      vgl_point_2d<double > origin(x0, y0);

      for (int i_theta = 0; i_theta < (grid.num_theta_)/2; ++i_theta)
      {
        double theta_diff = grid.theta_[i_theta] - theta0;

        // rotate the point set
        dbsks_rotate_point_set(pts_ref, tangents_ref, origin, theta_diff,
          pts_rot, tangents_rot);

        int i_plane = grid.grid_to_linear(i_chord, i_height, i_theta);
        vnl_matrix<float>& image_cost_plane = image_cost[i_plane].as_ref();
        for (int i_x = 0; i_x < grid.num_x_; ++i_x)
        {
          double x_diff = grid.x_[i_x] - x0;
          for (int i_y = 0; i_y < grid.num_y_; ++i_y)
          {
            double y_diff = grid.y_[i_y] - y0;
            vgl_vector_2d<double > t(x_diff, y_diff);

            // Translate the point set
            dbsks_translate_point_set(pts_rot, tangents_rot, t, pts, tangents);


            // sample the potential map with these points
            float pot_cost = this->f(pts, tangents); 
            image_cost_plane(i_x, i_y) = float(pot_cost);
            
          } //i_y
        } // i_x
      } // i_theta
    } // height
  } // chord
  vcl_cout << "\n";



  // Mirrored the second half from the first half
  vcl_cout << "Mirroring: i_chord =";
  for (int i_chord=0; i_chord < grid.num_chord_; ++i_chord)
  {
    vcl_cout << " " << i_chord;
    double chord = grid.chord_[i_chord];
    for (int i_height=0; i_height < grid.num_height_; ++i_height)
    { 
      double height = grid.height_[i_height];
      if (vnl_math_abs(height) > chord/3)
      {
        continue;
      }
     
      // iterate thru rotation and translation
      // we only have to compute for half of the rotation angles
      // the other half can be mirrored from the computed ones
      int i_height_mirror = (grid.num_height_-1) - i_height;
      for (int i_theta = grid.num_theta_/2; i_theta < grid.num_theta_; ++i_theta)
      {
        int i_plane = grid.grid_to_linear(i_chord, i_height, i_theta);
        vnl_matrix<float>& image_cost_plane = image_cost[i_plane].as_ref();
        
        // retrieve the mirror cost plane
        int i_theta_mirror = i_theta - grid.num_theta_/2;
        int i_plane_mirror = grid.grid_to_linear(i_chord, i_height_mirror, i_theta_mirror);
        vnl_matrix<float>& mirror_cost_plane = image_cost[i_plane_mirror].as_ref();

        // Mirror the data
        image_cost_plane.copy_in(mirror_cost_plane.data_block());
      } // i_theta
    } // height
  } // chord  
  vcl_cout << "\n";
  return;
}


// ----------------------------------------------------------------------------
//: (Normalized) L^2 - Oriented Chamfer Matching of a shapelet
float dbsks_ocm_image_cost::
f(const dbsksp_shapelet_sptr& s, 
  bool include_front_arc,
  bool include_rear_arc,
  float ds)
{
  double total_cost = 0;
  double total_len = 0;

  vcl_vector<vgl_point_2d<double > > s_pts;
  vcl_vector<vgl_vector_2d<double > > s_tangents;
  this->compute_boundary_samples_uniform(s, ds, s_pts, s_tangents);

  
  float s_avg_cost = this->f(s_pts, s_tangents);
  double s_len = s->bnd_arc_left().length() + s->bnd_arc_right().length();

  total_cost += s_avg_cost * s_len;
  total_len += s_len;

  // check for front and rear arcs
  if (include_front_arc)
  {
    dbsksp_shapelet_sptr s_front = s->terminal_shapelet_front();
    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;
    this->compute_boundary_samples_uniform(s_front, ds, pts, tangents);

    float avg_cost = this->f(pts, tangents);

    total_cost += avg_cost * 2*s_front->bnd_arc(0).len();
    total_len += 2*s_front->bnd_arc(0).len();
  }

  if (include_rear_arc)
  {
    dbsksp_shapelet_sptr s_rear = s->terminal_shapelet_rear();
    vcl_vector<vgl_point_2d<double > > pts;
    vcl_vector<vgl_vector_2d<double > > tangents;
    this->compute_boundary_samples_uniform(s_rear, ds, pts, tangents);

    float avg_cost = this->f(pts, tangents);
    
    total_cost += avg_cost * 2* s_rear->bnd_arc(0).len();
    total_len += 2* s_rear->bnd_arc(0).len();
  }

  float image_cost = float(total_cost / total_len);
  return image_cost;
}







// ----------------------------------------------------------------------------
//: (Normalized) Oriented Chamfer Matching of a set of (point-tangent)s
float dbsks_ocm_image_cost::
f_L2(const vcl_vector<vgl_point_2d<double > >& pts,
     const vcl_vector<vgl_vector_2d<double > >& tangents) const
{
  // clip the values at 3 standard deviations ////////////////
  const float max_value = 9.0f;
  ////////////////////////////////////////////////////////////

  float sum = 0;
  int ni = this->dt_.ni();
  int nj = this->dt_.nj();


  // sum up the ocm cost at each (point-tangent) pair
  for (unsigned k=0; k<pts.size(); ++k) 
  {
    // pick the closest point
    int im_i = int(pts[k].x());
    int im_j = int(pts[k].y());
    if (im_i<0 || im_i>=ni || im_j<0 || im_j>= nj)
    {
      sum += max_value;
    }
    else
    {
      float mag_component = vnl_math_sqr(this->dt_(im_i, im_j) / this->sigma_distance_);

      // if the gradient is not definite, assume is well-aligned
      if (this->dt_grad_mag_(im_i, im_j) < 0.4)
      {
        sum += vnl_math_min(mag_component, max_value);
      }
      else
      {
        float grad_x = this->dt_grad_x_(im_i, im_j);
        float grad_y = this->dt_grad_y_(im_i, im_j);
        vgl_vector_2d<double > t0(-grad_y, grad_x);
        float theta = float(angle(t0, tangents[k]));

        theta = vnl_math_min(theta, float(vnl_math::pi - theta));
        float dir_component = vnl_math_sqr(theta / this->sigma_angle_);

        sum += vnl_math_min(mag_component + dir_component, max_value);
      }
    }
  }

  return sum / pts.size();
}


// -----------------------------------------------------------------------------
//: (Normalized) L^1 - Oriented Chamfer Matching of a set of (point-tangent)s
float dbsks_ocm_image_cost::
f_shotton_ocm(const vcl_vector<vgl_point_2d<double > >& pts,
              const vcl_vector<vgl_vector_2d<double > >& tangents) const
{
  if (pts.empty()) return 0;

  // clip the value. For exp(-x) distribution, x = 3 correspond to 95.55% of the values
  const float max_value = 3;
  ////////////////////////////////////////////////////////////

  float sum = 0;
  int ni = this->dt_.ni();
  int nj = this->dt_.nj();


  // sum up the ocm cost at each (point-tangent) pair
  for (unsigned k=0; k<pts.size(); ++k) 
  {
    // pick the closest point
    int im_i = int(pts[k].x());
    int im_j = int(pts[k].y());
    if (im_i<0 || im_i>=ni || im_j<0 || im_j>= nj)
    {
      sum += max_value;
    }
    else
    {
      //// chamfer component
      //float chamfer_component = 
      //  vnl_math_abs(this->dt_(im_i, im_j))/this->sigma_distance_;
      //
      //// clip the cost
      //chamfer_component = vnl_math_min(chamfer_component, max_value);


      float distance = vnl_math_abs(this->dt_(im_i, im_j));
      float chamfer_component = this->chamfer_cost_w_near_zero_tolerance(
        distance, this->sigma_distance_, this->tolerance_near_zero_, max_value);


      // orientation component
      float orient_component = max_value;

      // if the gradient is not definite, assume is well-aligned
      if (this->dt_grad_mag_(im_i, im_j) < 0.4)
      {
        orient_component = 0;
      }
      else
      {
        // tangent on the edges == direction of iso-contour on distance map
        float grad_x = this->dt_grad_x_(im_i, im_j);
        float grad_y = this->dt_grad_y_(im_i, im_j);
        vgl_vector_2d<double > t0(-grad_y, grad_x);
        
        // the angle difference should be between [0, pi/2]
        float theta = float(vnl_math_abs(angle(t0, tangents[k])));
        theta = vnl_math_min(theta, float(vnl_math::pi - theta));
        orient_component = float(theta / this->sigma_angle_);

        // clip the orientation component
        orient_component = vnl_math_min(orient_component, max_value);
      }

      float ocm = chamfer_component + orient_component;

      // clip the final cost
      ocm = vnl_math_min(ocm, max_value);

      // update overall sum
      sum += ocm;
    }
  }

  return sum / pts.size();
}


// ----------------------------------------------------------------------------
//: Compute distance transform of an edgemap
void dbsks_ocm_image_cost::
compute_dt(const vil_image_view<float >& edgemap, float edge_threshold,
           vil_image_view<float >& dt_edgemap)
{
  // threshold the edge map to generate a binary image
  vil_image_view<unsigned int > image_unsigned(edgemap.ni(), edgemap.nj());
    for (unsigned int i=0; i<edgemap.ni(); ++i)
    {
      for (unsigned int j=0; j<edgemap.nj(); ++j)
      {
        image_unsigned(i, j) = (edgemap(i, j) < edge_threshold) ? 0 : 255;
      }
    }

  // Compute distance transform of the image
  bil_edt_signed(image_unsigned, dt_edgemap);
  return;
}


// ----------------------------------------------------------------------------
//: Compute gradient of an image using Gaussian kernel
void dbsks_ocm_image_cost::
compute_gradient(vil_image_view<float >& dt,
                 vil_image_view<float >& dt_grad_x,
                 vil_image_view<float >& dt_grad_y,
                 vil_image_view<float >& dt_grad_mag)
{
  // Compute gradient of DT
  float sigma = 1.0f;
  int N = 0;
  dbdet_subpix_convolve_2d_sep(dt, dt_grad_x, 
    dbdet_Gx_kernel(sigma), float(), N);
  dbdet_subpix_convolve_2d_sep(dt, dt_grad_y, 
    dbdet_Gy_kernel(sigma), float(), N);

  //compute gradient magnitude
  dt_grad_mag.set_size(dt_grad_x.ni(), dt_grad_x.nj());

  //get the pointers to the memory chunks
  float *gx  =  dt_grad_x.top_left_ptr();
  float *gy  =  dt_grad_y.top_left_ptr();
  float *g_mag  =  dt_grad_mag.top_left_ptr();

  //compute the gradient magnitude
  for(unsigned long i=0; i<dt_grad_mag.size(); i++)
  {
    g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);
  }
  return;
}






//: Compute Chamfer Matching cost image from DT and its gradient
vil_image_view<float > dbsks_ocm_image_cost::
compute_chamfer_cost()
{
  float max_value = this->max_cost();

  unsigned ni = this->edgemap_.ni();
  unsigned nj = this->edgemap_.nj();
  assert(ni > 0 && nj > 0);

  // Allocate memory
  vil_image_view<float > chamfer_cost(ni, nj);

  // Compute chamfer cost for each pixel
  for (unsigned i =0; i < ni; ++i)
  {
    for (unsigned j =0; j <nj; ++j)
    {
      float distance = vnl_math_abs(this->dt_(i, j));
      
      //// chamfer component
      //float chamfer_component = distance /this->sigma_distance_;
      //
      //// clip the cost
      //chamfer_cost(i, j) = vnl_math_min(chamfer_component, max_value);    
      chamfer_cost(i, j) = this->chamfer_cost_w_near_zero_tolerance(
        distance, this->sigma_distance_, this->tolerance_near_zero_, max_value);
    }
  }
  
  return chamfer_cost;
}




//: transform function, tolerance when near edge
float dbsks_ocm_image_cost::
chamfer_cost_w_near_zero_tolerance(float distance, 
                                   float sigma_distance,
                                   float tolerance, float clip_cost_value) const
{
  if (distance < tolerance) return 0;

  ////
  //float x = distance - tolerance;
  //float a = sigma_distance*clip_cost_value - tolerance;
  //float y = 2*a * x*x / (a*a + x*x);

  // linearly interpolate
  float x = distance - tolerance;
  float clip_value = sigma_distance * clip_cost_value;
  float slope = clip_value  / (clip_value - tolerance);
  float y = x * slope;

  return vnl_math_min(y / sigma_distance, clip_cost_value);
}




//------------------------------------------------------------------------------
//: Compute point-tangent samples of the shapelet, given sampling rate "ds"
void dbsks_ocm_image_cost::
compute_boundary_samples_uniform(const dbsksp_shapelet_sptr& sh, double ds, 
                                 vcl_vector<vgl_point_2d<double > >& pts,
                                 vcl_vector<vgl_vector_2d<double > >& tangents)
{
  for (int i=0; i<2; ++i)
  {
    dbgl_circ_arc arc = sh->bnd_arc(i);
    // sample at the middle of the interval
    for (double s = ds/2; s<arc.len(); s += ds)
    {
      pts.push_back(arc.point_at_length(s));
      tangents.push_back(arc.tangent_at_length(s));
    }
  }
  return;
}








