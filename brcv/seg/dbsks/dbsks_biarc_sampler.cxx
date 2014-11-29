// This is file seg/dbsks/dbsks_biarc_sampler.cxx

//:
// \file

#include "dbsks_biarc_sampler.h"

#include <dbnl/dbnl_angle.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vcl_iostream.h>




// =============================================================================
// dbsks_biarc_sampler_params
// =============================================================================
//: one type of parameter set to construct a xnode grid
void dbsks_biarc_sampler_params::set_to_default_values()
{
  // max dx = 64
  this->step_dx = 4;
  this->num_dx = 33;
  this->min_dx = -this->step_dx * (this->num_dx-1)/2;

  // max dy = 64
  this->step_dy = 4;
  this->num_dy = 33;
  this->min_dy = -this->step_dy * (this->num_dy-1)/2;

  this->step_alpha0 = vnl_math::pi/17;
  this->num_alpha0 = 13;
  this->min_alpha0 = -this->step_alpha0*(this->num_alpha0-1)/2;

  this->step_alpha2 = vnl_math::pi/17;
  this->num_alpha2 = 13;
  this->min_alpha2 = -this->step_alpha2*(this->num_alpha2-1)/2;
  return;
}






// ============================================================================
// dbsks_biarc_sampler
// ============================================================================

//: Clear all information
void dbsks_biarc_sampler::
clear()
{
  this->dx_.clear();
  this->dy_.clear();
  this->alpha0_.clear();
  this->alpha2_.clear();
  this->biarc_sample_.resize(0, 0, 0);
  this->biarc_sample_index_.resize(0, 0, 0);
  this->ds_ = 2;
  this->num_bins_0to2pi_ = 36;
  this->has_cache_samples_ = false;
  
  this->has_cache_nkdiff_ = false;
  this->biarc_nkdiff_.clear();
}



// Return instance of the biarc_sampler with default parameter settings
dbsks_biarc_sampler& dbsks_biarc_sampler::
default_instance()
{
  static dbsks_biarc_sampler biarc_sampler;

  // initialize with default parameters the first time the instance is created
  if (!biarc_sampler.has_cache_samples())
  {
    vcl_cout << "\n>>Constructing a biarc sampler ...";
    
    // Set parameters of biarc sampler
    dbsks_biarc_sampler_params bsp;

    // max dx = 32
    bsp.step_dx = 2; // heuristic
    bsp.num_dx = 33;
    bsp.min_dx = -bsp.step_dx * (bsp.num_dx-1)/2;

    // max dy = 32
    bsp.step_dy = 2; // heuristic
    bsp.num_dy = 33;
    bsp.min_dy = -bsp.step_dy * (bsp.num_dy-1)/2;

    bsp.step_alpha0 = vnl_math::pi/17;
    bsp.num_alpha0 = 13;
    bsp.min_alpha0 = -bsp.step_alpha0*(bsp.num_alpha0-1)/2;

    bsp.step_alpha2 = vnl_math::pi/17;
    bsp.num_alpha2 = 13;
    bsp.min_alpha2 = -bsp.step_alpha2*(bsp.num_alpha2-1)/2;

    // compute coordinates of the grid points
    biarc_sampler.clear();
    biarc_sampler.set_grid(bsp);
    biarc_sampler.set_sampling_params(36, 1.5, 51);
    biarc_sampler.compute_cache_sample_points();

    vcl_cout << "[ OK ]\n";
  }

  return biarc_sampler;
}


// -----------------------------------------------------------------------------
//: Compute indices of a biarc, given a pair of oriented points
// "i_profile" encodes both i_alpha0 and i_alpha2
// Return false if the given biarc is out of range
bool dbsks_biarc_sampler::
biarc_to_grid(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
              const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
              int& i_dx, int& i_dy, int& i_profile) const
{
  vgl_vector_2d<double > chord = end-start;
  return (this->compute_biarc_i_dx_and_i_dy(chord, i_dx, i_dy) &&
          this->compute_biarc_i_profile(chord, start_tangent, end_tangent, i_profile));
}



