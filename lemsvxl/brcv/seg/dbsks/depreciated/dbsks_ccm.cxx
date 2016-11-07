// This is file seg/dbsks/dbsks_ccm.cxx

//:
// \file

#include "dbsks_ccm.h"


#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <bil/algo/bil_edt.h>
#include <dbnl/dbnl_angle.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>



// ============================================================================
//  dbsks_ccm
// ============================================================================


//: Clear the memory
void dbsks_ccm::
clear()
{
  this->edge_strength_.clear();
  this->edge_orient_x_.clear();
  this->edge_orient_y_.clear();
  this->edge_angle_.clear();

  
  this->matched_edge_ix_.clear();
  this->matched_edge_iy_.clear();

  this->closest_edge_orient_x_.clear();
  this->closest_edge_orient_y_.clear();

  // Cost computation window
  this->roi_.empty();
  
  // Distance (Chamfer) cost
  this->chamfer_cost_.clear();

  // edge orientation cost
  this->edge_orient_cost_.clear();

  // contour orientation cost
  this->contour_orient_cost_.clear();

  return;
}


// -----------------------------------------------------------------------------
//: Compute edge strength at each pixel from an edgemap
bool dbsks_ccm::
set_edge_strength(const vil_image_view<float >& edgemap, 
                      float edge_val_threshold,
                      float max_edge_val)
{
  // preliminary checks
  if (!edgemap.top_left_ptr())
    return false;

  // We threshold the edges from below with "edge_val_threshold" and then normalized
  // with "max_edge_val"
  this->edge_strength_.set_size(edgemap.ni(), edgemap.nj());
  this->edge_strength_.fill(0);
  for (unsigned i =0; i < edgemap.ni(); ++i)
  {
    for (unsigned j =0; j < edgemap.nj(); ++j)
    {
      float s = edgemap(i, j);

      // threshold
      if (s < edge_val_threshold)
        continue;

      // normalize
      this->edge_strength_(i, j) = s / max_edge_val;
    }
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Set edge orientation from a matrix of edge angles
bool dbsks_ccm::
set_edge_orient(const vil_image_view<float >& edge_angle)
{
  if (!edge_angle.top_left_ptr())
    return false;
  this->edge_angle_.deep_copy(edge_angle);
  
  // compute x- and y-component of orientation vector
  unsigned ni = this->edge_angle_.ni();
  unsigned nj = this->edge_angle_.nj();
  this->edge_orient_x_.set_size(ni, nj);
  this->edge_orient_x_.fill(0);
  this->edge_orient_y_.set_size(ni, nj);
  this->edge_orient_y_.fill(0);
  for (unsigned i =0; i < ni; ++i)
  {
    for (unsigned j =0; j < nj; ++j)
    {
      this->edge_orient_x_(i, j) = vcl_cos(this->edge_angle_(i,j));
      this->edge_orient_y_(i, j) = vcl_sin(this->edge_angle_(i,j));
    }
  }

  return true;
}


// -----------------------------------------------------------------------------
//: Angle per orientation channel
double dbsks_ccm::
radians_per_channel() const
{
  return vnl_math::pi / this->nchannel_0topi_;
}



// ------------------------------------------------------------------------------
//: Set edge orientation by computing gradient of DT of binary (by thresholding) edgemap
bool dbsks_ccm::
set_closest_edge_orient(const vil_image_view<float >& edgemap, float threshold)
{
  unsigned ni = edgemap.ni();
  unsigned nj = edgemap.nj();

  // threshold edgemap to generate a binary edgemap
  vil_image_view<unsigned int > binary_edgemap(ni, nj);
  for (unsigned int i=0; i< ni; ++i)
  {
    for (unsigned int j=0; j< nj; ++j)
    {
      binary_edgemap(i, j) = (edgemap(i, j) < threshold) ? 0 : 255;
    }
  }

  // Compute distance transform of the binary edgemap
  vil_image_view<float > dt;
  bil_edt_signed(binary_edgemap, dt);

  // Compute gradient of DT
  float sigma = 1.0f;
  int N = 0;
  vil_image_view<float > dt_grad_x;
  vil_image_view<float > dt_grad_y;
  dbdet_subpix_convolve_2d_sep(dt, dt_grad_x, dbdet_Gx_kernel(sigma), float(), N);
  dbdet_subpix_convolve_2d_sep(dt, dt_grad_y, dbdet_Gy_kernel(sigma), float(), N);


  // Edge orientation is orthogonal to direction of gradient of DT
  this->closest_edge_orient_x_.set_size(ni, nj);
  this->closest_edge_orient_x_.set_size(ni, nj);
  
  for (unsigned int i=0; i< ni; ++i)
  {
    for (unsigned int j=0; j< nj; ++j)
    {
      vgl_vector_2d<float > n(dt_grad_x(i, j), dt_grad_y(i, j));
      normalize(n);
      this->closest_edge_orient_x_(i, j) = -n.y();
      this->closest_edge_orient_y_(i, j) = n.x();
    }
  }
  return true;
}



//: Compute cost components for the whole image
void dbsks_ccm::
compute(edge_correspondence_method method, bool precompute_contour_orient_cost)
{
  vgl_box_2d<int > roi(0, this->edge_strength_.ni(), 0, this->edge_strength_.nj());
  this->compute(roi, method, precompute_contour_orient_cost);
}


// ------------------------------------------------------------------------------
//: Compute cost components for a window
void dbsks_ccm::
compute(const vgl_box_2d<int >& roi, edge_correspondence_method method, 
        bool precompute_contour_orient_cost)
{
  this->roi_ = roi;
  this->precompute_contour_orient_cost_ = precompute_contour_orient_cost;

  
  if (method = dbsks_ccm::USE_CLOSEST_ORIENTED_EDGE)
  {
    // Correspondence considers both distance and orientation difference
    this->compute_ocm_cost_using_closest_oriented_edge();
  }
  else if (method == dbsks_ccm::USE_CLOSEST_EDGE)
  {
    // Correspondence consider only distance difference
    this->compute_ocm_cost_using_closest_edge();
  }
  else
  {
    vcl_cout << "\nERROR: unknown edge correspondence method.\n";
  }
  return;
}


// -----------------------------------------------------------------------------
//: cost of an oriented point, represented by its index
float dbsks_ccm::
f(const vcl_vector<int >& x, const vcl_vector<int >& y, const vcl_vector<int >& orient_channel)
{
  // need to have sufficient number of sample points to compute cost
  if (x.size() < 5)
    return 1.0f;

  // Chamfer and Edge-orient cost
  float total_chamfer = 0;
  float total_edge_orient = 0;
  unsigned npts = x.size();
  for (unsigned k =0; k < npts; ++k)
  {
    int ix = x[k];
    int iy = y[k];
    int ichannel = orient_channel[k];

    float chamfer_cost = 1.0f;
    float edge_orient_cost = 1.0f;

    if (this->edge_strength_.in_range(ix, iy))
    {
      // chamfer cost
      if (!this->chamfer_cost_(ix, iy).empty())
      {
        chamfer_cost = this->chamfer_cost_(ix, iy)[ichannel];
      }

      // edge orient cost
      if (!this->edge_orient_cost_(ix, iy).empty())
      {
        edge_orient_cost = this->edge_orient_cost_(ix, iy)[ichannel];
      }
    }
    total_chamfer += chamfer_cost;
    total_edge_orient += edge_orient_cost;
  } // k
  float avg_chamfer = total_chamfer / npts;
  float avg_edge_orient = total_edge_orient / npts;


  // Contour-orientation cost
  // Get contour orient cost whenever they're readily available
  float total_contour_orient = 0;
  float avg_contour_orient = 1.0f;
  if (this->precompute_contour_orient_cost_)
  {
    for (unsigned k =0; k < (npts-1); ++k)
    {
      int ix = x[k];
      int iy = y[k];
      int ichannel = orient_channel[k];

      float contour_orient_cost = 1.0f;
      if (this->edge_strength_.in_range(ix, iy))
      {
        // contour orient cost
        if (!this->contour_orient_cost_(ix, iy).empty())
        {
          contour_orient_cost = this->contour_orient_cost_(ix, iy)[ichannel];
        }
      }
      total_contour_orient += contour_orient_cost;
    } // k
    avg_contour_orient = total_contour_orient / (npts-1);
  }
  else
  {
    // computate contour-orientation cost directly from given pts
    for (unsigned k =0; k < (npts-2); ++k)
    {
      // take a jump of two sample points for stability
      total_contour_orient += this->contour_orient_cost(x[k], y[k], orient_channel[k],
        x[k+2], y[k+2], orient_channel[k+2]);
    }
    avg_contour_orient = total_contour_orient / (npts-2);
  }

  // final cost is a linear combination of the three components
  float ccm_cost = (1-this->lambda_-this->gamma_) * avg_chamfer  +
    this->lambda_* avg_edge_orient + 
    this->gamma_ * avg_contour_orient;

  //ccm_cost = vnl_math_min(1.0f, ccm_cost);  
  return ccm_cost;
}




// -----------------------------------------------------------------------------
//: Chamfer cost [0, 1] given distance of edge to query point
float dbsks_ccm::
chamfer_cost(double distance) const
{
   double cost = vnl_math_max(0.0, distance-this->tol_near_zero_) / this->distance_threshold_;
   return float(vnl_math_min(cost, 1.0)); // upper-bound = 1
}



// -----------------------------------------------------------------------------
//: Edge orientation cost [0, 1] given difference in orientation between the edge point
// and the query point
float dbsks_ccm::
edge_orient_cost(double signed_angle_difference) const
{
  // modulo the angle difference to [0, pi]
  double diff = vcl_fmod(signed_angle_difference, vnl_math::pi);
  if (diff < 0)
    diff += vnl_math::pi;

  // now convert this angle difference to [0, pi/2]
  diff = vnl_math_min(diff, vnl_math::pi - diff);
           
  // due to discretization, we allow a "tolerance" zone equal to half the size of an angle channel
  double tol = (vnl_math::pi / this->nchannel_0topi_) / 2;
  double cost = vnl_math_max(diff-tol, 0.0) / vnl_math::pi_over_4;

  // clip the cost at 2.0
  return float(vnl_math_min(cost, 1.0));
}


// -----------------------------------------------------------------------------
//: Contour orientation cost
float dbsks_ccm::
contour_orient_cost(int cur_x, int cur_y, int cur_orient_bin,
    int next_x, int next_y, int next_orient_bin)
{
  if (!this->edge_strength_.in_range(cur_x, cur_y))
  {
    return 1.0f;
  }

  if (this->matched_edge_ix_(cur_x, cur_y).empty())
  {
    return 1.0f;
  }

  // coordinate of corresponding edge
  int edge_x = this->matched_edge_ix_(cur_x, cur_y)[cur_orient_bin];
  int edge_y = this->matched_edge_iy_(cur_x, cur_y)[cur_orient_bin];
  if (!this->edge_strength_.in_range(edge_x, edge_y))
  {
    return 1.0f;
  }
  
  // if this point at the image border, shouldn't impose contour cost
  if (!this->edge_strength_.in_range(next_x, next_y))
  {
    return 0.0f;
  }

  // Best-matched edge of next point
  if (this->matched_edge_ix_(next_x, next_y).empty())
  {
    return 1.0f;
  }

  int next_edge_x = this->matched_edge_ix_(next_x, next_y)[next_orient_bin];
  int next_edge_y = this->matched_edge_iy_(next_x, next_y)[next_orient_bin];

  if (!this->edge_strength_.in_range(next_edge_x, next_edge_y))
  {
    return 1.0f;
  }

  double angle_diff = 0;
  vgl_vector_2d<double > v_query(next_x-cur_x, next_y-cur_y);
  if (next_edge_x == edge_x && next_edge_y == edge_y)
  {
    // use the edge orientation as the contour orientation
    double edge_contour_angle = this->edge_angle_(edge_x, edge_y);
    vgl_vector_2d<double > v_edge(vcl_cos(edge_contour_angle), vcl_sin(edge_contour_angle));
    
    angle_diff = vnl_math_abs(signed_angle(v_query, v_edge));

    // since edge angle has the ambiguity of +-pi, the angle difference cannot be more than pi/2
    // there are two possible values, differeing by pi
    angle_diff = vnl_math_min(angle_diff, vnl_math::pi - angle_diff);
  }
  else
  {
    // Compute orientation of "contour" along the edges
    vgl_vector_2d<double > v_edge(next_edge_x-edge_x, next_edge_y- edge_y);
    angle_diff = vnl_math_abs(signed_angle(v_query, v_edge));
  }
  assert(angle_diff >= 0); // make sure nothing weird happens
           
  // due to discretization, we allow a "tolerance" zone equal
  double tol = vnl_math::pi/8;
  double angle_cost = vnl_math_max(angle_diff-tol, 0.0) / vnl_math::pi_over_4;

  // clip the cost at 2.0
  return float(vnl_math_min(angle_cost, 2.0));
}





// -----------------------------------------------------------------------------
//: Compute OCM cost as cost to closest edge
bool dbsks_ccm::
compute_ocm_cost_using_closest_edge()
{
  float neighborhood_radius = 3.0f; // \todo should be set to sampling rate of contour

  int ni = this->edge_strength_.ni();
  int nj = this->edge_strength_.nj();
  int nchannel = this->nchannel_0topi_; // covering [0, pi]
  int radius = (int) (this->distance_threshold_+this->tol_near_zero_);

  // size of each orientation channel
  double radians_per_channel = vnl_math::pi / nchannel;

  // pre-compute a neighborhood distance map
  vnl_matrix<float > dist_map(2*radius+1, 2*radius+1, 0);
  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      dist_map(i+radius, j+radius) = vcl_sqrt(float(i*i + j*j));
    }
  }

  //>>> Locate best-matched edges
  // these two matrices hold the (x,y) coordinate of the best-matched edge
  // The current implementation uses closest (and sufficiently strong) edge

  vnl_matrix<int > matched_edge_x(ni, nj, -1);
  vnl_matrix<int > matched_edge_y(ni, nj, -1);
  vnl_matrix<float > matched_edge_dist(ni, nj, vnl_numeric_traits<float >::maxval);

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

      // find the closest edge
      float closest_edge_dist = vnl_numeric_traits<float >::maxval;
      int closest_edge_x = -1;
      int closest_edge_y = -1;
      for (int wx = wmin_x; wx <= wmax_x; ++wx)
      {
        for (int wy = wmin_y; wy <= wmax_y; ++wy)
        {
          if (this->edge_strength_(wx, wy) <= 0.0001)
          { 
            continue;
          }

          // distance to point
          float dist = dist_map(wx-i+radius, wy-j+radius);

          // ignore points that are too far away
          if (dist > (this->distance_threshold_+this->tol_near_zero_))
            continue;

          if (dist < closest_edge_dist)
          {
            closest_edge_dist = dist;
            closest_edge_x = wx;
            closest_edge_y = wy;
          }
        }
      }

      // move on to next point if there is no edge in the neighborhood
      if (closest_edge_x < 0 || closest_edge_y <0)
        continue;

      // record the closest edge
      matched_edge_x(i, j) = closest_edge_x;
      matched_edge_y(i, j) = closest_edge_y;
      matched_edge_dist(i, j) = closest_edge_dist;
    }
  }


  // Compute the cost components for oriented points on image
  this->chamfer_cost_.resize(ni, nj);
  
  // edge orientation cost
  this->edge_orient_cost_.resize(ni, nj);

  // contour orientation cost
  this->contour_orient_cost_.resize(ni, nj);
  
  // location of matched edge points
  this->matched_edge_ix_.resize(ni, nj);
  this->matched_edge_iy_.resize(ni, nj);

  // Clean up
  for (int i =0; i < ni; ++i)
  {
    for (int j =0; j <nj; ++j)
    {
      this->chamfer_cost_(i, j).clear();
      this->edge_orient_cost_(i, j).clear();
      this->contour_orient_cost_(i, j).clear();
      this->matched_edge_ix_(i, j).clear();
      this->matched_edge_iy_(i, j).clear();
    }
  }

  //>>> Compute cost at each location and orientation using compute best-matched edges
  for (int i = this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j = this->roi_.min_y(); j < this->roi_.max_y(); ++j)
    {
      int edge_x = matched_edge_x(i, j);
      int edge_y = matched_edge_y(i, j);
      float edge_dist = matched_edge_dist(i, j);

      // move on to next point if there is no edge in the neighborhood
      if (edge_x < 0 || edge_y <0)
      {
        this->chamfer_cost_(i, j).clear();
        this->edge_orient_cost_(i, j).clear();
        this->contour_orient_cost_(i, j).clear();
        this->matched_edge_ix_(i, j).clear();
        this->matched_edge_iy_(i, j).clear();
        continue;
      }

      // with this "closest edge" scheme, position of matched edge is the same
      // regardless of orientation of the query point
      this->matched_edge_ix_(i, j).set_size(2*nchannel);
      this->matched_edge_ix_(i, j).fill(edge_x);

      this->matched_edge_iy_(i, j).set_size(2*nchannel);
      this->matched_edge_iy_(i, j).fill(edge_y);

      // save chamfer cost
      this->chamfer_cost_(i, j).set_size(2*nchannel);
      this->chamfer_cost_(i, j).fill(this->chamfer_cost(edge_dist)); 

      // allocate space for edge_orientation and contour_orientation cost
      this->edge_orient_cost_(i, j).set_size(2*nchannel);
      this->contour_orient_cost_(i, j).set_size(2*nchannel);

      // iterate thru all possible angles of the query point
      for (int channel =0; channel < 2*nchannel; ++channel)
      {
        double angle = channel * radians_per_channel;
        {
          //>> edge orientation cost
          double edge_dir = this->edge_angle_(edge_x, edge_y);
          this->edge_orient_cost_(i, j)[channel] = this->edge_orient_cost(angle-edge_dir);
        } // edge-orient cost

        //>> contour orientation cost
        {
          // Coordinate of next point along contour's tangent
          int next_point_x = vnl_math_rnd(i + vcl_cos(angle) * neighborhood_radius);
          int next_point_y = vnl_math_rnd(j + vcl_sin(angle) * neighborhood_radius);

          //
          if (next_point_x < 0 || next_point_x >= ni || next_point_y < 0 || next_point_y >= nj)
          {
            this->contour_orient_cost_(i, j)[channel] = 0.0f;
            continue;
          }

          // Best-matched edge of next point
          int next_edge_x = matched_edge_x(next_point_x, next_point_y);
          int next_edge_y = matched_edge_y(next_point_x, next_point_y);
          float next_edge_dist = matched_edge_dist(next_point_x, next_point_y);

          if (next_edge_x < 0 || next_edge_y < 0)
          {
            this->contour_orient_cost_(i, j)[channel] = 1.0f;
            continue;
          }

          // if the two edge points coincide then two contour points match to the same edge point
          // we assign maximum cost for this case
          double contour_dir = 0;
          if (next_edge_x == edge_x && next_edge_y == edge_y)
          {
            contour_dir = vcl_atan2(double(next_edge_x-next_point_x), -double(next_edge_y-next_point_y));
          }
          else
          {
            // Compute orientation of "contour" on the edges
            contour_dir = vcl_atan2(double(next_edge_y-edge_y), double(next_edge_x-edge_x));
          }

          // normalize angle difference to [0, pi]
          double contour_angle_diff = vcl_fmod(angle - contour_dir, vnl_math::pi);
          if (contour_angle_diff < 0)
            contour_angle_diff += vnl_math::pi;

          // now convert this angle difference to [0, pi/2]
          contour_angle_diff = vnl_math_min(contour_angle_diff, vnl_math::pi - contour_angle_diff);

          // we allow a "tolerance" zone for angle difference to take into account error due to discretization
          float contour_orient_cost = float( (contour_angle_diff-radians_per_channel/2) /vnl_math::pi_over_4);
          contour_orient_cost = vnl_math_max(contour_orient_cost, 0.0f);

          this->contour_orient_cost_(i, j)[channel] = vnl_math_min(contour_orient_cost, 1.0f);
        } // contour-orient cost
      }
    }
  }


  return true;
}











