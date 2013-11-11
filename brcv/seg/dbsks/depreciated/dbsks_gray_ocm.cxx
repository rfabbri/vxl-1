// This is file seg/dbsks/dbsks_gray_ocm.cxx

//:
// \file

#include "dbsks_gray_ocm.h"


#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <bil/algo/bil_edt.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_convert.h>

#include <dbsks/dbsks_shotton_ocm.h>

// ============================================================================
//  dbsks_gray_ocm
// ============================================================================


// ------------------------------------------------------------------------------
//: Compute chamfer cost for the whole image
void dbsks_gray_ocm::
compute()
{
  // Compute gradient maps of the image
  this->compute_gradient();


  // consider remove this 
  ///////////////////////////////////////////////////////////////////////////
  //: Compute distance transform of the edge map 
  this->compute_dt(this->edgemap_, this->edge_val_threshold_, this->dt_);
  
  // Compute gradient maps of the DT
  this->compute_gradient(this->dt_, 
    this->dt_grad_x_, this->dt_grad_y_, this->dt_grad_mag_);
  ///////////////////////////////////////////////////////////////////////////


  // Compute OCM using brute-force approach
  //this->compute_gray_ocm_brute_force();
  this->compute_gray_ocm_brute_force_closest_edge();
}




// -----------------------------------------------------------------------------
//: cost of an oriented point, represented by its index
float dbsks_gray_ocm::
f(int i, int j, int orient_channel)
{
  if (!this->edgemap_.in_range(i, j) || orient_channel<0) 
  {
    return 1.0f;
  }

  // just in case it falls to other half of the circle
  orient_channel %= this->nchannel_;

  // final cost has been precomputed 
  return this->ocm_cost_[i][j][orient_channel];
}




// -----------------------------------------------------------------------------
//: cost of an oriented point, represented by its index
float dbsks_gray_ocm::
f(vcl_vector<int >& x, vcl_vector<int >& y, vcl_vector<int >& orient_channel)
{
  if (x.empty())
    return 1.0f;

  float sum_cost = 0;
  unsigned npts = x.size();
  for (unsigned k =0; k != npts; ++k)
  {
    sum_cost += this->f(x[k], y[k], orient_channel[k]);
  }

  return sum_cost / npts;
}


// -----------------------------------------------------------------------------
//: Compute gradient of an image using Gaussian kernel
void dbsks_gray_ocm::
compute_gradient()
{
  //convert to grayscale
  vil_image_view<float > greyscale_view;
  if(this->image_.nplanes() == 3)
  {
    vil_convert_planes_to_grey(this->image_, greyscale_view);
  }
  else if (this->image_.nplanes() == 1)
  {
    vil_convert_cast(this->image_, greyscale_view);
  }

  // Compute gradient of source image
  float sigma = 1.0f;
  int N = 0;
  dbdet_subpix_convolve_2d_sep(greyscale_view, this->grad_x_, 
    dbdet_Gx_kernel(sigma), float(), N);
  dbdet_subpix_convolve_2d_sep(greyscale_view, this->grad_y_, 
    dbdet_Gy_kernel(sigma), float(), N);

  //compute gradient magnitude
  this->grad_mag_.set_size(this->grad_x_.ni(), this->grad_x_.nj());

  //get the pointers to the memory chunks
  float *gx  =  this->grad_x_.top_left_ptr();
  float *gy  =  this->grad_y_.top_left_ptr();
  float *g_mag  =  this->grad_mag_.top_left_ptr();

  //compute the gradient magnitude
  for(unsigned long i=0; i< this->grad_mag_.size(); i++)
  {
    g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);
  }
  return;
}