//// -----------------------------------------------------------------------------
////: Compute the "profile" index of a biarc (encoding both alpha0 and alpha2)
//// Return false if biarc is out of range
//bool dbsks_biarc_sampler::
//biarc_to_profile(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
//                 const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
//                 int& i_profile) const
//{
//  // profile index
//  double alpha0 = signed_angle(end-start, start_tangent);
//  int i_alpha0 = vnl_math::rnd( (alpha0-this->alpha0_[0]) / this->step_alpha0_);
//  if (i_alpha0 < 0 || i_alpha0 >= int(this->alpha0_.size()))
//    return false;
//  
//  double alpha2 = signed_angle(end-start, end_tangent);
//  int i_alpha2 = vnl_math::rnd( (alpha2-this->alpha2_[0]) / this->step_alpha2_);
//  if (i_alpha2 < 0 || i_alpha2 >= int(this->alpha2_.size()))
//    return false;
//
//  i_profile = i_alpha2 + i_alpha0 * this->alpha2_.size();
//  return true;
//}


// -----------------------------------------------------------------------------
//: Compute the "profile" index of a biarc (encoding both alpha0 and alpha2)
// Return false if biarc is out of range
bool dbsks_biarc_sampler::
compute_biarc_i_profile(const vgl_vector_2d<double >& biarc_chord, 
                        const vgl_vector_2d<double >& start_tangent,
                        const vgl_vector_2d<double >& end_tangent,
                        int& i_profile) const
{
  // profile index
  double alpha0 = signed_angle(biarc_chord, start_tangent);
  int i_alpha0 = vnl_math::rnd( (alpha0-this->alpha0_[0]) / this->step_alpha0_);
  if (i_alpha0 < 0 || i_alpha0 >= int(this->alpha0_.size()))
    return false;
  
  double alpha2 = signed_angle(biarc_chord, end_tangent);
  int i_alpha2 = vnl_math::rnd( (alpha2-this->alpha2_[0]) / this->step_alpha2_);
  if (i_alpha2 < 0 || i_alpha2 >= int(this->alpha2_.size()))
    return false;

  i_profile = i_alpha2 + i_alpha0 * this->alpha2_.size();
  return true;
}

//------------------------------------------------------------------------------
//: Compute i_dx and i_dy index of a biarc
// Return false if biarc is out of range
bool dbsks_biarc_sampler::
compute_biarc_i_dx_and_i_dy(const vgl_vector_2d<double >& biarc_chord, 
                            int& i_dx, int& i_dy) const
{
  // dx-index
  double dx = biarc_chord.x();
  i_dx = vnl_math::rnd((dx-this->dx_[0]) / this->step_dx_);
  if (i_dx < 0 || i_dx>=int(this->dx_.size()) ) 
    return false;


  // dy-index
  double dy = biarc_chord.y();
  i_dy = vnl_math::rnd((dy-this->dy_[0]) / this->step_dy_);
  if (i_dy<0 || i_dy>= int(this->dy_.size()))
    return false;

  return true;
}





//------------------------------------------------------------------------------
//: Compute the grid from a parameter set
bool dbsks_biarc_sampler::
set_grid(const dbsks_biarc_sampler_params& p)
{
  // dx
  this->step_dx_ = p.step_dx;
  this->dx_.set_size(p.num_dx);
  for (int i =0; i < p.num_dx; ++i)
  {
    this->dx_[i] = p.min_dx + i*p.step_dx;
  }


  // dy
  this->step_dy_ = p.step_dy;
  this->dy_.set_size(p.num_dy);
  for (int i =0; i < p.num_dy; ++i)
  {
    this->dy_[i] = p.min_dy + i*p.step_dy;
  }

  // alpha0
  this->step_alpha0_ = p.step_alpha0;
  this->alpha0_.set_size(p.num_alpha0);
  for (int i =0; i < p.num_alpha0; ++i)
  {
    this->alpha0_[i] = p.min_alpha0 + i*p.step_alpha0;
  }


  // alpha2
  this->step_alpha2_ = p.step_alpha2;
  this->alpha2_.set_size(p.num_alpha2);
  for (int i =0; i < p.num_alpha2; ++i)
  {
    this->alpha2_[i] = p.min_alpha2 + i*p.step_alpha2;
  }

  return true;
}



// -----------------------------------------------------------------------------
//: Number of radians per bin
double dbsks_biarc_sampler::
radians_per_bin() const
{
  return vnl_math::pi*2 / this->num_bins_0to2pi_;
}