// -----------------------------------------------------------------------------
//: Compute CCM cost as cost to edge closest in both distance and orientation
bool dbsks_ccm::
compute_ocm_cost_using_closest_oriented_edge()
{
  // parameter needed for contour cost
  float neighborhood_radius = 3.0f; // \todo should be set to sampling rate of contour

  int ni = this->edge_strength_.ni();
  int nj = this->edge_strength_.nj();
  int nchannel = this->nchannel_0topi_; // covering [0, pi]
  int radius = (int) (this->distance_threshold_+this->tol_near_zero_);

  // pre-compute a neighborhood distance map
  vnl_matrix<float > dist_map(2*radius+1, 2*radius+1, 0);
  for (int i = -radius; i <= radius; ++i)
  {
    for (int j = -radius; j <= radius; ++j)
    {
      dist_map(i+radius, j+radius) = vcl_sqrt(float(i*i + j*j));
    }
  }


  
  // Prepare storage
  this->matched_edge_ix_.resize(ni, nj);
  this->matched_edge_iy_.resize(ni, nj);
  this->chamfer_cost_.resize(ni, nj);
  this->edge_orient_cost_.resize(ni, nj);
  this->contour_orient_cost_.resize(ni, nj);

  // clean up
  for (int i =0; i < ni; ++i)
  {
    for (int j =0; j < nj; ++j)
    {
      this->matched_edge_ix_(i, j).clear();
      this->matched_edge_iy_(i, j).clear();
      this->chamfer_cost_(i, j).clear();
      this->edge_orient_cost_(i, j).clear();
      this->contour_orient_cost_(i, j).clear();
    }
  }
  

  //>>> Locate best-matched edges
  // iterate thru every point in the image
  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    // x-dimension of local window
    int wmin_x = vnl_math_max(i-radius, 0);
    int wmax_x = vnl_math_min(i+radius, ni-1);
    
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // y-dimension of local window
      int wmin_y = vnl_math_max(j-radius, 0);
      int wmax_y = vnl_math_min(j+radius, nj-1);

      // Collect locations of all the edges in the neighborhood
      vcl_vector<vgl_point_2d<int > > neighborhood_edges;
      neighborhood_edges.reserve((wmax_x-wmin_x+1)*(wmax_y-wmin_y+1));
      for (int wx = wmin_x; wx <= wmax_x; ++wx)
      {
        for (int wy = wmin_y; wy <= wmax_y; ++wy)
        {
          if (this->edge_strength_(wx, wy) > 0.0001) //>> edge_strength == 0 means no edge
          { 
            neighborhood_edges.push_back(vgl_point_2d<int >(wx, wy));
          }
        }
      }

      // if there is no edge in the neighborhood then there is no corresponding edges
      if (neighborhood_edges.empty())
      {
        this->matched_edge_ix_(i, j).clear(); // save storage space
        this->matched_edge_iy_(i, j).clear();
      }
      else
      {
        // prepare space to store location of corresponding edges
        this->matched_edge_ix_(i, j).set_size(2*this->nchannel_0topi_);
        this->matched_edge_ix_(i, j).fill(-1); // out of range location

        this->matched_edge_iy_(i, j).set_size(2*this->nchannel_0topi_);
        this->matched_edge_iy_(i, j).fill(-1); // out of range location

        // Iterate thru all possible orientations and find the best-matched edge point for each
        // combination (x, y, orientation)
        for (int p = 0; p < 2*this->nchannel_0topi_; ++p)
        {
          double angle = p * this->radians_per_channel();
          vgl_vector_2d<double > tangent(vcl_cos(angle), vcl_sin(angle));

          // Seach among all edges in the neighborhood and minimize Oriented Chamfer Matching cost
          float min_ocm_cost = 1.0f;
          int min_ocm_idx = -1;
          for (unsigned i_edge = 0; i_edge < neighborhood_edges.size(); ++i_edge)
          {
            // edge properties
            vgl_point_2d<int > edge = neighborhood_edges[i_edge];

            // limit the search strictly to edges that are along the "vertical" direction
            // wrt to the local coordinate system of the query (oriented) point
            vgl_vector_2d<double > query2edge(edge.x() - i, edge.y()-j);
            double local_dx = dot_product(tangent, query2edge);

            if (vnl_math_abs(local_dx) > 3.0) // vnl_math_max(2.0f, this->tol_near_zero_))
            {
              continue;
            }

            double edge_angle = this->edge_angle_(edge.x(), edge.y());

            // cost components
            float chamfer_cost = this->chamfer_cost(dist_map(edge.x()-i+radius, edge.y()-j+radius));
            float edge_orient_cost = this->edge_orient_cost(angle-edge_angle);

            // we avoid all edges that are too far away or too different in orientation
            if (chamfer_cost >= 1.0 || edge_orient_cost >= 1.0f)
              continue;

            // total oriented chamfer cost
            float ocm_cost = (1-this->lambda_-this->gamma_)*chamfer_cost + this->lambda_*edge_orient_cost;

            if (ocm_cost < min_ocm_cost)
            {
              min_ocm_cost = ocm_cost;
              min_ocm_idx = i_edge;
            }
          }

          // if a corresponding edge is found, assign it
          if (min_ocm_idx >= 0)
          {
            vgl_point_2d<int > matched_edge = neighborhood_edges[min_ocm_idx];
            this->matched_edge_ix_(i, j)[p] = matched_edge.x();
            this->matched_edge_iy_(i, j)[p] = matched_edge.y();
          }
        } // p
      } // if there are some edges in the neighborhood
    } // j
  } // i

  //>> Compute the three cost components (Chamfer, edge orient, and contour orient)
  
  // Compute cost at each location and orientation
  int nchannel_0to2pi = this->nchannel_0topi_ * 2;

  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // cost vectors when query point is at (i, j)
      vnl_vector<float >& f_chamfer = this->chamfer_cost_(i, j);
      vnl_vector<float >& f_edge_orient = this->edge_orient_cost_(i, j);
      vnl_vector<float >& f_contour_orient = this->contour_orient_cost_(i, j);

      // location of the corresponding edge points
      vnl_vector<int >& edge_ix = this->matched_edge_ix_(i, j);
      vnl_vector<int >& edge_iy = this->matched_edge_iy_(i, j);

      // if there is no corresponding edges then we can't compute the cost components
      if (edge_ix.empty() || edge_iy.empty())
      {
        f_chamfer.clear();
        f_edge_orient.clear();
        f_contour_orient.clear();
        continue;
      }

      assert(edge_ix.size() == nchannel_0to2pi);
      assert(edge_iy.size() == nchannel_0to2pi);

      // compute chamfer and edge orient cost
      f_chamfer.set_size(nchannel_0to2pi);
      f_chamfer.fill(1.0f);;

      f_edge_orient.set_size(nchannel_0to2pi);
      f_edge_orient.fill(1.0f);
      
      vgl_point_2d<int > query_pt(i, j);
      for (int p =0; p < nchannel_0to2pi; ++p)
      {
        double query_angle = p * this->radians_per_channel();

        // retrieve the query point's corresponding edge
        vgl_point_2d<int > edge_pt(edge_ix(p), edge_iy(p));
        if (!this->edge_strength_.in_range(edge_pt.x(), edge_pt.y()))
        {
          f_chamfer(p) = 1.0f;
          f_edge_orient(p) = 1.0f;
          continue;
        }

        // compute the cost components
        // chamfer cost depends only the distance between edge and query point
        f_chamfer(p) = this->chamfer_cost(vgl_distance(query_pt, edge_pt));

        // edge orientation cost
        double edge_angle = this->edge_angle_(edge_pt.x(), edge_pt.y());
        f_edge_orient(p) = this->edge_orient_cost(query_angle - edge_angle);
      } // p

      // Only compute contour orientation cost if flag is on to save memory and time
      if (this->precompute_contour_orient_cost_)
      {
        // compute contour orientation cost
        f_contour_orient.set_size(nchannel_0to2pi);
        f_contour_orient.fill(1.0f);
        for (int p =0; p < nchannel_0to2pi; ++p)
        {
          double query_angle = p * this->radians_per_channel();

          // Coordinate of next point along contour's tangent
          int next_point_x = vnl_math_rnd(i + vcl_cos(query_angle) * neighborhood_radius);
          int next_point_y = vnl_math_rnd(j + vcl_sin(query_angle) * neighborhood_radius);

          // assuming orientation doesn't change
          // \todo alternatively, we can average the cost for various next_point_p,
          // e.g., p, p-1, p+1. For simplicity, we are NOT doing it here.
          int next_point_p = p; 

          f_contour_orient(p) = this->contour_orient_cost(i, j, p, 
            next_point_x, next_point_y, next_point_p);
        } // p
      } // if precompute_contour_orient_cost
    } // j
  } // i
  return true;
}