// -----------------------------------------------------------------------------
//: Compute gray OCM cost by brute-force checking every pixel in the neighborhood
void dbsks_gray_ocm::
compute_gray_ocm_brute_force()
{
  int ni = this->edgemap_.ni();
  int nj = this->edgemap_.nj();
  int nchannel = this->nchannel_; // covering [0, pi]
  int radius = (int) (this->distance_threshold_+this->tol_near_zero_);

  // size of each orientation channel
  double radians_per_channel = vnl_math::pi / nchannel;

  // pre-compute a neighborhood distance map
  vnl_matrix<float > dist_map(2*radius+1, 2*radius+1, 0);
  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      dist_map(i+radius, j+radius) = 
        vnl_math_max(0.0f, vcl_sqrt(float(i*i + j*j))-this->tol_near_zero_);
    }
  }

  // Allocate memory and set default value
  this->ocm_cost_.resize(ni, nj, nchannel);
  this->ocm_cost_.fill(1.0f);

  // Compute chamfer cost for each pixel
  for (int i =0; i < ni; ++i)
  {
    // x-dimension of local window
    int wmin_x = vnl_math_max(i-radius, 0);
    int wmax_x = vnl_math_min(i+radius, ni-1);
    
    for (int j =0; j <nj; ++j)
    {
      int wmin_y = vnl_math_max(j-radius, 0);
      int wmax_y = vnl_math_min(j+radius, nj-1);

      for (int channel =0; channel < nchannel; ++channel)
      {
        double angle = channel * radians_per_channel;

        // check every point in the neighborhood
        float min_cost = 1.0f;
        for (int wx = wmin_x; wx <= wmax_x; ++wx)
        {
          for (int wy = wmin_y; wy <= wmax_y; ++wy)
          {
            // distance to point
            float dist = dist_map(wx-i+radius, wy-j+radius);

            // ignore points that are too far away
            if (dist > this->distance_threshold_)
              continue;

            // ignore edges that are too weak
            if (this->edgemap_(wx, wy) < this->edge_val_threshold_)
              continue;

            // >>>chamfer cost

            // option 1 :make it dependent on edge strength
            //// edge strength (normalized to [0, 1])
            //// \todo smooth this a little bit
            //float edge_strength = this->edgemap_(wx, wy) / this->max_edge_val_;

            //// transform it with a sigmoid-like function
            //// f(t) = sqrt(a+1) t / sqrt(1 + a*t*t), a = sigmoid_param_a_;
            //float a = 5;
            //float t = edge_strength;
            //edge_strength = vcl_sqrt(a+1)*t / vcl_sqrt(1 + a*t*t);

            // option 2: make it binary
            float edge_strength = 1.0f;

            // chamfer cost starts at (1-edge_strength) when the point of interest is right on the edge point
            // it increases linearly as it moves away from the edge point and reaches 1 when it is distance threshold
            float slope = edge_strength / this->distance_threshold_;
            float chamfer_cost = (1-edge_strength) + slope*dist;
            chamfer_cost = vnl_math_min(chamfer_cost, 1.0f); // upper-bound = 1

            // >>>orientation cost

            // we compute edge direction as orthogonal to gradient direction
            float grad_x = this->grad_x_(wx, wy);
            float grad_y = this->grad_y_(wx, wy);
            double edge_dir = vcl_atan2(grad_x, -grad_y);

            // modulo the angle difference to [0, pi]
            double angle_diff = vcl_fmod(angle - edge_dir, vnl_math::pi);
            if (angle_diff < 0)
              angle_diff += vnl_math::pi;

            // now convert this angle difference to [0, pi/2]
            angle_diff = vnl_math_min(angle_diff, vnl_math::pi - angle_diff);

            // normalize this cost to [0, 1]
            float orient_cost = float(angle_diff/vnl_math::pi_over_2);

            // >>> the final cost is a linear sum of chamfer cost and orientation cost
            float total_cost = (1-this->lambda_)*chamfer_cost + this->lambda_*orient_cost;

            // >>> compare with the current min
            min_cost = vnl_math_min(total_cost, min_cost);
          }
        }

        this->ocm_cost_[i][j][channel] = min_cost;
      }
    }
  }

  return;
}