// -----------------------------------------------------------------------------
//: Compute (retrieve) samples from valid biarc index
void dbsks_biarc_sampler::
retrieve_samples_from_valid_biarc_index(int i_dx, int i_dy, int i_profile,
                                        int start_x, int start_y,
                                        vcl_vector<int >& xs, vcl_vector<int >& ys, 
                                        vcl_vector<int >& angle_bins) const
{
  // retrieve the reference point set
  const vnl_matrix<int >& samples = this->biarc_sample_index_(i_dx, i_dy, i_profile);
  unsigned num_pts = samples.rows();
  
  xs.resize(num_pts);
  ys.resize(num_pts);
  angle_bins.resize(num_pts);
  
  for (unsigned i =0; i < num_pts; ++i)
  {
    xs[i] = samples[i][0] + start_x;
    ys[i] = samples[i][1] + start_y;
    angle_bins[i] = samples[i][2];
  }
  return;
}


// -----------------------------------------------------------------------------
//: Compute (retrieve) samples from valid biarc index
// These sample points are scaled up and then translated before assigning to 
// output vectors
void dbsks_biarc_sampler::
retrieve_and_scale_samples_from_valid_biarc_index(int i_dx, int i_dy, int i_profile,
                                                  double scaled_up,
                                                  const vgl_point_2d<double >& start,
                                                  vcl_vector<int >& xs, 
                                                  vcl_vector<int >& ys, 
                                                  vcl_vector<int >& angle_bins) const
{
  // retrieve the reference point set
  const vnl_matrix<int >& samples_index = this->biarc_sample_index_(i_dx, i_dy, i_profile);
  const vnl_matrix<double >& samples    = this->biarc_sample_(i_dx, i_dy, i_profile);

  unsigned num_pts = samples.rows();  
  xs.resize(num_pts);
  ys.resize(num_pts);
  angle_bins.resize(num_pts);
  
  for (unsigned i =0; i < num_pts; ++i)
  {
    xs[i] = vnl_math::rnd(samples[i][0] * scaled_up + start.x());
    ys[i] = vnl_math::rnd(samples[i][1] * scaled_up + start.y());
    angle_bins[i] = samples_index[i][2];
  }
  return;
}


// -----------------------------------------------------------------------------
//: Compute cache sample points for all biarcs defined by the grid, assuming a fixed starting point
bool dbsks_biarc_sampler::
compute_cache_sample_points()
{
  // allocate memory
  unsigned num_profiles = this->alpha0_.size()*this->alpha2_.size();
  this->biarc_sample_.resize(this->dx_.size(), this->dy_.size(), num_profiles);
  this->biarc_sample_index_.resize(this->dx_.size(), this->dy_.size(), num_profiles);

  double radians_per_bin = this->radians_per_bin();
  double ds = this->ds();

  // compute each biarc explicitly
  dbgl_biarc biarc;
  vgl_point_2d<double > start(0, 0);
  for (unsigned i_dx =0; i_dx < this->dx_.size(); ++i_dx)
  {
    for (unsigned i_dy =0; i_dy < this->dy_.size(); ++i_dy)
    {
      vgl_point_2d<double > end(this->dx_[i_dx], this->dy_[i_dy]);
      if (end==start) continue;

      // angle of vector connecting start to end
      double angle0 = signed_angle(vgl_vector_2d<double >(1, 0), end-start);

      for (unsigned i_alpha0 =0; i_alpha0 < this->alpha0_.size(); ++i_alpha0)
      {
        // alpha0 is angle between tangent at starting point and the chord
        double start_angle = angle0 + this->alpha0_[i_alpha0];
        for (unsigned i_alpha2 =0; i_alpha2 < this->alpha2_.size(); ++i_alpha2)
        {
          // alpha2 is angle between tangent at end point and the chord
          double end_angle = angle0 + this->alpha2_[i_alpha2];
          if (!biarc.compute_biarc_params(start, start_angle, end, end_angle))
          {
            continue;
          }

          // linear index of this biar profile
          int i_profile = i_alpha2 + i_alpha0 * this->alpha2_.size();

          // determine number of points to compute
          double len = biarc.len();
          int half_num_pts = vnl_math::floor(len/(2*ds));

          // set upper bound for the number of points to prevent exploding memory
          half_num_pts = vnl_math::min(half_num_pts, this->max_num_pts_per_biarc_/2);

          
          // allocate memory
          vnl_matrix<double >& samples = this->biarc_sample_(i_dx, i_dy, i_profile);
          vnl_matrix<int >& index = this->biarc_sample_index_(i_dx, i_dy, i_profile);

          samples.set_size(2*half_num_pts+1, 4);
          index.set_size(2*half_num_pts+1, 3);

          // compute sample points of the biarc
          for (int k = -half_num_pts; k <= half_num_pts; ++k)
          {
            // index in the vector
            int kk = k + half_num_pts;

            // oriented point at this index
            double s = len/2 + k*ds; // arc length
            vgl_point_2d<double > pt = biarc.point_at(s);
            vgl_vector_2d<double > t = biarc.tangent_at(s);


            samples(kk, 0) = pt.x();
            samples(kk, 1) = pt.y();
            samples(kk, 2) = t.x();
            samples(kk, 3) = t.y();

            // keep tangent angle in [0, 2pi]
            double angle = vcl_atan2(t.y(), t.x());
            angle = (angle < 0) ? (angle + 2*vnl_math::pi) : angle;
            
            // indices of these points
            index(kk, 0) = vnl_math::rnd(pt.x());
            index(kk, 1) = vnl_math::rnd(pt.y());
            index(kk, 2) = vnl_math::rnd(angle / radians_per_bin) % this->num_bins_0to2pi_;
          }        
        }      
      }
    }
  }

  // Set the "has_cache_samples_" flag;
  this->has_cache_samples_ = true;
  return true;
}







//------------------------------------------------------------------------------
//: Compute cache "normalized curvature difference" for all biarcs
bool dbsks_biarc_sampler::
compute_cache_nkdiff()
{
  //> allocate memory
  unsigned num_profiles = this->alpha0_.size() * this->alpha2_.size();
  this->biarc_nkdiff_.set_size(num_profiles);
  this->biarc_nkdiff_.fill(vnl_numeric_traits<double >::maxval);

  //> Compute Normalized curvature difference (nkdiff) for each biarc profile
  // nkdiff is invariant to position, orientation, and size of the biarc
  // we only need to vary over the profiles (combination of alpha0 and alpha2)
  
  vgl_point_2d<double > start(0, 0);
  vgl_point_2d<double > end(100, 0);
  
  double angle0 = signed_angle(vgl_vector_2d<double >(1, 0), end-start);
  double chord = (end-start).length();

  dbgl_biarc biarc;
  for (unsigned i_alpha0 =0; i_alpha0 < this->alpha0_.size(); ++i_alpha0)
  {
    // alpha0 is angle between tangent at starting point and the chord
    double start_angle = angle0 + this->alpha0_[i_alpha0];
    for (unsigned i_alpha2 =0; i_alpha2 < this->alpha2_.size(); ++i_alpha2)
    {
      // alpha2 is angle between tangent at end point and the chord
      double end_angle = angle0 + this->alpha2_[i_alpha2];
      if (!biarc.compute_biarc_params(start, start_angle, end, end_angle))
      {
        continue;
      }

      // linear index of this biar profile
      int i_profile = i_alpha2 + i_alpha0 * this->alpha2_.size();

      // curvature difference
      double kdiff = biarc.k1() - biarc.k2();

      // Normalized curvature difference
      this->biarc_nkdiff_[i_profile] = chord * kdiff;

    }
  }

  // Set the "has_cache_nkdiff_" flag;
  this->has_cache_nkdiff_ = true;
  return true;
}






// -----------------------------------------------------------------------------
//: Compute samples of a biarc by considering it as a translated version of 
// of the pre-computed biarcs
bool dbsks_biarc_sampler::
compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                vcl_vector<double >& x_vec, vcl_vector<double >& y_vec, 
                vcl_vector<double >& tx_vec, vcl_vector<double >& ty_vec)
{
  x_vec.clear();
  y_vec.clear();
  tx_vec.clear();
  ty_vec.clear();

  // preliminary check
  if (this->biarc_sample_.size() == 0) return false;
  
  int i_dx, i_dy, i_profile;
  if (!this->biarc_to_grid(start, start_tangent, end, end_tangent, i_dx, i_dy, i_profile))
    return false;
  
  // retrieve the reference point set
  vnl_matrix<double >& samples = this->biarc_sample_(i_dx, i_dy, i_profile);

  if (!samples.empty())
  {
    unsigned num_pts = samples.rows();
    double x0 = start.x();
    double y0 = start.y();

    x_vec.resize(num_pts);
    y_vec.resize(num_pts);
    tx_vec.resize(num_pts);
    ty_vec.resize(num_pts);
    for (unsigned i =0; i != num_pts; ++i)
    {
      x_vec[i] = samples[i][0] + x0;
      y_vec[i] = samples[i][1] + y0;
      tx_vec[i] = samples[i][2];
      ty_vec[i] = samples[i][3];
    }
  }
  return true;
}