// -----------------------------------------------------------------------------
//: Compute gray OCM cost by brute-force checking every pixel in the neighborhood
void dbsks_gray_ocm::
compute_gray_ocm_brute_force_closest_edge()
{

  int ni = this->edgemap_.ni();
  int nj = this->edgemap_.nj();
  int nchannel = this->nchannel_; // covering [0, pi]
  int radius = (int) (this->distance_threshold_+this->tol_near_zero_);

  // size of each orientation channel
  double radians_per_channel = vnl_math::pi / nchannel;

  // pre-compute a neighborhood distance map
  vnl_matrix<float > dist_map(2*radius+1, 2*radius+1, 0);
  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      dist_map(i+radius, j+radius) = 
        vnl_math_max(0.0f, vcl_sqrt(float(i*i + j*j))-this->tol_near_zero_);
    }
  }

  // Allocate memory and set default value
  this->ocm_cost_.resize(ni, nj, nchannel);
  this->ocm_cost_.fill(1.0f);

  // Compute chamfer cost for each pixel
  for (int i =0; i < ni; ++i)
  {
    // x-dimension of local window
    int wmin_x = vnl_math_max(i-radius, 0);
    int wmax_x = vnl_math_min(i+radius, ni-1);
    
    for (int j =0; j <nj; ++j)
    {
      int wmin_y = vnl_math_max(j-radius, 0);
      int wmax_y = vnl_math_min(j+radius, nj-1);

      for (int channel =0; channel < nchannel; ++channel)
      {
        double angle = channel * radians_per_channel;

        // find the closest edge
        float closest_edge_dist = 10.0f*radius;
        int closest_edge_wx = -1;
        int closest_edge_wy = -1;
        for (int wx = wmin_x; wx <= wmax_x; ++wx)
        {
          for (int wy = wmin_y; wy <= wmax_y; ++wy)
          {
            // ignore edges that are too weak
            if (this->edgemap_(wx, wy) < this->edge_val_threshold_)
              continue;

            // distance to point
            float dist = dist_map(wx-i+radius, wy-j+radius);

            // ignore points that are too far away
            if (dist > this->distance_threshold_)
              continue;

            if (dist < closest_edge_dist)
            {
              closest_edge_dist = dist;
              closest_edge_wx = wx;
              closest_edge_wy = wy;
            }
          }
        }


        // compute gray-value oriented chamfer matching
        if (closest_edge_wx >= 0 && closest_edge_wy >= 0)
        {
          //// option 1 :make it dependent on edge strength
          //// edge strength (normalized to [0, 1])
          //float edge_strength = this->edgemap_(closest_edge_wx, closest_edge_wy) / this->max_edge_val_;

          //// transform it with a sigmoid-like function
          //// f(t) = sqrt(a+1) t / sqrt(1 + a*t*t), a = sigmoid_param_a_;
          //float a = 5;
          //float t = edge_strength;
          //edge_strength = vcl_sqrt(a+1)*t / vcl_sqrt(1 + a*t*t);
          
          // option 2: make it binary
          float edge_strength = 1.0f;



          // chamfer cost starts at (1-edge_strength) when the point of interest is right on the edge point
          // it increases linearly as it moves away from the edge point and reaches 1 when it is distance threshold
          float slope = edge_strength / this->distance_threshold_;
          float chamfer_cost = (1-edge_strength) + slope * closest_edge_dist;
          chamfer_cost = vnl_math_min(chamfer_cost, 1.0f); // upper-bound = 1

          // >>>orientation cost


          //// option 1: get edge orientation from gradient map of image
          //// we compute edge direction as orthogonal to gradient direction
          //float grad_x = this->grad_x_(closest_edge_wx, closest_edge_wy);
          //float grad_y = this->grad_y_(closest_edge_wx, closest_edge_wy);
          //double edge_dir = vcl_atan2(grad_x, -grad_y);

          //// copied from shotton_ocm for testing purpose only //////////////////////
          // option 2: get orientation from gradient map of DT of edges
          // tangent on the edges == direction of iso-contour on distance map
          float grad_x = this->dt_grad_x_(i, j);
          float grad_y = this->dt_grad_y_(i, j);
          double edge_dir = vcl_atan2(grad_x, -grad_y);

          ///////////////////////////////////////////////////////////////////////////

          // modulo the angle difference to [0, pi]
          double angle_diff = vcl_fmod(angle - edge_dir, vnl_math::pi);
          if (angle_diff < 0)
            angle_diff += vnl_math::pi;

          // now convert this angle difference to [0, pi/2]
          angle_diff = vnl_math_min(angle_diff, vnl_math::pi - angle_diff);

          // normalize this cost to [0, 1]
          float orient_cost = float(angle_diff/vnl_math::pi_over_2);

          // >>> the final cost is a linear sum of chamfer cost and orientation cost
          float total_cost = (1-this->lambda_)*chamfer_cost + this->lambda_*orient_cost;

          // update the cost grid
          this->ocm_cost_[i][j][channel] = total_cost;
        }
      }
    }
  }

  return;

}
















// ----------------------------------------------------------------------------
//: Compute distance transform of an edgemap
void dbsks_gray_ocm::
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

  // Compute distance transform of the edgemap
  bil_edt_signed(image_unsigned, dt_edgemap);
  return;
}



// ----------------------------------------------------------------------------
//: Compute gradient of an image using Gaussian kernel
void dbsks_gray_ocm::
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