// -----------------------------------------------------------------------------
//: Compute samples of a biarc by considering it as a translated version of 
// of the pre-computed biarcs
bool dbsks_biarc_sampler::
compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                vcl_vector<vgl_point_2d<double > >& pts, vcl_vector<vgl_vector_2d<double > >& tangents)
{
  // sanitize output storage
  pts.clear();
  tangents.clear();


  // retrieve sample points using a similar function
  vcl_vector<double > x_vec, y_vec; 
  vcl_vector<double > tx_vec, ty_vec;
  bool ok = this->compute_samples(start, start_tangent, end, end_tangent, 
                                  x_vec, y_vec, tx_vec, ty_vec);

  if (!ok)
    return false;

  // re-package the sample points to the output format
  unsigned num_pts = x_vec.size();
  pts.reserve(num_pts);
  tangents.reserve(num_pts);

  for (unsigned i =0; i < num_pts; ++i)
  {
    pts.push_back(vgl_point_2d<double >(x_vec[i], y_vec[i]));
    tangents.push_back(vgl_vector_2d<double >(tx_vec[i], ty_vec[i]));
  }

  return true;
}



// -----------------------------------------------------------------------------
//: Compute samples of a biarc by considering it as a translated version of 
// of the pre-computed biarcs
bool dbsks_biarc_sampler::
compute_samples(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins, double& angle_step) const
{
  xs.clear();
  ys.clear();
  angle_bins.clear();
  angle_step = this->radians_per_bin();

  // Case 1: cache sample points are available. Retrieve them using given info
  if (this->has_cache_samples())
  {
    return this->compute_samples_using_cache(start, start_tangent, end, end_tangent,
      xs, ys, angle_bins);
  }
  // Case 2: No cache points are available. Compute the samples with uniform sampling
  else
  {
    return this->compute_samples_not_using_cache(start, start_tangent, end, end_tangent,
      xs, ys, angle_bins);
  }
}










//------------------------------------------------------------------------------
//: Compute samples of a biarc using cache data
// Required: cache data have been computed
bool dbsks_biarc_sampler::
compute_samples_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                            const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                            vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const
{
  int i_dx, i_dy, i_profile;

  // case 1a: input biarc is within coverage range: simply retrieve the samples
  if (this->biarc_to_grid(start, start_tangent, end, end_tangent, i_dx, i_dy, i_profile))
  {
    // this version is faster than the general scale+translate
    this->retrieve_samples_from_valid_biarc_index(i_dx, i_dy, i_profile, 
      int(start.x()), int(start.y()),
      xs, ys, angle_bins);
    return true;
  }

  // case 1b: the biarc's profile (alpha0 and alpha2) is within coverage but its
  // extrinsic range is not. We scale the biarc down to get the samples then scale
  // it back up
  else if (this->compute_biarc_i_profile(end-start, start_tangent, end_tangent, i_profile))
  {
    double biarc_dx = end.x() - start.x();
    double biarc_dy = end.y() - start.y();

    // \assumption: this->dx_ and this->dy_ start with a negative number and
    // end with a positive number
    double scale_down_along_x = (biarc_dx < 0) ? 
      biarc_dx/this->dx_[0] : biarc_dx/this->dx_[this->dx_.size()-1];

    double scale_down_along_y = (biarc_dy < 0) ?
      biarc_dy/this->dy_[0] : biarc_dy/this->dy_[this->dy_.size()-1];

    // scaling down value necessary to put the biarc back in range
    double scale_down = vnl_math::max(scale_down_along_x, scale_down_along_y);
    assert(vnl_math::abs(scale_down) >= 1);

    vgl_point_2d<double > origin(0,0);
    vgl_point_2d<double > end_scaled_down = origin + (end-start)/scale_down;

    bool success = this->biarc_to_grid(origin, start_tangent, end_scaled_down, end_tangent, 
      i_dx, i_dy, i_profile);

    // success should be true. Otherwise something is seriously wrong
    if (!success)
    {
      vcl_cout << "\nERROR: something is seriously wrong with this biarc sampler.\n";
      return false;
    }
    this->retrieve_and_scale_samples_from_valid_biarc_index(i_dx, i_dy, i_profile, 
      scale_down, start, 
      xs, ys, angle_bins);
    return true;
  }
  else
  {
    return false;
  }
}



//------------------------------------------------------------------------------
//: Compute samples of a biarc - not using cache regarless of whether cache data are available
bool dbsks_biarc_sampler::
compute_samples_not_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                                const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                                vcl_vector<int >& xs, vcl_vector<int >& ys, vcl_vector<int >& angle_bins) const
{
  vcl_vector<double > x_vec;
  vcl_vector<double > y_vec;
  vcl_vector<double > tx_vec;
  vcl_vector<double > ty_vec;
  vcl_vector<double > angle_vec;
  if (!this->compute_uniform_biarc_samples(start, start_tangent, end, end_tangent, ds_,
    x_vec, y_vec, tx_vec, ty_vec, angle_vec))
  {
    return false;
  }
  unsigned num_pts = x_vec.size();
  xs.resize(num_pts);
  ys.resize(num_pts);
  angle_bins.resize(num_pts);
  for (unsigned i =0; i < num_pts; ++i)
  {
    xs[i] = vnl_math::rnd(x_vec[i]);
    ys[i] = vnl_math::rnd(y_vec[i]);
    double angle = dbnl_angle_0to2pi(angle_vec[i]); 

    //>> compute the bin-index of the angle, use % to handle angle ~= 2pi;
    angle_bins[i] = vnl_math::rnd(angle / this->radians_per_bin()) % this->num_bins_0to2pi_;
  }
  return true;
}



//------------------------------------------------------------------------------
//: Compute normalized curvature difference
bool dbsks_biarc_sampler::
compute_nkdiff_using_cache(const vgl_point_2d<double >& start, const vgl_vector_2d<double >& start_tangent,
                           const vgl_point_2d<double >& end, const vgl_vector_2d<double >& end_tangent,
                           double& nkdiff_val) const
{
  int i_profile;
  if (!this->compute_biarc_i_profile(end - start, start_tangent, end_tangent, i_profile))
    return false;
  nkdiff_val = this->biarc_nkdiff_[i_profile];
  return true;
}
  

// -----------------------------------------------------------------------------
//: (Literatally) compute samples of a biarc uniformly
bool dbsks_biarc_sampler::
compute_uniform_biarc_samples(const vgl_point_2d<double >& start, 
                              const vgl_vector_2d<double >& start_tangent,
                              const vgl_point_2d<double >& end, 
                              const vgl_vector_2d<double >& end_tangent,
                              double ds,
                              vcl_vector<double >& x_vec, 
                              vcl_vector<double >& y_vec, 
                              vcl_vector<double >& tx_vec,
                              vcl_vector<double >& ty_vec,
                              vcl_vector<double >& angle_vec) const
{
  x_vec.clear();
  y_vec.clear();
  tx_vec.clear();
  ty_vec.clear();
  angle_vec.clear();

  // alpha2 is angle between tangent at end point and the chord
  dbgl_biarc biarc;
  if (!biarc.compute_biarc_params(start, start_tangent, end, end_tangent))
  {
    return false;
  }

  // determine number of points to compute
  double len = biarc.len();
  int half_num_pts = vnl_math::floor(len/(2*ds));

  x_vec.resize(2*half_num_pts+1);
  y_vec.resize(2*half_num_pts+1);
  tx_vec.resize(2*half_num_pts+1);
  ty_vec.resize(2*half_num_pts+1);
  angle_vec.resize(2*half_num_pts+1);

  // compute sample points of the biarc
  for (int k = -half_num_pts; k <= half_num_pts; ++k)
  {
    // index in the vector
    int kk = k + half_num_pts;

    // oriented point at this index
    double s = len/2 + k*ds; // arc length
    vgl_point_2d<double > pt = biarc.point_at(s);
    vgl_vector_2d<double > t = biarc.tangent_at(s);


    x_vec[kk] = pt.x();
    y_vec[kk] = pt.y();
    tx_vec[kk] = t.x();
    ty_vec[kk] = t.y();

    // keep tangent angle in [0, 2pi]
    double angle = vcl_atan2(t.y(), t.x());
    if (angle < 0) 
    {
      angle += 2*vnl_math::pi;
    }
    angle_vec[kk] = angle;
  }        

  return true;
}



